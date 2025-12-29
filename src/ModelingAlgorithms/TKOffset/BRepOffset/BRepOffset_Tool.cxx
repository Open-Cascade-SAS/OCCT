// Created on: 1995-10-23
// Created by: Yves FRICAUD
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

#include <BRepOffset_Tool.hxx>

#include <Bnd_Box2d.hxx>
#include <BndLib_Add3dCurve.hxx>
#include <BOPAlgo_PaveFiller.hxx>
#include <BOPDS_DS.hxx>
#include <BOPTools_AlgoTools.hxx>
#include <BOPTools_AlgoTools2D.hxx>
#include <BRep_TEdge.hxx>
#include <BRep_Builder.hxx>
#include <BRepAdaptor_Curve.hxx>
#include <BRepAdaptor_Curve2d.hxx>
#include <BRepAdaptor_Surface.hxx>
#include <BRepAlgo_AsDes.hxx>
#include <BRepAlgo_Image.hxx>
#include <BRepLib.hxx>
#include <BRepLib_MakeEdge.hxx>
#include <BRepLib_MakeFace.hxx>
#include <BRepLib_MakeVertex.hxx>
#include <BRepOffset_Analyse.hxx>
#include <BRepOffset_Interval.hxx>
#include <NCollection_List.hxx>
#include <BRepTools.hxx>
#include <BRepTools_Modifier.hxx>
#include <BRepTools_WireExplorer.hxx>
#include <BRepTopAdaptor_FClass2d.hxx>
#include <ElCLib.hxx>
#include <ElSLib.hxx>
#include <Extrema_ExtPC2d.hxx>
#include <BRepExtrema_DistShapeShape.hxx>
#include <GCPnts_AbscissaPoint.hxx>
#include <GCPnts_QuasiUniformDeflection.hxx>
#include <Geom2d_BezierCurve.hxx>
#include <Geom2d_BSplineCurve.hxx>
#include <Geom2d_Circle.hxx>
#include <Geom2d_Curve.hxx>
#include <Geom2d_Ellipse.hxx>
#include <Geom2d_Hyperbola.hxx>
#include <Geom2d_Line.hxx>
#include <Geom2d_Parabola.hxx>
#include <Geom2d_TrimmedCurve.hxx>
#include <Geom2dAdaptor_Curve.hxx>
#include <Geom2dConvert_ApproxCurve.hxx>
#include <Geom2dConvert_CompCurveToBSplineCurve.hxx>
#include <Geom2dInt_GInter.hxx>
#include <Geom_BezierSurface.hxx>
#include <Geom_BSplineCurve.hxx>
#include <Geom_Conic.hxx>
#include <Geom_ConicalSurface.hxx>
#include <Geom_Curve.hxx>
#include <Geom_Line.hxx>
#include <Geom_OffsetSurface.hxx>
#include <Geom_Plane.hxx>
#include <Geom_RectangularTrimmedSurface.hxx>
#include <Geom_Surface.hxx>
#include <Geom_SurfaceOfLinearExtrusion.hxx>
#include <Geom_SurfaceOfRevolution.hxx>
#include <Geom_TrimmedCurve.hxx>
#include <GeomAdaptor_Surface.hxx>
#include <GeomAPI.hxx>
#include <GeomAPI_ExtremaCurveCurve.hxx>
#include <GeomAPI_ProjectPointOnCurve.hxx>
#include <GeomConvert_ApproxCurve.hxx>
#include <GeomConvert_CompCurveToBSplineCurve.hxx>
#include <GeomInt_IntSS.hxx>
#include <GeomLib.hxx>
#include <GeomProjLib.hxx>
#include <gp.hxx>
#include <gp_Pnt.hxx>
#include <gp_Vec.hxx>
#include <IntRes2d_IntersectionPoint.hxx>
#include <IntRes2d_IntersectionSegment.hxx>
#include <IntTools_FaceFace.hxx>
#include <Precision.hxx>
#include <ProjLib_ProjectedCurve.hxx>
#include <ShapeCustom_Curve2d.hxx>
#include <Standard_ConstructionError.hxx>
#include <TopAbs.hxx>
#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Compound.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Iterator.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopoDS_Wire.hxx>
#include <TopTools_ShapeMapHasher.hxx>
#include <NCollection_IndexedDataMap.hxx>
#include <NCollection_Sequence.hxx>

#include <cstdio>

// The constant defines the maximal value to enlarge surfaces.
// It is limited to 1.e+7. This limitation is justified by the
// floating point format. As we can have only 15
// valuable decimal numbers, then during intersection of surfaces with
// bounds of 1.e+8 the possible inaccuracy might appear already in seventh
// decimal place which will be more than Precision::Confusion value -
// 1.e-7, default tolerance value for the section curves.
// By decreasing the max enlarge value to 1.e+7 the inaccuracy will be
// shifted to eighth decimal place, i.e. the inaccuracy will be
// decreased to values less than 1.e-7.
const double TheInfini = 1.e+7;

// tma: for new boolean operation

#ifdef OCCT_DEBUG
static bool AffichExtent = false;
#endif

static void PerformPlanes(const TopoDS_Face&              theFace1,
                          const TopoDS_Face&              theFace2,
                          const TopAbs_State              theState,
                          NCollection_List<TopoDS_Shape>& theL1,
                          NCollection_List<TopoDS_Shape>& theL2);

static void UpdateVertexTolerances(const TopoDS_Face& theFace);

inline bool IsInf(const double theVal);

//=================================================================================================

void BRepOffset_Tool::EdgeVertices(const TopoDS_Edge& E, TopoDS_Vertex& V1, TopoDS_Vertex& V2)
{
  if (E.Orientation() == TopAbs_REVERSED)
  {
    TopExp::Vertices(E, V2, V1);
  }
  else
  {
    TopExp::Vertices(E, V1, V2);
  }
}

//=================================================================================================

static void FindPeriod(const TopoDS_Face& F, double& umin, double& umax, double& vmin, double& vmax)
{

  Bnd_Box2d       B;
  TopExp_Explorer exp;
  for (exp.Init(F, TopAbs_EDGE); exp.More(); exp.Next())
  {
    const TopoDS_Edge& E = TopoDS::Edge(exp.Current());

    double                          pf, pl;
    const occ::handle<Geom2d_Curve> C = BRep_Tool::CurveOnSurface(E, F, pf, pl);
    if (C.IsNull())
      return;
    Geom2dAdaptor_Curve PC(C, pf, pl);
    double              i, nbp = 20;
    if (PC.GetType() == GeomAbs_Line)
      nbp = 2;
    double   step = (pl - pf) / nbp;
    gp_Pnt2d P;
    PC.D0(pf, P);
    B.Add(P);
    for (i = 2; i < nbp; i++)
    {
      pf += step;
      PC.D0(pf, P);
      B.Add(P);
    }
    PC.D0(pl, P);
    B.Add(P);
    B.Get(umin, vmin, umax, vmax);
  }
}

//=======================================================================
// function : PutInBounds
// purpose  : Recadre la courbe 2d dans les bounds de la face
//=======================================================================

static void PutInBounds(const TopoDS_Face& F, const TopoDS_Edge& E, occ::handle<Geom2d_Curve>& C2d)
{
  double umin, umax, vmin, vmax;
  double f, l;
  BRep_Tool::Range(E, f, l);

  TopLoc_Location           L; // Recup S avec la location pour eviter la copie.
  occ::handle<Geom_Surface> S = BRep_Tool::Surface(F, L);

  if (S->IsInstance(STANDARD_TYPE(Geom_RectangularTrimmedSurface)))
  {
    S = occ::down_cast<Geom_RectangularTrimmedSurface>(S)->BasisSurface();
  }
  //---------------
  // Recadre en U.
  //---------------
  if (!S->IsUPeriodic() && !S->IsVPeriodic())
    return;

  FindPeriod(F, umin, umax, vmin, vmax);

  if (S->IsUPeriodic())
  {
    double   period = S->UPeriod();
    double   eps    = period * 1.e-6;
    gp_Pnt2d Pf     = C2d->Value(f);
    gp_Pnt2d Pl     = C2d->Value(l);
    gp_Pnt2d Pm     = C2d->Value(0.34 * f + 0.66 * l);
    double   minC   = std::min(Pf.X(), Pl.X());
    minC            = std::min(minC, Pm.X());
    double maxC     = std::max(Pf.X(), Pl.X());
    maxC            = std::max(maxC, Pm.X());
    double du       = 0.;
    if (minC < umin - eps)
    {
      du = (int((umin - minC) / period) + 1) * period;
    }
    if (minC > umax + eps)
    {
      du = -(int((minC - umax) / period) + 1) * period;
    }
    if (du != 0)
    {
      gp_Vec2d T1(du, 0.);
      C2d->Translate(T1);
      minC += du;
      maxC += du;
    }
    // Ajuste au mieux la courbe dans le domaine.
    if (maxC > umax + 100 * eps)
    {
      double d1 = maxC - umax;
      double d2 = umin - minC + period;
      if (d2 < d1)
        du = -period;
      if (du != 0.)
      {
        gp_Vec2d T2(du, 0.);
        C2d->Translate(T2);
      }
    }
  }
  //------------------
  // Recadre en V.
  //------------------
  if (S->IsVPeriodic())
  {
    double   period = S->VPeriod();
    double   eps    = period * 1.e-6;
    gp_Pnt2d Pf     = C2d->Value(f);
    gp_Pnt2d Pl     = C2d->Value(l);
    gp_Pnt2d Pm     = C2d->Value(0.34 * f + 0.66 * l);
    double   minC   = std::min(Pf.Y(), Pl.Y());
    minC            = std::min(minC, Pm.Y());
    double maxC     = std::max(Pf.Y(), Pl.Y());
    maxC            = std::max(maxC, Pm.Y());
    double dv       = 0.;
    if (minC < vmin - eps)
    {
      dv = (int((vmin - minC) / period) + 1) * period;
    }
    if (minC > vmax + eps)
    {
      dv = -(int((minC - vmax) / period) + 1) * period;
    }
    if (dv != 0)
    {
      gp_Vec2d T1(0., dv);
      C2d->Translate(T1);
      minC += dv;
      maxC += dv;
    }
    // Ajuste au mieux la courbe dans le domaine.
    if (maxC > vmax + 100 * eps)
    {
      double d1 = maxC - vmax;
      double d2 = vmin - minC + period;
      if (d2 < d1)
        dv = -period;
      if (dv != 0.)
      {
        gp_Vec2d T2(0., dv);
        C2d->Translate(T2);
      }
    }
  }
}

//=================================================================================================

double BRepOffset_Tool::Gabarit(const occ::handle<Geom_Curve>& aCurve)
{
  GeomAdaptor_Curve GC(aCurve);
  Bnd_Box           aBox;
  BndLib_Add3dCurve::Add(GC, Precision::Confusion(), aBox);
  double aXmin, aYmin, aZmin, aXmax, aYmax, aZmax, dist;
  aBox.Get(aXmin, aYmin, aZmin, aXmax, aYmax, aZmax);
  dist = std::max((aXmax - aXmin), (aYmax - aYmin));
  dist = std::max(dist, (aZmax - aZmin));
  return dist;
}

//=================================================================================================

static void BuildPCurves(const TopoDS_Edge& E, const TopoDS_Face& F)
{
  double                    ff, ll;
  occ::handle<Geom2d_Curve> C2d = BRep_Tool::CurveOnSurface(E, F, ff, ll);
  if (!C2d.IsNull())
    return;

  // double Tolerance = std::max(Precision::Confusion(),BRep_Tool::Tolerance(E));
  constexpr double Tolerance = Precision::Confusion();

  BRepAdaptor_Surface AS(F, false);
  BRepAdaptor_Curve   AC(E);

  // Try to find pcurve on a bound of BSpline or Bezier surface
  occ::handle<Geom_Surface>  theSurf = BRep_Tool::Surface(F);
  occ::handle<Standard_Type> typS    = theSurf->DynamicType();
  if (typS == STANDARD_TYPE(Geom_OffsetSurface))
    typS = occ::down_cast<Geom_OffsetSurface>(theSurf)->BasisSurface()->DynamicType();
  if (typS == STANDARD_TYPE(Geom_BezierSurface) || typS == STANDARD_TYPE(Geom_BSplineSurface))
  {
    gp_Pnt          fpoint  = AC.Value(AC.FirstParameter());
    gp_Pnt          lpoint  = AC.Value(AC.LastParameter());
    TopoDS_Face     theFace = BRepLib_MakeFace(theSurf, Precision::Confusion());
    double          U1 = 0., U2 = 0., TolProj = 1.e-4; // 1.e-5;
    TopoDS_Edge     theEdge;
    TopExp_Explorer Explo;
    Explo.Init(theFace, TopAbs_EDGE);
    for (; Explo.More(); Explo.Next())
    {
      TopoDS_Edge       anEdge = TopoDS::Edge(Explo.Current());
      BRepAdaptor_Curve aCurve(anEdge);
      Extrema_ExtPC     fextr(fpoint, aCurve);
      if (!fextr.IsDone() || fextr.NbExt() < 1)
        continue;
      double dist2, dist2min = RealLast();
      int    i;
      for (i = 1; i <= fextr.NbExt(); i++)
      {
        dist2 = fextr.SquareDistance(i);
        if (dist2 < dist2min)
        {
          dist2min = dist2;
          U1       = fextr.Point(i).Parameter();
        }
      }
      if (dist2min > TolProj * TolProj)
        continue;
      Extrema_ExtPC lextr(lpoint, aCurve);
      if (!lextr.IsDone() || lextr.NbExt() < 1)
        continue;
      dist2min = RealLast();
      for (i = 1; i <= lextr.NbExt(); i++)
      {
        dist2 = lextr.SquareDistance(i);
        if (dist2 < dist2min)
        {
          dist2min = dist2;
          U2       = lextr.Point(i).Parameter();
        }
      }
      if (dist2min <= TolProj * TolProj)
      {
        theEdge = anEdge;
        break;
      }
    } // for (; Explo.More(); Explo.Current())

    if (!theEdge.IsNull())
    {
      // Construction of pcurve
      if (U2 < U1)
      {
        double temp = U1;
        U1          = U2;
        U2          = temp;
      }
      double f, l;
      C2d = BRep_Tool::CurveOnSurface(theEdge, theFace, f, l);
      C2d = new Geom2d_TrimmedCurve(C2d, U1, U2);

      if (theSurf->IsUPeriodic() || theSurf->IsVPeriodic())
        PutInBounds(F, E, C2d);

      BRep_Builder B;
      B.UpdateEdge(E, C2d, F, BRep_Tool::Tolerance(E));
      BRepLib::SameRange(E);

      return;
    }
  } // if (typS == ...

  occ::handle<BRepAdaptor_Surface> HS = new BRepAdaptor_Surface(AS);
  occ::handle<BRepAdaptor_Curve>   HC = new BRepAdaptor_Curve(AC);

  ProjLib_ProjectedCurve Proj(HS, HC, Tolerance);

  switch (Proj.GetType())
  {

    case GeomAbs_Line:
      C2d = new Geom2d_Line(Proj.Line());
      break;

    case GeomAbs_Circle:
      C2d = new Geom2d_Circle(Proj.Circle());
      break;

    case GeomAbs_Ellipse:
      C2d = new Geom2d_Ellipse(Proj.Ellipse());
      break;

    case GeomAbs_Parabola:
      C2d = new Geom2d_Parabola(Proj.Parabola());
      break;

    case GeomAbs_Hyperbola:
      C2d = new Geom2d_Hyperbola(Proj.Hyperbola());
      break;

    case GeomAbs_BezierCurve:
      C2d = Proj.Bezier();
      break;

    case GeomAbs_BSplineCurve:
      C2d = Proj.BSpline();
      break;
    default:
      break;
  }

  if (AS.IsUPeriodic() || AS.IsVPeriodic())
  {
    PutInBounds(F, E, C2d);
  }
  if (!C2d.IsNull())
  {
    BRep_Builder B;
    B.UpdateEdge(E, C2d, F, BRep_Tool::Tolerance(E));
  }
  else
  {
    throw Standard_ConstructionError("BRepOffset_Tool::BuildPCurves");
  }
}

//=================================================================================================

void BRepOffset_Tool::OrientSection(const TopoDS_Edge&  E,
                                    const TopoDS_Face&  F1,
                                    const TopoDS_Face&  F2,
                                    TopAbs_Orientation& O1,
                                    TopAbs_Orientation& O2)
{
  TopLoc_Location L;
  double          f, l;

  occ::handle<Geom_Surface> S1 = BRep_Tool::Surface(F1);
  occ::handle<Geom_Surface> S2 = BRep_Tool::Surface(F2);
  occ::handle<Geom2d_Curve> C1 = BRep_Tool::CurveOnSurface(E, F1, f, l);
  occ::handle<Geom2d_Curve> C2 = BRep_Tool::CurveOnSurface(E, F2, f, l);
  occ::handle<Geom_Curve>   C  = BRep_Tool::Curve(E, L, f, l);

  BRepAdaptor_Curve BAcurve(E);

  GCPnts_AbscissaPoint AP(BAcurve, GCPnts_AbscissaPoint::Length(BAcurve) / 2.0, f);
  double               ParOnC;

  if (AP.IsDone())
    ParOnC = AP.Parameter();
  else
    ParOnC = BOPTools_AlgoTools2D::IntermediatePoint(f, l);

  gp_Vec T1 = C->DN(ParOnC, 1).Transformed(L.Transformation());
  if (T1.SquareMagnitude() > gp::Resolution())
  {
    T1.Normalize();
  }

  gp_Pnt2d P = C1->Value(ParOnC);
  gp_Pnt   P3;
  gp_Vec   D1U, D1V;

  S1->D1(P.X(), P.Y(), P3, D1U, D1V);
  gp_Vec DN1(D1U ^ D1V);
  if (F1.Orientation() == TopAbs_REVERSED)
    DN1.Reverse();

  P = C2->Value(ParOnC);
  S2->D1(P.X(), P.Y(), P3, D1U, D1V);
  gp_Vec DN2(D1U ^ D1V);
  if (F2.Orientation() == TopAbs_REVERSED)
    DN2.Reverse();

  gp_Vec ProVec = DN2 ^ T1;
  double Prod   = DN1.Dot(ProVec);
  if (Prod < 0.0)
  {
    O1 = TopAbs_FORWARD;
  }
  else
  {
    O1 = TopAbs_REVERSED;
  }
  ProVec = DN1 ^ T1;
  Prod   = DN2.Dot(ProVec);
  if (Prod < 0.0)
  {
    O2 = TopAbs_FORWARD;
  }
  else
  {
    O2 = TopAbs_REVERSED;
  }
  if (F1.Orientation() == TopAbs_REVERSED)
    O1 = TopAbs::Reverse(O1);
  if (F2.Orientation() == TopAbs_REVERSED)
    O2 = TopAbs::Reverse(O2);
}

//=================================================================================================

bool BRepOffset_Tool::FindCommonShapes(const TopoDS_Face&              theF1,
                                       const TopoDS_Face&              theF2,
                                       NCollection_List<TopoDS_Shape>& theLE,
                                       NCollection_List<TopoDS_Shape>& theLV)
{
  bool bFoundEdges = FindCommonShapes(theF1, theF2, TopAbs_EDGE, theLE);
  bool bFoundVerts = FindCommonShapes(theF1, theF2, TopAbs_VERTEX, theLV);
  return bFoundEdges || bFoundVerts;
}

//=================================================================================================

bool BRepOffset_Tool::FindCommonShapes(const TopoDS_Shape&             theS1,
                                       const TopoDS_Shape&             theS2,
                                       const TopAbs_ShapeEnum          theType,
                                       NCollection_List<TopoDS_Shape>& theLSC)
{
  theLSC.Clear();
  //
  NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher> aMS;
  TopExp_Explorer                                        aExp(theS1, theType);
  for (; aExp.More(); aExp.Next())
  {
    aMS.Add(aExp.Current());
  }
  //
  if (aMS.IsEmpty())
  {
    return false;
  }
  //
  NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher> aMFence;
  aExp.Init(theS2, theType);
  for (; aExp.More(); aExp.Next())
  {
    const TopoDS_Shape& aS2 = aExp.Current();
    if (aMS.Contains(aS2))
    {
      if (aMFence.Add(aS2))
      {
        theLSC.Append(aS2);
      }
    }
  }
  //
  return !theLSC.IsEmpty();
}

//=================================================================================================

static bool ToSmall(const occ::handle<Geom_Curve>& C)
{
  constexpr double Tol = 10 * Precision::Confusion();
  double           m   = (C->FirstParameter() * 0.668 + C->LastParameter() * 0.332);
  gp_Pnt           P1  = C->Value(C->FirstParameter());
  gp_Pnt           P2  = C->Value(C->LastParameter());
  gp_Pnt           P3  = C->Value(m);
  if (P1.Distance(P2) > Tol)
    return false;
  if (P2.Distance(P3) > Tol)
    return false;
  return true;
}

//=================================================================================================

static bool IsOnSurface(const occ::handle<Geom_Curve>&   C,
                        const occ::handle<Geom_Surface>& S,
                        double                           TolConf,
                        double&                          TolReached)
{
  double f   = C->FirstParameter();
  double l   = C->LastParameter();
  int    n   = 5;
  double du  = (f - l) / (n - 1);
  TolReached = 0.;

  gp_Pnt P;
  double U, V;

  GeomAdaptor_Surface AS(S);

  switch (AS.GetType())
  {
    case GeomAbs_Plane: {
      gp_Ax3 Ax = AS.Plane().Position();
      for (int i = 0; i < n; i++)
      {
        P = C->Value(f + i * du);
        ElSLib::PlaneParameters(Ax, P, U, V);
        TolReached = P.Distance(ElSLib::PlaneValue(U, V, Ax));
        if (TolReached > TolConf)
          return false;
      }
      break;
    }
    case GeomAbs_Cylinder: {
      gp_Ax3 Ax  = AS.Cylinder().Position();
      double Rad = AS.Cylinder().Radius();
      for (int i = 0; i < n; i++)
      {
        P = C->Value(f + i * du);
        ElSLib::CylinderParameters(Ax, Rad, P, U, V);
        TolReached = P.Distance(ElSLib::CylinderValue(U, V, Ax, Rad));
        if (TolReached > TolConf)
          return false;
      }
      break;
    }
    case GeomAbs_Cone: {
      gp_Ax3 Ax  = AS.Cone().Position();
      double Rad = AS.Cone().RefRadius();
      double Alp = AS.Cone().SemiAngle();
      for (int i = 0; i < n; i++)
      {
        P = C->Value(f + i * du);
        ElSLib::ConeParameters(Ax, Rad, Alp, P, U, V);
        TolReached = P.Distance(ElSLib::ConeValue(U, V, Ax, Rad, Alp));
        if (TolReached > TolConf)
          return false;
      }
      break;
    }
    case GeomAbs_Sphere: {
      gp_Ax3 Ax  = AS.Sphere().Position();
      double Rad = AS.Sphere().Radius();
      for (int i = 0; i < n; i++)
      {
        P = C->Value(f + i * du);
        ElSLib::SphereParameters(Ax, Rad, P, U, V);
        TolReached = P.Distance(ElSLib::SphereValue(U, V, Ax, Rad));
        if (TolReached > TolConf)
          return false;
      }
      break;
    }
    case GeomAbs_Torus: {
      gp_Ax3 Ax = AS.Torus().Position();
      double R1 = AS.Torus().MajorRadius();
      double R2 = AS.Torus().MinorRadius();
      for (int i = 0; i < n; i++)
      {
        P = C->Value(f + i * du);
        ElSLib::TorusParameters(Ax, R1, R2, P, U, V);
        TolReached = P.Distance(ElSLib::TorusValue(U, V, Ax, R1, R2));
        if (TolReached > TolConf)
          return false;
      }
      break;
    }

    default: {
      return false;
    }
  }

  return true;
}

//=================================================================================================

void BRepOffset_Tool::PipeInter(const TopoDS_Face&              F1,
                                const TopoDS_Face&              F2,
                                NCollection_List<TopoDS_Shape>& L1,
                                NCollection_List<TopoDS_Shape>& L2,
                                const TopAbs_State              Side)
{

  occ::handle<Geom_Curve> CI;
  TopAbs_Orientation      O1, O2;
  L1.Clear();
  L2.Clear();
  BRep_Builder              B;
  occ::handle<Geom_Surface> S1 = BRep_Tool::Surface(F1);
  occ::handle<Geom_Surface> S2 = BRep_Tool::Surface(F2);

  GeomInt_IntSS Inter(S1, S2, Precision::Confusion(), true, true, true);

  if (Inter.IsDone())
  {
    for (int i = 1; i <= Inter.NbLines(); i++)
    {
      CI = Inter.Line(i);
      if (ToSmall(CI))
        continue;
      TopoDS_Edge E = BRepLib_MakeEdge(CI);
      if (Inter.HasLineOnS1(i))
      {
        occ::handle<Geom2d_Curve> C2 = Inter.LineOnS1(i);
        PutInBounds(F1, E, C2);
        B.UpdateEdge(E, C2, F1, BRep_Tool::Tolerance(E));
      }
      else
      {
        BuildPCurves(E, F1);
      }
      if (Inter.HasLineOnS2(i))
      {
        occ::handle<Geom2d_Curve> C2 = Inter.LineOnS2(i);
        PutInBounds(F2, E, C2);
        B.UpdateEdge(E, C2, F2, BRep_Tool::Tolerance(E));
      }
      else
      {
        BuildPCurves(E, F2);
      }
      OrientSection(E, F1, F2, O1, O2);
      if (Side == TopAbs_OUT)
      {
        O1 = TopAbs::Reverse(O1);
        O2 = TopAbs::Reverse(O2);
      }
      L1.Append(E.Oriented(O1));
      L2.Append(E.Oriented(O2));
    }
  }
}

//=======================================================================
// function : IsAutonomVertex
// purpose  : Checks whether a vertex is "autonom" or not
//=======================================================================

static bool IsAutonomVertex(const TopoDS_Shape& theVertex,
                            const BOPDS_PDS&    thePDS,
                            const TopoDS_Face&  theFace1,
                            const TopoDS_Face&  theFace2)
{
  int nV = thePDS->Index(theVertex);
  int nF[2];
  nF[0] = thePDS->Index(theFace1);
  nF[1] = thePDS->Index(theFace2);

  for (int i = 0; i < 2; i++)
  {
    const BOPDS_FaceInfo&       aFaceInfo = thePDS->FaceInfo(nF[i]);
    const NCollection_Map<int>& IndMap    = aFaceInfo.VerticesOn();
    if (IndMap.Contains(nV))
      return false;
  }

  return true;
}

//=======================================================================
// function : IsAutonomVertex
// purpose  : Checks whether a vertex is "autonom" or not
//=======================================================================

static bool IsAutonomVertex(const TopoDS_Shape& aVertex, const BOPDS_PDS& pDS)
{
  int index;
  int aNbVVs, aNbEEs, aNbEFs, aInt;
  //
  index = pDS->Index(aVertex);
  if (index == -1)
  {
    int i, i1, i2;
    i1 = pDS->NbSourceShapes();
    i2 = pDS->NbShapes();
    for (i = i1; i < i2; ++i)
    {
      const TopoDS_Shape& aSx = pDS->Shape(i);
      if (aSx.IsSame(aVertex))
      {
        index = i;
        break;
      }
    }
  }
  //
  if (!pDS->IsNewShape(index))
  {
    return false;
  }
  // check if vertex with index "index" is not created in VV or EE or EF interference
  // VV
  NCollection_Vector<BOPDS_InterfVV>& aVVs = pDS->InterfVV();
  aNbVVs                                   = aVVs.Length();
  for (aInt = 0; aInt < aNbVVs; aInt++)
  {
    const BOPDS_InterfVV& aVV = aVVs(aInt);
    if (aVV.HasIndexNew())
    {
      if (aVV.IndexNew() == index)
      {
        return false;
      }
    }
  }
  // EE
  NCollection_Vector<BOPDS_InterfEE>& aEEs = pDS->InterfEE();
  aNbEEs                                   = aEEs.Length();
  for (aInt = 0; aInt < aNbEEs; aInt++)
  {
    const BOPDS_InterfEE& aEE = aEEs(aInt);
    IntTools_CommonPrt    aCP = aEE.CommonPart();
    if (aCP.Type() == TopAbs_VERTEX)
    {
      if (aEE.IndexNew() == index)
      {
        return false;
      }
    }
  }
  // EF
  NCollection_Vector<BOPDS_InterfEF>& aEFs = pDS->InterfEF();
  aNbEFs                                   = aEFs.Length();
  for (aInt = 0; aInt < aNbEFs; aInt++)
  {
    const BOPDS_InterfEF& aEF = aEFs(aInt);
    IntTools_CommonPrt    aCP = aEF.CommonPart();
    if (aCP.Type() == TopAbs_VERTEX)
    {
      if (aEF.IndexNew() == index)
      {
        return false;
      }
    }
  }
  return true;
}

//=======================================================================
// function : AreConnex
// purpose  : define if two shapes are connex by a vertex (vertices)
//=======================================================================

static bool AreConnex(const TopoDS_Wire& W1, const TopoDS_Wire& W2)
{
  TopoDS_Vertex V11, V12, V21, V22;
  TopExp::Vertices(W1, V11, V12);
  TopExp::Vertices(W2, V21, V22);

  if (V11.IsSame(V21) || V11.IsSame(V22) || V12.IsSame(V21) || V12.IsSame(V22))
    return true;

  return false;
}

//=======================================================================
// function : AreClosed
// purpose  : define if two edges are connex by two vertices
//=======================================================================

static bool AreClosed(const TopoDS_Edge& E1, const TopoDS_Edge& E2)
{
  TopoDS_Vertex V11, V12, V21, V22;
  TopExp::Vertices(E1, V11, V12);
  TopExp::Vertices(E2, V21, V22);

  if ((V11.IsSame(V21) && V12.IsSame(V22)) || (V11.IsSame(V22) && V12.IsSame(V21)))
    return true;

  return false;
}

//=================================================================================================

static bool BSplineEdges(const TopoDS_Edge& E1,
                         const TopoDS_Edge& E2,
                         const int          par1,
                         const int          par2,
                         double&            angle)
{
  double first1, last1, first2, last2, Param1, Param2;

  occ::handle<Geom_Curve> C1 = BRep_Tool::Curve(E1, first1, last1);
  if (C1->IsInstance(STANDARD_TYPE(Geom_TrimmedCurve)))
    C1 = occ::down_cast<Geom_TrimmedCurve>(C1)->BasisCurve();

  occ::handle<Geom_Curve> C2 = BRep_Tool::Curve(E2, first2, last2);
  if (C2->IsInstance(STANDARD_TYPE(Geom_TrimmedCurve)))
    C2 = occ::down_cast<Geom_TrimmedCurve>(C2)->BasisCurve();

  if (!C1->IsInstance(STANDARD_TYPE(Geom_BSplineCurve))
      || !C2->IsInstance(STANDARD_TYPE(Geom_BSplineCurve)))
    return false;

  Param1 = (par1 == 0) ? first1 : last1;
  Param2 = (par2 == 0) ? first2 : last2;

  gp_Pnt Pnt1, Pnt2;
  gp_Vec Der1, Der2;
  C1->D1(Param1, Pnt1, Der1);
  C2->D1(Param2, Pnt2, Der2);

  if (Der1.Magnitude() <= gp::Resolution() || Der2.Magnitude() <= gp::Resolution())
    angle = M_PI / 2.;
  else
    angle = Der1.Angle(Der2);

  return true;
}

//=================================================================================================

static double AngleWireEdge(const TopoDS_Wire& aWire, const TopoDS_Edge& anEdge)
{
  TopoDS_Vertex V11, V12, V21, V22, CV;
  TopExp::Vertices(aWire, V11, V12);
  TopExp::Vertices(anEdge, V21, V22);
  CV = (V11.IsSame(V21) || V11.IsSame(V22)) ? V11 : V12;
  TopoDS_Edge     FirstEdge;
  TopoDS_Iterator itw(aWire);
  for (; itw.More(); itw.Next())
  {
    FirstEdge = TopoDS::Edge(itw.Value());
    TopoDS_Vertex v1, v2;
    TopExp::Vertices(FirstEdge, v1, v2);
    if (v1.IsSame(CV) || v2.IsSame(CV))
    {
      V11 = v1;
      V12 = v2;
      break;
    }
  }
  double Angle;
  if (V11.IsSame(CV) && V21.IsSame(CV))
  {
    BSplineEdges(FirstEdge, anEdge, 0, 0, Angle);
    Angle = M_PI - Angle;
  }
  else if (V11.IsSame(CV) && V22.IsSame(CV))
    BSplineEdges(FirstEdge, anEdge, 0, 1, Angle);
  else if (V12.IsSame(CV) && V21.IsSame(CV))
    BSplineEdges(FirstEdge, anEdge, 1, 0, Angle);
  else
  {
    BSplineEdges(FirstEdge, anEdge, 1, 1, Angle);
    Angle = M_PI - Angle;
  }
  return Angle;
}

//=================================================================================================

static void ReconstructPCurves(const TopoDS_Edge& anEdge)
{
  double                  f, l;
  occ::handle<Geom_Curve> C3d = BRep_Tool::Curve(anEdge, f, l);

  NCollection_List<occ::handle<BRep_CurveRepresentation>>::Iterator itcr(
    (occ::down_cast<BRep_TEdge>(anEdge.TShape()))->ChangeCurves());
  for (; itcr.More(); itcr.Next())
  {
    occ::handle<BRep_CurveRepresentation> CurveRep = itcr.Value();
    if (CurveRep->IsCurveOnSurface())
    {
      occ::handle<Geom_Surface> theSurf = CurveRep->Surface();
      TopLoc_Location           theLoc  = CurveRep->Location();
      theLoc                            = anEdge.Location() * theLoc;
      theSurf = occ::down_cast<Geom_Surface>(theSurf->Transformed(theLoc.Transformation()));
      occ::handle<Geom2d_Curve> ProjPCurve = GeomProjLib::Curve2d(C3d, f, l, theSurf);
      if (!ProjPCurve.IsNull())
      {
        CurveRep->PCurve(ProjPCurve);
      }
    }
  }
}

//=================================================================================================

static occ::handle<Geom2d_Curve> ConcatPCurves(const TopoDS_Edge& E1,
                                               const TopoDS_Edge& E2,
                                               const TopoDS_Face& F,
                                               const bool         After,
                                               double&            newFirst,
                                               double&            newLast)
{
  double        Tol        = 1.e-7;
  GeomAbs_Shape Continuity = GeomAbs_C1;
  int           MaxDeg     = 14;
  int           MaxSeg     = 16;

  double                    first1, last1, first2, last2;
  occ::handle<Geom2d_Curve> PCurve1, PCurve2, newPCurve;

  PCurve1 = BRep_Tool::CurveOnSurface(E1, F, first1, last1);
  if (PCurve1->IsInstance(STANDARD_TYPE(Geom2d_TrimmedCurve)))
    PCurve1 = occ::down_cast<Geom2d_TrimmedCurve>(PCurve1)->BasisCurve();

  PCurve2 = BRep_Tool::CurveOnSurface(E2, F, first2, last2);
  if (PCurve2->IsInstance(STANDARD_TYPE(Geom2d_TrimmedCurve)))
    PCurve2 = occ::down_cast<Geom2d_TrimmedCurve>(PCurve2)->BasisCurve();

  if (PCurve1 == PCurve2)
  {
    newPCurve = PCurve1;
    newFirst  = std::min(first1, first2);
    newLast   = std::max(last1, last2);
  }
  else if (PCurve1->DynamicType() == PCurve2->DynamicType()
           && (PCurve1->IsInstance(STANDARD_TYPE(Geom2d_Line))
               || PCurve1->IsKind(STANDARD_TYPE(Geom2d_Conic))))
  {
    newPCurve = PCurve1;
    gp_Pnt2d P1, P2;
    P1 = PCurve2->Value(first2);
    P2 = PCurve2->Value(last2);
    if (PCurve1->IsInstance(STANDARD_TYPE(Geom2d_Line)))
    {
      occ::handle<Geom2d_Line> Lin1   = occ::down_cast<Geom2d_Line>(PCurve1);
      gp_Lin2d                 theLin = Lin1->Lin2d();
      first2                          = ElCLib::Parameter(theLin, P1);
      last2                           = ElCLib::Parameter(theLin, P2);
    }
    else if (PCurve1->IsInstance(STANDARD_TYPE(Geom2d_Circle)))
    {
      occ::handle<Geom2d_Circle> Circ1   = occ::down_cast<Geom2d_Circle>(PCurve1);
      gp_Circ2d                  theCirc = Circ1->Circ2d();
      first2                             = ElCLib::Parameter(theCirc, P1);
      last2                              = ElCLib::Parameter(theCirc, P2);
    }
    else if (PCurve1->IsInstance(STANDARD_TYPE(Geom2d_Ellipse)))
    {
      occ::handle<Geom2d_Ellipse> Ell1     = occ::down_cast<Geom2d_Ellipse>(PCurve1);
      gp_Elips2d                  theElips = Ell1->Elips2d();
      first2                               = ElCLib::Parameter(theElips, P1);
      last2                                = ElCLib::Parameter(theElips, P2);
    }
    else if (PCurve1->IsInstance(STANDARD_TYPE(Geom2d_Parabola)))
    {
      occ::handle<Geom2d_Parabola> Parab1   = occ::down_cast<Geom2d_Parabola>(PCurve1);
      gp_Parab2d                   theParab = Parab1->Parab2d();
      first2                                = ElCLib::Parameter(theParab, P1);
      last2                                 = ElCLib::Parameter(theParab, P2);
    }
    else if (PCurve1->IsInstance(STANDARD_TYPE(Geom2d_Hyperbola)))
    {
      occ::handle<Geom2d_Hyperbola> Hypr1   = occ::down_cast<Geom2d_Hyperbola>(PCurve1);
      gp_Hypr2d                     theHypr = Hypr1->Hypr2d();
      first2                                = ElCLib::Parameter(theHypr, P1);
      last2                                 = ElCLib::Parameter(theHypr, P2);
    }
    newFirst = std::min(first1, first2);
    newLast  = std::max(last1, last2);
  }
  else
  {
    occ::handle<Geom2d_TrimmedCurve>      TC1 = new Geom2d_TrimmedCurve(PCurve1, first1, last1);
    occ::handle<Geom2d_TrimmedCurve>      TC2 = new Geom2d_TrimmedCurve(PCurve2, first2, last2);
    Geom2dConvert_CompCurveToBSplineCurve Concat2d(TC1);
    Concat2d.Add(TC2, Precision::Confusion(), After);
    newPCurve = Concat2d.BSplineCurve();
    if (newPCurve->Continuity() < GeomAbs_C1)
    {
      Geom2dConvert_ApproxCurve Approx2d(newPCurve, Tol, Continuity, MaxSeg, MaxDeg);
      if (Approx2d.HasResult())
        newPCurve = Approx2d.Curve();
    }
    newFirst = newPCurve->FirstParameter();
    newLast  = newPCurve->LastParameter();
  }

  return newPCurve;
}

//=================================================================================================

static TopoDS_Edge Glue(const TopoDS_Edge&   E1,
                        const TopoDS_Edge&   E2,
                        const TopoDS_Vertex& Vfirst,
                        const TopoDS_Vertex& Vlast,
                        const bool           After,
                        const TopoDS_Face&   F1,
                        const bool           addPCurve1,
                        const TopoDS_Face&   F2,
                        const bool           addPCurve2,
                        const double         theGlueTol)
{
  TopoDS_Edge newEdge;

  double        Tol        = 1.e-7;
  GeomAbs_Shape Continuity = GeomAbs_C1;
  int           MaxDeg     = 14;
  int           MaxSeg     = 16;

  occ::handle<Geom_Curve>   C1, C2, newCurve;
  occ::handle<Geom2d_Curve> PCurve1, PCurve2, newPCurve;
  double                    first1, last1, first2, last2, fparam = 0., lparam = 0.;
  bool                      IsCanonic = false;

  C1 = BRep_Tool::Curve(E1, first1, last1);
  if (C1->IsInstance(STANDARD_TYPE(Geom_TrimmedCurve)))
    C1 = occ::down_cast<Geom_TrimmedCurve>(C1)->BasisCurve();

  C2 = BRep_Tool::Curve(E2, first2, last2);
  if (C2->IsInstance(STANDARD_TYPE(Geom_TrimmedCurve)))
    C2 = occ::down_cast<Geom_TrimmedCurve>(C2)->BasisCurve();

  if (C1 == C2)
  {
    newCurve = C1;
    fparam   = std::min(first1, first2);
    lparam   = std::max(last1, last2);
  }
  else if (C1->DynamicType() == C2->DynamicType()
           && (C1->IsInstance(STANDARD_TYPE(Geom_Line)) || C1->IsKind(STANDARD_TYPE(Geom_Conic))))
  {
    IsCanonic = true;
    newCurve  = C1;
  }
  else
  {
    occ::handle<Geom_TrimmedCurve>      TC1 = new Geom_TrimmedCurve(C1, first1, last1);
    occ::handle<Geom_TrimmedCurve>      TC2 = new Geom_TrimmedCurve(C2, first2, last2);
    GeomConvert_CompCurveToBSplineCurve Concat(TC1);
    if (!Concat.Add(TC2, theGlueTol, After))
      return newEdge;
    newCurve = Concat.BSplineCurve();
    if (newCurve->Continuity() < GeomAbs_C1)
    {
      GeomConvert_ApproxCurve Approx3d(newCurve, Tol, Continuity, MaxSeg, MaxDeg);
      if (Approx3d.HasResult())
        newCurve = Approx3d.Curve();
    }
    fparam = newCurve->FirstParameter();
    lparam = newCurve->LastParameter();
  }

  BRep_Builder BB;

  if (IsCanonic)
    newEdge = BRepLib_MakeEdge(newCurve, Vfirst, Vlast);
  else
    newEdge = BRepLib_MakeEdge(newCurve, Vfirst, Vlast, fparam, lparam);

  double newFirst, newLast;
  if (addPCurve1)
  {
    newPCurve = ConcatPCurves(E1, E2, F1, After, newFirst, newLast);
    BB.UpdateEdge(newEdge, newPCurve, F1, 0.);
    BB.Range(newEdge, F1, newFirst, newLast);
  }
  if (addPCurve2)
  {
    newPCurve = ConcatPCurves(E1, E2, F2, After, newFirst, newLast);
    BB.UpdateEdge(newEdge, newPCurve, F2, 0.);
    BB.Range(newEdge, F2, newFirst, newLast);
  }

  return newEdge;
}

//=================================================================================================

static void CheckIntersFF(const BOPDS_PDS&                                               pDS,
                          const TopoDS_Edge&                                             RefEdge,
                          NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher>& TrueEdges)
{
  NCollection_Vector<BOPDS_InterfFF>& aFFs = pDS->InterfFF();
  int                                 aNb  = aFFs.Length();
  int                                 i, j, nbe = 0;

  TopoDS_Compound Edges;
  BRep_Builder    BB;
  BB.MakeCompound(Edges);

  for (i = 0; i < aNb; ++i)
  {
    BOPDS_InterfFF&                        aFFi      = aFFs(i);
    const NCollection_Vector<BOPDS_Curve>& aBCurves  = aFFi.Curves();
    int                                    aNbCurves = aBCurves.Length();

    for (j = 0; j < aNbCurves; ++j)
    {
      const BOPDS_Curve&                                    aBC        = aBCurves(j);
      const NCollection_List<occ::handle<BOPDS_PaveBlock>>& aSectEdges = aBC.PaveBlocks();

      NCollection_List<occ::handle<BOPDS_PaveBlock>>::Iterator aPBIt;
      aPBIt.Initialize(aSectEdges);

      for (; aPBIt.More(); aPBIt.Next())
      {
        const occ::handle<BOPDS_PaveBlock>& aPB    = aPBIt.Value();
        int                                 nSect  = aPB->Edge();
        const TopoDS_Edge&                  anEdge = *(TopoDS_Edge*)&pDS->Shape(nSect);
        BB.Add(Edges, anEdge);
        nbe++;
      }
    }
  }

  if (nbe == 0)
    return;

  NCollection_List<TopoDS_Shape> CompList;
  BOPTools_AlgoTools::MakeConnexityBlocks(Edges, TopAbs_VERTEX, TopAbs_EDGE, CompList);

  TopoDS_Shape NearestCompound;
  if (CompList.Extent() == 1)
    NearestCompound = CompList.First();
  else
  {
    BRepAdaptor_Curve BAcurve(RefEdge);
    gp_Pnt        Pref    = BAcurve.Value((BAcurve.FirstParameter() + BAcurve.LastParameter()) / 2);
    TopoDS_Vertex Vref    = BRepLib_MakeVertex(Pref);
    double        MinDist = RealLast();
    NCollection_List<TopoDS_Shape>::Iterator itl(CompList);
    for (; itl.More(); itl.Next())
    {
      const TopoDS_Shape& aCompound = itl.Value();

      BRepExtrema_DistShapeShape Projector(Vref, aCompound);
      if (!Projector.IsDone() || Projector.NbSolution() == 0)
        continue;

      double aDist = Projector.Value();
      if (aDist < MinDist)
      {
        MinDist         = aDist;
        NearestCompound = aCompound;
      }
    }
  }

  TopExp::MapShapes(NearestCompound, TopAbs_EDGE, TrueEdges);
}

//=================================================================================================

static TopoDS_Edge AssembleEdge(const BOPDS_PDS&                          pDS,
                                const TopoDS_Face&                        F1,
                                const TopoDS_Face&                        F2,
                                const bool                                addPCurve1,
                                const bool                                addPCurve2,
                                const NCollection_Sequence<TopoDS_Shape>& EdgesForConcat)
{
  TopoDS_Edge NullEdge;
  TopoDS_Edge CurEdge  = TopoDS::Edge(EdgesForConcat(1));
  double      aGlueTol = Precision::Confusion();

  for (int j = 2; j <= EdgesForConcat.Length(); j++)
  {
    TopoDS_Edge   anEdge = TopoDS::Edge(EdgesForConcat(j));
    bool          After  = false;
    TopoDS_Vertex Vfirst, Vlast;
    bool          AreClosedWire = AreClosed(CurEdge, anEdge);
    if (AreClosedWire)
    {
      TopoDS_Vertex V1, V2;
      TopExp::Vertices(CurEdge, V1, V2);
      bool IsAutonomV1 = IsAutonomVertex(V1, pDS, F1, F2);
      bool IsAutonomV2 = IsAutonomVertex(V2, pDS, F1, F2);
      if (IsAutonomV1)
      {
        After  = false;
        Vfirst = Vlast = V2;
      }
      else if (IsAutonomV2)
      {
        After  = true;
        Vfirst = Vlast = V1;
      }
      else
        return NullEdge;
    }
    else
    {
      TopoDS_Vertex CV, V11, V12, V21, V22;
      TopExp::CommonVertex(CurEdge, anEdge, CV);
      bool IsAutonomCV = false;
      if (!CV.IsNull())
      {
        IsAutonomCV = IsAutonomVertex(CV, pDS, F1, F2);
      }
      if (IsAutonomCV)
      {
        aGlueTol = BRep_Tool::Tolerance(CV);
        TopExp::Vertices(CurEdge, V11, V12);
        TopExp::Vertices(anEdge, V21, V22);
        if (V11.IsSame(CV) && V21.IsSame(CV))
        {
          Vfirst = V22;
          Vlast  = V12;
        }
        else if (V11.IsSame(CV) && V22.IsSame(CV))
        {
          Vfirst = V21;
          Vlast  = V12;
        }
        else if (V12.IsSame(CV) && V21.IsSame(CV))
        {
          Vfirst = V11;
          Vlast  = V22;
        }
        else
        {
          Vfirst = V11;
          Vlast  = V21;
        }
      }
      else
        return NullEdge;
    } // end of else (open wire)

    TopoDS_Edge NewEdge =
      Glue(CurEdge, anEdge, Vfirst, Vlast, After, F1, addPCurve1, F2, addPCurve2, aGlueTol);
    if (NewEdge.IsNull())
      return NullEdge;
    else
      CurEdge = NewEdge;
  } // end of for (int j = 2; j <= EdgesForConcat.Length(); j++)

  return CurEdge;
}

//=================================================================================================

void BRepOffset_Tool::Inter3D(const TopoDS_Face&              F1,
                              const TopoDS_Face&              F2,
                              NCollection_List<TopoDS_Shape>& L1,
                              NCollection_List<TopoDS_Shape>& L2,
                              const TopAbs_State              Side,
                              const TopoDS_Edge&              RefEdge,
                              const TopoDS_Face&              theRefFace1,
                              const TopoDS_Face&              theRefFace2)
{

  // Check if the faces are planar and not trimmed - in this case
  // the IntTools_FaceFace intersection algorithm will be used directly.
  BRepAdaptor_Surface aBAS1(F1, false), aBAS2(F2, false);
  if (aBAS1.GetType() == GeomAbs_Plane && aBAS2.GetType() == GeomAbs_Plane)
  {
    aBAS1.Initialize(F1, true);
    if (IsInf(aBAS1.LastUParameter()) && IsInf(aBAS1.LastVParameter()))
    {
      aBAS2.Initialize(F2, true);
      if (IsInf(aBAS2.LastUParameter()) && IsInf(aBAS2.LastVParameter()))
      {
        // Intersect the planes without pave filler
        PerformPlanes(F1, F2, Side, L1, L2);
        return;
      }
    }
  }

  // create 3D curves on faces
  BRepLib::BuildCurves3d(F1);
  BRepLib::BuildCurves3d(F2);
  UpdateVertexTolerances(F1);
  UpdateVertexTolerances(F2);

  BOPAlgo_PaveFiller             aPF;
  NCollection_List<TopoDS_Shape> aLS;
  aLS.Append(F1);
  aLS.Append(F2);
  aPF.SetArguments(aLS);
  //
  aPF.Perform();

  NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher> TrueEdges;
  if (!RefEdge.IsNull())
    CheckIntersFF(aPF.PDS(), RefEdge, TrueEdges);

  bool addPCurve1 = true;
  bool addPCurve2 = true;

  const BOPDS_PDS&                    pDS  = aPF.PDS();
  NCollection_Vector<BOPDS_InterfFF>& aFFs = pDS->InterfFF();
  int                                 aNb  = aFFs.Length();
  int                                 i = 0, j = 0, k;
  // Store Result
  L1.Clear();
  L2.Clear();
  TopAbs_Orientation O1, O2;
  BRep_Builder       BB;
  //
  const occ::handle<IntTools_Context>& aContext = aPF.Context();
  //
  for (i = 0; i < aNb; i++)
  {
    BOPDS_InterfFF&                        aFFi     = aFFs(i);
    const NCollection_Vector<BOPDS_Curve>& aBCurves = aFFi.Curves();

    int aNbCurves = aBCurves.Length();

    for (j = 0; j < aNbCurves; j++)
    {
      const BOPDS_Curve&                                    aBC        = aBCurves(j);
      const NCollection_List<occ::handle<BOPDS_PaveBlock>>& aSectEdges = aBC.PaveBlocks();

      NCollection_List<occ::handle<BOPDS_PaveBlock>>::Iterator aPBIt;
      aPBIt.Initialize(aSectEdges);

      for (; aPBIt.More(); aPBIt.Next())
      {
        const occ::handle<BOPDS_PaveBlock>& aPB    = aPBIt.Value();
        int                                 nSect  = aPB->Edge();
        const TopoDS_Edge&                  anEdge = *(TopoDS_Edge*)&pDS->Shape(nSect);
        if (!TrueEdges.IsEmpty() && !TrueEdges.Contains(anEdge))
          continue;

        double                         f, l;
        const occ::handle<Geom_Curve>& aC3DE = BRep_Tool::Curve(anEdge, f, l);
        occ::handle<Geom_TrimmedCurve> aC3DETrim;

        if (!aC3DE.IsNull())
          aC3DETrim = new Geom_TrimmedCurve(aC3DE, f, l);

        double aTolEdge = BRep_Tool::Tolerance(anEdge);

        if (!BOPTools_AlgoTools2D::HasCurveOnSurface(anEdge, F1))
        {
          occ::handle<Geom2d_Curve> aC2d = aBC.Curve().FirstCurve2d();
          if (!aC3DETrim.IsNull())
          {
            occ::handle<Geom2d_Curve> aC2dNew;

            if (aC3DE->IsPeriodic())
            {
              BOPTools_AlgoTools2D::AdjustPCurveOnFace(F1, f, l, aC2d, aC2dNew, aContext);
            }
            else
            {
              BOPTools_AlgoTools2D::AdjustPCurveOnFace(F1, aC3DETrim, aC2d, aC2dNew, aContext);
            }
            aC2d = aC2dNew;
          }
          BB.UpdateEdge(anEdge, aC2d, F1, aTolEdge);
        }

        if (!BOPTools_AlgoTools2D::HasCurveOnSurface(anEdge, F2))
        {
          occ::handle<Geom2d_Curve> aC2d = aBC.Curve().SecondCurve2d();
          if (!aC3DETrim.IsNull())
          {
            occ::handle<Geom2d_Curve> aC2dNew;

            if (aC3DE->IsPeriodic())
            {
              BOPTools_AlgoTools2D::AdjustPCurveOnFace(F2, f, l, aC2d, aC2dNew, aContext);
            }
            else
            {
              BOPTools_AlgoTools2D::AdjustPCurveOnFace(F2, aC3DETrim, aC2d, aC2dNew, aContext);
            }
            aC2d = aC2dNew;
          }
          BB.UpdateEdge(anEdge, aC2d, F2, aTolEdge);
        }

        OrientSection(anEdge, F1, F2, O1, O2);
        if (Side == TopAbs_OUT)
        {
          O1 = TopAbs::Reverse(O1);
          O2 = TopAbs::Reverse(O2);
        }

        L1.Append(anEdge.Oriented(O1));
        L2.Append(anEdge.Oriented(O2));

      }
    }
  }

  constexpr double aSameParTol = Precision::Confusion();
  bool             isEl1 = false, isEl2 = false;

  occ::handle<Geom_Surface> aSurf = BRep_Tool::Surface(F1);
  if (aSurf->IsInstance(STANDARD_TYPE(Geom_RectangularTrimmedSurface)))
    aSurf = occ::down_cast<Geom_RectangularTrimmedSurface>(aSurf)->BasisSurface();
  if (aSurf->IsInstance(STANDARD_TYPE(Geom_Plane)))
    addPCurve1 = false;
  else if (aSurf->IsKind(STANDARD_TYPE(Geom_ElementarySurface)))
    isEl1 = true;

  aSurf = BRep_Tool::Surface(F2);
  if (aSurf->IsInstance(STANDARD_TYPE(Geom_RectangularTrimmedSurface)))
    aSurf = occ::down_cast<Geom_RectangularTrimmedSurface>(aSurf)->BasisSurface();
  if (aSurf->IsInstance(STANDARD_TYPE(Geom_Plane)))
    addPCurve2 = false;
  else if (aSurf->IsKind(STANDARD_TYPE(Geom_ElementarySurface)))
    isEl2 = true;

  if (L1.Extent() > 1 && (!isEl1 || !isEl2) && !theRefFace1.IsNull())
  {
    // remove excess edges that are out of range
    TopoDS_Vertex aV1, aV2;
    TopExp::Vertices(RefEdge, aV1, aV2);
    if (!aV1.IsSame(aV2)) // only if RefEdge is open
    {
      occ::handle<Geom_Surface> aRefSurf1 = BRep_Tool::Surface(theRefFace1);
      occ::handle<Geom_Surface> aRefSurf2 = BRep_Tool::Surface(theRefFace2);
      if (aRefSurf1->IsUClosed() || aRefSurf1->IsVClosed() || aRefSurf2->IsUClosed()
          || aRefSurf2->IsVClosed())
      {
        TopoDS_Edge       MinAngleEdge;
        double            MinAngle = Precision::Infinite();
        BRepAdaptor_Curve aRefBAcurve(RefEdge);
        gp_Pnt            aRefPnt =
          aRefBAcurve.Value((aRefBAcurve.FirstParameter() + aRefBAcurve.LastParameter()) / 2);

        NCollection_List<TopoDS_Shape>::Iterator itl(L1);
        for (; itl.More(); itl.Next())
        {
          const TopoDS_Edge& anEdge = TopoDS::Edge(itl.Value());

          BRepAdaptor_Curve aBAcurve(anEdge);
          gp_Pnt            aMidPntOnEdge =
            aBAcurve.Value((aBAcurve.FirstParameter() + aBAcurve.LastParameter()) / 2);
          gp_Vec RefToMid(aRefPnt, aMidPntOnEdge);

          Extrema_ExtPC aProjector(aRefPnt, aBAcurve);
          if (aProjector.IsDone())
          {
            int    imin      = 0;
            double MinSqDist = Precision::Infinite();
            for (int ind = 1; ind <= aProjector.NbExt(); ind++)
            {
              double aSqDist = aProjector.SquareDistance(ind);
              if (aSqDist < MinSqDist)
              {
                MinSqDist = aSqDist;
                imin      = ind;
              }
            }
            if (imin != 0)
            {
              gp_Pnt aProjectionOnEdge = aProjector.Point(imin).Value();
              gp_Vec RefToProj(aRefPnt, aProjectionOnEdge);
              double anAngle = RefToProj.Angle(RefToMid);
              if (anAngle < MinAngle)
              {
                MinAngle     = anAngle;
                MinAngleEdge = anEdge;
              }
            }
          }
        }

        if (!MinAngleEdge.IsNull())
        {
          NCollection_List<TopoDS_Shape>::Iterator itlist1(L1);
          NCollection_List<TopoDS_Shape>::Iterator itlist2(L2);

          while (itlist1.More())
          {
            const TopoDS_Shape& anEdge = itlist1.Value();
            if (anEdge.IsSame(MinAngleEdge))
            {
              itlist1.Next();
              itlist2.Next();
            }
            else
            {
              L1.Remove(itlist1);
              L2.Remove(itlist2);
            }
          }
        }
      } // if closed
    } // if (!aV1.IsSame(aV2))
  } // if (L1.Extent() > 1 && (!isEl1 || !isEl2) && !theRefFace1.IsNull())

  if (L1.Extent() > 1 && (!isEl1 || !isEl2))
  {
    NCollection_Sequence<TopoDS_Shape> eseq;
    NCollection_Sequence<TopoDS_Shape> EdgesForConcat;

    if (!TrueEdges.IsEmpty())
    {
      for (i = TrueEdges.Extent(); i >= 1; i--)
        EdgesForConcat.Append(TrueEdges(i));
      TopoDS_Edge AssembledEdge = AssembleEdge(pDS, F1, F2, addPCurve1, addPCurve2, EdgesForConcat);
      if (AssembledEdge.IsNull())
        for (i = TrueEdges.Extent(); i >= 1; i--)
          eseq.Append(TrueEdges(i));
      else
        eseq.Append(AssembledEdge);
    }
    else
    {
      NCollection_Sequence<TopoDS_Shape>       wseq;
      NCollection_Sequence<TopoDS_Shape>       edges;
      NCollection_List<TopoDS_Shape>::Iterator itl(L1);
      for (; itl.More(); itl.Next())
        edges.Append(itl.Value());
      while (!edges.IsEmpty())
      {
        TopoDS_Edge anEdge = TopoDS::Edge(edges.First());
        TopoDS_Wire aWire, resWire;
        BB.MakeWire(aWire);
        BB.Add(aWire, anEdge);
        NCollection_Sequence<int> Candidates;
        for (k = 1; k <= wseq.Length(); k++)
        {
          resWire = TopoDS::Wire(wseq(k));
          if (AreConnex(resWire, aWire))
          {
            Candidates.Append(1);
            break;
          }
        }
        if (Candidates.IsEmpty())
        {
          wseq.Append(aWire);
          edges.Remove(1);
        }
        else
        {
          for (j = 2; j <= edges.Length(); j++)
          {
            anEdge = TopoDS::Edge(edges(j));
            aWire.Nullify();
            BB.MakeWire(aWire);
            BB.Add(aWire, anEdge);
            if (AreConnex(resWire, aWire))
              Candidates.Append(j);
          }
          int minind = 1;
          if (Candidates.Length() > 1)
          {
            double MinAngle = RealLast();
            for (j = 1; j <= Candidates.Length(); j++)
            {
              anEdge         = TopoDS::Edge(edges(Candidates(j)));
              double anAngle = AngleWireEdge(resWire, anEdge);
              if (anAngle < MinAngle)
              {
                MinAngle = anAngle;
                minind   = j;
              }
            }
          }
          BB.Add(resWire, TopoDS::Edge(edges(Candidates(minind))));
          wseq(k) = resWire;
          edges.Remove(Candidates(minind));
        }
      } // end of while (!edges.IsEmpty())

      for (i = 1; i <= wseq.Length(); i++)
      {
        TopoDS_Wire                        aWire = TopoDS::Wire(wseq(i));
        NCollection_Sequence<TopoDS_Shape> aLocalEdgesForConcat;
        if (aWire.Closed())
        {
          TopoDS_Vertex                  StartVertex;
          TopoDS_Edge                    StartEdge;
          bool                           StartFound = false;
          NCollection_List<TopoDS_Shape> Elist;

          TopoDS_Iterator itw(aWire);
          for (; itw.More(); itw.Next())
          {
            TopoDS_Edge anEdge = TopoDS::Edge(itw.Value());
            if (StartFound)
              Elist.Append(anEdge);
            else
            {
              TopoDS_Vertex V1, V2;
              TopExp::Vertices(anEdge, V1, V2);
              if (!IsAutonomVertex(V1, pDS))
              {
                StartVertex = V2;
                StartEdge   = anEdge;
                StartFound  = true;
              }
              else if (!IsAutonomVertex(V2, pDS))
              {
                StartVertex = V1;
                StartEdge   = anEdge;
                StartFound  = true;
              }
              else
                Elist.Append(anEdge);
            }
          } // end of for (; itw.More(); itw.Next())
          if (!StartFound)
          {
            itl.Initialize(Elist);
            StartEdge = TopoDS::Edge(itl.Value());
            Elist.Remove(itl);
            TopoDS_Vertex V1, V2;
            TopExp::Vertices(StartEdge, V1, V2);
            StartVertex = V1;
          }
          aLocalEdgesForConcat.Append(StartEdge);
          while (!Elist.IsEmpty())
          {
            for (itl.Initialize(Elist); itl.More(); itl.Next())
            {
              TopoDS_Edge   anEdge = TopoDS::Edge(itl.Value());
              TopoDS_Vertex V1, V2;
              TopExp::Vertices(anEdge, V1, V2);
              if (V1.IsSame(StartVertex))
              {
                StartVertex = V2;
                aLocalEdgesForConcat.Append(anEdge);
                Elist.Remove(itl);
                break;
              }
              else if (V2.IsSame(StartVertex))
              {
                StartVertex = V1;
                aLocalEdgesForConcat.Append(anEdge);
                Elist.Remove(itl);
                break;
              }
            }
          } // end of while (!Elist.IsEmpty())
        } // end of if (aWire.Closed())
        else
        {
          BRepTools_WireExplorer Wexp(aWire);
          for (; Wexp.More(); Wexp.Next())
            aLocalEdgesForConcat.Append(Wexp.Current());
        }

        TopoDS_Edge AssembledEdge =
          AssembleEdge(pDS, F1, F2, addPCurve1, addPCurve2, aLocalEdgesForConcat);
        if (AssembledEdge.IsNull())
          for (j = aLocalEdgesForConcat.Length(); j >= 1; j--)
            eseq.Append(aLocalEdgesForConcat(j));
        else
          eseq.Append(AssembledEdge);
      } // for (i = 1; i <= wseq.Length(); i++)
    } // end of else (when TrueEdges is empty)

    if (eseq.Length() < L1.Extent())
    {
      L1.Clear();
      L2.Clear();
      for (i = 1; i <= eseq.Length(); i++)
      {
        TopoDS_Shape aShape = eseq(i);
        TopoDS_Edge  anEdge = TopoDS::Edge(eseq(i));
        BRepLib::SameParameter(anEdge, aSameParTol, true);
        double EdgeTol = BRep_Tool::Tolerance(anEdge);
#ifdef OCCT_DEBUG
        std::cout << "Tolerance of glued E =      " << EdgeTol << std::endl;
#endif
        if (EdgeTol > 1.e-2)
          continue;

        if (EdgeTol >= 1.e-4)
        {
          ReconstructPCurves(anEdge);
          BRepLib::SameParameter(anEdge, aSameParTol, true);
#ifdef OCCT_DEBUG
          std::cout << "After projection tol of E = " << BRep_Tool::Tolerance(anEdge) << std::endl;
#endif
        }

        OrientSection(anEdge, F1, F2, O1, O2);
        if (Side == TopAbs_OUT)
        {
          O1 = TopAbs::Reverse(O1);
          O2 = TopAbs::Reverse(O2);
        }

        L1.Append(anEdge.Oriented(O1));
        L2.Append(anEdge.Oriented(O2));
      }
    }
  } // end of if (L1.Extent() > 1)

  else
  {
    NCollection_List<TopoDS_Shape>::Iterator itl(L1);
    for (; itl.More(); itl.Next())
    {
      const TopoDS_Edge& anEdge = TopoDS::Edge(itl.Value());
      BRepLib::SameParameter(anEdge, aSameParTol, true);
    }
  }
}

//=================================================================================================

bool BRepOffset_Tool::TryProject(const TopoDS_Face&                    F1,
                                 const TopoDS_Face&                    F2,
                                 const NCollection_List<TopoDS_Shape>& Edges,
                                 NCollection_List<TopoDS_Shape>&       LInt1,
                                 NCollection_List<TopoDS_Shape>&       LInt2,
                                 const TopAbs_State                    Side,
                                 const double                          TolConf)
{

  // try to find if the edges <Edges> are laying on the face F1.
  LInt1.Clear();
  LInt2.Clear();
  NCollection_List<TopoDS_Shape>::Iterator it(Edges);
  bool                                     isOk = true;
  bool                                     Ok   = true;
  TopAbs_Orientation                       O1, O2;
  occ::handle<Geom_Surface>                Bouchon = BRep_Tool::Surface(F1);
  BRep_Builder                             B;

  for (; it.More(); it.Next())
  {
    TopLoc_Location         L;
    double                  f, l;
    TopoDS_Edge             CurE = TopoDS::Edge(it.Value());
    occ::handle<Geom_Curve> C    = BRep_Tool::Curve(CurE, L, f, l);
    if (C.IsNull())
    {
      BRepLib::BuildCurve3d(CurE, BRep_Tool::Tolerance(CurE));
      C = BRep_Tool::Curve(CurE, L, f, l);
      if (C.IsNull()) // not 3d curve, can be degenerated, need to skip
      {
        continue;
      }
    }
    C = new Geom_TrimmedCurve(C, f, l);
    if (!L.IsIdentity())
      C->Transform(L);
    double TolReached;
    isOk = IsOnSurface(C, Bouchon, TolConf, TolReached);

    if (isOk)
    {
      B.UpdateEdge(CurE, TolReached);
      BuildPCurves(CurE, F1);
      OrientSection(CurE, F1, F2, O1, O2);
      if (Side == TopAbs_OUT)
      {
        O1 = TopAbs::Reverse(O1);
        O2 = TopAbs::Reverse(O2);
      }
      LInt1.Append(CurE.Oriented(O1));
      LInt2.Append(CurE.Oriented(O2));
    }
    else
      Ok = false;
  }
  return Ok;
}

//=================================================================================================

void BRepOffset_Tool::InterOrExtent(const TopoDS_Face&              F1,
                                    const TopoDS_Face&              F2,
                                    NCollection_List<TopoDS_Shape>& L1,
                                    NCollection_List<TopoDS_Shape>& L2,
                                    const TopAbs_State              Side)
{

  occ::handle<Geom_Curve> CI;
  TopAbs_Orientation      O1, O2;
  L1.Clear();
  L2.Clear();
  occ::handle<Geom_Surface> S1 = BRep_Tool::Surface(F1);
  occ::handle<Geom_Surface> S2 = BRep_Tool::Surface(F2);

  if (S1->DynamicType() == STANDARD_TYPE(Geom_RectangularTrimmedSurface))
  {
    occ::handle<Geom_RectangularTrimmedSurface> RTS;
    RTS = occ::down_cast<Geom_RectangularTrimmedSurface>(S1);
    if (RTS->BasisSurface()->DynamicType() == STANDARD_TYPE(Geom_Plane))
    {
      S1 = RTS->BasisSurface();
    }
  }
  if (S2->DynamicType() == STANDARD_TYPE(Geom_RectangularTrimmedSurface))
  {
    occ::handle<Geom_RectangularTrimmedSurface> RTS;
    RTS = occ::down_cast<Geom_RectangularTrimmedSurface>(S2);
    if (RTS->BasisSurface()->DynamicType() == STANDARD_TYPE(Geom_Plane))
    {
      S2 = RTS->BasisSurface();
    }
  }

  GeomInt_IntSS Inter(S1, S2, Precision::Confusion());

  if (Inter.IsDone())
  {
    for (int i = 1; i <= Inter.NbLines(); i++)
    {
      CI = Inter.Line(i);

      if (ToSmall(CI))
        continue;
      TopoDS_Edge E = BRepLib_MakeEdge(CI);
      BuildPCurves(E, F1);
      BuildPCurves(E, F2);
      OrientSection(E, F1, F2, O1, O2);
      if (Side == TopAbs_OUT)
      {
        O1 = TopAbs::Reverse(O1);
        O2 = TopAbs::Reverse(O2);
      }
      L1.Append(E.Oriented(O1));
      L2.Append(E.Oriented(O2));
    }
  }
}

//=================================================================================================

static void ExtentEdge(const TopoDS_Face& F,
                       const TopoDS_Face& EF,
                       const TopoDS_Edge& E,
                       TopoDS_Edge&       NE)
{
  BRepAdaptor_Curve CE(E);
  GeomAbs_CurveType Type       = CE.GetType();
  TopoDS_Shape      aLocalEdge = E.EmptyCopied();
  NE                           = TopoDS::Edge(aLocalEdge);
  //  NE = TopoDS::Edge(E.EmptyCopied());

  if (Type == GeomAbs_Line || Type == GeomAbs_Circle || Type == GeomAbs_Ellipse
      || Type == GeomAbs_Hyperbola || Type == GeomAbs_Parabola)
  {
    return;
  }
  // Extension en tangence jusqu'au bord de la surface.
  double                    PMax = 1.e2;
  TopLoc_Location           L;
  occ::handle<Geom_Surface> S = BRep_Tool::Surface(F, L);
  double                    umin, umax, vmin, vmax;

  S->Bounds(umin, umax, vmin, vmax);
  umin = std::max(umin, -PMax);
  vmin = std::max(vmin, -PMax);
  umax = std::min(umax, PMax);
  vmax = std::min(vmax, PMax);

  double                    f, l;
  occ::handle<Geom2d_Curve> C2d = BRep_Tool::CurveOnSurface(E, F, f, l);

  // calcul point cible. ie point d'intersection du prolongement tangent et des bords.
  gp_Pnt2d P;
  gp_Vec2d Tang;
  C2d->D1(CE.FirstParameter(), P, Tang);
  double tx, ty, tmin;
  tx = ty = Precision::Infinite();
  if (std::abs(Tang.X()) > Precision::Confusion())
    tx = std::min(std::abs((umax - P.X()) / Tang.X()), std::abs((umin - P.X()) / Tang.X()));
  if (std::abs(Tang.Y()) > Precision::Confusion())
    ty = std::min(std::abs((vmax - P.Y()) / Tang.Y()), std::abs((vmin - P.Y()) / Tang.Y()));
  tmin = std::min(tx, ty);
  Tang = tmin * Tang;
  gp_Pnt2d PF2d(P.X() - Tang.X(), P.Y() - Tang.Y());

  C2d->D1(CE.LastParameter(), P, Tang);
  tx = ty = Precision::Infinite();
  if (std::abs(Tang.X()) > Precision::Confusion())
    tx = std::min(std::abs((umax - P.X()) / Tang.X()), std::abs((umin - P.X()) / Tang.X()));
  if (std::abs(Tang.Y()) > Precision::Confusion())
    ty = std::min(std::abs((vmax - P.Y()) / Tang.Y()), std::abs((vmin - P.Y()) / Tang.Y()));
  tmin = std::min(tx, ty);
  Tang = tmin * Tang;
  gp_Pnt2d PL2d(P.X() + Tang.X(), P.Y() + Tang.Y());

  occ::handle<Geom_Curve> CC = GeomAPI::To3d(C2d, gp_Pln(gp::XOY()));
  gp_Pnt                  PF(PF2d.X(), PF2d.Y(), 0.);
  gp_Pnt                  PL(PL2d.X(), PL2d.Y(), 0.);

  occ::handle<Geom_BoundedCurve> ExtC = occ::down_cast<Geom_BoundedCurve>(CC);
  if (ExtC.IsNull())
    return;

  GeomLib::ExtendCurveToPoint(ExtC, PF, 1, false);
  GeomLib::ExtendCurveToPoint(ExtC, PL, 1, true);

  occ::handle<Geom2d_Curve> CNE2d = GeomAPI::To2d(ExtC, gp_Pln(gp::XOY()));

  // Construction de la nouvelle arrete;
  BRep_Builder B;
  B.MakeEdge(NE);
  //  B.UpdateEdge (NE,CNE2d,F,BRep_Tool::Tolerance(E));
  B.UpdateEdge(NE, CNE2d, EF, BRep_Tool::Tolerance(E));
  B.Range(NE, CNE2d->FirstParameter(), CNE2d->LastParameter());
  NE.Orientation(E.Orientation());
}

//=================================================================================================

static bool ProjectVertexOnEdge(TopoDS_Vertex& V, const TopoDS_Edge& E, double TolConf)
{
  BRep_Builder    B;
  double          f, l;
  double          U = 0.;
  TopLoc_Location L;
  bool            found = false;

  gp_Pnt            P = BRep_Tool::Pnt(V);
  BRepAdaptor_Curve C = BRepAdaptor_Curve(E);
  f                   = C.FirstParameter();
  l                   = C.LastParameter();

  if (V.Orientation() == TopAbs_FORWARD)
  {
    if (std::abs(f) < Precision::Infinite())
    {
      gp_Pnt PF = C.Value(f);
      if (PF.IsEqual(P, TolConf))
      {
        U     = f;
        found = true;
      }
    }
  }
  if (V.Orientation() == TopAbs_REVERSED)
  {
    if (!found && std::abs(l) < Precision::Infinite())
    {
      gp_Pnt PL = C.Value(l);
      if (PL.IsEqual(P, TolConf))
      {
        U     = l;
        found = true;
      }
    }
  }
  if (!found)
  {
    Extrema_ExtPC Proj(P, C);
    if (Proj.IsDone() && Proj.NbExt() > 0)
    {
      double Dist2, Dist2Min = Proj.SquareDistance(1);
      U = Proj.Point(1).Parameter();
      for (int i = 2; i <= Proj.NbExt(); i++)
      {
        Dist2 = Proj.SquareDistance(i);
        if (Dist2 < Dist2Min)
        {
          Dist2Min = Dist2;
          U        = Proj.Point(i).Parameter();
        }
      }
      found = true;
    }
  }

#ifdef OCCT_DEBUG
  if (AffichExtent)
  {
    double Dist = P.Distance(C.Value(U));
    if (Dist > TolConf)
    {
      std::cout << " ProjectVertexOnEdge :distance vertex edge :" << Dist << std::endl;
    }
    if (U < f - Precision::Confusion() || U > l + Precision::Confusion())
    {
      std::cout << " ProjectVertexOnEdge : hors borne :" << std::endl;
      std::cout << " f = " << f << " l =" << l << " U =" << U << std::endl;
    }
  }
  if (!found)
  {
    std::cout << "BRepOffset_Tool::ProjectVertexOnEdge Parameter no found" << std::endl;
    if (std::abs(f) < Precision::Infinite() && std::abs(l) < Precision::Infinite())
    {
    }
  }
#endif
  if (found)
  {
    TopoDS_Shape aLocalShape = E.Oriented(TopAbs_FORWARD);
    TopoDS_Edge  EE          = TopoDS::Edge(aLocalShape);
    aLocalShape              = V.Oriented(TopAbs_INTERNAL);
    //    TopoDS_Edge EE = TopoDS::Edge(E.Oriented(TopAbs_FORWARD));
    B.UpdateVertex(TopoDS::Vertex(aLocalShape), U, EE, BRep_Tool::Tolerance(E));
  }
  return found;
}

//=================================================================================================

void BRepOffset_Tool::Inter2d(const TopoDS_Face&              F,
                              const TopoDS_Edge&              E1,
                              const TopoDS_Edge&              E2,
                              NCollection_List<TopoDS_Shape>& LV,
                              const double                    TolConf)
{
  BRep_Builder B;
  double       fl1[2], fl2[2];
  LV.Clear();

  // Si l edge a ete etendu les pcurves ne sont pas forcement
  // a jour.
  BuildPCurves(E1, F);
  BuildPCurves(E2, F);

  // Construction des curves 3d si elles n existent pas
  // utile pour coder correctement les parametres des vertex
  // d intersection sur les edges.
  // TopLoc_Location L;
  // double   f,l;
  // occ::handle<Geom_Curve> C3d1 = BRep_Tool::Curve(E1,L,f,l);
  // if (C3d1.IsNull()) {
  //  BRepLib::BuildCurve3d(E1,BRep_Tool::Tolerance(E1));
  //}
  // occ::handle<Geom_Curve> C3d2 = BRep_Tool::Curve(E2,L,f,l);
  // if (C3d2.IsNull()) {
  //  BRepLib::BuildCurve3d(E2,BRep_Tool::Tolerance(E2));
  //}

  int NbPC1 = 1, NbPC2 = 1;
  if (BRep_Tool::IsClosed(E1, F))
    NbPC1++;
  if (BRep_Tool::IsClosed(E2, F))
    NbPC2++;

  occ::handle<Geom_Surface> S = BRep_Tool::Surface(F);
  occ::handle<Geom2d_Curve> C1, C2;
  bool                      YaSol = false;
  int                       itry  = 0;

  while (!YaSol && itry < 2)
  {
    for (int i = 1; i <= NbPC1; i++)
    {
      TopoDS_Shape aLocalEdgeReversedE1 = E1.Reversed();
      if (i == 1)
        C1 = BRep_Tool::CurveOnSurface(E1, F, fl1[0], fl1[1]);
      else
        C1 = BRep_Tool::CurveOnSurface(TopoDS::Edge(aLocalEdgeReversedE1), F, fl1[0], fl1[1]);
      //      if (i == 1)  C1 = BRep_Tool::CurveOnSurface(E1,F,fl1[0],fl1[1]);
      //     else         C1 = BRep_Tool::CurveOnSurface(TopoDS::Edge(E1.Reversed()),
      //						  F,fl1[0],fl1[1]);
      for (int j = 1; j <= NbPC2; j++)
      {
        TopoDS_Shape aLocalEdge = E2.Reversed();
        if (j == 1)
          C2 = BRep_Tool::CurveOnSurface(E2, F, fl2[0], fl2[1]);
        else
          C2 = BRep_Tool::CurveOnSurface(TopoDS::Edge(aLocalEdge), F, fl2[0], fl2[1]);
//	if (j == 1)  C2 = BRep_Tool::CurveOnSurface(E2,F,fl2[0],fl2[1]);
//	else         C2 = BRep_Tool::CurveOnSurface(TopoDS::Edge(E2.Reversed()),
//						    F,fl2[0],fl2[1]);
#ifdef OCCT_DEBUG
        if (C1.IsNull() || C2.IsNull())
        {
          std::cout << "Inter2d : Pas de pcurve" << std::endl;
          return;
        }
#endif
        double   U1 = 0., U2 = 0.;
        gp_Pnt2d P2d;
        bool     aCurrentFind = false;
        if (itry == 1)
        {
          fl1[0] = C1->FirstParameter();
          fl1[1] = C1->LastParameter();
          fl2[0] = C2->FirstParameter();
          fl2[1] = C2->LastParameter();
        }
        Geom2dAdaptor_Curve AC1(C1, fl1[0], fl1[1]);
        Geom2dAdaptor_Curve AC2(C2, fl2[0], fl2[1]);

        if (itry == 0)
        {
          gp_Pnt2d P1[2], P2[2];
          P1[0] = C1->Value(fl1[0]);
          P1[1] = C1->Value(fl1[1]);
          P2[0] = C2->Value(fl2[0]);
          P2[1] = C2->Value(fl2[1]);

          int i1;
          for (i1 = 0; i1 < 2; i1++)
          {
            for (int i2 = 0; i2 < 2; i2++)
            {
              if (std::abs(fl1[i1]) < Precision::Infinite()
                  && std::abs(fl2[i2]) < Precision::Infinite())
              {
                if (P1[i1].IsEqual(P2[i2], TolConf))
                {
                  YaSol        = true;
                  aCurrentFind = true;
                  U1           = fl1[i1];
                  U2           = fl2[i2];
                  P2d          = C1->Value(U1);
                }
              }
            }
          }
          if (!YaSol)
            for (i1 = 0; i1 < 2; i1++)
            {
              Extrema_ExtPC2d extr(P1[i1], AC2);
              if (extr.IsDone() && extr.NbExt() > 0)
              {
                double Dist2, Dist2Min = extr.SquareDistance(1);
                int    IndexMin = 1;
                for (int ind = 2; ind <= extr.NbExt(); ind++)
                {
                  Dist2 = extr.SquareDistance(ind);
                  if (Dist2 < Dist2Min)
                  {
                    Dist2Min = Dist2;
                    IndexMin = ind;
                  }
                }
                if (Dist2Min <= Precision::SquareConfusion())
                {
                  YaSol        = true;
                  aCurrentFind = true;
                  P2d          = P1[i1];
                  U1           = fl1[i1];
                  U2           = (extr.Point(IndexMin)).Parameter();
                  break;
                }
              }
            }
          if (!YaSol)
            for (int i2 = 0; i2 < 2; i2++)
            {
              Extrema_ExtPC2d extr(P2[i2], AC1);
              if (extr.IsDone() && extr.NbExt() > 0)
              {
                double Dist2, Dist2Min = extr.SquareDistance(1);
                int    IndexMin = 1;
                for (int ind = 2; ind <= extr.NbExt(); ind++)
                {
                  Dist2 = extr.SquareDistance(ind);
                  if (Dist2 < Dist2Min)
                  {
                    Dist2Min = Dist2;
                    IndexMin = ind;
                  }
                }
                if (Dist2Min <= Precision::SquareConfusion())
                {
                  YaSol        = true;
                  aCurrentFind = true;
                  P2d          = P2[i2];
                  U2           = fl2[i2];
                  U1           = (extr.Point(IndexMin)).Parameter();
                  break;
                }
              }
            }
        }

        if (!YaSol)
        {
          Geom2dInt_GInter Inter(AC1, AC2, TolConf, TolConf);

          if (!Inter.IsEmpty() && Inter.NbPoints() > 0)
          {
            YaSol        = true;
            aCurrentFind = true;
            U1           = Inter.Point(1).ParamOnFirst();
            U2           = Inter.Point(1).ParamOnSecond();
            P2d          = Inter.Point(1).Value();
          }
          else if (!Inter.IsEmpty() && Inter.NbSegments() > 0)
          {
            YaSol                              = true;
            aCurrentFind                       = true;
            IntRes2d_IntersectionSegment Seg   = Inter.Segment(1);
            IntRes2d_IntersectionPoint   IntP1 = Seg.FirstPoint();
            IntRes2d_IntersectionPoint   IntP2 = Seg.LastPoint();
            double                       U1on1 = IntP1.ParamOnFirst();
            double                       U1on2 = IntP2.ParamOnFirst();
            double                       U2on1 = IntP1.ParamOnSecond();
            double                       U2on2 = IntP2.ParamOnSecond();
#ifdef OCCT_DEBUG
            std::cout << " BRepOffset_Tool::Inter2d SEGMENT d intersection" << std::endl;
            std::cout << "     ===> Parametres sur Curve1 : ";
            std::cout << U1on1 << " " << U1on2 << std::endl;
            std::cout << "     ===> Parametres sur Curve2 : ";
            std::cout << U2on1 << " " << U2on2 << std::endl;
#endif
            U1            = (U1on1 + U1on2) / 2.;
            U2            = (U2on1 + U2on2) / 2.;
            gp_Pnt2d P2d1 = C1->Value(U1);
            gp_Pnt2d P2d2 = C2->Value(U2);
            P2d.SetX((P2d1.X() + P2d2.X()) / 2.);
            P2d.SetY((P2d1.Y() + P2d2.Y()) / 2.);
          }
        }
        if (aCurrentFind)
        {
          gp_Pnt        P = S->Value(P2d.X(), P2d.Y());
          TopoDS_Vertex V = BRepLib_MakeVertex(P);
          V.Orientation(TopAbs_INTERNAL);
          TopoDS_Shape aLocalEdgeOrientedE1 = E1.Oriented(TopAbs_FORWARD);
          B.UpdateVertex(V, U1, TopoDS::Edge(aLocalEdgeOrientedE1), TolConf);
          aLocalEdgeOrientedE1 = E2.Oriented(TopAbs_FORWARD);
          B.UpdateVertex(V, U2, TopoDS::Edge(aLocalEdgeOrientedE1), TolConf);
          //	  B.UpdateVertex(V,U1,TopoDS::Edge(E1.Oriented(TopAbs_FORWARD)),TolConf);
          //	  B.UpdateVertex(V,U2,TopoDS::Edge(E2.Oriented(TopAbs_FORWARD)),TolConf);
          LV.Append(V);
        }
      }
    }
    itry++;
  }

  if (LV.Extent() > 1)
  {
    //------------------------------------------------
    // garde seulement les vertex les plus proches du
    // debut et de la fin.
    //------------------------------------------------
    NCollection_List<TopoDS_Shape>::Iterator it(LV);
    TopoDS_Vertex                            VF, VL;
    double                                   UMin = Precision::Infinite();
    double                                   UMax = -Precision::Infinite();
    double                                   U;

    for (; it.More(); it.Next())
    {
      TopoDS_Vertex CV         = TopoDS::Vertex(it.Value());
      TopoDS_Shape  aLocalEdge = E1.Oriented(TopAbs_FORWARD);
      U                        = BRep_Tool::Parameter(CV, TopoDS::Edge(aLocalEdge));
      //      U = BRep_Tool::Parameter(CV,TopoDS::Edge(E1.Oriented(TopAbs_FORWARD)));
      if (U < UMin)
      {
        VF   = CV;
        UMin = U;
      }
      if (U > UMax)
      {
        VL   = CV;
        UMax = U;
      }
    }
    LV.Clear();
    LV.Append(VF);
    LV.Append(VL);
  }

#ifdef OCCT_DEBUG
  if (!YaSol)
  {
    std::cout << "Inter2d : Pas de solution" << std::endl;
  }
#endif
}

//=================================================================================================

static void SelectEdge(const TopoDS_Face& /*F*/,
                       const TopoDS_Face& /*EF*/,
                       const TopoDS_Edge&              E,
                       NCollection_List<TopoDS_Shape>& LInt)
{
  //------------------------------------------------------------
  // detrompeur sur les intersections sur les faces periodiques
  //------------------------------------------------------------
  NCollection_List<TopoDS_Shape>::Iterator it(LInt);
  double                                   dU = 1.0e100;
  TopoDS_Edge                              GE;

  double Fst, Lst, tmp;
  BRep_Tool::Range(E, Fst, Lst);
  BRepAdaptor_Curve Ad1(E);

  gp_Pnt PFirst = Ad1.Value(Fst);
  gp_Pnt PLast  = Ad1.Value(Lst);

  //----------------------------------------------------------------------
  // Selection de l edge qui couvre le plus le domaine de l edge initiale.
  //----------------------------------------------------------------------
  for (; it.More(); it.Next())
  {
    const TopoDS_Edge& EI = TopoDS::Edge(it.Value());

    BRep_Tool::Range(EI, Fst, Lst);
    BRepAdaptor_Curve Ad2(EI);
    gp_Pnt            P1 = Ad2.Value(Fst);
    gp_Pnt            P2 = Ad2.Value(Lst);

    tmp = P1.Distance(PFirst) + P2.Distance(PLast);
    if (tmp <= dU)
    {
      dU = tmp;
      GE = EI;
    }
  }
  LInt.Clear();
  LInt.Append(GE);
}

//=================================================================================================

static void MakeFace(const occ::handle<Geom_Surface>& S,
                     const double                     Um,
                     const double                     UM,
                     const double                     Vm,
                     const double                     VM,
                     const bool                       uclosed,
                     const bool                       vclosed,
                     const bool                       isVminDegen,
                     const bool                       isVmaxDegen,
                     TopoDS_Face&                     F)
{
  double UMin = Um;
  double UMax = UM;
  double VMin = Vm;
  double VMax = VM;

  // compute infinite flags
  bool umininf = Precision::IsNegativeInfinite(UMin);
  bool umaxinf = Precision::IsPositiveInfinite(UMax);
  bool vmininf = Precision::IsNegativeInfinite(VMin);
  bool vmaxinf = Precision::IsPositiveInfinite(VMax);

  // degenerated flags (for cones)
  bool                      vmindegen = isVminDegen, vmaxdegen = isVmaxDegen;
  occ::handle<Geom_Surface> theSurf = S;
  if (S->DynamicType() == STANDARD_TYPE(Geom_RectangularTrimmedSurface))
    theSurf = occ::down_cast<Geom_RectangularTrimmedSurface>(S)->BasisSurface();
  if (theSurf->DynamicType() == STANDARD_TYPE(Geom_ConicalSurface))
  {
    occ::handle<Geom_ConicalSurface> ConicalS = occ::down_cast<Geom_ConicalSurface>(theSurf);
    gp_Cone                          theCone  = ConicalS->Cone();
    gp_Pnt                           theApex  = theCone.Apex();
    double                           Uapex, Vapex;
    ElSLib::Parameters(theCone, theApex, Uapex, Vapex);
    if (std::abs(VMin - Vapex) <= Precision::Confusion())
      vmindegen = true;
    if (std::abs(VMax - Vapex) <= Precision::Confusion())
      vmaxdegen = true;
  }

  // compute vertices
  BRep_Builder     B;
  constexpr double tol = Precision::Confusion();

  TopoDS_Vertex V00, V10, V11, V01;

  if (!umininf)
  {
    if (!vmininf)
      B.MakeVertex(V00, S->Value(UMin, VMin), tol);
    if (!vmaxinf)
      B.MakeVertex(V01, S->Value(UMin, VMax), tol);
  }
  if (!umaxinf)
  {
    if (!vmininf)
      B.MakeVertex(V10, S->Value(UMax, VMin), tol);
    if (!vmaxinf)
      B.MakeVertex(V11, S->Value(UMax, VMax), tol);
  }

  if (uclosed)
  {
    V10 = V00;
    V11 = V01;
  }

  if (vclosed)
  {
    V01 = V00;
    V11 = V10;
  }

  if (vmindegen)
    V10 = V00;
  if (vmaxdegen)
    V11 = V01;

  // make the lines
  occ::handle<Geom2d_Line> Lumin, Lumax, Lvmin, Lvmax;
  if (!umininf)
    Lumin = new Geom2d_Line(gp_Pnt2d(UMin, 0), gp_Dir2d(gp_Dir2d::D::Y));
  if (!umaxinf)
    Lumax = new Geom2d_Line(gp_Pnt2d(UMax, 0), gp_Dir2d(gp_Dir2d::D::Y));
  if (!vmininf)
    Lvmin = new Geom2d_Line(gp_Pnt2d(0, VMin), gp_Dir2d(gp_Dir2d::D::X));
  if (!vmaxinf)
    Lvmax = new Geom2d_Line(gp_Pnt2d(0, VMax), gp_Dir2d(gp_Dir2d::D::X));

  occ::handle<Geom_Curve> Cumin, Cumax, Cvmin, Cvmax;
  double                  TolApex = 1.e-5;
  // bool hasiso = ! S->IsKind(STANDARD_TYPE(Geom_OffsetSurface));
  bool hasiso = S->IsKind(STANDARD_TYPE(Geom_ElementarySurface));
  if (hasiso)
  {
    if (!umininf)
      Cumin = S->UIso(UMin);
    if (!umaxinf)
      Cumax = S->UIso(UMax);
    if (!vmininf)
    {
      Cvmin = S->VIso(VMin);
      if (BRepOffset_Tool::Gabarit(Cvmin) <= TolApex)
        vmindegen = true;
    }
    if (!vmaxinf)
    {
      Cvmax = S->VIso(VMax);
      if (BRepOffset_Tool::Gabarit(Cvmax) <= TolApex)
        vmaxdegen = true;
    }
  }

  // make the face
  B.MakeFace(F, S, tol);

  // make the edges
  TopoDS_Edge eumin, eumax, evmin, evmax;

  if (!umininf)
  {
    if (hasiso)
      B.MakeEdge(eumin, Cumin, tol);
    else
      B.MakeEdge(eumin);
    if (uclosed)
      B.UpdateEdge(eumin, Lumax, Lumin, F, tol);
    else
      B.UpdateEdge(eumin, Lumin, F, tol);
    if (!vmininf)
    {
      V00.Orientation(TopAbs_FORWARD);
      B.Add(eumin, V00);
    }
    if (!vmaxinf)
    {
      V01.Orientation(TopAbs_REVERSED);
      B.Add(eumin, V01);
    }
    B.Range(eumin, VMin, VMax);
  }

  if (!umaxinf)
  {
    if (uclosed)
      eumax = eumin;
    else
    {
      if (hasiso)
        B.MakeEdge(eumax, Cumax, tol);
      else
        B.MakeEdge(eumax);
      B.UpdateEdge(eumax, Lumax, F, tol);
      if (!vmininf)
      {
        V10.Orientation(TopAbs_FORWARD);
        B.Add(eumax, V10);
      }
      if (!vmaxinf)
      {
        V11.Orientation(TopAbs_REVERSED);
        B.Add(eumax, V11);
      }
      B.Range(eumax, VMin, VMax);
    }
  }

  if (!vmininf)
  {
    if (hasiso && !vmindegen)
      B.MakeEdge(evmin, Cvmin, tol);
    else
      B.MakeEdge(evmin);
    if (vclosed)
      B.UpdateEdge(evmin, Lvmin, Lvmax, F, tol);
    else
      B.UpdateEdge(evmin, Lvmin, F, tol);
    if (!umininf)
    {
      V00.Orientation(TopAbs_FORWARD);
      B.Add(evmin, V00);
    }
    if (!umaxinf)
    {
      V10.Orientation(TopAbs_REVERSED);
      B.Add(evmin, V10);
    }
    B.Range(evmin, UMin, UMax);
    if (vmindegen)
      B.Degenerated(evmin, true);
  }

  if (!vmaxinf)
  {
    if (vclosed)
      evmax = evmin;
    else
    {
      if (hasiso && !vmaxdegen)
        B.MakeEdge(evmax, Cvmax, tol);
      else
        B.MakeEdge(evmax);
      B.UpdateEdge(evmax, Lvmax, F, tol);
      if (!umininf)
      {
        V01.Orientation(TopAbs_FORWARD);
        B.Add(evmax, V01);
      }
      if (!umaxinf)
      {
        V11.Orientation(TopAbs_REVERSED);
        B.Add(evmax, V11);
      }
      B.Range(evmax, UMin, UMax);
      if (vmaxdegen)
        B.Degenerated(evmax, true);
    }
  }

  // make the wires and add them to the face
  eumin.Orientation(TopAbs_REVERSED);
  evmax.Orientation(TopAbs_REVERSED);

  TopoDS_Wire W;

  if (!umininf && !umaxinf && vmininf && vmaxinf)
  {
    // two wires in u
    B.MakeWire(W);
    B.Add(W, eumin);
    B.Add(F, W);
    B.MakeWire(W);
    B.Add(W, eumax);
    B.Add(F, W);
    F.Closed(uclosed);
  }

  else if (umininf && umaxinf && !vmininf && !vmaxinf)
  {
    // two wires in v
    B.MakeWire(W);
    B.Add(W, evmin);
    B.Add(F, W);
    B.MakeWire(W);
    B.Add(W, evmax);
    B.Add(F, W);
    F.Closed(vclosed);
  }

  else if (!umininf || !umaxinf || !vmininf || !vmaxinf)
  {
    // one wire
    B.MakeWire(W);
    if (!umininf)
      B.Add(W, eumin);
    if (!vmininf)
      B.Add(W, evmin);
    if (!umaxinf)
      B.Add(W, eumax);
    if (!vmaxinf)
      B.Add(W, evmax);
    B.Add(F, W);
    W.Closed(!umininf && !umaxinf && !vmininf && !vmaxinf);
    F.Closed(uclosed && vclosed);
  }
}

//=================================================================================================

static bool EnlargeGeometry(occ::handle<Geom_Surface>& S,
                            double&                    U1,
                            double&                    U2,
                            double&                    V1,
                            double&                    V2,
                            bool&                      IsV1degen,
                            bool&                      IsV2degen,
                            const double               uf1,
                            const double               uf2,
                            const double               vf1,
                            const double               vf2,
                            const double               coeff,
                            const bool                 theGlobalEnlargeU,
                            const bool                 theGlobalEnlargeVfirst,
                            const bool                 theGlobalEnlargeVlast,
                            const double               theLenBeforeUfirst,
                            const double               theLenAfterUlast,
                            const double               theLenBeforeVfirst,
                            const double               theLenAfterVlast)
{
  const double TolApex = 1.e-5;

  bool SurfaceChange = false;
  if (S->DynamicType() == STANDARD_TYPE(Geom_RectangularTrimmedSurface))
  {
    occ::handle<Geom_Surface> BS =
      occ::down_cast<Geom_RectangularTrimmedSurface>(S)->BasisSurface();
    EnlargeGeometry(BS,
                    U1,
                    U2,
                    V1,
                    V2,
                    IsV1degen,
                    IsV2degen,
                    uf1,
                    uf2,
                    vf1,
                    vf2,
                    coeff,
                    theGlobalEnlargeU,
                    theGlobalEnlargeVfirst,
                    theGlobalEnlargeVlast,
                    theLenBeforeUfirst,
                    theLenAfterUlast,
                    theLenBeforeVfirst,
                    theLenAfterVlast);
    if (!theGlobalEnlargeVfirst)
      V1 = vf1;
    if (!theGlobalEnlargeVlast)
      V2 = vf2;
    if (!theGlobalEnlargeVfirst || !theGlobalEnlargeVlast)
      // Handle(Geom_RectangularTrimmedSurface)::DownCast (S)->SetTrim( U1, U2, V1, V2 );
      S = new Geom_RectangularTrimmedSurface(BS, U1, U2, V1, V2);
    else
      S = BS;
    SurfaceChange = true;
  }
  else if (S->DynamicType() == STANDARD_TYPE(Geom_OffsetSurface))
  {
    occ::handle<Geom_Surface> Surf = occ::down_cast<Geom_OffsetSurface>(S)->BasisSurface();
    SurfaceChange                  = EnlargeGeometry(Surf,
                                    U1,
                                    U2,
                                    V1,
                                    V2,
                                    IsV1degen,
                                    IsV2degen,
                                    uf1,
                                    uf2,
                                    vf1,
                                    vf2,
                                    coeff,
                                    theGlobalEnlargeU,
                                    theGlobalEnlargeVfirst,
                                    theGlobalEnlargeVlast,
                                    theLenBeforeUfirst,
                                    theLenAfterUlast,
                                    theLenBeforeVfirst,
                                    theLenAfterVlast);
    occ::down_cast<Geom_OffsetSurface>(S)->SetBasisSurface(Surf);
  }
  else if (S->DynamicType() == STANDARD_TYPE(Geom_SurfaceOfLinearExtrusion)
           || S->DynamicType() == STANDARD_TYPE(Geom_SurfaceOfRevolution))
  {
    double                  du_first = 0., du_last = 0., dv_first = 0., dv_last = 0.;
    occ::handle<Geom_Curve> uiso, viso, uiso1, uiso2, viso1, viso2;
    double                  u1, u2, v1, v2;
    bool                    enlargeU = theGlobalEnlargeU, enlargeV = true;
    bool                    enlargeUfirst = enlargeU, enlargeUlast = enlargeU;
    bool enlargeVfirst = theGlobalEnlargeVfirst, enlargeVlast = theGlobalEnlargeVlast;
    S->Bounds(u1, u2, v1, v2);
    if (Precision::IsInfinite(u1) || Precision::IsInfinite(u2))
    {
      du_first = du_last = uf2 - uf1;
      u1                 = uf1 - du_first;
      u2                 = uf2 + du_last;
      enlargeU           = false;
    }
    else if (S->IsUClosed())
      enlargeU = false;
    else
    {
      viso = S->VIso(vf1);
      GeomAdaptor_Curve gac(viso);
      double            du_default = GCPnts_AbscissaPoint::Length(gac) * coeff;
      du_first                     = (theLenBeforeUfirst == -1) ? du_default : theLenBeforeUfirst;
      du_last                      = (theLenAfterUlast == -1) ? du_default : theLenAfterUlast;
      uiso1                        = S->UIso(uf1);
      uiso2                        = S->UIso(uf2);
      if (BRepOffset_Tool::Gabarit(uiso1) <= TolApex)
        enlargeUfirst = false;
      if (BRepOffset_Tool::Gabarit(uiso2) <= TolApex)
        enlargeUlast = false;
    }
    if (Precision::IsInfinite(v1) || Precision::IsInfinite(v2))
    {
      dv_first = dv_last = vf2 - vf1;
      v1                 = vf1 - dv_first;
      v2                 = vf2 + dv_last;
      enlargeV           = false;
    }
    else if (S->IsVClosed())
      enlargeV = false;
    else
    {
      uiso = S->UIso(uf1);
      GeomAdaptor_Curve gac(uiso);
      double            dv_default = GCPnts_AbscissaPoint::Length(gac) * coeff;
      dv_first                     = (theLenBeforeVfirst == -1) ? dv_default : theLenBeforeVfirst;
      dv_last                      = (theLenAfterVlast == -1) ? dv_default : theLenAfterVlast;
      viso1                        = S->VIso(vf1);
      viso2                        = S->VIso(vf2);
      if (BRepOffset_Tool::Gabarit(viso1) <= TolApex)
      {
        enlargeVfirst = false;
        IsV1degen     = true;
      }
      if (BRepOffset_Tool::Gabarit(viso2) <= TolApex)
      {
        enlargeVlast = false;
        IsV2degen    = true;
      }
    }
    occ::handle<Geom_BoundedSurface> aSurf = new Geom_RectangularTrimmedSurface(S, u1, u2, v1, v2);
    if (enlargeU)
    {
      if (enlargeUfirst && du_first != 0.)
        GeomLib::ExtendSurfByLength(aSurf, du_first, 1, true, false);
      if (enlargeUlast && du_last != 0.)
        GeomLib::ExtendSurfByLength(aSurf, du_last, 1, true, true);
    }
    if (enlargeV)
    {
      if (enlargeVfirst && dv_first != 0.)
        GeomLib::ExtendSurfByLength(aSurf, dv_first, 1, false, false);
      if (enlargeVlast && dv_last != 0.)
        GeomLib::ExtendSurfByLength(aSurf, dv_last, 1, false, true);
    }
    S = aSurf;
    S->Bounds(U1, U2, V1, V2);
    SurfaceChange = true;
  }
  else if (S->DynamicType() == STANDARD_TYPE(Geom_BezierSurface)
           || S->DynamicType() == STANDARD_TYPE(Geom_BSplineSurface))
  {
    bool enlargeU = theGlobalEnlargeU, enlargeV = true;
    bool enlargeUfirst = enlargeU, enlargeUlast = enlargeU;
    bool enlargeVfirst = theGlobalEnlargeVfirst, enlargeVlast = theGlobalEnlargeVlast;
    if (S->IsUClosed())
      enlargeU = false;
    if (S->IsVClosed())
      enlargeV = false;

    double duf = uf2 - uf1, dvf = vf2 - vf1;
    double u1, u2, v1, v2;
    S->Bounds(u1, u2, v1, v2);

    double                  du_first = 0., du_last = 0., dv_first = 0., dv_last = 0.;
    occ::handle<Geom_Curve> uiso1, uiso2, viso1, viso2;
    double                  gabarit_uiso1, gabarit_uiso2, gabarit_viso1, gabarit_viso2;

    uiso1         = S->UIso(u1);
    uiso2         = S->UIso(u2);
    viso1         = S->VIso(v1);
    viso2         = S->VIso(v2);
    gabarit_uiso1 = BRepOffset_Tool::Gabarit(uiso1);
    gabarit_uiso2 = BRepOffset_Tool::Gabarit(uiso2);
    gabarit_viso1 = BRepOffset_Tool::Gabarit(viso1);
    gabarit_viso2 = BRepOffset_Tool::Gabarit(viso2);
    if (gabarit_viso1 <= TolApex || gabarit_viso2 <= TolApex)
      enlargeU = false;
    if (gabarit_uiso1 <= TolApex || gabarit_uiso2 <= TolApex)
      enlargeV = false;

    GeomAdaptor_Curve gac;
    if (enlargeU)
    {
      gac.Load(viso1);
      double du_default = GCPnts_AbscissaPoint::Length(gac) * coeff;
      du_first          = (theLenBeforeUfirst == -1) ? du_default : theLenBeforeUfirst;
      du_last           = (theLenAfterUlast == -1) ? du_default : theLenAfterUlast;
      if (gabarit_uiso1 <= TolApex)
        enlargeUfirst = false;
      if (gabarit_uiso2 <= TolApex)
        enlargeUlast = false;
    }
    if (enlargeV)
    {
      gac.Load(uiso1);
      double dv_default = GCPnts_AbscissaPoint::Length(gac) * coeff;
      dv_first          = (theLenBeforeVfirst == -1) ? dv_default : theLenBeforeVfirst;
      dv_last           = (theLenAfterVlast == -1) ? dv_default : theLenAfterVlast;
      if (gabarit_viso1 <= TolApex)
      {
        enlargeVfirst = false;
        IsV1degen     = true;
      }
      if (gabarit_viso2 <= TolApex)
      {
        enlargeVlast = false;
        IsV2degen    = true;
      }
    }

    occ::handle<Geom_BoundedSurface> aSurf = occ::down_cast<Geom_BoundedSurface>(S);
    if (enlargeU)
    {
      if (enlargeUfirst && uf1 - u1 < duf && du_first != 0.)
        GeomLib::ExtendSurfByLength(aSurf, du_first, 1, true, false);
      if (enlargeUlast && u2 - uf2 < duf && du_last != 0.)
        GeomLib::ExtendSurfByLength(aSurf, du_last, 1, true, true);
    }
    if (enlargeV)
    {
      if (enlargeVfirst && vf1 - v1 < dvf && dv_first != 0.)
        GeomLib::ExtendSurfByLength(aSurf, dv_first, 1, false, false);
      if (enlargeVlast && v2 - vf2 < dvf && dv_last != 0.)
        GeomLib::ExtendSurfByLength(aSurf, dv_last, 1, false, true);
    }
    S = aSurf;

    S->Bounds(U1, U2, V1, V2);
    SurfaceChange = true;
  }
  else
  {
    double UU1, UU2, VV1, VV2;
    S->Bounds(UU1, UU2, VV1, VV2);
    // Pas d extension au dela des bornes de la surface.
    U1 = std::max(UU1, U1);
    V1 = std::max(VV1, V1);
    U2 = std::min(UU2, U2);
    V2 = std::min(VV2, V2);
  }
  return SurfaceChange;
}

//=======================================================================
// function : UpDatePCurve
// purpose  :  Mise a jour des pcurves de F sur la surface de de BF.
//            F and BF has to be FORWARD,
//=======================================================================

static void UpdatePCurves(const TopoDS_Face& F, TopoDS_Face& BF)
{
  double                                                        f, l;
  int                                                           i;
  BRep_Builder                                                  B;
  NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher> Emap;
  occ::handle<Geom2d_Curve>                                     NullPCurve;

  TopExp::MapShapes(F, TopAbs_EDGE, Emap);

  for (i = 1; i <= Emap.Extent(); i++)
  {
    TopoDS_Edge CE = TopoDS::Edge(Emap(i));
    CE.Orientation(TopAbs_FORWARD);
    occ::handle<Geom2d_Curve> C2 = BRep_Tool::CurveOnSurface(CE, F, f, l);
    if (!C2.IsNull())
    {
      if (BRep_Tool::IsClosed(CE, F))
      {
        CE.Reverse();
        occ::handle<Geom2d_Curve> C2R = BRep_Tool::CurveOnSurface(CE, F, f, l);
        B.UpdateEdge(CE, NullPCurve, NullPCurve, F, BRep_Tool::Tolerance(CE));
        B.UpdateEdge(CE, C2, C2R, BF, BRep_Tool::Tolerance(CE));
      }
      else
      {
        B.UpdateEdge(CE, NullPCurve, F, BRep_Tool::Tolerance(CE));
        B.UpdateEdge(CE, C2, BF, BRep_Tool::Tolerance(CE));
      }

      B.Range(CE, f, l);
    }
  }
}

//=================================================================================================

static void CompactUVBounds(const TopoDS_Face& F,
                            double&            UMin,
                            double&            UMax,
                            double&            VMin,
                            double&            VMax)
{
  // Calcul serre pour que les bornes ne couvrent pas plus d une periode
  double    U1, U2;
  double    N = 33;
  Bnd_Box2d B;

  TopExp_Explorer exp;
  for (exp.Init(F, TopAbs_EDGE); exp.More(); exp.Next())
  {
    const TopoDS_Edge&  E = TopoDS::Edge(exp.Current());
    BRepAdaptor_Curve2d C(E, F);
    BRep_Tool::Range(E, U1, U2);
    gp_Pnt2d P;
    double   U  = U1;
    double   DU = (U2 - U1) / (N - 1);
    for (int j = 1; j < N; j++)
    {
      C.D0(U, P);
      U += DU;
      B.Add(P);
    }
    C.D0(U2, P);
    B.Add(P);
  }

  if (!B.IsVoid())
    B.Get(UMin, VMin, UMax, VMax);
  else
    BRep_Tool::Surface(F)->Bounds(UMin, UMax, VMin, VMax);
}

//=================================================================================================

void BRepOffset_Tool::CheckBounds(const TopoDS_Face&        F,
                                  const BRepOffset_Analyse& Analyse,
                                  bool&                     enlargeU,
                                  bool&                     enlargeVfirst,
                                  bool&                     enlargeVlast)
{
  enlargeU      = true;
  enlargeVfirst = true;
  enlargeVlast  = true;

  int    Ubound = 0, Vbound = 0;
  double Ufirst = RealLast(), Ulast = RealFirst();
  double Vfirst = RealLast(), Vlast = RealFirst();

  double UF1, UF2, VF1, VF2;
  CompactUVBounds(F, UF1, UF2, VF1, VF2);

  occ::handle<Geom_Surface> theSurf = BRep_Tool::Surface(F);
  if (theSurf->DynamicType() == STANDARD_TYPE(Geom_RectangularTrimmedSurface))
    theSurf = occ::down_cast<Geom_RectangularTrimmedSurface>(theSurf)->BasisSurface();

  if (theSurf->DynamicType() == STANDARD_TYPE(Geom_SurfaceOfLinearExtrusion)
      || theSurf->DynamicType() == STANDARD_TYPE(Geom_SurfaceOfRevolution)
      || theSurf->DynamicType() == STANDARD_TYPE(Geom_BezierSurface)
      || theSurf->DynamicType() == STANDARD_TYPE(Geom_BSplineSurface))
  {
    TopExp_Explorer Explo(F, TopAbs_EDGE);
    for (; Explo.More(); Explo.Next())
    {
      const TopoDS_Edge&                           anEdge = TopoDS::Edge(Explo.Current());
      const NCollection_List<BRepOffset_Interval>& L      = Analyse.Type(anEdge);
      if (!L.IsEmpty() || BRep_Tool::Degenerated(anEdge))
      {
        ChFiDS_TypeOfConcavity OT = L.First().Type();
        if (OT == ChFiDS_Tangential || BRep_Tool::Degenerated(anEdge))
        {
          double                    fpar, lpar;
          occ::handle<Geom2d_Curve> aCurve = BRep_Tool::CurveOnSurface(anEdge, F, fpar, lpar);
          if (aCurve->DynamicType() == STANDARD_TYPE(Geom2d_TrimmedCurve))
            aCurve = occ::down_cast<Geom2d_TrimmedCurve>(aCurve)->BasisCurve();

          occ::handle<Geom2d_Line> theLine;
          if (aCurve->DynamicType() == STANDARD_TYPE(Geom2d_Line))
            theLine = occ::down_cast<Geom2d_Line>(aCurve);
          else if (aCurve->DynamicType() == STANDARD_TYPE(Geom2d_BezierCurve)
                   || aCurve->DynamicType() == STANDARD_TYPE(Geom2d_BSplineCurve))
          {
            double newFpar, newLpar, deviation;
            theLine = ShapeCustom_Curve2d::ConvertToLine2d(aCurve,
                                                           fpar,
                                                           lpar,
                                                           Precision::Confusion(),
                                                           newFpar,
                                                           newLpar,
                                                           deviation);
          }

          if (!theLine.IsNull())
          {
            gp_Dir2d theDir = theLine->Direction();
            if (theDir.IsParallel(gp::DX2d(), Precision::Angular()))
            {
              Vbound++;
              if (BRep_Tool::Degenerated(anEdge))
              {
                if (std::abs(theLine->Location().Y() - VF1) <= Precision::Confusion())
                  enlargeVfirst = false;
                else // theLine->Location().Y() is near VF2
                  enlargeVlast = false;
              }
              else
              {
                if (theLine->Location().Y() < Vfirst)
                  Vfirst = theLine->Location().Y();
                if (theLine->Location().Y() > Vlast)
                  Vlast = theLine->Location().Y();
              }
            }
            else if (theDir.IsParallel(gp::DY2d(), Precision::Angular()))
            {
              Ubound++;
              if (theLine->Location().X() < Ufirst)
                Ufirst = theLine->Location().X();
              if (theLine->Location().X() > Ulast)
                Ulast = theLine->Location().X();
            }
          }
        }
      }
    }
  }

  if (Ubound >= 2 || Vbound >= 2)
  {
    if (Ubound >= 2 && std::abs(UF1 - Ufirst) <= Precision::Confusion()
        && std::abs(UF2 - Ulast) <= Precision::Confusion())
      enlargeU = false;
    if (Vbound >= 2 && std::abs(VF1 - Vfirst) <= Precision::Confusion()
        && std::abs(VF2 - Vlast) <= Precision::Confusion())
    {
      enlargeVfirst = false;
      enlargeVlast  = false;
    }
  }
}

//=================================================================================================

bool BRepOffset_Tool::EnLargeFace(const TopoDS_Face& F,
                                  TopoDS_Face&       BF,
                                  const bool         CanExtentSurface,
                                  const bool         UpdatePCurve,
                                  const bool         theEnlargeU,
                                  const bool         theEnlargeVfirst,
                                  const bool         theEnlargeVlast,
                                  const int          theExtensionMode,
                                  const double       theLenBeforeUfirst,
                                  const double       theLenAfterUlast,
                                  const double       theLenBeforeVfirst,
                                  const double       theLenAfterVlast)
{
  //---------------------------
  // extension de la geometrie.
  //---------------------------
  TopLoc_Location           L;
  occ::handle<Geom_Surface> S = BRep_Tool::Surface(F, L);
  double                    UU1, VV1, UU2, VV2;
  bool                      uperiodic = false, vperiodic = false;
  bool                      isVV1degen = false, isVV2degen = false;
  double                    US1, VS1, US2, VS2;
  double                    UF1, VF1, UF2, VF2;
  bool                      SurfaceChange = false;

  if (S->IsUPeriodic() || S->IsVPeriodic())
  {
    // Calcul serre pour que les bornes ne couvre pas plus d une periode
    CompactUVBounds(F, UF1, UF2, VF1, VF2);
  }
  else
  {
    BRepTools::UVBounds(F, UF1, UF2, VF1, VF2);
  }

  S->Bounds(US1, US2, VS1, VS2);
  double coeff;
  if (theExtensionMode == 1)
  {
    UU1 = VV1 = -TheInfini;
    UU2 = VV2 = TheInfini;
    coeff     = 0.25;
  }
  else
  {
    double FaceDU = UF2 - UF1;
    double FaceDV = VF2 - VF1;
    UU1           = UF1 - 10 * FaceDU;
    UU2           = UF2 + 10 * FaceDU;
    VV1           = VF1 - 10 * FaceDV;
    VV2           = VF2 + 10 * FaceDV;
    coeff         = 1.;
  }

  if (CanExtentSurface)
  {
    SurfaceChange = EnlargeGeometry(S,
                                    UU1,
                                    UU2,
                                    VV1,
                                    VV2,
                                    isVV1degen,
                                    isVV2degen,
                                    UF1,
                                    UF2,
                                    VF1,
                                    VF2,
                                    coeff,
                                    theEnlargeU,
                                    theEnlargeVfirst,
                                    theEnlargeVlast,
                                    theLenBeforeUfirst,
                                    theLenAfterUlast,
                                    theLenBeforeVfirst,
                                    theLenAfterVlast);
  }
  else
  {
    UU1 = std::max(US1, UU1);
    UU2 = std::min(UU2, US2);
    VV1 = std::max(VS1, VV1);
    VV2 = std::min(VS2, VV2);
  }

  if (S->IsUPeriodic())
  {
    uperiodic     = true;
    double Period = S->UPeriod();
    double Delta  = Period - (UF2 - UF1);
    double alpha  = 0.1;
    UU1           = UF1 - alpha * Delta;
    UU2           = UF2 + alpha * Delta;
    if ((UU2 - UU1) > Period)
    {
      UU2 = UU1 + Period;
    }
  }
  if (S->IsVPeriodic())
  {
    vperiodic     = true;
    double Period = S->VPeriod();
    double Delta  = Period - (VF2 - VF1);
    double alpha  = 0.1;
    VV1           = VF1 - alpha * Delta;
    VV2           = VF2 + alpha * Delta;
    if ((VV2 - VV1) > Period)
    {
      VV2 = VV1 + Period;
    }
  }

  // Special treatment for conical surfaces
  occ::handle<Geom_Surface> theSurf = S;
  if (S->DynamicType() == STANDARD_TYPE(Geom_RectangularTrimmedSurface))
    theSurf = occ::down_cast<Geom_RectangularTrimmedSurface>(S)->BasisSurface();
  if (theSurf->DynamicType() == STANDARD_TYPE(Geom_ConicalSurface))
  {
    occ::handle<Geom_ConicalSurface> ConicalS = occ::down_cast<Geom_ConicalSurface>(theSurf);
    gp_Cone                          theCone  = ConicalS->Cone();
    gp_Pnt                           theApex  = theCone.Apex();
    double                           Uapex, Vapex;
    ElSLib::Parameters(theCone, theApex, Uapex, Vapex);
    if (VV1 < Vapex && Vapex < VV2)
    {
      // consider that VF1 and VF2 are on the same side from apex
      double TolApex = 1.e-5;
      if (Vapex - VF1 >= TolApex || Vapex - VF2 >= TolApex) // if (VF1 < Vapex || VF2 < Vapex)
        VV2 = Vapex;
      else
        VV1 = Vapex;
    }
  }

  if (!theEnlargeU)
  {
    UU1 = UF1;
    UU2 = UF2;
  }
  if (!theEnlargeVfirst)
    VV1 = VF1;
  if (!theEnlargeVlast)
    VV2 = VF2;

  // Detect closedness in U and V directions
  bool uclosed = false, vclosed = false;
  BRepTools::DetectClosedness(F, uclosed, vclosed);
  if (uclosed && !uperiodic && (theLenBeforeUfirst != 0. || theLenAfterUlast != 0.))
    uclosed = false;
  if (vclosed && !vperiodic && (theLenBeforeVfirst != 0. && theLenAfterVlast != 0.))
    vclosed = false;

  MakeFace(S, UU1, UU2, VV1, VV2, uclosed, vclosed, isVV1degen, isVV2degen, BF);
  BF.Location(L);
  /*
    if (S->DynamicType() == STANDARD_TYPE(Geom_RectangularTrimmedSurface)) {
      BRep_Builder B;
      //----------------------------------------------------------------
      // utile pour les bouchons on ne doit pas changer leur geometrie.
      // (Ce que fait BRepLib_MakeFace si S est restreinte).
      // On remet S et on update les pcurves.
      //----------------------------------------------------------------
      TopExp_Explorer exp;
      exp.Init(BF,TopAbs_EDGE);
      double f=0.,l=0.;
      for (; exp.More(); exp.Next()) {
        TopoDS_Edge   CE  = TopoDS::Edge(exp.Current());
        occ::handle<Geom2d_Curve> C2 = BRep_Tool::CurveOnSurface(CE,BF,f,l);
        B.UpdateEdge (CE,C2,S,L,BRep_Tool::Tolerance(CE));
      }
      B.UpdateFace(BF,S,L,BRep_Tool::Tolerance(F));
    }
  */
  if (SurfaceChange && UpdatePCurve)
  {
    TopoDS_Shape aLocalFace = F.Oriented(TopAbs_FORWARD);
    UpdatePCurves(TopoDS::Face(aLocalFace), BF);
    // UpdatePCurves(TopoDS::Face(F.Oriented(TopAbs_FORWARD)),BF);
    BRep_Builder BB;
    BB.UpdateFace(F, S, L, BRep_Tool::Tolerance(F));
  }

  BF.Orientation(F.Orientation());
  return SurfaceChange;
}

//=================================================================================================

static bool TryParameter(const TopoDS_Edge& OE,
                         TopoDS_Vertex&     V,
                         const TopoDS_Edge& NE,
                         double             TolConf)
{
  BRepAdaptor_Curve OC(OE);
  BRepAdaptor_Curve NC(NE);
  double            Of = OC.FirstParameter();
  double            Ol = OC.LastParameter();
  double            Nf = NC.FirstParameter();
  double            Nl = NC.LastParameter();
  double            U  = 0.;
  gp_Pnt            P  = BRep_Tool::Pnt(V);
  bool              OK = false;

  if (P.Distance(OC.Value(Of)) < TolConf)
  {
    if (Of > Nf && Of < Nl && P.Distance(NC.Value(Of)) < TolConf)
    {
      OK = true;
      U  = Of;
    }
  }
  if (P.Distance(OC.Value(Ol)) < TolConf)
  {
    if (Ol > Nf && Ol < Nl && P.Distance(NC.Value(Ol)) < TolConf)
    {
      OK = true;
      U  = Ol;
    }
  }
  if (OK)
  {
    BRep_Builder B;
    TopoDS_Shape aLocalShape = NE.Oriented(TopAbs_FORWARD);
    TopoDS_Edge  EE          = TopoDS::Edge(aLocalShape);
    //    TopoDS_Edge EE = TopoDS::Edge(NE.Oriented(TopAbs_FORWARD));
    aLocalShape = V.Oriented(TopAbs_INTERNAL);
    B.UpdateVertex(TopoDS::Vertex(aLocalShape), U, NE, BRep_Tool::Tolerance(NE));
    //    B.UpdateVertex(TopoDS::Vertex(V.Oriented(TopAbs_INTERNAL)),
    //		   U,NE,BRep_Tool::Tolerance(NE));
  }
  return OK;
}

//=================================================================================================

void BRepOffset_Tool::MapVertexEdges(
  const TopoDS_Shape&                                                                         S,
  NCollection_DataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher>& MEV)
{
  TopExp_Explorer exp;
  exp.Init(S.Oriented(TopAbs_FORWARD), TopAbs_EDGE);
  NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher> DejaVu;
  for (; exp.More(); exp.Next())
  {
    const TopoDS_Edge& E = TopoDS::Edge(exp.Current());
    if (DejaVu.Add(E))
    {
      TopoDS_Vertex V1, V2;
      TopExp::Vertices(E, V1, V2);
      if (!MEV.IsBound(V1))
      {
        NCollection_List<TopoDS_Shape> empty;
        MEV.Bind(V1, empty);
      }
      MEV(V1).Append(E);
      if (!V1.IsSame(V2))
      {
        if (!MEV.IsBound(V2))
        {
          NCollection_List<TopoDS_Shape> empty;
          MEV.Bind(V2, empty);
        }
        MEV(V2).Append(E);
      }
    }
  }
}

//=================================================================================================

void BRepOffset_Tool::BuildNeighbour(
  const TopoDS_Wire&                                                        W,
  const TopoDS_Face&                                                        F,
  NCollection_DataMap<TopoDS_Shape, TopoDS_Shape, TopTools_ShapeMapHasher>& NOnV1,
  NCollection_DataMap<TopoDS_Shape, TopoDS_Shape, TopTools_ShapeMapHasher>& NOnV2)
{
  TopoDS_Vertex          V1, V2, VP1, VP2, FV1, FV2;
  TopoDS_Edge            CurE, FirstE, PrecE;
  BRepTools_WireExplorer wexp;

  TopoDS_Shape aLocalFace = F.Oriented(TopAbs_FORWARD);
  TopoDS_Shape aLocalWire = W.Oriented(TopAbs_FORWARD);
  wexp.Init(TopoDS::Wire(aLocalWire), TopoDS::Face(aLocalFace));
  //  wexp.Init(TopoDS::Wire(W.Oriented(TopAbs_FORWARD)),
  //	    TopoDS::Face(F.Oriented(TopAbs_FORWARD)));
  CurE = FirstE = PrecE = wexp.Current();
  TopExp::Vertices(CurE, V1, V2);
  FV1 = VP1 = V1;
  FV2 = VP2 = V2;
  wexp.Next();
  while (wexp.More())
  {
    CurE = wexp.Current();
    TopExp::Vertices(CurE, V1, V2);
    if (V1.IsSame(VP1))
    {
      NOnV1.Bind(PrecE, CurE);
      NOnV1.Bind(CurE, PrecE);
    }
    if (V1.IsSame(VP2))
    {
      NOnV2.Bind(PrecE, CurE);
      NOnV1.Bind(CurE, PrecE);
    }
    if (V2.IsSame(VP1))
    {
      NOnV1.Bind(PrecE, CurE);
      NOnV2.Bind(CurE, PrecE);
    }
    if (V2.IsSame(VP2))
    {
      NOnV2.Bind(PrecE, CurE);
      NOnV2.Bind(CurE, PrecE);
    }
    PrecE = CurE;
    VP1   = V1;
    VP2   = V2;
    wexp.Next();
  }
  if (V1.IsSame(FV1))
  {
    NOnV1.Bind(FirstE, CurE);
    NOnV1.Bind(CurE, FirstE);
  }
  if (V1.IsSame(FV2))
  {
    NOnV2.Bind(FirstE, CurE);
    NOnV1.Bind(CurE, FirstE);
  }
  if (V2.IsSame(FV1))
  {
    NOnV1.Bind(FirstE, CurE);
    NOnV2.Bind(CurE, FirstE);
  }
  if (V2.IsSame(FV2))
  {
    NOnV2.Bind(FirstE, CurE);
    NOnV2.Bind(CurE, FirstE);
  }
}

//=================================================================================================

void BRepOffset_Tool::ExtentFace(
  const TopoDS_Face&                                                        F,
  NCollection_DataMap<TopoDS_Shape, TopoDS_Shape, TopTools_ShapeMapHasher>& ConstShapes,
  NCollection_DataMap<TopoDS_Shape, TopoDS_Shape, TopTools_ShapeMapHasher>& ToBuild,
  const TopAbs_State                                                        Side,
  const double                                                              TolConf,
  TopoDS_Face&                                                              NF)
{

  TopExp_Explorer                                                          exp, exp2;
  NCollection_DataMap<TopoDS_Shape, TopoDS_Shape, TopTools_ShapeMapHasher> Build;
  NCollection_DataMap<TopoDS_Shape, TopoDS_Shape, TopTools_ShapeMapHasher> Extent;
  TopoDS_Edge                                                              FirstE, PrecE, CurE, NE;
  BRep_Builder                                                             B;
  TopoDS_Face                                                              EF;

  // Construction de la boite englobante de la face a etendre et des bouchons pour
  // limiter les extensions.
  // Bnd_Box ContextBox;
  // BRepBndLib::Add(F,B);
  // TopTools_DataMapIteratorOfDataMapOfShape itTB(ToBuild);
  // for (; itTB.More(); itTB.Next()) {
  // BRepBndLib::Add(TopBuild.Value(), ContextBox);
  //}

  bool SurfaceChange;
  SurfaceChange = EnLargeFace(F, EF, true);

  TopoDS_Shape aLocalShape = EF.EmptyCopied();
  NF                       = TopoDS::Face(aLocalShape);
  //  NF = TopoDS::Face(EF.EmptyCopied());
  NF.Orientation(TopAbs_FORWARD);

  if (SurfaceChange)
  {
    //------------------------------------------------
    // Mise a jour des pcurves sur la surface de base.
    //------------------------------------------------
    TopoDS_Face Fforward = F;
    Fforward.Orientation(TopAbs_FORWARD);
    NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher> Emap;
    TopExp::MapShapes(Fforward, TopAbs_EDGE, Emap);
    double f, l;
    for (int i = 1; i <= Emap.Extent(); i++)
    {
      TopoDS_Edge CE = TopoDS::Edge(Emap(i));
      CE.Orientation(TopAbs_FORWARD);
      TopoDS_Edge               Ecs; // patch
      occ::handle<Geom2d_Curve> C2 = BRep_Tool::CurveOnSurface(CE, Fforward, f, l);
      if (!C2.IsNull())
      {
        if (ConstShapes.IsBound(CE))
        {
          Ecs = TopoDS::Edge(ConstShapes(CE));
          BRep_Tool::Range(Ecs, f, l);
        }
        if (BRep_Tool::IsClosed(CE, Fforward))
        {
          TopoDS_Shape              aLocalShapeReversedCE = CE.Reversed();
          occ::handle<Geom2d_Curve> C2R =
            BRep_Tool::CurveOnSurface(TopoDS::Edge(aLocalShapeReversedCE), Fforward, f, l);
          //	  occ::handle<Geom2d_Curve> C2R =
          //	    BRep_Tool::CurveOnSurface(TopoDS::Edge(CE.Reversed()),F,f,l);
          B.UpdateEdge(CE, C2, C2R, EF, BRep_Tool::Tolerance(CE));
          if (!Ecs.IsNull())
            B.UpdateEdge(Ecs, C2, C2R, EF, BRep_Tool::Tolerance(CE));
        }
        else
        {
          B.UpdateEdge(CE, C2, EF, BRep_Tool::Tolerance(CE));
          if (!Ecs.IsNull())
            B.UpdateEdge(Ecs, C2, EF, BRep_Tool::Tolerance(CE));
        }
        B.Range(CE, f, l);
        if (!Ecs.IsNull())
          B.Range(Ecs, f, l);
      }
    }
  }

  for (exp.Init(F.Oriented(TopAbs_FORWARD), TopAbs_WIRE); exp.More(); exp.Next())
  {
    const TopoDS_Wire& W = TopoDS::Wire(exp.Current());
    NCollection_DataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher>
      MVE; // Vertex -> Edges incidentes.
    NCollection_DataMap<TopoDS_Shape, TopoDS_Shape, TopTools_ShapeMapHasher> NOnV1;
    NCollection_DataMap<TopoDS_Shape, TopoDS_Shape, TopTools_ShapeMapHasher> NOnV2;

    MapVertexEdges(W, MVE);
    BuildNeighbour(W, F, NOnV1, NOnV2);

    NCollection_List<TopoDS_Shape> LInt1, LInt2;
    TopoDS_Face                    StopFace;
    //------------------------------------------------
    // Construction edges
    //------------------------------------------------
    for (exp2.Init(W.Oriented(TopAbs_FORWARD), TopAbs_EDGE); exp2.More(); exp2.Next())
    {
      const TopoDS_Edge& E = TopoDS::Edge(exp2.Current());
      if (ConstShapes.IsBound(E))
        ToBuild.UnBind(E);
      if (ToBuild.IsBound(E))
      {
        NCollection_List<TopoDS_Shape> LOE;
        LOE.Append(E);
        if (BRepOffset_Tool::TryProject(TopoDS::Face(ToBuild(E)),
                                        EF,
                                        LOE,
                                        LInt2,
                                        LInt1,
                                        Side,
                                        TolConf)
            && !LInt1.IsEmpty())
          ToBuild.UnBind(E);
      }
    }

    for (exp2.Init(W.Oriented(TopAbs_FORWARD), TopAbs_EDGE); exp2.More(); exp2.Next())
    {
      const TopoDS_Edge& E = TopoDS::Edge(exp2.Current());
      if (ConstShapes.IsBound(E))
        ToBuild.UnBind(E);
      if (ToBuild.IsBound(E))
      {
        EnLargeFace(TopoDS::Face(ToBuild(E)), StopFace, false);
        TopoDS_Face NullFace;
        BRepOffset_Tool::Inter3D(EF, StopFace, LInt1, LInt2, Side, E, NullFace, NullFace);
        // No intersection, it may happen for example for a chosen (non-offsetted) planar face and
        // its neighbour offsetted cylindrical face, if the offset is directed so that
        // the radius of the cylinder becomes smaller.
        if (LInt1.IsEmpty())
          continue;
        if (LInt1.Extent() > 1)
        {
          // l intersection est en plusieurs edges (franchissement de couture)
          SelectEdge(F, EF, E, LInt1);
        }
        NE                          = TopoDS::Edge(LInt1.First());
        occ::handle<BRep_TEdge>& TE = *((occ::handle<BRep_TEdge>*)&NE.TShape());
        TE->Tolerance(TE->Tolerance() * 10.); //????
        if (NE.Orientation() == E.Orientation())
        {
          Build.Bind(E, NE.Oriented(TopAbs_FORWARD));
        }
        else
        {
          Build.Bind(E, NE.Oriented(TopAbs_REVERSED));
        }
        const TopoDS_Edge& EOnV1 = TopoDS::Edge(NOnV1(E));
        if (!ToBuild.IsBound(EOnV1) && !ConstShapes.IsBound(EOnV1) && !Build.IsBound(EOnV1))
        {
          ExtentEdge(F, EF, EOnV1, NE);
          Build.Bind(EOnV1, NE.Oriented(TopAbs_FORWARD));
        }
        const TopoDS_Edge& EOnV2 = TopoDS::Edge(NOnV2(E));
        if (!ToBuild.IsBound(EOnV2) && !ConstShapes.IsBound(EOnV2) && !Build.IsBound(EOnV2))
        {
          ExtentEdge(F, EF, EOnV2, NE);
          Build.Bind(EOnV2, NE.Oriented(TopAbs_FORWARD));
        }
      }
    }

    //------------------------------------------------
    // Construction Vertex.
    //------------------------------------------------
    NCollection_List<TopoDS_Shape> LV;
    double                         f, l;
    TopoDS_Edge                    ERef;
    TopoDS_Vertex                  V1, V2;

    for (exp2.Init(W.Oriented(TopAbs_FORWARD), TopAbs_EDGE); exp2.More(); exp2.Next())
    {
      const TopoDS_Edge& E = TopoDS::Edge(exp2.Current());
      TopExp::Vertices(E, V1, V2);
      BRep_Tool::Range(E, f, l);
      TopoDS_Vertex V;
      if (Build.IsBound(E))
      {
        const TopoDS_Edge& NEOnV1 = TopoDS::Edge(NOnV1(E));
        if (Build.IsBound(NEOnV1) && (ToBuild.IsBound(E) || ToBuild.IsBound(NEOnV1)))
        {
          if (E.IsSame(NEOnV1))
            V = TopExp::FirstVertex(TopoDS::Edge(Build(E)));
          else
          {
            //---------------
            // intersection.
            //---------------
            if (!Build.IsBound(V1))
            {
              Inter2d(EF,
                      TopoDS::Edge(Build(E)),
                      TopoDS::Edge(Build(NEOnV1)),
                      LV,
                      /*TolConf*/ Precision::Confusion());

              if (!LV.IsEmpty())
              {
                if (Build(E).Orientation() == TopAbs_FORWARD)
                {
                  V = TopoDS::Vertex(LV.First());
                }
                else
                {
                  V = TopoDS::Vertex(LV.Last());
                }
              }
              else
              {
                return;
              }
            }
            else
            {
              V = TopoDS::Vertex(Build(V1));
              if (MVE(V1).Extent() > 2)
              {
                V.Orientation(TopAbs_FORWARD);
                if (Build(E).Orientation() == TopAbs_REVERSED)
                  V.Orientation(TopAbs_REVERSED);

                ProjectVertexOnEdge(V, TopoDS::Edge(Build(E)), TolConf);
              }
            }
          }
        }
        else
        {
          //------------
          // projection
          //------------
          V = V1;
          if (ConstShapes.IsBound(V1))
            V = TopoDS::Vertex(ConstShapes(V1));
          V.Orientation(TopAbs_FORWARD);
          if (Build(E).Orientation() == TopAbs_REVERSED)
            V.Orientation(TopAbs_REVERSED);
          if (!TryParameter(E, V, TopoDS::Edge(Build(E)), TolConf))
            ProjectVertexOnEdge(V, TopoDS::Edge(Build(E)), TolConf);
        }

        ConstShapes.Bind(V1, V);
        Build.Bind(V1, V);
        const TopoDS_Edge& NEOnV2 = TopoDS::Edge(NOnV2(E));
        if (Build.IsBound(NEOnV2) && (ToBuild.IsBound(E) || ToBuild.IsBound(NEOnV2)))
        {
          if (E.IsSame(NEOnV2))
            V = TopExp::LastVertex(TopoDS::Edge(Build(E)));
          else
          {
            //--------------
            // intersection.
            //---------------

            if (!Build.IsBound(V2))
            {
              Inter2d(EF,
                      TopoDS::Edge(Build(E)),
                      TopoDS::Edge(Build(NEOnV2)),
                      LV,
                      /*TolConf*/ Precision::Confusion());

              if (!LV.IsEmpty())
              {
                if (Build(E).Orientation() == TopAbs_FORWARD)
                {
                  V = TopoDS::Vertex(LV.Last());
                }
                else
                {
                  V = TopoDS::Vertex(LV.First());
                }
              }
              else
              {
                return;
              }
            }
            else
            {
              V = TopoDS::Vertex(Build(V2));
              if (MVE(V2).Extent() > 2)
              {
                V.Orientation(TopAbs_REVERSED);
                if (Build(E).Orientation() == TopAbs_REVERSED)
                  V.Orientation(TopAbs_FORWARD);

                ProjectVertexOnEdge(V, TopoDS::Edge(Build(E)), TolConf);
              }
            }
          }
        }
        else
        {
          //------------
          // projection
          //------------
          V = V2;
          if (ConstShapes.IsBound(V2))
            V = TopoDS::Vertex(ConstShapes(V2));
          V.Orientation(TopAbs_REVERSED);
          if (Build(E).Orientation() == TopAbs_REVERSED)
            V.Orientation(TopAbs_FORWARD);
          if (!TryParameter(E, V, TopoDS::Edge(Build(E)), TolConf))
            ProjectVertexOnEdge(V, TopoDS::Edge(Build(E)), TolConf);
        }
        ConstShapes.Bind(V2, V);
        Build.Bind(V2, V);
      }
    }

    TopoDS_Wire        NW;
    TopoDS_Vertex      NV1, NV2;
    TopAbs_Orientation Or;
    double             U1, U2;
    constexpr double   eps = Precision::Confusion();

#ifdef OCCT_DEBUG
    TopLoc_Location L;
#endif
    B.MakeWire(NW);

    //-----------------
    // Reconstruction.
    //-----------------
    for (exp2.Init(W.Oriented(TopAbs_FORWARD), TopAbs_EDGE); exp2.More(); exp2.Next())
    {
      const TopoDS_Edge& E = TopoDS::Edge(exp2.Current());
      TopExp::Vertices(E, V1, V2);
      if (Build.IsBound(E))
      {
        NE = TopoDS::Edge(Build(E));
        BRep_Tool::Range(NE, f, l);
        Or = NE.Orientation();
        //-----------------------------------------------------
        // Copy pour virer les vertex deja sur la nouvelle edge.
        //-----------------------------------------------------
        NV1 = TopoDS::Vertex(ConstShapes(V1));
        NV2 = TopoDS::Vertex(ConstShapes(V2));

        TopoDS_Shape aLocalVertexOrientedNV1 = NV1.Oriented(TopAbs_INTERNAL);
        TopoDS_Shape aLocalEdge              = NE.Oriented(TopAbs_INTERNAL);

        U1 =
          BRep_Tool::Parameter(TopoDS::Vertex(aLocalVertexOrientedNV1), TopoDS::Edge(aLocalEdge));
        aLocalVertexOrientedNV1 = NV2.Oriented(TopAbs_INTERNAL);
        aLocalEdge              = NE.Oriented(TopAbs_FORWARD);
        U2 =
          BRep_Tool::Parameter(TopoDS::Vertex(aLocalVertexOrientedNV1), TopoDS::Edge(aLocalEdge));
        //	U1 = BRep_Tool::Parameter
        //	  (TopoDS::Vertex(NV1.Oriented(TopAbs_INTERNAL)),
        //	   TopoDS::Edge  (NE .Oriented(TopAbs_FORWARD)));
        //	U2 = BRep_Tool::Parameter
        //	  (TopoDS::Vertex(NV2.Oriented(TopAbs_INTERNAL)),
        //	   TopoDS::Edge  (NE.Oriented(TopAbs_FORWARD)));
        aLocalEdge = NE.EmptyCopied();
        NE         = TopoDS::Edge(aLocalEdge);
        NE.Orientation(TopAbs_FORWARD);
        if (NV1.IsSame(NV2))
        {
          //--------------
          // edge ferme.
          //--------------
          if (Or == TopAbs_FORWARD)
          {
            U1 = f;
            U2 = l;
          }
          else
          {
            U1 = l;
            U2 = f;
          }
          if (Or == TopAbs_FORWARD)
          {
            if (U1 > U2)
            {
              if (std::abs(U1 - l) < eps)
                U1 = f;
              if (std::abs(U2 - f) < eps)
                U2 = l;
            }
            TopoDS_Shape aLocalVertex = NV1.Oriented(TopAbs_FORWARD);
            B.Add(NE, TopoDS::Vertex(aLocalVertex));
            aLocalVertex = NV2.Oriented(TopAbs_REVERSED);
            B.Add(NE, TopoDS::Vertex(aLocalVertex));
            //		B.Add (NE,TopoDS::Vertex(NV1.Oriented(TopAbs_FORWARD )));
            //		B.Add (NE,TopoDS::Vertex(NV2.Oriented(TopAbs_REVERSED)));
            B.Range(NE, U1, U2);
            ConstShapes.Bind(E, NE);
            NE.Orientation(E.Orientation());
          }
          else
          {
            if (U2 > U1)
            {
              if (std::abs(U2 - l) < eps)
                U2 = f;
              if (std::abs(U1 - f) < eps)
                U1 = l;
            }
            TopoDS_Shape aLocalVertex = NV2.Oriented(TopAbs_FORWARD);
            B.Add(NE, TopoDS::Vertex(aLocalVertex));
            aLocalVertex = NV1.Oriented(TopAbs_REVERSED);
            B.Add(NE, TopoDS::Vertex(aLocalVertex));
            //		B.Add (NE,TopoDS::Vertex(NV2.Oriented(TopAbs_FORWARD )));
            //		B.Add (NE,TopoDS::Vertex(NV1.Oriented(TopAbs_REVERSED)));
            B.Range(NE, U2, U1);
            ConstShapes.Bind(E, NE.Oriented(TopAbs_REVERSED));
            NE.Orientation(TopAbs::Reverse(E.Orientation()));
          }
        }
        else
        {
          //-------------------
          // edge is not ferme.
          //-------------------
          if (Or == TopAbs_FORWARD)
          {
            if (U1 > U2)
            {
              TopoDS_Shape aLocalVertex = NV2.Oriented(TopAbs_FORWARD);
              B.Add(NE, TopoDS::Vertex(aLocalVertex));
              aLocalVertex = NV1.Oriented(TopAbs_REVERSED);
              B.Add(NE, TopoDS::Vertex(aLocalVertex));
              //		B.Add (NE,TopoDS::Vertex(NV2.Oriented(TopAbs_FORWARD )));
              //		B.Add (NE,TopoDS::Vertex(NV1.Oriented(TopAbs_REVERSED)));
              B.Range(NE, U2, U1);
            }
            else
            {
              TopoDS_Shape aLocalVertex = NV1.Oriented(TopAbs_FORWARD);
              B.Add(NE, TopoDS::Vertex(aLocalVertex));
              aLocalVertex = NV2.Oriented(TopAbs_REVERSED);
              B.Add(NE, TopoDS::Vertex(aLocalVertex));
              //		  B.Add (NE,TopoDS::Vertex(NV1.Oriented(TopAbs_FORWARD )));
              //		  B.Add (NE,TopoDS::Vertex(NV2.Oriented(TopAbs_REVERSED)));
              B.Range(NE, U1, U2);
            }
            ConstShapes.Bind(E, NE);
            NE.Orientation(E.Orientation());
          }
          else
          {
            if (U2 > U1)
            {
              TopoDS_Shape aLocalVertex = NV1.Oriented(TopAbs_FORWARD);
              B.Add(NE, TopoDS::Vertex(aLocalVertex));
              aLocalVertex = NV2.Oriented(TopAbs_REVERSED);
              B.Add(NE, TopoDS::Vertex(aLocalVertex));
              //		B.Add (NE,TopoDS::Vertex(NV1.Oriented(TopAbs_FORWARD )));
              //		B.Add (NE,TopoDS::Vertex(NV2.Oriented(TopAbs_REVERSED)));
              B.Range(NE, U1, U2);
              ConstShapes.Bind(E, NE);
              NE.Orientation(E.Orientation());
            }
            else
            {
              TopoDS_Shape aLocalVertex = NV2.Oriented(TopAbs_FORWARD);
              B.Add(NE, TopoDS::Vertex(aLocalVertex));
              aLocalVertex = NV1.Oriented(TopAbs_REVERSED);
              B.Add(NE, TopoDS::Vertex(aLocalVertex));
              //		  B.Add (NE,TopoDS::Vertex(NV2.Oriented(TopAbs_FORWARD )));
              //		  B.Add (NE,TopoDS::Vertex(NV1.Oriented(TopAbs_REVERSED)));
              B.Range(NE, U2, U1);
              ConstShapes.Bind(E, NE.Oriented(TopAbs_REVERSED));
              NE.Orientation(TopAbs::Reverse(E.Orientation()));
            }
          }
        }
        Build.UnBind(E);
      } // Build.IsBound(E)
      else if (ConstShapes.IsBound(E))
      { // !Build.IsBound(E)
        NE = TopoDS::Edge(ConstShapes(E));
        BuildPCurves(NE, NF);
        Or = NE.Orientation();
        if (Or == TopAbs_REVERSED)
        {
          NE.Orientation(TopAbs::Reverse(E.Orientation()));
        }
        else
        {
          NE.Orientation(E.Orientation());
        }
      }
      else
      {
        NE = E;
        ConstShapes.Bind(E, NE.Oriented(TopAbs_FORWARD));
      }
      B.Add(NW, NE);
    }
    B.Add(NF, NW.Oriented(W.Orientation()));
  }
  NF.Orientation(F.Orientation());
  BRepTools::Update(NF); // Maj des UVPoints

}

//=================================================================================================

TopoDS_Shape BRepOffset_Tool::Deboucle3D(
  const TopoDS_Shape&                                           S,
  const NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher>& Boundary)
{
  TopoDS_Shape SS;
  switch (S.ShapeType())
  {
    case TopAbs_SHELL: {
      // if the shell contains free borders that do not belong to the
      // free borders of caps (Boundary) it is removed.
      NCollection_IndexedDataMap<TopoDS_Shape,
                                 NCollection_List<TopoDS_Shape>,
                                 TopTools_ShapeMapHasher>
        Map;
      TopExp::MapShapesAndAncestors(S, TopAbs_EDGE, TopAbs_FACE, Map);

      bool JeGarde = true;
      for (int i = 1; i <= Map.Extent() && JeGarde; i++)
      {
        const NCollection_List<TopoDS_Shape>& aLF = Map(i);
        if (aLF.Extent() < 2)
        {
          const TopoDS_Edge& anEdge = TopoDS::Edge(Map.FindKey(i));
          if (anEdge.Orientation() == TopAbs_INTERNAL)
          {
            const TopoDS_Face& aFace = TopoDS::Face(aLF.First());
            if (aFace.Orientation() != TopAbs_INTERNAL)
            {
              continue;
            }
          }
          if (!Boundary.Contains(anEdge) && !BRep_Tool::Degenerated(anEdge))
            JeGarde = false;
        }
      }
      if (JeGarde)
        SS = S;
    }
    break;

    case TopAbs_COMPOUND:
    case TopAbs_SOLID: {
      // iterate on sub-shapes and add non-empty.
      TopoDS_Iterator it(S);
      TopoDS_Shape    SubShape;
      int             NbSub = 0;
      BRep_Builder    B;
      if (S.ShapeType() == TopAbs_COMPOUND)
      {
        B.MakeCompound(TopoDS::Compound(SS));
      }
      else
      {
        B.MakeSolid(TopoDS::Solid(SS));
      }
      for (; it.More(); it.Next())
      {
        const TopoDS_Shape& CurS = it.Value();
        SubShape                 = Deboucle3D(CurS, Boundary);
        if (!SubShape.IsNull())
        {
          B.Add(SS, SubShape);
          NbSub++;
        }
      }
      if (NbSub == 0)
      {
        SS = TopoDS_Shape();
      }
    }
    break;

    default:
      break;
  }

  return SS;
}

//=================================================================================================

static bool IsInOut(BRepTopAdaptor_FClass2d&   FC,
                    const Geom2dAdaptor_Curve& AC,
                    const TopAbs_State&        S)
{
  constexpr double              Def = 100 * Precision::Confusion();
  GCPnts_QuasiUniformDeflection QU(AC, Def);

  for (int i = 1; i <= QU.NbPoints(); i++)
  {
    gp_Pnt2d P = AC.Value(QU.Parameter(i));
    if (FC.Perform(P) != S)
    {
      return false;
    }
  }
  return true;
}

//=================================================================================================

void BRepOffset_Tool::CorrectOrientation(
  const TopoDS_Shape&                                                  SI,
  const NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher>& NewEdges,
  occ::handle<BRepAlgo_AsDes>&                                         AsDes,
  BRepAlgo_Image&                                                      InitOffset,
  const double                                                         Offset)
{

  TopExp_Explorer exp;
  exp.Init(SI, TopAbs_FACE);
  double f = 0., l = 0.;

  for (; exp.More(); exp.Next())
  {

    const TopoDS_Face&                       FI  = TopoDS::Face(exp.Current());
    const NCollection_List<TopoDS_Shape>&    LOF = InitOffset.Image(FI);
    NCollection_List<TopoDS_Shape>::Iterator it(LOF);
    for (; it.More(); it.Next())
    {
      const TopoDS_Face&                       OF  = TopoDS::Face(it.Value());
      NCollection_List<TopoDS_Shape>&          LOE = AsDes->ChangeDescendant(OF);
      NCollection_List<TopoDS_Shape>::Iterator itE(LOE);

      bool YaInt = false;
      for (; itE.More(); itE.Next())
      {
        const TopoDS_Edge& OE = TopoDS::Edge(itE.Value());
        if (NewEdges.Contains(OE))
        {
          YaInt = true;
          break;
        }
      }
      if (YaInt)
      {
        TopoDS_Shape            aLocalFace = FI.Oriented(TopAbs_FORWARD);
        BRepTopAdaptor_FClass2d FC(TopoDS::Face(aLocalFace), Precision::Confusion());
        //	BRepTopAdaptor_FClass2d FC (TopoDS::Face(FI.Oriented(TopAbs_FORWARD)),
        //				    Precision::Confusion());
        for (itE.Initialize(LOE); itE.More(); itE.Next())
        {
          TopoDS_Shape& OE = itE.ChangeValue();
          if (NewEdges.Contains(OE))
          {
            occ::handle<Geom2d_Curve> CO2d = BRep_Tool::CurveOnSurface(TopoDS::Edge(OE), OF, f, l);
            Geom2dAdaptor_Curve       AC(CO2d, f, l);

            if (Offset > 0)
            {
              if (IsInOut(FC, AC, TopAbs_OUT))
                OE.Reverse();
            }
            //	    else {
            //	      if (IsInOut(FC,AC,TopAbs_IN)) OE.Reverse();
            //	    }
          }
        }
      }
    }
  }
}

//=================================================================================================

bool BRepOffset_Tool::CheckPlanesNormals(const TopoDS_Face& theFace1,
                                         const TopoDS_Face& theFace2,
                                         const double       theTolAng)
{
  BRepAdaptor_Surface aBAS1(theFace1, false), aBAS2(theFace2, false);
  if (aBAS1.GetType() != GeomAbs_Plane || aBAS2.GetType() != GeomAbs_Plane)
  {
    return false;
  }
  //
  gp_Dir aDN1 = aBAS1.Plane().Position().Direction();
  if (theFace1.Orientation() == TopAbs_REVERSED)
  {
    aDN1.Reverse();
  }
  //
  gp_Dir aDN2 = aBAS2.Plane().Position().Direction();
  if (theFace2.Orientation() == TopAbs_REVERSED)
  {
    aDN2.Reverse();
  }
  //
  double anAngle = aDN1.Angle(aDN2);
  return (anAngle < theTolAng);
}

//=================================================================================================

void PerformPlanes(const TopoDS_Face&              theFace1,
                   const TopoDS_Face&              theFace2,
                   const TopAbs_State              theSide,
                   NCollection_List<TopoDS_Shape>& theL1,
                   NCollection_List<TopoDS_Shape>& theL2)
{
  theL1.Clear();
  theL2.Clear();
  // Intersect the planes using IntTools_FaceFace directly
  IntTools_FaceFace aFF;
  aFF.SetParameters(true, true, true, Precision::Confusion());
  aFF.Perform(theFace1, theFace2);
  //
  if (!aFF.IsDone())
  {
    return;
  }
  //
  const NCollection_Sequence<IntTools_Curve>& aSC = aFF.Lines();
  if (aSC.IsEmpty())
  {
    return;
  }
  //
  // In Plane/Plane intersection only one curve is always produced.
  // Make the edge from this section curve.
  TopoDS_Edge aE;
  {
    BRep_Builder                   aBB;
    const IntTools_Curve&          aIC  = aSC(1);
    const occ::handle<Geom_Curve>& aC3D = aIC.Curve();
    aBB.MakeEdge(aE, aC3D, aIC.Tolerance());
    // Get bounds of the curve
    double aTF, aTL;
    gp_Pnt aPF, aPL;
    aIC.Bounds(aTF, aTL, aPF, aPL);
    // Make the bounding vertices
    TopoDS_Vertex aVF, aVL;
    aBB.MakeVertex(aVF, aPF, aIC.Tolerance());
    aBB.MakeVertex(aVL, aPL, aIC.Tolerance());
    aVL.Orientation(TopAbs_REVERSED);
    // Add vertices to the edge
    aBB.Add(aE, aVF);
    aBB.Add(aE, aVL);
    // Add 2D curves to the edge
    aBB.UpdateEdge(aE, aIC.FirstCurve2d(), theFace1, aIC.Tolerance());
    aBB.UpdateEdge(aE, aIC.SecondCurve2d(), theFace2, aIC.Tolerance());
    // Update range of the new edge
    aBB.Range(aE, aTF, aTL);
  }
  //
  // Orient section
  TopAbs_Orientation O1, O2;
  BRepOffset_Tool::OrientSection(aE, theFace1, theFace2, O1, O2);
  if (theSide == TopAbs_OUT)
  {
    O1 = TopAbs::Reverse(O1);
    O2 = TopAbs::Reverse(O2);
  }
  //
  BRepLib::SameParameter(aE, Precision::Confusion(), true);
  //
  // Add edge to result
  theL1.Append(aE.Oriented(O1));
  theL2.Append(aE.Oriented(O2));
}

//=======================================================================
// function : IsInf
// purpose  : Checks if the given value is close to infinite (TheInfini)
//=======================================================================
bool IsInf(const double theVal)
{
  return (theVal > TheInfini * 0.9);
}

static void UpdateVertexTolerances(const TopoDS_Face& theFace)
{
  BRep_Builder BB;
  NCollection_IndexedDataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher>
    VEmap;
  TopExp::MapShapesAndAncestors(theFace, TopAbs_VERTEX, TopAbs_EDGE, VEmap);

  for (int i = 1; i <= VEmap.Extent(); i++)
  {
    const TopoDS_Vertex&                     aVertex = TopoDS::Vertex(VEmap.FindKey(i));
    const NCollection_List<TopoDS_Shape>&    Elist   = VEmap(i);
    gp_Pnt                                   PntVtx  = BRep_Tool::Pnt(aVertex);
    NCollection_List<TopoDS_Shape>::Iterator itl(Elist);
    for (; itl.More(); itl.Next())
    {
      const TopoDS_Edge& anEdge = TopoDS::Edge(itl.Value());
      TopoDS_Vertex      V1, V2;
      TopExp::Vertices(anEdge, V1, V2);
      double fpar, lpar;
      BRep_Tool::Range(anEdge, fpar, lpar);
      double aParam = (V1.IsSame(aVertex)) ? fpar : lpar;
      if (!BRep_Tool::Degenerated(anEdge))
      {
        BRepAdaptor_Curve BAcurve(anEdge);
        gp_Pnt            aPnt  = BAcurve.Value(aParam);
        double            aDist = PntVtx.Distance(aPnt);
        BB.UpdateVertex(aVertex, aDist);
        if (V1.IsSame(V2))
        {
          aPnt  = BAcurve.Value(lpar);
          aDist = PntVtx.Distance(aPnt);
          BB.UpdateVertex(aVertex, aDist);
        }
      }
      BRepAdaptor_Curve BAcurveonsurf(anEdge, theFace);
      gp_Pnt            aPnt  = BAcurveonsurf.Value(aParam);
      double            aDist = PntVtx.Distance(aPnt);
      BB.UpdateVertex(aVertex, aDist);
      if (V1.IsSame(V2))
      {
        aPnt  = BAcurveonsurf.Value(lpar);
        aDist = PntVtx.Distance(aPnt);
        BB.UpdateVertex(aVertex, aDist);
      }
    }
  }
}
