#!/usr/bin/env python3

import sys
import os
import shutil
import shlex
import subprocess
from contextlib import contextmanager

CONAN_PROFILE = "workflow"


@contextmanager
def cd(path):
    current = os.getcwd()
    try:
        os.chdir(path)
        yield
    finally:
        os.chdir(current)


def call(*args, stdout=subprocess.PIPE):
    print(shlex.join(args))
    proc = subprocess.run(args, stdout=stdout, check=True)
    return proc.stdout.decode().strip() if stdout is not None else None


def conan(build_type):
    call("conan", "profile", "new", "--detect", "--force", CONAN_PROFILE)
    if sys.platform == "linux":
        call(
            "conan",
            "profile",
            "update",
            "settings.compiler.libcxx=libstdc++11",
            CONAN_PROFILE,
        )

    call(
        "conan", "profile", "update", f"settings.build_type={build_type}", CONAN_PROFILE
    )

    call(
        "conan",
        "install",
        "../..",
        "--build",
        "missing",
        "-pr:b",
        CONAN_PROFILE,
        "-pr:h",
        CONAN_PROFILE,
        stdout=None,
    )


if len(sys.argv) < 3:
    print("setup.py <compiler> <build_type>[,<build_type>...]", file=sys.stderr)
    sys.exit(1)

compilers = {
    "gcc": ["linux"],
    "clang": ["linux"],
    "msvc": ["nt"],
}


normalized = {"clang++": "clang", "g++": "gcc"}
names = {"clang": ["clang", "clang++"], "gcc": ["gcc", "g++"]}


def normalize_compiler(compiler: str):
    dirname = os.path.dirname(compiler)
    filename = os.path.basename(compiler)
    if sys.platform == "nt":
        filename = os.path.splitext()[0]
    chunks = filename.split("-", 1)
    if len(chunks) == 1:
        version = None
    else:
        version = chunks[1]
    filename = chunks[0].lower()
    try:
        filename = normalized[filename]
    except KeyError:
        pass

    try:
        compiler_names = names[filename]
    except:
        compiler_names = [filename]

    return filename, [
        os.path.join(dirname, name if version is None else f"{name}-{version}")
        for name in compiler_names
    ]


compiler_name, compilers_used = normalize_compiler(sys.argv[1])
BUILD_TYPE = sys.argv[2].split(",")

try:
    if sys.platform not in compilers[compiler_name]:
        print(
            f"Cannot use {compiler_name} on {'windows' if sys.platform == 'nt' else sys.platform}",
            file=sys.stderr,
        )
        sys.exit(1)
except KeyError:
    print(f"Unknown compiler: {compiler_name}", file=sys.stderr)
    sys.exit(1)

for var, value in zip(["CC", "CXX"], compilers_used):
    os.environ[var] = value

shutil.rmtree("build/conan", ignore_errors=True)
os.makedirs("build/conan", exist_ok=True)
with cd("build/conan"):
    print("cd build/conan")

    for build_type in BUILD_TYPE:
        build_type = build_type.strip()
        conan(build_type)

    print("cd ../..")

generator = "msbuild" if sys.platform == "nt" else "make"

for build_type in BUILD_TYPE:
    build_type = build_type.strip().lower()
    preset = f"{build_type}-{generator}"
    call("cmake", "--preset", preset, stdout=None)
    call("cmake", "--build", "--preset", build_type, "--parallel", stdout=None)
