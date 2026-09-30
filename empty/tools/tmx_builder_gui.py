"""Local build/convert/flash GUI for the Tianmengxing MSPM0G3507 CCS project.

This is intentionally a thin wrapper around TI's installed tools:
  CCS Theia headless build -> TI Arm Hex Converter -> DSLite/XDS110 flash.
It never edits SysConfig-generated files and does not perform erase/unlock
operations beyond the explicit DSLite programming command shown in the UI.
"""

from __future__ import annotations

import os
import queue
import shlex
import shutil
import subprocess
import threading
import tkinter as tk
from dataclasses import dataclass
from pathlib import Path
from tkinter import filedialog, messagebox, ttk


PROJECT = Path(__file__).resolve().parents[1]


def discover_ccs() -> Path:
    """Find CCS without embedding a user-specific installation path."""
    for name in ("CCS_ROOT", "TI_CCS_ROOT"):
        value = os.environ.get(name)
        if value:
            candidate = Path(value)
            if candidate.exists():
                return candidate
    for executable in ("ccs-server-cli.bat", "ccs-server-cli"):
        located = shutil.which(executable)
        if located:
            return Path(located).resolve().parents[1]
    return Path("ccs")


CCS = discover_ccs()
CCSTUDIO = CCS / "theia" / "ccstudio.exe"
CCS_SERVER = CCS / "eclipse" / "ccs-server-cli.bat"
DSLite = CCS / "ccs_base" / "DebugServer" / "bin" / "DSLite.exe"
HEX = CCS / "tools" / "compiler" / "ti-cgt-armllvm_5.1.1.LTS" / "bin" / "tiarmhex.exe"
GMAKE = CCS / "utils" / "bin" / "gmake.exe"
CCXML = PROJECT / "targetConfigs" / "MSPM0G3507.ccxml"
OUT = PROJECT / "Debug" / f"{PROJECT.name}.out"
HEX_OUT = PROJECT / "Debug" / f"{PROJECT.name}.hex"


@dataclass
class ToolResult:
    code: int
    command: str


class App(tk.Tk):
    def __init__(self) -> None:
        super().__init__()
        self.title("天猛星 MSPM0G3507 · 编译 / 转换 / 烧录")
        self.geometry("900x610")
        self.minsize(760, 500)
        self.events: queue.Queue[tuple[str, str]] = queue.Queue()
        self.running = False
        self._vars()
        self._build_ui()
        self.after(100, self._drain)

    def _vars(self) -> None:
        self.project_var = tk.StringVar(value=str(PROJECT))
        self.config_var = tk.StringVar(value="Debug")
        self.out_var = tk.StringVar(value=str(OUT))
        self.hex_var = tk.StringVar(value=str(HEX_OUT))
        self.ccxml_var = tk.StringVar(value=str(CCXML))
        self.status_var = tk.StringVar(value="就绪")

    def _build_ui(self) -> None:
        root = ttk.Frame(self, padding=12)
        root.pack(fill="both", expand=True)
        form = ttk.LabelFrame(root, text="工程与工具", padding=8)
        form.pack(fill="x")
        self._row(form, 0, "工程目录", self.project_var, directory=True)
        self._row(form, 1, "配置", self.config_var)
        self._row(form, 2, "OUT 文件", self.out_var, file=True)
        self._row(form, 3, "HEX 文件", self.hex_var, save=True)
        self._row(form, 4, "CCXML", self.ccxml_var, file=True)

        buttons = ttk.Frame(root, padding=(0, 10))
        buttons.pack(fill="x")
        self.build_btn = ttk.Button(buttons, text="1. 编译", command=lambda: self._start("build"))
        self.convert_btn = ttk.Button(buttons, text="2. 转换 HEX", command=lambda: self._start("convert"))
        self.flash_btn = ttk.Button(buttons, text="3. 烧录并复位", command=lambda: self._start("flash"))
        self.all_btn = ttk.Button(buttons, text="一键：编译 → 转换 → 烧录", command=lambda: self._start("all"))
        for b in (self.build_btn, self.convert_btn, self.flash_btn, self.all_btn):
            b.pack(side="left", padx=(0, 8))
        ttk.Label(buttons, textvariable=self.status_var).pack(side="right")

        note = ttk.Label(root, text="烧录使用 XDS110 + DSLite；不会自动执行 Mass Erase / Unlock。请确认开发板已连接。", foreground="#805500")
        note.pack(anchor="w", pady=(0, 6))
        box = ttk.LabelFrame(root, text="输出", padding=6)
        box.pack(fill="both", expand=True)
        self.log = tk.Text(box, wrap="none", state="disabled", font=("Consolas", 10))
        self.log.pack(side="left", fill="both", expand=True)
        scroll = ttk.Scrollbar(box, orient="vertical", command=self.log.yview)
        scroll.pack(side="right", fill="y")
        self.log.configure(yscrollcommand=scroll.set)

    def _row(self, parent: ttk.Widget, row: int, label: str, var: tk.StringVar, directory=False, file=False, save=False) -> None:
        ttk.Label(parent, text=label, width=12).grid(row=row, column=0, sticky="w", pady=3)
        ttk.Entry(parent, textvariable=var).grid(row=row, column=1, sticky="ew", pady=3)
        parent.columnconfigure(1, weight=1)
        if directory:
            cmd = lambda: self._pick_dir(var)
        elif save:
            cmd = lambda: self._pick_file(var, save=True)
        else:
            cmd = lambda: self._pick_file(var)
        ttk.Button(parent, text="浏览…", command=cmd).grid(row=row, column=2, padx=(6, 0))

    def _pick_dir(self, var: tk.StringVar) -> None:
        p = filedialog.askdirectory(initialdir=var.get())
        if p:
            var.set(p)

    def _pick_file(self, var: tk.StringVar, save=False) -> None:
        fn = filedialog.asksaveasfilename(initialdir=str(Path(var.get()).parent), initialfile=Path(var.get()).name) if save else filedialog.askopenfilename(initialdir=str(Path(var.get()).parent))
        if fn:
            var.set(fn)

    def _append(self, text: str) -> None:
        self.log.configure(state="normal")
        self.log.insert("end", text)
        self.log.see("end")
        self.log.configure(state="disabled")

    def _start(self, action: str) -> None:
        if self.running:
            return
        self.running = True
        self._set_buttons(False)
        threading.Thread(target=self._worker, args=(action,), daemon=True).start()

    def _set_buttons(self, enabled: bool) -> None:
        state = "normal" if enabled else "disabled"
        for b in (self.build_btn, self.convert_btn, self.flash_btn, self.all_btn):
            b.configure(state=state)

    def _run(self, args: list[str], cwd: Path) -> ToolResult:
        shown = " ".join(shlex.quote(str(a)) for a in args)
        self.events.put(("log", f"\n> {shown}\n"))
        try:
            p = subprocess.Popen(args, cwd=str(cwd), stdout=subprocess.PIPE, stderr=subprocess.STDOUT, text=True, encoding="utf-8", errors="replace")
        except OSError as e:
            self.events.put(("log", f"无法启动工具：{e}\n"))
            return ToolResult(127, shown)
        assert p.stdout is not None
        for line in p.stdout:
            self.events.put(("log", line))
        return ToolResult(p.wait(), shown)

    def _build(self) -> ToolResult:
        project = Path(self.project_var.get()).resolve()
        # Prefer CCS Theia's managed headless builder. If this checkout has a
        # generated makefile, gmake is a useful fallback for older CCS layouts.
        makefile = project / self.config_var.get() / "makefile"
        if makefile.exists():
            return self._run([str(GMAKE), "-C", str(makefile.parent), "all"], project)
        return self._run([str(CCS_SERVER), "-workspace", str(project.parent), "-application", "projectBuild", "-ccs.projects", project.name, "-ccs.configuration", self.config_var.get(), "-ccs.buildType", "incremental", "-ccs.listProblems"], project)

    def _convert(self) -> ToolResult:
        out = Path(self.out_var.get()).resolve()
        target = Path(self.hex_var.get()).resolve()
        target.parent.mkdir(parents=True, exist_ok=True)
        return self._run([str(HEX), "--intel", "-o", str(target), str(out)], out.parent)

    def _flash(self) -> ToolResult:
        ccxml = Path(self.ccxml_var.get()).resolve()
        out = Path(self.out_var.get()).resolve()
        return self._run([str(DSLite), "-c", str(ccxml), "-e", "-r", "2", "-u", str(out)], out.parent)

    def _worker(self, action: str) -> None:
        steps = {"build": [self._build], "convert": [self._convert], "flash": [self._flash], "all": [self._build, self._convert, self._flash]}[action]
        ok = True
        for fn in steps:
            result = fn()
            if result.code != 0:
                self.events.put(("log", f"失败，退出码 {result.code}\n"))
                ok = False
                break
        self.events.put(("done", "成功" if ok else "失败"))

    def _drain(self) -> None:
        try:
            while True:
                kind, value = self.events.get_nowait()
                if kind == "log":
                    self._append(value)
                else:
                    self.running = False
                    self.status_var.set(value)
                    self._set_buttons(True)
                    if value == "成功":
                        messagebox.showinfo("完成", "操作完成。请结合输出确认工具返回结果；实体板行为仍需实际验证。")
        except queue.Empty:
            pass
        self.after(100, self._drain)


if __name__ == "__main__":
    App().mainloop()
