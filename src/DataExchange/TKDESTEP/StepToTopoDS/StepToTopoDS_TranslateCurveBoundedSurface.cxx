// Created on: 1999-02-12
// Created by: Andrey BETENEV
// Copyright (c) 1999-1999 Matra Datavision
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

// #4  szv          S4163: optimization
//:   abv 07.04.99 S4136: turn off fixing intersection of non-adjacent edges

#include <BRep_Builder.hxx>
#include <BRepBuilderAPI_MakeFace.hxx>
#include <Geom_BoundedSurface.hxx>
#include <Precision.hxx>
#include <ShapeAlgo.hxx>
#include <ShapeAlgo_AlgoContainer.hxx>
#include <StepData_Factors.hxx>
#include <StepGeom_BoundaryCurve.hxx>
#include <StepGeom_BSplineSurface.hxx>
#include <StepGeom_CurveBoundedSurface.hxx>
#include <StepGeom_SurfaceBoundary.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_HArray1.hxx>
#include <StepToGeom.hxx>
#include <StepToTopoDS_TranslateCompositeCurve.hxx>
#include <StepToTopoDS_TranslateCurveBoundedSurface.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <Transfer_TransientProcess.hxx>

//=================================================================================================

StepToTopoDS_TranslateCurveBoundedSurface::StepToTopoDS_TranslateCurveBoundedSurface() {}

//=================================================================================================

StepToTopoDS_TranslateCurveBoundedSurface::StepToTopoDS_TranslateCurveBoundedSurface(
  const occ::handle<StepGeom_CurveBoundedSurface>& CBS,
  const occ::handle<Transfer_TransientProcess>&    TP,
  const StepData_Factors&                     theLocalFactors)
{
  Init(CBS, TP, theLocalFactors);
}

//=================================================================================================

bool StepToTopoDS_TranslateCurveBoundedSurface::Init(
  const occ::handle<StepGeom_CurveBoundedSurface>& CBS,
  const occ::handle<Transfer_TransientProcess>&    TP,
  const StepData_Factors&                     theLocalFactors)
{
  myFace.Nullify();
  if (CBS.IsNull())
    return false;

  // translate basis surface
  occ::handle<StepGeom_Surface> S    = CBS->BasisSurface();
  occ::handle<Geom_Surface>     Surf = StepToGeom::MakeSurface(S, theLocalFactors);
  if (Surf.IsNull())
  {
    TP->AddFail(CBS, "Basis surface not translated");
    return false;
  }

  // abv 30.06.00: trj4_k1_geo-tu.stp #108: do as in TranslateFace
  // pdn to force bsplsurf to be periodic
  occ::handle<StepGeom_BSplineSurface> sgbss = occ::down_cast<StepGeom_BSplineSurface>(S);
  if (!sgbss.IsNull())
  {
    occ::handle<Geom_Surface> periodicSurf = ShapeAlgo::AlgoContainer()->ConvertToPeriodic(Surf);
    if (!periodicSurf.IsNull())
    {
      TP->AddWarning(S, "Surface forced to be periodic");
      Surf = periodicSurf;
    }
  }

  // create face
  BRep_Builder B;
  B.MakeFace(myFace, Surf, Precision::Confusion());

  // add natural bound if implicit
  if (CBS->ImplicitOuter())
  {
    if (Surf->IsKind(STANDARD_TYPE(Geom_BoundedSurface)))
    {
      BRepBuilderAPI_MakeFace mf(Surf, Precision::Confusion());
      myFace = mf.Face();
    }
    else
      TP->AddWarning(CBS, "Cannot make natural bounds on infinite surface");
  }

  // translate boundaries
  occ::handle<NCollection_HArray1<StepGeom_SurfaceBoundary>> bnd = CBS->Boundaries();
  int                          nb  = bnd->Length();
  for (int i = 1; i <= nb; i++)
  {
    occ::handle<StepGeom_CompositeCurve> cc = bnd->Value(i).BoundaryCurve();
    if (cc.IsNull())
      continue;
    StepToTopoDS_TranslateCompositeCurve TrCC(cc, TP, S, Surf, theLocalFactors);
    if (!TrCC.IsDone())
    {
      TP->AddWarning(CBS, "Boundary not translated");
      continue;
    }
    B.Add(myFace, TrCC.Value());
  }

  done = !myFace.IsNull();
  return done;
}

//=================================================================================================

const TopoDS_Face& StepToTopoDS_TranslateCurveBoundedSurface::Value() const
{
  return myFace;
}
