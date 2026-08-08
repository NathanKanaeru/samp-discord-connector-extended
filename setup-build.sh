#!/usr/bin/env bash
#
# Setup dependencies and build discord-connector (32-bit plugin) on Debian/Ubuntu.
#
# Usage:
#   ./setup-build.sh            # install deps + configure + build
#   ./setup-build.sh --build    # skip dependency install, just configure + build
set -euo pipefail

BUILD_DIR="${BUILD_DIR:-build}"
SKIP_DEPS=0
FORCE_CONAN_OPENSSL=0

for arg in "$@"; do
    case "$arg" in
        --build) SKIP_DEPS=1 ;;
        --force-conan-openssl) FORCE_CONAN_OPENSSL=1 ;;
        -h|--help)
            echo "Usage: $0 [--build] [--force-conan-openssl]"
            exit 0
            ;;
    esac
done

GCC_MAJOR=$(gcc -dumpversion 2>/dev/null | cut -d. -f1)
USE_GCC14=0
if [ -n "$GCC_MAJOR" ] && [ "$GCC_MAJOR" -ge 15 ]; then
    echo "[*] gcc $GCC_MAJOR detected; Conan 1.x needs gcc <= 14, installing gcc-14/g++-14"
    USE_GCC14=1
fi

if [ "$SKIP_DEPS" -eq 0 ]; then
    echo "[*] Installing system dependencies (needs sudo)..."
    sudo apt-get update
    sudo apt-get install -y --no-install-recommends \
        build-essential \
        gcc-multilib \
        g++-multilib \
        git \
        ninja-build \
        pkg-config \
        python3 \
        python3-pip \
        ca-certificates

    # OpenSSL (libssl) + 32-bit static libs so CMake can link OpenSSL statically.
    if ! dpkg --print-foreign-architectures | grep -q i386; then
        sudo dpkg --add-architecture i386
        sudo apt-get update
    fi
    sudo apt-get install -y --no-install-recommends \
        libssl-dev \
        libssl-dev:i386

    if [ "$USE_GCC14" -eq 1 ]; then
        sudo apt-get install -y --no-install-recommends gcc-14 g++-14
    fi

    echo "[*] Installing CMake and Conan 1.x via pip..."
    python3 -m pip install --user --break-system-packages "conan<2" "cmake>=3.19,<4"
    export PATH="$HOME/.local/bin:$PATH"
fi

echo "[*] Checking toolchain..."
command -v cmake >/dev/null || { echo "cmake not found, re-run without --build"; exit 1; }
command -v conan >/dev/null || { echo "conan not found, re-run without --build"; exit 1; }

export CMAKE_POLICY_VERSION_MINIMUM=3.5

CONFIGURE_ARGS=(
    -S . -B "$BUILD_DIR" -G Ninja
    -DCMAKE_BUILD_TYPE=Release
    -DCMAKE_C_FLAGS=-m32
    -DCMAKE_CXX_FLAGS=-m32
    -DCMAKE_EXE_LINKER_FLAGS=-m32
    -DCMAKE_SHARED_LINKER_FLAGS=-m32
    -DGH_ACBUILD=TRUE
)

if [ "$FORCE_CONAN_OPENSSL" -eq 1 ]; then
    echo "[*] Forcing OpenSSL from Conan"
    CONAN_ARGS+=(-DFORCE_CONAN_OPENSSL=ON)
fi

if [ "$USE_GCC14" -eq 1 ]; then
    CONAN_ARGS+=(-DCMAKE_C_COMPILER=gcc-14 -DCMAKE_CXX_COMPILER=g++-14)
fi

echo "[*] Configuring CMake..."
cmake "${CONAN_ARGS[@]}"

echo "[*] Building..."
cmake --build "$BUILD_DIR" --config Release

echo ""
echo "[+] Done!"
echo "    Plugin:  $BUILD_DIR/artifact/plugins/discord-connector.so"
echo "    Include: $BUILD_DIR/artifact/pawno/include/discord-connector.inc"