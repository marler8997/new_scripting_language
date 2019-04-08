(
{ stdenv, fetchgit, ldc } :

stdenv.mkDerivation rec {
  pname = "run_new_scriptlang";
  version = "0.0.0";
  name = "${pname}-${version}";

  thisSrc = ./.;
  marSrc = fetchgit {
    url = https://github.com/dragon-lang/mar;
    sha256 = "128wvfhz7wff32wzdja2xbpnynaz1fv23a32kj32n637yj8kw012";
  };
  srcs = [thisSrc marSrc];
#    (fetchFromGithub {
#      owner = "dragon-lang";
#      repo = "mar";
#      name = "mar";
#    })
  
  #buildInputs = [ pkgconfig perl glib gpm slang zip unzip file gettext
  #    xlibs.libX11 xlibs.libICE e2fsprogs ];
  buildInputs = [ ldc ];

  buildCommand = ''
    mkdir -p $out/bin
    ${ldc}/bin/ldmd2 -of=$out/bin/script -I=$marSrc/src -I=$thisSrc -i $thisSrc/main.d
  '';

  meta = {
    description = "A new scripting language interpreter";
    maintainers = [ stdenv.lib.maintainers.JonathanMarler ];
  };
}
# call the function
) { inherit (import <nixpkgs> {}) stdenv fetchgit ldc; }
