%define api.token.prefix {TKN_}

%{
/* TileOps */

void yyerror(const char* s);
int yylex();

using namespace std;

#define TTILE TATileBuffer::getInstance().tile()

%}

%code requires {
    #include <string>
    #include <vector>
    #include "TOTATileBuffer.hpp"
    #include <iostream>
}

%union {
    char* str_ptr;
    std::vector<char*>* str_list;
    Clock* clock_ptr;
    std::vector<Clock*>* clock_list;
    ClockAssignment* assignment_ptr;
    std::vector<ClockAssignment*>* assignment_list;
    ClockConstraint* constraint_ptr;
    std::vector<ClockConstraint*>* constraint_list;
    Transition* transition_ptr;
    std::vector<Transition*>* transition_list;
    Value* value_ptr;
    std::vector<Value*>* value_list;
    int int_val;
    std::vector<int>* int_list;
    double float_val;
    COP c_op;
}

%token CREATE TILE ACT BIN TRI RNG LBRACE RBRACE WITH COMMA ACCEPTING INITIAL SEMICOLON EQ LE NE LT GE GT
       INPUT OUTPUT TRANSITIONS ASSIGNMENT CLOCKS PARAMS

%token <str_ptr> SSTR
%token <int_val> IINT
%token <float_val> FFLOAT

%type <str_ptr> clock_name
%type <str_ptr> tile_name
%type <str_ptr> param
%type <assignment_ptr> assignment
%type <assignment_list> assignment_list
%type <assignment_list> opt_assignment_list
%type <constraint_ptr> constraint
%type <constraint_list> constraint_list
%type <constraint_list> opt_constraint_list
%type <transition_ptr> transition
%type <transition_list> transition_list
%type <transition_list> opt_transition_list
%type <clock_ptr> clock
%type <clock_list> clock_list
%type <clock_list> opt_clock_list
%type <value_ptr> value
%type <value_list> value_list
%type <value_list> opt_value_list
%type <c_op> cop
%type <int_list> int_list
%type <int_list> opt_int_list
%type <str_list> param_list
%type <str_list> opt_param_list

%start TO

%%

TO:
    create
    {
        // no action
    }
    ;

create:
    CREATE t_type t_def with_clause SEMICOLON
    {
        // no action
    }
    ;

t_type:
    TILE
    {
        // no action
    }
    | ACT
    {
        // no action
    }
    | BIN
    {
        // no action
    }
    | TRI
    {
        // no action
    }
    | RNG
    {
        // no action
    }
    ;

t_def:
    tile_name LBRACE IINT RBRACE
    {
        TTILE->setName($1);
        TTILE->setLocationsNum($3);
    }
    ;

tile_name:
    SSTR
    {
        $$ = $1;
    }
    ;

with_clause:
    /* empty */
    {
        // no action
    }
    | WITH LBRACE with_args_list RBRACE
    {
        // no action
    }
    ;

with_args_list:
    /* empty */
    {
        // no action
    }
    | with_arg
    {
        // no action
    }
    | with_args_list COMMA with_arg
    {
        // no action
    }
    ;

with_arg:
    ACCEPTING EQ LBRACE opt_int_list RBRACE
    {
        if (!TTILE->getAcceptingStates().empty()) {
            yyerror("Accepting states already set");
            YYABORT;
        }

        if ($4 != nullptr)
        {
            for (int i = 0; i < (int)$4->size(); i++)
                TTILE->addAcceptingState($4->at(i));
        }
    }
    | INITIAL EQ LBRACE opt_int_list RBRACE
    {
        if (!TTILE->getInitialStates().empty()) {
            yyerror("Initial states already set");
            YYABORT;
        }

        if ($4 != nullptr)
        {
            for (int i = 0; i < (int)$4->size(); i++)
                TTILE->addInitialState($4->at(i));
        }
    }
    | INPUT EQ LBRACE opt_int_list RBRACE
    {
        if (!TTILE->getInputStates().empty()) {
            yyerror("Input states already set");
            YYABORT;
        }

        if ($4 != nullptr)
        {
            for (int i = 0; i < (int)$4->size(); i++)
                TTILE->addInputState($4->at(i));
        }
    }
    | OUTPUT EQ LBRACE opt_int_list RBRACE
    {
        if (!TTILE->getOutputStates().empty()) {
            yyerror("Output states already set");
            YYABORT;
        }

        if ($4 != nullptr)
        {
            for (int i = 0; i < (int)$4->size(); i++)
                TTILE->addOutputState($4->at(i));
        }
    }
    | TRANSITIONS EQ LBRACE opt_transition_list RBRACE 
    {
        // no action
    }
    | CLOCKS EQ LBRACE opt_clock_list RBRACE
    {
        yyerror("Clocks not supported yet(we use default clocks for now)");
        YYABORT;
    }
    | PARAMS EQ LBRACE opt_param_list RBRACE
    {
        // no action
    }
    ;

param:
    SSTR
    {
        $$ = $1;
    }
    ;

param_list:
    param
    {
        $$ = new vector<char*>;
        $$->push_back($1);
    }
    | param_list COMMA param
    {
        $$ = $1;
        $$->push_back($3);
    }
    ;

opt_param_list:
    param_list
    {
        $$ = $1;
    }
    | /* empty */
    {
        $$ = nullptr;
    }
    ;

transition:
    LBRACE LBRACE IINT COMMA IINT RBRACE COMMA LBRACE opt_constraint_list RBRACE COMMA LBRACE opt_assignment_list RBRACE RBRACE
    {
        //((x,y),(),())
        $$ = new Transition($3, $5);
        if ($9 == nullptr)
        {

        }
        if ($13 == nullptr)
        {

        }
    }
    ;

transition_list:
    transition
    {
        $$ = new vector<Transition*>;
        $$->push_back($1);
    }
    | transition_list COMMA transition
    {
        // no action
        $$ = $1;
        $$->push_back($3);
    }
    ;

opt_transition_list:
    transition_list
    {
        // no action
    }
    | /* empty */
    {
        // no action
    }
    ;

constraint:
    clock cop value
    {
    }
    ;

constraint_list:
    constraint
    {
        // no action
    }
    | constraint_list COMMA constraint
    {
        // no action
    }
    ;

opt_constraint_list:
    constraint_list
    {
        // no action
    }
    | /* empty */
    {
        // no action
    }
    ;

assignment:
    clock_name ASSIGNMENT value
    {
        // no action
    }
    ;

assignment_list:
    assignment
    {
        // no action
    }
    | assignment_list COMMA assignment
    {
        // no action
    }
    ;

opt_assignment_list:
    assignment_list
    {
        $$ = $1;
    }
    | /* empty */
    {
        $$ = nullptr;
    }
    ;

clock:
    IINT COMMA clock_name
    {
        // no action
    }
    ;

clock_list:
    clock
    {
        $$ = new vector<Clock*>;
        $$->push_back($1);
    }
    | clock_list COMMA clock
    {
        $$ = $1;
        $$->push_back($3);
    }
    ;

opt_clock_list:
    clock_list
    {
        $$ = $1;
    }
    | /* empty */
    {
        $$ = nullptr;
    }
    ;

clock_name:
    SSTR
    {
        $$ = $1;
    }

value:
    IINT
    {
        $$ = new Value(ValueType::INT, nullptr, $1);
    }
    | FFLOAT
    {
        $$ = new Value(ValueType::FLOAT, nullptr, $1);
    }
    | SSTR
    {
        $$ = new Value(ValueType::PARAM, $1, 0);
    }
    ;

opt_int_list:
    int_list
    {
        $$ = $1;
    }
    |
    {
        $$ = nullptr;
    }
    ;

int_list:
    IINT
    {
        $$ = new vector<int>;
        $$->push_back($1);
    }
    | int_list COMMA IINT
    {
        $$ = $1;
        $$->push_back($3);
    }
    ;

cop:
    EQ
    {
        $$ = COP::EQ;
    }
    | LE
    {
        $$ = COP::LE;
    }
    | NE
    {
        $$ = COP::NE;
    }
    | LT
    {
        $$ = COP::LT;
    }
    | GE
    {
        $$ = COP::GE;
    }
    | GT
    {
        $$ = COP::GT;
    }
    | ASSIGNMENT
    {
        $$ = COP::ASSIGNMENT;
    }
    ;

%%

void yyerror(const char* s) {
    std::cerr << "Error: " << s << std::endl;

    // Deconstruct the tile buffer (actually delete all data stored in it)
    TATileBuffer::getInstance().destroy();
}
