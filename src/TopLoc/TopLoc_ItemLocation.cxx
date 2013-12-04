// Created on: 1991-01-21
// Created by: Christophe MARION
// Copyright (c) 1991-1999 Matra Datavision
// Copyright (c) 1999-2014 OPEN CASCADE SAS
//
// This file is part of Open CASCADE Technology software library.
//
// This library is free software; you can redistribute it and / or modify it
// under the terms of the GNU Lesser General Public version 2.1 as published
// by the Free Software Foundation, with special exception defined in the file
// OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
// distribution for complete text of the license and disclaimer of any warranty.
//
// Alternatively, this file may be used under the terms of Open CASCADE
// commercial license or contractual agreement.

#include <TopLoc_ItemLocation.ixx>

//=======================================================================
//function : TopLoc_ItemLocation
//purpose  : constructor
//=======================================================================

TopLoc_ItemLocation::TopLoc_ItemLocation 
  (const Handle(TopLoc_Datum3D)& D, 
   const Standard_Integer P,
//   const Standard_Boolean fromtrsf) :
   const Standard_Boolean ) :
  myDatum(D),
  myPower(P),
  myTrsf(NULL)
{
}


TopLoc_ItemLocation::TopLoc_ItemLocation(const TopLoc_ItemLocation& anOther): myTrsf(NULL)
{
  if (anOther.myTrsf != NULL) {
    myTrsf = new gp_Trsf;  
    *myTrsf = *(anOther.myTrsf);
  }
  myDatum = anOther.myDatum;
  myPower = anOther.myPower;
}

TopLoc_ItemLocation& TopLoc_ItemLocation::Assign(const TopLoc_ItemLocation& anOther)
{
  if (anOther.myTrsf == NULL) {
    if (myTrsf != NULL) delete myTrsf;
    myTrsf = NULL;
  }
  else if (myTrsf != anOther.myTrsf) {
    if (myTrsf == NULL) myTrsf = new gp_Trsf;  
    *myTrsf = *(anOther.myTrsf);
  }
  myDatum = anOther.myDatum;
  myPower = anOther.myPower;

  return *this;
}

void TopLoc_ItemLocation::Destroy()
{
  if (myTrsf != NULL) delete myTrsf;
  myTrsf = NULL;
}

