// File:      IntPatch_PrmPrmIntersection_T3Bits.cxx
// Created:   Thu Dec 16 16:34:03 1999
// Author:    Atelier CAS2000
// Copyright: OPEN CASCADE 1999

#include <IntPatch_PrmPrmIntersection_T3Bits.ixx>

IntPatch_PrmPrmIntersection_T3Bits::IntPatch_PrmPrmIntersection_T3Bits(const Standard_Integer size)
{
  //-- ex: size=4  -> 4**3 = 64 bits -> 2 mots 32bits
  Standard_Integer nb = (size*size*size)>>5;
  Isize = nb;
  p = new Standard_Integer [nb];
  do { ((Standard_Integer *) p)[--nb]=0; } while(nb);
}

void IntPatch_PrmPrmIntersection_T3Bits::Destroy()
{
  if(p) { delete[] ((Standard_Integer*)p); p=NULL; }
}

void IntPatch_PrmPrmIntersection_T3Bits::ResetAnd()
{
  //ind = 0;
}

Standard_Integer IntPatch_PrmPrmIntersection_T3Bits::And(IntPatch_PrmPrmIntersection_T3Bits& Oth,
                                                         Standard_Integer& indice)
{
  int k=indice>>5;
  Standard_Integer r=0;
  while(k<Isize)
  {
    Standard_Integer r=((Standard_Integer *) p)[k] & ((Standard_Integer *) Oth.p)[k];
    if(r)
    {
      unsigned long int c=0;
      do
      {
        if(r&1)
        {
          const Standard_Integer op = (k<<5)|(c);
          Raz(op);
          Oth.Raz(op);
          indice = op;
          return(1);
        }
        c++;
        r>>=1;
      }
      while(c<32);
    }
    k++;
  }
  return(0);
}
