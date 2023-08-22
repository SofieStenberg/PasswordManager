#include <iostream>
#include <stdio.h>
#include <cstring>
#include <sys/stat.h>
#include <functional>
#include <fstream>
#include ".\sqlite3.h"
#include ".\PasswordManager.h"

// Creates a new database
void PasswordManager::createDatabase()
{
    std::string userInput = "";
    std::cout << "Choose name of database: " << std::endl;
    std::cin >> userInput;
    std::string passwordFile = userInput + ".db";
    const char *filePwd = passwordFile.c_str();
    sqlite3 *db;
    struct stat sb;
    if (stat(filePwd, &sb) == 0)
    {
        std::cout << "Database file exists" << std::endl;
    }
    else
    {
        std::string sqlTable = "CREATE TABLE PASSWORDS("
                               "userName   TEXT    NOT NULL"
                               "password   TEXT    NOT NULL"
                               "description    Text";
        int file_status = sqlite3_open(filePwd, &db);
        if (file_status)
        {
            std::cerr << "Database could not be created" << sqlite3_errmsg(db) << std::endl;
            return;
        }
        char *messageError;
        // database, SQL-statement, Callback function, 1st argument to callback function, error msg
        sqlite3_exec(db, sqlTable.c_str(), NULL, 0, &messageError);
        std::cout << "Database created!" << std::endl;
    }
    PasswordManager::m_masterPWD = userInput;
    sqlite3_close(db);
}

// Hashes passwords
size_t PasswordManager::pwdHashing(std::string pwd)
{

    std::cout << "You choose the password: " << pwd << std::endl;

    size_t hashedPwd = pwdHashing(pwd);
    std::cout << hashedPwd;

    return hashedPwd;
}

// Creates a file with the hash of the master-password
void createMasterFile(size_t hashedPwd)
{
    std::ofstream masterFile(PasswordManager::m_masterPWD + ".txt");
    masterFile << hashedPwd;
    masterFile.close();
    return;
}