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
     CAT = 261,
     CLOSE = 262,
     COPY = 263,
     DONE = 264,
     ELSE = 265,
     EOS = 266,
     EQUALS = 267,
     FALSE = 268,
     GOTO = 269,
     GTE = 270,
     ID_ = 271,
     IF = 272,
     INPUT = 273,
     LEFT_SHIFT = 274,
     LOGIC_AND = 275,
     LOGIC_OR = 276,
     LTE = 277,
     NOT_EQUALS = 278,
     NUM = 279,
     OUTPUT = 280,
     PARAM = 281,
     PASS_THROUGH_EXCEPTION = 282,
     PRINTF = 283,
     RIGHT_SHIFT = 284,
     SEGMENT_R_ = 285,
     SEGMENT_RW_ = 286,
     SEGMENT_SEQ_R_ = 287,
     SEGMENT_SEQ_RW_ = 288,
     SEGMENT_SEQ_W_ = 289,
     SEGMENT_W_ = 290,
     SIGNED = 291,
     STATE = 292,
     STAY = 293,
     STRING = 294,
     TRUE = 295,
     UNSIGNED = 296,
     WIDTHOF = 297
   };
#endif
/* Tokens.  */
#define ATTENTION 258
#define BITSOF 259
#define BOOLEAN 260
#define CAT 261
#define CLOSE 262
#define COPY 263
#define DONE 264
#define ELSE 265
#define EOS 266
#define EQUALS 267
#define FALSE 268
#define GOTO 269
#define GTE 270
#define ID_ 271
#define IF 272
#define INPUT 273
#define LEFT_SHIFT 274
#define LOGIC_AND 275
#define LOGIC_OR 276
#define LTE 277
#define NOT_EQUALS 278
#define NUM 279
#define OUTPUT 280
#define PARAM 281
#define PASS_THROUGH_EXCEPTION 282
#define PRINTF 283
#define RIGHT_SHIFT 284
#define SEGMENT_R_ 285
#define SEGMENT_RW_ 286
#define SEGMENT_SEQ_R_ 287
#define SEGMENT_SEQ_RW_ 288
#define SEGMENT_SEQ_W_ 289
#define SEGMENT_W_ 290
#define SIGNED 291
#define STATE 292
#define STAY 293
#define STRING 294
#define TRUE 295
#define UNSIGNED 296
#define WIDTHOF 297




#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE
#line 170 "../src/parse_tdf.y"
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
#line 158 "parse_tdf.tab.hh"
	YYSTYPE;
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif

extern YYSTYPE yylval;

