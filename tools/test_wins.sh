#!/bin/bash

wins=0
loses=0

while true
do
    ./socat.sh 2>&1 | grep -q "Player 2 won" && wins=$((wins+1)) || loses=$((loses+1))
    echo "$wins wins vs $loses loses"
    killall Fanorona
done
