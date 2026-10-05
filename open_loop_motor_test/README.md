# Open-loop motor test

This is an independent copy of the `car` project for isolating the motor driver.
The original `car` directory is not modified by this test firmware.

## Behavior

- B21 or `RUN15`/`RUNPID` starts both motors with a fixed signed command of `250/1000`.
- The motors run for 5 seconds, then both PWM inputs are cleared.
- `STOP` or a second B21 press stops immediately.
- Encoder, wheel PID, gray sensor, MPU6050 and JY61S data are not read by the active app.

## Wiring

Use the existing AT8236 wiring: AIN1/AIN2/BIN1/BIN2 on PA0/PA1/PA8/PA9, common ground and 12 V motor supply.
Keep the wheels lifted for the first test.

## Firmware

Flash [`car.hex`](car.hex). UART1 debug is 115200-8-N-1 on PB4/PB5.

