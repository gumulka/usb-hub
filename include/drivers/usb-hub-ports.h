/*
 * Copyright (c) 2024 Fabian Pflug
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef USB_HUB_PORTS_H
#define USB_HUB_PORTS_H 1

#include <zephyr/device.h>
#include <errno.h>
#include <zephyr/kernel.h>

#ifdef __cplusplus
extern "C" {
#endif

/** @brief USB hub ports driver class operations */
__subsystem struct usb_hub_driver_api {
	/**
	 * @brief Set the USB port to a specific state.
	 *
	 * @param dev Usb hub ports device instance.
	 * @param port_number Counter for the port number to be set
	 * @param enable Wether to enable the port or not.
	 *
	 * @retval 0 if successful.
	 * @retval -EINVAL if @p port_number is not in range can not be set.
	 * @retval -errno Other negative errno code on failure.
	 */
	int (*set_port)(const struct device *dev, unsigned int port_number, bool enable);

	/**
	 * @brief Get the current state of the specific port.
	 *
	 * @param dev Usb hub ports device instance.
	 * @param port_number Counter for the port number to be set
	 *
	 * @retval 0 if port is disabled
	 * @retval 1 if port is enabled
	 * @retval -EINVAL if @p port_number is not in range can not be set.
	 * @retval -errno Other negative errno code on failure.
	 */
	int (*get_port)(const struct device *dev, unsigned int port_number);
};

/**
 * @brief Set the USB port to a specific state.
 *
 * @param dev Usb hub ports device instance.
 * @param port_number Counter for the port number to be set
 * @param enable Wether to enable the port or not.
 *
 * @retval 0 if successful.
 * @retval -EINVAL if @p port_number is not in range can not be set.
 * @retval -errno Other negative errno code on failure.
 */
__syscall int usb_hub_set_port(const struct device *dev, unsigned int port_number, bool enable);

static inline int z_impl_usb_hub_set_port(const struct device *dev, unsigned int port_number,
					  bool enable)
{
	if (dev == NULL) {
		return -EINVAL;
	}

	const struct usb_hub_driver_api *api = (const struct usb_hub_driver_api *)dev->api;

	return api->set_port(dev, port_number, enable);
}

/**
 * @brief Get the current state of the specific port.
 *
 * @param dev Usb hub ports device instance.
 * @param port_number Counter for the port number to be set
 *
 * @retval 0 if port is disabled
 * @retval 1 if port is enabled
 * @retval -EINVAL if @p port_number is not in range can not be read.
 * @retval -errno Other negative errno code on failure.
 */
__syscall int usb_hub_get_port(const struct device *dev, unsigned int port_number);

static inline int z_impl_usb_hub_get_port(const struct device *dev, unsigned int port_number)
{
	if (dev == NULL) {
		return -EINVAL;
	}

	const struct usb_hub_driver_api *api = (const struct usb_hub_driver_api *)dev->api;

	return api->get_port(dev, port_number);
}

/**
 * @brief Toggle the state of the specific port.
 *
 * @param dev Usb hub ports device instance.
 * @param port_number Counter for the port number to be set
 *
 * @retval 0 if successful.
 * @retval -EINVAL if @p port_number is not in range.
 * @retval -errno Other negative errno code on failure.
 */
static inline int usb_hub_toggle_port(const struct device *dev, unsigned int port_number)
{
	int ret = usb_hub_get_port(dev, port_number);
	if (ret < 0) {
		return ret;
	}
	return usb_hub_set_port(dev, port_number, !ret);
}

#ifdef __cplusplus
}
#endif

#include <syscalls/usb-hub-ports.h>

#endif // USB_HUB_PORTS_H
