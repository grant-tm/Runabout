#!/usr/bin/env bash
set -euo pipefail

ARCH="${1:-arm64}"  # arm64 or x86_64

ROOT="$(cd "$(dirname "$0")/../.." && pwd)"
OUTDIR="$ROOT/build/builds/macos_${ARCH}"
mkdir -p "$OUTDIR"

INC_DIRS=()
while IFS= read -r d; do INC_DIRS+=("$d"); done < <(
  find "$ROOT/source" -type d -print | sort -u
)

SOURCES=()
while IFS= read -r f; do SOURCES+=("$f"); done < <(
  find "$ROOT/source" -type f -name "*.c" -print | sort -u
)

INCLUDE_FLAGS=()
for d in "${INC_DIRS[@]}"; do INCLUDE_FLAGS+=( -I"$d" ); done

CFLAGS=( -std=c11 -O2 -g -Wall -Wextra -Wpedantic -arch "$ARCH" )

echo "Building macOS ${ARCH} (Apple Clang)..."
echo "Sources: ${#SOURCES[@]}  Include dirs: ${#INC_DIRS[@]}"

clang "${CFLAGS[@]}" "${INCLUDE_FLAGS[@]}" "${SOURCES[@]}" -o "$OUTDIR/app"
