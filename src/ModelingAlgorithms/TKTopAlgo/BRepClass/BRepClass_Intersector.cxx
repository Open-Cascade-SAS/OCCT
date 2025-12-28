// Created on: 1992-11-19
// Created by: Remi LEQUETTE
// Copyright (c) 1992-1999 Matra Datavision
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

#include <Bnd_Box2d.hxx>
#include <BndLib_Add2dCurve.hxx>
#include <BRep_Tool.hxx>
#include <BRepAdaptor_Curve2d.hxx>
#include <BRepAdaptor_Surface.hxx>
#include <BRepClass_Edge.hxx>
#include <BRepClass_Intersector.hxx>
#include <ElCLib.hxx>
#include <Extrema_ExtPC2d.hxx>
#include <GCE2d_MakeSegment.hxx>
#include <Geom2d_Curve.hxx>
#include <Geom2d_Line.hxx>
#include <Geom2dInt_GInter.hxx>
#include <Geom2dLProp_CLProps2d.hxx>
#include <gp_Dir2d.hxx>
#include <gp_Lin2d.hxx>
#include <IntRes2d_Domain.hxx>
#include <IntRes2d_IntersectionPoint.hxx>
#include <IntRes2d_Transition.hxx>
#include <Precision.hxx>
#include <TopExp.hxx>
#include <TopoDS_Vertex.hxx>

static void GetTangentAsChord(const occ::handle<Geom2d_Curve>& thePCurve,
                              gp_Dir2d&                        theTangent,
                              const double                     theParam,
                              const double                     theFirst,
                              const double                     theLast);

static void RefineTolerance(const TopoDS_Face&         aF,
                            const Geom2dAdaptor_Curve& aC,
                            const double               aT,
                            double&                    aTolZ);

static bool CheckOn(IntRes2d_IntersectionPoint& thePntInter,
                    const TopoDS_Face&          theF,
                    const gp_Lin2d&             theL,
                    const Geom2dAdaptor_Curve&  theCur,
                    double&                     theTolZ,
                    const double                theFin,
                    const double                theDeb);

static void CheckSkip(Geom2dInt_GInter&                theInter,
                      const gp_Lin2d&                  theL,
                      const BRepClass_Edge&            theE,
                      const occ::handle<Geom2d_Curve>& theC2D,
                      const IntRes2d_Domain&           theDL,
                      Geom2dAdaptor_Curve&             theCur,
                      const Geom2dAdaptor_Curve&       theCGA,
                      double&                          theFin,
                      double&                          theDeb,
                      const double                     theMaxTol,
                      gp_Pnt2d&                        thePdeb,
                      gp_Pnt2d&                        thePfin);

static double MaxTol2DCurEdge(const TopoDS_Vertex& theV1,
                              const TopoDS_Vertex& theV2,
                              const TopoDS_Face&   theF,
                              const double         theTol);

static bool IsInter(Bnd_Box2d& theBox, const gp_Lin2d& theL, const double theP);

//=================================================================================================

BRepClass_Intersector::BRepClass_Intersector() {}

//=================================================================================================

double MaxTol2DCurEdge(const TopoDS_Vertex& theV1,
                       const TopoDS_Vertex& theV2,
                       const TopoDS_Face&   theF,
                       const double         theTol)
{
  double aTolV3D1, aTolV3D2;
  if (theV1.IsNull())
  {
    aTolV3D1 = 0.0;
  }
  else
  {
    aTolV3D1 = BRep_Tool::Tolerance(theV1);
  }
  if (theV2.IsNull())
  {
    aTolV3D2 = 0.0;
  }
  else
  {
    aTolV3D2 = BRep_Tool::Tolerance(theV2);
  }
  double aTol2D, anUr, aVr;

  double              aTolV3D = std::max(aTolV3D1, aTolV3D2);
  BRepAdaptor_Surface aS(theF, false);

  anUr   = aS.UResolution(aTolV3D);
  aVr    = aS.VResolution(aTolV3D);
  aTol2D = std::max(anUr, aVr);
  //
  aTol2D = std::max(aTol2D, theTol);
  return aTol2D;
}

//=================================================================================================

bool IsInter(Bnd_Box2d& theBox, const gp_Lin2d& theL, const double theP)
{
  bool aStatusInter = true;
  if (Precision::IsInfinite(theP))
  {
    aStatusInter = theBox.IsOut(theL);
  }
  else
  {
    gp_Pnt2d aPntF = theL.Location();
    gp_Pnt2d aPntL = ElCLib::Value(theP, theL);
    aStatusInter   = theBox.IsOut(aPntF, aPntL);
  }
  return !aStatusInter;
}

//=================================================================================================

bool CheckOn(IntRes2d_IntersectionPoint& thePntInter,
             const TopoDS_Face&          theF,
             const gp_Lin2d&             theL,
             const Geom2dAdaptor_Curve&  theCur,
             double&                     theTolZ,
             const double                theFin,
             const double                theDeb)
{
  Extrema_ExtPC2d anExtPC2d(theL.Location(), theCur);
  double          aMinDist = RealLast();
  int             aMinInd  = 0;
  if (anExtPC2d.IsDone())
  {
    const int aNbPnts = anExtPC2d.NbExt();
    for (int i = 1; i <= aNbPnts; ++i)
    {
      double aDist = anExtPC2d.SquareDistance(i);

      if (aDist < aMinDist)
      {
        aMinDist = aDist;
        aMinInd  = i;
      }
    }
  }

  if (aMinInd != 0)
  {
    aMinDist = std::sqrt(aMinDist);
  }
  if (aMinDist <= theTolZ)
  {
    gp_Pnt2d aPntExact = (anExtPC2d.Point(aMinInd)).Value();
    double   aPar      = (anExtPC2d.Point(aMinInd)).Parameter();
    //
    RefineTolerance(theF, theCur, aPar, theTolZ);
    //
    if (aMinDist <= theTolZ)
    {
      IntRes2d_Transition aTrOnLin(IntRes2d_Head);
      IntRes2d_Position   aPosOnCurve = IntRes2d_Middle;
      if ((std::abs(aPar - theDeb) <= Precision::Confusion()) || (aPar < theDeb))
      {
        aPosOnCurve = IntRes2d_Head;
      }
      else if ((std::abs(aPar - theFin) <= Precision::Confusion()) || (aPar > theFin))
      {
        aPosOnCurve = IntRes2d_End;
      }
      //
      IntRes2d_Transition aTrOnCurve(aPosOnCurve);
      thePntInter = IntRes2d_IntersectionPoint(aPntExact, 0., aPar, aTrOnLin, aTrOnCurve, false);
      //
      return true;
    }
  }
  return false;
}

//=================================================================================================

void CheckSkip(Geom2dInt_GInter&                theInter,
               const gp_Lin2d&                  theL,
               const BRepClass_Edge&            theE,
               const occ::handle<Geom2d_Curve>& theC2D,
               const IntRes2d_Domain&           theDL,
               Geom2dAdaptor_Curve&             theCur,
               const Geom2dAdaptor_Curve&       theCGA,
               double&                          theFin,
               double&                          theDeb,
               const double                     theMaxTol,
               gp_Pnt2d&                        thePdeb,
               gp_Pnt2d&                        thePfin)
{
  if (theE.Edge().IsNull() || theE.Face().IsNull())
  {
    return;
  }
  bool          anIsLSkip = false;
  TopoDS_Vertex aVl; // the last vertex of current edge

  occ::handle<Geom2d_Curve> aSkipC2D;

  aVl = TopExp::LastVertex(theE.Edge(), true);
  if (aVl.IsNull())
  {
    return;
  }
  if (!(BRep_Tool::Tolerance(aVl) > theMaxTol) || theE.NextEdge().IsNull())
  {
    return;
  }
  double                    aLdeb = 0.0, aLfin = 0.0;
  occ::handle<Geom2d_Curve> aLC2D; // the next curve

  aLC2D = BRep_Tool::CurveOnSurface(theE.NextEdge(), theE.Face(), aLdeb, aLfin);
  if (aLC2D.IsNull())
  {
    return;
  }
  double   anA, aB, aC;          // coefficients of the straight line
  double   aX1, anY1, aX2, anY2; // coordinates of the ends of edges
  gp_Pnt2d aP1, aP2;             // the ends of edges

  theL.Coefficients(anA, aB, aC);

  double at1 = theFin;
  if (theE.Edge().Orientation() != TopAbs_FORWARD)
  {
    at1 = theDeb;
  }

  double at2 = aLdeb;
  if (theE.NextEdge().Orientation() != TopAbs_FORWARD)
  {
    at2 = aLfin;
  }

  aP1 = theC2D->Value(at1);
  aP2 = aLC2D->Value(at2);

  // Check if points belong to DL domain
  double aPar1 = ElCLib::Parameter(theL, aP1);
  double aPar2 = ElCLib::Parameter(theL, aP2);

  if (!(aPar1 > theDL.FirstParameter() && aPar1 < theDL.LastParameter())
      || !(aPar2 > theDL.FirstParameter() && aPar2 < theDL.LastParameter()))
  {
    return;
  }
  aX1        = aP1.X();
  anY1       = aP1.Y();
  aX2        = aP2.X();
  anY2       = aP2.Y();
  double aFV = anA * aX1 + aB * anY1 + aC;
  double aSV = anA * aX2 + aB * anY2 + aC;

  // Check for getting into vertex with high tolerance
  if ((aFV * aSV) >= 0)
  {
    anIsLSkip = false;
  }
  else
  {
    anIsLSkip = true;
    GCE2d_MakeSegment aMkSeg(aP1, aP2);
    if (!aMkSeg.IsDone())
    {
      return;
    }
    aSkipC2D = aMkSeg.Value();

    if (aSkipC2D.IsNull() || !anIsLSkip)
    {
      return;
    }
    // if we got
    theCur.Load(aSkipC2D);
    if (theCur.Curve().IsNull())
    {
      return;
    }
    double atoldeb = 1.e-5, atolfin = 1.e-5;

    theDeb = theCur.FirstParameter();
    theFin = theCur.LastParameter();
    theCur.D0(theDeb, thePdeb);
    theCur.D0(theFin, thePfin);

    IntRes2d_Domain aDE(thePdeb, theDeb, atoldeb, thePfin, theFin, atolfin);
    // temporary periodic domain
    if (theCur.Curve()->IsPeriodic())
    {
      aDE.SetEquivalentParameters(theCur.FirstParameter(),
                                  theCur.FirstParameter() + theCur.Curve()->LastParameter()
                                    - theCur.Curve()->FirstParameter());
    }

    theInter = Geom2dInt_GInter(theCGA,
                                theDL,
                                theCur,
                                aDE,
                                Precision::PConfusion(),
                                Precision::PIntersection());
  }
}

//=================================================================================================

void BRepClass_Intersector::Perform(const gp_Lin2d&       L,
                                    const double          P,
                                    const double          Tol,
                                    const BRepClass_Edge& E)
{
  double                    deb = 0.0, fin = 0.0, aTolZ = Tol;
  occ::handle<Geom2d_Curve> aC2D;
  //
  const TopoDS_Edge& EE = E.Edge();
  const TopoDS_Face& F  = E.Face();

  //
  aC2D = BRep_Tool::CurveOnSurface(EE, F, deb, fin);
  if (aC2D.IsNull())
  {
    done = false; // !IsDone()
    return;
  }
  //
  Bnd_Box2d aBond;
  gp_Pnt2d  aPntF;
  bool      anUseBndBox = E.UseBndBox();
  if (anUseBndBox)
  {
    BndLib_Add2dCurve::Add(aC2D, deb, fin, 0., aBond);
    aBond.SetGap(aTolZ);
    aPntF = L.Location();
  }
  //
  Geom2dAdaptor_Curve C(aC2D, deb, fin);
  //
  // Case of "ON": direct check of belonging to edge
  // taking into account the tolerance
  if (!anUseBndBox || (anUseBndBox && !aBond.IsOut(aPntF)))
  {
    bool                       aStatusOn = false;
    IntRes2d_IntersectionPoint aPntInter;
    double                     aDebTol = deb;
    double                     aFinTol = fin;
    if (aTolZ > Precision::Confusion())
    {
      aDebTol = deb - aTolZ;
      aFinTol = fin + aTolZ;
    }
    Geom2dAdaptor_Curve aCurAdaptor(aC2D, aDebTol, aFinTol);

    aStatusOn = CheckOn(aPntInter, F, L, aCurAdaptor, aTolZ, fin, deb);
    if (aStatusOn)
    {
      Append(aPntInter);
      done = true;
      return;
    }
  }
  //
  if (anUseBndBox)
  {
    TopoDS_Vertex aVF, aVL;
    TopExp::Vertices(EE, aVF, aVL);

    aTolZ = MaxTol2DCurEdge(aVF, aVL, F, Tol);
    aBond.SetGap(aTolZ);

    if (!IsInter(aBond, L, P))
    {
      done = false;
      return;
    }
  }
  gp_Pnt2d pdeb, pfin;
  C.D0(deb, pdeb);
  C.D0(fin, pfin);
  double toldeb = 1.e-5, tolfin = 1.e-5;

  IntRes2d_Domain DL;
  //
  if (P != RealLast())
  {
    DL.SetValues(L.Location(),
                 0.,
                 Precision::PConfusion(),
                 ElCLib::Value(P, L),
                 P,
                 Precision::PConfusion());
  }
  else
  {
    DL.SetValues(L.Location(), 0., Precision::PConfusion(), true);
  }

  IntRes2d_Domain DE(pdeb, deb, toldeb, pfin, fin, tolfin);
  // temporary periodic domain
  if (C.Curve()->IsPeriodic())
  {
    DE.SetEquivalentParameters(C.FirstParameter(),
                               C.FirstParameter() + C.Curve()->LastParameter()
                                 - C.Curve()->FirstParameter());
  }

  occ::handle<Geom2d_Line> GL = new Geom2d_Line(L);
  Geom2dAdaptor_Curve      CGA(GL);
  Geom2dInt_GInter Inter(CGA, DL, C, DE, Precision::PConfusion(), Precision::PIntersection());
  //
  // The check is for hitting the intersector to
  // a vertex with high tolerance
  if (Inter.IsEmpty())
  {
    CheckSkip(Inter, L, E, aC2D, DL, C, CGA, fin, deb, E.MaxTolerance(), pdeb, pfin);
  }

  //
  SetValues(Inter);
}

//=================================================================================================

void BRepClass_Intersector::LocalGeometry(const BRepClass_Edge& E,
                                          const double          U,
                                          gp_Dir2d&             Tang,
                                          gp_Dir2d&             Norm,
                                          double&               C) const
{
  double                    fpar, lpar;
  occ::handle<Geom2d_Curve> aPCurve = BRep_Tool::CurveOnSurface(E.Edge(), E.Face(), fpar, lpar);
  Geom2dLProp_CLProps2d     Prop(aPCurve, U, 2, Precision::PConfusion());

  C = 0.;
  if (Prop.IsTangentDefined())
  {
    Prop.Tangent(Tang);
    C = Prop.Curvature();
  }
  else
    GetTangentAsChord(aPCurve, Tang, U, fpar, lpar);

  if (C > Precision::PConfusion() && !Precision::IsInfinite(C))
    Prop.Normal(Norm);
  else
    Norm.SetCoord(Tang.Y(), -Tang.X());
}

//=================================================================================================

void RefineTolerance(const TopoDS_Face&         aF,
                     const Geom2dAdaptor_Curve& aC,
                     const double               aT,
                     double&                    aTolZ)
{
  GeomAbs_SurfaceType aTypeS;
  //
  BRepAdaptor_Surface aBAS(aF, false);
  //
  aTypeS = aBAS.GetType();
  if (aTypeS == GeomAbs_Cylinder)
  {
    double   aURes, aVRes, aTolX;
    gp_Pnt2d aP2D;
    gp_Vec2d aV2D;
    //
    aURes = aBAS.UResolution(aTolZ);
    aVRes = aBAS.VResolution(aTolZ);
    //
    aC.D1(aT, aP2D, aV2D);
    gp_Dir2d aD2D(aV2D);
    //
    aTolX = aURes * aD2D.Y() + aVRes * aD2D.X();
    if (aTolX < 0.)
    {
      aTolX = -aTolX;
    }
    //
    if (aTolX < Precision::Confusion())
    {
      aTolX = Precision::Confusion();
    }
    //
    if (aTolX < aTolZ)
    {
      aTolZ = aTolX;
    }
  }
}

//=================================================================================================

void GetTangentAsChord(const occ::handle<Geom2d_Curve>& thePCurve,
                       gp_Dir2d&                        theTangent,
                       const double                     theParam,
                       const double                     theFirst,
                       const double                     theLast)
{
  double Offset = 0.1 * (theLast - theFirst);

  if (theLast - theParam < Precision::PConfusion()) // theParam == theLast
    Offset *= -1;
  else if (theParam + Offset > theLast) //<theParam> is close to <theLast>
    Offset = 0.5 * (theLast - theParam);

  gp_Pnt2d aPnt2d      = thePCurve->Value(theParam);
  gp_Pnt2d OffsetPnt2d = thePCurve->Value(theParam + Offset);

  gp_Vec2d aChord(aPnt2d, OffsetPnt2d);
  if (Offset < 0.)
    aChord.Reverse();

  double SqLength = aChord.SquareMagnitude();
  if (SqLength > Precision::SquarePConfusion())
    theTangent = aChord;
}
