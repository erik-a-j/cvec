#!/usr/bin/bash

SCRIPTDIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )
SRCDIR="$SCRIPTDIR/../src"
OH="$SRCDIR/cvec.h"

get_headers() {
    local headers=()
    for file in "$SRCDIR"/inc/*.h; do
        h="$(basename -- "$file")"
        [[ ! "$h" =~ .*impl.h$ ]] && headers+=("$h")
    done
    printf '%s\n' "${headers[@]}"
}

#mapfile -t H < <(get_headers)

in_array() {
    local needle="$1"; shift
    local array=("$@")
    for i in "${array[@]}"; do
        [[ "$i" == "$needle" ]] && return 0
    done
    return 1
}

D=("$@")
HDEF=(
    "cvec/cvec_types.h"
    "cvec/cvec_default_hooks.h"
    "cvec/cvec_base.h"
)
HEXT=(
    "cvec/cvec_macro.h"
    "cvec/cvec_string_ext.h"
    "cvec/cvec_dump.h"
    "cvec/cvec_fmt.h"
)

printf '#ifndef CVEC_H\n#define CVEC_H\n\n' > "$OH"

for h in "${HDEF[@]}"; do
    printf '#include "%s"\n' "$h" >> "$OH"
done

in_array "-DUSE_MACRO" "${D[@]}" && \
printf '#include "%s"\n' "${HEXT[0]}" >> "$OH"

in_array "-DUSE_STRING_EXT" "${D[@]}" && \
printf '#include "%s"\n' "${HEXT[1]}" >> "$OH"

if in_array "-DUSE_DUMP" "${D[@]}"; then
    printf '#include "%s"\n' "${HEXT[3]}" >> "$OH"
    printf '#include "%s"\n' "${HEXT[2]}" >> "$OH"
elif in_array "-DUSE_FMT" "${D[@]}"; then
    printf '#include "%s"\n' "${HEXT[3]}" >> "$OH"
fi

printf '\n#endif\n' >> "$OH"

#printf 'H: %s\n' "${H[@]}"
#printf 'D: %s\n' "${D[@]}"