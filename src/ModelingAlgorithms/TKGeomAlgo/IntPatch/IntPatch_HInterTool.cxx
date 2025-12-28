// Created on: 1995-07-02
// Created by: Laurent BUCHARD
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

#include <Adaptor3d_HVertex.hxx>
#include <Extrema_EPCOfExtPC2d.hxx>
#include <Extrema_POnCurv2d.hxx>
#include <Geom2d_BezierCurve.hxx>
#include <Geom2d_BSplineCurve.hxx>
#include <Geom_BezierSurface.hxx>
#include <Geom_BSplineSurface.hxx>
#include <gp_Pnt.hxx>
#include <gp_Pnt2d.hxx>
#include <IntPatch_HInterTool.hxx>
#include <Standard_OutOfRange.hxx>

IntPatch_HInterTool::IntPatch_HInterTool()
    : uinf(0.),
      vinf(0.),
      usup(0.),
      vsup(0.)
{
}

int IntPatch_HInterTool::NbSamplesV(const occ::handle<Adaptor3d_Surface>& S,
                                    const double,
                                    const double)
{
  switch (S->GetType())
  {
    case GeomAbs_Plane:
      return 2;
    case GeomAbs_BezierSurface:
      return (3 + S->NbVPoles());
    case GeomAbs_BSplineSurface: {
      int nbs = S->NbVKnots();
      nbs *= S->VDegree();
      if (!S->IsVRational())
        nbs *= 2;
      if (nbs < 4)
        nbs = 4;
      return nbs;
    }
    break;
    case GeomAbs_Cylinder:
    case GeomAbs_Cone:
    case GeomAbs_Sphere:
    case GeomAbs_Torus:
    case GeomAbs_SurfaceOfRevolution:
    case GeomAbs_SurfaceOfExtrusion:
      return 15;

    case GeomAbs_OffsetSurface:
    case GeomAbs_OtherSurface:
      return 10;
  }
  return 10;
}

int IntPatch_HInterTool::NbSamplesU(const occ::handle<Adaptor3d_Surface>& S,
                                    const double,
                                    const double)
{
  switch (S->GetType())
  {
    case GeomAbs_Plane:
      return 2;
    case GeomAbs_BezierSurface:
      return (3 + S->NbUPoles());
    case GeomAbs_BSplineSurface: {
      int nbs = S->NbUKnots();
      nbs *= S->UDegree();
      if (!S->IsURational())
        nbs *= 2;
      if (nbs < 4)
        nbs = 4;
      return nbs;
    }
    case GeomAbs_Torus:
      return 20;

    case GeomAbs_Cylinder:
    case GeomAbs_Cone:
    case GeomAbs_Sphere:
    case GeomAbs_SurfaceOfRevolution:
    case GeomAbs_SurfaceOfExtrusion:
    case GeomAbs_OffsetSurface:
    case GeomAbs_OtherSurface:
      return 10;
  }
  return 10;
}

int IntPatch_HInterTool::NbSamplePoints(const occ::handle<Adaptor3d_Surface>& S)
{
  uinf = S->FirstUParameter();
  usup = S->LastUParameter();
  vinf = S->FirstVParameter();
  vsup = S->LastVParameter();

  if (usup < uinf)
  {
    double temp = uinf;
    uinf        = usup;
    usup        = temp;
  }
  if (vsup < vinf)
  {
    double temp = vinf;
    vinf        = vsup;
    vsup        = temp;
  }
  if (uinf == RealFirst() && usup == RealLast())
  {
    uinf = -1.e5;
    usup = 1.e5;
  }
  else if (uinf == RealFirst())
  {
    uinf = usup - 2.e5;
  }
  else if (usup == RealLast())
  {
    usup = uinf + 2.e5;
  }

  if (vinf == RealFirst() && vsup == RealLast())
  {
    vinf = -1.e5;
    vsup = 1.e5;
  }
  else if (vinf == RealFirst())
  {
    vinf = vsup - 2.e5;
  }
  else if (vsup == RealLast())
  {
    vsup = vinf + 2.e5;
  }
  const int m = (1 + NbSamplesU(S, uinf, usup) / 2) * (1 + NbSamplesV(S, vinf, vsup) / 2);
  return (m);
}

void IntPatch_HInterTool::SamplePoint(const occ::handle<Adaptor3d_Surface>& S,
                                      const int                             Index,
                                      double&                               U,
                                      double&                               V) const
{
  int nbIntU = 1 + NbSamplesU(S, uinf, usup);
  nbIntU >>= 1;
  int nbIntV = 1 + NbSamplesV(S, vinf, vsup);
  nbIntV >>= 1;

  if (nbIntU * nbIntV > 5)
  {

    int    NV = (Index - 1) / nbIntU;
    int    NU = (Index - 1) - NV * nbIntU;
    double du = ((usup - uinf) / ((double)(nbIntU + 1)));
    double dv = ((vsup - vinf) / ((double)(nbIntV + 1)));

    int perturb = (NU + NV) & 3;

    //-- petite perturbation pou eviter les pbs de symetrie avec Les recherches de points int.

    switch (perturb)
    {
      case 1:
        dv *= 1.001;
        dv *= 0.999;
        break;
      case 2:
        du *= 1.001;
        dv *= 1.001;
        break;
      case 3:
        du *= 0.999;
        break;
      default:
        break;
    }

    U = uinf + du * (double)(NU + 1);
    V = vinf + dv * (double)(NV + 1);
    return;
  }

  switch (Index)
  {
    case 1:
      U = 0.76 * uinf + 0.24 * usup;
      V = 0.74 * vinf + 0.26 * vsup;
      break;
    case 2:
      U = 0.73 * uinf + 0.27 * usup;
      V = 0.24 * vinf + 0.76 * vsup;
      break;
    case 3:
      U = 0.25 * uinf + 0.75 * usup;
      V = 0.76 * vinf + 0.24 * vsup;
      break;
    case 4:
      U = 0.26 * uinf + 0.74 * usup;
      V = 0.25 * vinf + 0.75 * vsup;
      break;
    default:
      U = 0.51 * uinf + 0.49 * usup;
      V = 0.49 * vinf + 0.51 * vsup;
  }
}

int IntPatch_HInterTool::NbSamplesOnArc(const occ::handle<Adaptor2d_Curve2d>& A)
{
  GeomAbs_CurveType CurveType = A->GetType();

  //  double nbsOnC = 5;
  int nbsOnC = 5;
  switch (CurveType)
  {
    case GeomAbs_Line:
      nbsOnC = 2;
      break;
    case GeomAbs_Circle:
    case GeomAbs_Ellipse:
    case GeomAbs_Hyperbola:
    case GeomAbs_Parabola:
      nbsOnC = 10;
      break;
    case GeomAbs_BezierCurve:
      nbsOnC = A->NbPoles();
      break;
    case GeomAbs_BSplineCurve: {
      //-- occ::handle<Geom2d_BSplineCurve>& BSC=A->BSpline();
      nbsOnC = 2 + A->NbKnots() * A->Degree();
      break;
    }
    default:
      nbsOnC = 10;
  }
  return (nbsOnC);
}

void IntPatch_HInterTool::Bounds(const occ::handle<Adaptor2d_Curve2d>& A,
                                 double&                               Ufirst,
                                 double&                               Ulast)
{
  Ufirst = A->FirstParameter();
  Ulast  = A->LastParameter();
}

bool IntPatch_HInterTool::Project(const occ::handle<Adaptor2d_Curve2d>& C,
                                  const gp_Pnt2d&                       P,
                                  double&                               Paramproj,
                                  gp_Pnt2d&                             Ptproj)
{
  double epsX = 1.e-8;
  int    Nbu  = 20;
  double Tol  = 1.e-5;
  double Dist2;

  Extrema_EPCOfExtPC2d extrema(P, *C, Nbu, epsX, Tol);
  if (!extrema.IsDone())
  {
    return false;
  }
  int Nbext = extrema.NbExt();
  if (Nbext == 0)
  {
    return false;
  }
  int indexmin = 1;
  Dist2        = extrema.SquareDistance(1);
  for (int i = 2; i <= Nbext; i++)
  {
    if (extrema.SquareDistance(i) < Dist2)
    {
      indexmin = i;
      Dist2    = extrema.SquareDistance(i);
    }
  }
  Paramproj = extrema.Point(indexmin).Parameter();
  Ptproj    = extrema.Point(indexmin).Value();
  return true;
}

double IntPatch_HInterTool::Tolerance(const occ::handle<Adaptor3d_HVertex>& V,
                                      const occ::handle<Adaptor2d_Curve2d>& C)
{
  return V->Resolution(C);
}

double IntPatch_HInterTool::Parameter(const occ::handle<Adaptor3d_HVertex>& V,
                                      const occ::handle<Adaptor2d_Curve2d>& C)
{
  return V->Parameter(C);
}

bool IntPatch_HInterTool::HasBeenSeen(const occ::handle<Adaptor2d_Curve2d>&)
{
  return false;
}

int IntPatch_HInterTool::NbPoints(const occ::handle<Adaptor2d_Curve2d>&)
{
  return 0;
}

void IntPatch_HInterTool::Value(const occ::handle<Adaptor2d_Curve2d>&,
                                const int,
                                gp_Pnt&,
                                double&,
                                double&)
{
  throw Standard_OutOfRange();
}

bool IntPatch_HInterTool::IsVertex(const occ::handle<Adaptor2d_Curve2d>&, const int)
{
  return false;
}

void IntPatch_HInterTool::Vertex(const occ::handle<Adaptor2d_Curve2d>&,
                                 const int,
                                 occ::handle<Adaptor3d_HVertex>&)
{
  throw Standard_OutOfRange();
}

int IntPatch_HInterTool::NbSegments(const occ::handle<Adaptor2d_Curve2d>&)
{
  return 0;
}

bool IntPatch_HInterTool::HasFirstPoint(const occ::handle<Adaptor2d_Curve2d>&, const int, int&)
{
  throw Standard_OutOfRange();
}

bool IntPatch_HInterTool::HasLastPoint(const occ::handle<Adaptor2d_Curve2d>&, const int, int&)
{
  throw Standard_OutOfRange();
}

bool IntPatch_HInterTool::IsAllSolution(const occ::handle<Adaptor2d_Curve2d>&)
{
  return false;
}
