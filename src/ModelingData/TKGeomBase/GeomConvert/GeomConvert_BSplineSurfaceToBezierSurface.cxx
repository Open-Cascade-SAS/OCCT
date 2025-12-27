// Created on: 1996-03-12
// Created by: Bruno DUMORTIER
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

#include <Geom_BezierSurface.hxx>
#include <Geom_BSplineSurface.hxx>
#include <GeomConvert_BSplineSurfaceToBezierSurface.hxx>
#include <Standard_DomainError.hxx>
#include <Standard_OutOfRange.hxx>
#include <gp_Pnt.hxx>
#include <NCollection_Array2.hxx>
#include <NCollection_Array2.hxx>

//=================================================================================================

GeomConvert_BSplineSurfaceToBezierSurface::GeomConvert_BSplineSurfaceToBezierSurface(
  const occ::handle<Geom_BSplineSurface>& BasisSurface)
{
  mySurface = occ::down_cast<Geom_BSplineSurface>(BasisSurface->Copy());
  double U1, U2, V1, V2;
  mySurface->Bounds(U1, U2, V1, V2);
  mySurface->Segment(U1, U2, V1, V2);
  mySurface->IncreaseUMultiplicity(mySurface->FirstUKnotIndex(),
                                   mySurface->LastUKnotIndex(),
                                   mySurface->UDegree());
  mySurface->IncreaseVMultiplicity(mySurface->FirstVKnotIndex(),
                                   mySurface->LastVKnotIndex(),
                                   mySurface->VDegree());
}

//=================================================================================================

GeomConvert_BSplineSurfaceToBezierSurface::GeomConvert_BSplineSurfaceToBezierSurface(
  const occ::handle<Geom_BSplineSurface>& BasisSurface,
  const double                U1,
  const double                U2,
  const double                V1,
  const double                V2,
  const double                ParametricTolerance)
{
  if ((U2 - U1 < ParametricTolerance) || (V2 - V1 < ParametricTolerance))
    throw Standard_DomainError("GeomConvert_BSplineSurfaceToBezierSurface");

  double    Uf = U1, Ul = U2, Vf = V1, Vl = V2, PTol = ParametricTolerance / 2;
  int I1, I2;

  mySurface = occ::down_cast<Geom_BSplineSurface>(BasisSurface->Copy());

  mySurface->LocateU(U1, PTol, I1, I2);
  if (I1 == I2)
  { // On est sur le noeud
    if (mySurface->UKnot(I1) > U1)
      Uf = mySurface->UKnot(I1);
  }

  mySurface->LocateU(U2, PTol, I1, I2);
  if (I1 == I2)
  { // On est sur le noeud
    if (mySurface->UKnot(I1) < U2)
      Ul = mySurface->UKnot(I1);
  }

  mySurface->LocateV(V1, PTol, I1, I2);
  if (I1 == I2)
  { // On est sur le noeud
    if (mySurface->VKnot(I1) > V1)
      Vf = mySurface->VKnot(I1);
  }

  mySurface->LocateV(V2, PTol, I1, I2);
  if (I1 == I2)
  { // On est sur le noeud
    if (mySurface->VKnot(I1) < V2)
      Vl = mySurface->VKnot(I1);
  }

  mySurface->Segment(Uf, Ul, Vf, Vl);
  mySurface->IncreaseUMultiplicity(mySurface->FirstUKnotIndex(),
                                   mySurface->LastUKnotIndex(),
                                   mySurface->UDegree());
  mySurface->IncreaseVMultiplicity(mySurface->FirstVKnotIndex(),
                                   mySurface->LastVKnotIndex(),
                                   mySurface->VDegree());
}

//=================================================================================================

occ::handle<Geom_BezierSurface> GeomConvert_BSplineSurfaceToBezierSurface::Patch(
  const int UIndex,
  const int VIndex)
{
  if (UIndex < 1 || UIndex > mySurface->NbUKnots() - 1 || VIndex < 1
      || VIndex > mySurface->NbVKnots() - 1)
  {
    throw Standard_OutOfRange("GeomConvert_BSplineSurfaceToBezierSurface");
  }
  int UDeg = mySurface->UDegree();
  int VDeg = mySurface->VDegree();

  NCollection_Array2<gp_Pnt> Poles(1, UDeg + 1, 1, VDeg + 1);

  occ::handle<Geom_BezierSurface> S;
  if (mySurface->IsURational() || mySurface->IsVRational())
  {
    NCollection_Array2<double> Weights(1, UDeg + 1, 1, VDeg + 1);
    for (int i = 1; i <= UDeg + 1; i++)
    {
      int CurI = i + UDeg * (UIndex - 1);
      for (int j = 1; j <= VDeg + 1; j++)
      {
        Poles(i, j)   = mySurface->Pole(CurI, j + VDeg * (VIndex - 1));
        Weights(i, j) = mySurface->Weight(CurI, j + VDeg * (VIndex - 1));
      }
    }
    S = new Geom_BezierSurface(Poles, Weights);
  }
  else
  {
    for (int i = 1; i <= UDeg + 1; i++)
    {
      int CurI = i + UDeg * (UIndex - 1);
      for (int j = 1; j <= VDeg + 1; j++)
      {
        Poles(i, j) = mySurface->Pole(CurI, j + VDeg * (VIndex - 1));
      }
    }
    S = new Geom_BezierSurface(Poles);
  }
  return S;
}

//=================================================================================================

void GeomConvert_BSplineSurfaceToBezierSurface::Patches(NCollection_Array2<occ::handle<Geom_BezierSurface>>& Surfaces)
{
  int NbU = NbUPatches();
  int NbV = NbVPatches();
  for (int i = 1; i <= NbU; i++)
  {
    for (int j = 1; j <= NbV; j++)
    {
      Surfaces(i, j) = Patch(i, j);
    }
  }
}

//=================================================================================================

void GeomConvert_BSplineSurfaceToBezierSurface::UKnots(NCollection_Array1<double>& TKnots) const
{
  int ii, kk;
  for (ii = 1, kk = TKnots.Lower(); ii <= mySurface->NbUKnots(); ii++, kk++)
    TKnots(kk) = mySurface->UKnot(ii);
}

//=================================================================================================

void GeomConvert_BSplineSurfaceToBezierSurface::VKnots(NCollection_Array1<double>& TKnots) const
{
  int ii, kk;
  for (ii = 1, kk = TKnots.Lower(); ii <= mySurface->NbVKnots(); ii++, kk++)
    TKnots(kk) = mySurface->VKnot(ii);
}

//=================================================================================================

int GeomConvert_BSplineSurfaceToBezierSurface::NbUPatches() const
{
  return (mySurface->NbUKnots() - 1);
}

//=================================================================================================

int GeomConvert_BSplineSurfaceToBezierSurface::NbVPatches() const
{
  return (mySurface->NbVKnots() - 1);
}
