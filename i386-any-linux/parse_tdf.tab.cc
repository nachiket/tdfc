/* A Bison parser, made by GNU Bison 2.3.  */

/* Skeleton implementation for Bison's Yacc-like parsers in C

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

/* C LALR(1) parser skeleton written by Richard Stallman, by
   simplifying the original so-called "semantic" parser.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Identify Bison output.  */
#define YYBISON 1

/* Bison version.  */
#define YYBISON_VERSION "2.3"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 0

/* Using locations.  */
#define YYLSP_NEEDED 0



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




/* Copy the first part of user declarations.  */
#line 1 "../src/parse_tdf.y"

//////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 1999 The Regents of the University of California 
// Permission to use, copy, modify, and distribute this software and
// its documentation for any purpose, without fee, and without a
// written agreement is hereby granted, provided that the above copyright 
// notice and this paragraph and the following two paragraphs appear in
// all copies. 
//
// IN NO EVENT SHALL THE UNIVERSITY OF CALIFORNIA BE LIABLE TO ANY PARTY FOR
// DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL DAMAGES, INCLUDING
// LOST PROFITS, ARISING OUT OF THE USE OF THIS SOFTWARE AND ITS DOCUMENTATION,
// EVEN IF THE UNIVERSITY OF CALIFORNIA HAS BEEN ADVISED OF THE POSSIBILITY OF
// SUCH DAMAGE. 
//
// THE UNIVERSITY OF CALIFORNIA SPECIFICALLY DISCLAIMS ANY WARRANTIES,
// INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY
// AND FITNESS FOR A PARTICULAR PURPOSE. THE SOFTWARE PROVIDED HEREUNDER IS ON
// AN "AS IS" BASIS, AND THE UNIVERSITY OF CALIFORNIA HAS NO OBLIGATIONS TO
// PROVIDE MAINTENANCE, SUPPORT, UPDATES, ENHANCEMENTS, OR MODIFICATIONS. 
//
//////////////////////////////////////////////////////////////////////////////
//
// BRASS source file
//
// SCORE TDF compiler:  TDF parser using bison
// $Revision: 1.143 $
//
//////////////////////////////////////////////////////////////////////////////


#include <assert.h>

#include <LEDA/core/string.h>
#include <LEDA/core/list.h>
#include <LEDA/core/dictionary.h>

#include "parse.h"
#include "type.h"
#include "symbol.h"
#include "suite.h"
#include "operator.h"
#include "state.h"
#include "stmt.h"
#include "expr.h"
#include "misc.h"
#include "annotes.h"
#include "gc.h"

#ifdef __APPLE__
#define strtoll strtoq
#endif


////////////////////////////////////////////////////////////////
//  classes (local)

// - auxiliary info for parsing type decls:
class TypeDeclElem
{
public:
  Token *id; Expr *val; Expr *depth;
  TypeDeclElem (Token *id_i, Expr *val_i, Expr *depth_i)
    : id(id_i), val(val_i), depth(depth_i) {}
};


////////////////////////////////////////////////////////////////
//  globals

string	*gFileName = new string("<null>");  // file being parsed
int	 gLineNum  = 0;			    // line being parsed (by lex)
bool	 gParsing  = false;		    // true during parse (by parseFile)
Suite	*gSuite	   = NULL;		    // filled with operators by parser

// - auxiliary info for objects being parsed:
static	SymTab				*gSymtab = NULL;
static	dictionary<string,State*>	*gStates = NULL;


////////////////////////////////////////////////////////////////
//  yyerror, yywarn

void yyerror (const char *msg)
{
  extern char *yytext;
  fatal(1, string(msg), new Token(0,gLineNum,gFileName,yytext));
}

void yywarn (const char *msg)
{
  extern char *yytext;
  warn(string(msg), new Token(0,gLineNum,gFileName,yytext));
}


////////////////////////////////////////////////////////////////
//  symbol table management

SymTab* symtabPush (SymTab *s)
{
  // - push new child symtab into gSymtab
  assert(s);
  if (gSymtab)
    gSymtab->addChild(s);
  gSymtab=s;
  return gSymtab;
}

SymTab* symtabPop ()
{
  // - pop a symtab from gSymtab
  assert(gSymtab);
  gSymtab=gSymtab->getScope();
  return gSymtab;
}

Symbol* lookupDuplicateDecl (Token *t)
{
  // - return duplicate or overriden type-decl for given name
  // - yyerror on duplicate, warn on overriden type-decl
  Symbol *s=NULL;
  if (gSymtab && (s=gSymtab->lookup(t->str)))
  {
    if (s->getScope()==gSymtab)
      yyerror((const char*)(string("redefinition of \"") + t->str + "\""));
    else
      {
	Token *st=s->getToken();
	yywarn ((const char*)(string("definition of \"") + t->str +
			      string("\" overrides previous definition") +
			      (st ? (string(" from ")+*st->file+":"+
				     string("%d",st->line)) : string()) ));
      }
  }
  return s;
}

Symbol* lookupTry (Token *t)
{
  // - return visible symbol of given name (NULL if none visible)
  Symbol *s=NULL;
  if (gSymtab && (s=gSymtab->lookup(t->str)))
    return s;
  else
    return NULL;
}

Symbol* lookup (Token *t)
{
  // - return visible symbol of given name (yyerror if none visible)
  Symbol *s=NULL;
  if (gSymtab && (s=gSymtab->lookup(t->str)))
    return s;
  else
    if (gParsing)
      yyerror((const char*)(string("symbol \"") + t->str +
			    string("\" not found")));
    else
      fatal(1, string("symbol \"")+t->str+"\" not found", t);
      // - may be called from ExprLValue::link() after parse,
      //   when gFileName/gLineNum are not valid (so use token)
  return NULL;
}

////////////////////////////////////////////////////////////////


/* Enabling traces.  */
#ifndef YYDEBUG
# define YYDEBUG 1
#endif

/* Enabling verbose error messages.  */
#ifdef YYERROR_VERBOSE
# undef YYERROR_VERBOSE
# define YYERROR_VERBOSE 1
#else
# define YYERROR_VERBOSE 0
#endif

/* Enabling the token table.  */
#ifndef YYTOKEN_TABLE
# define YYTOKEN_TABLE 0
#endif

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
/* Line 187 of yacc.c.  */
#line 374 "parse_tdf.tab.cc"
	YYSTYPE;
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif



/* Copy the second part of user declarations.  */


/* Line 216 of yacc.c.  */
#line 387 "parse_tdf.tab.cc"

#ifdef short
# undef short
#endif

#ifdef YYTYPE_UINT8
typedef YYTYPE_UINT8 yytype_uint8;
#else
typedef unsigned char yytype_uint8;
#endif

#ifdef YYTYPE_INT8
typedef YYTYPE_INT8 yytype_int8;
#elif (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
typedef signed char yytype_int8;
#else
typedef short int yytype_int8;
#endif

#ifdef YYTYPE_UINT16
typedef YYTYPE_UINT16 yytype_uint16;
#else
typedef unsigned short int yytype_uint16;
#endif

#ifdef YYTYPE_INT16
typedef YYTYPE_INT16 yytype_int16;
#else
typedef short int yytype_int16;
#endif

#ifndef YYSIZE_T
# ifdef __SIZE_TYPE__
#  define YYSIZE_T __SIZE_TYPE__
# elif defined size_t
#  define YYSIZE_T size_t
# elif ! defined YYSIZE_T && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# else
#  define YYSIZE_T unsigned int
# endif
#endif

#define YYSIZE_MAXIMUM ((YYSIZE_T) -1)

#ifndef YY_
# if YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> /* INFRINGES ON USER NAME SPACE */
#   define YY_(msgid) dgettext ("bison-runtime", msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(msgid) msgid
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YYUSE(e) ((void) (e))
#else
# define YYUSE(e) /* empty */
#endif

/* Identity function, used to suppress warnings about constant conditions.  */
#ifndef lint
# define YYID(n) (n)
#else
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static int
YYID (int i)
#else
static int
YYID (i)
    int i;
#endif
{
  return i;
}
#endif

#if ! defined yyoverflow || YYERROR_VERBOSE

/* The parser invokes alloca or malloc; define the necessary symbols.  */

# ifdef YYSTACK_USE_ALLOCA
#  if YYSTACK_USE_ALLOCA
#   ifdef __GNUC__
#    define YYSTACK_ALLOC __builtin_alloca
#   elif defined __BUILTIN_VA_ARG_INCR
#    include <alloca.h> /* INFRINGES ON USER NAME SPACE */
#   elif defined _AIX
#    define YYSTACK_ALLOC __alloca
#   elif defined _MSC_VER
#    include <malloc.h> /* INFRINGES ON USER NAME SPACE */
#    define alloca _alloca
#   else
#    define YYSTACK_ALLOC alloca
#    if ! defined _ALLOCA_H && ! defined _STDLIB_H && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
#     include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#     ifndef _STDLIB_H
#      define _STDLIB_H 1
#     endif
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's `empty if-body' warning.  */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (YYID (0))
#  ifndef YYSTACK_ALLOC_MAXIMUM
    /* The OS might guarantee only one guard page at the bottom of the stack,
       and a page size can be as small as 4096 bytes.  So we cannot safely
       invoke alloca (N) if N exceeds 4096.  Use a slightly smaller number
       to allow for a few compiler-allocated temporary stack slots.  */
#   define YYSTACK_ALLOC_MAXIMUM 4032 /* reasonable circa 2006 */
#  endif
# else
#  define YYSTACK_ALLOC YYMALLOC
#  define YYSTACK_FREE YYFREE
#  ifndef YYSTACK_ALLOC_MAXIMUM
#   define YYSTACK_ALLOC_MAXIMUM YYSIZE_MAXIMUM
#  endif
#  if (defined __cplusplus && ! defined _STDLIB_H \
       && ! ((defined YYMALLOC || defined malloc) \
	     && (defined YYFREE || defined free)))
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   ifndef _STDLIB_H
#    define _STDLIB_H 1
#   endif
#  endif
#  ifndef YYMALLOC
#   define YYMALLOC malloc
#   if ! defined malloc && ! defined _STDLIB_H && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
void *malloc (YYSIZE_T); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifndef YYFREE
#   define YYFREE free
#   if ! defined free && ! defined _STDLIB_H && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
void free (void *); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
# endif
#endif /* ! defined yyoverflow || YYERROR_VERBOSE */


#if (! defined yyoverflow \
     && (! defined __cplusplus \
	 || (defined YYSTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  yytype_int16 yyss;
  YYSTYPE yyvs;
  };

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (sizeof (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (sizeof (yytype_int16) + sizeof (YYSTYPE)) \
      + YYSTACK_GAP_MAXIMUM)

/* Copy COUNT objects from FROM to TO.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined __GNUC__ && 1 < __GNUC__
#   define YYCOPY(To, From, Count) \
      __builtin_memcpy (To, From, (Count) * sizeof (*(From)))
#  else
#   define YYCOPY(To, From, Count)		\
      do					\
	{					\
	  YYSIZE_T yyi;				\
	  for (yyi = 0; yyi < (Count); yyi++)	\
	    (To)[yyi] = (From)[yyi];		\
	}					\
      while (YYID (0))
#  endif
# endif

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack)					\
    do									\
      {									\
	YYSIZE_T yynewbytes;						\
	YYCOPY (&yyptr->Stack, Stack, yysize);				\
	Stack = &yyptr->Stack;						\
	yynewbytes = yystacksize * sizeof (*Stack) + YYSTACK_GAP_MAXIMUM; \
	yyptr += yynewbytes / sizeof (*yyptr);				\
      }									\
    while (YYID (0))

#endif

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  13
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   341

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  69
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  77
/* YYNRULES -- Number of rules.  */
#define YYNRULES  174
/* YYNRULES -- Number of states.  */
#define YYNSTATES  307

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   297

#define YYTRANSLATE(YYX)						\
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[YYLEX] -- Bison symbol number corresponding to YYLEX.  */
static const yytype_uint8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,    54,     2,    56,     2,    57,    59,     2,
      42,    44,    60,    52,    66,    51,    67,    61,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,    65,    64,
      49,    62,    50,    68,    55,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,    47,     2,    48,    58,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,    45,    63,    46,    53,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     1,     2,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      35,    36,    37,    38,    39,    40,    41,    43
};

#if YYDEBUG
/* YYPRHS[YYN] -- Index of the first RHS symbol of rule number YYN in
   YYRHS.  */
static const yytype_uint16 yyprhs[] =
{
       0,     0,     3,     5,     7,     8,    11,    13,    14,    20,
      22,    23,    25,    28,    33,    39,    41,    43,    47,    52,
      54,    55,    59,    61,    65,    67,    69,    71,    73,    74,
      76,    77,    83,    84,    90,    94,    96,    97,   100,   101,
     104,   108,   110,   112,   116,   118,   119,   124,   125,   131,
     134,   136,   138,   140,   143,   148,   153,   156,   159,   161,
     169,   171,   172,   176,   178,   180,   184,   189,   191,   192,
     195,   197,   199,   201,   203,   205,   213,   217,   220,   223,
     226,   229,   231,   237,   245,   250,   255,   260,   264,   270,
     272,   274,   276,   278,   283,   288,   293,   298,   303,   308,
     313,   318,   320,   321,   324,   325,   329,   331,   335,   337,
     342,   349,   357,   367,   369,   373,   375,   376,   380,   382,
     388,   390,   394,   396,   400,   402,   406,   408,   412,   414,
     418,   420,   424,   426,   430,   432,   436,   438,   442,   444,
     448,   450,   453,   458,   463,   465,   469,   471,   473,   477,
     479,   481,   483,   485,   487,   491,   496,   501,   506,   508,
     510,   512,   514,   516,   518,   520,   522,   524,   526,   528,
     530,   532,   534,   536,   538
};

/* YYRHS -- A `-1'-separated list of the rules' RHS.  */
static const yytype_int16 yyrhs[] =
{
      70,     0,    -1,    71,    -1,    72,    -1,    -1,    72,    73,
      -1,    73,    -1,    -1,    75,    79,    74,    86,    84,    -1,
      76,    -1,    -1,     5,    -1,     5,    78,    -1,    77,    47,
     122,    48,    -1,    77,    78,    47,   122,    48,    -1,    36,
      -1,    41,    -1,    47,   122,    48,    -1,    16,    42,    80,
      44,    -1,    81,    -1,    -1,    81,    66,    82,    -1,    82,
      -1,    83,    76,    16,    -1,    18,    -1,    25,    -1,    26,
      -1,    85,    -1,    -1,    27,    -1,    -1,    45,    89,    87,
     100,    46,    -1,    -1,    45,    89,    88,    97,    46,    -1,
      45,    89,    46,    -1,    90,    -1,    -1,    90,    92,    -1,
      -1,    91,    92,    -1,    76,    93,    64,    -1,    64,    -1,
      94,    -1,    93,    66,    94,    -1,    16,    -1,    -1,    16,
      95,    62,   122,    -1,    -1,    16,    96,    42,   122,    44,
      -1,    97,    98,    -1,    99,    -1,    99,    -1,    64,    -1,
     116,    64,    -1,   121,    62,   116,    64,    -1,   121,    62,
     121,    64,    -1,   113,    64,    -1,   100,   101,    -1,   101,
      -1,    37,    16,    42,   102,    44,    65,   105,    -1,   103,
      -1,    -1,   103,    66,   104,    -1,   104,    -1,    16,    -1,
      16,    56,   122,    -1,    11,    42,    16,    44,    -1,   106,
      -1,    -1,   106,   107,    -1,   108,    -1,   108,    -1,    64,
      -1,   109,    -1,   110,    -1,    17,    42,   122,    44,   109,
      10,   109,    -1,    14,    16,    64,    -1,    38,    64,    -1,
     120,    64,    -1,   112,    64,    -1,   116,    64,    -1,   111,
      -1,    17,    42,   122,    44,   107,    -1,    17,    42,   122,
      44,   109,    10,   110,    -1,    45,    89,   105,    46,    -1,
       3,    42,   122,    44,    -1,     7,    42,    16,    44,    -1,
       9,    42,    44,    -1,    28,    42,    39,   118,    44,    -1,
     114,    -1,   115,    -1,   114,    -1,   115,    -1,     8,    42,
     119,    44,    -1,    30,    42,   119,    44,    -1,    35,    42,
     119,    44,    -1,    31,    42,   119,    44,    -1,    32,    42,
     119,    44,    -1,    34,    42,   119,    44,    -1,    33,    42,
     119,    44,    -1,    16,    42,   117,    44,    -1,   119,    -1,
      -1,    66,   119,    -1,    -1,   119,    66,   122,    -1,   122,
      -1,   121,    62,   122,    -1,    16,    -1,    16,    47,   122,
      48,    -1,    16,    47,   122,    65,   122,    48,    -1,    16,
      47,   122,    48,    47,   122,    48,    -1,    16,    47,   122,
      48,    47,   122,    65,   122,    48,    -1,   125,    -1,    45,
     123,    46,    -1,   124,    -1,    -1,   124,    66,   125,    -1,
     125,    -1,   126,    68,   125,    65,   125,    -1,   126,    -1,
     126,    21,   127,    -1,   127,    -1,   127,    20,   128,    -1,
     128,    -1,   128,    63,   129,    -1,   129,    -1,   129,    58,
     130,    -1,   130,    -1,   130,    59,   131,    -1,   131,    -1,
     131,   140,   132,    -1,   132,    -1,   132,   141,   133,    -1,
     133,    -1,   133,   142,   134,    -1,   134,    -1,   134,   143,
     135,    -1,   135,    -1,   135,   144,   136,    -1,   136,    -1,
     145,   137,    -1,    42,    76,    44,   137,    -1,    42,    77,
      44,   137,    -1,   137,    -1,   138,    67,   138,    -1,   138,
      -1,   121,    -1,   121,    55,   138,    -1,    24,    -1,    40,
      -1,    13,    -1,   116,    -1,   139,    -1,    42,   122,    44,
      -1,     6,    42,   119,    44,    -1,    43,    42,   122,    44,
      -1,     4,    42,   122,    44,    -1,    12,    -1,    23,    -1,
      49,    -1,    50,    -1,    22,    -1,    15,    -1,    19,    -1,
      29,    -1,    52,    -1,    51,    -1,    60,    -1,    61,    -1,
      57,    -1,    52,    -1,    51,    -1,    54,    -1,    53,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const yytype_uint16 yyrline[] =
{
       0,   220,   220,   225,   228,   240,   245,   260,   259,   300,
     303,   307,   309,   311,   333,   362,   364,   369,   388,   393,
     396,   403,   409,   419,   431,   433,   435,   440,   443,   447,
     453,   452,   464,   463,   472,   481,   484,   492,   495,   495,
     504,   518,   523,   524,   529,   534,   533,   539,   538,   545,
     547,   552,   554,   559,   561,   563,   565,   574,   579,   588,
     606,   609,   613,   615,   620,   622,   624,   629,   632,   636,
     638,   643,   645,   650,   652,   657,   659,   661,   663,   665,
     667,   669,   674,   676,   681,   690,   698,   707,   715,   723,
     725,   730,   732,   737,   745,   754,   763,   772,   781,   790,
     802,   807,   810,   818,   821,   825,   827,   832,   837,   850,
     858,   860,   862,   867,   869,   877,   880,   884,   886,   891,
     893,   898,   900,   905,   907,   912,   914,   919,   921,   926,
     928,   933,   935,   940,   942,   947,   949,   954,   956,   961,
     963,   968,   970,   972,   974,   979,  1007,  1012,  1014,  1027,
    1040,  1042,  1044,  1046,  1048,  1053,  1064,  1070,  1079,  1080,
    1084,  1085,  1086,  1087,  1091,  1092,  1096,  1097,  1101,  1102,
    1103,  1107,  1108,  1109,  1110
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || YYTOKEN_TABLE
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "ATTENTION", "BITSOF", "BOOLEAN", "CAT",
  "CLOSE", "COPY", "DONE", "ELSE", "EOS", "EQUALS", "FALSE", "GOTO", "GTE",
  "ID_", "IF", "INPUT", "LEFT_SHIFT", "LOGIC_AND", "LOGIC_OR", "LTE",
  "NOT_EQUALS", "NUM", "OUTPUT", "PARAM", "PASS_THROUGH_EXCEPTION",
  "PRINTF", "RIGHT_SHIFT", "SEGMENT_R_", "SEGMENT_RW_", "SEGMENT_SEQ_R_",
  "SEGMENT_SEQ_RW_", "SEGMENT_SEQ_W_", "SEGMENT_W_", "SIGNED", "STATE",
  "STAY", "STRING", "TRUE", "UNSIGNED", "'('", "WIDTHOF", "')'", "'{'",
  "'}'", "'['", "']'", "'<'", "'>'", "'-'", "'+'", "'~'", "'!'", "'@'",
  "'#'", "'%'", "'^'", "'&'", "'*'", "'/'", "'='", "'|'", "';'", "':'",
  "','", "'.'", "'?'", "$accept", "start", "operators_opt", "operators",
  "operator", "@1", "type_opt", "type", "sizedType", "arraySize",
  "opSignature", "ioDecls_opt", "ioDecls", "ioDecl", "ioKind",
  "exception_opt", "exception", "operatorBody", "@2", "@3",
  "typeDecls_opt", "typeDecls", "@4", "typeDecl", "typeDeclElems",
  "typeDeclElem", "@5", "@6", "callsOrAssigns", "callOrAssign",
  "callOrAssign_nonEmpty", "states", "stateCase", "stateVars_opt",
  "stateVars", "stateVar", "stmts_opt", "stmts", "stmt", "stmt_nonEmpty",
  "matchedStmt", "unmatchedStmt", "stmtBlock", "builtinStmtBehav",
  "builtinStmtCompose", "copyStmt", "segmentStmt", "call", "args_opt",
  "more_args_opt", "args", "assign", "lvalue", "expr", "condExprs_opt",
  "condExprs", "condExpr", "logOrExpr", "logAndExpr", "bitOrExpr",
  "bitXorExpr", "bitAndExpr", "equalExpr", "inequalExpr", "shiftExpr",
  "addExpr", "prodExpr", "unaryExpr", "fixedExpr", "atomExpr",
  "builtinExpr", "equalOp", "inequalOp", "shiftOp", "addOp", "prodOp",
  "unaryOp", 0
};
#endif

# ifdef YYPRINT
/* YYTOKNUM[YYLEX-NUM] -- Internal token number corresponding to
   token YYLEX-NUM.  */
static const yytype_uint16 yytoknum[] =
{
       0,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,   268,   269,   270,   271,   272,   273,   274,
     275,   276,   277,   278,   279,   280,   281,   282,   283,   284,
     285,   286,   287,   288,   289,   290,   291,   292,   293,   294,
     295,   296,    40,   297,    41,   123,   125,    91,    93,    60,
      62,    45,    43,   126,    33,    64,    35,    37,    94,    38,
      42,    47,    61,   124,    59,    58,    44,    46,    63
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint8 yyr1[] =
{
       0,    69,    70,    71,    71,    72,    72,    74,    73,    75,
      75,    76,    76,    76,    76,    77,    77,    78,    79,    80,
      80,    81,    81,    82,    83,    83,    83,    84,    84,    85,
      87,    86,    88,    86,    86,    89,    89,    90,    91,    90,
      92,    92,    93,    93,    94,    95,    94,    96,    94,    97,
      97,    98,    98,    99,    99,    99,    99,   100,   100,   101,
     102,   102,   103,   103,   104,   104,   104,   105,   105,   106,
     106,   107,   107,   108,   108,   109,   109,   109,   109,   109,
     109,   109,   110,   110,   111,   112,   112,   112,   112,   112,
     112,   113,   113,   114,   115,   115,   115,   115,   115,   115,
     116,   117,   117,   118,   118,   119,   119,   120,   121,   121,
     121,   121,   121,   122,   122,   123,   123,   124,   124,   125,
     125,   126,   126,   127,   127,   128,   128,   129,   129,   130,
     130,   131,   131,   132,   132,   133,   133,   134,   134,   135,
     135,   136,   136,   136,   136,   137,   137,   138,   138,   138,
     138,   138,   138,   138,   138,   139,   139,   139,   140,   140,
     141,   141,   141,   141,   142,   142,   143,   143,   144,   144,
     144,   145,   145,   145,   145
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const yytype_uint8 yyr2[] =
{
       0,     2,     1,     1,     0,     2,     1,     0,     5,     1,
       0,     1,     2,     4,     5,     1,     1,     3,     4,     1,
       0,     3,     1,     3,     1,     1,     1,     1,     0,     1,
       0,     5,     0,     5,     3,     1,     0,     2,     0,     2,
       3,     1,     1,     3,     1,     0,     4,     0,     5,     2,
       1,     1,     1,     2,     4,     4,     2,     2,     1,     7,
       1,     0,     3,     1,     1,     3,     4,     1,     0,     2,
       1,     1,     1,     1,     1,     7,     3,     2,     2,     2,
       2,     1,     5,     7,     4,     4,     4,     3,     5,     1,
       1,     1,     1,     4,     4,     4,     4,     4,     4,     4,
       4,     1,     0,     2,     0,     3,     1,     3,     1,     4,
       6,     7,     9,     1,     3,     1,     0,     3,     1,     5,
       1,     3,     1,     3,     1,     3,     1,     3,     1,     3,
       1,     3,     1,     3,     1,     3,     1,     3,     1,     3,
       1,     2,     4,     4,     1,     3,     1,     1,     3,     1,
       1,     1,     1,     1,     3,     4,     4,     4,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1
};

/* YYDEFACT[STATE-NAME] -- Default rule to reduce with in state
   STATE-NUM when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const yytype_uint8 yydefact[] =
{
       4,    11,    15,    16,     0,     2,     3,     6,     0,     9,
       0,     0,    12,     1,     5,     0,     7,     0,     0,     0,
       0,   151,   108,   149,   150,     0,     0,   116,   172,   171,
     174,   173,   152,   147,     0,   113,   120,   122,   124,   126,
     128,   130,   132,   134,   136,   138,   140,   144,   146,   153,
       0,    20,     0,     0,     0,     0,     0,   102,     0,     0,
       0,     0,     0,     0,   115,   118,     0,    17,     0,     0,
       0,     0,     0,     0,   158,   159,     0,   163,   162,   160,
     161,     0,   164,   165,     0,   167,   166,     0,   170,   168,
     169,     0,     0,     0,   141,    24,    25,    26,     0,    19,
      22,     0,    36,    28,    13,     0,     0,     0,   106,     0,
     101,     0,     0,     0,   154,     0,   114,     0,   148,   121,
       0,   123,   125,   127,   129,   131,   133,   135,   137,   139,
     145,    18,     0,     0,    32,    35,     0,    29,     8,    27,
      14,   157,   155,     0,   100,   109,     0,   142,   143,   156,
     117,     0,    21,    23,    34,     0,     0,    41,     0,    37,
      39,   105,     0,     0,   119,     0,     0,    58,     0,     0,
       0,     0,     0,     0,     0,     0,    50,     0,    91,    92,
       0,     0,    44,     0,    42,     0,   110,     0,    31,    57,
       0,     0,     0,     0,     0,     0,     0,    33,    52,    49,
      51,    56,    53,     0,     0,     0,    40,     0,   111,     0,
      61,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    43,     0,     0,    64,     0,    60,    63,    93,
      94,    96,    97,    99,    98,    95,    54,    55,    46,     0,
     112,     0,     0,     0,     0,    48,     0,    65,    68,    62,
      66,     0,     0,     0,     0,     0,     0,     0,    36,    59,
      67,    70,    73,    74,    81,     0,    89,    90,     0,     0,
       0,     0,     0,     0,     0,     0,     0,    77,    68,    72,
      69,    71,    79,    80,    78,     0,     0,     0,    87,    76,
       0,   104,     0,   107,    85,    86,     0,     0,     0,    84,
      82,    73,   103,    88,     0,    75,    83
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
      -1,     4,     5,     6,     7,    52,     8,     9,    10,    18,
      16,    98,    99,   100,   101,   138,   139,   103,   155,   156,
     134,   135,   136,   159,   183,   184,   204,   205,   175,   199,
     176,   166,   167,   226,   227,   228,   259,   260,   280,   261,
     262,   263,   264,   265,   177,   266,   267,    32,   109,   298,
     107,   269,    33,   108,    63,    64,    35,    36,    37,    38,
      39,    40,    41,    42,    43,    44,    45,    46,    47,    48,
      49,    76,    81,    84,    87,    91,    50
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -241
static const yytype_int16 yypact[] =
{
      64,   -24,  -241,  -241,    27,  -241,    64,  -241,    55,  -241,
      30,   175,  -241,  -241,  -241,    68,  -241,   175,    36,    82,
     117,  -241,    78,  -241,  -241,   171,   118,   226,  -241,  -241,
    -241,  -241,  -241,    66,   113,  -241,   -19,    86,    99,   105,
     106,     6,    10,    83,    79,    89,  -241,  -241,    97,  -241,
      15,   108,   127,   125,   175,   175,   175,   175,   175,   130,
     110,   134,   175,   137,   114,  -241,    15,  -241,   226,   226,
     226,   226,   226,   226,  -241,  -241,   226,  -241,  -241,  -241,
    -241,   226,  -241,  -241,   226,  -241,  -241,   226,  -241,  -241,
    -241,   226,    15,   175,  -241,  -241,  -241,  -241,   141,   120,
    -241,    37,     4,   162,   143,   144,   149,   -33,  -241,   150,
     131,    22,    15,    15,  -241,   152,  -241,   226,  -241,    86,
     138,    99,   105,   106,     6,    10,    83,    79,    89,  -241,
    -241,  -241,   108,   185,    67,    12,    12,  -241,  -241,  -241,
    -241,  -241,  -241,   175,  -241,   155,   175,  -241,  -241,  -241,
    -241,   226,  -241,  -241,  -241,   167,   267,  -241,   189,  -241,
    -241,  -241,   175,   158,  -241,   192,    77,  -241,   177,   179,
     191,   193,   194,   195,   196,   136,  -241,   170,  -241,  -241,
     176,   182,   -12,    92,  -241,    50,  -241,   199,  -241,  -241,
     175,   175,   175,   175,   175,   175,   175,  -241,  -241,  -241,
    -241,  -241,  -241,   229,   184,   205,  -241,   189,  -241,   175,
     132,   -32,   -31,   -29,   -28,    35,    41,    51,   187,   188,
     175,   175,  -241,   200,   207,   197,   210,   190,  -241,  -241,
    -241,  -241,  -241,  -241,  -241,  -241,  -241,  -241,  -241,   211,
    -241,   241,   175,   198,   132,  -241,   214,  -241,   278,  -241,
    -241,   217,   219,   220,   249,   225,   228,   208,     4,  -241,
      58,  -241,  -241,  -241,  -241,   209,  -241,  -241,   212,   218,
     222,   175,   255,   244,   227,   175,   250,  -241,   278,  -241,
    -241,  -241,  -241,  -241,  -241,   175,   246,   252,  -241,  -241,
     259,   238,   247,  -241,  -241,  -241,    58,   175,   261,  -241,
    -241,   297,   131,  -241,   278,  -241,  -241
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -241,  -241,  -241,  -241,   308,  -241,  -241,   -17,   290,   316,
    -241,  -241,  -241,   186,  -241,  -241,  -241,  -241,  -241,  -241,
      61,  -241,  -241,   201,  -241,   115,  -241,  -241,  -241,  -241,
     145,  -241,   159,  -241,  -241,    80,    43,  -241,    31,  -240,
    -188,    24,  -241,  -241,  -241,  -134,   -94,  -151,  -241,  -241,
     -54,  -241,  -149,   -11,  -241,  -241,   -23,  -241,   258,   260,
     262,   257,   263,   256,   253,   251,   254,   240,   -49,   -56,
    -241,  -241,  -241,  -241,  -241,  -241,  -241
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -48
static const yytype_int16 yytable[] =
{
      34,    94,    68,   110,    65,   180,    53,   181,    59,   -38,
     118,   142,   229,   230,    61,   231,   232,     1,    74,    19,
     281,    20,   178,    11,   180,    77,   181,    13,    21,    75,
     -47,    22,    78,   143,   143,   143,   130,   143,   143,    23,
     -38,   178,     1,   105,   106,   -38,   120,   111,     2,    69,
     -45,   115,   218,     3,   219,    24,   281,    93,    26,    79,
      80,   251,   179,   147,   148,   252,   168,   253,   -38,     1,
     145,    15,   254,     2,    22,   255,   157,    17,     3,   233,
     -10,   179,    61,    54,   133,   234,   256,   146,   169,   170,
     171,   172,   173,   174,   150,   235,   257,   268,   208,   270,
       2,   143,    82,   258,   -30,     3,    70,   143,   301,   268,
      51,   270,    83,   154,   165,   209,   305,   143,   158,   158,
      57,    66,   279,   188,    55,    58,    95,   268,   164,   270,
      85,    86,   161,    96,    97,   163,   211,   212,   213,   214,
     215,   216,   217,   224,   168,   268,    88,   270,   225,    89,
      90,   185,    22,   268,   113,   270,   206,    17,   207,    56,
      62,    67,    71,    72,    92,    73,   169,   170,   171,   172,
     173,   174,   102,   104,   112,    19,     1,    20,   114,    19,
     117,    20,   197,   116,    21,   131,   132,    22,    21,   137,
     -17,    22,   140,   141,   144,    23,   149,   143,   223,    23,
     198,   153,   162,   151,   165,   182,   186,     2,   187,   238,
     239,    24,     3,    25,    26,    24,    27,    25,    26,   190,
      27,   191,    28,    29,    30,    31,    28,    29,    30,    31,
      19,   247,    20,   192,   201,   193,   194,   195,   196,    21,
     202,   210,    22,   302,   203,    22,   220,   221,   240,   241,
      23,   236,   237,   242,   243,   245,   244,   246,   250,   271,
     286,   272,   273,   248,   290,   274,    24,   275,    25,    26,
     276,   287,   277,   282,   293,   168,   283,    28,    29,    30,
      31,   251,   284,    22,   285,   252,   168,   253,   288,   291,
     294,   289,   254,   299,    22,   255,   295,   169,   170,   171,
     172,   173,   174,   296,   297,   303,   256,   304,   169,   170,
     171,   172,   173,   174,    14,    60,   257,    12,   152,   278,
     200,   292,   222,   258,   249,   189,   119,   300,   306,   123,
     121,   129,   125,   122,   126,   127,   124,   160,     0,     0,
       0,   128
};

static const yytype_int16 yycheck[] =
{
      11,    50,    21,    57,    27,   156,    17,   156,    25,     5,
      66,    44,    44,    44,    25,    44,    44,     5,    12,     4,
     260,     6,   156,    47,   175,    15,   175,     0,    13,    23,
      42,    16,    22,    66,    66,    66,    92,    66,    66,    24,
      36,   175,     5,    54,    55,    41,    69,    58,    36,    68,
      62,    62,   203,    41,   203,    40,   296,    42,    43,    49,
      50,     3,   156,   112,   113,     7,     8,     9,    64,     5,
      48,    16,    14,    36,    16,    17,    64,    47,    41,    44,
      16,   175,    93,    47,   101,    44,    28,    65,    30,    31,
      32,    33,    34,    35,   117,    44,    38,   248,    48,   248,
      36,    66,    19,    45,    37,    41,    20,    66,   296,   260,
      42,   260,    29,    46,    37,    65,   304,    66,   135,   136,
      42,    55,    64,    46,    42,    47,    18,   278,   151,   278,
      51,    52,   143,    25,    26,   146,   190,   191,   192,   193,
     194,   195,   196,    11,     8,   296,    57,   296,    16,    60,
      61,   162,    16,   304,    44,   304,    64,    47,    66,    42,
      42,    48,    63,    58,    67,    59,    30,    31,    32,    33,
      34,    35,    45,    48,    44,     4,     5,     6,    44,     4,
      66,     6,    46,    46,    13,    44,    66,    16,    13,    27,
      47,    16,    48,    44,    44,    24,    44,    66,   209,    24,
      64,    16,    47,    65,    37,    16,    48,    36,    16,   220,
     221,    40,    41,    42,    43,    40,    45,    42,    43,    42,
      45,    42,    51,    52,    53,    54,    51,    52,    53,    54,
       4,   242,     6,    42,    64,    42,    42,    42,    42,    13,
      64,    42,    16,   297,    62,    16,    62,    42,    48,    42,
      24,    64,    64,    56,    44,    44,    66,    16,    44,    42,
     271,    42,    42,    65,   275,    16,    40,    42,    42,    43,
      42,    16,    64,    64,   285,     8,    64,    51,    52,    53,
      54,     3,    64,    16,    62,     7,     8,     9,    44,    39,
      44,    64,    14,    46,    16,    17,    44,    30,    31,    32,
      33,    34,    35,    44,    66,    44,    28,    10,    30,    31,
      32,    33,    34,    35,     6,    25,    38,     1,   132,   258,
     175,   278,   207,    45,   244,   166,    68,   296,   304,    72,
      70,    91,    76,    71,    81,    84,    73,   136,    -1,    -1,
      -1,    87
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,     5,    36,    41,    70,    71,    72,    73,    75,    76,
      77,    47,    78,     0,    73,    16,    79,    47,    78,     4,
       6,    13,    16,    24,    40,    42,    43,    45,    51,    52,
      53,    54,   116,   121,   122,   125,   126,   127,   128,   129,
     130,   131,   132,   133,   134,   135,   136,   137,   138,   139,
     145,    42,    74,   122,    47,    42,    42,    42,    47,    76,
      77,   122,    42,   123,   124,   125,    55,    48,    21,    68,
      20,    63,    58,    59,    12,    23,   140,    15,    22,    49,
      50,   141,    19,    29,   142,    51,    52,   143,    57,    60,
      61,   144,    67,    42,   137,    18,    25,    26,    80,    81,
      82,    83,    45,    86,    48,   122,   122,   119,   122,   117,
     119,   122,    44,    44,    44,   122,    46,    66,   138,   127,
     125,   128,   129,   130,   131,   132,   133,   134,   135,   136,
     138,    44,    66,    76,    89,    90,    91,    27,    84,    85,
      48,    44,    44,    66,    44,    48,    65,   137,   137,    44,
     125,    65,    82,    16,    46,    87,    88,    64,    76,    92,
      92,   122,    47,   122,   125,    37,   100,   101,     8,    30,
      31,    32,    33,    34,    35,    97,    99,   113,   114,   115,
     116,   121,    16,    93,    94,   122,    48,    16,    46,   101,
      42,    42,    42,    42,    42,    42,    42,    46,    64,    98,
      99,    64,    64,    62,    95,    96,    64,    66,    48,    65,
      42,   119,   119,   119,   119,   119,   119,   119,   116,   121,
      62,    42,    94,   122,    11,    16,   102,   103,   104,    44,
      44,    44,    44,    44,    44,    44,    64,    64,   122,   122,
      48,    42,    56,    44,    66,    44,    16,   122,    65,   104,
      44,     3,     7,     9,    14,    17,    28,    38,    45,   105,
     106,   108,   109,   110,   111,   112,   114,   115,   116,   120,
     121,    42,    42,    42,    16,    42,    42,    64,    89,    64,
     107,   108,    64,    64,    64,    62,   122,    16,    44,    64,
     122,    39,   105,   122,    44,    44,    44,    66,   118,    46,
     107,   109,   119,    44,    10,   109,   110
};

#define yyerrok		(yyerrstatus = 0)
#define yyclearin	(yychar = YYEMPTY)
#define YYEMPTY		(-2)
#define YYEOF		0

#define YYACCEPT	goto yyacceptlab
#define YYABORT		goto yyabortlab
#define YYERROR		goto yyerrorlab


/* Like YYERROR except do call yyerror.  This remains here temporarily
   to ease the transition to the new meaning of YYERROR, for GCC.
   Once GCC version 2 has supplanted version 1, this can go.  */

#define YYFAIL		goto yyerrlab

#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)					\
do								\
  if (yychar == YYEMPTY && yylen == 1)				\
    {								\
      yychar = (Token);						\
      yylval = (Value);						\
      yytoken = YYTRANSLATE (yychar);				\
      YYPOPSTACK (1);						\
      goto yybackup;						\
    }								\
  else								\
    {								\
      yyerror (YY_("syntax error: cannot back up")); \
      YYERROR;							\
    }								\
while (YYID (0))


#define YYTERROR	1
#define YYERRCODE	256


/* YYLLOC_DEFAULT -- Set CURRENT to span from RHS[1] to RHS[N].
   If N is 0, then set CURRENT to the empty location which ends
   the previous symbol: RHS[0] (always defined).  */

#define YYRHSLOC(Rhs, K) ((Rhs)[K])
#ifndef YYLLOC_DEFAULT
# define YYLLOC_DEFAULT(Current, Rhs, N)				\
    do									\
      if (YYID (N))                                                    \
	{								\
	  (Current).first_line   = YYRHSLOC (Rhs, 1).first_line;	\
	  (Current).first_column = YYRHSLOC (Rhs, 1).first_column;	\
	  (Current).last_line    = YYRHSLOC (Rhs, N).last_line;		\
	  (Current).last_column  = YYRHSLOC (Rhs, N).last_column;	\
	}								\
      else								\
	{								\
	  (Current).first_line   = (Current).last_line   =		\
	    YYRHSLOC (Rhs, 0).last_line;				\
	  (Current).first_column = (Current).last_column =		\
	    YYRHSLOC (Rhs, 0).last_column;				\
	}								\
    while (YYID (0))
#endif


/* YY_LOCATION_PRINT -- Print the location on the stream.
   This macro was not mandated originally: define only if we know
   we won't break user code: when these are the locations we know.  */

#ifndef YY_LOCATION_PRINT
# if YYLTYPE_IS_TRIVIAL
#  define YY_LOCATION_PRINT(File, Loc)			\
     fprintf (File, "%d.%d-%d.%d",			\
	      (Loc).first_line, (Loc).first_column,	\
	      (Loc).last_line,  (Loc).last_column)
# else
#  define YY_LOCATION_PRINT(File, Loc) ((void) 0)
# endif
#endif


/* YYLEX -- calling `yylex' with the right arguments.  */

#ifdef YYLEX_PARAM
# define YYLEX yylex (YYLEX_PARAM)
#else
# define YYLEX yylex ()
#endif

/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)			\
do {						\
  if (yydebug)					\
    YYFPRINTF Args;				\
} while (YYID (0))

# define YY_SYMBOL_PRINT(Title, Type, Value, Location)			  \
do {									  \
  if (yydebug)								  \
    {									  \
      YYFPRINTF (stderr, "%s ", Title);					  \
      yy_symbol_print (stderr,						  \
		  Type, Value); \
      YYFPRINTF (stderr, "\n");						  \
    }									  \
} while (YYID (0))


/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

/*ARGSUSED*/
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_symbol_value_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep)
#else
static void
yy_symbol_value_print (yyoutput, yytype, yyvaluep)
    FILE *yyoutput;
    int yytype;
    YYSTYPE const * const yyvaluep;
#endif
{
  if (!yyvaluep)
    return;
# ifdef YYPRINT
  if (yytype < YYNTOKENS)
    YYPRINT (yyoutput, yytoknum[yytype], *yyvaluep);
# else
  YYUSE (yyoutput);
# endif
  switch (yytype)
    {
      default:
	break;
    }
}


/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_symbol_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep)
#else
static void
yy_symbol_print (yyoutput, yytype, yyvaluep)
    FILE *yyoutput;
    int yytype;
    YYSTYPE const * const yyvaluep;
#endif
{
  if (yytype < YYNTOKENS)
    YYFPRINTF (yyoutput, "token %s (", yytname[yytype]);
  else
    YYFPRINTF (yyoutput, "nterm %s (", yytname[yytype]);

  yy_symbol_value_print (yyoutput, yytype, yyvaluep);
  YYFPRINTF (yyoutput, ")");
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_stack_print (yytype_int16 *bottom, yytype_int16 *top)
#else
static void
yy_stack_print (bottom, top)
    yytype_int16 *bottom;
    yytype_int16 *top;
#endif
{
  YYFPRINTF (stderr, "Stack now");
  for (; bottom <= top; ++bottom)
    YYFPRINTF (stderr, " %d", *bottom);
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)				\
do {								\
  if (yydebug)							\
    yy_stack_print ((Bottom), (Top));				\
} while (YYID (0))


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_reduce_print (YYSTYPE *yyvsp, int yyrule)
#else
static void
yy_reduce_print (yyvsp, yyrule)
    YYSTYPE *yyvsp;
    int yyrule;
#endif
{
  int yynrhs = yyr2[yyrule];
  int yyi;
  unsigned long int yylno = yyrline[yyrule];
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %lu):\n",
	     yyrule - 1, yylno);
  /* The symbols being reduced.  */
  for (yyi = 0; yyi < yynrhs; yyi++)
    {
      fprintf (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr, yyrhs[yyprhs[yyrule] + yyi],
		       &(yyvsp[(yyi + 1) - (yynrhs)])
		       		       );
      fprintf (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)		\
do {					\
  if (yydebug)				\
    yy_reduce_print (yyvsp, Rule); \
} while (YYID (0))

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args)
# define YY_SYMBOL_PRINT(Title, Type, Value, Location)
# define YY_STACK_PRINT(Bottom, Top)
# define YY_REDUCE_PRINT(Rule)
#endif /* !YYDEBUG */


/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef	YYINITDEPTH
# define YYINITDEPTH 200
#endif

/* YYMAXDEPTH -- maximum size the stacks can grow to (effective only
   if the built-in stack extension method is used).

   Do not make this value too large; the results are undefined if
   YYSTACK_ALLOC_MAXIMUM < YYSTACK_BYTES (YYMAXDEPTH)
   evaluated with infinite-precision integer arithmetic.  */

#ifndef YYMAXDEPTH
# define YYMAXDEPTH 10000
#endif



#if YYERROR_VERBOSE

# ifndef yystrlen
#  if defined __GLIBC__ && defined _STRING_H
#   define yystrlen strlen
#  else
/* Return the length of YYSTR.  */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static YYSIZE_T
yystrlen (const char *yystr)
#else
static YYSIZE_T
yystrlen (yystr)
    const char *yystr;
#endif
{
  YYSIZE_T yylen;
  for (yylen = 0; yystr[yylen]; yylen++)
    continue;
  return yylen;
}
#  endif
# endif

# ifndef yystpcpy
#  if defined __GLIBC__ && defined _STRING_H && defined _GNU_SOURCE
#   define yystpcpy stpcpy
#  else
/* Copy YYSRC to YYDEST, returning the address of the terminating '\0' in
   YYDEST.  */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static char *
yystpcpy (char *yydest, const char *yysrc)
#else
static char *
yystpcpy (yydest, yysrc)
    char *yydest;
    const char *yysrc;
#endif
{
  char *yyd = yydest;
  const char *yys = yysrc;

  while ((*yyd++ = *yys++) != '\0')
    continue;

  return yyd - 1;
}
#  endif
# endif

# ifndef yytnamerr
/* Copy to YYRES the contents of YYSTR after stripping away unnecessary
   quotes and backslashes, so that it's suitable for yyerror.  The
   heuristic is that double-quoting is unnecessary unless the string
   contains an apostrophe, a comma, or backslash (other than
   backslash-backslash).  YYSTR is taken from yytname.  If YYRES is
   null, do not copy; instead, return the length of what the result
   would have been.  */
static YYSIZE_T
yytnamerr (char *yyres, const char *yystr)
{
  if (*yystr == '"')
    {
      YYSIZE_T yyn = 0;
      char const *yyp = yystr;

      for (;;)
	switch (*++yyp)
	  {
	  case '\'':
	  case ',':
	    goto do_not_strip_quotes;

	  case '\\':
	    if (*++yyp != '\\')
	      goto do_not_strip_quotes;
	    /* Fall through.  */
	  default:
	    if (yyres)
	      yyres[yyn] = *yyp;
	    yyn++;
	    break;

	  case '"':
	    if (yyres)
	      yyres[yyn] = '\0';
	    return yyn;
	  }
    do_not_strip_quotes: ;
    }

  if (! yyres)
    return yystrlen (yystr);

  return yystpcpy (yyres, yystr) - yyres;
}
# endif

/* Copy into YYRESULT an error message about the unexpected token
   YYCHAR while in state YYSTATE.  Return the number of bytes copied,
   including the terminating null byte.  If YYRESULT is null, do not
   copy anything; just return the number of bytes that would be
   copied.  As a special case, return 0 if an ordinary "syntax error"
   message will do.  Return YYSIZE_MAXIMUM if overflow occurs during
   size calculation.  */
static YYSIZE_T
yysyntax_error (char *yyresult, int yystate, int yychar)
{
  int yyn = yypact[yystate];

  if (! (YYPACT_NINF < yyn && yyn <= YYLAST))
    return 0;
  else
    {
      int yytype = YYTRANSLATE (yychar);
      YYSIZE_T yysize0 = yytnamerr (0, yytname[yytype]);
      YYSIZE_T yysize = yysize0;
      YYSIZE_T yysize1;
      int yysize_overflow = 0;
      enum { YYERROR_VERBOSE_ARGS_MAXIMUM = 5 };
      char const *yyarg[YYERROR_VERBOSE_ARGS_MAXIMUM];
      int yyx;

# if 0
      /* This is so xgettext sees the translatable formats that are
	 constructed on the fly.  */
      YY_("syntax error, unexpected %s");
      YY_("syntax error, unexpected %s, expecting %s");
      YY_("syntax error, unexpected %s, expecting %s or %s");
      YY_("syntax error, unexpected %s, expecting %s or %s or %s");
      YY_("syntax error, unexpected %s, expecting %s or %s or %s or %s");
# endif
      char *yyfmt;
      char const *yyf;
      static char const yyunexpected[] = "syntax error, unexpected %s";
      static char const yyexpecting[] = ", expecting %s";
      static char const yyor[] = " or %s";
      char yyformat[sizeof yyunexpected
		    + sizeof yyexpecting - 1
		    + ((YYERROR_VERBOSE_ARGS_MAXIMUM - 2)
		       * (sizeof yyor - 1))];
      char const *yyprefix = yyexpecting;

      /* Start YYX at -YYN if negative to avoid negative indexes in
	 YYCHECK.  */
      int yyxbegin = yyn < 0 ? -yyn : 0;

      /* Stay within bounds of both yycheck and yytname.  */
      int yychecklim = YYLAST - yyn + 1;
      int yyxend = yychecklim < YYNTOKENS ? yychecklim : YYNTOKENS;
      int yycount = 1;

      yyarg[0] = yytname[yytype];
      yyfmt = yystpcpy (yyformat, yyunexpected);

      for (yyx = yyxbegin; yyx < yyxend; ++yyx)
	if (yycheck[yyx + yyn] == yyx && yyx != YYTERROR)
	  {
	    if (yycount == YYERROR_VERBOSE_ARGS_MAXIMUM)
	      {
		yycount = 1;
		yysize = yysize0;
		yyformat[sizeof yyunexpected - 1] = '\0';
		break;
	      }
	    yyarg[yycount++] = yytname[yyx];
	    yysize1 = yysize + yytnamerr (0, yytname[yyx]);
	    yysize_overflow |= (yysize1 < yysize);
	    yysize = yysize1;
	    yyfmt = yystpcpy (yyfmt, yyprefix);
	    yyprefix = yyor;
	  }

      yyf = YY_(yyformat);
      yysize1 = yysize + yystrlen (yyf);
      yysize_overflow |= (yysize1 < yysize);
      yysize = yysize1;

      if (yysize_overflow)
	return YYSIZE_MAXIMUM;

      if (yyresult)
	{
	  /* Avoid sprintf, as that infringes on the user's name space.
	     Don't have undefined behavior even if the translation
	     produced a string with the wrong number of "%s"s.  */
	  char *yyp = yyresult;
	  int yyi = 0;
	  while ((*yyp = *yyf) != '\0')
	    {
	      if (*yyp == '%' && yyf[1] == 's' && yyi < yycount)
		{
		  yyp += yytnamerr (yyp, yyarg[yyi++]);
		  yyf += 2;
		}
	      else
		{
		  yyp++;
		  yyf++;
		}
	    }
	}
      return yysize;
    }
}
#endif /* YYERROR_VERBOSE */


/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

/*ARGSUSED*/
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yydestruct (const char *yymsg, int yytype, YYSTYPE *yyvaluep)
#else
static void
yydestruct (yymsg, yytype, yyvaluep)
    const char *yymsg;
    int yytype;
    YYSTYPE *yyvaluep;
#endif
{
  YYUSE (yyvaluep);

  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yytype, yyvaluep, yylocationp);

  switch (yytype)
    {

      default:
	break;
    }
}


/* Prevent warnings from -Wmissing-prototypes.  */

#ifdef YYPARSE_PARAM
#if defined __STDC__ || defined __cplusplus
int yyparse (void *YYPARSE_PARAM);
#else
int yyparse ();
#endif
#else /* ! YYPARSE_PARAM */
#if defined __STDC__ || defined __cplusplus
int yyparse (void);
#else
int yyparse ();
#endif
#endif /* ! YYPARSE_PARAM */



/* The look-ahead symbol.  */
int yychar;

/* The semantic value of the look-ahead symbol.  */
YYSTYPE yylval;

/* Number of syntax errors so far.  */
int yynerrs;



/*----------.
| yyparse.  |
`----------*/

#ifdef YYPARSE_PARAM
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
int
yyparse (void *YYPARSE_PARAM)
#else
int
yyparse (YYPARSE_PARAM)
    void *YYPARSE_PARAM;
#endif
#else /* ! YYPARSE_PARAM */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
int
yyparse (void)
#else
int
yyparse ()

#endif
#endif
{
  
  int yystate;
  int yyn;
  int yyresult;
  /* Number of tokens to shift before error messages enabled.  */
  int yyerrstatus;
  /* Look-ahead token as an internal (translated) token number.  */
  int yytoken = 0;
#if YYERROR_VERBOSE
  /* Buffer for error messages, and its allocated size.  */
  char yymsgbuf[128];
  char *yymsg = yymsgbuf;
  YYSIZE_T yymsg_alloc = sizeof yymsgbuf;
#endif

  /* Three stacks and their tools:
     `yyss': related to states,
     `yyvs': related to semantic values,
     `yyls': related to locations.

     Refer to the stacks thru separate pointers, to allow yyoverflow
     to reallocate them elsewhere.  */

  /* The state stack.  */
  yytype_int16 yyssa[YYINITDEPTH];
  yytype_int16 *yyss = yyssa;
  yytype_int16 *yyssp;

  /* The semantic value stack.  */
  YYSTYPE yyvsa[YYINITDEPTH];
  YYSTYPE *yyvs = yyvsa;
  YYSTYPE *yyvsp;



#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N))

  YYSIZE_T yystacksize = YYINITDEPTH;

  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;


  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY;		/* Cause a token to be read.  */

  /* Initialize stack pointers.
     Waste one element of value and location stack
     so that they stay on the same level as the state stack.
     The wasted elements are never initialized.  */

  yyssp = yyss;
  yyvsp = yyvs;

  goto yysetstate;

/*------------------------------------------------------------.
| yynewstate -- Push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
 yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed.  So pushing a state here evens the stacks.  */
  yyssp++;

 yysetstate:
  *yyssp = yystate;

  if (yyss + yystacksize - 1 <= yyssp)
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYSIZE_T yysize = yyssp - yyss + 1;

#ifdef yyoverflow
      {
	/* Give user a chance to reallocate the stack.  Use copies of
	   these so that the &'s don't force the real ones into
	   memory.  */
	YYSTYPE *yyvs1 = yyvs;
	yytype_int16 *yyss1 = yyss;


	/* Each stack pointer address is followed by the size of the
	   data in use in that stack, in bytes.  This used to be a
	   conditional around just the two extra args, but that might
	   be undefined if yyoverflow is a macro.  */
	yyoverflow (YY_("memory exhausted"),
		    &yyss1, yysize * sizeof (*yyssp),
		    &yyvs1, yysize * sizeof (*yyvsp),

		    &yystacksize);

	yyss = yyss1;
	yyvs = yyvs1;
      }
#else /* no yyoverflow */
# ifndef YYSTACK_RELOCATE
      goto yyexhaustedlab;
# else
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
	goto yyexhaustedlab;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
	yystacksize = YYMAXDEPTH;

      {
	yytype_int16 *yyss1 = yyss;
	union yyalloc *yyptr =
	  (union yyalloc *) YYSTACK_ALLOC (YYSTACK_BYTES (yystacksize));
	if (! yyptr)
	  goto yyexhaustedlab;
	YYSTACK_RELOCATE (yyss);
	YYSTACK_RELOCATE (yyvs);

#  undef YYSTACK_RELOCATE
	if (yyss1 != yyssa)
	  YYSTACK_FREE (yyss1);
      }
# endif
#endif /* no yyoverflow */

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;


      YYDPRINTF ((stderr, "Stack size increased to %lu\n",
		  (unsigned long int) yystacksize));

      if (yyss + yystacksize - 1 <= yyssp)
	YYABORT;
    }

  YYDPRINTF ((stderr, "Entering state %d\n", yystate));

  goto yybackup;

/*-----------.
| yybackup.  |
`-----------*/
yybackup:

  /* Do appropriate processing given the current state.  Read a
     look-ahead token if we need one and don't already have one.  */

  /* First try to decide what to do without reference to look-ahead token.  */
  yyn = yypact[yystate];
  if (yyn == YYPACT_NINF)
    goto yydefault;

  /* Not known => get a look-ahead token if don't already have one.  */

  /* YYCHAR is either YYEMPTY or YYEOF or a valid look-ahead symbol.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token: "));
      yychar = YYLEX;
    }

  if (yychar <= YYEOF)
    {
      yychar = yytoken = YYEOF;
      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else
    {
      yytoken = YYTRANSLATE (yychar);
      YY_SYMBOL_PRINT ("Next token is", yytoken, &yylval, &yylloc);
    }

  /* If the proper action on seeing token YYTOKEN is to reduce or to
     detect an error, take that action.  */
  yyn += yytoken;
  if (yyn < 0 || YYLAST < yyn || yycheck[yyn] != yytoken)
    goto yydefault;
  yyn = yytable[yyn];
  if (yyn <= 0)
    {
      if (yyn == 0 || yyn == YYTABLE_NINF)
	goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }

  if (yyn == YYFINAL)
    YYACCEPT;

  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  /* Shift the look-ahead token.  */
  YY_SYMBOL_PRINT ("Shifting", yytoken, &yylval, &yylloc);

  /* Discard the shifted token unless it is eof.  */
  if (yychar != YYEOF)
    yychar = YYEMPTY;

  yystate = yyn;
  *++yyvsp = yylval;

  goto yynewstate;


/*-----------------------------------------------------------.
| yydefault -- do the default action for the current state.  |
`-----------------------------------------------------------*/
yydefault:
  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;
  goto yyreduce;


/*-----------------------------.
| yyreduce -- Do a reduction.  |
`-----------------------------*/
yyreduce:
  /* yyn is the number of a rule to reduce with.  */
  yylen = yyr2[yyn];

  /* If YYLEN is nonzero, implement the default value of the action:
     `$$ = $1'.

     Otherwise, the following line sets YYVAL to garbage.
     This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  yyval = yyvsp[1-yylen];


  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
        case 2:
#line 221 "../src/parse_tdf.y"
    { (yyval.suite)=(yyvsp[(1) - (1)].suite); ;}
    break;

  case 3:
#line 226 "../src/parse_tdf.y"
    { (yyval.suite)=(yyvsp[(1) - (1)].suite); ;}
    break;

  case 4:
#line 228 "../src/parse_tdf.y"
    {
			  if (!gSuite)
			  {
			    gSuite=new Suite();
			    if (gEnableGC)
			      TreeGC::addRoot(gSuite);
			  }
			  (yyval.suite)=gSuite;
			;}
    break;

  case 5:
#line 241 "../src/parse_tdf.y"
    {
			  (yyval.suite)=(yyvsp[(1) - (2)].suite);
			  (yyval.suite)->addOperator((yyvsp[(2) - (2)].operatr));
			;}
    break;

  case 6:
#line 246 "../src/parse_tdf.y"
    {
			  if (!gSuite)
			  {
			    gSuite=new Suite();
			    if (gEnableGC)
			      TreeGC::addRoot(gSuite);
			  }
			  (yyval.suite)=gSuite;
			  (yyval.suite)->addOperator((yyvsp[(1) - (1)].operatr));
			;}
    break;

  case 7:
#line 260 "../src/parse_tdf.y"
    {
			  // opSignature leaves iodecls in pushed symtab
			  if ((yyvsp[(1) - (2)].type))
			  {
			    // add operator return to iodecls symtab:
			    (yyvsp[(2) - (2)].opSignature)->retSym = new SymbolStream((yyvsp[(2) - (2)].opSignature)->token,(yyvsp[(2) - (2)].opSignature)->name,
							  (yyvsp[(1) - (2)].type),STREAM_OUT);
			    gSymtab->addSymbol((yyvsp[(2) - (2)].opSignature)->retSym);
			  }
			  else
			  {
			    // no operator return, use TYPE_NONE symbol:
			    (yyvsp[(2) - (2)].opSignature)->retSym=(Symbol*)sym_none->duplicate();
			  }
			;}
    break;

  case 8:
#line 276 "../src/parse_tdf.y"
    {
			  if ((yyvsp[(4) - (5)].opBody)->opKind==OP_BEHAVIORAL)
			  {
			    BehavOpBody *body=(BehavOpBody*)(yyvsp[(4) - (5)].opBody);
			    (yyval.operatr)=new OperatorBehavioral((yyvsp[(2) - (5)].opSignature)->token, (yyvsp[(2) - (5)].opSignature)->name,
						      (yyvsp[(2) - (5)].opSignature)->retSym, (yyvsp[(2) - (5)].opSignature)->args,
						      body->vars, body->states,
						      body->startState, (yyvsp[(5) - (5)].token));
			  }
			  else if ((yyvsp[(4) - (5)].opBody)->opKind==OP_COMPOSE)
			  {
			    ComposeOpBody *body=(ComposeOpBody*)(yyvsp[(4) - (5)].opBody);
			    (yyval.operatr)=new OperatorCompose((yyvsp[(2) - (5)].opSignature)->token, (yyvsp[(2) - (5)].opSignature)->name,
						   (yyvsp[(2) - (5)].opSignature)->retSym, (yyvsp[(2) - (5)].opSignature)->args,
						   body->vars, body->stmts,(yyvsp[(5) - (5)].token));
			  }
			  else  // ($4->opKind==OP_BUILTIN)
			    assert(!"internal inconsistency");
			  // $$->thread(NULL);	// defunct
			  delete symtabPop();	// kill opSignature's symtab
			;}
    break;

  case 9:
#line 301 "../src/parse_tdf.y"
    { (yyval.type)=(yyvsp[(1) - (1)].type); ;}
    break;

  case 10:
#line 303 "../src/parse_tdf.y"
    { (yyval.type)=NULL; ;}
    break;

  case 11:
#line 308 "../src/parse_tdf.y"
    { (yyval.type)=new Type(TYPE_BOOL); ;}
    break;

  case 12:
#line 310 "../src/parse_tdf.y"
    { (yyval.type)=new TypeArray(new Type(TYPE_BOOL),(yyvsp[(2) - (2)].expr)); ;}
    break;

  case 13:
#line 312 "../src/parse_tdf.y"
    {
			  bool signd  = ((yyvsp[(1) - (4)].token)->code==SIGNED) ? true : false;
			  Type *typeE = (yyvsp[(3) - (4)].expr)->getType();
			  if      (typeE->getTypeKind()==TYPE_ANY)
			    // possible unlinked width expr of type TYPE_ANY
			    (yyval.type)=new Type(TYPE_INT,(yyvsp[(3) - (4)].expr),signd);	// assume int
			  else if (typeE->getTypeKind()==TYPE_INT)
			  {
			    (yyval.type)=new Type(TYPE_INT,(yyvsp[(3) - (4)].expr),signd);
			    if ((yyvsp[(3) - (4)].expr)->getExprKind()==EXPR_VALUE &&
				((ExprValue*)(yyvsp[(3) - (4)].expr))->getIntVal()<0)
			      yyerror("bit-width must be non-negative");
			  }
			  else if (typeE->getTypeKind()==TYPE_FIXED)
			  {
			    yyerror("fixed types not supported");
			    // requires const expr or '.' operator: "x.y"
			  }
			  else
			    yyerror("invalid type for bit-width expression");
			;}
    break;

  case 14:
#line 334 "../src/parse_tdf.y"
    {
			  bool signd  = ((yyvsp[(1) - (5)].token)->code==SIGNED) ? true : false;
			  Type *typeE = (yyvsp[(4) - (5)].expr)->getType();
			  if      (typeE->getTypeKind()==TYPE_ANY)
			  {
			    // possible unlinked width expr of type TYPE_ANY
			    Type *elemType=new Type(TYPE_INT,(yyvsp[(4) - (5)].expr),signd);
			    (yyval.type)=new TypeArray(elemType,(yyvsp[(2) - (5)].expr));
			  }
			  else if (typeE->getTypeKind()==TYPE_INT)
			  {
			    Type *elemType=new Type(TYPE_INT,(yyvsp[(4) - (5)].expr),signd);
			    if ((yyvsp[(4) - (5)].expr)->getExprKind()==EXPR_VALUE &&
				((ExprValue*)(yyvsp[(4) - (5)].expr))->getIntVal()<0)
			      yyerror("bit-width must be non-negative");
			    (yyval.type)=new TypeArray(elemType,(yyvsp[(2) - (5)].expr));
			  }
			  else if (typeE->getTypeKind()==TYPE_FIXED)
			  {
			    yyerror("fixed types not supported");
			    // requires const expr or '.' operator: "x.y"
			  }
			  else
			    yyerror("invalid type for bit-width expression");
			;}
    break;

  case 15:
#line 363 "../src/parse_tdf.y"
    { (yyval.token)=(yyvsp[(1) - (1)].token); ;}
    break;

  case 16:
#line 365 "../src/parse_tdf.y"
    { (yyval.token)=(yyvsp[(1) - (1)].token); ;}
    break;

  case 17:
#line 370 "../src/parse_tdf.y"
    {
			  Type *typeE = (yyvsp[(2) - (3)].expr)->getType();
			  if (typeE->getTypeKind()==TYPE_ANY)
			    // possible unlinked nelems expr of type TYPE_ANY
			    (yyval.expr)=(yyvsp[(2) - (3)].expr);
			  else if (typeE->getTypeKind()==TYPE_INT)
			  {
			    (yyval.expr)=(yyvsp[(2) - (3)].expr);
			    if ((yyvsp[(2) - (3)].expr)->getExprKind()==EXPR_VALUE &&
				((ExprValue*)(yyvsp[(2) - (3)].expr))->getIntVal()<0)
			      yyerror("array size must be non-negative");
			  }
			  else
			    yyerror("invalid type for array size expression");
			;}
    break;

  case 18:
#line 389 "../src/parse_tdf.y"
    { (yyval.opSignature)=new OpSignature((yyvsp[(1) - (4)].token),(yyvsp[(1) - (4)].token)->str,(yyvsp[(3) - (4)].symList)); ;}
    break;

  case 19:
#line 394 "../src/parse_tdf.y"
    { (yyval.symList)=(yyvsp[(1) - (1)].symList); ;}
    break;

  case 20:
#line 396 "../src/parse_tdf.y"
    {
			  (yyval.symList)=new list<Symbol*>;
			  symtabPush(new SymTab(SYMTAB_OP));
			;}
    break;

  case 21:
#line 404 "../src/parse_tdf.y"
    {
			  (yyval.symList)=(yyvsp[(1) - (3)].symList);
			  (yyval.symList)->append((yyvsp[(3) - (3)].sym));
			  gSymtab->addSymbol((yyvsp[(3) - (3)].sym));
			;}
    break;

  case 22:
#line 410 "../src/parse_tdf.y"
    {
			  (yyval.symList)=new list<Symbol*>;
			  (yyval.symList)->append((yyvsp[(1) - (1)].sym));
			  symtabPush(new SymTab(SYMTAB_OP));
			  gSymtab->addSymbol((yyvsp[(1) - (1)].sym));
			;}
    break;

  case 23:
#line 420 "../src/parse_tdf.y"
    {
			  if ((yyvsp[(1) - (3)].token)->code==INPUT)
			    (yyval.sym)=new SymbolStream((yyvsp[(3) - (3)].token),(yyvsp[(3) - (3)].token)->str,(yyvsp[(2) - (3)].type),STREAM_IN);
			  else if ((yyvsp[(1) - (3)].token)->code==OUTPUT)
			    (yyval.sym)=new SymbolStream((yyvsp[(3) - (3)].token),(yyvsp[(3) - (3)].token)->str,(yyvsp[(2) - (3)].type),STREAM_OUT);
			  else  // ($1->code==PARAM)
			    (yyval.sym)=new SymbolVar((yyvsp[(3) - (3)].token),(yyvsp[(3) - (3)].token)->str,(yyvsp[(2) - (3)].type));
			;}
    break;

  case 24:
#line 432 "../src/parse_tdf.y"
    { (yyval.token)=(yyvsp[(1) - (1)].token); ;}
    break;

  case 25:
#line 434 "../src/parse_tdf.y"
    { (yyval.token)=(yyvsp[(1) - (1)].token); ;}
    break;

  case 26:
#line 436 "../src/parse_tdf.y"
    { (yyval.token)=(yyvsp[(1) - (1)].token); ;}
    break;

  case 27:
#line 441 "../src/parse_tdf.y"
    { (yyval.token)=(yyvsp[(1) - (1)].token); ;}
    break;

  case 28:
#line 443 "../src/parse_tdf.y"
    { (yyval.token)=NULL; ;}
    break;

  case 29:
#line 448 "../src/parse_tdf.y"
    { (yyval.token)=(yyvsp[(1) - (1)].token); ;}
    break;

  case 30:
#line 453 "../src/parse_tdf.y"
    {
			  // typeDecls_opt leaves pushed symtab
			  gStates=new dictionary<string,State*>;
			;}
    break;

  case 31:
#line 458 "../src/parse_tdf.y"
    {
			  (yyval.opBody)=(OpBody*)new BehavOpBody(gSymtab,gStates,(yyvsp[(4) - (5)].states));
			  symtabPop();
			  gStates=NULL;
			;}
    break;

  case 32:
#line 464 "../src/parse_tdf.y"
    {
			  // typeDecls_opt leaves pushed symtab
			;}
    break;

  case 33:
#line 468 "../src/parse_tdf.y"
    {
			  (yyval.opBody)=(OpBody*)new ComposeOpBody(gSymtab,(yyvsp[(4) - (5)].stmts));
			  symtabPop();
			;}
    break;

  case 34:
#line 473 "../src/parse_tdf.y"
    {
			  // typeDecls_opt leaves pushed symtab
			  delete symtabPop();
			  yyerror("empty operator body");
			;}
    break;

  case 35:
#line 482 "../src/parse_tdf.y"
    { (yyval.symtab)=(yyvsp[(1) - (1)].symtab); ;}
    break;

  case 36:
#line 484 "../src/parse_tdf.y"
    {
			  // push new empty symtab before processing first decl
			  symtabPush(new SymTab(SYMTAB_BLOCK));
			  (yyval.symtab)=gSymtab;
			;}
    break;

  case 37:
#line 493 "../src/parse_tdf.y"
    { (yyval.symtab)=gSymtab; ;}
    break;

  case 38:
#line 495 "../src/parse_tdf.y"
    {
			  // push new empty symtab before processing first decl
			  symtabPush(new SymTab(SYMTAB_BLOCK));
			;}
    break;

  case 39:
#line 500 "../src/parse_tdf.y"
    { (yyval.symtab)=gSymtab; ;}
    break;

  case 40:
#line 505 "../src/parse_tdf.y"
    {
			  (yyval.symtab)=gSymtab;
			  bool first=true;	// - mark 1st loop iter
			  TypeDeclElem *tde;
			  forall (tde,*(yyvsp[(2) - (3)].typeDeclElems)) {
			    Type   *typ=first ? (yyvsp[(1) - (3)].type) : (Type*)(yyvsp[(1) - (3)].type)->duplicate();
			    Symbol *sym=new SymbolVar(tde->id,tde->id->str,
						      typ,tde->val,tde->depth);
			    gSymtab->addSymbol(sym);
			    first=false;
			  }
			  // delete $2;
			;}
    break;

  case 41:
#line 519 "../src/parse_tdf.y"
    { (yyval.symtab)=gSymtab; ;}
    break;

  case 42:
#line 523 "../src/parse_tdf.y"
    { (yyval.typeDeclElems)=new list<TypeDeclElem*>; (yyval.typeDeclElems)->append((yyvsp[(1) - (1)].typeDeclElem)); ;}
    break;

  case 43:
#line 525 "../src/parse_tdf.y"
    { (yyval.typeDeclElems)=(yyvsp[(1) - (3)].typeDeclElems);                      (yyval.typeDeclElems)->append((yyvsp[(3) - (3)].typeDeclElem)); ;}
    break;

  case 44:
#line 529 "../src/parse_tdf.y"
    {
			  lookupDuplicateDecl((yyvsp[(1) - (1)].token));
			  (yyval.typeDeclElem)=new TypeDeclElem((yyvsp[(1) - (1)].token),NULL,NULL);
			;}
    break;

  case 45:
#line 534 "../src/parse_tdf.y"
    { lookupDuplicateDecl((yyvsp[(1) - (1)].token)); ;}
    break;

  case 46:
#line 536 "../src/parse_tdf.y"
    { (yyval.typeDeclElem)=new TypeDeclElem((yyvsp[(1) - (4)].token),(yyvsp[(4) - (4)].expr),NULL); ;}
    break;

  case 47:
#line 539 "../src/parse_tdf.y"
    { lookupDuplicateDecl((yyvsp[(1) - (1)].token)); ;}
    break;

  case 48:
#line 541 "../src/parse_tdf.y"
    { (yyval.typeDeclElem)=new TypeDeclElem((yyvsp[(1) - (5)].token),NULL,(yyvsp[(4) - (5)].expr)); ;}
    break;

  case 49:
#line 546 "../src/parse_tdf.y"
    { (yyval.stmts)=(yyvsp[(1) - (2)].stmts);		if ((yyvsp[(2) - (2)].stmt)) (yyval.stmts)->append((yyvsp[(2) - (2)].stmt)); ;}
    break;

  case 50:
#line 548 "../src/parse_tdf.y"
    { (yyval.stmts)=new list<Stmt*>;	if ((yyvsp[(1) - (1)].stmt)) (yyval.stmts)->append((yyvsp[(1) - (1)].stmt)); ;}
    break;

  case 51:
#line 553 "../src/parse_tdf.y"
    { (yyval.stmt)=(yyvsp[(1) - (1)].stmt); ;}
    break;

  case 52:
#line 555 "../src/parse_tdf.y"
    { (yyval.stmt)=NULL; ;}
    break;

  case 53:
#line 560 "../src/parse_tdf.y"
    { (yyval.stmt)=new StmtCall((yyvsp[(1) - (2)].expr)->getToken(),(ExprCall*)(yyvsp[(1) - (2)].expr)); ;}
    break;

  case 54:
#line 562 "../src/parse_tdf.y"
    { (yyval.stmt)=new StmtAssign((yyvsp[(2) - (4)].token),(ExprLValue*)(yyvsp[(1) - (4)].expr),(yyvsp[(3) - (4)].expr)); ;}
    break;

  case 55:
#line 564 "../src/parse_tdf.y"
    { (yyval.stmt)=new StmtAssign((yyvsp[(2) - (4)].token),(ExprLValue*)(yyvsp[(1) - (4)].expr),(yyvsp[(3) - (4)].expr)); ;}
    break;

  case 56:
#line 566 "../src/parse_tdf.y"
    { (yyval.stmt)=(yyvsp[(1) - (2)].stmt); ;}
    break;

  case 57:
#line 575 "../src/parse_tdf.y"
    {
			  (yyval.states)=(yyvsp[(1) - (2)].states);			// $$ = start state
			  // states are stored in gStates by stateCase action
			;}
    break;

  case 58:
#line 580 "../src/parse_tdf.y"
    {
			  (yyval.states)=(State*)((yyvsp[(1) - (1)].stateCase)->getParent());	// $$ = start state
			  // states are stored in gStates by stateCase action
			;}
    break;

  case 59:
#line 589 "../src/parse_tdf.y"
    {
			  // lookup/create state and add stateCase to it
			  dic_item state_item=gStates->lookup((yyvsp[(2) - (7)].token)->str);
			  State *state;
			  if (state_item)
			    state=gStates->inf(state_item);
			  else
			  {
			    state=new State((yyvsp[(2) - (7)].token),(yyvsp[(2) - (7)].token)->str,new set<StateCase*>);
			    gStates->insert((yyvsp[(2) - (7)].token)->str,state);
			  }
			  (yyval.stateCase)=new StateCase((yyvsp[(2) - (7)].token),(yyvsp[(4) - (7)].inputSpecs),(yyvsp[(7) - (7)].stmts));
			  state->addCase((yyval.stateCase));
			;}
    break;

  case 60:
#line 607 "../src/parse_tdf.y"
    { (yyval.inputSpecs)=(yyvsp[(1) - (1)].inputSpecs); ;}
    break;

  case 61:
#line 609 "../src/parse_tdf.y"
    { (yyval.inputSpecs)=new list<InputSpec*>; ;}
    break;

  case 62:
#line 614 "../src/parse_tdf.y"
    { (yyval.inputSpecs)=(yyvsp[(1) - (3)].inputSpecs);			(yyval.inputSpecs)->append((yyvsp[(3) - (3)].inputSpec)); ;}
    break;

  case 63:
#line 616 "../src/parse_tdf.y"
    { (yyval.inputSpecs)=new list<InputSpec*>;	(yyval.inputSpecs)->append((yyvsp[(1) - (1)].inputSpec)); ;}
    break;

  case 64:
#line 621 "../src/parse_tdf.y"
    { (yyval.inputSpec)=new InputSpec((yyvsp[(1) - (1)].token),lookup((yyvsp[(1) - (1)].token))); ;}
    break;

  case 65:
#line 623 "../src/parse_tdf.y"
    { (yyval.inputSpec)=new InputSpec((yyvsp[(1) - (3)].token),lookup((yyvsp[(1) - (3)].token)),(yyvsp[(3) - (3)].expr)); ;}
    break;

  case 66:
#line 625 "../src/parse_tdf.y"
    { (yyval.inputSpec)=new InputSpec((yyvsp[(3) - (4)].token),lookup((yyvsp[(3) - (4)].token)),NULL,true); ;}
    break;

  case 67:
#line 630 "../src/parse_tdf.y"
    { (yyval.stmts)=(yyvsp[(1) - (1)].stmts); ;}
    break;

  case 68:
#line 632 "../src/parse_tdf.y"
    { (yyval.stmts)=new list<Stmt*>; ;}
    break;

  case 69:
#line 637 "../src/parse_tdf.y"
    { (yyval.stmts)=(yyvsp[(1) - (2)].stmts);		if ((yyvsp[(2) - (2)].stmt)) (yyval.stmts)->append((yyvsp[(2) - (2)].stmt)); ;}
    break;

  case 70:
#line 639 "../src/parse_tdf.y"
    { (yyval.stmts)=new list<Stmt*>;	if ((yyvsp[(1) - (1)].stmt)) (yyval.stmts)->append((yyvsp[(1) - (1)].stmt)); ;}
    break;

  case 71:
#line 644 "../src/parse_tdf.y"
    { (yyval.stmt)=(yyvsp[(1) - (1)].stmt); ;}
    break;

  case 72:
#line 646 "../src/parse_tdf.y"
    { (yyval.stmt)=NULL; ;}
    break;

  case 73:
#line 651 "../src/parse_tdf.y"
    { (yyval.stmt)=(yyvsp[(1) - (1)].stmt); ;}
    break;

  case 74:
#line 653 "../src/parse_tdf.y"
    { (yyval.stmt)=(yyvsp[(1) - (1)].stmt); ;}
    break;

  case 75:
#line 658 "../src/parse_tdf.y"
    { (yyval.stmt)=new StmtIf((yyvsp[(1) - (7)].token),(yyvsp[(3) - (7)].expr),(yyvsp[(5) - (7)].stmt),(yyvsp[(7) - (7)].stmt)); ;}
    break;

  case 76:
#line 660 "../src/parse_tdf.y"
    { (yyval.stmt)=new StmtGoto((yyvsp[(1) - (3)].token),(yyvsp[(2) - (3)].token)->str); ;}
    break;

  case 77:
#line 662 "../src/parse_tdf.y"
    { (yyval.stmt)=new StmtGoto((yyvsp[(1) - (2)].token),NULL); ;}
    break;

  case 78:
#line 664 "../src/parse_tdf.y"
    { (yyval.stmt)=(yyvsp[(1) - (2)].stmt); ;}
    break;

  case 79:
#line 666 "../src/parse_tdf.y"
    { (yyval.stmt)=(yyvsp[(1) - (2)].stmt); ;}
    break;

  case 80:
#line 668 "../src/parse_tdf.y"
    { (yyval.stmt)=new StmtCall((yyvsp[(1) - (2)].expr)->getToken(),(ExprCall*)(yyvsp[(1) - (2)].expr)); ;}
    break;

  case 81:
#line 670 "../src/parse_tdf.y"
    { (yyval.stmt)=(yyvsp[(1) - (1)].stmt); ;}
    break;

  case 82:
#line 675 "../src/parse_tdf.y"
    { (yyval.stmt)=new StmtIf((yyvsp[(1) - (5)].token),(yyvsp[(3) - (5)].expr),(yyvsp[(5) - (5)].stmt)); ;}
    break;

  case 83:
#line 677 "../src/parse_tdf.y"
    { (yyval.stmt)=new StmtIf((yyvsp[(1) - (7)].token),(yyvsp[(3) - (7)].expr),(yyvsp[(5) - (7)].stmt),(yyvsp[(7) - (7)].stmt)); ;}
    break;

  case 84:
#line 682 "../src/parse_tdf.y"
    {
			  // typeDecls_opt leaves pushed symtab
			  (yyval.stmt)=new StmtBlock((yyvsp[(1) - (4)].token),gSymtab,(yyvsp[(3) - (4)].stmts));
			  symtabPop();
			;}
    break;

  case 85:
#line 691 "../src/parse_tdf.y"
    {
			  list<Expr*> *args=new list<Expr*>;
			  args->append((yyvsp[(3) - (4)].expr));
			  ExprBuiltin *eb=new ExprBuiltin((yyvsp[(1) - (4)].token),args,
							  builtin_attn);
			  (yyval.stmt)=new StmtBuiltin((yyvsp[(1) - (4)].token),eb);
			;}
    break;

  case 86:
#line 699 "../src/parse_tdf.y"
    {
			  ExprLValue *el=new ExprLValue((yyvsp[(3) - (4)].token),lookup((yyvsp[(3) - (4)].token)));
			  list<Expr*> *args=new list<Expr*>;
			  args->append(el);
			  ExprBuiltin *eb=new ExprBuiltin((yyvsp[(1) - (4)].token),args,
							  builtin_close);
			  (yyval.stmt)=new StmtBuiltin((yyvsp[(1) - (4)].token),eb);
			;}
    break;

  case 87:
#line 708 "../src/parse_tdf.y"
    {
			  list<Expr*> *args=new list<Expr*>;
			  args->append((Expr*)expr_0->duplicate());   // sync 0
			  ExprBuiltin *eb=new ExprBuiltin((yyvsp[(1) - (3)].token),args,
							  builtin_done);
			  (yyval.stmt)=new StmtBuiltin((yyvsp[(1) - (3)].token),eb);
			;}
    break;

  case 88:
#line 716 "../src/parse_tdf.y"
    {
			  list<Expr*> *args=(yyvsp[(4) - (5)].args);
			  ExprBuiltin *eb=new ExprBuiltin((yyvsp[(1) - (5)].token),args,
							  builtin_printf);
			  eb->setAnnote_(ANNOTE_PRINTF_STRING_TOKEN,(yyvsp[(3) - (5)].token));
			  (yyval.stmt)=new StmtBuiltin((yyvsp[(1) - (5)].token),eb);
			;}
    break;

  case 89:
#line 724 "../src/parse_tdf.y"
    { (yyval.stmt)=(yyvsp[(1) - (1)].stmt); ;}
    break;

  case 90:
#line 726 "../src/parse_tdf.y"
    { (yyval.stmt)=(yyvsp[(1) - (1)].stmt); ;}
    break;

  case 91:
#line 731 "../src/parse_tdf.y"
    { (yyval.stmt)=(yyvsp[(1) - (1)].stmt); ;}
    break;

  case 92:
#line 733 "../src/parse_tdf.y"
    { (yyval.stmt)=(yyvsp[(1) - (1)].stmt); ;}
    break;

  case 93:
#line 738 "../src/parse_tdf.y"
    {
			  ExprBuiltin *eb=new ExprBuiltin((yyvsp[(1) - (4)].token),(yyvsp[(3) - (4)].args),builtin_copy);
			  (yyval.stmt)=new StmtBuiltin((yyvsp[(1) - (4)].token),eb);
			;}
    break;

  case 94:
#line 746 "../src/parse_tdf.y"
    {
			  OperatorSegment *o
			    = new OperatorSegment((yyvsp[(1) - (4)].token),string("_dummy_seg_op"),
						  SEGMENT_R,
						  type_none,new list<Symbol*>);
			  ExprBuiltin *e=new ExprBuiltin((yyvsp[(1) - (4)].token),(yyvsp[(3) - (4)].args),o);
			  (yyval.stmt)=new StmtBuiltin((yyvsp[(1) - (4)].token),e);
			;}
    break;

  case 95:
#line 755 "../src/parse_tdf.y"
    {
			  OperatorSegment *o
			    = new OperatorSegment((yyvsp[(1) - (4)].token),string("_dummy_seg_op"),
						  SEGMENT_W,
						  type_none,new list<Symbol*>);
			  ExprBuiltin *e=new ExprBuiltin((yyvsp[(1) - (4)].token),(yyvsp[(3) - (4)].args),o);
			  (yyval.stmt)=new StmtBuiltin((yyvsp[(1) - (4)].token),e);
			;}
    break;

  case 96:
#line 764 "../src/parse_tdf.y"
    {
			  OperatorSegment *o
			    = new OperatorSegment((yyvsp[(1) - (4)].token),string("_dummy_seg_op"),
						  SEGMENT_RW,
						  type_none,new list<Symbol*>);
			  ExprBuiltin *e=new ExprBuiltin((yyvsp[(1) - (4)].token),(yyvsp[(3) - (4)].args),o);
			  (yyval.stmt)=new StmtBuiltin((yyvsp[(1) - (4)].token),e);
			;}
    break;

  case 97:
#line 773 "../src/parse_tdf.y"
    {
			  OperatorSegment *o
			    = new OperatorSegment((yyvsp[(1) - (4)].token),string("_dummy_seg_op"),
						  SEGMENT_SEQ_R,
						  type_none,new list<Symbol*>);
			  ExprBuiltin *e=new ExprBuiltin((yyvsp[(1) - (4)].token),(yyvsp[(3) - (4)].args),o);
			  (yyval.stmt)=new StmtBuiltin((yyvsp[(1) - (4)].token),e);
			;}
    break;

  case 98:
#line 782 "../src/parse_tdf.y"
    {
			  OperatorSegment *o
			    = new OperatorSegment((yyvsp[(1) - (4)].token),string("_dummy_seg_op"),
						  SEGMENT_SEQ_W,
						  type_none,new list<Symbol*>);
			  ExprBuiltin *e=new ExprBuiltin((yyvsp[(1) - (4)].token),(yyvsp[(3) - (4)].args),o);
			  (yyval.stmt)=new StmtBuiltin((yyvsp[(1) - (4)].token),e);
			;}
    break;

  case 99:
#line 791 "../src/parse_tdf.y"
    {
			  OperatorSegment *o
			    = new OperatorSegment((yyvsp[(1) - (4)].token),string("_dummy_seg_op"),
						  SEGMENT_SEQ_RW,
						  type_none,new list<Symbol*>);
			  ExprBuiltin *e=new ExprBuiltin((yyvsp[(1) - (4)].token),(yyvsp[(3) - (4)].args),o);
			  (yyval.stmt)=new StmtBuiltin((yyvsp[(1) - (4)].token),e);
			;}
    break;

  case 100:
#line 803 "../src/parse_tdf.y"
    { (yyval.expr)=new ExprCall((yyvsp[(1) - (4)].token),(yyvsp[(3) - (4)].args)); ;}
    break;

  case 101:
#line 808 "../src/parse_tdf.y"
    { (yyval.args)=(yyvsp[(1) - (1)].args); ;}
    break;

  case 102:
#line 810 "../src/parse_tdf.y"
    {
			  // empty args list implies 0-width sync stream
			  (yyval.args)=new list<Expr*>;
			  (yyval.args)->append((Expr*)expr_0->duplicate());
			;}
    break;

  case 103:
#line 819 "../src/parse_tdf.y"
    { (yyval.args)=(yyvsp[(2) - (2)].args); ;}
    break;

  case 104:
#line 821 "../src/parse_tdf.y"
    { (yyval.args)=new list<Expr*>; ;}
    break;

  case 105:
#line 826 "../src/parse_tdf.y"
    { (yyval.args)=(yyvsp[(1) - (3)].args);		(yyval.args)->append((yyvsp[(3) - (3)].expr)); ;}
    break;

  case 106:
#line 828 "../src/parse_tdf.y"
    { (yyval.args)=new list<Expr*>;	(yyval.args)->append((yyvsp[(1) - (1)].expr)); ;}
    break;

  case 107:
#line 833 "../src/parse_tdf.y"
    { (yyval.stmt)=new StmtAssign((yyvsp[(2) - (3)].token),(ExprLValue*)(yyvsp[(1) - (3)].expr),(yyvsp[(3) - (3)].expr)); ;}
    break;

  case 108:
#line 838 "../src/parse_tdf.y"
    {
			  Symbol *s  = lookupTry((yyvsp[(1) - (1)].token)),
				 *sym= s?s:new SymbolVar((yyvsp[(1) - (1)].token),(yyvsp[(1) - (1)].token)->str,type_any);
			  (yyval.expr)=new ExprLValue((yyvsp[(1) - (1)].token),sym);
			  // warning - a param appearing in the width
			  //           expression of an operator's return type,
			  //           where the param formal has not yet been
			  //           parsed, is left as an unlinked symbol.
			  //           the symbol ref is linked by link().
			  //           e.g.:
			  // "unsigned[n] doit(param unsigned[8] n, ...)"
			;}
    break;

  case 109:
#line 851 "../src/parse_tdf.y"
    {
			  Symbol *s=lookup((yyvsp[(1) - (4)].token));
			  if (s->getType()->getTypeKind()==TYPE_ARRAY)
			    (yyval.expr)=new ExprLValue((yyvsp[(1) - (4)].token),lookup((yyvsp[(1) - (4)].token)),NULL,NULL,NULL,(yyvsp[(3) - (4)].expr));
			  else
			    (yyval.expr)=new ExprLValue((yyvsp[(1) - (4)].token),lookup((yyvsp[(1) - (4)].token)),(yyvsp[(3) - (4)].expr));
			;}
    break;

  case 110:
#line 859 "../src/parse_tdf.y"
    { (yyval.expr)=new ExprLValue((yyvsp[(1) - (6)].token),lookup((yyvsp[(1) - (6)].token)),(yyvsp[(5) - (6)].expr),(yyvsp[(3) - (6)].expr)); ;}
    break;

  case 111:
#line 861 "../src/parse_tdf.y"
    { (yyval.expr)=new ExprLValue((yyvsp[(1) - (7)].token),lookup((yyvsp[(1) - (7)].token)),(yyvsp[(6) - (7)].expr),NULL,NULL,(yyvsp[(3) - (7)].expr)); ;}
    break;

  case 112:
#line 863 "../src/parse_tdf.y"
    { (yyval.expr)=new ExprLValue((yyvsp[(1) - (9)].token),lookup((yyvsp[(1) - (9)].token)),(yyvsp[(8) - (9)].expr),(yyvsp[(6) - (9)].expr),NULL,(yyvsp[(3) - (9)].expr)); ;}
    break;

  case 113:
#line 868 "../src/parse_tdf.y"
    { (yyval.expr)=(yyvsp[(1) - (1)].expr); ;}
    break;

  case 114:
#line 870 "../src/parse_tdf.y"
    {
			  TypeArray *type=new TypeArray(type_any,(yyvsp[(2) - (3)].exprs)->size());
			  (yyval.expr)=new ExprArray((yyvsp[(1) - (3)].token),type,(yyvsp[(2) - (3)].exprs));
			;}
    break;

  case 115:
#line 878 "../src/parse_tdf.y"
    { (yyval.exprs)=(yyvsp[(1) - (1)].exprs); ;}
    break;

  case 116:
#line 880 "../src/parse_tdf.y"
    { (yyval.exprs)=new list<Expr*>; ;}
    break;

  case 117:
#line 885 "../src/parse_tdf.y"
    { (yyval.exprs)=(yyvsp[(1) - (3)].exprs); (yyval.exprs)->append((yyvsp[(3) - (3)].expr)); ;}
    break;

  case 118:
#line 887 "../src/parse_tdf.y"
    { (yyval.exprs)=new list<Expr*>; (yyval.exprs)->append((yyvsp[(1) - (1)].expr)); ;}
    break;

  case 119:
#line 892 "../src/parse_tdf.y"
    { (yyval.expr)=new ExprCond((yyvsp[(2) - (5)].token),(yyvsp[(1) - (5)].expr),(yyvsp[(3) - (5)].expr),(yyvsp[(5) - (5)].expr)); ;}
    break;

  case 120:
#line 894 "../src/parse_tdf.y"
    { (yyval.expr)=(yyvsp[(1) - (1)].expr); ;}
    break;

  case 121:
#line 899 "../src/parse_tdf.y"
    { (yyval.expr)=new ExprBop((yyvsp[(2) - (3)].token),LOGIC_OR,(yyvsp[(1) - (3)].expr),(yyvsp[(3) - (3)].expr)); ;}
    break;

  case 122:
#line 901 "../src/parse_tdf.y"
    { (yyval.expr)=(yyvsp[(1) - (1)].expr); ;}
    break;

  case 123:
#line 906 "../src/parse_tdf.y"
    { (yyval.expr)=new ExprBop((yyvsp[(2) - (3)].token),LOGIC_AND,(yyvsp[(1) - (3)].expr),(yyvsp[(3) - (3)].expr)); ;}
    break;

  case 124:
#line 908 "../src/parse_tdf.y"
    { (yyval.expr)=(yyvsp[(1) - (1)].expr); ;}
    break;

  case 125:
#line 913 "../src/parse_tdf.y"
    { (yyval.expr)=new ExprBop((yyvsp[(2) - (3)].token),(yyvsp[(2) - (3)].token)->code,(yyvsp[(1) - (3)].expr),(yyvsp[(3) - (3)].expr)); ;}
    break;

  case 126:
#line 915 "../src/parse_tdf.y"
    { (yyval.expr)=(yyvsp[(1) - (1)].expr); ;}
    break;

  case 127:
#line 920 "../src/parse_tdf.y"
    { (yyval.expr)=new ExprBop((yyvsp[(2) - (3)].token),(yyvsp[(2) - (3)].token)->code,(yyvsp[(1) - (3)].expr),(yyvsp[(3) - (3)].expr)); ;}
    break;

  case 128:
#line 922 "../src/parse_tdf.y"
    { (yyval.expr)=(yyvsp[(1) - (1)].expr); ;}
    break;

  case 129:
#line 927 "../src/parse_tdf.y"
    { (yyval.expr)=new ExprBop((yyvsp[(2) - (3)].token),(yyvsp[(2) - (3)].token)->code,(yyvsp[(1) - (3)].expr),(yyvsp[(3) - (3)].expr)); ;}
    break;

  case 130:
#line 929 "../src/parse_tdf.y"
    { (yyval.expr)=(yyvsp[(1) - (1)].expr); ;}
    break;

  case 131:
#line 934 "../src/parse_tdf.y"
    { (yyval.expr)=new ExprBop((yyvsp[(2) - (3)].token),(yyvsp[(2) - (3)].token)->code,(yyvsp[(1) - (3)].expr),(yyvsp[(3) - (3)].expr)); ;}
    break;

  case 132:
#line 936 "../src/parse_tdf.y"
    { (yyval.expr)=(yyvsp[(1) - (1)].expr); ;}
    break;

  case 133:
#line 941 "../src/parse_tdf.y"
    { (yyval.expr)=new ExprBop((yyvsp[(2) - (3)].token),(yyvsp[(2) - (3)].token)->code,(yyvsp[(1) - (3)].expr),(yyvsp[(3) - (3)].expr)); ;}
    break;

  case 134:
#line 943 "../src/parse_tdf.y"
    { (yyval.expr)=(yyvsp[(1) - (1)].expr); ;}
    break;

  case 135:
#line 948 "../src/parse_tdf.y"
    { (yyval.expr)=new ExprBop((yyvsp[(2) - (3)].token),(yyvsp[(2) - (3)].token)->code,(yyvsp[(1) - (3)].expr),(yyvsp[(3) - (3)].expr)); ;}
    break;

  case 136:
#line 950 "../src/parse_tdf.y"
    { (yyval.expr)=(yyvsp[(1) - (1)].expr); ;}
    break;

  case 137:
#line 955 "../src/parse_tdf.y"
    { (yyval.expr)=new ExprBop((yyvsp[(2) - (3)].token),(yyvsp[(2) - (3)].token)->code,(yyvsp[(1) - (3)].expr),(yyvsp[(3) - (3)].expr)); ;}
    break;

  case 138:
#line 957 "../src/parse_tdf.y"
    { (yyval.expr)=(yyvsp[(1) - (1)].expr); ;}
    break;

  case 139:
#line 962 "../src/parse_tdf.y"
    { (yyval.expr)=new ExprBop((yyvsp[(2) - (3)].token),(yyvsp[(2) - (3)].token)->code,(yyvsp[(1) - (3)].expr),(yyvsp[(3) - (3)].expr)); ;}
    break;

  case 140:
#line 964 "../src/parse_tdf.y"
    { (yyval.expr)=(yyvsp[(1) - (1)].expr); ;}
    break;

  case 141:
#line 969 "../src/parse_tdf.y"
    { (yyval.expr)=new ExprUop((yyvsp[(1) - (2)].token),(yyvsp[(1) - (2)].token)->code,(yyvsp[(2) - (2)].expr)); ;}
    break;

  case 142:
#line 971 "../src/parse_tdf.y"
    { (yyval.expr)=new ExprCast((yyvsp[(1) - (4)].token),(yyvsp[(2) - (4)].type),(yyvsp[(4) - (4)].expr)); ;}
    break;

  case 143:
#line 973 "../src/parse_tdf.y"
    { (yyval.expr)=new ExprCast((yyvsp[(1) - (4)].token),(yyvsp[(2) - (4)].token)->code==SIGNED?true:false,(yyvsp[(4) - (4)].expr)); ;}
    break;

  case 144:
#line 975 "../src/parse_tdf.y"
    { (yyval.expr)=(yyvsp[(1) - (1)].expr); ;}
    break;

  case 145:
#line 980 "../src/parse_tdf.y"
    {
			  Type *ti=(yyvsp[(1) - (3)].expr)->getType(),
			       *tf=(yyvsp[(3) - (3)].expr)->getType();
			  // possible unlinked param types (ti,tf=NULL)
			  if (ti && tf)
			  {
			    if (ti->isSigned() || tf->isSigned())
			      yyerror("fixed number requires unsigned parts");
			    // detect constant parts:
			    if ((yyvsp[(1) - (3)].expr)->getExprKind()==EXPR_VALUE &&
			        (yyvsp[(1) - (3)].expr)->getExprKind()==EXPR_VALUE &&
				ti->getTypeKind()==TYPE_INT   &&
				tf->getTypeKind()==TYPE_INT)
			    {
			      int	wi=ti->getWidth(),
					wf=tf->getWidth();
			      long long vi=((ExprValue*)(yyvsp[(1) - (3)].expr))->getIntVal(),
					vf=((ExprValue*)(yyvsp[(3) - (3)].expr))->getIntVal();
			      Type *t = new TypeFixed(wi,wf,false);
			      (yyval.expr)=(Expr*)new ExprValue((yyvsp[(2) - (3)].token),t,vi,vf);
			    }
			    else // non-const parts
			      (yyval.expr)=(Expr*)new ExprBop((yyvsp[(2) - (3)].token),'.',(yyvsp[(1) - (3)].expr),(yyvsp[(3) - (3)].expr));
			  }
			  else
			    (yyval.expr)=(Expr*)new ExprBop((yyvsp[(2) - (3)].token),'.',(yyvsp[(1) - (3)].expr),(yyvsp[(3) - (3)].expr));
			;}
    break;

  case 146:
#line 1008 "../src/parse_tdf.y"
    { (yyval.expr)=(yyvsp[(1) - (1)].expr); ;}
    break;

  case 147:
#line 1013 "../src/parse_tdf.y"
    { (yyval.expr)=(yyvsp[(1) - (1)].expr); ;}
    break;

  case 148:
#line 1015 "../src/parse_tdf.y"
    {
			  ExprLValue *lvalue=(ExprLValue*)(yyvsp[(1) - (3)].expr);
			  (yyval.expr)=new ExprLValue(lvalue->getToken(),
					    lvalue->getSymbol(),
					    lvalue->usesAllBits() ? NULL :
					      lvalue->getPosLow(),
					    lvalue->usesAllBits() ? NULL :
					      lvalue->getPosHigh(),
					    (yyvsp[(3) - (3)].expr),
					    lvalue->getArrayLoc());
			  // should just update token & retime directly in $1
			;}
    break;

  case 149:
#line 1028 "../src/parse_tdf.y"
    {
			  const char *tokenChars=(yyvsp[(1) - (1)].token)->str;
			  if (  tokenChars[0]=='0' &&
			       (tokenChars[1]=='b' || tokenChars[1]=='B'))
			    // binary "0b..."
			    (yyval.expr)=constIntExpr(strtoll((yyvsp[(1) - (1)].token)->str.del(0,1),NULL,2),
					    (yyvsp[(1) - (1)].token));
			  else
			    // decimal, octal, or hex
			    (yyval.expr)=constIntExpr(strtoll((yyvsp[(1) - (1)].token)->str,NULL,0),(yyvsp[(1) - (1)].token));
			;}
    break;

  case 150:
#line 1041 "../src/parse_tdf.y"
    { (yyval.expr)=(Expr*)new ExprValue((yyvsp[(1) - (1)].token),new Type(TYPE_BOOL),1); ;}
    break;

  case 151:
#line 1043 "../src/parse_tdf.y"
    { (yyval.expr)=(Expr*)new ExprValue((yyvsp[(1) - (1)].token),new Type(TYPE_BOOL),0); ;}
    break;

  case 152:
#line 1045 "../src/parse_tdf.y"
    { (yyval.expr)=(yyvsp[(1) - (1)].expr); ;}
    break;

  case 153:
#line 1047 "../src/parse_tdf.y"
    { (yyval.expr)=(yyvsp[(1) - (1)].expr); ;}
    break;

  case 154:
#line 1049 "../src/parse_tdf.y"
    { (yyval.expr)=(yyvsp[(2) - (3)].expr); ;}
    break;

  case 155:
#line 1054 "../src/parse_tdf.y"
    { (yyval.expr)=new ExprBuiltin((yyvsp[(1) - (4)].token),(yyvsp[(3) - (4)].args),builtin_cat); ;}
    break;

  case 156:
#line 1065 "../src/parse_tdf.y"
    {
			  list<Expr*> *args=new list<Expr*>;
			  args->append((yyvsp[(3) - (4)].expr));
			  (yyval.expr)=new ExprBuiltin((yyvsp[(1) - (4)].token),args,builtin_widthof);
			;}
    break;

  case 157:
#line 1071 "../src/parse_tdf.y"
    {
			  list<Expr*> *args=new list<Expr*>;
			  args->append((yyvsp[(3) - (4)].expr));
			  (yyval.expr)=new ExprBuiltin((yyvsp[(1) - (4)].token),args,builtin_bitsof);
			;}
    break;


/* Line 1267 of yacc.c.  */
#line 3013 "parse_tdf.tab.cc"
      default: break;
    }
  YY_SYMBOL_PRINT ("-> $$ =", yyr1[yyn], &yyval, &yyloc);

  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);

  *++yyvsp = yyval;


  /* Now `shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */

  yyn = yyr1[yyn];

  yystate = yypgoto[yyn - YYNTOKENS] + *yyssp;
  if (0 <= yystate && yystate <= YYLAST && yycheck[yystate] == *yyssp)
    yystate = yytable[yystate];
  else
    yystate = yydefgoto[yyn - YYNTOKENS];

  goto yynewstate;


/*------------------------------------.
| yyerrlab -- here on detecting error |
`------------------------------------*/
yyerrlab:
  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
#if ! YYERROR_VERBOSE
      yyerror (YY_("syntax error"));
#else
      {
	YYSIZE_T yysize = yysyntax_error (0, yystate, yychar);
	if (yymsg_alloc < yysize && yymsg_alloc < YYSTACK_ALLOC_MAXIMUM)
	  {
	    YYSIZE_T yyalloc = 2 * yysize;
	    if (! (yysize <= yyalloc && yyalloc <= YYSTACK_ALLOC_MAXIMUM))
	      yyalloc = YYSTACK_ALLOC_MAXIMUM;
	    if (yymsg != yymsgbuf)
	      YYSTACK_FREE (yymsg);
	    yymsg = (char *) YYSTACK_ALLOC (yyalloc);
	    if (yymsg)
	      yymsg_alloc = yyalloc;
	    else
	      {
		yymsg = yymsgbuf;
		yymsg_alloc = sizeof yymsgbuf;
	      }
	  }

	if (0 < yysize && yysize <= yymsg_alloc)
	  {
	    (void) yysyntax_error (yymsg, yystate, yychar);
	    yyerror (yymsg);
	  }
	else
	  {
	    yyerror (YY_("syntax error"));
	    if (yysize != 0)
	      goto yyexhaustedlab;
	  }
      }
#endif
    }



  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse look-ahead token after an
	 error, discard it.  */

      if (yychar <= YYEOF)
	{
	  /* Return failure if at end of input.  */
	  if (yychar == YYEOF)
	    YYABORT;
	}
      else
	{
	  yydestruct ("Error: discarding",
		      yytoken, &yylval);
	  yychar = YYEMPTY;
	}
    }

  /* Else will try to reuse look-ahead token after shifting the error
     token.  */
  goto yyerrlab1;


/*---------------------------------------------------.
| yyerrorlab -- error raised explicitly by YYERROR.  |
`---------------------------------------------------*/
yyerrorlab:

  /* Pacify compilers like GCC when the user code never invokes
     YYERROR and the label yyerrorlab therefore never appears in user
     code.  */
  if (/*CONSTCOND*/ 0)
     goto yyerrorlab;

  /* Do not reclaim the symbols of the rule which action triggered
     this YYERROR.  */
  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);
  yystate = *yyssp;
  goto yyerrlab1;


/*-------------------------------------------------------------.
| yyerrlab1 -- common code for both syntax error and YYERROR.  |
`-------------------------------------------------------------*/
yyerrlab1:
  yyerrstatus = 3;	/* Each real token shifted decrements this.  */

  for (;;)
    {
      yyn = yypact[yystate];
      if (yyn != YYPACT_NINF)
	{
	  yyn += YYTERROR;
	  if (0 <= yyn && yyn <= YYLAST && yycheck[yyn] == YYTERROR)
	    {
	      yyn = yytable[yyn];
	      if (0 < yyn)
		break;
	    }
	}

      /* Pop the current state because it cannot handle the error token.  */
      if (yyssp == yyss)
	YYABORT;


      yydestruct ("Error: popping",
		  yystos[yystate], yyvsp);
      YYPOPSTACK (1);
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  if (yyn == YYFINAL)
    YYACCEPT;

  *++yyvsp = yylval;


  /* Shift the error token.  */
  YY_SYMBOL_PRINT ("Shifting", yystos[yyn], yyvsp, yylsp);

  yystate = yyn;
  goto yynewstate;


/*-------------------------------------.
| yyacceptlab -- YYACCEPT comes here.  |
`-------------------------------------*/
yyacceptlab:
  yyresult = 0;
  goto yyreturn;

/*-----------------------------------.
| yyabortlab -- YYABORT comes here.  |
`-----------------------------------*/
yyabortlab:
  yyresult = 1;
  goto yyreturn;

#ifndef yyoverflow
/*-------------------------------------------------.
| yyexhaustedlab -- memory exhaustion comes here.  |
`-------------------------------------------------*/
yyexhaustedlab:
  yyerror (YY_("memory exhausted"));
  yyresult = 2;
  /* Fall through.  */
#endif

yyreturn:
  if (yychar != YYEOF && yychar != YYEMPTY)
     yydestruct ("Cleanup: discarding lookahead",
		 yytoken, &yylval);
  /* Do not reclaim the symbols of the rule which action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK (yylen);
  YY_STACK_PRINT (yyss, yyssp);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
		  yystos[*yyssp], yyvsp);
      YYPOPSTACK (1);
    }
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif
#if YYERROR_VERBOSE
  if (yymsg != yymsgbuf)
    YYSTACK_FREE (yymsg);
#endif
  /* Make sure YYID is used.  */
  return YYID (yyresult);
}


#line 1113 "../src/parse_tdf.y"


