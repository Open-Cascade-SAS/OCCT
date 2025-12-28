// Created on: 1995-04-24
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

#include <Bisector_Bisec.hxx>
#include <Bisector_BisecAna.hxx>
#include <BRep_Tool.hxx>
#include <BRepFill_TrimEdgeTool.hxx>
#include <ElCLib.hxx>
#include <Geom2d_CartesianPoint.hxx>
#include <Geom2d_Curve.hxx>
#include <Geom2d_Geometry.hxx>
#include <Geom2d_TrimmedCurve.hxx>
#include <Geom2dAPI_ProjectPointOnCurve.hxx>
#include <Geom2dInt_GInter.hxx>
#include <gp_Pnt.hxx>
#include <gp_Pnt2d.hxx>
#include <IntRes2d_IntersectionPoint.hxx>
#include <IntRes2d_IntersectionSegment.hxx>
#include <Precision.hxx>
#include <StdFail_NotDone.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopExp.hxx>
#include <BRepAdaptor_Curve.hxx>

#ifdef OCCT_DEBUG
  // #define DRAW
  #ifdef DRAW
    #include <DrawTrSurf.hxx>
    #include <DBRep.hxx>
    #include <Geom2d_Point.hxx>
static bool Affich    = false;
static bool AffichInt = false;
static int intind    = 0;
  #endif
#endif

//=================================================================================================

static void SimpleExpression(const Bisector_Bisec& B, occ::handle<Geom2d_Curve>& Bis)
{
  Bis = B.Value();

  occ::handle<Standard_Type> BT = Bis->DynamicType();
  if (BT == STANDARD_TYPE(Geom2d_TrimmedCurve))
  {
    occ::handle<Geom2d_TrimmedCurve> TrBis  = occ::down_cast<Geom2d_TrimmedCurve>(Bis);
    occ::handle<Geom2d_Curve>        BasBis = TrBis->BasisCurve();
    BT                                 = BasBis->DynamicType();
    if (BT == STANDARD_TYPE(Bisector_BisecAna))
    {
      Bis = occ::down_cast<Bisector_BisecAna>(BasBis)->Geom2dCurve();
      Bis = new Geom2d_TrimmedCurve(Bis, TrBis->FirstParameter(), TrBis->LastParameter());
    }
  }
}

//=================================================================================================

BRepFill_TrimEdgeTool::BRepFill_TrimEdgeTool() {}

//=================================================================================================

BRepFill_TrimEdgeTool::BRepFill_TrimEdgeTool(const Bisector_Bisec&          Bisec,
                                             const occ::handle<Geom2d_Geometry>& S1,
                                             const occ::handle<Geom2d_Geometry>& S2,
                                             const double            Offset)
    : myOffset(Offset),
      myBisec(Bisec)
{
  isPoint1 = (S1->DynamicType() == STANDARD_TYPE(Geom2d_CartesianPoint));
  isPoint2 = (S2->DynamicType() == STANDARD_TYPE(Geom2d_CartesianPoint));

  // return geometries of shapes.
  //  double f,l;
  if (isPoint1)
  {
    myP1 = occ::down_cast<Geom2d_Point>(S1)->Pnt2d();
  }
  else
  {
    myC1 = occ::down_cast<Geom2d_Curve>(S1);
#ifdef DRAW
    if (Affich)
    {
      // POP pour NT
      char* myC1name = "myC1";
      DrawTrSurf::Set(myC1name, myC1);
      //      DrawTrSurf::Set("myC1",myC1);
    }
#endif
  }
  if (isPoint2)
  {
    myP2 = occ::down_cast<Geom2d_Point>(S2)->Pnt2d();
  }
  else
  {
    myC2 = occ::down_cast<Geom2d_Curve>(S2);
#ifdef DRAW
    if (Affich)
    {
      char* myC2name = "myC2";
      DrawTrSurf::Set(myC2name, myC2);
      //      DrawTrSurf::Set("myC2",myC2);
    }
#endif
  }
  // return the simple expression of the bissectrice
  occ::handle<Geom2d_Curve> Bis;
  SimpleExpression(myBisec, Bis);
  myBis = Geom2dAdaptor_Curve(Bis);
#ifdef DRAW
  if (Affich)
  {
    char* myBisname = "myBis";
    DrawTrSurf::Set(myBisname, Bis);
  }
#endif
}

//=======================================================================
// function : Bubble
// purpose  : Order the sequence of points by increasing x.
//=======================================================================

static void Bubble(NCollection_Sequence<gp_Pnt>& Seq)
{
  bool Invert   = true;
  int NbPoints = Seq.Length();
  while (Invert)
  {
    Invert = false;
    for (int i = 1; i < NbPoints; i++)
    {
      gp_Pnt P1 = Seq.Value(i);
      gp_Pnt P2 = Seq.Value(i + 1);
      if (P2.X() < P1.X())
      {
        Seq.Exchange(i, i + 1);
        Invert = true;
      }
    }
  }
}

//=================================================================================================

static void EvalParameters(const Geom2dAdaptor_Curve& Bis,
                           const Geom2dAdaptor_Curve& AC,
                           NCollection_Sequence<gp_Pnt>&      Params)
{
  Geom2dInt_GInter        Intersector;
  constexpr double Tol = Precision::Confusion();
  //  double TolC = 1.e-9;

  const Geom2dAdaptor_Curve& CBis(Bis);
  const Geom2dAdaptor_Curve& CAC(AC);

  // Intersector = Geom2dInt_GInter(CBis, CAC, TolC, Tol);
  Intersector = Geom2dInt_GInter(CAC, CBis, Tol, Tol);

  int NbPoints, NbSegments;
  double    U1, U2;
  gp_Pnt           P;

  if (!Intersector.IsDone())
  {
    throw StdFail_NotDone("BRepFill_TrimSurfaceTool::IntersectWith");
  }

  NbPoints = Intersector.NbPoints();

  if (NbPoints > 0)
  {
    for (int i = 1; i <= NbPoints; i++)
    {
      U1 = Intersector.Point(i).ParamOnSecond();
      U2 = Intersector.Point(i).ParamOnFirst();
      P  = gp_Pnt(U1, U2, 0.);
      Params.Append(P);
    }
  }

  NbSegments = Intersector.NbSegments();

  if (NbSegments > 0)
  {
    IntRes2d_IntersectionSegment Seg;
    for (int i = 1; i <= NbSegments; i++)
    {
      Seg                 = Intersector.Segment(i);
      U1                  = Seg.FirstPoint().ParamOnSecond();
      double Ulast = Seg.LastPoint().ParamOnSecond();
      if (std::abs(U1 - CBis.FirstParameter()) <= Tol
          && std::abs(Ulast - CBis.LastParameter()) <= Tol)
      {
        P = gp_Pnt(U1, Seg.FirstPoint().ParamOnFirst(), 0.);
        Params.Append(P);
        P = gp_Pnt(Ulast, Seg.LastPoint().ParamOnFirst(), 0.);
        Params.Append(P);
      }
      else
      {
        U1 += Seg.LastPoint().ParamOnSecond();
        U1 /= 2.;
        U2 = Seg.FirstPoint().ParamOnFirst();
        U2 += Seg.LastPoint().ParamOnFirst();
        U2 /= 2.;
        P = gp_Pnt(U1, U2, 0.);
        Params.Append(P);
      }
    }
  }

  // Order the sequence by growing parameter on the bissectrice.
  Bubble(Params);
}

static void EvalParametersBis(const Geom2dAdaptor_Curve& Bis,
                              const Geom2dAdaptor_Curve& AC,
                              NCollection_Sequence<gp_Pnt>&      Params,
                              const double        Tol)
{
  Geom2dInt_GInter Intersector;
  double    TolC = Tol;

  const Geom2dAdaptor_Curve& CBis(Bis);
  const Geom2dAdaptor_Curve& CAC(AC);

  Intersector = Geom2dInt_GInter(CAC, CBis, TolC, Tol);

  int NbPoints, NbSegments;
  double    U1, U2;
  gp_Pnt           P;

  if (!Intersector.IsDone())
  {
    throw StdFail_NotDone("BRepFill_TrimSurfaceTool::IntersectWith");
  }

  NbPoints = Intersector.NbPoints();

  if (NbPoints > 0)
  {
    for (int i = 1; i <= NbPoints; i++)
    {
      U1 = Intersector.Point(i).ParamOnSecond();
      U2 = Intersector.Point(i).ParamOnFirst();
      P  = gp_Pnt(U1, U2, 0.);
      Params.Append(P);
    }
  }

  NbSegments = Intersector.NbSegments();

  if (NbSegments > 0)
  {
    IntRes2d_IntersectionSegment Seg;
    for (int i = 1; i <= NbSegments; i++)
    {
      Seg                 = Intersector.Segment(i);
      U1                  = Seg.FirstPoint().ParamOnSecond();
      double Ulast = Seg.LastPoint().ParamOnSecond();
      if (std::abs(U1 - CBis.FirstParameter()) <= Tol
          && std::abs(Ulast - CBis.LastParameter()) <= Tol)
      {
        P = gp_Pnt(U1, Seg.FirstPoint().ParamOnFirst(), 0.);
        Params.Append(P);
        P = gp_Pnt(Ulast, Seg.LastPoint().ParamOnFirst(), 0.);
        Params.Append(P);
      }
      else
      {
        U1 += Seg.LastPoint().ParamOnSecond();
        U1 /= 2.;
        U2 = Seg.FirstPoint().ParamOnFirst();
        U2 += Seg.LastPoint().ParamOnFirst();
        U2 /= 2.;
        P = gp_Pnt(U1, U2, 0.);
        Params.Append(P);
      }
    }
  }

  // Order the sequence by parameter growing on the bissectrice.
  Bubble(Params);
}

//=================================================================================================

void BRepFill_TrimEdgeTool::IntersectWith(const TopoDS_Edge&     Edge1,
                                          const TopoDS_Edge&     Edge2,
                                          const TopoDS_Shape&    InitShape1,
                                          const TopoDS_Shape&    InitShape2,
                                          const TopoDS_Vertex&   End1,
                                          const TopoDS_Vertex&   End2,
                                          const GeomAbs_JoinType theJoinType,
                                          const bool IsOpenResult,
                                          NCollection_Sequence<gp_Pnt>&  Params)
{
  Params.Clear();

  // return curves associated to edges.
  TopLoc_Location      L;
  double        f, l;
  occ::handle<Geom_Surface> Surf;

  occ::handle<Geom2d_Curve> C1;
  BRep_Tool::CurveOnSurface(Edge1, C1, Surf, L, f, l);
  Geom2dAdaptor_Curve AC1(C1, f, l);

  occ::handle<Geom2d_Curve> C2;
  BRep_Tool::CurveOnSurface(Edge2, C2, Surf, L, f, l);
  Geom2dAdaptor_Curve AC2(C2, f, l);

#ifdef DRAW
  if (AffichInt)
  {
    f = AC1.FirstParameter();
    l = AC1.LastParameter();
    char name[32];
    Sprintf(name, "C1_%d", ++intind);
    DrawTrSurf::Set(name, new Geom2d_TrimmedCurve(C1, f, l));
    f = AC2.FirstParameter();
    l = AC2.LastParameter();
    Sprintf(name, "C2_%d", intind);
    DrawTrSurf::Set(name, new Geom2d_TrimmedCurve(C2, f, l));
    f = myBis.FirstParameter();
    l = myBis.LastParameter();
    Sprintf(name, "BIS%d", intind);
    DrawTrSurf::Set(name, new Geom2d_TrimmedCurve(myBis.Curve(), f, l));
    Sprintf(name, "E1_%d", intind);
    DBRep::Set(name, Edge1);
    Sprintf(name, "E2_%d", intind);
    DBRep::Set(name, Edge2);
  }
#endif

  // Calculate intersection
  NCollection_Sequence<gp_Pnt> Points2;
  gp_Pnt               PSeq;

  EvalParameters(myBis, AC1, Params);
  EvalParameters(myBis, AC2, Points2);

  int SeanceDeRattrapage = 0;
  double    TolInit            = 1.e-9;
  int nn                 = 7;

  if ((AC1.GetType() != GeomAbs_Circle && AC1.GetType() != GeomAbs_Line)
      || (AC2.GetType() != GeomAbs_Circle && AC2.GetType() != GeomAbs_Line))
  {

    TolInit = 1.e-8;
    nn      = 6;
  }

  if (Params.IsEmpty() && Points2.IsEmpty())
  {
    // Check, may be there are no intersections at all
    //  for case myBis == Line
    if (myBis.GetType() == GeomAbs_Line)
    {
      double    dmax = TolInit;
      int n    = 0;
      while (n < nn)
      {
        dmax *= 10.0;
        ++n;
      }
      dmax *= dmax;
      //
      gp_Lin2d         anL    = myBis.Line();
      bool isFar1 = true;
      bool isFar2 = true;
      gp_Pnt2d         aP;
      //
      double d = RealLast();
      AC1.D0(AC1.FirstParameter(), aP);
      double par = ElCLib::Parameter(anL, aP);
      if (par >= myBis.FirstParameter() && par <= myBis.LastParameter())
      {
        d = anL.SquareDistance(aP);
      }
      AC1.D0(AC1.LastParameter(), aP);
      par = ElCLib::Parameter(anL, aP);
      if (par >= myBis.FirstParameter() && par <= myBis.LastParameter())
      {
        d = std::min(anL.SquareDistance(aP), d);
      }
      isFar1 = d > dmax;
      //
      d = RealLast();
      AC2.D0(AC2.FirstParameter(), aP);
      par = ElCLib::Parameter(anL, aP);
      if (par >= myBis.FirstParameter() && par <= myBis.LastParameter())
      {
        d = anL.SquareDistance(aP);
      }
      AC2.D0(AC2.LastParameter(), aP);
      par = ElCLib::Parameter(anL, aP);
      if (par >= myBis.FirstParameter() && par <= myBis.LastParameter())
      {
        d = std::min(anL.SquareDistance(aP), d);
      }
      isFar2 = d > dmax;
      //
      if (isFar1 && isFar2)
      {
        return;
      }
    }
  }

  while (
    SeanceDeRattrapage < nn // TolInit <= 0.01
    && (Points2.Length() != Params.Length() || (Points2.Length() == 0 && Params.Length() == 0)))
  {

#ifdef OCCT_DEBUG
    std::cout << "BRepFill_TrimEdgeTool: incoherent intersection. Try with a greater tolerance"
              << std::endl;
#endif

    Params.Clear();
    Points2.Clear();

    TolInit *= 10.0;

    EvalParametersBis(myBis, AC1, Params, TolInit);
    EvalParametersBis(myBis, AC2, Points2, TolInit);
    SeanceDeRattrapage++;
  }

#ifdef OCCT_DEBUG
  if (SeanceDeRattrapage != 0)
    std::cout << "SeanceDeRattrapage = " << SeanceDeRattrapage << std::endl;
  if (SeanceDeRattrapage == nn)
  {
    std::cout << "BRepFill_TrimEdgeTool: incoherent intersection" << std::endl;
  }
#endif

  if (Params.Length() == 0 && Points2.Length() == 1)
  {

    // std::cout << "Params.Length() == 0 && Points2.Length() == 1" << std::endl;
    double dmin, dmax = 0.25 * myOffset * myOffset;
    double tBis = Points2(1).X();
    gp_Pnt2d      PBis = myBis.Value(tBis);

    double t  = AC1.FirstParameter();
    gp_Pnt2d      PC = AC1.Value(t);
    dmin             = PC.SquareDistance(PBis);
    gp_Pnt P(tBis, t, 0.);
    if (dmin < dmax)
    {
      Params.Append(P);
    }

    t                   = AC1.LastParameter();
    PC                  = AC1.Value(t);
    double dmin1 = PC.SquareDistance(PBis);
    if (dmin > dmin1 && dmin1 < dmax)
    {
      P.SetY(t);
      if (Params.IsEmpty())
        Params.Append(P);
      else
        Params.SetValue(1, P);
    }
  }
  else if (Params.Length() == 1 && Points2.Length() == 0)
  {

    // std::cout << "Params.Length() == 1 && Points2.Length() == 0" << std::endl;
    double dmin, dmax = 0.25 * myOffset * myOffset;
    double tBis = Params(1).X();
    gp_Pnt2d      PBis = myBis.Value(tBis);

    double t  = AC2.FirstParameter();
    gp_Pnt2d      PC = AC2.Value(t);
    dmin             = PC.SquareDistance(PBis);
    gp_Pnt P(tBis, t, 0.);
    if (dmin < dmax)
    {
      Points2.Append(P);
    }

    t                   = AC2.LastParameter();
    PC                  = AC2.Value(t);
    double dmin1 = PC.SquareDistance(PBis);
    if (dmin > dmin1 && dmin1 < dmax)
    {
      P.SetY(t);
      if (Points2.IsEmpty())
        Points2.Append(P);
      else
        Points2.SetValue(1, P);
    }
  }

  // small manipulation to remove incorrect intersections:
  // return only common intersections (same parameter
  // on the bissectrice.).
  // The tolerance can be eventually changed.

  gp_Pnt                  P1, P2;
  constexpr double Tol      = 4 * 100 * Precision::PConfusion();
  int        i        = 1;
  int        NbPoints = Params.Length();

  if (NbPoints == 1 && Points2.Length() == 1)
  {
    // std::cout << "NbPoints == 1 && Points2.Length() == 1" << std::endl;
    for (i = 1; i <= NbPoints; i++)
    {
      PSeq = Params(i);
      PSeq.SetZ((Points2.Value(i)).Y());
      Params.SetValue(i, PSeq);
    }
    return;
  }

  i = 1;
  while (i <= std::min(Params.Length(), Points2.Length()))
  {
    P1                   = Params(i);
    P2                   = Points2(i);
    double P1xP2x = std::abs(P1.X() - P2.X());

    if (P1xP2x > Tol)
    {
#ifdef OCCT_DEBUG
      std::cout << "BRepFill_TrimEdgeTool: no same parameter on the bissectrice" << std::endl;
#endif
      if (P1xP2x > TolInit)
      {
#ifdef OCCT_DEBUG
        std::cout << "BRepFill_TrimEdgeTool: Continue somehow" << std::endl;
#endif
        i++;
      }
      else
      {
        if (P1.X() < P2.X())
          Params.Remove(i);
        else
          Points2.Remove(i);
      }
    }
    else
      i++;
  }

  if (Params.Length() > Points2.Length())
  {
    Params.Remove(Points2.Length() + 1, Params.Length());
  }
  else if (Params.Length() < Points2.Length())
  {
    Points2.Remove(Params.Length() + 1, Points2.Length());
  }

  NbPoints = Params.Length();

  // Now we define: if there are more than one point of intersection
  // is it Ok ?
  double init_fpar = RealFirst(), init_lpar = RealLast();
  if (NbPoints > 1 && theJoinType == GeomAbs_Intersection && InitShape1.ShapeType() != TopAbs_VERTEX
      && InitShape2.ShapeType() != TopAbs_VERTEX)
  {
    // definition of initial first and last parameters:
    // this is inverse procedure to extension of parameters
    //(see BRepFill_OffsetWire, function MakeOffset, case of Circle)
    const TopoDS_Edge& InitEdge1        = TopoDS::Edge(InitShape1);
    bool   ToExtendFirstPar = true;
    bool   ToExtendLastPar  = true;
    if (IsOpenResult)
    {
      TopoDS_Vertex V1, V2;
      TopExp::Vertices(InitEdge1, V1, V2);
      if (V1.IsSame(End1) || V1.IsSame(End2))
        ToExtendFirstPar = false;
      if (V2.IsSame(End1) || V2.IsSame(End2))
        ToExtendLastPar = false;
    }
    BRepAdaptor_Curve IC1(InitEdge1);
    if (IC1.GetType() == GeomAbs_Circle)
    {
      double Delta = 2 * M_PI - IC1.LastParameter() + IC1.FirstParameter();
      if (ToExtendFirstPar && ToExtendLastPar)
        init_fpar = AC1.FirstParameter() + Delta / 2;
      else if (ToExtendFirstPar)
        init_fpar = AC1.FirstParameter() + Delta;
      else if (ToExtendLastPar)
        init_fpar = AC1.FirstParameter();
      init_lpar = init_fpar + IC1.LastParameter() - IC1.FirstParameter();
    }
  }

  if (NbPoints > 1 && theJoinType == GeomAbs_Intersection)
  {
    // Remove all vertices with non-minimal parameter
    // if they are out of initial range
    int imin = 1;
    for (i = 2; i <= NbPoints; i++)
      if (Params(i).X() < Params(imin).X())
        imin = i;

    NCollection_Sequence<gp_Pnt> ParamsCopy  = Params;
    NCollection_Sequence<gp_Pnt> Points2Copy = Points2;
    Params.Clear();
    Points2.Clear();
    for (i = 1; i <= ParamsCopy.Length(); i++)
      if (imin == i || (ParamsCopy(i).Y() >= init_fpar && ParamsCopy(i).Y() <= init_lpar))
      {
        Params.Append(ParamsCopy(i));
        Points2.Append(Points2Copy(i));
      }

    /*
    gp_Pnt Pnt1 = Params(imin);
    gp_Pnt Pnt2 = Points2(imin);
    Params.Clear();
    Points2.Clear();
    Params.Append(Pnt1);
    Points2.Append(Pnt2);
    */
  }

  NbPoints = Params.Length();
  for (i = 1; i <= NbPoints; i++)
  {
    PSeq = Params(i);
    PSeq.SetZ((Points2.Value(i)).Y());
    Params.SetValue(i, PSeq);
  }
}

//=======================================================================
// function : AddOrConfuse
// purpose  : the first or the last point of the bissectrice is on the
//           parallel if it was not found in the intersections,
//           it is projected on parallel lines and added in the parameters
//=======================================================================

void BRepFill_TrimEdgeTool::AddOrConfuse(const bool Start,
                                         const TopoDS_Edge&     Edge1,
                                         const TopoDS_Edge&     Edge2,
                                         NCollection_Sequence<gp_Pnt>&  Params) const
{
  bool        ToProj = true;
  gp_Pnt2d                PBis;
  constexpr double Tol = 10 * Precision::Confusion();

  // return curves associated to edges.
  TopLoc_Location      L;
  double        f, l;
  occ::handle<Geom_Surface> Surf;

  occ::handle<Geom2d_Curve> C1;
  BRep_Tool::CurveOnSurface(Edge1, C1, Surf, L, f, l);
  Geom2dAdaptor_Curve AC1(C1, f, l);

  if (Start)
    PBis = myBis.Value(myBis.FirstParameter());
  else
    PBis = myBis.Value(myBis.LastParameter());

  // Test if the end of the bissectrice is in the set of intersection points.
  if (!Params.IsEmpty())
  {
    gp_Pnt2d P;
    if (Start)
      P = AC1.Value(Params.First().Y());
    else
      P = AC1.Value(Params.Last().Y());
    ToProj = !PBis.IsEqual(P, Tol);
  }

  if (ToProj)
  {
#ifdef OCCT_DEBUG
    std::cout << " project extremity bissectrice on parallel." << std::endl;
#endif

    // Project point on parallels and add in Params

    double        f2, l2;
    occ::handle<Geom2d_Curve> C2;
    BRep_Tool::CurveOnSurface(Edge2, C2, Surf, L, f2, l2);

    Geom2dAPI_ProjectPointOnCurve Projector1(PBis, C1, f, l);
    Geom2dAPI_ProjectPointOnCurve Projector2(PBis, C2, f2, l2);

    if (Projector1.NbPoints() == 0)
    {
#ifdef OCCT_DEBUG
      std::cout << "Failed projection in BRepFill_TrimEdgeTool::AddOrConfuse" << std::endl;
#endif
      return;
    }
    if (!Projector1.NearestPoint().IsEqual(PBis, Tol))
    {
#ifdef OCCT_DEBUG
      std::cout << "Incorrect solution in BRepFill_TrimEdgeTool::AddOrConfuse" << std::endl;
#endif
      return;
    }
    if (Projector2.NbPoints() == 0)
    {
#ifdef OCCT_DEBUG
      std::cout << "Failed projection in BRepFill_TrimEdgeTool::AddOrConfuse" << std::endl;
#endif
      return;
    }
    if (!Projector2.NearestPoint().IsEqual(PBis, Tol))
    {
#ifdef OCCT_DEBUG
      std::cout << " Mauvaisesolution dans BRepFill_TrimEdgeTool::AddOrConfuse" << std::endl;
#endif
      return;
    }
    gp_Pnt PInt(0, Projector1.LowerDistanceParameter(), Projector2.LowerDistanceParameter());
    if (Start)
    {
      PInt.SetX(myBis.FirstParameter());
      Params.Prepend(PInt);
    }
    else
    {
      PInt.SetX(myBis.LastParameter());
      Params.Append(PInt);
    }
  }
}

//=================================================================================================

bool BRepFill_TrimEdgeTool::IsInside(const gp_Pnt2d& P) const
{
  //  Modified by Sergey KHROMOV - Fri Sep 27 11:43:12 2002 Begin
  //   double Dist;
  double Dist = RealLast();
  //  Modified by Sergey KHROMOV - Fri Sep 27 11:43:12 2002 End
  if (isPoint1)
    Dist = P.Distance(myP1);
  else if (isPoint2)
    Dist = P.Distance(myP2);
  else
  {
    Geom2dAPI_ProjectPointOnCurve Projector(P, myC1);
    if (Projector.NbPoints() > 0)
    {
      Dist = Projector.LowerDistance();
    }
    //  Modified by Sergey KHROMOV - Fri Sep 27 11:43:43 2002 Begin
    //     else {
    //       gp_Pnt2d PF = myC1->Value(myC1->FirstParameter());
    //       gp_Pnt2d PL = myC1->Value(myC1->LastParameter());
    //       Dist = Min (P.Distance(PF),P.Distance(PL));
    //     }

    // Check of distances between P and first and last point of the first curve
    // should be performed in any case, despite of the results of projection.
    gp_Pnt2d      PF       = myC1->Value(myC1->FirstParameter());
    gp_Pnt2d      PL       = myC1->Value(myC1->LastParameter());
    double aDistMin = std::min(P.Distance(PF), P.Distance(PL));

    if (Dist > aDistMin)
      Dist = aDistMin;
    //  Modified by Sergey KHROMOV - Fri Sep 27 11:43:44 2002 End
  }

  //  return (Dist < std::abs(myOffset);
  // return (Dist < std::abs(myOffset) + Precision::Confusion());
  return (Dist < std::abs(myOffset) - Precision::Confusion());
}
