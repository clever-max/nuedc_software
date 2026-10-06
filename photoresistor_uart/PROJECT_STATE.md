# photoresistor_uart project continuation state

- Project: `photoresistor_uart`
- Status: `example`
- Last handoff: `2026-10-07`
- Stable artifact: `photoresistor_uart/photoresistor_uart.hex`

## Current objective

Maintain the LM393 photoresistor ADC, digital input, board LED, and UART monitor example.

## Current implementation

- Entry: `photoresistor_uart.c`; SysConfig source: `photoresistor_uart.syscfg`.
- AO is PA27/ADC0 channel 0; DO is PA26 with pull-up; VCC is 3V3 and GND is common.
- Board UART0/CH340E uses PA10 TX and PA11 RX at 115200-8-N-1.
- `photoresistor_monitor.py` displays measurements and sends threshold commands.

## Validation

- An Intel HEX artifact and Python monitor are present.
- Physical ADC voltage, DO threshold, LED response, and serial output require a connected board.

## Known issues

- The software voltage conversion assumes a 3.3V VDDA; confirm this if the hardware power scheme changes.

## Next actions

- When changing the sensor or UART, update the `.syscfg`, regenerate, rebuild, validate the HEX, and record a short monitor trace.

## Handoff log

- `2026-10-07` — Initial committed continuation state created.
