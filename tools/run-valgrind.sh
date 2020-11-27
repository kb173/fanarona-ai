# valgrind default
#valgrind --tool=memcheck ./funorona

# verbose output
#valgrind --leak-check=full -v --show-leak-kinds=all --tool=memcheck ./funorona

# direct log to file, most verbose
valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes --verbose --log-file=valgrind-out.txt ./funorona