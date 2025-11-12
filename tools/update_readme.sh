README=README.md
README_DEV=md/README.md
README_BACKUP=md/README.md.bak

[ -f "$README" ] && [ -f "$README_DEV" ] || exit 1

diff "$README" "$README_DEV" >/dev/null && echo "nothing to update" && exit 0

printf '  COPY  %s -> %s\n' "$README" "$README_BACKUP"
cp -af "$README" "$README_BACKUP" || exit 1
printf '  COPY  %s -> %s\n' "$README_DEV" "$README"
cp -af "$README_DEV" "$README" || exit 1