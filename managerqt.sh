#!/usr/bin/env bash
# Launches the Qt Lab Manager (Manager.Qt), building it first if necessary.
# Usage: ./managerqt.sh [--theme dark|light] [--screenshot file.png]
set -euo pipefail

ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
BIN="$ROOT/Manager/Manager.Qt/bin/ManagerQt"

if [ ! -x "$BIN" ]; then
    echo "ManagerQt not built yet — building (release)…" >&2
    "$ROOT/Manager/Manager.Qt/build.sh" release
fi

exec "$BIN" "$@"
