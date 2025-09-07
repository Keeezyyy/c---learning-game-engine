#!/bin/zsh

set -euo pipefail

SCRIPT_DIR=$(cd "$(dirname "$0")" && pwd)
cd "$SCRIPT_DIR"

g++ -std=c++17 -I./dependencies/include \
  main.cpp includes/utils.cpp dependencies/include/glad/glad.c stb_impl.cpp \
  -L./dependencies/library -lglfw.3.4 \
  -framework Cocoa -framework IOKit -framework CoreVideo \
  -Wl,-rpath,@executable_path/dependencies/library \
  -o myprogram

echo "Build successful: $SCRIPT_DIR/myprogram"

