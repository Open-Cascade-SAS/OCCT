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

#include <HLRBRep_SurfaceTool.hxx>

#include <HLRBRep_Surface.hxx>

//=================================================================================================

int HLRBRep_SurfaceTool::NbSamplesU(const HLRBRep_Surface* theSurf)
{
  int                       nbs;
  const GeomAbs_SurfaceType typS = theSurf->GetType();
  switch (typS)
  {
    case GeomAbs_Plane: {
      nbs = 2;
    }
    break;
    case GeomAbs_BezierSurface: {
      nbs = 3 + theSurf->NbUPoles();
    }
    break;
    case GeomAbs_BSplineSurface: {
      nbs = theSurf->NbUKnots();
      nbs *= theSurf->UDegree();
      if (nbs < 2)
        nbs = 2;
    }
    break;
    case GeomAbs_Torus: {
      nbs = 20;
    }
    break;
    case GeomAbs_Cylinder:
    case GeomAbs_Cone:
    case GeomAbs_Sphere:
    case GeomAbs_SurfaceOfRevolution:
    case GeomAbs_SurfaceOfExtrusion: {
      nbs = 10;
    }
    break;

    default: {
      nbs = 10;
    }
    break;
  }
  return nbs;
}

//=================================================================================================

int HLRBRep_SurfaceTool::NbSamplesV(const HLRBRep_Surface* theSurf)
{
  int                       nbs;
  const GeomAbs_SurfaceType typS = theSurf->GetType();
  switch (typS)
  {
    case GeomAbs_Plane: {
      nbs = 2;
    }
    break;
    case GeomAbs_BezierSurface: {
      nbs = 3 + theSurf->NbVPoles();
    }
    break;
    case GeomAbs_BSplineSurface: {
      nbs = theSurf->NbVKnots();
      nbs *= theSurf->VDegree();
      if (nbs < 2)
        nbs = 2;
    }
    break;
    case GeomAbs_Cylinder:
    case GeomAbs_Cone:
    case GeomAbs_Sphere:
    case GeomAbs_Torus:
    case GeomAbs_SurfaceOfRevolution:
    case GeomAbs_SurfaceOfExtrusion: {
      nbs = 15;
    }
    break;

    default: {
      nbs = 10;
    }
    break;
  }
  return nbs;
}

//=================================================================================================

int HLRBRep_SurfaceTool::NbSamplesU(const HLRBRep_Surface* theSurf,
                                    const double           theU1,
                                    const double           theU2)
{
  int nbs = NbSamplesU(theSurf);
  int n   = nbs;
  if (nbs > 10)
  {
    const double uf = FirstUParameter(theSurf);
    const double ul = LastUParameter(theSurf);
    n *= (int)((theU2 - theU1) / (uf - ul));
    if (n > nbs)
      n = nbs;
    if (n < 5)
      n = 5;
  }
  return n;
}

//=================================================================================================

int HLRBRep_SurfaceTool::NbSamplesV(const HLRBRep_Surface* theSurf,
                                    const double           theV1,
                                    const double           theV2)
{
  int nbs = NbSamplesV(theSurf);
  int n   = nbs;
  if (nbs > 10)
  {
    const double vf = FirstVParameter(theSurf);
    const double vl = LastVParameter(theSurf);
    n *= (int)((theV2 - theV1) / (vf - vl));
    if (n > nbs)
      n = nbs;
    if (n < 5)
      n = 5;
  }
  return n;
}
