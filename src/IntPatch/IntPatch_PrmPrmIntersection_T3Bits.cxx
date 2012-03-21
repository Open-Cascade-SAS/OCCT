// Created on: 1999-12-16
// Created by: Atelier CAS2000
// Copyright (c) 1999-1999 Matra Datavision
// Copyright (c) 1999-2012 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.


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
