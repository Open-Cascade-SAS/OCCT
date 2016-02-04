// Created on: 1994-07-12
// Created by: Bruno DUMORTIER
// Copyright (c) 1994-1999 Matra Datavision
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


#include <BRepFill_Pipe.hxx>
#include <BRepOffsetAPI_MakePipe.hxx>
#include <TopAbs_ShapeEnum.hxx>
#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopoDS_Wire.hxx>
#include <TopTools_IndexedMapOfShape.hxx>

//=======================================================================
//function : BRepOffsetAPI_MakePipe
//purpose  : constructor
//=======================================================================
BRepOffsetAPI_MakePipe::BRepOffsetAPI_MakePipe(const TopoDS_Wire&  Spine ,
                                               const TopoDS_Shape& Profile)
     : myPipe(Spine, Profile)
{
  Build();
}

//=======================================================================
//function : BRepOffsetAPI_MakePipe
//purpose  : constructor
//           Set the mode of sweeping
//           It can be:
//           - Frenet
//           - Corrected Frenet
//           - Discrete Trihedron
//           Also set the flag that indicates attempt to approximate
//           a C1-continuous surface if a swept surface proved
//           to be C0.
//=======================================================================

BRepOffsetAPI_MakePipe::BRepOffsetAPI_MakePipe(const TopoDS_Wire&  Spine ,
                                               const TopoDS_Shape& Profile,
                                               const GeomFill_Trihedron aMode,
                                               const Standard_Boolean ForceApproxC1)
  : myPipe(Spine, Profile, aMode, ForceApproxC1)
{
  Build();
}

//=======================================================================
//function : Pipe
//purpose  : 
//=======================================================================

const BRepFill_Pipe& BRepOffsetAPI_MakePipe::Pipe() const
{
  return myPipe;
}


//=======================================================================
//function : Build
//purpose  : 
//=======================================================================

void BRepOffsetAPI_MakePipe::Build() 
{
  myShape = myPipe.Shape();
  //Check for emptiness of result
  TopTools_IndexedMapOfShape theMap;
  TopExp::MapShapes(myShape, theMap);
  if (theMap.Extent() == 1)
    NotDone();
  else
    Done();
}


//=======================================================================
//function : FirstShape
//purpose  : 
//=======================================================================

TopoDS_Shape BRepOffsetAPI_MakePipe::FirstShape()
{
  return myPipe.FirstShape();
}


//=======================================================================
//function : LastShape
//purpose  : 
//=======================================================================

TopoDS_Shape BRepOffsetAPI_MakePipe::LastShape()
{
  return myPipe.LastShape();
}

//=======================================================================
//function : Generated
//purpose  : standard method
//=======================================================================
const TopTools_ListOfShape& BRepOffsetAPI_MakePipe::Generated(const TopoDS_Shape& S) 
{
  myPipe.Generated(S, myGenerated);
  return myGenerated;
}

//=======================================================================
//function : Generated
//purpose  : returns generated elementary subshape
//=======================================================================

TopoDS_Shape BRepOffsetAPI_MakePipe::Generated(const TopoDS_Shape& SSpine,
                                               const TopoDS_Shape& SProfile)
{
  if (SProfile.ShapeType () == TopAbs_EDGE) {
    return myPipe.Face (TopoDS::Edge (SSpine), TopoDS::Edge (SProfile));
  }
  else if (SProfile.ShapeType () == TopAbs_VERTEX) {
    return myPipe.Edge (TopoDS::Edge (SSpine), TopoDS::Vertex (SProfile));
  }

//POP pour NT
  TopoDS_Shape bid;
  return bid;
}

//=======================================================================
//function : ErrorOnSurface
//purpose  : 
//=======================================================================

Standard_Real BRepOffsetAPI_MakePipe::ErrorOnSurface() const
{
  return myPipe.ErrorOnSurface();
}
