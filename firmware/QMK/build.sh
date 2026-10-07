#!/usr/bin/env bash
set -euo pipefail

if [[ "${1:-}" != "--inside-wsl" ]]; then
    if [[ -z "${1:-}" ]]; then
        printf 'Start the persistent shell with sBuild.cmd, then run sBuild inside it.\n' >&2
        exit 2
    fi

    WINDOWS_PROJECT="$1"
    if [[ "$WINDOWS_PROJECT" != /* ]]; then
        WINDOWS_PROJECT="$(wslpath -u "$WINDOWS_PROJECT")"
    fi
    WSL_PROJECT="$HOME/mediapad-main"

    mkdir -p "$WSL_PROJECT"
    cp -a "$WINDOWS_PROJECT/." "$WSL_PROJECT/"
    exec bash "$WSL_PROJECT/build.sh" --inside-wsl "$WINDOWS_PROJECT"
fi

PROJECT_ROOT="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd)"
WINDOWS_PROJECT="${2:?Pass the Windows project directory}"
SOURCE_DIR="$PROJECT_ROOT/FirmwareBK"
QMK_ROOT="${QMK_FIRMWARE:-$HOME/qmk_firmware}"
KEYBOARD_DIR="$QMK_ROOT/keyboards/firmwarebk"

if [[ ! -f "$QMK_ROOT/Makefile" ]]; then
    qmk setup -H "$QMK_ROOT" -y
fi

if ! command -v arm-none-eabi-gcc >/dev/null 2>&1; then
    printf 'Installing the ARM toolchain required by QMK...\n'
    sudo apt-get update
    sudo apt-get install -y gcc-arm-none-eabi binutils-arm-none-eabi libnewlib-arm-none-eabi
fi

if ! command -v arm-none-eabi-gcc >/dev/null 2>&1; then
    printf 'arm-none-eabi-gcc is still unavailable; install the QMK WSL toolchain and retry.\n' >&2
    exit 1
fi

if [[ ! -f "$SOURCE_DIR/keyboard.json" || ! -f "$SOURCE_DIR/keymaps/kmk_port/keymap.c" ]]; then
    printf 'Firmware sources are incomplete in %s\n' "$SOURCE_DIR" >&2
    exit 1
fi

mkdir -p "$KEYBOARD_DIR"
cp -a "$SOURCE_DIR/." "$KEYBOARD_DIR/"

make -j1 -C "$QMK_ROOT" firmwarebk:kmk_port

UF2_PATH="$QMK_ROOT/firmwarebk_kmk_port.uf2"
cp "$UF2_PATH" "$PROJECT_ROOT/"
cp "$UF2_PATH" "$WINDOWS_PROJECT/"
printf 'UF2 copied to %s/firmwarebk_kmk_port.uf2\n' "$WINDOWS_PROJECT"