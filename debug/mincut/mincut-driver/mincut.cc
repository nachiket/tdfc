// Graph:	1-2-3
//		| | |
//		4-5-6
//		| | |
//		7-8-9

#include <iostream.h>
#include <LEDA/string.h>
#include <LEDA/graph.h>
#include <LEDA/min_cut.h>
#include "../../../src/clusterstates.cc"
#include "../../../src/mincut.cc"

void collapse_nodes (graph& G, list<node> nodes)
{
  // collapse all v \in nodes into first d \in nodes
  node v,d=NULL;
  forall (v,nodes)
    if (d==NULL)
      d=v;
    else
      G.merge_nodes(d,v);
    
}


main ()
{
  GRAPH<string,string> G;
  node v;
  edge e;

  ////////  Build Graph  ////////

  node n1 = G.new_node(string("n1"));
  node n2 = G.new_node(string("n2"));
  node n3 = G.new_node(string("n3"));
  node n4 = G.new_node(string("n4"));
  node n5 = G.new_node(string("n5"));
  node n6 = G.new_node(string("n6"));
  node n7 = G.new_node(string("n7"));
  node n8 = G.new_node(string("n8"));
  node n9 = G.new_node(string("n9"));
  edge e12 = G.new_edge(n1,n2,string("e12"));
  edge e23 = G.new_edge(n2,n3,string("e23"));
  edge e14 = G.new_edge(n1,n4,string("e14"));
  edge e25 = G.new_edge(n2,n5,string("e25"));
  edge e36 = G.new_edge(n3,n6,string("e36"));
  edge e45 = G.new_edge(n4,n5,string("e45"));
  edge e56 = G.new_edge(n5,n6,string("e56"));
  edge e47 = G.new_edge(n4,n7,string("e47"));
  edge e58 = G.new_edge(n5,n8,string("e58"));
  edge e69 = G.new_edge(n6,n9,string("e69"));
  edge e78 = G.new_edge(n7,n8,string("e78"));
  edge e89 = G.new_edge(n8,n9,string("e89"));

  /*
  edge e28 = G.new_edge(n2,n8,string("e28"));
  edge e59 = G.new_edge(n5,n9,string("e59"));
  edge e39 = G.new_edge(n3,n9,string("e39"));
  edge e48 = G.new_edge(n4,n8,string("e48"));
  */

  cout << "======== Original Graph ========\n";
  G.write();
  cout << "\n";

  ////////  Weights  ////////

  edge_array<int> w(G,100);
  w[e12] = 10;
  w[e25] = 10;
  w[e36] = 20;
  w[e56] = 10;
  w[e89] = 30;
  w[e58] = 14;
  w[e78] = 15;

  cout << "======== Weights ========\n";
  forall_edges (e,G)
    cout << "w[" << G[e] << "] = " << w[e] << '\n';
  cout << '\n';

  ////////  Shortest Path  ////////

  node_array<edge> path;
  node src=n1, dst=n9;
  shortestPathUnitWeight(G,w,n1,n9,path);

  cout << "======== Shortest Path ========\n";
  cout << "src: ";  printNode(G,src);
  cout << "dst: ";  printNode(G,dst);
  printPathArray(G,path);
  printPath(G,path,src,dst);
  cout << '\n';

  ////////  Min Cut  ////////

  // list<node> cutset = MIN_CUT(G,w);
  // list<node> cutset = min_cut_src(G,w,n1);

  /*
  node_array<bool> cutset_array;
  minCutMaxFlow_inParts(G,w,src,dst,cutset_array);

  list<node> cutset;
  forall_nodes (v,G)
    if (cutset_array[v])
      cutset.append(v);

  list<node> negset;
  forall_nodes (v,G)
    if (!cutset.search(v))
      negset.append(v);

  list<node> negnegset;
  forall_nodes (v,G)
    if (!negset.search(v))
      negnegset.append(v);

  cout << "======== Cut Set ========\n";
  forall(v,cutset)
    cout << G[v] << '\n';
  cout << '\n';

  cout << "======== Negative Cut Set ========\n";
  forall(v,negset)
    cout << G[v] << '\n';
  cout << '\n';

  cout << "======== Negative Negative Cut Set ========\n";
  forall(v,negnegset)
    cout << G[v] << '\n';
  cout << '\n';
  */

  ////////  Collapse Cut Set  ////////

  /*
  list<node> collapse;
  collapse.append(n2);
  collapse.append(n3);
  collapse.append(n5);
  collapse_nodes(G,collapse);
  */
  /*
  collapse_nodes(G,cutset);

  cout << "======== Collapsed Graph ========\n";
  G.write();
  cout << '\n';
  */

  ////////  AREA-CONSTRAINED MIN-CUT  ////////

  int page_area=4;
  int epsilon=1;
  list<set<node> > clusters = clusterStates(G,page_area,epsilon,&w);

  cout << "======== Area-constrained partitioning ========\n";
  set<node> cluster;
  forall (cluster,clusters) {
    cout << "cluster {";
    forall (v,cluster)
      cout << G[v] << ' ';
    cout << "}\n";
  }
  cout << '\n';
}
