#!/usr/bin/env python3
"""Display and validate JY61/JY61S UART frames on a PC.

The monitor accepts the standard 11-byte 0x55 0x51/0x52/0x53 frames:
acceleration, angular velocity and Euler angles.  It is deliberately
independent of the car firmware so the sensor and USB-TTL link can be checked
before connecting the sensor to the MSPM0 I2C bus.
"""

from __future__ import annotations

import argparse
import struct
import sys
import time
from dataclasses import dataclass

import serial


FRAME_LENGTH = 11
FRAME_HEADER = 0x55


@dataclass
class Measurements:
    accel_g: tuple[float, float, float] | None = None
    gyro_dps: tuple[float, float, float] | None = None
    angle_deg: tuple[float, float, float] | None = None
    magnetic_raw: tuple[int, int, int] | None = None


def decode_frame(frame: bytes, measurements: Measurements) -> str | None:
    if len(frame) != FRAME_LENGTH or frame[0] != FRAME_HEADER:
        return None
    if sum(frame[:10]) & 0xFF != frame[10]:
        return "checksum"

    kind = frame[1]
    values = struct.unpack_from("<hhh", frame, 2)
    if kind == 0x51:
        measurements.accel_g = tuple(value / 32768.0 * 16.0 for value in values)
        return "accel"
    if kind == 0x52:
        measurements.gyro_dps = tuple(value / 32768.0 * 2000.0 for value in values)
        return "gyro"
    if kind == 0x53:
        measurements.angle_deg = tuple(value / 32768.0 * 180.0 for value in values)
        return "angle"
    if kind == 0x54:
        measurements.magnetic_raw = values
        return "magnetic"
    return "unknown"


def format_measurements(measurements: Measurements, frames: int, bad: int) -> str:
    def fmt(values: tuple[float, float, float] | None) -> str:
        if values is None:
            return "--"
        return "/".join(f"{value:8.2f}" for value in values)

    return (
        f"acc[g]={fmt(measurements.accel_g)}  "
        f"gyro[dps]={fmt(measurements.gyro_dps)}  "
        f"angle[deg]={fmt(measurements.angle_deg)}  "
        f"mag[raw]={fmt(measurements.magnetic_raw)}  "
        f"frames={frames} bad={bad}"
    )


def run(port: str, baudrate: int, duration: float) -> int:
    try:
        connection = serial.Serial(port, baudrate, timeout=0.05)
    except serial.SerialException as exc:
        print(f"Serial error: {exc}", file=sys.stderr)
        return 2

    measurements = Measurements()
    buffer = bytearray()
    frame_count = 0
    bad_count = 0
    started = time.monotonic()
    next_report = started
    print(f"Opened {port} at {baudrate} 8N1; waiting for JY61S 0x55 frames")
    try:
        while duration <= 0.0 or time.monotonic() - started < duration:
            data = connection.read(256)
            if data:
                buffer.extend(data)
            while True:
                try:
                    offset = buffer.index(FRAME_HEADER)
                except ValueError:
                    buffer.clear()
                    break
                if offset:
                    del buffer[:offset]
                if len(buffer) < FRAME_LENGTH:
                    break
                frame = bytes(buffer[:FRAME_LENGTH])
                result = decode_frame(frame, measurements)
                if result in ("accel", "gyro", "angle", "magnetic"):
                    del buffer[:FRAME_LENGTH]
                    frame_count += 1
                else:
                    # A checksum failure may have started from a payload byte
                    # equal to 0x55. Drop one byte and search again so a single
                    # startup alignment loss does not poison the whole stream.
                    del buffer[:1]
                    bad_count += 1

            now = time.monotonic()
            if now >= next_report:
                print(format_measurements(measurements, frame_count, bad_count), flush=True)
                next_report = now + 0.5
    finally:
        connection.close()

    if frame_count == 0:
        print("No valid JY61S frames received. Check serial mode, TX/RX crossover, baud and GND.")
        return 1
    print(f"Done: {frame_count} valid frames, {bad_count} invalid/unknown frames")
    return 0


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--port", "-p", required=True, help="USB-TTL COM port, for example COM8")
    parser.add_argument("--baudrate", "-b", type=int, default=115200)
    parser.add_argument("--duration", "-d", type=float, default=15.0,
                        help="seconds; use 0 to run until Ctrl+C")
    args = parser.parse_args()
    return run(args.port, args.baudrate, args.duration)


if __name__ == "__main__":
    raise SystemExit(main())
