#!/usr/bin/env python3
"""Validate Intel HEX files for the MSPM0 UART BSL."""
from __future__ import annotations
import argparse
from pathlib import Path
import sys

def validate(path: Path) -> tuple[int, int]:
    lines = path.read_text(encoding="ascii").splitlines()
    if not lines:
        raise ValueError("empty file")
    upper = 0
    records = 0
    data_bytes = 0
    alignment_errors = []
    saw_eof = False
    for line_no, line in enumerate(lines, 1):
        if not line.startswith(":"):
            raise ValueError(f"line {line_no}: missing ':'")
        try:
            raw = bytes.fromhex(line[1:])
        except ValueError as exc:
            raise ValueError(f"line {line_no}: non-hex characters") from exc
        if len(raw) < 5:
            raise ValueError(f"line {line_no}: record too short")
        count = raw[0]
        address = int.from_bytes(raw[1:3], "big")
        record_type = raw[3]
        if len(raw) != count + 5:
            raise ValueError(f"line {line_no}: length mismatch")
        if sum(raw) & 0xFF:
            raise ValueError(f"line {line_no}: checksum mismatch")
        if record_type == 0x00:
            absolute = upper + address
            records += 1
            data_bytes += count
            if absolute % 8 or count % 8:
                alignment_errors.append(f"line {line_no}: address=0x{absolute:X}, length={count}")
        elif record_type == 0x01:
            if count != 0 or address != 0:
                raise ValueError(f"line {line_no}: malformed EOF")
            saw_eof = True
        elif record_type == 0x04:
            if count != 2:
                raise ValueError(f"line {line_no}: malformed extended address")
            upper = int.from_bytes(raw[4:6], "big") << 16
    if not saw_eof:
        raise ValueError("missing EOF record")
    if alignment_errors:
        raise ValueError("BSL alignment errors: " + "; ".join(alignment_errors))
    if records == 0:
        raise ValueError("no data records")
    return records, data_bytes

def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("hex_files", nargs="+", type=Path)
    args = parser.parse_args()
    failed = False
    for path in args.hex_files:
        try:
            records, data_bytes = validate(path)
            print(f"OK {path}: records={records} data_bytes={data_bytes}")
        except (OSError, ValueError) as exc:
            print(f"FAIL {path}: {exc}", file=sys.stderr)
            failed = True
    return 1 if failed else 0

if __name__ == "__main__":
    raise SystemExit(main())
