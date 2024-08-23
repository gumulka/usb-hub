/*
 * Copyright (c) 2024 Nordic Semiconductor ASA
 * SPDX-License-Identifier: Apache-2.0
 */

#define DT_DRV_COMPAT usb_hub_ports

#include <zephyr/device.h>

#include <zephyr/devicetree.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/kernel.h>

#include <drivers/usb-hub-ports.h>

#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(usb_hub_ports, CONFIG_HUB_PORTS_LOG_LEVEL);

struct usb_hub_ports_config {
	const unsigned int num_ports;
	const struct gpio_dt_spec *enable;
	const struct gpio_dt_spec *request;
};

struct usb_hub_ports_data {
	struct gpio_callback *callback;
};

static int usb_hub_ports_set(const struct device *dev, unsigned int port_number, bool enable);
static int usb_hub_ports_get(const struct device *dev, unsigned int port_number);
static int usb_hub_ports_init(const struct device *dev);

static const struct usb_hub_driver_api usb_hub_ports_api = {
	.set_port = &usb_hub_ports_set,
	.get_port = &usb_hub_ports_get,
};

#define USB_HUB_NUM_PORTS(inst) DT_PROP_LEN(DT_DRV_INST(inst), enable_gpios)

#define USB_HUB_GPIO_INST(index, inst, name) GPIO_DT_SPEC_INST_GET_BY_IDX(inst, name, index)

#define USB_HUB_GPIO_LIST(inst, name)                                                              \
	{LISTIFY(USB_HUB_NUM_PORTS(inst), USB_HUB_GPIO_INST, (, ), inst, name)}

#define USB_HUB_DEFINE(inst)                                                                       \
	BUILD_ASSERT(DT_PROP_LEN(DT_DRV_INST(inst), enable_gpios) ==                               \
			     DT_PROP_LEN(DT_DRV_INST(inst), request_gpios),                        \
		     "usb-hub-ports: unequal amount of req and enable gpios!");                    \
                                                                                                   \
	static const struct gpio_dt_spec enable##inst[USB_HUB_NUM_PORTS(inst)] =                   \
		USB_HUB_GPIO_LIST(inst, enable_gpios);                                             \
	static const struct gpio_dt_spec request##inst[USB_HUB_NUM_PORTS(inst)] =                  \
		USB_HUB_GPIO_LIST(inst, request_gpios);                                            \
	static struct gpio_callback callback##inst[USB_HUB_NUM_PORTS(inst)];                       \
                                                                                                   \
	static const struct usb_hub_ports_config config##inst = {                                  \
		.num_ports = USB_HUB_NUM_PORTS(inst),                                              \
		.enable = enable##inst,                                                            \
		.request = request##inst,                                                          \
	};                                                                                         \
                                                                                                   \
	static struct usb_hub_ports_data data##inst = {                                            \
		.callback = callback##inst,                                                        \
	};                                                                                         \
                                                                                                   \
	DEVICE_DT_INST_DEFINE(inst, usb_hub_ports_init, NULL, &data##inst, &config##inst,          \
			      POST_KERNEL, CONFIG_KERNEL_INIT_PRIORITY_DEVICE,                     \
			      &usb_hub_ports_api);

DT_INST_FOREACH_STATUS_OKAY(USB_HUB_DEFINE)

static int usb_hub_ports_set(const struct device *dev, unsigned int port_number, bool enable)
{
	const struct usb_hub_ports_config *config = dev->config;

	if (port_number >= config->num_ports) {
		return -EINVAL;
	}

	return gpio_pin_set_dt(&config->enable[port_number], enable);
}

static int usb_hub_ports_get(const struct device *dev, unsigned int port_number)
{
	const struct usb_hub_ports_config *config = dev->config;

	if (port_number >= config->num_ports) {
		return -EINVAL;
	}

	return gpio_pin_get_dt(&config->enable[port_number]);
}

static int usb_hub_request_callback_config(const struct usb_hub_ports_config *config,
					   const struct device *gpio_dev, int gpio_pin)
{
	for (int i = 0; i < config->num_ports; i++) {
		if (config->request[i].port == gpio_dev && config->request[i].pin == gpio_pin) {
			LOG_INF("Received toggle request for port %d", i);
			gpio_pin_toggle_dt(&config->enable[i]);
			return 1;
		}
	}
	return 0;
}

#define USB_HUB_ITERATE_GPIO_CALLBACK(inst)                                                        \
	executed |= usb_hub_request_callback_config(&config##inst, dev, pin);

static void usb_hub_request_callback(const struct device *dev, struct gpio_callback *cb,
				     uint32_t pins)
{
	LOG_DBG("Received button press from %s %d", dev->name, pins);
	int pin = 0;
	while (pins) {
		if (pins & 1) {
			int executed = 0;
			DT_INST_FOREACH_STATUS_OKAY(USB_HUB_ITERATE_GPIO_CALLBACK)
			if (executed == 0) {
				LOG_WRN("No callback found for %s %d", dev->name, pin);
			}
		}
		pins >>= 1;
		pin++;
	}
}

static int usb_hub_init_gpio(const struct gpio_dt_spec *gpio, gpio_flags_t direction)
{
	int ret;

	LOG_DBG("Configure GPIO %s pin %d as %d", gpio->port->name, gpio->pin, direction);

	if (!gpio_is_ready_dt(gpio)) {
		LOG_ERR("GPIO %s pin %d not ready", gpio->port->name, gpio->pin);
		return -ENODEV;
	}

	ret = gpio_pin_configure_dt(gpio, direction);
	if (ret < 0) {
		LOG_ERR("Could not configure GPIO %s pin %d", gpio->port->name, gpio->pin);
		return ret;
	}
	return 0;
}

static int usb_hub_ports_init(const struct device *dev)
{
	const struct usb_hub_ports_config *config = dev->config;
	const struct usb_hub_ports_data *data = dev->data;
	int ret;

	LOG_DBG("Initializing usb hub ports with %d ports", config->num_ports);

	for (int i = 0; i < config->num_ports; i++) {
		ret = usb_hub_init_gpio(&config->enable[i], GPIO_OUTPUT_ACTIVE);
		if (ret < 0) {
			return ret;
		}

		ret = usb_hub_init_gpio(&config->request[i], GPIO_INPUT);
		if (ret < 0) {
			return ret;
		}
		gpio_init_callback(&data->callback[i], usb_hub_request_callback,
				   BIT(config->request[i].pin));
		ret = gpio_add_callback(config->request[i].port, &data->callback[i]);
		if (ret < 0) {
			LOG_ERR("Could not add callback for GPIO %s pin %d",
				config->request[i].port->name, config->request[i].pin);
			return ret;
		}

		ret = gpio_pin_interrupt_configure_dt(&config->request[i], GPIO_INT_EDGE_TO_ACTIVE);
		if (ret != 0) {
			printk("Error %d: failed to configure interrupt on %s pin %d\n", ret,
			       config->request[i].port->name, config->request[i].pin);
			return 0;
		}
	}

	return 0;
}
