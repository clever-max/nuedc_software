# LP MSPM0G3507 Local Reference

This directory is a local, offline reference cache for the TI LP-MSPM0G3507 LaunchPad.
The source page is:

<https://www.ti.com/tool/LP-MSPM0G3507>

The downloaded files are kept locally for development and search. Large vendor binaries and
extracted hardware files are intentionally ignored by Git; this index remains versioned.

## Local Files

| Local item | Purpose |
| --- | --- |
| `source_page.html` | Snapshot of the TI product page |
| `MSPM0G3507_LaunchPad_User_Guide_RevE.pdf` | Current Rev. E user guide |
| `text/MSPM0G3507_LaunchPad_User_Guide_RevE.txt` | Layout-preserving text extraction for `rg` search |
| `SLAR172A_Hardware_Design_Files.zip` | TI hardware design archive, Rev. A |
| `hardware/` | Extracted hardware archive, including schematic, assembly, BOM, PCB and manufacturing files |

## Important Local Facts

- Target MCU: MSPM0G3507
- CPU: Arm Cortex-M0+ up to 80 MHz
- Memory: 128 KB flash and 32 KB SRAM
- Onboard debug probe: XDS110
- Red LED1: PA0, connected through J4; it is active-low
- RGB LED: PB22 blue, PB26 red, PB27 green through J5-J7
- PA0 open-drain pull-up: J19
- UART0/XDS back-channel selection: J21 and J22
- QEI extension header: J12

## Search Locally

Search the text version of the user guide:

```powershell
rg -n -i "PA0|LED1|J4|J19|J101|SWD|BoosterPack|UART|QEI" `
  docs\reference\LP-MSPM0G3507\text\MSPM0G3507_LaunchPad_User_Guide_RevE.txt
```

List extracted hardware files:

```powershell
Get-ChildItem docs\reference\LP-MSPM0G3507\hardware -Recurse -File
```

## Download Metadata

| Item | Source | SHA-256 |
| --- | --- | --- |
| Product page snapshot | `https://www.ti.com/tool/LP-MSPM0G3507` | `BDFE193455E300A6D4CD040F6246DDA16B91DD6458AD872E37CCB06E233FEA11` |
| User guide Rev. E | `https://www.ti.com/lit/pdf/SLAU873` | `1B38137A5E569A2AC608DF7F8371F53448CD01CDA7914EE25C62F06239424957` |
| Hardware design files Rev. A | `https://www.ti.com/lit/zip/SLAR172` | `6FB5F6CF6EEE496171A5F19E9117DE267836CA0C915722E1A493386D67C7048A` |

Downloaded on 2026-09-26.

