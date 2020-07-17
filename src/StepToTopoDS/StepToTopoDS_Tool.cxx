// Created on: 1995-01-03
// Created by: Frederic MAUPAS
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

//  Complement : CKY, alimentation du TransientProcess
//    (Bind general;  BindVertex ?)

#include <Geom2d_Curve.hxx>
#include <Geom_Curve.hxx>
#include <Geom_Surface.hxx>
#include <StepGeom_CartesianPoint.hxx>
#include <StepShape_TopologicalRepresentationItem.hxx>
#include <StepToTopoDS_PointPair.hxx>
#include <StepToTopoDS_Tool.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Vertex.hxx>
#include <Transfer_TransientProcess.hxx>
#include <TransferBRep.hxx>

// ============================================================================
// Method  : StepToTopoDS_Tool::StepToTopoDS_Tool
// Purpose : Empty Constructor
// ============================================================================
StepToTopoDS_Tool::StepToTopoDS_Tool()
: myComputePC(Standard_False),
  myNbC0Surf(0),
  myNbC1Surf(0),
  myNbC2Surf(0),
  myNbC0Cur2(0),
  myNbC1Cur2(0),
  myNbC2Cur2(0),
  myNbC0Cur3(0),
  myNbC1Cur3(0),
  myNbC2Cur3(0)
{
}

// ============================================================================
// Method  : StepToTopoDS_Tool::StepToTopoDS_Tool
// Purpose : Constructor with a Map and a TransientProcess
// ============================================================================

StepToTopoDS_Tool::StepToTopoDS_Tool(const StepToTopoDS_DataMapOfTRI& Map, 
				   const Handle(Transfer_TransientProcess)& TP)
{
  Init(Map, TP);
}

// ============================================================================
// Method  : Init
// Purpose : Init with a Map
// ============================================================================

void StepToTopoDS_Tool::Init(const StepToTopoDS_DataMapOfTRI& Map, 
			    const Handle(Transfer_TransientProcess)& TP)
{ 
  myComputePC = Standard_False;

  StepToTopoDS_PointVertexMap aVertexMap;
  StepToTopoDS_PointEdgeMap   aEdgeMap;
  
  myDataMap   = Map;
  myVertexMap = aVertexMap;
  myEdgeMap   = aEdgeMap;
  myTransProc = TP;

  myNbC0Surf = myNbC1Surf = myNbC2Surf = 0;
  myNbC0Cur2 = myNbC1Cur2 = myNbC2Cur2 = 0;
  myNbC0Cur3 = myNbC1Cur3 = myNbC2Cur3 = 0;


}

// ============================================================================
// Method  : StepToTopoDS_Tool::IsBound
// Purpose : Indicates weither a TRI is bound or not in the Map
// ============================================================================

Standard_Boolean StepToTopoDS_Tool::IsBound(const Handle(StepShape_TopologicalRepresentationItem)& TRI)
{
  return myDataMap.IsBound(TRI);
}

// ============================================================================
// Method  : StepToTopoDS_Tool::Bind
// Purpose : Binds a TRI with a Shape in the Map
// ============================================================================

void StepToTopoDS_Tool::Bind(const Handle(StepShape_TopologicalRepresentationItem)& TRI, const TopoDS_Shape& S)
{
  myDataMap.Bind(TRI, S);
  TransferBRep::SetShapeResult (myTransProc,TRI,S);
}

// ============================================================================
// Method  : StepToTopoDS_Tool::Find
// Purpose : Returns the Shape corresponding to the bounded TRI
// ============================================================================

const TopoDS_Shape& StepToTopoDS_Tool::Find(const Handle(StepShape_TopologicalRepresentationItem)& TRI)
{
  return myDataMap.Find(TRI);
}


// ============================================================================
// Method  : StepToTopoDS_Tool::ClearEdgeMap
// Purpose : 
// ============================================================================

void StepToTopoDS_Tool::ClearEdgeMap()
{
  myEdgeMap.Clear();
}

// ============================================================================
// Method  : StepToTopoDS_Tool::IsEdgeBound
// Purpose : 
// ============================================================================

Standard_Boolean StepToTopoDS_Tool::IsEdgeBound(const StepToTopoDS_PointPair& PP)
{
  return myEdgeMap.IsBound(PP);
}

// ============================================================================
// Method  : StepToTopoDS_Tool_BindEdge
// Purpose : 
// ============================================================================

void StepToTopoDS_Tool::BindEdge(const StepToTopoDS_PointPair& PP, const TopoDS_Edge& E)
{
  myEdgeMap.Bind(PP, E);
}

// ============================================================================
// Method  : StepToTopoDS_Tool::FindEdge
// Purpose : 
// ============================================================================

const TopoDS_Edge& StepToTopoDS_Tool::FindEdge(const StepToTopoDS_PointPair& PP)
{
  return myEdgeMap.Find(PP);
}

// ============================================================================
// Method  : StepToTopoDS_Tool::ClearVertexMap
// Purpose : 
// ============================================================================

void StepToTopoDS_Tool::ClearVertexMap()
{
  myVertexMap.Clear();
}

// ============================================================================
// Method  : StepToTopoDS_Tool::IsVertexBound
// Purpose : 
// ============================================================================

Standard_Boolean StepToTopoDS_Tool::IsVertexBound(const Handle(StepGeom_CartesianPoint)& PG)
{
  return myVertexMap.IsBound(PG);
}

// ============================================================================
// Method  : StepToTopoDS_Tool::BindVertex
// Purpose : 
// ============================================================================

void StepToTopoDS_Tool::BindVertex(const Handle(StepGeom_CartesianPoint)& P, const TopoDS_Vertex& V)
{
  myVertexMap.Bind(P, V);
#ifdef OCCT_DEBUG
  TransferBRep::SetShapeResult (myTransProc,P,V);
#endif
}

// ============================================================================
// Method  : StepToTopoDS_Tool::FindVertex
// Purpose : 
// ============================================================================

const TopoDS_Vertex& StepToTopoDS_Tool::FindVertex(const Handle(StepGeom_CartesianPoint)& P)
{
  return myVertexMap.Find(P);
}

// ============================================================================
// Method  : ComputePCurve
// Purpose : 
// ============================================================================

void StepToTopoDS_Tool::ComputePCurve(const Standard_Boolean B)
{
  myComputePC = B;
}

// ============================================================================
// Method  : ComputePCurve
// Purpose : 
// ============================================================================

Standard_Boolean StepToTopoDS_Tool::ComputePCurve() const
{
  return myComputePC;
}

// ============================================================================
// Method  : StepToTopoDS_Tool::TransientProcess
// Purpose : Returns the TransientProcess
// ============================================================================

Handle(Transfer_TransientProcess) StepToTopoDS_Tool::TransientProcess() const
{
  return myTransProc;
}


//===========
// AddStatistics
//===========

void  StepToTopoDS_Tool::AddContinuity (const Handle(Geom_Surface)& GeomSurf)
{
  switch(GeomSurf->Continuity())
    {
    case GeomAbs_C0:	myNbC0Surf ++;  break;
    case GeomAbs_C1:	myNbC1Surf ++;  break;
    default:		myNbC2Surf ++;
    }
}

void  StepToTopoDS_Tool::AddContinuity (const Handle(Geom_Curve)&   GeomCurve)
{
  switch(GeomCurve->Continuity())
    {
    case GeomAbs_C0:	myNbC0Cur3 ++;  break;
    case GeomAbs_C1:	myNbC1Cur3 ++;  break;
    default:		myNbC2Cur3 ++;
    }
}

void  StepToTopoDS_Tool::AddContinuity (const Handle(Geom2d_Curve)& GeomCur2d)
{
  switch(GeomCur2d->Continuity())
    {
    case GeomAbs_C0:	myNbC0Cur2 ++;  break;
    case GeomAbs_C1:	myNbC1Cur2 ++;  break;
    default:		myNbC2Cur2 ++;
    }
}

//===========
// Statistics
//===========

Standard_Integer StepToTopoDS_Tool::C0Surf() const
{
  return myNbC0Surf;
}


Standard_Integer StepToTopoDS_Tool::C1Surf() const
{
  return myNbC1Surf;
}


Standard_Integer StepToTopoDS_Tool::C2Surf() const
{
  return myNbC2Surf;
}


//===========
// Statistics
//===========

Standard_Integer StepToTopoDS_Tool::C0Cur2() const
{
  return myNbC0Cur2;
}


Standard_Integer StepToTopoDS_Tool::C1Cur2() const
{
  return myNbC1Cur2;
}


Standard_Integer StepToTopoDS_Tool::C2Cur2() const
{
  return myNbC2Cur2;
}


//===========
// Statistics
//===========

Standard_Integer StepToTopoDS_Tool::C0Cur3() const
{
  return myNbC0Cur3;
}


Standard_Integer StepToTopoDS_Tool::C1Cur3() const
{
  return myNbC1Cur3;
}


Standard_Integer StepToTopoDS_Tool::C2Cur3() const
{
  return myNbC2Cur3;
}

