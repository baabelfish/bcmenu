bcmenu
======

Bcmenu is a fuzzy menu for command line. It currently comes with a wrapper for
opening files, navigating folders and going back directory stack.

Installation
------------
First you should compile bcmenu\_exe with ``make``. After that you should move
bcmenu and bcmenu\_exe to your preferred directory for execution.

Usage
-----

This basically works as the same as [dmenu](http://tools.suckless.org/dmenu/)
but has fuzzy matching and it's for the terminal.

The next bit gives a list of options to bcmenu and it let's you choose one
that's returned to stdout after the program stops running.
``
$ echo "option1\nnoption2"|bcmenu
``

<!--(
#### Opening a file from terminal: 
``````

#### Add the following to your `.zshrc` to engage the previous command with `C-t`:
```bindkey -s '^T' '^qbcmenu_files && vim $(bcmenu_result)\n'```

#### This one gets you to the directory you want:
```bindkey -s '^F' '^qbcmenu_dirs && cd "$(bcmenu_result)"\n'```
)-->
Keybindings
-----------

- `Enter` Saves the result and exits the program.
- `C-n` Next match.
- `C-p` Previous match.
- `C-w` Clears the search.
- `C-c` Normal interrupt (result is not saved).
- `C-o` Add focused item to selection.
- `C-i` Deselect focused item.

Examples
--------
##### Opening files with vim
``
cd /usr/include
vim $(find|bcmenu)
``
<img src="https://raw.github.com/baabelfish/bcmenu/master/examples/example1.gif" alt="Finding files" />

TODO
----
- Add colors.
- Make characters customizable.
- Write more match functions (including a proper fuzzy match).
- Show selected characters underlined in search.
- Make options scroll with focused item.
- Add regex support.

### Done
- Fix pipe and get rid of bcmenu\_result.
- Add support for multiple selection.
- Remember terminal screen.

Similar software
----------------
[slmenu](https://bitbucket.org/rafaelgg/slmenu)
