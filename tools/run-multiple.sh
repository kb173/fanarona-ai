#!/bin/bash

times=$1

echo "$(date +"%T") running run_multiple - $1..."

mkdir results
for i in $(seq 1 1 $times); do
  echo "Run $i (saving result in results/$i.txt)"
  ./funorona > results/$i.txt # run socketclient
  #./socat.sh 2>&1 | tee results/$i.txt # run localclient
done
