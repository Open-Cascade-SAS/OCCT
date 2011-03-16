// File:	ShapeFix_FreeBounds.cxx
// Created:	Wed Sep 16 17:27:59 1998
// Author:	Roman LYGIN <rln@nnov.matra-dtv.fr>
//              Pavel DURANDIN <pdn@nnov.matra-dtv.fr>
// 25.12.98 pdn: renaming methods GetWires and GetEdges to GetClosedWires
//               and GetOpenWires respectively

#include <ShapeFix_FreeBounds.ixx>

#include <BRep_Builder.hxx>

#include <TopoDS.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Iterator.hxx>
#include <TopExp_Explorer.hxx>
#include <TopTools_DataMapOfShapeShape.hxx>

#include <ShapeExtend_Explorer.hxx>
#include <ShapeAnalysis_FreeBounds.hxx>

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
