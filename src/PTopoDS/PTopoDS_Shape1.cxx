// File:	PTopoDS_Shape1.cxx
//      	------------------
// Author:	DAUTRY Philippe
//		<fid@fox.paris1.matra-dtv.fr>
// Copyright:	Matra Datavision 1998

// Version:	0.0
// History:	Version	Date		Purpose
//		0.0	Feb  4 1998	Creation



#include <PTopoDS_Shape1.ixx>

//=======================================================================
//function : PTopoDS_Shape1
//purpose  : 
//=======================================================================

PTopoDS_Shape1::PTopoDS_Shape1()
: myOrient(TopAbs_FORWARD)
{}


//=======================================================================
//function : Nullify
//purpose  : 
//=======================================================================

void PTopoDS_Shape1::Nullify()
{
  myTShape.Nullify();
  myLocation = PTopLoc_Location();
  myOrient = TopAbs_FORWARD;
}

//=======================================================================
//function : TShape
//purpose  : 
//=======================================================================

const Handle(PTopoDS_TShape1)&  PTopoDS_Shape1::TShape() const 
{ return myTShape; }


//=======================================================================
//function : TShape
//purpose  : 
//=======================================================================

void  PTopoDS_Shape1::TShape(const Handle(PTopoDS_TShape1)& T)
{ myTShape = T; }


//=======================================================================
//function : Location
//purpose  : 
//=======================================================================

PTopLoc_Location  PTopoDS_Shape1::Location() const 
{ return myLocation; }


//=======================================================================
//function : Location
//purpose  : 
//=======================================================================

void  PTopoDS_Shape1::Location(const PTopLoc_Location& L)
{ myLocation = L; }


//=======================================================================
//function : Orientation
//purpose  : 
//=======================================================================

TopAbs_Orientation  PTopoDS_Shape1::Orientation() const 
{ return myOrient; }


//=======================================================================
//function : Orientation
//purpose  : 
//=======================================================================

void  PTopoDS_Shape1::Orientation(const TopAbs_Orientation O)
{ myOrient = O; }
