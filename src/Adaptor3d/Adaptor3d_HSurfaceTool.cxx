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


#include <Adaptor3d_HCurve.hxx>
#include <Adaptor3d_HSurface.hxx>
#include <Adaptor3d_HSurfaceTool.hxx>
#include <Geom_BezierSurface.hxx>
#include <Geom_BSplineSurface.hxx>
#include <gp_Pnt.hxx>
#include <gp_Vec.hxx>
#include <Standard_NoSuchObject.hxx>
#include <Standard_OutOfRange.hxx>

Standard_Integer Adaptor3d_HSurfaceTool::NbSamplesU(const Handle(Adaptor3d_HSurface)& S)
{
  switch (S->GetType())
  {
    case GeomAbs_Plane: return 2;
    case GeomAbs_BezierSurface: return (3 + S->NbUPoles());
    case GeomAbs_BSplineSurface:
    {
      const Standard_Integer nbs = S->NbUKnots() * S->UDegree();
      return (nbs < 2 ? 2 : nbs);
    }
    case GeomAbs_Torus: return 20;
    default:
      break;
  }
  return 10;
}

Standard_Integer Adaptor3d_HSurfaceTool::NbSamplesV(const Handle(Adaptor3d_HSurface)& S)
{
  switch (S->GetType())
  {
    case GeomAbs_Plane: return 2;
    case GeomAbs_BezierSurface: return (3 + S->NbVPoles());
    case GeomAbs_BSplineSurface: 
    {
      const Standard_Integer nbs = S->NbVKnots() * S->VDegree();
      return (nbs < 2 ? 2 : nbs);
    }
    case GeomAbs_Cylinder:
    case GeomAbs_Cone:
    case GeomAbs_Sphere:
    case GeomAbs_Torus:
    case GeomAbs_SurfaceOfRevolution:
    case GeomAbs_SurfaceOfExtrusion: return 15;
    default:
      break;
  }
  return 10;
}

Standard_Integer Adaptor3d_HSurfaceTool::NbSamplesU(const Handle(Adaptor3d_HSurface)& S,
                                                    const Standard_Real u1,
                                                    const Standard_Real u2)
{
  const Standard_Integer nbs = NbSamplesU(S);
  Standard_Integer n = nbs;
  if(nbs>10)
  { 
    const Standard_Real uf = FirstUParameter(S);
    const Standard_Real ul = LastUParameter(S);
    n *= (Standard_Integer)((u2-u1)/(ul-uf));
    if (n>nbs || n>50) n = nbs;
    if (n<5)   n = 5;
  }
  return n;
}

Standard_Integer Adaptor3d_HSurfaceTool::NbSamplesV(const Handle(Adaptor3d_HSurface)& S,
                                                    const Standard_Real v1,
                                                    const Standard_Real v2)
{
  const Standard_Integer nbs = NbSamplesV(S);
  Standard_Integer n = nbs;
  if(nbs>10)
  {
    const Standard_Real vf = FirstVParameter(S);
    const Standard_Real vl = LastVParameter(S);
    n *= (Standard_Integer)((v2-v1)/(vl-vf));
    if (n>nbs || n>50) n = nbs;
    if (n<5)   n = 5;
  }
  return n;
}
