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

## Compile the bootloader

The bootloader uses a lot of memory and needs some special attention to get it
to work. We are nearing the limit of what the STM32 can do and process.

Compile and flash it with the following commands:

```bash
west build -p -b fabians_usb_hub -d build_mcuboot ../bootloader/mcuboot/boot/zephyr/ -- -DOVERLAY_CONFIG="`pwd`/mcuboot.conf"
west flash -d build_mcuboot
```

[zephyr]: https://github.com/zephyrproject-rtos/zephyr
[zephyr_getting_started]: https://docs.zephyrproject.org/latest/develop/getting_started/index.html
