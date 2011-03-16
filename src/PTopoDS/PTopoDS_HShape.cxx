// File:	PTopoDS_HShape.cxx
// Created:	Tue Mar  9 14:04:14 1993
// Author:	Remi LEQUETTE
//		<rle@phobox>

#include <PTopoDS_HShape.ixx>

//=======================================================================
//function : PTopoDS_Shape
//purpose  : 
//=======================================================================

PTopoDS_HShape::PTopoDS_HShape() :
       myOrient(TopAbs_FORWARD)
{
}


//=======================================================================
//function : TShape
//purpose  : 
//=======================================================================

Handle(PTopoDS_TShape)  PTopoDS_HShape::TShape()const 
{
  return myTShape;
}


//=======================================================================
//function : TShape
//purpose  : 
//=======================================================================

void  PTopoDS_HShape::TShape(const Handle(PTopoDS_TShape)& T)
{
  myTShape = T;
}


//=======================================================================
//function : Location
//purpose  : 
//=======================================================================

PTopLoc_Location  PTopoDS_HShape::Location()const 
{
  return myLocation;
}


//=======================================================================
//function : Location
//purpose  : 
//=======================================================================

void  PTopoDS_HShape::Location(const PTopLoc_Location& L)
{
  myLocation = L;
}


//=======================================================================
//function : Orientation
//purpose  : 
//=======================================================================

TopAbs_Orientation  PTopoDS_HShape::Orientation()const 
{
  return myOrient;
}


//=======================================================================
//function : Orientation
//purpose  : 
//=======================================================================

void  PTopoDS_HShape::Orientation(const TopAbs_Orientation O)
{
  myOrient = O;
}







