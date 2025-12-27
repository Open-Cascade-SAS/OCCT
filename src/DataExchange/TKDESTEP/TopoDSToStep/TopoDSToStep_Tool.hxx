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

#ifndef _TopoDSToStep_Tool_HeaderFile
#define _TopoDSToStep_Tool_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <TopoDS_Shape.hxx>
#include <Standard_Transient.hxx>
#include <TopTools_ShapeMapHasher.hxx>
#include <NCollection_DataMap.hxx>
#include <TopoDS_Shell.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Wire.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Vertex.hxx>
#include <Standard_Integer.hxx>
class StepData_StepModel;
class TopoDS_Shape;
class StepShape_TopologicalRepresentationItem;

//! This Tool Class provides Information to build
//! a ProSTEP Shape model from a Cas.Cad BRep.
class TopoDSToStep_Tool
{
public:
  DEFINE_STANDARD_ALLOC

  Standard_EXPORT TopoDSToStep_Tool(const occ::handle<StepData_StepModel>& theModel);

  Standard_EXPORT TopoDSToStep_Tool(const NCollection_DataMap<TopoDS_Shape, occ::handle<Standard_Transient>, TopTools_ShapeMapHasher>& M,
                                    const bool                  FacetedContext,
                                    int                        theSurfCurveMode);

  Standard_EXPORT void Init(const NCollection_DataMap<TopoDS_Shape, occ::handle<Standard_Transient>, TopTools_ShapeMapHasher>& M,
                            const bool                  FacetedContext,
                            int                        theSurfCurveMode);

  Standard_EXPORT bool IsBound(const TopoDS_Shape& S);

  Standard_EXPORT void Bind(const TopoDS_Shape&                                    S,
                            const occ::handle<StepShape_TopologicalRepresentationItem>& T);

  Standard_EXPORT occ::handle<StepShape_TopologicalRepresentationItem> Find(const TopoDS_Shape& S);

  Standard_EXPORT bool Faceted() const;

  Standard_EXPORT void SetCurrentShell(const TopoDS_Shell& S);

  Standard_EXPORT const TopoDS_Shell& CurrentShell() const;

  Standard_EXPORT void SetCurrentFace(const TopoDS_Face& F);

  Standard_EXPORT const TopoDS_Face& CurrentFace() const;

  Standard_EXPORT void SetCurrentWire(const TopoDS_Wire& W);

  Standard_EXPORT const TopoDS_Wire& CurrentWire() const;

  Standard_EXPORT void SetCurrentEdge(const TopoDS_Edge& E);

  Standard_EXPORT const TopoDS_Edge& CurrentEdge() const;

  Standard_EXPORT void SetCurrentVertex(const TopoDS_Vertex& V);

  Standard_EXPORT const TopoDS_Vertex& CurrentVertex() const;

  Standard_EXPORT double Lowest3DTolerance() const;

  Standard_EXPORT void SetSurfaceReversed(const bool B);

  Standard_EXPORT bool SurfaceReversed() const;

  Standard_EXPORT const NCollection_DataMap<TopoDS_Shape, occ::handle<Standard_Transient>, TopTools_ShapeMapHasher>& Map() const;

  //! Returns mode for writing pcurves
  //! (initialized by parameter write.surfacecurve.mode)
  Standard_EXPORT int PCurveMode() const;

private:
  NCollection_DataMap<TopoDS_Shape, occ::handle<Standard_Transient>, TopTools_ShapeMapHasher> myDataMap;
  bool                 myFacetedContext;
  double                    myLowestTol;
  TopoDS_Shell                     myCurrentShell;
  TopoDS_Face                      myCurrentFace;
  TopoDS_Wire                      myCurrentWire;
  TopoDS_Edge                      myCurrentEdge;
  TopoDS_Vertex                    myCurrentVertex;
  bool                 myReversedSurface;
  int                 myPCurveMode;
};

#endif // _TopoDSToStep_Tool_HeaderFile
