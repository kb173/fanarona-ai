socat TCP4-LISTEN:4455,reuseaddr EXEC:"./Fanorona",end-close,pty &
socat -v TCP4-CONNECT:localhost:4455 EXEC:"./funorona"
