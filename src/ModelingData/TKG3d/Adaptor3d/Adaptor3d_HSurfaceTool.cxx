// Created by: Laurent BUCHARD
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

#include <Adaptor3d_HSurfaceTool.hxx>

#include <Adaptor3d_Curve.hxx>
#include <Adaptor3d_Surface.hxx>
#include <Geom2dAdaptor_Curve.hxx>
#include <GeomAdaptor_Curve.hxx>
#include <Geom_BezierSurface.hxx>
#include <Geom_BSplineCurve.hxx>
#include <Geom_BSplineSurface.hxx>
#include <Geom_OffsetCurve.hxx>

int Adaptor3d_HSurfaceTool::NbSamplesU(const occ::handle<Adaptor3d_Surface>& S)
{
  switch (S->GetType())
  {
    case GeomAbs_Plane:
      return 2;
    case GeomAbs_BezierSurface:
      return (3 + S->NbUPoles());
    case GeomAbs_BSplineSurface: {
      const int nbs = S->NbUKnots() * S->UDegree();
      return (nbs < 2 ? 2 : nbs);
    }
    case GeomAbs_Torus:
      return 20;
    default:
      break;
  }
  return 10;
}

int Adaptor3d_HSurfaceTool::NbSamplesV(const occ::handle<Adaptor3d_Surface>& S)
{
  switch (S->GetType())
  {
    case GeomAbs_Plane:
      return 2;
    case GeomAbs_BezierSurface:
      return (3 + S->NbVPoles());
    case GeomAbs_BSplineSurface: {
      const int nbs = S->NbVKnots() * S->VDegree();
      return (nbs < 2 ? 2 : nbs);
    }
    case GeomAbs_Cylinder:
    case GeomAbs_Cone:
    case GeomAbs_Sphere:
    case GeomAbs_Torus:
    case GeomAbs_SurfaceOfRevolution:
    case GeomAbs_SurfaceOfExtrusion:
      return 15;
    default:
      break;
  }
  return 10;
}

int Adaptor3d_HSurfaceTool::NbSamplesU(const occ::handle<Adaptor3d_Surface>& S,
                                       const double                          u1,
                                       const double                          u2)
{
  const int nbs = NbSamplesU(S);
  int       n   = nbs;
  if (nbs > 10)
  {
    const double uf = FirstUParameter(S);
    const double ul = LastUParameter(S);
    n *= (int)((u2 - u1) / (ul - uf));
    if (n > nbs || n > 50)
      n = nbs;
    if (n < 5)
      n = 5;
  }
  return n;
}

int Adaptor3d_HSurfaceTool::NbSamplesV(const occ::handle<Adaptor3d_Surface>& S,
                                       const double                          v1,
                                       const double                          v2)
{
  const int nbs = NbSamplesV(S);
  int       n   = nbs;
  if (nbs > 10)
  {
    const double vf = FirstVParameter(S);
    const double vl = LastVParameter(S);
    n *= (int)((v2 - v1) / (vl - vf));
    if (n > nbs || n > 50)
      n = nbs;
    if (n < 5)
      n = 5;
  }
  return n;
}

bool Adaptor3d_HSurfaceTool::IsSurfG1(const occ::handle<Adaptor3d_Surface>& theSurf,
                                      const bool                            theAlongU,
                                      const double                          theAngTol)
{
  double aUf, aUl, aVf, aVl;
  aUf = theSurf->FirstUParameter();
  aUl = theSurf->LastUParameter();
  aVf = theSurf->FirstVParameter();
  aVl = theSurf->LastVParameter();

  occ::handle<Adaptor3d_Surface> aS = theSurf;
  occ::handle<Adaptor3d_Curve>   aC;

  occ::handle<Geom_BSplineSurface> aBS;
  occ::handle<Geom_BSplineCurve>   aBC;

  if (aS->GetType() == GeomAbs_OffsetSurface)
  {
    aS = aS->BasisSurface();
  }

  if (aS->GetType() == GeomAbs_SurfaceOfRevolution || aS->GetType() == GeomAbs_SurfaceOfExtrusion)
  {
    aC = aS->BasisCurve();
  }

  if (!aC.IsNull())
  {
    if (aC->GetType() == GeomAbs_OffsetCurve)
    {
      occ::handle<Geom_OffsetCurve> aOC = aC->OffsetCurve();
      aC                                = new GeomAdaptor_Curve(aOC->BasisCurve());
    }

    if (aC->GetType() == GeomAbs_BSplineCurve)
    {
      if ((theAlongU && aS->GetType() == GeomAbs_SurfaceOfExtrusion)
          || (!theAlongU && aS->GetType() == GeomAbs_SurfaceOfRevolution))
      {
        aBC = aC->BSpline();
      }
    }
  }

  if (aS->GetType() == GeomAbs_BSplineSurface)
  {
    aBS = aS->BSpline();

    if (theAlongU)
    {
      const double anIsoPar = (aVf + aVl) / 2.0;
      aBC                   = occ::down_cast<Geom_BSplineCurve>(aBS->VIso(anIsoPar));
    }
    else
    {
      const double anIsoPar = (aUf + aUl) / 2.0;
      aBC                   = occ::down_cast<Geom_BSplineCurve>(aBS->UIso(anIsoPar));
    }
  }

  if (!aBC.IsNull())
  {
    if (theAlongU)
    {
      return aBC->IsG1(aUf, aUl, theAngTol);
    }
    else
    {
      return aBC->IsG1(aVf, aVl, theAngTol);
    }
  }

  return false;
}
