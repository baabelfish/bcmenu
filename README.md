fuzzy-cli-finder
================

Bcmenu is a fuzzy menu for terminal. It currently comes with a wrapper for opening files
and navigating back directory stack.

Installation
------------
Run `./install.sh` if you dare. (FIXME)

Usage
-----

Opening a file from terminal: `bcmenu_files && vim $(finderresult.sh)`

Add this line to your `.zshrc` to engage the previous command with `C-t`:
`bindkey -s '^T' '^qbcmenu_files && vim $(bcmenu\_result)\n'`

Keybindings
-----------

- `Enter` Saves the result and exits the program.
- `C-n` Next match.
- `C-p` Previous match.
- `C-w` Clears the search.
- `C-c` Normal interrupt (result is not saved).

TODO
----
- Fix pipe and get rid of bcmenu\_result.
- Add colors.
- Make characters customizable.
