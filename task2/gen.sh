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

# $(rm tmp/*)

for i in {0..2000}
do
  LEN=$(random_range "5" "12")
  STR_DIGS=$(random_bytes "a-zA-Z0-9" "${LEN}")
  DIGS=$(random_bytes "0-9" "${LEN}")
  file="tmp/${STR_DIGS}_${DIGS}.msg"
  $(touch ${file})
  NUM=$(random_bytes "0-9" "${LEN}")
  HASH=$(random_bytes "a-zA-Z0-9" "${LEN}")
  SPEC=$(printf "\x00\x1e")
  SENDER=$(random_range "1" "2")
  MSG_LEN=$(random_range "1" "1200")
  DATA=$(random_bytes "a-zA-Z0-9" "${MSG_LEN}")
  echo "****${DIGS}****${HASH}${SPEC}DD${SENDER}P${DATA}" >> "${file}"
done
