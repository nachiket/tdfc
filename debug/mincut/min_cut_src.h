// min-cut, returns cut-set containing designated source node

#ifndef _TDF_MIN_CUT_SRC_
#define _TDF_MIN_CUT_SRC_

#include <LEDA/graph.h>

list<node> MIN_CUT_SRC (graph G, edge_array<int>& weight, node src);

#endif	/* _TDF_MIN_CUT_SRC_ */
