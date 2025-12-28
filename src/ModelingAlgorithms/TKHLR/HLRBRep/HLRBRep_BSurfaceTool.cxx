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

#include <HLRBRep_BSurfaceTool.hxx>

#include <BRepAdaptor_Surface.hxx>

//=================================================================================================

int HLRBRep_BSurfaceTool::NbSamplesU(const BRepAdaptor_Surface& S)
{
  int    nbs;
  GeomAbs_SurfaceType typS = S.GetType();
  switch (typS)
  {
    case GeomAbs_Plane: {
      nbs = 2;
    }
    break;
    case GeomAbs_BezierSurface: {
      nbs = 3 + S.NbUPoles();
    }
    break;
    case GeomAbs_BSplineSurface: {
      nbs = S.NbUKnots();
      nbs *= S.UDegree();
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
  return (nbs);
}

//=================================================================================================

int HLRBRep_BSurfaceTool::NbSamplesV(const BRepAdaptor_Surface& S)
{
  int    nbs;
  GeomAbs_SurfaceType typS = S.GetType();
  switch (typS)
  {
    case GeomAbs_Plane: {
      nbs = 2;
    }
    break;
    case GeomAbs_BezierSurface: {
      nbs = 3 + S.NbVPoles();
    }
    break;
    case GeomAbs_BSplineSurface: {
      nbs = S.NbVKnots();
      nbs *= S.VDegree();
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
  return (nbs);
}

//=================================================================================================

int HLRBRep_BSurfaceTool::NbSamplesU(const BRepAdaptor_Surface& S,
                                                  const double        u1,
                                                  const double        u2)
{
  int nbs = NbSamplesU(S);
  int n   = nbs;
  if (nbs > 10)
  {
    double uf = FirstUParameter(S);
    double ul = LastUParameter(S);
    n *= (int)((u2 - u1) / (uf - ul));
    if (n > nbs)
      n = nbs;
    if (n < 5)
      n = 5;
  }
  return (n);
}

//=================================================================================================

int HLRBRep_BSurfaceTool::NbSamplesV(const BRepAdaptor_Surface& S,
                                                  const double        v1,
                                                  const double        v2)
{
  int nbs = NbSamplesV(S);
  int n   = nbs;
  if (nbs > 10)
  {
    double vf = FirstVParameter(S);
    double vl = LastVParameter(S);
    n *= (int)((v2 - v1) / (vf - vl));
    if (n > nbs)
      n = nbs;
    if (n < 5)
      n = 5;
  }
  return (n);
}
