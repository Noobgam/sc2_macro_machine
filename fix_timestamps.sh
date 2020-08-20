# This script estimates git timestamps for files based on commit log
#  used to make sure cmake caches files correctly.

git ls-tree -r --name-only HEAD | while read filename; do 
  unixtime=$(git log -1 --format="%at" -- "${filename}")
  touchtime=$(date -d @$unixtime +'%Y%m%d%H%M.%S')
  touch -t ${touchtime} "${filename}"
done
