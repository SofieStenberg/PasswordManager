#include <iostream>
#include <stdio.h>
#include <cstring>
#include <sys/stat.h>
#include <functional>
#include <fstream>
#include <sstream>
#include ".\sqlite3.h"
#include ".\PasswordManager.h"

std::string PasswordManager::m_databaseName;

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
        return;
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
    PasswordManager::m_databaseName = userInput;
    sqlite3_close(db);
}

// Hashes passwords
size_t PasswordManager::pwdHashing(std::string pwd)
{
    std::hash<std::string> pwdHashing;
    size_t hashedPwd = pwdHashing(pwd);
    return hashedPwd;
}

// Creates a file with the hash of the master-password
void PasswordManager::createMasterFile(size_t hashedPwd)
{
    std::ofstream masterFile(PasswordManager::m_databaseName + ".txt");
    masterFile << hashedPwd;
    masterFile.close();
    return;
}

// Changes the master-password after controlling that the user knows the old master-password
void PasswordManager::changeMasterPwd()
{
    if (PasswordManager::m_databaseName == "")
    {
        std::cout << "You have to choose a database to change its master-password.\n"
                  << std::endl;
        return;
    }
    std::string userInput = "";
    std::cout << "What is the current master-password?" << std::endl;
    std::cin >> userInput;
    size_t oldPasswordHashUser = PasswordManager::pwdHashing(userInput);
    std::string oldPasswordHashDatabase = "";
    std::ifstream masterFileInput(PasswordManager::m_databaseName + ".txt");
    std::getline(masterFileInput, oldPasswordHashDatabase);
    std::stringstream stream(oldPasswordHashDatabase);
    size_t oldPwdDatabase;
    stream >> oldPwdDatabase;
    masterFileInput.close();

    if (oldPasswordHashUser != oldPwdDatabase)
    {
        std::cout << "The current master-password is wrong..." << std::endl;
        return;
    }

    std::cout << "Enter new master-password: " << std::endl;
    std::cin >> userInput;
    size_t hashedPwd = PasswordManager::pwdHashing(userInput);

    std::ofstream masterFileOutput(PasswordManager::m_databaseName + ".txt", std::ofstream::trunc);
    masterFileOutput << hashedPwd;
    masterFileOutput.close();
}