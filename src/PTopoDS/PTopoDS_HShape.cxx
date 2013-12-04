// Created on: 1993-03-09
// Created by: Remi LEQUETTE
// Copyright (c) 1993-1999 Matra Datavision
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







