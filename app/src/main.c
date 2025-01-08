/*
 * Copyright (c) 2024 Fabian Pflug
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/drivers/i2c.h>
#include <zephyr/kernel.h>

#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(main, CONFIG_APP_LOG_LEVEL);

static struct i2c_dt_spec hub_i2c = I2C_DT_SPEC_GET(DT_NODELABEL(hub_i2c));

static const uint8_t basic_data[] = {0x24, 0x04, 0x17, 0x25, 0,    0,    0x9b,
                                     0x20, 0x80, 0x80, 0x80, 0x01, 0x32, 0x01,
                                     0x32, 0x32, 0,    0,    7,    23,   8};

static const uint8_t manufacturer[] = {0x47, 0x0,  0x75, 0x0,  0x6d, 0x0, 0x75,
                                       0x0,  0x6c, 0x0,  0x6b, 0x0,  0x61};
static const uint8_t product[] = {0x55, 0x0,  0x53, 0x0,  0x42, 0x0,  0x32,
                                  0x0,  0x2e, 0x0,  0x30, 0x0,  0x20, 0x0,
                                  0x48, 0x0,  0x75, 0x0,  0x62};
static const uint8_t serial[] = {0x30, 0x0, 0x30, 0x0, 0x30, 0x0,
                                 0x30, 0x0, 0x30, 0x0, 0x31};

static int write_registers(struct i2c_dt_spec *device, uint8_t reg,
                           const uint8_t *data, uint8_t data_len) {

  uint8_t send_buf[34];

  if (data_len > 32) {
    return -EINVAL;
  }

  send_buf[0] = reg;
  send_buf[1] = data_len;
  memcpy(send_buf + 2, data, data_len);
  return i2c_write_dt(device, send_buf, data_len + 2);
}

int main(void) {
  if (!i2c_is_ready_dt(&hub_i2c)) {
    LOG_ERR("I2C device is not ready!");
    return -ENODEV;
  }
  int ret;
  ret = write_registers(&hub_i2c, 0x00, basic_data, ARRAY_SIZE(basic_data));
  if (ret != 0) {
    LOG_ERR("Could not send basic data %d", ret);
    return ret;
  }
  ret = write_registers(&hub_i2c, 0x16, manufacturer, ARRAY_SIZE(manufacturer));
  if (ret != 0) {
    LOG_ERR("Could not send manufacturer %d", ret);
    return ret;
  }
  ret = write_registers(&hub_i2c, 0x54, product, ARRAY_SIZE(product));
  if (ret != 0) {
    LOG_ERR("Could not send product %d", ret);
    return ret;
  }
  ret = write_registers(&hub_i2c, 0x92, serial, ARRAY_SIZE(serial));
  if (ret != 0) {
    LOG_ERR("Could not send serial %d", ret);
    return ret;
  }

  uint8_t start = 0x01;
  ret = write_registers(&hub_i2c, 0xff, &start, 1);
  if (ret != 0) {
    LOG_ERR("Could not activate hub %d", ret);
    return ret;
  }

  LOG_INF("Done with init!");

  return 0;
}
