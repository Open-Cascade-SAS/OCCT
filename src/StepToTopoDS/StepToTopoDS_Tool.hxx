// Created on: 1994-12-16
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

#ifndef _StepToTopoDS_Tool_HeaderFile
#define _StepToTopoDS_Tool_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <StepToTopoDS_DataMapOfTRI.hxx>
#include <StepToTopoDS_PointVertexMap.hxx>
#include <StepToTopoDS_PointEdgeMap.hxx>
#include <Standard_Integer.hxx>
class Transfer_TransientProcess;
class StepShape_TopologicalRepresentationItem;
class TopoDS_Shape;
class StepToTopoDS_PointPair;
class TopoDS_Edge;
class StepGeom_CartesianPoint;
class TopoDS_Vertex;
class Geom_Surface;
class Geom_Curve;
class Geom2d_Curve;


//! This Tool Class provides Information to build
//! a Cas.Cad BRep from a ProSTEP Shape model.
class StepToTopoDS_Tool 
{
public:

  DEFINE_STANDARD_ALLOC

  
  Standard_EXPORT StepToTopoDS_Tool();
  
  Standard_EXPORT StepToTopoDS_Tool(const StepToTopoDS_DataMapOfTRI& Map, const Handle(Transfer_TransientProcess)& TP);
  
  Standard_EXPORT void Init (const StepToTopoDS_DataMapOfTRI& Map, const Handle(Transfer_TransientProcess)& TP);
  
  Standard_EXPORT Standard_Boolean IsBound (const Handle(StepShape_TopologicalRepresentationItem)& TRI);
  
  Standard_EXPORT void Bind (const Handle(StepShape_TopologicalRepresentationItem)& TRI, const TopoDS_Shape& S);
  
  Standard_EXPORT const TopoDS_Shape& Find (const Handle(StepShape_TopologicalRepresentationItem)& TRI);
  
  Standard_EXPORT void ClearEdgeMap();
  
  Standard_EXPORT Standard_Boolean IsEdgeBound (const StepToTopoDS_PointPair& PP);
  
  Standard_EXPORT void BindEdge (const StepToTopoDS_PointPair& PP, const TopoDS_Edge& E);
  
  Standard_EXPORT const TopoDS_Edge& FindEdge (const StepToTopoDS_PointPair& PP);
  
  Standard_EXPORT void ClearVertexMap();
  
  Standard_EXPORT Standard_Boolean IsVertexBound (const Handle(StepGeom_CartesianPoint)& PG);
  
  Standard_EXPORT void BindVertex (const Handle(StepGeom_CartesianPoint)& P, const TopoDS_Vertex& V);
  
  Standard_EXPORT const TopoDS_Vertex& FindVertex (const Handle(StepGeom_CartesianPoint)& P);
  
  Standard_EXPORT void ComputePCurve (const Standard_Boolean B);
  
  Standard_EXPORT Standard_Boolean ComputePCurve() const;
  
  Standard_EXPORT Handle(Transfer_TransientProcess) TransientProcess() const;
  
  Standard_EXPORT void AddContinuity (const Handle(Geom_Surface)& GeomSurf);
  
  Standard_EXPORT void AddContinuity (const Handle(Geom_Curve)& GeomCurve);
  
  Standard_EXPORT void AddContinuity (const Handle(Geom2d_Curve)& GeomCur2d);
  
  Standard_EXPORT Standard_Integer C0Surf() const;
  
  Standard_EXPORT Standard_Integer C1Surf() const;
  
  Standard_EXPORT Standard_Integer C2Surf() const;
  
  Standard_EXPORT Standard_Integer C0Cur2() const;
  
  Standard_EXPORT Standard_Integer C1Cur2() const;
  
  Standard_EXPORT Standard_Integer C2Cur2() const;
  
  Standard_EXPORT Standard_Integer C0Cur3() const;
  
  Standard_EXPORT Standard_Integer C1Cur3() const;
  
  Standard_EXPORT Standard_Integer C2Cur3() const;




protected:





private:



  StepToTopoDS_DataMapOfTRI myDataMap;
  StepToTopoDS_PointVertexMap myVertexMap;
  StepToTopoDS_PointEdgeMap myEdgeMap;
  Standard_Boolean myComputePC;
  Handle(Transfer_TransientProcess) myTransProc;
  Standard_Integer myNbC0Surf;
  Standard_Integer myNbC1Surf;
  Standard_Integer myNbC2Surf;
  Standard_Integer myNbC0Cur2;
  Standard_Integer myNbC1Cur2;
  Standard_Integer myNbC2Cur2;
  Standard_Integer myNbC0Cur3;
  Standard_Integer myNbC1Cur3;
  Standard_Integer myNbC2Cur3;


};







#endif // _StepToTopoDS_Tool_HeaderFile
