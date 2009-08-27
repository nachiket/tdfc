// operator ftabmod_noinline
// operator ftabmod

ftabmod_noinline (param unsigned[125][64] ftab, input unsigned[8] ind, output unsigned[8] oval, output unsigned[32] segment_r_addr, input unsigned[64] segment_r_data)
{
  unsigned[64] intermed;

  state only (ind) :
    {
      segment_r_addr=ind;
      goto only_1;
    }

  state only_1 (segment_r_data) :
    {
      intermed=segment_r_data;
      oval=intermed[7:0];
      goto only;
    }
}

ftabmod (param unsigned[125][64] ftab, input unsigned[8] ind, output unsigned[8] oval)
{
  unsigned[32] segment_r_addr;
  unsigned[64] segment_r_data;

  segment_r(64,32,125,ftab,segment_r_addr,segment_r_data);
  ftabmod_noinline(ftab,ind,oval,segment_r_addr,segment_r_data);
}

Douchebag.. where's the graph?
Douchebag.. where's the graph?ftabmod_noinline_d1_ScOrEtMp0
Douchebag.. where's the graph?
Douchebag.. where's the graph?ftabmod_d1_ScOrEtMp1
