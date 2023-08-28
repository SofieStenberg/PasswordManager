# PasswordManager

A terminal based password manager.

To compile with a makefile on windows: mingw32-make

The files regarding the sqlite was downloaded from: https://www.sqlite.org/download.html,
And the file downloaded was: sqlite-amalgamation-3420000.zip

To include sqlite3:

1. Create objectfile; `gcc sqlite3.c -c`
2. In the c++ file, be sure to `#include ".\sqlite3.h"`
3. Compile with the object-file; `g++ main.cpp sqlite3.o -Isqlite3`
