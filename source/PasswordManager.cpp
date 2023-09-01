//
//                      Created by Sofie Stenberg 2023-08
//

#include <iostream>
#include <sys/stat.h>
#include <fstream>
#include <sstream>
#include <time.h>
#include <vector>
#include ".\sqlite3.h"
#include ".\PasswordManager.h"
#include ".\Ceasar.h"

std::string PasswordManager::m_databaseName;
size_t PasswordManager::m_masterHash;

// Function used to modify the private variable
void PasswordManager::setDatabaseName(std::string databaseName)
{
    PasswordManager::m_databaseName = databaseName;
}

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
    PasswordManager::m_masterHash = hashedPwd;

    PasswordManager::createMasterFile(hashedPwd);
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

    // Retrieving the current master password from the .txt file
    std::string masterPwd = "";
    std::ifstream fileMasterPwd(PasswordManager::m_databaseName + ".txt");
    std::getline(fileMasterPwd, masterPwd);
    // Need to convert string to size_t
    std::stringstream master(masterPwd);
    size_t masterPwdDatabase;
    master >> masterPwdDatabase;
    fileMasterPwd.close();

    PasswordManager::m_masterHash = masterPwdDatabase;

    std::cout << "Opened database: " << userInput + ".db" << std::endl;
}

// This function creates a file with the hash of the master password
void PasswordManager::createMasterFile(size_t hashedPwd)
{
    std::ofstream masterFile(PasswordManager::m_databaseName + ".txt");
    masterFile << hashedPwd;
    masterFile.close();
    return;
}

// A function that hashes the specified passwords (master password)
size_t PasswordManager::pwdHashing(std::string pwd)
{
    std::hash<std::string> pwdHashing;
    size_t hashedPwd = pwdHashing(pwd);
    return hashedPwd;
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

    std::string oldMasterPWD = std::to_string(PasswordManager::m_masterHash);

    // The user chooses a new master password
    std::string userInput = "";
    std::cout << "Enter new master-password: " << std::endl;
    std::cin >> userInput;
    size_t hashedPwd = PasswordManager::pwdHashing(userInput);

    PasswordManager::m_masterHash = hashedPwd;
    // The password replaces the old one in the .txt file
    std::ofstream masterFileOutput(PasswordManager::m_databaseName + ".txt", std::ofstream::trunc);
    masterFileOutput << hashedPwd;
    masterFileOutput.close();

    // Have to extract every pasword, decrypt it with the old master pwd
    // encrypt it again with the new master pwd and then update the database
    const char *filePwd = (PasswordManager::m_databaseName + ".db").c_str();
    sqlite3 *db;
    sqlite3_stmt *st;
    std::stringstream sqlCommand;
    int countOfRows = 0;
    char *messageError;
    sqlCommand << "SELECT COUNT(*) FROM Passwords";

    sqlite3_open(filePwd, &db);
    int res = sqlite3_exec(db, sqlCommand.str().c_str(), PasswordManager::callbackCount, &countOfRows, &messageError);
    if (res != SQLITE_OK)
    {
        std::cout << "Error in retrieving count: " << messageError << std::endl;
        return;
    }

    std::vector<std::string> pwdContainer;
    // To empty the stream
    sqlCommand.str(std::string());
    std::string decryptPwd = "";
    std::string encryptedPwd = "";

    // Extracts all the passwords from the database and re-decrypts them with the new master password
    sqlCommand << "SELECT password FROM Passwords";
    sqlite3_prepare_v2(db, sqlCommand.str().c_str(), -1, &st, NULL);
    while (sqlite3_step(st) != SQLITE_DONE)
    {

        std::string pwd((char *)(sqlite3_column_text(st, 0)));
        decryptPwd = Ceasar::decrypt3(pwd, oldMasterPWD);
        decryptPwd = Ceasar::decrypt3(pwd, oldMasterPWD);
        encryptedPwd = Ceasar::encrypt3(decryptPwd, std::to_string(PasswordManager::m_masterHash));
        pwdContainer.push_back(encryptedPwd);
    }
    sqlCommand.str(std::string());

    // re-open the database to prepare for the updates
    sqlite3_close(db);
    sqlite3_open(filePwd, &db);

    // Updates all the new encryptions int the database
    for (int i = 0; i < (int)pwdContainer.size(); i++)
    {
        sqlCommand << "UPDATE Passwords SET password = \"" << pwdContainer[i] << "\" WHERE id = " << (i + 1);
        int res = sqlite3_exec(db, sqlCommand.str().c_str(), NULL, 0, &messageError);
        if (res != SQLITE_OK)
        {
            std::cout << "Password insertion error:  " << messageError << std::endl;
            sqlite3_close(db);
            return;
        }
        sqlCommand.str(std::string());
    }
    sqlite3_close(db);
}

// This function controls if the user knows the master-password to the current database
bool PasswordManager::controlMasterPwd()
{
    // The user is prompted for the current master password, which is then hashed
    std::string userInput = "";
    std::cout << "What is the current master-password?" << std::endl;
    std::cin >> userInput;
    size_t oldPasswordHashUser = PasswordManager::pwdHashing(userInput);

    // Checks if the user provided password is correct
    if (oldPasswordHashUser != PasswordManager::m_masterHash)
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
    std::cout << "Enter the new username: " << std::endl;
    std::cin >> newUsername;
    std::cout << "Enter the new password: " << std::endl;
    std::cin >> newPwd;
    std::cout << "Enter a description (Optional): " << std::endl;
    std::getline(std::cin, newDesc);

    std::string encryptedPwd = Ceasar::encrypt3(newPwd, std::to_string(PasswordManager::m_masterHash));

    // Open the database file and creates the SQL statement
    const char *filePwd = (PasswordManager::m_databaseName + ".db").c_str();
    sqlite3 *db;
    sqlite3_open(filePwd, &db);
    char *messageError;
    std::stringstream sqlCommand;
    sqlCommand << "INSERT INTO Passwords (username, password, description) VALUES('"
               << newUsername
               << "', '" << encryptedPwd
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
    int id = sqlite3_column_int(st, 0);
    std::string username = reinterpret_cast<const char *>(sqlite3_column_text(st, 1));
    std::string password = Ceasar::decrypt3(reinterpret_cast<const char *>(sqlite3_column_text(st, 2)), std::to_string(PasswordManager::m_masterHash));
    std::string description = reinterpret_cast<const char *>(sqlite3_column_text(st, 3));
    std::cout << id << "\t" << username << "\t" << password << "\t" << description << std::endl;
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

// Working more with ASCII values
std::string PasswordManager::generatePwd()
{
    // This to make sure that the rand() function really is random..
    // Otherwise it spits out the same sequence every time you restart the program..
    srand(time(0));
    char ASCIIstart = '!';
    std::string generatedPwd = "";
    char ch = ' ';
    std::map<char, int> container = {{'A', 0}, {'a', 0}, {'1', 0}, {'!', 0}};
    int randomNr = 0;
    bool continuing = true;

    for (int i = 0; i < 16; i++)
    {
        randomNr = rand() % 94;
        ch = ASCIIstart + randomNr;

        // Don't want the following chars; ', <, >, `
        while ((int)ch == 39 || (int)ch == 60 || (int)ch == 62 || (int)ch == 96)
        {
            randomNr = rand() % 94;
            ch = ASCIIstart + randomNr;
        }

        if (((int)ch <= 90) && ((int)ch >= 65)) // A-Z
        {
            container['A']++;
            generatedPwd += ch;
        }

        else if (((int)ch <= 122) && ((int)ch >= 97)) // a-z
        {
            container['a']++;
            generatedPwd += ch;
        }

        else if (((int)ch <= 57) && ((int)ch >= 48)) // 0-9
        {
            container['1']++;
            generatedPwd += ch;
        }

        else
        {
            container['!']++;
            generatedPwd += ch;
        }
    }

    if (container['A'] > 0 && container['a'] > 0 && container['1'] > 0 && container['!'] > 0)
        return generatedPwd;

    int biggestInt = 0;
    char biggestChar = ' ';
    continuing = true;
    int index = 0;

    while (continuing)
    {
        if (container['A'] > 0 && container['a'] > 0 && container['1'] > 0 && container['!'] > 0)
        {
            continuing = false;
            break;
        }

        // Fetching which entry has most occurencies
        for (const auto &pair : container)
        {
            if (pair.second > biggestInt)
            {
                biggestInt = pair.second;
                biggestChar = pair.first;
            }
        }

        index = PasswordManager::replaceIndex(generatedPwd, biggestChar);
        container[biggestChar]--;

        if (container['A'] == 0)
        {
            randomNr = rand() % 26;
            generatedPwd[index] = ('A' + randomNr);
            container['A']++;
        }

        else if (container['a'] == 0)
        {
            randomNr = rand() % 26;
            generatedPwd[index] = ('a' + randomNr);
            container['a']++;
        }

        else if (container['1'] == 0)
        {
            randomNr = rand() % 10;
            generatedPwd[index] = ('0' + randomNr);
            container['1']++;
        }

        else
        {
            bool goOn = true;
            while (goOn)
            {
                randomNr = rand() % 94;
                if (((randomNr >= ('!' + 0)) && (randomNr <= ('!' + 6))) ||
                    ((randomNr >= ('!' + 7)) && (randomNr <= ('!' + 14))) ||
                    ((randomNr >= ('!' + 25)) && (randomNr <= '!' + 26)) ||
                    ((randomNr == ('!' + 28))) ||
                    ((randomNr >= ('!' + 30)) && (randomNr <= ('!' + 31))) ||
                    ((randomNr >= ('!' + 58)) && (randomNr <= ('!' + 62))) ||
                    ((randomNr >= ('!' + 90)) && (randomNr <= ('!' + 93))))
                    goOn = false;
            }

            generatedPwd[index] = (ASCIIstart + randomNr);
            container['!']++;
        }
    }
    return generatedPwd;
}

// Finds the index of the first occurrency of the biggest char (the one we want to replace)
int PasswordManager::replaceIndex(std::string generatedPwd, char biggestChar)
{
    int index = 0;
    int pwdLength = generatedPwd.length();
    if ((biggestChar == 'A') || (biggestChar == 'a'))
    {
        for (index = 0; index < pwdLength; index++)
        {
            if ((generatedPwd[index] > (biggestChar + 0)) && (generatedPwd[index] < biggestChar + 26))
                return index;
        }
    }

    else if (biggestChar == '1')
    {
        for (index = 0; index < pwdLength; index++)
        {
            if ((generatedPwd[index] > (biggestChar + 0)) && (generatedPwd[index] < biggestChar + 10))
                return index;
        }
    }

    else
    {
        for (index = 0; index < pwdLength; index++)
        {
            if (((generatedPwd[index] > (biggestChar + 0)) && (generatedPwd[index] < biggestChar + 6)) ||
                ((generatedPwd[index] > (biggestChar + 7)) && (generatedPwd[index] < biggestChar + 14)) ||
                ((generatedPwd[index] > (biggestChar + 24)) && (generatedPwd[index] < biggestChar + 27)) ||
                ((generatedPwd[index] == (biggestChar + 29))) ||
                ((generatedPwd[index] > (biggestChar + 31)) && (generatedPwd[index] < biggestChar + 33)) ||
                ((generatedPwd[index] > (biggestChar + 59)) && (generatedPwd[index] < biggestChar + 64)) ||
                ((generatedPwd[index] > (biggestChar + 91)) && (generatedPwd[index] < biggestChar + 95)))
                return index;
        }
    }

    return index;
}
