// Created on: 1994-11-30
// Created by: Frederic MAUPAS
// Copyright (c) 1994-1999 Matra Datavision
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


#include <BRep_Tool.hxx>
#include <Interface_Static.hxx>
#include <StepShape_TopologicalRepresentationItem.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Shell.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopoDS_Wire.hxx>
#include <TopoDSToStep_Tool.hxx>

//=======================================================================
//function : TopoDSToStep_Tool
//purpose  : 
//=======================================================================
TopoDSToStep_Tool::TopoDSToStep_Tool()
     :myLowestTol(0.),myReversedSurface (Standard_False)
{
  myPCurveMode = Interface_Static::IVal("write.surfacecurve.mode");
}

//=======================================================================
//function : TopoDSToStep_Tool
//purpose  : 
//=======================================================================

TopoDSToStep_Tool::TopoDSToStep_Tool(const MoniTool_DataMapOfShapeTransient& M, const Standard_Boolean FacetedContext)
     :myLowestTol(0.),myReversedSurface(Standard_False)
{
  Init ( M, FacetedContext );
}

//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

void TopoDSToStep_Tool::Init(const MoniTool_DataMapOfShapeTransient& M, const Standard_Boolean FacetedContext)
{
  myDataMap = M;
  myFacetedContext = FacetedContext;
  myPCurveMode = Interface_Static::IVal("write.surfacecurve.mode");
}

//=======================================================================
//function : IsBound
//purpose  : 
//=======================================================================

Standard_Boolean TopoDSToStep_Tool::IsBound(const TopoDS_Shape& S) 
{
  return myDataMap.IsBound(S);
}

//=======================================================================
//function : Bind
//purpose  : 
//=======================================================================

void TopoDSToStep_Tool::Bind(const TopoDS_Shape& S,
			     const Handle(StepShape_TopologicalRepresentationItem)& T) 
{
  myDataMap.Bind(S, T);
}

//=======================================================================
//function : Find
//purpose  : 
//=======================================================================

Handle(StepShape_TopologicalRepresentationItem) TopoDSToStep_Tool::Find(const TopoDS_Shape& S)
{
  return Handle(StepShape_TopologicalRepresentationItem)::DownCast(myDataMap.Find(S));
}

//=======================================================================
//function : Faceted
//purpose  : 
//=======================================================================

Standard_Boolean TopoDSToStep_Tool::Faceted() const 
{
  return myFacetedContext;
}

//=======================================================================
//function : SetCurrentShell
//purpose  : 
//=======================================================================

void TopoDSToStep_Tool::SetCurrentShell(const TopoDS_Shell& S)
{
#ifdef OCCT_DEBUG
  cout << "Process a Shell which is ";
  switch(S.Orientation())
    {
    case TopAbs_FORWARD:
      {
	cout << "FORWARD in the Solid;" << endl;
	break;
      }
    case TopAbs_REVERSED:
      {
	cout << "REVERSED in the Solid;" << endl;
	break;
      }
    default:
      {
	cout << "INTERNAL OR EXTERNAL SHELL" << endl;
      }
    }
#endif
  myCurrentShell = S;
}

//=======================================================================
//function : CurrentShell
//purpose  : 
//=======================================================================

const TopoDS_Shell& TopoDSToStep_Tool::CurrentShell() const 
{
  return myCurrentShell;
}

//=======================================================================
//function : SetCurrentFace
//purpose  : 
//=======================================================================

void TopoDSToStep_Tool::SetCurrentFace(const TopoDS_Face& F)
{
#ifdef OCCT_DEBUG
  cout << "  Process a Face which is ";
  switch(F.Orientation())
    {
    case TopAbs_FORWARD:
      {
	cout << "FORWARD in the Shell;" << endl;
	break;
      }
    case TopAbs_REVERSED:
      {
	cout << "REVERSED in the Shell;" << endl;
	break;
      }
    default:
      {
	cout << "INTERNAL OR EXTERNAL FACE" << endl;
      }
    }
#endif  
  Standard_Real FaceTol = BRep_Tool::Tolerance(F);
  if (FaceTol > myLowestTol)
    myLowestTol = FaceTol;
  myCurrentFace = F;
}

//=======================================================================
//function : CurrentFace
//purpose  : 
//=======================================================================

const TopoDS_Face& TopoDSToStep_Tool::CurrentFace() const 
{
  return myCurrentFace;
}

//=======================================================================
//function : SetCurrentWire
//purpose  : 
//=======================================================================

void TopoDSToStep_Tool::SetCurrentWire(const TopoDS_Wire& W)
{
#ifdef OCCT_DEBUG
  cout << "    Process a Wire which is ";
  switch(W.Orientation())
    {
    case TopAbs_FORWARD:
      {
	cout << "FORWARD in the Face" << endl;
	break;
      }
    case TopAbs_REVERSED:
      {
	cout << "REVERSED in the Face;" << endl;
	break;
      }
    default:
      {
	cout << "INTERNAL OR EXTERNAL Wire" << endl;
      }
    }
#endif  
  myCurrentWire = W;
}

//=======================================================================
//function : CurrentWire
//purpose  : 
//=======================================================================

const TopoDS_Wire& TopoDSToStep_Tool::CurrentWire() const 
{
  return myCurrentWire;
}

//=======================================================================
//function : SetCurrentEdge
//purpose  : 
//=======================================================================

void TopoDSToStep_Tool::SetCurrentEdge(const TopoDS_Edge& E)
{
#ifdef OCCT_DEBUG
  cout << "      Process Edge which is ";
  switch(E.Orientation())
    {
    case TopAbs_FORWARD:
      {
	cout << "FORWARD in the Wire" << endl;
	break;
      }
    case TopAbs_REVERSED:
      {
	cout << "REVERSED in the Wire" << endl;
	break;
      }
    default:
      {
	cout << "INTERNAL OR EXTERNAL EDGE" << endl;
      }
    }
#endif  
  Standard_Real EdgeTol = BRep_Tool::Tolerance(E);
  if (EdgeTol > myLowestTol)
    myLowestTol = EdgeTol;
  myCurrentEdge = E;
}

//=======================================================================
//function : CurrentEdge
//purpose  : 
//=======================================================================

const TopoDS_Edge& TopoDSToStep_Tool::CurrentEdge() const 
{
  return myCurrentEdge;
}

//=======================================================================
//function : SetCurrentVertex
//purpose  : 
//=======================================================================

void TopoDSToStep_Tool::SetCurrentVertex(const TopoDS_Vertex& V)
{
  Standard_Real VertexTol = BRep_Tool::Tolerance(V);
  if (VertexTol > myLowestTol)
    myLowestTol = VertexTol;
  myCurrentVertex = V;
}

//=======================================================================
//function : CurrentVertex
//purpose  : 
//=======================================================================

const TopoDS_Vertex& TopoDSToStep_Tool::CurrentVertex() const 
{
  return myCurrentVertex;
}

//=======================================================================
//function : Lowest3DTolerance
//purpose  : 
//=======================================================================

Standard_Real TopoDSToStep_Tool::Lowest3DTolerance() const
{
  return myLowestTol;
}

//=======================================================================
//function : SetSurfaceReversed
//purpose  : 
//=======================================================================

void TopoDSToStep_Tool::SetSurfaceReversed(const Standard_Boolean B)
{
  myReversedSurface = B;
}

//=======================================================================
//function : SurfaceReversed
//purpose  : 
//=======================================================================

Standard_Boolean TopoDSToStep_Tool::SurfaceReversed() const
{
  return myReversedSurface;
}

//=======================================================================
//function : Map
//purpose  : 
//=======================================================================

const MoniTool_DataMapOfShapeTransient &TopoDSToStep_Tool::Map () const
{
  return myDataMap;
}

//=======================================================================
//function : PCurveMode
//purpose  : 
//=======================================================================

Standard_Integer TopoDSToStep_Tool::PCurveMode () const
{
  return myPCurveMode;
}
