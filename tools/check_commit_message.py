#!/usr/bin/env python3
"""Check one commit message against the local Conventional Commit policy."""

from __future__ import annotations

import re
import sys
from pathlib import Path


PATTERN = re.compile(
    r"^(feat|fix|docs|build|ci|test|refactor|perf|chore|revert)"
    r"(?:\([a-z0-9_.-]+\))?!?: .{1,72}$"
)


def main() -> int:
    if len(sys.argv) != 2:
        print("usage: check_commit_message.py <commit-message-file>")
        return 2
    first_line = Path(sys.argv[1]).read_text(encoding="utf-8").splitlines()[0].strip()
    if PATTERN.match(first_line):
        print(f"COMMIT_MESSAGE_OK: {first_line}")
        return 0
    print("COMMIT_MESSAGE_ERROR: use <type>(optional-scope): <description>")
    print("allowed types: feat fix docs build ci test refactor perf chore revert")
    print(f"received: {first_line}")
    return 1


if __name__ == "__main__":
    raise SystemExit(main())
