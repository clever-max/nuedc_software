# Low-speed demo UART operation

The car firmware uses the Tianmengxing board's CH340E USB-UART bridge. Open the corresponding virtual COM port at 115200 baud, 8 data bits, no parity, one stop bit (8-N-1). UART0 routes through PA10 TX and PA11 RX. Send commands with a newline ending.

| Command | Behavior |
| --- | --- |
| `RUN15` | Starts the fixed low-speed demo for 15 seconds, when idle or finished. |
| `STOP` | Aborts a running demo and coasts both motors immediately. |

The on-board B21 button has the same start/abort behavior. At start, PWM ramps over 0.5 seconds. The current provisional fixed commands are A=400‰ and B=190‰. There is no PID, encoder-based correction, distance target, or guarantee of a straight trajectory. When the timer expires, both inputs go low (coast); the car may continue rolling due to inertia. Test in a clear area and keep access to motor power.

Telemetry lines are printed about every 100 ms:

```text
ms=100 state=RUN ticks=.../... speed_mm_s=.../... pos_mm=.../... pwm_permille=400/190
```

Encoder speed and position are estimates based on the user-confirmed 65 mm wheel, 1:28 gear ratio and 13 PPR Hall encoder, counting one A rising edge and sampling B for direction. CPR semantics have not been provided; verify the count scale against one measured wheel revolution. Telemetry is diagnostic only and does not affect motor output. `IDLE`, `RUN`, `DONE`, and `ABORT` indicate the state.

The trace in [serial_debug_20261001.md](serial_debug_20261001.md) was captured from an earlier PID build and must not be treated as a measurement of this open-loop demo.

