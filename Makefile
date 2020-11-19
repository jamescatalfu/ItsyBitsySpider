#BOOST_HOME = /usr/local/boost_1_71_0

maze: main.o
	g++ -std=c++17 main.o -o maze
main.o: main.cpp
	g++ -std=c++17 -c -I$(BOOST_HOME) main.cpp
clean:
	rm -f *.o