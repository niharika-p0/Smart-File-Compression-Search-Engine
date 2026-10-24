
from pathlib import Path
import os
import json
import subprocess

BASE_DIR = Path(__file__).resolve().parents[3]
CPP_EXE = BASE_DIR / ("WebBridge.exe" if os.name == "nt" else "WebBridge-linux")
RECOMMEND_EXE = BASE_DIR / ("WebBridge.recommend.exe" if os.name == "nt" else "WebBridge-linux")
DEFAULT_DOCUMENT_DIRECTORY = BASE_DIR / "data" / "documents"


def run_cpp_command(
    command,
    query="",
    document_directory=None,
    timeout=60,
    input_file=None,
    output_file=None,
):
    if not CPP_EXE.exists():
        return {
            "success": False,
            "error": f"C++ executable not found: {CPP_EXE}",
        }

    document_dir = Path(
        document_directory or DEFAULT_DOCUMENT_DIRECTORY
    ).resolve()

    if command in ("search", "autocomplete"):
        args = [
            str(CPP_EXE),
            command,
            query,
            str(document_dir),
        ]

    elif command == "status":
        args = [
            str(CPP_EXE),
            "status",
            str(document_dir),
        ]

    elif command in ("compress", "restore"):
        if not input_file or not output_file:
            return {
                "success": False,
                "error": "input_file and output_file are required.",
            }

        input_path = Path(input_file).resolve()
        output_path = Path(output_file).resolve()

        if not input_path.is_file():
            return {
                "success": False,
                "error": f"Input file not found: {input_path}",
            }

        output_path.parent.mkdir(parents=True, exist_ok=True)

        args = [
            str(CPP_EXE),
            command,
            str(input_path),
            str(output_path),
        ]

    else:
        return {
            "success": False,
            "error": f"Unknown C++ command: {command}",
        }

    try:
        print(f"[CPP DEBUG] Command: {args}")
        print(f"[CPP DEBUG] Working directory: {BASE_DIR}")
        print(f"[CPP DEBUG] Input file: {input_file}")
        print(f"[CPP DEBUG] Output file: {output_file}")

        process = subprocess.run(
            args,
            capture_output=True,
            text=True,
            timeout=timeout,
            cwd=str(BASE_DIR),
            check=False,
        )

        print(f"[CPP DEBUG] Return code: {process.returncode}")
        print(f"[CPP DEBUG] Stdout: {process.stdout[:2000]}")
        print(f"[CPP DEBUG] Stderr: {process.stderr[:2000]}")

        stdout = process.stdout.strip()
        stderr = process.stderr.strip()

        if process.returncode != 0:
            return {
                "success": False,
                "error": stderr or stdout or (
                    f"C++ process exited with code {process.returncode}"
                ),
            }

        if not stdout:
            return {
                "success": False,
                "error": stderr or "C++ engine returned no output.",
            }

        try:
            result = json.loads(stdout)
        except json.JSONDecodeError:
            return {
                "success": False,
                "error": "C++ engine returned invalid JSON.",
                "output": stdout,
            }

        return result

    except subprocess.TimeoutExpired as exc:
        print(f"[CPP DEBUG] Timeout after {timeout} seconds.")
        print(f"[CPP DEBUG] Partial stdout: {exc.stdout}")
        print(f"[CPP DEBUG] Partial stderr: {exc.stderr}")

        return {
            "success": False,
            "error": f"C++ command timed out after {timeout} seconds.",
        }

    except OSError as exc:
        print(f"[CPP DEBUG] OS error: {exc}")

        return {
            "success": False,
            "error": str(exc),
        }


def search_documents(query, document_directory=None):
    return run_cpp_command(
        "search",
        query=query,
        document_directory=document_directory,
    )


def autocomplete(query, document_directory=None):
    return run_cpp_command(
        "autocomplete",
        query=query,
        document_directory=document_directory,
    )


def get_engine_status(document_directory=None):
    return run_cpp_command(
        "status",
        document_directory=document_directory,
    )


def engine_status(document_directory=None):
    return get_engine_status(document_directory)


def compress_file(input_file, output_file):
    return run_cpp_command(
        "compress",
        input_file=input_file,
        output_file=output_file,
        timeout=120,
    )


def restore_file(input_file, output_file):
    return run_cpp_command(
        "restore",
        input_file=input_file,
        output_file=output_file,
        timeout=120,
    )


def recommend_documents(
    document_id: int,
    document_directory=None,
    limit: int = 5,
):
    if not RECOMMEND_EXE.exists():
        return {
            "success": False,
            "error": f"Recommendation executable not found: {RECOMMEND_EXE}",
        }

    document_dir = Path(
        document_directory or DEFAULT_DOCUMENT_DIRECTORY
    ).resolve()

    limit = max(1, min(int(limit), 20))

    args = [
        str(RECOMMEND_EXE),
        "recommend",
        str(int(document_id)),
        str(document_dir),
        str(limit),
    ]

    try:
        print(f"[CPP DEBUG] Recommendation command: {args}")
        print(f"[CPP DEBUG] Working directory: {BASE_DIR}")

        process = subprocess.run(
            args,
            capture_output=True,
            text=True,
            timeout=60,
            cwd=str(BASE_DIR),
            check=False,
        )

        print(
            f"[CPP DEBUG] Recommendation return code: "
            f"{process.returncode}"
        )
        print(
            f"[CPP DEBUG] Recommendation stdout: "
            f"{process.stdout[:2000]}"
        )
        print(
            f"[CPP DEBUG] Recommendation stderr: "
            f"{process.stderr[:2000]}"
        )

        if process.returncode != 0:
            return {
                "success": False,
                "error": (
                    process.stderr.strip()
                    or process.stdout.strip()
                    or "C++ recommendation command failed."
                ),
            }

        if not process.stdout.strip():
            return {
                "success": False,
                "error": "C++ recommendation command returned no output.",
            }

        result = json.loads(process.stdout)

        if not isinstance(result, dict):
            return {
                "success": False,
                "error": "Unexpected recommendation response.",
            }

        return result

    except subprocess.TimeoutExpired:
        return {
            "success": False,
            "error": "Recommendation command timed out after 60 seconds.",
        }

    except (OSError, json.JSONDecodeError, ValueError) as exc:
        return {
            "success": False,
            "error": str(exc),
        }
