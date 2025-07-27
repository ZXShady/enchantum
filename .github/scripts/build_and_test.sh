#!/usr/bin/env bash

set -e

if [[ $# -lt 1 ]]; then
  echo "Usage: $0 <c++ standard> [flags...]"
  exit 1
fi

STD="$1"
shift
FLAGS="$@"

for CONFIG in Debug Release; do
  DIR="build-cpp${STD}-${CONFIG}"
  cmake -B "$DIR" -G Ninja $FLAGS -DCMAKE_BUILD_TYPE=$CONFIG -DENCHANTUM_BUILD_TESTS=ON -DCMAKE_CXX_STANDARD=$STD
  cmake --build "$DIR"
  ctest --test-dir "$DIR" --output-on-failure
done
