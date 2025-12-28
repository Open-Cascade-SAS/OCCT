// Created on: 1993-07-23
// Created by: Joelle CHAUVET
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

// Modified:	Wed Oct 23 09:17:47 1996
//		check ponctuallity (PRO4896)

#include <Adaptor3d_CurveOnSurface.hxx>
#include <BRep_Builder.hxx>
#include <BRep_Tool.hxx>
#include <BRepLib.hxx>
#include <BRepLib_MakeEdge.hxx>
#include <ElCLib.hxx>
#include <Extrema_ExtPC.hxx>
#include <Geom2d_Curve.hxx>
#include <Geom2d_TrimmedCurve.hxx>
#include <Geom2dAdaptor_Curve.hxx>
#include <Geom_Circle.hxx>
#include <Geom_Curve.hxx>
#include <Geom_Ellipse.hxx>
#include <Geom_Hyperbola.hxx>
#include <Geom_Line.hxx>
#include <Geom_Parabola.hxx>
#include <Geom_Surface.hxx>
#include <Geom_TrimmedCurve.hxx>
#include <GeomAdaptor_Curve.hxx>
#include <GeomAdaptor_Surface.hxx>
#include <gp.hxx>
#include <gp_Circ.hxx>
#include <gp_Elips.hxx>
#include <gp_Hypr.hxx>
#include <gp_Lin.hxx>
#include <gp_Parab.hxx>
#include <gp_Pnt.hxx>
#include <Precision.hxx>
#include <StdFail_NotDone.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Vertex.hxx>

//=======================================================================
// function : Project
// purpose  : project a vertex on a curve
//=======================================================================
static bool Project(const occ::handle<Geom_Curve>& C,
                                const TopoDS_Vertex&      V,
                                double&            p)
{
  double Eps2 = BRep_Tool::Tolerance(V);
  Eps2 *= Eps2;

  gp_Pnt            P = BRep_Tool::Pnt(V);
  GeomAdaptor_Curve GAC(C);

  // Afin de faire les extremas, on verifie les distances en bout
  double D1, D2;
  gp_Pnt        P1, P2;
  P1 = GAC.Value(GAC.FirstParameter());
  P2 = GAC.Value(GAC.LastParameter());
  D1 = P1.SquareDistance(P);
  D2 = P2.SquareDistance(P);
  if ((D1 < D2) && (D1 <= Eps2))
  {
    p = GAC.FirstParameter();
    return true;
  }
  else if ((D2 < D1) && (D2 <= Eps2))
  {
    p = GAC.LastParameter();
    return true;
  }

  // Sinon, on calcule les extremas.

  Extrema_ExtPC extrema(P, GAC);
  if (extrema.IsDone())
  {
    int i, index = 0, n = extrema.NbExt();
    double    Dist2 = RealLast(), dist2min;

    for (i = 1; i <= n; i++)
    {
      dist2min = extrema.SquareDistance(i);
      if (dist2min < Dist2)
      {
        index = i;
        Dist2 = dist2min;
      }
    }

    if (index != 0)
    {
      if (Dist2 <= Eps2)
      {
        p = (extrema.Point(index)).Parameter();
        return true;
      }
    }
  }
  return false;
}

//=======================================================================
// function : Project
// purpose  : project a vertex on a curve on surface
//=======================================================================

static bool Project(const occ::handle<Geom2d_Curve>& C,
                                const occ::handle<Geom_Surface>& S,
                                const TopoDS_Vertex&        V,
                                double&              p)
{
  gp_Pnt        P    = BRep_Tool::Pnt(V);
  double Eps2 = BRep_Tool::Tolerance(V);
  Eps2 *= Eps2;

  occ::handle<Geom2dAdaptor_Curve> HG2AHC = new Geom2dAdaptor_Curve(C);
  occ::handle<GeomAdaptor_Surface> HGAHS  = new GeomAdaptor_Surface(S);
  Adaptor3d_CurveOnSurface    ACOS(HG2AHC, HGAHS);

  double D1, D2;
  gp_Pnt        P1, P2;
  P1 = ACOS.Value(ACOS.FirstParameter());
  P2 = ACOS.Value(ACOS.LastParameter());
  D1 = P1.SquareDistance(P);
  D2 = P2.SquareDistance(P);
  if ((D1 < D2) && (D1 <= Eps2))
  {
    p = ACOS.FirstParameter();
    return true;
  }
  else if ((D2 < D1) && (D2 <= Eps2))
  {
    p = ACOS.LastParameter();
    return true;
  }

  Extrema_ExtPC extrema(P, ACOS);

  if (extrema.IsDone())
  {
    int i, index = 0, n = extrema.NbExt();
    double    Dist2 = RealLast(), dist2min;

    for (i = 1; i <= n; i++)
    {
      dist2min = extrema.SquareDistance(i);
      if (dist2min < Dist2)
      {
        index = i;
        Dist2 = dist2min;
      }
    }

    if (index != 0)
    {
      Extrema_POnCurv POC = extrema.Point(index);
      if (P.SquareDistance(POC.Value()) <= Precision::SquareConfusion())
      {
        p = POC.Parameter();
        return true;
      }
    }
  }
  return false;
}

//=================================================================================================

BRepLib_MakeEdge::BRepLib_MakeEdge()
    : myError(BRepLib_PointProjectionFailed)
{
}

//=================================================================================================

BRepLib_MakeEdge::BRepLib_MakeEdge(const TopoDS_Vertex& V1, const TopoDS_Vertex& V2)
{
  gp_Pnt        P1 = BRep_Tool::Pnt(V1);
  gp_Pnt        P2 = BRep_Tool::Pnt(V2);
  double l  = P1.Distance(P2);
  if (l <= gp::Resolution())
  {
    myError = BRepLib_LineThroughIdenticPoints;
    return;
  }
  gp_Lin            L(P1, gp_Vec(P1, P2));
  occ::handle<Geom_Line> GL = new Geom_Line(L);
  Init(GL, V1, V2, 0, l);
}

//=================================================================================================

BRepLib_MakeEdge::BRepLib_MakeEdge(const gp_Pnt& P1, const gp_Pnt& P2)
{
  double l = P1.Distance(P2);
  if (l <= gp::Resolution())
  {
    myError = BRepLib_LineThroughIdenticPoints;
    return;
  }
  gp_Lin            L(P1, gp_Vec(P1, P2));
  occ::handle<Geom_Line> GL = new Geom_Line(L);
  Init(GL, P1, P2, 0, l);
}

//=================================================================================================

BRepLib_MakeEdge::BRepLib_MakeEdge(const gp_Lin& L)
{
  occ::handle<Geom_Line> GL = new Geom_Line(L);
  Init(GL);
}

//=================================================================================================

BRepLib_MakeEdge::BRepLib_MakeEdge(const gp_Lin& L, const double p1, const double p2)
{
  occ::handle<Geom_Line> GL = new Geom_Line(L);
  Init(GL, p1, p2);
}

//=================================================================================================

BRepLib_MakeEdge::BRepLib_MakeEdge(const gp_Lin& L, const gp_Pnt& P1, const gp_Pnt& P2)
{
  occ::handle<Geom_Line> GL = new Geom_Line(L);
  Init(GL, P1, P2);
}

//=================================================================================================

BRepLib_MakeEdge::BRepLib_MakeEdge(const gp_Lin&        L,
                                   const TopoDS_Vertex& V1,
                                   const TopoDS_Vertex& V2)
{
  occ::handle<Geom_Line> GL = new Geom_Line(L);
  Init(GL, V1, V2);
}

//=================================================================================================

BRepLib_MakeEdge::BRepLib_MakeEdge(const gp_Circ& C)
{
  occ::handle<Geom_Circle> GC = new Geom_Circle(C);
  Init(GC);
}

//=================================================================================================

BRepLib_MakeEdge::BRepLib_MakeEdge(const gp_Circ& C, const double p1, const double p2)
{
  occ::handle<Geom_Circle> GC = new Geom_Circle(C);
  Init(GC, p1, p2);
}

//=================================================================================================

BRepLib_MakeEdge::BRepLib_MakeEdge(const gp_Circ& C, const gp_Pnt& P1, const gp_Pnt& P2)
{
  occ::handle<Geom_Circle> GC = new Geom_Circle(C);
  Init(GC, P1, P2);
}

//=================================================================================================

BRepLib_MakeEdge::BRepLib_MakeEdge(const gp_Circ&       C,
                                   const TopoDS_Vertex& V1,
                                   const TopoDS_Vertex& V2)
{
  occ::handle<Geom_Circle> GC = new Geom_Circle(C);
  Init(GC, V1, V2);
}

//=================================================================================================

BRepLib_MakeEdge::BRepLib_MakeEdge(const gp_Elips& E)
{
  occ::handle<Geom_Ellipse> GE = new Geom_Ellipse(E);
  Init(GE);
}

//=================================================================================================

BRepLib_MakeEdge::BRepLib_MakeEdge(const gp_Elips&     E,
                                   const double p1,
                                   const double p2)
{
  occ::handle<Geom_Ellipse> GE = new Geom_Ellipse(E);
  Init(GE, p1, p2);
}

//=================================================================================================

BRepLib_MakeEdge::BRepLib_MakeEdge(const gp_Elips& E, const gp_Pnt& P1, const gp_Pnt& P2)
{
  occ::handle<Geom_Ellipse> GE = new Geom_Ellipse(E);
  Init(GE, P1, P2);
}

//=================================================================================================

BRepLib_MakeEdge::BRepLib_MakeEdge(const gp_Elips&      E,
                                   const TopoDS_Vertex& V1,
                                   const TopoDS_Vertex& V2)
{
  occ::handle<Geom_Ellipse> GE = new Geom_Ellipse(E);
  Init(GE, V1, V2);
}

//=================================================================================================

BRepLib_MakeEdge::BRepLib_MakeEdge(const gp_Hypr& H)
{
  occ::handle<Geom_Hyperbola> GH = new Geom_Hyperbola(H);
  Init(GH);
}

//=================================================================================================

BRepLib_MakeEdge::BRepLib_MakeEdge(const gp_Hypr& H, const double p1, const double p2)
{
  occ::handle<Geom_Hyperbola> GH = new Geom_Hyperbola(H);
  Init(GH, p1, p2);
}

//=================================================================================================

BRepLib_MakeEdge::BRepLib_MakeEdge(const gp_Hypr& H, const gp_Pnt& P1, const gp_Pnt& P2)
{
  occ::handle<Geom_Hyperbola> GH = new Geom_Hyperbola(H);
  Init(GH, P1, P2);
}

//=================================================================================================

BRepLib_MakeEdge::BRepLib_MakeEdge(const gp_Hypr&       H,
                                   const TopoDS_Vertex& V1,
                                   const TopoDS_Vertex& V2)
{
  occ::handle<Geom_Hyperbola> GH = new Geom_Hyperbola(H);
  Init(GH, V1, V2);
}

//=================================================================================================

BRepLib_MakeEdge::BRepLib_MakeEdge(const gp_Parab& P)
{
  occ::handle<Geom_Parabola> GP = new Geom_Parabola(P);
  Init(GP);
}

//=================================================================================================

BRepLib_MakeEdge::BRepLib_MakeEdge(const gp_Parab&     P,
                                   const double p1,
                                   const double p2)
{
  occ::handle<Geom_Parabola> GP = new Geom_Parabola(P);
  Init(GP, p1, p2);
}

//=================================================================================================

BRepLib_MakeEdge::BRepLib_MakeEdge(const gp_Parab& P, const gp_Pnt& P1, const gp_Pnt& P2)
{
  occ::handle<Geom_Parabola> GP = new Geom_Parabola(P);
  Init(GP, P1, P2);
}

//=================================================================================================

BRepLib_MakeEdge::BRepLib_MakeEdge(const gp_Parab&      P,
                                   const TopoDS_Vertex& V1,
                                   const TopoDS_Vertex& V2)
{
  occ::handle<Geom_Parabola> GP = new Geom_Parabola(P);
  Init(GP, V1, V2);
}

//=================================================================================================

BRepLib_MakeEdge::BRepLib_MakeEdge(const occ::handle<Geom_Curve>& L)
{
  Init(L);
}

//=================================================================================================

BRepLib_MakeEdge::BRepLib_MakeEdge(const occ::handle<Geom_Curve>& L,
                                   const double       p1,
                                   const double       p2)
{
  Init(L, p1, p2);
}

//=================================================================================================

BRepLib_MakeEdge::BRepLib_MakeEdge(const occ::handle<Geom_Curve>& L, const gp_Pnt& P1, const gp_Pnt& P2)
{
  Init(L, P1, P2);
}

//=================================================================================================

BRepLib_MakeEdge::BRepLib_MakeEdge(const occ::handle<Geom_Curve>& L,
                                   const TopoDS_Vertex&      V1,
                                   const TopoDS_Vertex&      V2)
{
  Init(L, V1, V2);
}

//=================================================================================================

BRepLib_MakeEdge::BRepLib_MakeEdge(const occ::handle<Geom_Curve>& L,
                                   const gp_Pnt&             P1,
                                   const gp_Pnt&             P2,
                                   const double       p1,
                                   const double       p2)
{
  Init(L, P1, P2, p1, p2);
}

//=================================================================================================

BRepLib_MakeEdge::BRepLib_MakeEdge(const occ::handle<Geom_Curve>& L,
                                   const TopoDS_Vertex&      V1,
                                   const TopoDS_Vertex&      V2,
                                   const double       p1,
                                   const double       p2)
{
  Init(L, V1, V2, p1, p2);
}

//=================================================================================================

BRepLib_MakeEdge::BRepLib_MakeEdge(const occ::handle<Geom2d_Curve>& L, const occ::handle<Geom_Surface>& S)
{
  Init(L, S);
}

//=================================================================================================

BRepLib_MakeEdge::BRepLib_MakeEdge(const occ::handle<Geom2d_Curve>& L,
                                   const occ::handle<Geom_Surface>& S,
                                   const double         p1,
                                   const double         p2)
{
  Init(L, S, p1, p2);
}

//=================================================================================================

BRepLib_MakeEdge::BRepLib_MakeEdge(const occ::handle<Geom2d_Curve>& L,
                                   const occ::handle<Geom_Surface>& S,
                                   const gp_Pnt&               P1,
                                   const gp_Pnt&               P2)
{
  Init(L, S, P1, P2);
}

//=================================================================================================

BRepLib_MakeEdge::BRepLib_MakeEdge(const occ::handle<Geom2d_Curve>& L,
                                   const occ::handle<Geom_Surface>& S,
                                   const TopoDS_Vertex&        V1,
                                   const TopoDS_Vertex&        V2)
{
  Init(L, S, V1, V2);
}

//=================================================================================================

BRepLib_MakeEdge::BRepLib_MakeEdge(const occ::handle<Geom2d_Curve>& L,
                                   const occ::handle<Geom_Surface>& S,
                                   const gp_Pnt&               P1,
                                   const gp_Pnt&               P2,
                                   const double         p1,
                                   const double         p2)
{
  Init(L, S, P1, P2, p1, p2);
}

//=================================================================================================

BRepLib_MakeEdge::BRepLib_MakeEdge(const occ::handle<Geom2d_Curve>& L,
                                   const occ::handle<Geom_Surface>& S,
                                   const TopoDS_Vertex&        V1,
                                   const TopoDS_Vertex&        V2,
                                   const double         p1,
                                   const double         p2)
{
  Init(L, S, V1, V2, p1, p2);
}

//=================================================================================================

void BRepLib_MakeEdge::Init(const occ::handle<Geom_Curve>& C)
{
  Init(C, C->FirstParameter(), C->LastParameter());
}

//=================================================================================================

void BRepLib_MakeEdge::Init(const occ::handle<Geom_Curve>& C,
                            const double       p1,
                            const double       p2)
{
  //  BRep_Builder B;

  TopoDS_Vertex V1, V2;
  Init(C, V1, V2, p1, p2);
}

//=================================================================================================

void BRepLib_MakeEdge::Init(const occ::handle<Geom_Curve>& C, const gp_Pnt& P1, const gp_Pnt& P2)
{
  double Tol = BRepLib::Precision();

  BRep_Builder  B;
  TopoDS_Vertex V1, V2;
  B.MakeVertex(V1, P1, Tol);
  if (P1.Distance(P2) < Tol)
    V2 = V1;
  else
    B.MakeVertex(V2, P2, Tol);

  Init(C, V1, V2);
}

//=================================================================================================

void BRepLib_MakeEdge::Init(const occ::handle<Geom_Curve>& C,
                            const TopoDS_Vertex&      V1,
                            const TopoDS_Vertex&      V2)
{
  // try projecting the vertices on the curve

  double p1, p2;

  if (V1.IsNull())
    p1 = C->FirstParameter();
  else if (!Project(C, V1, p1))
  {
    myError = BRepLib_PointProjectionFailed;
    return;
  }
  if (V2.IsNull())
    p2 = C->LastParameter();
  else if (!Project(C, V2, p2))
  {
    myError = BRepLib_PointProjectionFailed;
    return;
  }

  Init(C, V1, V2, p1, p2);
}

//=================================================================================================

void BRepLib_MakeEdge::Init(const occ::handle<Geom_Curve>& C,
                            const gp_Pnt&             P1,
                            const gp_Pnt&             P2,
                            const double       p1,
                            const double       p2)
{
  double Tol = BRepLib::Precision();
  BRep_Builder  B;

  TopoDS_Vertex V1, V2;
  B.MakeVertex(V1, P1, Tol);
  if (P1.Distance(P2) < Tol)
    V2 = V1;
  else
    B.MakeVertex(V2, P2, Tol);

  Init(C, V1, V2, p1, p2);
}

//=======================================================================
// function : Init
// purpose  : this one really makes the job ...
//=======================================================================

void BRepLib_MakeEdge::Init(const occ::handle<Geom_Curve>& CC,
                            const TopoDS_Vertex&      VV1,
                            const TopoDS_Vertex&      VV2,
                            const double       pp1,
                            const double       pp2)
{
  // kill trimmed curves
  occ::handle<Geom_Curve>        C  = CC;
  occ::handle<Geom_TrimmedCurve> CT = occ::down_cast<Geom_TrimmedCurve>(C);
  while (!CT.IsNull())
  {
    C  = CT->BasisCurve();
    CT = occ::down_cast<Geom_TrimmedCurve>(C);
  }

  // check parameters
  double           p1       = pp1;
  double           p2       = pp2;
  double           cf       = C->FirstParameter();
  double           cl       = C->LastParameter();
  constexpr double epsilon  = Precision::PConfusion();
  bool        periodic = C->IsPeriodic();
  GeomAdaptor_Curve       aCA(C);

  TopoDS_Vertex V1, V2;
  if (periodic)
  {
    // adjust in period
    ElCLib::AdjustPeriodic(cf, cl, epsilon, p1, p2);
    V1 = VV1;
    V2 = VV2;
  }
  else
  {
    // reordonate
    if (p1 < p2)
    {
      V1 = VV1;
      V2 = VV2;
    }
    else
    {
      V2              = VV1;
      V1              = VV2;
      double x = p1;
      p1              = p2;
      p2              = x;
    }

    // check range
    if ((cf - p1 > epsilon) || (p2 - cl > epsilon))
    {
      myError = BRepLib_ParameterOutOfRange;
      return;
    }

    // check ponctuallity
    if ((p2 - p1) <= gp::Resolution())
    {
      myError = BRepLib_LineThroughIdenticPoints;
      return;
    }
  }

  // compute points on the curve
  bool p1inf = Precision::IsNegativeInfinite(p1);
  bool p2inf = Precision::IsPositiveInfinite(p2);
  gp_Pnt           P1, P2;
  if (!p1inf)
    P1 = aCA.Value(p1);
  if (!p2inf)
    P2 = aCA.Value(p2);

  double preci = BRepLib::Precision();
  BRep_Builder  B;

  // check for closed curve
  bool closed      = false;
  bool degenerated = false;
  if (!p1inf && !p2inf)
    closed = (P1.Distance(P2) <= preci);

  // check if the vertices are on the curve
  if (closed)
  {
    if (V1.IsNull() && V2.IsNull())
    {
      B.MakeVertex(V1, P1, preci);
      V2 = V1;
    }
    else if (V1.IsNull())
      V1 = V2;
    else if (V2.IsNull())
      V2 = V1;
    else
    {
      if (!V1.IsSame(V2))
      {
        myError = BRepLib_DifferentPointsOnClosedCurve;
        return;
      }
      else if (P1.Distance(BRep_Tool::Pnt(V1)) > std::max(preci, BRep_Tool::Tolerance(V1)))
      {
        myError = BRepLib_DifferentPointsOnClosedCurve;
        return;
      }
      else
      {
        gp_Pnt PM = aCA.Value((p1 + p2) / 2);
        if (P1.Distance(PM) < preci)
          degenerated = true;
      }
    }
  }

  else
  { // not closed

    if (p1inf)
    {
      if (!V1.IsNull())
      {
        myError = BRepLib_PointWithInfiniteParameter;
        return;
      }
    }
    else
    {
      if (V1.IsNull())
      {
        B.MakeVertex(V1, P1, preci);
      }
      else if (P1.Distance(BRep_Tool::Pnt(V1)) > std::max(preci, BRep_Tool::Tolerance(V1)))
      {
        myError = BRepLib_DifferentsPointAndParameter;
        return;
      }
    }

    if (p2inf)
    {
      if (!V2.IsNull())
      {
        myError = BRepLib_PointWithInfiniteParameter;
        return;
      }
    }
    else
    {
      if (V2.IsNull())
      {
        B.MakeVertex(V2, P2, preci);
      }
      else if (P2.Distance(BRep_Tool::Pnt(V2)) > std::max(preci, BRep_Tool::Tolerance(V2)))
      {
        myError = BRepLib_DifferentsPointAndParameter;
        return;
      }
    }
  }

  V1.Orientation(TopAbs_FORWARD);
  V2.Orientation(TopAbs_REVERSED);
  myVertex1 = V1;
  myVertex2 = V2;

  TopoDS_Edge& E = TopoDS::Edge(myShape);
  B.MakeEdge(E, C, preci);
  if (!V1.IsNull())
  {
    B.Add(E, V1);
  }
  if (!V2.IsNull())
  {
    B.Add(E, V2);
  }
  B.Range(E, p1, p2);
  B.Degenerated(E, degenerated);
  E.Closed(closed);

  myError = BRepLib_EdgeDone;
  Done();
}

//=================================================================================================

void BRepLib_MakeEdge::Init(const occ::handle<Geom2d_Curve>& C, const occ::handle<Geom_Surface>& S)
{
  Init(C, S, C->FirstParameter(), C->LastParameter());
}

//=================================================================================================

void BRepLib_MakeEdge::Init(const occ::handle<Geom2d_Curve>& C,
                            const occ::handle<Geom_Surface>& S,
                            const double         p1,
                            const double         p2)
{
  //  BRep_Builder B;

  TopoDS_Vertex V1, V2;
  Init(C, S, V1, V2, p1, p2);
}

//=================================================================================================

void BRepLib_MakeEdge::Init(const occ::handle<Geom2d_Curve>& C,
                            const occ::handle<Geom_Surface>& S,
                            const gp_Pnt&               P1,
                            const gp_Pnt&               P2)
{
  double Tol = BRepLib::Precision();

  BRep_Builder  B;
  TopoDS_Vertex V1, V2;
  B.MakeVertex(V1, P1, Tol);
  if (P1.Distance(P2) < Tol)
    V2 = V1;
  else
    B.MakeVertex(V2, P2, Tol);

  Init(C, S, V1, V2);
}

//=================================================================================================

void BRepLib_MakeEdge::Init(const occ::handle<Geom2d_Curve>& C,
                            const occ::handle<Geom_Surface>& S,
                            const TopoDS_Vertex&        V1,
                            const TopoDS_Vertex&        V2)
{
  // try projecting the vertices on the curve

  double p1, p2;

  if (V1.IsNull())
    p1 = C->FirstParameter();
  else if (!Project(C, S, V1, p1))
  {
    myError = BRepLib_PointProjectionFailed;
    return;
  }
  if (V2.IsNull())
    p2 = C->LastParameter();
  else if (!Project(C, S, V2, p2))
  {
    myError = BRepLib_PointProjectionFailed;
    return;
  }

  Init(C, S, V1, V2, p1, p2);
}

//=================================================================================================

void BRepLib_MakeEdge::Init(const occ::handle<Geom2d_Curve>& C,
                            const occ::handle<Geom_Surface>& S,
                            const gp_Pnt&               P1,
                            const gp_Pnt&               P2,
                            const double         p1,
                            const double         p2)
{
  double Tol = BRepLib::Precision();
  BRep_Builder  B;

  TopoDS_Vertex V1, V2;
  B.MakeVertex(V1, P1, Tol);
  if (P1.Distance(P2) < Tol)
    V2 = V1;
  else
    B.MakeVertex(V2, P2, Tol);

  Init(C, S, V1, V2, p1, p2);
}

//=======================================================================
// function : Init
// purpose  : this one really makes the job ...
//=======================================================================

void BRepLib_MakeEdge::Init(const occ::handle<Geom2d_Curve>& CC,
                            const occ::handle<Geom_Surface>& S,
                            const TopoDS_Vertex&        VV1,
                            const TopoDS_Vertex&        VV2,
                            const double         pp1,
                            const double         pp2)
{
  // kill trimmed curves
  occ::handle<Geom2d_Curve>        C  = CC;
  occ::handle<Geom2d_TrimmedCurve> CT = occ::down_cast<Geom2d_TrimmedCurve>(C);
  while (!CT.IsNull())
  {
    C  = CT->BasisCurve();
    CT = occ::down_cast<Geom2d_TrimmedCurve>(C);
  }

  // check parameters
  double           p1       = pp1;
  double           p2       = pp2;
  double           cf       = C->FirstParameter();
  double           cl       = C->LastParameter();
  constexpr double epsilon  = Precision::PConfusion();
  bool        periodic = C->IsPeriodic();

  TopoDS_Vertex    V1, V2;
  bool reverse = false;

  if (periodic)
  {
    // adjust in period
    ElCLib::AdjustPeriodic(cf, cl, epsilon, p1, p2);
    V1 = VV1;
    V2 = VV2;
  }
  else
  {
    // reordonate
    if (p1 < p2)
    {
      V1 = VV1;
      V2 = VV2;
    }
    else
    {
      V2              = VV1;
      V1              = VV2;
      double x = p1;
      p1              = p2;
      p2              = x;
      reverse         = true;
    }

    // check range
    if ((cf - p1 > epsilon) || (p2 - cl > epsilon))
    {
      myError = BRepLib_ParameterOutOfRange;
      return;
    }
  }

  // compute points on the curve
  bool p1inf = Precision::IsNegativeInfinite(p1);
  bool p2inf = Precision::IsPositiveInfinite(p2);
  gp_Pnt           P1, P2;
  gp_Pnt2d         P2d1, P2d2;
  if (!p1inf)
  {
    P2d1 = C->Value(p1);
    P1   = S->Value(P2d1.X(), P2d1.Y());
  }
  if (!p2inf)
  {
    P2d2 = C->Value(p2);
    P2   = S->Value(P2d2.X(), P2d2.Y());
  }

  double preci = BRepLib::Precision();
  BRep_Builder  B;

  // check for closed curve
  bool closed = false;
  if (!p1inf && !p2inf)
    closed = (P1.Distance(P2) <= preci);

  // check if the vertices are on the curve
  if (closed)
  {
    if (V1.IsNull() && V2.IsNull())
    {
      B.MakeVertex(V1, P1, preci);
      V2 = V1;
    }
    else if (V1.IsNull())
      V1 = V2;
    else if (V2.IsNull())
      V2 = V1;
    else
    {
      if (!V1.IsSame(V2))
      {
        myError = BRepLib_DifferentPointsOnClosedCurve;
        return;
      }
      else if (P1.Distance(BRep_Tool::Pnt(V1)) > std::max(preci, BRep_Tool::Tolerance(V1)))
      {
        myError = BRepLib_DifferentPointsOnClosedCurve;
        return;
      }
    }
  }

  else
  { // not closed

    if (p1inf)
    {
      if (!V1.IsNull())
      {
        myError = BRepLib_PointWithInfiniteParameter;
        return;
      }
    }
    else
    {
      if (V1.IsNull())
      {
        B.MakeVertex(V1, P1, preci);
      }
      else if (P1.Distance(BRep_Tool::Pnt(V1)) > std::max(preci, BRep_Tool::Tolerance(V1)))
      {
        myError = BRepLib_DifferentsPointAndParameter;
        return;
      }
    }

    if (p2inf)
    {
      if (!V2.IsNull())
      {
        myError = BRepLib_PointWithInfiniteParameter;
        return;
      }
    }
    else
    {
      if (V2.IsNull())
      {
        B.MakeVertex(V2, P2, preci);
      }
      else if (P2.Distance(BRep_Tool::Pnt(V2)) > std::max(preci, BRep_Tool::Tolerance(V2)))
      {
        myError = BRepLib_DifferentsPointAndParameter;
        return;
      }
    }
  }

  V1.Orientation(TopAbs_FORWARD);
  V2.Orientation(TopAbs_REVERSED);
  myVertex1 = V1;
  myVertex2 = V2;

  TopoDS_Edge& E = TopoDS::Edge(myShape);
  B.MakeEdge(E);
  B.UpdateEdge(E, C, S, TopLoc_Location(), preci);

  if (!V1.IsNull())
  {
    B.Add(E, V1);
  }
  if (!V2.IsNull())
  {
    B.Add(E, V2);
  }
  B.Range(E, p1, p2);

  if (reverse)
    E.Orientation(TopAbs_REVERSED);

  myError = BRepLib_EdgeDone;
  Done();
}

//=================================================================================================

BRepLib_EdgeError BRepLib_MakeEdge::Error() const
{
  return myError;
}

//=================================================================================================

const TopoDS_Edge& BRepLib_MakeEdge::Edge()
{
  return TopoDS::Edge(Shape());
}

//=================================================================================================

const TopoDS_Vertex& BRepLib_MakeEdge::Vertex1() const
{
  Check();
  return myVertex1;
}

//=================================================================================================

const TopoDS_Vertex& BRepLib_MakeEdge::Vertex2() const
{
  Check();
  return myVertex2;
}

//=================================================================================================

BRepLib_MakeEdge::operator TopoDS_Edge()
{
  return Edge();
}
