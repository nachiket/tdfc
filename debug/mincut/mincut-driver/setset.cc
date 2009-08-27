// set<set<node>*> works in LEDA 3.8, but not 4.0 or 4.2
// typedef-ing does not help

#include <LEDA/graph.h>
#include <LEDA/set.h>

typedef set<node> nodeset;

main () {
  set<set<node>*> s;
  // set<nodeset*> s;
}
