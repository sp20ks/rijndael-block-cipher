#include <iostream>
#include <vector>
#include <algorithm>
#include <iomanip>
#include "aes/rijndael.hpp"

void printHexBlock(const std::vector<int>& block) {
    for (int value : block) {
        std::cout << std::setw(2) << std::setfill('0') << std::hex << value << " ";
    }
    std::cout << std::endl;
}


int gmul(int a, int b) {
    if (b == 1) {
        return a;
    }
    int tmp = (a << 1) & 0xff;
    if (b == 2) {
        return (a < 128) ? tmp : tmp ^ 0x1b;
    }
    if (b == 3) {
        return gmul(a, 2) ^ a;
    }
    return 0;
}

void mixColumns(int a, int b, int c, int d) {
    std::cout << std::hex << std::setw(2) << std::setfill('0') << (aes::GaloisMul(a, 2) ^ aes::GaloisMul(b, 3) ^ aes::GaloisMul(c, 1) ^ aes::GaloisMul(d, 1)) << " ";
    std::cout << std::hex << std::setw(2) << std::setfill('0') << (aes::GaloisMul(a, 1) ^ aes::GaloisMul(b, 2) ^ aes::GaloisMul(c, 3) ^ aes::GaloisMul(d, 1)) << " ";
    std::cout << std::hex << std::setw(2) << std::setfill('0') << (aes::GaloisMul(a, 1) ^ aes::GaloisMul(b, 1) ^ aes::GaloisMul(c, 2) ^ aes::GaloisMul(d, 3)) << " ";
    std::cout << std::hex << std::setw(2) << std::setfill('0') << (aes::GaloisMul(a, 3) ^ aes::GaloisMul(b, 1) ^ aes::GaloisMul(c, 1) ^ aes::GaloisMul(d, 2)) << std::endl;
}

int main() {

    // std::vector<std::vector<int>> key = aes::StringToBlock("2b7e151628aed2a6abf7158809cf4f3c");
    // std::vector<std::vector<int>> state = {
    //     {0x32, 0x88, 0x31, 0xe0},
    //     {0x43, 0x5a, 0x31, 0x37},
    //     {0xf6, 0x30, 0x98, 0x07},
    //     {0xa8, 0x8d, 0xa2, 0x34}
    // };
    // std::vector<std::vector<int>> keys = aes::KeyExpansion(key);

    // std::cout << "Key Expansion:" << std::endl;
    // for (size_t i = 0; i < keys.size(); ++i) {
    //     printHexBlock(keys[i]);
    // }

    // std::cout << "State:" << std::endl;
    // for (const auto& row : state) {
    //     for (int num : row) {
    //         std::cout << num << " ";
    //     }
    //     std::cout << std::endl;
    // }

    // std::cout << "..............." << std::endl;
    // aes::AddRoundKey(state, keys, 0);

    // std::cout << "Encrypted State:" << std::endl;
    // for (const auto& row : state) {
    //     for (int num : row) {
    //         std::cout << std::hex << std::setw(2) << std::setfill('0') << num << " ";
    //     }
    //     std::cout << std::endl;
    // }
  

  
    std::vector<std::vector<int>> a = {{219, 19, 83, 69},
                                        {242, 10, 34, 92},
                                        {1, 1, 1, 1},
                                        {198,198,198,198},
                                        {45, 38, 49, 76},
                                        {212, 212, 212, 213},
                                        {242, 242, 242, 242}}; 
    std::vector<std::vector<int>> b = aes::MixColumns(a);

      for (const auto& row : b) {
        for (int num : row) {
            std::cout << std::hex << std::setw(2) << std::setfill('0') << num << " ";
        }
        std::cout << std::endl;
    }

    std::cout << "////////////////////////////////////////////////" << std::endl;
    for(int i = 0; i < 7; ++i)
    {
        mixColumns(a[i][0],a[i][1],a[i][2],a[i][3]);
    }


    return 0;
}



// #include <iostream>

// const int nb = 4;

// int mul_by_02(int num) {
//     if (num < 0x80) {
//         return (num << 1);
//     } else {
//         return (num << 1) ^ 0x1b;
//     }
// }

// int mul_by_03(int num) {
//     return (mul_by_02(num) ^ num);
// }

// int mul_by_09(int num) {
//     return mul_by_02(mul_by_02(mul_by_02(num))) ^ num;
// }

// int mul_by_0b(int num) {
//     return mul_by_02(mul_by_02(mul_by_02(num))) ^ mul_by_02(num) ^ num;
// }

// int mul_by_0d(int num) {
//     return mul_by_02(mul_by_02(mul_by_02(num))) ^ mul_by_02(mul_by_02(num)) ^ num;
// }

// int mul_by_0e(int num) {
//     return mul_by_02(mul_by_02(mul_by_02(num))) ^ mul_by_02(mul_by_02(num)) ^ mul_by_02(num);
// }

// void mix_columns(int state[4][nb], bool inv = false) {
//     for (int i = 0; i < nb; ++i) {
//         int s0, s1, s2, s3;
//         if (!inv) {
//             s0 = mul_by_02(state[0][i]) ^ mul_by_03(state[1][i]) ^ state[2][i] ^ state[3][i];
//             s1 = state[0][i] ^ mul_by_02(state[1][i]) ^ mul_by_03(state[2][i]) ^ state[3][i];
//             s2 = state[0][i] ^ state[1][i] ^ mul_by_02(state[2][i]) ^ mul_by_03(state[3][i]);
//             s3 = mul_by_03(state[0][i]) ^ state[1][i] ^ state[2][i] ^ mul_by_02(state[3][i]);
//         } else {
//             s0 = mul_by_0e(state[0][i]) ^ mul_by_0b(state[1][i]) ^ mul_by_0d(state[2][i]) ^ mul_by_09(state[3][i]);
//             s1 = mul_by_09(state[0][i]) ^ mul_by_0e(state[1][i]) ^ mul_by_0b(state[2][i]) ^ mul_by_0d(state[3][i]);
//             s2 = mul_by_0d(state[0][i]) ^ mul_by_09(state[1][i]) ^ mul_by_0e(state[2][i]) ^ mul_by_0b(state[3][i]);
//             s3 = mul_by_0b(state[0][i]) ^ mul_by_0d(state[1][i]) ^ mul_by_09(state[2][i]) ^ mul_by_0e(state[3][i]);
//         }
//         state[0][i] = s0;
//         state[1][i] = s1;
//         state[2][i] = s2;
//         state[3][i] = s3;
//     }
// }

// int main() {
//     // Пример использования функции mix_columns и связанных функций
//     int state[4][nb] = {
//         {0xdb, 0x13, 0x53, 0x45},
//         {0xf2, 0x0a, 0x22, 0x5c},
//         {0x2d, 0x26, 0x31, 0x4c},
//         {0xd4, 0xd4, 0xd4, 0xd5}
//     };

//     std::cout << "Original State:" << std::endl;
//     for (int i = 0; i < nb; ++i) {
//         for (int j = 0; j < nb; ++j) {
//             std::cout << std::hex << state[j][i] << " ";
//         }
//         std::cout << std::endl;
//     }

//     mix_columns(state);

//     std::cout << "\nState After mix_columns:" << std::endl;
//     for (int i = 0; i < nb; ++i) {
//         for (int j = 0; j < nb; ++j) {
//             std::cout << std::hex << state[j][i] << " ";
//         }
//         std::cout << std::endl;
//     }

//     return 0;
// }
