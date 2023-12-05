#ifndef UTOTPARSER_TILEOPERATORENUM_H
#define UTOTPARSER_TILEOPERATORENUM_H


/**
 * Enumeration used to collect different methodologies for connecting tiles.
 */
enum TileOperatorEnum {
    only_one_out,
    match_inout_size,
    lparen,
    rparen,
    maybe_tile
};


TileOperatorEnum fromStrTileOperatorEnum(std::string_view str)
{
    if (str == "only_one_out")
    {
        return only_one_out;
    } else if (str == "match_inout_size")
    {
        return match_inout_size;
    } else if (str == "lparen")
    {
        return lparen;
    } else if (str == "rparen")
    {
        return rparen;
    } else
    {
        return maybe_tile;
    }
}


std::string toStrTileOperatorEnum(TileOperatorEnum tileConstructMethod)
{
    switch (tileConstructMethod)
    {
        case only_one_out:
            return "only_one_out";

        case match_inout_size:
            return "match_inout_size";

        case lparen:
            return "lparen";

        case rparen:
            return "rparen";

        default:
            return "";
    }
}


#endif //UTOTPARSER_TILEOPERATORENUM_H
