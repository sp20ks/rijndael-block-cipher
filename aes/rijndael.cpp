#include <string>
#include <vector>
#include <algorithm>
#include <iostream>
#include "rijndael.hpp"

std::string aes::Encrypt( std::string text, std::string key )
{
    std::vector< std::vector< int > > state = StringToBlock( text );
    std::vector< std::vector< int > > keys = KeyExpansion( StringToBlock( key ) );

    AddRoundKey( state, keys, 0 );
    for ( size_t round = 1; round < nr; ++round )
    {
        for ( size_t i = 0; i < state.size(); ++i )
        {
            state[i] = SubBytes( state[i] );
        };
        state = ShiftRows( state );
        state = MixColumns( state );
        AddRoundKey( state, keys, round );
    }

    // Final Round (without MixColumns)
    for ( size_t i = 0; i < state.size(); ++i )
    {
        state[i] = SubBytes( state[i] );
    };
    state = ShiftRows( state );
    AddRoundKey( state, keys, nr );
    return BlockToString( state );
};

std::string aes::Decrypt( std::string text, std::string key )
{
    std::vector< std::vector< int > > state = StringToBlock( text );
    std::vector< std::vector< int > > keys = KeyExpansion( StringToBlock( key ) );

    // Initial Round (without MixColumns)
    AddRoundKey( state, keys, nr );
    state = ShiftRows( state, true );
    for ( size_t i = 0; i < state.size(); ++i )
    {
        state[i] = SubBytes( state[i], true );
    };
    // Main Rounds
    for ( int round = nr - 1; round > 0; --round )
    {
        AddRoundKey( state, keys, round) ;
        state = MixColumns( state, true );
        state = ShiftRows( state, true );
        for ( size_t i = 0; i < state.size(); ++i )
        {
            state[i] = SubBytes( state[i], true );
        }
    }

    AddRoundKey( state, keys, 0 );
    return BlockToString( state );
};

std::vector< std::vector< int > > aes::StringToBlock( std::string input )
{
    std::vector< std::vector< int > > result;
    size_t total_bytes = input.size();

    if ( total_bytes < nb )
    {
        input.append( nb - total_bytes, '\0' );
        total_bytes = nb;
    };

    for ( size_t i = 0; i < total_bytes; i += nb )
    {
        std::vector< int > block;
        for ( size_t j = 0; j < nb; ++j )
        {
            block.push_back( static_cast< int > ( static_cast< unsigned char > ( input[i + j] ) ) );
        }
        result.push_back( block );
    };

    return result;
}

std::vector< int > aes::Xor( std::vector< int > array1, std::vector< int > array2 )
{
    std::vector< int > result;

    if ( array1.size() != array2.size() )
    {
        return result;
    };

    for ( size_t i = 0; i < array1.size(); ++i )
    {
        result.push_back( array1[i] ^ array2[i] );
    };

    return result;
};

int aes::GetSboxValue( int num, bool inv )
{
    int row = num / 0x10; // Most significant nibble
    int col = num % 0x10; // Least significant nibble

    return inv? InvSBox[row][col] : SBox[row][col];
};

std::vector< int > aes::SubBytes( std::vector< int > array1, bool inv )
{
    std::vector< int > result;
    for ( size_t i = 0; i < array1.size(); ++i )
    {
        result.push_back( GetSboxValue( array1[i], inv ) );
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
    for ( size_t i = 0; i < nk; ++i )
    {
        result.push_back( key[i] );
    };

    std::vector< int > buf; 
    for ( size_t i = nk; i < nb * ( nr + 1 ); ++i )
    {   
        buf = result[i - 1];
        if ( i % nk == 0)
        {   
            buf = RotWord( buf, 1 );
            buf = SubBytes( buf );
            buf = Xor( result[i - nk], buf );
            std::vector< int > RCon_vec( RCon[i / nk], RCon[i / nk] + 4 );
            result.push_back( Xor( buf, RCon_vec ) );
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

    for ( size_t i = 0; i < rows; ++i )
    {
        for ( int j = 0; j < cols; ++j )
        {
            transposed[j][i] = matrix[i][j];
        };
    };

    return transposed;
};

std::vector< std::vector< int > > aes::ShiftRows( std::vector< std::vector< int > > matrix, bool inv)
{
    std::vector< std::vector< int > > result = Transpose( matrix );

    if ( inv )
    {
        for ( size_t i = 0; i < 4; ++i )
        {
            result[i] = RotWord( result[i], result[i].size() - i );
        };
    } else
    {
        for ( size_t i = 0; i < 4; ++i )
        {
            result[i] = RotWord( result[i], i );
        };
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
    if ( b == 2 )
    {
        int tmp = ( a << 1 ) & 0xff;
        return ( a < 128 ) ? tmp : tmp ^ 0x1b;
    }
    if ( b == 3 )
    {
        return GaloisMul( a, 2 ) ^ a;
    }
    if ( b == 9 )
    {
        return GaloisMul( GaloisMul( GaloisMul( a, 2 ), 2 ), 2 ) ^ a;
    }
    if ( b == 11 )
    {
        return GaloisMul( GaloisMul( GaloisMul( a, 2 ), 2 ), 2 ) ^ GaloisMul( a, 2 ) ^ a;
    }
    if ( b == 14 )
    {
        return GaloisMul( GaloisMul( GaloisMul( a, 2 ), 2 ), 2 ) ^ GaloisMul( GaloisMul( a, 2 ), 2 ) ^ a;
    }
    if ( b == 15 )
    {
       return GaloisMul( GaloisMul( GaloisMul( a, 2 ), 2 ), 2 ) ^ GaloisMul( GaloisMul( a, 2 ), 2 ) ^ GaloisMul( a, 2 );
    }
    return 0;

    // if ( a * b == 0 )
    // {
    //     return 0;
    // };

    // int result = 0;
    // while ( b > 0 )
    // {
    //     if ( b & 1 )
    //     {
    //         result ^= a;
    //     };

    //     a = ( a << 1 ) ^ ( ( a & 0x80 ) ? 0x1b : 0 );
    //     b >>= 1;
    // };

    // return result;

};

std::vector< std::vector< int > >  aes::MixColumns( std::vector< std::vector< int > > state, bool inv )
{
    std::vector< std::vector< int > > tmp( state.size(), std::vector< int >( state[0].size(), 0 ) );
    if ( inv )
    {
        for ( size_t c = 0; c < state.size(); ++c )
        {
            tmp[c][0] = ( GaloisMul( state[c][0], 15 ) ^ GaloisMul( state[c][1], 11 ) ^ GaloisMul( state[c][2], 14 ) ^ GaloisMul( state[c][3], 9 ) );
            tmp[c][1] = ( GaloisMul( state[c][0], 9 ) ^ GaloisMul( state[c][1], 15 ) ^ GaloisMul( state[c][2], 11 ) ^ GaloisMul( state[c][3], 14 ) );
            tmp[c][2] = ( GaloisMul( state[c][0], 14 ) ^ GaloisMul( state[c][1], 9 ) ^ GaloisMul( state[c][2], 15 ) ^ GaloisMul( state[c][3], 11 ) );
            tmp[c][3] = ( GaloisMul( state[c][0], 11 ) ^ GaloisMul( state[c][1], 14 ) ^ GaloisMul( state[c][2], 9 ) ^ GaloisMul( state[c][3], 15 ) );
        }
    } else
    {
        for ( size_t c = 0; c < state.size(); ++c )
        {
            tmp[c][0] = GaloisMul( state[c][0], 2 ) ^ GaloisMul( state[c][1], 3 ) ^ GaloisMul( state[c][2], 1 ) ^ GaloisMul( state[c][3], 1 );
            tmp[c][1] = GaloisMul( state[c][0], 1 ) ^ GaloisMul( state[c][1], 2 ) ^ GaloisMul( state[c][2], 3 ) ^ GaloisMul( state[c][3], 1 );
            tmp[c][2] = GaloisMul( state[c][0], 1 ) ^ GaloisMul( state[c][1], 1 ) ^ GaloisMul( state[c][2], 2 ) ^ GaloisMul( state[c][3], 3 );
            tmp[c][3] = GaloisMul( state[c][0], 3 ) ^ GaloisMul( state[c][1], 1 ) ^ GaloisMul( state[c][2], 1 ) ^ GaloisMul( state[c][3], 2 );
        };
    };

    return tmp;
};

std::string aes::BlockToString( const std::vector< std::vector< int > > & block )
{
    std::string result;
    for ( const auto & row : block )
    {
        for ( int num : row )
        {
            result.push_back( static_cast< char >( num ) );
        }
    }
    return result;
};
