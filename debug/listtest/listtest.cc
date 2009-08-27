// - Create, modify, and print a simple graph
// - Intended to test storage order and traversal order of lists

#include <iostream.h>
#include <LEDA/list.h>
#include <LEDA/string.h>


void printList (const list<string> &l)
{
  cout <<   "forall:        ";
  string s;
  forall (s,l)
	cout << s << " ";

  cout << "\nforall_items:  ";
  list_item it;
  forall_items (it,l)
    cout << l.inf(it) << " ";

  cout << "\nfor (i=0...):  ";
  for (int i=0; i<l.size(); i++)
    cout << l.inf(l.get_item(i)) << " ";
  cout << '\n';
}

main ()
{
  list<string> l;
  l.append(string("C"));
  l.append(string("D"));
  l.push  (string("B"));
  l.push  (string("A"));
  printList(l);
}
