CXX = g++
CXXFLAGS = -Wall --std=c++11 -g

funorona: main.o Client.o Board.o
	$(CXX) $(CXXFLAGS) -o funorona main.o Client.o Board.o

main.o: main.cpp Client.o Board.o
	$(CXX) $(CXXFLAGS) -c main.cpp

Client.o: Client.cpp
	$(CXX) $(CXXFLAGS) -c Client.cpp

Board.o: Board.cpp
	$(CXX) $(CXXFLAGS) -c Board.cpp

clean:
	-rm *.o funorona
