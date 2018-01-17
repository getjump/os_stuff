#!/usr/local/Cellar/bash/4.3.42/bin/bash
LC_CTYPE=C # OSx Fix

random_bytes()
{
  cat /dev/urandom | tr -dc ${1} | fold -w ${2} | head -n 1
}

random_range()
{
  jot -r 1 ${1} ${2}
}

$(rm tmp/*)

for i in {0..1000}
do
  ABBR=$(random_bytes "a-z" "4")
  YEAR=$(random_range "1990" "2015")
  MONTH=`printf %02d $(random_range "1" "12"})`
  DAY=`printf %02d $(random_range "1" "31"})`
  $(touch tmp/${ABBR}${YEAR}${MONTH}${DAY}.res)
done
