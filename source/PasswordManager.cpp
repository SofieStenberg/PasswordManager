//
//                      Created by Sofie Stenberg 2023-08
//

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <cctype>
#include <sys/stat.h>
#include <functional>
#include <fstream>
#include <sstream>
#include <map>
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

    std::string sqlTable = "CREATE TABLE IF NOT EXISTS Passwords("
                           "id          INTEGER     PRIMARY KEY     AUTOINCREMENT, "
                           "userName    TEXT        NOT NULL, "
                           "password    TEXT        NOT NULL, "
                           "description Text);";

    int file_status = sqlite3_open(filePwd, &db);
    if (file_status)
    {
        std::cerr << "Database could not be created" << sqlite3_errmsg(db) << std::endl;
        return;
    }
    char *messageError;
    //  database, SQL-statement, Callback function, 1st argument to callback function, error msg
    int res = sqlite3_exec(db, sqlTable.c_str(), NULL, 0, &messageError);
    if (res != SQLITE_OK)
    {
        std::cout << "Database error:  " << messageError << std::endl;
        sqlite3_close(db);
        return;
    }

    PasswordManager::m_databaseName = userInput;
    sqlite3_close(db);

    std::cout << "Enter master-password: " << std::endl;
    std::cin >> userInput;
    size_t hashedPwd = PasswordManager::pwdHashing(userInput);
    PasswordManager::createMasterFile(hashedPwd);
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

    bool master = PasswordManager::controlMasterPwd();
    if (!master)
    {
        std::cout << "The current master-password is wrong..." << std::endl;
        return;
    }

    std::string userInput = "";
    std::cout << "Enter new master-password: " << std::endl;
    std::cin >> userInput;
    size_t hashedPwd = PasswordManager::pwdHashing(userInput);

    std::ofstream masterFileOutput(PasswordManager::m_databaseName + ".txt", std::ofstream::trunc);
    masterFileOutput << hashedPwd;
    masterFileOutput.close();
}

// Used to access the private variable
void PasswordManager::setDatabaseName(std::string databaseName)
{
    PasswordManager::m_databaseName = databaseName;
}

// Controls if the user knows the master-password to the current database
bool PasswordManager::controlMasterPwd()
{
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
        return false;
    }
    return true;
}

void PasswordManager::addNewPassword()
{
    std::string newPwd = "";
    std::string newUsername = "";
    std::string newDesc = "";
    std::cout << "Enter the new password: " << std::endl;
    std::cin >> newPwd;
    std::cout << "Enter the new username: " << std::endl;
    std::cin >> newUsername;
    std::cout << "Enter a description (Optional): " << std::endl;
    std::getline(std::cin, newDesc);

    const char *filePwd = (PasswordManager::m_databaseName + ".db").c_str();
    sqlite3 *db;

    sqlite3_open(filePwd, &db);
    char *messageError;
    std::stringstream sqlCommand;
    sqlCommand << "INSERT INTO Passwords (username, password, description) VALUES('"
               << newUsername
               << "', '" << newPwd
               << "', '" << newDesc
               << "');";

    int res = sqlite3_exec(db, sqlCommand.str().c_str(), NULL, 0, &messageError);
    if (res != SQLITE_OK)
    {
        std::cout << "Insertion error:  " << messageError << std::endl;
        sqlite3_close(db);
        return;
    }

    sqlite3_close(db);

    std::cout << "Credentials added" << std::endl;
}

// Function to display the contents of the database
void PasswordManager::displayEntries()
{
    bool master = PasswordManager::controlMasterPwd();
    if (!master)
    {
        std::cout << "Wrong master-password!\nYou are not allowed to view the content of the database!" << std::endl;
        return;
    }

    const char *filePwd = (PasswordManager::m_databaseName + ".db").c_str();
    sqlite3 *db;
    sqlite3_stmt *st;
    std::string sqlCommand = "SELECT id, username, description FROM Passwords";

    sqlite3_open(filePwd, &db);
    sqlite3_prepare_v2(db, sqlCommand.c_str(), -1, &st, NULL);
    std::cout << "ID"
              << "\t"
              << "Usernames"
              << "\t"
              << "Descriptions" << std::endl;
    // Displays everything except the id
    while (sqlite3_step(st) != SQLITE_DONE)
    {
        std::cout << sqlite3_column_int(st, 0) << "\t" << sqlite3_column_text(st, 1) << "\t" << sqlite3_column_text(st, 2) << std::endl;
    }
    sqlite3_close(db);
}

int PasswordManager::callbackCount(void *countOfRows, int argc, char **argv, char **colName)
{
    int *c = (int *)countOfRows;
    // just because the compiler explains that the variables are unused otherwise...
    *c = argc;
    *c = atoi(colName[0]);
    *c = atoi(argv[0]);
    return 0;
}

void PasswordManager::displayPwd()
{
    bool master = PasswordManager::controlMasterPwd();
    if (!master)
    {
        std::cout << "Wrong master-password!\nYou are not allowed to view the content of the database!" << std::endl;
        return;
    }

    std::string userInput = "";
    std::cout << "Choose the entry/ID to the password you want to reveal:" << std::endl;
    std::cin >> userInput;
    int nrUserInput = 0;

    try
    {
        nrUserInput = stoi(userInput);
    }
    catch (...)
    {
        std::cout << "Invalid input..." << std::endl;
        return;
    }
    const char *filePwd = (PasswordManager::m_databaseName + ".db").c_str();
    sqlite3 *db;
    sqlite3_stmt *st;
    int countOfRows = 0;
    char *messageError;
    std::string sqlCommand = "SELECT COUNT(*) FROM Passwords";

    sqlite3_open(filePwd, &db);
    int res = sqlite3_exec(db, sqlCommand.c_str(), PasswordManager::callbackCount, &countOfRows, &messageError);
    if (res != SQLITE_OK)
    {
        std::cout << "Error in retrieving count: " << messageError << std::endl;
        return;
    }
    if (nrUserInput < 1 || (nrUserInput > countOfRows))
    {
        std::cout << "Entry does not exists..." << std::endl;
        return;
    }

    sqlCommand = "SELECT * FROM passwords WHERE id = " + userInput;
    sqlite3_prepare_v2(db, sqlCommand.c_str(), -1, &st, NULL);
    std::cout << "ID"
              << "\t"
              << "Usernames"
              << "\t"
              << "Password"
              << "\t"
              << "Descriptions" << std::endl;

    // while (sqlite3_step(st) != SQLITE_DONE)
    // {
    //     std::cout << sqlite3_column_int(st, 0) << "\t" << sqlite3_column_text(st, 1) << "\t" << sqlite3_column_text(st, 2) << std::endl;
    // }
    sqlite3_step(st);
    std::cout << sqlite3_column_int(st, 0) << "\t" << sqlite3_column_text(st, 1) << "\t" << sqlite3_column_text(st, 2) << "\t" << sqlite3_column_text(st, 3) << std::endl;
    sqlite3_close(db);
}

void PasswordManager::openExistingDatabase()
{
    std::string userInput = "";
    std::cout << "Choose database (exclude the extension): " << std::endl;
    std::cin >> userInput;
    struct stat sb;
    if (stat((userInput + ".db").c_str(), &sb) != 0)
    {
        std::cout << "File does not exists..." << std::endl;
        return;
    }

    PasswordManager::setDatabaseName(userInput);
    std::cout << "Opened database: " << userInput + ".db" << std::endl;
}

std::string PasswordManager::generatePwd()
{
    std::string pwd = "";
    std::map<int, std::string> alphabetUpper;
    alphabetUpper = {{0, "A"}, {1, "B"}, {2, "C"}, {3, "D"}, {4, "E"}, {5, "F"}, {6, "G"}, {7, "H"}, {8, "I"}, {9, "J"}, {10, "K"}, {11, "L"}, {12, "M"}, {13, "N"}, {14, "O"}, {15, "P"}, {16, "Q"}, {17, "R"}, {18, "S"}, {19, "T"}, {20, "U"}, {21, "V"}, {22, "W"}, {23, "X"}, {24, "Y"}, {25, "Z"}};

    std::map<int, std::string> alphabetLower;
    alphabetLower = {{0, "a"}, {1, "b"}, {2, "c"}, {3, "d"}, {4, "e"}, {5, "f"}, {6, "g"}, {7, "h"}, {8, "i"}, {9, "j"}, {10, "k"}, {11, "l"}, {12, "m"}, {13, "n"}, {14, "o"}, {15, "p"}, {16, "q"}, {17, "r"}, {18, "s"}, {19, "t"}, {20, "u"}, {21, "v"}, {22, "w"}, {23, "x"}, {24, "y"}, {25, "z"}};

    std::map<int, std::string> specials;
    specials = {{0, "!"}, {1, "-"}, {2, "_"}, {3, "&"}, {4, "@"}, {5, "/"}, {6, "?"}, {7, "%"}, {8, "#"}};

    int randomNr = 0;
    int randomChoice = 0;
    for (int i = 0; i < 16; i++)
    {
        randomChoice = rand() % 4;
        switch (randomChoice)
        {
        case 0:
        {
            randomNr = rand() % 26;
            pwd = pwd + alphabetUpper[randomNr];
        }
        case 1:
        {
            randomNr = rand() % 26;
            pwd = pwd + alphabetLower[randomNr];
        }
        case 2:
        {
            randomNr = rand() % 10;
            pwd = pwd + std::to_string(randomNr);
        }
        case 3:
        {
            randomNr = rand() % 9;
            pwd = pwd + specials[randomNr];
        }
        }
    }
    return pwd;
}