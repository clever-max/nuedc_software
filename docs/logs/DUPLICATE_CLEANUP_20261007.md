# Duplicate cleanup log

Date: `2026-10-07`

The following exact duplicate documentation files were deleted after manual review. Source/example SDK files were retained to preserve runnable reference packages.

## Deleted `docs/reference/sensors/text/NCHD1__灰度传感器用户使用手册（迹系列20230119).pdf.txt`
- SHA256: `8efe9c2c06f5118f38353f58290aba6be4403ed22ff78364e95e36f0b2313ab7`
- Canonical retained: `docs/reference/sensors/text/NCHD1_user_manual_20230119.txt`
- Reason: duplicate extracted manual text; ASCII filename retained

## Deleted `docs/reference/sensors/NCHD1_I2C/I2C读取多组12路灰度传感器实验/多组12路灰度传感器检测（MSPM0G3507）/ReadMe.md`
- SHA256: `f9ab0aabba993ada59bff615011dadd66dff4b3862a2d78147c2418c62dda6e3`
- Canonical retained: `docs/reference/sensors/NCHD1_I2C/.../单个.../ReadMe.md`
- Reason: duplicate example README; single-board archive copy retained

## Deleted `docs/reference/sensors/NCHD1_I2C/I2C读取多组12路灰度传感器实验/多组12路灰度传感器检测（MSPM0G3507）/硬件连接说明.txt`
- SHA256: `8a05dd2a57e1b332e56cbc46d82f6fbb17321170d844a04379bfd454e96baf53`
- Canonical retained: `docs/reference/sensors/examples/nchd12_mspm0g3507/硬件连接说明.txt`
- Reason: duplicate hardware note; filtered MSPM0 example retained

## Deleted `docs/reference/sensors/NCHD1_I2C/I2C读取单个12路灰度传感器例程/12路灰度传感器检测（MSPM0G3507）/硬件连接说明.txt`
- SHA256: `8a05dd2a57e1b332e56cbc46d82f6fbb17321170d844a04379bfd454e96baf53`
- Canonical retained: `docs/reference/sensors/examples/nchd12_mspm0g3507/硬件连接说明.txt`
- Reason: duplicate hardware note; filtered MSPM0 example retained

## Retained on purpose

- Duplicate MSPM0/TIVA/STM32 example source and SysConfig files were retained when removing them would make a vendor example incomplete.
- Repeated TI SDK/linker/startup/ODB files were retained as package content and are not deletion candidates by hash alone.

## Deleted duplicate NCHD1 manual PDF
- Original filename: `灰度传感器用户使用手册（迹系列20230119).pdf`
- SHA256: `c0f42b317dd1dafa99f53b63e9bef5850148173eb7c9a1c3ee3630203d36b3d4`
- Canonical retained: `docs/reference/sensors/NCHD1/NCHD1_user_manual_20230119.pdf`
- Reason: duplicate manual PDF; ASCII filename retained.

## Deleted duplicate example archive files
- Removed exact copies in `docs/reference/sensors/NCHD1_I2C/` for the 16 candidate groups listed in the duplicate report.
- Canonical retained: `docs/reference/sensors/examples/nchd12_mspm0g3507/`.
- Files removed: 31.
  - `docs/reference/sensors/NCHD1_I2C/I2C读取单个12路灰度传感器例程/12路灰度传感器检测（MSPM0G3507）/main.c` — `6550ed12c3a0029a917380eee5b6479ce2f68e2b943655793583a226ce2b82dc`
  - `docs/reference/sensors/NCHD1_I2C/I2C读取单个12路灰度传感器例程/12路灰度传感器检测（MSPM0G3507）/ncontroller.syscfg` — `22d73947f9e1490485c3f24c9a03e7e2d6386a73841509d95f0eab5b6d84dd85`
  - `docs/reference/sensors/NCHD1_I2C/I2C读取单个12路灰度传感器例程/12路灰度传感器检测（MSPM0G3507）/ndrivers/binary.h` — `eb269f852b00983e93f106daf35b708a75334b7e38f6b25510363f72a781950c`
  - `docs/reference/sensors/NCHD1_I2C/I2C读取单个12路灰度传感器例程/12路灰度传感器检测（MSPM0G3507）/ndrivers/drv_oled.c` — `bb174faf95f8b0a48dce8498e55266a74aefe302238fdda5d75f336a739fe2dd`
  - `docs/reference/sensors/NCHD1_I2C/I2C读取单个12路灰度传感器例程/12路灰度传感器检测（MSPM0G3507）/ndrivers/drv_oled.h` — `c2af636e41429e9cf836655cad6da588d227c59af976cf7c4d6a4dd4655be023`
  - `docs/reference/sensors/NCHD1_I2C/I2C读取单个12路灰度传感器例程/12路灰度传感器检测（MSPM0G3507）/ndrivers/glcdfont.c` — `b712fafc9bd3369094f593f7e1c21164581d2da771de5f4a5742b6c96c133e31`
  - `docs/reference/sensors/NCHD1_I2C/I2C读取单个12路灰度传感器例程/12路灰度传感器检测（MSPM0G3507）/ndrivers/gray_detection.c` — `52bc70e6d17b2b3f203993698fccc65c1f9f479490681300d860905f8277435b`
  - `docs/reference/sensors/NCHD1_I2C/I2C读取单个12路灰度传感器例程/12路灰度传感器检测（MSPM0G3507）/ndrivers/gray_detection.h` — `32259d4c0a9baa7cae1abe151d3ef12c56e57e4dcc7b63090dccabc7236f194a`
  - `docs/reference/sensors/NCHD1_I2C/I2C读取单个12路灰度传感器例程/12路灰度传感器检测（MSPM0G3507）/ndrivers/nchd12.c` — `fb2e2f824645c8055a9686e69caee21542110bdb48b324d274ab3e352f6e8a19`
  - `docs/reference/sensors/NCHD1_I2C/I2C读取单个12路灰度传感器例程/12路灰度传感器检测（MSPM0G3507）/ndrivers/nchd12.h` — `fcf1ce5b97a3c6ba5d083c08468f0392e08d93ba97f50fe0fd170de51abb70e9`
  - `docs/reference/sensors/NCHD1_I2C/I2C读取单个12路灰度传感器例程/12路灰度传感器检测（MSPM0G3507）/ndrivers/soft_i2c.c` — `1a4a5a996abe43954c5265393fbeec5b148dc48013a5c9f80654821595811a2e`
  - `docs/reference/sensors/NCHD1_I2C/I2C读取单个12路灰度传感器例程/12路灰度传感器检测（MSPM0G3507）/ndrivers/soft_i2c.h` — `40bb372b0b69c3124ca1a30ee51e1654373a2708b1482630542020a755173c7e`
  - `docs/reference/sensors/NCHD1_I2C/I2C读取单个12路灰度传感器例程/12路灰度传感器检测（MSPM0G3507）/ndrivers/ssd1306.c` — `c73b8243c0152cd3a71ec3eb9451d20748e2d16fa25c473c1b70dcfc0e7ea194`
  - `docs/reference/sensors/NCHD1_I2C/I2C读取单个12路灰度传感器例程/12路灰度传感器检测（MSPM0G3507）/ndrivers/ssd1306.h` — `38e2efb9d1cb5ecf4ce058a06202ca2b852196cb4c8abbc88e146d7eaa4bf2bf`
  - `docs/reference/sensors/NCHD1_I2C/I2C读取单个12路灰度传感器例程/12路灰度传感器检测（MSPM0G3507）/ti_msp_dl_config.c` — `4e0f19b78389c9bbfe3caad2536a878cad90dc436fe51af077a9edf82c46436a`
  - `docs/reference/sensors/NCHD1_I2C/I2C读取单个12路灰度传感器例程/12路灰度传感器检测（MSPM0G3507）/ti_msp_dl_config.h` — `ee5fa8c8ed41b57efd183cd3834b326bd8caa9f40538695d1a244967b898816d`
  - `docs/reference/sensors/NCHD1_I2C/I2C读取多组12路灰度传感器实验/多组12路灰度传感器检测（MSPM0G3507）/ncontroller.syscfg` — `22d73947f9e1490485c3f24c9a03e7e2d6386a73841509d95f0eab5b6d84dd85`
  - `docs/reference/sensors/NCHD1_I2C/I2C读取多组12路灰度传感器实验/多组12路灰度传感器检测（MSPM0G3507）/ndrivers/binary.h` — `eb269f852b00983e93f106daf35b708a75334b7e38f6b25510363f72a781950c`
  - `docs/reference/sensors/NCHD1_I2C/I2C读取多组12路灰度传感器实验/多组12路灰度传感器检测（MSPM0G3507）/ndrivers/drv_oled.c` — `bb174faf95f8b0a48dce8498e55266a74aefe302238fdda5d75f336a739fe2dd`
  - `docs/reference/sensors/NCHD1_I2C/I2C读取多组12路灰度传感器实验/多组12路灰度传感器检测（MSPM0G3507）/ndrivers/drv_oled.h` — `c2af636e41429e9cf836655cad6da588d227c59af976cf7c4d6a4dd4655be023`
  - `docs/reference/sensors/NCHD1_I2C/I2C读取多组12路灰度传感器实验/多组12路灰度传感器检测（MSPM0G3507）/ndrivers/glcdfont.c` — `b712fafc9bd3369094f593f7e1c21164581d2da771de5f4a5742b6c96c133e31`
  - `docs/reference/sensors/NCHD1_I2C/I2C读取多组12路灰度传感器实验/多组12路灰度传感器检测（MSPM0G3507）/ndrivers/nchd12.c` — `fb2e2f824645c8055a9686e69caee21542110bdb48b324d274ab3e352f6e8a19`
  - `docs/reference/sensors/NCHD1_I2C/I2C读取多组12路灰度传感器实验/多组12路灰度传感器检测（MSPM0G3507）/ndrivers/nchd12.h` — `fcf1ce5b97a3c6ba5d083c08468f0392e08d93ba97f50fe0fd170de51abb70e9`
  - `docs/reference/sensors/NCHD1_I2C/I2C读取多组12路灰度传感器实验/多组12路灰度传感器检测（MSPM0G3507）/ndrivers/soft_i2c.c` — `1a4a5a996abe43954c5265393fbeec5b148dc48013a5c9f80654821595811a2e`
  - `docs/reference/sensors/NCHD1_I2C/I2C读取多组12路灰度传感器实验/多组12路灰度传感器检测（MSPM0G3507）/ndrivers/soft_i2c.h` — `40bb372b0b69c3124ca1a30ee51e1654373a2708b1482630542020a755173c7e`
  - `docs/reference/sensors/NCHD1_I2C/I2C读取多组12路灰度传感器实验/多组12路灰度传感器检测（MSPM0G3507）/ndrivers/ssd1306.c` — `c73b8243c0152cd3a71ec3eb9451d20748e2d16fa25c473c1b70dcfc0e7ea194`
  - `docs/reference/sensors/NCHD1_I2C/I2C读取多组12路灰度传感器实验/多组12路灰度传感器检测（MSPM0G3507）/ndrivers/ssd1306.h` — `38e2efb9d1cb5ecf4ce058a06202ca2b852196cb4c8abbc88e146d7eaa4bf2bf`
  - `docs/reference/sensors/NCHD1_I2C/I2C读取多组12路灰度传感器实验/多组12路灰度传感器检测（MSPM0G3507）/ti_msp_dl_config.c` — `4e0f19b78389c9bbfe3caad2536a878cad90dc436fe51af077a9edf82c46436a`
  - `docs/reference/sensors/NCHD1_I2C/I2C读取多组12路灰度传感器实验/多组12路灰度传感器检测（MSPM0G3507）/ti_msp_dl_config.h` — `ee5fa8c8ed41b57efd183cd3834b326bd8caa9f40538695d1a244967b898816d`
  - `docs/reference/sensors/NCHD1_I2C/I2C读取多组12路灰度传感器实验/多组12路灰度传感器检测（盘古TIVA核心板）/Drivers/binary.h` — `eb269f852b00983e93f106daf35b708a75334b7e38f6b25510363f72a781950c`
  - `docs/reference/sensors/NCHD1_I2C/I2C读取多组12路灰度传感器实验/多组12路灰度传感器检测（盘古TIVA核心板）/Drivers/glcdfont.c` — `b712fafc9bd3369094f593f7e1c21164581d2da771de5f4a5742b6c96c133e31`
