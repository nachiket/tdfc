/* A Bison parser, made by GNU Bison 2.3.  */

/* Skeleton interface for Bison's Yacc-like parsers in C

   Copyright (C) 1984, 1989, 1990, 2000, 2001, 2002, 2003, 2004, 2005, 2006
   Free Software Foundation, Inc.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.  */

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
     COPY = 265,
     DONE = 266,
     ELSE = 267,
     EOS = 268,
     EQUALS = 269,
     FALSE = 270,
     GOTO = 271,
     GTE = 272,
     ID_ = 273,
     IF = 274,
     INPUT = 275,
     LEFT_SHIFT = 276,
     LOGIC_AND = 277,
     LOGIC_OR = 278,
     LTE = 279,
     NOT_EQUALS = 280,
     NUM = 281,
     NUMDBL = 282,
     OUTPUT = 283,
     PARAM = 284,
     PASS_THROUGH_EXCEPTION = 285,
     PRINTF = 286,
     RIGHT_SHIFT = 287,
     SEGMENT_R_ = 288,
     SEGMENT_RW_ = 289,
     SEGMENT_SEQ_R_ = 290,
     SEGMENT_SEQ_RW_ = 291,
     SEGMENT_SEQ_W_ = 292,
     SEGMENT_W_ = 293,
     SIGNED = 294,
     STATE = 295,
     STAY = 296,
     STRING = 297,
     TRUE = 298,
     UNSIGNED = 299,
     WIDTHOF = 300,
     EXP = 301,
     LOG = 302,
     SQRT = 303,
     FLOOR = 304
   };
#endif
/* Tokens.  */
#define ATTENTION 258
#define BITSOF 259
#define BOOLEAN 260
#define FLOAT 261
#define DOUBLE 262
#define CAT 263
#define CLOSE 264
#define COPY 265
#define DONE 266
#define ELSE 267
#define EOS 268
#define EQUALS 269
#define FALSE 270
#define GOTO 271
#define GTE 272
#define ID_ 273
#define IF 274
#define INPUT 275
#define LEFT_SHIFT 276
#define LOGIC_AND 277
#define LOGIC_OR 278
#define LTE 279
#define NOT_EQUALS 280
#define NUM 281
#define NUMDBL 282
#define OUTPUT 283
#define PARAM 284
#define PASS_THROUGH_EXCEPTION 285
#define PRINTF 286
#define RIGHT_SHIFT 287
#define SEGMENT_R_ 288
#define SEGMENT_RW_ 289
#define SEGMENT_SEQ_R_ 290
#define SEGMENT_SEQ_RW_ 291
#define SEGMENT_SEQ_W_ 292
#define SEGMENT_W_ 293
#define SIGNED 294
#define STATE 295
#define STAY 296
#define STRING 297
#define TRUE 298
#define UNSIGNED 299
#define WIDTHOF 300
#define EXP 301
#define LOG 302
#define SQRT 303
#define FLOOR 304




#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE
#line 172 "../src/parse_tdf.y"
{
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
}
/* Line 1489 of yacc.c.  */
#line 172 "parse_tdf.tab.hh"
	YYSTYPE;
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif

extern YYSTYPE yylval;

