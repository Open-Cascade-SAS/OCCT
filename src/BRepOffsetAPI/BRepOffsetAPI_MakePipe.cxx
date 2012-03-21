// Created on: 1994-07-12
// Created by: Bruno DUMORTIER
// Copyright (c) 1994-1999 Matra Datavision
// Copyright (c) 1999-2012 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.


#include <BRepOffsetAPI_MakePipe.ixx>

#include <TopExp_Explorer.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopoDS.hxx>
#include <TopAbs_ShapeEnum.hxx>

//=======================================================================
//function : BRepOffsetAPI_MakePipe
//purpose  : 
//=======================================================================

BRepOffsetAPI_MakePipe::BRepOffsetAPI_MakePipe(const TopoDS_Wire&  Spine ,
				   const TopoDS_Shape& Profile)
     : myPipe(Spine, Profile)
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
//purpose  : 
//=======================================================================

TopoDS_Shape BRepOffsetAPI_MakePipe::Generated (const TopoDS_Shape& SSpine,
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

