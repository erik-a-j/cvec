#!/usr/bin/env bash
set -euo pipefail

SCRIPTPATH="${BASH_SOURCE[0]}"
SCRIPTDIR="${SCRIPTPATH%/*}"
INCDIR="$SCRIPTDIR/../src/include"

DESTDIR="$1"; shift
mkdir -p "$DESTDIR"

H=()
if (( $# )); then
    H=("$@")
else
    mapfile -t H
fi

((${#H[@]})) || exit 0

(cd -- "$INCDIR" && cp -p -- "cvec_api.h" "${H[@]}" "$DESTDIR/")