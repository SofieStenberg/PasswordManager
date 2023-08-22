#include <iostream>
#include <string>
#include ".\PasswordManager.h"

int main()
{
    // PasswordManager pwdManager;
    std::string userInput = "";
    std::cout << "Welcome to the password manager." << std::endl;

    bool continuing = true;

    while (continuing)
    {
        std::cout << "Choose a number corresponding to the action you want to perform:" << std::endl;
        std::cout << "1. Create new database" << std::endl;
        std::cout << "2. Open existing database" << std::endl;
        std::cout << "3. Change master-password" << std::endl;
        std::cout << "4. Exit the password manager" << std::endl;
        std::cin >> userInput;

        int nr = stoi(userInput);

        // std::string pwdPlain;
        switch (nr)
        {
        case 1:
        {
            PasswordManager::createDatabase();
            std::cout << "Enter master-password: " << std::endl;
            std::cin >> userInput;
            size_t hashedPwd = PasswordManager::pwdHashing(userInput);
            PasswordManager::createMasterFile(hashedPwd);
            break;
        }
        case 2:
        {
            std::cout << "2. open database\n"
                      << std::endl;
            break;
        }
        case 3:
        {
            PasswordManager::changeMasterPwd();
            break;
        }
        case 4:
        {
            continuing = false;
        }
        }
    }

    return 0;
}
