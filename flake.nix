{
  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/nixpkgs-unstable";

    flake-parts.url = "github:hercules-ci/flake-parts";
  };

  outputs = { flake-parts, ... }@inputs: flake-parts.lib.mkFlake { inherit inputs; } {
    perSystem = { config, self', inputs', pkgs, system, ... }:
      with pkgs;
      let
        devInputs = [ clang-tools ];
        nativeBuildInputs = [
          meson
          ninja
          cmake
          pkg-config
        ];
        buildInputs = [
          fuse3
        ];
      in
      {
        devShells.default = mkShell {
          nativeBuildInputs = devInputs ++ nativeBuildInputs;
          inherit buildInputs;
        };
      };
    systems = [ "x86_64-linux" ];
  };
}
