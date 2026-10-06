#!/usr/bin/env python3
"""Show and update the small, committed handoff state for a workspace project.

The state file is deliberately plain Markdown so a new Codex conversation can
read it without depending on this helper. Git facts are printed separately so
uncommitted work is never mistaken for the last recorded handoff.
"""

from __future__ import annotations

import argparse
import datetime as dt
import re
import subprocess
import sys
from pathlib import Path


ROOT = Path(__file__).resolve().parents[1]
STATE_NAME = "PROJECT_STATE.md"
SECTION_NAMES = {
    "objective": "Current objective",
    "implementation": "Current implementation",
    "validation": "Validation",
    "issues": "Known issues",
    "next": "Next actions",
}
COMMIT_REQUIRED_SUFFIXES = {
    ".c", ".h", ".cc", ".cpp", ".py", ".ps1", ".sh", ".bat", ".syscfg",
    ".s", ".S", ".cmd", ".ld", ".hex", ".out", ".elf", ".axf", ".cproject",
    ".ccsproject", ".project", ".uvprojx", ".sct",
}


def run_git(args: list[str], cwd: Path = ROOT) -> str:
    result = subprocess.run(
        ["git", *args], cwd=cwd, text=True, capture_output=True, check=False
    )
    if result.returncode != 0:
        raise SystemExit(result.stderr.strip() or f"git {' '.join(args)} failed")
    return result.stdout.strip()


def project_path(value: str) -> Path:
    candidate = (ROOT / value).resolve()
    try:
        candidate.relative_to(ROOT)
    except ValueError as exc:
        raise SystemExit("project must be inside the workspace") from exc
    if not candidate.is_dir():
        raise SystemExit(f"project directory does not exist: {value}")
    return candidate


def state_path(project: Path) -> Path:
    return project / STATE_NAME


def today() -> str:
    return dt.datetime.now().astimezone().strftime("%Y-%m-%d %H:%M %z")


def git_facts(project: Path) -> tuple[str, str, str]:
    branch = run_git(["branch", "--show-current"])
    commit = run_git(["rev-parse", "--short", "HEAD"])
    status = run_git(["status", "--short", "--", str(project.relative_to(ROOT))])
    return branch or "(detached)", commit, status


def changed_paths(status: str) -> list[str]:
    paths = []
    for line in status.splitlines():
        if len(line) < 4:
            continue
        value = line[3:].strip()
        if " -> " in value:
            paths.extend(part.strip() for part in value.split(" -> "))
        else:
            paths.append(value)
    return paths


def requires_commit(path: str) -> bool:
    return Path(path).suffix in COMMIT_REQUIRED_SUFFIXES


def template(project: Path, status: str = "active") -> str:
    name = project.relative_to(ROOT).as_posix()
    return f"""# {name} project continuation state

This file is the committed handoff point for a new Codex conversation. Read it
with the project `AGENTS.md`, the project `README.md`, and the current Git
status before changing files.

- Project: `{name}`
- Status: `{status}`
- Last handoff: `{today()}`
- Last recorded branch: `(run project_context.py update after the first handoff)`
- Last recorded commit: `(unknown)`

## Current objective

Record the active user objective here.

## Current implementation

Record the important source/configuration facts here. Keep generated files and
machine-specific paths out of this section.

## Validation

Record source checks, SysConfig generation, compilation, linking, flash-tool
results, and physical/serial observations separately.

## Known issues

Record unresolved hardware, software, or measurement issues here.

## Next actions

Record the next concrete action a new conversation should take.

## Handoff log

- `{today()}` — state file initialized.
"""


def replace_field(text: str, label: str, value: str) -> str:
    pattern = rf"(?m)^- {re.escape(label)}:.*$"
    replacement = f"- {label}: `{value}`" if value else f"- {label}:"
    if re.search(pattern, text):
        return re.sub(pattern, replacement, text, count=1)
    return text


def replace_section(text: str, heading: str, value: str) -> str:
    pattern = rf"(?ms)^## {re.escape(heading)}\n.*?(?=^## |\Z)"
    body = value.strip() or "(not recorded)"
    replacement = f"## {heading}\n\n{body}\n\n"
    if re.search(pattern, text):
        return re.sub(pattern, replacement, text, count=1)
    return text.rstrip() + "\n\n" + replacement


def init(project: Path, status: str) -> None:
    path = state_path(project)
    if path.exists():
        raise SystemExit(f"state already exists: {path.relative_to(ROOT)}")
    path.write_text(template(project, status), encoding="utf-8")
    print(path.relative_to(ROOT).as_posix())


def update(project: Path, args: argparse.Namespace) -> None:
    path = state_path(project)
    if not path.exists():
        path.write_text(template(project), encoding="utf-8")
    text = path.read_text(encoding="utf-8")
    branch, commit, _ = git_facts(project)
    if args.status is not None:
        text = replace_field(text, "Status", args.status)
    text = replace_field(text, "Last handoff", today())
    text = replace_field(text, "Last recorded branch", branch)
    text = replace_field(text, "Last recorded commit", commit)
    for key, heading in SECTION_NAMES.items():
        value = getattr(args, key)
        if value is not None:
            text = replace_section(text, heading, value)
    if args.note:
        marker = "## Handoff log\n"
        entry = f"- `{today()}` — {args.note.strip()} (commit `{commit}`, branch `{branch}`).\n"
        if marker in text:
            text = text.replace(marker, marker + "\n" + entry, 1)
        else:
            text = text.rstrip() + "\n\n" + marker + "\n" + entry
    path.write_text(text.rstrip() + "\n", encoding="utf-8")
    print(path.relative_to(ROOT).as_posix())


def show(project: Path) -> None:
    branch, commit, status = git_facts(project)
    path = state_path(project)
    print(f"Project: {project.relative_to(ROOT).as_posix()}")
    print(f"Git branch: {branch}")
    print(f"Git HEAD: {commit}")
    print("Worktree: clean" if not status else "Worktree changes:\n" + status)
    print(f"State file: {path.relative_to(ROOT).as_posix()}")
    print("\n--- committed handoff state ---")
    if path.exists():
        print(path.read_text(encoding="utf-8"), end="")
    else:
        print("(missing; run init)")


def guard(project: Path) -> int:
    """Fail when code/config/build/artifact files are not committed."""
    _, commit, status = git_facts(project)
    paths = [path for path in changed_paths(status) if requires_commit(path)]
    if paths:
        print("COMMIT_REQUIRED: uncommitted code/config/artifact files detected")
        for path in paths:
            print(f"- {path}")
        print(f"Current HEAD: {commit}")
        print("Create a Git commit before reporting this work complete.")
        return 2
    print(f"COMMIT_OK: no uncommitted code/config/artifact files for {project.relative_to(ROOT).as_posix()}")
    return 0


def parser() -> argparse.ArgumentParser:
    p = argparse.ArgumentParser(description=__doc__)
    sub = p.add_subparsers(dest="command", required=True)
    for name in ("show", "init", "update", "guard"):
        cmd = sub.add_parser(name)
        cmd.add_argument("project", help="immediate child project directory")
        if name == "init":
            cmd.add_argument("--status", default="active")
        if name == "update":
            cmd.add_argument("--status")
            for key in SECTION_NAMES:
                cmd.add_argument(f"--{key}", help=f"replace the {SECTION_NAMES[key]} section")
            cmd.add_argument("--note", help="append one handoff log entry")
    return p


def main() -> int:
    if hasattr(sys.stdout, "reconfigure"):
        sys.stdout.reconfigure(encoding="utf-8")
    args = parser().parse_args()
    project = project_path(args.project)
    if args.command == "show":
        show(project)
    elif args.command == "init":
        init(project, args.status)
    elif args.command == "guard":
        return guard(project)
    else:
        update(project, args)
    return 0


if __name__ == "__main__":
    sys.exit(main())
