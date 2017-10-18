// Created on: 1998-07-22
// Created by: Philippe MANGIN
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


#include <BRep_Builder.hxx>
#include <BRepFill_Section.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopoDS_Wire.hxx>
#include <TopoDS_Iterator.hxx>
#include <TopExp_Explorer.hxx>
#include <ShapeUpgrade_RemoveLocations.hxx>

BRepFill_Section::BRepFill_Section() :islaw(0),
                                      ispunctual(0),
                                      contact(0),
                                      correction(0)
{
}


BRepFill_Section::BRepFill_Section(const TopoDS_Shape& Profile,
				   const TopoDS_Vertex& V,
				   const Standard_Boolean WithContact,
				   const Standard_Boolean WithCorrection) 
                                  : vertex(V),
				    islaw(0),
                                    ispunctual(0),
                                    contact(WithContact),
				    correction(WithCorrection)
{
  myOriginalShape = Profile;
  
  ShapeUpgrade_RemoveLocations RemLoc;
  RemLoc.SetRemoveLevel(TopAbs_COMPOUND);
  RemLoc.Remove(Profile);
  TopoDS_Shape aProfile = RemLoc.GetResult();
  
  if (aProfile.ShapeType() == TopAbs_WIRE)
    wire = TopoDS::Wire(aProfile);
  else if (aProfile.ShapeType() == TopAbs_VERTEX)
    {
      ispunctual = Standard_True;
      TopoDS_Vertex aVertex = TopoDS::Vertex(aProfile);
      BRep_Builder BB;
      
      TopoDS_Edge DegEdge;
      BB.MakeEdge( DegEdge );
      BB.Add( DegEdge, aVertex.Oriented(TopAbs_FORWARD) );
      BB.Add( DegEdge, aVertex.Oriented(TopAbs_REVERSED) );
      BB.Degenerated( DegEdge, Standard_True );
      
      BB.MakeWire( wire );
      BB.Add( wire, DegEdge );
      wire.Closed( Standard_True );
    }
  else
    throw Standard_Failure("BRepFill_Section: bad shape type of section");
}

void BRepFill_Section::Set(const Standard_Boolean IsLaw)
{
  islaw = IsLaw;
}

TopoDS_Shape BRepFill_Section::ModifiedShape(const TopoDS_Shape& theShape) const
{
  TopoDS_Shape aModifiedShape;
  
  switch (theShape.ShapeType())
  {
  case TopAbs_WIRE:
    if (theShape.IsSame(myOriginalShape))
      aModifiedShape = wire;
    break;
  case TopAbs_EDGE:
    {
      TopoDS_Iterator itor(myOriginalShape);
      TopoDS_Iterator itw(wire);
      for (; itor.More(); itor.Next(),itw.Next())
      {
        const TopoDS_Shape& anOriginalEdge = itor.Value();
        const TopoDS_Shape& anEdge         = itw.Value();
        if (anOriginalEdge.IsSame(theShape))
        {
          aModifiedShape = anEdge;
          break;
        }
      }
    }
    break;
  case TopAbs_VERTEX:
    if (theShape.IsSame(myOriginalShape))
    {
      TopExp_Explorer Explo(wire, TopAbs_VERTEX);
      aModifiedShape = Explo.Current();
    }
    else
    {
      TopExp_Explorer ExpOrig(myOriginalShape, TopAbs_VERTEX);
      TopExp_Explorer ExpWire(wire, TopAbs_VERTEX);
      for (; ExpOrig.More(); ExpOrig.Next(),ExpWire.Next())
      {
        const TopoDS_Shape& anOriginalVertex = ExpOrig.Current();
        const TopoDS_Shape& aVertex          = ExpWire.Current();
        if (anOriginalVertex.IsSame(theShape))
        {
          aModifiedShape = aVertex;
          break;
        }
      }
    }
    break;
  default:
    break;
  }

  return aModifiedShape;
}
