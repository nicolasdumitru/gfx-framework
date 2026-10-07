{
  description = "Development environment for GFX Framework";

  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/nixos-unstable";
  };
  outputs =
    {
      nixpkgs,
      ...
    }:
    let
      system = "x86_64-linux";
      pkgs = import nixpkgs {
        inherit system;
        config.allowUnfree = true;
      };
    in
    {
      devShell."${system}" = pkgs.mkShell.override { stdenv = pkgs.clangStdenv; } {
        packages = with pkgs; [
          # WARNING: Do NOT add gcc, clang, gligc, libcxx, etc.
          # They will break the environment, leading to compilation or LSP errors.
          clang-tools
          meson
          ninja
          cmake
          pkg-config

          libGL
          glew
          glfw
          assimp
          spdlog
          freetype

          (python3.withPackages (
            ps: with ps; [
              python-lsp-server
            ]
          ))
          pyrefly
          pyright
          black

          just

          nil
          nixd
          nixfmt
          nixfmt-tree
        ];

        CMAKE_EXPORT_COMPILE_COMMANDS = "ON";
      };
    };
}
