#include <iostream>
#include <vector>
#include <algorithm>
#include <iomanip>
#include <string>
#include "aes/rijndael.hpp"
using namespace std;

int main()
{
    string text = "aaaaaaaaaaaaaaa";
    string key = "aaaaaaaaaaaaaaa";


        std::vector< std::vector< int > > state = {
    {0x00, 0x11, 0x22, 0x33},
    {0x44, 0x55, 0x66, 0x77},
    {0x88, 0x99, 0xAA, 0xBB},
    {0xCC, 0xDD, 0xEE, 0xFF}
};



    std::vector< std::vector< int > > keys ={
    {0x00, 0x01, 0x02, 0x03},
    {0x04, 0x05, 0x06, 0x07},
    {0x08, 0x09, 0x0A, 0x0B},
    {0x0C, 0x0D, 0x0E, 0x0F}
};
    string result = aes::Encrypt(aes::BlockToString(state), aes::BlockToString(keys));
    cout << "Текст до: " << text << ", ключ: " << key << endl << "Результат: " << result << endl;
    
    
    return 0;
}
