import json
import sys
import shutil
import subprocess
import os
from pathlib import Path
from typing import List


def load_targets(repo_root: Path) -> List[str]:
    cfg = repo_root / "scripts" / "cpprun.json"
    with cfg.open("r", encoding="utf-8") as fh:
        data = json.load(fh)
    return list(data)


def run_command(cmd, env=None, log_file: Path = None) -> int:
    print("命令:", " ".join(map(str, cmd)))
    if log_file:
        with log_file.open("w", encoding="utf-8") as fh:
            proc = subprocess.run(cmd, env=env, stdout=fh, stderr=subprocess.STDOUT)
    else:
        proc = subprocess.run(cmd, env=env)
    return proc.returncode


def main() -> int:
    repo_root = Path(__file__).resolve().parent.parent
    python_exe = sys.executable or "python"
    env = os.environ.copy()
    env["PYTHONUTF8"] = "1"

    # prepare logs
    logs_dir = repo_root / "logs"
    if logs_dir.exists():
        try:
            shutil.rmtree(logs_dir)
        except Exception:
            pass
    logs_dir.mkdir(parents=True, exist_ok=True)
    master_log = logs_dir / "tests.log"

    targets = load_targets(repo_root)
    last_rc = 0
    cpprun = repo_root / "scripts" / "cpprun.py"

    for item in targets:
        src = item
        # determine build/log name: use last path part; if file, use stem
        p = Path(item)
        if p.is_dir() or str(item).endswith("/"):
            name = p.name
        else:
            name = p.stem

        build_path = repo_root / "build" / name
        log_path = logs_dir / f"{name}.log"
        log_path.parent.mkdir(parents=True, exist_ok=True)

        cmd = [python_exe, str(cpprun), "-s", src, "-b", str(build_path)]
        print(f"运行: {src} -> {build_path} (日志: {log_path})")
        rc = run_command(cmd, env=env, log_file=log_path)
        with master_log.open("a", encoding="utf-8") as fh:
            if rc == 0:
                fh.write(f"✅ {src}\n")
            else:
                fh.write(f"❌ {src} LINE#{rc}\n")
        last_rc = rc or last_rc

    return last_rc


if __name__ == "__main__":
    rc = main()
    sys.exit(rc)
