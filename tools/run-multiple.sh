#!/bin/bash
times=$1
mkdir results
for i in $(seq 1 1 $times); do
  echo "Run $i (saving result in results/$i.txt)"
  ./funorona > results/$i.txt
done
