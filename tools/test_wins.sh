#!/bin/bash

wins=0
loses=0

while true
do
    ./socat.sh 2>&1 | tee lose_output.txt | grep -q "Player 2 won" && wins=$((wins+1)) || loses=$((loses+1))
    
    echo "$wins wins vs $loses loses"
    
    if (( loses > 0 )); then
        break
    else
        rm lose_output.txt
    fi
    
    killall Fanorona &>/dev/null
done
