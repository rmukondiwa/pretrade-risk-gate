#!/usr/bin/env bash
set -euo pipefail

cd "$(dirname "$0")"
mkdir -p bin

g++ -std=c++20 -O3 -march=native -Wall -Wextra \
    -Iinclude -Ibench \
    bench/replay.cpp -o bin/replay

echo "built bin/replay"
