// Created on: 1995-09-01
// Created by: Bruno DUMORTIER
// Copyright (c) 1995-1999 Matra Datavision
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


#include <BRepFill_OffsetAncestors.hxx>
#include <BRepFill_OffsetWire.hxx>
#include <StdFail_NotDone.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Shape.hxx>
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
  StdFail_NotDone_Raise_if (!myIsPerform, "BRepFill_OffsetAncestors::Ancestor() - Perform() should be called before accessing results");
  return myMap(S1);
}
