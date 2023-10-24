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

// This is not a real attribute used in UPPAAL, but is used by us for computations involving TA parameters.
// TODO alpha can be 0 as we compute it right now (see bigTA).
//      If you set ALPHA_MAG too big, scientific notation will be used to write the final result in the .tck file.
#define ALPHA_MAG 10000
