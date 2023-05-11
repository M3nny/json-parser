all:
	g++ -std=c++17 test.cpp -o test
	./test
debug:
	g++ -std=c++17 test.cpp -o test -g
	./test
	valgrind --leak-check=full --track-origins=yes -s  ./test
