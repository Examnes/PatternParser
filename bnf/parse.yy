%{     /* PARSER */

#include "parser.hpp"
#include "scanner.hh"

#define yylex driver.m_scanner->yylex

%}

%code requires
{
  #include <iostream>
  #include "driver.hh"
  #include "location.hh"
  #include "position.hh"

}

%code provides
{
  namespace parse
  {
    // Forward declaration of the Driver class
    class Driver;

    inline void
    yyerror (const char* msg)
    {
      std::cerr << msg << std::endl;
    }
  }
}


%require "2.4"
%language "C++"
%locations
%defines
%debug
%define api.namespace {parse}
%define api.parser.class {Parser}
%parse-param {Driver &driver}
%lex-param {Driver &driver}
%define parse.error verbose

%union
{
        int* num;
        TypeExpression* type;
        FieldExpression* field;
        StructureExpression* strct;
        Expression* expr;
        AccessExpression* acc;
        RequestExpression* req;
        std::string* str;
        std::vector<Expression*>* expr_list;
        std::vector<FieldExpression*>* field_list;
        std::vector<StructureExpression*>* strct_list;
 /* YYLTYPE */
}

/* Tokens */
%token TOK_EOF 0
%token STRUCT INT CHAR SHORT LONG FLOAT DOUBLE 
%token IDENT NUMBER LPAREN RPAREN LBRACE RBRACE SEMICOLON LBRACKET RBRACKET DOT
%token ASSIGN PLUS MINUS TIMES DIVIDE MODULO
%token EQ NE LT LE GT GE
%token AND OR NOT
%token NUMBER_FLOAT NUMBER_HEX
%token REQUEST

%type <strct> decl
%type <field_list> field_list
%type <field> field
%type <acc> expr
%type <type> type
%type <str> IDENT
%type <num> NUMBER
%type <strct_list> program
%type <req> request


/* Entry point of grammar */
%start start

%%

start : program { driver.set_result(*$1); }
        ;


program : decl 
        { 
                $$ = new std::vector<StructureExpression*>();
                $$->push_back($1); 
        }
        | request SEMICOLON
        {
                driver.set_current_request($1);
        }
        | program decl  
        { 
                $$ = $1;
                $$->push_back($2); 
        }
        

decl : STRUCT IDENT LBRACE field_list RBRACE SEMICOLON { $$ = driver.push_type($2); };

field_list : field { $$ = driver.initalize_current_structure($1);}
        | field_list field
        { 
                if (!driver.contains_field($2->name()))
                { 
                        //yyerror("Duplicate field name"); 
                } 
                $$ = driver.add_field($2);
        };

field : type IDENT SEMICOLON { $$ = driver.create_field($2, $1); }
        | type IDENT LBRACKET expr RBRACKET SEMICOLON { $$ = driver.create_field($2, $1, $4); }
        | IDENT IDENT SEMICOLON 
        {
                if (!driver.contains_type(*$1)) { yyerror("Unknown type"); } 
                $$ = driver.create_field($2, $1);
        }
        | IDENT IDENT LBRACKET expr RBRACKET SEMICOLON 
        {
                if (!driver.contains_type(*$1)) { yyerror("Unknown type"); } 
                $$ = driver.create_field($2, $1, $4);
        };

type :          INT { $$ = driver.get_type("int"); }
        |       CHAR { $$ = driver.get_type("char"); }
        |       SHORT { $$ = driver.get_type("short"); }
        |       LONG { $$ = driver.get_type("long"); }
        |       FLOAT { $$ = driver.get_type("float"); }
        |       DOUBLE { $$ = driver.get_type("double"); };

expr : IDENT 
        { 
                if (!driver.contains_field(*$1)) { yyerror("Unknown field"); } 
                $$ = driver.initalize_access($1);
        }
        | expr DOT IDENT { $$ = driver.add_selector($1, $3); }
        | expr LBRACKET expr RBRACKET { $$ = driver.add_index($1, $3); }
        | expr LBRACKET NUMBER RBRACKET { $$ = driver.add_index($1,$3); }
        | NUMBER { $$ = driver.create_number($1); }
        ;

request : REQUEST IDENT DOT
        {
                if (!driver.contains_type(*$2)) { yyerror("Unknown type"); }
                $$ = driver.create_request($2);
        }
        | request expr
        {
                $$ = driver.set_request_access($1, $2);
        }
%%

namespace parse
{
    void Parser::error(const location& l, const std::string& m)
    {
        std::cerr << l << ": " << m << std::endl;
        driver._error = (driver._error == 127 ? 127 : driver._error + 1);
    }
}