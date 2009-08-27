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
// SCORE TDF compiler:  emit Synplify project directory tree for -everilog
// $Revision: 1.5 $
//
//////////////////////////////////////////////////////////////////////////////

#include <fstream>
#include <sys/stat.h>
#include <sys/unistd.h>
#include <errno.h>
#include "operator.h"
#include "misc.h"
#include "synplify.h"

using std::ofstream;


////////////////////////////////////////////////////////////////
//  Synplify project directory structure
//
//    + Common project directory:    proj/
//    + Module project directory:    proj/<mod>/           (*)
//    + Project file:                proj/<mod>/<mod>.prj
//    + Project watch file:          proj/<mod>/<mod>.prd
//    + Implementation 1 directory:  proj/<mod>/rev_1__speed/
//    + Implementation 2 directory:  proj/<mod>/rev_2__area/
//    + Implementation 3 directory:  proj/<mod>/rev_3__no_optim/
//    + Implementation 4 directory:  proj/<mod>/rev_4__200mhz/
//
//  (*) For each TDF operator <op>, create 3 modules:
//        <op>  <op>_fsm  <op>_dp
//
////////////////////////////////////////////////////////////////


// - Permission (mode) for creating directories
#define MODE 0755

// - Names of the 3 implementations (revisions):
string rev1 = "rev_1__speed";
string rev2 = "rev_2__area";
string rev3 = "rev_3__no_optim";
string rev4 = "rev_4__200mhz";


////////////////////////////////////////////////////////////////

// - Target architecture:  smallest Virtex II
string gSynplifyTechnology = "VIRTEX2";
string gSynplifyPart       = "XC2V40";
string gSynplifyPackage    = "CS144";
string gSynplifySpeedGrade = "-6";

// - Target architecture:  large Virtex II
// string gSynplifyTechnology = "VIRTEX2";
// string gSynplifyPart       = "XC2V6000";
// string gSynplifyPackage    = "FF1152";
// string gSynplifySpeedGrade = "-6";

// - Target architecture:  smallest Virtex II Pro
// string gSynplifyTechnology = "VIRTEX2P";
// string gSynplifyPart       = "XC2VP2";
// string gSynplifyPackage    = "FG256";
// string gSynplifySpeedGrade = "-7";

// - Target architecture:  large Virtex II Pro
// string gSynplifyTechnology = "VIRTEX2P";
// string gSynplifyPart       = "XC2VP70";
// string gSynplifyPackage    = "FF1704";
// string gSynplifySpeedGrade = "-7";

// - Target architecture:  smallest Virtex 4
// string gSynplifyTechnology = "VIRTEX4";
// string gSynplifyPart       = "XC4VLX15";
// string gSynplifyPackage    = "FF668";
// string gSynplifySpeedGrade = "-11";

// - Target architecture:  large Virtex 4
// string gSynplifyTechnology = "VIRTEX4";
// string gSynplifyPart       = "XC4VLX80";
// string gSynplifyPackage    = "FF1148";
// string gSynplifySpeedGrade = "-11";

// - Target architecture:  smallest Spartan 3
// string gSynplifyTechnology = "SPARTAN3";
// string gSynplifyPart       = "XC3S50";
// string gSynplifyPackage    = "PQ208";
// string gSynplifySpeedGrade = "-5";

// - Target architecture:  large Spartan 3
// string gSynplifyTechnology = "SPARTAN3";
// string gSynplifyPart       = "XC3S5000";
// string gSynplifyPackage    = "FG900";
// string gSynplifySpeedGrade = "-5";


////////////////////////////////////////////////////////////////

int parseSynplifyTechStr (string str) {
  // - parse -syntech ``tech-part-pkg-speed'' cmd-line option
  // - on success, return 1, set gSynplifyTechnology, gSynplifyPart,
  //                             gSynplifyPackage,    gSynplifySpeedGrade
  // - on failure, return 0, leave gSynplify... vars undefined

  int pos1, pos2, pos3;
  if ((pos1=str.pos("-"))<0)		// - find 1st "-"
    return 0;
  gSynplifyTechnology=str(0,pos1-1);	// -      1st component = tech
  str=str.del(0,pos1);
  if ((pos2=str.pos("-"))<0)		// - find 2nd "-"
    return 0;
  gSynplifyPart=str(0,pos2-1);		// -      2nd component = part
  str=str.del(0,pos2);
  if ((pos3=str.pos("-"))<0)		// - find 3rd "-"
    return 0;
  gSynplifyPackage=str(0,pos3-1);	// -      3rd component = pkg
  str=str.del(0,pos3);
  if (str.pos("-")>=0)			// - find erroneous "-"
    return 0;
  gSynplifySpeedGrade="-"+str;		// -      4th component = speed

  // warn( "gSynplifyTechnology="+gSynplifyTechnology+
  //      " gSynplifyPart="      +gSynplifyPart+
  //      " gSynplifyPackage="   +gSynplifyPackage+
  //      " gSynplifySpeedGrade="+gSynplifySpeedGrade );

  return 1;
}


////////////////////////////////////////////////////////////////

void emitSynplify_prj (string module, string projPath, string verilogPath)
{
  // - emit Synplify project file "<module>.prj" for verilog <module>
  // - assume project file is:     <projPath>/<module>.prj
  // - assume module has source:   <verilogPath>/<module>.v

  string prjFile = projPath + "/" + module + ".prj";

  string common_impl_options =
    "#device options"		 					"\n"
    "set_option -technology  " + gSynplifyTechnology  +			"\n"
    "set_option -part        " + gSynplifyPart        +			"\n"
    "set_option -package     " + gSynplifyPackage     +			"\n"
    "set_option -speed_grade " + gSynplifySpeedGrade +			"\n"
									"\n"
    "#simulation options"						"\n"
    "set_option -write_verilog 0"					"\n"
    "set_option -write_vhdl    0"					"\n"
									"\n"
    "#automatic place and route (vendor) options"			"\n"
    "set_option -write_apr_constraint 1"				"\n"
									"\n"
    "#implementation attributes"					"\n"
    "set_option -vlog_std v2001"					"\n"
    "set_option -compiler_compatible 0"					"\n";

  string ret;

  ret += "#-- Synplicity, Inc."						"\n";
  ret += "#-- Version 7.1"						"\n";
  ret += "#-- Project file " + prjFile +				"\n";
  ret += "#-- " + tdfcComment();				    //	"\n";
  ret += 								"\n";
  ret += "#add_file options"						"\n";
  ret += "add_file -verilog \"" + verilogPath + "/" + module + ".v\""	"\n";
  ret += 								"\n";
  ret += "#reporting options"						"\n";
  ret += 								"\n";

  // ----- IMPLEMENTATION 1: SPEED -----
  ret += "#implementation: \"" + rev1 + "\""				"\n";
  ret += "impl -add " + rev1 + 						"\n";
  ret += 								"\n";
  ret += common_impl_options;					    //	"\n";
  ret += 								"\n";
  ret += "#compilation/mapping options"					"\n";
  ret += "set_option -default_enum_encoding default"			"\n";
  ret += "set_option -symbolic_fsm_compiler 1"				"\n";
  ret += "set_option -resource_sharing 1"				"\n";
  ret += "set_option -use_fsm_explorer 1"				"\n";
  ret += "set_option -top_module \"" + module + "\""			"\n";
  ret += 								"\n";
  ret += "#map options"							"\n";
  ret += "set_option -frequency 500.000"				"\n";
  ret += "set_option -fanout_limit 100"					"\n";
  ret += "set_option -disable_io_insertion 1"				"\n";
  ret += "set_option -pipe 1"						"\n";
  ret += "set_option -modular 0"					"\n";
  ret += "set_option -retiming 1"					"\n";
  ret += 								"\n";
  ret += "#set result format/file last"					"\n";
  ret += "project -result_file \"" + rev1 + "/" + module + ".edf\""	"\n";
  ret += 								"\n";

  // ----- IMPLEMENTATION 2: AREA -----
  ret += "#implementation: \"" + rev2 + "\""				"\n";
  ret += "impl -add " + rev2 + 						"\n";
  ret += 								"\n";
  ret += common_impl_options;					    //	"\n";
  ret += 								"\n";
  ret += "#compilation/mapping options"					"\n";
  ret += "set_option -default_enum_encoding default"			"\n";
  ret += "set_option -symbolic_fsm_compiler 1"				"\n";
  ret += "set_option -resource_sharing 1"				"\n";
  ret += "set_option -use_fsm_explorer 1"				"\n";
  ret += "set_option -top_module \"" + module + "\""			"\n";
  ret += 								"\n";
  ret += "#map options"							"\n";
  ret += "set_option -frequency 0.000"					"\n";
  ret += "set_option -fanout_limit 100"					"\n";
  ret += "set_option -disable_io_insertion 1"				"\n";
  ret += "set_option -pipe 1"						"\n";
  ret += "set_option -modular 0"					"\n";
  ret += "set_option -retiming 1"					"\n";
  ret += 								"\n";
  ret += "#set result format/file last"					"\n";
  ret += "project -result_file \"" + rev2 + "/" + module + ".edf\""	"\n";
  ret += 								"\n";

  // ----- IMPLEMENTATION 3: NO OPTIM -----
  ret += "#implementation: \"" + rev3 + "\""				"\n";
  ret += "impl -add " + rev3 + 						"\n";
  ret += 								"\n";
  ret += common_impl_options;					    //	"\n";
  ret += 								"\n";
  ret += "#compilation/mapping options"					"\n";
  ret += "set_option -default_enum_encoding default"			"\n";
  ret += "set_option -symbolic_fsm_compiler 0"				"\n";
  ret += "set_option -resource_sharing 0"				"\n";
  ret += "set_option -use_fsm_explorer 0"				"\n";
  ret += "set_option -top_module \"" + module + "\""			"\n";
  ret += 								"\n";
  ret += "#map options"							"\n";
  ret += "set_option -frequency 0.000"					"\n";
  ret += "set_option -fanout_limit 100"					"\n";
  ret += "set_option -disable_io_insertion 1"				"\n";
  ret += "set_option -pipe 0"						"\n";
  ret += "set_option -modular 0"					"\n";
  ret += "set_option -retiming 0"					"\n";
  ret += 								"\n";
  ret += "#set result format/file last"					"\n";
  ret += "project -result_file \"" + rev3 + "/" + module + ".edf\""	"\n";
  ret += 								"\n";

  // ----- IMPLEMENTATION 4: 200MHZ -----
  ret += "#implementation: \"" + rev4 + "\""				"\n";
  ret += "impl -add " + rev4 + 						"\n";
  ret += 								"\n";
  ret += common_impl_options;					    //	"\n";
  ret += 								"\n";
  ret += "#compilation/mapping options"					"\n";
  ret += "set_option -default_enum_encoding default"			"\n";
  ret += "set_option -symbolic_fsm_compiler 1"				"\n";
  ret += "set_option -resource_sharing 1"				"\n";
  ret += "set_option -use_fsm_explorer 1"				"\n";
  ret += "set_option -top_module \"" + module + "\""			"\n";
  ret += 								"\n";
  ret += "#map options"							"\n";
  ret += "set_option -frequency 200.000"				"\n";
  ret += "set_option -fanout_limit 100"					"\n";
  ret += "set_option -disable_io_insertion 1"				"\n";
  ret += "set_option -pipe 1"						"\n";
  ret += "set_option -modular 0"					"\n";
  ret += "set_option -retiming 1"					"\n";
  ret += 								"\n";
  ret += "#set result format/file last"					"\n";
  ret += "project -result_file \"" + rev4 + "/" + module + ".edf\""	"\n";
  ret += 								"\n";

  ret += "impl -active \"" + rev4 + "\""				"\n";

  ofstream fout(prjFile);
  if (!fout)
    fatal(1,"-everilog -synplify could not open output file " + prjFile);
  fout << ret;
}


void emitSynplify_prd (string module, string projPath, string verilogPath)
{
  // - emit Synplify project file "<module>.prd" for verilog <module>
  // - assume project file is:     <projPath>/<module>.prd
  // - assume module has source:   <verilogPath>/<module>.v

  string prdFile = projPath + "/" + module + ".prd";

  string ret;

  ret += "#-- Synplicity, Inc."						"\n";
  ret += "#-- Version 7.1"						"\n";
  ret += "#-- Project file " + prdFile +				"\n";
  ret += "#-- " + tdfcComment();				    //	"\n";
  ret += "#" 								"\n";
  ret += "### Watch Implementation type ###"				"\n";
  ret += "#" 								"\n";
  ret += "watch_impl -active"						"\n";
  ret += "#" 								"\n";
  ret += "### Watch Implementation properties ###"			"\n";
  ret += "#" 								"\n";
  ret += "watch_prop -clear"						"\n";

  ofstream fout(prdFile);
  if (!fout)
    fatal(1,"-everilog -synplify could not open output file " + prdFile);
  fout << ret;
}


void safeMkdir (const string &dir, int mode)
{
  mkdir(dir,mode);

  // - WARNING:  not checking whether mkdir failed  (how to do it?)
  // if (???)
  //   fatal(1,"-everilog -synplify could not create directory "+dir);
}


void emitSynplify_module (string module, string projPath, string verilogPath)
{
  // - emit Synplify project directory tree <projPath> for module <module>
  //     (<projPath>/<module>.prj, etc.)
  // - assume module has source <verilogPath>/<module>.v

  safeMkdir(projPath, MODE);			  // - project directory
  emitSynplify_prj(module,projPath,verilogPath);  // - project file       .prj
  emitSynplify_prd(module,projPath,verilogPath);  // - project watch file .prd
  safeMkdir(projPath+"/"+rev1, MODE);		  // - impl. 1 directory
  safeMkdir(projPath+"/"+rev2, MODE);		  // - impl. 2 directory
  safeMkdir(projPath+"/"+rev3, MODE);		  // - impl. 3 directory
  safeMkdir(projPath+"/"+rev4, MODE);		  // - impl. 4 directory
}


void emitSynplify (OperatorBehavioral *op)
{
  // - emit Synplify project directories for behavioral op:
  //     ./proj/<op>/  ./proj/<op>_fsm/  ./proj/<op>_dp/

  string modName = op->getName();		    // -     module name
  string fsmName = modName + "_fsm";		    // - FSM module name
  string  dpName = modName + "_dp";		    // - DP  module name

  string    projPath = "proj";			    // -     common project dir
  string modProjPath = projPath + "/" + modName;    // -     module project dir
  string fsmProjPath = projPath + "/" + fsmName;    // - FSM module project dir
  string  dpProjPath = projPath + "/" +  dpName;    // - DP  module project dir

  char *cwd = getcwd(NULL,0);
  string verilogPath = cwd;		// - Verilog source dir is CWD
  free(cwd);

  safeMkdir(projPath, MODE);
  emitSynplify_module(modName, modProjPath, verilogPath);
  emitSynplify_module(fsmName, fsmProjPath, verilogPath);
  emitSynplify_module( dpName,  dpProjPath, verilogPath);
}


void emitSynplifyCompose (OperatorCompose *op)
{
  // - emit Synplify project directories for compositional op:  (i.e. page)
  //     ./proj/<op>/  ./proj/<op>_q/
  // - emission for called behavioral operators is NOT handled here

  string  modName = op->getName();		    // -            module name
  string  qinName = modName + "_qin";		    // -    input q module name
  string    qName = modName + "_q";		    // - no input q module name
  string  dpqName = modName + "_dpq";		    // - no in +dps module name
  string noinName = modName + "_noin";		    // - no in +ops module name

  string     projPath = "proj";			    // -   common project dir
  string  modProjPath = projPath + "/" +  modName;  // -   module project dir
  string  qinProjPath = projPath + "/" +  qinName;  // -    input q prjct dir
  string    qProjPath = projPath + "/" +    qName;  // - no input q prjct dir
  string  dpqProjPath = projPath + "/" +  dpqName;  // - no in +dps prjct dir
  string noinProjPath = projPath + "/" + noinName;  // - no in +ops prjct dir

  char *cwd = getcwd(NULL,0);
  string verilogPath = cwd;		// - Verilog source dir is CWD
  free(cwd);

  safeMkdir(projPath, MODE);
  emitSynplify_module( modName,  modProjPath, verilogPath);
  emitSynplify_module( qinName,  qinProjPath, verilogPath);
  emitSynplify_module(   qName,    qProjPath, verilogPath);
  emitSynplify_module( dpqName,  dpqProjPath, verilogPath);
  emitSynplify_module(noinName, noinProjPath, verilogPath);
}


void emitSynplifyComposeAndCalled (OperatorCompose *op)
{
  // - emit Synplify project dirs/files for flat compositional *op
  //     and for all behavioral ops called inside *op

  emitSynplifyCompose((OperatorCompose*)op);		// - page + queues

  list<Operator*> calledops = findCalledOps(op);
  Operator *calledop;
  forall (calledop, calledops) {
    assert(calledop->getOpKind()==OP_BEHAVIORAL);
    emitSynplify((OperatorBehavioral*)calledop);	// - called behav ops
  }
}


void emitSynplify_instance (Operator *iop,
			    list<OperatorBehavioral*> *instances)
{
  if (iop->getOpKind()==OP_COMPOSE) {
    // - iop is a composition

    extern bool gPagePartitioning, gPagePartitioning1, gPagePartitioningMetis;

    if (gPagePartitioning || gPagePartitioning1 || gPagePartitioningMetis) {
      // - emit Synplify project files after page partitioning
      // - top level composition (*iop) calls page compositional ops,
      //     so we emit Synplify files for (*iop) AND each called operator
      // - we do NOT emit Synplify files for the top composition here
      emitSynplifyCompose((OperatorCompose*)iop);
      list<Operator*> calledops = findCalledOps((OperatorCompose*)iop);
      Operator *calledop;
      forall (calledop, calledops) {
	assert(calledop->getOpKind()==OP_COMPOSE);
	emitSynplifyComposeAndCalled((OperatorCompose*)calledop);
      }
    }
    else {
      // - emit Synplify project files without page partitioning
      // - top level composition (*op) calls behavioral ops,
      //     so we treat it as a single page
      emitSynplifyComposeAndCalled((OperatorCompose*)iop);
    }
  }

  else {
    // - iop is single behavioral operator
    // - emit Synplify project files for just this behavioral op
    emitSynplify((OperatorBehavioral*)iop);
  }
}
