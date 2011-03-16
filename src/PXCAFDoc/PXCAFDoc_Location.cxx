// File:	PXCAFDoc_Location.cxx
// Created:	Tue Aug 15 14:45:19 2000
// Author:	data exchange team
//		<det@strelox.nnov.matra-dtv.fr>


#include <PXCAFDoc_Location.ixx>

//=======================================================================
//function : Constructor
//purpose  : 
//=======================================================================

PXCAFDoc_Location::PXCAFDoc_Location()
{
}

//=======================================================================
//function : Constructor
//purpose  : 
//=======================================================================

PXCAFDoc_Location::PXCAFDoc_Location(const PTopLoc_Location& Loc):
       myPLocation(Loc)
{
}

//=======================================================================
//function : Set
//purpose  : 
//=======================================================================

 void PXCAFDoc_Location::Set(const PTopLoc_Location& Loc) 
{
  myPLocation = Loc;
}

//=======================================================================
//function : Get
//purpose  : 
//=======================================================================

 PTopLoc_Location PXCAFDoc_Location::Get() const
{
  return myPLocation;
}

