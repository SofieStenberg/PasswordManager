#include <iostream>
#include <string>
#include ".\PasswordManager.h"

int main()
{
    std::string userInput = "";
    std::cout << "Welcome to the password manager. Choose a number corresponding to the action you want to perform:\n"
              << std::endl;
    std::cout << "1. Create new database\n"
              << std::endl;
    std::cout << "2. Open existing database\n"
              << std::endl;
    std::cout << "3. Change master-password\n"
              << std::endl;
    std::cin >> userInput;

    int nr = stoi(userInput);
    std::hash<std::string> pwdHashing;
    // std::string pwdPlain;
    switch (nr)
    {
    case 1:
        std::cout << "1. Creating database" << std::endl;
        PasswordManager::createDatabase();
        std::cout << "Enter master-password: " << std::endl;
        std::cin >> userInput;
        size_t hashedPwd = pwdHashing(userInput);
        PasswordManager::createMasterFile(hashedPwd);
        break;
    case 2:
        std::cout << "2. open database\n"
                  << std::endl;
        break;
    case 3:
        std::cout << "3. Change master-password\n"
                  << std::endl;
        break;
    }

    return 0;
}
