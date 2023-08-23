#include <iostream>
#include <string>
#include ".\PasswordManager.h"

int main()
{
    PasswordManager pwdManager;
    std::string userInput = "";
    std::cout << "Welcome to the password manager." << std::endl;

    bool continuing = true;

    while (continuing)
    {
        std::cout << "" << std::endl;
        std::cout << "Choose a number corresponding to the action you want to perform:" << std::endl;
        std::cout << "1. Create new database" << std::endl;
        std::cout << "2. Open existing database" << std::endl;
        std::cout << "3. Change master-password" << std::endl;
        std::cout << "4. Add new password to current database" << std::endl;
        std::cout << "5. Display all passwords" << std::endl;
        std::cout << "9. Exit the password manager\n"
                  << std::endl;
        std::cin >> userInput;

        int nr = stoi(userInput);

        // std::string pwdPlain;
        switch (nr)
        {
        case 1:
        {
            pwdManager.createDatabase();
            std::cout << "Database created" << std::endl;
            break;
        }
        case 2:
        {
            std::cout << "Choose database: " << std::endl;
            std::cin >> userInput;
            pwdManager.setDatabaseName(userInput);
            std::cout << "Opened database: " << userInput + ".db" << std::endl;
            break;
        }
        case 3:
        {
            PasswordManager::changeMasterPwd();
            std::cout << "Master password changed." << std::endl;
            break;
        }
        case 4:
        {
            PasswordManager::addNewPassword();
            break;
        }
        case 5:
        {
            std::cout << "Content of database:" << std::endl;
            PasswordManager::displayPwd();
            std::cout << "\nEnding of database." << std::endl;
            break;
        }
        case 9:
        {
            continuing = false;
            std::cout << "Exiting..." << std::endl;
            break;
        }
        }
    }

    return 0;
}
