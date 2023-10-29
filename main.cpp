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

int main() {
    std::vector<std::vector<int>> state = {
        {0x32, 0x88, 0x31, 0xe0},
        {0x43, 0x5a, 0x31, 0x37},
        {0xf6, 0x30, 0x98, 0x07},
        {0xa8, 0x8d, 0xa2, 0x34}
    };
    // std::vector<std::vector<int>> keys = aes::KeyExpansion(state);

    // std::cout << "Key Expansion:" << std::endl;
    // for (size_t i = 0; i < keys.size(); ++i) {
    //     std::cout << "Round " << i << ": ";
    //     printHexBlock(keys[i]);
    // }

    for (const auto& row : state) {
        for (int num : row) {
            std::cout << num << " ";
        }
        std::cout << std::endl;
    }
    std::cout << "..............." << std::endl;

    std::vector<std::vector<int>> transposedKey = aes::ShiftRows(state);

    for (const auto row : transposedKey) {
        for (int num : row) {
            std::cout << num << " ";
        }
        std::cout << std::endl;
    }

    return 0;
}
