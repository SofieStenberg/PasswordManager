#ifndef PASSWORDMANAGER_H
#define PASSWORDMANAGER_H
#include <string>

class PasswordManager
{
private:
    static std::string m_databaseName;

public:
    // static std::string m_masterPWD; // NEJ!!!!

    static void createDatabase();
    static size_t pwdHashing(std::string pwd);
    static void createMasterFile(size_t hashedPwd);
    static void changeMasterPwd();
};

#endif