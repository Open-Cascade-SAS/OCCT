// Created on: 1998-03-26
// Created by: Robert COUBLANC
// Copyright (c) 1998-1999 Matra Datavision
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


#include <Precision.hxx>
#include <SelectMgr_SortCriterion.hxx>

//=======================================================================
//function : SelectMgr_SortCriterion
//purpose  : Empty constructor
//=======================================================================
SelectMgr_SortCriterion::SelectMgr_SortCriterion()
     : myPrior  (0),
       myDepth  (0.0),
       myDist   (0.0),
       myTol    (0.0),
       myPreferClosest(Standard_True)
{}

//=======================================================================
//function : SelectMgr_SortCriterion
//purpose  : Constructor
//=======================================================================

SelectMgr_SortCriterion::SelectMgr_SortCriterion(const Standard_Integer Prior,
                                                 const Standard_Real Depth,
                                                 const Standard_Real Dist,
                                                 const Standard_Real Tol,
                                                 const Standard_Boolean PreferClosest)
     : myPrior  (Prior),
       myDepth  (Depth),
       myDist   (Dist),
       myTol    (Tol),
       myPreferClosest(PreferClosest)
{}

//=======================================================================
//function : IsGreater
//purpose  :  priorite d'abor, puis profondeur + distance...
//=======================================================================
Standard_Boolean SelectMgr_SortCriterion::IsGreater
                                (const SelectMgr_SortCriterion& SC) const
{
  if ( myPreferClosest )
  {
    // closest object is selected unless difference is within tolerance
    if ( Abs (myDepth - SC.Depth()) > myTol + SC.Tol() )
      return myDepth < SC.Depth();

    // if two objects have similar depth, select the one with higher 
    // priority or, if priorities are equal, one closest to the mouse
    return myPrior > SC.Priority() ? Standard_True :
           myPrior < SC.Priority() ? Standard_False :
           myDist < SC.MinDist();
  }

  // old logic (OCCT version <= 6.3.1)
  if(myPrior>SC.Priority()) return Standard_True;
  if(myPrior<SC.Priority()) return Standard_False;
  if(Abs(myDepth-SC.Depth())<=Precision::Confusion())
    return myDist < SC.MinDist();
  return (myDepth < SC.Depth() );
}

//=======================================================================
//function : IsLower
//purpose  : On n'utilise que les criteres de profondeur et de priorite...
//=======================================================================
Standard_Boolean SelectMgr_SortCriterion::IsLower
                                (const SelectMgr_SortCriterion& SC) const
{
  if ( myPreferClosest )
  {
    // closest object is selected unless difference is within tolerance
    if ( myPreferClosest && Abs (myDepth - SC.Depth()) > myTol + SC.Tol() )
      return myDepth > SC.Depth();

    // if two objects have similar depth, select the one with higher 
    // priority or, if priorities are equal, one closest to the mouse
    return myPrior < SC.Priority() ? Standard_True :
           myPrior > SC.Priority() ? Standard_False :
           myDist > SC.MinDist();
  }

  // old logic (OCCT version <= 6.3.1)
  if(myPrior>SC.Priority()) return Standard_False;
  if(myPrior<SC.Priority()) return Standard_True;
  if(Abs(myDepth-SC.Depth())<=Precision::Confusion())
    return myDist > SC.MinDist();
  return (myDepth > SC.Depth() );
}
