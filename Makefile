CXX = g++
CXXFLAGS = -Wall

funorona: main.o
	$(CXX) $(CXXFLAGS) -o funorona main.o

main.o: main.cpp
	$(CXX) $(CXXFLAGS) -c main.cpp

clean :
	-rm *.o funorona
