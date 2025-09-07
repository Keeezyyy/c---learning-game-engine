#!/bin/zsh

set -euo pipefail

SCRIPT_DIR=$(cd "$(dirname "$0")" && pwd)
cd "$SCRIPT_DIR"

g++ -std=c++17 -I./dependencies/include \
  main.cpp includes/utils.cpp includes/shader.cpp includes/textures.cpp includes/block.cpp includes/world.cpp constants/texture_constants.cpp dependencies/include/glad/glad.c stb_impl.cpp \
  -L./dependencies/library -lglfw.3.4 \
  -framework Cocoa -framework IOKit -framework CoreVideo \
  -Wl,-rpath,@executable_path/dependencies/library \
  -o app

echo "Build successful: $SCRIPT_DIR/app"

