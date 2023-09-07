//
//                      Created by Sofie Stenberg 2023-08
//

#include <algorithm>
#include <iostream>
#include ".\Ceasar.h"

std::string Ceasar::m_fixedPwd = "f%5DT/6f2c8!&fdz-G?54gd7";

std::string Ceasar::encrypt3(std::string plainText, std::string masterHash)
{
    int masterLength = masterHash.length();
    int fixedLength = Ceasar::m_fixedPwd.length();
    int plainLength = plainText.length();
    std::string encryptedPwd = "";
    char ASCIIstart = '!';

    for (int i = 0; i < plainLength; i++)
    {
        int seed = (int)masterHash[i%masterLength] * ((int)Ceasar::m_fixedPwd[(i + 1) % masterLength]);
        srand(seed);
        int fixedIndex = rand() % fixedLength;

        int encryptedIndex = ((int)plainText[i] - ASCIIstart + (int)Ceasar::m_fixedPwd[fixedIndex] - ASCIIstart) % 94;
        encryptedPwd += (ASCIIstart + encryptedIndex);
    }
    return encryptedPwd;
}

std::string Ceasar::decrypt3(std::string encPwd, std::string masterHash)
{
    int masterLength = masterHash.length();
    int fixedLength = Ceasar::m_fixedPwd.length();
    int encLength = encPwd.length();
    std::string decodedPwd = "";
    char ASCIIstart = '!';

    for (int i = 0; i < encLength; i++)
    {
        int seed = (int)masterHash[i%masterLength] * ((int)Ceasar::m_fixedPwd[(i + 1) % masterLength]);
        srand(seed);
        int fixedIndex = rand() % fixedLength;

        int decodedIndex = (94 + (((int)encPwd[i] - ASCIIstart) - ((int)Ceasar::m_fixedPwd[fixedIndex] - ASCIIstart))) % 94;
        decodedPwd += (ASCIIstart + decodedIndex);
    }
    return decodedPwd;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// // Initializes the array to be used when replacing the plaintext passwords
// char Ceasar::m_container[CONTAINER_LENGTH] = {'A', 'a', 'B', 'b', 'C', 'c', 'D', 'd', 'E', 'e', 'F', 'f',
//                                               'G', 'g', 'H', 'h', 'I', 'i', 'J', 'j', 'K', 'k', 'L', 'l',
//                                               'M', 'm', 'N', 'n', 'O', 'o', 'P', 'p', 'Q', 'q', 'R', 'r',
//                                               'S', 's', 'T', 't', 'U', 'u', 'V', 'v', 'W', 'w', 'X', 'x',
//                                               'Y', 'y', 'Z', 'z',
//                                               '0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
//                                               '!', '-', '_', '&', '@', '/', '?', '%', '#'};

// // Function that encrypts the provided password
// std::string Ceasar::encrypt(std::string plainText, std::string masterHash)
// {
//     std::string encodedPwd = "";
//     int indexPlain = 0;
//     int indexFixed = 0;
//     int indexMaster = 0;
//     int offsetMaster = 0;
//     int offsetFixed = 0;
//     int masterLength = masterHash.length();
//     int fixedLength = Ceasar::m_fixedPwd.length();

//     for (int i = 0; i < (int)plainText.length(); i++)
//     {
//         if (indexMaster >= (int)masterHash.length())
//             indexMaster = 0;
//         if (indexFixed >= (int)Ceasar::m_fixedPwd.length())
//             indexFixed = 0;

//         // Using find() to locate the current character in the string to be encoded
//         // Using distance() to extract the index of located character in the m_container
//         indexPlain = std::distance(m_container, (std::find(m_container, m_container + CONTAINER_LENGTH, plainText[i])));
//         // Starting at the index retrieved earlier, stepping forward ASCII decimal representation of
//         // current char in the master password. All this modulus the length of the container to
//         // not exceed the containers length. Returning the resulted index value.
//         offsetMaster = (indexPlain + ((int)masterHash[i % masterLength] % CONTAINER_LENGTH)) % CONTAINER_LENGTH;
//         // Same as above, but starting from the index calculated above and using the fixed password
//         // included in this Ceasar class
//         offsetFixed = (offsetMaster + ((int)Ceasar::m_fixedPwd[i % fixedLength] % CONTAINER_LENGTH)) % CONTAINER_LENGTH;
//         // Adding the final calculated character to the final password string
//         encodedPwd += m_container[offsetFixed];

//         indexMaster++;
//         indexFixed++;
//     }

//     return encodedPwd;
// }

// // Function that decrypts the provided password
// std::string Ceasar::decrypt(std::string encString, std::string masterHash)
// {
//     std::cout << "Decrypting... " << std::endl;
//     std::string decodedPwd = "";
//     int indexEncoded = 0;
//     int indexFixed = 0;
//     int indexMaster = 0;
//     int offsetMaster = 0;
//     int offsetFixed = 0;
//     int fixedLength = Ceasar::m_fixedPwd.length();
//     int masterLength = masterHash.length();

//     for (int i = 0; i < (int)encString.length(); i++)
//     {
//         std::cout << "i: " << i << std::endl;

//         indexEncoded = std::distance(m_container, (std::find(m_container, m_container + CONTAINER_LENGTH, encString[i])));
//         offsetFixed = (CONTAINER_LENGTH + (indexEncoded - (int)Ceasar::m_fixedPwd[i % fixedLength]) % CONTAINER_LENGTH) % CONTAINER_LENGTH;
//         offsetMaster = (CONTAINER_LENGTH + (offsetFixed - (int)masterHash[i % masterLength]) % CONTAINER_LENGTH) % CONTAINER_LENGTH;
//         decodedPwd += m_container[offsetMaster];

//         indexMaster++;
//         indexFixed++;
//     }

//     return decodedPwd;
// }
