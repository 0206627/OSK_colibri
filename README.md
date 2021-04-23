# OSK Colibri

This is a working example for an app that does the following:
- Receives a NOOP command and adds 2 to a counter 
- Receives a RESET command and resets the counter
- Receives a NEW_CMD command and logs text depending on the given number
- IMPORTANT:
- Please before cloning this repository, make sure to have OSK installed. 
- After cloning it, go to cfs directory and run the following commands:
-make distclean
-make prep
-make
-make install
-chmod urwx cmake.sh
