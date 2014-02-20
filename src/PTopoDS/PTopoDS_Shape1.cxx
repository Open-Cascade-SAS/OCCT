// Created by: DAUTRY Philippe
// Copyright (c) 1998-1999 Matra Datavision
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

//      	------------------

// Version:	0.0
//Version	Date		Purpose
//		0.0	Feb  4 1998	Creation



#include <PTopoDS_Shape1.ixx>

//=======================================================================
//function : PTopoDS_Shape1
//purpose  : 
//=======================================================================

PTopoDS_Shape1::PTopoDS_Shape1() : myOrient(TopAbs_FORWARD)
{
}


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
