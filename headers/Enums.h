#ifndef UTOTPARSER_ENUMS_H
#define UTOTPARSER_ENUMS_H

// TODO YOU CAN MOVE THID IN THE TATILEFOLDER AND BEAUTIFY IT

/**
 * Enumeration used to collect different methodologies for connecting tiles.
 */
enum TileConstructMethod {
    only_one_out,
    match_inout_size
};


TileConstructMethod fromStrTileConstructMethod(std::string_view str)
{
    if (str == "only_one_out")
    {
        return only_one_out;
    } else if (str == "match_inout_size")
    {
        return match_inout_size;
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

        default:
            return "";
    }
}


#endif //UTOTPARSER_ENUMS_H
