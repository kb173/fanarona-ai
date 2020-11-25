CXX = g++
CXXFLAGS = -Wall --std=c++11 -pg

funorona: main.o Client.o LocalClient.o GameElements.o Board.o AIPlayer.o
	$(CXX) $(CXXFLAGS) -o funorona main.o Client.o LocalClient.o GameElements.o Board.o AIPlayer.o

main.o: main.cpp Client.o Board.o
	$(CXX) $(CXXFLAGS) -c main.cpp

Client.o: Client.cpp
	$(CXX) $(CXXFLAGS) -c Client.cpp
	
LocalClient.o: LocalClient.cpp
	$(CXX) $(CXXFLAGS) -c LocalClient.cpp

GameElements.o: GameElements.cpp
	$(CXX) $(CXXFLAGS) -c GameElements.cpp

Board.o: Board.cpp
	$(CXX) $(CXXFLAGS) -c Board.cpp

AIPlayer.o: AIPlayer.cpp
	$(CXX) $(CXXFLAGS) -c AIPlayer.cpp

clean:
	-rm *.o funorona
