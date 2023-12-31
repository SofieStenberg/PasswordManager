//
//                      Created by Sofie Stenberg 2023-08
//

#include <iostream>
#include <string>
#include ".\PasswordManager.h"
#include ".\Ceasar.h"

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
        std::cout << "5. Display all entries (except for the password)" << std::endl;
        std::cout << "6. Choose entry to display the corresponding password" << std::endl;
        std::cout << "7. Generate a password" << std::endl;
        std::cout << "8. Exit the password manager\n"
                  << std::endl;
        std::cin >> userInput;
        int nr = 0;

        try
        {
            nr = stoi(userInput);
        }
        catch (...)
        {
            std::cout << "Invalid input..." << std::endl;
            continue;
        }
        if (nr < 1 || nr > 8)
        {
            std::cout << "Invalid number..." << std::endl;
            continue;
        }

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
            PasswordManager::openExistingDatabase();
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
            PasswordManager::displayEntries();
            std::cout << "\nEnding of database." << std::endl;
            break;
        }
        case 6:
        {
            PasswordManager::displayPwd();
            break;
        }
        case 7:
        {
            std::string generatedPwd = PasswordManager::generatePwd();
            std::cout << "The generated password is: " << generatedPwd << std::endl;
            break;
        }
        case 8:
        {
            continuing = false;
            std::cout << "Exiting..." << std::endl;
            break;
        }
        }
    }

    return 0;
}
