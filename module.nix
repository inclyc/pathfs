{ config
, lib
, pkgs
, ...
}:
with lib;
let
  cfg = config.service.pathfs;
in
{
  options = {
    service.pathfs.enable = mkEnableOption "Enable pathfs mounts";
    service.pathfs.env = mkOption {
      default = "PATH";
    };
  };

  config = mkIf cfg.enable (
    let
      mounts = {
        "/usr/bin" = {
          device = cfg.env;
          fsType = "fuse.pathfs";
          options = [ "defaults" ];
        };
        "/bin" = {
          device = "/usr/bin";
          fsType = "none";
          options = [ "bind" ];
        };
      };
    in
    {
      environment.systemPackages = [ (pkgs.callPackage ./. { }) ];
      fileSystems = if config.virtualisation ? qemu then lib.mkVMOverride mounts else mounts;

      system.activationScripts.usrbinenv = lib.mkForce "";
      system.activationScripts.binsh = lib.mkForce "";
    }
  );
}
