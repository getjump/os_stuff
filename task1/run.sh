#!/usr/local/Cellar/bash/4.3.42/bin/bash

for file in *; do
  if [[ $file =~ ([a-z]{4})([0-9]{4})([0-9]{4})(\..*) ]]; then
    m=("${BASH_REMATCH[@]}")
    yr=$((10#${m[2]}+1))

    if [ "$yr" -gt "9999" ]; then
      let "yr = 0"
    fi

    year=`printf "%.4i" "${yr}"`

    mv "$file" "${m[1]^^}${year}${m[3]}${m[4]}"
  fi
done
