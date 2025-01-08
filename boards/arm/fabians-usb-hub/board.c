#include <zephyr/drivers/gpio.h>
#include <zephyr/init.h>
#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>

static int usb_hub_reset_init(void)
{
	int err;
	const struct gpio_dt_spec reset_gpio = {
		.port = DEVICE_DT_GET(DT_NODELABEL(gpioa)),
		.pin = 9,
		.dt_flags = 0,
	};
        const struct gpio_dt_spec sel_2 = {
            .port = DEVICE_DT_GET(DT_NODELABEL(gpiob)),
            .pin = 12,
            .dt_flags = 0,
        };
        const struct gpio_dt_spec sel_1 = {
            .port = DEVICE_DT_GET(DT_NODELABEL(gpiob)),
            .pin = 13,
            .dt_flags = 0,
        };

        err = gpio_pin_configure_dt(&reset_gpio,
                                    GPIO_OUTPUT_ACTIVE | GPIO_ACTIVE_LOW);
        if (err) {
          printk("Failed to configure reset pin: %s %d (%d)\n",
                 reset_gpio.port->name, reset_gpio.pin, err);
          return err;
        }

        err = gpio_pin_configure_dt(&sel_2,
                                    GPIO_OUTPUT_INACTIVE | GPIO_ACTIVE_HIGH);
        if (err) {
          printk("Failed to configure reset pin: %s %d (%d)\n",
                 sel_2.port->name, sel_2.pin, err);
          return err;
        }

        err = gpio_pin_configure_dt(&sel_1,
                                    GPIO_OUTPUT_INACTIVE | GPIO_ACTIVE_HIGH);
        if (err) {
          printk("Failed to configure reset pin: %s %d (%d)\n",
                 sel_1.port->name, sel_1.pin, err);
          return err;
        }

        k_msleep(2);

        err = gpio_pin_set_dt(&reset_gpio, 0);
        if (err) {
          printk("Failed to set reset pin: %s %d (%d)\n", reset_gpio.port->name,
                 reset_gpio.pin, err);
          return err;
        }

        return 0;
}

SYS_INIT(usb_hub_reset_init, POST_KERNEL, CONFIG_KERNEL_INIT_PRIORITY_DEVICE);
