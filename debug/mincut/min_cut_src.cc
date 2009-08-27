// min-cut, returns cut-set containing designated source node

#include <LEDA/graph.h>
#include <LEDA/min_cut.h>

list<node> min_cut_src (graph& G, edge_array<int>& weight, node src)
{
  list<node> cutset=MIN_CUT(G,weight);
  if (cutset.search(src))
    return cutset;
  else {
    list<node> negset;
    node v;
    forall_nodes (v,G) {
      if (!cutset.search(v))
	negset.append(v);
    }
    return negset;
  }
}
