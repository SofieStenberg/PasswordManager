//
//                      Created by Sofie Stenberg 2023-08
//

#ifndef PASSWORDMANAGER_H
#define PASSWORDMANAGER_H
#include <string>
#include <map>

class PasswordManager
{
private:
    static std::string m_databaseName;

public:
    static void setDatabaseName(std::string databaseName);
    static void createDatabase();
    static size_t pwdHashing(std::string pwd);
    static void createMasterFile(size_t hashedPwd);
    static void changeMasterPwd();
    static bool controlMasterPwd();
    static void addNewPassword();
    static void displayEntries();
    static int callbackCount(void *countOfRows, int argc, char **argv, char **colName);
    static void displayPwd();
    static void openExistingDatabase();
    static std::string generatePwd();
    static int replaceIndex(std::string pwd, char biggestChar, char specials[9]);
};

#endif

// -----------------------------------------------------------------------------------------------//

// This is also a valid and functional code snippet to insert values in the database!
// sqlite_exec is a shortcut for using sqlite3_prepare_v2(), sqlite3_step() and sqlite3_finalize()

// sqlite3_stmt \*st;
// sqlite3 *db;
// const char *filePwd = (PasswordManager::m_databaseName + ".db").c_str();
// std::string sqlCommand = "INSERT INTO Passwords (username, password, description) VALUES(?, ?, ?);";
// sqlite3_open(filePwd, &db);
// int res = sqlite3_prepare_v2(db, sqlCommand.c_str(), -1, &st, NULL);
// if (res == SQLITE_OK)
// {
//     sqlite3_bind_text(st, 1, newUsername.c_str(), newUsername.length(), SQLITE_TRANSIENT);
//     sqlite3_bind_text(st, 2, newPwd.c_str(), newPwd.length(), SQLITE_TRANSIENT);
//     sqlite3_bind_text(st, 3, newDesc.c_str(), newDesc.length(), SQLITE_TRANSIENT);
//     sqlite3_step(st);
//     sqlite3_finalize(st);
// }

// -----------------------------------------------------------------------------------------------//