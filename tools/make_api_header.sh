#!/usr/bin/env bash
set -euo pipefail

SCRIPTPATH="${BASH_SOURCE[0]}"
SCRIPTDIR="${SCRIPTPATH%/*}"
DOCDIR="$SCRIPTDIR/../src/doc"
OH="$1"; shift

H=()
if (( $# )); then
    H=("$@")
else
    mapfile -t H
fi

((${#H[@]})) || exit 0

exec >"$OH"

printf '%s\n' '#ifndef CVEC_H' '#define CVEC_H' ''

cat "$DOCDIR/cvec.h.txt"
printf '\n\n'

printf '#include "%s"\n' "${H[@]}"

printf '\n%s\n' '#endif'
