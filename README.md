# USB Hub

A simple USB 2.0 USB-Hub, with physical buttons to toggle the power to
the port as well as an usb interface to toggle the different ports.

## Compile firmware

The firmware is written with [zephyr](zephyr).
Click [here](zephyr_getting_started) for information on how to
[get started](zephyr_getting_started).

With zephyr and west installed execute the following to compile and flash the
firmware with debug information.

```bash
west init -m https://github.com/gumulka/usb-hub --mr zephyr usb-hub
cd usb-hub
west update
cd usb-hub
west build -p -b fabians_usb_hub app -- -DOVERLAY_CONFIG="debug.conf"
west flash
```

[zephyr]: https://github.com/zephyrproject-rtos/zephyr
[zephyr_getting_started]: https://docs.zephyrproject.org/latest/develop/getting_started/index.html
