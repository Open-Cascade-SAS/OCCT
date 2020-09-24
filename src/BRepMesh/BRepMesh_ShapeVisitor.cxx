// Created on: 2016-04-07
// Copyright (c) 2016 OPEN CASCADE SAS
// Created by: Oleg AGASHIN
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

#include <BRepMesh_ShapeVisitor.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Wire.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopExp.hxx>
#include <ShapeAnalysis.hxx>
#include <TopExp_Explorer.hxx>
#include <IMeshData_Edge.hxx>
#include <IMeshData_Wire.hxx>
#include <IMeshData_Face.hxx>
#include <ShapeAnalysis_Wire.hxx>
#include <ShapeAnalysis_WireOrder.hxx>
#include <ShapeExtend_WireData.hxx>
#include <Precision.hxx>
#include <IMeshData_Status.hxx>
#include <IMeshTools_Context.hxx>
#include <BRepTools.hxx>

IMPLEMENT_STANDARD_RTTIEXT(BRepMesh_ShapeVisitor, IMeshTools_ShapeVisitor)

//=======================================================================
// Function: Constructor
// Purpose : 
//=======================================================================
BRepMesh_ShapeVisitor::BRepMesh_ShapeVisitor (const Handle (IMeshData_Model)& theModel)
: myModel (theModel),
  myDEdgeMap(1, new NCollection_IncAllocator(IMeshData::MEMORY_BLOCK_SIZE_HUGE))
{
}

//=======================================================================
// Function: Destructor
// Purpose : 
//=======================================================================
BRepMesh_ShapeVisitor::~BRepMesh_ShapeVisitor ()
{
}

//=======================================================================
// Function: Visit (edge)
// Purpose : 
//=======================================================================
void BRepMesh_ShapeVisitor::Visit(const TopoDS_Edge& theEdge)
{
  if (!myDEdgeMap.IsBound (theEdge))
  {
    myModel->AddEdge (theEdge);
    myDEdgeMap.Bind  (theEdge, myModel->EdgesNb () - 1);
  }
}

//=======================================================================
// Function: Visit (face)
// Purpose : 
//=======================================================================
void BRepMesh_ShapeVisitor::Visit (const TopoDS_Face& theFace)
{
  BRepTools::Update(theFace);
  const IMeshData::IFaceHandle& aDFace = myModel->AddFace (theFace);

  // Outer wire should always be the first in the model. 
  TopoDS_Wire aOuterWire = ShapeAnalysis::OuterWire (theFace);
  if (!addWire (aOuterWire, aDFace))
  {
    aDFace->SetStatus (IMeshData_Failure);
    return;
  }
  
  TopExp_Explorer aWireIt (theFace, TopAbs_WIRE);
  for (; aWireIt.More (); aWireIt.Next ())
  {
    const TopoDS_Wire& aWire = TopoDS::Wire (aWireIt.Current ());
    if (aWire.IsSame(aOuterWire))
    {
      continue;
    }

    if (!addWire (aWire, aDFace))
    {
      // If there is a failure on internal wire, just skip it.
      // The most significant is an outer wire.
      aDFace->SetStatus (IMeshData_UnorientedWire);
    }
  }
}

//=======================================================================
// Function: addWire
// Purpose : 
//=======================================================================
Standard_Boolean BRepMesh_ShapeVisitor::addWire (
  const TopoDS_Wire&            theWire,
  const IMeshData::IFaceHandle& theDFace)
{
  if (theWire.IsNull())
  {
    return Standard_False;
  }

  Handle(ShapeExtend_WireData) aWireData = new ShapeExtend_WireData(theWire, Standard_True, Standard_False);
  ShapeAnalysis_Wire aWireTool (aWireData, theDFace->GetFace (), Precision::Confusion ());

  ShapeAnalysis_WireOrder aOrderTool;
  aWireTool.CheckOrder (aOrderTool, Standard_True, Standard_False);
  if (aWireTool.LastCheckStatus(ShapeExtend_FAIL))
  {
    return Standard_False;
  }

  if (aWireTool.LastCheckStatus(ShapeExtend_DONE3))
  {
    theDFace->SetStatus(IMeshData_UnorientedWire);
  }

  const Standard_Integer aEdgesNb = aOrderTool.NbEdges ();
  if (aEdgesNb != aWireData->NbEdges())
  {
    return Standard_False;
  }

  const IMeshData::IWireHandle& aDWire = theDFace->AddWire (theWire, aEdgesNb);
  for (Standard_Integer i = 1; i <= aEdgesNb; ++i)
  {
    const Standard_Integer aEdgeIndex = aOrderTool.Ordered (i);
    const TopoDS_Edge& aEdge = aWireData->Edge (aEdgeIndex);
    if (aEdge.Orientation() != TopAbs_EXTERNAL)
    {
      const IMeshData::IEdgeHandle& aDEdge = myModel->GetEdge (myDEdgeMap.Find (aEdge));

      aDEdge->AddPCurve (theDFace.get(), aEdge.Orientation());
      aDWire->AddEdge   (aDEdge.get(),   aEdge.Orientation());
    }
  }

  return Standard_True;
}
