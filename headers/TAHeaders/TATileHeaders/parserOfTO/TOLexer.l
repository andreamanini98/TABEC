%option noyywrap
%option case-insensitive

%{
#include "TOTATileBuffer.hpp"
#include "TOParser.hpp"
#include <iostream>
#include <cstdlib>
#include <cstring>
#include <vector>

using namespace std;

#define RETURN_TOKEN(tok) return TKN_##tok

extern YYSTYPE yylval;  // using bison YYSTYPE
extern int yyparse();
// extern yy::parser::semantic_type yylval;

%}

WHITE_SAPCE      [\ \t\b\f]
DIGIT            [0-9]+
DOT              \.

%%
{WHITE_SAPCE}                                           { /* ignore whitespace */ }
";"                                                     { RETURN_TOKEN(SEMICOLON); }
","                                                     { RETURN_TOKEN(COMMA); }
"="                                                     { RETURN_TOKEN(EQ); }
":="                                                    { RETURN_TOKEN(ASSIGNMENT); }
"=="                                                    { RETURN_TOKEN(ASSIGNMENT); }
"<="                                                    { RETURN_TOKEN(LE); }
"<>" | "!="                                             { RETURN_TOKEN(NE); }
"<"                                                     { RETURN_TOKEN(LT); }
">="                                                    { RETURN_TOKEN(GE); }
">"                                                     { RETURN_TOKEN(GT); }
"CREATE"                                                { RETURN_TOKEN(CREATE); }
"TILE"                                                  { RETURN_TOKEN(TILE); }
"ACT"                                                   { RETURN_TOKEN(ACT); }
"BIN"                                                   { RETURN_TOKEN(BIN); }
"TRI"                                                   { RETURN_TOKEN(TRI); }
"RNG"                                                   { RETURN_TOKEN(RNG); }
"WITH"                                                  { RETURN_TOKEN(WITH); }
"ACCEPTING"                                             { RETURN_TOKEN(ACCEPTING); }
"INITIAL"                                               { RETURN_TOKEN(INITIAL); }
"INPUT"                                                 { RETURN_TOKEN(INPUT); }
"OUTPUT"                                                { RETURN_TOKEN(OUTPUT); }
"TRANSITIONS"                                           { RETURN_TOKEN(TRANSITIONS); }
"CLOCKS"                                                { RETURN_TOKEN(CLOCKS); }
"PARAMS"                                                { RETURN_TOKEN(PARAMS); }
"("                                                     { RETURN_TOKEN(LBRACE); }
")"                                                     { RETURN_TOKEN(RBRACE); }
[\-]?{DIGIT}+{DOT}{DIGIT}+                              { yylval.float_val = atof(yytext); RETURN_TOKEN(FFLOAT); }
[\-]?{DIGIT}+                                           { yylval.int_val = atoi(yytext); RETURN_TOKEN(IINT); }
[A-Za-z0-9_]*                                           { yylval.str_ptr = strdup_new(yytext); RETURN_TOKEN(SSTR); }

%%
