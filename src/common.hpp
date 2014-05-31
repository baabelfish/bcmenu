#pragma once

#include <string>

std::string getHelptext() {
    return
"Usage: <app>|bcmenu [OPTIONS]\n\
\n\
Bcmenu is a fuzzy menu for the terminal, made with ncurses.\n\
\n\
\n\
Keybindings:\n\
------------\n\
\n\
Enter   Saves the result and exits the program.\n\
C-n     Next match.\n\
C-p     Previous match.\n\
C-w     Clears the search.\n\
C-c     Normal interrupt (result is not saved).\n\
C-o     Add focused item to selection.\n\
C-i     Unselect focused item.\n\
C-u     Move page up\n\
C-d     Move page down\n\
C-b     Move to first item\n\
C-e     Move to last item\n\
\n\
\n\
Arguments:\n\
----------\
";
}
