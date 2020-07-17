// Created on: 1995-01-03
// Created by: Frederic MAUPAS/Dieter THIEMANN
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

//:   gka 09.04.99: S4136: eliminate BRepAPI::Precision()

#include <BRep_Builder.hxx>
#include <StdFail_NotDone.hxx>
#include <StepShape_Vertex.hxx>
#include <StepShape_VertexLoop.hxx>
#include <StepToTopoDS_NMTool.hxx>
#include <StepToTopoDS_Tool.hxx>
#include <StepToTopoDS_TranslateVertex.hxx>
#include <StepToTopoDS_TranslateVertexLoop.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopoDS_Wire.hxx>
#include <Transfer_TransientProcess.hxx>

//#include <BRepAPI.hxx>
// ============================================================================
// Method  : StepToTopoDS_TranslateVertexLoop::StepToTopoDS_TranslateVertexLoop
// Purpose : Empty Constructor
// ============================================================================
StepToTopoDS_TranslateVertexLoop::StepToTopoDS_TranslateVertexLoop()
: myError(StepToTopoDS_TranslateVertexLoopOther)
{
}

// ============================================================================
// Method  : StepToTopoDS_TranslateVertexLoop::StepToTopoDS_TranslateVertexLoop
// Purpose : Constructor with a VertexLoop and a Tool
// ============================================================================

StepToTopoDS_TranslateVertexLoop::StepToTopoDS_TranslateVertexLoop(const Handle(StepShape_VertexLoop)& VL, 
                                                                   StepToTopoDS_Tool& T,
                                                                   StepToTopoDS_NMTool& NMTool)
{
  Init(VL, T, NMTool);
}

// ============================================================================
// Method  : Init
// Purpose : Init  with a VertexLoop and a Tool
// ============================================================================

void StepToTopoDS_TranslateVertexLoop::Init(const Handle(StepShape_VertexLoop)& VL,
                                            StepToTopoDS_Tool& aTool,
                                            StepToTopoDS_NMTool& NMTool)
{
  // A Vertex Loop shall be mapped onto a Vertex + Edge + Wire;
  if (!aTool.IsBound(VL)) {
    BRep_Builder B;
    Handle(Transfer_TransientProcess) TP = aTool.TransientProcess();

//:S4136    Standard_Real preci = BRepAPI::Precision();
    Handle(StepShape_Vertex) Vtx;
    TopoDS_Vertex V1,V2;
    TopoDS_Edge E;
    TopoDS_Wire W;
    Vtx = VL->LoopVertex();
    StepToTopoDS_TranslateVertex myTranVtx(Vtx, aTool, NMTool);
    if (myTranVtx.IsDone()) {
      V1 = TopoDS::Vertex(myTranVtx.Value());
      V2 = TopoDS::Vertex(myTranVtx.Value());
    }
    else {
      TP->AddWarning(VL,"VertexLoop not mapped to TopoDS ");
      myError  = StepToTopoDS_TranslateVertexLoopOther;
      done     = Standard_False;    
      return;
    }
    V1.Orientation(TopAbs_FORWARD);
    V2.Orientation(TopAbs_REVERSED);
    B.MakeEdge(E);
    B.Add(E, V1);
    B.Add(E, V2);
    B.Degenerated(E, Standard_True);

    B.MakeWire(W);
    W.Closed (Standard_True);
    B.Add(W, E);
    aTool.Bind(VL, W);
    myResult = W;
    myError  = StepToTopoDS_TranslateVertexLoopDone;
    done     = Standard_True;
  }
  else {
    myResult = TopoDS::Wire(aTool.Find(VL));
    myError  = StepToTopoDS_TranslateVertexLoopDone;
    done     = Standard_True;    
  }
}

// ============================================================================
// Method  : Value
// Purpose : Return the mapped Shape
// ============================================================================

const TopoDS_Shape& StepToTopoDS_TranslateVertexLoop::Value() const 
{
  StdFail_NotDone_Raise_if (!done, "StepToTopoDS_TranslateVertexLoop::Value() - no result");
  return myResult;
}

// ============================================================================
// Method  : Error
// Purpose : Return the TranslateVertexLoop Error code
// ============================================================================

StepToTopoDS_TranslateVertexLoopError StepToTopoDS_TranslateVertexLoop::Error() const
{
  return myError;
}
