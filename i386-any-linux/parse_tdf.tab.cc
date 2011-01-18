
/* A Bison parser, made by GNU Bison 2.4.1.  */

/* Skeleton implementation for Bison's Yacc-like parsers in C
   
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
#define YYBISON_VERSION "2.4.1"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 0

/* Push parsers.  */
#define YYPUSH 0

/* Pull parsers.  */
#define YYPULL 1

/* Using locations.  */
#define YYLSP_NEEDED 0



/* Copy the first part of user declarations.  */

/* Line 189 of yacc.c  */
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


/* Line 189 of yacc.c  */
#line 245 "parse_tdf.tab.cc"

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

/* Line 214 of yacc.c  */
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



/* Line 214 of yacc.c  */
#line 360 "parse_tdf.tab.cc"
} YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
#endif


/* Copy the second part of user declarations.  */


/* Line 264 of yacc.c  */
#line 372 "parse_tdf.tab.cc"

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
YYID (int yyi)
#else
static int
YYID (yyi)
    int yyi;
#endif
{
  return yyi;
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
  yytype_int16 yyss_alloc;
  YYSTYPE yyvs_alloc;
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
# define YYSTACK_RELOCATE(Stack_alloc, Stack)				\
    do									\
      {									\
	YYSIZE_T yynewbytes;						\
	YYCOPY (&yyptr->Stack_alloc, Stack, yysize);			\
	Stack = &yyptr->Stack_alloc;					\
	yynewbytes = yystacksize * sizeof (*Stack) + YYSTACK_GAP_MAXIMUM; \
	yyptr += yynewbytes / sizeof (*yyptr);				\
      }									\
    while (YYID (0))

#endif

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  16
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   401

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  79
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  83
/* YYNRULES -- Number of rules.  */
#define YYNRULES  191
/* YYNRULES -- Number of states.  */
#define YYNSTATES  345

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   307

#define YYTRANSLATE(YYX)						\
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[YYLEX] -- Bison symbol number corresponding to YYLEX.  */
static const yytype_uint8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,    59,     2,    61,     2,    62,    64,     2,
      48,    49,    65,    57,    71,    56,    72,    66,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,    70,    69,
      54,    67,    55,    73,    60,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,    52,     2,    53,    63,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,    50,    68,    51,    58,     2,     2,     2,
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
      45,    46,    47,    74,    75,    76,    77,    78
};

#if YYDEBUG
/* YYPRHS[YYN] -- Index of the first RHS symbol of rule number YYN in
   YYRHS.  */
static const yytype_uint16 yyprhs[] =
{
       0,     0,     3,     5,     7,     8,    11,    13,    14,    20,
      22,    23,    25,    27,    29,    32,    35,    40,    46,    48,
      50,    54,    59,    61,    62,    66,    68,    72,    74,    76,
      78,    80,    81,    83,    84,    90,    91,    97,   101,   103,
     104,   107,   108,   111,   115,   117,   119,   123,   125,   126,
     131,   132,   138,   141,   143,   145,   147,   150,   155,   160,
     163,   166,   168,   176,   178,   179,   183,   185,   187,   191,
     196,   201,   203,   204,   207,   209,   211,   213,   215,   217,
     225,   229,   232,   235,   238,   241,   243,   249,   257,   262,
     267,   272,   277,   281,   287,   289,   291,   293,   295,   300,
     305,   310,   315,   320,   325,   330,   335,   337,   338,   341,
     342,   346,   348,   352,   354,   359,   366,   374,   384,   386,
     390,   392,   393,   397,   399,   405,   407,   411,   413,   417,
     419,   423,   425,   429,   431,   435,   437,   441,   443,   447,
     449,   453,   455,   459,   461,   465,   467,   470,   475,   480,
     485,   490,   495,   500,   505,   507,   509,   513,   515,   517,
     521,   523,   525,   527,   529,   531,   533,   537,   542,   547,
     552,   554,   556,   558,   560,   562,   564,   566,   568,   570,
     572,   574,   576,   578,   580,   582,   584,   586,   588,   590,
     592,   594
};

/* YYRHS -- A `-1'-separated list of the rules' RHS.  */
static const yytype_int16 yyrhs[] =
{
      80,     0,    -1,    81,    -1,    82,    -1,    -1,    82,    83,
      -1,    83,    -1,    -1,    85,    89,    84,    96,    94,    -1,
      86,    -1,    -1,     5,    -1,     6,    -1,     7,    -1,     7,
      88,    -1,     5,    88,    -1,    87,    52,   132,    53,    -1,
      87,    88,    52,   132,    53,    -1,    41,    -1,    46,    -1,
      52,   132,    53,    -1,    20,    48,    90,    49,    -1,    91,
      -1,    -1,    91,    71,    92,    -1,    92,    -1,    93,    86,
      20,    -1,    22,    -1,    30,    -1,    31,    -1,    95,    -1,
      -1,    32,    -1,    -1,    50,    99,    97,   110,    51,    -1,
      -1,    50,    99,    98,   107,    51,    -1,    50,    99,    51,
      -1,   100,    -1,    -1,   100,   102,    -1,    -1,   101,   102,
      -1,    86,   103,    69,    -1,    69,    -1,   104,    -1,   103,
      71,   104,    -1,    20,    -1,    -1,    20,   105,    67,   132,
      -1,    -1,    20,   106,    48,   132,    49,    -1,   107,   108,
      -1,   109,    -1,   109,    -1,    69,    -1,   126,    69,    -1,
     131,    67,   126,    69,    -1,   131,    67,   131,    69,    -1,
     123,    69,    -1,   110,   111,    -1,   111,    -1,    42,    20,
      48,   112,    49,    70,   115,    -1,   113,    -1,    -1,   113,
      71,   114,    -1,   114,    -1,    20,    -1,    20,    61,   132,
      -1,    14,    48,    20,    49,    -1,    15,    48,    20,    49,
      -1,   116,    -1,    -1,   116,   117,    -1,   118,    -1,   118,
      -1,    69,    -1,   119,    -1,   120,    -1,    21,    48,   132,
      49,   119,    13,   119,    -1,    18,    20,    69,    -1,    43,
      69,    -1,   130,    69,    -1,   122,    69,    -1,   126,    69,
      -1,   121,    -1,    21,    48,   132,    49,   117,    -1,    21,
      48,   132,    49,   119,    13,   120,    -1,    50,    99,   115,
      51,    -1,     3,    48,   132,    49,    -1,     9,    48,    20,
      49,    -1,    10,    48,    20,    49,    -1,    12,    48,    49,
      -1,    33,    48,    44,   128,    49,    -1,   124,    -1,   125,
      -1,   124,    -1,   125,    -1,    11,    48,   129,    49,    -1,
      35,    48,   129,    49,    -1,    40,    48,   129,    49,    -1,
      36,    48,   129,    49,    -1,    37,    48,   129,    49,    -1,
      39,    48,   129,    49,    -1,    38,    48,   129,    49,    -1,
      20,    48,   127,    49,    -1,   129,    -1,    -1,    71,   129,
      -1,    -1,   129,    71,   132,    -1,   132,    -1,   131,    67,
     132,    -1,    20,    -1,    20,    52,   132,    53,    -1,    20,
      52,   132,    70,   132,    53,    -1,    20,    52,   132,    53,
      52,   132,    53,    -1,    20,    52,   132,    53,    52,   132,
      70,   132,    53,    -1,   135,    -1,    50,   133,    51,    -1,
     134,    -1,    -1,   134,    71,   135,    -1,   135,    -1,   136,
      73,   135,    70,   135,    -1,   136,    -1,   136,    25,   137,
      -1,   137,    -1,   137,    24,   138,    -1,   138,    -1,   138,
      68,   139,    -1,   139,    -1,   139,    63,   140,    -1,   140,
      -1,   140,    64,   141,    -1,   141,    -1,   141,   151,   142,
      -1,   142,    -1,   142,   152,   143,    -1,   143,    -1,   143,
     153,   144,    -1,   144,    -1,   144,   154,   145,    -1,   145,
      -1,   145,   160,   146,    -1,   146,    -1,   161,   148,    -1,
     155,    48,   132,    49,    -1,   156,    48,   132,    49,    -1,
     157,    48,   132,    49,    -1,   158,    48,   132,    49,    -1,
     159,    48,   132,    49,    -1,    48,    86,    49,   148,    -1,
      48,    87,    49,   148,    -1,   147,    -1,   148,    -1,   149,
      72,   149,    -1,   149,    -1,   131,    -1,   131,    60,   149,
      -1,    28,    -1,    29,    -1,    45,    -1,    17,    -1,   126,
      -1,   150,    -1,    48,   132,    49,    -1,     8,    48,   129,
      49,    -1,    47,    48,   132,    49,    -1,     4,    48,   132,
      49,    -1,    16,    -1,    27,    -1,    54,    -1,    55,    -1,
      26,    -1,    19,    -1,    23,    -1,    34,    -1,    57,    -1,
      56,    -1,    74,    -1,    75,    -1,    76,    -1,    78,    -1,
      77,    -1,    65,    -1,    66,    -1,    62,    -1,    57,    -1,
      56,    -1,    59,    -1,    58,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const yytype_uint16 yyrline[] =
{
       0,   222,   222,   227,   230,   242,   247,   262,   261,   302,
     305,   309,   311,   313,   315,   317,   319,   341,   370,   372,
     377,   396,   401,   404,   411,   417,   427,   439,   441,   443,
     448,   451,   455,   461,   460,   472,   471,   480,   489,   492,
     500,   503,   503,   512,   526,   531,   532,   537,   542,   541,
     547,   546,   553,   555,   560,   562,   567,   569,   571,   573,
     582,   587,   596,   614,   617,   621,   623,   628,   630,   632,
     634,   639,   642,   646,   648,   653,   655,   660,   662,   667,
     669,   671,   673,   675,   677,   679,   684,   686,   691,   700,
     708,   717,   726,   734,   742,   744,   749,   751,   756,   764,
     773,   782,   791,   800,   809,   821,   826,   829,   837,   840,
     844,   846,   851,   856,   869,   877,   879,   881,   886,   888,
     896,   899,   903,   905,   910,   912,   917,   919,   924,   926,
     931,   933,   938,   940,   945,   947,   952,   954,   959,   961,
     966,   968,   973,   975,   980,   982,   987,   989,   991,   993,
     995,   997,   999,  1001,  1003,  1008,  1032,  1060,  1065,  1067,
    1080,  1093,  1097,  1099,  1101,  1103,  1105,  1110,  1121,  1127,
    1136,  1137,  1141,  1142,  1143,  1144,  1148,  1149,  1153,  1154,
    1158,  1162,  1166,  1170,  1174,  1178,  1179,  1180,  1184,  1185,
    1186,  1187
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || YYTOKEN_TABLE
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "ATTENTION", "BITSOF", "BOOLEAN",
  "FLOAT", "DOUBLE", "CAT", "CLOSE", "FRAMECLOSE", "COPY", "DONE", "ELSE",
  "EOS", "EOFR", "EQUALS", "FALSE", "GOTO", "GTE", "ID_", "IF", "INPUT",
  "LEFT_SHIFT", "LOGIC_AND", "LOGIC_OR", "LTE", "NOT_EQUALS", "NUM",
  "NUMDBL", "OUTPUT", "PARAM", "PASS_THROUGH_EXCEPTION", "PRINTF",
  "RIGHT_SHIFT", "SEGMENT_R_", "SEGMENT_RW_", "SEGMENT_SEQ_R_",
  "SEGMENT_SEQ_RW_", "SEGMENT_SEQ_W_", "SEGMENT_W_", "SIGNED", "STATE",
  "STAY", "STRING", "TRUE", "UNSIGNED", "WIDTHOF", "'('", "')'", "'{'",
  "'}'", "'['", "']'", "'<'", "'>'", "'-'", "'+'", "'~'", "'!'", "'@'",
  "'#'", "'%'", "'^'", "'&'", "'*'", "'/'", "'='", "'|'", "';'", "':'",
  "','", "'.'", "'?'", "EXP", "LOG", "SQRT", "FLOOR", "ABS", "$accept",
  "start", "operators_opt", "operators", "operator", "$@1", "type_opt",
  "type", "sizedType", "arraySize", "opSignature", "ioDecls_opt",
  "ioDecls", "ioDecl", "ioKind", "exception_opt", "exception",
  "operatorBody", "$@2", "$@3", "typeDecls_opt", "typeDecls", "$@4",
  "typeDecl", "typeDeclElems", "typeDeclElem", "$@5", "$@6",
  "callsOrAssigns", "callOrAssign", "callOrAssign_nonEmpty", "states",
  "stateCase", "stateVars_opt", "stateVars", "stateVar", "stmts_opt",
  "stmts", "stmt", "stmt_nonEmpty", "matchedStmt", "unmatchedStmt",
  "stmtBlock", "builtinStmtBehav", "builtinStmtCompose", "copyStmt",
  "segmentStmt", "call", "args_opt", "more_args_opt", "args", "assign",
  "lvalue", "expr", "condExprs_opt", "condExprs", "condExpr", "logOrExpr",
  "logAndExpr", "bitOrExpr", "bitXorExpr", "bitAndExpr", "equalExpr",
  "inequalExpr", "shiftExpr", "addExpr", "prodExpr", "unaryExpr",
  "floatExpr", "fixedExpr", "atomExpr", "builtinExpr", "equalOp",
  "inequalOp", "shiftOp", "addOp", "exprOp", "logOp", "sqrtOp", "absOp",
  "floorOp", "prodOp", "unaryOp", 0
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
     295,   296,   297,   298,   299,   300,   301,   302,    40,    41,
     123,   125,    91,    93,    60,    62,    45,    43,   126,    33,
      64,    35,    37,    94,    38,    42,    47,    61,   124,    59,
      58,    44,    46,    63,   303,   304,   305,   306,   307
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint8 yyr1[] =
{
       0,    79,    80,    81,    81,    82,    82,    84,    83,    85,
      85,    86,    86,    86,    86,    86,    86,    86,    87,    87,
      88,    89,    90,    90,    91,    91,    92,    93,    93,    93,
      94,    94,    95,    97,    96,    98,    96,    96,    99,    99,
     100,   101,   100,   102,   102,   103,   103,   104,   105,   104,
     106,   104,   107,   107,   108,   108,   109,   109,   109,   109,
     110,   110,   111,   112,   112,   113,   113,   114,   114,   114,
     114,   115,   115,   116,   116,   117,   117,   118,   118,   119,
     119,   119,   119,   119,   119,   119,   120,   120,   121,   122,
     122,   122,   122,   122,   122,   122,   123,   123,   124,   125,
     125,   125,   125,   125,   125,   126,   127,   127,   128,   128,
     129,   129,   130,   131,   131,   131,   131,   131,   132,   132,
     133,   133,   134,   134,   135,   135,   136,   136,   137,   137,
     138,   138,   139,   139,   140,   140,   141,   141,   142,   142,
     143,   143,   144,   144,   145,   145,   146,   146,   146,   146,
     146,   146,   146,   146,   146,   147,   148,   148,   149,   149,
     149,   149,   149,   149,   149,   149,   149,   150,   150,   150,
     151,   151,   152,   152,   152,   152,   153,   153,   154,   154,
     155,   156,   157,   158,   159,   160,   160,   160,   161,   161,
     161,   161
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const yytype_uint8 yyr2[] =
{
       0,     2,     1,     1,     0,     2,     1,     0,     5,     1,
       0,     1,     1,     1,     2,     2,     4,     5,     1,     1,
       3,     4,     1,     0,     3,     1,     3,     1,     1,     1,
       1,     0,     1,     0,     5,     0,     5,     3,     1,     0,
       2,     0,     2,     3,     1,     1,     3,     1,     0,     4,
       0,     5,     2,     1,     1,     1,     2,     4,     4,     2,
       2,     1,     7,     1,     0,     3,     1,     1,     3,     4,
       4,     1,     0,     2,     1,     1,     1,     1,     1,     7,
       3,     2,     2,     2,     2,     1,     5,     7,     4,     4,
       4,     4,     3,     5,     1,     1,     1,     1,     4,     4,
       4,     4,     4,     4,     4,     4,     1,     0,     2,     0,
       3,     1,     3,     1,     4,     6,     7,     9,     1,     3,
       1,     0,     3,     1,     5,     1,     3,     1,     3,     1,
       3,     1,     3,     1,     3,     1,     3,     1,     3,     1,
       3,     1,     3,     1,     3,     1,     2,     4,     4,     4,
       4,     4,     4,     4,     1,     1,     3,     1,     1,     3,
       1,     1,     1,     1,     1,     1,     3,     4,     4,     4,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1
};

/* YYDEFACT[STATE-NAME] -- Default rule to reduce with in state
   STATE-NUM when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const yytype_uint8 yydefact[] =
{
       4,    11,    12,    13,    18,    19,     0,     2,     3,     6,
       0,     9,     0,     0,    15,    14,     1,     5,     0,     7,
       0,     0,     0,     0,   163,   113,   160,   161,   162,     0,
       0,   121,   189,   188,   191,   190,   180,   181,   182,   184,
     183,   164,   158,     0,   118,   125,   127,   129,   131,   133,
     135,   137,   139,   141,   143,   145,   154,   155,   157,   165,
       0,     0,     0,     0,     0,     0,    23,     0,     0,     0,
       0,     0,   107,     0,     0,     0,     0,     0,     0,   120,
     123,     0,    20,     0,     0,     0,     0,     0,     0,   170,
     171,     0,   175,   174,   172,   173,     0,   176,   177,     0,
     179,   178,     0,   187,   185,   186,     0,     0,     0,     0,
       0,     0,     0,     0,   146,    27,    28,    29,     0,    22,
      25,     0,    39,    31,    16,     0,     0,     0,   111,     0,
     106,     0,     0,     0,     0,   166,   119,     0,   159,   126,
       0,   128,   130,   132,   134,   136,   138,   140,   142,   144,
     156,     0,     0,     0,     0,     0,    21,     0,     0,    35,
      38,     0,    32,     8,    30,    17,   169,   167,     0,   105,
     114,     0,   168,   152,   153,   122,     0,   147,   148,   149,
     150,   151,    24,    26,    37,     0,     0,    44,     0,    40,
      42,   110,     0,     0,   124,     0,     0,    61,     0,     0,
       0,     0,     0,     0,     0,     0,    53,     0,    96,    97,
       0,     0,    47,     0,    45,     0,   115,     0,    34,    60,
       0,     0,     0,     0,     0,     0,     0,    36,    55,    52,
      54,    59,    56,     0,     0,     0,    43,     0,   116,     0,
      64,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    46,     0,     0,     0,    67,     0,    63,    66,
      98,    99,   101,   102,   104,   103,   100,    57,    58,    49,
       0,   117,     0,     0,     0,     0,     0,    51,     0,     0,
      68,    72,    65,    69,    70,     0,     0,     0,     0,     0,
       0,     0,     0,    39,    62,    71,    74,    77,    78,    85,
       0,    94,    95,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    81,    72,    76,    73,    75,    83,    84,
      82,     0,     0,     0,     0,    92,    80,     0,   109,     0,
     112,    89,    90,    91,     0,     0,     0,    88,    86,    77,
     108,    93,     0,    79,    87
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
      -1,     6,     7,     8,     9,    67,    10,    11,    12,    21,
      19,   118,   119,   120,   121,   163,   164,   123,   185,   186,
     159,   160,   161,   189,   213,   214,   234,   235,   205,   229,
     206,   196,   197,   257,   258,   259,   294,   295,   316,   296,
     297,   298,   299,   300,   207,   301,   302,    41,   129,   336,
     127,   304,    42,   128,    78,    79,    44,    45,    46,    47,
      48,    49,    50,    51,    52,    53,    54,    55,    56,    57,
      58,    59,    91,    96,    99,   102,    60,    61,    62,    63,
      64,   106,    65
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -315
static const yytype_int16 yypact[] =
{
     137,   -42,  -315,   -42,  -315,  -315,    16,  -315,   137,  -315,
      39,  -315,    25,    59,  -315,  -315,  -315,  -315,    62,  -315,
      59,    30,    78,   108,  -315,    71,  -315,  -315,  -315,   139,
     165,   172,  -315,  -315,  -315,  -315,  -315,  -315,  -315,  -315,
    -315,  -315,   135,   146,  -315,   -19,   178,   136,   140,   143,
       9,    75,   117,   141,    87,  -315,  -315,  -315,   133,  -315,
     160,   161,   166,   168,   170,   303,    42,   175,   179,    59,
      59,    59,    59,    59,    59,   184,   132,   186,   183,   173,
    -315,   303,  -315,   172,   172,   172,   172,   172,   172,  -315,
    -315,   172,  -315,  -315,  -315,  -315,   172,  -315,  -315,   172,
    -315,  -315,   172,  -315,  -315,  -315,   172,   303,    59,    59,
      59,    59,    59,    59,  -315,  -315,  -315,  -315,   196,   187,
    -315,    37,    34,   225,   207,   209,   211,   -45,  -315,   214,
     193,    52,   217,   303,   303,  -315,  -315,   172,  -315,   178,
     195,   136,   140,   143,     9,    75,   117,   141,    87,  -315,
    -315,   219,   221,   222,   223,   224,  -315,    42,   259,   103,
      79,    79,  -315,  -315,  -315,  -315,  -315,  -315,    59,  -315,
     228,    59,  -315,  -315,  -315,  -315,   172,  -315,  -315,  -315,
    -315,  -315,  -315,  -315,  -315,   239,   299,  -315,   262,  -315,
    -315,  -315,    59,   230,  -315,   268,   126,  -315,   241,   242,
     243,   244,   246,   247,   248,   216,  -315,   231,  -315,  -315,
     245,   250,    26,    70,  -315,    68,  -315,   249,  -315,  -315,
      59,    59,    59,    59,    59,    59,    59,  -315,  -315,  -315,
    -315,  -315,  -315,   292,   251,   265,  -315,   262,  -315,    59,
     176,   -40,   -37,   -36,    -3,    20,    41,    53,   252,   253,
      59,    59,  -315,   271,   267,   277,   269,   278,   255,  -315,
    -315,  -315,  -315,  -315,  -315,  -315,  -315,  -315,  -315,  -315,
     279,  -315,   309,   313,    59,   270,   176,  -315,   293,   294,
    -315,   266,  -315,  -315,  -315,   296,   297,   298,   301,   321,
     304,   305,   285,    34,  -315,    12,  -315,  -315,  -315,  -315,
     286,  -315,  -315,   287,   288,   280,    59,   338,   339,   311,
     295,    59,   317,  -315,   266,  -315,  -315,  -315,  -315,  -315,
    -315,    59,   314,   316,   318,  -315,  -315,   319,   291,   315,
    -315,  -315,  -315,  -315,    12,    59,   320,  -315,  -315,   357,
     193,  -315,   266,  -315,  -315
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -315,  -315,  -315,  -315,   363,  -315,  -315,   -29,   342,   185,
    -315,  -315,  -315,   218,  -315,  -315,  -315,  -315,  -315,  -315,
      80,  -315,  -315,   213,  -315,   142,  -315,  -315,  -315,  -315,
     171,  -315,   181,  -315,  -315,   102,    66,  -315,    47,  -281,
    -314,    40,  -315,  -315,  -315,  -178,   -97,  -168,  -315,  -315,
     -61,  -315,  -167,   -13,  -315,  -315,   -26,  -315,   300,   302,
     306,   307,   308,   310,   289,   290,   282,   284,  -315,   -63,
     -78,  -315,  -315,  -315,  -315,  -315,  -315,  -315,  -315,  -315,
    -315,  -315,  -315
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -51
static const yytype_int16 yytable[] =
{
      43,    75,   114,   138,   167,    80,    83,    68,   208,   260,
      13,   130,   261,   262,   317,   285,    16,    77,   210,   211,
     339,   286,   287,   198,   288,    89,   168,   208,   343,   150,
     289,   168,    25,   290,   168,   168,    90,   210,   211,   -41,
     -41,   -41,     1,     2,     3,   291,   263,   199,   200,   201,
     202,   203,   204,   317,    84,   292,   125,   126,   140,    18,
     131,   132,   293,    22,   115,   248,   249,    23,   168,   264,
     173,   174,   116,   117,   -50,   -41,    24,    20,     4,    25,
     -41,   315,    69,     5,     1,     2,     3,    26,    27,   209,
     265,   168,   158,   -48,    92,   151,   152,   153,   154,   155,
      77,    93,   266,   -41,    28,   170,    29,    30,   209,    31,
      66,   175,   168,   303,   305,    32,    33,    34,    35,    72,
       4,   238,   171,    73,   168,     5,    70,   303,   305,    94,
      95,   188,   188,    36,    37,    38,    39,    40,   239,   236,
      97,   237,     1,     2,     3,   -33,   303,   305,   187,   103,
     194,    98,   104,   105,   184,   191,    71,   -10,   193,   241,
     242,   243,   244,   245,   246,   247,   303,   305,   195,    22,
       1,     2,     3,    23,   303,   305,    22,   218,     4,   215,
      23,   134,    24,     5,    20,    25,    14,    74,    15,    24,
     254,   255,    25,    26,    27,    81,   256,   100,   101,    82,
      26,    27,    85,    87,    86,   107,     4,    88,   108,   109,
      28,     5,    29,    30,   110,    31,   111,    28,   112,    29,
      30,    32,    33,    34,    35,   122,   253,   198,    32,    33,
      34,    35,   124,   133,   136,   135,    25,   269,   270,    36,
      37,    38,    39,    40,   137,   156,    36,    37,    38,    39,
      40,   199,   200,   201,   202,   203,   204,   162,   157,   -20,
     166,   280,   165,   169,   168,   176,   172,   227,   177,   285,
     178,   179,   180,   181,   340,   286,   287,   198,   288,   183,
     192,   195,   212,   216,   289,   228,    25,   290,   217,   220,
     221,   222,   223,   322,   224,   225,   226,   240,   327,   291,
     231,   199,   200,   201,   202,   203,   204,    22,   330,   292,
     198,    23,    25,   251,   232,   272,   293,   233,   250,    25,
      24,   267,   268,    25,   271,   273,   276,   275,   277,   278,
     274,    26,    27,   279,   199,   200,   201,   202,   203,   204,
     281,   310,   283,   284,   306,   307,   308,   321,    28,   309,
      29,   113,   311,   312,   313,   318,   319,   320,   323,   324,
     325,   328,   335,   331,   326,   332,   337,   333,   334,   341,
     342,    17,    76,   314,   190,   182,   230,   219,   282,   252,
     329,   338,   344,   139,   148,   146,     0,   141,     0,   147,
     149,     0,   142,     0,   143,     0,   144,     0,     0,     0,
       0,   145
};

static const yytype_int16 yycheck[] =
{
      13,    30,    65,    81,    49,    31,    25,    20,   186,    49,
      52,    72,    49,    49,   295,     3,     0,    30,   186,   186,
     334,     9,    10,    11,    12,    16,    71,   205,   342,   107,
      18,    71,    20,    21,    71,    71,    27,   205,   205,     5,
       6,     7,     5,     6,     7,    33,    49,    35,    36,    37,
      38,    39,    40,   334,    73,    43,    69,    70,    84,    20,
      73,    74,    50,     4,    22,   233,   233,     8,    71,    49,
     133,   134,    30,    31,    48,    41,    17,    52,    41,    20,
      46,    69,    52,    46,     5,     6,     7,    28,    29,   186,
      49,    71,   121,    67,    19,   108,   109,   110,   111,   112,
     113,    26,    49,    69,    45,    53,    47,    48,   205,    50,
      48,   137,    71,   281,   281,    56,    57,    58,    59,    48,
      41,    53,    70,    52,    71,    46,    48,   295,   295,    54,
      55,   160,   161,    74,    75,    76,    77,    78,    70,    69,
      23,    71,     5,     6,     7,    42,   314,   314,    69,    62,
     176,    34,    65,    66,    51,   168,    48,    20,   171,   220,
     221,   222,   223,   224,   225,   226,   334,   334,    42,     4,
       5,     6,     7,     8,   342,   342,     4,    51,    41,   192,
       8,    49,    17,    46,    52,    20,     1,    48,     3,    17,
      14,    15,    20,    28,    29,    60,    20,    56,    57,    53,
      28,    29,    24,    63,    68,    72,    41,    64,    48,    48,
      45,    46,    47,    48,    48,    50,    48,    45,    48,    47,
      48,    56,    57,    58,    59,    50,   239,    11,    56,    57,
      58,    59,    53,    49,    51,    49,    20,   250,   251,    74,
      75,    76,    77,    78,    71,    49,    74,    75,    76,    77,
      78,    35,    36,    37,    38,    39,    40,    32,    71,    52,
      49,   274,    53,    49,    71,    70,    49,    51,    49,     3,
      49,    49,    49,    49,   335,     9,    10,    11,    12,    20,
      52,    42,    20,    53,    18,    69,    20,    21,    20,    48,
      48,    48,    48,   306,    48,    48,    48,    48,   311,    33,
      69,    35,    36,    37,    38,    39,    40,     4,   321,    43,
      11,     8,    20,    48,    69,    48,    50,    67,    67,    20,
      17,    69,    69,    20,    53,    48,    71,    49,    49,    20,
      61,    28,    29,    20,    35,    36,    37,    38,    39,    40,
      70,    20,    49,    49,    48,    48,    48,    67,    45,    48,
      47,    48,    48,    48,    69,    69,    69,    69,    20,    20,
      49,    44,    71,    49,    69,    49,    51,    49,    49,    49,
      13,     8,    30,   293,   161,   157,   205,   196,   276,   237,
     314,   334,   342,    83,   102,    96,    -1,    85,    -1,    99,
     106,    -1,    86,    -1,    87,    -1,    88,    -1,    -1,    -1,
      -1,    91
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,     5,     6,     7,    41,    46,    80,    81,    82,    83,
      85,    86,    87,    52,    88,    88,     0,    83,    20,    89,
      52,    88,     4,     8,    17,    20,    28,    29,    45,    47,
      48,    50,    56,    57,    58,    59,    74,    75,    76,    77,
      78,   126,   131,   132,   135,   136,   137,   138,   139,   140,
     141,   142,   143,   144,   145,   146,   147,   148,   149,   150,
     155,   156,   157,   158,   159,   161,    48,    84,   132,    52,
      48,    48,    48,    52,    48,    86,    87,   132,   133,   134,
     135,    60,    53,    25,    73,    24,    68,    63,    64,    16,
      27,   151,    19,    26,    54,    55,   152,    23,    34,   153,
      56,    57,   154,    62,    65,    66,   160,    72,    48,    48,
      48,    48,    48,    48,   148,    22,    30,    31,    90,    91,
      92,    93,    50,    96,    53,   132,   132,   129,   132,   127,
     129,   132,   132,    49,    49,    49,    51,    71,   149,   137,
     135,   138,   139,   140,   141,   142,   143,   144,   145,   146,
     149,   132,   132,   132,   132,   132,    49,    71,    86,    99,
     100,   101,    32,    94,    95,    53,    49,    49,    71,    49,
      53,    70,    49,   148,   148,   135,    70,    49,    49,    49,
      49,    49,    92,    20,    51,    97,    98,    69,    86,   102,
     102,   132,    52,   132,   135,    42,   110,   111,    11,    35,
      36,    37,    38,    39,    40,   107,   109,   123,   124,   125,
     126,   131,    20,   103,   104,   132,    53,    20,    51,   111,
      48,    48,    48,    48,    48,    48,    48,    51,    69,   108,
     109,    69,    69,    67,   105,   106,    69,    71,    53,    70,
      48,   129,   129,   129,   129,   129,   129,   129,   126,   131,
      67,    48,   104,   132,    14,    15,    20,   112,   113,   114,
      49,    49,    49,    49,    49,    49,    49,    69,    69,   132,
     132,    53,    48,    48,    61,    49,    71,    49,    20,    20,
     132,    70,   114,    49,    49,     3,     9,    10,    12,    18,
      21,    33,    43,    50,   115,   116,   118,   119,   120,   121,
     122,   124,   125,   126,   130,   131,    48,    48,    48,    48,
      20,    48,    48,    69,    99,    69,   117,   118,    69,    69,
      69,    67,   132,    20,    20,    49,    69,   132,    44,   115,
     132,    49,    49,    49,    49,    71,   128,    51,   117,   119,
     129,    49,    13,   119,   120
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
yy_stack_print (yytype_int16 *yybottom, yytype_int16 *yytop)
#else
static void
yy_stack_print (yybottom, yytop)
    yytype_int16 *yybottom;
    yytype_int16 *yytop;
#endif
{
  YYFPRINTF (stderr, "Stack now");
  for (; yybottom <= yytop; yybottom++)
    {
      int yybot = *yybottom;
      YYFPRINTF (stderr, " %d", yybot);
    }
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
      YYFPRINTF (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr, yyrhs[yyprhs[yyrule] + yyi],
		       &(yyvsp[(yyi + 1) - (yynrhs)])
		       		       );
      YYFPRINTF (stderr, "\n");
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


/* The lookahead symbol.  */
int yychar;

/* The semantic value of the lookahead symbol.  */
YYSTYPE yylval;

/* Number of syntax errors so far.  */
int yynerrs;



/*-------------------------.
| yyparse or yypush_parse.  |
`-------------------------*/

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
    /* Number of tokens to shift before error messages enabled.  */
    int yyerrstatus;

    /* The stacks and their tools:
       `yyss': related to states.
       `yyvs': related to semantic values.

       Refer to the stacks thru separate pointers, to allow yyoverflow
       to reallocate them elsewhere.  */

    /* The state stack.  */
    yytype_int16 yyssa[YYINITDEPTH];
    yytype_int16 *yyss;
    yytype_int16 *yyssp;

    /* The semantic value stack.  */
    YYSTYPE yyvsa[YYINITDEPTH];
    YYSTYPE *yyvs;
    YYSTYPE *yyvsp;

    YYSIZE_T yystacksize;

  int yyn;
  int yyresult;
  /* Lookahead token as an internal (translated) token number.  */
  int yytoken;
  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;

#if YYERROR_VERBOSE
  /* Buffer for error messages, and its allocated size.  */
  char yymsgbuf[128];
  char *yymsg = yymsgbuf;
  YYSIZE_T yymsg_alloc = sizeof yymsgbuf;
#endif

#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N))

  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

  yytoken = 0;
  yyss = yyssa;
  yyvs = yyvsa;
  yystacksize = YYINITDEPTH;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY; /* Cause a token to be read.  */

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
	YYSTACK_RELOCATE (yyss_alloc, yyss);
	YYSTACK_RELOCATE (yyvs_alloc, yyvs);
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

  if (yystate == YYFINAL)
    YYACCEPT;

  goto yybackup;

/*-----------.
| yybackup.  |
`-----------*/
yybackup:

  /* Do appropriate processing given the current state.  Read a
     lookahead token if we need one and don't already have one.  */

  /* First try to decide what to do without reference to lookahead token.  */
  yyn = yypact[yystate];
  if (yyn == YYPACT_NINF)
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* YYCHAR is either YYEMPTY or YYEOF or a valid lookahead symbol.  */
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

  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  /* Shift the lookahead token.  */
  YY_SYMBOL_PRINT ("Shifting", yytoken, &yylval, &yylloc);

  /* Discard the shifted token.  */
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

/* Line 1455 of yacc.c  */
#line 223 "../src/parse_tdf.y"
    { (yyval.suite)=(yyvsp[(1) - (1)].suite); ;}
    break;

  case 3:

/* Line 1455 of yacc.c  */
#line 228 "../src/parse_tdf.y"
    { (yyval.suite)=(yyvsp[(1) - (1)].suite); ;}
    break;

  case 4:

/* Line 1455 of yacc.c  */
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

/* Line 1455 of yacc.c  */
#line 243 "../src/parse_tdf.y"
    {
			  (yyval.suite)=(yyvsp[(1) - (2)].suite);
			  (yyval.suite)->addOperator((yyvsp[(2) - (2)].operatr));
			;}
    break;

  case 6:

/* Line 1455 of yacc.c  */
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

/* Line 1455 of yacc.c  */
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

/* Line 1455 of yacc.c  */
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

/* Line 1455 of yacc.c  */
#line 303 "../src/parse_tdf.y"
    { (yyval.type)=(yyvsp[(1) - (1)].type); ;}
    break;

  case 10:

/* Line 1455 of yacc.c  */
#line 305 "../src/parse_tdf.y"
    { (yyval.type)=NULL; ;}
    break;

  case 11:

/* Line 1455 of yacc.c  */
#line 310 "../src/parse_tdf.y"
    { (yyval.type)=new Type(TYPE_BOOL); ;}
    break;

  case 12:

/* Line 1455 of yacc.c  */
#line 312 "../src/parse_tdf.y"
    { (yyval.type)=new Type(TYPE_FLOAT); ;}
    break;

  case 13:

/* Line 1455 of yacc.c  */
#line 314 "../src/parse_tdf.y"
    { (yyval.type)=new Type(TYPE_DOUBLE); ;}
    break;

  case 14:

/* Line 1455 of yacc.c  */
#line 316 "../src/parse_tdf.y"
    { (yyval.type)=new TypeArray(new Type(TYPE_DOUBLE),(yyvsp[(2) - (2)].expr)); ;}
    break;

  case 15:

/* Line 1455 of yacc.c  */
#line 318 "../src/parse_tdf.y"
    { (yyval.type)=new TypeArray(new Type(TYPE_BOOL),(yyvsp[(2) - (2)].expr)); ;}
    break;

  case 16:

/* Line 1455 of yacc.c  */
#line 320 "../src/parse_tdf.y"
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

  case 17:

/* Line 1455 of yacc.c  */
#line 342 "../src/parse_tdf.y"
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

  case 18:

/* Line 1455 of yacc.c  */
#line 371 "../src/parse_tdf.y"
    { (yyval.token)=(yyvsp[(1) - (1)].token); ;}
    break;

  case 19:

/* Line 1455 of yacc.c  */
#line 373 "../src/parse_tdf.y"
    { (yyval.token)=(yyvsp[(1) - (1)].token); ;}
    break;

  case 20:

/* Line 1455 of yacc.c  */
#line 378 "../src/parse_tdf.y"
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

  case 21:

/* Line 1455 of yacc.c  */
#line 397 "../src/parse_tdf.y"
    { (yyval.opSignature)=new OpSignature((yyvsp[(1) - (4)].token),(yyvsp[(1) - (4)].token)->str,(yyvsp[(3) - (4)].symList)); ;}
    break;

  case 22:

/* Line 1455 of yacc.c  */
#line 402 "../src/parse_tdf.y"
    { (yyval.symList)=(yyvsp[(1) - (1)].symList); ;}
    break;

  case 23:

/* Line 1455 of yacc.c  */
#line 404 "../src/parse_tdf.y"
    {
			  (yyval.symList)=new list<Symbol*>;
			  symtabPush(new SymTab(SYMTAB_OP));
			;}
    break;

  case 24:

/* Line 1455 of yacc.c  */
#line 412 "../src/parse_tdf.y"
    {
			  (yyval.symList)=(yyvsp[(1) - (3)].symList);
			  (yyval.symList)->append((yyvsp[(3) - (3)].sym));
			  gSymtab->addSymbol((yyvsp[(3) - (3)].sym));
			;}
    break;

  case 25:

/* Line 1455 of yacc.c  */
#line 418 "../src/parse_tdf.y"
    {
			  (yyval.symList)=new list<Symbol*>;
			  (yyval.symList)->append((yyvsp[(1) - (1)].sym));
			  symtabPush(new SymTab(SYMTAB_OP));
			  gSymtab->addSymbol((yyvsp[(1) - (1)].sym));
			;}
    break;

  case 26:

/* Line 1455 of yacc.c  */
#line 428 "../src/parse_tdf.y"
    {
			  if ((yyvsp[(1) - (3)].token)->code==INPUT)
			    (yyval.sym)=new SymbolStream((yyvsp[(3) - (3)].token),(yyvsp[(3) - (3)].token)->str,(yyvsp[(2) - (3)].type),STREAM_IN);
			  else if ((yyvsp[(1) - (3)].token)->code==OUTPUT)
			    (yyval.sym)=new SymbolStream((yyvsp[(3) - (3)].token),(yyvsp[(3) - (3)].token)->str,(yyvsp[(2) - (3)].type),STREAM_OUT);
			  else  // ($1->code==PARAM)
			    (yyval.sym)=new SymbolVar((yyvsp[(3) - (3)].token),(yyvsp[(3) - (3)].token)->str,(yyvsp[(2) - (3)].type));
			;}
    break;

  case 27:

/* Line 1455 of yacc.c  */
#line 440 "../src/parse_tdf.y"
    { (yyval.token)=(yyvsp[(1) - (1)].token); ;}
    break;

  case 28:

/* Line 1455 of yacc.c  */
#line 442 "../src/parse_tdf.y"
    { (yyval.token)=(yyvsp[(1) - (1)].token); ;}
    break;

  case 29:

/* Line 1455 of yacc.c  */
#line 444 "../src/parse_tdf.y"
    { (yyval.token)=(yyvsp[(1) - (1)].token); ;}
    break;

  case 30:

/* Line 1455 of yacc.c  */
#line 449 "../src/parse_tdf.y"
    { (yyval.token)=(yyvsp[(1) - (1)].token); ;}
    break;

  case 31:

/* Line 1455 of yacc.c  */
#line 451 "../src/parse_tdf.y"
    { (yyval.token)=NULL; ;}
    break;

  case 32:

/* Line 1455 of yacc.c  */
#line 456 "../src/parse_tdf.y"
    { (yyval.token)=(yyvsp[(1) - (1)].token); ;}
    break;

  case 33:

/* Line 1455 of yacc.c  */
#line 461 "../src/parse_tdf.y"
    {
			  // typeDecls_opt leaves pushed symtab
			  gStates=new dictionary<string,State*>;
			;}
    break;

  case 34:

/* Line 1455 of yacc.c  */
#line 466 "../src/parse_tdf.y"
    {
			  (yyval.opBody)=(OpBody*)new BehavOpBody(gSymtab,gStates,(yyvsp[(4) - (5)].states));
			  symtabPop();
			  gStates=NULL;
			;}
    break;

  case 35:

/* Line 1455 of yacc.c  */
#line 472 "../src/parse_tdf.y"
    {
			  // typeDecls_opt leaves pushed symtab
			;}
    break;

  case 36:

/* Line 1455 of yacc.c  */
#line 476 "../src/parse_tdf.y"
    {
			  (yyval.opBody)=(OpBody*)new ComposeOpBody(gSymtab,(yyvsp[(4) - (5)].stmts));
			  symtabPop();
			;}
    break;

  case 37:

/* Line 1455 of yacc.c  */
#line 481 "../src/parse_tdf.y"
    {
			  // typeDecls_opt leaves pushed symtab
			  delete symtabPop();
			  yyerror("empty operator body");
			;}
    break;

  case 38:

/* Line 1455 of yacc.c  */
#line 490 "../src/parse_tdf.y"
    { (yyval.symtab)=(yyvsp[(1) - (1)].symtab); ;}
    break;

  case 39:

/* Line 1455 of yacc.c  */
#line 492 "../src/parse_tdf.y"
    {
			  // push new empty symtab before processing first decl
			  symtabPush(new SymTab(SYMTAB_BLOCK));
			  (yyval.symtab)=gSymtab;
			;}
    break;

  case 40:

/* Line 1455 of yacc.c  */
#line 501 "../src/parse_tdf.y"
    { (yyval.symtab)=gSymtab; ;}
    break;

  case 41:

/* Line 1455 of yacc.c  */
#line 503 "../src/parse_tdf.y"
    {
			  // push new empty symtab before processing first decl
			  symtabPush(new SymTab(SYMTAB_BLOCK));
			;}
    break;

  case 42:

/* Line 1455 of yacc.c  */
#line 508 "../src/parse_tdf.y"
    { (yyval.symtab)=gSymtab; ;}
    break;

  case 43:

/* Line 1455 of yacc.c  */
#line 513 "../src/parse_tdf.y"
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

  case 44:

/* Line 1455 of yacc.c  */
#line 527 "../src/parse_tdf.y"
    { (yyval.symtab)=gSymtab; ;}
    break;

  case 45:

/* Line 1455 of yacc.c  */
#line 531 "../src/parse_tdf.y"
    { (yyval.typeDeclElems)=new list<TypeDeclElem*>; (yyval.typeDeclElems)->append((yyvsp[(1) - (1)].typeDeclElem)); ;}
    break;

  case 46:

/* Line 1455 of yacc.c  */
#line 533 "../src/parse_tdf.y"
    { (yyval.typeDeclElems)=(yyvsp[(1) - (3)].typeDeclElems);                      (yyval.typeDeclElems)->append((yyvsp[(3) - (3)].typeDeclElem)); ;}
    break;

  case 47:

/* Line 1455 of yacc.c  */
#line 537 "../src/parse_tdf.y"
    {
			  lookupDuplicateDecl((yyvsp[(1) - (1)].token));
			  (yyval.typeDeclElem)=new TypeDeclElem((yyvsp[(1) - (1)].token),NULL,NULL);
			;}
    break;

  case 48:

/* Line 1455 of yacc.c  */
#line 542 "../src/parse_tdf.y"
    { lookupDuplicateDecl((yyvsp[(1) - (1)].token)); ;}
    break;

  case 49:

/* Line 1455 of yacc.c  */
#line 544 "../src/parse_tdf.y"
    { (yyval.typeDeclElem)=new TypeDeclElem((yyvsp[(1) - (4)].token),(yyvsp[(4) - (4)].expr),NULL); ;}
    break;

  case 50:

/* Line 1455 of yacc.c  */
#line 547 "../src/parse_tdf.y"
    { lookupDuplicateDecl((yyvsp[(1) - (1)].token)); ;}
    break;

  case 51:

/* Line 1455 of yacc.c  */
#line 549 "../src/parse_tdf.y"
    { (yyval.typeDeclElem)=new TypeDeclElem((yyvsp[(1) - (5)].token),NULL,(yyvsp[(4) - (5)].expr)); ;}
    break;

  case 52:

/* Line 1455 of yacc.c  */
#line 554 "../src/parse_tdf.y"
    { (yyval.stmts)=(yyvsp[(1) - (2)].stmts);		if ((yyvsp[(2) - (2)].stmt)) (yyval.stmts)->append((yyvsp[(2) - (2)].stmt)); ;}
    break;

  case 53:

/* Line 1455 of yacc.c  */
#line 556 "../src/parse_tdf.y"
    { (yyval.stmts)=new list<Stmt*>;	if ((yyvsp[(1) - (1)].stmt)) (yyval.stmts)->append((yyvsp[(1) - (1)].stmt)); ;}
    break;

  case 54:

/* Line 1455 of yacc.c  */
#line 561 "../src/parse_tdf.y"
    { (yyval.stmt)=(yyvsp[(1) - (1)].stmt); ;}
    break;

  case 55:

/* Line 1455 of yacc.c  */
#line 563 "../src/parse_tdf.y"
    { (yyval.stmt)=NULL; ;}
    break;

  case 56:

/* Line 1455 of yacc.c  */
#line 568 "../src/parse_tdf.y"
    { (yyval.stmt)=new StmtCall((yyvsp[(1) - (2)].expr)->getToken(),(ExprCall*)(yyvsp[(1) - (2)].expr)); ;}
    break;

  case 57:

/* Line 1455 of yacc.c  */
#line 570 "../src/parse_tdf.y"
    { (yyval.stmt)=new StmtAssign((yyvsp[(2) - (4)].token),(ExprLValue*)(yyvsp[(1) - (4)].expr),(yyvsp[(3) - (4)].expr)); ;}
    break;

  case 58:

/* Line 1455 of yacc.c  */
#line 572 "../src/parse_tdf.y"
    { (yyval.stmt)=new StmtAssign((yyvsp[(2) - (4)].token),(ExprLValue*)(yyvsp[(1) - (4)].expr),(yyvsp[(3) - (4)].expr)); ;}
    break;

  case 59:

/* Line 1455 of yacc.c  */
#line 574 "../src/parse_tdf.y"
    { (yyval.stmt)=(yyvsp[(1) - (2)].stmt); ;}
    break;

  case 60:

/* Line 1455 of yacc.c  */
#line 583 "../src/parse_tdf.y"
    {
			  (yyval.states)=(yyvsp[(1) - (2)].states);			// $$ = start state
			  // states are stored in gStates by stateCase action
			;}
    break;

  case 61:

/* Line 1455 of yacc.c  */
#line 588 "../src/parse_tdf.y"
    {
			  (yyval.states)=(State*)((yyvsp[(1) - (1)].stateCase)->getParent());	// $$ = start state
			  // states are stored in gStates by stateCase action
			;}
    break;

  case 62:

/* Line 1455 of yacc.c  */
#line 597 "../src/parse_tdf.y"
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

  case 63:

/* Line 1455 of yacc.c  */
#line 615 "../src/parse_tdf.y"
    { (yyval.inputSpecs)=(yyvsp[(1) - (1)].inputSpecs); ;}
    break;

  case 64:

/* Line 1455 of yacc.c  */
#line 617 "../src/parse_tdf.y"
    { (yyval.inputSpecs)=new list<InputSpec*>; ;}
    break;

  case 65:

/* Line 1455 of yacc.c  */
#line 622 "../src/parse_tdf.y"
    { (yyval.inputSpecs)=(yyvsp[(1) - (3)].inputSpecs);			(yyval.inputSpecs)->append((yyvsp[(3) - (3)].inputSpec)); ;}
    break;

  case 66:

/* Line 1455 of yacc.c  */
#line 624 "../src/parse_tdf.y"
    { (yyval.inputSpecs)=new list<InputSpec*>;	(yyval.inputSpecs)->append((yyvsp[(1) - (1)].inputSpec)); ;}
    break;

  case 67:

/* Line 1455 of yacc.c  */
#line 629 "../src/parse_tdf.y"
    { (yyval.inputSpec)=new InputSpec((yyvsp[(1) - (1)].token),lookup((yyvsp[(1) - (1)].token)),NULL,false,false); ;}
    break;

  case 68:

/* Line 1455 of yacc.c  */
#line 631 "../src/parse_tdf.y"
    { (yyval.inputSpec)=new InputSpec((yyvsp[(1) - (3)].token),lookup((yyvsp[(1) - (3)].token)),(yyvsp[(3) - (3)].expr),false,false); ;}
    break;

  case 69:

/* Line 1455 of yacc.c  */
#line 633 "../src/parse_tdf.y"
    { (yyval.inputSpec)=new InputSpec((yyvsp[(3) - (4)].token),lookup((yyvsp[(3) - (4)].token)),NULL,true,false); ;}
    break;

  case 70:

/* Line 1455 of yacc.c  */
#line 635 "../src/parse_tdf.y"
    { (yyval.inputSpec)=new InputSpec((yyvsp[(3) - (4)].token),lookup((yyvsp[(3) - (4)].token)),NULL,false,true); ;}
    break;

  case 71:

/* Line 1455 of yacc.c  */
#line 640 "../src/parse_tdf.y"
    { (yyval.stmts)=(yyvsp[(1) - (1)].stmts); ;}
    break;

  case 72:

/* Line 1455 of yacc.c  */
#line 642 "../src/parse_tdf.y"
    { (yyval.stmts)=new list<Stmt*>; ;}
    break;

  case 73:

/* Line 1455 of yacc.c  */
#line 647 "../src/parse_tdf.y"
    { (yyval.stmts)=(yyvsp[(1) - (2)].stmts);		if ((yyvsp[(2) - (2)].stmt)) (yyval.stmts)->append((yyvsp[(2) - (2)].stmt)); ;}
    break;

  case 74:

/* Line 1455 of yacc.c  */
#line 649 "../src/parse_tdf.y"
    { (yyval.stmts)=new list<Stmt*>;	if ((yyvsp[(1) - (1)].stmt)) (yyval.stmts)->append((yyvsp[(1) - (1)].stmt)); ;}
    break;

  case 75:

/* Line 1455 of yacc.c  */
#line 654 "../src/parse_tdf.y"
    { (yyval.stmt)=(yyvsp[(1) - (1)].stmt); ;}
    break;

  case 76:

/* Line 1455 of yacc.c  */
#line 656 "../src/parse_tdf.y"
    { (yyval.stmt)=NULL; ;}
    break;

  case 77:

/* Line 1455 of yacc.c  */
#line 661 "../src/parse_tdf.y"
    { (yyval.stmt)=(yyvsp[(1) - (1)].stmt); ;}
    break;

  case 78:

/* Line 1455 of yacc.c  */
#line 663 "../src/parse_tdf.y"
    { (yyval.stmt)=(yyvsp[(1) - (1)].stmt); ;}
    break;

  case 79:

/* Line 1455 of yacc.c  */
#line 668 "../src/parse_tdf.y"
    { (yyval.stmt)=new StmtIf((yyvsp[(1) - (7)].token),(yyvsp[(3) - (7)].expr),(yyvsp[(5) - (7)].stmt),(yyvsp[(7) - (7)].stmt)); ;}
    break;

  case 80:

/* Line 1455 of yacc.c  */
#line 670 "../src/parse_tdf.y"
    { (yyval.stmt)=new StmtGoto((yyvsp[(1) - (3)].token),(yyvsp[(2) - (3)].token)->str); ;}
    break;

  case 81:

/* Line 1455 of yacc.c  */
#line 672 "../src/parse_tdf.y"
    { (yyval.stmt)=new StmtGoto((yyvsp[(1) - (2)].token),NULL); ;}
    break;

  case 82:

/* Line 1455 of yacc.c  */
#line 674 "../src/parse_tdf.y"
    { (yyval.stmt)=(yyvsp[(1) - (2)].stmt); ;}
    break;

  case 83:

/* Line 1455 of yacc.c  */
#line 676 "../src/parse_tdf.y"
    { (yyval.stmt)=(yyvsp[(1) - (2)].stmt); ;}
    break;

  case 84:

/* Line 1455 of yacc.c  */
#line 678 "../src/parse_tdf.y"
    { (yyval.stmt)=new StmtCall((yyvsp[(1) - (2)].expr)->getToken(),(ExprCall*)(yyvsp[(1) - (2)].expr)); ;}
    break;

  case 85:

/* Line 1455 of yacc.c  */
#line 680 "../src/parse_tdf.y"
    { (yyval.stmt)=(yyvsp[(1) - (1)].stmt); ;}
    break;

  case 86:

/* Line 1455 of yacc.c  */
#line 685 "../src/parse_tdf.y"
    { (yyval.stmt)=new StmtIf((yyvsp[(1) - (5)].token),(yyvsp[(3) - (5)].expr),(yyvsp[(5) - (5)].stmt)); ;}
    break;

  case 87:

/* Line 1455 of yacc.c  */
#line 687 "../src/parse_tdf.y"
    { (yyval.stmt)=new StmtIf((yyvsp[(1) - (7)].token),(yyvsp[(3) - (7)].expr),(yyvsp[(5) - (7)].stmt),(yyvsp[(7) - (7)].stmt)); ;}
    break;

  case 88:

/* Line 1455 of yacc.c  */
#line 692 "../src/parse_tdf.y"
    {
			  // typeDecls_opt leaves pushed symtab
			  (yyval.stmt)=new StmtBlock((yyvsp[(1) - (4)].token),gSymtab,(yyvsp[(3) - (4)].stmts));
			  symtabPop();
			;}
    break;

  case 89:

/* Line 1455 of yacc.c  */
#line 701 "../src/parse_tdf.y"
    {
			  list<Expr*> *args=new list<Expr*>;
			  args->append((yyvsp[(3) - (4)].expr));
			  ExprBuiltin *eb=new ExprBuiltin((yyvsp[(1) - (4)].token),args,
							  builtin_attn);
			  (yyval.stmt)=new StmtBuiltin((yyvsp[(1) - (4)].token),eb);
			;}
    break;

  case 90:

/* Line 1455 of yacc.c  */
#line 709 "../src/parse_tdf.y"
    {
			  ExprLValue *el=new ExprLValue((yyvsp[(3) - (4)].token),lookup((yyvsp[(3) - (4)].token)));
			  list<Expr*> *args=new list<Expr*>;
			  args->append(el);
			  ExprBuiltin *eb=new ExprBuiltin((yyvsp[(1) - (4)].token),args,
							  builtin_close);
			  (yyval.stmt)=new StmtBuiltin((yyvsp[(1) - (4)].token),eb);
			;}
    break;

  case 91:

/* Line 1455 of yacc.c  */
#line 718 "../src/parse_tdf.y"
    {
			  ExprLValue *el=new ExprLValue((yyvsp[(3) - (4)].token),lookup((yyvsp[(3) - (4)].token)));
			  list<Expr*> *args=new list<Expr*>;
			  args->append(el);
			  ExprBuiltin *eb=new ExprBuiltin((yyvsp[(1) - (4)].token),args,
							  builtin_frameclose);
			  (yyval.stmt)=new StmtBuiltin((yyvsp[(1) - (4)].token),eb);
			;}
    break;

  case 92:

/* Line 1455 of yacc.c  */
#line 727 "../src/parse_tdf.y"
    {
			  list<Expr*> *args=new list<Expr*>;
			  args->append((Expr*)expr_0->duplicate());   // sync 0
			  ExprBuiltin *eb=new ExprBuiltin((yyvsp[(1) - (3)].token),args,
							  builtin_done);
			  (yyval.stmt)=new StmtBuiltin((yyvsp[(1) - (3)].token),eb);
			;}
    break;

  case 93:

/* Line 1455 of yacc.c  */
#line 735 "../src/parse_tdf.y"
    {
			  list<Expr*> *args=(yyvsp[(4) - (5)].args);
			  ExprBuiltin *eb=new ExprBuiltin((yyvsp[(1) - (5)].token),args,
							  builtin_printf);
			  eb->setAnnote_(ANNOTE_PRINTF_STRING_TOKEN,(yyvsp[(3) - (5)].token));
			  (yyval.stmt)=new StmtBuiltin((yyvsp[(1) - (5)].token),eb);
			;}
    break;

  case 94:

/* Line 1455 of yacc.c  */
#line 743 "../src/parse_tdf.y"
    { (yyval.stmt)=(yyvsp[(1) - (1)].stmt); ;}
    break;

  case 95:

/* Line 1455 of yacc.c  */
#line 745 "../src/parse_tdf.y"
    { (yyval.stmt)=(yyvsp[(1) - (1)].stmt); ;}
    break;

  case 96:

/* Line 1455 of yacc.c  */
#line 750 "../src/parse_tdf.y"
    { (yyval.stmt)=(yyvsp[(1) - (1)].stmt); ;}
    break;

  case 97:

/* Line 1455 of yacc.c  */
#line 752 "../src/parse_tdf.y"
    { (yyval.stmt)=(yyvsp[(1) - (1)].stmt); ;}
    break;

  case 98:

/* Line 1455 of yacc.c  */
#line 757 "../src/parse_tdf.y"
    {
			  ExprBuiltin *eb=new ExprBuiltin((yyvsp[(1) - (4)].token),(yyvsp[(3) - (4)].args),builtin_copy);
			  (yyval.stmt)=new StmtBuiltin((yyvsp[(1) - (4)].token),eb);
			;}
    break;

  case 99:

/* Line 1455 of yacc.c  */
#line 765 "../src/parse_tdf.y"
    {
			  OperatorSegment *o
			    = new OperatorSegment((yyvsp[(1) - (4)].token),string("_dummy_seg_op"),
						  SEGMENT_R,
						  type_none,new list<Symbol*>);
			  ExprBuiltin *e=new ExprBuiltin((yyvsp[(1) - (4)].token),(yyvsp[(3) - (4)].args),o);
			  (yyval.stmt)=new StmtBuiltin((yyvsp[(1) - (4)].token),e);
			;}
    break;

  case 100:

/* Line 1455 of yacc.c  */
#line 774 "../src/parse_tdf.y"
    {
			  OperatorSegment *o
			    = new OperatorSegment((yyvsp[(1) - (4)].token),string("_dummy_seg_op"),
						  SEGMENT_W,
						  type_none,new list<Symbol*>);
			  ExprBuiltin *e=new ExprBuiltin((yyvsp[(1) - (4)].token),(yyvsp[(3) - (4)].args),o);
			  (yyval.stmt)=new StmtBuiltin((yyvsp[(1) - (4)].token),e);
			;}
    break;

  case 101:

/* Line 1455 of yacc.c  */
#line 783 "../src/parse_tdf.y"
    {
			  OperatorSegment *o
			    = new OperatorSegment((yyvsp[(1) - (4)].token),string("_dummy_seg_op"),
						  SEGMENT_RW,
						  type_none,new list<Symbol*>);
			  ExprBuiltin *e=new ExprBuiltin((yyvsp[(1) - (4)].token),(yyvsp[(3) - (4)].args),o);
			  (yyval.stmt)=new StmtBuiltin((yyvsp[(1) - (4)].token),e);
			;}
    break;

  case 102:

/* Line 1455 of yacc.c  */
#line 792 "../src/parse_tdf.y"
    {
			  OperatorSegment *o
			    = new OperatorSegment((yyvsp[(1) - (4)].token),string("_dummy_seg_op"),
						  SEGMENT_SEQ_R,
						  type_none,new list<Symbol*>);
			  ExprBuiltin *e=new ExprBuiltin((yyvsp[(1) - (4)].token),(yyvsp[(3) - (4)].args),o);
			  (yyval.stmt)=new StmtBuiltin((yyvsp[(1) - (4)].token),e);
			;}
    break;

  case 103:

/* Line 1455 of yacc.c  */
#line 801 "../src/parse_tdf.y"
    {
			  OperatorSegment *o
			    = new OperatorSegment((yyvsp[(1) - (4)].token),string("_dummy_seg_op"),
						  SEGMENT_SEQ_W,
						  type_none,new list<Symbol*>);
			  ExprBuiltin *e=new ExprBuiltin((yyvsp[(1) - (4)].token),(yyvsp[(3) - (4)].args),o);
			  (yyval.stmt)=new StmtBuiltin((yyvsp[(1) - (4)].token),e);
			;}
    break;

  case 104:

/* Line 1455 of yacc.c  */
#line 810 "../src/parse_tdf.y"
    {
			  OperatorSegment *o
			    = new OperatorSegment((yyvsp[(1) - (4)].token),string("_dummy_seg_op"),
						  SEGMENT_SEQ_RW,
						  type_none,new list<Symbol*>);
			  ExprBuiltin *e=new ExprBuiltin((yyvsp[(1) - (4)].token),(yyvsp[(3) - (4)].args),o);
			  (yyval.stmt)=new StmtBuiltin((yyvsp[(1) - (4)].token),e);
			;}
    break;

  case 105:

/* Line 1455 of yacc.c  */
#line 822 "../src/parse_tdf.y"
    { (yyval.expr)=new ExprCall((yyvsp[(1) - (4)].token),(yyvsp[(3) - (4)].args)); ;}
    break;

  case 106:

/* Line 1455 of yacc.c  */
#line 827 "../src/parse_tdf.y"
    { (yyval.args)=(yyvsp[(1) - (1)].args); ;}
    break;

  case 107:

/* Line 1455 of yacc.c  */
#line 829 "../src/parse_tdf.y"
    {
			  // empty args list implies 0-width sync stream
			  (yyval.args)=new list<Expr*>;
			  (yyval.args)->append((Expr*)expr_0->duplicate());
			;}
    break;

  case 108:

/* Line 1455 of yacc.c  */
#line 838 "../src/parse_tdf.y"
    { (yyval.args)=(yyvsp[(2) - (2)].args); ;}
    break;

  case 109:

/* Line 1455 of yacc.c  */
#line 840 "../src/parse_tdf.y"
    { (yyval.args)=new list<Expr*>; ;}
    break;

  case 110:

/* Line 1455 of yacc.c  */
#line 845 "../src/parse_tdf.y"
    { (yyval.args)=(yyvsp[(1) - (3)].args);		(yyval.args)->append((yyvsp[(3) - (3)].expr)); ;}
    break;

  case 111:

/* Line 1455 of yacc.c  */
#line 847 "../src/parse_tdf.y"
    { (yyval.args)=new list<Expr*>;	(yyval.args)->append((yyvsp[(1) - (1)].expr)); ;}
    break;

  case 112:

/* Line 1455 of yacc.c  */
#line 852 "../src/parse_tdf.y"
    { (yyval.stmt)=new StmtAssign((yyvsp[(2) - (3)].token),(ExprLValue*)(yyvsp[(1) - (3)].expr),(yyvsp[(3) - (3)].expr)); ;}
    break;

  case 113:

/* Line 1455 of yacc.c  */
#line 857 "../src/parse_tdf.y"
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

  case 114:

/* Line 1455 of yacc.c  */
#line 870 "../src/parse_tdf.y"
    {
			  Symbol *s=lookup((yyvsp[(1) - (4)].token));
			  if (s->getType()->getTypeKind()==TYPE_ARRAY)
			    (yyval.expr)=new ExprLValue((yyvsp[(1) - (4)].token),lookup((yyvsp[(1) - (4)].token)),NULL,NULL,NULL,(yyvsp[(3) - (4)].expr));
			  else
			    (yyval.expr)=new ExprLValue((yyvsp[(1) - (4)].token),lookup((yyvsp[(1) - (4)].token)),(yyvsp[(3) - (4)].expr));
			;}
    break;

  case 115:

/* Line 1455 of yacc.c  */
#line 878 "../src/parse_tdf.y"
    { (yyval.expr)=new ExprLValue((yyvsp[(1) - (6)].token),lookup((yyvsp[(1) - (6)].token)),(yyvsp[(5) - (6)].expr),(yyvsp[(3) - (6)].expr)); ;}
    break;

  case 116:

/* Line 1455 of yacc.c  */
#line 880 "../src/parse_tdf.y"
    { (yyval.expr)=new ExprLValue((yyvsp[(1) - (7)].token),lookup((yyvsp[(1) - (7)].token)),(yyvsp[(6) - (7)].expr),NULL,NULL,(yyvsp[(3) - (7)].expr)); ;}
    break;

  case 117:

/* Line 1455 of yacc.c  */
#line 882 "../src/parse_tdf.y"
    { (yyval.expr)=new ExprLValue((yyvsp[(1) - (9)].token),lookup((yyvsp[(1) - (9)].token)),(yyvsp[(8) - (9)].expr),(yyvsp[(6) - (9)].expr),NULL,(yyvsp[(3) - (9)].expr)); ;}
    break;

  case 118:

/* Line 1455 of yacc.c  */
#line 887 "../src/parse_tdf.y"
    { (yyval.expr)=(yyvsp[(1) - (1)].expr); ;}
    break;

  case 119:

/* Line 1455 of yacc.c  */
#line 889 "../src/parse_tdf.y"
    {
			  TypeArray *type=new TypeArray(type_any,(yyvsp[(2) - (3)].exprs)->size());
			  (yyval.expr)=new ExprArray((yyvsp[(1) - (3)].token),type,(yyvsp[(2) - (3)].exprs));
			;}
    break;

  case 120:

/* Line 1455 of yacc.c  */
#line 897 "../src/parse_tdf.y"
    { (yyval.exprs)=(yyvsp[(1) - (1)].exprs); ;}
    break;

  case 121:

/* Line 1455 of yacc.c  */
#line 899 "../src/parse_tdf.y"
    { (yyval.exprs)=new list<Expr*>; ;}
    break;

  case 122:

/* Line 1455 of yacc.c  */
#line 904 "../src/parse_tdf.y"
    { (yyval.exprs)=(yyvsp[(1) - (3)].exprs); (yyval.exprs)->append((yyvsp[(3) - (3)].expr)); ;}
    break;

  case 123:

/* Line 1455 of yacc.c  */
#line 906 "../src/parse_tdf.y"
    { (yyval.exprs)=new list<Expr*>; (yyval.exprs)->append((yyvsp[(1) - (1)].expr)); ;}
    break;

  case 124:

/* Line 1455 of yacc.c  */
#line 911 "../src/parse_tdf.y"
    { (yyval.expr)=new ExprCond((yyvsp[(2) - (5)].token),(yyvsp[(1) - (5)].expr),(yyvsp[(3) - (5)].expr),(yyvsp[(5) - (5)].expr)); ;}
    break;

  case 125:

/* Line 1455 of yacc.c  */
#line 913 "../src/parse_tdf.y"
    { (yyval.expr)=(yyvsp[(1) - (1)].expr); ;}
    break;

  case 126:

/* Line 1455 of yacc.c  */
#line 918 "../src/parse_tdf.y"
    { (yyval.expr)=new ExprBop((yyvsp[(2) - (3)].token),LOGIC_OR,(yyvsp[(1) - (3)].expr),(yyvsp[(3) - (3)].expr)); ;}
    break;

  case 127:

/* Line 1455 of yacc.c  */
#line 920 "../src/parse_tdf.y"
    { (yyval.expr)=(yyvsp[(1) - (1)].expr); ;}
    break;

  case 128:

/* Line 1455 of yacc.c  */
#line 925 "../src/parse_tdf.y"
    { (yyval.expr)=new ExprBop((yyvsp[(2) - (3)].token),LOGIC_AND,(yyvsp[(1) - (3)].expr),(yyvsp[(3) - (3)].expr)); ;}
    break;

  case 129:

/* Line 1455 of yacc.c  */
#line 927 "../src/parse_tdf.y"
    { (yyval.expr)=(yyvsp[(1) - (1)].expr); ;}
    break;

  case 130:

/* Line 1455 of yacc.c  */
#line 932 "../src/parse_tdf.y"
    { (yyval.expr)=new ExprBop((yyvsp[(2) - (3)].token),(yyvsp[(2) - (3)].token)->code,(yyvsp[(1) - (3)].expr),(yyvsp[(3) - (3)].expr)); ;}
    break;

  case 131:

/* Line 1455 of yacc.c  */
#line 934 "../src/parse_tdf.y"
    { (yyval.expr)=(yyvsp[(1) - (1)].expr); ;}
    break;

  case 132:

/* Line 1455 of yacc.c  */
#line 939 "../src/parse_tdf.y"
    { (yyval.expr)=new ExprBop((yyvsp[(2) - (3)].token),(yyvsp[(2) - (3)].token)->code,(yyvsp[(1) - (3)].expr),(yyvsp[(3) - (3)].expr)); ;}
    break;

  case 133:

/* Line 1455 of yacc.c  */
#line 941 "../src/parse_tdf.y"
    { (yyval.expr)=(yyvsp[(1) - (1)].expr); ;}
    break;

  case 134:

/* Line 1455 of yacc.c  */
#line 946 "../src/parse_tdf.y"
    { (yyval.expr)=new ExprBop((yyvsp[(2) - (3)].token),(yyvsp[(2) - (3)].token)->code,(yyvsp[(1) - (3)].expr),(yyvsp[(3) - (3)].expr)); ;}
    break;

  case 135:

/* Line 1455 of yacc.c  */
#line 948 "../src/parse_tdf.y"
    { (yyval.expr)=(yyvsp[(1) - (1)].expr); ;}
    break;

  case 136:

/* Line 1455 of yacc.c  */
#line 953 "../src/parse_tdf.y"
    { (yyval.expr)=new ExprBop((yyvsp[(2) - (3)].token),(yyvsp[(2) - (3)].token)->code,(yyvsp[(1) - (3)].expr),(yyvsp[(3) - (3)].expr)); ;}
    break;

  case 137:

/* Line 1455 of yacc.c  */
#line 955 "../src/parse_tdf.y"
    { (yyval.expr)=(yyvsp[(1) - (1)].expr); ;}
    break;

  case 138:

/* Line 1455 of yacc.c  */
#line 960 "../src/parse_tdf.y"
    { (yyval.expr)=new ExprBop((yyvsp[(2) - (3)].token),(yyvsp[(2) - (3)].token)->code,(yyvsp[(1) - (3)].expr),(yyvsp[(3) - (3)].expr)); ;}
    break;

  case 139:

/* Line 1455 of yacc.c  */
#line 962 "../src/parse_tdf.y"
    { (yyval.expr)=(yyvsp[(1) - (1)].expr); ;}
    break;

  case 140:

/* Line 1455 of yacc.c  */
#line 967 "../src/parse_tdf.y"
    { (yyval.expr)=new ExprBop((yyvsp[(2) - (3)].token),(yyvsp[(2) - (3)].token)->code,(yyvsp[(1) - (3)].expr),(yyvsp[(3) - (3)].expr)); ;}
    break;

  case 141:

/* Line 1455 of yacc.c  */
#line 969 "../src/parse_tdf.y"
    { (yyval.expr)=(yyvsp[(1) - (1)].expr); ;}
    break;

  case 142:

/* Line 1455 of yacc.c  */
#line 974 "../src/parse_tdf.y"
    { (yyval.expr)=new ExprBop((yyvsp[(2) - (3)].token),(yyvsp[(2) - (3)].token)->code,(yyvsp[(1) - (3)].expr),(yyvsp[(3) - (3)].expr)); ;}
    break;

  case 143:

/* Line 1455 of yacc.c  */
#line 976 "../src/parse_tdf.y"
    { (yyval.expr)=(yyvsp[(1) - (1)].expr); ;}
    break;

  case 144:

/* Line 1455 of yacc.c  */
#line 981 "../src/parse_tdf.y"
    { (yyval.expr)=new ExprBop((yyvsp[(2) - (3)].token),(yyvsp[(2) - (3)].token)->code,(yyvsp[(1) - (3)].expr),(yyvsp[(3) - (3)].expr)); ;}
    break;

  case 145:

/* Line 1455 of yacc.c  */
#line 983 "../src/parse_tdf.y"
    { (yyval.expr)=(yyvsp[(1) - (1)].expr); ;}
    break;

  case 146:

/* Line 1455 of yacc.c  */
#line 988 "../src/parse_tdf.y"
    { (yyval.expr)=new ExprUop((yyvsp[(1) - (2)].token),(yyvsp[(1) - (2)].token)->code,(yyvsp[(2) - (2)].expr)); ;}
    break;

  case 147:

/* Line 1455 of yacc.c  */
#line 990 "../src/parse_tdf.y"
    { (yyval.expr)=new ExprUop((yyvsp[(1) - (4)].token),(yyvsp[(1) - (4)].token)->code,(yyvsp[(3) - (4)].expr)); ;}
    break;

  case 148:

/* Line 1455 of yacc.c  */
#line 992 "../src/parse_tdf.y"
    { (yyval.expr)=new ExprUop((yyvsp[(1) - (4)].token),(yyvsp[(1) - (4)].token)->code,(yyvsp[(3) - (4)].expr)); ;}
    break;

  case 149:

/* Line 1455 of yacc.c  */
#line 994 "../src/parse_tdf.y"
    { (yyval.expr)=new ExprUop((yyvsp[(1) - (4)].token),(yyvsp[(1) - (4)].token)->code,(yyvsp[(3) - (4)].expr)); ;}
    break;

  case 150:

/* Line 1455 of yacc.c  */
#line 996 "../src/parse_tdf.y"
    { (yyval.expr)=new ExprUop((yyvsp[(1) - (4)].token),(yyvsp[(1) - (4)].token)->code,(yyvsp[(3) - (4)].expr)); ;}
    break;

  case 151:

/* Line 1455 of yacc.c  */
#line 998 "../src/parse_tdf.y"
    { (yyval.expr)=new ExprUop((yyvsp[(1) - (4)].token),(yyvsp[(1) - (4)].token)->code,(yyvsp[(3) - (4)].expr)); ;}
    break;

  case 152:

/* Line 1455 of yacc.c  */
#line 1000 "../src/parse_tdf.y"
    { (yyval.expr)=new ExprCast((yyvsp[(1) - (4)].token),(yyvsp[(2) - (4)].type),(yyvsp[(4) - (4)].expr)); ;}
    break;

  case 153:

/* Line 1455 of yacc.c  */
#line 1002 "../src/parse_tdf.y"
    { (yyval.expr)=new ExprCast((yyvsp[(1) - (4)].token),(yyvsp[(2) - (4)].token)->code==SIGNED?true:false,(yyvsp[(4) - (4)].expr)); ;}
    break;

  case 154:

/* Line 1455 of yacc.c  */
#line 1004 "../src/parse_tdf.y"
    { (yyval.expr)=(yyvsp[(1) - (1)].expr); ;}
    break;

  case 155:

/* Line 1455 of yacc.c  */
#line 1009 "../src/parse_tdf.y"
    { (yyval.expr)=(yyvsp[(1) - (1)].expr); ;}
    break;

  case 156:

/* Line 1455 of yacc.c  */
#line 1033 "../src/parse_tdf.y"
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

  case 157:

/* Line 1455 of yacc.c  */
#line 1061 "../src/parse_tdf.y"
    { (yyval.expr)=(yyvsp[(1) - (1)].expr); ;}
    break;

  case 158:

/* Line 1455 of yacc.c  */
#line 1066 "../src/parse_tdf.y"
    { (yyval.expr)=(yyvsp[(1) - (1)].expr); ;}
    break;

  case 159:

/* Line 1455 of yacc.c  */
#line 1068 "../src/parse_tdf.y"
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

  case 160:

/* Line 1455 of yacc.c  */
#line 1081 "../src/parse_tdf.y"
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

  case 161:

/* Line 1455 of yacc.c  */
#line 1094 "../src/parse_tdf.y"
    {
			  (yyval.expr)=(Expr*)new ExprValue((yyvsp[(1) - (1)].token), new Type(TYPE_DOUBLE),strtod((yyvsp[(1) - (1)].token)->str,NULL));
			;}
    break;

  case 162:

/* Line 1455 of yacc.c  */
#line 1098 "../src/parse_tdf.y"
    { (yyval.expr)=(Expr*)new ExprValue((yyvsp[(1) - (1)].token),new Type(TYPE_BOOL),1,0); ;}
    break;

  case 163:

/* Line 1455 of yacc.c  */
#line 1100 "../src/parse_tdf.y"
    { (yyval.expr)=(Expr*)new ExprValue((yyvsp[(1) - (1)].token),new Type(TYPE_BOOL),0,0); ;}
    break;

  case 164:

/* Line 1455 of yacc.c  */
#line 1102 "../src/parse_tdf.y"
    { (yyval.expr)=(yyvsp[(1) - (1)].expr); ;}
    break;

  case 165:

/* Line 1455 of yacc.c  */
#line 1104 "../src/parse_tdf.y"
    { (yyval.expr)=(yyvsp[(1) - (1)].expr); ;}
    break;

  case 166:

/* Line 1455 of yacc.c  */
#line 1106 "../src/parse_tdf.y"
    { (yyval.expr)=(yyvsp[(2) - (3)].expr); ;}
    break;

  case 167:

/* Line 1455 of yacc.c  */
#line 1111 "../src/parse_tdf.y"
    { (yyval.expr)=new ExprBuiltin((yyvsp[(1) - (4)].token),(yyvsp[(3) - (4)].args),builtin_cat); ;}
    break;

  case 168:

/* Line 1455 of yacc.c  */
#line 1122 "../src/parse_tdf.y"
    {
			  list<Expr*> *args=new list<Expr*>;
			  args->append((yyvsp[(3) - (4)].expr));
			  (yyval.expr)=new ExprBuiltin((yyvsp[(1) - (4)].token),args,builtin_widthof);
			;}
    break;

  case 169:

/* Line 1455 of yacc.c  */
#line 1128 "../src/parse_tdf.y"
    {
			  list<Expr*> *args=new list<Expr*>;
			  args->append((yyvsp[(3) - (4)].expr));
			  (yyval.expr)=new ExprBuiltin((yyvsp[(1) - (4)].token),args,builtin_bitsof);
			;}
    break;



/* Line 1455 of yacc.c  */
#line 3445 "parse_tdf.tab.cc"
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
      /* If just tried and failed to reuse lookahead token after an
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

  /* Else will try to reuse lookahead token after shifting the error
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

#if !defined(yyoverflow) || YYERROR_VERBOSE
/*-------------------------------------------------.
| yyexhaustedlab -- memory exhaustion comes here.  |
`-------------------------------------------------*/
yyexhaustedlab:
  yyerror (YY_("memory exhausted"));
  yyresult = 2;
  /* Fall through.  */
#endif

yyreturn:
  if (yychar != YYEMPTY)
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



/* Line 1675 of yacc.c  */
#line 1190 "../src/parse_tdf.y"


