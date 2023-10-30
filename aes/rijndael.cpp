#include <string>
#include <vector>
#include <array>
#include <algorithm>
#include <iostream>

#include "rijndael.hpp"

std::string aes::Encrypt( std::string text, std::string key )
{
    std::vector<std::vector<int>> state = StringToBlock( text );

    std::vector<std::vector<int>> keys = KeyExpansion( StringToBlock( key ) );
    
    std::string a;
    return a;
};

std::vector< std::vector< int > > aes::StringToBlock( std::string input )
{
    std::vector< std::vector< int > > result;
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

std::vector< int > aes::Xor( std::vector< int > array1, std::vector<int> array2 )
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

std::vector< int > aes::SubBytes( std::vector< int > array1 )
{
    std::vector< int > result;
    for ( int i = 0; i < array1.size(); ++i )
    {
        result.push_back( GetSboxValue( array1[i] ) );
    };
    return result;
};

std::vector< int > aes::RotWord( std::vector< int > array, int shift )
{
    std::vector<int> result = array;
    std::rotate( result.begin(), result.begin() + shift, result.end() );
    return result;
};

std::vector< std::vector< int > > aes::KeyExpansion( std::vector< std::vector< int > > key )
{
    std::vector< std::vector< int > > result;
    for ( int i = 0; i < nk; ++i )
    {
        result.push_back( key[i] );
    };

    std::vector< int > buf; 
    for ( int i = nk; i < nb * ( nr + 1 ); ++i )
    {   
        buf = result[i - 1];
        if ( i % nk == 0)
        {   
            buf = RotWord( buf, 1 );
            buf = SubBytes( buf );
            buf = Xor( result[i - nk], buf );
            std::vector< int > rcon_vec( rcon[i / nk], rcon[i / nk] + 4 );
            result.push_back( Xor( buf, rcon_vec ) );
        } else
        {
            buf = Xor( result[i - nk], buf );
            result.push_back ( buf );
        };
    };

    return result;
};

std::vector< std::vector< int > > aes::Transpose( std::vector< std::vector< int > > matrix )
{
    int rows = matrix.size();
    int cols = matrix[0].size();

    std::vector< std::vector< int > > transposed( cols, std::vector<int>( rows, 0 ) );

    for ( int i = 0; i < rows; ++i )
    {
        for ( int j = 0; j < cols; ++j )
        {
            transposed[j][i] = matrix[i][j];
        };
    };

    return transposed;
};

std::vector< std::vector< int > > aes::ShiftRows( std::vector< std::vector< int > > matrix )
{
    std::vector< std::vector< int > > result = Transpose( matrix );

    for ( int i = 0; i < 4; ++i )
    {
        result[i] = RotWord( result[i], i );
    };

    return Transpose( result );
};

void aes::AddRoundKey(std::vector< std::vector< int > > & state, const std::vector< std::vector< int > > & round_keys, int round )
{
    for ( size_t i = 0; i < state.size(); ++i )
    {
        for ( size_t j = 0; j < state[i].size(); ++j )
        {
            state[i][j] ^= round_keys[round * nb + i][j];
        };
    };
};

int aes::GaloisMul( int a, int b )
{
    if ( b == 1 )
    {
        return a;
    }
    int tmp = ( a << 1 ) & 0xff;
    if ( b == 2 )
    {
        return ( a < 128 ) ? tmp : tmp ^ 0x1b;
    }
    if ( b == 3 )
    {
        return GaloisMul( a, 2 ) ^ a;
    }
    return 0;
};

std::vector< std::vector< int > >  aes::MixColumns( std::vector< std::vector< int > > & state )
{
    std::vector< std::vector< int > > tmp( state.size(), std::vector< int >( state[0].size(), 0 ) );
   for ( size_t c = 0; c < state.size(); ++c )
    {
        tmp[c][0] = GaloisMul( state[c][0], 2 ) ^ GaloisMul( state[c][1], 3 ) ^ GaloisMul( state[c][2], 1 ) ^ GaloisMul( state[c][3], 1 );
        tmp[c][1] = GaloisMul( state[c][0], 1 ) ^ GaloisMul( state[c][1], 2 ) ^ GaloisMul( state[c][2], 3 ) ^ GaloisMul( state[c][3], 1 );
        tmp[c][2] = GaloisMul( state[c][0], 1 ) ^ GaloisMul( state[c][1], 1 ) ^ GaloisMul( state[c][2], 2 ) ^ GaloisMul( state[c][3], 3 );
        tmp[c][3] = GaloisMul( state[c][0], 3 ) ^ GaloisMul( state[c][1], 1 ) ^ GaloisMul( state[c][2], 1 ) ^ GaloisMul( state[c][3], 2 );
    }

    return tmp;
}