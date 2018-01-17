#!/bin/bash
ls -t|xargs grep -l "\x1eDD${1}[PS]"|xargs cat>$2 # 49
# ls -t|xargs grep -l "\*+\d+[A-Za-z0-9]+\x1eDD${1}[PS].*">$2 # 59
