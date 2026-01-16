// Created on: 2000-11-16
// Created by: Peter KURNEV
// Copyright (c) 2000-2014 OPEN CASCADE SAS
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

#include <Bnd_Box.hxx>
#include <BRep_Builder.hxx>
#include <BRep_Tool.hxx>
#include <BRepAdaptor_Curve.hxx>
#include <Geom2d_Curve.hxx>
#include <Geom2d_TrimmedCurve.hxx>
#include <Geom2dAdaptor_Curve.hxx>
#include <Geom_Curve.hxx>
#include <Geom_Line.hxx>
#include <Geom_Surface.hxx>
#include <Geom_TrimmedCurve.hxx>
#include <GeomAbs_CurveType.hxx>
#include <GeomAdaptor_Curve.hxx>
#include <GeomAdaptor_Surface.hxx>
#include <GeomAPI_ProjectPointOnCurve.hxx>
#include <gp_Ax1.hxx>
#include <gp_Dir.hxx>
#include <gp_Lin.hxx>
#include <gp_Pln.hxx>
#include <gp_Pnt.hxx>
#include <gp_Pnt2d.hxx>
#include <IntTools_CommonPrt.hxx>
#include <IntTools_FClass2d.hxx>
#include <IntTools_Range.hxx>
#include <IntTools_Tools.hxx>
#include <Precision.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopoDS_Wire.hxx>

static void ParabolaTolerance(const occ::handle<Geom_Curve>&,
                              const double,
                              const double,
                              const double,
                              double&,
                              double&);

//=================================================================================================

bool IntTools_Tools::HasInternalEdge(const TopoDS_Wire& aW)
{
  bool bFlag = true;

  TopExp_Explorer anExp(aW, TopAbs_EDGE);
  for (; anExp.More(); anExp.Next())
  {
    const TopoDS_Edge& aE   = TopoDS::Edge(anExp.Current());
    TopAbs_Orientation anOr = aE.Orientation();
    if (anOr == TopAbs_INTERNAL)
    {
      return bFlag;
    }
  }
  return !bFlag;
}

//=================================================================================================

bool IntTools_Tools::IsClosed(const occ::handle<Geom_Curve>& aC3D)
{
  bool   bRet;
  double aF, aL, aDist, aPC;
  gp_Pnt aP1, aP2;

  occ::handle<Geom_BoundedCurve> aGBC = occ::down_cast<Geom_BoundedCurve>(aC3D);
  if (aGBC.IsNull())
  {
    return false;
  }

  aF = aC3D->FirstParameter();
  aL = aC3D->LastParameter();

  aC3D->D0(aF, aP1);
  aC3D->D0(aL, aP2);

  //
  aPC   = Precision::Confusion();
  aPC   = aPC * aPC;
  aDist = aP1.SquareDistance(aP2);
  bRet  = aDist < aPC;
  return bRet;
}

//=================================================================================================

void IntTools_Tools::RejectLines(const NCollection_Sequence<IntTools_Curve>& aSIn,
                                 NCollection_Sequence<IntTools_Curve>&       aSOut)
{
  int                     i, j, aNb;
  bool                    bFlag;
  occ::handle<Geom_Curve> aC3D;

  gp_Dir aD1, aD2;

  aSOut.Clear();

  aNb = aSIn.Length();
  for (i = 1; i <= aNb; i++)
  {
    const IntTools_Curve& IC = aSIn(i);
    aC3D                     = IC.Curve();
    //
    occ::handle<Geom_TrimmedCurve> aGTC = occ::down_cast<Geom_TrimmedCurve>(aC3D);

    if (!aGTC.IsNull())
    {
      aC3D                = aGTC->BasisCurve();
      IntTools_Curve* pIC = (IntTools_Curve*)&IC;
      pIC->SetCurve(aC3D);
    }
    //
    occ::handle<Geom_Line> aGLine = occ::down_cast<Geom_Line>(aC3D);

    if (aGLine.IsNull())
    {
      aSOut.Clear();
      for (j = 1; j <= aNb; j++)
      {
        aSOut.Append(aSIn(j));
      }
      return;
    }
    //
    gp_Lin aLin = aGLine->Lin();
    aD2         = aLin.Direction();
    if (i == 1)
    {
      aSOut.Append(IC);
      aD1 = aD2;
      continue;
    }

    bFlag = IntTools_Tools::IsDirsCoinside(aD1, aD2);
    if (!bFlag)
    {
      aSOut.Append(IC);
      return;
    }
  }
}

//=================================================================================================

bool IntTools_Tools::IsDirsCoinside(const gp_Dir& D1, const gp_Dir& D2)
{
  bool   bFlag;
  gp_Pnt P1(D1.X(), D1.Y(), D1.Z());
  gp_Pnt P2(D2.X(), D2.Y(), D2.Z());
  double dLim = 0.0002, d;
  d           = P1.Distance(P2);
  bFlag       = (d < dLim || fabs(2. - d) < dLim);
  return bFlag;
}

//=================================================================================================

bool IntTools_Tools::IsDirsCoinside(const gp_Dir& D1, const gp_Dir& D2, const double dLim)
{
  bool   bFlag;
  double d;
  //
  gp_Pnt P1(D1.X(), D1.Y(), D1.Z());
  gp_Pnt P2(D2.X(), D2.Y(), D2.Z());
  d     = P1.Distance(P2);
  bFlag = (d < dLim || fabs(2. - d) < dLim);
  return bFlag;
}

//=================================================================================================

int IntTools_Tools::SplitCurve(const IntTools_Curve& IC, NCollection_Sequence<IntTools_Curve>& aCvs)
{
  const occ::handle<Geom_Curve>& aC3D = IC.Curve();
  if (aC3D.IsNull())
    return 0;
  //
  const occ::handle<Geom2d_Curve>& aC2D1 = IC.FirstCurve2d();
  const occ::handle<Geom2d_Curve>& aC2D2 = IC.SecondCurve2d();
  bool                             bIsClosed;

  bIsClosed = IntTools_Tools::IsClosed(aC3D);
  if (!bIsClosed)
  {
    return 0;
  }

  double aF, aL, aMid;

  //
  aF   = aC3D->FirstParameter();
  aL   = aC3D->LastParameter();
  aMid = 0.5 * (aF + aL);
  GeomAdaptor_Curve aGAC(aC3D);
  GeomAbs_CurveType aCT = aGAC.GetType();
  if (aCT == GeomAbs_BSplineCurve || aCT == GeomAbs_BezierCurve)
  {
    // aMid=0.5*aMid;
    aMid = IntTools_Tools::IntermediatePoint(aF, aL);
  }
  //
  occ::handle<Geom_Curve> aC3DNewF, aC3DNewL;
  aC3DNewF = new Geom_TrimmedCurve(aC3D, aF, aMid);
  aC3DNewL = new Geom_TrimmedCurve(aC3D, aMid, aL);

  //
  occ::handle<Geom2d_Curve> aC2D1F, aC2D1L, aC2D2F, aC2D2L;
  //
  if (!aC2D1.IsNull())
  {
    aC2D1F = new Geom2d_TrimmedCurve(aC2D1, aF, aMid);
    aC2D1L = new Geom2d_TrimmedCurve(aC2D1, aMid, aL);
  }

  if (!aC2D2.IsNull())
  {
    aC2D2F = new Geom2d_TrimmedCurve(aC2D2, aF, aMid);
    aC2D2L = new Geom2d_TrimmedCurve(aC2D2, aMid, aL);
  }
  //
  IntTools_Curve aIC1(aC3DNewF, aC2D1F, aC2D2F, IC.Tolerance(), IC.TangentialTolerance());
  IntTools_Curve aIC2(aC3DNewL, aC2D1L, aC2D2L, IC.Tolerance(), IC.TangentialTolerance());
  //
  aCvs.Append(aIC1);
  //
  aCvs.Append(aIC2);
  //
  return 2;
}

//=================================================================================================

double IntTools_Tools::IntermediatePoint(const double aFirst, const double aLast)
{
  // define parameter division number as 10*e^(-M_PI) = 0.43213918
  constexpr double PAR_T = 0.43213918;
  return (1. - PAR_T) * aFirst + PAR_T * aLast;
}

//=================================================================================================

bool IntTools_Tools::IsVertex(const gp_Pnt& aP, const double aTolPV, const TopoDS_Vertex& aV)
{
  bool   bRet;
  double aTolV, aD, dTol;
  gp_Pnt aPv;

  aTolV = BRep_Tool::Tolerance(aV);
  //
  dTol  = Precision::Confusion();
  aTolV = aTolV + aTolPV + dTol;
  //
  aPv = BRep_Tool::Pnt(aV);
  //
  aD    = aPv.SquareDistance(aP);
  aTolV = aTolV * aTolV;
  bRet  = (aD <= aTolV);
  return bRet;
}

//=================================================================================================

bool IntTools_Tools::IsVertex(const IntTools_CommonPrt& aCmnPrt)
{
  bool   anIsVertex;
  double aParam;

  const TopoDS_Edge&    aE1 = aCmnPrt.Edge1();
  const IntTools_Range& aR1 = aCmnPrt.Range1();
  aParam                    = 0.5 * (aR1.First() + aR1.Last());
  anIsVertex                = IntTools_Tools::IsVertex(aE1, aParam);

  if (anIsVertex)
  {
    return true;
  }

  const TopoDS_Edge&                          aE2  = aCmnPrt.Edge2();
  const NCollection_Sequence<IntTools_Range>& aRs2 = aCmnPrt.Ranges2();
  const IntTools_Range&                       aR2  = aRs2(1);
  aParam                                           = 0.5 * (aR2.First() + aR2.Last());
  anIsVertex                                       = IntTools_Tools::IsVertex(aE2, aParam);
  return anIsVertex;
}

//=================================================================================================

bool IntTools_Tools::IsVertex(const TopoDS_Edge& aE, const TopoDS_Vertex& aV, const double t)
{
  double aTolV, aTolV2, d2;
  gp_Pnt aPv, aPt;

  BRepAdaptor_Curve aBAC(aE);
  aBAC.D0(t, aPt);

  aTolV  = BRep_Tool::Tolerance(aV);
  aTolV2 = aTolV * aTolV;
  aPv    = BRep_Tool::Pnt(aV);
  d2     = aPv.SquareDistance(aPt);
  return d2 < aTolV2;
}

//=================================================================================================

bool IntTools_Tools::IsVertex(const TopoDS_Edge& aE, const double t)
{
  double        aTolV, aTolV2, d2;
  TopoDS_Vertex aV;
  gp_Pnt        aPv, aPt;

  BRepAdaptor_Curve aBAC(aE);
  aBAC.D0(t, aPt);

  TopExp_Explorer anExp(aE, TopAbs_VERTEX);
  for (; anExp.More(); anExp.Next())
  {
    aV     = TopoDS::Vertex(anExp.Current());
    aTolV  = BRep_Tool::Tolerance(aV);
    aTolV2 = aTolV * aTolV;
    aTolV2 = 1.e-12;
    aPv    = BRep_Tool::Pnt(aV);
    d2     = aPv.SquareDistance(aPt);
    if (d2 < aTolV2)
    {
      return true;
    }
  }
  return false;
}

//=================================================================================================

int IntTools_Tools::ComputeVV(const TopoDS_Vertex& aV1, const TopoDS_Vertex& aV2)
{
  double aTolV1, aTolV2, aTolSum, d;
  gp_Pnt aP1, aP2;

  aTolV1  = BRep_Tool::Tolerance(aV1);
  aTolV2  = BRep_Tool::Tolerance(aV2);
  aTolSum = aTolV1 + aTolV2;

  aP1     = BRep_Tool::Pnt(aV1);
  aP2     = BRep_Tool::Pnt(aV2);
  aTolSum = aTolSum * aTolSum;
  d       = aP1.SquareDistance(aP2);
  if (d < aTolSum)
  {
    return 0;
  }
  return -1;
}

//=================================================================================================

void IntTools_Tools::MakeFaceFromWireAndFace(const TopoDS_Wire& aW,
                                             const TopoDS_Face& aF,
                                             TopoDS_Face&       aFNew)
{
  TopoDS_Face aFF;
  aFF = aF;
  aFF.Orientation(TopAbs_FORWARD);
  aFNew = TopoDS::Face(aFF.EmptyCopied());
  BRep_Builder BB;
  BB.Add(aFNew, aW);
}

//=================================================================================================

TopAbs_State IntTools_Tools::ClassifyPointByFace(const TopoDS_Face& aF, const gp_Pnt2d& aP2d)
{
  double       aFaceTolerance;
  TopAbs_State aState;

  aFaceTolerance = BRep_Tool::Tolerance(aF);
  IntTools_FClass2d aClass2d(aF, aFaceTolerance);
  aState = aClass2d.Perform(aP2d);

  return aState;
}

//=================================================================================================

bool IntTools_Tools::IsMiddlePointsEqual(const TopoDS_Edge& aE1, const TopoDS_Edge& aE2)

{
  bool   bRet;
  double f1, l1, m1, f2, l2, m2, aTol1, aTol2, aSumTol, aD2;
  gp_Pnt aP1, aP2;

  aTol1                      = BRep_Tool::Tolerance(aE1);
  occ::handle<Geom_Curve> C1 = BRep_Tool::Curve(aE1, f1, l1);
  m1                         = 0.5 * (f1 + l1);
  C1->D0(m1, aP1);

  aTol2                      = BRep_Tool::Tolerance(aE2);
  occ::handle<Geom_Curve> C2 = BRep_Tool::Curve(aE2, f2, l2);
  m2                         = 0.5 * (f2 + l2);
  C2->D0(m2, aP2);

  aSumTol = aTol1 + aTol2;
  aSumTol = aSumTol * aSumTol;
  aD2     = aP1.SquareDistance(aP2);
  bRet    = aD2 < aSumTol;
  return bRet;
}

//=================================================================================================

double IntTools_Tools::CurveTolerance(const occ::handle<Geom_Curve>& aC3D, const double aTolBase)
{
  double aTolReached, aTf, aTl, aTolMin, aTolMax;

  aTolReached = aTolBase;
  //
  if (aC3D.IsNull())
  {
    return aTolReached;
  }
  //
  occ::handle<Geom_TrimmedCurve> aCT3D = occ::down_cast<Geom_TrimmedCurve>(aC3D);
  if (aCT3D.IsNull())
  {
    return aTolReached;
  }
  //
  aTolMin = aTolBase;
  aTolMax = aTolBase;
  //
  aTf = aCT3D->FirstParameter();
  aTl = aCT3D->LastParameter();
  //
  GeomAdaptor_Curve aGAC(aCT3D);
  GeomAbs_CurveType aCType = aGAC.GetType();
  //
  if (aCType == GeomAbs_Parabola)
  {
    occ::handle<Geom_Curve> aC3DBase = aCT3D->BasisCurve();
    ParabolaTolerance(aC3DBase, aTf, aTl, aTolBase, aTolMin, aTolMax);
    aTolReached = aTolMax;
  }
  //
  return aTolReached;
}

#include <Geom_Parabola.hxx>
#include <gp_Parab.hxx>
#include <BndLib_Add3dCurve.hxx>
#include <BRepLib_CheckCurveOnSurface.hxx>

//=================================================================================================

void ParabolaTolerance(const occ::handle<Geom_Curve>& aC3D,
                       const double                   aTf,
                       const double                   aTl,
                       const double                   aTol,
                       double&                        aTolMin,
                       double&                        aTolMax)
{

  aTolMin = aTol;
  aTolMax = aTol;

  occ::handle<Geom_Parabola> aGP = occ::down_cast<Geom_Parabola>(aC3D);
  if (aGP.IsNull())
  {
    return;
  }

  int                    aNbPoints;
  double                 aFocal, aX1, aX2, aTol1, aTol2;
  gp_Pnt                 aPf, aPl;
  gp_Parab               aParab = aGP->Parab();
  gp_Ax1                 aXAxis = aParab.XAxis();
  occ::handle<Geom_Line> aGAxis = new Geom_Line(aXAxis);

  aFocal = aGP->Focal();
  if (aFocal == 0.)
  {
    return;
  }
  //
  // aTol1
  aTol1 = aTol;
  aX1   = 0.;
  aGP->D0(aTf, aPf);
  GeomAPI_ProjectPointOnCurve aProj1(aPf, aGAxis);
  aNbPoints = aProj1.NbPoints();
  if (aNbPoints)
  {
    aX1 = aProj1.LowerDistanceParameter();
  }
  if (aX1 >= 0.)
  {
    aTol1 = aTol * sqrt(0.5 * aX1 / aFocal);
  }
  if (aTol1 == 0.)
  {
    aTol1 = aTol;
  }
  //
  // aTol2
  aTol2 = aTol;
  aX2   = 0.;
  aGP->D0(aTl, aPl);
  GeomAPI_ProjectPointOnCurve aProj2(aPl, aGAxis);
  aNbPoints = aProj2.NbPoints();
  if (aNbPoints)
  {
    aX2 = aProj2.LowerDistanceParameter();
  }

  if (aX2 >= 0.)
  {
    aTol2 = aTol * sqrt(0.5 * aX2 / aFocal);
  }
  if (aTol2 == 0.)
  {
    aTol2 = aTol;
  }
  //
  aTolMax = (aTol1 > aTol2) ? aTol1 : aTol2;
  aTolMin = (aTol1 < aTol2) ? aTol1 : aTol2;
}

/////////////////////////////////////////////////////////////////////////
//=================================================================================================

bool IntTools_Tools::CheckCurve(const IntTools_Curve& theCurve, Bnd_Box& theBox)
{
  const occ::handle<Geom_Curve>& aC3D   = theCurve.Curve();
  bool                           bValid = !aC3D.IsNull();
  if (!bValid)
  {
    return bValid;
  }
  //
  // Build bounding box for the curve
  BndLib_Add3dCurve::Add(GeomAdaptor_Curve(aC3D),
                         std::max(theCurve.Tolerance(), theCurve.TangentialTolerance()),
                         theBox);
  //
  // Estimate the bounding box of the curve comparing it with the
  // minimal length for the curve from which the valid edge can be built -
  // 3*Precision::Confusion():
  // - 2 vertices with the Precision::Confusion() tolerance;
  // - plus Precision::Confusion() as the minimal distance between vertices.
  double aTolCmp = 3 * Precision::Confusion();
  //
  // Check the size of the box using the Bnd_Box::IsThin() method
  // which does not use the gap of the box.
  bValid = !theBox.IsThin(aTolCmp);
  //
  return bValid;
}

//=================================================================================================

bool IntTools_Tools::IsOnPave(const double          aT1,
                              const IntTools_Range& aRange,
                              const double          aTolerance)
{
  bool firstisonpave1, firstisonpave2, bIsOnPave;
  //
  firstisonpave1 = (std::abs(aRange.First() - aT1) < aTolerance);
  firstisonpave2 = (std::abs(aRange.Last() - aT1) < aTolerance);
  bIsOnPave      = (firstisonpave1 || firstisonpave2);
  return bIsOnPave;
}

//=================================================================================================

void IntTools_Tools::VertexParameters(const IntTools_CommonPrt& aCPart, double& aT1, double& aT2)
{
  const IntTools_Range& aR1 = aCPart.Range1();
  aT1                       = 0.5 * (aR1.First() + aR1.Last());
  //
  if ((aCPart.VertexParameter1() >= aR1.First()) && (aCPart.VertexParameter1() <= aR1.Last()))
  {
    aT1 = aCPart.VertexParameter1();
  }
  //
  const NCollection_Sequence<IntTools_Range>& aRanges2 = aCPart.Ranges2();
  const IntTools_Range&                       aR2      = aRanges2(1);
  aT2                                                  = 0.5 * (aR2.First() + aR2.Last());
  //
  if ((aCPart.VertexParameter2() >= aR2.First()) && (aCPart.VertexParameter2() <= aR2.Last()))
  {
    aT2 = aCPart.VertexParameter2();
  }
}

//=================================================================================================

void IntTools_Tools::VertexParameter(const IntTools_CommonPrt& aCPart, double& aT)
{
  const IntTools_Range& aR = aCPart.Range1();
  aT                       = 0.5 * (aR.First() + aR.Last());
  if ((aCPart.VertexParameter1() >= aR.First()) && (aCPart.VertexParameter1() <= aR.Last()))
  {
    aT = aCPart.VertexParameter1();
  }
}

//=================================================================================================

bool IntTools_Tools::IsOnPave1(const double          aTR,
                               const IntTools_Range& aCPRange,
                               const double          aTolerance)
{
  bool   bIsOnPave;
  double aT1, aT2, dT1, dT2;
  //
  aT1       = aCPRange.First();
  aT2       = aCPRange.Last();
  bIsOnPave = (aTR >= aT1 && aTR <= aT2);
  if (bIsOnPave)
  {
    return bIsOnPave;
  }
  //
  dT1       = std::abs(aTR - aT1);
  dT2       = std::abs(aTR - aT2);
  bIsOnPave = (dT1 <= aTolerance || dT2 <= aTolerance);
  return bIsOnPave;
}

//=================================================================================================

bool IntTools_Tools::IsInRange(const IntTools_Range& aRRef,
                               const IntTools_Range& aR,
                               const double          aTolerance)
{
  bool   bIsIn;
  double aT1, aT2, aTRef1, aTRef2;
  //
  aR.Range(aT1, aT2);
  aRRef.Range(aTRef1, aTRef2);
  //
  aTRef1 -= aTolerance;
  aTRef2 += aTolerance;
  //
  bIsIn = (aT1 >= aTRef1 && aT1 <= aTRef2) || (aT2 >= aTRef1 && aT2 <= aTRef2);
  //
  return bIsIn;
}

//=================================================================================================

int IntTools_Tools::SegPln(const gp_Lin& theLin,
                           const double  theTLin1,
                           const double  theTLin2,
                           const double  theTolLin,
                           const gp_Pln& thePln,
                           const double  theTolPln,
                           gp_Pnt&       theP,
                           double&       theTP,
                           double&       theTolP,
                           double&       theTPmin,
                           double&       theTPmax)
{
  int    iRet;
  double aTol, aA, aB, aC, aD, aE, aH, aTP, aDist1, aDist2;
  gp_Pnt aP1, aP2;
  //
  iRet = 0;
  aTol = theTolLin + theTolPln;
  //
  const gp_Ax3& aPosPln = thePln.Position();
  const gp_Dir& aDirPln = aPosPln.Direction();
  const gp_Pnt& aLocPln = aPosPln.Location();
  //
  const gp_Dir& aDirLin = theLin.Direction();
  const gp_Pnt& aLocLin = theLin.Location();
  //
  aP1.SetXYZ(aLocLin.XYZ() + theTLin1 * aDirLin.XYZ());
  aDist1 = aDirPln.X() * (aP1.X() - aLocPln.X()) + aDirPln.Y() * (aP1.Y() - aLocPln.Y())
           + aDirPln.Z() * (aP1.Z() - aLocPln.Z());
  //
  aP2.SetXYZ(aLocLin.XYZ() + theTLin2 * aDirLin.XYZ());
  aDist2 = aDirPln.X() * (aP2.X() - aLocPln.X()) + aDirPln.Y() * (aP2.Y() - aLocPln.Y())
           + aDirPln.Z() * (aP2.Z() - aLocPln.Z());
  //
  if (aDist1 < aTol && aDist2 < aTol)
  {
    iRet = 1; // common block
    return iRet;
  }
  //
  if (aDist1 * aDist2 > 0.)
  {
    iRet = 2; // segment lays on one side to the Plane
    return iRet;
  }
  //
  thePln.Coefficients(aA, aB, aC, aD);
  aE  = aA * aLocLin.X() + aB * aLocLin.Y() + aC * aLocLin.Z() + aD;
  aH  = aA * aDirLin.X() + aB * aDirLin.Y() + aC * aDirLin.Z();
  aTP = -aE / aH;
  if (aTP < theTLin1 - aTol || aTP > theTLin2 + aTol)
  {
    iRet = 3; // no intersections due to range of the Line
    return iRet;
  }
  //
  theTP = aTP;
  theP.SetXYZ(aLocLin.XYZ() + aTP * aDirLin.XYZ());
  theTolP  = aTol;
  theTPmin = theTP - theTolPln;
  theTPmax = theTP + theTolPln;
  iRet     = 0; // intersection point
  return iRet;
}

//=================================================================================================

bool IntTools_Tools::ComputeTolerance(const occ::handle<Geom_Curve>&   theCurve3D,
                                      const occ::handle<Geom2d_Curve>& theCurve2D,
                                      const occ::handle<Geom_Surface>& theSurf,
                                      const double                     theFirst,
                                      const double                     theLast,
                                      double&                          theMaxDist,
                                      double&                          theMaxPar,
                                      const double                     theTolRange,
                                      const bool                       theToRunParallel)
{
  GeomLib_CheckCurveOnSurface aCS;
  //
  const occ::handle<Adaptor3d_Curve> aGeomAdaptorCurve =
    new GeomAdaptor_Curve(theCurve3D, theFirst, theLast);

  occ::handle<Adaptor2d_Curve2d> aGeom2dAdaptorCurve =
    new Geom2dAdaptor_Curve(theCurve2D, theFirst, theLast);
  occ::handle<GeomAdaptor_Surface> aGeomAdaptorSurface = new GeomAdaptor_Surface(theSurf);

  occ::handle<Adaptor3d_CurveOnSurface> anAdaptor3dCurveOnSurface =
    new Adaptor3d_CurveOnSurface(aGeom2dAdaptorCurve, aGeomAdaptorSurface);

  aCS.Init(aGeomAdaptorCurve, theTolRange);
  aCS.SetParallel(theToRunParallel);
  aCS.Perform(anAdaptor3dCurveOnSurface);
  if (!aCS.IsDone())
  {
    return false;
  }

  // Obtaining precise result is impossible if we use
  // numeric methods for solution. Therefore, we must provide
  // some margin. Otherwise, in the future
  //(when geometrical properties of the curve will be changed,
  // e.g. after trimming) we will be able to come
  // to the more precise minimum point. As result, this curve with the
  // tolerance computed earlier will become invalid.
  const double anEps = (1.0 + 1.0e-5);
  theMaxDist         = anEps * aCS.MaxDistance();
  theMaxPar          = aCS.MaxParameter();
  //
  return true;
}

//=================================================================================================

double IntTools_Tools::ComputeIntRange(const double theTol1,
                                       const double theTol2,
                                       const double theAngle)
{
  double aDt;
  //
  if (std::abs(M_PI_2 - theAngle) < Precision::Angular())
  {
    aDt = theTol2;
  }
  else
  {
    double a1, a2, anAngle;
    //
    anAngle = (theAngle > M_PI_2) ? (M_PI - theAngle) : theAngle;
    a1      = theTol1 * tan(M_PI_2 - anAngle);
    a2      = theTol2 / sin(anAngle);
    aDt     = a1 + a2;
  }
  //
  return aDt;
}
