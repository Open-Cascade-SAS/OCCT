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


#include <BRepOffsetAPI_MakeDraft.hxx>
#include <Geom_Surface.hxx>
#include <gp_Dir.hxx>
#include <Standard_NoSuchObject.hxx>
#include <StdFail_NotDone.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Shell.hxx>

BRepOffsetAPI_MakeDraft::BRepOffsetAPI_MakeDraft(const TopoDS_Shape& Shape,
				     const gp_Dir& Dir,
				     const Standard_Real Angle)
                               : myDraft( Shape, Dir, Angle)
{
  NotDone();
}

 void BRepOffsetAPI_MakeDraft::SetOptions(const BRepBuilderAPI_TransitionMode Style,
				    const Standard_Real AngleMin, 
				    const Standard_Real AngleMax)
{
  BRepFill_TransitionStyle style =  BRepFill_Right;
  if (Style == BRepBuilderAPI_RoundCorner) style =  BRepFill_Round;
  myDraft.SetOptions( style, AngleMin,  AngleMax);
}

 void BRepOffsetAPI_MakeDraft::SetDraft(const Standard_Boolean IsInternal)
{
   myDraft.SetDraft(IsInternal);
}

 void BRepOffsetAPI_MakeDraft::Perform(const Standard_Real LengthMax) 
{
  myDraft.Perform( LengthMax);
  if (myDraft.IsDone()) {
    Done();
    myShape = myDraft.Shape();
  }
}

 void BRepOffsetAPI_MakeDraft::Perform(const Handle(Geom_Surface)& Surface,
				 const Standard_Boolean KeepInsideSurface) 
{
  myDraft.Perform(Surface, KeepInsideSurface);
  if (myDraft.IsDone()) {
    Done();
    myShape = myDraft.Shape();
  }  
}

 void BRepOffsetAPI_MakeDraft::Perform(const TopoDS_Shape& StopShape,
				 const Standard_Boolean KeepOutSide) 
{
  myDraft.Perform( StopShape, KeepOutSide);
  if (myDraft.IsDone()) {
    Done();
    myShape = myDraft.Shape();
  } 
}

 TopoDS_Shell BRepOffsetAPI_MakeDraft::Shell() const
{
  return myDraft.Shell();
}

const TopTools_ListOfShape& BRepOffsetAPI_MakeDraft::Generated(const TopoDS_Shape& S) 
{
  return myDraft.Generated( S );
}

