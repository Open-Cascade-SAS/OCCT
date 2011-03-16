// File:	TopLoc_ItemLocation.cxx
// Created:	Mon Jan 21 15:24:04 1991
// Author:	Christophe MARION
//		<cma@topsn3>

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

