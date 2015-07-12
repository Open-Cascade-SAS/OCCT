// Created on: 1996-07-03
// Created by: Joelle CHAUVET
// Copyright (c) 1996-1999 Matra Datavision
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


#include <AdvApprox_PrefCutting.hxx>
#include <Precision.hxx>

AdvApprox_PrefCutting::AdvApprox_PrefCutting(const TColStd_Array1OfReal& CutPnts):
  myPntOfCutting(1, CutPnts.Length()) 
{
  myPntOfCutting = CutPnts;
}

Standard_Boolean AdvApprox_PrefCutting::Value(const Standard_Real a,
					      const Standard_Real b,
					      Standard_Real& cuttingvalue) const
{
//  longueur minimum d'un intervalle parametrique : PConfusion()
//                                    pour F(U,V) : EPS1=1.e-9 (cf.MMEPS1)
  Standard_Real lgmin = 10 * Precision::PConfusion();
  Standard_Integer i;
  Standard_Real cut, mil=(a+b)/2,
                dist = Abs((a-b)/2);
  cut = mil;
  for ( i=myPntOfCutting.Lower(); i<= myPntOfCutting.Upper(); i++) {
    if ((dist-lgmin) > Abs(mil-myPntOfCutting.Value(i))) {
      cut = myPntOfCutting.Value(i);
      dist = Abs(mil-myPntOfCutting.Value(i));
    }
  }
  cuttingvalue = cut;
  return (Abs(cut-a)>=lgmin && Abs(b-cut)>=lgmin);
}
