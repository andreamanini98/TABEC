%define api.token.prefix {TKN_}

%{
/* TileOps */

void yyerror(const char* s);
int yylex();

using namespace std;

#define TTILE TOBuffer::getInstance().tile()

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
    State* state_ptr;
    std::vector<State*>* state_list;
    Transition* transition_ptr;
    std::vector<Transition*>* transition_list;
    Value* value_ptr;
    std::vector<Value*>* value_list;
    Param* param_ptr;
    std::vector<Param*>* param_list;
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

%type <assignment_list> opt_assignment_list
%type <assignment_list> assignment_list
%type <assignment_ptr> assignment
%type <constraint_list> opt_constraint_list
%type <constraint_list> constraint_list
%type <constraint_ptr> constraint
%type <transition_list> opt_transition_list
%type <transition_list> transition_list
%type <transition_ptr> transition
%type <clock_list> opt_clock_list
%type <clock_list> clock_list
%type <clock_ptr> clock
%type <value_list> opt_value_list
%type <value_list> value_list
%type <value_ptr> value
%type <param_list> opt_param_list
%type <param_list> param_list
%type <param_ptr> param
%type <int_list> opt_int_list
%type <int_list> int_list
%type <c_op> cop

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
    SSTR LBRACE IINT RBRACE
    {
        TTILE->setName($1);
        for (int i = 0;i < $3;i++)
        {
            State * state = new State(i);
            TTILE->addState(state);
        }
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
    PARAMS ASSIGNMENT LBRACE opt_param_list RBRACE
    {
        if ($4 != nullptr)
        {
            for (int i = 0; i < (int)$4->size(); i++)
            {
                TTILE->appendParam($4->at(i));
            }
        }
    }
    | CLOCKS ASSIGNMENT LBRACE opt_clock_list RBRACE
    {
        if ($4 != nullptr)
        {
            for (int i = 0; i < (int)$4->size(); i++)
            {
                TTILE->appendClock($4->at(i));
            }
        }
    }
    | TRANSITIONS ASSIGNMENT LBRACE opt_transition_list RBRACE 
    {
        if ($4 != nullptr)
        {
            for (int i = 0; i < (int)$4->size(); i++)
            {
                TTILE->appendTransition($4->at(i));
            }
        }
    }
    | ACCEPTING ASSIGNMENT LBRACE opt_int_list RBRACE
    {
        if ($4 != nullptr)
        {
            for (int i = 0; i < (int)$4->size(); i++)
            {
                if (!TTILE->setStateAccepting($4->at(i), true))
                {
                    yyerror("Invalid state index while setting accepting states");
                    YYABORT;
                }
            }
        }
    }
    | INITIAL ASSIGNMENT LBRACE opt_int_list RBRACE
    {
        if ($4 != nullptr)
        {
            for (int i = 0; i < (int)$4->size(); i++)
            {
                if(!TTILE->setStateInitial($4->at(i), true))
                {
                    yyerror("Invalid state index while setting initial state");
                    YYABORT;
                }
            }
        }
    }
    | INPUT ASSIGNMENT LBRACE opt_int_list RBRACE
    {
        if ($4 != nullptr)
        {
            for (int i = 0; i < (int)$4->size(); i++)
            {
                if(!TTILE->setStateInput($4->at(i), true))
                {
                    yyerror("Invalid state index while setting input states");
                    YYABORT;
                }
            }
        }
    }
    | OUTPUT ASSIGNMENT LBRACE opt_int_list RBRACE
    {
        if ($4 != nullptr)
        {
            for (int i = 0; i < (int)$4->size(); i++)
            {
                if(!TTILE->setStateOutput($4->at(i), true))
                {
                    yyerror("Invalid state index while setting output states");
                    YYABORT;
                }
            }
        }
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

param_list:
    param
    {
        $$ = new vector<Param*>;
        $$->push_back($1);
    }
    | param_list COMMA param
    {
        $$ = $1;
        $$->push_back($3);
    }
    ;

param:
    SSTR
    {
        $$ = new Param($1);
    }
    ;

opt_transition_list:
    transition_list
    {
        $$ = $1;
    }
    | /* empty */
    {
        $$ = nullptr;
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

transition:
    LBRACE LBRACE IINT COMMA IINT RBRACE COMMA LBRACE opt_constraint_list RBRACE COMMA LBRACE opt_assignment_list RBRACE RBRACE
    {
        //((from,to),(guard1,guard2,...),(action1,action2,...))
        $$ = new Transition($3, $5);

        if ($9 != nullptr)
        {
            for (int i = 0; i < (int)$9->size(); i++)
            {
                $$->addGuard($9->at(i));
            }
        }

        if ($13 != nullptr)
        {
            for (int i = 0; i < (int)$13->size(); i++)
            {
                $$->addAction($13->at(i));
            }
        }
    }
    ;

opt_constraint_list:
    constraint_list
    {
        $$ = $1;
    }
    | /* empty */
    {
        $$ = nullptr;
    }
    ;

constraint_list:
    constraint
    {
        $$ = new vector<ClockConstraint*>;
        $$->push_back($1);
    }
    | constraint_list COMMA constraint
    {
        $$ = $1;
        $$->push_back($3);
    }
    ;

constraint:
    SSTR cop value
    {
        $$ = new ClockConstraint($1, $2, $3);
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

assignment_list:
    assignment
    {
        $$ = new vector<ClockAssignment*>;
        $$->push_back($1);
    }
    | assignment_list COMMA assignment
    {
        $$ = $1;
        $$->push_back($3);
    }
    ;

assignment:
    SSTR ASSIGNMENT value
    {
        $$ = new ClockAssignment($1, $3);
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

clock:
    SSTR
    {
        // no action
        $$ = new Clock($1);
    }
    ;

opt_value_list:
    value_list
    {
        $$ = $1;
    }
    | /* empty */
    {
        $$ = nullptr;
    }
    ;

value_list:
    value
    {
        $$ = new vector<Value*>;
        $$->push_back($1);
    }
    | value_list COMMA value
    {
        $$ = $1;
        $$->push_back($3);
    }
    ;

value:
    IINT
    {
        $$ = new Value(ValueType::INT, static_cast<void*>(&$1));
    }
    | FFLOAT
    {
        $$ = new Value(ValueType::DOUBLE, static_cast<void*>(&$1));
    }
    | SSTR
    {
        $$ = new Value(ValueType::PARAM, static_cast<void*>($1));
    }
    ;

opt_int_list:
    int_list
    {
        $$ = $1;
    }
    | /* empty */
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
    // TOBuffer::getInstance().destroy();
}
