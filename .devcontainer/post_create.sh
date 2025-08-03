#!/usr/bin/env bash

set -ex

cat /etc/os-release
uname -r

gcc --version
g++ --version
gdb --version
objcopy --version
addr2line --version
gprof --version
/sbin/ldconfig -p | grep libstdc++

clang --version
clang++ --version
lldb --version
clangd --version

cmake --version
ninja --version
valgrind --version
perf --version
/sbin/ldconfig -p | grep liburing
/sbin/ldconfig -p | grep libfmt

rustc --version
cargo --version
