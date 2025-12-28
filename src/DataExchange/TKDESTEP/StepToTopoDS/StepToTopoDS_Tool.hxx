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

#include <StepShape_TopologicalRepresentationItem.hxx>
#include <TopoDS_Shape.hxx>
#include <NCollection_DataMap.hxx>
#include <StepGeom_CartesianPoint.hxx>
#include <TopoDS_Vertex.hxx>
#include <StepToTopoDS_PointPair.hxx>
#include <TopoDS_Edge.hxx>
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

  Standard_EXPORT StepToTopoDS_Tool(
    const NCollection_DataMap<occ::handle<StepShape_TopologicalRepresentationItem>, TopoDS_Shape>&
                                                  Map,
    const occ::handle<Transfer_TransientProcess>& TP);

  Standard_EXPORT void Init(
    const NCollection_DataMap<occ::handle<StepShape_TopologicalRepresentationItem>, TopoDS_Shape>&
                                                  Map,
    const occ::handle<Transfer_TransientProcess>& TP);

  Standard_EXPORT bool IsBound(const occ::handle<StepShape_TopologicalRepresentationItem>& TRI);

  Standard_EXPORT void Bind(const occ::handle<StepShape_TopologicalRepresentationItem>& TRI,
                            const TopoDS_Shape&                                         S);

  Standard_EXPORT const TopoDS_Shape& Find(
    const occ::handle<StepShape_TopologicalRepresentationItem>& TRI);

  Standard_EXPORT void ClearEdgeMap();

  Standard_EXPORT bool IsEdgeBound(const StepToTopoDS_PointPair& PP);

  Standard_EXPORT void BindEdge(const StepToTopoDS_PointPair& PP, const TopoDS_Edge& E);

  Standard_EXPORT const TopoDS_Edge& FindEdge(const StepToTopoDS_PointPair& PP);

  Standard_EXPORT void ClearVertexMap();

  Standard_EXPORT bool IsVertexBound(const occ::handle<StepGeom_CartesianPoint>& PG);

  Standard_EXPORT void BindVertex(const occ::handle<StepGeom_CartesianPoint>& P,
                                  const TopoDS_Vertex&                        V);

  Standard_EXPORT const TopoDS_Vertex& FindVertex(const occ::handle<StepGeom_CartesianPoint>& P);

  Standard_EXPORT void ComputePCurve(const bool B);

  Standard_EXPORT bool ComputePCurve() const;

  Standard_EXPORT occ::handle<Transfer_TransientProcess> TransientProcess() const;

  Standard_EXPORT void AddContinuity(const occ::handle<Geom_Surface>& GeomSurf);

  Standard_EXPORT void AddContinuity(const occ::handle<Geom_Curve>& GeomCurve);

  Standard_EXPORT void AddContinuity(const occ::handle<Geom2d_Curve>& GeomCur2d);

  Standard_EXPORT int C0Surf() const;

  Standard_EXPORT int C1Surf() const;

  Standard_EXPORT int C2Surf() const;

  Standard_EXPORT int C0Cur2() const;

  Standard_EXPORT int C1Cur2() const;

  Standard_EXPORT int C2Cur2() const;

  Standard_EXPORT int C0Cur3() const;

  Standard_EXPORT int C1Cur3() const;

  Standard_EXPORT int C2Cur3() const;

private:
  NCollection_DataMap<occ::handle<StepShape_TopologicalRepresentationItem>, TopoDS_Shape> myDataMap;
  NCollection_DataMap<occ::handle<StepGeom_CartesianPoint>, TopoDS_Vertex> myVertexMap;
  NCollection_DataMap<StepToTopoDS_PointPair, TopoDS_Edge>                 myEdgeMap;
  bool                                                                     myComputePC;
  occ::handle<Transfer_TransientProcess>                                   myTransProc;
  int                                                                      myNbC0Surf;
  int                                                                      myNbC1Surf;
  int                                                                      myNbC2Surf;
  int                                                                      myNbC0Cur2;
  int                                                                      myNbC1Cur2;
  int                                                                      myNbC2Cur2;
  int                                                                      myNbC0Cur3;
  int                                                                      myNbC1Cur3;
  int                                                                      myNbC2Cur3;
};

#endif // _StepToTopoDS_Tool_HeaderFile
