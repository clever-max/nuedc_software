#!/usr/bin/env python3
"""Validate the repository's continuity and documentation gates."""

from __future__ import annotations

import re
import sys
from pathlib import Path


ROOT = Path(__file__).resolve().parents[1]
PROJECTS = (
    "breathing_led",
    "button_led_test",
    "car",
    "empty",
    "open_loop_motor_test",
    "photoresistor_uart",
)
ABSOLUTE_PATH = re.compile(r"(?:[A-Za-z]:[\\/]|/Users/|/home/|/mnt/)")


def main() -> int:
    errors: list[str] = []
    for name in PROJECTS:
        project = ROOT / name
        state = project / "PROJECT_STATE.md"
        agents = project / "AGENTS.md"
        if not state.is_file():
            errors.append(f"missing {state.relative_to(ROOT)}")
        if not agents.is_file():
            errors.append(f"missing {agents.relative_to(ROOT)}")
        elif "project_context.py guard" not in agents.read_text(encoding="utf-8"):
            errors.append(f"{agents.relative_to(ROOT)} does not require project_context guard")

    required_docs = (
        ROOT / "docs/CONTINUATION_WORKFLOW.md",
        ROOT / "docs/DOCUMENTATION_STRUCTURE.md",
        ROOT / "docs/reference/README.md",
        ROOT / "docs/reference/DOCUMENT_DUPLICATES.md",
        ROOT / "docs/reference/00_inbox/README.md",
        ROOT / "docs/reference/manifests/README.md",
        ROOT / "docs/reference/99_quarantine/README.md",
    )
    for path in required_docs:
        if not path.is_file():
            errors.append(f"missing {path.relative_to(ROOT)}")

    for path in [ROOT / "docs/CONTINUATION_WORKFLOW.md", ROOT / "docs/DOCUMENTATION_STRUCTURE.md"]:
        if path.is_file() and ABSOLUTE_PATH.search(path.read_text(encoding="utf-8")):
            errors.append(f"absolute path found in {path.relative_to(ROOT)}")

    if errors:
        for error in errors:
            print(f"WORKFLOW_ERROR: {error}")
        return 1
    print("WORKFLOW_OK: project states, documentation layers, and portability checks passed")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
