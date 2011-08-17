/////////////////////////////////////////////////////////////////////////////
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
// SCORE TDF compiler:  compiler entry point (main)
// $Revision: 1.154 $
//
//////////////////////////////////////////////////////////////////////////////


//#include <string.h>
#include <LEDA/core/string.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/types.h>
#include <unistd.h>
#include "suite.h"
#include "operator.h"
#include "stmt.h"
#include "expr.h"
#include "misc.h"
#include "file.h"
#include "instance.h"
#include "feedback.h"
#include "feedback_fsm.h"
#include "version.h"
#include "ccrename.h"
#include "ccheader.h"
#include "ccbody.h"
#include "ccinstance.h"
#include "rateInstance.h"
#include "ribody.h"
#include "gc.h"
#include "canonical.h"
#include "ir_graph.h"
#include "ir_misc.h"
#include "clusterstates.h"
#include "dismantle.h"
#include "pagenetlist.h"
#include "pagenetlistdot.h"
#include "streamdepth.h"
#include "everilog.h"
#include "synplify.h"
#include "dummypages.h"
#include "pipeext.h"
#include "blockdfg.h"


char	*gProgName   = NULL;	// argv[0]
bool	 gNoWarnings = false;	// true for "-q" quiet option (used by warn())

// - EC: should move these elsewhere, maybe to clusterstates.cc
bool	 gPageTiming            = false; // true for "-pt"  page timing
bool	 gPageTiming1           = false; // true for "-pt1" page timing (debug)
bool	 gPagePartitioning      = false; // true for "-pp"  page partitioning
bool	 gPagePartitioningMetis = false; // true for "-ppmetis"
bool	 gPagePartitioning1     = false; // true for "-pp1" one op per page
int	 gPageAreaMin           = 256;	 // "-A" min page area for "-pp"
int	 gPageAreaMax           = 512;	 // "-A" max page area for "-pp"
int      gCycleTime             = 1;	 // "-T" cycle time    for "-pt"

// - EC: should move these elsewhere, maybe to streamdepth.cc
// bool  gEmitStreamDepth	= false; // true for "-esd" stream depth analss
int	 gStreamDepthMax	= 2;     // "-SD" max stream depth for "-esd"

// - EC: should move these elsewhere, maybe to synplify.cc
bool     gSynplify              = false; // true for "-synplify"

enum Target { TARGET_TDF,
	      TARGET_CC,
	      TARGET_CUDA,
		TARGET_GAPPA,
	      TARGET_MICROBLAZE,
	      TARGET_AUTOESL,
	      TARGET_VERILOG,
	      TARGET_NEWREP,
	    //TARGET_CLUSTERS,
	      TARGET_STREAMDEPTH,
	      TARGET_RI,
	      TARGET_DOT,		// added by Nachiket on 1/10/2010
	      TARGET_DFG,		// added by Nachiket on 9/10/2009
	      TARGET_DFGCC,		// added by Nachiket on 11/29/2009
	    };

char *feedback_ext[2]={"fuser","fauto"};
int num_feedback_ext=2;


void usage ()
{
  // - print usage message and exit

  cerr
    << "Usage:   " << gProgName << " [options] <inputFile> ...\n"
    << "\n"
    << "Options: -b           : parser tracing\n"
    << "         -q           : quiet (supress warnings)\n"
    << "         -v           : print version information and exit\n"
    << "         -E           : preprocess only (emit to standard output)\n"
    << "         -D...        : define macro for C-preprocessor\n"
    << "         -I<dir>      : specify #include directory for C-preprocessor\n"
    << "         -etdf        : emit TDF code (default)\n"
    << "         -ecc         : emit behavioral C++ code\n"
    << "         -egappa      : emit input language for gappa (precision analysis)\n"
    << "         -edot        : emit dataflow graph for Graphviz visualization\n"
    << "         -edfg        : emit dataflow graph for Nachiket's SPICE backend\n"
    << "         -edfgcc      : emit C++ version of the dataflow graph for verifying Nachiket's SPICE backend\n"
    << "         -embz        : emit C code for the Microblaze\n"
    << "         -ecuda       : emit CUDA code for NVIDIA GPU\n"
    << "         -eautoesl    : emit C code for AutoESL/Xilinx\n"
    << "         -everilog    : emit Verilog\n"
    << "         -eIR         : emit page synthesis info (implies -xc -mt)\n"
				  // -mt avoids a core-dump, to be investigated
    << "         -evcg        : emit .vcg files for DFGs, SFGs (requires -eIR)\n"
    << "         -eRI         : emit rate information\n"
    << "         -epn         : emit netlist of pages (adds to other -eXXX)\n"
    << "         -esd         : emit stream depth analysis    (implies -xa)\n"
    << "         -synplify    : create Synplify project files (implies -everilog)\n"
    << "         -syntech <t> : technology <t>=tech-part-pkg-speed for -synplify\n"
    << "         -pp          : page partitioning             (implies -xa -eIR)\n"
    << "         -ppmetis     : page partitioning using metis (implies -pp)\n"
    << "         -pp1         : page partitioning with 1 op per page\n"
    << "         -pt          : page timing                   (implies -xc)\n"
    << "         -pt1         : page timing with unit delays  (implies -xc)\n"
    << "         -qim  <m>    : use Verilog module          <m> for page input  queues\n"
    << "         -qid  <d>    : use queue depth             <d> for page input  queues\n"
    << "         -qili <l>    : use logic  input pipe depth <l> for page input  queues\n"
    << "         -qilo <l>    : use logic output pipe depth <l> for page input  queues\n"
    << "         -qiw  <w>    : use wire         pipe depth <w> for page input  queues\n"
    << "         -qom  <m>    : use Verilog module          <m> for page output queues\n"
    << "         -qod  <d>    : use queue depth             <d> for page output queues\n"
    << "         -qoli <l>    : use logic  input pipe depth <l> for page output queues\n"
    << "         -qolo <l>    : use logic output pipe depth <l> for page output queues\n"
    << "         -qow  <w>    : use wire         pipe depth <w> for page output queues\n"
    << "         -qlm  <m>    : use Verilog module          <m> for page local  queues\n"
    << "         -qld  <d>    : use queue depth             <d> for page local  queues\n"
    << "         -qlli <l>    : use logic  input pipe depth <l> for page local  queues\n"
    << "         -qllo <l>    : use logic output pipe depth <l> for page local  queues\n"
    << "         -qlw  <w>    : use wire         pipe depth <w> for page local  queues\n"
    << "         -as          : add code to profile FSMs (requires -ecc)\n"
    << "         -adpr        : add code to debug proc_run\n"
    << "         -adps        : add code to debug pagestep\n"
    << "         -ads         : add code to debug state firing\n"
    << "         -dt          : debug: show time at timestamps\n"
    << "         -dm          : debug: show memory at timestamps\n"
    << "         -dshared     : debug: detect shared AST nodes at timestamps\n"
    << "         -mt          : memory mgmt: keep types\n"
    << "         -mgc         : memory mgmt: enable garbage collection\n"
    << "         -xa          : dismantle reg/stream access (implies -xc)\n"
    << "         -xc          : enable canonical-form transformations\n"
    << "         -T <t>       : set cycle time (4-LUT evaluations) for -pt\n"
    << "         -A <l> <h>   : set page area bounds (4-LUTs)      for -pp\n"
    << "         -mix <c>     : set state clustering mix coef c=[0(DF)..1(SF)]\n"
    << "         -SD  <d>     : set maximum static stream depth for -esd\n"
    << "\n";
  exit(-1);
}


void printVersion ()
{
  // - print version number and exit

  /*
  char *gRcsRev  = "$Revision: 1.154 $";
  char *gRcsDate = "$Date: 2005/09/02 07:19:54 $";
  string rev     = (string(gRcsRev)+" "+string(gRcsDate)).replace_all("$","");
  cout << gProgName << "  " << rev << '\n';
  */
  cout << gProgName << " version " << TDFC_VERSION << '\n';
  exit(0);
}


void loadFiles (list<char*> fileNames, bool preprocessOnly,
		list<char*> preprocessorFlags, int debug)
{
  // - read TDF source files ... emit verified AST
  //     (preprocess, parse, link, type-check)
  // - preprocessor writes to temporary file in env $TMP or /tmp

  const char *f;
  forall (f,fileNames)
  {
    const char *tmpDir = getenv("TMP");
    tmpDir = tmpDir ? tmpDir : "/tmp";
    char hostname[40];
    gethostname(hostname,40);
    pid_t pid = getpid();
    string tmpFileName = string("%s/tdfc-%s-%d",tmpDir,hostname,pid);
    warn(string("preprocess ")+f+"...");
    preprocessFile(f,tmpFileName,&preprocessorFlags);	  // - preprocess
#ifdef __CYGWIN32__
    // yes, this is kinda kludgy.
    //  if there is an option we can give to the processor
    //  to name the output, we might want to try that instead
    // Problem here is that cygwin gcc defaults outputs to add .exe to make
    //  windows happy.
    tmpFileName=tmpFileName+string(".exe");
#endif
    if (preprocessOnly)					  // - emit preprocessd
    {
      FILE *fp = fopen((const char*)tmpFileName, "r");
      copyFile(fp,stdout);
      fclose(fp);
    }
    else
    {
      parseFile(tmpFileName,debug);		// - parse (create AST)
    }
    unlink(tmpFileName);			// - delete tmp file
  }
  if (!preprocessOnly)
  {
    timestamp(string("begin linking"));
    gSuite->link();				// - link AST
    timestamp(string("begin typechecking"));
    gSuite->typeCheck();			// - type-check AST
    timestamp(string("end typecheck"));
    if (gDeleteType)
      rmType(gSuite);
    timestamp(string("after rmType"));

  }
}


bool find_behav_ops_map (Tree *t, void *aux)
{
  // - find called behavioral ops, append to list<OperatorBehavioral*>* aux
  // - used to recover pages from an "instance" top-level compositional op
  // - assumes that operators have been exlined + flattened

  list<OperatorBehavioral*> *ops = (list<OperatorBehavioral*>*)aux;
  Operator *op;
  switch (t->getKind())
  {
    case TREE_OPERATOR:
    case TREE_STMT:
      return true;
      break;

    case TREE_EXPR:
      if (   ((Expr*)t)->getExprKind()==EXPR_CALL
	  && (op=((ExprCall*)t)->getOp())->getOpKind()==OP_BEHAVIORAL) {
	ops->append((OperatorBehavioral*)op);
	return false;
      }
      else
	return true;
      break;

    default:
      return false;
      break;
  }
}


set<string> *instances(Operator *op, Target targ,
			int debug_page_step=0)
{

  set<string> *res=new set<string>();

  int args=count_args(op->getArgs());
  unsigned long param_locs=parameter_locs(op->getArgs());

  list <FeedbackRecord *>* ilist=new list<FeedbackRecord *>();

  // Added by Nachiket on 11/4/2009 to get rid of the bs tdfc behavior
  // There is no need to insist on having fuser.. only an idiot will require this.
  int* init_params=new int[args];
  for(int init_args=0;init_args<args;init_args++) {
  	init_params[init_args]=0;
  }
  FeedbackRecord *init_record = new FeedbackRecord(args, param_locs, init_params);
  ilist->push(init_record);

  // End of Nachiket's additions..


  int spos=0;
  // search through path
  string colon=string(":");
  while (spos<gFeedbackPath.length())
    {
      string pdir;
      int cpos=gFeedbackPath.pos(colon,spos);
      if (cpos<0)
	{
	  pdir=gFeedbackPath(spos,gFeedbackPath.length());
	  spos=gFeedbackPath.length();
	}
      else
	{
	  pdir=gFeedbackPath(spos,(cpos-1));
	  spos=cpos+1;
	}
      
      string dir;
      if (pdir[pdir.length()-1]=='/')
	dir=pdir;
      else
	dir=string("%s/",pdir);

      // for all files with user/auto extension
      char **tmp_ext=feedback_ext;
      for (int i=0;i<num_feedback_ext;i++)
	{
	  char *ext=*tmp_ext;
	  char *fname=(char *)malloc(sizeof(char)*(dir.length()+
						   (op->getName()).length()+
						   strlen(ext)+2));

	  // BLAH can't use LEDA strings in sprintf or string("...",...)
	  char *tstr=fname;
	  strcpy(fname,dir);
	  tstr+=dir.length();
	  strcpy(tstr,op->getName());
	  tstr+=(op->getName()).length();
	  *tstr='.';
	  tstr++;
	  strcpy(tstr,ext);
	  tstr+=strlen(ext);
	  *tstr=(char)NULL;

	  FILE *tf=fopen(fname,"r");
	  //warn("Reading file" + *fname);
	  if (tf!=(FILE *)NULL)
	    {
	      //   lock and read all records
	      //   - flock is BSD, not in Solaris:
	      //     flock(fileno(tf),LOCK_EX); // blocking
	      lockfile(fileno(tf));	// blocking
	      read_feedback(tf,fname,
			    ilist,args,param_locs,op->getName());
	      //   maybe uniq and rewrite file ?
	      //   unlock file 
	      //   - flock is BSD, not in Solaris:
	      //     flock(fileno(tf),LOCK_UN); 
	      unlockfile(fileno(tf));
	      fclose(tf);
	      // do stuff
	    }
	  else
	    {
	      // pass
	    }
	  tmp_ext++;
	}
    }



  // TODO: hoist this above header/body generation
  //       going to want flatten before all kinds of code gen
  Operator *fop=flatten(op);

  if (gEnableGC)
  {
    TreeGC::addRoot(fop);
  }

  list<FeedbackRecord *> *olist=canonicalize_feedback_record_list(ilist);
  //   for each uniq
  FeedbackRecord *rec;
  forall(rec,*olist)
    {
      // TODO: check if instance already exists and is newer than 
      //         the tdf source before actually building
      // DEBUG
      warn("Generating instance " + op->getName() + rec->toString());
      Operator *iop=generate_instance(fop,rec);
      list<OperatorBehavioral*> instance_list;
      iop->map(find_behav_ops_map,(TreeMap)NULL,&instance_list);

      if (gPagePartitioning) {
	CreateNewRep(iop,&instance_list);
	BindTimeArea();
	iop = clusterStates_instance((OperatorCompose*)iop,&instance_list,
				     gPageAreaMin,gPageAreaMax);
	  // - requires compositional iop at top
	instance_list.clear();
	iop->map(find_behav_ops_map,(TreeMap)NULL,&instance_list);
      }

      if (gPagePartitioning1) {
	iop = dummyPages_instance((OperatorCompose*)iop);
	instance_list.clear();
	iop->map(find_behav_ops_map,(TreeMap)NULL,&instance_list);
      }

      if (gPageTiming || gPageTiming1) {
	iop = timing_instance(iop,&instance_list,gCycleTime);
	instance_list.clear();
	iop->map(find_behav_ops_map,(TreeMap)NULL,&instance_list);
      }

      // - Instances now complete and transformed;  type-check one last time
      iop->typeCheck();
      OperatorBehavioral *inst_op;
      forall (inst_op, instance_list)
	inst_op->typeCheck();

      // if (gEmitStreamDepth)
      //   streamDepth_instance(iop,&instance_list);

      if (gEmitPageNetlist) {
        // - requires compositional iop at top
	page_netlist_instance((OperatorCompose*)iop,&instance_list);
	page_netlist_dot_instance((OperatorCompose*)iop,&instance_list);
      }

      switch (targ)
      {
	case TARGET_TDF:		
	  cout << "\n// operator instance [compositional] " << iop->getName() 
	       << rec->toString()
	       << "\n\n";
	  cout << iop->toString();
	  Operator* inst_op;
	  forall(inst_op,instance_list) {
	    cout << "\n// operator instance [page] " << inst_op->getName()
	         << ((inst_op->getOpKind()==OP_BEHAVIORAL &&
		      isTrivialPipeline((OperatorBehavioral*)inst_op)) ?
					     " (trivial pipeline)" : "")
	         << ", part of " << iop->getName() 
	         << rec->toString()
		 << "\n\n";
	    cout << inst_op->toString();
	  }
	  break;
	case TARGET_DFG:
	  if(iop->getOpKind()==OP_BEHAVIORAL) {
	    cout << ((OperatorBehavioral*)iop)->toDFGString();
	  }
	  break;
	case TARGET_DFGCC:
		if(iop->getOpKind()==OP_BEHAVIORAL) {
			((OperatorBehavioral*)iop)->buildDataflowGraph();
		} else {
			cout << "Not behavioral??" << endl; exit(1);
		}
		break;
	case TARGET_CC:		
	  res->insert(ccinstance(iop,op->getName(),rec,debug_page_step));
	  break;
	case TARGET_CUDA:		
	  res->insert(ccinstance(iop,op->getName(),rec,debug_page_step));
	  break;
	case TARGET_GAPPA:		
	  res->insert(ccinstance(iop,op->getName(),rec,debug_page_step));
	  break;
	case TARGET_MICROBLAZE:		
	  res->insert(ccinstance(iop,op->getName(),rec,debug_page_step));
	  break;
	case TARGET_AUTOESL:		
	  res->insert(ccinstance(iop,op->getName(),rec,debug_page_step));
	  break;
	case TARGET_VERILOG:
	  tdfToVerilog_instance(iop,&instance_list);
	  if (gSynplify)
	    emitSynplify_instance(iop,&instance_list);
	  break;
        case TARGET_NEWREP:
	  if (!gPagePartitioning) {		// - do not do it twice
	    CreateNewRep(iop,&instance_list);
	    BindTimeArea();
	  }
	  break;
        case TARGET_STREAMDEPTH:
	  streamDepth_instance(iop,&instance_list);
	  break;
	case TARGET_RI:
	  // put in constructs to rate files here
	  rateInstance(iop);
	  break;
      }

      if (gEnableGC)
	TreeGC::gc();	// gc's iop
    }

  if (gEnableGC)
  {
    timestamp(string("done emitting ")+fop->getName());
    TreeGC::removeRoot(fop);
    TreeGC::gc();
  }
  
  return(res);
}


void emitTDF ()
{
  // - emit TDF code for all operators  (-etdf option)

  Operator *op;
  forall(op,*gSuite->getOperators())
    cout << "// operator " << op->getName() << '\n';
  cout << '\n' << gSuite->toString();
  // not print instances
  forall(op,*gSuite->getOperators())
    instances(op,TARGET_TDF,0);
}


void emitDFG ()
{
  // - emit DFG code for all operators  (-edfg option)

  Operator *op;

  int leaf_operators=0;
  forall(op,*gSuite->getOperators()) {
	if(op->getOpKind()==OP_BEHAVIORAL) {
		leaf_operators++;
	}
  }
  cout << "TotalOperators " << leaf_operators << endl;

  forall(op,*gSuite->getOperators()) {
    if(op->getOpKind()==OP_BEHAVIORAL) {
       cout << ((OperatorBehavioral*)op)->toDFGString();
    }
    // avoiding call to instances due to inexplicable crash on 64-bit machines
    //instances(op,TARGET_DFG,0);
  }
}

// Added on 1/10/2010 to support visualization of SCORE graphs
void emitDOT ()
{
  // - emit Graphviz DOT code for all operators  (-edot option)

  Operator *op;

  cout << "digraph G {" << endl;
  cout << endl;



  forall(op,*gSuite->getOperators()) {
    if(op->getOpKind()==OP_COMPOSE) {
//  	Operator *fop=flatten(op);
       cout << ((Operator*)op)->toDOTString("\t");
       cout << endl;
    }
  }

  cout << "}" << endl;
}

void emitDFGCC ()
{
	// - emit DFG and the associated C++ code (-edfgcc option)

	// --------------------------------------------------
	// Added 2/1/2010 for renaming all variables
	Operator *op;
	forall(op,*(gSuite->getOperators()))
		ccrename(op);
	// --------------------------------------------------

	forall(op,*gSuite->getOperators()) {
		//instances(op, TARGET_DFGCC,0); // CAUTION: DON'T NEED TO STORE DFGs INSIDE INSTANCE! KEEP IN ORIGINAL..
		if(op->getOpKind()==OP_BEHAVIORAL) {
			((OperatorBehavioral*)op)->buildDataflowGraph();
		} else {
			cout << "Non-behavioral operator!" << endl;
		}
		ccheader(op);
		ccdfgbody(op,0);
	}

}

void emitCC (int dpr, int dps)
{
  // - emit C++ code for all operators  (-ecc option)
  
  Operator *op;
  forall(op,*(gSuite->getOperators()))
    ccrename(op);
  // all operators must be renamed before the processing in the
  // following loop
  forall(op,*(gSuite->getOperators()))
  {
    timestamp(string("begin processing ")+op->getName());
    // TODO: eventually move flatten here
    ccheader(op); 
    ccbody(op,dpr); // NACHIKET
    /*timestamp(string("begin instances for ")+op->getName());
    set<string>* instance_names=instances(op,TARGET_CC,dps); 
    
    string fname=op->getName() + string(".instances");
    ofstream *fout=new ofstream(fname);
    string str;
    forall(str,*instance_names)
      {
	*fout << " " << str;
      }
    *fout << endl;
    fout->close();
    */
    
    //cout << endl;
  }
}

void emitCUDA ()
{
  // - emit C code for all operators  (-embz option)
  
  Operator *op;
  forall(op,*(gSuite->getOperators()))
    ccrename(op);
  // all operators must be renamed before the processing in the
  // following loop
  forall(op,*(gSuite->getOperators()))
  {
    timestamp(string("begin processing ")+op->getName());
    // TODO: eventually move flatten here
    cccudaheader(op); 
    cccudabody(op);
    cccudawrapper(op); 
    cout << endl;
  }
}

void emitGAPPA ()
{
  // - emit gappa code for all operators  (-egappa option)
  
  Operator *op;
	
//    forall(op,*(gSuite->getOperators()))
//    ccrename(op);
   // I am not sure I need to rename the operators
  // all operators must be renamed before the processing in the
  // following loop
  forall(op,*(gSuite->getOperators()))
  {  
	  if (op->getOpKind()==OP_BEHAVIORAL)
	  {
		  OperatorBehavioral *bop=(OperatorBehavioral *)op;
		  bop->buildDataflowGraph();
	  }
    timestamp(string("begin processing ")+op->getName());
    // TODO: eventually move flatten here
    if (ccCheckRanges(op))
//		cout << "Operator : \n" <<op->toString() << endl;
		ccgappabody(op); // Helene
    cout << endl;
  }
}

void emitMicroblazeC ()
{
  // - emit C code for all operators  (-embz option)
  
  Operator *op;
  forall(op,*(gSuite->getOperators()))
    ccrename(op);
  // all operators must be renamed before the processing in the
  // following loop
  forall(op,*(gSuite->getOperators()))
  {
    timestamp(string("begin processing ")+op->getName());
    // TODO: eventually move flatten here
    ccmicroblazeheader(op); 
    ccmicroblazebody(op); 
    cout << endl;
  }
}

void emitAutoESLC ()
{
  // - emit C code for all operators  (-eautoesl option)
  
  Operator *op;
  forall(op,*(gSuite->getOperators()))
    ccrename(op);
  // all operators must be renamed before the processing in the
  // following loop
  forall(op,*(gSuite->getOperators()))
  {
    timestamp(string("begin processing ")+op->getName());
    // TODO: eventually move flatten here
    bool exp = false; 
    bool log = false;
    ccautoeslbody(op , &exp, &log);
    ccautoeslheader(op, exp, log); 
    ccautoeslwrapper(op); 
    ccautoesltcl(op); 
    ccautoeslmake(op);
    cout << endl;
  }
}

void emitVerilog ()
{
  // - emit Verilog code for all operators  (-everilog option)

  // fatal(-1,"Verilog not supported");

  Operator *op;
  forall(op,*gSuite->getOperators()) {
    cout << "Processing op" << op->getName() << endl;
    instances(op,TARGET_VERILOG,0);
  }
}


void emitNewRep ()
{
  // - emit page synthesis info for all operators  (-eIR option)

  Operator *op;
  forall(op,*(gSuite->getOperators()))
  {
    timestamp(string("begin instances + newrep/synthesis for ")+op->getName());
    instances(op,TARGET_NEWREP,0);
  }
}


/*
void emitClusters (int page_area_min, int page_area_max)
{
  // - emit state clusters for all operators  (-ecl option)

  Operator *op;
  forall(op,*(gSuite->getOperators()))
  {
    timestamp(string("begin instances + clustering for ")+op->getName());
    instances(op,TARGET_CLUSTERS,0,page_area_min,page_area_max);
  }
  printStateClusterStats();
}
*/


void emitStreamDepth ()
{
  // - emit stream depth analysis for all streams (-esd option)

  Operator *op;
  forall(op,*(gSuite->getOperators()))
  {
    timestamp(string("begin instances + stream depth for ")+op->getName());
    instances(op,TARGET_STREAMDEPTH,0);
  }
}


void emitRI ()
{
  // - emit Rate Identification Table for all operators (-eRI option)

  Operator *op;
  forall(op,*(gSuite->getOperators()))
  {
    timestamp(string("begin processing ")+op->getName());
    ribody(op);
    timestamp(string("begin instances for ")+op->getName());
    instances(op,TARGET_RI,0); 
    cout << endl;
  }
}


int main(int argc, char *argv[])
{
  // - read cmd-line args and parse each file
  // - sets gProgName to executable's name

  // - initialize:

  tdf_init();
  gProgName=argv[0];
  initFeedbackPath();

  // - parse command line:

  Target	optionTarget		= TARGET_TDF;
  int		optionParserDebug	= 0;
  int           optionDebugProcRun      = 0;
  int           optionDebugPageStep     = 0;
  int           optionDebugStateFiring  = 0;
  bool		optionPreprocessOnly	= false;
  list<char*>	optionPreprocessorFlags;
  list<char*>	optionSrcFiles;
  bool		optionCanonical		= false;
  bool		optionDismantleAccess	= false;

  for (int arg=1; arg<argc; arg++)
  {
    if (strcmp(argv[arg],"-b")==0)		// -b : parser debugging
      optionParserDebug=1;
    else if (strcmp(argv[arg],"-q")==0)		// -q : quiet (no warnings)
      gNoWarnings=true;
    else if (strcmp(argv[arg],"-v")==0)		// -v : print versions info
      printVersion();
    else if (strcmp(argv[arg],"-E")==0)		// -E : stop after preprocessng
      optionPreprocessOnly=true;
    else if (strncmp(argv[arg],"-D",2)==0)	// -D... : cpp macro
      optionPreprocessorFlags.append(argv[arg]);
    else if (strncmp(argv[arg],"-I",2)==0)	// -I... : cpp directory
      optionPreprocessorFlags.append(argv[arg]);      
    else if (strcmp(argv[arg],"-etdf")==0)	// -etdf     : emit TDF
      optionTarget = TARGET_TDF;
    else if (strcmp(argv[arg],"-edfg")==0)	// -edfg     : emit DFG
      optionTarget = TARGET_DFG;
    else if (strcmp(argv[arg],"-edot")==0)	// -edot     : emit Graphviz DOT
      optionTarget = TARGET_DOT;
    else if (strcmp(argv[arg],"-edfgcc")==0)	// -edfg : emit DFG and C++ code wrappers for verification
      optionTarget = TARGET_DFGCC;
    else if (strcmp(argv[arg],"-ecc")==0)	// -ecc      : emit C++
      optionTarget = TARGET_CC;
    else if (strcmp(argv[arg],"-ecuda")==0)	// -ecuda      : emit CUDA
      optionTarget = TARGET_CUDA;
    else if (strcmp(argv[arg],"-egappa")==0)// -egappa    : emit gappa
      optionTarget = TARGET_GAPPA;
    else if (strcmp(argv[arg],"-embz")==0)	// -embz      : emit C for Microblaze
      optionTarget = TARGET_MICROBLAZE;
    else if (strcmp(argv[arg],"-eautoesl")==0)	// -eautoesl   : emit AutoESL C
      optionTarget = TARGET_AUTOESL;
    else if (strcmp(argv[arg],"-everilog")==0)	// -everilog : emit Verilog
      optionTarget = TARGET_VERILOG;
    else if (strcmp(argv[arg],"-eIR")==0) {	// -eIR      : IntermRep/synth
      optionTarget = TARGET_NEWREP;
      optionCanonical = true;	// implies -xc
      gDeleteType = 0;		// implies -mt
    }
    else if (strcmp(argv[arg],"-evcg")==0)	// -evcg     : emit .vcg 4 -eIR
      gEmitVCG = 1;
    else if (strcmp(argv[arg],"-eRI")==0)	// -eRI      : emit rate info
      optionTarget = TARGET_RI;
    else if (strcmp(argv[arg],"-epn")==0)	// -epn      : emit page netlst
      gEmitPageNetlist = true;
    else if (strcmp(argv[arg],"-esd")==0) {	// -esd      : emit strm depth
      optionTarget = TARGET_STREAMDEPTH;
      // gEmitStreamDepth = true;
      optionDismantleAccess = true; // imp -xa
      optionCanonical = true;	// implies -xc
    }
    else if (strcmp(argv[arg],"-synplify")==0){	// -synplify : Synplify project
      gSynplify = true;
      optionTarget = TARGET_VERILOG;
			// - implies -everilog
    }
    else if (strcmp(argv[arg],"-syntech")==0
			&& argc>=arg+1+1) {	// -syntech : tech-part-pkg-spd
      string synplifyTechStr = argv[++arg];
      if (!parseSynplifyTechStr(synplifyTechStr)) {
	cerr << "Cannot understand -syntech " << synplifyTechStr << "\n";
	usage();
      }
    }
    else if (strcmp(argv[arg],"-pp")==0) {	// -pp       : page partition
      gPagePartitioning = true;
      optionDismantleAccess = true; // imp -xa
      optionCanonical = true;	// implies -xc
      gDeleteType = 0;		// implies -mt
    }
    else if (strcmp(argv[arg],"-ppmetis")==0) {	// -ppmetis  : emit metis files
      gPagePartitioningMetis = true;
      gPagePartitioning = true; // implies -pp
      optionDismantleAccess = true; // imp -xa
      optionCanonical = true;	// implies -xc
      gDeleteType = 0;		// implies -mt
    }
    else if (strcmp(argv[arg],"-pp1")==0) {	// -pp       : 1 op per page
      gPagePartitioning1 = true;
    }
    else if (strcmp(argv[arg],"-pt")==0) {	// -pt       : page timing
      gPageTiming = true;
      optionCanonical = true;	// implies -xc
    }
    else if (strcmp(argv[arg],"-pt1")==0) {	// -pt1      : page timing unit
      gPageTiming1 = true;
      optionCanonical = true;	// implies -xc
    }
    else if (strcmp(argv[arg],"-qim")==0
			&& argc>=arg+1+1) {	// -qim <m> : input q module
      gPageInputQueueModule = argv[++arg];
    }
    else if (strcmp(argv[arg],"-qid")==0
			&& argc>=arg+1+1) {	// -qid <d> : input q depth
      gPageInputQueueDepth = atoi(argv[++arg]);
      if (gPageInputQueueDepth<=0)
	usage();
    }
    else if (strcmp(argv[arg],"-qili")==0
			&& argc>=arg+1+1) {	// -qili <l> : input q logic in depth
      gPageInputQueueLogicInDepth = atoi(argv[++arg]);
      if (gPageInputQueueLogicInDepth<=-1)
	usage();
    }
    else if (strcmp(argv[arg],"-qilo")==0
			&& argc>=arg+1+1) {	// -qilo <l> : input q logic out depth
      gPageInputQueueLogicOutDepth = atoi(argv[++arg]);
      if (gPageInputQueueLogicOutDepth<=-1)
	usage();
    }
    else if (strcmp(argv[arg],"-qiw")==0
			&& argc>=arg+1+1) {	// -qiw <w> : input q wire depth
      gPageInputQueueWireDepth = atoi(argv[++arg]);
      if (gPageInputQueueWireDepth<=-1)
	usage();
    }
    else if (strcmp(argv[arg],"-qom")==0
			&& argc>=arg+1+1) {	// -qom <m> : output q module
      gPageOutputQueueModule = argv[++arg];
    }
    else if (strcmp(argv[arg],"-qod")==0
			&& argc>=arg+1+1) {	// -qod <d> : output q depth
      gPageOutputQueueDepth = atoi(argv[++arg]);
      if (gPageOutputQueueDepth<=0)
	usage();
    }
    else if (strcmp(argv[arg],"-qoli")==0
			&& argc>=arg+1+1) {	// -qoli <l> : output q logic in depth
      gPageOutputQueueLogicInDepth = atoi(argv[++arg]);
      if (gPageOutputQueueLogicInDepth<=-1)
	usage();
    }
    else if (strcmp(argv[arg],"-qolo")==0
			&& argc>=arg+1+1) {	// -qolo <l> : output q logic out depth
      gPageOutputQueueLogicOutDepth = atoi(argv[++arg]);
      if (gPageOutputQueueLogicOutDepth<=-1)
	usage();
    }
    else if (strcmp(argv[arg],"-qow")==0
			&& argc>=arg+1+1) {	// -qow <w> : output q wire depth
      gPageOutputQueueWireDepth = atoi(argv[++arg]);
      if (gPageOutputQueueWireDepth<=-1)
	usage();
    }
    else if (strcmp(argv[arg],"-qlm")==0
			&& argc>=arg+1+1) {	// -qlm <m> : local q module
      gPageLocalQueueModule = argv[++arg];
    }
    else if (strcmp(argv[arg],"-qld")==0
			&& argc>=arg+1+1) {	// -qld <d> : local q depth
      gPageLocalQueueDepth = atoi(argv[++arg]);
      if (gPageLocalQueueDepth<=0)
	usage();
    }
    else if (strcmp(argv[arg],"-qlli")==0
			&& argc>=arg+1+1) {	// -qlli <l> : local q logic in depth
      gPageLocalQueueLogicInDepth = atoi(argv[++arg]);
      if (gPageLocalQueueLogicInDepth<=-1)
	usage();
    }
    else if (strcmp(argv[arg],"-qllo")==0
			&& argc>=arg+1+1) {	// -qllo <l> : local q logic out depth
      gPageLocalQueueLogicOutDepth = atoi(argv[++arg]);
      if (gPageLocalQueueLogicOutDepth<=-1)
	usage();
    }
    else if (strcmp(argv[arg],"-qlw")==0
			&& argc>=arg+1+1) {	// -qlw <w> : local q wire depth
      gPageLocalQueueWireDepth = atoi(argv[++arg]);
      if (gPageLocalQueueWireDepth<=-1)
	usage();
    }
    else if (strcmp(argv[arg],"-as")==0)	// -as      : profile states
      gProfileStates = 1;
    else if (strcmp(argv[arg],"-adpr")==0)	// -adpr    : debug proc run
      optionDebugProcRun = 1;
    else if (strcmp(argv[arg],"-adps")==0)	// -adps    : debug page step
      optionDebugPageStep = 1;
    else if (strcmp(argv[arg],"-ads")==0)	// -ads     : debug state firng
      optionDebugStateFiring = 1;
    else if (strcmp(argv[arg],"-dt")==0)	// -dt      : print time
      gPrintTime = 1;
    else if (strcmp(argv[arg],"-dm")==0)	// -dm      : print memory
      gPrintMemory = 1;
    else if (strcmp(argv[arg],"-dshared")==0)	// -dshared : find shared nodes
      gDebugShared = 1;
    else if (strcmp(argv[arg],"-mgc")==0)	// -mgc     : enable GC
      gEnableGC = 1;
    else if (strcmp(argv[arg],"-mt")==0)	// -mt      : disable type rm
      gDeleteType = 0;
    else if (strcmp(argv[arg],"-xa")==0) {	// -xa      : dsmntl reg access
      optionDismantleAccess = true;
      optionCanonical = true;	// implies -xc
    }
    else if (strcmp(argv[arg],"-xc")==0)	// -xc      : enable canonical
      optionCanonical = true;
    else if (strcmp(argv[arg],"-T")==0
			&& argc>=arg+1+1) {	// -T <t>     : cycle time
      gCycleTime = atoi(argv[++arg]);
      if (gCycleTime<=0)
	usage();
    }
    else if (strcmp(argv[arg],"-A")==0
			&& argc>=arg+1+2) {	// -A <l> <h> : page area bound
      gPageAreaMin = atoi(argv[++arg]);
      gPageAreaMax = atoi(argv[++arg]);
      if (gPageAreaMin<=0 || gPageAreaMin<=0)
	usage();
    }
    else if (strcmp(argv[arg],"-mix")==0	// -mix <c> : DF/SF weight mix
			&& argc>=arg+1+1) {
      gDFSFmix = atof(argv[++arg]);
      if (gDFSFmix<0 || gDFSFmix>1)
	usage();
    }
    else if (strcmp(argv[arg],"-SD")==0		// -SD <d>  : max stream depth
			&& argc>=arg+1+1) {
      gStreamDepthMax = atoi(argv[++arg]);
      if (gStreamDepthMax<0)
	usage();
    }
    else if (argv[arg][0]=='-')			// -...  : unrecognised option
      usage();
    else					// file name to parse
      optionSrcFiles.append(argv[arg]);
  }
  if (optionSrcFiles.empty())
    usage();

  // - pre-process + parse + verify source files:  (build AST in gSuite)

  timestamp(string("begin load files"));

  loadFiles(optionSrcFiles,
	    optionPreprocessOnly, optionPreprocessorFlags, optionParserDebug);

  if (!optionPreprocessOnly)
  {
    // - Canonical form xforms (exline, etc.)
    if (optionCanonical)
    {
      warn(string("begin canonical-form xforms"));
      canonicalForm((Tree**)&gSuite);
    }

    // - Dismantle reg/stream access to be in own state (for state clustering)
    if (optionDismantleAccess)
    {
      timestamp(string("-- before dismantleRegisterAccess()"));
      dismantleRegStreamAccess(gSuite);
    }

    // - Infer fanout and expand calls to copy() fanout operator
    timestamp(string("begin infering fanout and expanding copy()"));
    doCopyOps((Tree**)&gSuite);

    // - Add printf() calls to debug state firing
    if (optionDebugStateFiring) {
      timestamp(string("begin adding debug info for state firing"));
      debugStateFiring(gSuite);
    }

    // - emit target language:
    timestamp(string("begin emission"));
    switch (optionTarget)
    {
      case TARGET_TDF:		emitTDF();			  break;
      case TARGET_DOT:		emitDOT();			  break;
      case TARGET_DFG:		emitDFG();			  break;
      case TARGET_DFGCC:	emitDFGCC();		  break;
      case TARGET_CC:		emitCC(optionDebugProcRun,
							optionDebugPageStep); break;
      case TARGET_CUDA:		emitCUDA();			  break;
      case TARGET_GAPPA :	emitGAPPA();			  break;
      case TARGET_MICROBLAZE:	emitMicroblazeC();		  break;
      case TARGET_AUTOESL:	emitAutoESLC();		  	break;
      case TARGET_VERILOG:	emitVerilog();			  break;
      case TARGET_NEWREP:	emitNewRep();			  break;
      case TARGET_RI:		emitRI();			  break;
//    case TARGET_CLUSTERS:	emitClusters(optionPageAreaMin,
//					     optionPageAreaMax);  break;
      case TARGET_STREAMDEPTH:	emitStreamDepth();		  break;
    }
  }

  if (gPagePartitioning)
    printStateClusterStats();

  // - cleanup:
  warn("Exiting...");

  tdf_exit();
  return(0);
}
