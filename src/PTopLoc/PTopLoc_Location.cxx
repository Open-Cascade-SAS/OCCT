// File:	PTopLoc_Location.cxx
// Created:	Wed Mar  3 18:25:03 1993
// Author:	Remi LEQUETTE
//		<rle@phobox>


#include <PTopLoc_Location.ixx>
#include <Standard_NoSuchObject.hxx>

//=======================================================================
//function : PTopLoc_Location
//purpose  : 
//=======================================================================

PTopLoc_Location::PTopLoc_Location() 
{
}


//=======================================================================
//function : PTopLoc_Location
//purpose  : 
//=======================================================================

PTopLoc_Location::PTopLoc_Location(const Handle(PTopLoc_Datum3D)& D,
				   const Standard_Integer P,
				   const PTopLoc_Location& N) 
{
  myData = new PTopLoc_ItemLocation(D,P,N);
}


//=======================================================================
//function : IsIdentity
//purpose  : 
//=======================================================================

Standard_Boolean PTopLoc_Location::IsIdentity() const 
{
  return myData.IsNull();
}

//=======================================================================
//function : Datum3D
//purpose  : 
//=======================================================================

Handle(PTopLoc_Datum3D)  PTopLoc_Location::Datum3D()const 
{
  Standard_NoSuchObject_Raise_if(IsIdentity(),"PTopLoc_Location::Datum3D");
  return myData->Datum3D();
}


//=======================================================================
//function : Power
//purpose  : 
//=======================================================================

Standard_Integer  PTopLoc_Location::Power()const 
{
  Standard_NoSuchObject_Raise_if(IsIdentity(),"PTopLoc_Location::Power");
  return myData->Power();
}


//=======================================================================
//function : PTopLoc_Location
//purpose  : 
//=======================================================================

PTopLoc_Location PTopLoc_Location::Next() const
{
  Standard_NoSuchObject_Raise_if(IsIdentity(),"PTopLoc_Location::Next");
  return myData->Next();
}



