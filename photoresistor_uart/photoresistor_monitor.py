"""Live monitor and software threshold controller for the photoresistor demo."""
import argparse
import msvcrt
import re
import threading
import time

import serial


FRAME = re.compile(r"AO=(\d+)\s+V=([0-9.]+)\s+V\s+DO=(\d+)\s+SW=(\d+)\s+THR=(\d+)")


def reader(port: serial.Serial, stop: threading.Event, state: dict) -> None:
    while not stop.is_set():
        line = port.readline().decode("ascii", errors="replace").strip()
        if not line:
            continue
        match = FRAME.search(line)
        if match:
            state["frame"] = match.groups()
        elif line.startswith("OK ") or line.startswith("THR="):
            state["message"] = line


def main() -> None:
    parser = argparse.ArgumentParser(description="光敏电阻串口监视器")
    parser.add_argument("-p", "--port", default="COM8", help="串口号，例如 COM8")
    parser.add_argument("-b", "--baud", type=int, default=115200)
    args = parser.parse_args()

    try:
        port_context = serial.Serial(args.port, args.baud, timeout=1)
    except serial.SerialException as exc:
        if getattr(exc, "winerror", None) == 5 or "PermissionError" in str(exc):
            raise SystemExit(
                f"无法打开 {args.port}：串口已被其他程序占用。"
                "请关闭 VOFA+、串口助手、CCS 串口终端或其他 Python 串口程序后重试。"
            ) from exc
        raise SystemExit(f"无法打开 {args.port}：{exc}") from exc

    with port_context as port:
        stop = threading.Event()
        state = {"frame": None, "message": "等待单片机数据"}
        thread = threading.Thread(target=reader, args=(port, stop, state), daemon=True)
        thread.start()
        threshold = 2048
        try:
            while True:
                if state["frame"] is not None:
                    ao, volts, hw_do, sw, reported_threshold = state["frame"]
                    threshold = int(reported_threshold)
                    text = (f"AO={ao:>4}  {volts} V  硬件DO={hw_do}  "
                            f"软件={sw}  阈值={threshold:>4}   [+/-]±50 [/]±10 [g]查询 [q]退出")
                else:
                    text = f"{state['message']}   [+/-]±50 [/]±10 [g]查询 [q]退出"
                print("\r" + text[:150].ljust(150), end="", flush=True)
                if msvcrt.kbhit():
                    key = msvcrt.getwch().lower()
                    if key == "q":
                        break
                    if key in ("+", "="):
                        threshold = min(4095, threshold + 50)
                        port.write(f"THR={threshold}\r\n".encode("ascii"))
                    elif key in ("-", "_"):
                        threshold = max(0, threshold - 50)
                        port.write(f"THR={threshold}\r\n".encode("ascii"))
                    elif key == "]":
                        threshold = min(4095, threshold + 10)
                        port.write(f"THR={threshold}\r\n".encode("ascii"))
                    elif key == "[":
                        threshold = max(0, threshold - 10)
                        port.write(f"THR={threshold}\r\n".encode("ascii"))
                    elif key == "g":
                        port.write(b"GET\r\n")
                time.sleep(0.05)
        finally:
            stop.set()
            thread.join(timeout=1)
            print()


if __name__ == "__main__":
    main()
