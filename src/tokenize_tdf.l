%{
/*
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
// SCORE TDF compiler:  TDF tokenizer using flex
// $Revision: 1.145 $
//
//////////////////////////////////////////////////////////////////////////////
*/


#include <ctype.h>
#include <stdio.h>
#include <LEDA/core/string.h>
#include "parse.h"
#include "misc.h"

using leda::string;

extern YYSTYPE yylval;

#define return_token(code) { yylval.token=new Token(code,gLineNum,	\
						    gFileName,yytext);	\
			     return code; }

%}

return		[\n]
blank		[ \t\r]
ws		{blank}+
letter		[A-Za-z]
digit		[0-9]
binary_digit	[0-1]
hex_digit	[0-9A-Fa-f]
hex_digit_or_x	[0-9A-FXa-fx]
oct_digit	[0-7]
unders		[_]
openclosebr	[\{\}]
openclosebar	[\[\]]
openclosepar	[\(\)]
lgt		[><]
sign		[\-\+]
osym		[\~\!\@\#\%\^\&\*\/\=\|\;\:\,\.\?]
 /*
 bb		[bB]
 xx		[xX]
 zero		[0]
 at		[@]
 tilde		[\~]
 */
letter_under	({letter}|{unders})
id		{letter_under}({letter_under}|{digit})*
numbin		"0b"{binary_digit}+
numhex		"0x"{hex_digit}+
numoct		"0"{oct_digit}+
numdec		{digit}+
num		{numbin}|{numhex}|{numoct}|{numdec}
syms		({openclosebr}|{openclosebar}|{openclosepar}|{lgt}|{sign}|{osym})

%%

{return}	{ gLineNum++; }

{ws}		{ /* No action */ }

"//"		{ /* 1-line comment */
		  while (yyinput()!='\n');
		  gLineNum++;
		}

"%{"		{ /* C++ block, copied verbatim */
		  int    startLine  = gLineNum;
		  string *startFile = gFileName;
		  string pass;
		  /* not handling # lines inside pass through 
		     Obscure case, if exit pass through 
			in a DIFFERENT file than started, won't
			be updated at this level...
			(and LineNumber will be wrong)
		     ??? worth worrying about?
		  */
		  char a=yyinput();
		  while (true)
		  {
		    while (a!='\n')
		    {
		      pass += a;
		      a=yyinput();
		    } 
		    gLineNum++;
		    pass += a;
		    a=yyinput();
		    if (a=='%')
		    {
		      a=yyinput();
		      if (a=='}')
		      {
			yylval.token = new Token(PASS_THROUGH_EXCEPTION,
						 startLine,startFile,pass);
			return PASS_THROUGH_EXCEPTION;
		      }
		      else 
		      {	
			pass += '%';
		      }
		    }
		  }
		}
^"#"		{ /* CPP line number directives 
		     ...blah, this is gross...
		     really need a separate lexer/parser for this...
		  */
		  char    a	  = '#';
		  int     newLine = 0;
		  int     done	  = 0;
		  string *newFile = new string();
		  while (!isdigit(a) && (done==0))
		  {
		    if (a=='\n')
		    {
		      gLineNum++;
		      done=1;
		    }
		    else a=yyinput();
		  }
		  if (done==0)
		   {
		     newLine=(a-'0');
		     a=yyinput();
		     while (isdigit(a))
		     {
		       newLine=newLine*10+(a-'0');
		       a=yyinput();
		     }
		     gLineNum=newLine;
		     if (a=='\n') 
		       done=1;
		     while ((a!='\"') & (done==0))
		     {
		       if (a=='\n') done=1;
		       else a=yyinput();
		     }
		     if (done==0)
		     {
		       a=yyinput();
		       while (a!='\"' & (done==0))
		       {
			 if (a=='\n')
			   done=1;
			 else 
			 {
			   *newFile+=a;
			   a=yyinput();
			 }
		       }
		       if (done==0)
			 gFileName=newFile;
		       while (a!='\n')
			 a=yyinput();
		     }
		   }
		  // cout << "line directive: " << "line=" << gLineNum
		  //      << " file=" << *gFileName << '\n' ;
		}

"\""		{ /* character string */
		  yylval.token=new Token(STRING,gLineNum,gFileName,"");
		  while (1) {
		    int c=yyinput();
		    if (c==EOF) {
		      yylval.token->str = "\""+yylval.token->str(1,10);
		      fatal(1, string("Unterminated character string"),
			       yylval.token);
		    }
		    else if (c=='\"') {
		      if (yylval.token->str.tail(1)==string('\\'))
			yylval.token->str+=c;
		      else
			return STRING;
		    }
		    else {
		      yylval.token->str+=c;
		      if (c=='\n')
			gLineNum++;
		    }
		  }
		}

"if"		{ return_token(IF);		}
"else"		{ return_token(ELSE);		}
"state"		{ return_token(STATE);		}
"boolean"	{ return_token(BOOLEAN);	}
"float"		{ return_token(FLOAT);		}
"double"	{ return_token(DOUBLE);		}

"exp"		{ return_token(EXP);		}
"log"		{ return_token(LOG);		}
"sqrt"		{ return_token(SQRT);		}

"signed"	{ return_token(SIGNED);		}
"unsigned"	{ return_token(UNSIGNED);	}
 /* "fixed"	{ return_token(FIXED);		} */
"input"		{ return_token(INPUT);		}
"output"	{ return_token(OUTPUT);		}
"param"		{ return_token(PARAM);		}
"goto"		{ return_token(GOTO);		}
"stay"		{ return_token(STAY);		}
"attn"		{ return_token(ATTENTION);	}
"cat"		{ return_token(CAT);		}
"close"		{ return_token(CLOSE);		}
"copy"		{ return_token(COPY);		}
"done"		{ return_token(DONE);		}
"eos"		{ return_token(EOS);		}
"widthof"	{ return_token(WIDTHOF);	}
"bitsof"	{ return_token(BITSOF);		}
"printf"	{ return_token(PRINTF);		}
"segment_r"	{ return_token(SEGMENT_R_);	}
"segment_w"	{ return_token(SEGMENT_W_);	}
"segment_rw"	{ return_token(SEGMENT_RW_);	}
"segment_seq_r"	{ return_token(SEGMENT_SEQ_R_);	}
"segment_seq_w"	{ return_token(SEGMENT_SEQ_W_);	}
"segment_seq_rw" {return_token(SEGMENT_SEQ_RW_);}
"true"		{ return_token(TRUE);		}
"false"		{ return_token(FALSE);		}
">>"		{ return_token(RIGHT_SHIFT);	}
"<<"		{ return_token(LEFT_SHIFT);	}
"=="		{ return_token(EQUALS);		}
"!="		{ return_token(NOT_EQUALS);	}
"&&"		{ return_token(LOGIC_AND);	}
"||"		{ return_token(LOGIC_OR);	}
">="		{ return_token(GTE);		}
"<="		{ return_token(LTE);		}
{id}		{ return_token(ID_);		}
{num}		{ return_token(NUM);		}
{syms}		{ return_token(yytext[0]);	}

%%
