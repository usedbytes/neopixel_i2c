# Register Map

| Address | Name       | Description          | Access | Reset |
|--------:|------------|:---------------------|--------|------:|
|   0x00  | **CTRL**   | Control Register     | R/W    |    0  |
|   0x01  | **GLB_G**  | Global Green Value   | R/W    |    0  |
|   0x02  | **GLB_R**  | Global Red Value     | R/W    |    0  |
|   0x03  | **GLB_B**  | Global Blue Value    | R/W    |    0  |

# Register Descriptions

## **CTRL**
The control register sets the operating mode.

| RSVD | RSVD | RSVD | RSVD | RSVD | RSVD | GLB  | RST  |
|:----:|:----:|:----:|:----:|:----:|:----:|:----:|:----:|
|    7 |    6 |    5 |    4 |    3 |    2 |    1 |    0 |
|    r |    r |   r  |   r  |   r  |   r  |  rw  |  rw  |

### *RST*
Writing a 1 to this bit will reset the LED controler, setting all LEDs to OFF

This bit will be automatically cleared once the reset has completed.

### *GLB*
Writing a one to this bit causes the global color value to be displayed on all
LEDs at the end of the transaction - Normally you would set the **GLB_R**,
**GLB_G**, and **GLB_B** values in the same transaction as setting the *GLB*
bit so that the new colour is immediately applied.

Writing a zero to this bit will disable the global colour override and return to
normal operation.

## **GLB_R**, **GLB_G**, **GLB_B**
These registers hold the global colour value. When the *GLB* bit in the
**CTRL** register is set, all LEDs will display this colour.
