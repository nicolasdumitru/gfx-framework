build:
    cmake -S . -B build && cmake --build build --parallel 8

run: build
    ./build/bin/Debug/GFXFramework