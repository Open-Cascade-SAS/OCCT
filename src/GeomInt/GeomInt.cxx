// File:      GeomInt.cxx
// Created:   25.08.14 17:59:59
// Author:    jgv@VIVEX
// Copyright: Open CASCADE 2014

#include <GeomInt.hxx>
#include <TColStd_IndexedMapOfInteger.hxx>

//=======================================================================
//function : AdjustPeriodic
//purpose  : 
//=======================================================================
Standard_Boolean GeomInt::AdjustPeriodic(const Standard_Real thePar,
                                         const Standard_Real theParMin,
                                         const Standard_Real theParMax,
                                         const Standard_Real thePeriod,
                                         Standard_Real &theNewPar,
                                         Standard_Real &theOffset,
                                         const Standard_Real theEps)
{
  Standard_Boolean bMin, bMax;
  //
  theOffset = 0.;
  theNewPar = thePar;
  bMin = theParMin - thePar > theEps;
  bMax = thePar - theParMax > theEps;
  //
  if (bMin || bMax) {
    Standard_Real dp, aNbPer;
    //
    dp = (bMin) ? (theParMax - thePar) : (theParMin - thePar);
    modf(dp / thePeriod, &aNbPer);
    //
    theOffset = aNbPer * thePeriod;
    theNewPar += theOffset;
  }
  //
  return (theOffset > 0.);
}
