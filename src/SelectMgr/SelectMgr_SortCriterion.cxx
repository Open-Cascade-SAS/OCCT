// Created on: 1998-03-26
// Created by: Robert COUBLANC
// Copyright (c) 1998-1999 Matra Datavision
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



#include <SelectMgr_SortCriterion.ixx>
#include <Precision.hxx>

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
