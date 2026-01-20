#!/usr/bin/env bash
set -euo pipefail

run_bench() {
  echo "= $1"
  echo "($2)"
  stdbuf -oL $2
  echo
}

run_level() {
  echo "[$1] Building..."
  cmake -B build/ -DCMAKE_BUILD_TYPE="$2" > /dev/null
  cmake --build build/ > /dev/null
  echo

  run_bench \
    "Standard full picture, 50 iterations" \
    "build/leben_simd --bench 10 100 1920 1080 -.5 0 3 50"
}

run_suite() {
  run_level O0 Debug
  run_level O2 RelWithDebInfo
  run_level O3 Release
}

run_suite | tee benchmark.log
echo "Output log to [benchmark.log]"
