all:
	g++ -O2 -o bcmenu src/bcmenu.cpp -std=c++11 -lncurses -lncursesw

debug:
	g++ -g -o bcmenu src/bcmenu.cpp -std=c++11 -lncurses -lncursesw
