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
// SCORE TDF compiler:  State clustering for partitioning
// $Revision: 1.144 $
//
//////////////////////////////////////////////////////////////////////////////


// LEDA BUG
//   - symptom:      iterate "forall (e,G.out_edges(v))" destroys graph!!!
//   - work-around:  iter over copy "list<edge> G_out_edges_v=G.out_edges(v);"



#include <stdio.h>
#include <unistd.h>
#include <math.h>
#include <LEDA/graph/graph.h>
#include "operator.h"
#include "stmt.h"
#include "expr.h"
#include "file.h"
#include "feedback.h"
#include "feedback_fsm.h"
#include "misc.h"
#include "annotes.h"
#include "ir_analyze_registers.h"
#include "clusterstates.h"
#include "mincut.h"
#include "dismantle.h"
#include "instance.h"


using leda::list_item;
using leda::dic_item;
using leda::map;
using leda::graph;
using leda::node_struct;

using std::istream;
using std::ostream;
using std::cerr;
using std::endl;
using std::cout;

float gDFSFmix = 1.;	// mix frac [0..1] for data-flow/state-flow edge weight

typedef GRAPH<string,string> sgraph;
typedef GRAPH<node,edge> ShadowGraph;


////////////////////////////////////////////////////////////////
// LEDA 3.7.1 fixes

// - In LEDA version 3.8 and higher,
//     edge_map is subclass of edge_array,
//     node_map is subclass of node_array.
// - In LEDA version 3.7.1, classes are unrelated,
//     so we 'fake' subsumption using template polymorphism (less type-safe):
//     * replace edge_array<int> with template var (or typedef) EDGE_ARRAY_INT
//     * replace node_array<int> with template var (or typedef) NODE_ARRAY_INT
//     * precede any function that uses edge_array<int> / node_array<int>
//         with macro:  USE_EDGE_ARRAY_INT_NODE_ARRAY_INT

#if __LEDA__==371
  #define USE_EDGE_ARRAY_INT \
    template<class EDGE_ARRAY_INT>
  #define USE_NODE_ARRAY_INT \
    template<class NODE_ARRAY_INT>
  #define USE_EDGE_ARRAY_INT_NODE_ARRAY_INT \
    template<class EDGE_ARRAY_INT, class NODE_ARRAY_INT>
#else
  #define USE_EDGE_ARRAY_INT
  #define USE_NODE_ARRAY_INT
  #define USE_EDGE_ARRAY_INT_NODE_ARRAY_INT
  typedef edge_array<int> EDGE_ARRAY_INT;
  typedef node_array<int> NODE_ARRAY_INT;
#endif


////////////////////////////////////////////////////////////////
//  Clustering Statistics

static long long total_instances	      =0; // pages before clustering
static long long total_instance_area	      =0; // area  before clustering
static long long total_clusters		      =0; // pages after  clustering
static long long total_cluster_area	      =0; // area  after  clustering
static long long total_cluster_cut_df_bits    =0; // total cut of dataflow bits
static long long total_cluster_cut_df_streams =0; // total cut of df streams
static long long total_cluster_cut_freq	      =0; // total cut of state xfer freq
static long long total_cluster_state_freq     =0; // total state freqs


void printStateClusterStats ()
{
  // - Print global clustering statistics after done clustering all pages

  if (total_clusters==0)
    return;
  cout << "Number of instances (pre-parttn pages) = " << total_instances<<'\n';
  cout << "Ave. instance area = " << total_instance_area/total_instances<<'\n';
  cout << "Number of clusters (pages) = " << total_clusters << '\n';
  cout << "Ave. cluster area = " << total_cluster_area/total_clusters << '\n';
  cout << "Total (double) cluster cut-freq / state-freq = "
       << total_cluster_cut_freq << '/' << total_cluster_state_freq
       << " (" << (total_cluster_cut_freq*100./total_cluster_state_freq)
       << "%)\n";
  cout << "Total (double) cluster cut dataflow streams = "
       << total_cluster_cut_df_streams << " ("
       << total_cluster_cut_df_streams*100/total_clusters/100.
       << " per page)\n";
  cout << "Total (double) cluster cut dataflow bits = "
       << total_cluster_cut_df_bits << " ("
       << total_cluster_cut_df_bits*100/total_clusters/100.
       << " per page)\n";
}


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


string sprintPathArraySG (graph& G, node_array<edge>& path, int repeat=0)
{
  string ret;

  ShadowGraph& tsfg  = (ShadowGraph&)G;
  ShadowGraph* lsfgp = (ShadowGraph*)graph_of(tsfg.first_node());
  sgraph*       sfgp = (sgraph*)graph_of(lsfgp->first_node());

  node v;
  ret += "Shadowed Path Array: ";
  forall_nodes (v,tsfg) {
    edge e=path[v];
    ret += sfgp->inf(lsfgp->inf(tsfg[v]));
    if (e) {
      edge   e_lsfg=tsfg[e];
      edge   e_sfg =lsfgp->inf(e_lsfg);
      string e_str =sfgp->inf(e_sfg);
      ret += "<-" + e_str;
      node   n_tsfg=tsfg.source(e);
      node   n_lsfg=tsfg[n_tsfg];
      node   n_sfg =lsfgp->inf(n_lsfg);
      string n_str =sfgp->inf(n_sfg);
      ret += "-" + n_str;
    }
    ret += ' ';

  }
  ret += '\n';

  if (repeat>0)
    ret += sprintPathArraySG(G,path,repeat-1);

  return ret;
}


string sprintPathSG (graph& G, node_array<edge>& path, node src, node dst,
		     int repeat=0)
{
  string ret;

  ShadowGraph& tsfg  = (ShadowGraph&)G;
  ShadowGraph* lsfgp = (ShadowGraph*)graph_of(tsfg.first_node());
  sgraph*       sfgp = (sgraph*)graph_of(lsfgp->first_node());

  node v;
  edge e;
  ret += "Shadowed Path: {" + sfgp->inf(lsfgp->inf(tsfg[dst])) + "} <-- ";
  for (v=dst; (e=path[v]); v=tsfg.source(e))
    ret += sfgp->inf(lsfgp->inf(tsfg[e])) + " ";
  ret += " <-- {" + sfgp->inf(lsfgp->inf(tsfg[src])) + "}\n";

  if (repeat>0)
    ret += sprintPathSG(G,path,src,dst,repeat-1);

  return ret;
}


USE_EDGE_ARRAY_INT

string sprintSFGedgeWeights (GRAPH<SFNode*,SFEdge*> &sfg,
			     EDGE_ARRAY_INT *sfg_edge_weights)
{
  string ret;
  ret += "SFG edge weights: \n";
  edge e;
  ret += string("  (%d edges in graph)\n",sfg.number_of_edges());
  forall_edges(e,sfg) {
    ret += "  "
	+  sfg[sfg.source(e)]->getState()->getName() + "->";
    ret += sfg[sfg.target(e)]->getState()->getName() + " = ";
    ret += string("%d\n",(*sfg_edge_weights)[e]);
  }
  return ret;
}


USE_EDGE_ARRAY_INT_NODE_ARRAY_INT

string sprintTSFG (GRAPH<SFNode*,SFEdge*> &sfg,
		   ShadowGraph &lsfg, ShadowGraph &tsfg,
		   EDGE_ARRAY_INT *edge_weights=NULL,
		   NODE_ARRAY_INT *node_weights=NULL)
{
  string ret ("TSFG:  %d nodes, %d edges\n",
	      tsfg.number_of_nodes(), tsfg.number_of_edges());
  node n;
  forall_nodes (n,tsfg) {
    ret += "  node " + sfg[lsfg[tsfg[n]]]->getState()->getName();
    if (node_weights)
      ret += string("  (area=%d)",(*node_weights)[n]);
    ret += '\n';
  }
  edge e;
  forall_edges (e,tsfg) {
    ret += "  edge " + sfg[lsfg[tsfg[tsfg.source(e)]]]->getState()->getName()
        +  " -> "    + sfg[lsfg[tsfg[tsfg.target(e)]]]->getState()->getName();
    ret += (lsfg[tsfg[e]] ? (sfg[lsfg[tsfg[e]]] ? "  [SF]" : "  [DF]")
			  : ("  [N/A]")                               );
    if (edge_weights)
      ret += string("  (weight=%d)",(*edge_weights)[e]);
    ret += '\n';
  }
  return ret;
}


USE_EDGE_ARRAY_INT_NODE_ARRAY_INT

void printTSFG_vcg (FILE* fp,
		    GRAPH<SFNode*,SFEdge*> &sfg,
		    ShadowGraph &lsfg, ShadowGraph &tsfg,
		    EDGE_ARRAY_INT *edge_weights=NULL,
		    NODE_ARRAY_INT *node_weights=NULL)
{
  // - print tsfg in vcg format to FILE* fp
  fprintf(fp,"graph: {\n");
  node n;
  forall_nodes (n,tsfg) {
    char weight_str[100]="";
    if (node_weights)
      sprintf(weight_str," (%d)",(*node_weights)[n]);
    char name_str[1024];
    strcpy(name_str,sfg[lsfg[tsfg[n]]]->getState()->getName());
    fprintf(fp,"  node: {title: \"%s\" label: \"%s%s\"}\n",
	    name_str,name_str,weight_str);
  }
  edge e;
  forall_edges (e,tsfg) {
    char weight_str[100]="";
    if (edge_weights)
      sprintf(weight_str,"(%d)",(*edge_weights)[e]);
    char src_str[1024], dst_str[1024];
    strcpy(src_str,sfg[lsfg[tsfg[tsfg.source(e)]]]->getState()->getName());
    strcpy(dst_str,sfg[lsfg[tsfg[tsfg.target(e)]]]->getState()->getName());
    const char *color_str = lsfg[tsfg[e]] ? (sfg[lsfg[tsfg[e]]] ?"red" :"blue")
					  : ("black");
    fprintf(fp,"  edge: {sourcename: \"%s\" targetname: \"%s\" "
	                "color: %s textcolor: %s label: \"%s\"}\n",
	    src_str,dst_str,color_str,color_str,weight_str);
  }
  fprintf(fp,"}\n");
}


USE_EDGE_ARRAY_INT_NODE_ARRAY_INT

void printTSFG_vcg_file (const char *basename,
			 GRAPH<SFNode*,SFEdge*> &sfg,
			 ShadowGraph &lsfg, ShadowGraph &tsfg,
			 EDGE_ARRAY_INT *edge_weights=NULL,
			 NODE_ARRAY_INT *node_weights=NULL)
{
  // - prints tsfg to file "basename.nnn.vcg" where nnn is incrementing counter
  static int filenum=0;
  char filename[1024];
  sprintf(filename,"%s.%03d.vcg",basename,filenum++);
  FILE *fp=fopen(filename,"w");
  printTSFG_vcg(fp,sfg,lsfg,tsfg,edge_weights,node_weights);
  fclose(fp);
}


////////////////////////////////////////////////////////////////
//  ShadowGraph

// ShadowGraph is used to shadow (copy) another graph
// s.t. each node/edge points to the original graph's node/edge

typedef GRAPH<node,edge> ShadowGraph;

void createShadowGraph (graph& G, ShadowGraph& SG)
{
  SG=ShadowGraph();			// - clear SG

  node_array<node> copy_nodes(G);	// - map from G to corresponding in SG
  //edge_array<edge> copy_edges(G);

  node n;
  forall_nodes (n,G) {
    node Sn=SG.new_node(n);
    copy_nodes[n]=Sn;
  }

  edge e;
  forall_edges (e,G) {
    edge Se=SG.new_edge(copy_nodes[G.source(e)],
			copy_nodes[G.target(e)],e);	if (Se);
    // copy_edges[e]=Se;
  }
}


////////////////////////////////////////////////////////////////
//  Graph dup

template<class N,class E>
void createDupGraph (GRAPH<N,E>& G, GRAPH<N,E>& DG,
		     node_array<node>* orig_nodes=NULL,
		     edge_array<edge>* orig_edges=NULL)
{
  // - duplicate graph G in DG  (non-hidden nodes/edges only)
  // - fill indirection arrays orig_nodes, orig_edges (if provided)
  //     to map from node/edge of DG to corresponding node/edge of G

  DG.clear();

  node_array<node> copy_nodes(G);	// - map from G to corresponding in DG
  edge_array<edge> copy_edges(G);

  // - construct dup graph DG

  node n;
  forall_nodes (n,G) {
    node Dn=DG.new_node(G[n]);
    copy_nodes[n]=Dn;
  }

  edge e;
  forall_edges (e,G) {
    edge De=DG.new_edge(copy_nodes[G.source(e)],
			copy_nodes[G.target(e)],G[e]);
    copy_edges[e]=De;
  }

  // - fill indirection maps orig_nodes, orig_edges

  if (orig_nodes) {
    orig_nodes->init(DG);
    forall_nodes (n,G) {
      node Dn=copy_nodes[n];
      (*orig_nodes)[Dn]=n;
    }
  }

  if (orig_edges) {
    orig_edges->init(DG);
    forall_edges (e,G) {
      edge De=copy_edges[e];
      (*orig_edges)[De]=e;
    }
  }
}


////////////////////////////////////////////////////////////////
//  merge_nodes

USE_EDGE_ARRAY_INT_NODE_ARRAY_INT

void merge_nodes (graph& G, node src, node dst,
		  EDGE_ARRAY_INT* ew=NULL,  NODE_ARRAY_INT* nw=NULL,
		  EDGE_ARRAY_INT* ew2=NULL, NODE_ARRAY_INT* nw2=NULL,
		  EDGE_ARRAY_INT* ew3=NULL, NODE_ARRAY_INT* nw3=NULL)
{
  // merge node dst into node src.
  // merged nodes are combined with summed node weights (nw, nw2, nw3).
  // merged edges are combined with summed edge weights (ew, ew2, ew3).
  // collapsed edges (would-be self loops) are removed

  assert(src!=dst);
  list<edge> G_neighbor_edges_src = G.out_edges(src),
             G_in_edges_src       = G.in_edges(src);
  G_neighbor_edges_src.conc(G_in_edges_src);
  edge e;
  forall (e,G_neighbor_edges_src) {
    node target=G.opposite(src,e);
    if (target==src)				// - ignore existing self-loops
      continue;
    else if (target==dst)			// - remove collapsed edge
      G.del_edge(e);
    else if (target!=src) {			// - check for mergeable edges
      edge ee;
      list<edge> G_neighbor_edges_target = G.out_edges(target),
		 G_in_edges_target       = G.in_edges(target);
      G_neighbor_edges_target.conc(G_in_edges_target);
      forall (ee,G_neighbor_edges_target) {
	if (G.opposite(target,ee)==dst) {	// - possible merge, check dir
	  if (G.source(e)==G.source(ee) ||	//   src <-e- target -ee-> dst
	      G.target(e)==G.target(ee)   ) {	//   src -e-> target <-ee- dst
						// - merge edge ee into e
	    if (ew)
	      (*ew)[e] += (*ew)[ee];
	    if (ew2)
	      (*ew2)[e] += (*ew2)[ee];
	    if (ew3)
	      (*ew3)[e] += (*ew3)[ee];
	    G.del_edge(ee);
	  }
	}
      }
    }
  }
  if (nw)
    (*nw)[src] += (*nw)[dst];
  if (nw2)
    (*nw2)[src] += (*nw2)[dst];
  if (nw3)
    (*nw3)[src] += (*nw3)[dst];
  G.merge_nodes(src,dst);		// - merge nodes (transplants edges)
}


////////////////////////////////////////////////////////////////
//  clusterStates version 1

node farthestNodeUnitWeight (graph& G, edge_array<int>& weights, node src)
{
  // returns node of G that is farthest from src
  // along path of positive-weight edges,
  // with positive weights treated as unit weight (uses BFS)

  list<node> frontier;
  node_array<int> color(G,0);		  // 0=unvisited, 1=frontier, 2=done
  color[src]=1;
  frontier.append(src);
  node v;
  while (!frontier.empty()) {
    v=frontier.pop();			  // - grab frontier node v
    assert(color[v]==1);
    color[v]=2;
    edge e;
    list<edge> G_out_edges_v=G.out_edges(v);
    forall (e,G_out_edges_v) {		  // - check neighbors d of v
      node d=G.target(e);
      if (weights[e]>0 && color[d]==0) {  // - add reachable neighbors to frntr
	color[d]=1;
	frontier.append(d);
      }
    }
    // color[v]=2;
  }
  return v;				  // - return last, i.e. farthest node
}


static node_array<int> *cmpNodesByWeight_weights = NULL;
int cmpNodesByWeight (const node& a, const node& b)
{
  // cmp nodes by weight, backwards result
  return (*cmpNodesByWeight_weights)[a]<(*cmpNodesByWeight_weights)[b] ? 1 :
         (*cmpNodesByWeight_weights)[a]>(*cmpNodesByWeight_weights)[b] ?-1 : 0;
}


// clusterStates()
// - Cluster states into page-sized groups
//     using Wong-style balanced min-cut bipartitioning
// - "Efficient Network Flow Based Min-Cut Balanced Partitioning"
//     Yang+Wong (U.T.Austing) ICCAD '94
// - Several hacks:
//     (1) Graph model duplicates fwd edges into bwd edges
//           (not using Wong's gadget that captures fanout, not needed for SF)
//     (2) Choose source node as (a) random or (b) largest
//           from among nodes that have not been tried as sources
//           since last graph modification (Wong: pure random)
//     (3) Choose dest.  node as (a) topologically farthest from src
//           or (b) largest or (c) largest unused (Wong: random)
//     (4) After collapsing cutset into src,
//           choose extra node to hoist randomly
//           (choose 1st node w/ok area on randomized edge list)
//           (Wong: choose node that gives best next min-cut)
//     (5) Previously-clustered nodes stay as super-node to be re-clustered
//           (Wong: remove cluster from graph)
//     (6) No IO constraints
// - This function modifies edge hiding of sfg
// - Defunct debug mode:
//          To ignore IR, comment-out C lines and uncomment UC lines
//          (in this mode, sgraph* sfg is built according to
//           connectivity of feedback file's state xfer freqs)

list<set<node> > clusterStates_version_1 (
				GRAPH<SFNode*,SFEdge*> &sfg,		// C
				// sgraph& sfg,				// UC
				int page_area_min,
				int page_area_max,
				edge_array<int> *Freqs=NULL,
				node_array<int> *Areas=NULL)
{
  edge_array<int> freqs;		// - edge weight = transition frequency
  if (Freqs)
    freqs=*Freqs;
  else
    freqs.init(sfg,1);			// - HACK: all freqs 1, no profiling

  node_array<int> areas;		// - node weight = state area
  if (Areas)
    areas=*Areas;
  else
    areas.init(sfg,1);			// - HACK: all areas 1

  int total_area=0;
  node v;
  forall_nodes (v,sfg) {		// - make sure indiv states fit in page
    if (areas[v] > page_area_max)
      // fatal(-1, string("State ")+sfg[v]+" is too large");		// UC
      fatal(-1,								// C
	    string("State ")+sfg[v]->getState()->getName()+		// C
	    string(" is too large (%d, max %d)",areas[v],page_area_max)); // C
    total_area += areas[v];
  }

  // - special cases:
  if (sfg.number_of_edges()==0)
    fatal(-1,"clusterStates: SFG has no edges");
  if (sfg.number_of_nodes()==0) {
    warn("clusterStates: trivial clustering for zero states");
    list<set<node> > cluster_list;
    return cluster_list;
  }
  if (sfg.number_of_nodes()==1) {
    warn("clusterStates: trivial clustering for single state");
    list<set<node> > cluster_list;
    set<node> cluster;
    cluster.insert(sfg.first_node());
    cluster_list.append(cluster);
    return cluster_list;
  }
  if (total_area <= page_area_max) {
    warn("clusterStates: trivial clustering into single page");
    list<set<node> > cluster_list;
    set<node> cluster;
    node n;
    forall (n,sfg.all_nodes())
      cluster.insert(n);
    cluster_list.append(cluster);
    return cluster_list;
  }

  // - Hide zero-freq. edges
  //     (makes min-cut faster; maybe required for backwards edge model)
  edge e;
  forall_edges (e,sfg)
    if (freqs[e]==0)
      sfg.hide_edge(e);

  cerr << "=== clustering SFG w/hidden edges ===\n";
  sfg.write(cerr);

  ShadowGraph lsfg;			// - local copy (shadow) of SFG
  createShadowGraph(sfg,lsfg);		//     (to be converted into model)

  cerr << "=== local shadow graph LSFG (before backwards edges) ===\n";
  lsfg.write(cerr);

  // - Create model graph that captures cutting backwards edges  (convert lsfg)
  edge_map<int> lfreqs(lsfg,0);		// - modified weights for lsfg
  list<edge> lsfg_all_edges=lsfg.all_edges();
  forall (e,lsfg_all_edges) {
    edge re=lsfg.new_edge(lsfg.target(e),lsfg.source(e));   // - new bwd edge
    lfreqs[e] = lfreqs[re] = freqs[lsfg[e]];		    //   w/same weight
  }
  node_array<int> lareas(lsfg,0);	// - state areas for lsfg
  forall_nodes (v,lsfg)
    lareas[v] = areas[lsfg[v]];

  cerr << "======== Model graph with backwards flow ========\n";
  lsfg.write(cerr);

  node_array<set<node> > clusters(sfg);	// - map from cluster representative
  forall_nodes (v,sfg)			//   to set of orig clustered nodes
    clusters[v].insert(v);		//   (all nodes point to orig sfg)

  node_array<bool> isClustered(lsfg,0);	// - marks previously clustered nodes
					// - suffices to mark "src" nodes,
					//   since clusters collapse into them

  // - clusters[] and isClustered[] are somewhat redundant, since:
  //     unclustered nodes:    !isClustered[v],  clusters[v]=={v}
  //     merged/deleted nodes: !isClustered[v],  clusters[v].empty()
  //     cluster src nodes:     isClustered[v], !clusters[v].empty(), =={v,...}
  // - better implementation:
  //     node_array<set<node>*> clusters(lsfg);  // (initialized to unit sets)
  //     unclustered nodes:     clusters[v]==&{v}
  //     merged/deleted nodes:  clusters[v]==NULL
  //     cluster src nodes:     clusters[v]!=NULL, =={v,...}

  set<node> clustersNotUsedAsSrc;	// - Terminate clustering when this set
					//     becomes empty (points to sfg)
  bool modifiedClusters=true;		// - Rebuild clustersNotUsedAsSrc
					//     when this var is true

  bool doneClustering=false;
  while (!doneClustering)		// - Create new partition...
  {
    ShadowGraph tsfg;			// - temp SFG, will be munged
    createShadowGraph(lsfg,tsfg);
    edge_array<int> tfreqs(tsfg);	// - freqs for temp SFG
    edge e;
    forall_edges (e,tsfg)
      tfreqs[e]=lfreqs[tsfg[e]];
    node_array<int> tareas(tsfg);	// - areas for temp SFG
    node v;
    forall_nodes (v,tsfg)
      tareas[v]=lareas[tsfg[v]];

    warn ("Choosing new partition in intermediate SFG:\n");
    // tsfg.write(cerr);
    warn(sprintTSFG(sfg,lsfg,tsfg,&tfreqs,&tareas));
    // warn(sprintTSFG_vcg_file(sfg[sfg.choose_node()]
    // 			          ->getState()->getOperator()->getName(),
    // 			        sfg,lsfg,tsfg,&tfreqs,&tareas));

    if (modifiedClusters) {
      // - if modified clusters last time, recompute clustersNotUsedAsSrc
      clustersNotUsedAsSrc.clear();
      forall_nodes (v,tsfg)
	// if (!clustersNotUsedAsSrc.member(lsfg[tsfg[v]]))
	clustersNotUsedAsSrc.insert(lsfg[tsfg[v]]);	// - worry about dups?
      modifiedClusters=false;

      // cerr << "*** NEW clustersNotUsedAsSrc = {";
      // node n;
      // forall (n,clustersNotUsedAsSrc)
      //   cerr << sfg[n]->getState()->getName() << ' ';
      // cerr << "}\n";
    }

    node src,dst;			// - src/dst for flow-based min-cut
    // - HACK: src is chosen randomly from clusters that
    //         have not been tried since last modification to clusters

    // cerr << "*** clustersNotUsedAsSrc = {";
    // node n;
    // forall (n,clustersNotUsedAsSrc)
    //   cerr << sfg[n]->getState()->getName() << ' ';
    // cerr << "}\n";

    if (clustersNotUsedAsSrc.size()<=1) {	// - done if all clusters were
						//     tried as src/dst but
						//     did not modify clusters
      doneClustering=1;
      break;  // while (!doneClustering)
    }
    list<node> possibleSrcs=tsfg.all_nodes();
    // possibleSrcs.permute();			// - choose random src
    cmpNodesByWeight_weights=&tareas;		// - choose largest src (1of2)
    possibleSrcs.merge_sort(cmpNodesByWeight);	// - choose largest src (2of2)
    bool foundUnusedSrc=false;
    forall (src,possibleSrcs) {
      if (clustersNotUsedAsSrc.member(lsfg[tsfg[src]])) {
	clustersNotUsedAsSrc.del(lsfg[tsfg[src]]);
	foundUnusedSrc=true;
	break;  // forall (src,possibleSrcs)
      }
    }
    if (!foundUnusedSrc)
      warn("clusterstates: could NOT find an unused src, "
	   "yet clustersNotUsedAsSrc is not empty");
    /*
    for (src=tsfg.first_node();		// - src = first unclustered node
	 src && isClustered[tsfg[src]];	// - HACK: may miss small clustrd nodes
	 src=tsfg.succ_node(src));
    if (src==NULL) {			// - DONE if no more unclustered nodes
      doneClustering=1;
      break;  // while (!doneClustering)
    }
    isClustered[tsfg[src]]=1;
    */
    // - HACK: dst is chosen as node topologically farthest from src
    // dst=farthestNodeUnitWeight(tsfg,tfreqs,src);	// - choose random dst
    bool foundUnusedDst=false;				// - choose largest dst
    forall (dst,possibleSrcs) {
      if (dst!=src) {						// 1st arbitrry
      //if (clustersNotUsedAsSrc.member(lsfg[tsfg[dst]])) {	// 1st unused
        clustersNotUsedAsSrc.del(lsfg[tsfg[dst]]);
	foundUnusedDst=true;
	break;  // forall (dst,possibleSrcs)
      }
    }
    if (!foundUnusedDst)
      warn("clusterstates: could NOT find an unused dst, "
	   "yet clustersNotUsedAsSrc is not empty");
    cerr << "Cluster src=" << sfg[lsfg[tsfg[src]]]
						  ->getState()->getName() // C
         <<       "  dst=" << sfg[lsfg[tsfg[dst]]]
						  ->getState()->getName() // C
	 << '\n';
    assert(src!=dst);

    edge_array<int> flows, residual;
    node_array<bool> cutset;
    MinCutMaxFlow_Parts p(tsfg,tfreqs,src,dst,flows,residual,cutset);
    minCutMaxFlow_init(p);
    bool doneRefining=false;
    while (!doneRefining)		// - iteratively refine partition
    {
      while (minCutMaxFlow_augment(p));	// - new min-cut
      minCutMaxFlow_cutset(p);
      int cluster_area=0;
      node v;
      forall_nodes (v,tsfg)
	if (cutset[v])
	  cluster_area += tareas[v];

      cerr << "Intermediate cutset = {";
      forall_nodes (v,tsfg)
	if (cutset[v])
	  cerr << sfg[lsfg[tsfg[v]]]
				    ->getState()->getName()		// C
	       << ' ';
      cerr << "}\n";

      if (cluster_area < page_area_min)
      {
	// - partition too small:  collapse cutset + hoist 1 more
	cerr << "  partition too small (" << cluster_area << ") - collapsing";
	forall_nodes (v,tsfg) {
	  if (cutset[v] && v!=src) {
	    clusters[lsfg[tsfg[src]]] += clusters[lsfg[tsfg[v]]];
	    clusters[lsfg[tsfg[v]]].clear();
	    merge_nodes(lsfg,tsfg[src],tsfg[v],&lfreqs,&lareas);
	    merge_nodes(tsfg,src,v,&tfreqs,&tareas,&residual,
			(typeof(tareas)*)NULL,&flows);
	    modifiedClusters=true;
	    // cerr << "  - modifiedClusters=true;\n";
	  }
	}
	// - randomize cut-set order (dst last), hoist first node with Ok area
	bool hoisted=false;
	bool dst_is_hoistable=false;
	list<edge> tsfg_out_edge_src=tsfg.out_edges(src);
	list<node> tsfg_out_node_src;
	edge e;
	forall (e,tsfg_out_edge_src) {		// - (build permuted cut-set)
	  node h=tsfg.target(e);
	  // if (h==dst)			// - hoisting dst is
	  //   dst_is_hoistable=true;		//     sub-optimal, don't do it
	  // else if (h!=src)
	  //   tsfg_out_node_src.append(h);
	  if (h!=src && h!=dst)
	    tsfg_out_node_src.append(h);
	}
	tsfg_out_node_src.permute();
	if (dst_is_hoistable)
	  tsfg_out_node_src.append(dst);
	node h;
	forall (h,tsfg_out_node_src) {		// - (hoist 1st node w/ok area)
	  if (cluster_area+tareas[h] <= page_area_max) {
	    clusters[lsfg[tsfg[src]]] += clusters[lsfg[tsfg[h]]];
	    clusters[lsfg[tsfg[h]]].clear();
	    merge_nodes(lsfg,tsfg[src],tsfg[h],&lfreqs,&lareas);
	    merge_nodes(tsfg,src,h,&tfreqs,&tareas,&residual,
			(typeof(tareas)*)NULL,&flows);
	    cerr << " + hoisting "
		 << sfg[lsfg[tsfg[h]]]
				      ->getState()->getName()		// C
		 << " into src\n";
	    hoisted=true;
	    modifiedClusters=true;
	    // cerr << "  - modifiedClusters=true;\n";
	    if (h==dst)
	      doneRefining=true;	// goto doneRefining;
	    break;  // forall (h,tsfg_out_node_src)
	  }
	  // else cannot hoist this this node, try another
	}
	if (!hoisted) {		// - done refining if no nodes left to hoist
				//    (resulting cluster may be too small)
	  cerr << " + no more to hoist into src\n";
	  doneRefining=true;	// goto doneRefining;
	}
	// tsfg.write(cerr);
      }
      else if (cluster_area > page_area_max)
      {
	// - partition too large: collapse neg cutset + hoist 1 more
	cerr << "  partition too large (" << cluster_area << ") - collapsing";
	forall_nodes (v,tsfg) {
	  if (!cutset[v] && v!=dst) {
	    // - collapsing to dst is temporary, do NOT modify lsfg or clusters
	    merge_nodes(tsfg,dst,v,&tfreqs,&tareas,&residual,
			(typeof(tareas)*)NULL,&flows);
	  }
	}
	// - randomize cut-set order (ignore src), hoist first node
	bool hoisted=false;
	list<edge> tsfg_in_edge_dst=tsfg.in_edges(dst);
	list<node> tsfg_in_node_dst;
	edge e;
	forall (e,tsfg_in_edge_dst) {		// - (build permuted cut-set)
	  node h=tsfg.source(e);
	  if (h!=src && h!=dst)
	    tsfg_in_node_dst.append(h);
	}
	tsfg_in_node_dst.permute();
	node h;
	forall (h,tsfg_in_node_dst) {		// - (hoist 1st node)
	  {
	    // - hoisting to dst is temporary, do NOT modify lsfg or clusters
	    merge_nodes(tsfg,dst,h,&tfreqs,&tareas,&residual,
			(typeof(tareas)*)NULL,&flows);
	    cerr << " + hoisting "
		 << sfg[lsfg[tsfg[h]]]
				      ->getState()->getName()		// C
		 << " into dst\n";
	    hoisted=true;
	    break;  // forall (h,tsfg_in_node_dst)
	  }
	}
	if (!hoisted) {		// - done refining if no nodes left to hoist
				//    (resulting cluster may be too small)
	  cerr << " + no more to hoist into dst\n";
	  doneRefining=true;	// goto doneRefining;
	}
	// tsfg.write(cerr);
      }
      else
      {
	// - partition area ok:  collapse cutset
	cerr << "  partition area ok (" << cluster_area << ") - collapsing\n";
	forall_nodes (v,tsfg) {
	  if (cutset[v] && v!=src) {
	    clusters[lsfg[tsfg[src]]] += clusters[lsfg[tsfg[v]]];
	    clusters[lsfg[tsfg[v]]].clear();
	    merge_nodes(lsfg,tsfg[src],tsfg[v],&lfreqs,&lareas);
	    merge_nodes(tsfg,src,v,&tfreqs,&tareas,&residual,
			(typeof(tareas)*)NULL,&flows);
	    modifiedClusters=true;
	    // cerr << "  - modifiedClusters=true;\n";
	  }
	}
	// tsfg.write(cerr);
	doneRefining=true;	// goto doneRefining;
      }
    }  // while (!doneRefining)

    // doneRefining:

    cerr << "Accepting cluster {";
    forall (v,clusters[lsfg[tsfg[src]]])
      cerr << sfg[v]
		    ->getState()->getName()				// C
	   << ' ';
    cerr << "} / cutset {";
    forall_nodes (v,tsfg)
      if (cutset[v])
	cerr << sfg[lsfg[tsfg[v]]]
				  ->getState()->getName()		// C
	     << ' ';
    cerr << "}\n";

    // clusters.insert(cluster);	// - not yet, since may modify clusters

    cerr << "Present clusters:  ";
    forall_nodes (v,sfg) {
      if (!clusters[v].empty()) {
	cerr << '{';
	node vv;
	forall (vv,clusters[v])
	  cerr << sfg[vv]
			 ->getState()->getName()			// C
	       << ' ';
	cerr << "} ";
      }
    }
    cerr << '\n';

    if (lsfg.number_of_nodes()<=1)
      doneClustering=true;
  }

  sfg.restore_all_edges();		// - unhide hidden (zero-freq.) edges

  list<set<node> > cluster_list;
  forall_nodes (v,sfg)
    if (!clusters[v].empty())
      cluster_list.append(clusters[v]);

  return cluster_list;
}


////////////////////////////////////////////////////////////////
//  clusterStates version 2

static ShadowGraph            *glsfg;	// - for debug printing
static GRAPH<SFNode*,SFEdge*> *gsfg;


node clusterStates_chooseNodeToHoist(ShadowGraph &tsfg,
				     node tsrc, node tdst,
				     edge_array<int> &tfreqs,
				     node_array<int> &tareas,
				     MinCutMaxFlow_Parts &tp,
				     int page_area_max=0,
				     bool allow_large_cutset=true)
{
  // - part of Wong balanced partitioning algorithm,
  //     find a node to hoist across old tsrc-tdst cut to get best next cut.
  // - return node neighboring tsrc (not tsrc, tdst) that,
  //     when collapsed into tsrc, yields lowest min-cut b/w tsrc-tdst.
  // - return NULL if there are no candidate nodes.
  // - if page_area_max is given (>0),
  //     then will reject hoisting nodes that lead to too-large a set
  //     before min-cutting (i.e. area(src) + area(hoist) > page_area_max);
  //     for Wong, need this when hoisting into dst, but not when into dst.
  // - if allow_large_cutset==false,
  //     then will reject hoisting nodes that lead to too-large a cut-set.
  // - use intermediate min-cut info (flows, residuals) from tp,
  //     but don't modify it.

  node best_tn  = NULL;		// - to return
  int  best_cut = -1;

  list<node> adj_nodes_tsrc_list=tsfg.adj_nodes(tsrc);
  set<node>  adj_nodes_tsrc;
  node v;
  forall (v,adj_nodes_tsrc_list)
    adj_nodes_tsrc.insert(v);
  adj_nodes_tsrc.del(tsrc);
  adj_nodes_tsrc.del(tdst);
  if (adj_nodes_tsrc.empty())
    return NULL;
  if (adj_nodes_tsrc.size()==1)
    return adj_nodes_tsrc.choose();

  while (!adj_nodes_tsrc.empty()) {
    node tn = adj_nodes_tsrc.choose();
    adj_nodes_tsrc.del(tn);

    // - try hoisting tn into src  (in copy of graph)

    warn("test hoist " + (*gsfg)[(*glsfg)[tsfg[tn]]]->getState()->getName()+
	 " into "      + (*gsfg)[(*glsfg)[tsfg[tsrc]]]->getState()->getName());

    int base_area = tareas[tsrc]+tareas[tn];	// - base area for this cutset
    if (page_area_max && (base_area > page_area_max)) {
      // - src+hoist too large, hoist no good
      warn(string("  - base set too large (%d)",base_area));
      continue;   // while (!adj_nodes_tsrc.empty())
    }

    // - create local copy of graph and min-cut info
    ShadowGraph     hsfg;			// - local copy of SFG
    createShadowGraph(tsfg,hsfg);
    edge_array<int>  hfreqs(hsfg);		// - local edge weights
    node_array<int>  hareas(hsfg);		// - local node areas
    edge_array<int>  hflows(hsfg);		// - local flows   for min-cut
    edge_array<int>  hresidual(hsfg);		// - local residuals " "
    node_array<bool> hcutset(hsfg,0);		// - local cutset    " "
    node             hsrc, hdst, hn;		// - local src, dst, hoist node
    MinCutMaxFlow_Parts hp(hsfg,hfreqs,hsrc,hdst,hflows,hresidual,hcutset);
    edge e;
    forall_edges (e,hsfg) {
      edge te = hsfg[e];
      hfreqs[e]    = tfreqs[te];
      hflows[e]    = tp.flows[te];
      hresidual[e] = tp.residual[te];
    }
    node v;
    forall_nodes (v,hsfg) {
      node tv = hsfg[v];
      hareas[v] = tareas[tv];
      if (tn==tv)
	hn=v;
      else if (tsrc==tv)
	hsrc=v;
      else if (tdst==tv)
	hdst=v;
    }
    // - note, hcutset does not need to be initialized, will be generated

    // - set local graph's edge reversals to make min-cut happy
    list<edge> added_edges;
    hsfg.make_map(added_edges);
    assert(added_edges.size()==0);

    /*
    // - DEBUG failed assertion
    cerr << "hsfg has "
	 << hsfg.number_of_nodes() << " nodes, "
         << hsfg.number_of_edges() << " edges, "
         << added_edges.size()     << " added reversal edges\n";
    forall (e,added_edges) {
      cerr << "  added edge ";
      cerr << hsfg.source(e) << "->" << hsfg.target(e) << " in hsfg ";
      if (hsfg[e])
	cerr << "(" << tsfg.source(hsfg[e]) << "->" << tsfg.target(hsfg[e])
	     << " in tsfg, freq=" << tfreqs[hsfg[e]] << ") ";
      else
	cerr << "(not in tsfg) ";
      cerr << (*gsfg)[(*glsfg)[tsfg[hsfg[hsfg.source(e)]]]]
					->getState()->getName() << "->";
      cerr << (*gsfg)[(*glsfg)[tsfg[hsfg[hsfg.target(e)]]]]
					->getState()->getName() << '\n';
      edge re=hsfg.reversal(e);
      if (re) {
	cerr << "  reversal ";
	cerr << hsfg.source(re) << "->" << hsfg.target(re) << " in hsfg ";
	if (hsfg[re])
	  cerr << "(" << tsfg.source(hsfg[re]) << "->" << tsfg.target(hsfg[re])
	       << " in tsfg, freq=" << tfreqs[hsfg[re]] << ") ";
	else
	  cerr << "(not in tsfg) ";
	cerr << (*gsfg)[(*glsfg)[tsfg[hsfg[hsfg.source(re)]]]]
					->getState()->getName() << "->";
	cerr << (*gsfg)[(*glsfg)[tsfg[hsfg[hsfg.target(re)]]]]
					->getState()->getName() << '\n';
      }
    }
    if (added_edges.size()!=0) {
      cerr << "  tsfg is\n";
      cerr << sprintTSFG(*gsfg,*glsfg,tsfg,&tfreqs,&tareas);
    }
    assert(added_edges.size()==0);
    */

    // - hoist hn into src
    merge_nodes(hsfg,hsrc,hn,&hfreqs,&hareas,&hresidual,
		(typeof(hareas)*)NULL,&hflows);

    // - min-cut  (hsrc,hdst)
    while (minCutMaxFlow_augment(hp));
    minCutMaxFlow_cutset(hp);

    // - remove equivalent hoist nodes from candidate set
    forall_nodes (v,hsfg) {
      if (hcutset[v]) {
	if (adj_nodes_tsrc.member(hsfg[v])) {
	  warn("  - equivalent to hoist "+
	       (*gsfg)[(*glsfg)[tsfg[hsfg[v]]]]->getState()->getName());
	  adj_nodes_tsrc.del(hsfg[v]);
	}
      }
    }

    // - evaluate cut
    int hcutset_area=0;
    forall_nodes (v,hsfg)
      if (hcutset[v])
	hcutset_area+=hareas[v];
    if (!allow_large_cutset && (hcutset_area > page_area_max)) {
      // - cutset too large, hoist no good
      warn(string("  - cutset too large (%d)",hcutset_area));
      continue;   // while (!adj_nodes_tsrc.empty())
    }
    int hcutset_cut=0;
    forall_edges (e,hsfg)
      if (hcutset[hsfg.source(e)] != hcutset[hsfg.target(e)])
	hcutset_cut+=hfreqs[e];
    if (   best_cut>hcutset_cut		// - found better min-cut
    	|| best_cut==-1        ) {	// - or 1st time
      best_tn  = tn;
      best_cut = hcutset_cut;
    }

    warn(string("  - creates min-cut %d",hcutset_cut)+
	 (best_tn==tn ? " (best)" : ""));

  }	// while (!adj_nodes_tsrc.empty())

  return best_tn;
}


USE_EDGE_ARRAY_INT_NODE_ARRAY_INT

node_array<bool> clusterStates_bipartition(ShadowGraph &lsfg,
					   node lsrc, node ldst,
					   EDGE_ARRAY_INT &lfreqs,
					   NODE_ARRAY_INT &lareas,
					   int page_area_min,
					   int page_area_max)
{
  // - part of Wong balanced partitioning algorithm,
  //     create a single min-cut partition within area bounds.
  // - return a single, area-bound partition
  //     in the form of a boolean set membership array (indicator array).
  // - the partition will contains lsrc but not ldst.
  // - the partition will be within page area bounds
  //     (but it is possible to get a partition smaller than page_area_min).

  assert(lsrc!=ldst);

  node_array<bool> lcluster(lsfg,false);    // - to be returned
  // lcluster[lsrc]=true;

  ShadowGraph     tsfg;			// - local copy of SFG
  createShadowGraph(lsfg,tsfg);
  edge_array<int> tfreqs(tsfg);		// - local edge weights
  node_array<int> tareas(tsfg);		// - local node areas
  node tsrc, tdst;			// - local src, dst
  edge e;
  forall_edges (e,tsfg) {
    edge le = tsfg[e];
    tfreqs[e] = lfreqs[le];
  }
  node v;
  forall_nodes (v,tsfg) {
    node lv = tsfg[v];
    tareas[v] = lareas[lv];
    if (lsrc==lv)
      tsrc=v;
    else if (ldst==lv)
      tdst=v;
  }

  edge_array<int>  tflows, tresidual;
  node_array<bool> tcutset;
  MinCutMaxFlow_Parts p(tsfg,tfreqs,tsrc,tdst,tflows,tresidual,tcutset);
  minCutMaxFlow_init(p);

  bool done_refining=false;
  while (!done_refining)
  {
    warn("computing next min-cut in:" +
	 sprintTSFG(*gsfg,*glsfg,tsfg,&tfreqs,&tareas));

    // - min-cut
    while (minCutMaxFlow_augment(p));
    minCutMaxFlow_cutset(p);
    
    // - eval min-cut
    int cutset_area=0;
    forall_nodes (v,tsfg)
      if (tcutset[v])
	cutset_area+=tareas[v];

    string msg="intermediate cutset = {";
    forall_nodes (v,tsfg)
      if (tcutset[v])
	msg += (*gsfg)[(*glsfg)[tsfg[v]]]->getState()->getName() + " ";
    msg += "}";
    warn(msg);

    if (cutset_area < page_area_min) {
      // - cutset too small
      // - collapse cutset into src
      warn(string("cutset too small (%d), collapsing into src",cutset_area));
      node n;
      forall_nodes (n,tsfg) {
	if (tcutset[n]) {
	  lcluster[tsfg[n]]=true;
	  if (n!=tsrc)
	    merge_nodes(tsfg,tsrc,n,&tfreqs,&tareas,&tresidual,
			(typeof(tareas)*)NULL,&tflows);
	}	
      }
      // - collapse one more node into src
      node h = clusterStates_chooseNodeToHoist(tsfg,tsrc,tdst,tfreqs,tareas,p,
					       page_area_max);	     // okLarge
      //  				       page_area_max,false); // noLarge
      if (!h) {
	warn("no more to hoist");
	done_refining=true;
      }
      else {
	warn("hoisting "+(*gsfg)[lsfg[tsfg[h]]]->getState()->getName());
	lcluster[tsfg[h]]=true;
	merge_nodes(tsfg,tsrc,h,&tfreqs,&tareas,&tresidual,
		    (typeof(tareas)*)NULL,&tflows);
      }
    }
    else if (cutset_area > page_area_max) {
      // - cutset too large
      // - collapse cutset into dst
      warn(string("cutset too large (%d), collapsing into dst",cutset_area));
      node n;
      forall_nodes (n,tsfg) {
	if (!tcutset[n]) {
	  if (n!=tdst)
	    merge_nodes(tsfg,tdst,n,&tfreqs,&tareas,&tresidual,
			(typeof(tareas)*)NULL,&tflows);
	}	
      }
      // - collapse one more node into dst
      node h = clusterStates_chooseNodeToHoist(tsfg,tdst,tsrc,tfreqs,tareas,p);
      if (!h) {
	warn("no more to hoist");
	done_refining=true;
      }
      else {
	warn("hoisting "+(*gsfg)[lsfg[tsfg[h]]]->getState()->getName());
	merge_nodes(tsfg,tdst,h,&tfreqs,&tareas,&tresidual,
		    (typeof(tareas)*)NULL,&tflows);
      }	
    }
    else {
      // - cutset size ok
      // - collapse cutset into src
      warn(string("cutset ok (%d), collapsing into src",cutset_area));
      node n;
      forall_nodes (n,tsfg) {
	if (tcutset[n]) {
	  lcluster[tsfg[n]]=true;
	  if (n!=tsrc)
	    merge_nodes(tsfg,tsrc,n,&tfreqs,&tareas,&tresidual,
			(typeof(tareas)*)NULL,&tflows);
	}	
      }
      done_refining=true;
    }
  }

  string msg="accepting cluster {";
  forall_nodes (v,lsfg)
    if (lcluster[v])
      msg += (*gsfg)[(*glsfg)[v]]->getState()->getName() + " ";
  msg += "}";
  warn(msg);

  minCutMaxFlow_exit(p);
  return lcluster;
}


list<set<node> > clusterStates (GRAPH<SFNode*,SFEdge*> &sfg,
				int page_area_min,
				int page_area_max,
				edge_array<int> *Freqs,
				node_array<int> *Areas)
{
  // - cluster nodes (states) of sfg into partitions having given area bound,
  //     return clusters as list of node-sets
  // - modifies edge hiding of sfg
  // - Freqs is state transition freqs, i.e. edge weights (all 1 if not given)
  // - Areas is state areas,            i.e. node weights (all 1 if not given)
  // - based on Wong balanced min-cut partitioning:
  //     "Efficient Network Flow Based Min-Cut Balanced Partitioning"
  //      Yang+Wong (U.T.Austing) ICCAD '94
  // - with several hacks:
  //     (1) Graph model duplicates fwd edges into bwd edges
  //           (Wong uses a gadget to capture fanout, not needed for SF)
  //     (2) edge weights are NOT bit widths
  //           they are linear mix of FSM state transition frequencies (counts)
  //           and bit widths of state-to-state register data flow (new edges)
  //     (3) no IO constraint
  //           (Wong: edge weights are pure bit widths, used for IO constraint)
  //     (4) all previous partitions are collapsed to become dst of next parttn
  //           (Wong: remove partitions, random src/dst)

  list<set<node> > cluster_list;	// - to be returned, initially empty

  edge_array<int> freqs_default(sfg,1);
  node_array<int> areas_default(sfg,1);
  edge_array<int> &freqs = (Freqs ? *Freqs : freqs_default);
  node_array<int> &areas = (Areas ? *Areas : areas_default);

  // - Sanity checks + special cases

  // - Trivial if there are no states
  if (sfg.number_of_nodes()==0) {
    warn("clusterStates: trivial clustering for zero states");
    return cluster_list;	// - empty
  }

  // - Make sure each state fits on a page
  int total_area=0;
  node v;
  forall_nodes (v,sfg) {
    if (areas[v]>page_area_max)
      fatal(-1,
	    string("State ")+sfg[v]->getState()->getName()+
	    string(" is too large (%d, max %d)",areas[v],page_area_max));
    total_area+=areas[v];
  }

  // - Trivial if all states fit on same page
  if (total_area <= page_area_max) {
    warn("clusterStates: trivial clustering into single page");
    set<node> cluster;			// - single cluster for all nodes
    node v;
    forall_nodes (v,sfg)
      cluster.insert(v);
    cluster_list.append(cluster);
    return cluster_list;
  }

  // - Hide zero-freq. edges  (makes min-cut faster)
  edge e;
  forall_edges (e,sfg)
    if (freqs[e]==0)
      sfg.hide_edge(e);

  // - create local model graph with backwards edges

  ShadowGraph     lsfg;			// - local copy of SFG, will be model
  createShadowGraph(sfg,lsfg);
  edge_map<int>   lfreqs(lsfg);		// - lsfg edge weights
  node_array<int> lareas(lsfg,0);	// - lsfg node areas

  list<edge> lsfg_all_edges=lsfg.all_edges();
  // edge e;
  forall (e,lsfg_all_edges) {
    edge re=lsfg.new_edge(lsfg.target(e),lsfg.source(e));   // - new bwd edge
    lfreqs[e] = lfreqs[re] = freqs[lsfg[e]];		    //   w/same weight
  }
  // node v;
  forall_nodes (v,lsfg)
    lareas[v] = areas[lsfg[v]];

  // DEBUG:
  gsfg =&sfg;
  glsfg=&lsfg;

  // - repeatedly bipartition and collapse

  node src;
  node dst = lsfg.choose_node();    // - dummy 1st dst
				    // - normally contains previous clusters
  int first_time=1;
  while (first_time ||
	 total_area-lareas[dst] > page_area_max)    // - more than a page left
  {
    assert(lsfg.number_of_nodes()>=2);

    // - bipartition from random src
    do {
      src = lsfg.choose_node();
    } while (src==dst);
    warn(string("clusterStates(): src=")+sfg[lsfg[src]]->getState()->getName()+
	 " dst="+sfg[lsfg[dst]]->getState()->getName());
    node_array<bool> cluster_array
      = clusterStates_bipartition(lsfg,src,dst,lfreqs,lareas,
				  page_area_min,page_area_max);

    // - record cluster and merge into src
    set<node> cluster_set;
    node n;
    forall_nodes (n,lsfg) {
      if (cluster_array[n]) {
        cluster_set.insert(lsfg[n]);
	if (n!=src)
	  merge_nodes(lsfg,src,n,&lfreqs,&lareas);
      }
    }
    cluster_list.append(cluster_set);

    // - collapse cluster into dst for next bipartition
    if (first_time) {
      dst=src;
      first_time=0;
    }
    else
      merge_nodes(lsfg,dst,src,&lfreqs,&lareas);

  }	// while more than a page left
  
  // - put all remaining nodes in final cluster
  
  set<node> cluster_set;
  forall_nodes (v,lsfg)
    if (v!=dst)
      cluster_set.insert(lsfg[v]);
  if (!cluster_set.empty())
    cluster_list.append(cluster_set);

  sfg.restore_all_edges();		// - unhide hidden (zero-freq.) edges

  return cluster_list;
}


//  -- FSM PARTITIONING ALGORITHM --
//     (version 2)
//  
//  Choose node to hoist:
//    Given:   G=(V,E,w,a), src, dst
//    Produce: node u connected to src (not dst) with minimum area-feasible cut
//  
//    let Wmin=infinity, u=nil
//    forall node h connected to src  (or could limit to R neighbors)
//      skip h==dst, h==src
//      create local copy of graph, GL, work only on GL
//      collapse h into src
//      min-cut src->dst to get cut-set X (nodes) with edge cut C (edges)
//      evaluate A(X), W(C)
//      if (W(C)<Wmin) and (A(X)<A(page))
//        Wmin <- W(C)
//        u    <- h
//    return u  (u may be nil if never got good A(X))
//  
//  
//  Bipartition:
//    Given:   G=(V,E,w,a), src, dst
//    Produce: source-set X in area bound
//  
//    create local copy of graph, GL, work only on GL
//    min-cut src->dst to get cut-set X (nodes) with edge cut C (edges)
//    while (not done)
//      if (X too small)
//        collapse X into src
//        choose node h (not src) to hoist into src  (get h's Xh)
//        if (nil h) then done
//        collapse h into src
//        // let X <- Xh    [not implemented, instead do min-cut on next iter]
//      else if (X too large)
//        collapse GL\X into dst
//        choose node h (not dst) to hoist into dst  (get h's Xh)
//        if (nil h) then done
//        collapse h into dst
//        // let X <- G\Xh  [not implemented, instead do min-cut on next iter]
//      else
//        done
//    return corresponding X in G
//  
//  
//  Multi-way partition:
//    Given:   G=(V,E,w,a)
//    Produce: set of clusters {C_i \subset G}
//  
//    choose random dst
//    while (A(G\dst) > A(page))
//      choose random src
//      Bipartition(G,src,dst)
//      record source-set as cluster
//      collapse source-set into src
//      if (first time)  let dst <- src
//      else             merge src into dst
//    take G\dst as final cluster


////////////////////////////////////////////////////////////////
//  Edge weight mixing

void addDFedgesToSFG (OperatorBehavioral *op,
		      GRAPH<SFNode*,SFEdge*> &sfg,
		      dictionary<string,DefUsePairList*> *defUsePairs,
		      edge_map<int> *BitWidths,
		      edge_map<Symbol*> *Symbols=NULL)
{
  // - Add state-to-state dataflow edges to state-flow graph "sfg",
  //     and copy their bit-widths to "BitWidths" array.
  // - New dataflow edges are inferenced from def-use chain "defUsePairs"
  // - New dataflow edges have have nil SFEdge* in sfg
  // - Note, edge weights are not mixed here!

  // - Create map (state name)->(SFG graph node)
  dictionary<string,node> SFG_name_to_node;
  node n;
  forall_nodes(n,sfg)
    SFG_name_to_node.insert(sfg[n]->getState()->getName(),n);

  // - For each register of op, add data-flow edges based on def-use:
  dic_item it;
  forall_items (it,*defUsePairs) {
    DefUsePairList *defUsePairList = defUsePairs->inf(it);
    string          regName        = defUsePairs->key(it);
    Symbol         *regSym         = op->getVars()->lookup(regName);
    if (regSym==NULL || regSym->getSymKind()!=SYMBOL_VAR)
      assert(!"bad register in def-use chain");
    DefUsePair *pair;
    forall (pair,*defUsePairList) {
      // - add data-flow edge for each def-use pair of the reg:
      if (pair->def==NULL)	// HACK: nil def in def-use pair is
	continue;		//       register initialization-- ignore pair
      node def_state = SFG_name_to_node.access(pair->def->getName());
      node use_state = SFG_name_to_node.access(pair->use->getName());
      edge e=sfg.new_edge(def_state,use_state,NULL);
      int bitWidth = regSym->getType()->getWidth();
      if (bitWidth<0)	// HACK: assume that width expression is 64 bits
	bitWidth=64;	//       (instance width is static, should const-prop)
      (*BitWidths)[e] = bitWidth;
      if (Symbols)
	(*Symbols)[e] = regSym;
      warn(string("addDFedgesToSFG():  adding %d-bit DF edge ",bitWidth) +
	   sfg[def_state]->getState()->getName() + "->" +
	   sfg[use_state]->getState()->getName());
    }      
  }

  // - for each state transition, add a 1-bit activation DF edge:
  edge ee;
  list<edge> edges = sfg.all_edges();
  forall (ee,edges) {
    if (sfg[ee]) {
      node def_state = sfg.source(ee);
      node use_state = sfg.target(ee);
      edge e=sfg.new_edge(def_state,use_state,NULL);
      (*BitWidths)[e] = 1;
      warn("addDFedgesToSFG():  adding 1-bit activation DF edge " +
	   sfg[def_state]->getState()->getName() + "->" +
	   sfg[use_state]->getState()->getName());
    }
  }
}


void removeDFedgesFromSFG (GRAPH<SFNode*,SFEdge*> &sfg)
{
  // - remove DF edges added by addDFedgesToSFG()
  // - specifically, removes all edges with nil SFEdge*

  edge e;
  forall_edges (e,sfg) {
    if (sfg[e]==NULL) {
      sfg.del_edge(e);
      // warn("removeDFedgesFromSFG():  removing DF edge " +
      //      sfg[sfg.source(e)]->getState()->getName() + "->" +
      //      sfg[sfg.target(e)]->getState()->getName());
    }
  }
}


USE_EDGE_ARRAY_INT

void mixSFGedgeWeights(GRAPH<SFNode*,SFEdge*> &sfg,
		       EDGE_ARRAY_INT *state_xfer_freqs,
		       EDGE_ARRAY_INT *df_bit_widths,
		       EDGE_ARRAY_INT *sfg_edge_weights,
		       float mix,
		       int scale=0)
{
  // - 0<=mix<=1 is mixing factor, 0: pure df width, 1: pure state xfer
  // - "scale" is max value to which to scale df widths and xfer freqs
  // - if scale=0, will retain xfer freqs,
  //     and will scale df widths to match max xfer freq
  //     * exception: if mix=0 or mix=1, will not scale anything
  //     * exception: if if max xfer freq =0 or max df width =0 ???
  // - DF edges have nil SFEdge* (sfg[e])

  if (mix==0) {
    // - pure DF widths
    warn("mixSFGedgeWeights():  mix=0, pure DF widths");
    // *sfg_edge_weights = *df_bit_widths;
    edge e;
    forall_edges(e,sfg)
      (*sfg_edge_weights)[e] = sfg[e] ? 0 : (*df_bit_widths)[e];
  }
  else if (mix==1) {
    // - pure state xfer freqs
    warn("mixSFGedgeWeights():  mix=1, pure SF freqs");
    // *sfg_edge_weights = *state_xfer_freqs;
    edge e;
    forall_edges(e,sfg)
      (*sfg_edge_weights)[e] = sfg[e] ? (*state_xfer_freqs)[e] : 0;
  }
  else {
    // - linear mix with scaling
    // - first, find maximum state xfer freq & max df bit width:
    warn(string("mixSFGedgeWeights(): mix=%d, scale=%d,",mix,scale));
    int max_freq=0, max_width=0;
    edge e;
    forall_edges (e,sfg) {
      if (sfg[e]) {	// - SF edge  (state_xfer_freqs lacks DF edges!)
	int f=(*state_xfer_freqs)[e];
	if (max_freq<f)
	  max_freq=f;
      }
      else {		// - DF edge
	int w=(*df_bit_widths)[e];
	if (max_width<w)
	  max_width=w;
      }
    }
    warn(string("                     max_width=%d, max_freq=%d",
							max_width,max_freq));
    // - now mix weights:
    // edge e;
    forall_edges (e,sfg) {
      if (sfg[e]) {
	// - e is state flow
	if (scale)
	  (*sfg_edge_weights)[e] = (int)ceil((*state_xfer_freqs)[e] * (1-mix)
						    * (1.*scale/max_freq));
	else
	  (*sfg_edge_weights)[e] = (int)ceil((*state_xfer_freqs)[e] * (1-mix));
      }
      else {
	// - e is data flow
	if (scale)
	  (*sfg_edge_weights)[e] = (int)ceil((*df_bit_widths)[e] * mix
						    * (1.*scale/max_width));
	else
	  (*sfg_edge_weights)[e] = (int)ceil((*df_bit_widths)[e] * mix
						    * (1.*max_freq/max_width));
      }
    }
  }

  warn("Mixed " + sprintSFGedgeWeights(sfg,sfg_edge_weights));
}


////////////////////////////////////////////////////////////////
//  Find Symbol Accessors

template<class K,class E>
void add_accessor (map<K,set<E>*>& accessors, K key, E el)
{
  // - does:  accessors[key]->insert(el)
  // - creates the set accessors[key] if it does not exist yet (NULL)

  set<E> *accessor_set=accessors[key];
  if (!accessor_set)
    accessor_set = accessors[key] = new set<E>;
  accessor_set->insert(el);
}


void find_accessors (SFGraph sfg, map<Symbol*,set<node>*> &accessors)
{
  // - find which states of a behavioral op access which symbols
  //     (needed for pre-clustering states)
  // - given sfg
  // - fill accessors to map each symbol (reg/stream) to states that access it

  node n;
  forall_nodes (n,*sfg) {		// - record accesses in state n
    string stateName=(*sfg)[n]->getState()->getName();
    SCCell *sccell;
    forall (sccell,*(*sfg)[n]->getSCCells()) {
      // - record accesses of input consumption (by symbol)
      StateCase *sc=sccell->getStateCase();
      InputSpec *ispec;
      forall (ispec,*sc->getInputs()) {
	add_accessor(accessors,ispec->getStream(),n);
	// warn(stateName+" inputs "+ispec->getStream()->getName());
      }
      // - record accesses of input-stream values (by symbol)
      DFGraph dfg=sccell->getDFG();
      node dn;
      forall (dn,*sccell->getUsedStreams()) {
	Expr *expr=(*dfg)[dn]->getExpr();
	assert(expr->getExprKind()==EXPR_LVALUE);
	Symbol *sym=((ExprLValue*)expr)->getSymbol();
	add_accessor(accessors,sym,n);
	// warn(stateName+" uses stream "+sym->getName());
      }
      // - record accesses of output-stream emission (by symbol)
      DFNode *dfn;
      forall (dfn,*sccell->getWrittenStreams()) {
	Expr *expr=dfn->getExpr();
	assert(expr->getExprKind()==EXPR_LVALUE);
	Symbol *sym=((ExprLValue*)expr)->getSymbol();
	add_accessor(accessors,sym,n);
	// warn(stateName+" writes stream "+sym->getName());
      }
      // - record accesses of register reads (by symbol)
      forall (dn,*sccell->getUsedReg()) {
	Expr *expr=(*dfg)[dn]->getExpr();
	assert(expr->getExprKind()==EXPR_LVALUE);
	Symbol *sym=((ExprLValue*)expr)->getSymbol();
	if (!sym->getAnnote(ANNOTE_IS_SIMPLE_REG)) {
	  add_accessor(accessors,sym,n);
	  // warn(stateName+" uses reg "+sym->getName());
	}
      }
      // - record accesses of register writes (by symbol)
      forall (dfn,*sccell->getDefReg()) {
	Expr *expr=dfn->getExpr();
	assert(expr->getExprKind()==EXPR_LVALUE);
	Symbol *sym=((ExprLValue*)expr)->getSymbol();
	if (!sym->getAnnote(ANNOTE_IS_SIMPLE_REG)) {
	  add_accessor(accessors,sym,n);
	  // warn(stateName+" writes reg "+sym->getName());
	}
      }
    }  // forall (sccell,*(*sfg)[n]->getSCCells())
  }  // forall_nodes (n,*sfg)
}


////////////////////////////////////////////////////////////////
//  synthesizePartitionedOp

// The following code synthesizes page code for a partitioned operator.
// This is fairly straightforward for code dismantled
//   such that each reg/stream access gets its own state.
// Inter-page register data-flow is converted directly into a data-stream
//   (reg write becomes stream write; reg read requires stream input).
// Inter-page state transition is converted into activation token and wait.
// Pseudo code is roughly as follows:
//
// convert behav op into compos op
// 
// for each cluster
//   create new behav op
//   if an IO stream lives in this cluster
//     add stream to behav op
//     add stream to call of behav op in compos op
//   create wait state as start state (initially, no state cases)
//                                    (will fix actual start state later)
// 
// for each inter-cluster stream
//   if stream is activation
//     create activation stream 'source_goto_target'
//     add activation stream to source behav op
//     add activation stream to target behav op
//     add activation stream to calls of behav ops in compos op
//     in source: convert 'goto target' into 'source_goto_target=0; goto wait;'
//     in target: add state-case 'state wait(source_goto_target): goto target;'
//   else if stream is reg data-flow
//     turn reg into data stream  (already has unique name)
//     add data stream to source behav op
//     add data stream to target behav op
//     add data stream to calls of behav ops in compos op
//     in source:  'reg=val' is now stream write-- do nothing
//     in target:  consuming state ('...=reg') gets reg (stream) in input sig


class CallInfo {
public:
  Operator *op;
  ExprCall *call;
  CallInfo (Operator *op_, ExprCall *call_) : op(op_), call(call_) {}
};


bool find_call_map (Tree *t, void *i)
{
  // - find call to i->op in compositional op t; return it in i->call

  CallInfo *call_info=(CallInfo*)i;

  switch (t->getKind()) {
    case TREE_OPERATOR:	return ((Operator*)t)->getOpKind()==OP_COMPOSE;
    case TREE_STMT:	return ((Stmt*)t)->getStmtKind()==STMT_CALL;
    case TREE_EXPR:	if (((Expr*)t)->getExprKind()==EXPR_CALL
			    && ((ExprCall*)t)->getOp()==call_info->op) {
			  call_info->call=(ExprCall*)t;
			  return false;
			}
			else
			  return true;
    default:		return false;
  }
}


StmtGoto* sfedge_to_stmtgoto (SFGraph sfg, edge e)
{
  // - find StmtGoto that corresponds to state transition edge e of sfg

  SFEdge *sfe=(*sfg)[e];
  assert(sfe);
  StateCase *sc=sfe->getSCCell()->getStateCase();
  Stmt *lastStmt=sc->getStmts()->tail();
  if (lastStmt->getStmtKind()==STMT_BLOCK)
    lastStmt = ((StmtBlock*)lastStmt)->getStmts()->tail();
  Stmt *ret=lastStmt;
  if (sfe->getCond()) {
    assert(lastStmt->getStmtKind()==STMT_IF);
    ret = sfe->getOutcome() ? ((StmtIf*)lastStmt)->getThenPart()
			    : ((StmtIf*)lastStmt)->getElsePart();
  }
  assert(ret->getStmtKind()==STMT_GOTO);
  return (StmtGoto*)ret;
}


OperatorCompose* synthesizePartitionedOp (OperatorBehavioral *op,
					  SFGraph sfg,
					  list<set<node> > clusters,
					  map<Symbol*,set<node>*> *accessors,
					  edge_map<Symbol*> *df_symbols)
{
  // - given behav-op 'op' with
  //     + state-flow graph 'sfg' with SF and DF edges
  //     + page-size 'clusters' of sfg nodes (states)
  //     + accessors   (map: symbol   -> set of sfg nodes that access it)
  //     + df symbols  (map: sfg edge -> symbol for associated reg data-flow)
  // - create a new behav-op for each cluster
  // - create a new compos-op that connects new behav-ops
  // - 'op' is destroyed
  // - return the new compos-op

  warn(string("Synthesizing page-partitioned op:\n")+op->toString());

  set<OperatorBehavioral*>	     bops;	// - new behav-ops
  map<State*,OperatorBehavioral*>    bop_of;	// - new behav-op for state
  map<OperatorBehavioral*,ExprCall*> call_of;	// - compos-op call for new bop

  // - create compos-op  (it steals all args from orig behav-op 'op')
  SymTab          *cop_vars=new SymTab(SYMTAB_BLOCK);
  list<Stmt*>     *cop_stmts=new list<Stmt*>;
  OperatorCompose *cop=new OperatorCompose(NULL,op->getName(),
					   op->getRetSym(),op->getArgs(),
					   cop_vars,cop_stmts);
  // CallInfo call_info(op,NULL);
  // iop->map(find_call_map,(TreeMap)NULL,&call_info);
  // assert(call_info.call);
  // call_info.call->setOp(cop);

  int cluster_num=1;
  set<node> cluster;
  forall (cluster,clusters) {
    // - create new behav-op for this cluster
    dictionary<string,State*> *states=new dictionary<string,State*>;
    list<Symbol*>             *args  =new list<Symbol*>;
    SymTab                    *vars  =new SymTab(SYMTAB_BLOCK);
    node n;
    forall (n,cluster) {
      State *state=(*sfg)[n]->getState();
      states->insert(state->getName(),state);      
    }
    State *waitState=new State(NULL,"wait");
    uniqueRenameState_inc(waitState,op->getStates());
    states->insert(waitState->getName(),waitState);
    string bopName=op->getName()+string("_%d",cluster_num++);
    OperatorBehavioral *bop =
      new OperatorBehavioral(NULL,bopName,(Symbol*)sym_none->duplicate(),
			     args,vars,states,waitState);
    bops.insert(bop);
    warn(string("Created new behav op:\n")+bop->toString());
    // - bop presently has no args and vars (fill in later)

    State *state;
    forall (state,*states)
      bop_of[state]=bop;

    // - create compos-op call for the new behav-op
    ExprCall *bop_call_expr=new ExprCall(NULL,new list<Expr*>,bop);
    StmtCall *bop_call_stmt=new StmtCall(NULL,bop_call_expr);
    call_of[bop]=bop_call_expr;
    cop->getStmts()->append(bop_call_stmt);	// - HACK: modifying stmt list
    bop_call_stmt->setParent(cop);
    warn(string("Adding call in compos-op: ")+bop_call_stmt->toString());
  }  // forall (cluster,clusters)

  // - at this point, compos-op has been created w/args,
  //     and all behav-ops have been created w/o args;
  //     compos-op has arg-less calls to all behav-ops;
  // - note, orig behav-op (and hence new compose-op) may have params,
  //     but params have already been bound,
  //     so they do not need to be passed to new behav-ops.
  // - to do below:  add vars to behav-ops; add inter-behav-op streams.

  // - connect compos-op's IO streams to new behav-ops
  //     (assuming each IO stream lives in one cluster, hence one behav-op)
  Symbol *symStream;
  forall (symStream,*op->getArgs()) {
    // - add symStream to new behav-op
    if (symStream->isParam())	// - ignore params, they are already bound
      continue;
    assert(symStream->isStream());
    State *accessor=(*sfg)[(*accessors)[symStream]->choose()]->getState();
    OperatorBehavioral *bop = bop_of[accessor];
    Symbol *newSymStream=(Symbol*)symStream->duplicate();
    bop->getSymtab()->addSymbol(newSymStream);
    bop->getArgs()->append(newSymStream);
    warn(string("Adding stream ")+newSymStream->getName()+
	 string(" to new behav-op ")+bop->getName());
    // - add symStream to call of behav-op in compos-op
    ExprLValue *e=new ExprLValue(NULL,symStream);
    call_of[bop]->getArgs()->append(e);
    e->setParent(call_of[bop]);
    warn(string("Modified call in compos-op to: ")+call_of[bop]->toString());
  }

  /*
  // - recover register variables (non-transfer) in new behav-ops
  Symbol *symReg;
  forall (symReg,*op->getVars()->getSymbolOrder()) {
    assert(symReg->isReg());
    if (!symReg->getAnnote(ANNOTE_IS_SIMPLE_REG)) {
      State *accessor=(*sfg)[(*accessors)[symReg]->choose()]->getState();
      OperatorBehavioral *bop = bop_of[accessor];
      bop->getVars()->addSymbol(symReg);
      warn(string("Restoring reg var ")+symReg->getName()+
	   string(" to new behav-op ")+bop->getName());
    }
  }
  */

  // - convert inter-page state transition into activation token
  edge e;
  forall_edges (e,*sfg) {
    if (!(*sfg)[e])		// - ignore data-flow edges
      continue;
    State *src_state=(*sfg)[sfg->source(e)]->getState();
    State *dst_state=(*sfg)[sfg->target(e)]->getState();
    OperatorBehavioral *src_bop=bop_of[src_state];
    OperatorBehavioral *dst_bop=bop_of[dst_state];
    if (src_bop!=dst_bop) {
      // - add activation stream to src behav-op
      string streamName=src_state->getName()+"_goto_"+dst_state->getName();
      SymbolStream *src_stream=new SymbolStream(NULL,streamName,
						new Type(TYPE_INT,0,false),
						STREAM_OUT);
      src_bop->getSymtab()->addSymbol(src_stream);
      src_bop->getArgs()->append(src_stream);
      warn(string("Added activation stream ")+streamName+
	   string(" to src: ")+src_bop->getName());
      // - add activation stream to dst behav-op
      SymbolStream *dst_stream=new SymbolStream(NULL,streamName,
						new Type(TYPE_INT,0,false),
						STREAM_IN);
      dst_bop->getSymtab()->addSymbol(dst_stream);
      dst_bop->getArgs()->append(dst_stream);
      warn(string("Added activation stream ")+streamName+
	   string(" to dst: ")+dst_bop->getName());
      // - add activation stream to compos-op
      SymbolVar *cop_stream=new SymbolVar(NULL,streamName,
					  new Type(TYPE_INT,0,false));
      cop->getVars()->addSymbol(cop_stream);
      ExprLValue *src_e=new ExprLValue(NULL,cop_stream);
      ExprLValue *dst_e=new ExprLValue(NULL,cop_stream);
      call_of[src_bop]->getArgs()->append(src_e);
      call_of[dst_bop]->getArgs()->append(dst_e);
      src_e->setParent(call_of[src_bop]);
      dst_e->setParent(call_of[dst_bop]);
      warn(string("Added activation stream ")+streamName+
	   string(" to compose-op: ")+cop->getName()+
	   string(", new calls: ")+call_of[src_bop]->toString()+
	   string(", ")+call_of[dst_bop]->toString());
      // - convert 'goto dst_state' into 'goto xfer_state'
      //     that does activation write + wait
      string xferStateName="goto_"+dst_state->getName();
      State *xferState=src_bop->lookupState(xferStateName);
      if (!xferState) {
	// - create xfer state if it does not exist yet
	xferState=new State(NULL,xferStateName);
	list<Stmt*> *src_stmts=new list<Stmt*>;
	src_stmts->append(new StmtAssign(NULL,new ExprLValue(NULL,src_stream),
					 constIntExpr(0)));   // - activn write
	src_stmts->append(new StmtGoto(NULL,src_bop->getStartState()));
							      // - goto wait
	StateCase *src_sc=new StateCase(NULL,new list<InputSpec*>,src_stmts);
	xferState->addCase(src_sc);
	src_bop->addState(xferState);
	warn(string("Added transfer state in ")+src_bop->getName()+
	     string(": ")+xferState->toString());
      }
      StmtGoto *origGoto=sfedge_to_stmtgoto(sfg,e);
      origGoto->setState(xferState);
      warn(string("Modified goto ")+dst_state->getName()+
	   string(" into: ")+origGoto->toString()+
	   string(" in ")+src_bop->getName());
      // - add wait state in destination bop
      InputSpec *dst_ispec=new InputSpec(NULL,dst_stream);
      list<InputSpec*> *dst_ispecs=new list<InputSpec*>;
      dst_ispecs->append(dst_ispec);
      list<Stmt*> *dst_stmts=new list<Stmt*>;
      dst_stmts->append(new StmtGoto(NULL,dst_state));
      StateCase *dst_sc=new StateCase(NULL,dst_ispecs,dst_stmts);
      dst_bop->getStartState()->addCase(dst_sc);
      warn(string("Added activation wait state:\n")+dst_sc->toString());
    }  // if (src_bop!=dst_bop)
  }  // forall_edges (e,*sfg)

  // - convert inter-page register data-flow into data stream
  // edge e;
  forall_edges (e,*sfg) {
    Symbol *symReg=(*df_symbols)[e];
    if ((*sfg)[e] || !symReg)	// - ignore state transition edges  (*sfg)[e]
      continue;			//     and DF activation edges      !symReg
    State *src_state=(*sfg)[sfg->source(e)]->getState();
    State *dst_state=(*sfg)[sfg->target(e)]->getState();
    OperatorBehavioral *src_bop=bop_of[src_state];
    OperatorBehavioral *dst_bop=bop_of[dst_state];
    warn(string("DF edge ")+symReg->getName()+
	 string(" : ")  +src_bop->getName()+":"+src_state->getName()+
	 string(" --> ")+dst_bop->getName()+":"+dst_state->getName());
    if (src_bop!=dst_bop) {
      // - add data-flow stream to src behav-op
      assert(symReg->isReg());
      assert(((SymbolVar*)symReg)->getValue()==NULL);
      string streamName=symReg->getName();
      SymbolStream *src_stream =
	new SymbolStream(NULL,streamName,
			 (Type*)symReg->getType()->duplicate(),STREAM_OUT);
      src_bop->getSymtab()->addSymbol(src_stream);
      src_bop->getArgs()->append(src_stream);
      warn(string("Added data-flow stream ")+streamName+
	   string(" to src: ")+src_bop->getName());
      // - add data-flow stream to dst behav-op
      SymbolStream *dst_stream =
	new SymbolStream(NULL,streamName,
			 (Type*)symReg->getType()->duplicate(),STREAM_IN);
      dst_bop->getSymtab()->addSymbol(dst_stream);
      dst_bop->getArgs()->append(dst_stream);
      warn(string("Added data-flow stream ")+streamName+
	   string(" to dst: ")+dst_bop->getName());
      // - add data-flow stream to compos-op  (steal orig reg)
      SymbolVar *cop_stream=(SymbolVar*)symReg;
      // SymbolVar *cop_stream=new SymbolVar(NULL,streamName,
      // 				(Type*)symReg->getType()->duplicate());
      cop->getVars()->addSymbol(cop_stream);
      ExprLValue *src_e=new ExprLValue(NULL,cop_stream);
      ExprLValue *dst_e=new ExprLValue(NULL,cop_stream);
      call_of[src_bop]->getArgs()->append(src_e);
      call_of[dst_bop]->getArgs()->append(dst_e);
      src_e->setParent(call_of[src_bop]);
      dst_e->setParent(call_of[dst_bop]);
      warn(string("Added data-flow stream ")+streamName+
	   string(" to compose-op: ")+cop->getName()+
	   string(", new calls: ")+call_of[src_bop]->toString()+
	   string(", ")+call_of[dst_bop]->toString());
      // - src state is ok (reg write became stream write)
      // - add stream input to target state
      InputSpec ispec(NULL,dst_stream);
      StateCase *sc;
      forall (sc,*dst_state->getCases())
	sc->addInput((InputSpec*)ispec.duplicate());
      warn(string("Added input of stream ")+streamName+
	   string(" to state ")+dst_state->getName()+
	   string(" of dst: ") +dst_bop->getName());
    }  // if (src_bop!=dst_bop)
    else {
      // - register is intra-page data-flow in src_bop==dst_bop;  recover it
      assert(symReg->isReg());
      if (!src_bop->getVars()->lookup(symReg->getName())) {
	src_bop->getVars()->addSymbol(symReg);
	warn(string("Restoring reg var ")+symReg->getName()+
	     string(" to new behav-op ")+src_bop->getName());
      }
    }
  }  // forall_edges (e,*sfg)

  // - fix start state
  //     (at this point, all new bops have 'wait' as start state;
  //      one bop contains the original start state-- set it as start state)
  State *start_state=op->getStartState();
  assert(start_state);
  warn(string("start_state = ")+start_state->getName());
  OperatorBehavioral *start_bop=bop_of[start_state];
  assert(start_bop);
  warn(string("start_bop   = ")+start_bop->getName());  
  start_bop->setStartState(start_state);
  warn(string("Recovering start state ")+start_state->getName()+
       string(" in new behav-op ")+start_bop->getName());

  // - link new ops (fix stream/reg pointers)
  OperatorBehavioral *bop;
  forall (bop,bops) {
    bop->thread(NULL);
    bop->link();
  }
  cop->thread(cop->getParent());
  cop->link();

  // - print results
  warn(string("New compositional op:\n")+cop->toString());
  warn(string("New behavioral ops:  (partitions)\n"));
  // OperatorBehavioral *bop;
  forall (bop,bops)
    warn(bop->toString());

  return cop;
}


////////////////////////////////////////////////////////////////
//  flatten1

OperatorCompose* flatten1 (OperatorCompose *iop)
{
  // - simplified flattening of compositional hierarchy
  // - assumes 2-levels of composition, innermost contains only behaviorals
  //     outer:    iop (compositional)
  //     inner:    pop (compositional, "partitioned" op)
  //     in inner: bop (behavioral, a page)
  // - modifies/mangles compositional ops in place
  // - wrote my own because flatten() is crashing on memory allocation
  // - HACK: the following code directly modifies stmt lists and call args

  list<Stmt*> iopStmts=*iop->getStmts();
  Stmt *iopStmt;
  forall (iopStmt,iopStmts) {
    if (iopStmt->getStmtKind()==STMT_CALL) {
      ExprCall *popCallInIop=((StmtCall*)iopStmt)->getCall();
      Operator *pop_        =popCallInIop->getOp();
      warn(string("flatten1: call to ")+pop_->getName()+
	   (pop_->getOpKind()==OP_COMPOSE ? "" : " not")+" compositional");
      if (pop_->getOpKind()==OP_COMPOSE) {
	// - lift pop's behav-ops into iop
	iop->getStmts()->remove(iopStmt);	// - remove pop call from iop
	OperatorCompose *pop=(OperatorCompose*)pop_;
	Stmt *popStmt;
	forall (popStmt,*pop->getStmts()) {	// - forall bops called in pop
	  iop->getStmts()->append(popStmt);	// - move bop call to iop
	  popStmt->setParent(iop);
	  ExprCall *bopCallInPop=((StmtCall*)popStmt)->getCall();   // - in iop
	  list_item bopCallIt;
	  forall_items (bopCallIt,*bopCallInPop->getArgs()) { // - fix bop call
	    Expr *exprBopArg=bopCallInPop->getArgs()->inf(bopCallIt);
	    assert(exprBopArg->getExprKind()==EXPR_LVALUE);
	    Symbol *symBopArg=((ExprLValue*)exprBopArg)->getSymbol();
	    int rank=pop->getArgs()->rank(symBopArg);
	    if (rank>0) {	// - symBopArg is Pop IO; replace by iop wire
	      list_item popCallIt=popCallInIop->getArgs()->get_item(rank-1);
	      Expr *wireRefInIop = popCallInIop->getArgs()->inf(popCallIt);
	      bopCallInPop->getArgs()->assign(bopCallIt,wireRefInIop);
	      wireRefInIop->setParent(bopCallInPop);	// - call is in iop
	    }
	  }
	  warn(string("flatten1: flattened call ")+bopCallInPop->toString());
	}  // forall (popStmt,*pop->getStmts())
	// - lift pop's internal streams into iop
	list<Symbol*> symPopWires;
	Symbol *symPopWire;
	forall (symPopWire,*pop->getVars()->getSymbolOrder())
	  symPopWires.append(symPopWire);
	forall (symPopWire,symPopWires) {
	  pop->getVars()->removeSymbol(symPopWire);
	  uniqueRenameSym_inc(symPopWire,iop->getVars());
	  iop->getVars()->addSymbol(symPopWire);
	}
      }  // if (pop->getOpKind()==OPERATOR_COMPOSE)
      // - ignore non-compositional calls in iop

    }  // if (iopStmt->getStmtKind()==STMT_CALL)
    // - ignore non-calls in iop

  }  // forall (iopStmt,iopStmts)

  return iop;
}


////////////////////////////////////////////////////////////////
//  dump to Metis

USE_EDGE_ARRAY_INT_NODE_ARRAY_INT

void print_metis (FILE *mfp, const char* mfname,
		  SFGraph csfg,
		  OperatorBehavioral *op,
		  NODE_ARRAY_INT *csfg_state_areas,
		  EDGE_ARRAY_INT *csfg_state_xfer_freqs,
		  EDGE_ARRAY_INT *csfg_df_bit_widths,
		  SFGraph                 sfg,		   // for comments only
		  node_array<node>       *csfg_to_sfg_n,   // ''
		  node_array<set<node>*> *pre_cluster_of   // ''
		  )
{
  // - write extended metis file for *op into *mfp
  // - header:              "<numnodes> <numedges> 11 1 2"
  // - vertex description:  "<area> <neighborvertex> <xferfreq> <bitwidth> ..."

  // - build node numbers
  NODE_ARRAY_INT nodenums(*csfg);
  int nodenum=1;
  node n;
  forall_nodes (n,*csfg)
    nodenums[n]=nodenum++;

  // - emit comments
  fprintf(mfp, "%%  extended metis file for partitioning:  %s\n",
						  (const char*)op->getName());
  fprintf(mfp, "%%\n%%  vertex state pre-clusters:\n");
  forall_nodes (n,*csfg) {
    fprintf(mfp, "%%  %d  { ", nodenums[n]);
    node n_sfg;
    forall (n_sfg, *(*pre_cluster_of)[(*csfg_to_sfg_n)[n]])
      fprintf(mfp, "%s ", (const char*)((*sfg)[n_sfg]->getState()->getName()));
    fprintf(mfp, "}  area=%d\n", (*csfg_state_areas)[n]);
  }
  fprintf(mfp,"%%\n");

  // - emit header
  fprintf(mfp,"%d %d 11 1 2\n", csfg->number_of_nodes(),
				csfg->number_of_edges());

  // - emit vertex info  (1 vertex per line)
  forall_nodes (n,*csfg) {
    fprintf(mfp,"%d ",(*csfg_state_areas)[n]);
    list<edge> n_out_edges=csfg->out_edges(n);
    edge e;
    forall (e,n_out_edges) {
      node n_dst=csfg->target(e);
      fprintf(mfp,"%d %d %d ",  nodenums[n_dst],
				(*csfg_state_xfer_freqs)[e],
				(*csfg_df_bit_widths)[e]    );
    }
    fprintf(mfp,"\n");
  }
}


////////////////////////////////////////////////////////////////

Operator* clusterStates_instance (OperatorCompose *iop,
				  list<OperatorBehavioral*> *page_ops,
				  int page_area_min, int page_area_max)
{
  // - for each (unpartitioned) page op in iop:
  //     (1) read FSM feedback file (to get state transition freqs)
  //     (2) add register data-flow (DF) edges to state-flow graph (SFG)
  //           and mix their weights
  //     (3) pre-cluster states that access same stream / non-simple reg
  //     (4) call clusterStates()
  //     (5) print stats
  //     (6) synthesize partitioned pages + plug them into iop
  // - expect:  iop      = flattened compositional operator ("instance")
  //            page_ops = flattened behavioral ops (pages) called from iop
  // - return:  iop'     = flattened compositional operator w/partitioned pages
  //                         (original compositional iop is destroyed)
  //                         (original ops larger than a page are destroyed)

  // cerr << "--- clusterStates_instance(" << iop->getName()
  //      << ",(...)," << page_area_min << "," << page_area_max << ")\n";

  OperatorBehavioral *op;
  forall (op,*page_ops) {
    cerr << "--- Clustering page " << op->getName() << endl;

    // - HACK: ignore single-state ops, assuming they are pipelines
    if (op->getStates()->size()==1 ||
	op->getAnnote(ANNOTE_WAS_SINGLE_STATE_OP)) {
      warn(string("clusterStates_instance: ignoring single-state operator \"")+
	   op->getName()+"\", assuming pipeline");
      continue;   // forall (op,*page_ops)
    }

    // - read FSM feedback file  (get state transition counts)
    string fname = op->getName()+".ffsm";
    string fpath = which(fname, gFeedbackPath, R_OK);
    FILE *fp=fopen(fpath,"r");
    if (fp==NULL) {
      warn("Could not find FSM feedback file \""+fname+"\", ignoring page");
      continue;  // forall (op,*page_ops)
    }
    extern OFGraph gOFG;
    node    ofn = (node)op->getAnnote(TO_BEH_OP_INSTANCE);
    OFNode *OFN = (*gOFG)[ofn];
    SFGraph sfg = OFN->getSFG();
    node_map<int> state_freqs;
    edge_map<int> state_xfer_freqs;
    read_fsm_feedback(fp,fpath,sfg,op,
		      &state_freqs,&state_xfer_freqs);
    fclose(fp);

    // - force states/transitions with profiled freq. of 0 to have freq. 1
    //     so analysis does not completely ignore them.
    //     Note, this may make state/transition freqs inconsistent w/e/o
    node n;
    forall_nodes (n,*sfg)
      if (state_freqs[n]==0)
	state_freqs[n]=1;
    edge e;
    forall_edges (e,*sfg)
      if (state_xfer_freqs[e]==0)
	state_xfer_freqs[e]=1;
      
    // - add dataflow edges to SFG + mix DF/SF weights
    edge_map<int>     df_bit_widths(*sfg,0);
    edge_map<Symbol*> df_symbols(*sfg,NULL);
    dictionary<string,DefUsePairList*> *defUsePairs = OFN->getDefUsePairs();
    addDFedgesToSFG(op,*sfg,defUsePairs,&df_bit_widths,&df_symbols);
    edge_map<int> sfg_edge_weights(*sfg,0);   // - can be edge_array if no 371
    mixSFGedgeWeights(*sfg,&state_xfer_freqs,&df_bit_widths,
		      &sfg_edge_weights,gDFSFmix);

    // - get state areas
    node_map<int> state_areas(*sfg);
    node v;
    forall_nodes (v,*sfg) {
      int state_area = (*sfg)[v]->getArea();
      state_areas[v] = state_area;
      total_instance_area += state_area;
      warn(string("area(") + (*sfg)[v]->getState()->getName() +
	   string(") = %d",state_area));
    }

    // - determine pre-clustering of SFG nodes  (by common reg/stream access)
    map<Symbol*,set<node>*> accessors;	// - reg/stream -> sfg node set
					//     map from each sym to states that
					//     access it; code below fills this
    find_accessors(sfg,accessors);

    // - copy SFG --> csfg
    GRAPH<SFNode*,SFEdge*> csfg_graph;
    SFGraph csfg=&csfg_graph;			// - copy graph
    node_array<node> csfg_to_sfg_n;		// - indirection arrays,
    edge_array<edge> csfg_to_sfg_e;		//     init in createDupGraph()
    createDupGraph(*sfg,*csfg,&csfg_to_sfg_n,&csfg_to_sfg_e);
    node_array<int> csfg_state_areas(*csfg);	// - copy weights
    edge_array<int> csfg_edge_weights(*csfg);
    edge_array<int> csfg_state_xfer_freqs(*csfg);	// -ppmetis
    edge_array<int> csfg_df_bit_widths(*csfg);		// -ppmetis
    // - csfg_edge_weights[] is a mix of csfg_state_xfer_freqs[] and
    //   csfg_df_bit_widths[];  we keep the latter 2 only for option "-ppmetis"
    forall_nodes (n,*csfg)
      csfg_state_areas[n]=state_areas[csfg_to_sfg_n[n]];
    forall_edges (e,*csfg) {
      csfg_edge_weights    [e]=sfg_edge_weights[csfg_to_sfg_e[e]];
      csfg_state_xfer_freqs[e]=state_xfer_freqs[csfg_to_sfg_e[e]]; // -ppmetis
      csfg_df_bit_widths   [e]=df_bit_widths   [csfg_to_sfg_e[e]]; // -ppmetis
    }

    // - pre-cluster in copy of SFG (csfg)
    node_array<set<node>*> pre_cluster_of(*sfg);// sfg node -> sfg node set
    map<set<node>*,node>   pre_cluster_rep_of;	// sfg node set -> csfg node
						//   representative of cluster
    node csfg_n;
    forall_nodes (csfg_n,*csfg) {		// - initialize pre-clusters
      node n=csfg_to_sfg_n[csfg_n];		//     to singleton sets
      set<node> *pre_cluster = pre_cluster_of[n] = new set<node>;
      pre_cluster->insert(n);
      pre_cluster_rep_of[pre_cluster] = csfg_n;
    }
    set<node> *accessor_set;
    forall (accessor_set,accessors) {
      // - merge nodes (states) that access same symbol (all of *accessor_set)
      node n1=NULL,     nn;	// - sfg nodes  (from *accessor_set)
      node n1_rep=NULL, nn_rep;	// - corresponding csfg nodes  (to cluster)
      forall (nn,*accessor_set) {
	nn_rep = pre_cluster_rep_of[pre_cluster_of[nn]];
	// if (csfg->all_nodes().rank(nn_rep)==0)
	//   warn(string("Node %p not in csfg",nn_rep)+
	//        string(", is rep for %p, ",nn)+
	//        (*sfg)[nn]->getState()->getName());
	assert(pre_cluster_of[nn]==pre_cluster_of[csfg_to_sfg_n[nn_rep]]);
	if (!n1) {	// - first time:  record 1st node of *accessor_set, n1
	  n1=nn;
	  n1_rep=nn_rep;
	}
	else {		// - other times:  merge (images/clstrs of) n1 into nn
	  if (n1_rep!=nn_rep) {
	    set<node> *pre_cluster_of_n1=pre_cluster_of[n1],
		      *pre_cluster_of_nn=pre_cluster_of[nn];
	    // warn("merging: " +(*sfg)[n1]->getState()->getName()+
	    //      " + "       +(*sfg)[nn]->getState()->getName() );
	    // warn("   reps: " +(*csfg)[n1_rep]->getState()->getName()+
	    //      " + "       +(*csfg)[nn_rep]->getState()->getName() );
	    // warn(string("    sfg: %p + %p",n1,nn));
	    // warn(string("   csfg: %p + %p",n1_rep,nn_rep));
	    // warn(string("   sets: %p + %p",
	    //      pre_cluster_of_n1,pre_cluster_of_nn));
	    (*pre_cluster_of_n1) += (*pre_cluster_of_nn);
	    node nnn;
	    forall (nnn,*pre_cluster_of_nn)
	      pre_cluster_of[nnn] = pre_cluster_of_n1;
	    pre_cluster_rep_of[pre_cluster_of_nn] = NULL;
	    delete pre_cluster_of_nn;
//	    merge_nodes(*csfg,n1_rep,nn_rep,&csfg_edge_weights,&csfg_state_areas);
	    merge_nodes(*csfg,n1_rep,nn_rep,
			&csfg_edge_weights,    &csfg_state_areas,
			&csfg_state_xfer_freqs,			// -ppmetis
			  (typeof(csfg_state_areas)*)NULL,	//  ''
			&csfg_df_bit_widths,			// -ppmetis
			  (typeof(csfg_state_areas)*)NULL);	//  ''
	  }
	}
      }  // forall (nn,*accessor_set)
    }  // forall (accessor_set,accessors)
    // warn("done merging pre-clusters");

    // - print pre-clusters  (debug)
    string msg="Pre-clusters for "+op->getName()+":\n";
    int pre_cluster_num=1;
    forall_nodes (csfg_n,*csfg) {
      msg += string("(%d): {",pre_cluster_num++);
      node n;
      forall (n,*pre_cluster_of[csfg_to_sfg_n[csfg_n]])
	msg += (*sfg)[n]->getState()->getName() + " ";
      msg += string("}  area=%d\n", csfg_state_areas[csfg_n]);
    }
    warn(msg);
    warn("Mixed, pre-clustered "+
	 sprintSFGedgeWeights(*csfg,&csfg_edge_weights));

    // - dump pre-clustered graph to Metis
    extern bool gPagePartitioningMetis;
    if (gPagePartitioningMetis) {
      string mfname = op->getName()+".metis";
      FILE *mfp=fopen(mfname,"w");
      if (mfp==NULL)
	warn("Could not create Metis file \""+mfname+"\"");
      else {
	print_metis(mfp,mfname,csfg,op,    &csfg_state_areas,
		    &csfg_state_xfer_freqs,&csfg_df_bit_widths,
		    sfg, &csfg_to_sfg_n,   &pre_cluster_of);
	fclose(mfp);
      }
      removeDFedgesFromSFG(*sfg);		// - bail out,
      continue;  // forall (op,*page_ops)	//     do not partition op
    }

    // - cluster states in csfg
    list<set<node> > csfg_clusters =
      clusterStates(*csfg,page_area_min,page_area_max,
		    &csfg_edge_weights,&csfg_state_areas);

    // - recover clusters in sfg from pre-clusters
    list<set<node> > clusters;		// - recover this, sfg nodes
    set<node> csfg_cluster;
    forall (csfg_cluster,csfg_clusters) {
      set<node> cluster;
      node csfg_n;
      forall (csfg_n,csfg_cluster) {
	node n;
	forall (n,*pre_cluster_of[csfg_to_sfg_n[csfg_n]])
	  cluster.insert(n);
      }
      clusters.append(cluster);
    }

    // - print clusters + statistics
    cout << "Clusters for " << op->getName() << ":\n";
    int clusternum=1;
    set<node> cluster;
    forall (cluster,clusters) {
      cout << "(" << clusternum++ << ") {";
      node s;
      int cluster_area=0, cluster_cut_freq=0, cluster_state_freq=0,
	  cluster_cut_df_streams=0, cluster_cut_df_bits=0;
      forall (s,cluster) {
	cout << (*sfg)[s]->getState()->getName() << " ";
	cluster_area+=state_areas[s];
	cluster_state_freq+=state_freqs[s];
	edge e;
	list<edge> sfg_out_edges_s=sfg->out_edges(s),
		   sfg_edges_s    =sfg->in_edges(s);
	sfg_edges_s.conc(sfg_out_edges_s);	// - incoming + outgoing edges
	forall (e,sfg_edges_s) {
	  if (!cluster.member(sfg->opposite(s,e))) {
	    if ((*sfg)[e])	// - SF edge
	      cluster_cut_freq+=state_xfer_freqs[e];
	    else {		// - DF edge
	      cluster_cut_df_bits+=df_bit_widths[e];
	      cluster_cut_df_streams++;
	    }
	  }
	}
      }  // forall (s,cluster) 

      // - report cluster statistics
      cout << "}  area=" << cluster_area
	   << " cut_freq/state_freq=" << cluster_cut_freq
	   << "/" << cluster_state_freq
	   << " cut_df_streams=" << cluster_cut_df_streams
	   << " cut_df_bits=" << cluster_cut_df_bits << '\n';

      // - update global statistics
      total_cluster_area	   += cluster_area;
      total_cluster_cut_df_streams += cluster_cut_df_streams;
      total_cluster_cut_df_bits    += cluster_cut_df_bits;
      total_cluster_cut_freq	   += cluster_cut_freq;
      total_cluster_state_freq     += cluster_state_freq;
      total_clusters++;
    }  // forall (cluster,clusters)
    cout << '\n';
    total_instances++;

    // - synthesize pages (1 per cluster),  replace op by pages
    if (clusters.size()>1) {
      // - partitionedOp := composition of pages (1 per cluster) of op
      OperatorCompose *partitionedOp =
	synthesizePartitionedOp(op,sfg,clusters,&accessors,&df_symbols);
			// - (this mangles op but not call to op in iop)
      // - replace op call by partitionedOp call, in iop composition
      CallInfo call_info(op,NULL);
      iop->map(find_call_map,(TreeMap)NULL,&call_info);
      ExprCall *opCallInIop = call_info.call;
      assert(opCallInIop);
      opCallInIop->setOp(partitionedOp);
    }

    // - remove dataflow edges from SFG  (restore for caller)
    removeDFedgesFromSFG(*sfg);

  }  // forall (op,*page_ops) 

  // - flatten composition  (lift pages from 1 level deep)
  iop=flatten1(iop);

  /*
  string msg = "*** RESULTS OF PARTITIONING " + iop->getName() + "\n" + *iop;
  Stmt *stmt;
  forall (stmt,*iop->getStmts())
    if (stmt->getStmtKind()==STMT_CALL)
      msg += "\n" + ((StmtCall*)stmt)->getCall()->getOp()->toString();
  warn(msg);
  */

  warn("--- done partitioning");

  return iop;
}


// -  g++ 2.91 (egcs 1.1.2) linker fails without these stubs,
//     presumably due to template bug from "list<set<node> >"
#if __LEDA__!=371
istream & operator>>(istream &i, set<node> &)       {return i;}
ostream & operator<<(ostream &o, set<node> const &) {return o;}
#endif
