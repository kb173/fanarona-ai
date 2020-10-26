CXX = g++
CXXFLAGS = -Wall --std=c++11

funorona: main.o Client.o
	$(CXX) $(CXXFLAGS) -o funorona main.o Client.o

main.o: main.cpp
	$(CXX) $(CXXFLAGS) -c main.cpp

Client.o: Client.cpp
	$(CXX) $(CXXFLAGS) -c Client.cpp

clean :
	-rm *.o funorona
