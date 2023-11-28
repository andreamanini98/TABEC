#ifndef UTOTPARSER_UPPAALXMLATTRIBUTES_H
#define UTOTPARSER_UPPAALXMLATTRIBUTES_H


// This header contains macros for keywords used by UPPAAL in xml files.

const std::string ID { "@id" };
const std::string LABEL { "label" };
const std::string KIND { "@kind" };
const std::string TEXT { "#text" };
const std::string SOURCE { "source" };
const std::string REF { "@ref" };
const std::string TARGET { "target" };
const std::string ASSIGNMENT { "assignment" };
const std::string GUARD { "guard" };
const std::string NTA { "nta" };
const std::string TEMPLATE { "template" };
const std::string DECLARATION { "declaration" };
const std::string INIT { "init" };
const std::string LOCATION { "location" };
const std::string TRANSITION { "transition" };
const std::string CLOCK { "clock" };
const std::string COLOR { "@color" };
const std::string INVARIANT { "invariant" };
const std::string NAME { "name" };
const std::string COMMENTS { "comments" };


// The following are not real attribute used in UPPAAL, but are used by us for computations involving TA parameters.

// These are the names of input and output states one can use when making a tile.
const std::string IN { "in" };
const std::string OUT { "out" };

// TODO alpha can be 0 as we compute it right now (see bigTA).
//      If you set ALPHA_MAG too big, scientific notation will be used to write the final result in the .tck file.
// A scaling factor used for getting rid of decimal values. This is obtained by
// rescaling all the constants in the TA thus working with only (big) integer values.
// Consider keeping it a multiple of (10)^n: in this case, it is just a shift of the floating point over the decimal values.
#define ALPHA_MAG 10000
// This accounts for the number of digits (stating from the most significant one i.e. from the left) of ALPHA_MAG to ignore.
// This is useful since in the scripts, we're just going to append strings when resizing constants.
// Since ALPHA_MAG should be a multiple of (10)^n, a value of 1 should suffice.
// e.g. if ALPHA_MAG = 10000 then ALPHA_MAG_IGNORE_DIGITS = 1
#define ALPHA_MAG_IGNORE_DIGITS 1


#endif // UTOTPARSER_UPPAALXMLATTRIBUTES_H
