// This header contains macros for keywords used by UPPAAL in xml files.

#define ID "@id"
#define LABEL "label"
#define KIND "@kind"
#define TEXT "#text"
#define SOURCE "source"
#define REF "@ref"
#define TARGET "target"
#define ASSIGNMENT "assignment"
#define GUARD "guard"
#define NTA "nta"
#define TEMPLATE "template"
#define DECLARATION "declaration"
#define INIT "init"
#define LOCATION "location"
#define TRANSITION "transition"
#define CLOCK "clock"
#define COLOR "@color"
#define INVARIANT "invariant"


// The following are not real attribute used in UPPAAL, but are used by us for computations involving TA parameters.

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
