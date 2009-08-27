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
// SCORE TDF compiler:  Read/process FSM feedback files
// $Revision: 1.138 $
//
//////////////////////////////////////////////////////////////////////////////


#include "operator.h"
#include "state.h"
#include "misc.h"
#include "feedback_fsm.h"

// - for use with debugging main() at end
// #include "clusterstates.cc"
// #include "mincut.cc"

using leda::graph;
using leda::map;
using leda::dic_item;

////////////////////////////////////////////////////////////////
//  Debug printing

typedef GRAPH<string,string> sgraph;


/*
static
void fatal (int errcode, string msg)
{
  cerr << msg;
  exit(errcode);
}
*/


void fsm_feedback_error (const char* fname, int line, int pos)
{
  fatal(-1,
	string("Syntax error in FSM feedback file \"")+string(fname)+
	string("\" at line %d position %d",line,pos));
}


void fsm_feedback_error_nostate (const char* fname, int line, int pos,
				 string statename)
{
  fatal(-1,
	string("State \"") + statename +
	string("\" not found in FSM feedback file \"") +
	string(fname) + string("\" at line %d position %d",line,pos));
}


////////////////////////////////////////////////////////////////
//  Algorithms

edge get_edge (node n1, node n2)
{
  // - return the LEDA graph edge connecting nodes n1, n2

  graph* g=graph_of(n1);
  edge e;
  list<edge> g_out_edges_n1 = g->out_edges(n1);
  forall (e,g_out_edges_n1)
    if (g->target(e)==n2)
      return e;
  return NULL;
}


int checksum (string str)
{
  // - checksum a string by adding all char values

  int c=0;
  for (int i=0; i<str.length(); i++)
    c+=str[i];
  return c;
}


void read_fsm_feedback(FILE *fp, const char* fname,
		       // sfgraph *sfg					// UC
		       SFGraph sfg,					// C
		       OperatorBehavioral *op,
		       node_map<int> *state_freqs,
		       edge_map<int> *state_xfer_freqs)
{
  // - fill-in state_freqs + state_xfer_freqs for SFG
  // - debug: to ignore IR, comment-out C lines and uncomment UC lines
  //          (in this mode, sgraph* sfg is built according to
  //           connectivity of feedback file's state xfer freqs)

  // - map:  State* --> SFGraph node
  map<State*,node> stateNode(NULL);
  node v;								// C
  forall_nodes (v,*sfg)							// C
    stateNode[sfg->inf(v)->getState()] = v;				// C

  #define MAX_LINE_LEN 4096
  // HACK:  max line length 4096 chars (~50 lines)

  int lcnt=0;
  char line[MAX_LINE_LEN];
  while (fgets(line,MAX_LINE_LEN,fp))
  {
    // - read line of FB file
    // - state frequency:            "count(statename)=NNN"
    // - state xfer edge frequency:  "count(statename1,statename2)=NNN"
    lcnt++;
    int pos=0, lpos=0;
    string linestr = string(line).del("\n");
    if (linestr.length()==0)			// - skip empty lines
      continue;
    if ((lpos=linestr.pos("count("))!=0)	// - "count(" starts a line
      fsm_feedback_error(fname,lcnt,pos);
    pos=6;
    if ((lpos=linestr.pos(",",pos))>=0) {	// - comma:    state xfer freq
      string state1str=linestr(pos,lpos-1);
      // cerr << "// state xfer freq:  count(\"" << state1str << "\",";
      dic_item state1_it = op->getStates()->lookup(state1str);
      if (state1_it==NULL)
	fsm_feedback_error_nostate(fname,lcnt,pos,state1str);
      State *state1=op->getStates()->inf(state1_it);
      // State *state1=(State*)checksum(state1str);
      pos=lpos+1;
      if ((lpos=linestr.pos(")=",pos))<0)
	fsm_feedback_error(fname,lcnt,pos);
      string state2str=linestr(pos,lpos-1);
      // cerr << "\"" << state2str << "\")=";
      dic_item state2_it = op->getStates()->lookup(state2str);
      if (state2_it==NULL)
	fsm_feedback_error_nostate(fname,lcnt,pos,state2str);
      State *state2=op->getStates()->inf(state2_it);
      // State *state2=(State*)checksum(state2str);
      pos=lpos+2;
      string countstr=linestr(pos,linestr.length()-1);
      // cerr << "\"" << countstr << "\"";
      int freq=atoi(countstr);
      // cerr << " (" << freq << ")\n";
      warn(string("read_fsm_feedback: state xfer freq: count(\"") +
	   state1str + "\",\"" + state2str + "\")=\"" + countstr +
	   string("\" (%d)",freq));
      node state1node=stateNode[state1];
      node state2node=stateNode[state2];
      // - stateXnode may be NULL in IR, since IR removes empty states
      // if (state1node==NULL)						// UC
      //   state1node = stateNode[state1] = sfg->new_node(state1str);	// UC
      // if (state2node==NULL)						// UC
      //   state2node = stateNode[state2] = sfg->new_node(state2str);	// UC
      if (state1node && state2node) {
	edge e=get_edge(state1node,state2node);
	// if (e==NULL)							// UC
	//   e = sfg->new_edge(state1node, state2node,			// UC
	//		       state1str+"->"+state2str);		// UC
	// else								// UC
	//   cerr << "// WARNING, already found edge " <<(*sfg)[e]<<'\n'; // UC
	(*state_xfer_freqs)[e]=freq;
      }
    }
    else if ((lpos=linestr.pos(")=",pos))>=0) {	// - no comma: state freq
      string statestr=linestr(pos,lpos-1);
      // cerr << "// state freq:  count(\"" << statestr << "\")=";
      dic_item state_it = op->getStates()->lookup(statestr);
      if (state_it==NULL)
	fsm_feedback_error_nostate(fname,lcnt,pos,statestr);
      State *state=op->getStates()->inf(state_it);
      // State *state=(State*)checksum(statestr);
      pos=lpos+2;
      string countstr=linestr(pos,linestr.length()-1);
      // cerr << "\"" << countstr << "\"";
      int freq=atoi(countstr);
      // cerr << " (" << freq << ")\n";
      warn(string("read_fsm_feedback: state freq: count(\"") +
	   statestr + "\")=\"" + countstr + string("\" (%d)",freq));
      node statenode=stateNode[state];
      // - statenode may be NULL in IR, since IR removes empty states
      // if (statenode==NULL)						// UC
      //   statenode = stateNode[state] = sfg->new_node(statestr);	// UC
      if (statenode)
	(*state_freqs)[statenode]=freq;
    }
    else {
      fsm_feedback_error(fname,lcnt,pos);
    }
  }

  /*
  cerr << "// sfg:\n";							// UC
  sfg->print(cerr);
  node s;
  forall_nodes(s,*sfg)
    cerr << "// count(" << (*sfg)[s] << ")=" << (*state_freqs)[s] << '\n';
  edge e;
  forall_edges(e,*sfg)
    cerr << "// count(" << (*sfg)[sfg->source(e)]
	 << ','         << (*sfg)[sfg->target(e)]
	 << ")="        << (*state_xfer_freqs)[e] << '\n';
  */
}


////////////////////////////////////////////////////////////////

/*
int main (int argc, char **argv)
{
  // - simple driver:  "feedback_fsm xxx.ffsm"
  sgraph sfg;
  node_map<int> state_freqs;
  edge_map<int> state_xfer_freqs;
  FILE *fp=fopen(argv[1],"r");
  read_fsm_feedback(fp, argv[1], &sfg, NULL, &state_freqs, &state_xfer_freqs);
  fclose(fp);

  #define PAGE_AREA_MIN 3
  #define PAGE_AREA_MAX 5
  list<set<node> > state_clusters = clusterStates(sfg,
						  PAGE_AREA_MIN,PAGE_AREA_MAX,
						  &state_xfer_freqs);
}
*/
