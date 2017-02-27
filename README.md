# Desklayer
Desklayer is the result of my thesis work on the theme "Developing an advanced user interface" (2007), the program aims to simplify the desktop experience and order visible windows on the desktop.

Desklayer is a windows manager that provides to arrange windows on the desktop in various types of layout. Desklayer allows you to move windows across all positions of the layout and arrange the windows size and the layout arrangement dynamically according to the size of the downsized window. Desklayer also re-arrange the layout in consequence of events that involve windows visible on the screen. 

Main Features

Here is a list of main Desklayer's features:

    Border Layout and Grid Layout (Java-like) for windows arrangement
    Dynamic resizing of windows to exploit all the possible space on the desktop
    Dynamic re-arrangement after windows events (close window,add window,minimize window,maximize window, ...)
    Possibility of add and move windows from and to all the sides of the layout (in border layout mode)
    Exclusion of windows associated to selected executables
    Assignment of custom dimensions to the sides of the layout (border layout only)
    Automatic arrangement of all visible windows on desktop
    Restore windows positions to their positions before the applying of Desklayer's layout

Short manual

    CTRL+ALT+B : activate border layout
    CTRL+ALT+G :activate grid layout
    CTRL+ARROWS: move or exchange windows arranged from the side indicated by the arrow to the central cell cyclically (only border layout)
    ALT+ARROWS : move selected window to the side pointed by the pressed arrow
    ALT+F10 : move selected window to the central cell
    CTRL+BACKSPACE : restore windows positions to their previous arrangement on the desktop (before applying Desklayer's layout)

The program is written in C++ using Win32 APIs.

More info, screenshots and videos of the features here:
http://www.lkrprojects.com/home.html
