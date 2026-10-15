# Debugprobe

Firmware source for the Raspberry Pi Debug Probe SWD/UART accessory. Can also be run on a Raspberry Pi Pico or Pico 2.

[Raspberry Pi Debug Probe product page](https://www.raspberrypi.com/products/debug-probe/)

[Raspberry Pi Pico product page](https://www.raspberrypi.com/products/raspberry-pi-pico/)

[Raspberry Pi Pico 2 product page](https://www.raspberrypi.com/products/raspberry-pi-pico-2/)

## Documentation

Debug Probe documentation can be found at the [Raspberry Pi documentation](https://www.raspberrypi.com/documentation/microcontrollers/debug-probe.html#about-the-debug-probe) and in the [Getting Started with Pico PDF](https://pip.raspberrypi.com/documents/RP-008276-DS).

## Hacking

For the purpose of making changes or studying of the code, you may want to compile the code yourself.

First, clone the repository:
```bash
git clone https://github.com/raspberrypi/debugprobe
cd debugprobe
```

Initialize and update the submodules:
```bash
 git submodule update --init --recursive
```

Then create and switch to the build directory:
```bash
 mkdir build
 cd build
```

If your environment doesn't contain `PICO_SDK_PATH`, then either add it to your environment variables with `export PICO_SDK_PATH=/path/to/sdk` or add `-DPICO_SDK_PATH=/path/to/sdk` to the arguments to CMake below. `PICO_SDK_PATH` must point to a version 2.3.0 or greater install.

Run cmake and build the code:
```bash
 cmake ..
 make
```

Done! You should now have a `debugprobe.uf2` that you can upload to your Debug Probe via the UF2 bootloader.

## Building for the Pico 1

If you want to create the version that runs on the Pico, then you need to invoke `cmake` in the sequence above with the `-DPICO_BOARD=pico` option:
```bash
cmake -DPICO_BOARD=pico ..
```

This will build with the configuration for the Pico and call the output program `debugprobe_on_pico.uf2`, as opposed to `debugprobe.uf2` for the accessory hardware.

Note that if you first ran through the whole sequence to compile for the Debug Probe, then you don't need to start back at the top. You can just go back to the `cmake` step and start from there.

## Building for the Pico 2

If using an existing debugprobe clone:
- You must completely regenerate your build directory, or use a different one.
- You must also sync and update submodules.

```bash
git submodule sync
git submodule update --init --recursive
mkdir build-pico2
cd build-pico2
cmake -DPICO_BOARD=pico2 ../
```

This will build with the configuration for the Pico 2 and call the output program `debugprobe_on_pico2.uf2`.

## AutoBaud Mode

Mode which automatically detects and sets the UART baud rate as data arrives.

To enable AutoBaud, configure the USB CDC port to the following custom baud rate:
```
9728 (0x2600)
```
> **Note:** Some Linux serial tools cannot set custom baud values. PuTTY on Windows and any terminal that supports arbitrary baud rates works.

Changing the baud rate to any other value disables AutoBaud.


# TBD-16 direct Pico debug probe

A standard Raspberry Pi Pico or Pico 2 that plugs straight into the TBD-16's 6-pin debug/extension port.

**Hardware:** solder a straight 1x5 2.54 mm male header into the Pico's physical pins 3-7 (GND, GP2, GP3, GP4, GP5;
the same positions on Pico and Pico 2). Plug it into the TBD-16's port, Pico pin 3 (GND) onto TBD pin 1 (GND, the
rightmost pin seen from outside the case). The Pico's USB connector stays outside the enclosure and powers the Pico;
TBD pin 6 (+5 V) stays unconnected.

| TBD-16 port | Pico / Pico 2 | Probe function |
|---|---|---|
| 1 GND | pin 3 GND | ground |
| 2 SWCLK | pin 4 GP2 | SWCLK |
| 3 SWDAT | pin 5 GP3 | SWDIO |
| 4 SDA (GPIO20, the TBD's debug UART TX) | pin 6 GP4 | UART RX |
| 5 SCL (optional) | pin 7 GP5 | UART TX |

GP4 receiving and GP5 transmitting is the reverse of the chips' hardware UART pins, so this build runs the USB CDC
UART bridge on a PIO UART (8n1; parity and stop-bit settings from the host are ignored). SWD and CMSIS-DAP are
unchanged.

**Build** (Pico SDK 2.3.0 or later, an arm-none-eabi GCC):

    cmake -S . -B build-tbd-pico2 -G Ninja -DPICO_BOARD=pico2 -DDEBUGPROBE_TBD=ON && ninja -C build-tbd-pico2
    cmake -S . -B build-tbd-pico  -G Ninja -DPICO_BOARD=pico  -DDEBUGPROBE_TBD=ON && ninja -C build-tbd-pico

giving `tbd-debugprobe-pico2.uf2` and `tbd-debugprobe-pico.uf2`.

**Flash:** hold BOOTSEL, connect USB, copy the UF2 for your board onto the RPI-RP2 / RP2350 drive; the probe restarts.

**Use:** `pio run -e possan_rev_c -t upload` (cmsis-dap) and `pio device monitor -b 115200`.

**UART check without the TBD:** connect GP5 (pin 7) to GP4 (pin 6) with one wire, open the probe's serial port at
115200 and type: every character comes back.
