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
        pathfs = pkgs.callPackage ./default.nix { };
        inherit (pathfs) nativeBuildInputs buildInputs;
      in
      {
        devShells.default = mkShell {
          nativeBuildInputs = devInputs ++ nativeBuildInputs;
          inherit buildInputs;
        };
        packages = {
          inherit pathfs;
        };
      };
    systems = [ "x86_64-linux" ];
  };
}
