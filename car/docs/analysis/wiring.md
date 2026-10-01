# User wiring and project mapping

## Motor control inputs

| MSPM0G3507 port pin | AT8236 input | Status |
| --- | --- | --- |
| PA0 (A0) | AIN1 | user confirmed |
| PA1 (A1) | AIN2 | user confirmed |
| PA8 (A8) | BIN1 | user confirmed |
| PA9 (A9) | BIN2 | user confirmed |

This is the wiring supplied by the user and is configured in `../../car.syscfg`.

On the supplied D157B schematic, the 4-pin `Input_IO` header J4 is pin 1=BIN2, pin 2=BIN1, pin 3=AIN2, pin 4=AIN1. The J8/MOTORC1 and J3/MOTORC2 connectors carry the motor leads; those power outputs do not connect to MSPM0 GPIO.

## Encoder feedback outputs and current Hall version

The user corrected the installed motor type from GMR to **Hall encoder MG513X**. The supplied motor wiring image and local D157B schematic show these 6-pin motor connectors:

| Connector | Pin 1 | Pin 2 | Pin 3 | Pin 4 | Pin 5 | Pin 6 |
| --- | --- | --- | --- | --- | --- | --- |
| MOTORC1 / Motor A | AOUT2 | GND | E1A | E1B | 5V | AOUT1 |
| MOTORC2 / Motor B | BOUT2 | GND | E2A | E2B | 5V | BOUT1 |

The MCU firmware mapping currently uses E1A/E1B→PB0/PB1 and E2A/E2B→PB2/PB3. Pin 5 is labeled 5V in the diagram. The signal high voltage/output topology is not established by this connector drawing; verify before directly wiring A/B into MCU pins. Only designated MSPM0 pins are 5-V tolerant open-drain pins, so do not assume PB0-PB3 accept 5 V. Use compatible 3.3 V signaling or level translation if required.

The previous notes claiming a GMR variant, 500 PPR, and 3.3 V encoder supply were based on the earlier mistaken identification and are superseded. Do not apply the old J5/3V3 wiring claim to the current Hall motor setup without checking the actual harness.
## Configuration and motion start

The firmware uses a 5 ms task to apply the open-loop ramp/timer and publish encoder telemetry. GPIO interrupts only count A-phase rising edges while B phase determines sign. Encoder feedback is diagnostic only and does not correct speed or direction. The firmware remains stopped on reset.

The motor input mapping and 65 mm wheel diameter are user confirmed. The user now identifies the motors as 12 V, 1:28 Hall MG513X with 13 PPR encoders; these values have not yet been independently reconciled with the connector electrical levels or a measured revolution count. Current fixed PWM trims do not ensure straight travel or a specified distance.


