fuzzy-cli-finder
================

Bcmenu is a fuzzy menu for command line. It currently comes with a wrapper for
opening files, navigating folders and going back directory stack.

Installation
------------

##### Compile bmenu\_exe
``
make
``

#####

Usage
-----

This basically works as the same as [dmenu](http://tools.suckless.org/dmenu/)
but has fuzzy matching and it's for the terminal.

The next bit gives a list of options to bcmenu and it let's you choose one
that's returned to stdout after the program stops running.
``
$ echo "option1\nnoption2"|bcmenu
``

##### Opening a file in vim
``
vim $(find|bcmenu)
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

TODO
----
- Add colors.
- Make characters customizable.
- Add bookmarks.
- Write more match functions (including a proper fuzzy match).
- Remember terminal screen.

### Done
- Fix pipe and get rid of bcmenu\_result.

Similar software
----------------
[slmenu](https://bitbucket.org/rafaelgg/slmenu)
