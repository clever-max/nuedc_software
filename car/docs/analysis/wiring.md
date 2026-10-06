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

## MPU6050（I²C 模式）

| MPU6050 signal | MCU pin | Function |
| --- | --- | --- |
| SCL | PB2 | I2C1_SCL |
| SDA | PB3 | I2C1_SDA |
| INT | PB1 | GPIO input, reserved data-ready edge |

The active driver uses the raw MPU6050 7-bit address `0x68`, configures ±250 dps and samples the Z-axis gyro through I2C1. It removes a 100-sample static bias and applies a first-order rate filter before integrating yaw. The external CH340 debug console is UART1: PB4=TX and PB5=RX. Keep the car still after reset until the bias calibration has completed.

## Buzzer

The passive buzzer signal is on PB27. The pin is configured as a GPIO output and starts low. The active demo toggles it at a low audible rate for start/stop notification; do not drive a high-current buzzer directly from the MCU pin.

## Gray sensor reservation

The current user arrangement uses PA28/PA31 as a software I2C bus for the NCHD1/NCHD12 gray sensor. These pins are reserved as `GRAY_SENSOR_BUS` in SysConfig and are dynamically switched by `bsp/gray_sensor.c` to emulate open-drain I2C. Do not use PA0/PA1 for gray I2C because they are motor PWM. See the shared sensor index at `../../../docs/reference/sensors/INDEX.md`.

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
