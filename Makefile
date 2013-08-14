all:
	g++ -O2 -o bcmenu bcmenu.cpp -std=c++11 -lncurses -lncursesw

debug:
	g++ -g -o bcmenu bcmenu.cpp -std=c++11 -lncurses -lncursesw
