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
#include <StepData_StepModel.hxx>
#include <StepShape_TopologicalRepresentationItem.hxx>
#include <TopoDSToStep_Tool.hxx>

//=================================================================================================

TopoDSToStep_Tool::TopoDSToStep_Tool(const occ::handle<StepData_StepModel>& theModel)
    : myFacetedContext(false),
      myLowestTol(0.),
      myReversedSurface(false)
{
  myPCurveMode = theModel->InternalParameters.WriteSurfaceCurMode;
}

//=================================================================================================

TopoDSToStep_Tool::TopoDSToStep_Tool(const NCollection_DataMap<TopoDS_Shape, occ::handle<Standard_Transient>, TopTools_ShapeMapHasher>& M,
                                     const bool                  FacetedContext,
                                     int                        theSurfCurveMode)
    : myLowestTol(0.),
      myReversedSurface(false)
{
  Init(M, FacetedContext, theSurfCurveMode);
}

//=================================================================================================

void TopoDSToStep_Tool::Init(const NCollection_DataMap<TopoDS_Shape, occ::handle<Standard_Transient>, TopTools_ShapeMapHasher>& M,
                             const bool                  FacetedContext,
                             int                        theSurfCurveMode)
{
  myDataMap        = M;
  myFacetedContext = FacetedContext;
  myPCurveMode     = theSurfCurveMode;
}

//=================================================================================================

bool TopoDSToStep_Tool::IsBound(const TopoDS_Shape& S)
{
  return myDataMap.IsBound(S);
}

//=================================================================================================

void TopoDSToStep_Tool::Bind(const TopoDS_Shape&                                    S,
                             const occ::handle<StepShape_TopologicalRepresentationItem>& T)
{
  myDataMap.Bind(S, T);
}

//=================================================================================================

occ::handle<StepShape_TopologicalRepresentationItem> TopoDSToStep_Tool::Find(const TopoDS_Shape& S)
{
  return occ::down_cast<StepShape_TopologicalRepresentationItem>(myDataMap.Find(S));
}

//=================================================================================================

bool TopoDSToStep_Tool::Faceted() const
{
  return myFacetedContext;
}

//=================================================================================================

void TopoDSToStep_Tool::SetCurrentShell(const TopoDS_Shell& S)
{
#ifdef OCCT_DEBUG
  std::cout << "Process a Shell which is ";
  switch (S.Orientation())
  {
    case TopAbs_FORWARD: {
      std::cout << "FORWARD in the Solid;" << std::endl;
      break;
    }
    case TopAbs_REVERSED: {
      std::cout << "REVERSED in the Solid;" << std::endl;
      break;
    }
    default: {
      std::cout << "INTERNAL OR EXTERNAL SHELL" << std::endl;
    }
  }
#endif
  myCurrentShell = S;
}

//=================================================================================================

const TopoDS_Shell& TopoDSToStep_Tool::CurrentShell() const
{
  return myCurrentShell;
}

//=================================================================================================

void TopoDSToStep_Tool::SetCurrentFace(const TopoDS_Face& F)
{
#ifdef OCCT_DEBUG
  std::cout << "  Process a Face which is ";
  switch (F.Orientation())
  {
    case TopAbs_FORWARD: {
      std::cout << "FORWARD in the Shell;" << std::endl;
      break;
    }
    case TopAbs_REVERSED: {
      std::cout << "REVERSED in the Shell;" << std::endl;
      break;
    }
    default: {
      std::cout << "INTERNAL OR EXTERNAL FACE" << std::endl;
    }
  }
#endif
  double FaceTol = BRep_Tool::Tolerance(F);
  if (FaceTol > myLowestTol)
    myLowestTol = FaceTol;
  myCurrentFace = F;
}

//=================================================================================================

const TopoDS_Face& TopoDSToStep_Tool::CurrentFace() const
{
  return myCurrentFace;
}

//=================================================================================================

void TopoDSToStep_Tool::SetCurrentWire(const TopoDS_Wire& W)
{
#ifdef OCCT_DEBUG
  std::cout << "    Process a Wire which is ";
  switch (W.Orientation())
  {
    case TopAbs_FORWARD: {
      std::cout << "FORWARD in the Face" << std::endl;
      break;
    }
    case TopAbs_REVERSED: {
      std::cout << "REVERSED in the Face;" << std::endl;
      break;
    }
    default: {
      std::cout << "INTERNAL OR EXTERNAL Wire" << std::endl;
    }
  }
#endif
  myCurrentWire = W;
}

//=================================================================================================

const TopoDS_Wire& TopoDSToStep_Tool::CurrentWire() const
{
  return myCurrentWire;
}

//=================================================================================================

void TopoDSToStep_Tool::SetCurrentEdge(const TopoDS_Edge& E)
{
#ifdef OCCT_DEBUG
  std::cout << "      Process Edge which is ";
  switch (E.Orientation())
  {
    case TopAbs_FORWARD: {
      std::cout << "FORWARD in the Wire" << std::endl;
      break;
    }
    case TopAbs_REVERSED: {
      std::cout << "REVERSED in the Wire" << std::endl;
      break;
    }
    default: {
      std::cout << "INTERNAL OR EXTERNAL EDGE" << std::endl;
    }
  }
#endif
  double EdgeTol = BRep_Tool::Tolerance(E);
  if (EdgeTol > myLowestTol)
    myLowestTol = EdgeTol;
  myCurrentEdge = E;
}

//=================================================================================================

const TopoDS_Edge& TopoDSToStep_Tool::CurrentEdge() const
{
  return myCurrentEdge;
}

//=================================================================================================

void TopoDSToStep_Tool::SetCurrentVertex(const TopoDS_Vertex& V)
{
  double VertexTol = BRep_Tool::Tolerance(V);
  if (VertexTol > myLowestTol)
    myLowestTol = VertexTol;
  myCurrentVertex = V;
}

//=================================================================================================

const TopoDS_Vertex& TopoDSToStep_Tool::CurrentVertex() const
{
  return myCurrentVertex;
}

//=================================================================================================

double TopoDSToStep_Tool::Lowest3DTolerance() const
{
  return myLowestTol;
}

//=================================================================================================

void TopoDSToStep_Tool::SetSurfaceReversed(const bool B)
{
  myReversedSurface = B;
}

//=================================================================================================

bool TopoDSToStep_Tool::SurfaceReversed() const
{
  return myReversedSurface;
}

//=================================================================================================

const NCollection_DataMap<TopoDS_Shape, occ::handle<Standard_Transient>, TopTools_ShapeMapHasher>& TopoDSToStep_Tool::Map() const
{
  return myDataMap;
}

//=================================================================================================

int TopoDSToStep_Tool::PCurveMode() const
{
  return myPCurveMode;
}
