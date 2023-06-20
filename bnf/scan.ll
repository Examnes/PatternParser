%{                                                            /* -*- C++ -*- */

#include "parser.hpp"
#include "scanner.hh"
#include "driver.hh"

/*  Defines some macros to update locations */


#define STEP()                                      \
  do {                                              \
    yylloc->step ();                                \
  } while (0)

#define COL(Col)				                    \
  yylloc->columns (Col)

#define LINE(Line)				                    \
  do{						                        \
    yylloc->lines (Line);		                    \
 } while (0)

#define YY_USER_ACTION				                \
  COL(yyleng);


typedef parse::Parser::token token;
typedef parse::Parser::token_type token_type;

#define yyterminate() return token::TOK_EOF

%}

%option debug
%option c++
%option noyywrap
%option never-interactive
%option yylineno
%option nounput
%option batch
%option prefix="parse"

/*
%option stack
*/

/* Abbreviations.  */

blank         [ \t]+
eol           [\n\r]+
COMMENT       [/][/].*\n 
STRUCT        struct
INT           int
FLOAT         float
DOUBLE        double
CHAR          char
SHORT         short
LONG          long
IDENT         [a-zA-Z_][a-zA-Z0-9_]*
NUMBER        [0-9]+
NUMBER_FLOAT  [0-9]+[.][0-9]+
NUMBER_HEX    [0-9]+[xX][0-9a-fA-F]+
LPAREN        "("
RPAREN        ")"
LBRACE        "{"
RBRACE        "}"
LBRACKET      "["
RBRACKET      "]"
SEMICOLON     ";"
DOT           "."
ASSIGN        "="
PLUS          "+"
MINUS         "-"
TIMES         "*"
DIVIDE        "/"
MODULO        "%"
EQ            "=="
AND           "&&"
OR            "||"
NOT           "!"
REQUEST       request

%%

 /* The rules.  */
%{
  STEP();
%}


{blank}       STEP();
{eol}         LINE(yyleng);
{COMMENT}     STEP();
{STRUCT}      return token::STRUCT;
{INT}         return token::INT;
{FLOAT}       return token::FLOAT;
{DOUBLE}      return token::DOUBLE;
{CHAR}        return token::CHAR;
{SHORT}       return token::SHORT;
{LONG}        return token::LONG;
{REQUEST}     return token::REQUEST;
{IDENT}       {
                yylval->str = new std::string(yytext, yyleng);
                return token::IDENT;
              }
{NUMBER}     {
                yylval->num = new int(std::stoi(std::string(yytext, yyleng)));
                return token::NUMBER;
              }
{NUMBER_FLOAT} {
                yylval->str = new std::string(yytext, yyleng);
                return token::NUMBER_FLOAT;
              }
{NUMBER_HEX}  {
                yylval->str = new std::string(yytext, yyleng);
                return token::NUMBER_HEX;
              }
{LPAREN}      return token::LPAREN;
{RPAREN}      return token::RPAREN;
{LBRACE}      return token::LBRACE;
{RBRACE}      return token::RBRACE;
{LBRACKET}    return token::LBRACKET;
{RBRACKET}    return token::RBRACKET;
{SEMICOLON}   return token::SEMICOLON;
{DOT}         return token::DOT;
{ASSIGN}      return token::ASSIGN;
{PLUS}        return token::PLUS;
{MINUS}       return token::MINUS;
{TIMES}       return token::TIMES;
{DIVIDE}      return token::DIVIDE;
{MODULO}      return token::MODULO;
{EQ}          return token::EQ;
{AND}         return token::AND;
{OR}          return token::OR;
{NOT}         return token::NOT;
.   {
      std::cerr << *yylloc << " Unexpected token : " << *yytext << std::endl;
      driver._error = (driver._error == 127 ? 127 : driver._error + 1);
      STEP ();
    }

%%


/*

   CUSTOM C++ CODE

*/

namespace parse
{
    Scanner::Scanner()
    : parseFlexLexer()
    {
    }

    Scanner::~Scanner()
    {
    }

    void Scanner::set_debug(bool b)
    {
        yy_flex_debug = b;
    }
}

#ifdef yylex
# undef yylex
#endif

int parseFlexLexer::yylex()
{
  std::cerr << "call parsepitFlexLexer::yylex()!" << std::endl;
  return 0;
}