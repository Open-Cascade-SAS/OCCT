// Created on: 1995-01-03
// Created by: Frederic MAUPAS
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


//  Complement : CKY, alimentation du TransientProcess
//    (Bind general;  BindVertex ?)

#include <StepToTopoDS_Tool.ixx>
#include <TransferBRep.hxx>

// ============================================================================
// Method  : StepToTopoDS_Tool::StepToTopoDS_Tool
// Purpose : Empty Constructor
// ============================================================================

StepToTopoDS_Tool::StepToTopoDS_Tool() {}

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
#ifdef DEBUG
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

