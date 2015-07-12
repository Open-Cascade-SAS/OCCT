// Created on: 1998-09-16
// Created by: Roman LYGIN
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

// 25.12.98 pdn: renaming methods GetWires and GetEdges to GetClosedWires
//               and GetOpenWires respectively

#include <BRep_Builder.hxx>
#include <ShapeAnalysis_FreeBounds.hxx>
#include <ShapeExtend_Explorer.hxx>
#include <ShapeFix_FreeBounds.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Compound.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Iterator.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopTools_DataMapOfShapeShape.hxx>

//=======================================================================
//function : ShapeFix_FreeBounds
//purpose  : 
//=======================================================================
ShapeFix_FreeBounds::ShapeFix_FreeBounds() {}

//=======================================================================
//function : ShapeFix_FreeBounds
//purpose  : 
//=======================================================================

ShapeFix_FreeBounds::ShapeFix_FreeBounds(const TopoDS_Shape& shape,
					 const Standard_Real sewtoler,
					 const Standard_Real closetoler,
					 const Standard_Boolean splitclosed,
					 const Standard_Boolean splitopen) :
       myShared (Standard_False), mySewToler (sewtoler), myCloseToler (closetoler),
       mySplitClosed (splitclosed), mySplitOpen (splitopen)
{
  myShape = shape;
  Perform();
}

//=======================================================================
//function : ShapeFix_FreeBounds
//purpose  : 
//=======================================================================

ShapeFix_FreeBounds::ShapeFix_FreeBounds(const TopoDS_Shape& shape,
					 const Standard_Real closetoler,
					 const Standard_Boolean splitclosed,
					 const Standard_Boolean splitopen):
       myShared (Standard_True), mySewToler (0.), myCloseToler (closetoler),
       mySplitClosed (splitclosed), mySplitOpen (splitopen)
{
  myShape = shape;
  Perform();
}

//=======================================================================
//function : Perform
//purpose  : 
//=======================================================================

Standard_Boolean ShapeFix_FreeBounds::Perform() 
{
  ShapeAnalysis_FreeBounds safb;
  if (myShared)
    safb = ShapeAnalysis_FreeBounds (myShape, mySplitClosed, mySplitOpen);
  else
    safb = ShapeAnalysis_FreeBounds (myShape, mySewToler, mySplitClosed, mySplitOpen);
  
  myWires = safb.GetClosedWires();
  myEdges = safb.GetOpenWires();
  
  if (myCloseToler > mySewToler) {
    ShapeExtend_Explorer see;
    Handle(TopTools_HSequenceOfShape) newwires,
                                      open = see.SeqFromCompound (myEdges,
								  Standard_False);
    TopTools_DataMapOfShapeShape vertices;
    ShapeAnalysis_FreeBounds::ConnectWiresToWires (open, myCloseToler, myShared,
						   newwires, vertices);
    myEdges.Nullify();
    ShapeAnalysis_FreeBounds::DispatchWires (newwires, myWires, myEdges);
    
    for( TopExp_Explorer exp (myShape, TopAbs_EDGE); exp.More(); exp.Next()) {
      TopoDS_Edge Edge = TopoDS::Edge(exp.Current());
      for( TopoDS_Iterator iter (Edge); iter.More(); iter.Next()) {
	TopoDS_Vertex V = TopoDS::Vertex (iter.Value());
	BRep_Builder B;
	TopoDS_Vertex newV;
	if( vertices.IsBound(V)) {
	  newV = TopoDS::Vertex (vertices.Find(V));
	  newV.Orientation(V.Orientation());
	  B.Remove(Edge, V);
	  B.Add(Edge, newV);
	}
      }
    }
  }
  return Standard_True;
}
