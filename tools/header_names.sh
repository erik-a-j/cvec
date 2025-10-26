#!/usr/bin/env bash
set -u

declare -A D=()
for d in "$@"; do D["$d"]=1; done

#p="cvec"
HDEF=(
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

[[ ${D[-DUSE_MACRO]+x} ]] && printf '%s\n' "${HEXT[0]}"

if [[ ${D[-DUSE_DUMP]+x} ]]; then
    printf '%s\n' "${HEXT[1]}"
    printf '%s\n' "${HEXT[2]}"
    printf '%s\n' "${HEXT[3]}"
else
    [[ ${D[-DUSE_STRING_EXT]+x} ]] && printf '%s\n' "${HEXT[1]}"
    [[ ${D[-DUSE_FMT]+x} ]]        && printf '%s\n' "${HEXT[2]}"
fi

:
