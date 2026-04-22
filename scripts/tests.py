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

    file_list = []
    src_dir = repo_root / "src"
    exclude_dirs = []
    for root, dirs, files in os.walk(src_dir):
        print(f"扫描目录: {root}, 包含子目录: {dirs}, 包含文件: {files}")
        if root in exclude_dirs:
            print(f"跳过目录: {root} (已标记为测试目录)")
            continue

        # iterate over a copy so we can modify `dirs` to prevent descending
        for dir_name in list(dirs):
            print(f"发现子目录: {root}/{dir_name}")
            c_path = os.path.join(root, dir_name, 'cpprun_test.c')
            cpp_path = os.path.join(root, dir_name, 'cpprun_test.cpp')
            found = None
            if os.path.exists(c_path):
                found = c_path
            elif os.path.exists(cpp_path):
                found = cpp_path
            if found:
                # prevent os.walk from descending into this directory
                try:
                    dirs.remove(dir_name)
                except ValueError:
                    pass
                exclude_dirs.append(os.path.join(root, dir_name))
                print(f"发现测试文件: {found}")
                file_list.append((os.path.join(root, dir_name), 
                                  repo_root / "build" / f"{dir_name}", 
                                  repo_root / "logs" / f"{dir_name}"))
                
        for file_name in files:
            if file_name.endswith((".hpp", ".cpp", ".c")):
                file_path = os.path.join(root, file_name)
                print(f"发现源文件: {file_path}")
                file_list.append((file_path, 
                                    repo_root / "build" / f"{Path(file_name).stem}", 
                                    repo_root / "logs" / f"{Path(file_name).stem}"))

    print(f"文件列表: {file_list}")
    return file_list


def run_command(cmd: List[str], env: Optional[dict] = None, log_file: Optional[str] = None) -> int:
    """运行命令并返回退出码；若提供 `log_file`，将 stdout/stderr 写入该文件。"""
    print(f"命令: {' '.join(str(c) for c in cmd)}")
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
            print(f"正在测试: {src} -> {bld} (日志: {log})")
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
