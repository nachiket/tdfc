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
// SCORE TDF compiler:  TDF parser globals
// $Revision: 1.119 $
//
//////////////////////////////////////////////////////////////////////////////


#ifndef _TDF_PARSE_
#define _TDF_PARSE_

#include "state.h"
#include <LEDA/core/string.h>
#include <LEDA/core/list.h>
//#include <LEDA/core/set.h>
#include <LEDA/core/dictionary.h>
#include <stdio.h>

// for parse_tdf.y
using leda::string;
using leda::list;
using leda::dic_item;

class Suite;


class Token
{
public:
  int		code;
  int		line;
  const string	*file;
  string	str;

  Token (int code_i, int line_i, const string *file_i, const string &str_i)
    : code(code_i), line(line_i), file(file_i), str(str_i) {}
  Token (int code_i, int line_i, const string *file_i, const char *str_i)
    : code(code_i), line(line_i), file(file_i), str(str_i) {}
};


// - flex/bison stuff
extern void	yyerror	(const char *msg);
extern int	yylex	(void);
extern int	yyparse	(void);
extern int	yydebug;
extern FILE	*yyin, *yyout;


// - globals filled during parse:
extern	char	*gProgName;	// argv[0]
extern	string	*gFileName;	// file being parsed
extern	int	 gLineNum;	// line being parsed
extern	bool	 gParsing;	// true during parse
extern	Suite	*gSuite;	// filled with operators by parser


// - declare token codes and YYSTYPE (yacc token union)
//   inside all files except parser itself (includes/declares its own)

#ifndef YYBISON
class Token;
class Tree;
class Type;
class Symbol;
class SymTab;
class Suite;
class Operator;
class State;
class StateCase;
class InputSpec;
class Stmt;
class Expr;
class OpBody;
class OpSignature;
class TypeDeclElem;
#include "parse_tdf.tab.h"
#endif	// #ifndef YYBISON


#endif	// #ifndef _TDF_PARSE_
