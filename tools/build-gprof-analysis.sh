echo "running gprof (make sure funorona is build with -pg compiler option)"
gprof funorona gmon.out > gprofanalysis.txt
echo "results written to gprofanalysis.txt"