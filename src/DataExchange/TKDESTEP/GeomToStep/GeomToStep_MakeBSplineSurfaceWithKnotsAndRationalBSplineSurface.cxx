// Created on: 1993-06-22
// Created by: Martine LANGLOIS
// Copyright (c) 1993-1999 Matra Datavision
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

//              .cxx

#include <Geom_BSplineSurface.hxx>
#include <GeomAbs_BSplKnotDistribution.hxx>
#include <GeomToStep_MakeBSplineSurfaceWithKnotsAndRationalBSplineSurface.hxx>
#include <GeomToStep_MakeCartesianPoint.hxx>
#include <StdFail_NotDone.hxx>
#include <StepData_Factors.hxx>
#include <StepGeom_BSplineSurfaceWithKnotsAndRationalBSplineSurface.hxx>
#include <StepGeom_CartesianPoint.hxx>
#include <NCollection_Array2.hxx>
#include <NCollection_HArray2.hxx>
#include <StepGeom_KnotType.hxx>
#include <gp_Pnt.hxx>
#include <TCollection_HAsciiString.hxx>
#include <Standard_Integer.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_HArray1.hxx>

//=============================================================================
// Creation d' une bspline_Surface_with_knots_and_rational_bspline_Surface de
// prostep a partir d' une BSplineSurface de Geom
//=============================================================================
GeomToStep_MakeBSplineSurfaceWithKnotsAndRationalBSplineSurface::
  GeomToStep_MakeBSplineSurfaceWithKnotsAndRationalBSplineSurface(
    const occ::handle<Geom_BSplineSurface>& BS,
    const StepData_Factors&                 theLocalFactors)
{
  occ::handle<StepGeom_BSplineSurfaceWithKnotsAndRationalBSplineSurface> BSWK;
  int                                  aUDegree, aVDegree, NU, NV, i, j, NUknots, NVknots, itampon;
  double                               rtampon;
  occ::handle<StepGeom_CartesianPoint> Pt = new StepGeom_CartesianPoint;
  occ::handle<NCollection_HArray2<occ::handle<StepGeom_CartesianPoint>>> aControlPointsList;
  StepGeom_BSplineSurfaceForm                                            aSurfaceForm;
  StepData_Logical                         aUClosed, aVClosed, aSelfIntersect;
  occ::handle<NCollection_HArray1<int>>    aUMultiplicities, aVMultiplicities;
  occ::handle<NCollection_HArray1<double>> aUKnots, aVKnots;
  occ::handle<NCollection_HArray2<double>> aWeightsData;
  GeomAbs_BSplKnotDistribution             UDistribution, VDistribution;
  StepGeom_KnotType                        KnotSpec;

  aUDegree = BS->UDegree();
  aVDegree = BS->VDegree();

  NU = BS->NbUPoles();
  NV = BS->NbVPoles();
  const NCollection_Array2<gp_Pnt>& P = BS->Poles();
  aControlPointsList = new NCollection_HArray2<occ::handle<StepGeom_CartesianPoint>>(1, NU, 1, NV);
  for (i = P.LowerRow(); i <= P.UpperRow(); i++)
  {
    for (j = P.LowerCol(); j <= P.UpperCol(); j++)
    {
      GeomToStep_MakeCartesianPoint MkPoint(P.Value(i, j), theLocalFactors.LengthFactor());
      Pt = MkPoint.Value();
      aControlPointsList->SetValue(i, j, Pt);
    }
  }

  aSurfaceForm = StepGeom_bssfUnspecified;

  if (BS->IsUClosed())
    aUClosed = StepData_LTrue;
  else
    aUClosed = StepData_LFalse;

  if (BS->IsVClosed())
    aVClosed = StepData_LTrue;
  else
    aVClosed = StepData_LFalse;

  aSelfIntersect = StepData_LFalse;

  NUknots = BS->NbUKnots();
  NVknots = BS->NbVKnots();
  const NCollection_Array1<int>& MU = BS->UMultiplicities();
  aUMultiplicities = new NCollection_HArray1<int>(1, NUknots);
  for (i = MU.Lower(); i <= MU.Upper(); i++)
  {
    itampon = MU.Value(i);
    aUMultiplicities->SetValue(i, itampon);
  }
  const NCollection_Array1<int>& MV = BS->VMultiplicities();
  aVMultiplicities = new NCollection_HArray1<int>(1, NVknots);
  for (i = MV.Lower(); i <= MV.Upper(); i++)
  {
    itampon = MV.Value(i);
    aVMultiplicities->SetValue(i, itampon);
  }

  const NCollection_Array1<double>& KU = BS->UKnots();
  const NCollection_Array1<double>& KV = BS->VKnots();
  aUKnots = new NCollection_HArray1<double>(1, NUknots);
  aVKnots = new NCollection_HArray1<double>(1, NVknots);
  for (i = KU.Lower(); i <= KU.Upper(); i++)
  {
    rtampon = KU.Value(i);
    aUKnots->SetValue(i, rtampon);
  }
  for (i = KV.Lower(); i <= KV.Upper(); i++)
  {
    rtampon = KV.Value(i);
    aVKnots->SetValue(i, rtampon);
  }

  UDistribution = BS->UKnotDistribution();
  VDistribution = BS->VKnotDistribution();
  if (UDistribution == GeomAbs_NonUniform && VDistribution == GeomAbs_NonUniform)
    KnotSpec = StepGeom_ktUnspecified;
  else if (UDistribution == GeomAbs_Uniform && VDistribution == GeomAbs_Uniform)
    KnotSpec = StepGeom_ktUniformKnots;
  else if (UDistribution == GeomAbs_QuasiUniform && VDistribution == GeomAbs_QuasiUniform)
    KnotSpec = StepGeom_ktQuasiUniformKnots;
  else if (UDistribution == GeomAbs_PiecewiseBezier && VDistribution == GeomAbs_PiecewiseBezier)
    KnotSpec = StepGeom_ktPiecewiseBezierKnots;
  else
    KnotSpec = StepGeom_ktUnspecified;

  const NCollection_Array2<double>* aWPtr = BS->Weights();
  aWeightsData = new NCollection_HArray2<double>(1, NU, 1, NV);
  if (aWPtr != nullptr)
  {
    const NCollection_Array2<double>& W = *aWPtr;
    for (i = W.LowerRow(); i <= W.UpperRow(); i++)
    {
      for (j = W.LowerCol(); j <= W.UpperCol(); j++)
      {
        rtampon = W.Value(i, j);
        aWeightsData->SetValue(i, j, rtampon);
      }
    }
  }
  else
  {
    aWeightsData->Init(1.0);
  }

  BSWK = new StepGeom_BSplineSurfaceWithKnotsAndRationalBSplineSurface;
  occ::handle<TCollection_HAsciiString> name = new TCollection_HAsciiString("");
  BSWK->Init(name,
             aUDegree,
             aVDegree,
             aControlPointsList,
             aSurfaceForm,
             aUClosed,
             aVClosed,
             aSelfIntersect,
             aUMultiplicities,
             aVMultiplicities,
             aUKnots,
             aVKnots,
             KnotSpec,
             aWeightsData);

  theBSplineSurfaceWithKnotsAndRationalBSplineSurface = BSWK;
  done                                                = true;
}

//=============================================================================
// renvoi des valeurs
//=============================================================================

const occ::handle<StepGeom_BSplineSurfaceWithKnotsAndRationalBSplineSurface>&
  GeomToStep_MakeBSplineSurfaceWithKnotsAndRationalBSplineSurface::Value() const
{
  StdFail_NotDone_Raise_if(
    !done,
    "GeomToStep_MakeBSplineSurfaceWithKnotsAndRationalBSplineSurface::Value() - no result");
  return theBSplineSurfaceWithKnotsAndRationalBSplineSurface;
}
