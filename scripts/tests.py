import os
import sys
import shutil
import subprocess
from pathlib import Path
from typing import List, Tuple, Optional


def get_target_list(repo_root: Path) -> List[Tuple[str, str, str]]:
    """收集要测试的源文件及对应的 build/log 路径。

    返回列表，元素为 (src_path, build_path, log_path)。
    """
    dir_list = ["src"]
    postfix_list = [".hpp", ".cpp", ".c"]

    file_list = [
        p for d in dir_list
        for p in (repo_root / d).glob("**/*")
        if p.is_file() and p.suffix in postfix_list
    ]
    print(f"文件列表: {file_list}")

    target_list = [
        (
            str(p.resolve()),
            str((repo_root / "build" / p.stem).resolve()),
            str((repo_root / "logs" / p.stem).resolve()),
        )
        for p in file_list
    ]
    print(f"目标列表: {target_list}")

    return target_list


def run_command(cmd: List[str], env: Optional[dict] = None, log_file: Optional[str] = None) -> int:
    """运行命令并返回退出码；若提供 `log_file`，将 stdout/stderr 写入该文件。"""
    print("运行: " + " ".join(cmd))
    if log_file:
        with open(log_file, "w", encoding="utf-8") as fh:
            proc = subprocess.run(cmd, env=env, stdout=fh, stderr=subprocess.STDOUT)
    else:
        proc = subprocess.run(cmd, env=env)
    return proc.returncode


def main() -> int:
    orig_cwd = Path.cwd()
    repo_root = Path(__file__).resolve().parent.parent

    try:
        os.environ["PYTHONUTF8"] = "1"

        print(f"原始目录: {orig_cwd}")

        os.chdir(repo_root)
        print(f"当前目录: {Path.cwd()}")

        # 删除旧的 build 目录
        build_dir = repo_root / "build"
        if build_dir.exists():
            print("删除 build 目录...")
            try:
                shutil.rmtree(build_dir)
            except Exception as e:
                print(f"删除 build 失败: {e}")
        else:
            print("build 目录不存在，跳过删除")

        # 删除旧的 logs 目录
        logs_dir = repo_root / "logs"
        if logs_dir.exists():
            print("删除 logs 目录...")
            try:
                shutil.rmtree(logs_dir)
            except Exception as e:
                print(f"删除 logs 失败: {e}")
        else:
            print("logs 目录不存在，跳过删除")

        logs_dir.mkdir(parents=True, exist_ok=True)
        test_log = logs_dir / "tests.log"

        cpprun_script = repo_root / "scripts" / "cpprun.py"
        python_exe = sys.executable or "python"

        env = os.environ.copy()
        env["PYTHONUTF8"] = "1"

        # 获取目标列表并执行测试
        target_list = get_target_list(repo_root)
        for src, bld, log in target_list:
            cmd = [python_exe, str(cpprun_script), "-s", src, "-b", bld, "--config", "Debug"]
            log_path = Path(log).with_suffix(".log")
            log_path.parent.mkdir(parents=True, exist_ok=True)
            print(f"将执行输出写入: {log_path}")
            rc = run_command(cmd, env=env, log_file=str(log_path))
            with open(test_log, "a", encoding="utf-8") as fh:
                if rc == 0:
                    fh.write(f"✅ {src}\n")
                    print(f"测试成功: {src}")
                else:
                    fh.write(f"❌ {src} LINE#{rc}\n")
                    print(f"测试失败: {src} (退出码: {rc})")

        return rc
    finally:
        os.chdir(orig_cwd)
        print(f"恢复目录: {Path.cwd()}")


if __name__ == "__main__":
    code = main()
    sys.exit(code)
