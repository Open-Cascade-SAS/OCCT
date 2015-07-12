// Created on: 1999-12-16
// Created by: Atelier CAS2000
// Copyright (c) 1999-1999 Matra Datavision
// Copyright (c) 1999-2014 OPEN CASCADE SAS
//
// This file is part of Open CASCADE Technology software library.
//
// This library is free software; you can redistribute it and/or modify it under
// the terms of the GNU Lesser General Public License version 2.1 as published
// by the Free Software Foundation, with special exception defined in the file
// OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
// distribution for complete text of the license and disclaimer of any warranty.
//
// Alternatively, this file may be used under the terms of Open CASCADE
// commercial license or contractual agreement.


#include <IntPatch_PrmPrmIntersection_T3Bits.hxx>

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
