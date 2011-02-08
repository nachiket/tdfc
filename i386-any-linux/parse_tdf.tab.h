
/* A Bison parser, made by GNU Bison 2.4.1.  */

/* Skeleton interface for Bison's Yacc-like parsers in C
   
      Copyright (C) 1984, 1989, 1990, 2000, 2001, 2002, 2003, 2004, 2005, 2006
   Free Software Foundation, Inc.
   
   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.
   
   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
   
   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.
   
   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */


/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum yytokentype {
     ATTENTION = 258,
     BITSOF = 259,
     BOOLEAN = 260,
     FLOAT = 261,
     DOUBLE = 262,
     CAT = 263,
     CLOSE = 264,
     FRAMECLOSE = 265,
     COPY = 266,
     DONE = 267,
     ELSE = 268,
     EOS = 269,
     EOFR = 270,
     EQUALS = 271,
     FALSE = 272,
     GOTO = 273,
     GTE = 274,
     ID_ = 275,
     IF = 276,
     INPUT = 277,
     LEFT_SHIFT = 278,
     LOGIC_AND = 279,
     LOGIC_OR = 280,
     LTE = 281,
     NOT_EQUALS = 282,
     NUM = 283,
     NUMDBL = 284,
     OUTPUT = 285,
     PARAM = 286,
     PASS_THROUGH_EXCEPTION = 287,
     PRINTF = 288,
     RIGHT_SHIFT = 289,
     SEGMENT_R_ = 290,
     SEGMENT_RW_ = 291,
     SEGMENT_SEQ_R_ = 292,
     SEGMENT_SEQ_RW_ = 293,
     SEGMENT_SEQ_W_ = 294,
     SEGMENT_W_ = 295,
     SIGNED = 296,
     STATE = 297,
     STAY = 298,
     STRING = 299,
     TRUE = 300,
     UNSIGNED = 301,
     WIDTHOF = 302,
     EXP = 303,
     LOG = 304,
     SQRT = 305,
     FLOOR = 306,
     ABS = 307
   };
#endif



#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE
{

/* Line 1676 of yacc.c  */
#line 172 "../src/parse_tdf.y"

  Token 	*token;
  Expr		*expr;
  Stmt		*stmt;
  StateCase	*stateCase;
//State		*state;
  State		*states;	// $$=start state
  Operator	*operatr;
  Symbol	*sym;
  SymTab	*symtab;
  Type		*type;
  InputSpec	*inputSpec;
  OpBody	*opBody;
  OpSignature	*opSignature;
  TypeDeclElem	*typeDeclElem;
  list<Expr*>			*exprs, *args;
  list<Stmt*>			*stmts;
  list<InputSpec*>		*inputSpecs;
  list<Symbol*>			*symList;
  list<TypeDeclElem*>		*typeDeclElems;
//dictionary<string,Symbol*>	*symDict;
//dictionary<string,State*>	*states;
  Suite				*suite;



/* Line 1676 of yacc.c  */
#line 131 "parse_tdf.tab.hh"
} YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
#endif

extern YYSTYPE yylval;


