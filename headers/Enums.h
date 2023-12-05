#ifndef UTOTPARSER_ENUMS_H
#define UTOTPARSER_ENUMS_H

// TODO YOU HAVE TO MODIFY THIS AND LET THE USER BE ABLE TO EXPAND IT EASILY

/**
 * Enumeration used to collect different methodologies for connecting tiles.
 */
enum TileConstructMethod {
    only_one_out,
    match_inout_size,
    lparen,
    rparen,
    maybe_tile
};


TileConstructMethod fromStrTileConstructMethod(std::string_view str)
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


std::string toStrTileConstructMethod(TileConstructMethod tileConstructMethod)
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


#endif //UTOTPARSER_ENUMS_H
