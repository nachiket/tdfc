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
     OUTPUT = 282,
     PARAM = 283,
     PASS_THROUGH_EXCEPTION = 284,
     PRINTF = 285,
     RIGHT_SHIFT = 286,
     SEGMENT_R_ = 287,
     SEGMENT_RW_ = 288,
     SEGMENT_SEQ_R_ = 289,
     SEGMENT_SEQ_RW_ = 290,
     SEGMENT_SEQ_W_ = 291,
     SEGMENT_W_ = 292,
     SIGNED = 293,
     STATE = 294,
     STAY = 295,
     STRING = 296,
     TRUE = 297,
     UNSIGNED = 298,
     WIDTHOF = 299,
     EXP = 300,
     LOG = 301,
     SQRT = 302
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
#define OUTPUT 282
#define PARAM 283
#define PASS_THROUGH_EXCEPTION 284
#define PRINTF 285
#define RIGHT_SHIFT 286
#define SEGMENT_R_ 287
#define SEGMENT_RW_ 288
#define SEGMENT_SEQ_R_ 289
#define SEGMENT_SEQ_RW_ 290
#define SEGMENT_SEQ_W_ 291
#define SEGMENT_W_ 292
#define SIGNED 293
#define STATE 294
#define STAY 295
#define STRING 296
#define TRUE 297
#define UNSIGNED 298
#define WIDTHOF 299
#define EXP 300
#define LOG 301
#define SQRT 302




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
#line 168 "parse_tdf.tab.hh"
	YYSTYPE;
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif

extern YYSTYPE yylval;

