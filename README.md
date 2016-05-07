# devbox

## "UNIX is an IDE"
I've heard this many times, nonetheless there are IDE's for Linux (like any other polatform), even if many tools (like build systems, static and dynamic analyzers, test suites, and so on) are not integrated in all the IDE's.
Devbox is born as an idea to extend the file manager, a simple menu entry where to start programs related to programming. Have adirectory with a cmake project? Just open it with your IDE (if it supports cmake), o with cmake-gui to generate a project. Or lets execute a static analyzer on you project, or a dynamic one, like Dr. Memory or Valgrind.

Probably in the long term, the terminal is still a better choice for moste of the tools, but sometimes it's simply nice to make simple thing simple, just a click away.

## Status of the project
This is more a toy project, I've tried to create a plugin for caja, the file manager of Mate, since it is the desktop environment I'm using. I wanted to use C++ as programming language (and not C), even if alla samples found are in C or python. The plugin works, but it's in alpha status, with many unanswered question (I have never used glib before).
Contributions are welcome, I've structured the project with the idea to extend it for other file managers (Explorer of Windows, Nemo of Gnome, Dolphin of KDE and so on...), even if I will work on this project only in my spare time.


## Build
As described before, this is a C++ Project, managed with cmake. As for now the dependencies are those for building a Caja plugin, ie gtk2, glib, caja and catch (for writing test cases)


## Roadmap(?)
Following things needs to be done, not necessarily in this order:
* Caja does some sort of caching when generating the menu entries the first time, and I've nevers seen the "closureNotify" function being called. If the function never gets called we have a memory leak, but I've found no other way for deleting "objects" when using g_signal_connect_data, or maybe my apporach is completely wrong?
* the cmake scripts more modular
* Add unit tests, there are already some test cases, but I've not managed to instantiate a GTypeModuleClass for calling caja_module_initialize
* There is no proper error handling, what should someone do if caja_module_initialize or caja_module_list_types goes wrong?
* Add more typesafety. As for know there are a lot of casts, due to the nature of the C interface.

