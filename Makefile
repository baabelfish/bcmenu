all:
	clang++ -O2 -o bcmenu_exe src/*.cpp -std=c++11 -lncurses -lncursesw
