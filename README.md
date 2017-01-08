# neopixel_i2c_slave (picopixel)

This is an AVR-based neopixel driver. It accepts commands over i2c to drive a
a number of ws2812 LED pixels.

The code on the master branch is set up for an Attiny85, with the LEDs on PB3,
using an usbasp programmer. The fuses should be set to use 8 MHz internal
oscillator, **no divider**

## How many LEDs?
The number of LEDs is currently hardcoded in the firmware. The maximum number
depends on the amount of RAM available on your AVR.
An Attiny45 should be able to drive 82 LEDs, and an Attiny85, 167.

## Circuit

Suggested circuit.

```
                            ^ VCC
                            |
    +--------+--------------+-------------+-----+-------+
    |        |                            |     |       |
    |       |"| 10k                       |     |       |
    |       |_|                           |     |       |
    |        |     +Attinyx5--------+     |    |"| 2k2 |"| 2k2
    |        +-----| Reset      Vcc |-----'    |_|     |_|
    |              |                |           |       |
  _____ 0.1u       |                |           |       |
  _____  ,---------|            SCL |-----------+-------|-----<> SCL
    |    |         |                |                   |
    |    |         |                |                   |
    |    |       --|                |--                 |
    |    |         |                |                   |
    |    |         |                |                   |
    +----|---+-----| GND        SDA |-------------------+-----<> SDA
         |   |     +----------------+
         |   v GND
         |
         '-----------------------------------------------------> NeoPixel Data

```

## Getting the code

This project uses git submodules (I kinda wish it didn't...). You can clone
it like so:

```git clone --recursive https://github.com/usedbytes/neopixel_i2c```

If your git version is too old to support that, do this instead:

```
git clone --recursive https://github.com/usedbytes/neopixel_i2c
cd neopixel_i2c
git submodule update --init --recursive
```


## Basic Functionality

There are two basic operating modes:
 * In *normal* mode, each LED is individually driven based on the value in its
   control register.
 * In *global* mode, all LEDs are driven to the same value, based on the values
   in the global value registers.

The operating modes are selected by flipping the *GLB* bit in the **CTRL**
register.

## i2c Protocol

**The slave address is currently hardcoded to 0x40 in the firmware**, see

```
i2c/i2c_slave_defs.h:30
```


This utilises my i2c slave library (https://github.com/usedbytes/usi_i2c_slave)
which means it follows a fairly standard i2c protocol (for more, see here:
http://www.robot-electronics.co.uk/i2c-tutorial).

Writes look like this:

| Start | Slave Address << 1 | Register Address | Data | Stop |
|-------|--------------------|------------------|------|------|

The register address will auto-increment after every byte, so you can write
data in bursts.

Reads look like this:

| Start | Slave Address | Register Address | Restart | (Slave Address << 1) + 1  | Data | Stop |
|-------|---------------|------------------|---------|---------------------------|------|------|

First you do a write transaction to set the register address to read from, then
a read transaction to read the data. When you've read all the data you want,
send a NAK after the last byte to terminate the read.

**The LED values are only updated after a STOP is received**

## Register Map
The register map consists of a number of global control registers - address
0x0-0x3 - followed by an array of registers which hold the individual value
for each LED in normal mode.

| **Address** | **Name**     | **Description**      | **Access** | **Reset** |
|------------:|--------------|:---------------------|--------|------:|
|   0x00      | **CTRL**     | Control Register     | R/W    |    0  |
|   0x01      | **GLB_G**    | Global Green Value   | R/W    |    0  |
|   0x02      | **GLB_R**    | Global Red Value     | R/W    |    0  |
|   0x03      | **GLB_B**    | Global Blue Value    | R/W    |    0  |
|Array follows|--------------|----------------------|--------|-------|
|   0x04      | **GREEN[0]** | Green value, LED0    | R/W    |    0  |
|   0x05      | **RED[0]**   | Red value, LED0      | R/W    |    0  |
|   0x06      | **BLUE[0]**  | Blue value, LED0     | R/W    |    0  |
|   ....      | ....         | ....                 | ....   | ....  |
| (3*n) + 4   | **GREEN[n]** | Green value, LEDn    | R/W    |    0  |
| (3*n) + 5   | **RED[n]**   | Red value, LEDn      | R/W    |    0  |
| (3*n) + 6   | **BLUE[n]**  | Blue value, LEDn     | R/W    |    0  |

## Register Descriptions

### **CTRL**
The control register sets the operating mode.

|   Name: | RSVD | RSVD | RSVD | RSVD | RSVD | RSVD | GLB  | RST  |
|--------:|:----:|:----:|:----:|:----:|:----:|:----:|:----:|:----:|
|    Bit: |    7 |    6 |    5 |    4 |    3 |    2 |    1 |    0 |
| Access: |    r |    r |   r  |   r  |   r  |   r  |  rw  |  rw  |

#### *RST*
Writing a 1 to this bit will reset the LED controler, setting all LEDs to OFF

This bit will be automatically cleared once the reset has completed.

#### *GLB*
Writing a one to this bit causes the global color value to be displayed on all
LEDs at the end of the transaction - Normally you would set the **GLB_R**,
**GLB_G**, and **GLB_B** values in the same transaction as setting the *GLB*
bit so that the new colour is immediately applied.

Writing a zero to this bit will disable the global colour override and return to
normal operation.

### **GLB_R**, **GLB_G**, **GLB_B**
These registers hold the global colour value. When the *GLB* bit in the
**CTRL** register is set, all LEDs will display this colour.

### **LED Value Array**
Everything after the global registers is an array of data for each LED.
When the *GLB* bit is not set, each LED will display whatever value is
programmed in its corresponding register set.
