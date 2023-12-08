#ifndef UTOTPARSER_TILETOKENSENUM_H
#define UTOTPARSER_TILETOKENSENUM_H

#include <string_view>


/**
 * Enumeration containing an enum representation of the tokens used in the parser.
 * The choice of using an enumeration in addition to the string representation of the tokens (given in the lexer) is
 * that, since factory methods have been used, with an enumeration the switch construct can be used and it can make optimizations
 * like creating a jump table, avoiding to test each if-else case.
 */
enum TileTokensEnum {
    only_one_out,
    match_inout_size,
    tree_op,
    lparen,
    rparen,
    t_barabasi_albert,
    maybe_tile
};


// TODO Maybe find another way to handle this.
TileTokensEnum fromStrTileTokenEnum(std::string_view str)
{
    if (str == "only_one_out")
    {
        return only_one_out;
    } else if (str == "match_inout_size")
    {
        return match_inout_size;
    } else if (str == "tree_op")
    {
        return tree_op;
    } else if (str == "lparen")
    {
        return lparen;
    } else if (str == "rparen")
    {
        return rparen;
    } else if (str == "t_barabasi_albert")
    {
        return t_barabasi_albert;
    } else
    {
        return maybe_tile;
    }
}


#endif //UTOTPARSER_TILETOKENSENUM_H
