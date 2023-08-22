#ifndef PASSWORDMANAGER_H
#define PASSWORDMANAGER_H
#include <string>

class PasswordManager
{
private:
public:
    static std::string m_masterPWD;
    static void createDatabase();
    static size_t pwdHashing(std::string pwd);
    static void createMasterFile(size_t hashedPwd);
};

#endif