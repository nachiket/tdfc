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
// SCORE TDF compiler:  expressions
// $Revision: 1.152 $
//
//////////////////////////////////////////////////////////////////////////////


#include <assert.h>
#include <stdio.h>
#include "parse.h"
#include "expr.h"
#include "symbol.h"
#include "suite.h"
#include "operator.h"
#include "state.h"
#include "stmt.h"
#include "ops.h"
#include "misc.h"
#include "annotes.h"
#include "bindvalues.h"
#include "gc.h"

using leda::list_item;

////////////////////////////////////////////////////////////////
//  pre-defined expressions

const ExprValue *expr_0 = NULL;		// the constant "0"
const ExprValue *expr_1 = NULL;		// the constant "1"


////////////////////////////////////////////////////////////////
//  init

void Expr::init ()
{
  expr_0 = new ExprValue(NULL, new Type(TYPE_INT,0,false), 0, 0);
  expr_1 = new ExprValue(NULL, new Type(TYPE_INT,1,false), 1, 0);

  TreeGC::addRoot(expr_0);
  TreeGC::addRoot(expr_1);
}


////////////////////////////////////////////////////////////////
//  constructors
//   - no type-checking done here!

Expr::Expr (Token *token_i, ExprKind exprKind_i, Type *type_i)
  : Tree(token_i,TREE_EXPR),
    exprKind(exprKind_i),
    type(type_i)
{
  if (type)
    type->setParent(this);

  // - WARNING:  ExprBitSel (for blockdfg.cc) is the only expr with NULL type;
  //             everyone else has real type  (EC 3/20/02)
}


ExprValue::ExprValue (Token *token_i, Type *type_i,
		      long long intVal_i, long long fracVal_i)
  : Expr(token_i,EXPR_VALUE,type_i),
    intVal(intVal_i),
    fracVal(fracVal_i)
{
  assert(type->getTypeKind()!=TYPE_NONE);
  assert(type->getTypeKind()!=TYPE_ANY);
}


ExprValue::ExprValue (Token *token_i, Type *type_i, float floatVal_i)
  : Expr(token_i,EXPR_VALUE,type_i),
    floatVal(floatVal_i)
{
  assert(type->getTypeKind()==TYPE_FLOAT);
}


ExprValue::ExprValue (Token *token_i, Type *type_i, double doubleVal_i)
  : Expr(token_i,EXPR_VALUE,type_i),
    doubleVal(doubleVal_i)
{
  assert(type->getTypeKind()==TYPE_DOUBLE);
}

/**
 * Added by Nachiket on 12/27/2009 to support nextstate values
 */
ExprValue::ExprValue (Token *token_i, string state_i)
  : Expr(token_i,EXPR_VALUE,new Type(TYPE_STATE)),
    stateVal(state_i)
{

}



ExprLValue::ExprLValue (Token *token_i, Symbol *sym_i,
			Expr  *posLow_i, Expr *posHigh_i,
			Expr  *retime_i, Expr *arrayLoc_i)
  : Expr(token_i,
	 EXPR_LVALUE,
	 sym_i ? (Type*)sym_i->getType()->duplicate() : type_any),
    sym(sym_i),
    posLow(posLow_i),
    posHigh(posHigh_i),
    retime(retime_i),
    arrayLoc(arrayLoc_i),
    allBits(posLow_i==NULL && posHigh_i==NULL)
{
  assert(sym);
  assert(sym->getType()->getTypeKind()!=TYPE_NONE);	// got no widthExpr

  if(sym->getType()->getTypeKind()==TYPE_STATE) {
	  type = new Type(TYPE_STATE);
  }

  if (posLow==NULL && posHigh==NULL)
  {
    // ID : full bit range
    // type	= (Type*)sym->getType()->duplicate();	// this is default
    posLow	= (Expr*)expr_0->duplicate();
    posHigh	= (type->getWidth()==0) ? constIntExpr(0)
		: (type->getWidth()>0)  ? constIntExpr(type->getWidth()-1)
		: (Expr*)new ExprBop(NULL,'-',type->makeWidthExpr(),
				     (Expr*)expr_1->duplicate());
  }
  else if (posLow!=NULL && posHigh==NULL)
  {
    // ID[a] : single bit
    type    = (Type*)type_uint1->duplicate();
    posHigh = (Expr*)posLow->duplicate();
  }
  else if (posLow!=NULL && posHigh!=NULL)
  {
    // ID[a:b] : bit range fully specified
    type=new Type(TYPE_INT,
		  new ExprBop(NULL, '-',
			      new ExprBop(NULL,'+',
					  (Expr*)posHigh->duplicate(),
					  (Expr*)expr_1->duplicate()),
			      (Expr*)posLow->duplicate()),
		  false);
  }
  else  // (posLow==NULL && posHigh!=NULL)
    assert(!"internal inconsistency");
  type   ->setParent(this);
  posLow ->setParent(this);
  posHigh->setParent(this);

  if ( sym->getSymKind()==SYMBOL_STREAM  &&
       ((SymbolStream*)sym)->getDir()!=STREAM_OUT  )	// input/any stream
  {
    if (retime==NULL)
      retime=(Expr*)expr_0->duplicate();
  }
  else  // (not input/any stream)
  {
    if (retime!=NULL)
      fatal(1, string("retime operator @ permitted only on input stream"),
	    token);
  }
  if (retime)	retime  ->setParent(this);
  if (arrayLoc) arrayLoc->setParent(this);
 
  // non-NULL retime only for input-stream LValue
}


ExprCall::ExprCall (Token *token_i, list<Expr*> *args_i, Operator *op_i)
  : Expr(token_i,
	 EXPR_CALL,
	 op_i ? ((Type*)op_i->getType()->duplicate()) : type_any),
    op(op_i),
    args(args_i)
{
  // NULL op is unlinked, op name in token, type TYPE_ANY
  if (op==NULL)
    assert(token);
  Expr *e;
  forall (e,*args)
    e->setParent(this);
}


ExprBuiltin::ExprBuiltin (Token *token_i, list<Expr*> *args_i,
			  OperatorBuiltin *builtin_i)
  : ExprCall(token_i,args_i,builtin_i)
{
  exprKind=EXPR_BUILTIN;	// fix after ExprCall()
  assert(op);
}


ExprCond::ExprCond (Token *token_i,    Expr *cond_i,
		    Expr  *thenPart_i, Expr *elsePart_i)
  : Expr(token_i, EXPR_COND, (Type*)thenPart_i->getType()->duplicate()),
    cond(cond_i),
    thenPart(thenPart_i),
    elsePart(elsePart_i)
{
  // we do not type-match thenPart and elsePart here
  cond    ->setParent(this);
  thenPart->setParent(this);
  elsePart->setParent(this);
}

// added NULL checks on 31st Oct 2009 for processing nested IFs..
ExprCond::ExprCond (Token *token_i,    Expr *cond_i, Type *type_i)
  : Expr(token_i, EXPR_COND, type_i),
    cond(cond_i),
    thenPart(NULL),
    elsePart(NULL)
{
  // we do not type-match thenPart and elsePart here
  cond    ->setParent(this);
}


ExprBop::ExprBop (Token *token_i, int bop_i, Expr *e1_i, Expr *e2_i)
  : Expr(token_i, EXPR_BOP, type_any),
    bop(bop_i),
    e1(e1_i),
    e2(e2_i)    
{
  e1->setParent(this);
  e2->setParent(this);
}


ExprUop::ExprUop (Token *token_i, int uop_i, Expr *e_i)
  : Expr(token_i, EXPR_UOP, type_any),
    uop(uop_i),
    e(e_i)
{
  e->setParent(this);
}


ExprCast::ExprCast (Token *token_i, Type *type_i, Expr *e_i)
  : Expr(token_i,EXPR_CAST,type_i), e(e_i)
{
  // assert(type);	// done in Expr::Expr
  e->setParent(this);
}


ExprCast::ExprCast (Token *token_i, bool sign_i, Expr *e_i)
  : Expr(token_i,EXPR_CAST,e_i->getType()->upgradeSign(sign_i)),
    e(e_i)
{
  e->setParent(this);
}


ExprArray::ExprArray (Token *token_i, TypeArray *type_i,
		      list<Expr*> *exprs_i)
  : Expr(token_i,EXPR_ARRAY,type_i),
    exprs(exprs_i ? exprs_i : new list<Expr*>)
{
  Expr *e;
  forall (e,*exprs)
    e->setParent(this);
}


////////////////////////////////////////////////////////////////
//  thread

void ExprValue::thread (Tree *p)
{
  setParent(p);
  if (type)
    type->thread(this);
}


void ExprLValue::thread (Tree *p)
{
  setParent(p);
  if (type)	type->thread(this);
  if (posLow)	posLow  ->thread(this);
  if (posHigh)	posHigh ->thread(this);
  if (retime)	retime  ->thread(this);
  if (arrayLoc) arrayLoc->thread(this);
  // do NOT thread symbol (done thru symtab)
}


void ExprCall::thread (Tree *p)
{
  setParent(p);
  if (type)
    type->thread(this);
  Expr *e;
  forall (e,*args)
    e->thread(this);
}


// void ExprBuiltin::thread (Tree *p)
//  --> ExprCall::thread


void ExprCond::thread (Tree *p)
{
  setParent(p);
  if (type)
    type->thread(this);
  cond->thread(this);
  thenPart->thread(this);
  elsePart->thread(this);
}


void ExprBop::thread (Tree *p)
{
  setParent(p);
  if (type)
    type->thread(this);
  e1->thread(this);
  e2->thread(this);
}


void ExprUop::thread (Tree *p)
{
  setParent(p);
  if (type)
    type->thread(this);
  e->thread(this);
}


void ExprCast::thread (Tree *p)
{
  setParent(p);
  if (type)
    type->thread(this);
  e->thread(this);
}


void ExprArray::thread (Tree *p)
{
  setParent(p);
  if (type)
    type->thread(this);
  Expr *e;
  forall (e,*exprs)
    e->thread(this);
}


////////////////////////////////////////////////////////////////
//  link

bool ExprValue::link ()
{
  return type->link();
}


bool ExprLValue::link ()
{
  // - first, find symbol in scope
  //     * normally, the symbol has already been found during parse
  //     * the exception is a param appearing in the width expression of an
  //       operator return type, where the param is used before being parsed
  //       in the formals list (leaves unlinked symbol ref)
  //     * also, if a symbol is replaced (e.g. during operator duplication),
  //       we must find the new one by name (relink)
  SymTab *symtab = getScope();
  if (symtab)
  {
    Symbol *visibleSym = symtab->lookup(sym->getName());
    if (visibleSym)
      sym=visibleSym;
    else
    {
      Operator	*op_lval =      getOperator(),
		*op_sym  = sym->getOperator();
      fatal(1, string("symbol \"")+sym->getName()+"\" not found"+
	       ", in lvalue from operator \""+
	       (op_lval?op_lval->getName():string("<none>"))+
	       string("\" (0x%p), ",op_lval)+
	       // "\n"+op_lval->toString()+
	       "symbol from operator \""+
	       (op_sym?op_sym->getName():string("<none>"))+
	       string("\" (0x%p)",op_sym),
	       // "\n"+op_sym->toString(),
	    token);
    }
  }

  Operator *op=getOperator();
  if ( (!op || op->getOpKind()!=OP_BEHAVIORAL) &&
       retime &&
       !retime->equals((Expr*)expr_0) )
    fatal(1,"retime operator @ permitted only in behavioral operator",token);

  bool linked=true;
  if (type)
    linked = linked && type->link();
  if (posLow)
    linked = linked && posLow->link();
  if (posHigh)
    linked = linked && posHigh->link();
  if (retime)
    linked = linked && retime->link();
  if (arrayLoc)
    linked = linked && arrayLoc->link();
  // do NOT relink target symbol
  return linked;
}


static
bool betaRenameInCall_map (Tree **h, void *i)
{
  // - map over a locally-scoped tree (e.g. over ExprCall's return-type)
  //   to replace (beta-rename) param formals (e.g. in widthExpr) by actuals
  // - void *i = ExprCall

  ExprCall *call         = (ExprCall*)i;
  Operator *calledOp     = call->getOp();
  SymTab   *calledSymTab = calledOp->getSymtab();

  if ((*h)->getKind()==TREE_EXPR && (*(Expr**)h)->getExprKind()==EXPR_LVALUE)
  {
    ExprLValue *lval=*(ExprLValue**)h;
    // - paranoid: lookup symbol in called operator's environment,
    //             in case called operator is not linked
    Symbol *calledSym = calledSymTab->lookup(lval->getSymbol()->getName());
    if (calledSym==NULL)
    {
      error(1, string("while calling operator \"")+calledOp->getName()+"\"",
	    call->getToken());
      fatal(1, string("symbol \"")+lval->getSymbol()->getName()+
	       "\" (from return type?) not found in called operator",
	    lval->getToken());
    }
    int pos1=calledOp->getArgs()->rank(calledSym);    // (arg position+1) or 0
    if (pos1==0)
    {
      error(1, string("while calling operator \"")+calledOp->getName()+"\"",
	    call->getToken());
      fatal(1, string("symbol \"")+calledSym->getName()+
	       "\" (from return type?) not found in called operator's formals",
	    lval->getToken());
    }
    list_item it = call->getArgs()->get_item(pos1-1);
    if (it==NULL)
      fatal(1, string("too few arguments in call to \"")+
	       calledOp->getName(), call->getToken());
    *h = (Expr*)call->getArgs()->inf(it)->duplicate();
    return 0;
  }
  /*
  else if ((*h)->getKind()==TREE_TYPE && (*(Type**)h)->getWidth()>=0)
    // - do not descend into constant-width widthExpr's
    return 0;
  */
  else
    return 1;
}

Tree* betaRenameInCall (Tree *renameMe, ExprCall *call)
{
  renameMe->map2(&renameMe,betaRenameInCall_map,NULL,call);
  return renameMe;
}


bool ExprCall::link ()
{
  Tree *suite;
  for (suite=getOperator();
       suite && suite->getKind()!=TREE_SUITE;
       suite=suite->getParent());
  if (suite)
  {
    // - resolve operator by name within a suite (if found enclosing suite)
    // - prefer existing operator's name, else use ExprCall's token name
    if (op==NULL && token==NULL)
      fatal(1,"internal error: cannot bind call, no operator or token",token);
    string opName = op?op->getName():token->str;
    Operator *resolvedOp=((Suite*)suite)->lookupOperator(opName);
    if (resolvedOp==NULL)
      fatal(1, string("stupid operator ")+opName+" not found", token);
    else
      op=resolvedOp;
  }
  else if (op==NULL)
  {
    // - no suite in which to resolve operator name,
    //     so operator better already be known (fatal if not)
    fatal(1,"internal error: cannot bind call, no operator or suite",token);
  }

  /*
  // - replace (beta-rename) symbols inside return-type to actuals
  // - HACK:  must rename only if ret-type is in called operator's env,
  //          so we duplicate op's type and rename fresh type (memory leak)
  //          - better solution is to mark type as having been renamed?
  //          - deleting present Type is bad due to pre-link type-sharing
  type=(Type*)op->getType()->duplicate();
  betaRenameInCall(type,this);
  type->setParent(this);
  */
  type=NULL;	// generate type on-demand

  bool linked=true;
  if (type)
    linked = linked && type->link();
  Expr *e;
  forall (e,*args)
    linked = linked && e->link();
  return linked;
}


bool ExprBuiltin::link ()
{
  bool linked=true;
  if (type)
    linked = linked && type->link();
  Expr *e;
  forall (e,*args)
    linked = linked && e->link();

  BuiltinKind builtinKind=((OperatorBuiltin*)op)->getBuiltinKind();
  if (builtinKind==BUILTIN_SEGMENT)
  {
    if (op->getArgs()->empty())
    {
      // - unlinked segment operator call -- create OperatorSegment here
      // - requires retrieving type of data stream from call args
      SegmentKind segKind=((OperatorSegment*)op)->getSegmentKind();
      d_array<SegmentKind,int> segKindToDataArgPos;
      segKindToDataArgPos[SEGMENT_R]		= SEGMENT_R_DATA;
      segKindToDataArgPos[SEGMENT_W]		= SEGMENT_W_DATA;
      segKindToDataArgPos[SEGMENT_RW]		= SEGMENT_RW_DATA_W;
      segKindToDataArgPos[SEGMENT_SEQ_R]	= SEGMENT_SEQ_R_DATA;
      segKindToDataArgPos[SEGMENT_SEQ_W]	= SEGMENT_SEQ_W_DATA;
      segKindToDataArgPos[SEGMENT_SEQ_RW]	= SEGMENT_SEQ_RW_DATA_W;
      int dataArgPos=segKindToDataArgPos[segKind];
      if (args->size()<dataArgPos)
	fatal(1,string("too few args to ")+op->getName(),token);
      Type *dataType=args->inf(args->get_item(dataArgPos))->getType();
      OperatorSegment *segOp=makeOperatorSegment(op->getToken(),segKind,
						 (Type*)dataType->duplicate());
      delete op;	// - was a dummy op
      op=segOp;
    }

    Operator *o=getOperator();
    if (o && o->getOpKind()!=OP_COMPOSE)
      fatal(1,string("segment operator \"")+op->getName()+
	      "\" permitted only in compositional operator", token);
  }
  else if (builtinKind==BUILTIN_COPY)
  {
    Operator *o=getOperator();
    if (o && o->getOpKind()!=OP_COMPOSE)
      fatal(1,string("copy operator \"")+op->getName()+
	      "\" supported only in compositional operator "+
	      "(I was lazy; use assignment in behavioral op)", token);
      // - exlining copy() presently fails because ExprBuiltin for copy()
      //     uses the generic operator_copy with generic args; must either:
      //     (1) instantiate type-specific copy operators, as for segments, or
      //     (2) special case copy() in exlining code (yuck)
      // - quick fix: disallow copy() in behavioral ops, so no need to exline
  }

  return linked;
}


bool ExprCond::link ()
{
  // - inappropriate for type width exprs
  // Operator *op=getOperator();
  // if (!op || op->getOpKind()!=OP_BEHAVIORAL)
  //   fatal(1,"operator ?: permitted only in behavioral operator",token);

  bool linked=true;
  if (type)
    linked = linked && type->link();
  linked = linked && cond->link() && thenPart->link() && elsePart->link();
  return linked;
}


bool ExprBop::link ()
{
  // - inappropriate for type width exprs
  // Operator *op=getOperator();
  // if ( (!op || op->getOpKind()!=OP_BEHAVIORAL) && !okInComposeOp() )
  //   fatal(1, string("binary operator ")+(token?token->str:string("%c",bop))+
  // 	     string(" permitted only in behavioral operator"), token);

  bool linked=true;
  if (type)
    linked = linked && type->link();
  linked = linked && e1->link() && e2->link();
  return linked;
}


bool ExprUop::link ()
{
  // - inappropriate for type width exprs
  // Operator *op=getOperator();
  // if (!op || op->getOpKind()!=OP_BEHAVIORAL)
  //   fatal(1, string("unary operator ")+(token?token->str:string(uop))+
  //	     string(" permitted only in behavioral operator"), token);

  bool linked=true;
  if (type)
    linked = linked && type->link();
  linked = linked && e->link();
  return linked;
}


bool ExprCast::link ()
{
  bool linked=true;
  if (type)
    linked = linked && type->link();
  linked = linked && e->link();
  return linked;
}


bool ExprArray::link ()
{
  bool linked=true;
  if (type)
    linked = linked && type->link();
  Expr *e;
  forall (e,*exprs)
    linked = linked && e->link();
  return linked;
}


////////////////////////////////////////////////////////////////
//  typeCheck


// - assist Type->typeCheck(); really necessary?
//
#define badType(t)	(t==NULL)


Type* ExprValue::typeCheck ()
{
  type=type->typeCheck();
  if (badType(type))
    fatal(1,"incomplete/invalid type in value",token);
  type->setParent(this);

  if (type->getTypeKind()==TYPE_INT) {
    // - For integers, check if type has enough bits
    if (intVal<0 && !type->isSigned())
      fatal(1, "Negative immediate " + toString() + 
	       " has unsigned type " + type->toString(), token);
    int width = type->getWidth();
    if (width>0) {
      int minWidth = countBits(intVal<0 ? -intVal : intVal)
		            + (type->isSigned() ? 1 : 0);
      if (width<minWidth)
	fatal(1, "Immediate " + toString() +
		 string(" needs %d bits but is typed with %d bits",
						     minWidth,width), token);
    }
  }

  return type;
}


Type* ExprLValue::typeCheck ()
{
  // - parameter symbol may have nil bit positions!

  if ( !isRValue() )
  {
    // - disallow reading non-rvalues (namely output streams)
    if (   !(   parent->getKind()==TREE_STMT
	     && ((Stmt*)parent)->getStmtKind()==STMT_ASSIGN
	     && ((StmtAssign*)parent)->getLValue()==this   )
	     // - output stream is lvalue of assignment
	&& !(   parent->getKind()==TREE_EXPR
	     && (((Expr*)parent)->getExprKind()==EXPR_CALL ||
	         ((Expr*)parent)->getExprKind()==EXPR_BUILTIN) )
	     // - output stream is arg to call or builtin, type-checked there
        )
      fatal(1, string("cannot use output stream \"")+sym->getName()+
	       "\" at that point", token);
  }

  Type *typeSym, *typePosLow, *typePosHigh, *typeRetime, *typeArrayLoc;
  {
    // - check sym
    typeSym = sym->typeCheck();
    if ( badType(typeSym) && !sym->isParam() )
      fatal(1, "incomplete/invalid type in expression", token);
  }
  if (posLow)
  {
    // - check posLow
    typePosLow=posLow->typeCheck();
    if (typePosLow->getTypeKind()!=TYPE_INT)
      fatal(1, "non-integer type in low bit subscript", posLow->getToken());
    // warning: should reject or produce predicates for signed, dynamic posLow
  }
  if (posHigh && posHigh!=posLow)
  {
    // - check posHigh
    typePosHigh=posHigh->typeCheck();
    if (typePosHigh->getTypeKind()!=TYPE_INT)
      fatal(1, "non-integer type in high bit subscript", posHigh->getToken());
    // warning: should reject or produce predicates for signed, dynamic posHigh
  }
  if (retime)
  {
    // - check retime
    typeRetime=retime->typeCheck();
    if (typeRetime->getTypeKind()!=TYPE_INT)
      fatal(1, "non-integer type in '@' stream retiming", retime->getToken());
    if (retime->getExprKind()==EXPR_VALUE &&
	typeRetime->isSigned()            &&
	((ExprValue*)retime)->getIntVal()<0 )
      fatal(1, "illegal input retiming value (negative)", retime->getToken());
    // warning: should reject or produce predicates for signed, dynamic retime
  }
  if (arrayLoc)
  {
    // - check array location
    typeArrayLoc=arrayLoc->typeCheck();
    if (typeArrayLoc->getTypeKind()!=TYPE_INT)
      fatal(1, "non-integer type in array subscript", arrayLoc->getToken());
    if (typeSym->getTypeKind()!=TYPE_ARRAY)
      fatal(1, string("cannot access array element of non-array variable \"")+
	       sym->getName()+"\"", arrayLoc->getToken());
    if (arrayLoc->getExprKind()==EXPR_VALUE) {
      int loc = ((ExprValue*)arrayLoc)->getIntVal();
      if (typeArrayLoc->isSigned() && loc<0)
	fatal(1, string("array subscript %d out of bounds (negative)",loc),
	      arrayLoc->getToken());
//    ExprArray *arrayValue = (ExprArray*)((SymbolVar*)sym)->getValue();
      ExprArray *arrayValue = chaseArrayValue(sym);
      if (arrayValue && (loc >= arrayValue->getExprs()->size()))
	  fatal(1, string("array subscript %d out of bounds (too large)",loc),
		arrayLoc->getToken());
    }
    // warning: should reject or produce predicates for signed, dynamic retime
  }

  Type *typeRef = (Type*)
    		  (arrayLoc ? ((TypeArray*)typeSym)->getElemType() : typeSym)
		  ->duplicate();

  if (allBits)				// implicit full bit-range
  {
    type=typeRef;
    type->setParent(this);
    return type;
  }
  else if (posLow==posHigh || posLow->equals(posHigh))	    // single-bit
  {
    if (typeRef->getTypeKind()!=TYPE_INT || typeSym->isSigned())
      fatal(1, string("incompatible type for bit selection, symbol is ")+
	       typeRef->toString()+", should be unsigned integer", token);
    if (posLow->getExprKind()==EXPR_VALUE)	// - const bit position
    {
      if (((ExprValue*)posLow)->getIntVal()<0)
	fatal(1,"bit subscript out-of-bounds (negative)",
	      posLow->getToken());
      if (typeRef->getWidth()>=0)
      {
	if (((ExprValue*)posLow)->getIntVal() >= typeRef->getWidth())
	  fatal(1,"bit subscript out-of-bounds (too large)",
		posLow->getToken());
	type=(Type*)type_uint1->duplicate();
	type->setParent(this);
	return type;
      }
      else
      {
	Expr *predExpr=new ExprBop(NULL,'<',(Expr*)posLow->duplicate(),
				   typeRef->makeWidthExpr());
	type=(new Type(TYPE_INT,1,false,predExpr)) -> typeCheck();
	if (badType(type))
	  assert(!"internal inconsistency");
	type->setParent(this);
	return type;
      }
    }
    else					// - non-const bit position
    {
      Expr *predExpr1=new ExprBop(NULL,GTE,(Expr*)posLow->duplicate(),
				  (Expr*)expr_0->duplicate());
      Expr *predExpr2=new ExprBop(NULL,'<',(Expr*)posLow->duplicate(),
				  typeRef->makeWidthExpr());
      Expr *predExpr =new ExprBop(NULL,LOGIC_AND,predExpr1,predExpr2);
      type=(new Type(TYPE_INT,1,false,predExpr)) -> typeCheck();
      if (badType(type))
	assert(!"internal inconsistency");
      type->setParent(this);
      return type;
    }
    type=(Type*)type_uint1->duplicate();
    type->setParent(this);
    return type;
  }
  else					// arbitrary bit-range
  {
    if (typeRef->getTypeKind()!=TYPE_INT || typeRef->isSigned())
      fatal(1, string("incompatible type for bit-range selection, symbol is ")+
	       typeRef->toString()+", should be unsigned integer", token);
    if (    posLow->getExprKind()==EXPR_VALUE
	&& posHigh->getExprKind()==EXPR_VALUE)	// - const bit range
    {
      if (((ExprValue*)posLow)->getIntVal() < 0)
	fatal(1,"bit-range subscript out-of-bounds (negative)",
	      posLow->getToken());
      if (((ExprValue*)posLow)->getIntVal() >
	  ((ExprValue*)posHigh)->getIntVal() )
	fatal(1,"bit-range subscripts incorrectly ordered",token);
      if (typeRef->getWidth()>=0)
      {
	if (((ExprValue*)posHigh)->getIntVal() >= typeRef->getWidth())
	  fatal(1,"bit-range subscript out-of-bounds (too high)",
		posHigh->getToken());
	type=(new Type(TYPE_INT, 1+((ExprValue*)posHigh)->getIntVal()-
				   ((ExprValue*)posLow)->getIntVal(),
		       false)) -> typeCheck();
	type->setParent(this);
	return type;
      }
      else
      {
	Expr *predExpr=new ExprBop(NULL,'<',(Expr*)posHigh->duplicate(),
				   typeRef->makeWidthExpr());
	type=(new Type(TYPE_INT, 1+((ExprValue*)posHigh)->getIntVal()-
				   ((ExprValue*)posLow)->getIntVal(),
		      false, predExpr)) -> typeCheck();
	if (badType(type))
	  assert(!"internal inconsistency");
	type->setParent(this);
	return type;
      }
    }
    else				// - non-const bit range
    {
      Expr *widthExpr=new ExprBop(NULL,'-',
				  new ExprBop(NULL,'+',
					      (Expr*)posHigh->duplicate(),
					      (Expr*)expr_1->duplicate()),
				  (Expr*)posLow->duplicate());
      Expr *predExpr1=new ExprBop(NULL,'<',(Expr*)posHigh->duplicate(),
				  typeRef->makeWidthExpr());
      Expr *predExpr2=new ExprBop(NULL,LTE,(Expr*)posLow->duplicate(),
				  (Expr*)posHigh->duplicate());
      Expr *predExpr3=new ExprBop(NULL,GTE,(Expr*)posLow->duplicate(),
				  (Expr*)expr_0->duplicate());
      Expr *predExpr =new ExprBop(NULL,LOGIC_AND,
				  new ExprBop(NULL,LOGIC_AND,
					      predExpr1,predExpr2), predExpr3);
      type=(new Type(TYPE_INT,widthExpr,false,predExpr)) -> typeCheck();
      if (badType(type))
	assert(!"internal inconsistency");
      type->setParent(this);
      return type;
    }
  }
}


Type* ExprCall::typeCheck ()
{
  list<Symbol*> *formals=op->getArgs();
  list<Expr*>   *actuals=args;
  list_item      formal, actual;
  int argNum;
  for (formal=formals->first(), actual=actuals->first(), argNum=1;
       formal && actual;
       formal=formals->succ(formal), actual=actuals->succ(actual), argNum++)
  {
    // - check for stream-direction mismatch:
    Symbol *formalSym =formals->inf(formal);
    Expr   *actualExpr=actuals->inf(actual);
    if ( formalSym->getSymKind()==SYMBOL_STREAM
	 && ((SymbolStream*)formalSym)->getDir()==STREAM_OUT)
    {
      // - formal is an output, requires lvalue actual
      if (!actualExpr->isLValue())
	fatal(1, string("argument %d of call to \"",argNum)+op->getName()+
	         "\" must be an L-value (assignable)",
	      actualExpr->getToken());
    }
    else
    {
      // - formal is an input/param, requires rvalue actual
      if (!actualExpr->isRValue())
	fatal(1, string("argument %d of call to \"",argNum)+op->getName()+
	         "\" must be an R-value (valued)",
	      actualExpr->getToken());	
    }

    // - check for type mismatch:
    Type *formalType = formalSym ->typeCheck();
    Type *actualType = actualExpr->typeCheck();
    bool formalSymIsOutput =(formalSym->getSymKind()==SYMBOL_STREAM &&
			     ((SymbolStream*)formalSym)->getDir()==STREAM_OUT);
    Type *lvalueType = formalSymIsOutput ? actualType : formalType;
    Type *rvalueType = formalSymIsOutput ? formalType : actualType;
//  if (!actualType->upgradable(formalType))
//  {
//    if (   actualType->getTypeKind()==formalType->getTypeKind()
//	  && actualType->isSigned()   ==formalType->isSigned() )
    if (!rvalueType->upgradable(lvalueType))
    {
      if (   rvalueType->getTypeKind()==lvalueType->getTypeKind()
	  && rvalueType->isSigned()   ==lvalueType->isSigned() )
	// - allow assigning to narrower type
	// - WARNING - should sub-range from local, this violates auto-upgrade
	warn(string("converting argument %d in call to '",argNum)+
	     op->getName()+
	     "' from type "+rvalueType->toString()+
	     " to "   +lvalueType->toString()+" may lose information",
	     actualExpr->getToken());
      else
	fatal(1,
	      string("incompatible type for argument %d in call to '",argNum)+
	      op->getName()+
	      "', type is " +actualType->toString()+
	      ", should be "+formalType->toString(),
	      actualExpr->getToken());
    }
  }

  // - check for wrong number of arguments:
  if (formal)
    fatal(1, string("too few arguments in call to '")+op->getName()+"'",
	  token);
  else if (actual)
    fatal(1, string("too many arguments in call to '")+op->getName()+"'",
	  token);
  if (!type)
  {
    // - regenerate cleared return-type (same as ExprCall::link())
    type=(Type*)op->getType()->duplicate();
    betaRenameInCall(type,this);
    type->setParent(this);
  }
  return type->typeCheck();
  // - warning: INCOMPLETE, must bind parameters in return type to local scope
}


#define maxOneArg(numArgs)	\
  { if (numArgs>1)		\
      fatal(1, string("too many arguments in call to \"")+op->getName()+"\"", \
	    token); }


Type* ExprBuiltin::typeCheck ()
{
  BuiltinKind builtinkind=((OperatorBuiltin*)op)->getBuiltinKind();
  int numArgs=args->length();
  if (numArgs<1 && builtinkind!=BUILTIN_PRINTF)
    fatal(1, string("too few arguments in call to ")+op->getName(), token);

  switch(builtinkind)
  {
    case BUILTIN_ATTN:
      {
	maxOneArg(numArgs);
	Type *t=args->head()->typeCheck();
	if (t->getTypeKind()!=TYPE_INT)
	  fatal(1,"non-integer argument for attn()", args->head()->getToken());
	return type=type_none;
      }

    case BUILTIN_BITSOF:
      {
	maxOneArg(numArgs);
	Type *t=args->head()->typeCheck();
	if (badType(t))
	  fatal(1, string("argument for \"")+op->getName()+
		   "\" has incomplete type "+t->toString(),
		args->head()->getToken());
	if (t->getTypeKind()==TYPE_ARRAY)
	  fatal(1, "cannot use bitsof() on array", args->head()->getToken());
	if (t->getWidth()>=0)
	  type=(new Type(TYPE_INT,t->getWidth(),false)) -> typeCheck();
	else
	  type=(new Type(TYPE_INT,(Expr*)t->makeWidthExpr(),false))
								-> typeCheck();
	type->setParent(this);
	return type;
      }

    case BUILTIN_CAT:
      {
	int argNum=1;
	Expr *e, *widthExpr=NULL;
	if (args->empty())
	{
	  type=(Type*)type_uint0->duplicate();
	  type->setParent(this);
	  return type;
	}
	forall (e,*args)
	{
	  Type *t = e->typeCheck();
	  if (t->getTypeKind()!=TYPE_INT || t->isSigned())
	    fatal(1, string("incompatible type for argument %d of call to \"",
			    argNum) + op->getName() +
		     string("\", type is ") + t->toString() +
		     string(", should be unsigned integer"), e->getToken());
	  widthExpr = widthExpr
			? new ExprBop(NULL,'+',widthExpr,t->makeWidthExpr())
			: t->makeWidthExpr();
	}
	type = (new Type(TYPE_INT,widthExpr,false)) -> typeCheck();
	type->setParent(this);
	return type;
	// - lazy, does not fold const widths
      }

    case BUILTIN_COPY:
      {
	Expr *e;
	Type *prototype=(Type *)NULL;
	forall (e, *args)
	  {
	    Type *t = e->typeCheck();
	    if ((t->getTypeKind()==TYPE_BOOL)
		||(t->getTypeKind()==TYPE_INT)
		||(t->getTypeKind()==TYPE_FIXED))
	      {
		if (prototype==(Type *)NULL)
		  {
		    if ((e->getExprKind()==EXPR_LVALUE)
			|| (e->getExprKind()==EXPR_CALL)
			|| (e->getExprKind()==EXPR_BUILTIN))
		      prototype=t;
		    else
		      {
			fatal(1,"found invalid expression type as source of copy",
			      e->getToken());
			
		      }
		  }
		else
		  {
		    if (e->getExprKind()!=EXPR_LVALUE)
		      {
			fatal(1,"found non-lvalue output targets of copy",
			      e->getToken());
		      }
		    if (prototype->equals(t))
		      {
			// good...keep going...
			// I don't need type anymore, could gc?
		      }
		    else
		      {
			fatal(1,string("types mismatch in copy operator; input is [%s] but output has type [%s]",
				       (const char *)prototype->toString(),
				       (const char *)t->toString()),
			      e->getToken());
		      }
		  }
	      }
	    else
	      {
		fatal(1,"Copy only works on streams of boolean, integer, or fixed values",
		      e->getToken());
	      }
	  }
	// done with prototype --> could gc it here?
	type=type_none;
	return type;
      }

    case BUILTIN_CLOSE:
    case BUILTIN_FRAMECLOSE:
      {
	maxOneArg(numArgs);
	ExprLValue *e = (ExprLValue*)args->head();
	if (    e->getExprKind()!=EXPR_LVALUE
	    ||  e->getSymbol()->getSymKind()!=SYMBOL_STREAM
	    ||  ((SymbolStream*)e->getSymbol())->getDir()!=STREAM_OUT
	    || !e->usesAllBits()
	    ||  e->getRetime()!=NULL )
	  fatal(1, string("incompatible argument for \"")+op->getName()+
		   "\", requires output-stream name", e->getToken());
	e->typeCheck();
	type=type_none;
	return type;
      }

    case BUILTIN_DONE:
      {
	return ExprCall::typeCheck();
      }

    /*
    case BUILTIN_EOS:
      {
	maxOneArg(numArgs);
	ExprLValue *e = (ExprLValue*)args->head();
	if (     e->getExprKind()!=EXPR_LVALUE
	    ||   e->getSymbol()->getSymKind()!=SYMBOL_STREAM
	    ||   ((SymbolStream*)e->getSymbol())->getDir()!=STREAM_IN
	    ||  !e->usesAllBits()
	    || ( e->getRetime()!=NULL &&
		!e->getRetime()->equals((Expr*)expr_0)) )
	  fatal(1, string("incompatible argument for \"")+op->getName()+
		   "\", requires input-stream name", e->getToken());
	e->typeCheck();
	type=(Type*)type_bool->duplicate();
	type->setParent(this);
	return type;
      }
    */

    case BUILTIN_WIDTHOF:
      {
	maxOneArg(numArgs);
	Type *t=args->head()->typeCheck();
	if (badType(t))
	  fatal(1, string("argument for \"")+op->getName()+
		   "\" has incomplete type "+t->toString(),
		args->head()->getToken());
	if (t->getTypeKind()==TYPE_ARRAY)
	  fatal(1, "cannot use widthof() on array", args->head()->getToken());
	if (t->getWidth()>=0)
	  type=(new Type(TYPE_INT,t->getWidth(),false)) -> typeCheck();
	else
	  type=(new Type(TYPE_INT,t->makeWidthExpr(),false)) -> typeCheck();
	// warning - should be log(t->width) ?
	type->setParent(this);
	return type;
      }

    case BUILTIN_SEGMENT:
      {
	return ExprCall::typeCheck();
      }

    case BUILTIN_PRINTF:
      {
	if (getAnnote(ANNOTE_PRINTF_STRING_TOKEN)==NULL)
	  fatal(1, string("missing format string"), token);
	Expr *e;
	forall (e,*args)
	  e->typeCheck();
	type=(Type*)type_none->duplicate();
	type->setParent(this);
	return type;
      }

    default:
      assert(!"internal inconsistency");
  }

  return NULL;	// dummy
}


Type* ExprCond::typeCheck ()
{
	//std::cout << "checking..1" << std::endl;
  Type *condType  = cond->typeCheck();
  if (condType->getTypeKind()!=TYPE_BOOL)
    fatal(1, string("incompatible type for predicate expression of '?', is ")+
	     condType->toString()+", should be boolean", condType->getToken());
  //std::cout << "checking..2" << std::endl;
  Type *thenType  = (thenPart!=(Expr*)0)?thenPart->typeCheck(): type;
  Type *elseType  = (elsePart!=(Expr*)0)?elsePart->typeCheck(): type;
  //std::cout << "checking..3" << std::endl;
  Type *mergeType = thenType->merge(elseType);
  if (mergeType==NULL)
    fatal(1,
	  string("incompatible types for true and false sides of '?', ")+
	  thenType->toString()+" for true, "+elseType->toString()+" for false",
	  thenType->getToken());
  type=mergeType;
  type->setParent(this);
  return type;
}


Type* ExprBop::typeCheck ()
{
  Type *e1Type    = e1->typeCheck();
  Type *e2Type    = e2->typeCheck();
  Type *mergeType = e1Type->merge(e2Type);
  if (badType(mergeType))
    fatal(1,
	  string("incompatible types for binary operator '")+opToString(bop)+
	  "', left side is "+e1Type->toString()+
	  ", right side is "+e2Type->toString(), token);
  if (mergeType->getTypeKind()==TYPE_ARRAY)
    fatal(1, string("cannot use binary operator '")+opToString(bop)+
	     string("' on arrays"), token);

  switch (bop)
  {
    case '+':
    case '-':
      {
	// - widen by 1 bit
	// - warning:  unsigned - unsigned --> unsigned
	if (mergeType->getTypeKind()==TYPE_INT)
	{
	  type = (mergeType->getWidth()>=0)
			? (new Type(TYPE_INT,mergeType->getWidth()+1,
				    mergeType->isSigned(),
				    mergeType->getPredExpr())) -> typeCheck()
			: (new Type(TYPE_INT,
				    new ExprBop(NULL,'+',
						mergeType->makeWidthExpr(),
						(Expr*)expr_1->duplicate()),
				    mergeType->isSigned())) -> typeCheck();
	  type->setParent(this);
	  return type;
	}
	else if (mergeType->getTypeKind()==TYPE_FIXED)
	{
	  type = (new TypeFixed(new ExprBop(NULL,'+',
					    ((TypeFixed*)mergeType)->
						makeIntWidthExpr(),
					    (Expr*)expr_1->duplicate()),
				((TypeFixed*)mergeType)->makeFracWidthExpr(),
				mergeType->isSigned())) -> typeCheck();
	  type->setParent(this);
	  return type;
	  // lazy, does not fold constant widths
	}
	else if (mergeType->getTypeKind()==TYPE_FLOAT || mergeType->getTypeKind()==TYPE_DOUBLE)
	{
		//type= new Type(mergeType->getTypeKind(), new ExprBop(NULL,'+'))->typeCheck();
		type=mergeType;
		type->setParent(this);
		return type;
        }
	else
	  // assert(!"internal inconsistency");
	  fatal(1, string("binary operator '")+opToString(bop)+
		   "' requires integer or fixed point operands", token);
      }
    case '*':
      {
	// - width is sum of operand widths
	// - warning: ignoring widening due to sign upgrade
	if (mergeType->getTypeKind()==TYPE_INT)
	{
	  type = (e1Type->getWidth()>=0 && e2Type->getWidth()>=0)
			? (new Type(TYPE_INT,
				    e1Type->getWidth()+e2Type->getWidth(),
				    mergeType->isSigned())) -> typeCheck()
			: (new Type(TYPE_INT,
				    new ExprBop(NULL,'+',
						e1Type->makeWidthExpr(),
						e2Type->makeWidthExpr()),
				    mergeType->isSigned())) -> typeCheck();
	  type->setParent(this);
	  return type;
	}
	else if (mergeType->getTypeKind()==TYPE_FIXED)
	{
	  type = (new TypeFixed(new ExprBop(NULL,'+',
					    ((TypeFixed*)e1Type)->
					      makeIntWidthExpr(),
					    ((TypeFixed*)e2Type)->
					      makeIntWidthExpr()),
				new ExprBop(NULL,'+',
					    ((TypeFixed*)e1Type)->
					      makeFracWidthExpr(),
					    ((TypeFixed*)e2Type)->
					      makeFracWidthExpr()),
				mergeType->isSigned())) -> typeCheck();
	}
	else if (mergeType->getTypeKind()==TYPE_FLOAT || mergeType->getTypeKind()==TYPE_DOUBLE)
	{

		type=mergeType;
		type->setParent(this);
		return type;
	}
	else
	  // assert(!"internal inconsistency");
	  fatal(1, string("binary operator '")+opToString(bop)+
		   "' requires integer or fixed point operands", token);
      }
    case '/':
      {
	// - do NOT narrow width
	if (mergeType->getTypeKind()==TYPE_INT ||
	    mergeType->getTypeKind()==TYPE_FLOAT ||
	    mergeType->getTypeKind()==TYPE_DOUBLE ||
	    mergeType->getTypeKind()==TYPE_FIXED ) {
	  type=mergeType;	// should have width of 1st arg, not max?
	  type->setParent(this);
	  return type;
	}
	else
	  // assert(!"internal inconsistency");
	  fatal(1, string("binary operator '")+opToString(bop)+
		   "' requires integer or fixed point operands", token);
      }
      /* this should be in biultin? or call
    case 'exp':
    case 'log':
    case 'sqrt':
      {
	if (mergeType->getTypeKind()==TYPE_FLOAT ||
	    mergeType->getTypeKind()==TYPE_DOUBLE ) {
	  type=mergeType;	// should have width of 1st arg, not max?
	  type->setParent(this);
	  return type;
	}
	else
	  fatal(1, string("binary operator '")+opToString(bop)+
		   "' requires floating point operands", token);
      }
      */
    case '%':
      {
	// width comes from 2nd operand
	if (mergeType->getTypeKind()==TYPE_INT)
	{
	  type = (e2Type->getWidth()>=0)
			? (new Type(TYPE_INT,e2Type->getWidth(),
				    mergeType->isSigned())) -> typeCheck()
			: (new Type(TYPE_INT,e2Type->makeWidthExpr(),
				    mergeType->isSigned())) -> typeCheck();
	  type->setParent(this);
	  return type;
	}
	else if (mergeType->getTypeKind()==TYPE_FIXED)
	  fatal(1, "fixed type not supported for binary operator '%'", token);
	else
	  // assert(!"internal inconsistency");
	  fatal(1, string("binary operator '")+opToString(bop)+
		   "' requires integer operands",                token);
//		   "' requires integer or fixed point operands", token);
      }
    case '&':
    case '|':
    case '^':
      {
	// - require unsigned bits
	if (e1Type->getTypeKind()!=TYPE_INT || e1Type->isSigned())
	  fatal(1, string("incompatible type for binary operator '")+
		   opToString(bop)+"', left side is "+e1Type->toString()+
		   ", should be unsigned integer", e1->getToken());
	if (e2Type->getTypeKind()!=TYPE_INT || e2Type->isSigned())
	  fatal(1, string("incompatible type for binary operator '")+
		   opToString(bop)+"', right side is "+e2Type->toString()+
		   ", should be unsigned integer", e2->getToken());
	type=mergeType;
	type->setParent(this);
	return type;
      }
    case LOGIC_AND:
    case LOGIC_OR:
      {
	if (e1Type->getTypeKind()!=TYPE_BOOL)
	  fatal(1, string("incompatible type for binary operator '")+
		   opToString(bop)+"', left side is "+e1Type->toString()+
		   ", should be boolean", e1->getToken());
	if (e2Type->getTypeKind()!=TYPE_BOOL)
	  fatal(1, string("incompatible type for binary operator '")+
		   opToString(bop)+"', right side is "+e2Type->toString()+
		   ", should be boolean", e2->getToken());
	type=(Type*)type_bool->duplicate();
	type->setParent(this);
	return type;
      }
    case EQUALS:
    case NOT_EQUALS:
      {
	type=(Type*)type_bool->duplicate();
	type->setParent(this);
	return type;
      }
    case LTE:
    case GTE:
    case '<':
    case '>':
      {
	if (e1Type->getTypeKind()!=TYPE_INT &&
	    e1Type->getTypeKind()!=TYPE_FLOAT &&
	    e1Type->getTypeKind()!=TYPE_DOUBLE &&
	    e1Type->getTypeKind()!=TYPE_FIXED)
	  fatal(1, string("incompatible type for binary operator '")+
		   opToString(bop)+"', left side is "+e1Type->toString()+
		   ", should be numeric type", e1->getToken());
	if (e2Type->getTypeKind()!=TYPE_INT &&
	    e2Type->getTypeKind()!=TYPE_FLOAT &&
	    e2Type->getTypeKind()!=TYPE_DOUBLE &&
	    e2Type->getTypeKind()!=TYPE_FIXED)
	  fatal(1, string("incompatible type for binary operator '")+
		   opToString(bop)+"', right side is "+e2Type->toString()+
		   ", should be numeric type", e2->getToken());
	type=(Type*)type_bool->duplicate();
	type->setParent(this);
	return type;
      }
    case '.':
      {
	// - compose 2 ints into fixed (how?)
	if (e1Type->getTypeKind()!=TYPE_INT || e1Type->isSigned())
	  fatal(1, string("incompatible type for binary operator '.', ")+
		   "', left side is "+e1Type->toString()+
		   ", should be unsigned integer", e1->getToken());
	if (e2Type->getTypeKind()!=TYPE_INT || e2Type->isSigned())
	  fatal(1, string("incompatible type for binary operator '.', ")+
		   "', right side is "+e2Type->toString()+
		   ", should be unsigned integer", e2->getToken());
	type=(new TypeFixed(e1Type->makeWidthExpr(),
			    e2Type->makeWidthExpr(),false)) -> typeCheck();
	type->setParent(this);
	return type;
      }
    case LEFT_SHIFT:
    case RIGHT_SHIFT:
      {
	// - allow shifting signed ints
	// - allow negative shifts (opposite direction)
	if (e2Type->getTypeKind()!=TYPE_INT)
	  fatal(1, string("non-integer shift amount for operator ")+
		   opToString(bop), e2->getToken());
	/*
	 * - wrong, type width is NOT (width1 +/- width2), it is just width1
	 *
	Expr *widthExpr = new ExprBop(NULL, (bop==LEFT_SHIFT)?'+':'-',
				      e1Type->makeWidthExpr(),
				      e2Type->makeWidthExpr());
	Expr *predExpr  = new ExprBop(NULL,GTE,(Expr*)widthExpr->duplicate(),
				      (Expr*)expr_0->duplicate());
	type=(new Type(TYPE_INT,widthExpr,e1Type->isSigned(),predExpr))
								-> typeCheck();
	 */
	type=(Type*)e1Type->duplicate();	// - it is just width1
	type->setParent(this);
	return type;
      }
    default:
      {
	fatal(1, string("unhandled binary operator '")+opToString(bop)+"'",
	      token);
      }
  }

  return NULL;	// dummy
}


Type* ExprUop::typeCheck ()
{
  Type *eType=e->typeCheck();
  if (badType(eType))
    fatal(1, string("argument for unary operator '")+opToString(uop)+
	     string("', has incomplete type"), e->getToken());
  if (eType->getTypeKind()==TYPE_ARRAY)
    fatal(1, string("cannot use unary operator '")+opToString(uop)+
	     string("' on array"), token);

  switch(uop)
  {
    case '+':
    case '-':
      {
	// - upgrade to signed
	if (eType->getTypeKind()!=TYPE_INT &&
	    eType->getTypeKind()!=TYPE_FIXED &&
	    eType->getTypeKind()!=TYPE_DOUBLE &&
	    eType->getTypeKind()!=TYPE_FLOAT)
	  fatal(1, string("incompatible type for unary operator '")+
		   opToString(uop)+"', is "+eType->toString()+
		   ", should be numeric type", e->getToken());
	if (!eType->isSigned())
	{
	  if (eType->getTypeKind()==TYPE_INT)
	  {
	    type = (eType->getWidth()>=0)
			? (new Type(TYPE_INT,eType->getWidth()+1,true))
								 ->typeCheck()
			: (new Type(TYPE_INT,
				    new ExprBop(NULL,'+',
						(Expr*)eType->makeWidthExpr(),
						(Expr*)expr_1->duplicate()),
				    true)) ->typeCheck();
	    type->setParent(this);
	    return type;
	  }
	  else if (eType->getTypeKind()==TYPE_FIXED)
	  {
	    Expr *intWidthExpr = new ExprBop(NULL,'+',
					     ((TypeFixed*)eType)->
							makeIntWidthExpr(),
					     (Expr*)expr_1->duplicate());
	    Expr *fracWidthExpr = ((TypeFixed*)eType)->makeFracWidthExpr();
	    type = (new TypeFixed(intWidthExpr,fracWidthExpr,true))
								-> typeCheck();
	    type->setParent(this);
	    return type;
	  }
	  else if (eType->getTypeKind()==TYPE_FLOAT ||
	  		eType->getTypeKind()==TYPE_DOUBLE)
	  {
	    type=(Type*)eType->duplicate();	// - warning: duplicates pred
	    type->setParent(this);
	    return type;	    
	  }
	  else
	    assert(!"internal inconsistency");
	}
	else
	{
	  type=(Type*)eType->duplicate();	// - warning: duplicates pred
	  type->setParent(this);
	  return type;
	}
      }
    case SQRT:
      if(eType->getTypeKind()==TYPE_FLOAT || eType->getTypeKind()==TYPE_DOUBLE) {
	  type=(Type*)eType->duplicate();	// - warning: duplicates pred

	  type->setParent(this);
	  return type;
      } else {
	  fatal(1, string("incompatible type for unary operator '")+
		   opToString(uop)+"', is "+eType->toString()+
		   ", should be float or double ", e->getToken());
      }
    case FLOOR:
      if(eType->getTypeKind()==TYPE_FLOAT || eType->getTypeKind()==TYPE_DOUBLE) {
	  type=(Type*)eType->duplicate();	// - warning: duplicates pred

	  type->setParent(this);
	  return type;
      } else {
	  fatal(1, string("incompatible type for unary operator '")+
		   opToString(uop)+"', is "+eType->toString()+
		   ", should be float or double ", e->getToken());
      }
    case '~':
      {
	if (eType->getTypeKind()!=TYPE_INT || eType->isSigned())
	  fatal(1, string("incompatible type for unary operator '")+
		   opToString(uop)+"', is "+eType->toString()+
		   ", should be unsigned integer", e->getToken());
	  type=(Type*)eType->duplicate();	// - warning: duplicates pred

	  type->setParent(this);
	  return type;
      }
    case '!':
      {
	if (eType->getTypeKind()!=TYPE_BOOL)
	  fatal(1, string("incompatible type for unary operator '")+
		   opToString(uop)+"', is "+eType->toString()+
		   ", should be boolean", e->getToken());
	type=(Type*)type_bool->duplicate();
	type->setParent(this);
	return type;
      }
    default:
      {
	fatal(1, string("unhandled unary operator '")+opToString(uop)+"'",
	      token);
      }
  }

  return NULL;	// dummy
}


Type* ExprCast::typeCheck ()
{
  Type *eType       = e->typeCheck();
  Type *castType    = type->typeCheck();
  if (badType(castType))
    fatal(1, string("casting to incomplete type ")+type->toString(), token);
  if (      eType->getTypeKind()==TYPE_ARRAY
      || castType->getTypeKind()==TYPE_ARRAY)
    fatal(1, string("cannot cast to/from array types"), token);

  Type *upgradeType = eType->upgrade(castType);
  if (badType(upgradeType))
  {
    if (      eType->getTypeKind()==TYPE_INT
	&& castType->getTypeKind()==TYPE_INT
	&&    eType->isSigned()==castType->isSigned())
      // - HACK:    Allow casting int to narrower int of same sign
      //            (there is no other 'easy' syntax for narrowing signed int)
      // - WARNING: Should sub-range instead;
      //            this violates Eylon's philosophy that type conversion is
      //            non-destructive, and destruction is explicit by bit ops
      warn(string("casting from type ")+eType->toString()+
           " to type "+castType->toString()+" may lose information", token);
    else
      fatal(1, string("cannot cast type ")+eType->toString()+
		      " to type "+castType->toString(), token);
  }
  return type;
}


Type* ExprArray::typeCheck ()
{
  if (type)
  {
    type->typeCheck();
    if (badType(type))
      fatal(1,"incomplete/invalid type for array",token);
    if (type->getTypeKind()!=TYPE_ARRAY)
      assert(!"internal inconsistency");
  }
  TypeArray *aType     = (TypeArray*)type;
  Type      *aeType    = aType ? aType->getElemType() : NULL,
	    *mergeType = NULL;
  bool       untyped   = aeType ? aeType->getTypeKind()==TYPE_ANY : true;
  Expr *e;
  forall (e,*exprs)
  {
    Type *eType=e->typeCheck();
    if (untyped)
    {
      // - given array of TYPE_ANY, compute actual array type
      mergeType = mergeType ? mergeType->merge(eType) : eType;
    }
    else
    {
      // - type-check element type against array's elemType
      if (!eType->upgradable(aeType))
	fatal(1, string("incompatible type for array element, is ")+
	         eType->toString()+", should be upgradable to "+
	         aeType->toString(),  e->getToken());
    }
  }
  if (untyped)
    type=(new TypeArray(mergeType,exprs->size())) -> typeCheck();
  else
  {
    // - make sure number of expressions is correct
    long long nelems=aType->getNelems();
    if (nelems<0)
    {
      Expr *cmpExpr  = new ExprBop(NULL,EQUALS,aType->makeNelemsExpr(),
				   constIntExpr(exprs->size()));
      // Expr *predExpr1= aType->getPredExpr();
      Expr *predExpr1= NULL;
      Expr *predExpr = predExpr1? new ExprBop(NULL,LOGIC_AND,predExpr1,cmpExpr)
				: cmpExpr;
      type=(new TypeArray((Type*)aeType->duplicate(),
			  aType->makeNelemsExpr(), predExpr)) -> typeCheck();
    }
    else if (nelems!=exprs->size())
      fatal(1, string("incorrect number of expressions in array, got %ld, "
		      "expecting %ld", (long)nelems, (long)(exprs->size())),
	    token);
  }
  type->setParent(this);
  return type;
}


////////////////////////////////////////////////////////////////
//  addExpr
//   - add expr to ExprArray

void ExprArray::addExpr (Expr *e)
{
  // - warning, this does not update count in array type
  assert(e);
  assert(e->getExprKind()!=EXPR_ARRAY);
  exprs->append(e);
  e->setParent(this);
}


////////////////////////////////////////////////////////////////
//  isLValue, isRValue
//   - true if an Expr is assignable, valued (respectively)

bool Expr::isLValue () const
{
  if (exprKind==EXPR_LVALUE)
  {
    Symbol     *sym     = ((ExprLValue*)this)->getSymbol();
    SymbolKind  symKind = sym->getSymKind();
    return (   (   symKind==SYMBOL_STREAM
		&& ((SymbolStream*)sym)->getDir()!=STREAM_IN)
	    || (   symKind==SYMBOL_VAR
		&& !(sym->isParam() && !sym->isArray())) );
  }
  else
    return false;
}


bool Expr::isRValue () const
{
  if (exprKind==EXPR_LVALUE)
  {
    Symbol     *sym     = ((ExprLValue*)this)->getSymbol();
    SymbolKind  symKind = sym->getSymKind();
    return !(   symKind==SYMBOL_STREAM
	     && ((SymbolStream*)sym)->getDir()==STREAM_OUT);
  }
  else
    return true;
  // - calls to non-returning operators (ExprCall and ExprBuiltin)
  //   are not values, hence not rvalues, but they do not arise here,
  //   arise only as statements
}


////////////////////////////////////////////////////////////////
//  equals

#define safeEquals(a,b) ((a) ? ((b)?(a)->equals(b):false) : ((b)?false:true))


bool ExprValue::equals (const Expr *x) const
{
  if (    getExprKind() != x->getExprKind()
      || !getType()->equals(x->getType()) )
    return false;
  ExprValue *xx=(ExprValue*)x;
  if((*getType()).getTypeKind()==TYPE_FLOAT || (*getType()).getTypeKind()==TYPE_DOUBLE) {
  	return (floatVal==xx->floatVal
	  && doubleVal==xx->doubleVal);
  }
  return (   intVal ==xx->intVal
	  && fracVal==xx->fracVal);
}


bool ExprLValue::equals (const Expr *x) const
{
  if (    getExprKind() != x->getExprKind()
      || !getType()->equals(x->getType()) )
    return false;
  ExprLValue *xx=(ExprLValue*)x;
  return (   sym->equals(xx->sym)
	  && safeEquals(posLow,  xx->posLow)
	  && safeEquals(posHigh, xx->posHigh)
	  && safeEquals(retime,  xx->retime)
	  && safeEquals(arrayLoc,xx->retime)
	  && allBits==xx->allBits);
}


bool ExprCall::equals (const Expr *x) const
{
  if (    getExprKind() != x->getExprKind()
      || !getType()->equals(x->getType()) )
    return false;
  ExprCall *xx=(ExprCall*)x;
  if ((op!=xx->op) ||
      (token && xx->token && token->str!=xx->token->str))
    return false;
  if (args->length()!=xx->args->length())
    return false;
  list_item i,xxi;
  for (i=args->first(), xxi=xx->args->first();
       i && xxi;
       i=args->succ(i), xxi=xx->args->succ(xxi))
    if (!args->inf(i)->equals(xx->args->inf(xxi)))
      return false;
  return true;
}


bool ExprCond::equals (const Expr *x) const
{
  if (    getExprKind() != x->getExprKind()
      || !getType()->equals(x->getType()) )
    return false;
  ExprCond *xx=(ExprCond*)x;
  return (   cond->equals(xx->cond)
	  && thenPart->equals(xx->thenPart)
	  && elsePart->equals(xx->elsePart));
}


bool ExprBop::equals (const Expr *x) const
{
  if (    getExprKind() != x->getExprKind()
      || !getType()->equals(x->getType()) )
    return false;
  ExprBop *xx=(ExprBop*)x;
  return (   bop==xx->bop
	  && e1->equals(xx->e1)
	  && e2->equals(xx->e2));
}


bool ExprUop::equals (const Expr *x) const
{
  if (    getExprKind() != x->getExprKind()
      || !getType()->equals(x->getType()) )
    return false;
  ExprUop *xx=(ExprUop*)x;
  return (   uop==xx->uop
	  && e->equals(xx->e));
}


bool ExprCast::equals (const Expr *x) const
{
  if (    getExprKind() != x->getExprKind()
      || !getType()->equals(x->getType()) )
    return false;
  ExprCast *xx=(ExprCast*)x;
  return (e->equals(xx->e));
}


bool ExprArray::equals (const Expr *x) const
{
  if (    getExprKind() != x->getExprKind()
      || !getType()->equals(x->getType()) )
    return false;
  ExprArray *xx=(ExprArray*)x;
  list_item e, xxe;
  for (e=exprs->first(), xxe=xx->exprs->first();
       e && xxe;
       e=exprs->succ(e), xxe=xx->exprs->succ(xxe))
    if (!exprs->inf(e)->equals(xx->exprs->inf(xxe)))
      return false;
  if (e!=xxe)		// - make sure same #expr (redundant?)
    return false;
  return true;
}


////////////////////////////////////////////////////////////////
//  okInComposeOp

bool ExprLValue::okInComposeOp () const
{
  return    (retime==NULL || retime->equals(expr_0))
         && arrayLoc==NULL
	 && posLow->okInComposeOp()
	 && posHigh->okInComposeOp();
}


bool ExprCall::okInComposeOp () const
{
  bool ok=true;
  Expr *e;
  forall (e,*args)
    ok = ok && e->okInComposeOp();
  return ok;
}


bool ExprBuiltin::okInComposeOp () const
{
  return ((OperatorBuiltin*)op)->okInComposeOp() && ExprCall::okInComposeOp();
}


bool ExprCond::okInComposeOp () const
{
  return (       cond->okInComposeOp()
	  && thenPart->okInComposeOp()
	  && elsePart->okInComposeOp());
  // - warning:  not checking binding times!  cond ok only for bound subtrees
}


bool ExprBop::okInComposeOp () const
{
  return (e1->okInComposeOp() && e2->okInComposeOp());
  // - warning:  not checking binding times!  bop ok only for bound e1,e2
}


bool ExprUop::okInComposeOp () const
{
  return e->okInComposeOp();
  // - warning:  not checking binding times!  uop ok only for bound e
}

using std::cout;
using std::endl;

////////////////////////////////////////////////////////////////
//  Type management

Type* Expr::getType () const
{
  Expr *this_=(Expr*)this;
  if (!type)
  {

	        //cout << "Internal type-checking error while regenerating type of \"" << this_->getParent()->toString() << "\" from top-level expression \"" << this_->toString() << "\"" << endl;
    // - regenerate all types from top of this expr tree
    // - top == (farthest Expr ancestor before hitting nearest non-Expr)
    Tree *t, *p;	// t=(tree node), p=(t's parent)
    TreeKind k;		// k=(p's kind)

    for (t=this_;
	 t && (p=t->getParent()) && (k=p->getKind())==TREE_EXPR;
	 t=p);
    t->typeCheck();	// - regenerate types

    // - regenerate types for/under this node (less efficient?)
    // this_->type = typeCheck();		// - regenerate types
    // type->setParent(this_);

    if (!type)
      fatal(1, string("Internal type-checking error "
		      "while regenerating type of \"")+
	       toString()+"\" from top-level expression \""+t->toString()+"\"",
	    token);
  }
  return type;
}


static
bool delmap  (Tree *t, void *i)
{
  // - map to delete all nodes of an AST
  // - HACK: do NOT delete type_any, type_none, since they are shared
  if (t!=type_any && t!=type_none)
    delete t;
  return true;
}

#define deleteTypeMacro()	if (type) {	                        \
				  type->map((TreeMap)NULL,delmap);	\
				  type=NULL;				\
				}

void ExprLValue::deleteType  () { deleteTypeMacro(); }
void ExprCall::deleteType    () { deleteTypeMacro(); }
void ExprBuiltin::deleteType () { deleteTypeMacro(); }
void ExprCond::deleteType    () { deleteTypeMacro(); }
void ExprBop::deleteType     () { deleteTypeMacro(); }
void ExprUop::deleteType     () { deleteTypeMacro(); }
void ExprArray::deleteType   () { deleteTypeMacro(); }


////////////////////////////////////////////////////////////////
//  getBindTime  (stubs for now)

BindTime ExprValue::getBindTime () const    { return BindTime(BIND_UNKNOWN); }
BindTime ExprLValue::getBindTime () const   { return BindTime(BIND_UNKNOWN); }
BindTime ExprCall::getBindTime () const	    { return BindTime(BIND_UNKNOWN); }
BindTime ExprBuiltin::getBindTime () const  { return BindTime(BIND_UNKNOWN); }
BindTime ExprCond::getBindTime () const	    { return BindTime(BIND_UNKNOWN); }
BindTime ExprBop::getBindTime () const	    { return BindTime(BIND_UNKNOWN); }
BindTime ExprUop::getBindTime () const	    { return BindTime(BIND_UNKNOWN); }
BindTime ExprCast::getBindTime () const	    { return BindTime(BIND_UNKNOWN); }
BindTime ExprArray::getBindTime () const    { return BindTime(BIND_UNKNOWN); }


////////////////////////////////////////////////////////////////
//  duplicate()

Tree* ExprValue::duplicate () const
{
  ExprValue *e = new ExprValue(*this);
  e->setParent(NULL);
  e->type = (Type*)e->type->duplicate();
  e->type->setParent(e);
  return e;
}


Tree* ExprLValue::duplicate () const
{
  ExprLValue *e = new ExprLValue(*this);
  e->setParent(NULL);
  if (e->type)
  {
    e->type = (Type*)e->type->duplicate();
    e->type->setParent(e);
  }
  if (e->posLow)
  {
    e->posLow   = (Expr*)e->posLow  ->duplicate();
    e->posLow   ->setParent(e);
  }
  if (e->posHigh)
  {
    e->posHigh  = (Expr*)e->posHigh ->duplicate();
    e->posHigh  ->setParent(e);
  }
  if (e->retime)
  {
    e->retime   = (Expr*)e->retime  ->duplicate();
    e->retime   ->setParent(e);
  }
  if (e->arrayLoc)
  {
    e->arrayLoc = (Expr*)e->arrayLoc->duplicate();
    e->arrayLoc ->setParent(e);
  }
  return e;
}


Tree* ExprCall::duplicate () const
{
  ExprCall *e = exprKind==EXPR_CALL    ? new ExprCall(*this)
	      : exprKind==EXPR_BUILTIN ? new ExprBuiltin(*(ExprBuiltin*)this)
	      : (assert(!"internal inconsistency"), (ExprCall*)NULL);
  e->setParent(NULL);
  if (e->type)
  {
    e->type = (Type*)e->type->duplicate();
    e->type->setParent(e);
  }
  e->args = new list<Expr*>;
  Expr* arg;
  forall(arg,*args)
  {
    Expr *argCopy = (Expr*)arg->duplicate();
    argCopy->setParent(e);
    e->args->append(argCopy);
  }
  return e;
}


// Tree* ExprBuiltin::duplicate () const {}
//  -->  ExprCall::duplicate()

Tree* ExprCond::duplicate () const
{
  ExprCond *e = new ExprCond(*this);
  e->setParent(NULL);
  if (e->type)
  {
    e->type   = (Type*)e->type    ->duplicate();
    e->type   ->setParent(e);
  }
  e->cond     = (Expr*)e->cond    ->duplicate();
  e->cond     ->setParent(e);
  e->thenPart = (Expr*)e->thenPart->duplicate();
  e->thenPart ->setParent(e);
  e->elsePart = (Expr*)e->elsePart->duplicate();
  e->elsePart ->setParent(e);
  return e;
}


Tree* ExprBop::duplicate () const
{
  ExprBop *e = new ExprBop(*this);
  e->setParent(NULL);
  if (e->type)
  {
    e->type = (Type*)e->type->duplicate();
    e->type ->setParent(e);
  }
  e->e1 = (Expr*)e->e1->duplicate();
  e->e1 ->setParent(e);
  e->e2 = (Expr*)e->e2->duplicate();
  e->e2 ->setParent(e);
  return e;
}


Tree* ExprUop::duplicate () const
{
  ExprUop *e = new ExprUop(*this);
  e->setParent(NULL);
  if (e->type)
  {
    e->type = (Type*)e->type->duplicate();
    e->type ->setParent(e);
  }
  e->e = (Expr*)e->e->duplicate();
  e->e ->setParent(e);
  return e;
}


Tree* ExprCast::duplicate () const
{
  ExprCast *e = new ExprCast(*this);
  e->setParent(NULL);
  e->type = (Type*)e->type->duplicate();
  e->type ->setParent(e);
  e->e    = (Expr*)e->e->duplicate();
  e->e    ->setParent(e);
  return e;
}


Tree* ExprArray::duplicate () const
{
  ExprArray *e=new ExprArray(*this);
  e->setParent(NULL);
  if (e->type)
  {
    e->type = (Type*)e->type->duplicate();
    e->type ->setParent(e);
  }
  e->exprs = new list<Expr*>;
  Expr *ee;
  forall (ee,*exprs)
  {
    Expr *eeCopy = (Expr*)ee->duplicate();
    eeCopy->setParent(e);
    e->exprs->append(eeCopy);
  }
  return e;
}


////////////////////////////////////////////////////////////////
//  toString

static bool kindRequiresParens (const Expr *e)
{
  // - true if expr requires surrounding parens in TDF for proper order-of-eval
  ExprKind kind=e->getExprKind();
  return (    kind==EXPR_COND
	  ||  kind==EXPR_BOP
	  ||  kind==EXPR_UOP
	  ||  kind==EXPR_CAST
	  || (kind==EXPR_LVALUE && ((ExprLValue*)e)->getRetime()) );
}

static bool requiresParens (const Expr *e)
{
  // - true if expr requires surrounding parens in TDF for proper order-of-eval
  Expr *p = (Expr*)e->getParent();
  return (   kindRequiresParens(e)
	  && p
	  && (   (   p->getKind()==TREE_EXPR		// parent necessitates
		  && kindRequiresParens((Expr*)p))
	      || (   p->getKind()==TREE_INPUTSPEC	// is num tokens "#n"
		  &&  ((InputSpec*)p)->getNumTokens()==e
		  && !((InputSpec*)p)->isEosCase()
		  && !((InputSpec*)p)->isEofrCase()) ) );
}


string ExprValue::toString () const
{
  switch (type->getTypeKind())
  {
    case TYPE_ANY:
    case TYPE_NONE:
      return string("<nil value>");	  // not sure if this ever happens
    case TYPE_STATE:
    	// return string(state->getName());
    	return stateVal;
    	//return string("stupid_state");//
    case TYPE_BOOL:
      return string(intVal?"true":"false");
    case TYPE_FLOAT:
      return string("%g",floatVal);
    case TYPE_DOUBLE:
      return string("%g",doubleVal);
    case TYPE_INT:
      return string("%ld",(long)intVal);  // leda string can't handle long long
    case TYPE_FIXED:
      return string("%ld.%ld",(long)intVal,(long)fracVal);
    default:
      assert(!"internal inconsistency");
      return string();
  }
}


string ExprLValue::toString () const
{
  if (sym==NULL)
    return string("<nil sym>");

  switch (sym->getType()->getTypeKind())
  {
    case TYPE_NONE:
      return string("<nil sym>");
    case TYPE_ANY:
      return sym->getName();
    default:
      {
	string arrayLocStr, posStr, retimeStr;
	if (arrayLoc)
	  arrayLocStr = string("[") + arrayLoc->toString() + "]";
	if (posHigh && !allBits)
	  {
	    posStr = string("[") + posHigh->toString();
	    if (posLow && !(posHigh==posLow || posHigh->equals(posLow)))
	      posStr += string(":") + posLow->toString();
	    posStr += string("]");
	  }
	if (retime && !retime->equals((Expr*)expr_0))
	  retimeStr = string("@") + retime->toString();
	return sym->getName() + arrayLocStr + posStr + retimeStr;
      }
  }
}


string ExprCall::toString () const
{
  string argsStr;
  if (args->size()>0)
  {
    if (   args->size()==1				    // passing 1 arg
	&& args->head()->getType()->getWidth()==0	    // arg is 0 bits
	&& op						    // know called op
	&& op->getArgs()->size()==1			    // op has 1 formal
	&& op->getArgs()->head()->getType()->getWidth()==0) // formal is 0 bits
    {
      // - implicit 0-width sync token
    }
    else
    {
      // - normal args
      list_item i=args->first();
      argsStr += args->inf(i)->toString();
      while ((i=args->succ(i)))
	argsStr += string(",") + args->inf(i)->toString();
    }
  }
  string opStr = op ? op->getName() : token->str;
  return opStr + "(" + argsStr + ")";
}


string ExprBuiltin::toString () const
{
  if (((OperatorBuiltin*)op)->getBuiltinKind()==BUILTIN_PRINTF)
  {
    // - special handling for "printf" builtin, format string not in arg list
    string argsStr;
    for (list_item i=args->first(); i; i=args->succ(i))
      argsStr += string(",") + args->inf(i)->toString();
    string formatStr=((Token*)getAnnote(ANNOTE_PRINTF_STRING_TOKEN))->str;
    return op->getName() + "(\"" + formatStr + "\"" + argsStr + ")";
  }
  else
    return ExprCall::toString();
}


string ExprCond::toString () const
{
  bool parens=requiresParens(this);
  if(thenPart!=(Expr*)0 && elsePart!=(Expr*)0) {
	  return (parens?string("("):string()) +
			  cond->toString() + "?" +
			  thenPart->toString() + ":" +
			  elsePart->toString() +
			  (parens?string(")"):string());
  } else {
	  return (parens?string("("):string()) +
	  			  cond->toString() + "? <duck>:<duck>";
  }
}


string ExprBop::toString () const
{
  bool parens=requiresParens(this);
  return (parens?string("("):string()) +
	 e1->toString() + opToString(bop) + e2->toString() +
	 (parens?string(")"):string());
}


string ExprUop::toString () const
{
  bool parens=requiresParens(this);
  return (parens?string("("):string()) +
	 opToString(uop) + e->toString() +
	 (parens?string(")"):string());
}


string ExprCast::toString () const
{
  bool parens=requiresParens(this);
  return (parens?string("("):string()) +
	 "(" + type->toString() + ")" + e->toString() +
	 (parens?string(")"):string());
}


string ExprArray::toString () const
{
  if (exprs->empty())
    return string("{}");
  else
  {
    string ret = "{" + exprs->head()->toString();
    Expr *e;
    forall (e,*exprs)
      if (e!=exprs->head())
	ret += "," + e->toString();
    ret += "}";
    return ret;
  }
}


////////////////////////////////////////////////////////////////
//  map

void ExprValue::map (TreeMap pre, TreeMap post, void *i)
{
  bool descend=true;
  if (pre)
    descend=pre(this,i);
  if (descend)
    type->map(pre,post,i);
  if (post)
    post(this,i);
}


void ExprValue::map2 (Tree **h, TreeMap2 pre, TreeMap2 post,
		      void *i, bool skipPre)
{
  assert(this==*h);
  Tree *p=parent;	// save for replacement node in case pre/post modify it
  bool descend=true;
  if (pre && !skipPre)
  {
    descend=pre(h,i);
    if (this!=*h)
    {
      (*h)->setParent(p);
      if (descend)
      {
	(*h)->map2(h,pre,post,i,true);
	return;
      }
    }
  }
  if (descend)
  {
    type->map2((Tree**)&type,pre,post,i);
  }
  if (post)
  {
    post(h,i);
    if (this!=*h)
      (*h)->setParent(p);
  }
}


void ExprLValue::map (TreeMap pre, TreeMap post, void *i)
{
  bool descend=true;
  if (pre)
    descend=pre(this,i);
  if (descend)
  {
    if (type)
      type->map(pre,post,i);
    // do NOT map symbol
    if (posLow)
      posLow  ->map(pre,post,i);
    if (posHigh)
      posHigh ->map(pre,post,i);
    if (retime)
      retime  ->map(pre,post,i);
    if (arrayLoc)
      arrayLoc->map(pre,post,i);
  }
  if (post)
    post(this,i);
}


void ExprLValue::map2 (Tree **h, TreeMap2 pre, TreeMap2 post,
		       void *i, bool skipPre)
{
  assert(this==*h);
  Tree *p=parent;	// save for replacement node in case pre/post modify it
  bool descend=true;
  if (pre && !skipPre)
  {
    descend=pre(h,i);
    if (this!=*h)
    {
      (*h)->setParent(p);
      if (descend)
      {
	(*h)->map2(h,pre,post,i,true);
	return;
      }
    }
  }
  if (descend)
  {
    if (type)
      type->map2((Tree**)&type,pre,post,i);
    // do NOT map symbol
    if (posLow)
      posLow  ->map2((Tree**)&posLow,   pre,post,i);
    if (posHigh)
      posHigh ->map2((Tree**)&posHigh,  pre,post,i);
    if (retime)
      retime  ->map2((Tree**)&retime,   pre,post,i);
    if (arrayLoc)
      arrayLoc->map2((Tree**)&arrayLoc, pre,post,i);
  }
  if (post)
  {
    post(h,i);
    if (this!=*h)
      (*h)->setParent(p);
  }
}


void ExprCall::map (TreeMap pre, TreeMap post, void *i)
{
  bool descend=true;
  if (pre)
    descend=pre(this,i);
  if (descend)
  {
    if (type)
      type->map(pre,post,i);
    // do NOT map op
    Expr *arg;
    forall (arg,*args)
      arg->map(pre,post,i);
  }
  if (post)
    post(this,i);
}


void ExprCall::map2 (Tree **h, TreeMap2 pre, TreeMap2 post,
		     void *i, bool skipPre)
{
  assert(this==*h);
  Tree *p=parent;	// save for replacement node in case pre/post modify it
  bool descend=true;
  if (pre && !skipPre)
  {
    descend=pre(h,i);
    if (this!=*h)
    {
      (*h)->setParent(p);
      if (descend)
      {
	(*h)->map2(h,pre,post,i,true);
	return;
      }
    }
  }
  if (descend)
  {
    if (type)
      type->map2((Tree**)&type,pre,post,i);
    // do NOT map op
    list_item it;
    forall_items (it,*args)
    {
      Expr *arg=args->inf(it);
      arg->map2((Tree**)&arg,pre,post,i);
      args->assign(it,arg);
    }
  }
  if (post)
  {
    post(h,i);
    if (this!=*h)
      (*h)->setParent(p);
  }
}


// void ExprBuiltin::map
//  --> ExprCall::map


// void ExprBuiltin::map2
//  --> ExprCall::map2


void ExprCond::map (TreeMap pre, TreeMap post, void *i)
{
  bool descend=true;
  if (pre)
    descend=pre(this,i);
  if (descend)
  {
    if (type)
      type  ->map(pre,post,i);
    cond    ->map(pre,post,i);
    thenPart->map(pre,post,i);
    elsePart->map(pre,post,i);
  }
  if (post)
    post(this,i);
}


void ExprCond::map2 (Tree **h, TreeMap2 pre, TreeMap2 post,
		     void *i, bool skipPre)
{
  assert(this==*h);
  Tree *p=parent;	// save for replacement node in case pre/post modify it
  bool descend=true;
  if (pre && !skipPre)
  {
    descend=pre(h,i);
    if (this!=*h)
    {
      (*h)->setParent(p);
      if (descend)
      {
	(*h)->map2(h,pre,post,i,true);
	return;
      }
    }
  }
  if (descend)
  {
    if (type)
      type  ->map2((Tree**)&type,    pre,post,i);
    cond    ->map2((Tree**)&cond,    pre,post,i);
    thenPart->map2((Tree**)&thenPart,pre,post,i);
    elsePart->map2((Tree**)&elsePart,pre,post,i);
  }
  if (post)
  {
    post(h,i);
    if (this!=*h)
      (*h)->setParent(p);
  }
}


void ExprBop::map (TreeMap pre, TreeMap post, void *i)
{
  bool descend=true;
  if (pre)
    descend=pre(this,i);
  if (descend)
  {
    if (type)
      type->map(pre,post,i);
    e1    ->map(pre,post,i);
    e2    ->map(pre,post,i);
  }
  if (post)
    post(this,i);
}


void ExprBop::map2 (Tree **h, TreeMap2 pre, TreeMap2 post,
		    void *i, bool skipPre)
{
  assert(this==*h);
  Tree *p=parent;	// save for replacement node in case pre/post modify it
  bool descend=true;
  if (pre && !skipPre)
  {
    descend=pre(h,i);
    if (this!=*h)
    {
      (*h)->setParent(p);
      if (descend)
      {
	(*h)->map2(h,pre,post,i,true);
	return;
      }
    }
  }
  if (descend)
  {
    if (type)
      type->map2((Tree**)&type,pre,post,i);
    e1    ->map2((Tree**)&e1,  pre,post,i);
    e2    ->map2((Tree**)&e2,  pre,post,i);
  }
  if (post)
  {
    post(h,i);
    if (this!=*h)
      (*h)->setParent(p);
  }
}


void ExprUop::map (TreeMap pre, TreeMap post, void *i)
{
  bool descend=true;
  if (pre)
    descend=pre(this,i);
  if (descend)
  {
    if (type)
      type->map(pre,post,i);
    e     ->map(pre,post,i);
  }
  if (post)
    post(this,i);
}


void ExprUop::map2 (Tree **h, TreeMap2 pre, TreeMap2 post,
		    void *i, bool skipPre)
{
  assert(this==*h);
  Tree *p=parent;	// save for replacement node in case pre/post modify it
  bool descend=true;
  if (pre && !skipPre)
  {
    descend=pre(h,i);
    if (this!=*h)
    {
      (*h)->setParent(p);
      if (descend)
      {
	(*h)->map2(h,pre,post,i,true);
	return;
      }
    }
  }
  if (descend)
  {
    if (type)
      type->map2((Tree**)&type,pre,post,i);
    e     ->map2((Tree**)&e,   pre,post,i);
  }
  if (post)
  {
    post(h,i);
    if (this!=*h)
      (*h)->setParent(p);
  }
}


void ExprCast::map (TreeMap pre, TreeMap post, void *i)
{
  bool descend=true;
  if (pre)
    descend=pre(this,i);
  if (descend)
  {
    type->map(pre,post,i);
    e   ->map(pre,post,i);
  }
  if (post)
    post(this,i);
}


void ExprCast::map2 (Tree **h, TreeMap2 pre, TreeMap2 post,
		     void *i, bool skipPre)
{
  assert(this==*h);
  Tree *p=parent;	// save for replacement node in case pre/post modify it
  bool descend=true;
  if (pre && !skipPre)
  {
    descend=pre(h,i);
    if (this!=*h)    
    {
      (*h)->setParent(p);
      if (descend)
      {
	(*h)->map2(h,pre,post,i,true);
	return;
      }
    }
  }
  if (descend)
  {
    type->map2((Tree**)&type,pre,post,i);
    e   ->map2((Tree**)&e,   pre,post,i);
  }
  if (post)
  {
    post(h,i);
    if (this!=*h)
      (*h)->setParent(p);
  }
}


void ExprArray::map (TreeMap pre, TreeMap post, void *i)
{
  bool descend=true;
  if (pre)
    descend=pre(this,i);
  if (descend)
  {
    if (type)
      type->map(pre,post,i);
    Expr *e;
    forall (e,*exprs)
      e->map(pre,post,i);
  }
  if (post)
    post(this,i);
}


void ExprArray::map2 (Tree **h, TreeMap2 pre, TreeMap2 post,
		      void *i, bool skipPre)
{
  assert(this==*h);
  Tree *p=parent;	// save for replacement node in case pre/post modify it
  bool descend=true;
  if (pre && !skipPre)
  {
    descend=pre(h,i);
    if (this!=*h)
    {
      (*h)->setParent(p);
      if (descend)
      {
	(*h)->map2(h,pre,post,i,true);
	return;
      }
    }
  }
  if (descend)
  {
    if (type)
      type->map2((Tree**)&type,pre,post,i);
    list_item it;
    forall_items (it,*exprs)
    {
      Expr *e=exprs->inf(it);
      e->map2((Tree**)&e,pre,post,i);
      exprs->assign(it,e);
    }
  }
  if (post)
  {
    post(h,i);
    if (this!=*h)
      (*h)->setParent(p);
  }
}

string exprkindToString(ExprKind e) {
	switch(e) {
		case EXPR_VALUE: return string("EXPR_VALUE");
		case EXPR_LVALUE: return string("EXPR_LVALUE");
		case EXPR_CALL: return string("EXPR_CALL");
		case EXPR_BUILTIN: return string("EXPR_BUILTIN");
		case EXPR_COND: return string("EXPR_COND");
		case EXPR_BOP: return string("EXPR_BOP");
		case EXPR_UOP: return string("EXPR_UOP");
		case EXPR_CAST: return string("EXPR_CAST");
		case EXPR_ARRAY: return string("EXPR_ARRAY");
		case EXPR_BITSEL: return string("EXPR_BITSEL");
		default: return string("bs");
	};
}
