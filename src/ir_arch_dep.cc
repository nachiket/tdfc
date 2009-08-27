#include "operator.h"
#include "ops.h"
#include "math.h"


#define ADD  1
#define MUL  1
#define DIV  1    // 0: ASSUME IT'S A SHIFT... NOT ALWAYS TRUE THOUGH! (laurap)
                  // 1: ASSUME NxM ARRAY (eylon)
#define MOD  1    // ???
#define LOGIC 1   // because we are  bound by output constraints. it's worse case

// parameters are at the moment more than needed, 
// but I am leaving it like this until I have a stable model 

int ArchDepArea(int bop ,int bw1, bool c1,int bw2, bool c2 , bool is_Bop, 
		int abw){ //abw: assigned_bw
  if (is_Bop){
  switch (bop)
    {
    case GTE:       // all these cases fall through to sum
    case LTE:
    case '>': 
    case '<':
    case '-': 
    case '+': 
      {
	int bw_to_be_passed;
	int smaller = (bw1<bw2)?bw1:bw2;
	if (abw && abw<smaller)  // we are assigning to smaller quantity!
	  bw_to_be_passed=smaller;
	else 
	  bw_to_be_passed=smaller+1; //the smaller of the 2, +1
	return((bw_to_be_passed)*ADD);
      }   
    case '%': {}       // fall through to div
    case '/': 
      {
	if (c1||c2) ;  // fall through to mult
	else return (DIV*bw1*bw2);
      }
    case '*': 
      {
	int needed_bw = bw1+bw2-2; //this is the required bw of the result
	int needed_area;
	int i = ADD*bw1*bw2;
	if (c1 || c2) 
	  needed_area= ((int) floor(i/2)); // (assumes const is half 1s half 0s) 
	else 
	  needed_area=i;
	if(abw&&(abw<needed_bw))//we have dead bits
	  return ((int) floor (needed_area*abw)/needed_bw); 
	            //above: claiming that needed_area:needed_bw=X_area:assigned_bw
	else 
	  return needed_area;
      }
    case '&':      // bitwise logic cases
    case '|':
    case '^':
    case LOGIC_AND:
    case LOGIC_OR:
    case NOT_EQUALS: 
    case EQUALS: return ((int) ceil(LOGIC*((bw1>bw2)?bw1:bw2)));
    case LEFT_SHIFT: 
    case RIGHT_SHIFT: 
      if (c2) return 0; //free
      else{ 
	int mult = bw1*bw2;
	return ((c1)?mult/2:mult); // /2 because I imagine two muxes in a lut, 
	                           // if number to be shifted is constant
	// if abw!= 0 we save very little. not considered
      }
    }
  }
  else //UOP
    switch(bop)
    {
    case '-':
    case '~':
    case '!': 
      if(abw&&abw<bw1)
	return (LOGIC*abw);
      else
	return (LOGIC*bw1);
    case '+': return 0;
    }
  return 0;
}








