{ pkgs
, makeTest
}:
let
  pythonShebang = pkgs.writeScript "python-shebang" ''
    #!/usr/bin/python
    print("OK")
  '';

  bashShebang = pkgs.writeScript "bash-shebang" ''
    #!/usr/bin/bash
    echo "OK"
  '';

  shShebang = pkgs.writeScript "sh-shebang" ''
    #!/usr/bin/sh
    echo "OK"
  '';
in
makeTest
{
  name = "pathfs";
  nodes.machine = ({ ... }: {
    imports = [ ./module.nix ];
    service.pathfs.enable = true;
  });
  testScript = ''
    machine.start()
    machine.wait_until_succeeds("mountpoint -q /usr/bin/")
    machine.succeed(
        "PATH=${pkgs.coreutils}/bin /usr/bin/cp --version",
        # we have stat
        "PATH=${pkgs.coreutils}/bin test -e /usr/bin/cp",
        "! /usr/bin/hello",
        # "PATH=${pkgs.hello}/bin /usr/bin/hello",
    )
    # out = machine.succeed("PATH=${pkgs.python3}/bin ${pythonShebang}")
    # print(out)
    # assert out == "OK\n"

    out = machine.succeed("PATH=${pkgs.bash}/bin ${bashShebang}")
    assert out == "OK\n"

    out = machine.succeed("PATH=${pkgs.bash}/bin ${shShebang}")
    assert out == "OK\n"
  '';

  meta = {
    timeout = 10;
  };
}
{
  inherit pkgs;
  inherit (pkgs) system;
}
