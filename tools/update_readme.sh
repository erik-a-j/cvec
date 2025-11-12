README=README.md
README_DEV=md/README_DEV.md
README_NEW=md/README.md
README_BACKUP=md/README.md.bak

[ -f "$README" ] && [ -f "$README_DEV" ] || exit 1

perl tools/readme.pl

diff "$README_NEW" "$README" >/dev/null && rm -f "$README_NEW" && echo "nothing to update" && exit 0

printf '  COPY  %s -> %s\n' "$README" "$README_BACKUP"
cp -af "$README" "$README_BACKUP" || exit 1
printf '  MOVE  %s -> %s\n' "$README_NEW" "$README"
mv "$README_NEW" "$README" || exit 1