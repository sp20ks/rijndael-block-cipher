#include <string>
#include <vector>
#include <array>
#include <algorithm>

#include "rijndael.hpp"

// std::string aes::Encrypt( std::string text, std::string key )
// {
//     std::vector<std::vector<int>> state = StringToBlock( text );

//     std::vector<std::vector<int>> keys = KeyExpansion( StringToBlock( key ) );

//     // return key;
// };

std::vector<std::vector<int>> aes::StringToBlock( std::string input )
{
    std::vector<std::vector<int>> result;
    const size_t block_size = 4;
    size_t total_bytes = input.size();
    size_t extra_bytes = total_bytes % block_size;

    if ( !extra_bytes )
    {
        input.append( block_size - extra_bytes, '\0' );
        total_bytes = input.size();
    };

    for ( size_t i = 0; i < total_bytes; i += block_size )
    {
        std::vector<int> block;
        for ( size_t j = 0; j < block_size; ++j )
        {
            block.push_back( static_cast<int> ( static_cast<unsigned char> ( input[i + j] ) ) );
        }
        result.push_back( block );
    };

    return result;
};

std::vector<int> aes::Xor( std::vector<int> array1, std::vector<int> array2 )
{
    std::vector<int> result;

    if ( array1.size() != array2.size() )
    {
        return result;
    };

    for ( size_t i = 0; i < array1.size(); ++i )
    {
        result.push_back( array1[i] ^ array2[i] );
    }

    return result;
};

int aes::GetSboxValue( int num )
{
    int row = num / 0x10; // Most significant nibble
    int col = num % 0x10; // Least significant nibble
    return sbox[row][col];
};

std::vector<int> aes::SubWord( std::vector<int> array1 )
{
    std::vector<int> result;
    for ( int i = 0; i < array1.size(); ++i )
    {
        result.push_back( GetSboxValue( array1[i] ) );
    };
    return result;
};

std::vector<int> aes::RotWord( std::vector<int> array )
{
    std::vector<int> result = array;
    std::rotate( result.begin(), result.begin() + 1, result.end() );
    return result;
};

std::vector<std::vector<int>> aes::KeyExpansion( std::vector<std::vector<int>> key )
{
    std::vector<std::vector<int>> result;
    for ( int i = 0; i < nk; ++i )
    {
        result.push_back( key[i] );
    };

    std::vector<int> buf; 
    for ( int i = nk; i < nb * ( nr + 1 ); ++i )
    {   
        buf = result[i - 1];
        if ( i % nk == 0)
        {   
            buf = RotWord( buf );
            buf = SubWord( buf );
            buf = Xor( result[i - nk], buf );
            std::vector<int> rcon_vec( rcon[i / nk], rcon[i / nk] + 4 );
            result.push_back( Xor( buf, rcon_vec ) );
        } else
        {
            buf = Xor( result[i - nk], buf );
            result.push_back ( buf );
        };
    };

    return result;
};

