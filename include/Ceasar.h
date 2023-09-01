#ifndef Ceasar_H
#define Ceasar_H
#include <string>
const int CONTAINER_LENGTH = 71;

class Ceasar
{
private:
    // static char m_container[CONTAINER_LENGTH];
    static std::string m_fixedPwd;

public:
    // static std::string encrypt(std::string plainText, std::string masterHash);
    static std::string encrypt3(std::string plainText, std::string masterHash);
    // static std::string decrypt(std::string decString, std::string masterHash);
    static std::string decrypt3(std::string decString, std::string masterHash);
};

#endif