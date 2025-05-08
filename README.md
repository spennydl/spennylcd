# spennylcd Kernel Driver

This is a Linux device driver for a HD44780 LCD screen module that includes
an IO expander backpack. This driver communicates with the screen directly
over I2C. It has been developed and tested on a Raspberry Pi 5.

This driver was written mostly as an exercise for myself. As such, I would
recommend that you **do not use this driver.** There is already a driver in
the kernel for the HD4470 and the PCF857x IO expander module, and I would recommend
either using those or writing a userspace I2C driver yourself. **Use at your**
**own risk!**

That said...

## Building

```sh
$ make KERNEL_DIR={path-to-kernel-makefile}
```

The build requires a path to the root makefile of the kernel tree you wish to
build the module against. Alternatively, you could provide the path
to the build files of the kernel you are currently running, if they
are available (usually at /lib/modules/`uname -r`/build).

## Deploying

A successful build will produce a kernel module `spennylcd_mod.ko` that can
be loaded into a running kernel with `$ sudo insmod spennylcd_mod.ko`.

In order to use this driver, the kernel will need to know that the LCD
is connected to it. The recommended way to let the kernel know this is
by using the device tree. This repository includes a device tree overlay
(`spennylcd-i2c.dts`) that can be used to do this on the Raspberry Pi 5.
Other devices will require slightly different overlays.

Deploying the overlay is done by compiling a device tree blob and editing
`config.txt` so that it gets loaded.
First, on the raspberry pi, build the overlay blob with:
```
$ dtc spennylcd-i2c.dtbo spennylcd-i2c.dts
```
and place the resulting file in `/boot/firmware/overlays`.

Next, edit `/boot/firmware/config.txt` and add the line:
```
$ dtoverlay=spennylcd-i2c
```

Finally, restart the raspberry pi.

*TODO(spencer): include a udev rule that sets a group permission on the spennylcd device file*

## Using the Driver

The driver creates a misc device representing the LCD screen. If the module
loads and initializes correctly you should see a character device apppear at
`/dev/spennylcd`. Text written to this device should appear on the LCD screen.

This module is still under some development. It does not yet support non-printing
characters or escape sequences.
