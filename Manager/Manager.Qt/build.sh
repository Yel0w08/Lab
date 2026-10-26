#!/usr/bin/env bash
# Builds Manager.Qt (qmake + make). Usage: ./build.sh [release|debug]
set -euo pipefail

cd "$(dirname "$0")"

MODE="${1:-release}"
OUT_DIR="bin"

if ! command -v qmake6 >/dev/null 2>&1; then
    echo "qmake6 not found. Install Qt 6 (qt6-base-dev)." >&2
    exit 1
fi

BUILD_DIR="build-$MODE"
mkdir -p "$BUILD_DIR"
cd "$BUILD_DIR"

if [ "$MODE" = "debug" ]; then
    qmake6 ../Manager.Qt.pro CONFIG+=debug CONFIG-=release
else
    qmake6 ../Manager.Qt.pro CONFIG+=release CONFIG-=debug
fi

make -j"$(nproc)"

echo
echo "Built: $(cd .. && pwd)/$OUT_DIR/ManagerQt"
