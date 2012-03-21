// Created on: 1993-11-10
// Created by: Jean Marc LACHAUME
// Copyright (c) 1993-1999 Matra Datavision
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


#include <Standard_Stream.hxx>
#include <HatchGen_Domain.ixx>

//=======================================================================
// Function : HatchGen_Domain
// Purpose  : Constructor.
//=======================================================================

HatchGen_Domain::HatchGen_Domain () :
       myHasFirstPoint  (Standard_False) ,
       myHasSecondPoint (Standard_False)
{
}

//=======================================================================
// Function : HatchGen_Domain
// Purpose  : Constructor.
//=======================================================================

HatchGen_Domain::HatchGen_Domain (const HatchGen_PointOnHatching& P1,
				  const HatchGen_PointOnHatching& P2) :
       myHasFirstPoint  (Standard_True) ,
       myFirstPoint     (P1),
       myHasSecondPoint (Standard_True) ,
       mySecondPoint    (P2)
{
}

//=======================================================================
// Function : HatchGen_Domain
// Purpose  : Constructor.
//=======================================================================

HatchGen_Domain::HatchGen_Domain (const HatchGen_PointOnHatching& P,
				  const Standard_Boolean First)
{
  if (First) {
    myHasFirstPoint  = Standard_True ;
    myHasSecondPoint = Standard_False ;
    myFirstPoint     = P ;
  } else {
    myHasFirstPoint  = Standard_False ;
    myHasSecondPoint = Standard_True ;
    mySecondPoint    = P ;
  }
}


//=======================================================================
// Function : Dump
// Purpose  : Dump of the domain.
//=======================================================================

void HatchGen_Domain::Dump (const Standard_Integer Index) const
{
  cout << "=== Domain " ;
  if (Index > 0) {
    cout << "# " << setw(3) << Index << " " ;
  } else {
    cout << "======" ;
  }
  cout << "=============================" << endl ;

  if (myHasFirstPoint) {
    myFirstPoint.Dump (1) ;
  } else {
    cout << "    Has not a first point" << endl ;
  }

  if (myHasSecondPoint) {
    mySecondPoint.Dump (2) ;
  } else {
    cout << "    Has not a second point" << endl ;
  }

  cout << "==============================================" << endl ;
}
