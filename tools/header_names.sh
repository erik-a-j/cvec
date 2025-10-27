#!/usr/bin/env bash
set -u


USE_ALL=0
[[ "$1" == "-DUSE_ALL" ]] && USE_ALL=1

declare -A D=()
for d in "$@"; do D["$d"]=1; done


HDEF=(
    "cvec_api.h"
    "cvec_types.h"
    "cvec_base.h"
    "cvec_hooks.h"
)
HEXT=(
    "cvec_macro.h"
    "cvec_string_ext.h"
    "cvec_fmt.h"
    "cvec_dump.h"
)
printf '%s\n' "${HDEF[@]}"

[[ $USE_ALL -eq 1 ]] || [[ ${D[-DUSE_MACRO]+x} ]] && printf '%s\n' "${HEXT[0]}"

if [[ $USE_ALL -eq 1 ]] || [[ ${D[-DUSE_DUMP]+x} ]]; then
    printf '%s\n' "${HEXT[1]}"
    printf '%s\n' "${HEXT[2]}"
    printf '%s\n' "${HEXT[3]}"
else
    [[ ${D[-DUSE_STRING_EXT]+x} ]] && printf '%s\n' "${HEXT[1]}"
    [[ ${D[-DUSE_FMT]+x} ]]        && printf '%s\n' "${HEXT[2]}"
fi

:
