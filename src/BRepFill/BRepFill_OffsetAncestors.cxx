// Created on: 1995-09-01
// Created by: Bruno DUMORTIER
// Copyright (c) 1995-1999 Matra Datavision
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



#include <BRepFill_OffsetAncestors.ixx>

#include <TopExp_Explorer.hxx>
#include <TopoDS_Face.hxx>
#include <TopTools_ListIteratorOfListOfShape.hxx>

//=======================================================================
//function : BRepFill_OffsetAncestors
//purpose  : 
//=======================================================================

BRepFill_OffsetAncestors::BRepFill_OffsetAncestors()
:myIsPerform(Standard_False)
{
}


//=======================================================================
//function : BRepFill_OffsetAncestors
//purpose  : 
//=======================================================================

BRepFill_OffsetAncestors::BRepFill_OffsetAncestors
(BRepFill_OffsetWire& Paral)
{
  Perform(Paral);
}


//=======================================================================
//function : Perform
//purpose  : 
//=======================================================================

void BRepFill_OffsetAncestors::Perform(BRepFill_OffsetWire& Paral)
{
  TopoDS_Face Spine = Paral.Spine();
  
  TopExp_Explorer                    Exp;
  TopTools_ListIteratorOfListOfShape it;

  // on itere sur les edges.
  for ( Exp.Init(Spine, TopAbs_EDGE); Exp.More(); Exp.Next()) {
    for (it.Initialize(Paral.GeneratedShapes(Exp.Current()));
	 it.More(); it.Next()) {
      myMap.Bind( it.Value(), Exp.Current());
    }
  }

  // on itere sur les vertex.
  for ( Exp.Init(Spine, TopAbs_VERTEX); Exp.More(); Exp.Next()) {
    for (it.Initialize(Paral.GeneratedShapes(Exp.Current()));
	 it.More(); it.Next()) {
      myMap.Bind( it.Value(), Exp.Current());
    }
  }

  myIsPerform = Standard_True;
}

//=======================================================================
//function : IsDone
//purpose  : 
//=======================================================================

Standard_Boolean BRepFill_OffsetAncestors::IsDone() const
{
  return myIsPerform;
}

//=======================================================================
//function : IsDone
//purpose  : 
//=======================================================================

Standard_Boolean BRepFill_OffsetAncestors:: HasAncestor(const TopoDS_Edge& S1)
const
{
  return myMap.IsBound(S1);
}

//=======================================================================
//function : TopoDS_Shape&
//purpose  : 
//=======================================================================

const TopoDS_Shape& BRepFill_OffsetAncestors::Ancestor(const TopoDS_Edge& S1)
const
{
  StdFail_NotDone_Raise_if( !myIsPerform, "");
  return myMap(S1);
}
