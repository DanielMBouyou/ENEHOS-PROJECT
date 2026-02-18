#!/usr/bin/env python3
from __future__ import annotations

import argparse
import csv
import datetime as dt
import re
import sys
import time
from pathlib import Path

import serial


CSV_LINE = re.compile(
    r"^\s*(\d+),(-?\d+(?:\.\d+)?),(-?\d+(?:\.\d+)?),(-?\d+(?:\.\d+)?),(-?\d+(?:\.\d+)?)\s*$"
)


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(description="Capture IMU CSV lines from XIAO serial port.")
    parser.add_argument("--port", default="COM8", help="Serial port (default: COM8)")
    parser.add_argument("--baud", type=int, default=115200, help="Serial baudrate (default: 115200)")
    parser.add_argument("--duration", type=float, default=20.0, help="Capture duration in seconds")
    parser.add_argument(
        "--out",
        default="tools/scripts/logs",
        help="Output directory for .csv and .log (default: tools/scripts/logs)",
    )
    return parser.parse_args()


def main() -> int:
    args = parse_args()
    out_dir = Path(args.out)
    out_dir.mkdir(parents=True, exist_ok=True)

    stamp = dt.datetime.now().strftime("%Y%m%d_%H%M%S")
    csv_path = out_dir / f"imu_{stamp}.csv"
    log_path = out_dir / f"imu_{stamp}.log"

    print(f"Opening {args.port} @ {args.baud}")
    print(f"Capture duration: {args.duration:.1f}s")
    print(f"CSV output: {csv_path}")
    print(f"Raw output: {log_path}")

    start = time.time()
    rows = 0

    with serial.Serial(args.port, baudrate=args.baud, timeout=1) as ser, csv_path.open(
        "w", newline="", encoding="utf-8"
    ) as csv_file, log_path.open("w", encoding="utf-8") as log_file:
        writer = csv.writer(csv_file)
        writer.writerow(
            [
                "pc_iso_time",
                "device_millis",
                "roll_deg",
                "pitch_deg",
                "yaw_deg",
                "heading_deg",
            ]
        )

        while time.time() - start < args.duration:
            raw = ser.readline()
            if not raw:
                continue
            line = raw.decode("utf-8", errors="replace").strip()
            now_iso = dt.datetime.now().isoformat(timespec="milliseconds")
            log_file.write(f"{now_iso} {line}\n")

            match = CSV_LINE.match(line)
            if not match:
                continue

            device_millis, roll, pitch, yaw, heading = match.groups()
            writer.writerow([now_iso, device_millis, roll, pitch, yaw, heading])
            rows += 1
            if rows % 20 == 0:
                print(f"... {rows} samples")

    print(f"Done. Captured {rows} samples.")
    if rows == 0:
        print("No CSV measurements detected. Check wiring, firmware, or serial speed.")
        return 2

    return 0


if __name__ == "__main__":
    sys.exit(main())
