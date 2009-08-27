// - Create, copy, and print a simple graph
// - Intended to test ordering of adjacency lists in graph implementation


#include <iostream.h>
#include <LEDA/string.h>
#include <LEDA/graph.h>


void printGraphInfo (const GRAPH<string,string> &g)
{
  cout << "forall_nodes visits nodes:  ";
  node n;
  forall_nodes (n,g)
    cout << g[n] << ' ';
  cout << '\n';

  cout << "forall_edges visits edges:  ";
  edge e;
  forall_edges (e,g)
    cout << g[e] << ' ';
  cout << '\n';

  forall_nodes (n,g) {
    cout << "node " << g[n] << " outputs:  ";
    list<edge> outs=g.out_edges(n);
    edge e;
    forall (e,outs)
      cout << g[e] << " ";
    cout << "  (0=" << (outs.size()>0 ? g[outs.inf(outs.get_item(0))] : string("<nil>"));
    cout <<  ", 1=" << (outs.size()>1 ? g[outs.inf(outs.get_item(1))] : string("<nil>")) << ')';
    cout << '\n';
  }

  forall_nodes (n,g) {
    cout << "node " << g[n] << " inputs:  ";
    list<edge> ins=g.in_edges(n);
    edge e;
    forall (e,ins)
      cout << g[e] << " ";
    cout << "  (0=" << (ins.size()>0 ? g[ins.inf(ins.get_item(0))] : string("<nil>"));
    cout <<  ", 1=" << (ins.size()>1 ? g[ins.inf(ins.get_item(1))] : string("<nil>")) << ')';
    cout << '\n';
  }
}


main ()
{
  GRAPH<string,string> g;

  // - add nodes  r:root c1:child1 c2:child2
  node rr = g.new_node("rr");
  node c1 = g.new_node("c1");
  node c2 = g.new_node("c2");

  // - add edges
  edge rr_c1 = g.new_edge(rr,c1,"rr_c1");
  edge rr_c2 = g.new_edge(rr,c2,"rr_c2");
  edge c1_rr = g.new_edge(c1,rr,"c1_rr");
  edge c2_rr = g.new_edge(c2,rr,"c2_rr");
  edge c1_c2 = g.new_edge(c1,c2,"c1_c2");

  cout << "ORIGINAL GRAPH\n";
  printGraphInfo(g);

  GRAPH<string,string> gcopy(g);
  cout << "\nCOPY CONSTRUCTED GRAPH\n";
  printGraphInfo(gcopy);

  GRAPH<string,string> gcopycopy(gcopy);
  cout << "\nCOPY OF COPY CONSTRUCTED GRAPH\n";
  printGraphInfo(gcopycopy);

  GRAPH<string,string> gopeq;
  gopeq=g;
  cout << "\nASSIGNMENT OP GRAPH\n";
  printGraphInfo(gopeq);
}
