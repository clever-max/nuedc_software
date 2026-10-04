# Current user wiring and project mapping

## Motor driver inputs

| MCU pin / timer | AT8236 signal |
| --- | --- |
| PA0 / TIMG8_C1 | AIN1 |
| PA1 / TIMG8_C0 | AIN2 |
| PA8 / TIMA0_C0 | BIN1 |
| PA9 / TIMA0_C1 | BIN2 |

These are the four AT8236 logic inputs. The 12 V motor supply, motor outputs and common ground remain on the AT8236 side. Do not confuse AIN/BIN with the AOUT/BOUT motor terminals.

## Hall encoder inputs

| Motor signal | MCU pin | Mode |
| --- | --- | --- |
| E1A | PA27 | GPIO interrupt, A rising edge |
| E1B | PA25 | GPIO input, direction sample |
| E2A | PB25 | GPIO interrupt, A rising edge |
| E2B | PB20 | GPIO input, direction sample |

The current encoder code uses one A rising edge per encoder PPR cycle and samples B. With the user-provided 13 PPR, 1:28 gear ratio and 65 mm wheel, the initial estimate is 364 counts per wheel revolution. Verify this scale mechanically before treating speed as calibrated.

## JY61S（UART 模式）

| JY61S signal | MCU pin | Function |
| --- | --- | --- |
| TX | PB16 | UART2_RX |
| RX | PB15 | UART2_TX |
| SCL/SDA/INT | open | not used by the active UART driver |

The active driver parses JY61S `0x55 0x52` angular-velocity frames at 115200 baud and calibrates Z-axis bias from 100 samples while the car is still. UART0 PA10/PA11 remains the debug console. Set the sensor to UART mode with the configuration command `FF AA 61` before use.

## Buzzer

The passive buzzer signal is on PB27. The pin is configured as a GPIO output and starts low. The active demo toggles it at a low audible rate for start/stop notification; do not drive a high-current buzzer directly from the MCU pin.

## Gray sensor reservation

The current user arrangement uses PA28/PA31 as a software I2C bus for the NCHD1/NCHD12 gray sensor. Do not use PA0/PA1 for gray I2C because they are motor PWM. See the shared sensor index at `../../../docs/reference/sensors/INDEX.md`.

## NCHD12 12-channel grayscale sensor

| NCHD12 signal | MCU pin | Function |
| --- | --- | --- |
| SCL | PA28 | software I2C clock |
| SDA | PA31 | software I2C data |
| VCC | 3.3V output setting | MSPM0-safe logic level |
| GND | GND | common ground |

The PCA9555-compatible device is read at write/read addresses `0x40/0x41` (7-bit `0x20`). The input register starts at `0x00`; the lower 12 bits are the sensor state.

## Board configuration source

The active mapping is in `../../car.syscfg`. Generated headers under `Debug/` are inspection outputs only. The Tianmengxing pin map and schematic are indexed at `../../../docs/reference/Tianmengxing/INDEX.md`.
