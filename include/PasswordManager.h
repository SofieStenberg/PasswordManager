#ifndef PASSWORDMANAGER_H
#define PASSWORDMANAGER_H
#include <string>

class PasswordManager
{
private:
    static std::string m_databaseName;

public:
    static void setDatabaseName(std::string databaseName);
    static void createDatabase();
    static size_t pwdHashing(std::string pwd);
    static void createMasterFile(size_t hashedPwd);
    static void changeMasterPwd();
    static bool controlMasterPwd();
    static void addNewPassword();
    static void displayPwd();
    static int callback(void *data, int argc, char **argv, char **colName);
};

#endif