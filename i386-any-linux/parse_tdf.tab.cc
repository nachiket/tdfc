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
#line 390 "parse_tdf.tab.cc"
	YYSTYPE;
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif



/* Copy the second part of user declarations.  */


/* Line 216 of yacc.c.  */
#line 403 "parse_tdf.tab.cc"

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
#define YYLAST   377

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  76
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  82
/* YYNRULES -- Number of rules.  */
#define YYNRULES  186
/* YYNRULES -- Number of states.  */
#define YYNSTATES  331

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   304

#define YYTRANSLATE(YYX)						\
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[YYLEX] -- Bison symbol number corresponding to YYLEX.  */
static const yytype_uint8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,    57,     2,    59,     2,    60,    62,     2,
      45,    47,    63,    55,    69,    54,    70,    64,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,    68,    67,
      52,    65,    53,    71,    58,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,    50,     2,    51,    61,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,    48,    66,    49,    56,     2,     2,     2,
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
      35,    36,    37,    38,    39,    40,    41,    42,    43,    44,
      46,    72,    73,    74,    75
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
     487,   489,   491,   495,   497,   499,   503,   505,   507,   509,
     511,   513,   515,   519,   524,   529,   534,   536,   538,   540,
     542,   544,   546,   548,   550,   552,   554,   556,   558,   560,
     562,   564,   566,   568,   570,   572,   574
};

/* YYRHS -- A `-1'-separated list of the rules' RHS.  */
static const yytype_int16 yyrhs[] =
{
      77,     0,    -1,    78,    -1,    79,    -1,    -1,    79,    80,
      -1,    80,    -1,    -1,    82,    86,    81,    93,    91,    -1,
      83,    -1,    -1,     5,    -1,     6,    -1,     7,    -1,     5,
      85,    -1,    84,    50,   129,    51,    -1,    84,    85,    50,
     129,    51,    -1,    39,    -1,    44,    -1,    50,   129,    51,
      -1,    18,    45,    87,    47,    -1,    88,    -1,    -1,    88,
      69,    89,    -1,    89,    -1,    90,    83,    18,    -1,    20,
      -1,    28,    -1,    29,    -1,    92,    -1,    -1,    30,    -1,
      -1,    48,    96,    94,   107,    49,    -1,    -1,    48,    96,
      95,   104,    49,    -1,    48,    96,    49,    -1,    97,    -1,
      -1,    97,    99,    -1,    -1,    98,    99,    -1,    83,   100,
      67,    -1,    67,    -1,   101,    -1,   100,    69,   101,    -1,
      18,    -1,    -1,    18,   102,    65,   129,    -1,    -1,    18,
     103,    45,   129,    47,    -1,   104,   105,    -1,   106,    -1,
     106,    -1,    67,    -1,   123,    67,    -1,   128,    65,   123,
      67,    -1,   128,    65,   128,    67,    -1,   120,    67,    -1,
     107,   108,    -1,   108,    -1,    40,    18,    45,   109,    47,
      68,   112,    -1,   110,    -1,    -1,   110,    69,   111,    -1,
     111,    -1,    18,    -1,    18,    59,   129,    -1,    13,    45,
      18,    47,    -1,   113,    -1,    -1,   113,   114,    -1,   115,
      -1,   115,    -1,    67,    -1,   116,    -1,   117,    -1,    19,
      45,   129,    47,   116,    12,   116,    -1,    16,    18,    67,
      -1,    41,    67,    -1,   127,    67,    -1,   119,    67,    -1,
     123,    67,    -1,   118,    -1,    19,    45,   129,    47,   114,
      -1,    19,    45,   129,    47,   116,    12,   117,    -1,    48,
      96,   112,    49,    -1,     3,    45,   129,    47,    -1,     9,
      45,    18,    47,    -1,    11,    45,    47,    -1,    31,    45,
      42,   125,    47,    -1,   121,    -1,   122,    -1,   121,    -1,
     122,    -1,    10,    45,   126,    47,    -1,    33,    45,   126,
      47,    -1,    38,    45,   126,    47,    -1,    34,    45,   126,
      47,    -1,    35,    45,   126,    47,    -1,    37,    45,   126,
      47,    -1,    36,    45,   126,    47,    -1,    18,    45,   124,
      47,    -1,   126,    -1,    -1,    69,   126,    -1,    -1,   126,
      69,   129,    -1,   129,    -1,   128,    65,   129,    -1,    18,
      -1,    18,    50,   129,    51,    -1,    18,    50,   129,    68,
     129,    51,    -1,    18,    50,   129,    51,    50,   129,    51,
      -1,    18,    50,   129,    51,    50,   129,    68,   129,    51,
      -1,   132,    -1,    48,   130,    49,    -1,   131,    -1,    -1,
     131,    69,   132,    -1,   132,    -1,   133,    71,   132,    68,
     132,    -1,   133,    -1,   133,    23,   134,    -1,   134,    -1,
     134,    22,   135,    -1,   135,    -1,   135,    66,   136,    -1,
     136,    -1,   136,    61,   137,    -1,   137,    -1,   137,    62,
     138,    -1,   138,    -1,   138,   148,   139,    -1,   139,    -1,
     139,   149,   140,    -1,   140,    -1,   140,   150,   141,    -1,
     141,    -1,   141,   151,   142,    -1,   142,    -1,   142,   156,
     143,    -1,   143,    -1,   157,   145,    -1,   152,    45,   129,
      47,    -1,   153,    45,   129,    47,    -1,   154,    45,   129,
      47,    -1,   155,    45,   129,    47,    -1,    45,    83,    47,
     145,    -1,    45,    84,    47,   145,    -1,   144,    -1,   145,
      -1,   146,    70,   146,    -1,   146,    -1,   128,    -1,   128,
      58,   146,    -1,    26,    -1,    27,    -1,    43,    -1,    15,
      -1,   123,    -1,   147,    -1,    45,   129,    47,    -1,     8,
      45,   126,    47,    -1,    46,    45,   129,    47,    -1,     4,
      45,   129,    47,    -1,    14,    -1,    25,    -1,    52,    -1,
      53,    -1,    24,    -1,    17,    -1,    21,    -1,    32,    -1,
      55,    -1,    54,    -1,    72,    -1,    73,    -1,    74,    -1,
      75,    -1,    63,    -1,    64,    -1,    60,    -1,    55,    -1,
      54,    -1,    57,    -1,    56,    -1
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
     988,   993,  1017,  1045,  1050,  1052,  1065,  1078,  1082,  1084,
    1086,  1088,  1090,  1095,  1106,  1112,  1121,  1122,  1126,  1127,
    1128,  1129,  1133,  1134,  1138,  1139,  1143,  1147,  1151,  1155,
    1159,  1160,  1161,  1165,  1166,  1167,  1168
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
  "LOGIC_AND", "LOGIC_OR", "LTE", "NOT_EQUALS", "NUM", "NUMDBL", "OUTPUT",
  "PARAM", "PASS_THROUGH_EXCEPTION", "PRINTF", "RIGHT_SHIFT", "SEGMENT_R_",
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
  "floatExpr", "fixedExpr", "atomExpr", "builtinExpr", "equalOp",
  "inequalOp", "shiftOp", "addOp", "exprOp", "logOp", "sqrtOp", "floorOp",
  "prodOp", "unaryOp", 0
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
     295,   296,   297,   298,   299,    40,   300,    41,   123,   125,
      91,    93,    60,    62,    45,    43,   126,    33,    64,    35,
      37,    94,    38,    42,    47,    61,   124,    59,    58,    44,
      46,    63,   301,   302,   303,   304
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint8 yyr1[] =
{
       0,    76,    77,    78,    78,    79,    79,    81,    80,    82,
      82,    83,    83,    83,    83,    83,    83,    84,    84,    85,
      86,    87,    87,    88,    88,    89,    90,    90,    90,    91,
      91,    92,    94,    93,    95,    93,    93,    96,    96,    97,
      98,    97,    99,    99,   100,   100,   101,   102,   101,   103,
     101,   104,   104,   105,   105,   106,   106,   106,   106,   107,
     107,   108,   109,   109,   110,   110,   111,   111,   111,   112,
     112,   113,   113,   114,   114,   115,   115,   116,   116,   116,
     116,   116,   116,   116,   117,   117,   118,   119,   119,   119,
     119,   119,   119,   120,   120,   121,   122,   122,   122,   122,
     122,   122,   123,   124,   124,   125,   125,   126,   126,   127,
     128,   128,   128,   128,   128,   129,   129,   130,   130,   131,
     131,   132,   132,   133,   133,   134,   134,   135,   135,   136,
     136,   137,   137,   138,   138,   139,   139,   140,   140,   141,
     141,   142,   142,   143,   143,   143,   143,   143,   143,   143,
     143,   144,   145,   145,   146,   146,   146,   146,   146,   146,
     146,   146,   146,   147,   147,   147,   148,   148,   149,   149,
     149,   149,   150,   150,   151,   151,   152,   153,   154,   155,
     156,   156,   156,   157,   157,   157,   157
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
       1,     1,     3,     1,     1,     3,     1,     1,     1,     1,
       1,     1,     3,     4,     4,     4,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1
};

/* YYDEFACT[STATE-NAME] -- Default rule to reduce with in state
   STATE-NUM when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const yytype_uint8 yydefact[] =
{
       4,    11,    12,    13,    17,    18,     0,     2,     3,     6,
       0,     9,     0,     0,    14,     1,     5,     0,     7,     0,
       0,     0,     0,   159,   110,   156,   157,   158,     0,     0,
     118,   184,   183,   186,   185,   176,   177,   178,   179,   160,
     154,     0,   115,   122,   124,   126,   128,   130,   132,   134,
     136,   138,   140,   142,   150,   151,   153,   161,     0,     0,
       0,     0,     0,    22,     0,     0,     0,     0,     0,   104,
       0,     0,     0,     0,     0,     0,   117,   120,     0,    19,
       0,     0,     0,     0,     0,     0,   166,   167,     0,   171,
     170,   168,   169,     0,   172,   173,     0,   175,   174,     0,
     182,   180,   181,     0,     0,     0,     0,     0,     0,     0,
     143,    26,    27,    28,     0,    21,    24,     0,    38,    30,
      15,     0,     0,     0,   108,     0,   103,     0,     0,     0,
     162,     0,   116,     0,   155,   123,     0,   125,   127,   129,
     131,   133,   135,   137,   139,   141,   152,     0,     0,     0,
       0,    20,     0,     0,    34,    37,     0,    31,     8,    29,
      16,   165,   163,     0,   102,   111,     0,   148,   149,   164,
     119,     0,   144,   145,   146,   147,    23,    25,    36,     0,
       0,    43,     0,    39,    41,   107,     0,     0,   121,     0,
       0,    60,     0,     0,     0,     0,     0,     0,     0,     0,
      52,     0,    93,    94,     0,     0,    46,     0,    44,     0,
     112,     0,    33,    59,     0,     0,     0,     0,     0,     0,
       0,    35,    54,    51,    53,    58,    55,     0,     0,     0,
      42,     0,   113,     0,    63,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,    45,     0,     0,    66,
       0,    62,    65,    95,    96,    98,    99,   101,   100,    97,
      56,    57,    48,     0,   114,     0,     0,     0,     0,    50,
       0,    67,    70,    64,    68,     0,     0,     0,     0,     0,
       0,     0,    38,    61,    69,    72,    75,    76,    83,     0,
      91,    92,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    79,    70,    74,    71,    73,    81,    82,    80,     0,
       0,     0,    89,    78,     0,   106,     0,   109,    87,    88,
       0,     0,     0,    86,    84,    75,   105,    90,     0,    77,
      85
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
      -1,     6,     7,     8,     9,    64,    10,    11,    12,    20,
      18,   114,   115,   116,   117,   158,   159,   119,   179,   180,
     154,   155,   156,   183,   207,   208,   228,   229,   199,   223,
     200,   190,   191,   250,   251,   252,   283,   284,   304,   285,
     286,   287,   288,   289,   201,   290,   291,    39,   125,   322,
     123,   293,    40,   124,    75,    76,    42,    43,    44,    45,
      46,    47,    48,    49,    50,    51,    52,    53,    54,    55,
      56,    57,    88,    93,    96,    99,    58,    59,    60,    61,
     103,    62
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -262
static const yytype_int16 yypact[] =
{
      19,   -46,  -262,  -262,  -262,  -262,    14,  -262,    19,  -262,
      26,  -262,    83,    62,  -262,  -262,  -262,     3,  -262,    62,
     115,   104,   138,  -262,    33,  -262,  -262,  -262,   170,   141,
     233,  -262,  -262,  -262,  -262,  -262,  -262,  -262,  -262,  -262,
     131,   139,  -262,   -16,   171,   126,   134,   136,    61,   -14,
     106,   127,    88,  -262,  -262,  -262,   129,  -262,   155,   156,
     159,   160,   176,   143,   158,   157,    62,    62,    62,    62,
      62,   163,   117,   164,    62,   168,   154,  -262,   176,  -262,
     233,   233,   233,   233,   233,   233,  -262,  -262,   233,  -262,
    -262,  -262,  -262,   233,  -262,  -262,   233,  -262,  -262,   233,
    -262,  -262,  -262,   233,   176,    62,    62,    62,    62,    62,
    -262,  -262,  -262,  -262,   165,   161,  -262,    23,     6,   177,
     178,   182,   187,   -20,  -262,   188,   167,    -8,   176,   176,
    -262,   191,  -262,   233,  -262,   171,   172,   126,   134,   136,
      61,   -14,   106,   127,    88,  -262,  -262,   192,   199,   200,
     202,  -262,   143,   211,    36,    12,    12,  -262,  -262,  -262,
    -262,  -262,  -262,    62,  -262,   204,    62,  -262,  -262,  -262,
    -262,   233,  -262,  -262,  -262,  -262,  -262,  -262,  -262,   210,
      64,  -262,   234,  -262,  -262,  -262,    62,   205,  -262,   237,
      92,  -262,   212,   213,   217,   218,   219,   220,   221,    -2,
    -262,   201,  -262,  -262,   203,   206,    66,    99,  -262,    71,
    -262,   222,  -262,  -262,    62,    62,    62,    62,    62,    62,
      62,  -262,  -262,  -262,  -262,  -262,  -262,   251,   207,   228,
    -262,   234,  -262,    62,   102,    -5,    34,    37,    40,    43,
      76,    77,   208,   214,    62,    62,  -262,   223,   232,   224,
     247,   215,  -262,  -262,  -262,  -262,  -262,  -262,  -262,  -262,
    -262,  -262,  -262,   248,  -262,   262,    62,   229,   102,  -262,
     252,  -262,   322,  -262,  -262,   257,   258,   259,   291,   265,
     266,   245,     6,  -262,   282,  -262,  -262,  -262,  -262,   254,
    -262,  -262,   255,   260,   249,    62,   306,   279,   261,    62,
     287,  -262,   322,  -262,  -262,  -262,  -262,  -262,  -262,    62,
     288,   289,  -262,  -262,   290,   270,   285,  -262,  -262,  -262,
     282,    62,   295,  -262,  -262,   331,   167,  -262,   322,  -262,
    -262
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -262,  -262,  -262,  -262,   336,  -262,  -262,   -26,   317,   345,
    -262,  -262,  -262,   195,  -262,  -262,  -262,  -262,  -262,  -262,
      68,  -262,  -262,   196,  -262,   120,  -262,  -262,  -262,  -262,
     149,  -262,   174,  -262,  -262,    86,    59,  -262,    42,  -261,
    -141,    38,  -262,  -262,  -262,   -59,   -52,  -159,  -262,  -262,
     -60,  -262,  -158,   -13,  -262,  -262,   -29,  -262,   292,   283,
     284,   293,   286,   280,   276,   277,   275,   272,  -262,   -57,
     -58,  -262,  -262,  -262,  -262,  -262,  -262,  -262,  -262,  -262,
    -262,  -262
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -50
static const yytype_int16 yytable[] =
{
      41,    77,    71,    89,    13,   110,    65,    80,   192,   126,
      90,   -40,   -40,   -40,    15,    73,    24,     1,     2,     3,
     134,   204,   205,   305,     1,     2,     3,   162,     1,     2,
       3,   193,   194,   195,   196,   197,   198,   -10,    91,    92,
     204,   205,   253,   165,    17,   -40,   146,   221,    63,   163,
     -40,     4,   136,   121,   122,    81,     5,   127,     4,   305,
     166,   131,     4,     5,   163,   222,    21,     5,   242,   243,
      22,   167,   168,   -40,   192,    86,   -32,    23,    69,   181,
      24,   254,    24,    70,   255,   178,    87,   256,    25,    26,
     257,   153,   147,   148,   149,   150,    73,   193,   194,   195,
     196,   197,   198,   163,   170,    27,   163,    28,    29,   163,
      30,   -49,   163,   292,   294,   248,    31,    32,    33,    34,
     249,   202,   232,   258,   259,   292,   294,    94,   203,   182,
     182,   -47,   189,    19,    35,    36,    37,    38,    95,   233,
     202,   212,   188,   292,   294,   163,   163,   203,   100,    67,
     185,   101,   102,   187,   235,   236,   237,   238,   239,   240,
     241,   292,   294,   111,   129,    66,   230,    19,   231,   292,
     294,   112,   113,   209,    21,     1,     2,     3,    22,   325,
      21,    97,    98,    68,    22,    23,    74,   329,    24,    78,
      79,    23,    83,    82,    24,    84,    25,    26,    85,   104,
     105,   106,    25,    26,   107,   108,   118,   157,   120,     4,
     128,   130,   151,    27,     5,    28,    29,   132,    30,    27,
     247,   109,    29,   133,    31,    32,    33,    34,   -19,   177,
     152,   262,   263,   160,   161,   164,   163,    21,   169,   172,
     171,    22,    35,    36,    37,    38,   173,   174,    23,   175,
     189,    24,   206,   271,   186,   211,   210,   214,   215,    25,
      26,   326,   216,   217,   218,   219,   220,   234,   225,    24,
     226,   227,   244,   245,   264,   260,    27,   265,    28,    29,
     270,   261,   310,   266,   268,   275,   314,    31,    32,    33,
      34,   276,   192,   277,   267,   269,   317,   272,   278,   274,
      24,   279,   295,   296,   297,    35,    36,    37,    38,   298,
     299,   300,   301,   280,   309,   193,   194,   195,   196,   197,
     198,   306,   307,   281,   311,   275,   312,   308,   313,   315,
     282,   276,   192,   277,   323,   318,   319,   320,   278,   321,
      24,   279,   327,   328,    16,    72,    14,   176,   224,   303,
     302,   246,   184,   280,   273,   193,   194,   195,   196,   197,
     198,   316,   324,   281,   213,   137,   330,   138,   141,   142,
     282,   140,   135,   143,   144,   145,     0,   139
};

static const yytype_int16 yycheck[] =
{
      13,    30,    28,    17,    50,    62,    19,    23,    10,    69,
      24,     5,     6,     7,     0,    28,    18,     5,     6,     7,
      78,   180,   180,   284,     5,     6,     7,    47,     5,     6,
       7,    33,    34,    35,    36,    37,    38,    18,    52,    53,
     199,   199,    47,    51,    18,    39,   104,    49,    45,    69,
      44,    39,    81,    66,    67,    71,    44,    70,    39,   320,
      68,    74,    39,    44,    69,    67,     4,    44,   227,   227,
       8,   128,   129,    67,    10,    14,    40,    15,    45,    67,
      18,    47,    18,    50,    47,    49,    25,    47,    26,    27,
      47,   117,   105,   106,   107,   108,   109,    33,    34,    35,
      36,    37,    38,    69,   133,    43,    69,    45,    46,    69,
      48,    45,    69,   272,   272,    13,    54,    55,    56,    57,
      18,   180,    51,    47,    47,   284,   284,    21,   180,   155,
     156,    65,    40,    50,    72,    73,    74,    75,    32,    68,
     199,    49,   171,   302,   302,    69,    69,   199,    60,    45,
     163,    63,    64,   166,   214,   215,   216,   217,   218,   219,
     220,   320,   320,    20,    47,    50,    67,    50,    69,   328,
     328,    28,    29,   186,     4,     5,     6,     7,     8,   320,
       4,    54,    55,    45,     8,    15,    45,   328,    18,    58,
      51,    15,    66,    22,    18,    61,    26,    27,    62,    70,
      45,    45,    26,    27,    45,    45,    48,    30,    51,    39,
      47,    47,    47,    43,    44,    45,    46,    49,    48,    43,
     233,    45,    46,    69,    54,    55,    56,    57,    50,    18,
      69,   244,   245,    51,    47,    47,    69,     4,    47,    47,
      68,     8,    72,    73,    74,    75,    47,    47,    15,    47,
      40,    18,    18,   266,    50,    18,    51,    45,    45,    26,
      27,   321,    45,    45,    45,    45,    45,    45,    67,    18,
      67,    65,    65,    45,    51,    67,    43,    45,    45,    46,
      18,    67,   295,    59,    69,     3,   299,    54,    55,    56,
      57,     9,    10,    11,    47,    47,   309,    68,    16,    47,
      18,    19,    45,    45,    45,    72,    73,    74,    75,    18,
      45,    45,    67,    31,    65,    33,    34,    35,    36,    37,
      38,    67,    67,    41,    18,     3,    47,    67,    67,    42,
      48,     9,    10,    11,    49,    47,    47,    47,    16,    69,
      18,    19,    47,    12,     8,    28,     1,   152,   199,    67,
     282,   231,   156,    31,   268,    33,    34,    35,    36,    37,
      38,   302,   320,    41,   190,    82,   328,    83,    88,    93,
      48,    85,    80,    96,    99,   103,    -1,    84
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,     5,     6,     7,    39,    44,    77,    78,    79,    80,
      82,    83,    84,    50,    85,     0,    80,    18,    86,    50,
      85,     4,     8,    15,    18,    26,    27,    43,    45,    46,
      48,    54,    55,    56,    57,    72,    73,    74,    75,   123,
     128,   129,   132,   133,   134,   135,   136,   137,   138,   139,
     140,   141,   142,   143,   144,   145,   146,   147,   152,   153,
     154,   155,   157,    45,    81,   129,    50,    45,    45,    45,
      50,    83,    84,   129,    45,   130,   131,   132,    58,    51,
      23,    71,    22,    66,    61,    62,    14,    25,   148,    17,
      24,    52,    53,   149,    21,    32,   150,    54,    55,   151,
      60,    63,    64,   156,    70,    45,    45,    45,    45,    45,
     145,    20,    28,    29,    87,    88,    89,    90,    48,    93,
      51,   129,   129,   126,   129,   124,   126,   129,    47,    47,
      47,   129,    49,    69,   146,   134,   132,   135,   136,   137,
     138,   139,   140,   141,   142,   143,   146,   129,   129,   129,
     129,    47,    69,    83,    96,    97,    98,    30,    91,    92,
      51,    47,    47,    69,    47,    51,    68,   145,   145,    47,
     132,    68,    47,    47,    47,    47,    89,    18,    49,    94,
      95,    67,    83,    99,    99,   129,    50,   129,   132,    40,
     107,   108,    10,    33,    34,    35,    36,    37,    38,   104,
     106,   120,   121,   122,   123,   128,    18,   100,   101,   129,
      51,    18,    49,   108,    45,    45,    45,    45,    45,    45,
      45,    49,    67,   105,   106,    67,    67,    65,   102,   103,
      67,    69,    51,    68,    45,   126,   126,   126,   126,   126,
     126,   126,   123,   128,    65,    45,   101,   129,    13,    18,
     109,   110,   111,    47,    47,    47,    47,    47,    47,    47,
      67,    67,   129,   129,    51,    45,    59,    47,    69,    47,
      18,   129,    68,   111,    47,     3,     9,    11,    16,    19,
      31,    41,    48,   112,   113,   115,   116,   117,   118,   119,
     121,   122,   123,   127,   128,    45,    45,    45,    18,    45,
      45,    67,    96,    67,   114,   115,    67,    67,    67,    65,
     129,    18,    47,    67,   129,    42,   112,   129,    47,    47,
      47,    69,   125,    49,   114,   116,   126,    47,    12,   116,
     117
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
#line 994 "../src/parse_tdf.y"
    { (yyval.expr)=(yyvsp[(1) - (1)].expr); ;}
    break;

  case 152:
#line 1018 "../src/parse_tdf.y"
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

  case 153:
#line 1046 "../src/parse_tdf.y"
    { (yyval.expr)=(yyvsp[(1) - (1)].expr); ;}
    break;

  case 154:
#line 1051 "../src/parse_tdf.y"
    { (yyval.expr)=(yyvsp[(1) - (1)].expr); ;}
    break;

  case 155:
#line 1053 "../src/parse_tdf.y"
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

  case 156:
#line 1066 "../src/parse_tdf.y"
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

  case 157:
#line 1079 "../src/parse_tdf.y"
    {
			  (yyval.expr)=(Expr*)new ExprValue((yyvsp[(1) - (1)].token), new Type(TYPE_DOUBLE),strtod((yyvsp[(1) - (1)].token)->str,NULL));
			;}
    break;

  case 158:
#line 1083 "../src/parse_tdf.y"
    { (yyval.expr)=(Expr*)new ExprValue((yyvsp[(1) - (1)].token),new Type(TYPE_BOOL),1,0); ;}
    break;

  case 159:
#line 1085 "../src/parse_tdf.y"
    { (yyval.expr)=(Expr*)new ExprValue((yyvsp[(1) - (1)].token),new Type(TYPE_BOOL),0,0); ;}
    break;

  case 160:
#line 1087 "../src/parse_tdf.y"
    { (yyval.expr)=(yyvsp[(1) - (1)].expr); ;}
    break;

  case 161:
#line 1089 "../src/parse_tdf.y"
    { (yyval.expr)=(yyvsp[(1) - (1)].expr); ;}
    break;

  case 162:
#line 1091 "../src/parse_tdf.y"
    { (yyval.expr)=(yyvsp[(2) - (3)].expr); ;}
    break;

  case 163:
#line 1096 "../src/parse_tdf.y"
    { (yyval.expr)=new ExprBuiltin((yyvsp[(1) - (4)].token),(yyvsp[(3) - (4)].args),builtin_cat); ;}
    break;

  case 164:
#line 1107 "../src/parse_tdf.y"
    {
			  list<Expr*> *args=new list<Expr*>;
			  args->append((yyvsp[(3) - (4)].expr));
			  (yyval.expr)=new ExprBuiltin((yyvsp[(1) - (4)].token),args,builtin_widthof);
			;}
    break;

  case 165:
#line 1113 "../src/parse_tdf.y"
    {
			  list<Expr*> *args=new list<Expr*>;
			  args->append((yyvsp[(3) - (4)].expr));
			  (yyval.expr)=new ExprBuiltin((yyvsp[(1) - (4)].token),args,builtin_bitsof);
			;}
    break;


/* Line 1267 of yacc.c.  */
#line 3099 "parse_tdf.tab.cc"
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


#line 1171 "../src/parse_tdf.y"


