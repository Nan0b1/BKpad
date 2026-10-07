#!/usr/bin/env bash
set -euo pipefail

WSL_PROJECT="$HOME/mediapad-main"

if [[ "${1:-}" == "--shell" ]]; then
    WINDOWS_PROJECT="${2:?Pass the Windows project path}"
    if [[ "$WINDOWS_PROJECT" != /* ]]; then
        WINDOWS_PROJECT="$(wslpath -u "$WINDOWS_PROJECT")"
    fi

    mkdir -p "$WSL_PROJECT"
    cp -a "$WINDOWS_PROJECT/." "$WSL_PROJECT/"
    printf 'source "$HOME/.bashrc"\nexport MEDIAPAD_WINDOWS_PROJECT=%q\ncd "$HOME/mediapad-main"\nsBuild() { bash "$HOME/mediapad-main/sBuild.sh"; }\n' \
        "$WINDOWS_PROJECT" > "$WSL_PROJECT/.sbuildrc"

    printf '\nQMK WSL is ready. Type sBuild to compile; repeat with the up arrow.\n\n'
    exec bash --rcfile "$WSL_PROJECT/.sbuildrc" -i
fi

WINDOWS_PROJECT="${MEDIAPAD_WINDOWS_PROJECT:?Start the persistent shell with sBuild.cmd first}"
cp -a "$WINDOWS_PROJECT/." "$WSL_PROJECT/"
exec bash "$WSL_PROJECT/build.sh" --inside-wsl "$WINDOWS_PROJECT"