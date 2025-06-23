{
  description = "A very basic flake";

  inputs = {
    nixpkgs.url = "github:nixos/nixpkgs?ref=nixos-unstable";
    flake-utils.url = "github:numtide/flake-utils";
  };

  outputs = { self, nixpkgs, flake-utils }:
    flake-utils.lib.eachDefaultSystem (system:
      let pkgs = import nixpkgs { inherit system; }; in
      {
        devShells.default = pkgs.mkShell {
          packages = with pkgs; [
            glfw
            sdl3
            libepoxy
            libopus
            glm
            clang-tools
            cmake
            gdb
            gf

            nil
            nixd
            nixfmt-classic
          ];
        };
      }
    );

}
