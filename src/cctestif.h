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
// SCORE TDF compiler:  generate C++ output
// $Revision: 1.124 $
//
//////////////////////////////////////////////////////////////////////////////

#include <iostream>
#include <fstream>
#include "stmt.h"

using std::ofstream;

void Display(Stmt *stmt);

void ccGappaIfStmt(ofstream *fout, string indent, Stmt *stmt, int *early_close,
	    string state_prefix, bool in_pagestep, bool retime=true,  string type="",
	    string precision="", string classname="",  int *if_nb=NULL, string **variables = NULL , int nb_variables = 0, 
	    string* backup =NULL, string previous_cond = "", bool epart = true);
	    
void count_variable(Stmt *stmt, int* i);

void detect_if(Stmt *stmt, bool *if_detected, int *nb_nesting);

void ccGappaIfStmt(ofstream *fout, Stmt *stmt, string type, string precision, int nb_var, string **table_var, 
					string cond = "", Stmt* elsepart = NULL, bool treat_epart =false);

// Note: mblaze is Microblaze and not Matt Blaze!


