bcmenu
======

Bcmenu is a fuzzy menu for the command line. Pipe stuff in and get your
selection back, you know the drill.

Installation
------------
First you need to configure and compile bcmenu\_exe.
```
$ ./configure
$ make
```
After that you should move
bcmenu and bcmenu\_exe to your preferred directory for execution.

Usage
-----

This basically works as the same as [dmenu](http://tools.suckless.org/dmenu/)
but has fuzzy matching and it's for the terminal.

The next bit gives a list of options to bcmenu and it let's you choose one
that's returned to stdout after the program stops running.
```
$ echo "option1\nnoption2"|bcmenu
```

Keybindings
-----------

- `Enter` Saves the result and exits the program.
- `C-n` Next match.
- `C-p` Previous match.
- `C-w` Clears the search.
- `C-c` Normal interrupt (result is not saved).
- `C-o` Toggles selection.
- `C-u` Move page up.
- `C-d` Move page down.
- `C-b` Move to first item.
- `C-e` Move to last item.

Examples
--------
##### Opening files with vim
```
$ cd /usr/include
$ vim $(find|bcmenu)
```
<img src="https://raw.github.com/baabelfish/bcmenu/master/examples/example1.gif" alt="Finding files" />

#### Installing and removing packages with pacman
```
$ sudo pacman -S $(package-query -Slf"%r/%n %v %d"|awk '{ printf "%-30s",$1; printf "%-10s",$2; $1=$2=""; print }'|sort|bcmenu|cut -f2 -d'/'|cut -f1 -d' '|tr '\n' ' ')
$ sudo pacman -R $(pacman -Sl|grep '\[installed'|cut -f2 -d' '|sort|bcmenu|cut -f2 -d'/'|cut -f1 -d' ')
```
<img src="https://raw.github.com/baabelfish/bcmenu/master/examples/example2.gif" alt="Finding files" />

TODO
----
- Add search ranges.
- Add an option to remember last search.
- Add threaded matching as an option.
- Add one line support.
- Add regex support.
- Add colors.
- Make characters customizable.
- Show selected characters underlined in search.
- Write more match functions (including a proper fuzzy match).


Similar software
----------------
[slmenu](https://bitbucket.org/rafaelgg/slmenu)
