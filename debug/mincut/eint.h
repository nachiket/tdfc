// extended int:  {non-negative ints, infinity(-1)}

class eint
{
private:
  int v;

public:
  eint(int i) : v((i>=0)?i:-1) {}
  int() {return v;}
  eint operator+ (eint e) {return (v!=-1&&e.v!=-1)?eint(v+e.v):eint(-1);}
  int operator<  (int i)  {return (v!=-1)?(v<i):0;}
  int operator>  (int i)  {return (v!=-1)?(v>i):1;}
  int operator<= (int i)  {return (v!=-1)?(v<=i): 0;}
  int operator>= (int i)  {return (v!=-1)?(v>=i):-1;}
  int operator<  (eint e) {return (v!=-1&&e.v!=-1)?(v<e.v):(v!=-1)?1:0;}
  int operator>  (eint e) {return (v!=-1&&e.v!=-1)?(v>e.v):(v!=-1)?0:1;}
  int operator<= (eint e) {return (v!=-1&&e.v!=-1)?(v<=e.v):
			          (v==-1&&e.v==-1)?1       :(v!=-1)?1:0;}
  int operator>= (eint e) {return (v!=-1&&e.v!=-1)?(v<=e.v):
			          (v==-1&&e.v==-1)?1       :(v!=-1)?0:1;}
}
