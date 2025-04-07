{ pkgs ? (import <nixpkgs> {
    config.allowUnfree = true;
    config.segger-jlink.acceptLicense = true;
}), ... }:
pkgs.mkShell
{
	buildInputs = with pkgs; [
		cmake
		gcc
		clang
		emscripten
		ninja
		renderdoc
		valgrind
	];
	nativeBuildInputs = with pkgs; [ pkg-config ];
	propagatedBuildInputs = with pkgs; [
        gtest
        gtest.dev
        grpc
        protobuf
	];
	LD_LIBRARY_PATH="/run/opengl-driver/lib:/run/opengl-driver-32/lib";
}
