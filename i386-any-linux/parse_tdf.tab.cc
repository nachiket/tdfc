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
     SQRT = 302,
     FLOOR = 303
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
#define FLOOR 303




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

using std::cout;
using std::endl;

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
/* Line 187 of yacc.c.  */
#line 388 "parse_tdf.tab.cc"
	YYSTYPE;
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif



/* Copy the second part of user declarations.  */


/* Line 216 of yacc.c.  */
#line 401 "parse_tdf.tab.cc"

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
#define YYFINAL  15
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   373

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  75
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  81
/* YYNRULES -- Number of rules.  */
#define YYNRULES  184
/* YYNRULES -- Number of states.  */
#define YYNSTATES  329

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   303

#define YYTRANSLATE(YYX)						\
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[YYLEX] -- Bison symbol number corresponding to YYLEX.  */
static const yytype_uint8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,    56,     2,    58,     2,    59,    61,     2,
      44,    46,    62,    54,    68,    53,    69,    63,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,    67,    66,
      51,    64,    52,    70,    57,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,    49,     2,    50,    60,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,    47,    65,    48,    55,     2,     2,     2,
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
      35,    36,    37,    38,    39,    40,    41,    42,    43,    45,
      71,    72,    73,    74
};

#if YYDEBUG
/* YYPRHS[YYN] -- Index of the first RHS symbol of rule number YYN in
   YYRHS.  */
static const yytype_uint16 yyprhs[] =
{
       0,     0,     3,     5,     7,     8,    11,    13,    14,    20,
      22,    23,    25,    27,    29,    32,    37,    43,    45,    47,
      51,    56,    58,    59,    63,    65,    69,    71,    73,    75,
      77,    78,    80,    81,    87,    88,    94,    98,   100,   101,
     104,   105,   108,   112,   114,   116,   120,   122,   123,   128,
     129,   135,   138,   140,   142,   144,   147,   152,   157,   160,
     163,   165,   173,   175,   176,   180,   182,   184,   188,   193,
     195,   196,   199,   201,   203,   205,   207,   209,   217,   221,
     224,   227,   230,   233,   235,   241,   249,   254,   259,   264,
     268,   274,   276,   278,   280,   282,   287,   292,   297,   302,
     307,   312,   317,   322,   324,   325,   328,   329,   333,   335,
     339,   341,   346,   353,   361,   371,   373,   377,   379,   380,
     384,   386,   392,   394,   398,   400,   404,   406,   410,   412,
     416,   418,   422,   424,   428,   430,   434,   436,   440,   442,
     446,   448,   452,   454,   457,   462,   467,   472,   477,   482,
     487,   489,   493,   495,   497,   501,   503,   505,   507,   509,
     511,   515,   520,   525,   530,   532,   534,   536,   538,   540,
     542,   544,   546,   548,   550,   552,   554,   556,   558,   560,
     562,   564,   566,   568,   570
};

/* YYRHS -- A `-1'-separated list of the rules' RHS.  */
static const yytype_int16 yyrhs[] =
{
      76,     0,    -1,    77,    -1,    78,    -1,    -1,    78,    79,
      -1,    79,    -1,    -1,    81,    85,    80,    92,    90,    -1,
      82,    -1,    -1,     5,    -1,     6,    -1,     7,    -1,     5,
      84,    -1,    83,    49,   128,    50,    -1,    83,    84,    49,
     128,    50,    -1,    38,    -1,    43,    -1,    49,   128,    50,
      -1,    18,    44,    86,    46,    -1,    87,    -1,    -1,    87,
      68,    88,    -1,    88,    -1,    89,    82,    18,    -1,    20,
      -1,    27,    -1,    28,    -1,    91,    -1,    -1,    29,    -1,
      -1,    47,    95,    93,   106,    48,    -1,    -1,    47,    95,
      94,   103,    48,    -1,    47,    95,    48,    -1,    96,    -1,
      -1,    96,    98,    -1,    -1,    97,    98,    -1,    82,    99,
      66,    -1,    66,    -1,   100,    -1,    99,    68,   100,    -1,
      18,    -1,    -1,    18,   101,    64,   128,    -1,    -1,    18,
     102,    44,   128,    46,    -1,   103,   104,    -1,   105,    -1,
     105,    -1,    66,    -1,   122,    66,    -1,   127,    64,   122,
      66,    -1,   127,    64,   127,    66,    -1,   119,    66,    -1,
     106,   107,    -1,   107,    -1,    39,    18,    44,   108,    46,
      67,   111,    -1,   109,    -1,    -1,   109,    68,   110,    -1,
     110,    -1,    18,    -1,    18,    58,   128,    -1,    13,    44,
      18,    46,    -1,   112,    -1,    -1,   112,   113,    -1,   114,
      -1,   114,    -1,    66,    -1,   115,    -1,   116,    -1,    19,
      44,   128,    46,   115,    12,   115,    -1,    16,    18,    66,
      -1,    40,    66,    -1,   126,    66,    -1,   118,    66,    -1,
     122,    66,    -1,   117,    -1,    19,    44,   128,    46,   113,
      -1,    19,    44,   128,    46,   115,    12,   116,    -1,    47,
      95,   111,    48,    -1,     3,    44,   128,    46,    -1,     9,
      44,    18,    46,    -1,    11,    44,    46,    -1,    30,    44,
      41,   124,    46,    -1,   120,    -1,   121,    -1,   120,    -1,
     121,    -1,    10,    44,   125,    46,    -1,    32,    44,   125,
      46,    -1,    37,    44,   125,    46,    -1,    33,    44,   125,
      46,    -1,    34,    44,   125,    46,    -1,    36,    44,   125,
      46,    -1,    35,    44,   125,    46,    -1,    18,    44,   123,
      46,    -1,   125,    -1,    -1,    68,   125,    -1,    -1,   125,
      68,   128,    -1,   128,    -1,   127,    64,   128,    -1,    18,
      -1,    18,    49,   128,    50,    -1,    18,    49,   128,    67,
     128,    50,    -1,    18,    49,   128,    50,    49,   128,    50,
      -1,    18,    49,   128,    50,    49,   128,    67,   128,    50,
      -1,   131,    -1,    47,   129,    48,    -1,   130,    -1,    -1,
     130,    68,   131,    -1,   131,    -1,   132,    70,   131,    67,
     131,    -1,   132,    -1,   132,    23,   133,    -1,   133,    -1,
     133,    22,   134,    -1,   134,    -1,   134,    65,   135,    -1,
     135,    -1,   135,    60,   136,    -1,   136,    -1,   136,    61,
     137,    -1,   137,    -1,   137,   146,   138,    -1,   138,    -1,
     138,   147,   139,    -1,   139,    -1,   139,   148,   140,    -1,
     140,    -1,   140,   149,   141,    -1,   141,    -1,   141,   154,
     142,    -1,   142,    -1,   155,   143,    -1,   150,    44,   128,
      46,    -1,   151,    44,   128,    46,    -1,   152,    44,   128,
      46,    -1,   153,    44,   128,    46,    -1,    44,    82,    46,
     143,    -1,    44,    83,    46,   143,    -1,   143,    -1,   144,
      69,   144,    -1,   144,    -1,   127,    -1,   127,    57,   144,
      -1,    26,    -1,    42,    -1,    15,    -1,   122,    -1,   145,
      -1,    44,   128,    46,    -1,     8,    44,   125,    46,    -1,
      45,    44,   128,    46,    -1,     4,    44,   128,    46,    -1,
      14,    -1,    25,    -1,    51,    -1,    52,    -1,    24,    -1,
      17,    -1,    21,    -1,    31,    -1,    54,    -1,    53,    -1,
      71,    -1,    72,    -1,    73,    -1,    74,    -1,    62,    -1,
      63,    -1,    59,    -1,    54,    -1,    53,    -1,    56,    -1,
      55,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const yytype_uint16 yyrline[] =
{
       0,   222,   222,   227,   230,   242,   247,   262,   261,   302,
     305,   309,   311,   313,   315,   317,   339,   368,   370,   375,
     394,   399,   402,   409,   415,   425,   437,   439,   441,   446,
     449,   453,   459,   458,   470,   469,   478,   487,   490,   498,
     501,   501,   510,   524,   529,   530,   535,   540,   539,   545,
     544,   551,   553,   558,   560,   565,   567,   569,   571,   580,
     585,   594,   612,   615,   619,   621,   626,   628,   630,   635,
     638,   642,   644,   649,   651,   656,   658,   663,   665,   667,
     669,   671,   673,   675,   680,   682,   687,   696,   704,   713,
     721,   729,   731,   736,   738,   743,   751,   760,   769,   778,
     787,   796,   808,   813,   816,   824,   827,   831,   833,   838,
     843,   856,   864,   866,   868,   873,   875,   883,   886,   890,
     892,   897,   899,   904,   906,   911,   913,   918,   920,   925,
     927,   932,   934,   939,   941,   946,   948,   953,   955,   960,
     962,   967,   969,   974,   976,   978,   980,   982,   984,   986,
     988,   995,  1023,  1028,  1030,  1043,  1056,  1058,  1060,  1062,
    1064,  1069,  1080,  1086,  1095,  1096,  1100,  1101,  1102,  1103,
    1107,  1108,  1112,  1113,  1117,  1121,  1125,  1129,  1133,  1134,
    1135,  1139,  1140,  1141,  1142
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || YYTOKEN_TABLE
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "ATTENTION", "BITSOF", "BOOLEAN",
  "FLOAT", "DOUBLE", "CAT", "CLOSE", "COPY", "DONE", "ELSE", "EOS",
  "EQUALS", "FALSE", "GOTO", "GTE", "ID_", "IF", "INPUT", "LEFT_SHIFT",
  "LOGIC_AND", "LOGIC_OR", "LTE", "NOT_EQUALS", "NUM", "OUTPUT", "PARAM",
  "PASS_THROUGH_EXCEPTION", "PRINTF", "RIGHT_SHIFT", "SEGMENT_R_",
  "SEGMENT_RW_", "SEGMENT_SEQ_R_", "SEGMENT_SEQ_RW_", "SEGMENT_SEQ_W_",
  "SEGMENT_W_", "SIGNED", "STATE", "STAY", "STRING", "TRUE", "UNSIGNED",
  "'('", "WIDTHOF", "')'", "'{'", "'}'", "'['", "']'", "'<'", "'>'", "'-'",
  "'+'", "'~'", "'!'", "'@'", "'#'", "'%'", "'^'", "'&'", "'*'", "'/'",
  "'='", "'|'", "';'", "':'", "','", "'.'", "'?'", "EXP", "LOG", "SQRT",
  "FLOOR", "$accept", "start", "operators_opt", "operators", "operator",
  "@1", "type_opt", "type", "sizedType", "arraySize", "opSignature",
  "ioDecls_opt", "ioDecls", "ioDecl", "ioKind", "exception_opt",
  "exception", "operatorBody", "@2", "@3", "typeDecls_opt", "typeDecls",
  "@4", "typeDecl", "typeDeclElems", "typeDeclElem", "@5", "@6",
  "callsOrAssigns", "callOrAssign", "callOrAssign_nonEmpty", "states",
  "stateCase", "stateVars_opt", "stateVars", "stateVar", "stmts_opt",
  "stmts", "stmt", "stmt_nonEmpty", "matchedStmt", "unmatchedStmt",
  "stmtBlock", "builtinStmtBehav", "builtinStmtCompose", "copyStmt",
  "segmentStmt", "call", "args_opt", "more_args_opt", "args", "assign",
  "lvalue", "expr", "condExprs_opt", "condExprs", "condExpr", "logOrExpr",
  "logAndExpr", "bitOrExpr", "bitXorExpr", "bitAndExpr", "equalExpr",
  "inequalExpr", "shiftExpr", "addExpr", "prodExpr", "unaryExpr",
  "fixedExpr", "atomExpr", "builtinExpr", "equalOp", "inequalOp",
  "shiftOp", "addOp", "exprOp", "logOp", "sqrtOp", "floorOp", "prodOp",
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
     295,   296,   297,   298,    40,   299,    41,   123,   125,    91,
      93,    60,    62,    45,    43,   126,    33,    64,    35,    37,
      94,    38,    42,    47,    61,   124,    59,    58,    44,    46,
      63,   300,   301,   302,   303
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint8 yyr1[] =
{
       0,    75,    76,    77,    77,    78,    78,    80,    79,    81,
      81,    82,    82,    82,    82,    82,    82,    83,    83,    84,
      85,    86,    86,    87,    87,    88,    89,    89,    89,    90,
      90,    91,    93,    92,    94,    92,    92,    95,    95,    96,
      97,    96,    98,    98,    99,    99,   100,   101,   100,   102,
     100,   103,   103,   104,   104,   105,   105,   105,   105,   106,
     106,   107,   108,   108,   109,   109,   110,   110,   110,   111,
     111,   112,   112,   113,   113,   114,   114,   115,   115,   115,
     115,   115,   115,   115,   116,   116,   117,   118,   118,   118,
     118,   118,   118,   119,   119,   120,   121,   121,   121,   121,
     121,   121,   122,   123,   123,   124,   124,   125,   125,   126,
     127,   127,   127,   127,   127,   128,   128,   129,   129,   130,
     130,   131,   131,   132,   132,   133,   133,   134,   134,   135,
     135,   136,   136,   137,   137,   138,   138,   139,   139,   140,
     140,   141,   141,   142,   142,   142,   142,   142,   142,   142,
     142,   143,   143,   144,   144,   144,   144,   144,   144,   144,
     144,   145,   145,   145,   146,   146,   147,   147,   147,   147,
     148,   148,   149,   149,   150,   151,   152,   153,   154,   154,
     154,   155,   155,   155,   155
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const yytype_uint8 yyr2[] =
{
       0,     2,     1,     1,     0,     2,     1,     0,     5,     1,
       0,     1,     1,     1,     2,     4,     5,     1,     1,     3,
       4,     1,     0,     3,     1,     3,     1,     1,     1,     1,
       0,     1,     0,     5,     0,     5,     3,     1,     0,     2,
       0,     2,     3,     1,     1,     3,     1,     0,     4,     0,
       5,     2,     1,     1,     1,     2,     4,     4,     2,     2,
       1,     7,     1,     0,     3,     1,     1,     3,     4,     1,
       0,     2,     1,     1,     1,     1,     1,     7,     3,     2,
       2,     2,     2,     1,     5,     7,     4,     4,     4,     3,
       5,     1,     1,     1,     1,     4,     4,     4,     4,     4,
       4,     4,     4,     1,     0,     2,     0,     3,     1,     3,
       1,     4,     6,     7,     9,     1,     3,     1,     0,     3,
       1,     5,     1,     3,     1,     3,     1,     3,     1,     3,
       1,     3,     1,     3,     1,     3,     1,     3,     1,     3,
       1,     3,     1,     2,     4,     4,     4,     4,     4,     4,
       1,     3,     1,     1,     3,     1,     1,     1,     1,     1,
       3,     4,     4,     4,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1
};

/* YYDEFACT[STATE-NAME] -- Default rule to reduce with in state
   STATE-NUM when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const yytype_uint8 yydefact[] =
{
       4,    11,    12,    13,    17,    18,     0,     2,     3,     6,
       0,     9,     0,     0,    14,     1,     5,     0,     7,     0,
       0,     0,     0,   157,   110,   155,   156,     0,     0,   118,
     182,   181,   184,   183,   174,   175,   176,   177,   158,   153,
       0,   115,   122,   124,   126,   128,   130,   132,   134,   136,
     138,   140,   142,   150,   152,   159,     0,     0,     0,     0,
       0,    22,     0,     0,     0,     0,     0,   104,     0,     0,
       0,     0,     0,     0,   117,   120,     0,    19,     0,     0,
       0,     0,     0,     0,   164,   165,     0,   169,   168,   166,
     167,     0,   170,   171,     0,   173,   172,     0,   180,   178,
     179,     0,     0,     0,     0,     0,     0,     0,   143,    26,
      27,    28,     0,    21,    24,     0,    38,    30,    15,     0,
       0,     0,   108,     0,   103,     0,     0,     0,   160,     0,
     116,     0,   154,   123,     0,   125,   127,   129,   131,   133,
     135,   137,   139,   141,   151,     0,     0,     0,     0,    20,
       0,     0,    34,    37,     0,    31,     8,    29,    16,   163,
     161,     0,   102,   111,     0,   148,   149,   162,   119,     0,
     144,   145,   146,   147,    23,    25,    36,     0,     0,    43,
       0,    39,    41,   107,     0,     0,   121,     0,     0,    60,
       0,     0,     0,     0,     0,     0,     0,     0,    52,     0,
      93,    94,     0,     0,    46,     0,    44,     0,   112,     0,
      33,    59,     0,     0,     0,     0,     0,     0,     0,    35,
      54,    51,    53,    58,    55,     0,     0,     0,    42,     0,
     113,     0,    63,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    45,     0,     0,    66,     0,    62,
      65,    95,    96,    98,    99,   101,   100,    97,    56,    57,
      48,     0,   114,     0,     0,     0,     0,    50,     0,    67,
      70,    64,    68,     0,     0,     0,     0,     0,     0,     0,
      38,    61,    69,    72,    75,    76,    83,     0,    91,    92,
       0,     0,     0,     0,     0,     0,     0,     0,     0,    79,
      70,    74,    71,    73,    81,    82,    80,     0,     0,     0,
      89,    78,     0,   106,     0,   109,    87,    88,     0,     0,
       0,    86,    84,    75,   105,    90,     0,    77,    85
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
      -1,     6,     7,     8,     9,    62,    10,    11,    12,    20,
      18,   112,   113,   114,   115,   156,   157,   117,   177,   178,
     152,   153,   154,   181,   205,   206,   226,   227,   197,   221,
     198,   188,   189,   248,   249,   250,   281,   282,   302,   283,
     284,   285,   286,   287,   199,   288,   289,    38,   123,   320,
     121,   291,    39,   122,    73,    74,    41,    42,    43,    44,
      45,    46,    47,    48,    49,    50,    51,    52,    53,    54,
      55,    86,    91,    94,    97,    56,    57,    58,    59,   101,
      60
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -279
static const yytype_int16 yypact[] =
{
       4,   -16,  -279,  -279,  -279,  -279,    56,  -279,     4,  -279,
      89,  -279,    64,    28,  -279,  -279,  -279,    81,  -279,    28,
      78,    95,   106,  -279,   -28,  -279,  -279,   168,   120,   201,
    -279,  -279,  -279,  -279,  -279,  -279,  -279,  -279,  -279,    92,
     115,  -279,   -22,   144,   114,   128,   130,    84,   146,   105,
     -36,   119,  -279,  -279,   111,  -279,   145,   148,   149,   151,
     245,   157,   152,   150,    28,    28,    28,    28,    28,   155,
     141,   156,    28,   159,   135,  -279,   245,  -279,   201,   201,
     201,   201,   201,   201,  -279,  -279,   201,  -279,  -279,  -279,
    -279,   201,  -279,  -279,   201,  -279,  -279,   201,  -279,  -279,
    -279,   201,   245,    28,    28,    28,    28,    28,  -279,  -279,
    -279,  -279,   158,   140,  -279,    72,    19,   167,   165,   170,
     171,   -34,  -279,   179,   160,    -6,   245,   245,  -279,   180,
    -279,   201,  -279,   144,   164,   114,   128,   130,    84,   146,
     105,   -36,   119,  -279,  -279,   186,   187,   188,   189,  -279,
     157,   218,    98,    22,    22,  -279,  -279,  -279,  -279,  -279,
    -279,    28,  -279,   195,    28,  -279,  -279,  -279,  -279,   201,
    -279,  -279,  -279,  -279,  -279,  -279,  -279,   198,    85,  -279,
     220,  -279,  -279,  -279,    28,   197,  -279,   230,    99,  -279,
     206,   208,   214,   217,   221,   222,   223,    96,  -279,   196,
    -279,  -279,   202,   200,   -14,     8,  -279,    37,  -279,   225,
    -279,  -279,    28,    28,    28,    28,    28,    28,    28,  -279,
    -279,  -279,  -279,  -279,  -279,   261,   224,   237,  -279,   220,
    -279,    28,   127,   -33,   -31,   -23,    -5,     3,    12,    18,
     216,   226,    28,    28,  -279,   241,   249,   238,   252,   227,
    -279,  -279,  -279,  -279,  -279,  -279,  -279,  -279,  -279,  -279,
    -279,   260,  -279,   290,    28,   242,   127,  -279,   264,  -279,
     302,  -279,  -279,   271,   272,   273,   301,   278,   279,   258,
      19,  -279,   267,  -279,  -279,  -279,  -279,   259,  -279,  -279,
     262,   263,   266,    28,   308,   281,   265,    28,   299,  -279,
     302,  -279,  -279,  -279,  -279,  -279,  -279,    28,   295,   297,
    -279,  -279,   298,   277,   300,  -279,  -279,  -279,   267,    28,
     304,  -279,  -279,   334,   160,  -279,   302,  -279,  -279
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -279,  -279,  -279,  -279,   339,  -279,  -279,   -19,   324,   351,
    -279,  -279,  -279,   203,  -279,  -279,  -279,  -279,  -279,  -279,
      74,  -279,  -279,   204,  -279,   126,  -279,  -279,  -279,  -279,
     162,  -279,   169,  -279,  -279,    90,    60,  -279,    43,  -278,
    -157,    36,  -279,  -279,  -279,   -89,   -81,  -159,  -279,  -279,
     -60,  -279,  -158,   -13,  -279,  -279,   -26,  -279,   285,   284,
     286,   283,   287,   280,   282,   274,   275,   268,   -58,   -71,
    -279,  -279,  -279,  -279,  -279,  -279,  -279,  -279,  -279,  -279,
    -279
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -50
static const yytype_int16 yytable[] =
{
      40,    78,   108,    75,   303,   132,    63,   124,    69,     1,
       2,     3,   160,   251,    71,   252,    67,    95,    96,   202,
     203,    68,   -10,   253,   -40,   -40,   -40,     1,     2,     3,
     -49,   144,    21,    13,   161,   161,    22,   161,   202,   203,
     303,   254,     4,    23,   163,   161,    24,     5,    79,   255,
     -47,   119,   120,   134,    25,   125,    15,   -40,   256,   129,
       4,   164,   -40,   161,   257,     5,   240,   241,   165,   166,
      26,   161,    27,    28,   228,    29,   229,     1,     2,     3,
     161,    30,    31,    32,    33,   -40,   161,   230,   179,   200,
     145,   146,   147,   148,    71,   190,   151,   201,    84,    34,
      35,    36,    37,    24,   231,   168,   190,    17,   200,    85,
       4,   290,   292,    19,    24,     5,   201,   191,   192,   193,
     194,   195,   196,   290,   292,    61,    92,    64,   191,   192,
     193,   194,   195,   196,   180,   180,    93,   -32,   187,    65,
     246,   290,   292,   186,   219,   247,   176,   210,   183,    76,
      66,   185,   233,   234,   235,   236,   237,   238,   239,   290,
     292,   323,   220,    87,    72,    77,    80,   290,   292,   327,
      88,   207,    21,     1,     2,     3,    22,   109,    98,    81,
     102,    99,   100,    23,   110,   111,    24,   127,    82,   103,
      19,    83,   104,   105,    25,   106,   155,    89,    90,   116,
     118,   126,   128,   131,   149,    21,     4,   130,   150,    22,
      26,     5,    27,    28,   -19,    29,    23,   159,   245,    24,
     158,    30,    31,    32,    33,   162,   167,    25,   161,   260,
     261,   169,   170,   171,   172,   173,   175,   187,   204,    34,
      35,    36,    37,    26,   184,    27,    28,   208,   209,    21,
     212,   269,   213,    22,    30,    31,    32,    33,   214,   324,
      23,   215,   223,    24,   225,   216,   217,   218,   224,   232,
     273,    25,    34,    35,    36,    37,   274,   190,   275,    24,
     308,   243,   258,   276,   312,    24,   277,    26,   242,   107,
      28,   262,   259,   263,   315,   266,   264,   278,   265,   191,
     192,   193,   194,   195,   196,   273,   267,   279,   268,   270,
     272,   274,   190,   275,   280,   293,   294,   295,   276,   296,
      24,   277,   297,   298,   299,   304,   309,   310,   305,   306,
     307,   311,   278,   301,   191,   192,   193,   194,   195,   196,
     313,   316,   279,   317,   318,   319,   326,    16,   321,   280,
     325,    70,    14,   174,   300,   244,   271,   211,   182,   222,
     314,   322,   328,   133,   135,   137,   139,   136,   141,   143,
     138,     0,   142,   140
};

static const yytype_int16 yycheck[] =
{
      13,    23,    60,    29,   282,    76,    19,    67,    27,     5,
       6,     7,    46,    46,    27,    46,    44,    53,    54,   178,
     178,    49,    18,    46,     5,     6,     7,     5,     6,     7,
      44,   102,     4,    49,    68,    68,     8,    68,   197,   197,
     318,    46,    38,    15,    50,    68,    18,    43,    70,    46,
      64,    64,    65,    79,    26,    68,     0,    38,    46,    72,
      38,    67,    43,    68,    46,    43,   225,   225,   126,   127,
      42,    68,    44,    45,    66,    47,    68,     5,     6,     7,
      68,    53,    54,    55,    56,    66,    68,    50,    66,   178,
     103,   104,   105,   106,   107,    10,   115,   178,    14,    71,
      72,    73,    74,    18,    67,   131,    10,    18,   197,    25,
      38,   270,   270,    49,    18,    43,   197,    32,    33,    34,
      35,    36,    37,   282,   282,    44,    21,    49,    32,    33,
      34,    35,    36,    37,   153,   154,    31,    39,    39,    44,
      13,   300,   300,   169,    48,    18,    48,    48,   161,    57,
      44,   164,   212,   213,   214,   215,   216,   217,   218,   318,
     318,   318,    66,    17,    44,    50,    22,   326,   326,   326,
      24,   184,     4,     5,     6,     7,     8,    20,    59,    65,
      69,    62,    63,    15,    27,    28,    18,    46,    60,    44,
      49,    61,    44,    44,    26,    44,    29,    51,    52,    47,
      50,    46,    46,    68,    46,     4,    38,    48,    68,     8,
      42,    43,    44,    45,    49,    47,    15,    46,   231,    18,
      50,    53,    54,    55,    56,    46,    46,    26,    68,   242,
     243,    67,    46,    46,    46,    46,    18,    39,    18,    71,
      72,    73,    74,    42,    49,    44,    45,    50,    18,     4,
      44,   264,    44,     8,    53,    54,    55,    56,    44,   319,
      15,    44,    66,    18,    64,    44,    44,    44,    66,    44,
       3,    26,    71,    72,    73,    74,     9,    10,    11,    18,
     293,    44,    66,    16,   297,    18,    19,    42,    64,    44,
      45,    50,    66,    44,   307,    68,    58,    30,    46,    32,
      33,    34,    35,    36,    37,     3,    46,    40,    18,    67,
      46,     9,    10,    11,    47,    44,    44,    44,    16,    18,
      18,    19,    44,    44,    66,    66,    18,    46,    66,    66,
      64,    66,    30,    66,    32,    33,    34,    35,    36,    37,
      41,    46,    40,    46,    46,    68,    12,     8,    48,    47,
      46,    27,     1,   150,   280,   229,   266,   188,   154,   197,
     300,   318,   326,    78,    80,    82,    86,    81,    94,   101,
      83,    -1,    97,    91
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,     5,     6,     7,    38,    43,    76,    77,    78,    79,
      81,    82,    83,    49,    84,     0,    79,    18,    85,    49,
      84,     4,     8,    15,    18,    26,    42,    44,    45,    47,
      53,    54,    55,    56,    71,    72,    73,    74,   122,   127,
     128,   131,   132,   133,   134,   135,   136,   137,   138,   139,
     140,   141,   142,   143,   144,   145,   150,   151,   152,   153,
     155,    44,    80,   128,    49,    44,    44,    44,    49,    82,
      83,   128,    44,   129,   130,   131,    57,    50,    23,    70,
      22,    65,    60,    61,    14,    25,   146,    17,    24,    51,
      52,   147,    21,    31,   148,    53,    54,   149,    59,    62,
      63,   154,    69,    44,    44,    44,    44,    44,   143,    20,
      27,    28,    86,    87,    88,    89,    47,    92,    50,   128,
     128,   125,   128,   123,   125,   128,    46,    46,    46,   128,
      48,    68,   144,   133,   131,   134,   135,   136,   137,   138,
     139,   140,   141,   142,   144,   128,   128,   128,   128,    46,
      68,    82,    95,    96,    97,    29,    90,    91,    50,    46,
      46,    68,    46,    50,    67,   143,   143,    46,   131,    67,
      46,    46,    46,    46,    88,    18,    48,    93,    94,    66,
      82,    98,    98,   128,    49,   128,   131,    39,   106,   107,
      10,    32,    33,    34,    35,    36,    37,   103,   105,   119,
     120,   121,   122,   127,    18,    99,   100,   128,    50,    18,
      48,   107,    44,    44,    44,    44,    44,    44,    44,    48,
      66,   104,   105,    66,    66,    64,   101,   102,    66,    68,
      50,    67,    44,   125,   125,   125,   125,   125,   125,   125,
     122,   127,    64,    44,   100,   128,    13,    18,   108,   109,
     110,    46,    46,    46,    46,    46,    46,    46,    66,    66,
     128,   128,    50,    44,    58,    46,    68,    46,    18,   128,
      67,   110,    46,     3,     9,    11,    16,    19,    30,    40,
      47,   111,   112,   114,   115,   116,   117,   118,   120,   121,
     122,   126,   127,    44,    44,    44,    18,    44,    44,    66,
      95,    66,   113,   114,    66,    66,    66,    64,   128,    18,
      46,    66,   128,    41,   111,   128,    46,    46,    46,    68,
     124,    48,   113,   115,   125,    46,    12,   115,   116
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
#line 223 "../src/parse_tdf.y"
    { (yyval.suite)=(yyvsp[(1) - (1)].suite); ;}
    break;

  case 3:
#line 228 "../src/parse_tdf.y"
    { (yyval.suite)=(yyvsp[(1) - (1)].suite); ;}
    break;

  case 4:
#line 230 "../src/parse_tdf.y"
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
#line 243 "../src/parse_tdf.y"
    {
			  (yyval.suite)=(yyvsp[(1) - (2)].suite);
			  (yyval.suite)->addOperator((yyvsp[(2) - (2)].operatr));
			;}
    break;

  case 6:
#line 248 "../src/parse_tdf.y"
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
#line 262 "../src/parse_tdf.y"
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
#line 278 "../src/parse_tdf.y"
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
#line 303 "../src/parse_tdf.y"
    { (yyval.type)=(yyvsp[(1) - (1)].type); ;}
    break;

  case 10:
#line 305 "../src/parse_tdf.y"
    { (yyval.type)=NULL; ;}
    break;

  case 11:
#line 310 "../src/parse_tdf.y"
    { (yyval.type)=new Type(TYPE_BOOL); ;}
    break;

  case 12:
#line 312 "../src/parse_tdf.y"
    { (yyval.type)=new Type(TYPE_FLOAT); ;}
    break;

  case 13:
#line 314 "../src/parse_tdf.y"
    { (yyval.type)=new Type(TYPE_DOUBLE); ;}
    break;

  case 14:
#line 316 "../src/parse_tdf.y"
    { (yyval.type)=new TypeArray(new Type(TYPE_BOOL),(yyvsp[(2) - (2)].expr)); ;}
    break;

  case 15:
#line 318 "../src/parse_tdf.y"
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

  case 16:
#line 340 "../src/parse_tdf.y"
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

  case 17:
#line 369 "../src/parse_tdf.y"
    { (yyval.token)=(yyvsp[(1) - (1)].token); ;}
    break;

  case 18:
#line 371 "../src/parse_tdf.y"
    { (yyval.token)=(yyvsp[(1) - (1)].token); ;}
    break;

  case 19:
#line 376 "../src/parse_tdf.y"
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

  case 20:
#line 395 "../src/parse_tdf.y"
    { (yyval.opSignature)=new OpSignature((yyvsp[(1) - (4)].token),(yyvsp[(1) - (4)].token)->str,(yyvsp[(3) - (4)].symList)); ;}
    break;

  case 21:
#line 400 "../src/parse_tdf.y"
    { (yyval.symList)=(yyvsp[(1) - (1)].symList); ;}
    break;

  case 22:
#line 402 "../src/parse_tdf.y"
    {
			  (yyval.symList)=new list<Symbol*>;
			  symtabPush(new SymTab(SYMTAB_OP));
			;}
    break;

  case 23:
#line 410 "../src/parse_tdf.y"
    {
			  (yyval.symList)=(yyvsp[(1) - (3)].symList);
			  (yyval.symList)->append((yyvsp[(3) - (3)].sym));
			  gSymtab->addSymbol((yyvsp[(3) - (3)].sym));
			;}
    break;

  case 24:
#line 416 "../src/parse_tdf.y"
    {
			  (yyval.symList)=new list<Symbol*>;
			  (yyval.symList)->append((yyvsp[(1) - (1)].sym));
			  symtabPush(new SymTab(SYMTAB_OP));
			  gSymtab->addSymbol((yyvsp[(1) - (1)].sym));
			;}
    break;

  case 25:
#line 426 "../src/parse_tdf.y"
    {
			  if ((yyvsp[(1) - (3)].token)->code==INPUT)
			    (yyval.sym)=new SymbolStream((yyvsp[(3) - (3)].token),(yyvsp[(3) - (3)].token)->str,(yyvsp[(2) - (3)].type),STREAM_IN);
			  else if ((yyvsp[(1) - (3)].token)->code==OUTPUT)
			    (yyval.sym)=new SymbolStream((yyvsp[(3) - (3)].token),(yyvsp[(3) - (3)].token)->str,(yyvsp[(2) - (3)].type),STREAM_OUT);
			  else  // ($1->code==PARAM)
			    (yyval.sym)=new SymbolVar((yyvsp[(3) - (3)].token),(yyvsp[(3) - (3)].token)->str,(yyvsp[(2) - (3)].type));
			;}
    break;

  case 26:
#line 438 "../src/parse_tdf.y"
    { (yyval.token)=(yyvsp[(1) - (1)].token); ;}
    break;

  case 27:
#line 440 "../src/parse_tdf.y"
    { (yyval.token)=(yyvsp[(1) - (1)].token); ;}
    break;

  case 28:
#line 442 "../src/parse_tdf.y"
    { (yyval.token)=(yyvsp[(1) - (1)].token); ;}
    break;

  case 29:
#line 447 "../src/parse_tdf.y"
    { (yyval.token)=(yyvsp[(1) - (1)].token); ;}
    break;

  case 30:
#line 449 "../src/parse_tdf.y"
    { (yyval.token)=NULL; ;}
    break;

  case 31:
#line 454 "../src/parse_tdf.y"
    { (yyval.token)=(yyvsp[(1) - (1)].token); ;}
    break;

  case 32:
#line 459 "../src/parse_tdf.y"
    {
			  // typeDecls_opt leaves pushed symtab
			  gStates=new dictionary<string,State*>;
			;}
    break;

  case 33:
#line 464 "../src/parse_tdf.y"
    {
			  (yyval.opBody)=(OpBody*)new BehavOpBody(gSymtab,gStates,(yyvsp[(4) - (5)].states));
			  symtabPop();
			  gStates=NULL;
			;}
    break;

  case 34:
#line 470 "../src/parse_tdf.y"
    {
			  // typeDecls_opt leaves pushed symtab
			;}
    break;

  case 35:
#line 474 "../src/parse_tdf.y"
    {
			  (yyval.opBody)=(OpBody*)new ComposeOpBody(gSymtab,(yyvsp[(4) - (5)].stmts));
			  symtabPop();
			;}
    break;

  case 36:
#line 479 "../src/parse_tdf.y"
    {
			  // typeDecls_opt leaves pushed symtab
			  delete symtabPop();
			  yyerror("empty operator body");
			;}
    break;

  case 37:
#line 488 "../src/parse_tdf.y"
    { (yyval.symtab)=(yyvsp[(1) - (1)].symtab); ;}
    break;

  case 38:
#line 490 "../src/parse_tdf.y"
    {
			  // push new empty symtab before processing first decl
			  symtabPush(new SymTab(SYMTAB_BLOCK));
			  (yyval.symtab)=gSymtab;
			;}
    break;

  case 39:
#line 499 "../src/parse_tdf.y"
    { (yyval.symtab)=gSymtab; ;}
    break;

  case 40:
#line 501 "../src/parse_tdf.y"
    {
			  // push new empty symtab before processing first decl
			  symtabPush(new SymTab(SYMTAB_BLOCK));
			;}
    break;

  case 41:
#line 506 "../src/parse_tdf.y"
    { (yyval.symtab)=gSymtab; ;}
    break;

  case 42:
#line 511 "../src/parse_tdf.y"
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

  case 43:
#line 525 "../src/parse_tdf.y"
    { (yyval.symtab)=gSymtab; ;}
    break;

  case 44:
#line 529 "../src/parse_tdf.y"
    { (yyval.typeDeclElems)=new list<TypeDeclElem*>; (yyval.typeDeclElems)->append((yyvsp[(1) - (1)].typeDeclElem)); ;}
    break;

  case 45:
#line 531 "../src/parse_tdf.y"
    { (yyval.typeDeclElems)=(yyvsp[(1) - (3)].typeDeclElems);                      (yyval.typeDeclElems)->append((yyvsp[(3) - (3)].typeDeclElem)); ;}
    break;

  case 46:
#line 535 "../src/parse_tdf.y"
    {
			  lookupDuplicateDecl((yyvsp[(1) - (1)].token));
			  (yyval.typeDeclElem)=new TypeDeclElem((yyvsp[(1) - (1)].token),NULL,NULL);
			;}
    break;

  case 47:
#line 540 "../src/parse_tdf.y"
    { lookupDuplicateDecl((yyvsp[(1) - (1)].token)); ;}
    break;

  case 48:
#line 542 "../src/parse_tdf.y"
    { (yyval.typeDeclElem)=new TypeDeclElem((yyvsp[(1) - (4)].token),(yyvsp[(4) - (4)].expr),NULL); ;}
    break;

  case 49:
#line 545 "../src/parse_tdf.y"
    { lookupDuplicateDecl((yyvsp[(1) - (1)].token)); ;}
    break;

  case 50:
#line 547 "../src/parse_tdf.y"
    { (yyval.typeDeclElem)=new TypeDeclElem((yyvsp[(1) - (5)].token),NULL,(yyvsp[(4) - (5)].expr)); ;}
    break;

  case 51:
#line 552 "../src/parse_tdf.y"
    { (yyval.stmts)=(yyvsp[(1) - (2)].stmts);		if ((yyvsp[(2) - (2)].stmt)) (yyval.stmts)->append((yyvsp[(2) - (2)].stmt)); ;}
    break;

  case 52:
#line 554 "../src/parse_tdf.y"
    { (yyval.stmts)=new list<Stmt*>;	if ((yyvsp[(1) - (1)].stmt)) (yyval.stmts)->append((yyvsp[(1) - (1)].stmt)); ;}
    break;

  case 53:
#line 559 "../src/parse_tdf.y"
    { (yyval.stmt)=(yyvsp[(1) - (1)].stmt); ;}
    break;

  case 54:
#line 561 "../src/parse_tdf.y"
    { (yyval.stmt)=NULL; ;}
    break;

  case 55:
#line 566 "../src/parse_tdf.y"
    { (yyval.stmt)=new StmtCall((yyvsp[(1) - (2)].expr)->getToken(),(ExprCall*)(yyvsp[(1) - (2)].expr)); ;}
    break;

  case 56:
#line 568 "../src/parse_tdf.y"
    { (yyval.stmt)=new StmtAssign((yyvsp[(2) - (4)].token),(ExprLValue*)(yyvsp[(1) - (4)].expr),(yyvsp[(3) - (4)].expr)); ;}
    break;

  case 57:
#line 570 "../src/parse_tdf.y"
    { (yyval.stmt)=new StmtAssign((yyvsp[(2) - (4)].token),(ExprLValue*)(yyvsp[(1) - (4)].expr),(yyvsp[(3) - (4)].expr)); ;}
    break;

  case 58:
#line 572 "../src/parse_tdf.y"
    { (yyval.stmt)=(yyvsp[(1) - (2)].stmt); ;}
    break;

  case 59:
#line 581 "../src/parse_tdf.y"
    {
			  (yyval.states)=(yyvsp[(1) - (2)].states);			// $$ = start state
			  // states are stored in gStates by stateCase action
			;}
    break;

  case 60:
#line 586 "../src/parse_tdf.y"
    {
			  (yyval.states)=(State*)((yyvsp[(1) - (1)].stateCase)->getParent());	// $$ = start state
			  // states are stored in gStates by stateCase action
			;}
    break;

  case 61:
#line 595 "../src/parse_tdf.y"
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

  case 62:
#line 613 "../src/parse_tdf.y"
    { (yyval.inputSpecs)=(yyvsp[(1) - (1)].inputSpecs); ;}
    break;

  case 63:
#line 615 "../src/parse_tdf.y"
    { (yyval.inputSpecs)=new list<InputSpec*>; ;}
    break;

  case 64:
#line 620 "../src/parse_tdf.y"
    { (yyval.inputSpecs)=(yyvsp[(1) - (3)].inputSpecs);			(yyval.inputSpecs)->append((yyvsp[(3) - (3)].inputSpec)); ;}
    break;

  case 65:
#line 622 "../src/parse_tdf.y"
    { (yyval.inputSpecs)=new list<InputSpec*>;	(yyval.inputSpecs)->append((yyvsp[(1) - (1)].inputSpec)); ;}
    break;

  case 66:
#line 627 "../src/parse_tdf.y"
    { (yyval.inputSpec)=new InputSpec((yyvsp[(1) - (1)].token),lookup((yyvsp[(1) - (1)].token))); ;}
    break;

  case 67:
#line 629 "../src/parse_tdf.y"
    { (yyval.inputSpec)=new InputSpec((yyvsp[(1) - (3)].token),lookup((yyvsp[(1) - (3)].token)),(yyvsp[(3) - (3)].expr)); ;}
    break;

  case 68:
#line 631 "../src/parse_tdf.y"
    { (yyval.inputSpec)=new InputSpec((yyvsp[(3) - (4)].token),lookup((yyvsp[(3) - (4)].token)),NULL,true); ;}
    break;

  case 69:
#line 636 "../src/parse_tdf.y"
    { (yyval.stmts)=(yyvsp[(1) - (1)].stmts); ;}
    break;

  case 70:
#line 638 "../src/parse_tdf.y"
    { (yyval.stmts)=new list<Stmt*>; ;}
    break;

  case 71:
#line 643 "../src/parse_tdf.y"
    { (yyval.stmts)=(yyvsp[(1) - (2)].stmts);		if ((yyvsp[(2) - (2)].stmt)) (yyval.stmts)->append((yyvsp[(2) - (2)].stmt)); ;}
    break;

  case 72:
#line 645 "../src/parse_tdf.y"
    { (yyval.stmts)=new list<Stmt*>;	if ((yyvsp[(1) - (1)].stmt)) (yyval.stmts)->append((yyvsp[(1) - (1)].stmt)); ;}
    break;

  case 73:
#line 650 "../src/parse_tdf.y"
    { (yyval.stmt)=(yyvsp[(1) - (1)].stmt); ;}
    break;

  case 74:
#line 652 "../src/parse_tdf.y"
    { (yyval.stmt)=NULL; ;}
    break;

  case 75:
#line 657 "../src/parse_tdf.y"
    { (yyval.stmt)=(yyvsp[(1) - (1)].stmt); ;}
    break;

  case 76:
#line 659 "../src/parse_tdf.y"
    { (yyval.stmt)=(yyvsp[(1) - (1)].stmt); ;}
    break;

  case 77:
#line 664 "../src/parse_tdf.y"
    { (yyval.stmt)=new StmtIf((yyvsp[(1) - (7)].token),(yyvsp[(3) - (7)].expr),(yyvsp[(5) - (7)].stmt),(yyvsp[(7) - (7)].stmt)); ;}
    break;

  case 78:
#line 666 "../src/parse_tdf.y"
    { (yyval.stmt)=new StmtGoto((yyvsp[(1) - (3)].token),(yyvsp[(2) - (3)].token)->str); ;}
    break;

  case 79:
#line 668 "../src/parse_tdf.y"
    { (yyval.stmt)=new StmtGoto((yyvsp[(1) - (2)].token),NULL); ;}
    break;

  case 80:
#line 670 "../src/parse_tdf.y"
    { (yyval.stmt)=(yyvsp[(1) - (2)].stmt); ;}
    break;

  case 81:
#line 672 "../src/parse_tdf.y"
    { (yyval.stmt)=(yyvsp[(1) - (2)].stmt); ;}
    break;

  case 82:
#line 674 "../src/parse_tdf.y"
    { (yyval.stmt)=new StmtCall((yyvsp[(1) - (2)].expr)->getToken(),(ExprCall*)(yyvsp[(1) - (2)].expr)); ;}
    break;

  case 83:
#line 676 "../src/parse_tdf.y"
    { (yyval.stmt)=(yyvsp[(1) - (1)].stmt); ;}
    break;

  case 84:
#line 681 "../src/parse_tdf.y"
    { (yyval.stmt)=new StmtIf((yyvsp[(1) - (5)].token),(yyvsp[(3) - (5)].expr),(yyvsp[(5) - (5)].stmt)); ;}
    break;

  case 85:
#line 683 "../src/parse_tdf.y"
    { (yyval.stmt)=new StmtIf((yyvsp[(1) - (7)].token),(yyvsp[(3) - (7)].expr),(yyvsp[(5) - (7)].stmt),(yyvsp[(7) - (7)].stmt)); ;}
    break;

  case 86:
#line 688 "../src/parse_tdf.y"
    {
			  // typeDecls_opt leaves pushed symtab
			  (yyval.stmt)=new StmtBlock((yyvsp[(1) - (4)].token),gSymtab,(yyvsp[(3) - (4)].stmts));
			  symtabPop();
			;}
    break;

  case 87:
#line 697 "../src/parse_tdf.y"
    {
			  list<Expr*> *args=new list<Expr*>;
			  args->append((yyvsp[(3) - (4)].expr));
			  ExprBuiltin *eb=new ExprBuiltin((yyvsp[(1) - (4)].token),args,
							  builtin_attn);
			  (yyval.stmt)=new StmtBuiltin((yyvsp[(1) - (4)].token),eb);
			;}
    break;

  case 88:
#line 705 "../src/parse_tdf.y"
    {
			  ExprLValue *el=new ExprLValue((yyvsp[(3) - (4)].token),lookup((yyvsp[(3) - (4)].token)));
			  list<Expr*> *args=new list<Expr*>;
			  args->append(el);
			  ExprBuiltin *eb=new ExprBuiltin((yyvsp[(1) - (4)].token),args,
							  builtin_close);
			  (yyval.stmt)=new StmtBuiltin((yyvsp[(1) - (4)].token),eb);
			;}
    break;

  case 89:
#line 714 "../src/parse_tdf.y"
    {
			  list<Expr*> *args=new list<Expr*>;
			  args->append((Expr*)expr_0->duplicate());   // sync 0
			  ExprBuiltin *eb=new ExprBuiltin((yyvsp[(1) - (3)].token),args,
							  builtin_done);
			  (yyval.stmt)=new StmtBuiltin((yyvsp[(1) - (3)].token),eb);
			;}
    break;

  case 90:
#line 722 "../src/parse_tdf.y"
    {
			  list<Expr*> *args=(yyvsp[(4) - (5)].args);
			  ExprBuiltin *eb=new ExprBuiltin((yyvsp[(1) - (5)].token),args,
							  builtin_printf);
			  eb->setAnnote_(ANNOTE_PRINTF_STRING_TOKEN,(yyvsp[(3) - (5)].token));
			  (yyval.stmt)=new StmtBuiltin((yyvsp[(1) - (5)].token),eb);
			;}
    break;

  case 91:
#line 730 "../src/parse_tdf.y"
    { (yyval.stmt)=(yyvsp[(1) - (1)].stmt); ;}
    break;

  case 92:
#line 732 "../src/parse_tdf.y"
    { (yyval.stmt)=(yyvsp[(1) - (1)].stmt); ;}
    break;

  case 93:
#line 737 "../src/parse_tdf.y"
    { (yyval.stmt)=(yyvsp[(1) - (1)].stmt); ;}
    break;

  case 94:
#line 739 "../src/parse_tdf.y"
    { (yyval.stmt)=(yyvsp[(1) - (1)].stmt); ;}
    break;

  case 95:
#line 744 "../src/parse_tdf.y"
    {
			  ExprBuiltin *eb=new ExprBuiltin((yyvsp[(1) - (4)].token),(yyvsp[(3) - (4)].args),builtin_copy);
			  (yyval.stmt)=new StmtBuiltin((yyvsp[(1) - (4)].token),eb);
			;}
    break;

  case 96:
#line 752 "../src/parse_tdf.y"
    {
			  OperatorSegment *o
			    = new OperatorSegment((yyvsp[(1) - (4)].token),string("_dummy_seg_op"),
						  SEGMENT_R,
						  type_none,new list<Symbol*>);
			  ExprBuiltin *e=new ExprBuiltin((yyvsp[(1) - (4)].token),(yyvsp[(3) - (4)].args),o);
			  (yyval.stmt)=new StmtBuiltin((yyvsp[(1) - (4)].token),e);
			;}
    break;

  case 97:
#line 761 "../src/parse_tdf.y"
    {
			  OperatorSegment *o
			    = new OperatorSegment((yyvsp[(1) - (4)].token),string("_dummy_seg_op"),
						  SEGMENT_W,
						  type_none,new list<Symbol*>);
			  ExprBuiltin *e=new ExprBuiltin((yyvsp[(1) - (4)].token),(yyvsp[(3) - (4)].args),o);
			  (yyval.stmt)=new StmtBuiltin((yyvsp[(1) - (4)].token),e);
			;}
    break;

  case 98:
#line 770 "../src/parse_tdf.y"
    {
			  OperatorSegment *o
			    = new OperatorSegment((yyvsp[(1) - (4)].token),string("_dummy_seg_op"),
						  SEGMENT_RW,
						  type_none,new list<Symbol*>);
			  ExprBuiltin *e=new ExprBuiltin((yyvsp[(1) - (4)].token),(yyvsp[(3) - (4)].args),o);
			  (yyval.stmt)=new StmtBuiltin((yyvsp[(1) - (4)].token),e);
			;}
    break;

  case 99:
#line 779 "../src/parse_tdf.y"
    {
			  OperatorSegment *o
			    = new OperatorSegment((yyvsp[(1) - (4)].token),string("_dummy_seg_op"),
						  SEGMENT_SEQ_R,
						  type_none,new list<Symbol*>);
			  ExprBuiltin *e=new ExprBuiltin((yyvsp[(1) - (4)].token),(yyvsp[(3) - (4)].args),o);
			  (yyval.stmt)=new StmtBuiltin((yyvsp[(1) - (4)].token),e);
			;}
    break;

  case 100:
#line 788 "../src/parse_tdf.y"
    {
			  OperatorSegment *o
			    = new OperatorSegment((yyvsp[(1) - (4)].token),string("_dummy_seg_op"),
						  SEGMENT_SEQ_W,
						  type_none,new list<Symbol*>);
			  ExprBuiltin *e=new ExprBuiltin((yyvsp[(1) - (4)].token),(yyvsp[(3) - (4)].args),o);
			  (yyval.stmt)=new StmtBuiltin((yyvsp[(1) - (4)].token),e);
			;}
    break;

  case 101:
#line 797 "../src/parse_tdf.y"
    {
			  OperatorSegment *o
			    = new OperatorSegment((yyvsp[(1) - (4)].token),string("_dummy_seg_op"),
						  SEGMENT_SEQ_RW,
						  type_none,new list<Symbol*>);
			  ExprBuiltin *e=new ExprBuiltin((yyvsp[(1) - (4)].token),(yyvsp[(3) - (4)].args),o);
			  (yyval.stmt)=new StmtBuiltin((yyvsp[(1) - (4)].token),e);
			;}
    break;

  case 102:
#line 809 "../src/parse_tdf.y"
    { (yyval.expr)=new ExprCall((yyvsp[(1) - (4)].token),(yyvsp[(3) - (4)].args)); ;}
    break;

  case 103:
#line 814 "../src/parse_tdf.y"
    { (yyval.args)=(yyvsp[(1) - (1)].args); ;}
    break;

  case 104:
#line 816 "../src/parse_tdf.y"
    {
			  // empty args list implies 0-width sync stream
			  (yyval.args)=new list<Expr*>;
			  (yyval.args)->append((Expr*)expr_0->duplicate());
			;}
    break;

  case 105:
#line 825 "../src/parse_tdf.y"
    { (yyval.args)=(yyvsp[(2) - (2)].args); ;}
    break;

  case 106:
#line 827 "../src/parse_tdf.y"
    { (yyval.args)=new list<Expr*>; ;}
    break;

  case 107:
#line 832 "../src/parse_tdf.y"
    { (yyval.args)=(yyvsp[(1) - (3)].args);		(yyval.args)->append((yyvsp[(3) - (3)].expr)); ;}
    break;

  case 108:
#line 834 "../src/parse_tdf.y"
    { (yyval.args)=new list<Expr*>;	(yyval.args)->append((yyvsp[(1) - (1)].expr)); ;}
    break;

  case 109:
#line 839 "../src/parse_tdf.y"
    { (yyval.stmt)=new StmtAssign((yyvsp[(2) - (3)].token),(ExprLValue*)(yyvsp[(1) - (3)].expr),(yyvsp[(3) - (3)].expr)); ;}
    break;

  case 110:
#line 844 "../src/parse_tdf.y"
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

  case 111:
#line 857 "../src/parse_tdf.y"
    {
			  Symbol *s=lookup((yyvsp[(1) - (4)].token));
			  if (s->getType()->getTypeKind()==TYPE_ARRAY)
			    (yyval.expr)=new ExprLValue((yyvsp[(1) - (4)].token),lookup((yyvsp[(1) - (4)].token)),NULL,NULL,NULL,(yyvsp[(3) - (4)].expr));
			  else
			    (yyval.expr)=new ExprLValue((yyvsp[(1) - (4)].token),lookup((yyvsp[(1) - (4)].token)),(yyvsp[(3) - (4)].expr));
			;}
    break;

  case 112:
#line 865 "../src/parse_tdf.y"
    { (yyval.expr)=new ExprLValue((yyvsp[(1) - (6)].token),lookup((yyvsp[(1) - (6)].token)),(yyvsp[(5) - (6)].expr),(yyvsp[(3) - (6)].expr)); ;}
    break;

  case 113:
#line 867 "../src/parse_tdf.y"
    { (yyval.expr)=new ExprLValue((yyvsp[(1) - (7)].token),lookup((yyvsp[(1) - (7)].token)),(yyvsp[(6) - (7)].expr),NULL,NULL,(yyvsp[(3) - (7)].expr)); ;}
    break;

  case 114:
#line 869 "../src/parse_tdf.y"
    { (yyval.expr)=new ExprLValue((yyvsp[(1) - (9)].token),lookup((yyvsp[(1) - (9)].token)),(yyvsp[(8) - (9)].expr),(yyvsp[(6) - (9)].expr),NULL,(yyvsp[(3) - (9)].expr)); ;}
    break;

  case 115:
#line 874 "../src/parse_tdf.y"
    { (yyval.expr)=(yyvsp[(1) - (1)].expr); ;}
    break;

  case 116:
#line 876 "../src/parse_tdf.y"
    {
			  TypeArray *type=new TypeArray(type_any,(yyvsp[(2) - (3)].exprs)->size());
			  (yyval.expr)=new ExprArray((yyvsp[(1) - (3)].token),type,(yyvsp[(2) - (3)].exprs));
			;}
    break;

  case 117:
#line 884 "../src/parse_tdf.y"
    { (yyval.exprs)=(yyvsp[(1) - (1)].exprs); ;}
    break;

  case 118:
#line 886 "../src/parse_tdf.y"
    { (yyval.exprs)=new list<Expr*>; ;}
    break;

  case 119:
#line 891 "../src/parse_tdf.y"
    { (yyval.exprs)=(yyvsp[(1) - (3)].exprs); (yyval.exprs)->append((yyvsp[(3) - (3)].expr)); ;}
    break;

  case 120:
#line 893 "../src/parse_tdf.y"
    { (yyval.exprs)=new list<Expr*>; (yyval.exprs)->append((yyvsp[(1) - (1)].expr)); ;}
    break;

  case 121:
#line 898 "../src/parse_tdf.y"
    { (yyval.expr)=new ExprCond((yyvsp[(2) - (5)].token),(yyvsp[(1) - (5)].expr),(yyvsp[(3) - (5)].expr),(yyvsp[(5) - (5)].expr)); ;}
    break;

  case 122:
#line 900 "../src/parse_tdf.y"
    { (yyval.expr)=(yyvsp[(1) - (1)].expr); ;}
    break;

  case 123:
#line 905 "../src/parse_tdf.y"
    { (yyval.expr)=new ExprBop((yyvsp[(2) - (3)].token),LOGIC_OR,(yyvsp[(1) - (3)].expr),(yyvsp[(3) - (3)].expr)); ;}
    break;

  case 124:
#line 907 "../src/parse_tdf.y"
    { (yyval.expr)=(yyvsp[(1) - (1)].expr); ;}
    break;

  case 125:
#line 912 "../src/parse_tdf.y"
    { (yyval.expr)=new ExprBop((yyvsp[(2) - (3)].token),LOGIC_AND,(yyvsp[(1) - (3)].expr),(yyvsp[(3) - (3)].expr)); ;}
    break;

  case 126:
#line 914 "../src/parse_tdf.y"
    { (yyval.expr)=(yyvsp[(1) - (1)].expr); ;}
    break;

  case 127:
#line 919 "../src/parse_tdf.y"
    { (yyval.expr)=new ExprBop((yyvsp[(2) - (3)].token),(yyvsp[(2) - (3)].token)->code,(yyvsp[(1) - (3)].expr),(yyvsp[(3) - (3)].expr)); ;}
    break;

  case 128:
#line 921 "../src/parse_tdf.y"
    { (yyval.expr)=(yyvsp[(1) - (1)].expr); ;}
    break;

  case 129:
#line 926 "../src/parse_tdf.y"
    { (yyval.expr)=new ExprBop((yyvsp[(2) - (3)].token),(yyvsp[(2) - (3)].token)->code,(yyvsp[(1) - (3)].expr),(yyvsp[(3) - (3)].expr)); ;}
    break;

  case 130:
#line 928 "../src/parse_tdf.y"
    { (yyval.expr)=(yyvsp[(1) - (1)].expr); ;}
    break;

  case 131:
#line 933 "../src/parse_tdf.y"
    { (yyval.expr)=new ExprBop((yyvsp[(2) - (3)].token),(yyvsp[(2) - (3)].token)->code,(yyvsp[(1) - (3)].expr),(yyvsp[(3) - (3)].expr)); ;}
    break;

  case 132:
#line 935 "../src/parse_tdf.y"
    { (yyval.expr)=(yyvsp[(1) - (1)].expr); ;}
    break;

  case 133:
#line 940 "../src/parse_tdf.y"
    { (yyval.expr)=new ExprBop((yyvsp[(2) - (3)].token),(yyvsp[(2) - (3)].token)->code,(yyvsp[(1) - (3)].expr),(yyvsp[(3) - (3)].expr)); ;}
    break;

  case 134:
#line 942 "../src/parse_tdf.y"
    { (yyval.expr)=(yyvsp[(1) - (1)].expr); ;}
    break;

  case 135:
#line 947 "../src/parse_tdf.y"
    { (yyval.expr)=new ExprBop((yyvsp[(2) - (3)].token),(yyvsp[(2) - (3)].token)->code,(yyvsp[(1) - (3)].expr),(yyvsp[(3) - (3)].expr)); ;}
    break;

  case 136:
#line 949 "../src/parse_tdf.y"
    { (yyval.expr)=(yyvsp[(1) - (1)].expr); ;}
    break;

  case 137:
#line 954 "../src/parse_tdf.y"
    { (yyval.expr)=new ExprBop((yyvsp[(2) - (3)].token),(yyvsp[(2) - (3)].token)->code,(yyvsp[(1) - (3)].expr),(yyvsp[(3) - (3)].expr)); ;}
    break;

  case 138:
#line 956 "../src/parse_tdf.y"
    { (yyval.expr)=(yyvsp[(1) - (1)].expr); ;}
    break;

  case 139:
#line 961 "../src/parse_tdf.y"
    { (yyval.expr)=new ExprBop((yyvsp[(2) - (3)].token),(yyvsp[(2) - (3)].token)->code,(yyvsp[(1) - (3)].expr),(yyvsp[(3) - (3)].expr)); ;}
    break;

  case 140:
#line 963 "../src/parse_tdf.y"
    { (yyval.expr)=(yyvsp[(1) - (1)].expr); ;}
    break;

  case 141:
#line 968 "../src/parse_tdf.y"
    { (yyval.expr)=new ExprBop((yyvsp[(2) - (3)].token),(yyvsp[(2) - (3)].token)->code,(yyvsp[(1) - (3)].expr),(yyvsp[(3) - (3)].expr)); ;}
    break;

  case 142:
#line 970 "../src/parse_tdf.y"
    { (yyval.expr)=(yyvsp[(1) - (1)].expr); ;}
    break;

  case 143:
#line 975 "../src/parse_tdf.y"
    { (yyval.expr)=new ExprUop((yyvsp[(1) - (2)].token),(yyvsp[(1) - (2)].token)->code,(yyvsp[(2) - (2)].expr)); ;}
    break;

  case 144:
#line 977 "../src/parse_tdf.y"
    { (yyval.expr)=new ExprUop((yyvsp[(1) - (4)].token),(yyvsp[(1) - (4)].token)->code,(yyvsp[(3) - (4)].expr)); ;}
    break;

  case 145:
#line 979 "../src/parse_tdf.y"
    { (yyval.expr)=new ExprUop((yyvsp[(1) - (4)].token),(yyvsp[(1) - (4)].token)->code,(yyvsp[(3) - (4)].expr)); ;}
    break;

  case 146:
#line 981 "../src/parse_tdf.y"
    { (yyval.expr)=new ExprUop((yyvsp[(1) - (4)].token),(yyvsp[(1) - (4)].token)->code,(yyvsp[(3) - (4)].expr)); ;}
    break;

  case 147:
#line 983 "../src/parse_tdf.y"
    { (yyval.expr)=new ExprUop((yyvsp[(1) - (4)].token),(yyvsp[(1) - (4)].token)->code,(yyvsp[(3) - (4)].expr)); ;}
    break;

  case 148:
#line 985 "../src/parse_tdf.y"
    { (yyval.expr)=new ExprCast((yyvsp[(1) - (4)].token),(yyvsp[(2) - (4)].type),(yyvsp[(4) - (4)].expr)); ;}
    break;

  case 149:
#line 987 "../src/parse_tdf.y"
    { (yyval.expr)=new ExprCast((yyvsp[(1) - (4)].token),(yyvsp[(2) - (4)].token)->code==SIGNED?true:false,(yyvsp[(4) - (4)].expr)); ;}
    break;

  case 150:
#line 989 "../src/parse_tdf.y"
    { (yyval.expr)=(yyvsp[(1) - (1)].expr); ;}
    break;

  case 151:
#line 996 "../src/parse_tdf.y"
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

  case 152:
#line 1024 "../src/parse_tdf.y"
    { (yyval.expr)=(yyvsp[(1) - (1)].expr); ;}
    break;

  case 153:
#line 1029 "../src/parse_tdf.y"
    { (yyval.expr)=(yyvsp[(1) - (1)].expr); ;}
    break;

  case 154:
#line 1031 "../src/parse_tdf.y"
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

  case 155:
#line 1044 "../src/parse_tdf.y"
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

  case 156:
#line 1057 "../src/parse_tdf.y"
    { (yyval.expr)=(Expr*)new ExprValue((yyvsp[(1) - (1)].token),new Type(TYPE_BOOL),1,0); ;}
    break;

  case 157:
#line 1059 "../src/parse_tdf.y"
    { (yyval.expr)=(Expr*)new ExprValue((yyvsp[(1) - (1)].token),new Type(TYPE_BOOL),0,0); ;}
    break;

  case 158:
#line 1061 "../src/parse_tdf.y"
    { (yyval.expr)=(yyvsp[(1) - (1)].expr); ;}
    break;

  case 159:
#line 1063 "../src/parse_tdf.y"
    { (yyval.expr)=(yyvsp[(1) - (1)].expr); ;}
    break;

  case 160:
#line 1065 "../src/parse_tdf.y"
    { (yyval.expr)=(yyvsp[(2) - (3)].expr); ;}
    break;

  case 161:
#line 1070 "../src/parse_tdf.y"
    { (yyval.expr)=new ExprBuiltin((yyvsp[(1) - (4)].token),(yyvsp[(3) - (4)].args),builtin_cat); ;}
    break;

  case 162:
#line 1081 "../src/parse_tdf.y"
    {
			  list<Expr*> *args=new list<Expr*>;
			  args->append((yyvsp[(3) - (4)].expr));
			  (yyval.expr)=new ExprBuiltin((yyvsp[(1) - (4)].token),args,builtin_widthof);
			;}
    break;

  case 163:
#line 1087 "../src/parse_tdf.y"
    {
			  list<Expr*> *args=new list<Expr*>;
			  args->append((yyvsp[(3) - (4)].expr));
			  (yyval.expr)=new ExprBuiltin((yyvsp[(1) - (4)].token),args,builtin_bitsof);
			;}
    break;


/* Line 1267 of yacc.c.  */
#line 3082 "parse_tdf.tab.cc"
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


#line 1145 "../src/parse_tdf.y"


