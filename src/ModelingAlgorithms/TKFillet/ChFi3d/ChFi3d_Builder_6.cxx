// Created on: 1994-10-25
// Created by: Laurent BOURESCHE
// Copyright (c) 1994-1999 Matra Datavision
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

// modif : jlr branchement F(t) pour Edge/Face
//  Modified by skv - Wed Jun  9 17:16:26 2004 OCC5898
//  modified by Edward AGAPOV (eap) Fri Feb  8 2002 (bug occ67 == BUC61052)
//  ComputeData(), case where BRepBlend_Walking::Continu() can't get up to Target

#include <Adaptor2d_Curve2d.hxx>
#include <Blend_CurvPointFuncInv.hxx>
#include <Blend_RstRstFunction.hxx>
#include <Blend_SurfCurvFuncInv.hxx>
#include <Blend_SurfPointFuncInv.hxx>
#include <Blend_SurfRstFunction.hxx>
#include <BRepBlend_AppFunc.hxx>
#include <BRepBlend_AppFuncRst.hxx>
#include <BRepBlend_AppFuncRstRst.hxx>
#include <BRepBlend_AppSurf.hxx>
#include <BRepBlend_AppSurface.hxx>
#include <BRepBlend_ConstRad.hxx>
#include <BRepBlend_ConstRadInv.hxx>
#include <BRepBlend_CSWalking.hxx>
#include <BRepBlend_Line.hxx>
#include <BRepBlend_RstRstLineBuilder.hxx>
#include <BRepBlend_SurfRstLineBuilder.hxx>
#include <BRepBlend_Walking.hxx>
#include <BRepTopAdaptor_HVertex.hxx>
#include <BRepTopAdaptor_TopolTool.hxx>
#include <BSplCLib.hxx>
#include <ChFi3d_Builder.hxx>
#include <ChFi3d_Builder_0.hxx>
#include <ChFiDS_CommonPoint.hxx>
#include <ChFiDS_FaceInterference.hxx>
#include <ChFiDS_ElSpine.hxx>
#include <ChFiDS_Spine.hxx>
#include <ChFiDS_SurfData.hxx>
#include <Geom2d_BSplineCurve.hxx>
#include <Geom2d_Curve.hxx>
#include <Geom2d_Line.hxx>
#include <Geom2d_TrimmedCurve.hxx>
#include <Geom2dAPI_ProjectPointOnCurve.hxx>
#include <Geom2dInt_GInter.hxx>
#include <Geom_BSplineSurface.hxx>
#include <Geom_Curve.hxx>
#include <Geom_Surface.hxx>
#include <GeomAdaptor_Curve.hxx>
#include <GeomLib.hxx>
#include <gp_Dir2d.hxx>
#include <gp_Pnt.hxx>
#include <gp_Pnt2d.hxx>
#include <gp_Vec.hxx>
#include <gp_Vec2d.hxx>
#include <IntRes2d_IntersectionPoint.hxx>
#include <math_Vector.hxx>
#include <Precision.hxx>
#include <TopExp.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopOpeBRepBuild_HBuilder.hxx>
#include <TopOpeBRepDS_Curve.hxx>
#include <TopOpeBRepDS_DataStructure.hxx>
#include <TopOpeBRepDS_HDataStructure.hxx>
#include <TopOpeBRepDS_Surface.hxx>
#include <NCollection_List.hxx>

#include <cstdio>

// #define DRAW

#ifdef DRAW
  #include <Draw_Appli.hxx>
  #include <Draw_Segment2D.hxx>
  #include <Draw_Marker2D.hxx>
  #include <Draw_Segment3D.hxx>
  #include <Draw_Marker3D.hxx>
  #include <Draw.hxx>
  #include <DrawTrSurf.hxx>
  #include <BRepAdaptor_Surface.hxx>
static int IndexOfConge = 0;
#endif

#ifdef OCCT_DEBUG
extern bool ChFi3d_GettraceDRAWFIL();
extern bool ChFi3d_GettraceDRAWWALK();
extern bool ChFi3d_GetcontextNOOPT();
extern void ChFi3d_SettraceDRAWFIL(const bool b);
extern void ChFi3d_SettraceDRAWWALK(const bool b);
extern void ChFi3d_SetcontextNOOPT(const bool b);
#endif

#ifdef DRAW
static void drawline(const occ::handle<BRepBlend_Line>& lin, const bool iscs)
{
  occ::handle<Draw_Marker3D>  p3d;
  occ::handle<Draw_Marker2D>  p2d;
  occ::handle<Draw_Segment3D> tg3d;
  occ::handle<Draw_Segment2D> tg2d;

  for (int i = 1; i <= lin->NbPoints(); i++)
  {
    const Blend_Point& pt    = lin->Point(i);
    gp_Pnt             point = pt.PointOnS1();
    gp_Pnt             extr  = point.Translated(pt.TangentOnS1());
    p3d                      = new Draw_Marker3D(point, Draw_Square, Draw_rouge);
    dout << p3d;
    tg3d = new Draw_Segment3D(point, extr, Draw_rouge);
    dout << tg3d;
    point = pt.PointOnS2();
    extr  = point.Translated(pt.TangentOnS2());
    p3d   = new Draw_Marker3D(point, Draw_Plus, Draw_jaune);
    dout << p3d;
    tg3d = new Draw_Segment3D(point, extr, Draw_jaune);
    dout << tg3d;

    double u, v;
    pt.ParametersOnS1(u, v);
    gp_Pnt2d point2d(u, v);
    gp_Pnt2d extr2d = point2d.Translated(pt.Tangent2dOnS1());
    p2d             = new Draw_Marker2D(point2d, Draw_Square, Draw_rouge);
    dout << p2d;
    tg2d = new Draw_Segment2D(point2d, extr2d, Draw_rouge);
    dout << tg2d;
    pt.ParametersOnS2(u, v);
    point2d.SetCoord(u, v);
    extr2d = point2d.Translated(pt.Tangent2dOnS2());
    p2d    = new Draw_Marker2D(point2d, Draw_Plus, Draw_jaune);
    dout << p2d;
    tg2d = new Draw_Segment2D(point2d, extr2d, Draw_jaune);
    dout << tg2d;
    dout.Flush();
  }
}
#endif
//=================================================================================================

static int SearchIndex(const double Value, occ::handle<BRepBlend_Line>& Lin)
{
  int NbPnt = Lin->NbPoints(), Ind;

  for (Ind = 1; (Ind < NbPnt) && (Lin->Point(Ind).Parameter() < Value);)
    Ind++;
  return Ind;
}

//=================================================================================================

static int nbedconnex(const NCollection_List<TopoDS_Shape>& L)
{
  int                                      nb = 0, i = 0;
  NCollection_List<TopoDS_Shape>::Iterator It1(L);
  for (; It1.More(); It1.Next(), i++)
  {
    const TopoDS_Shape&                      curs   = It1.Value();
    bool                                     dejavu = false;
    NCollection_List<TopoDS_Shape>::Iterator It2(L);
    for (int j = 0; j < i && It2.More(); j++, It2.Next())
    {
      if (curs.IsSame(It2.Value()))
      {
        dejavu = true;
        break;
      }
    }
    if (!dejavu)
      nb++;
  }
  return nb;
}

static bool IsVois(const TopoDS_Edge&                                      E,
                   const TopoDS_Vertex&                                    Vref,
                   const ChFiDS_Map&                                       VEMap,
                   NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher>& DONE,
                   const int                                               prof,
                   const int                                               profmax)
{
  if (prof > profmax)
    return false;
  if (DONE.Contains(E))
    return false;
  TopoDS_Vertex V1, V2;
  TopExp::Vertices(E, V1, V2);
  if (Vref.IsSame(V1) || Vref.IsSame(V2))
    return true;
  DONE.Add(E);
  const NCollection_List<TopoDS_Shape>&    L1 = VEMap(V1);
  int                                      i1 = nbedconnex(L1);
  NCollection_List<TopoDS_Shape>::Iterator It1(L1);
  for (; It1.More(); It1.Next())
  {
    const TopoDS_Edge& curE = TopoDS::Edge(It1.Value());
    if (i1 <= 2)
    {
      if (IsVois(curE, Vref, VEMap, DONE, prof, profmax))
        return true;
    }
    else if (IsVois(curE, Vref, VEMap, DONE, prof + 1, profmax))
      return true;
  }
  const NCollection_List<TopoDS_Shape>& L2 = VEMap(V2);
#ifdef OCCT_DEBUG
//  int i2 = nbedconnex(L2);
#endif
  NCollection_List<TopoDS_Shape>::Iterator It2(L2);
  for (; It2.More(); It2.Next())
  {
    const TopoDS_Edge& curE = TopoDS::Edge(It2.Value());
    if (i1 <= 2)
    {
      if (IsVois(curE, Vref, VEMap, DONE, prof, profmax))
        return true;
    }
    else if (IsVois(curE, Vref, VEMap, DONE, prof + 1, profmax))
      return true;
  }
  return false;
}

static bool IsObst(const ChFiDS_CommonPoint& CP, const TopoDS_Vertex& Vref, const ChFiDS_Map& VEMap)
{
  if (!CP.IsOnArc())
    return false;
  const TopoDS_Edge&                                     E = CP.Arc();
  NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher> DONE;
  int                                                    prof = 4;
  return !IsVois(E, Vref, VEMap, DONE, 0, prof);
}

//=================================================================================================

static void CompParam(const Geom2dAdaptor_Curve&       Carc,
                      const occ::handle<Geom2d_Curve>& Ctg,
                      double&                          parc,
                      double&                          ptg,
                      const double                     prefarc,
                      const double                     preftg)
{
  bool found = false;
  //(1) It is checked if the provided parameters are good
  //    if pcurves have the same parameters as the spine.
  gp_Pnt2d point   = Carc.Value(prefarc);
  double   distini = point.Distance(Ctg->Value(preftg));
  if (distini <= Precision::PConfusion())
  {
    parc  = prefarc;
    ptg   = preftg;
    found = true;
  }
  else
  {
    //(2) Intersection
#ifdef OCCT_DEBUG
    std::cout << "CompParam : bad intersection parameters" << std::endl;
#endif
    IntRes2d_IntersectionPoint int2d;
    Geom2dInt_GInter           Intersection;
    int                        nbpt, nbseg;
    Intersection.Perform(Geom2dAdaptor_Curve(Ctg),
                         Carc,
                         Precision::PIntersection(),
                         Precision::PIntersection());

    double dist = Precision::Infinite(), p1, p2;
    if (Intersection.IsDone())
    {
      if (!Intersection.IsEmpty())
      {
        nbseg = Intersection.NbSegments();
        if (nbseg > 0)
        {
#ifdef OCCT_DEBUG
          std::cout << "segments of intersection on the restrictions" << std::endl;
#endif
        }
        nbpt = Intersection.NbPoints();
        for (int i = 1; i <= nbpt; i++)
        {
          int2d = Intersection.Point(i);
          p1    = int2d.ParamOnFirst();
          p2    = int2d.ParamOnSecond();
          if (std::abs(prefarc - p2) < dist)
          {
            ptg   = p1;
            parc  = p2;
            dist  = std::abs(prefarc - p2);
            found = true;
          }
        }
      }
    }
  }

  if (!found)
  {
    // (3) Projection...
#ifdef OCCT_DEBUG
    std::cout << "CompParam : failed intersection PC, projection is created." << std::endl;
#endif
    parc = prefarc;
    Geom2dAPI_ProjectPointOnCurve projector(point, Ctg);

    if (projector.NbPoints() == 0)
    {
      // This happens in some cases when there is a vertex
      // at the end of spine...
      ptg = preftg;
#ifdef OCCT_DEBUG
      std::cout << "CompParam : failed proj p2d/c2d, the extremity is taken!" << std::endl;
#endif
    }
    else
    {
      // It is checked if everything was calculated correctly (EDC402 C2)
      if (projector.LowerDistance() < distini)
        ptg = projector.LowerDistanceParameter();
      else
        ptg = preftg;
    }
  }
}

//=======================================================================
// function : CompBlendPoint
// purpose  : create BlendPoint corresponding to a tangency on Vertex
// pmn : 15/10/1997 : returns false, if there is no pcurve
//=======================================================================

static bool CompBlendPoint(const TopoDS_Vertex& V,
                           const TopoDS_Edge&   E,
                           const double         W,
                           const TopoDS_Face&   F1,
                           const TopoDS_Face&   F2,
                           Blend_Point&         BP)
{
  gp_Pnt2d                  P1, P2;
  gp_Pnt                    P3d;
  double                    param, f, l;
  occ::handle<Geom2d_Curve> pc;

  P3d   = BRep_Tool::Pnt(V);
  param = BRep_Tool::Parameter(V, E, F1);
  pc    = BRep_Tool::CurveOnSurface(E, F1, f, l);
  if (pc.IsNull())
    return false;
  P1    = pc->Value(param);
  param = BRep_Tool::Parameter(V, E, F2);
  pc    = BRep_Tool::CurveOnSurface(E, F2, f, l);
  if (pc.IsNull())
    return false;
  P2 = pc->Value(param);
  BP.SetValue(P3d, P3d, W, P1.X(), P1.Y(), P2.X(), P2.Y());
  return true;
}

//=======================================================================
// function :  UpdateLine
// purpose  : Updates extremities after a partial invalidation
//=======================================================================

static void UpdateLine(occ::handle<BRepBlend_Line>& Line, const bool isfirst)
{
  double tguide, U, V;
  if (isfirst)
  {
    const Blend_Point& BP = Line->Point(1);
    tguide                = BP.Parameter();
    if (Line->StartPointOnFirst().ParameterOnGuide() < tguide)
    {
      BRepBlend_Extremity BE;
      BP.ParametersOnS1(U, V);
      BE.SetValue(BP.PointOnS1(), U, V, Precision::Confusion());
      Line->SetStartPoints(BE, Line->StartPointOnSecond());
    }
    if (Line->StartPointOnSecond().ParameterOnGuide() < tguide)
    {
      BRepBlend_Extremity BE;
      BP.ParametersOnS2(U, V);
      BE.SetValue(BP.PointOnS2(), U, V, Precision::Confusion());
      Line->SetStartPoints(Line->StartPointOnFirst(), BE);
    }
  }
  else
  {
    const Blend_Point& BP = Line->Point(Line->NbPoints());
    tguide                = BP.Parameter();
    if (Line->EndPointOnFirst().ParameterOnGuide() > tguide)
    {
      BRepBlend_Extremity BE;
      BP.ParametersOnS1(U, V);
      BE.SetValue(BP.PointOnS1(), U, V, Precision::Confusion());
      Line->SetEndPoints(BE, Line->EndPointOnSecond());
    }
    if (Line->EndPointOnSecond().ParameterOnGuide() > tguide)
    {
      BRepBlend_Extremity BE;
      BP.ParametersOnS2(U, V);
      BE.SetValue(BP.PointOnS2(), U, V, Precision::Confusion());
      Line->SetEndPoints(Line->EndPointOnFirst(), BE);
    }
  }
}

//=======================================================================
// function : CompleteData
// purpose  : Calculates curves and CommonPoints from the data
//           calculated by filling.
//=======================================================================

bool ChFi3d_Builder::CompleteData(occ::handle<ChFiDS_SurfData>&         Data,
                                  const occ::handle<Geom_Surface>&      Surfcoin,
                                  const occ::handle<Adaptor3d_Surface>& S1,
                                  const occ::handle<Geom2d_Curve>&      PC1,
                                  const occ::handle<Adaptor3d_Surface>& S2,
                                  const occ::handle<Geom2d_Curve>&      PC2,
                                  const TopAbs_Orientation              Or,
                                  const bool                            On1,
                                  const bool                            Gd1,
                                  const bool                            Gd2,
                                  const bool                            Gf1,
                                  const bool                            Gf2)
{
  TopOpeBRepDS_DataStructure& DStr = myDS->ChangeDS();
  Data->ChangeSurf(DStr.AddSurface(TopOpeBRepDS_Surface(Surfcoin, tolapp3d)));
#ifdef DRAW
  ChFi3d_SettraceDRAWFIL(true);
  if (ChFi3d_GettraceDRAWFIL())
  {
    IndexOfConge++;
    //    char name[100];
    char* name = new char[100];
    Sprintf(name, "%s_%d", "Surf", IndexOfConge);
    DrawTrSurf::Set(name, Surfcoin);
  }
#endif

  double UFirst, ULast, VFirst, VLast;
  Surfcoin->Bounds(UFirst, ULast, VFirst, VLast);
  if (!Gd1)
    Data->ChangeVertexFirstOnS1().SetPoint(Surfcoin->Value(UFirst, VFirst));
  if (!Gd2)
    Data->ChangeVertexFirstOnS2().SetPoint(Surfcoin->Value(UFirst, VLast));
  if (!Gf1)
    Data->ChangeVertexLastOnS1().SetPoint(Surfcoin->Value(ULast, VFirst));
  if (!Gf2)
    Data->ChangeVertexLastOnS2().SetPoint(Surfcoin->Value(ULast, VLast));

  // calculate curves side S1
  occ::handle<Geom_Curve> Crv3d1;
  if (!PC1.IsNull())
    Crv3d1 = Surfcoin->VIso(VFirst);
  gp_Pnt2d                  pd1(UFirst, VFirst), pf1(ULast, VFirst);
  gp_Lin2d                  lfil1(pd1, gp_Dir2d(gp_Vec2d(pd1, pf1)));
  occ::handle<Geom2d_Curve> PCurveOnSurf = new Geom2d_Line(lfil1);
  TopAbs_Orientation        tra1 = TopAbs_FORWARD, orsurf = Or;
  double                    x, y, w = 0.5 * (UFirst + ULast);
  gp_Pnt                    p;
  gp_Vec                    du, dv;
  occ::handle<Geom2d_Curve> c2dtrim;
  double                    tolreached = 1.e-5;
  if (!PC1.IsNull())
  {
    occ::handle<GeomAdaptor_Curve> hcS1 = new GeomAdaptor_Curve(Crv3d1);
    c2dtrim                             = new Geom2d_TrimmedCurve(PC1, UFirst, ULast);
    ChFi3d_SameParameter(hcS1, c2dtrim, S1, tolapp3d, tolreached);
    c2dtrim->Value(w).Coord(x, y);
    S1->D1(x, y, p, du, dv);
    gp_Vec nf = du.Crossed(dv);
    Surfcoin->D1(w, VFirst, p, du, dv);
    gp_Vec ns = du.Crossed(dv);
    if (nf.Dot(ns) > 0.)
      tra1 = TopAbs_REVERSED;
    else if (On1)
      orsurf = TopAbs::Reverse(orsurf);
  }
  int                      Index1OfCurve = DStr.AddCurve(TopOpeBRepDS_Curve(Crv3d1, tolreached));
  ChFiDS_FaceInterference& Fint1         = Data->ChangeInterferenceOnS1();
  Fint1.SetFirstParameter(UFirst);
  Fint1.SetLastParameter(ULast);
  Fint1.SetInterference(Index1OfCurve, tra1, c2dtrim, PCurveOnSurf);
  // calculate curves side S2
  occ::handle<Geom_Curve> Crv3d2;
  if (!PC2.IsNull())
    Crv3d2 = Surfcoin->VIso(VLast);
  gp_Pnt2d pd2(UFirst, VLast), pf2(ULast, VLast);
  gp_Lin2d lfil2(pd2, gp_Dir2d(gp_Vec2d(pd2, pf2)));
  PCurveOnSurf            = new Geom2d_Line(lfil2);
  TopAbs_Orientation tra2 = TopAbs_FORWARD;
  if (!PC2.IsNull())
  {
    occ::handle<GeomAdaptor_Curve> hcS2 = new GeomAdaptor_Curve(Crv3d2);
    c2dtrim                             = new Geom2d_TrimmedCurve(PC2, UFirst, ULast);
    ChFi3d_SameParameter(hcS2, c2dtrim, S2, tolapp3d, tolreached);
    c2dtrim->Value(w).Coord(x, y);
    S2->D1(x, y, p, du, dv);
    gp_Vec np = du.Crossed(dv);
    Surfcoin->D1(w, VLast, p, du, dv);
    gp_Vec ns = du.Crossed(dv);
    if (np.Dot(ns) < 0.)
    {
      tra2 = TopAbs_REVERSED;
      if (!On1)
        orsurf = TopAbs::Reverse(orsurf);
    }
  }
  int                      Index2OfCurve = DStr.AddCurve(TopOpeBRepDS_Curve(Crv3d2, tolreached));
  ChFiDS_FaceInterference& Fint2         = Data->ChangeInterferenceOnS2();
  Fint2.SetFirstParameter(UFirst);
  Fint2.SetLastParameter(ULast);
  Fint2.SetInterference(Index2OfCurve, tra2, c2dtrim, PCurveOnSurf);
  Data->ChangeOrientation() = orsurf;
  return true;
}

//=======================================================================
// function : CompleteData
// purpose  : Calculates the surface of curves and eventually
//           CommonPoints from the data calculated in ComputeData.
//
//  11/08/1996 : Use of F(t)
//
//=======================================================================

bool ChFi3d_Builder::CompleteData(occ::handle<ChFiDS_SurfData>&         Data,
                                  Blend_Function&                       Func,
                                  occ::handle<BRepBlend_Line>&          lin,
                                  const occ::handle<Adaptor3d_Surface>& S1,
                                  const occ::handle<Adaptor3d_Surface>& S2,
                                  const TopAbs_Orientation              Or1,
                                  const bool                            Gd1,
                                  const bool                            Gd2,
                                  const bool                            Gf1,
                                  const bool                            Gf2,
                                  const bool                            Reversed)
{
  occ::handle<BRepBlend_AppFunc> TheFunc = new (BRepBlend_AppFunc)(lin, Func, tolapp3d, 1.e-5);

  int                  Degmax = 20, Segmax = 5000;
  BRepBlend_AppSurface approx(TheFunc,
                              lin->Point(1).Parameter(),
                              lin->Point(lin->NbPoints()).Parameter(),
                              tolapp3d,
                              1.e-5,       // tolapp2d, tolerance max
                              tolappangle, // Contact G1
                              myConti,
                              Degmax,
                              Segmax);
  if (!approx.IsDone())
  {
#ifdef OCCT_DEBUG
    std::cout << "Approximation non faite !!!" << std::endl;
#endif
    return false;
  }
#ifdef OCCT_DEBUG
  approx.Dump(std::cout);
#endif
  return StoreData(Data, approx, lin, S1, S2, Or1, Gd1, Gd2, Gf1, Gf2, Reversed);
}

//=======================================================================
// function : CompleteData
// purpose  : New overload for functions surf/rst
// jlr le 28/07/97 branchement F(t)
//=======================================================================

bool ChFi3d_Builder::CompleteData(occ::handle<ChFiDS_SurfData>&         Data,
                                  Blend_SurfRstFunction&                Func,
                                  occ::handle<BRepBlend_Line>&          lin,
                                  const occ::handle<Adaptor3d_Surface>& S1,
                                  const occ::handle<Adaptor3d_Surface>& S2,
                                  const TopAbs_Orientation              Or,
                                  const bool                            Reversed)
{
  occ::handle<BRepBlend_AppFuncRst> TheFunc =
    new (BRepBlend_AppFuncRst)(lin, Func, tolapp3d, 1.e-5);
  BRepBlend_AppSurface approx(TheFunc,
                              lin->Point(1).Parameter(),
                              lin->Point(lin->NbPoints()).Parameter(),
                              tolapp3d,
                              1.e-5,       // tolapp2d, tolerance max
                              tolappangle, // Contact G1
                              myConti);
  if (!approx.IsDone())
  {
#ifdef OCCT_DEBUG
    std::cout << "Approximation is not done!" << std::endl;
#endif
    return false;
  }
#ifdef OCCT_DEBUG
  approx.Dump(std::cout);
#endif

  return StoreData(Data, approx, lin, S1, S2, Or, false, false, false, false, Reversed);
}

//=======================================================================
// function : CompleteData
// purpose  : New overload for functions rst/rst
// jlr le 28/07/97 branchement F(t)
//=======================================================================

bool ChFi3d_Builder::CompleteData(occ::handle<ChFiDS_SurfData>&         Data,
                                  Blend_RstRstFunction&                 Func,
                                  occ::handle<BRepBlend_Line>&          lin,
                                  const occ::handle<Adaptor3d_Surface>& S1,
                                  const occ::handle<Adaptor3d_Surface>& S2,
                                  const TopAbs_Orientation              Or)
{
  occ::handle<BRepBlend_AppFuncRstRst> TheFunc =
    new (BRepBlend_AppFuncRstRst)(lin, Func, tolapp3d, 1.e-5);
  BRepBlend_AppSurface approx(TheFunc,
                              lin->Point(1).Parameter(),
                              lin->Point(lin->NbPoints()).Parameter(),
                              tolapp3d,
                              1.e-5,       // tolapp2d, tolerance max
                              tolappangle, // Contact G1
                              myConti);
  if (!approx.IsDone())
  {
#ifdef OCCT_DEBUG
    std::cout << "Approximation non faite !!!" << std::endl;
#endif
    return false;
  }
#ifdef OCCT_DEBUG
  approx.Dump(std::cout);
#endif

  return StoreData(Data, approx, lin, S1, S2, Or, false, false, false, false);
}

//=======================================================================
// function : StoreData
// purpose  : Copy of an approximation result in SurfData.
//=======================================================================

bool ChFi3d_Builder::StoreData(occ::handle<ChFiDS_SurfData>&         Data,
                               const AppBlend_Approx&                approx,
                               const occ::handle<BRepBlend_Line>&    lin,
                               const occ::handle<Adaptor3d_Surface>& S1,
                               const occ::handle<Adaptor3d_Surface>& S2,
                               const TopAbs_Orientation              Or1,
                               const bool                            Gd1,
                               const bool                            Gd2,
                               const bool                            Gf1,
                               const bool                            Gf2,
                               const bool                            Reversed)
{
  // Small control tools.
  static occ::handle<GeomAdaptor_Curve> checkcurve;
  if (checkcurve.IsNull())
    checkcurve = new GeomAdaptor_Curve();
  GeomAdaptor_Curve& chc = *checkcurve;
  double             tolget3d, tolget2d, tolaux, tolC1, tolcheck;
  double             tolC2 = 0.;
  approx.TolReached(tolget3d, tolget2d);
  tolaux = approx.TolCurveOnSurf(1);
  tolC1  = tolget3d + tolaux;
  if (!S2.IsNull())
  {
    tolaux = approx.TolCurveOnSurf(2);
    tolC2  = tolget3d + tolaux;
  }

  TopOpeBRepDS_DataStructure& DStr = myDS->ChangeDS();
  // By default parametric space is created using a square surface
  // to be able to parameterize in U by # R*teta // a revoir lbo 29/08/97
  const NCollection_Array1<double>& ku   = approx.SurfUKnots();
  const NCollection_Array1<double>& kv   = approx.SurfVKnots();
  double                            larg = (kv(kv.Upper()) - kv(kv.Lower()));
  NCollection_Array1<double>&       kku  = *((NCollection_Array1<double>*)((void*)&ku));
  BSplCLib::Reparametrize(0., larg, kku);
  occ::handle<Geom_BSplineSurface> Surf = new Geom_BSplineSurface(approx.SurfPoles(),
                                                                  approx.SurfWeights(),
                                                                  kku,
                                                                  kv,
                                                                  approx.SurfUMults(),
                                                                  approx.SurfVMults(),
                                                                  approx.UDegree(),
                                                                  approx.VDegree());
  // extension of the surface

  double length1, length2;
  length1 = Data->FirstExtensionValue();
  length2 = Data->LastExtensionValue();

  occ::handle<Geom_BoundedSurface> aBndSurf = Surf;
  bool                             ext1 = false, ext2 = false;
  double                           eps = std::max(tolget3d, 2. * Precision::Confusion());
  if (length1 > eps)
  {
    gp_Pnt P11, P21;
    P11 = Surf->Pole(1, 1);
    P21 = Surf->Pole(Surf->NbUPoles(), 1);
    if (P11.Distance(P21) > eps)
    {
      // to avoid extending surface with singular boundary
      GeomLib::ExtendSurfByLength(aBndSurf, length1, 1, false, false);
      ext1 = true;
    }
  }
  if (length2 > eps)
  {
    gp_Pnt P12, P22;
    P12 = Surf->Pole(1, Surf->NbVPoles());
    P22 = Surf->Pole(Surf->NbUPoles(), Surf->NbVPoles());
    if (P12.Distance(P22) > eps)
    {
      // to avoid extending surface with singular boundary
      GeomLib::ExtendSurfByLength(aBndSurf, length2, 1, false, true);
      ext2 = true;
    }
  }
  Surf = occ::down_cast<Geom_BSplineSurface>(aBndSurf);

  // Correction of surface on extremities
  if (!ext1)
  {
    gp_Pnt P11, P21;
    P11 = lin->StartPointOnFirst().Value();
    P21 = lin->StartPointOnSecond().Value();
    Surf->SetPole(1, 1, P11);
    Surf->SetPole(Surf->NbUPoles(), 1, P21);
  }
  if (!ext2)
  {
    gp_Pnt P12, P22;
    P12 = lin->EndPointOnFirst().Value();
    P22 = lin->EndPointOnSecond().Value();
    Surf->SetPole(1, Surf->NbVPoles(), P12);
    Surf->SetPole(Surf->NbUPoles(), Surf->NbVPoles(), P22);
  }

  Data->ChangeSurf(DStr.AddSurface(TopOpeBRepDS_Surface(Surf, tolget3d)));

#ifdef DRAW
  ChFi3d_SettraceDRAWFIL(true);
  if (ChFi3d_GettraceDRAWFIL())
  {
    IndexOfConge++;
    //    char name[100];
    char* name = new char[100];
    Sprintf(name, "%s_%d", "Surf", IndexOfConge);
    DrawTrSurf::Set(name, Surf);
  }
#endif
  double UFirst, ULast, VFirst, VLast, pppdeb, pppfin;
  Surf->Bounds(UFirst, ULast, VFirst, VLast);
  BRepAdaptor_Curve2d              brc;
  BRepAdaptor_Curve                CArc;
  occ::handle<BRepAdaptor_Surface> BS1 = occ::down_cast<BRepAdaptor_Surface>(S1);
  occ::handle<BRepAdaptor_Surface> BS2 = occ::down_cast<BRepAdaptor_Surface>(S2);
  Geom2dAPI_ProjectPointOnCurve    projector;

  double Uon1 = UFirst, Uon2 = ULast;
  int    ion1 = 1, ion2 = 2;
  if (Reversed)
  {
    Uon1 = ULast;
    Uon2 = UFirst;
    ion1 = 2;
    ion2 = 1;
  }

  // The SurfData is filled in what concerns S1,
  occ::handle<Geom_Curve>   Crv3d1 = Surf->UIso(Uon1);
  gp_Pnt2d                  pori1(Uon1, 0.);
  gp_Lin2d                  lfil1(pori1, gp::DY2d());
  occ::handle<Geom2d_Curve> PCurveOnSurf = new Geom2d_Line(lfil1);
  occ::handle<Geom2d_Curve> PCurveOnFace;
  PCurveOnFace = new Geom2d_BSplineCurve(approx.Curve2dPoles(ion1),
                                         approx.Curves2dKnots(),
                                         approx.Curves2dMults(),
                                         approx.Curves2dDegree());

  double par1 = PCurveOnFace->FirstParameter();
  double par2 = PCurveOnFace->LastParameter();
  chc.Load(Crv3d1, par1, par2);

  if (!ChFi3d_CheckSameParameter(checkcurve, PCurveOnFace, S1, tolC1, tolcheck))
  {
#ifdef OCCT_DEBUG
    std::cout << "approximate tolerance under-valued : " << tolC1 << " for " << tolcheck
              << std::endl;
#endif
    tolC1 = tolcheck;
  }
  int Index1OfCurve = DStr.AddCurve(TopOpeBRepDS_Curve(Crv3d1, tolC1));

  double uarc, utg;
  if (Gd1)
  {
    TopoDS_Face forwfac = BS1->Face();
    forwfac.Orientation(TopAbs_FORWARD);
    brc.Initialize(Data->VertexFirstOnS1().Arc(), forwfac);
    ChFiDS_CommonPoint& V = Data->ChangeVertexFirstOnS1();
    CArc.Initialize(V.Arc());
    CompParam(brc, PCurveOnFace, uarc, utg, V.ParameterOnArc(), V.Parameter());
    tolcheck = CArc.Value(uarc).Distance(V.Point());
    V.SetArc(tolC1 + tolcheck, V.Arc(), uarc, V.TransitionOnArc());
    pppdeb = utg;
  }
  else
    pppdeb = VFirst;
  if (Gf1)
  {
    TopoDS_Face forwfac = BS1->Face();
    forwfac.Orientation(TopAbs_FORWARD);
    ChFiDS_CommonPoint& V = Data->ChangeVertexLastOnS1();
    brc.Initialize(V.Arc(), forwfac);
    CArc.Initialize(V.Arc());
    CompParam(brc, PCurveOnFace, uarc, utg, V.ParameterOnArc(), V.Parameter());
    tolcheck = CArc.Value(uarc).Distance(V.Point());
    V.SetArc(tolC1 + tolcheck, V.Arc(), uarc, V.TransitionOnArc());
    pppfin = utg;
  }
  else
    pppfin = VLast;
  ChFiDS_FaceInterference& Fint1 = Data->ChangeInterferenceOnS1();
  Fint1.SetFirstParameter(pppdeb);
  Fint1.SetLastParameter(pppfin);
  TopAbs_Orientation TraOn1;
  if (Reversed)
    TraOn1 = ChFi3d_TrsfTrans(lin->TransitionOnS2());
  else
    TraOn1 = ChFi3d_TrsfTrans(lin->TransitionOnS1());
  Fint1.SetInterference(Index1OfCurve, TraOn1, PCurveOnFace, PCurveOnSurf);

  // SurfData is filled in what concerns S2,
  occ::handle<Geom_Curve> Crv3d2 = Surf->UIso(Uon2);
  gp_Pnt2d                pori2(Uon2, 0.);
  gp_Lin2d                lfil2(pori2, gp::DY2d());
  PCurveOnSurf = new Geom2d_Line(lfil2);
  if (!S2.IsNull())
  {
    PCurveOnFace = new Geom2d_BSplineCurve(approx.Curve2dPoles(ion2),
                                           approx.Curves2dKnots(),
                                           approx.Curves2dMults(),
                                           approx.Curves2dDegree());
    chc.Load(Crv3d2, par1, par2);
    if (!ChFi3d_CheckSameParameter(checkcurve, PCurveOnFace, S2, tolC2, tolcheck))
    {
#ifdef OCCT_DEBUG
      std::cout << "approximate tolerance under-evaluated : " << tolC2 << " for " << tolcheck
                << std::endl;
#endif
      tolC2 = tolcheck;
    }
  }
  int Index2OfCurve = DStr.AddCurve(TopOpeBRepDS_Curve(Crv3d2, tolC2));
  if (Gd2)
  {
    TopoDS_Face forwfac = BS2->Face();
    forwfac.Orientation(TopAbs_FORWARD);
    brc.Initialize(Data->VertexFirstOnS2().Arc(), forwfac);
    ChFiDS_CommonPoint& V = Data->ChangeVertexFirstOnS2();
    CArc.Initialize(V.Arc());
    CompParam(brc, PCurveOnFace, uarc, utg, V.ParameterOnArc(), V.Parameter());
    tolcheck = CArc.Value(uarc).Distance(V.Point());
    V.SetArc(tolC2 + tolcheck, V.Arc(), uarc, V.TransitionOnArc());
    pppdeb = utg;
  }
  else
    pppdeb = VFirst;
  if (Gf2)
  {
    TopoDS_Face forwfac = BS2->Face();
    forwfac.Orientation(TopAbs_FORWARD);
    brc.Initialize(Data->VertexLastOnS2().Arc(), forwfac);
    ChFiDS_CommonPoint& V = Data->ChangeVertexLastOnS2();
    CArc.Initialize(V.Arc());
    CompParam(brc, PCurveOnFace, uarc, utg, V.ParameterOnArc(), V.Parameter());
    tolcheck = CArc.Value(uarc).Distance(V.Point());
    V.SetArc(tolC2 + tolcheck, V.Arc(), uarc, V.TransitionOnArc());
    pppfin = utg;
  }
  else
    pppfin = VLast;
  ChFiDS_FaceInterference& Fint2 = Data->ChangeInterferenceOnS2();
  Fint2.SetFirstParameter(pppdeb);
  Fint2.SetLastParameter(pppfin);
  if (!S2.IsNull())
  {
    TopAbs_Orientation TraOn2;
    if (Reversed)
      TraOn2 = ChFi3d_TrsfTrans(lin->TransitionOnS1());
    else
      TraOn2 = ChFi3d_TrsfTrans(lin->TransitionOnS2());
    Fint2.SetInterference(Index2OfCurve, TraOn2, PCurveOnFace, PCurveOnSurf);
  }
  else
  {
    occ::handle<Geom2d_Curve> bidpc;
    Fint2.SetInterference(Index2OfCurve, TopAbs_FORWARD, bidpc, PCurveOnSurf);
  }

  // the orientation of the fillet in relation to the faces is evaluated,

  occ::handle<Adaptor3d_Surface> Sref = S1;
  PCurveOnFace                        = Fint1.PCurveOnFace();
  if (Reversed)
  {
    Sref         = S2;
    PCurveOnFace = Fint2.PCurveOnFace();
  }

  //  Modified by skv - Wed Jun  9 17:16:26 2004 OCC5898 Begin
  //   gp_Pnt2d PUV = PCurveOnFace->Value((VFirst+VLast)/2.);
  //   gp_Pnt P;
  //   gp_Vec Du1,Du2,Dv1,Dv2;
  //   Sref->D1(PUV.X(),PUV.Y(),P,Du1,Dv1);
  //   Du1.Cross(Dv1);
  //   if (Or1 == TopAbs_REVERSED) Du1.Reverse();
  //   Surf->D1(UFirst,(VFirst+VLast)/2.,P,Du2,Dv2);
  //   Du2.Cross(Dv2);
  //   if (Du1.Dot(Du2)>0) Data->ChangeOrientation() = TopAbs_FORWARD;
  //   else Data->ChangeOrientation() = TopAbs_REVERSED;

  double aDelta = VLast - VFirst;
  int    aDenom = 2;

  for (;;)
  {
    double   aDeltav = aDelta / aDenom;
    double   aParam  = VFirst + aDeltav;
    gp_Pnt2d PUV     = PCurveOnFace->Value(aParam);
    gp_Pnt   P;
    gp_Vec   Du1, Du2, Dv1, Dv2;

    Sref->D1(PUV.X(), PUV.Y(), P, Du1, Dv1);
    Du1.Cross(Dv1);

    if (Or1 == TopAbs_REVERSED)
      Du1.Reverse();

    Surf->D1(UFirst, aParam, P, Du2, Dv2);
    Du2.Cross(Dv2);

    if (Du1.Magnitude() <= tolget3d || Du2.Magnitude() <= tolget3d)
    {
      aDenom++;

      if (std::abs(aDeltav) <= tolget2d)
        return false;

      continue;
    }

    if (Du1.Dot(Du2) > 0)
      Data->ChangeOrientation() = TopAbs_FORWARD;
    else
      Data->ChangeOrientation() = TopAbs_REVERSED;

    break;
  }
  //  Modified by skv - Wed Jun  9 17:16:26 2004 OCC5898 End

  if (!Gd1 && !S1.IsNull())
    ChFi3d_FilCommonPoint(lin->StartPointOnFirst(),
                          lin->TransitionOnS1(),
                          true,
                          Data->ChangeVertex(true, ion1),
                          tolC1);
  if (!Gf1 && !S1.IsNull())
    ChFi3d_FilCommonPoint(lin->EndPointOnFirst(),
                          lin->TransitionOnS1(),
                          false,
                          Data->ChangeVertex(false, ion1),
                          tolC1);
  if (!Gd2 && !S2.IsNull())
    ChFi3d_FilCommonPoint(lin->StartPointOnSecond(),
                          lin->TransitionOnS2(),
                          true,
                          Data->ChangeVertex(true, ion2),
                          tolC2);
  if (!Gf2 && !S2.IsNull())
    ChFi3d_FilCommonPoint(lin->EndPointOnSecond(),
                          lin->TransitionOnS2(),
                          false,
                          Data->ChangeVertex(false, ion2),
                          tolC2);
  // Parameters on ElSpine
  int nbp = lin->NbPoints();
  Data->FirstSpineParam(lin->Point(1).Parameter());
  Data->LastSpineParam(lin->Point(nbp).Parameter());
  return true;
}

//=======================================================================
// function : ComputeData
// purpose  : Head of the path edge/face for the bypass of obstacle.
//=======================================================================

bool ChFi3d_Builder::ComputeData(occ::handle<ChFiDS_SurfData>&           Data,
                                 const occ::handle<ChFiDS_ElSpine>&      HGuide,
                                 occ::handle<BRepBlend_Line>&            Lin,
                                 const occ::handle<Adaptor3d_Surface>&   S1,
                                 const occ::handle<Adaptor3d_TopolTool>& I1,
                                 const occ::handle<Adaptor3d_Surface>&   S2,
                                 const occ::handle<Adaptor2d_Curve2d>&   PC2,
                                 const occ::handle<Adaptor3d_TopolTool>& I2,
                                 bool&                                   Decroch,
                                 Blend_SurfRstFunction&                  Func,
                                 Blend_FuncInv&                          FInv,
                                 Blend_SurfPointFuncInv&                 FInvP,
                                 Blend_SurfCurvFuncInv&                  FInvC,
                                 const double                            PFirst,
                                 const double                            MaxStep,
                                 const double                            Fleche,
                                 const double                            TolGuide,
                                 double&                                 First,
                                 double&                                 Last,
                                 const math_Vector&                      Soldep,
                                 const bool                              Inside,
                                 const bool                              Appro,
                                 const bool                              Forward,
                                 const bool                              RecP,
                                 const bool                              RecS,
                                 const bool                              RecRst)
{
  BRepBlend_SurfRstLineBuilder TheWalk(S1, I1, S2, PC2, I2);

  Data->FirstExtensionValue(0);
  Data->LastExtensionValue(0);

  bool   reverse = (!Forward || Inside);
  double SpFirst = HGuide->FirstParameter();
  double SpLast  = HGuide->LastParameter();
  double Target  = SpLast;
  if (reverse)
    Target = SpFirst;
  double Targetsov = Target;

  double MS      = MaxStep;
  int    again   = 0;
  int    nbptmin = 3; // jlr
  int    Nbpnt   = 1;
  // the initial solution is reframed if necessary.
  math_Vector ParSol(1, 3);
  double      NewFirst = PFirst;
  if (RecP || RecS || RecRst)
  {
    if (!TheWalk.PerformFirstSection(Func,
                                     FInv,
                                     FInvP,
                                     FInvC,
                                     PFirst,
                                     Target,
                                     Soldep,
                                     tolapp3d,
                                     tolapp2d,
                                     TolGuide,
                                     RecRst,
                                     RecP,
                                     RecS,
                                     NewFirst,
                                     ParSol))
    {
#ifdef OCCT_DEBUG
      std::cout << "ChFi3d_Builder::ComputeData : calculation fail first section" << std::endl;
#endif
      return false;
    }
  }
  else
  {
    ParSol = Soldep;
  }

  while (again < 2)
  {
    TheWalk.Perform(Func,
                    FInv,
                    FInvP,
                    FInvC,
                    NewFirst,
                    Last,
                    MS,
                    tolapp3d,
                    tolapp2d,
                    TolGuide,
                    ParSol,
                    Fleche,
                    Appro);

    if (!TheWalk.IsDone())
    {
#ifdef OCCT_DEBUG
      std::cout << "Path not created" << std::endl;
#endif
      return false;
    }

    if (reverse)
    {
      if (!TheWalk.Complete(Func, FInv, FInvP, FInvC, SpLast))
      {
#ifdef OCCT_DEBUG
        std::cout << "Not completed" << std::endl;
#endif
      }
    }

    Lin   = TheWalk.Line();
    Nbpnt = Lin->NbPoints();
    if (Nbpnt <= 1 && again == 0)
    {
      again++;
#ifdef OCCT_DEBUG
      std::cout << "one point of the path MS/50 is attempted." << std::endl;
#endif
      MS     = MS / 50.;
      Target = Targetsov;
    }
    else if (Nbpnt <= nbptmin && again == 0)
    {
      again++;
#ifdef OCCT_DEBUG
      std::cout << "Number of points is too small, the step is reduced" << std::endl;
#endif
      double u1 = Lin->Point(1).Parameter();
      double u2 = Lin->Point(Nbpnt).Parameter();
      MS        = (u2 - u1) / (nbptmin + 1.0);
      //      std::cout << " MS : " << MS << " u1 : " << u1 << " u2 : " << u2 << " nbptmin : " <<
      //      nbptmin << std::endl;
      Target = Targetsov;
    }
    else if (Nbpnt <= nbptmin)
    {
#ifdef OCCT_DEBUG
      std::cout << "Number of points is still too small, quit" << std::endl;
#endif
      return false;
    }
    else
    {
      again = 2;
    }
  }
#ifdef DRAW
  ChFi3d_SettraceDRAWWALK(true);
  if (ChFi3d_GettraceDRAWWALK())
    drawline(Lin, true);
#endif
  if (Forward)
    Decroch = TheWalk.DecrochEnd();
  else
    Decroch = TheWalk.DecrochStart();
  Last  = Lin->Point(Nbpnt).Parameter();
  First = Lin->Point(1).Parameter();
  return true;
}

//=======================================================================
// function : ComputeData
// purpose  : Heading of the path edge/edge for the bypass of obstacle.
//=======================================================================

bool ChFi3d_Builder::ComputeData(occ::handle<ChFiDS_SurfData>&           Data,
                                 const occ::handle<ChFiDS_ElSpine>&      HGuide,
                                 occ::handle<BRepBlend_Line>&            Lin,
                                 const occ::handle<Adaptor3d_Surface>&   S1,
                                 const occ::handle<Adaptor2d_Curve2d>&   PC1,
                                 const occ::handle<Adaptor3d_TopolTool>& I1,
                                 bool&                                   Decroch1,
                                 const occ::handle<Adaptor3d_Surface>&   S2,
                                 const occ::handle<Adaptor2d_Curve2d>&   PC2,
                                 const occ::handle<Adaptor3d_TopolTool>& I2,
                                 bool&                                   Decroch2,
                                 Blend_RstRstFunction&                   Func,
                                 Blend_SurfCurvFuncInv&                  FInv1,
                                 Blend_CurvPointFuncInv&                 FInvP1,
                                 Blend_SurfCurvFuncInv&                  FInv2,
                                 Blend_CurvPointFuncInv&                 FInvP2,
                                 const double                            PFirst,
                                 const double                            MaxStep,
                                 const double                            Fleche,
                                 const double                            TolGuide,
                                 double&                                 First,
                                 double&                                 Last,
                                 const math_Vector&                      Soldep,
                                 const bool                              Inside,
                                 const bool                              Appro,
                                 const bool                              Forward,
                                 const bool                              RecP1,
                                 const bool                              RecRst1,
                                 const bool                              RecP2,
                                 const bool                              RecRst2)
{
  BRepBlend_RstRstLineBuilder TheWalk(S1, PC1, I1, S2, PC2, I2);

  Data->FirstExtensionValue(0);
  Data->LastExtensionValue(0);

  bool   reverse = (!Forward || Inside);
  double SpFirst = HGuide->FirstParameter();
  double SpLast  = HGuide->LastParameter();
  double Target  = SpLast;
  if (reverse)
    Target = SpFirst;
  double Targetsov = Target;

  double MS      = MaxStep;
  int    again   = 0;
  int    nbptmin = 3; // jlr
  int    Nbpnt   = 0;
  // the initial solution is reframed if necessary.
  math_Vector ParSol(1, 2);
  double      NewFirst = PFirst;
  if (RecP1 || RecRst1 || RecP2 || RecRst2)
  {
    if (!TheWalk.PerformFirstSection(Func,
                                     FInv1,
                                     FInvP1,
                                     FInv2,
                                     FInvP2,
                                     PFirst,
                                     Target,
                                     Soldep,
                                     tolapp3d,
                                     TolGuide,
                                     RecRst1,
                                     RecP1,
                                     RecRst2,
                                     RecP2,
                                     NewFirst,
                                     ParSol))
    {
#ifdef OCCT_DEBUG
      std::cout << "ChFi3d_Builder::ComputeData : fail calculation first section" << std::endl;
#endif
      return false;
    }
  }
  else
  {
    ParSol = Soldep;
  }

  while (again < 2)
  {
    TheWalk.Perform(Func,
                    FInv1,
                    FInvP1,
                    FInv2,
                    FInvP2,
                    NewFirst,
                    Last,
                    MS,
                    tolapp3d,
                    TolGuide,
                    ParSol,
                    Fleche,
                    Appro);

    if (!TheWalk.IsDone())
    {
#ifdef OCCT_DEBUG
      std::cout << "Path not done" << std::endl;
#endif
      return false;
    }

    if (reverse)
    {
      if (!TheWalk.Complete(Func, FInv1, FInvP1, FInv2, FInvP2, SpLast))
      {
#ifdef OCCT_DEBUG
        std::cout << "Not completed" << std::endl;
#endif
      }
    }

    Lin   = TheWalk.Line();
    Nbpnt = Lin->NbPoints();
    if (Nbpnt <= 1 && again == 0)
    {
      again++;
#ifdef OCCT_DEBUG
      std::cout << "one point of path MS/50 is attempted." << std::endl;
#endif
      MS     = MS / 50.;
      Target = Targetsov;
    }
    else if (Nbpnt <= nbptmin && again == 0)
    {
      again++;
#ifdef OCCT_DEBUG
      std::cout << "Number of points is too small, the step is reduced" << std::endl;
#endif
      double u1 = Lin->Point(1).Parameter();
      double u2 = Lin->Point(Nbpnt).Parameter();
      MS        = (u2 - u1) / (nbptmin + 1);
      Target    = Targetsov;
    }
    else if (Nbpnt <= nbptmin)
    {
#ifdef OCCT_DEBUG
      std::cout << "Number of points is still too small, quit" << std::endl;
#endif
      return false;
    }
    else
    {
      again = 2;
    }
  }
#ifdef DRAW
  ChFi3d_SettraceDRAWWALK(true);
  if (ChFi3d_GettraceDRAWWALK())
    drawline(Lin, true);
#endif
  if (Forward)
  {
    Decroch1 = TheWalk.Decroch1End();
    Decroch2 = TheWalk.Decroch2End();
  }
  else
  {
    Decroch1 = TheWalk.Decroch1Start();
    Decroch2 = TheWalk.Decroch2Start();
  }
  Last  = Lin->Point(Nbpnt).Parameter();
  First = Lin->Point(1).Parameter();
  return true;
}

//=======================================================================
// function : SimulData
// purpose  : Heading of the path edge/face for the bypass of obstacle in simulation mode.
//=======================================================================

bool ChFi3d_Builder::SimulData(occ::handle<ChFiDS_SurfData>& /*Data*/,
                               const occ::handle<ChFiDS_ElSpine>&      HGuide,
                               occ::handle<BRepBlend_Line>&            Lin,
                               const occ::handle<Adaptor3d_Surface>&   S1,
                               const occ::handle<Adaptor3d_TopolTool>& I1,
                               const occ::handle<Adaptor3d_Surface>&   S2,
                               const occ::handle<Adaptor2d_Curve2d>&   PC2,
                               const occ::handle<Adaptor3d_TopolTool>& I2,
                               bool&                                   Decroch,
                               Blend_SurfRstFunction&                  Func,
                               Blend_FuncInv&                          FInv,
                               Blend_SurfPointFuncInv&                 FInvP,
                               Blend_SurfCurvFuncInv&                  FInvC,
                               const double                            PFirst,
                               const double                            MaxStep,
                               const double                            Fleche,
                               const double                            TolGuide,
                               double&                                 First,
                               double&                                 Last,
                               const math_Vector&                      Soldep,
                               const int                               NbSecMin,
                               const bool                              Inside,
                               const bool                              Appro,
                               const bool                              Forward,
                               const bool                              RecP,
                               const bool                              RecS,
                               const bool                              RecRst)
{
  BRepBlend_SurfRstLineBuilder TheWalk(S1, I1, S2, PC2, I2);

  bool   reverse = (!Forward || Inside);
  double SpFirst = HGuide->FirstParameter();
  double SpLast  = HGuide->LastParameter();
  double Target  = SpLast;
  if (reverse)
    Target = SpFirst;
  double Targetsov = Target;

  double MS    = MaxStep;
  int    again = 0;
  int    Nbpnt = 0;
  // the starting solution is reframed if needed.
  math_Vector ParSol(1, 3);
  double      NewFirst = PFirst;
  if (RecP || RecS || RecRst)
  {
    if (!TheWalk.PerformFirstSection(Func,
                                     FInv,
                                     FInvP,
                                     FInvC,
                                     PFirst,
                                     Target,
                                     Soldep,
                                     tolapp3d,
                                     tolapp2d,
                                     TolGuide,
                                     RecRst,
                                     RecP,
                                     RecS,
                                     NewFirst,
                                     ParSol))
    {
#ifdef OCCT_DEBUG

      std::cout << "ChFi3d_Builder::SimulData : fail calculate first section" << std::endl;
#endif
      return false;
    }
  }
  else
  {
    ParSol = Soldep;
  }

  while (again < 2)
  {
    TheWalk.Perform(Func,
                    FInv,
                    FInvP,
                    FInvC,
                    NewFirst,
                    Last,
                    MS,
                    tolapp3d,
                    tolapp2d,
                    TolGuide,
                    ParSol,
                    Fleche,
                    Appro);
    if (!TheWalk.IsDone())
    {
#ifdef OCCT_DEBUG
      std::cout << "Path not done" << std::endl;
#endif
      return false;
    }
    if (reverse)
    {
      if (!TheWalk.Complete(Func, FInv, FInvP, FInvC, SpLast))
      {
#ifdef OCCT_DEBUG
        std::cout << "Not completed" << std::endl;
#endif
      }
    }
    Lin   = TheWalk.Line();
    Nbpnt = Lin->NbPoints();
    if (Nbpnt <= 1 && again == 0)
    {
      again++;
#ifdef OCCT_DEBUG
      std::cout << "one point of path MS/50 is attempted." << std::endl;
#endif
      MS     = MS / 50.;
      Target = Targetsov;
    }
    else if (Nbpnt <= NbSecMin && again == 0)
    {
      again++;
#ifdef OCCT_DEBUG
      std::cout << "Number of points is too small, the step is reduced" << std::endl;
#endif
      double u1 = Lin->Point(1).Parameter();
      double u2 = Lin->Point(Nbpnt).Parameter();
      MS        = (u2 - u1) / (NbSecMin + 1);
      Target    = Targetsov;
    }
    else if (Nbpnt <= NbSecMin)
    {
#ifdef OCCT_DEBUG
      std::cout << "Number of points is still too small, quit" << std::endl;
#endif
      return false;
    }
    else
    {
      again = 2;
    }
  }
#ifdef DRAW
  ChFi3d_SettraceDRAWWALK(true);
  if (ChFi3d_GettraceDRAWWALK())
    drawline(Lin, true);
#endif
  if (Forward)
    Decroch = TheWalk.DecrochEnd();
  else
    Decroch = TheWalk.DecrochStart();
  Last  = Lin->Point(Nbpnt).Parameter();
  First = Lin->Point(1).Parameter();
  return true;
}

//=======================================================================
// function : SimulData
// purpose  : Heading of path edge/edge for the bypass
//           of obstacle in simulation mode.
//=======================================================================

bool ChFi3d_Builder::SimulData(occ::handle<ChFiDS_SurfData>& /*Data*/,
                               const occ::handle<ChFiDS_ElSpine>&      HGuide,
                               occ::handle<BRepBlend_Line>&            Lin,
                               const occ::handle<Adaptor3d_Surface>&   S1,
                               const occ::handle<Adaptor2d_Curve2d>&   PC1,
                               const occ::handle<Adaptor3d_TopolTool>& I1,
                               bool&                                   Decroch1,
                               const occ::handle<Adaptor3d_Surface>&   S2,
                               const occ::handle<Adaptor2d_Curve2d>&   PC2,
                               const occ::handle<Adaptor3d_TopolTool>& I2,
                               bool&                                   Decroch2,
                               Blend_RstRstFunction&                   Func,
                               Blend_SurfCurvFuncInv&                  FInv1,
                               Blend_CurvPointFuncInv&                 FInvP1,
                               Blend_SurfCurvFuncInv&                  FInv2,
                               Blend_CurvPointFuncInv&                 FInvP2,
                               const double                            PFirst,
                               const double                            MaxStep,
                               const double                            Fleche,
                               const double                            TolGuide,
                               double&                                 First,
                               double&                                 Last,
                               const math_Vector&                      Soldep,
                               const int                               NbSecMin,
                               const bool                              Inside,
                               const bool                              Appro,
                               const bool                              Forward,
                               const bool                              RecP1,
                               const bool                              RecRst1,
                               const bool                              RecP2,
                               const bool                              RecRst2)
{
  BRepBlend_RstRstLineBuilder TheWalk(S1, PC1, I1, S2, PC2, I2);

  bool   reverse = (!Forward || Inside);
  double SpFirst = HGuide->FirstParameter();
  double SpLast  = HGuide->LastParameter();
  double Target  = SpLast;
  if (reverse)
    Target = SpFirst;
  double Targetsov = Target;

  double MS    = MaxStep;
  int    again = 0;
  int    Nbpnt = 0;
  // The initial solution is reframed if necessary.
  math_Vector ParSol(1, 2);
  double      NewFirst = PFirst;
  if (RecP1 || RecRst1 || RecP2 || RecRst2)
  {
    if (!TheWalk.PerformFirstSection(Func,
                                     FInv1,
                                     FInvP1,
                                     FInv2,
                                     FInvP2,
                                     PFirst,
                                     Target,
                                     Soldep,
                                     tolapp3d,
                                     TolGuide,
                                     RecRst1,
                                     RecP1,
                                     RecRst2,
                                     RecP2,
                                     NewFirst,
                                     ParSol))
    {
#ifdef OCCT_DEBUG

      std::cout << "ChFi3d_Builder::SimulData : calculation fail first section" << std::endl;
#endif
      return false;
    }
  }
  else
  {
    ParSol = Soldep;
  }

  while (again < 2)
  {
    TheWalk.Perform(Func,
                    FInv1,
                    FInvP1,
                    FInv2,
                    FInvP2,
                    NewFirst,
                    Last,
                    MS,
                    tolapp3d,
                    TolGuide,
                    ParSol,
                    Fleche,
                    Appro);
    if (!TheWalk.IsDone())
    {
#ifdef OCCT_DEBUG
      std::cout << "Path not created" << std::endl;
#endif
      return false;
    }
    if (reverse)
    {
      if (!TheWalk.Complete(Func, FInv1, FInvP1, FInv2, FInvP2, SpLast))
      {
#ifdef OCCT_DEBUG
        std::cout << "Not completed" << std::endl;
#endif
      }
    }
    Lin   = TheWalk.Line();
    Nbpnt = Lin->NbPoints();
    if (Nbpnt <= 1 && again == 0)
    {
      again++;
#ifdef OCCT_DEBUG
      std::cout << "only one point of path MS/50 is attempted." << std::endl;
#endif
      MS     = MS / 50.;
      Target = Targetsov;
    }
    else if (Nbpnt <= NbSecMin && again == 0)
    {
      again++;
#ifdef OCCT_DEBUG
      std::cout << "Number of points is too small, the step is reduced" << std::endl;
#endif
      double u1 = Lin->Point(1).Parameter();
      double u2 = Lin->Point(Nbpnt).Parameter();
      MS        = (u2 - u1) / (NbSecMin + 1);
      Target    = Targetsov;
    }
    else if (Nbpnt <= NbSecMin)
    {
#ifdef OCCT_DEBUG
      std::cout << "Number of points is still too small, quit" << std::endl;
#endif
      return false;
    }
    else
    {
      again = 2;
    }
  }
#ifdef DRAW
  if (ChFi3d_GettraceDRAWWALK())
    drawline(Lin, true);
#endif
  if (Forward)
  {
    Decroch1 = TheWalk.Decroch1End();
    Decroch2 = TheWalk.Decroch2End();
  }
  else
  {
    Decroch1 = TheWalk.Decroch1Start();
    Decroch2 = TheWalk.Decroch2Start();
  }

  Last  = Lin->Point(Nbpnt).Parameter();
  First = Lin->Point(1).Parameter();
  return true;
}

//=======================================================================
// function : ComputeData
// purpose  : Construction of elementary fillet by path.
//
//=======================================================================

bool ChFi3d_Builder::ComputeData(occ::handle<ChFiDS_SurfData>&           Data,
                                 const occ::handle<ChFiDS_ElSpine>&      HGuide,
                                 const occ::handle<ChFiDS_Spine>&        Spine,
                                 occ::handle<BRepBlend_Line>&            Lin,
                                 const occ::handle<Adaptor3d_Surface>&   S1,
                                 const occ::handle<Adaptor3d_TopolTool>& I1,
                                 const occ::handle<Adaptor3d_Surface>&   S2,
                                 const occ::handle<Adaptor3d_TopolTool>& I2,
                                 Blend_Function&                         Func,
                                 Blend_FuncInv&                          FInv,
                                 const double                            PFirst,
                                 const double                            MaxStep,
                                 const double                            Fleche,
                                 const double                            tolguide,
                                 double&                                 First,
                                 double&                                 Last,
                                 const bool                              Inside,
                                 const bool                              Appro,
                                 const bool                              Forward,
                                 const math_Vector&                      Soldep,
                                 int&                                    intf,
                                 int&                                    intl,
                                 bool&                                   Gd1,
                                 bool&                                   Gd2,
                                 bool&                                   Gf1,
                                 bool&                                   Gf2,
                                 const bool                              RecOnS1,
                                 const bool                              RecOnS2)
{
  // Get offset guide if exists
  occ::handle<ChFiDS_ElSpine> OffsetHGuide;
  if (!Spine.IsNull() && Spine->Mode() == ChFiDS_ConstThroatWithPenetrationChamfer)
  {
    NCollection_List<occ::handle<ChFiDS_ElSpine>>& ll        = Spine->ChangeElSpines();
    NCollection_List<occ::handle<ChFiDS_ElSpine>>& ll_offset = Spine->ChangeOffsetElSpines();
    NCollection_List<occ::handle<ChFiDS_ElSpine>>::Iterator ILES(ll), ILES_offset(ll_offset);
    for (; ILES.More(); ILES.Next(), ILES_offset.Next())
    {
      const occ::handle<ChFiDS_ElSpine>& aHElSpine = ILES.Value();
      if (aHElSpine == HGuide)
        OffsetHGuide = ILES_offset.Value();
    }
  }

  // The extrensions are created in case of output of two domains
  // directly and not by path ( too hasardous ).
  Data->FirstExtensionValue(0);
  Data->LastExtensionValue(0);

  // The eventual faces are restored to test the jump of edge.
  TopoDS_Face                      F1, F2;
  occ::handle<BRepAdaptor_Surface> HS = occ::down_cast<BRepAdaptor_Surface>(S1);
  if (!HS.IsNull())
    F1 = HS->Face();
  HS = occ::down_cast<BRepAdaptor_Surface>(S2);
  if (!HS.IsNull())
    F2 = HS->Face();

  // Path framing variables
  double TolGuide = tolguide;
  int    nbptmin  = 4;

  BRepBlend_Walking TheWalk(S1, S2, I1, I2, HGuide);

  // Start of removal, 2D path controls
  // that qui s'accomodent mal des surfaces a parametrages non homogenes
  // en u et en v are extinguished.
  TheWalk.Check2d(false);

  double MS = MaxStep;
  int    Nbpnt;
  double SpFirst = HGuide->FirstParameter();
  double SpLast  = HGuide->LastParameter();

  // When the start point is inside, the path goes first to the left
  // to determine the Last for the periodicals.
  bool   reverse = (!Forward || Inside);
  double Target;
  if (reverse)
  {
    Target = SpFirst;
    if (!intf)
      Target = Last;
  }
  else
  {
    Target = SpLast + std::abs(SpLast);
    if (!intl)
      Target = Last;
  }

  // In case if the singularity is pre-determined,
  // the path is indicated.
  if (!Spine.IsNull())
  {
    if (Spine->IsTangencyExtremity(true))
    {
      TopoDS_Vertex V     = Spine->FirstVertex();
      TopoDS_Edge   E     = Spine->Edges(1);
      double        param = Spine->FirstParameter();
      Blend_Point   BP;
      if (CompBlendPoint(V, E, param, F1, F2, BP))
      {
        math_Vector vec(1, 4);
        BP.ParametersOnS1(vec(1), vec(2));
        BP.ParametersOnS2(vec(3), vec(4));
        Func.Set(param);
        if (Func.IsSolution(vec, tolapp3d))
        {
          TheWalk.AddSingularPoint(BP);
        }
      }
    }
    if (Spine->IsTangencyExtremity(false))
    {
      TopoDS_Vertex V     = Spine->LastVertex();
      TopoDS_Edge   E     = Spine->Edges(Spine->NbEdges());
      double        param = Spine->LastParameter();
      Blend_Point   BP;
      if (CompBlendPoint(V, E, param, F1, F2, BP))
      {
        math_Vector vec(1, 4);
        BP.ParametersOnS1(vec(1), vec(2));
        BP.ParametersOnS2(vec(3), vec(4));
        Func.Set(param);
        if (Func.IsSolution(vec, tolapp3d))
        {
          TheWalk.AddSingularPoint(BP);
        }
      }
    }
  }

  // The starting solution is reframed if necessary.
  //**********************************************//
  math_Vector ParSol(1, 4);
  double      NewFirst = PFirst;
  if (RecOnS1 || RecOnS2)
  {
    if (!TheWalk.PerformFirstSection(Func,
                                     FInv,
                                     PFirst,
                                     Target,
                                     Soldep,
                                     tolapp3d,
                                     TolGuide,
                                     RecOnS1,
                                     RecOnS2,
                                     NewFirst,
                                     ParSol))
    {
#ifdef OCCT_DEBUG
      std::cout << "ChFi3d_Builder::ComputeData : calculation fail first section" << std::endl;
#endif
      return false;
    }
  }
  else
  {
    ParSol = Soldep;
  }

  // First the valid part is calculate, without caring for the extensions.
  //******************************************************************//
  int         again      = 0;
  bool        tchernobyl = false;
  double      u1sov = 0., u2sov = 0.;
  TopoDS_Face bif;
  // Max step is relevant, but too great, the vector is required to detect
  // the twists.
  if ((std::abs(Last - First) <= MS * 5.)
      && (std::abs(Last - First) >= 0.01 * std::abs(NewFirst - Target)))
  {
    MS = std::abs(Last - First) * 0.2;
  }

  while (again < 3)
  {
    // Path.
    if (!again && (MS < 5 * TolGuide))
      MS = 5 * TolGuide;
    else
    {
      if (5 * TolGuide > MS)
        TolGuide = MS / 5;
    }
    TheWalk.Perform(Func, FInv, NewFirst, Target, MS, tolapp3d, TolGuide, ParSol, Fleche, Appro);
    if (!TheWalk.IsDone())
    {
#ifdef OCCT_DEBUG
      std::cout << "Path is not created" << std::endl;
#endif
      return false;
    }
    Lin = TheWalk.Line();
    if (HGuide->IsPeriodic() && Inside)
    {
      SpFirst = Lin->Point(1).Parameter();
      SpLast  = SpFirst + HGuide->Period();
      HGuide->FirstParameter(SpFirst);
      HGuide->LastParameter(SpLast);
      HGuide->SetOrigin(SpFirst);
      if (!OffsetHGuide.IsNull())
      {
        OffsetHGuide->FirstParameter(SpFirst);
        OffsetHGuide->LastParameter(SpLast);
        OffsetHGuide->SetOrigin(SpFirst);
      }
    }
    bool complmnt = true;
    if (Inside)
      complmnt = TheWalk.Complete(Func, FInv, SpLast);
    if (!complmnt)
    {
#ifdef OCCT_DEBUG
      std::cout << "Not completed" << std::endl;
#endif
      return false;
    }

    // The result is controlled using two criterions :
    //- if there is enough points,
    //- if one has gone far enough.
    Nbpnt = Lin->NbPoints();
    if (Nbpnt == 0)
    {
#ifdef OCCT_DEBUG
      std::cout << "0 point of path, quit." << std::endl;
#endif
      return false;
    }
    double fpointpar = Lin->Point(1).Parameter();
    double lpointpar = Lin->Point(Nbpnt).Parameter();

    double factor = 1. / (nbptmin + 1);
    bool   okdeb  = (Forward && !Inside);
    bool   okfin  = (!Forward && !Inside);
    if (!okdeb)
    {
      int narc1 = Lin->StartPointOnFirst().NbPointOnRst();
      int narc2 = Lin->StartPointOnSecond().NbPointOnRst();
      okdeb     = (narc1 > 0 || narc2 > 0 || (fpointpar - First) < 10 * TolGuide);
    }
    if (!okfin)
    {
      int narc1 = Lin->EndPointOnFirst().NbPointOnRst();
      int narc2 = Lin->EndPointOnSecond().NbPointOnRst();
      okfin     = (narc1 > 0 || narc2 > 0 || (Last - lpointpar) < 10 * TolGuide);
    }
    if (!okdeb || !okfin || Nbpnt == 1)
    {
      // It drags, the controls are extended, it is  expected to evaluate a
      // satisfactory maximum step. If it already done, quit.
      if (tchernobyl)
      {
#ifdef OCCT_DEBUG
        std::cout << "If it drags without control, quit." << std::endl;
#endif
        return false;
      }
      tchernobyl = true;
      TheWalk.Check(false);
      if (Nbpnt == 1)
      {
#ifdef OCCT_DEBUG
        std::cout << "only one point of path MS/100 is attempted" << std::endl;
        std::cout << "and the controls are extended." << std::endl;
#endif
        MS *= 0.01;
      }
      else
      {
#ifdef OCCT_DEBUG
        std::cout << "It drags, the controls are extended." << std::endl;
#endif
        MS = (lpointpar - fpointpar) / Nbpnt; // EvalStep(Lin);
      }
    }
    else if (Nbpnt < nbptmin)
    {
      if (again == 0)
      {
#ifdef OCCT_DEBUG
        std::cout << "Number of points is too small, the step is reduced" << std::endl;
#endif
        u1sov = fpointpar;
        u2sov = lpointpar;
        MS    = (lpointpar - fpointpar) * factor;
      }
      else if (again == 1)
      {
        if (std::abs(fpointpar - u1sov) >= TolGuide || std::abs(lpointpar - u2sov) >= TolGuide)
        {
#ifdef OCCT_DEBUG
          std::cout << "Number of points is still too small, the step is reduced" << std::endl;
#endif
          MS = (lpointpar - fpointpar) * factor;
        }
        else
        {
#ifdef OCCT_DEBUG
          std::cout << "Number of points is still too small, quit" << std::endl;
#endif
          return false;
        }
      }
      again++;
    }
    else
    {
      again = 3;
    }
  }

  if (TheWalk.TwistOnS1())
  {
    Data->TwistOnS1(true);
#ifdef OCCT_DEBUG
    std::cout << "Path completed, but TWIST on S1" << std::endl;
#endif
  }
  if (TheWalk.TwistOnS2())
  {
    Data->TwistOnS2(true);
#ifdef OCCT_DEBUG
    std::cout << "Parh completed, but TWIST on S2" << std::endl;
#endif
  }

  // Here there is a more or less presentable result
  // however it covers a the minimum zone.
  // The extensions are targeted.
  //*****************************//

  Gd1 = Gd2 = Gf1 = Gf2 = false;

  bool unseulsuffitdeb = (intf >= 2);
  bool unseulsuffitfin = (intl >= 2);
  bool noproldeb       = (intf >= 3);
  bool noprolfin       = (intl >= 3);

  double Rab = 0.03 * (SpLast - SpFirst);

  bool debarc1 = false, debarc2 = false;
  bool debcas1 = false, debcas2 = false;
  bool debobst1 = false, debobst2 = false;

  bool finarc1 = false, finarc2 = false;
  bool fincas1 = false, fincas2 = false;
  bool finobst1 = false, finobst2 = false;

  int narc1, narc2;

  bool backwContinueFailed = false; // eap
  if (reverse && intf)
  {
    narc1 = Lin->StartPointOnFirst().NbPointOnRst();
    narc2 = Lin->StartPointOnSecond().NbPointOnRst();
    if (narc1 != 0)
    {
      ChFi3d_FilCommonPoint(Lin->StartPointOnFirst(),
                            Lin->TransitionOnS1(),
                            true,
                            Data->ChangeVertexFirstOnS1(),
                            tolapp3d);
      debarc1 = true;
      if (!SearchFace(Spine, Data->VertexFirstOnS1(), F1, bif))
      {
        // It is checked if there is not an obstacle.
        debcas1 = true;
        if (!Spine.IsNull())
        {
          if (Spine->IsPeriodic())
          {
            debobst1 = true;
          }
          else
          {
            debobst1 = IsObst(Data->VertexFirstOnS1(), Spine->FirstVertex(), myVEMap);
          }
        }
      }
    }
    if (narc2 != 0)
    {
      ChFi3d_FilCommonPoint(Lin->StartPointOnSecond(),
                            Lin->TransitionOnS2(),
                            true,
                            Data->ChangeVertexFirstOnS2(),
                            tolapp3d);
      debarc2 = true;
      if (!SearchFace(Spine, Data->VertexFirstOnS2(), F2, bif))
      {
        // It is checked if it is not an obstacle.
        debcas2 = true;
        if (!Spine.IsNull())
        {
          if (Spine->IsPeriodic())
          {
            debobst2 = true;
          }
          else
          {
            debobst2 = IsObst(Data->VertexFirstOnS2(), Spine->FirstVertex(), myVEMap);
          }
        }
      }
    }
    bool oncontinue = !noproldeb && (narc1 != 0 || narc2 != 0);
    if (debobst1 || debobst2)
      oncontinue = false;
    else if (debcas1 && debcas2)
      oncontinue = false;
    else if ((!debcas1 && debarc1) || (!debcas2 && debarc2))
      oncontinue = false;

    if (oncontinue)
    {
      TheWalk.ClassificationOnS1(!debarc1);
      TheWalk.ClassificationOnS2(!debarc2);
      TheWalk.Check2d(true); // It should be strict (PMN)
      TheWalk.Continu(Func, FInv, Target);
      TheWalk.ClassificationOnS1(true);
      TheWalk.ClassificationOnS2(true);
      TheWalk.Check2d(false);
      narc1 = Lin->StartPointOnFirst().NbPointOnRst();
      narc2 = Lin->StartPointOnSecond().NbPointOnRst();
      //  modified by eap Fri Feb  8 11:43:48 2002 ___BEGIN___
      if (!debarc1)
      {
        if (narc1 == 0)
          backwContinueFailed = Lin->StartPointOnFirst().ParameterOnGuide() > Target;
        else
        {
          ChFi3d_FilCommonPoint(Lin->StartPointOnFirst(),
                                Lin->TransitionOnS1(),
                                true,
                                Data->ChangeVertexFirstOnS1(),
                                tolapp3d);
          debarc1 = true;
          if (!SearchFace(Spine, Data->VertexFirstOnS1(), F1, bif))
          {
            // It is checked if it is not an obstacle.
            debcas1 = true;
            // 	    if(!Spine.IsNull()) {
            // 	      if(Spine->IsPeriodic()){
            // 	        debobst1 = 1;
            // 	      }
            // 	      else{
            // 		debobst1 = IsObst(Data->VertexFirstOnS1(),
            // 				  Spine->FirstVertex(),myVEMap);
            // 	      }
            // 	    }
          }
        }
      }
      if (!debarc2)
      {
        if (narc2 == 0)
          backwContinueFailed = Lin->StartPointOnSecond().ParameterOnGuide() > Target;
        else
        {
          ChFi3d_FilCommonPoint(Lin->StartPointOnSecond(),
                                Lin->TransitionOnS2(),
                                true,
                                Data->ChangeVertexFirstOnS2(),
                                tolapp3d);
          debarc2 = true;
          if (!SearchFace(Spine, Data->VertexFirstOnS2(), F2, bif))
          {
            // It is checked if it is not an obstacle.
            debcas2 = true;
            //             if(!Spine.IsNull()){
            //               if(Spine->IsPeriodic()){
            //                 debobst2 = 1;
            //               }
            //               else{
            //                 debobst2 = IsObst(Data->VertexFirstOnS2(),
            //                                   Spine->FirstVertex(),myVEMap);
            //               }
            //             }
          }
        }
      }
      if (backwContinueFailed)
      {
        // if we leave backwContinueFailed as is, we will stop in this direction
        // but we are to continue if there are no more faces on the side with arc
        // check this condition
        const ChFiDS_CommonPoint& aCP = debarc1 ? Data->VertexFirstOnS1() : Data->VertexFirstOnS2();
        if (aCP.IsOnArc() && bif.IsNull())
          backwContinueFailed = false;
      }
    }
  }
  bool forwContinueFailed = false;
  //  modified by eap Fri Feb  8 11:44:11 2002 ___END___
  if (Forward && intl)
  {
    Target = SpLast;
    narc1  = Lin->EndPointOnFirst().NbPointOnRst();
    narc2  = Lin->EndPointOnSecond().NbPointOnRst();
    if (narc1 != 0)
    {
      ChFi3d_FilCommonPoint(Lin->EndPointOnFirst(),
                            Lin->TransitionOnS1(),
                            false,
                            Data->ChangeVertexLastOnS1(),
                            tolapp3d);
      finarc1 = true;
      if (!SearchFace(Spine, Data->VertexLastOnS1(), F1, bif))
      {
        // It is checked if it is not an obstacle.
        fincas1 = true;
        if (!Spine.IsNull())
        {
          finobst1 = IsObst(Data->VertexLastOnS1(), Spine->LastVertex(), myVEMap);
        }
      }
    }
    if (narc2 != 0)
    {
      ChFi3d_FilCommonPoint(Lin->EndPointOnSecond(),
                            Lin->TransitionOnS2(),
                            false,
                            Data->ChangeVertexLastOnS2(),
                            tolapp3d);
      finarc2 = true;
      if (!SearchFace(Spine, Data->VertexLastOnS2(), F2, bif))
      {
        // It is checked if it is not an obstacle.
        fincas2 = true;
        if (!Spine.IsNull())
        {
          finobst2 = IsObst(Data->VertexLastOnS2(), Spine->LastVertex(), myVEMap);
        }
      }
    }
    bool oncontinue = !noprolfin && (narc1 != 0 || narc2 != 0);
    if (finobst1 || finobst2)
      oncontinue = false;
    else if (fincas1 && fincas2)
      oncontinue = false;
    else if ((!fincas1 && finarc1) || (!fincas2 && finarc2))
      oncontinue = false;

    if (oncontinue)
    {
      TheWalk.ClassificationOnS1(!finarc1);
      TheWalk.ClassificationOnS2(!finarc2);
      TheWalk.Check2d(true); // It should be strict (PMN)
      TheWalk.Continu(Func, FInv, Target);
      TheWalk.ClassificationOnS1(true);
      TheWalk.ClassificationOnS2(true);
      TheWalk.Check2d(false);
      narc1 = Lin->EndPointOnFirst().NbPointOnRst();
      narc2 = Lin->EndPointOnSecond().NbPointOnRst();
      //  modified by eap Fri Feb  8 11:44:57 2002 ___BEGIN___
      if (!finarc1)
      {
        if (narc1 == 0)
          forwContinueFailed = Lin->EndPointOnFirst().ParameterOnGuide() < Target;
        else
        {
          ChFi3d_FilCommonPoint(Lin->EndPointOnFirst(),
                                Lin->TransitionOnS1(),
                                false,
                                Data->ChangeVertexLastOnS1(),
                                tolapp3d);
          finarc1 = true;
          if (!SearchFace(Spine, Data->VertexLastOnS1(), F1, bif))
          {
            // It is checked if it is not an obstacle.
            fincas1 = true;
            // 	    if(!Spine.IsNull()){
            // 	      finobst1 = IsObst(Data->VertexLastOnS1(),
            // 				Spine->LastVertex(),myVEMap);
            // 	    }
          }
        }
      }
      if (!finarc2)
      {
        if (narc2 == 0)
          forwContinueFailed = Lin->EndPointOnSecond().ParameterOnGuide() < Target;
        else
        {
          ChFi3d_FilCommonPoint(Lin->EndPointOnSecond(),
                                Lin->TransitionOnS2(),
                                false,
                                Data->ChangeVertexLastOnS2(),
                                tolapp3d);
          finarc2 = true;
          if (!SearchFace(Spine, Data->VertexLastOnS2(), F2, bif))
          {
            // On regarde si ce n'est pas un obstacle.
            fincas2 = true;
            // 	    if(!Spine.IsNull()){
            // 	      finobst2 = IsObst(Data->VertexLastOnS2(),
            // 				Spine->LastVertex(),myVEMap);
            // 	    }
          }
        }
      }
      if (forwContinueFailed)
      {
        // if we leave forwContinueFailed as is, we will stop in this direction
        // but we are to continue if there are no more faces on the side with arc
        // check this condition
        const ChFiDS_CommonPoint& aCP = finarc1 ? Data->VertexLastOnS1() : Data->VertexLastOnS2();
        if (aCP.IsOnArc() && bif.IsNull())
          forwContinueFailed = false;
      }
      //  modified by eap Fri Feb  8 11:45:10 2002 ___END___
    }
  }
  Nbpnt = Lin->NbPoints();
#ifdef DRAW
  if (ChFi3d_GettraceDRAWWALK())
    drawline(Lin, false);
#endif
  First = Lin->Point(1).Parameter();
  Last  = Lin->Point(Nbpnt).Parameter();

  // ============= INVALIDATION EVENTUELLE =============
  // ------ Preparation des prolongement par plan tangent -----
  if (reverse && intf)
  {
    Gd1 = debcas1 /* && !debobst1*/; // skv(occ67)
    Gd2 = debcas2 /* && !debobst2*/; // skv(occ67)
    if ((debarc1 ^ debarc2) && !unseulsuffitdeb && (First != SpFirst))
    {
      // Case of incomplete path, of course this ends badly :
      // the result is truncated instead of exit.
      double sortie;
      int    ind;
      if (debarc1)
        sortie = Data->VertexFirstOnS1().Parameter();
      else
        sortie = Data->VertexFirstOnS2().Parameter();
      if (sortie - First > tolesp)
      {
        ind = SearchIndex(sortie, Lin);
        if (Lin->Point(ind).Parameter() == sortie)
          ind--;
        if (ind >= 1)
        {
          Lin->Remove(1, ind);
          UpdateLine(Lin, true);
        }
        Nbpnt = Lin->NbPoints();
        First = Lin->Point(1).Parameter();
      }
    }
    else if ((intf >= 5) && !debarc1 && !debarc2 && (First != SpFirst))
    {
      double sortie = (2 * First + Last) / 3;
      int    ind;
      if (sortie - First > tolesp)
      {
        ind = SearchIndex(sortie, Lin);
        if (Lin->Point(ind).Parameter() == sortie)
          ind--;
        if (Nbpnt - ind < 3)
          ind = Nbpnt - 3;
        if (ind >= 1)
        {
          Lin->Remove(1, ind);
          UpdateLine(Lin, true);
        }
        Nbpnt = Lin->NbPoints();
        First = Lin->Point(1).Parameter();
      }
    }
    if (Gd1 && Gd2)
    {
      Target = std::min((Lin->Point(1).Parameter() - Rab), First);
      Target = std::max(Target, SpFirst);
      Data->FirstExtensionValue(std::abs(Lin->Point(1).Parameter() - Target));
    }
    if (intf && !unseulsuffitdeb)
      intf = (Gd1 && Gd2)            //;
             || backwContinueFailed; // eap
    else if (intf && unseulsuffitdeb && (intf < 5))
    {
      intf = (Gd1 || Gd2);
      // It is checked if there is no new face.
      if (intf && ((!debcas1 && debarc1) || (!debcas2 && debarc2)))
        intf = 0;
    }
    else if (intf < 5)
      intf = 0;
  }

  if (Forward && intl)
  {
    Gf1 = fincas1 /* && !finobst1*/; // skv(occ67)
    Gf2 = fincas2 /* && !finobst2*/; // skv(occ67)
    if ((finarc1 ^ finarc2) && !unseulsuffitfin && (Last != SpLast))
    {
      // Case of incomplete path, of course, this ends badly :
      // the result is truncated instead of exit.
      double sortie;
      int    ind;
      if (finarc1)
        sortie = Data->VertexLastOnS1().Parameter();
      else
        sortie = Data->VertexLastOnS2().Parameter();
      if (Last - sortie > tolesp)
      {
        ind = SearchIndex(sortie, Lin);
        if (Lin->Point(ind).Parameter() == sortie)
          ind++;
        if (ind <= Nbpnt)
        {
          Lin->Remove(ind, Nbpnt);
          UpdateLine(Lin, false);
        }
        Nbpnt = Lin->NbPoints();
        Last  = Lin->Point(Nbpnt).Parameter();
      }
    }
    else if ((intl >= 5) && !finarc1 && !finarc2 && (Last != SpLast))
    {
      // The same in case when the entire "Lin" is an extension
      double sortie = (First + 2 * Last) / 3;
      int    ind;
      if (Last - sortie > tolesp)
      {
        ind = SearchIndex(sortie, Lin);
        if (Lin->Point(ind).Parameter() == sortie)
          ind++;
        if (ind < 3)
          ind = 3;
        if (ind <= Nbpnt)
        {
          Lin->Remove(ind, Nbpnt);
          UpdateLine(Lin, false);
        }
        Nbpnt = Lin->NbPoints();
        Last  = Lin->Point(Nbpnt).Parameter();
      }
    }
    if (Gf1 && Gf2)
    {
      Target = std::max((Lin->Point(Nbpnt).Parameter() + Rab), Last);
      Target = std::min(Target, SpLast);
      Data->LastExtensionValue(std::abs(Target - Lin->Point(Nbpnt).Parameter()));
    }

    if (intl && !unseulsuffitfin)
      intl = (Gf1 && Gf2)           //;
             || forwContinueFailed; // eap
    else if (intl && unseulsuffitfin && (intl < 5))
    {
      intl = (Gf1 || Gf2); // It is checked if there is no new face.
      if (intl && ((!fincas1 && finarc1) || (!fincas2 && finarc2)))
        intl = 0;
    }
    else if (intl < 5)
      intl = 0;
  }
  return true;
}

//=================================================================================================

bool ChFi3d_Builder::SimulData(occ::handle<ChFiDS_SurfData>& /*Data*/,
                               const occ::handle<ChFiDS_ElSpine>&      HGuide,
                               const occ::handle<ChFiDS_ElSpine>&      AdditionalHGuide,
                               occ::handle<BRepBlend_Line>&            Lin,
                               const occ::handle<Adaptor3d_Surface>&   S1,
                               const occ::handle<Adaptor3d_TopolTool>& I1,
                               const occ::handle<Adaptor3d_Surface>&   S2,
                               const occ::handle<Adaptor3d_TopolTool>& I2,
                               Blend_Function&                         Func,
                               Blend_FuncInv&                          FInv,
                               const double                            PFirst,
                               const double                            MaxStep,
                               const double                            Fleche,
                               const double                            tolguide,
                               double&                                 First,
                               double&                                 Last,
                               const bool                              Inside,
                               const bool                              Appro,
                               const bool                              Forward,
                               const math_Vector&                      Soldep,
                               const int                               NbSecMin,
                               const bool                              RecOnS1,
                               const bool                              RecOnS2)
{
  BRepBlend_Walking TheWalk(S1, S2, I1, I2, HGuide);
  TheWalk.Check2d(false);

  double MS       = MaxStep;
  double TolGuide = tolguide;
  int    Nbpnt    = 0;
  double SpFirst  = HGuide->FirstParameter();
  double SpLast   = HGuide->LastParameter();
  bool   reverse  = (!Forward || Inside);
  double Target;
  if (reverse)
  {
    Target = SpFirst;
  }
  else
  {
    Target = SpLast;
  }

  double Targetsov = Target;
  double u1sov = 0., u2sov = 0.;
  // on recadre la solution de depart a la demande.
  math_Vector ParSol(1, 4);
  double      NewFirst = PFirst;
  if (RecOnS1 || RecOnS2)
  {
    if (!TheWalk.PerformFirstSection(Func,
                                     FInv,
                                     PFirst,
                                     Target,
                                     Soldep,
                                     tolapp3d,
                                     TolGuide,
                                     RecOnS1,
                                     RecOnS2,
                                     NewFirst,
                                     ParSol))
    {
#ifdef OCCT_DEBUG
      std::cout << "ChFi3d_Builder::SimulData : calculation fail first section" << std::endl;
#endif
      return false;
    }
  }
  else
  {
    ParSol = Soldep;
  }
  int again = 0;
  while (again < 3)
  {
    // When the start point is inside, the path goes first to the left
    // to determine the Last for the periodicals.
    if (!again && (MS < 5 * TolGuide))
      MS = 5 * TolGuide;
    else
    {
      if (5 * TolGuide > MS)
        TolGuide = MS / 5;
    }

    TheWalk.Perform(Func, FInv, NewFirst, Target, MS, tolapp3d, TolGuide, ParSol, Fleche, Appro);

    if (!TheWalk.IsDone())
    {
#ifdef OCCT_DEBUG
      std::cout << "Path not created" << std::endl;
#endif
      return false;
    }
    Lin = TheWalk.Line();
    if (reverse)
    {
      if (HGuide->IsPeriodic())
      {
        SpFirst = Lin->Point(1).Parameter();
        SpLast  = SpFirst + HGuide->Period();
        HGuide->FirstParameter(SpFirst);
        HGuide->LastParameter(SpLast);
        if (!AdditionalHGuide.IsNull())
        {
          AdditionalHGuide->FirstParameter(SpFirst);
          AdditionalHGuide->LastParameter(SpLast);
        }
      }
      bool complmnt = true;
      if (Inside)
        complmnt = TheWalk.Complete(Func, FInv, SpLast);
      if (!complmnt)
      {
#ifdef OCCT_DEBUG
        std::cout << "Not completed" << std::endl;
#endif
        return false;
      }
    }
    Nbpnt         = Lin->NbPoints();
    double factor = 1. / (NbSecMin + 1);
    if (Nbpnt == 0)
    {
#ifdef OCCT_DEBUG
      std::cout << "0 point of path, quit." << std::endl;
#endif
      return false;
    }
    else if (Nbpnt == 1 && again == 0)
    {
      again++;
#ifdef OCCT_DEBUG
      std::cout << "only one point of path, MS/100 is attempted." << std::endl;
#endif
      MS *= 0.01;
      Target = Targetsov;
      u1sov = u2sov = Lin->Point(1).Parameter();
    }
    else if (Nbpnt < NbSecMin && again == 0)
    {
      again++;
#ifdef OCCT_DEBUG
      std::cout << "Number of points is too small, the step is reduced" << std::endl;
#endif
      double u1 = u1sov = Lin->Point(1).Parameter();
      double u2 = u2sov = Lin->Point(Nbpnt).Parameter();
      MS                = (u2 - u1) * factor;
      Target            = Targetsov;
    }
    else if (Nbpnt < NbSecMin && again == 1)
    {
      double u1 = Lin->Point(1).Parameter();
      double u2 = Lin->Point(Nbpnt).Parameter();
      if (std::abs(u1 - u1sov) >= TolGuide || std::abs(u2 - u2sov) >= TolGuide)
      {
        again++;
#ifdef OCCT_DEBUG
        std::cout << "Number of points is still too small, the step is reduced" << std::endl;
#endif
        MS /= 100;
        Target = Targetsov;
      }
      else
      {
#ifdef OCCT_DEBUG
        std::cout << "Number of points is still too small, quit" << std::endl;
#endif
        return false;
      }
    }
    else if (Nbpnt < NbSecMin)
    {
#ifdef OCCT_DEBUG
      std::cout << "Number of points is still too small, quit" << std::endl;
#endif
      return false;
    }
    else
    {
      again = 3;
    }
  }
#ifdef DRAW
  if (ChFi3d_GettraceDRAWWALK())
    drawline(Lin, false);
#endif
  First = Lin->Point(1).Parameter();
  Last  = Lin->Point(Nbpnt).Parameter();
  return true;
}
