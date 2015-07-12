// Created on: 1993-11-10
// Created by: Jean Marc LACHAUME
// Copyright (c) 1993-1999 Matra Datavision
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


#include <HatchGen_Domain.hxx>
#include <HatchGen_PointOnHatching.hxx>
#include <Standard_DomainError.hxx>
#include <Standard_Stream.hxx>

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
