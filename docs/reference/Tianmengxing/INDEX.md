# Tianmengxing MSPM0G3507 shared reference

This is the workspace-wide, offline-searchable reference for projects that use the LCKFB Tianmengxing MSPM0G3507 board. It is shared by every immediate-child project. Read this index before using the board pin map or connector assumptions.

## Search locally

The `text/` directory contains page-numbered text extracted from the supplied board schematic, pin-assignment drawing, TI datasheet, TI technical reference manual, and TI hardware guide. The `examples/` directories contain searchable application source/configuration files selected from the vendor CCS and Keil example archives; generated build folders and SysConfig-generated outputs were omitted.

From the workspace root, search with:

```powershell
rg -n "PB22|PA10|TIMG8|BSL|5-V tolerant" docs/reference/Tianmengxing/text docs/reference/Tianmengxing/examples
```

## Source documents

| Topic | Original document | Searchable extraction |
| --- | --- | --- |
| Tianmengxing connector and MCU pin allocation | [Pin assignment drawing](source/hardware/Tianmengxing_MSPM0G3507_pin_assignment_2024-10-21.pdf) | [Extracted text](text/hardware/Tianmengxing_MSPM0G3507_pin_assignment_2024-10-21.txt) |
| Board schematic, power rails and headers | [Board schematic](source/hardware/Tianmengxing_MSPM0G3507_schematic_2024-11-14.pdf) | [Extracted text](text/hardware/Tianmengxing_MSPM0G3507_schematic_2024-11-14.txt) |
| MCU electrical limits and pin mux | [MSPM0G350x datasheet, Rev A, Chinese](source/ti_docs/MSPM0G350x_datasheet_RevA_zh.pdf) | [Extracted text](text/ti_docs/MSPM0G350x_datasheet_RevA_zh.txt) |
| Peripheral registers and operating details | [MSPM0G technical reference manual, Rev A, Chinese](source/ti_docs/MSPM0G_Technical_Reference_Manual_RevA_zh.pdf) | [Extracted text](text/ti_docs/MSPM0G_Technical_Reference_Manual_RevA_zh.txt) |
| Board-level hardware design guidance | [MSPM0G hardware development guide, Rev B](source/ti_docs/MSPM0G_Hardware_Development_Guide_RevB.pdf) | [Extracted text](text/ti_docs/MSPM0G_Hardware_Development_Guide_RevB.txt) |

## Example code

- [CCS examples](examples/ccs/) — LED, delay, key, GPIO interrupt, UART, timer, PWM, ADC, DMA, I²C, SPI, and other included exercises where present.
- [Keil examples](examples/keil/) — selected C source, SysConfig files, project metadata, startup source and linker scatter files.
- The copied examples target the development-board configuration recorded in their own `.syscfg`; treat them as examples and check the active project's `.syscfg` before reusing a pin or peripheral assignment.

## H8 SPI-LCD header pin map

The Tianmengxing schematic labels the 8-pin H8 connector as follows. Pins 1/2 are resistor-selectable board power rails (the schematic notes the default as pin 1=GND and pin 2=VCC/3.3 V); they are not GPIO signals. Pins 3-8 map to MCU GPIOs:

| H8 pin | Schematic signal | MCU pin | Relevant timer function |
| --- | --- | --- | --- |
| 1 | Power P1 | GND by default | Not a signal |
| 2 | Power P2 | VCC/3.3 V by default | Not a signal |
| 3 | LCD_SCL | PB9 | TIMA0_C1 |
| 4 | LCD_SDA | PB8 | TIMA0_C0 |
| 5 | LCD_RES | PB10 | TIMG8_C0 |
| 6 | LCD_DC | PB11 | TIMG8_C1 |
| 7 | LCD_CS | PB14 | TIMA0_C0 (also other mux options) |
| 8 | BLK | PB26 | TIMG6_C0 / TIMA1_C0 |

For a dual AT8236 bridge needing four PWM inputs, H8 pins 3-6 provide two convenient timer-channel pairs: PB9/PB8 on TIMA0_C1/C0 and PB10/PB11 on TIMG8_C0/C1. Pins 7/8 also have timer mux options, but pins 3-6 make the two direct channel pairs used by the current car project. This repurposes LCD signal lines; disconnect the display while using them for motor control. Verify the current project's `.syscfg` and the actual H8 resistor population before wiring.

## Existing workspace knowledge

- [Tianmengxing Wiki summary](../../tmx-mspm0g3507-wiki-summary.md) covers board resources, module tutorials, wiring examples and the original Wiki URLs.
- [LP-MSPM0G3507 LaunchPad reference](../LP-MSPM0G3507/INDEX.md) is a different board's source set. Do not mix its connector or pin assumptions with Tianmengxing.

## Original Wiki shortcuts in the supplied archive

The supplied HTML files are redirect stubs. Their destinations are recorded in [wiki_shortcuts.md](wiki_shortcuts.md); the detailed local Wiki summary above remains the first search stop.

## Scope and precedence

The supplied folder also contains large installers and USB/serial/J-Link driver packages. They are not copied into this Git reference: they are executable machine dependencies, not board design references. For driver/IDE installation, use the vendor's current official distribution. The datasheet and board schematic are hardware references; the current project's `.syscfg`, generated header, and target configuration remain authoritative for that project's actual settings. Distinguish board facts, tutorial examples, and measurements made on a specific project.
