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
