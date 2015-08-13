// Created on: 1993-07-23
// Created by: Remi LEQUETTE
// Copyright (c) 1993-1999 Matra Datavision
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


#include <BRepBuilderAPI_MakeSolid.hxx>
#include <StdFail_NotDone.hxx>
#include <TopoDS.hxx>
#include <TopoDS_CompSolid.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Shell.hxx>
#include <TopoDS_Solid.hxx>

//=======================================================================
//function : BRepBuilderAPI_MakeSolid
//purpose  : 
//=======================================================================
BRepBuilderAPI_MakeSolid::BRepBuilderAPI_MakeSolid()
{
}

//=======================================================================
//function : BRepBuilderAPI_MakeSolid
//purpose  : 
//=======================================================================

BRepBuilderAPI_MakeSolid::BRepBuilderAPI_MakeSolid(const TopoDS_CompSolid& S)
: myMakeSolid(S)
{
  if ( myMakeSolid.IsDone()) {
    Done();
    myShape = myMakeSolid.Shape();
  }
}


//=======================================================================
//function : BRepBuilderAPI_MakeSolid
//purpose  : 
//=======================================================================

BRepBuilderAPI_MakeSolid::BRepBuilderAPI_MakeSolid(const TopoDS_Shell& S)
: myMakeSolid(S)
{
  if ( myMakeSolid.IsDone()) {
    Done();
    myShape = myMakeSolid.Shape();
  }
}


//=======================================================================
//function : BRepBuilderAPI_MakeSolid
//purpose  : 
//=======================================================================

BRepBuilderAPI_MakeSolid::BRepBuilderAPI_MakeSolid(const TopoDS_Shell& S1, 
				     const TopoDS_Shell& S2)
: myMakeSolid(S1,S2)
{
  if ( myMakeSolid.IsDone()) {
    Done();
    myShape = myMakeSolid.Shape();
  }
}


//=======================================================================
//function : BRepBuilderAPI_MakeSolid
//purpose  : 
//=======================================================================

BRepBuilderAPI_MakeSolid::BRepBuilderAPI_MakeSolid(const TopoDS_Shell& S1, 
				     const TopoDS_Shell& S2, 
				     const TopoDS_Shell& S3)
: myMakeSolid(S1,S2,S3)
{
  if ( myMakeSolid.IsDone()) {
    Done();
    myShape = myMakeSolid.Shape();
  }
}


//=======================================================================
//function : BRepBuilderAPI_MakeSolid
//purpose  : 
//=======================================================================

BRepBuilderAPI_MakeSolid::BRepBuilderAPI_MakeSolid(const TopoDS_Solid& So)
: myMakeSolid(So)
{
  if ( myMakeSolid.IsDone()) {
    Done();
    myShape = myMakeSolid.Shape();
  }
}


//=======================================================================
//function : BRepBuilderAPI_MakeSolid
//purpose  : 
//=======================================================================

BRepBuilderAPI_MakeSolid::BRepBuilderAPI_MakeSolid(const TopoDS_Solid& So, 
				     const TopoDS_Shell& S)
: myMakeSolid(So,S)
{
  if ( myMakeSolid.IsDone()) {
    Done();
    myShape = myMakeSolid.Shape();
  }
}


//=======================================================================
//function : Add
//purpose  : 
//=======================================================================

void  BRepBuilderAPI_MakeSolid::Add(const TopoDS_Shell& S)
{
  myMakeSolid.Add(S);
  if ( myMakeSolid.IsDone()) {
    Done();
    myShape = myMakeSolid.Shape();
  }
}

//=======================================================================
//function : IsDone
//purpose  : 
//=======================================================================

Standard_Boolean BRepBuilderAPI_MakeSolid::IsDone() const
{
  return myMakeSolid.IsDone();
}


//=======================================================================
//function : Solid
//purpose  : 
//=======================================================================

const TopoDS_Solid&  BRepBuilderAPI_MakeSolid::Solid()
{
  return myMakeSolid.Solid();
}



//=======================================================================
//function : operator
//purpose  : 
//=======================================================================

BRepBuilderAPI_MakeSolid::operator TopoDS_Solid()
{
  return Solid();
}

//=======================================================================
//function : IsDeleted
//purpose  : 
//=======================================================================

Standard_Boolean BRepBuilderAPI_MakeSolid::IsDeleted (const TopoDS_Shape& S) 

{
  if(S.ShapeType() == TopAbs_FACE) {
    BRepLib_ShapeModification aStatus = myMakeSolid.FaceStatus(TopoDS::Face(S));
   
    if(aStatus == BRepLib_Deleted) return Standard_True;

  }
    
  return Standard_False;
}

