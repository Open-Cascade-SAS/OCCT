// File:	PTopLoc_ItemLocation.cxx
// Created:	Wed Mar  3 18:22:47 1993
// Author:	Remi LEQUETTE
//		<rle@phobox>


#include <PTopLoc_ItemLocation.ixx>

//=======================================================================
//function : PTopLoc_ItemLocation
//purpose  : 
//=======================================================================

PTopLoc_ItemLocation::PTopLoc_ItemLocation(const Handle(PTopLoc_Datum3D)& D, 
					   const Standard_Integer P,
					   const PTopLoc_Location& N) :
       myDatum(D),
       myPower(P),
       myNext(N)
{
}


//=======================================================================
//function : Datum3D
//purpose  : 
//=======================================================================

Handle(PTopLoc_Datum3D)  PTopLoc_ItemLocation::Datum3D()const 
{
  return myDatum;
}


//=======================================================================
//function : Power
//purpose  : 
//=======================================================================

Standard_Integer  PTopLoc_ItemLocation::Power()const 
{
  return myPower;
}


//=======================================================================
//function : Next
//purpose  : 
//=======================================================================

PTopLoc_Location  PTopLoc_ItemLocation::Next()const 
{
  return myNext;
}


