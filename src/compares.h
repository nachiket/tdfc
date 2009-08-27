#ifndef _TDF_COMPARES_
#define _TDF_COMPARES_

class Tree;
/////////////////////////////////////////////////////////////////
//  compare() for LEDA containers
namespace leda {
	inline int compare (Tree* const &a, Tree* const &b)
	{
		return a==b ? 0 : a<b ? -1 : 1;
	}
};

class Operator;
class OperatorBehavioral;
////////////////////////////////////////////////////////////////
//  compare() for LEDA containers
namespace leda {
	inline int compare (Operator* const &a, Operator* const &b)
	{
		  return a==b ? 0 : a<b ? -1 : 1;
	}
	inline int compare (OperatorBehavioral* const &a, OperatorBehavioral* const &b)
	{
		  return a==b ? 0 : a<b ? -1 : 1;
	}
};	

class Suite;
////////////////////////////////////////////////////////////////
//  compare() for LEDA containers
namespace leda {
	inline int compare (Suite* const &a, Suite* const &b)
	{
		  return a==b ? 0 : a<b ? -1 : 1;
	}

};

class Symbol;
class SymbolVar;
class SymTab;
////////////////////////////////////////////////////////////////
//  compare() for LEDA containers
namespace leda {
	inline int compare (SymTab* const &a, SymTab* const &b)
	{
		  return a==b ? 0 : a<b ? -1 : 1;
	}

	inline int compare (Symbol* const &a, Symbol* const &b)
	{
		  return a==b ? 0 : a<b ? -1 : 1;
	}
	inline int compare (SymbolVar* const &a, SymbolVar* const &b)
	{
		  return a==b ? 0 : a<b ? -1 : 1;
	}
};

class Expr;
////////////////////////////////////////////////////////////////
//  compare() for LEDA containers
namespace leda {
	inline int compare (Expr* const &a, Expr* const &b)
	{
		  return a==b ? 0 : a<b ? -1 : 1;
	}
};




class State;
class StateCase;
class InputSpec;
////////////////////////////////////////////////////////////////
//  compare() for LEDA containers
namespace leda {
	inline int compare (State* const &a, State* const &b)
	{
		  return a==b ? 0 : a<b ? -1 : 1;
	}

	inline int compare (StateCase* const &a, StateCase* const &b)
	{
		  return a==b ? 0 : a<b ? -1 : 1;
	}

	inline int compare (InputSpec* const &a, InputSpec* const &b)
	{
		  return a==b ? 0 : a<b ? -1 : 1;
	}
};


class SCCell;
////////////////////////////////////////////////////////////////
//  compare() for LEDA containers
namespace leda {
	inline int compare (SCCell *const &a, SCCell *const &b)
	{
		  return a==b ? 0 : a<b ? -1 : 1;
	}
};



class Stmt;
namespace leda {
inline int compare (Stmt* const &a, Stmt* const &b)
{
	  return a==b ? 0 : a<b ? -1 : 1;
}
};


class Type;
namespace leda {
inline int compare (Type* const &a, Type* const &b)
{
	  return a==b ? 0 : a<b ? -1 : 1;
}
};

#endif

