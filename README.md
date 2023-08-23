# PasswordManager

A password manager

compile: g++ ./PasswordManager

To compile with a makefile on windows: mingw32-make

The files regarding the sqlite was downloaded from: https://www.sqlite.org/download.html,
And the file downloaded was: sqlite-amalgamation-3420000.zip

To include sqlite3:

1. Create objectfile; `gcc sqlite3.c -c`
2. In the c++ file, be sure to `#include ".\sqlite3.h"`
3. Compile with the object-file; `g++ main.cpp sqlite3.o -Isqlite3`

[comment]: <> (This is also a valid and functional code snippet to insert values in the database!!)

[comment]: <> (sqlite_exec is a shortcut for using sqlite3_prepare_v2(), sqlite3_step() and sqlite3_finalize())

[comment]: <> (sqlite3_stmt \*st;)
[comment]: <> (std::string sqlCommand = "INSERT INTO Passwords (username, password, description) VALUES(?, ?, ?);"; //" + newUsername + ", " + newPwd + ", " + newDesc + ")
[comment]: <> (sqlite3_open(filePwd, &db);)
[comment]: <> (int res = sqlite3_prepare_v2(db, sqlCommand.c_str(), -1, &st, NULL);)
[comment]: <> (if (res == SQLITE_OK))
[comment]: <> ({)
[comment]: <> (sqlite3_bind_text(st, 1, newUsername.c_str(), newUsername.length(), SQLITE_TRANSIENT);)
[comment]: <> (sqlite3_bind_text(st, 2, newPwd.c_str(), newPwd.length(), SQLITE_TRANSIENT);)
[comment]: <> (sqlite3_bind_text(st, 3, newDesc.c_str(), newDesc.length(), SQLITE_TRANSIENT);)
[comment]: <> (sqlite3_step(st);)
[comment]: <> (sqlite3_finalize(st);)
[comment]: <> (})
