// Created on: 1996-04-22
// Created by: Herve LOUESSARD
// Copyright (c) 1996-1999 Matra Datavision
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

// Modified: Sergey ZERCHANINOV

#include <BRepExtrema_DistanceSS.hxx>

#include <TopExp.hxx>
#include <gp_Pnt.hxx>
#include <gp_Pnt2d.hxx>
#include <BRep_Tool.hxx>
#include <BRepExtrema_SupportType.hxx>
#include <Standard_Real.hxx>
#include <BRepExtrema_SolutionElem.hxx>
#include <NCollection_Sequence.hxx>
#include <BRepExtrema_SolutionElem.hxx>
#include <Standard_Boolean.hxx>
#include <Standard_Integer.hxx>
#include <TopAbs_ShapeEnum.hxx>
#include <TopoDS.hxx>
#include <Bnd_Box.hxx>
#include <BRepExtrema_ExtPC.hxx>
#include <BRepExtrema_ExtPF.hxx>
#include <BRepExtrema_ExtCC.hxx>
#include <BRepExtrema_ExtCF.hxx>
#include <BRepExtrema_ExtFF.hxx>
#include <BRepClass_FaceClassifier.hxx>
#include <Geom_Curve.hxx>
#include <GeomAbs_Shape.hxx>
#include <GeomAdaptor_Curve.hxx>
#include <NCollection_Array1.hxx>
#include <BRepBuilderAPI_MakeVertex.hxx>
#include <BRepBuilderAPI_MakeEdge.hxx>
#include <BRepBuilderAPI_MakeFace.hxx>
#include <Geom_Surface.hxx>
#include <GeomAPI_ProjectPointOnSurf.hxx>
#include <GeomAPI_ProjectPointOnCurve.hxx>
#include <Geom_RectangularTrimmedSurface.hxx>
#include <Geom_TrimmedCurve.hxx>
#include <BRepBndLib.hxx>
#include <BRepTools.hxx>

/*********************************************************************************/

//=================================================================================================

static bool TRI_SOLUTION(const NCollection_Sequence<BRepExtrema_SolutionElem>& SeqSol, const gp_Pnt& Pt)
{
  for (NCollection_Sequence<BRepExtrema_SolutionElem>::iterator anIt = SeqSol.begin(); anIt != SeqSol.end(); anIt++)
  {
    const double dst = anIt->Point().Distance(Pt);
    if (dst <= Precision::Confusion())
    {
      return false;
    }
  }
  return true;
}

//=================================================================================================

static void MIN_SOLUTION(const NCollection_Sequence<BRepExtrema_SolutionElem>& SeqSol1,
                         const NCollection_Sequence<BRepExtrema_SolutionElem>& SeqSol2,
                         const double              DstRef,
                         const double              Eps,
                         NCollection_Sequence<BRepExtrema_SolutionElem>&       seqSol1,
                         NCollection_Sequence<BRepExtrema_SolutionElem>&       seqSol2)
{
  for (NCollection_Sequence<BRepExtrema_SolutionElem>::iterator anIt1 = SeqSol1.begin(), anIt2 = SeqSol2.begin();
       anIt1 != SeqSol1.end();
       anIt1++, anIt2++)
  {
    const double dst1 = anIt1->Dist();
    if (fabs(dst1 - DstRef) < Eps)
    {
      seqSol1.Append(*anIt1);
      seqSol2.Append(*anIt2);
    }
  }
}

//=================================================================================================

static void TRIM_INFINIT_EDGE(const TopoDS_Edge& S1,
                              const TopoDS_Edge& S2,
                              TopoDS_Edge&       aResEdge,
                              bool&  bIsTrim1,
                              bool&  bIsTrim2)
{
  if (BRep_Tool::Degenerated(S1) || BRep_Tool::Degenerated(S2))
    return;

  aResEdge = S2;
  double      aFirst1, aLast1, aFirst2, aLast2;
  occ::handle<Geom_Curve> pCurv1 = BRep_Tool::Curve(S1, aFirst1, aLast1);
  occ::handle<Geom_Curve> pCurv2 = BRep_Tool::Curve(S2, aFirst2, aLast2);

  if (Precision::IsInfinite(aFirst1) && Precision::IsInfinite(aLast1)
      && Precision::IsInfinite(aFirst2) && Precision::IsInfinite(aLast2))
    return;

  double    Umin = 0., Umax = 0.;
  bool bUmin, bUmax;
  bUmin = bUmax = false;

  occ::handle<Geom_Curve> pCurv;
  if (!pCurv1.IsNull() && (Precision::IsInfinite(aFirst1) || Precision::IsInfinite(aLast1)))
  {
    pCurv    = pCurv1;
    bIsTrim1 = true;
    if (!Precision::IsInfinite(aFirst1))
    {
      bUmin = true;
      Umin  = aFirst1;
    }
    else if (!Precision::IsInfinite(aLast1))
    {
      bUmax = true;
      Umax  = aLast1;
    }
  }
  else if (!pCurv2.IsNull() && (Precision::IsInfinite(aFirst2) || Precision::IsInfinite(aLast2)))
  {
    pCurv    = pCurv2;
    bIsTrim2 = true;
    if (!Precision::IsInfinite(aFirst2))
    {
      bUmin = true;
      Umin  = aFirst2;
    }
    else if (!Precision::IsInfinite(aLast2))
    {
      bUmax = true;
      Umax  = aLast2;
    }
  }
  if (bIsTrim1 || bIsTrim2)
  {
    Bnd_Box aEdgeBox;
    if (bIsTrim1)
      BRepBndLib::Add(S2, aEdgeBox);
    if (bIsTrim2)
      BRepBndLib::Add(S1, aEdgeBox);
    double Xmin, Ymin, Zmin, Xmax, Ymax, Zmax;
    aEdgeBox.Get(Xmin, Ymin, Zmin, Xmax, Ymax, Zmax);

    const gp_Pnt aPnt0(Xmin, Ymin, Zmin);
    const gp_Pnt aPnt1(Xmin, Ymax, Zmin);
    const gp_Pnt aPnt2(Xmin, Ymax, Zmax);
    const gp_Pnt aPnt3(Xmin, Ymin, Zmax);
    const gp_Pnt aPnt4(Xmax, Ymax, Zmin);
    const gp_Pnt aPnt5(Xmax, Ymax, Zmax);
    const gp_Pnt aPnt6(Xmax, Ymin, Zmax);
    const gp_Pnt aPnt7(Xmax, Ymin, Zmin);

    double               arrU[8];
    GeomAPI_ProjectPointOnCurve aProj(aPnt0, pCurv);
    /*szv:aProj.Perform(aPnt0);*/ arrU[0] = aProj.LowerDistanceParameter();
    aProj.Perform(aPnt1);
    arrU[1] = aProj.LowerDistanceParameter();
    aProj.Perform(aPnt2);
    arrU[2] = aProj.LowerDistanceParameter();
    aProj.Perform(aPnt3);
    arrU[3] = aProj.LowerDistanceParameter();
    aProj.Perform(aPnt4);
    arrU[4] = aProj.LowerDistanceParameter();
    aProj.Perform(aPnt5);
    arrU[5] = aProj.LowerDistanceParameter();
    aProj.Perform(aPnt6);
    arrU[6] = aProj.LowerDistanceParameter();
    aProj.Perform(aPnt7);
    arrU[7] = aProj.LowerDistanceParameter();

    if (!bUmin)
      Umin = arrU[0];

    if (!bUmax)
      Umax = arrU[0];

    int i = 0;
    while (i < 8)
    {
      const double aU = arrU[i++];
      if (aU < Umin)
        Umin = aU;
      else if (aU > Umax)
        Umax = aU;
    }

    double tol = Precision::Confusion();
    if (bIsTrim1)
      tol = BRep_Tool::Tolerance(S1);
    else if (bIsTrim2)
      tol = BRep_Tool::Tolerance(S2);

    const double EpsU = GeomAdaptor_Curve(pCurv).Resolution(3. * tol);
    if (fabs(Umin - Umax) < EpsU)
    {
      Umin -= EpsU;
      Umax += EpsU;
    }

    occ::handle<Geom_Curve> result = new Geom_TrimmedCurve(pCurv, Umin, Umax);
    aResEdge                  = BRepBuilderAPI_MakeEdge(result);
  }
}

//=================================================================================================

static void TRIM_INFINIT_FACE(const TopoDS_Shape& S1,
                              const TopoDS_Shape& S2,
                              TopoDS_Face&        aResFace,
                              bool&   bIsInfinit)
{
  bIsInfinit = false;

  TopAbs_ShapeEnum Type1 = S1.ShapeType();
  TopAbs_ShapeEnum Type2 = S2.ShapeType();

  TopoDS_Edge aE;
  TopoDS_Face aF;

  if (Type1 == TopAbs_EDGE && Type2 == TopAbs_FACE)
  {
    aE = TopoDS::Edge(S1);
    if (BRep_Tool::Degenerated(aE))
      return;
    aF = TopoDS::Face(S2);
  }
  else if (Type2 == TopAbs_EDGE && Type1 == TopAbs_FACE)
  {
    aE = TopoDS::Edge(S2);
    if (BRep_Tool::Degenerated(aE))
      return;
    aF = TopoDS::Face(S1);
  }
  else
  {
    bIsInfinit = false;
    return;
  }

  aResFace                   = aF;
  occ::handle<Geom_Surface> pSurf = BRep_Tool::Surface(aF);

  const bool bRestrict = BRep_Tool::NaturalRestriction(aF);

  double    U1, V1, U2, V2;
  double    Umin = RealLast(), Umax = RealFirst(), Vmin = RealLast(), Vmax = RealFirst();
  bool bUmin, bUmax, bVmin, bVmax;
  bUmin = bUmax = bVmin = bVmax = false;
  bool bIsTrim      = false;

  if (bRestrict)
  {
    pSurf->Bounds(U1, U2, V1, V2);
    if (Precision::IsInfinite(U1))
      bIsTrim = true;
    else
    {
      Umin  = U1;
      bUmin = true;
    }

    if (Precision::IsInfinite(U2))
      bIsTrim = true;
    else
    {
      Umax  = U2;
      bUmax = true;
    }

    if (Precision::IsInfinite(V1))
      bIsTrim = true;
    else
    {
      Vmin  = V1;
      bVmin = true;
    }

    if (Precision::IsInfinite(V2))
      bIsTrim = true;
    else
    {
      Vmax  = V2;
      bVmax = true;
    }
  }
  else
  {
    BRepTools::UVBounds(aF, U1, U2, V1, V2);
    if (Precision::IsInfinite(U1) && Precision::IsInfinite(U2) && Precision::IsInfinite(V1)
        && Precision::IsInfinite(V2))
      bIsTrim = true;
  }

  if (bIsTrim)
  {
    Bnd_Box aEdgeBox;
    BRepBndLib::Add(aE, aEdgeBox);

    if (aEdgeBox.IsWhole())
      return;

    double Xmin, Ymin, Zmin, Xmax, Ymax, Zmax;
    aEdgeBox.Get(Xmin, Ymin, Zmin, Xmax, Ymax, Zmax);

    const gp_Pnt aPnt0(Xmin, Ymin, Zmin);
    const gp_Pnt aPnt1(Xmin, Ymax, Zmin);
    const gp_Pnt aPnt2(Xmin, Ymax, Zmax);
    const gp_Pnt aPnt3(Xmin, Ymin, Zmax);
    const gp_Pnt aPnt4(Xmax, Ymax, Zmin);
    const gp_Pnt aPnt5(Xmax, Ymax, Zmax);
    const gp_Pnt aPnt6(Xmax, Ymin, Zmax);
    const gp_Pnt aPnt7(Xmax, Ymin, Zmin);

    double              arrU[8], arrV[8];
    GeomAPI_ProjectPointOnSurf aProj(aPnt0, pSurf);
    /*szv:aProj.Perform(aPnt0);*/ if (aProj.IsDone())
      aProj.LowerDistanceParameters(arrU[0], arrV[0]);
    aProj.Perform(aPnt1);
    if (aProj.IsDone())
      aProj.LowerDistanceParameters(arrU[1], arrV[1]);
    aProj.Perform(aPnt2);
    if (aProj.IsDone())
      aProj.LowerDistanceParameters(arrU[2], arrV[2]);
    aProj.Perform(aPnt3);
    if (aProj.IsDone())
      aProj.LowerDistanceParameters(arrU[3], arrV[3]);
    aProj.Perform(aPnt4);
    if (aProj.IsDone())
      aProj.LowerDistanceParameters(arrU[4], arrV[4]);
    aProj.Perform(aPnt5);
    if (aProj.IsDone())
      aProj.LowerDistanceParameters(arrU[5], arrV[5]);
    aProj.Perform(aPnt6);
    if (aProj.IsDone())
      aProj.LowerDistanceParameters(arrU[6], arrV[6]);
    aProj.Perform(aPnt7);
    if (aProj.IsDone())
      aProj.LowerDistanceParameters(arrU[7], arrV[7]);

    if (!bUmin)
      Umin = arrU[0];
    if (!bUmax)
      Umax = arrU[0];
    if (!bVmin)
      Vmin = arrV[0];
    if (!bVmax)
      Vmax = arrV[0];

    int i = 0;
    while (i < 8)
    {
      const double aU = arrU[i];
      if (aU < Umin)
        Umin = aU;
      else if (aU > Umax)
        Umax = aU;

      const double aV = arrV[i];
      if (aV < Vmin)
        Vmin = aV;
      else if (aV > Vmax)
        Vmax = aV;

      i++;
    }

    GeomAdaptor_Surface aGAS(pSurf);
    const double tol = BRep_Tool::Tolerance(aF);

    const double EpsU = aGAS.UResolution(3. * tol);
    if (fabs(Umin - Umax) < EpsU)
    {
      Umin -= EpsU;
      Umax += EpsU;
    }

    const double EpsV = aGAS.VResolution(3. * tol);
    if (fabs(Vmin - Vmax) < EpsV)
    {
      Vmin -= EpsV;
      Vmax += EpsV;
    }

    occ::handle<Geom_Surface> result = new Geom_RectangularTrimmedSurface(pSurf, Umin, Umax, Vmin, Vmax);
    aResFace                    = BRepBuilderAPI_MakeFace(result, Precision::Confusion());

    bIsInfinit = true;
  }
  else
    bIsInfinit = false;
}

//=================================================================================================

static void PERFORM_C0(const TopoDS_Edge&         S1,
                       const TopoDS_Edge&         S2,
                       NCollection_Sequence<BRepExtrema_SolutionElem>& SeqSol1,
                       NCollection_Sequence<BRepExtrema_SolutionElem>& SeqSol2,
                       const double        DstRef,
                       double&             mDstRef,
                       const double        Eps)
{
  if (BRep_Tool::Degenerated(S1) || BRep_Tool::Degenerated(S2))
    return;

  int iE;
  for (iE = 0; iE < 2; iE++)
  {
    TopoDS_Edge E, Eother;
    if (iE == 0)
    {
      E      = S1;
      Eother = S2;
    }
    else
    {
      E      = S2;
      Eother = S1;
    }

    double      aFirst, aLast;
    occ::handle<Geom_Curve> pCurv = BRep_Tool::Curve(E, aFirst, aLast);

    double      aFOther, aLOther;
    occ::handle<Geom_Curve> pCurvOther = BRep_Tool::Curve(Eother, aFOther, aLOther);

    if (pCurv->Continuity() == GeomAbs_C0)
    {
      constexpr double epsP = Precision::PConfusion();

      GeomAdaptor_Curve      aAdaptorCurve(pCurv, aFirst, aLast);
      const int nbIntervals = aAdaptorCurve.NbIntervals(GeomAbs_C1);

      NCollection_Array1<double> arrInter(1, 1 + nbIntervals);
      aAdaptorCurve.Intervals(arrInter, GeomAbs_C1);

      GeomAdaptor_Curve      aAdaptorCurveOther(pCurvOther, aFOther, aLOther);
      const int nbIntervalsOther = aAdaptorCurveOther.NbIntervals(GeomAbs_C1);
      NCollection_Array1<double>   arrInterOther(1, 1 + nbIntervalsOther);
      aAdaptorCurveOther.Intervals(arrInterOther, GeomAbs_C1);

      double Udeb, Ufin;
      BRep_Tool::Range(Eother, Udeb, Ufin);

      gp_Pnt                   Pt;
      int         i, ii;
      BRepClass_FaceClassifier classifier;
      for (i = 1; i <= arrInter.Length(); i++)
      {
        const double aParameter = arrInter(i);
        const gp_Pnt        aPnt       = aAdaptorCurve.Value(aParameter);
        const TopoDS_Vertex V1         = BRepBuilderAPI_MakeVertex(aPnt);

        BRepExtrema_ExtPC      Ext(V1, Eother);
        const int NbExtrema = Ext.IsDone() ? Ext.NbExt() : 0;
        if (NbExtrema > 0)
        {
          // Search minimum distance dstmin
          double Dstmin = Ext.SquareDistance(1);
          for (ii = 2; ii <= NbExtrema; ii++)
          {
            const double sDst = Ext.SquareDistance(ii);
            if (sDst < Dstmin)
              Dstmin = sDst;
          }
          Dstmin = sqrt(Dstmin);

          if ((Dstmin < DstRef - Eps) || (fabs(Dstmin - DstRef) < Eps))
            for (ii = 1; ii <= NbExtrema; ii++)
            {
              if (fabs(Dstmin - sqrt(Ext.SquareDistance(ii))) < Eps)
              {
                Pt = Ext.Point(ii);
                // Pt - point on the curve pCurvOther/Eother, but
                // if iE == 0 -> Eother correspond to edge S2
                // and to edge S1 in the opposite case.
                // Therefore we should search Pt through previous solution points on Other curve
                // (edge): if iE == 0 - on edge S2, namely through SeqSol2, else       - on edge S1,
                // namely through SeqSol1.
                const bool triSolutionResult =
                  (iE == 0) ? TRI_SOLUTION(SeqSol2, Pt) : TRI_SOLUTION(SeqSol1, Pt);
                if (triSolutionResult)
                {
                  // Check if the parameter does not correspond to a vertex
                  const double t = Ext.Parameter(ii);
                  if ((fabs(t - Udeb) >= epsP) && (fabs(t - Ufin) > epsP))
                  {
                    if (mDstRef > Dstmin)
                      mDstRef = Dstmin;
                    const BRepExtrema_SolutionElem Sol1(Dstmin,
                                                        aPnt,
                                                        BRepExtrema_IsOnEdge,
                                                        E,
                                                        aParameter);
                    const BRepExtrema_SolutionElem Sol2(Dstmin,
                                                        Pt,
                                                        BRepExtrema_IsOnEdge,
                                                        Eother,
                                                        t);
                    SeqSol1.Append(iE == 0 ? Sol1 : Sol2);
                    SeqSol2.Append(iE == 0 ? Sol2 : Sol1);
                  }
                }
              }
            }
        }
        for (int i2 = 1; i2 <= arrInterOther.Length(); i2++)
        {
          const double aParameterOther = arrInterOther(i2);
          const gp_Pnt        aPntOther       = aAdaptorCurveOther.Value(aParameterOther);
          const double Dst             = aPnt.Distance(aPntOther);
          if ((Dst < DstRef - Eps) || (fabs(Dst - DstRef) < Eps))
          {
            if (mDstRef > Dst)
              mDstRef = Dst;
            const BRepExtrema_SolutionElem Sol1(Dst, aPnt, BRepExtrema_IsOnEdge, E, aParameter);
            const BRepExtrema_SolutionElem Sol2(Dst,
                                                aPntOther,
                                                BRepExtrema_IsOnEdge,
                                                Eother,
                                                aParameterOther);
            SeqSol1.Append(iE == 0 ? Sol1 : Sol2);
            SeqSol2.Append(iE == 0 ? Sol2 : Sol1);
          }
        }
      }
    }
  }
}

//=======================================================================
// function : isOnBoundary
// purpose  : Checks in 3d if the extrema point belongs to edge boundary
//=======================================================================
static bool isOnBoundary(const TopoDS_Edge&  theEdge,
                                     const gp_Pnt&       theSol,
                                     const double theParam,
                                     const double thePTol)
{
  for (TopoDS_Iterator it(theEdge); it.More(); it.Next())
  {
    const TopoDS_Vertex& aV      = TopoDS::Vertex(it.Value());
    double        aVParam = BRep_Tool::Parameter(aV, theEdge);
    if (std::abs(aVParam - theParam) < thePTol
        && BRep_Tool::Pnt(aV).Distance(theSol) < BRep_Tool::Tolerance(aV))
    {
      return true;
    }
  }
  return false;
}

//=================================================================================================

void BRepExtrema_DistanceSS::Perform(const TopoDS_Shape& theS1,
                                     const TopoDS_Shape& theS2,
                                     const Bnd_Box&      theBox1,
                                     const Bnd_Box&      theBox2)
{
  mySeqSolShape1.Clear();
  mySeqSolShape2.Clear();
  myModif = false;

  double aBBDist = theBox1.Distance(theBox2);
  if (aBBDist - myDstRef > myEps)
  {
    // The Box-Box distance is greater than the start distance.
    // The solution cannot be improved further.
    return;
  }

  switch (theS1.ShapeType())
  {
    case TopAbs_VERTEX: {
      TopoDS_Vertex aV1 = TopoDS::Vertex(theS1);
      switch (theS2.ShapeType())
      {
        case TopAbs_VERTEX: {
          TopoDS_Vertex aV2 = TopoDS::Vertex(theS2);
          Perform(aV1, aV2, mySeqSolShape1, mySeqSolShape2);
          break;
        }
        case TopAbs_EDGE: {
          TopoDS_Edge aE2 = TopoDS::Edge(theS2);
          Perform(aV1, aE2, mySeqSolShape1, mySeqSolShape2);
          break;
        }
        case TopAbs_FACE: {
          TopoDS_Face aF2 = TopoDS::Face(theS2);
          Perform(aV1, aF2, mySeqSolShape1, mySeqSolShape2);
          break;
        }
        default:
          break;
      }
      break;
    }
    case TopAbs_EDGE: {
      TopoDS_Edge aE1 = TopoDS::Edge(theS1);
      switch (theS2.ShapeType())
      {
        case TopAbs_VERTEX: {
          TopoDS_Vertex aV2 = TopoDS::Vertex(theS2);
          Perform(aV2, aE1, mySeqSolShape2, mySeqSolShape1);
          break;
        }
        case TopAbs_EDGE: {
          TopoDS_Edge      aE2 = TopoDS::Edge(theS2);
          TopoDS_Edge      aTrimEdge;
          bool bIsTrim1 = false;
          bool bIsTrim2 = false;
          TRIM_INFINIT_EDGE(aE1, aE2, aTrimEdge, bIsTrim1, bIsTrim2);
          if (bIsTrim1)
            aE1 = aTrimEdge;
          if (bIsTrim2)
            aE2 = aTrimEdge;
          Perform(aE1, aE2, mySeqSolShape1, mySeqSolShape2);
          break;
        }
        case TopAbs_FACE: {
          TopoDS_Face      aF2 = TopoDS::Face(theS2);
          TopoDS_Face      aTrimFace;
          bool bIsInfinit;
          TRIM_INFINIT_FACE(aE1, aF2, aTrimFace, bIsInfinit);
          if (bIsInfinit)
            aF2 = aTrimFace;
          Perform(aE1, aF2, mySeqSolShape1, mySeqSolShape2);
          break;
        }
        default:
          break;
      }
      break;
    }
    case TopAbs_FACE: {
      TopoDS_Face aF1 = TopoDS::Face(theS1);
      switch (theS2.ShapeType())
      {
        case TopAbs_VERTEX: {
          TopoDS_Vertex aV2 = TopoDS::Vertex(theS2);
          Perform(aV2, aF1, mySeqSolShape2, mySeqSolShape1);
          break;
        }
        case TopAbs_EDGE: {
          TopoDS_Edge      aE2 = TopoDS::Edge(theS2);
          TopoDS_Face      aTrimFace;
          bool bIsInfinit;
          TRIM_INFINIT_FACE(aF1, aE2, aTrimFace, bIsInfinit);
          if (bIsInfinit)
            aF1 = aTrimFace;
          Perform(aE2, aF1, mySeqSolShape2, mySeqSolShape1);
          break;
        }
        case TopAbs_FACE: {
          TopoDS_Face aF2 = TopoDS::Face(theS2);
          Perform(aF1, aF2, mySeqSolShape1, mySeqSolShape2);
          break;
        }
        default:
          break;
      }
      break;
    }
    default:
      break;
  }
}

//=======================================================================
// function : Perform
// purpose  : Vertex-Vertex
//=======================================================================
void BRepExtrema_DistanceSS::Perform(const TopoDS_Vertex&       theS1,
                                     const TopoDS_Vertex&       theS2,
                                     NCollection_Sequence<BRepExtrema_SolutionElem>& theSeqSolShape1,
                                     NCollection_Sequence<BRepExtrema_SolutionElem>& theSeqSolShape2)
{
  const gp_Pnt aP1 = BRep_Tool::Pnt(theS1);
  const gp_Pnt aP2 = BRep_Tool::Pnt(theS2);

  const double Dst = aP1.Distance(aP2);
  if ((Dst < myDstRef - myEps) || (fabs(Dst - myDstRef) < myEps))
  {
    if (myDstRef > Dst)
      myDstRef = Dst;
    myModif = true;
    const BRepExtrema_SolutionElem Sol1(Dst, aP1, BRepExtrema_IsVertex, theS1);
    const BRepExtrema_SolutionElem Sol2(Dst, aP2, BRepExtrema_IsVertex, theS2);
    theSeqSolShape1.Append(Sol1);
    theSeqSolShape2.Append(Sol2);
  }
}

//=======================================================================
// function : Perform
// purpose  : Vertex-Edge
//=======================================================================
void BRepExtrema_DistanceSS::Perform(const TopoDS_Vertex&       theS1,
                                     const TopoDS_Edge&         theS2,
                                     NCollection_Sequence<BRepExtrema_SolutionElem>& theSeqSolShape1,
                                     NCollection_Sequence<BRepExtrema_SolutionElem>& theSeqSolShape2)
{
  if (BRep_Tool::Degenerated(theS2))
    return;

  BRepExtrema_ExtPC      Ext(theS1, theS2);
  const int NbExtrema = Ext.IsDone() ? Ext.NbExt() : 0;
  if (NbExtrema > 0)
  {
    // Search minimum distance Dstmin
    int i;
    double    Dstmin = Ext.SquareDistance(1);
    for (i = 2; i <= NbExtrema; i++)
    {
      const double sDst = Ext.SquareDistance(i);
      if (sDst < Dstmin)
        Dstmin = sDst;
    }
    Dstmin = sqrt(Dstmin);
    if ((Dstmin < myDstRef - myEps) || (fabs(Dstmin - myDstRef) < myEps))
    {
      gp_Pnt                  Pt, P1 = BRep_Tool::Pnt(theS1);
      constexpr double epsP = Precision::PConfusion();

      for (i = 1; i <= NbExtrema; i++)
      {
        if (fabs(Dstmin - sqrt(Ext.SquareDistance(i))) < myEps)
        {
          Pt = Ext.Point(i);
          if (TRI_SOLUTION(theSeqSolShape2, Pt))
          {
            // Check if the parameter does not correspond to a vertex
            const double t = Ext.Parameter(i);
            if (!isOnBoundary(theS2, Pt, t, epsP))
            {
              if (myDstRef > Dstmin)
                myDstRef = Dstmin;
              myModif = true;
              const BRepExtrema_SolutionElem Sol1(Dstmin, P1, BRepExtrema_IsVertex, theS1);
              const BRepExtrema_SolutionElem Sol2(Dstmin, Pt, BRepExtrema_IsOnEdge, theS2, t);
              theSeqSolShape1.Append(Sol1);
              theSeqSolShape2.Append(Sol2);
            }
          }
        }
      }
    }
  }
}

//=======================================================================
// function : Perform
// purpose  : Vertex-Face
//=======================================================================
void BRepExtrema_DistanceSS::Perform(const TopoDS_Vertex&       theS1,
                                     const TopoDS_Face&         theS2,
                                     NCollection_Sequence<BRepExtrema_SolutionElem>& theSeqSolShape1,
                                     NCollection_Sequence<BRepExtrema_SolutionElem>& theSeqSolShape2)
{
  BRepExtrema_ExtPF      Ext(theS1, theS2, myFlag, myAlgo);
  const int NbExtrema = Ext.IsDone() ? Ext.NbExt() : 0;
  if (NbExtrema > 0)
  {
    // Search minimum distance Dstmin
    int i;
    double    Dstmin = Ext.SquareDistance(1);
    for (i = 2; i <= NbExtrema; i++)
    {
      const double sDst = Ext.SquareDistance(i);
      if (sDst < Dstmin)
        Dstmin = sDst;
    }
    Dstmin = sqrt(Dstmin);
    if ((Dstmin < myDstRef - myEps) || (fabs(Dstmin - myDstRef) < myEps))
    {
      double            U, V;
      gp_Pnt                   Pt, P1 = BRep_Tool::Pnt(theS1);
      BRepClass_FaceClassifier classifier;
      const double      tol = BRep_Tool::Tolerance(theS2);

      for (i = 1; i <= NbExtrema; i++)
      {
        if (fabs(Dstmin - sqrt(Ext.SquareDistance(i))) < myEps)
        {
          Pt = Ext.Point(i);
          if (TRI_SOLUTION(theSeqSolShape2, Pt))
          {
            // Check if the parameter does not correspond to a vertex
            Ext.Parameter(i, U, V);
            const gp_Pnt2d PUV(U, V);
            classifier.Perform(theS2, PUV, tol);
            if (classifier.State() == TopAbs_IN)
            {
              if (myDstRef > Dstmin)
                myDstRef = Dstmin;
              myModif = true;
              const BRepExtrema_SolutionElem Sol1(Dstmin, P1, BRepExtrema_IsVertex, theS1);
              const BRepExtrema_SolutionElem Sol2(Dstmin, Pt, BRepExtrema_IsInFace, theS2, U, V);
              theSeqSolShape1.Append(Sol1);
              theSeqSolShape2.Append(Sol2);
            }
          }
        }
      }
    }
  }
}

//=======================================================================
// function : Perform
// purpose  : Edge-Edge
//=======================================================================
void BRepExtrema_DistanceSS::Perform(const TopoDS_Edge&         theS1,
                                     const TopoDS_Edge&         theS2,
                                     NCollection_Sequence<BRepExtrema_SolutionElem>& theSeqSolShape1,
                                     NCollection_Sequence<BRepExtrema_SolutionElem>& theSeqSolShape2)
{
  if (BRep_Tool::Degenerated(theS1) || BRep_Tool::Degenerated(theS2))
    return;

  const double DstRef = myDstRef;

  BRepExtrema_ExtCC      Ext(theS1, theS2);
  const int NbExtrema = Ext.IsDone() ? (Ext.IsParallel() ? 0 : Ext.NbExt()) : 0;
  if (NbExtrema > 0)
  {
    // Search minimum distance Dstmin
    int i;
    double    Dstmin = Ext.SquareDistance(1);
    for (i = 2; i <= NbExtrema; i++)
    {
      const double sDst = Ext.SquareDistance(i);
      if (sDst < Dstmin)
        Dstmin = sDst;
    }
    Dstmin = sqrt(Dstmin);
    if ((Dstmin < myDstRef - myEps) || (fabs(Dstmin - myDstRef) < myEps))
    {
      gp_Pnt                  Pt1, Pt2;
      constexpr double epsP = Precision::PConfusion();

      for (i = 1; i <= NbExtrema; i++)
      {
        if (fabs(Dstmin - sqrt(Ext.SquareDistance(i))) < myEps)
        {
          Pt1 = Ext.PointOnE1(i);
          Pt2 = Ext.PointOnE2(i);
          if (TRI_SOLUTION(theSeqSolShape1, Pt1) || TRI_SOLUTION(theSeqSolShape2, Pt2))
          {
            // Check if the parameters do not correspond to a vertex
            const double t1 = Ext.ParameterOnE1(i);
            const double t2 = Ext.ParameterOnE2(i);

            if (!isOnBoundary(theS1, Pt1, t1, epsP) && !isOnBoundary(theS2, Pt2, t2, epsP))
            {
              if (myDstRef > Dstmin)
                myDstRef = Dstmin;
              myModif = true;
              const BRepExtrema_SolutionElem Sol1(Dstmin, Pt1, BRepExtrema_IsOnEdge, theS1, t1);
              const BRepExtrema_SolutionElem Sol2(Dstmin, Pt2, BRepExtrema_IsOnEdge, theS2, t2);
              theSeqSolShape1.Append(Sol1);
              theSeqSolShape2.Append(Sol2);
            }
          }
        }
      }
    }
  }

  NCollection_Sequence<BRepExtrema_SolutionElem> SeqSolution1;
  NCollection_Sequence<BRepExtrema_SolutionElem> SeqSolution2;

  PERFORM_C0(theS1, theS2, SeqSolution1, SeqSolution2, DstRef, myDstRef, myEps);

  NCollection_Sequence<BRepExtrema_SolutionElem> seqSol1;
  NCollection_Sequence<BRepExtrema_SolutionElem> seqSol2;

  if (SeqSolution1.Length() > 0 && SeqSolution2.Length() > 0)
    MIN_SOLUTION(SeqSolution1, SeqSolution2, myDstRef, myEps, seqSol1, seqSol2);

  if (!seqSol1.IsEmpty() && !seqSol2.IsEmpty())
  {
    NCollection_Sequence<BRepExtrema_SolutionElem>::iterator anIt1 = seqSol1.begin();
    NCollection_Sequence<BRepExtrema_SolutionElem>::iterator anIt2 = seqSol2.begin();
    for (; anIt1 != seqSol1.end() && anIt2 != seqSol2.end(); anIt1++, anIt2++)
    {
      gp_Pnt Pt1 = anIt1->Point();
      gp_Pnt Pt2 = anIt2->Point();
      if (TRI_SOLUTION(theSeqSolShape1, Pt1) || TRI_SOLUTION(theSeqSolShape2, Pt2))
      {
        theSeqSolShape1.Append(*anIt1);
        theSeqSolShape2.Append(*anIt2);
        myModif = true;
      }
    }
  }
}

//=======================================================================
// function : Perform
// purpose  : Edge-Face
//=======================================================================
void BRepExtrema_DistanceSS::Perform(const TopoDS_Edge&         theS1,
                                     const TopoDS_Face&         theS2,
                                     NCollection_Sequence<BRepExtrema_SolutionElem>& theSeqSolShape1,
                                     NCollection_Sequence<BRepExtrema_SolutionElem>& theSeqSolShape2)
{
  if (BRep_Tool::Degenerated(theS1))
    return;

  BRepClass_FaceClassifier classifier;

  BRepExtrema_ExtCF      Ext(theS1, theS2);
  const int NbExtrema = Ext.IsDone() ? (Ext.IsParallel() ? 0 : Ext.NbExt()) : 0;
  if (NbExtrema > 0)
  {
    // Search minimum distance Dstmin
    int i;
    double    Dstmin = Ext.SquareDistance(1);
    for (i = 2; i <= NbExtrema; i++)
    {
      const double sDst = Ext.SquareDistance(i);
      if (sDst < Dstmin)
        Dstmin = sDst;
    }
    Dstmin = sqrt(Dstmin);
    if ((Dstmin < myDstRef - myEps) || (fabs(Dstmin - myDstRef) < myEps))
    {
      double       U, V;
      const double tol = BRep_Tool::Tolerance(theS2);

      gp_Pnt                  Pt1, Pt2;
      constexpr double epsP = Precision::PConfusion();

      for (i = 1; i <= NbExtrema; i++)
      {
        if (fabs(Dstmin - sqrt(Ext.SquareDistance(i))) < myEps)
        {
          Pt1 = Ext.PointOnEdge(i);
          Pt2 = Ext.PointOnFace(i);
          if (TRI_SOLUTION(theSeqSolShape1, Pt1) || TRI_SOLUTION(theSeqSolShape2, Pt2))
          {
            // Check if the parameter does not correspond to a vertex
            const double t1 = Ext.ParameterOnEdge(i);
            if (!isOnBoundary(theS1, Pt1, t1, epsP))
            {
              Ext.ParameterOnFace(i, U, V);
              const gp_Pnt2d PUV(U, V);
              classifier.Perform(theS2, PUV, tol);
              if (classifier.State() == TopAbs_IN)
              {
                if (myDstRef > Dstmin)
                  myDstRef = Dstmin;
                myModif = true;
                const BRepExtrema_SolutionElem Sol1(Dstmin, Pt1, BRepExtrema_IsOnEdge, theS1, t1);
                const BRepExtrema_SolutionElem Sol2(Dstmin, Pt2, BRepExtrema_IsInFace, theS2, U, V);
                theSeqSolShape1.Append(Sol1);
                theSeqSolShape2.Append(Sol2);
              }
            }
          }
        }
      }
    }
  }

  double      aFirst, aLast;
  occ::handle<Geom_Curve> pCurv = BRep_Tool::Curve(theS1, aFirst, aLast);
  if (pCurv->Continuity() == GeomAbs_C0)
  {
    NCollection_Sequence<BRepExtrema_SolutionElem> SeqSolution1;
    NCollection_Sequence<BRepExtrema_SolutionElem> SeqSolution2;

    GeomAdaptor_Curve      aAdaptorCurve(pCurv, aFirst, aLast);
    const int nbIntervals = aAdaptorCurve.NbIntervals(GeomAbs_C1);

    NCollection_Array1<double> arrInter(1, 1 + nbIntervals);
    aAdaptorCurve.Intervals(arrInter, GeomAbs_C1);

    double       U, V;
    const double tol = BRep_Tool::Tolerance(theS2);

    int i;
    for (i = 1; i <= arrInter.Length(); i++)
    {
      const double aParameter = arrInter(i);
      gp_Pnt              aPnt       = aAdaptorCurve.Value(aParameter);
      TopoDS_Vertex       V1         = BRepBuilderAPI_MakeVertex(aPnt);

      BRepExtrema_ExtPF      ExtPF(V1, theS2);
      const int NbExtremaPF = ExtPF.IsDone() ? ExtPF.NbExt() : 0;
      if (NbExtremaPF > 0)
      {
        // Search minimum distance Dstmin
        int ii;
        double    Dstmin = ExtPF.SquareDistance(1);
        for (ii = 2; ii <= NbExtremaPF; ii++)
        {
          const double sDst = ExtPF.SquareDistance(ii);
          if (sDst < Dstmin)
            Dstmin = sDst;
        }
        Dstmin = sqrt(Dstmin);

        if ((Dstmin < myDstRef - myEps) || (fabs(Dstmin - myDstRef) < myEps))
        {
          for (ii = 1; ii <= NbExtremaPF; ii++)
          {
            if (fabs(Dstmin - sqrt(ExtPF.SquareDistance(ii))) < myEps)
            {
              // Check if the parameter does not correspond to a vertex
              ExtPF.Parameter(ii, U, V);
              const gp_Pnt2d PUV(U, V);
              classifier.Perform(theS2, PUV, tol);
              if (classifier.State() == TopAbs_IN)
              {
                if (myDstRef > Dstmin)
                  myDstRef = Dstmin;
                myModif = true;
                const BRepExtrema_SolutionElem Sol1(Dstmin,
                                                    aPnt,
                                                    BRepExtrema_IsOnEdge,
                                                    theS1,
                                                    aParameter);
                const BRepExtrema_SolutionElem Sol2(Dstmin,
                                                    ExtPF.Point(ii),
                                                    BRepExtrema_IsInFace,
                                                    theS2,
                                                    U,
                                                    V);
                SeqSolution1.Append(Sol1);
                SeqSolution2.Append(Sol2);
              }
            }
          }
        }
      }
    }

    NCollection_Sequence<BRepExtrema_SolutionElem> seqSol1;
    NCollection_Sequence<BRepExtrema_SolutionElem> seqSol2;
    if (SeqSolution1.Length() > 0 && SeqSolution2.Length() > 0)
      MIN_SOLUTION(SeqSolution1, SeqSolution2, myDstRef, myEps, seqSol1, seqSol2);

    if (!seqSol1.IsEmpty() && !seqSol2.IsEmpty())
    {
      theSeqSolShape1.Append(seqSol1);
      theSeqSolShape2.Append(seqSol2);
    }
  }
}

//=======================================================================
// function : Perform
// purpose  : Face-Face
//=======================================================================
void BRepExtrema_DistanceSS::Perform(const TopoDS_Face&         theS1,
                                     const TopoDS_Face&         theS2,
                                     NCollection_Sequence<BRepExtrema_SolutionElem>& theSeqSolShape1,
                                     NCollection_Sequence<BRepExtrema_SolutionElem>& theSeqSolShape2)
{
  BRepExtrema_ExtFF      Ext(theS1, theS2);
  const int NbExtrema = Ext.IsDone() ? (Ext.IsParallel() ? 0 : Ext.NbExt()) : 0;
  if (NbExtrema > 0)
  {
    // Search minimum distance Dstmin
    int i;
    double    Dstmin = Ext.SquareDistance(1);
    for (i = 2; i <= NbExtrema; i++)
    {
      const double sDst = Ext.SquareDistance(i);
      if (sDst < Dstmin)
        Dstmin = sDst;
    }
    Dstmin = sqrt(Dstmin);
    if ((Dstmin < myDstRef - myEps) || (fabs(Dstmin - myDstRef) < myEps))
    {
      const double tol1 = BRep_Tool::Tolerance(theS1);
      const double tol2 = BRep_Tool::Tolerance(theS2);

      gp_Pnt                   Pt1, Pt2;
      gp_Pnt2d                 PUV;
      double            U1, V1, U2, V2;
      BRepClass_FaceClassifier classifier;

      for (i = 1; i <= NbExtrema; i++)
      {
        if (fabs(Dstmin - sqrt(Ext.SquareDistance(i))) < myEps)
        {
          Pt1 = Ext.PointOnFace1(i);
          Pt2 = Ext.PointOnFace2(i);
          if (TRI_SOLUTION(theSeqSolShape1, Pt1) || TRI_SOLUTION(theSeqSolShape2, Pt2))
          {
            // Check if the parameter does not correspond to a vertex
            Ext.ParameterOnFace1(i, U1, V1);
            PUV.SetCoord(U1, V1);
            classifier.Perform(theS1, PUV, tol1);
            if (classifier.State() == TopAbs_IN)
            {
              Ext.ParameterOnFace2(i, U2, V2);
              PUV.SetCoord(U2, V2);
              classifier.Perform(theS2, PUV, tol2);
              if (classifier.State() == TopAbs_IN)
              {
                if (myDstRef > Dstmin)
                  myDstRef = Dstmin;
                myModif = true;
                const BRepExtrema_SolutionElem Sol1(Dstmin,
                                                    Pt1,
                                                    BRepExtrema_IsInFace,
                                                    theS1,
                                                    U1,
                                                    V1);
                const BRepExtrema_SolutionElem Sol2(Dstmin,
                                                    Pt2,
                                                    BRepExtrema_IsInFace,
                                                    theS2,
                                                    U2,
                                                    V2);
                theSeqSolShape1.Append(Sol1);
                theSeqSolShape2.Append(Sol2);
              }
            }
          }
        }
      }
    }
  }
}
