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
#include ".\sqlite3.h"
#include ".\PasswordManager.h"

std::string PasswordManager::m_databaseName;

// A function that creates a new database
void PasswordManager::createDatabase()
{
    // The user chooses the name of the database
    std::string userInput = "";
    std::cout << "Choose name of database: " << std::endl;
    std::cin >> userInput;
    std::string passwordFile = userInput + ".db";
    const char *filePwd = passwordFile.c_str();

    // Checks if the database already exists
    sqlite3 *db;
    struct stat sb;
    if (stat(filePwd, &sb) == 0)
    {
        std::cout << "Database file exists" << std::endl;
        return;
    }

    // Sets the table for the database
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

    // Executes the SQL statement and the table is here created
    char *messageError;
    int res = sqlite3_exec(db, sqlTable.c_str(), NULL, 0, &messageError);
    if (res != SQLITE_OK)
    {
        std::cout << "Database error:  " << messageError << std::endl;
        sqlite3_close(db);
        return;
    }

    // Setting the database name to the class-variable to keep track of which database are in use
    PasswordManager::m_databaseName = userInput;
    sqlite3_close(db);

    // The user creates a master password for the database
    // This password is then hashed and saved in a .txt file
    std::cout << "Enter master-password: " << std::endl;
    std::cin >> userInput;
    size_t hashedPwd = PasswordManager::pwdHashing(userInput);
    PasswordManager::createMasterFile(hashedPwd);
}

// A function that hashes the specified passwords (master password)
size_t PasswordManager::pwdHashing(std::string pwd)
{
    std::hash<std::string> pwdHashing;
    size_t hashedPwd = pwdHashing(pwd);
    return hashedPwd;
}

// This function creates a file with the hash of the master password
void PasswordManager::createMasterFile(size_t hashedPwd)
{
    std::ofstream masterFile(PasswordManager::m_databaseName + ".txt");
    masterFile << hashedPwd;
    masterFile.close();
    return;
}

// This function allowes the user to change the master password
void PasswordManager::changeMasterPwd()
{
    // Checks if a database is chosen
    if (PasswordManager::m_databaseName == "")
    {
        std::cout << "You have to choose a database to change its master-password.\n"
                  << std::endl;
        return;
    }

    // Checks if the user knows the current master password
    bool master = PasswordManager::controlMasterPwd();
    if (!master)
    {
        std::cout << "The current master-password is wrong..." << std::endl;
        return;
    }

    // The user chooses a new master password
    std::string userInput = "";
    std::cout << "Enter new master-password: " << std::endl;
    std::cin >> userInput;
    size_t hashedPwd = PasswordManager::pwdHashing(userInput);

    // The password replaces the old one in the .txt file
    std::ofstream masterFileOutput(PasswordManager::m_databaseName + ".txt", std::ofstream::trunc);
    masterFileOutput << hashedPwd;
    masterFileOutput.close();
}

// Function used to modify the private variable
void PasswordManager::setDatabaseName(std::string databaseName)
{
    PasswordManager::m_databaseName = databaseName;
}

// This function controls if the user knows the master-password to the current database
bool PasswordManager::controlMasterPwd()
{
    // The user is prompted for the current master password, which is then hashed
    std::string userInput = "";
    std::cout << "What is the current master-password?" << std::endl;
    std::cin >> userInput;
    size_t oldPasswordHashUser = PasswordManager::pwdHashing(userInput);

    // Retrieving the current master password from the .txt file
    std::string oldPasswordHashDatabase = "";
    std::ifstream masterFileInput(PasswordManager::m_databaseName + ".txt");
    std::getline(masterFileInput, oldPasswordHashDatabase);
    std::stringstream stream(oldPasswordHashDatabase);
    size_t oldPwdDatabase;
    stream >> oldPwdDatabase;
    masterFileInput.close();

    // Checks if the user provided password is correct
    if (oldPasswordHashUser != oldPwdDatabase)
    {
        return false;
    }
    return true;
}

// This function adds a new credential entry in the database
void PasswordManager::addNewPassword()
{
    // The user is prompted for a username (required), password (required) and a description (optional)
    std::string newPwd = "";
    std::string newUsername = "";
    std::string newDesc = "";
    std::cout << "Enter the new password: " << std::endl;
    std::cin >> newPwd;
    std::cout << "Enter the new username: " << std::endl;
    std::cin >> newUsername;
    std::cout << "Enter a description (Optional): " << std::endl;
    std::getline(std::cin, newDesc);

    // Open the database file and creates the SQL statement
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

    // Executes the SQL statement
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

// Function to display all the entries of the database
// Although it will only display the entry ID, username and description
void PasswordManager::displayEntries()
{
    // Checks if the user knows the master password
    bool master = PasswordManager::controlMasterPwd();
    if (!master)
    {
        std::cout << "Wrong master-password!\nYou are not allowed to view the content of the database!" << std::endl;
        return;
    }

    // Opens the database and create the SQL command
    const char *filePwd = (PasswordManager::m_databaseName + ".db").c_str();
    sqlite3 *db;
    sqlite3_stmt *st;
    std::string sqlCommand = "SELECT id, username, description FROM Passwords";

    sqlite3_open(filePwd, &db);
    // Prepares the SQL statement
    sqlite3_prepare_v2(db, sqlCommand.c_str(), -1, &st, NULL);
    std::cout << "ID"
              << "\t"
              << "Usernames"
              << "\t"
              << "Descriptions" << std::endl;
    // While there are entries, the information ID, username and description will be displayed
    while (sqlite3_step(st) != SQLITE_DONE)
    {
        std::cout << sqlite3_column_int(st, 0) << "\t" << sqlite3_column_text(st, 1) << "\t" << sqlite3_column_text(st, 2) << std::endl;
    }
    sqlite3_close(db);
}

// A help function to a SQL statement to count the number of rows present in the database
int PasswordManager::callbackCount(void *countOfRows, int argc, char **argv, char **colName)
{
    int *c = (int *)countOfRows;
    // just because the compiler explains that the variables are unused otherwise...
    *c = argc;
    *c = atoi(colName[0]);
    *c = atoi(argv[0]);
    return 0;
}

// Function to display the credentials, inclusive the password, for the user specified entry
void PasswordManager::displayPwd()
{
    // Controlls if the user knows the master password
    bool master = PasswordManager::controlMasterPwd();
    if (!master)
    {
        std::cout << "Wrong master-password!\nYou are not allowed to view the content of the database!" << std::endl;
        return;
    }

    // The user is prompted for an entry ID
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
    // Executes a SQL statement to count the number of entries/rows uis present in the database
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
    // Checking if the user provided ID is within the available range of entries
    if (nrUserInput < 1 || (nrUserInput > countOfRows))
    {
        std::cout << "Entry does not exists..." << std::endl;
        return;
    }

    // Prepares and executes the SQL statement
    sqlCommand = "SELECT * FROM passwords WHERE id = " + userInput;
    sqlite3_prepare_v2(db, sqlCommand.c_str(), -1, &st, NULL);
    std::cout << "ID"
              << "\t"
              << "Usernames"
              << "\t"
              << "Password"
              << "\t"
              << "Descriptions" << std::endl;

    // Displayes the credentials for the speficied entry
    sqlite3_step(st);
    std::cout << sqlite3_column_int(st, 0) << "\t" << sqlite3_column_text(st, 1) << "\t" << sqlite3_column_text(st, 2) << "\t" << sqlite3_column_text(st, 3) << std::endl;
    sqlite3_close(db);
}

// A function to choose an existing database to work with
void PasswordManager::openExistingDatabase()
{
    // The user is prompted for a database (excluding the extension (.db))
    std::string userInput = "";
    std::cout << "Choose database (exclude the extension): " << std::endl;
    std::cin >> userInput;
    struct stat sb;
    if (stat((userInput + ".db").c_str(), &sb) != 0)
    {
        std::cout << "File does not exists..." << std::endl;
        return;
    }

    // Sets the class variable to the chosen database name
    PasswordManager::setDatabaseName(userInput);
    std::cout << "Opened database: " << userInput + ".db" << std::endl;
}

// This function will generate a password of 16 characters containing lower and upper case alphabet,
// numbers and special characters.
std::string PasswordManager::generatePwd()
{
    std::string pwd = "";
    char alphabetUpper = 'A';
    char alphabetLower = 'a';
    char specials[9] = {'!', '-', '_', '&', '@', '/', '?', '%', '#'};
    std::map<char, int> container = {{'A', 0}, {'a', 0}, {'1', 0}, {'!', 0}};

    // bool upper = false;
    // bool lower = false;
    // bool number = false;
    // bool spec = false;

    int randomNr = 0;
    int randomChoice = 0;
    // We want 16 characters in the password
    for (int i = 0; i <= 16; i++)
    {
        // Random choice to add a upper, lower, number or a special character
        randomChoice = rand() % 4;
        // This to make sure that every instance is present at least once
        // if (i == 4 || i == 8 || i == 12 || i == 16)
        // {
        //     if (upper && lower && number && spec)
        //         continue;
        //     else if (!upper)
        //         randomChoice = 0;
        //     else if (!lower)
        //         randomChoice = 1;
        //     else if (!number)
        //         randomChoice = 2;
        //     else if (!spec)
        //         randomChoice = 3;

        switch (randomChoice)
        {
        // An uppercase alphabet
        case 0:
        {
            randomNr = rand() % 26;
            pwd += (alphabetUpper + randomNr);
            container['A']++;
        }
        // A lowercase alphabet
        case 1:
        {
            randomNr = rand() % 26;
            pwd += (alphabetLower + randomNr);
            container['a']++;
        }
        // A number
        case 2:
        {
            randomNr = rand() % 10;
            pwd += std::to_string(randomNr);
            container['1']++;
        }
        // A special character
        case 3:
        {
            randomNr = rand() % 9;
            pwd += specials[randomNr];
            container['!']++;
        }
        }
    }

    // If all types have been used, return the password
    if (container['A'] > 0 && container['a'] > 0 && container['1'] > 0 && container['!'] > 0)
        return pwd;

    // If not all types have been used, we need to fix this.
    // We find the type that has been used the most, finds the first entry of this type
    // and replace it with an entry of the type that was not present.
    bool goOn = true;
    int biggestInt = 0;
    char biggestChar = '-';
    int returnIndex = 0;
    while (goOn)
    {
        for (const auto &pair : container)
        {
            if (pair.second > biggestInt)
            {
                biggestInt = pair.second;
                biggestChar = pair.first;
            }
        }

        returnIndex = PasswordManager::replaceIndex(pwd, biggestChar, specials);
        if (container['A'] == 0)
        {
            randomNr = rand() % 26;
            pwd[returnIndex] = ('A' + randomNr);
            container['A']--;
        }
        if (container['a'] == 0)
        {
            randomNr = rand() % 26;
            pwd[returnIndex] = ('a' + randomNr);
            container['a']--;
        }
        if (container['1'] == 0)
        {
            randomNr = rand() % 10;
            pwd[returnIndex] = randomNr;
            container['1']--;
        }
        if (container['!'] == 0)
        {
            randomNr = rand() % 9;
            pwd[returnIndex] = specials[randomNr];
            container['!']--;
        }

        if (container['A'] > 0 && container['a'] > 0 && container['1'] > 0 && container['!'] > 0)
            goOn = false;
    }
    return pwd;
}

// Helper function to find the index at where to replace the caracter in the generated password string
int PasswordManager::replaceIndex(std::string pwd, char biggestChar, char specials[9])
{
    int index = 0;

    if (biggestChar == 'A' || biggestChar == 'a')
    {
        for (int index = 0; index <= (int)pwd.length(); index++)
        {
            if ((pwd[index] > (biggestChar + 0)) && (pwd[index] <= (biggestChar + 26)))
                break;
        }
    }

    if (biggestChar == '1')
    {
        for (int index = 0; index <= (int)pwd.length(); index++)
        {
            if ((pwd[index] > (biggestChar + 0)) && (pwd[index] <= (biggestChar + 10)))
                break;
        }
    }
    if (biggestChar == '!')
    {
        for (int index = 0; index <= (int)pwd.length(); index++)
        {
            for (int s = 0; s <= 9; s++)
            {
                if (pwd[index] == specials[s])
                    break;
            }
            if (index > 0)
                break;
        }
    }

    return index;
}