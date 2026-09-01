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
#include <Contap_HContTool.hxx>
#include <Extrema_EPCOfExtPC2d.hxx>
#include <Extrema_POnCurv2d.hxx>
#include <gp_Pnt.hxx>
#include <gp_Pnt2d.hxx>

#include <utility>

namespace
{
struct SurfaceBounds
{
  double UMin;
  double VMin;
  double UMax;
  double VMax;
};

SurfaceBounds getSurfaceBounds(const occ::handle<Adaptor3d_Surface>& theSurface)
{
  SurfaceBounds aBounds = {theSurface->FirstUParameter(),
                           theSurface->FirstVParameter(),
                           theSurface->LastUParameter(),
                           theSurface->LastVParameter()};

  if (aBounds.UMax < aBounds.UMin)
  {
    std::swap(aBounds.UMin, aBounds.UMax);
  }
  if (aBounds.VMax < aBounds.VMin)
  {
    std::swap(aBounds.VMin, aBounds.VMax);
  }

  if (aBounds.UMin == RealFirst() && aBounds.UMax == RealLast())
  {
    aBounds.UMin = -1.e5;
    aBounds.UMax = 1.e5;
  }
  else if (aBounds.UMin == RealFirst())
  {
    aBounds.UMin = aBounds.UMax - 2.e5;
  }
  else if (aBounds.UMax == RealLast())
  {
    aBounds.UMax = aBounds.UMin + 2.e5;
  }

  if (aBounds.VMin == RealFirst() && aBounds.VMax == RealLast())
  {
    aBounds.VMin = -1.e5;
    aBounds.VMax = 1.e5;
  }
  else if (aBounds.VMin == RealFirst())
  {
    aBounds.VMin = aBounds.VMax - 2.e5;
  }
  else if (aBounds.VMax == RealLast())
  {
    aBounds.VMax = aBounds.VMin + 2.e5;
  }
  return aBounds;
}
} // namespace

int Contap_HContTool::NbSamplesV(const occ::handle<Adaptor3d_Surface>& S,
                                 const double,
                                 const double)
{
  int                 nbs;
  GeomAbs_SurfaceType typS = S->GetType();
  switch (typS)
  {
    case GeomAbs_Plane: {
      nbs = 2;
    }
    break;
    case GeomAbs_BezierSurface: {
      nbs = 3 + S->NbVPoles();
    }
    break;
    case GeomAbs_BSplineSurface: {
      //-- occ::handle<Geom_BSplineSurface>& HBS=S->BSpline();
      nbs = S->NbVKnots();
      nbs *= S->VDegree();
      if (nbs < 2)
      {
        nbs = 2;
      }
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

int Contap_HContTool::NbSamplesU(const occ::handle<Adaptor3d_Surface>& S,
                                 const double,
                                 const double)
{
  int                 nbs;
  GeomAbs_SurfaceType typS = S->GetType();
  switch (typS)
  {
    case GeomAbs_Plane: {
      nbs = 2;
    }
    break;
    case GeomAbs_BezierSurface: {
      nbs = 3 + S->NbUPoles();
    }
    break;
    case GeomAbs_BSplineSurface: {
      //-- occ::handle<Geom_BSplineSurface>& HBS=S->BSpline();
      nbs = S->NbUKnots();
      nbs *= S->UDegree();
      if (nbs < 2)
      {
        nbs = 2;
      }
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

int Contap_HContTool::NbSamplePoints(const occ::handle<Adaptor3d_Surface>& S)
{
  const SurfaceBounds aBounds = getSurfaceBounds(S);
  if (S->GetType() == GeomAbs_BSplineSurface)
  {
    int m = (NbSamplesU(S, aBounds.UMin, aBounds.UMax) / 3)
            * (NbSamplesV(S, aBounds.VMin, aBounds.VMax) / 3);
    if (m > 5)
    {
      return (m);
    }
    else
    {
      return (5);
    }
  }
  else
  {
    return 5;
  }
}

void Contap_HContTool::SamplePoint(const occ::handle<Adaptor3d_Surface>& S,
                                   const int                             Index,
                                   double&                               U,
                                   double&                               V)
{
  const SurfaceBounds aBounds = getSurfaceBounds(S);
  if (S->GetType() == GeomAbs_BSplineSurface)
  {
    int nbIntU = NbSamplesU(S, aBounds.UMin, aBounds.UMax) / 3;
    int nbIntV = NbSamplesV(S, aBounds.VMin, aBounds.VMax) / 3;
    if (nbIntU * nbIntV > 5)
    {
      int indU = (Index - 1) / nbIntU;        //----   0 --> nbIntV
      int indV = (Index - 1) - indU * nbIntU; //----   0 --> nbIntU

      U = aBounds.UMin
          + ((aBounds.UMax - aBounds.UMin) / ((double)(nbIntU + 1))) * (double)(indU + 1);
      V = aBounds.VMin
          + ((aBounds.VMax - aBounds.VMin) / ((double)(nbIntV + 2))) * (double)(indV + 1);

      //-- std::cout<<"Index :"<<Index<<"  uinf:"<<uinf<<"  usup:"<<usup<<"  vinf:"<<vinf<<"
      // vsup:"<<vsup<<"  ";
      //-- std::cout<<"  ("<<indU<<"/"<<nbIntU<<" ->U:"<<U<<"  ";
      //-- std::cout<<"  ("<<indV<<"/"<<nbIntV<<" ->V:"<<V<<std::endl;
      return;
    }
  }

  switch (Index)
  {
    case 1:
      U = 0.75 * aBounds.UMin + 0.25 * aBounds.UMax; // 0.25;
      V = 0.75 * aBounds.VMin + 0.25 * aBounds.VMax; // 0.25;
      break;
    case 2:
      U = 0.75 * aBounds.UMin + 0.25 * aBounds.UMax; // 0.25;
      V = 0.25 * aBounds.VMin + 0.75 * aBounds.VMax; // 0.75;
      break;
    case 3:
      U = 0.25 * aBounds.UMin + 0.75 * aBounds.UMax; // 0.75;
      V = 0.75 * aBounds.VMin + 0.25 * aBounds.VMax; // 0.25;
      break;
    case 4:
      U = 0.25 * aBounds.UMin + 0.75 * aBounds.UMax; // 0.75;
      V = 0.25 * aBounds.VMin + 0.75 * aBounds.VMax; // 0.75;
      break;
    default:
      U = 0.5 * (aBounds.UMin + aBounds.UMax); // 0.5;
      V = 0.5 * (aBounds.VMin + aBounds.VMax); // 0.5;
  }
}

int Contap_HContTool::NbSamplesOnArc(const occ::handle<Adaptor2d_Curve2d>& A)
{

  GeomAbs_CurveType CurveType = A->GetType();

  double nbsOnC = 5;
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
  return (int)(nbsOnC);
}

void Contap_HContTool::Bounds(const occ::handle<Adaptor2d_Curve2d>& A,
                              double&                               Ufirst,
                              double&                               Ulast)
{
  Ufirst = A->FirstParameter();
  Ulast  = A->LastParameter();
}

bool Contap_HContTool::Project(const occ::handle<Adaptor2d_Curve2d>& C,
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

double Contap_HContTool::Tolerance(const occ::handle<Adaptor3d_HVertex>& V,
                                   const occ::handle<Adaptor2d_Curve2d>& C)
{
  //  return BRepAdaptor2d_Curve2dTool::Resolution(C,BRep_Tool::Tolerance(V));
  return V->Resolution(C);
}

double Contap_HContTool::Parameter(const occ::handle<Adaptor3d_HVertex>& V,
                                   const occ::handle<Adaptor2d_Curve2d>& C)
{
  //  return BRep_Tool::Parameter(V,C.Edge());
  return V->Parameter(C);
}

bool Contap_HContTool::HasBeenSeen(const occ::handle<Adaptor2d_Curve2d>&)
{
  return false;
}

int Contap_HContTool::NbPoints(const occ::handle<Adaptor2d_Curve2d>&)
{
  return 0;
}

void Contap_HContTool::Value(const occ::handle<Adaptor2d_Curve2d>&,
                             const int,
                             gp_Pnt&,
                             double&,
                             double&)
{
  throw Standard_OutOfRange();
}

bool Contap_HContTool::IsVertex(const occ::handle<Adaptor2d_Curve2d>&, const int)
{
  return false;
}

void Contap_HContTool::Vertex(const occ::handle<Adaptor2d_Curve2d>&,
                              const int,
                              occ::handle<Adaptor3d_HVertex>&)
{
  throw Standard_OutOfRange();
}

int Contap_HContTool::NbSegments(const occ::handle<Adaptor2d_Curve2d>&)
{
  return 0;
}

bool Contap_HContTool::HasFirstPoint(const occ::handle<Adaptor2d_Curve2d>&, const int, int&)
{
  throw Standard_OutOfRange();
}

bool Contap_HContTool::HasLastPoint(const occ::handle<Adaptor2d_Curve2d>&, const int, int&)
{
  throw Standard_OutOfRange();
}

bool Contap_HContTool::IsAllSolution(const occ::handle<Adaptor2d_Curve2d>&)

{
  return false;
}
