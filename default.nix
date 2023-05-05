{ stdenv
, meson
, ninja
, cmake
, pkg-config
, fuse3
}:
stdenv.mkDerivation rec {
  pname = "pathfs";
  version = "0.0.0";
  src = ./.;

  nativeBuildInputs = [
    meson
    ninja
    cmake
    pkg-config
  ];

  buildInputs = [
    fuse3
  ];

  postInstall = ''
    ln -s ${pname} $out/bin/mount.fuse.${pname}
    ln -s ${pname} $out/bin/mount.${pname}
  '';
}
