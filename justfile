build:
    nix develop --command bash -c 'cmake -S . -B build && cmake --build build --parallel 4'

run: build
    nix develop --command ./build/bin/Debug/GFXFramework
