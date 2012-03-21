// Created on: 1991-01-21
// Created by: Christophe MARION
// Copyright (c) 1991-1999 Matra Datavision
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

