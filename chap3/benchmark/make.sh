#!/bin/bash

NT=10
HT=10

if [ $# -eq 2 ]; then
    NT="$1"
    HT="$2"
fi

gcc -pthread -Wall -DNUM_THREADS="$NT" -DHOLDTIME="$HT" -o lock_benchmark lock_benchmark.c
