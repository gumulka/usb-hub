/*
 * Copyright (c) 2016 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/drivers/gpio.h>
#include <zephyr/kernel.h>

#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(main, CONFIG_APP_LOG_LEVEL);

static const struct gpio_dt_spec led = GPIO_DT_SPEC_GET(DT_ALIAS(led0), gpios);

int main(void) {
  int ret;

  if (!gpio_is_ready_dt(&led)) {
    LOG_WRN("LED is not ready!");
    return 0;
  }

  ret = gpio_pin_configure_dt(&led, GPIO_OUTPUT_ACTIVE);
  if (ret < 0) {
    LOG_WRN("Could not configure LED!");
    return 0;
  }

  while (1) {
    ret = gpio_pin_toggle_dt(&led);
    LOG_INF("Toggle output port");
    if (ret < 0) {
      return 0;
    }
    k_msleep(1000);
  }
  return 0;
}
