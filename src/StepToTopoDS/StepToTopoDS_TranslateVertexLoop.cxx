// Created on: 1995-01-03
// Created by: Frederic MAUPAS/Dieter THIEMANN
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

//:   gka 09.04.99: S4136: eliminate BRepAPI::Precision()

#include <StepToTopoDS_TranslateVertexLoop.ixx>

#include <BRep_Builder.hxx>
//#include <BRepAPI.hxx>

#include <StepShape_Vertex.hxx>
#include <StepToTopoDS_TranslateVertex.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Wire.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopoDS_Edge.hxx>
#include <Transfer_TransientProcess.hxx>

// ============================================================================
// Method  : StepToTopoDS_TranslateVertexLoop::StepToTopoDS_TranslateVertexLoop
// Purpose : Empty Constructor
// ============================================================================

StepToTopoDS_TranslateVertexLoop::StepToTopoDS_TranslateVertexLoop()
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
  StdFail_NotDone_Raise_if(!done,"");
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
