// Created by: Peter KURNEV
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

#include <Bnd_Box.hxx>
#include <BOPTools_AlgoTools2D.hxx>
#include <BOPTools_AlgoTools3D.hxx>
#include <BRep_Builder.hxx>
#include <BRep_TEdge.hxx>
#include <BRep_TFace.hxx>
#include <BRep_Tool.hxx>
#include <BRepTools.hxx>
#include <Geom2d_Curve.hxx>
#include <Geom2d_Line.hxx>
#include <Geom2d_TrimmedCurve.hxx>
#include <Geom2dHatch_Hatcher.hxx>
#include <Geom_BezierSurface.hxx>
#include <Geom_BSplineSurface.hxx>
#include <Geom_RectangularTrimmedSurface.hxx>
#include <Geom_Surface.hxx>
#include <Geom2dAPI_ProjectPointOnCurve.hxx>
#include <gp_Dir.hxx>
#include <gp_Dir2d.hxx>
#include <gp_Pnt.hxx>
#include <gp_Pnt2d.hxx>
#include <gp_Vec.hxx>
#include <gp_Vec2d.hxx>
#include <HatchGen_Domain.hxx>
#include <IntTools_Context.hxx>
#include <IntTools_Tools.hxx>
#include <Poly_Triangulation.hxx>
#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Shape.hxx>
#include <TopTools_IndexedMapOfShape.hxx>

static void             Add(const TopoDS_Shape&         aS,
                            TopTools_IndexedMapOfShape& myShapes,
                            Standard_Boolean&           bHasGeometry);
static Standard_Boolean HasGeometry(const TopoDS_Shape& aS);

//=================================================================================================

Standard_Boolean BOPTools_AlgoTools3D::DoSplitSEAMOnFace(const TopoDS_Edge& aSplit,
                                                         const TopoDS_Face& aF)
{
  Standard_Boolean     bIsUPeriodic, bIsVPeriodic, bIsLeft;
  Standard_Real        anUPeriod = 0., anVPeriod = 0.;
  Standard_Real        aTol, a, b, aT, anU, dU, anU1;
  Standard_Real        aScPr, anV, dV, anV1;
  Standard_Real        aUmin, aUmax, aVmin, aVmax;
  gp_Pnt2d             aP2D;
  gp_Vec2d             aVec2D;
  Handle(Geom2d_Curve) aTmpC1, aTmpC2;
  Handle(Geom2d_Curve) C2D1;
  Handle(Geom2d_Line)  aLD1;
  Handle(Geom_Surface) aS;
  BRep_Builder         BB;
  TopoDS_Edge          aSp;
  //
  bIsLeft = Standard_False;
  aSp     = aSplit;
  aSp.Orientation(TopAbs_FORWARD);
  aTol = BRep_Tool::Tolerance(aSp);
  //
  aS = BRep_Tool::Surface(aF);
  //
  aS->Bounds(aUmin, aUmax, aVmin, aVmax);
  //

  bIsUPeriodic = aS->IsUClosed();
  bIsVPeriodic = aS->IsVClosed();

  if (bIsUPeriodic)
    anUPeriod = aUmax - aUmin;
  if (bIsVPeriodic)
    anVPeriod = aVmax - aVmin;

  //
  if (!bIsUPeriodic && !bIsVPeriodic)
  {

    Handle(Geom_RectangularTrimmedSurface) aRTS;
    aRTS = Handle(Geom_RectangularTrimmedSurface)::DownCast(aS);
    //
    if (aRTS.IsNull())
      return Standard_False;

    else
    {
      Handle(Geom_Surface) aSB;
      //
      aSB          = aRTS->BasisSurface();
      bIsUPeriodic = aSB->IsUPeriodic();
      bIsVPeriodic = aSB->IsVPeriodic();
      //

      if (bIsUPeriodic || bIsVPeriodic)
      {
        anUPeriod = bIsUPeriodic ? aSB->UPeriod() : 0.;
        anVPeriod = bIsVPeriodic ? aSB->VPeriod() : 0.;
      }
      else
      {
        Standard_Boolean bIsUClosed = aSB->IsUClosed();
        Standard_Boolean bIsVClosed = aSB->IsVClosed();
        Standard_Real    aGlobalUmin, aGlobalUmax, aGlobalVmin, aGlobalVmax;
        aSB->Bounds(aGlobalUmin, aGlobalUmax, aGlobalVmin, aGlobalVmax);

        if (bIsUClosed && Abs(aUmin - aGlobalUmin) < aTol && Abs(aUmax - aGlobalUmax) < aTol)
        {
          bIsUPeriodic = Standard_True;
          anUPeriod    = aUmax - aUmin;
        }
        if (bIsVClosed && Abs(aVmin - aGlobalVmin) < aTol && Abs(aVmax - aGlobalVmax) < aTol)
        {
          bIsVPeriodic = Standard_True;
          anVPeriod    = aVmax - aVmin;
        }
      }

      if (!(bIsUPeriodic || bIsVPeriodic))
      {
        return Standard_False;
      }
    } // if !RTS.IsNull
  }
  //
  //---------------------------------------------------
  C2D1 = BRep_Tool::CurveOnSurface(aSp, aF, a, b);
  //
  aT = BOPTools_AlgoTools2D::IntermediatePoint(a, b);
  C2D1->D1(aT, aP2D, aVec2D);
  gp_Dir2d aDir2D1(aVec2D), aDOX(-1., 0.), aDOY(0., 1.);
  //
  anU = aP2D.X();
  anV = aP2D.Y();
  //
  anU1 = anU;
  anV1 = anV;
  //
  GeomAdaptor_Surface aGAS(aS);
  dU = aGAS.UResolution(aTol);
  dV = aGAS.VResolution(aTol);
  //
  if (anUPeriod > 0.)
  {
    if (fabs(anU - aUmin) < dU)
    {
      bIsLeft = Standard_True;
      anU1    = anU + anUPeriod;
    }
    else if (fabs(anU - aUmax) < dU)
    {
      bIsLeft = Standard_False;
      anU1    = anU - anUPeriod;
    }
  }
  //
  if (anVPeriod > 0.)
  {
    if (fabs(anV - aVmin) < dV)
    {
      bIsLeft = Standard_True;
      anV1    = anV + anVPeriod;
    }
    else if (fabs(anV - aVmax) < dV)
    {
      bIsLeft = Standard_False;
      anV1    = anV - anVPeriod;
    }
  }
  //
  if (anU1 == anU && anV1 == anV)
  {
    return Standard_False;
  }
  //
  aScPr = (anU1 == anU) ? aDir2D1 * aDOX : aDir2D1 * aDOY;
  //
  aTmpC1                          = Handle(Geom2d_Curve)::DownCast(C2D1->Copy());
  Handle(Geom2d_TrimmedCurve) aC1 = new Geom2d_TrimmedCurve(aTmpC1, a, b);
  //
  aTmpC2                          = Handle(Geom2d_Curve)::DownCast(C2D1->Copy());
  Handle(Geom2d_TrimmedCurve) aC2 = new Geom2d_TrimmedCurve(aTmpC2, a, b);
  gp_Vec2d                    aTrV(anU1 - anU, anV1 - anV);
  aC2->Translate(aTrV);
  //
  if (!bIsLeft)
  {
    if (aScPr < 0.)
    {
      BB.UpdateEdge(aSp, aC2, aC1, aF, aTol);
    }
    else
    {
      BB.UpdateEdge(aSp, aC1, aC2, aF, aTol);
    }
  }
  else
  {
    if (aScPr < 0.)
    {
      BB.UpdateEdge(aSp, aC1, aC2, aF, aTol);
    }
    else
    {
      BB.UpdateEdge(aSp, aC2, aC1, aF, aTol);
    }
  }
  return Standard_True;
}

//=================================================================================================

Standard_Boolean BOPTools_AlgoTools3D::DoSplitSEAMOnFace(const TopoDS_Edge& theEOrigin,
                                                         const TopoDS_Edge& theESplit,
                                                         const TopoDS_Face& theFace)
{
  if (!BRep_Tool::IsClosed(theEOrigin, theFace))
    return Standard_False;

  if (BRep_Tool::IsClosed(theESplit, theFace))
    return Standard_True;

  TopoDS_Edge aESplit = theESplit;
  aESplit.Orientation(TopAbs_FORWARD);

  TopoDS_Face aFace = theFace;
  aFace.Orientation(TopAbs_FORWARD);

  Standard_Real        aTS1, aTS2;
  Handle(Geom2d_Curve) aC2DSplit = BRep_Tool::CurveOnSurface(aESplit, aFace, aTS1, aTS2);
  if (aC2DSplit.IsNull())
    return Standard_False;

  Standard_Real        aT1, aT2;
  Handle(Geom2d_Curve) aC2D1 =
    BRep_Tool::CurveOnSurface(TopoDS::Edge(theEOrigin.Oriented(TopAbs_FORWARD)), aFace, aT1, aT2);
  Handle(Geom2d_Curve) aC2D2 =
    BRep_Tool::CurveOnSurface(TopoDS::Edge(theEOrigin.Oriented(TopAbs_REVERSED)), aFace, aT1, aT2);

  Standard_Real aT = BOPTools_AlgoTools2D::IntermediatePoint(aTS1, aTS2);
  gp_Pnt2d      aPMid;
  gp_Vec2d      aVTgt;
  aC2DSplit->D1(aT, aPMid, aVTgt);

  // project on original 2d curves
  Geom2dAPI_ProjectPointOnCurve aProjPC1, aProjPC2;
  aProjPC1.Init(aPMid, aC2D1, aT1, aT2);
  aProjPC2.Init(aPMid, aC2D2, aT1, aT2);

  if (!aProjPC1.NbPoints() && !aProjPC2.NbPoints())
    return Standard_False;

  Standard_Real aDist1 = aProjPC1.NbPoints() ? aProjPC1.LowerDistance() : RealLast();
  Standard_Real aDist2 = aProjPC2.NbPoints() ? aProjPC2.LowerDistance() : RealLast();

  if (aDist1 > Precision::PConfusion() && aDist2 > Precision::PConfusion())
    return Standard_False;

  // choose the closest and take corresponding point from the opposite
  gp_Pnt2d aNewPnt = aDist1 < aDist2 ? aC2D2->Value(aProjPC1.LowerDistanceParameter())
                                     : aC2D1->Value(aProjPC2.LowerDistanceParameter());

  Handle(Geom2d_Curve) aTmpC1 = Handle(Geom2d_Curve)::DownCast(aC2DSplit->Copy());
  Handle(Geom2d_Curve) aTmpC2 = Handle(Geom2d_Curve)::DownCast(aC2DSplit->Copy());

  Handle(Geom2d_TrimmedCurve) aC1 = new Geom2d_TrimmedCurve(aTmpC1, aTS1, aTS2);
  Handle(Geom2d_TrimmedCurve) aC2 = new Geom2d_TrimmedCurve(aTmpC2, aTS1, aTS2);

  gp_Vec2d aTrVec(aPMid, aNewPnt);
  aC2->Translate(aTrVec);

  gp_Pnt2d aPProj;
  gp_Vec2d aVTgtOrigin;
  if (aDist1 < aDist2)
  {
    aC2D1->D1(aProjPC1.LowerDistanceParameter(), aPProj, aVTgtOrigin);
  }
  else
  {
    aC2D2->D1(aProjPC2.LowerDistanceParameter(), aPProj, aVTgtOrigin);
  }

  Standard_Real aDot = aVTgt.Dot(aVTgtOrigin);

  if ((aDist1 < aDist2) == (aDot > 0))
  {
    BRep_Builder().UpdateEdge(aESplit, aC1, aC2, aFace, BRep_Tool::Tolerance(aESplit));
  }
  else
  {
    BRep_Builder().UpdateEdge(aESplit, aC2, aC1, aFace, BRep_Tool::Tolerance(aESplit));
  }
  return Standard_True;
}

//=================================================================================================

void BOPTools_AlgoTools3D::GetNormalToFaceOnEdge(const TopoDS_Edge&              aE,
                                                 const TopoDS_Face&              aF,
                                                 gp_Dir&                         aDNF,
                                                 const Handle(IntTools_Context)& theContext)
{
  Standard_Real aT, aT1, aT2;

  BRep_Tool::CurveOnSurface(aE, aF, aT1, aT2);
  aT = BOPTools_AlgoTools2D::IntermediatePoint(aT1, aT2);

  BOPTools_AlgoTools3D::GetNormalToFaceOnEdge(aE, aF, aT, aDNF, theContext);

  if (aF.Orientation() == TopAbs_REVERSED)
  {
    aDNF.Reverse();
  }
}

//=================================================================================================

void BOPTools_AlgoTools3D::GetNormalToFaceOnEdge(const TopoDS_Edge&              aE,
                                                 const TopoDS_Face&              aF1,
                                                 const Standard_Real             aT,
                                                 gp_Dir&                         aDNF1,
                                                 const Handle(IntTools_Context)& theContext)
{
  Standard_Real U, V, aTolPC;
  gp_Pnt2d      aP2D;
  gp_Pnt        aP;
  gp_Vec        aD1U, aD1V;

  Handle(Geom_Surface) aS1 = BRep_Tool::Surface(aF1);

  Handle(Geom2d_Curve) aC2D1;
  BOPTools_AlgoTools2D::CurveOnSurface(aE, aF1, aC2D1, aTolPC, theContext);

  aC2D1->D0(aT, aP2D);
  U = aP2D.X();
  V = aP2D.Y();

  aS1->D1(U, V, aP, aD1U, aD1V);
  gp_Dir aDD1U(aD1U);
  gp_Dir aDD1V(aD1V);

  aDNF1 = aDD1U ^ aDD1V;
}

//=================================================================================================

Standard_Integer BOPTools_AlgoTools3D::SenseFlag(const gp_Dir& aDNF1, const gp_Dir& aDNF2)
{
  Standard_Boolean bIsDirsCoinside;
  //
  bIsDirsCoinside = IntTools_Tools::IsDirsCoinside(aDNF1, aDNF2);
  if (!bIsDirsCoinside)
  {
    return 0;
  }

  Standard_Real aScPr;

  aScPr = aDNF1 * aDNF2;
  if (aScPr < 0.)
  {
    return -1;
  }
  else if (aScPr > 0.)
  {
    return 1;
  }
  return -1;
}

//=================================================================================================

Standard_Boolean BOPTools_AlgoTools3D::GetNormalToSurface(const Handle(Geom_Surface)& aS,
                                                          const Standard_Real         U,
                                                          const Standard_Real         V,
                                                          gp_Dir&                     aDNS)
{
  gp_Pnt aP;
  gp_Vec aD1U, aD1V;

  aS->D1(U, V, aP, aD1U, aD1V);

  Standard_Real aLenU = aD1U.SquareMagnitude();
  if (aLenU < gp::Resolution())
    return Standard_False;

  Standard_Real aLenV = aD1V.SquareMagnitude();
  if (aLenV < gp::Resolution())
    return Standard_False;

  gp_Dir aDD1U(aD1U);
  gp_Dir aDD1V(aD1V);

  Standard_Boolean bFlag = IntTools_Tools::IsDirsCoinside(aDD1U, aDD1U);
  if (!bFlag)
    return bFlag;

  aDNS = aDD1U ^ aDD1V;
  return bFlag;
}

//=================================================================================================

Standard_Boolean BOPTools_AlgoTools3D::GetApproxNormalToFaceOnEdge(const TopoDS_Edge&  aE,
                                                                   const TopoDS_Face&  aF,
                                                                   const Standard_Real aT,
                                                                   gp_Pnt&             aPNear,
                                                                   gp_Dir&             aDNF,
                                                                   Standard_Real       aDt2D)
{
  gp_Pnt2d         aPx2DNear;
  Standard_Integer iErr = BOPTools_AlgoTools3D::PointNearEdge(aE, aF, aT, aDt2D, aPx2DNear, aPNear);
  if (iErr != 1)
  {
    Handle(Geom_Surface) aS = BRep_Tool::Surface(aF);

    BOPTools_AlgoTools3D::GetNormalToSurface(aS, aPx2DNear.X(), aPx2DNear.Y(), aDNF);

    if (aF.Orientation() == TopAbs_REVERSED)
    {
      aDNF.Reverse();
    }
  }
  //
  return (iErr == 0);
}

//=================================================================================================

Standard_Boolean BOPTools_AlgoTools3D::GetApproxNormalToFaceOnEdge(
  const TopoDS_Edge&              aE,
  const TopoDS_Face&              aF,
  const Standard_Real             aT,
  gp_Pnt&                         aPNear,
  gp_Dir&                         aDNF,
  const Handle(IntTools_Context)& theContext)
{
  gp_Pnt2d         aPx2DNear;
  Standard_Integer iErr =
    BOPTools_AlgoTools3D::PointNearEdge(aE, aF, aT, aPx2DNear, aPNear, theContext);
  if (iErr != 1)
  {
    Handle(Geom_Surface) aS = BRep_Tool::Surface(aF);

    BOPTools_AlgoTools3D::GetNormalToSurface(aS, aPx2DNear.X(), aPx2DNear.Y(), aDNF);

    if (aF.Orientation() == TopAbs_REVERSED)
    {
      aDNF.Reverse();
    }
  }
  //
  return (iErr == 0);
}

//=================================================================================================

Standard_Boolean BOPTools_AlgoTools3D::GetApproxNormalToFaceOnEdge(
  const TopoDS_Edge&              aE,
  const TopoDS_Face&              aF,
  const Standard_Real             aT,
  const Standard_Real             theStep,
  gp_Pnt&                         aPNear,
  gp_Dir&                         aDNF,
  const Handle(IntTools_Context)& theContext)
{
  gp_Pnt2d         aPx2DNear;
  Standard_Integer iErr =
    BOPTools_AlgoTools3D::PointNearEdge(aE, aF, aT, theStep, aPx2DNear, aPNear, theContext);
  if (iErr != 1)
  {
    Handle(Geom_Surface) aS = BRep_Tool::Surface(aF);

    BOPTools_AlgoTools3D::GetNormalToSurface(aS, aPx2DNear.X(), aPx2DNear.Y(), aDNF);

    if (aF.Orientation() == TopAbs_REVERSED)
    {
      aDNF.Reverse();
    }
  }
  //
  return (iErr == 0);
}

//=================================================================================================

Standard_Integer BOPTools_AlgoTools3D::PointNearEdge(const TopoDS_Edge&  aE,
                                                     const TopoDS_Face&  aF,
                                                     const Standard_Real aT,
                                                     const Standard_Real aDt2D,
                                                     gp_Pnt2d&           aPx2DNear,
                                                     gp_Pnt&             aPxNear)
{
  Standard_Real        aFirst, aLast, aETol, aFTol, transVal;
  GeomAbs_SurfaceType  aTS;
  Handle(Geom2d_Curve) aC2D;
  Handle(Geom_Surface) aS;
  //
  aC2D                  = BRep_Tool::CurveOnSurface(aE, aF, aFirst, aLast);
  Standard_Integer iErr = aC2D.IsNull() ? 1 : 0;
  if (iErr)
  {
    return iErr;
  }
  //
  aS = BRep_Tool::Surface(aF);
  //
  gp_Pnt2d aPx2D;
  gp_Vec2d aVx2D;
  aC2D->D1(aT, aPx2D, aVx2D);
  gp_Dir2d aDx2D(aVx2D);

  gp_Dir2d aDP;
  aDP.SetCoord(-aDx2D.Y(), aDx2D.X());

  if (aE.Orientation() == TopAbs_REVERSED)
  {
    aDP.Reverse();
  }

  if (aF.Orientation() == TopAbs_REVERSED)
  {
    aDP.Reverse();
  }
  //
  aETol = BRep_Tool::Tolerance(aE);
  aFTol = BRep_Tool::Tolerance(aF);
  // NPAL19220
  GeomAdaptor_Surface aGAS(aS);
  aTS = aGAS.GetType();
  if (aTS == GeomAbs_BSplineSurface)
  {
    if (aETol > 1.e-5)
    {
      aFTol = aETol;
    }
  }
  if (aETol > 1.e-5 || aFTol > 1.e-5)
  {
    //
    if (aTS != GeomAbs_Sphere)
    {
      gp_Vec2d transVec(aDP);
      transVal = aDt2D + aETol + aFTol;
      if (aTS == GeomAbs_Cylinder)
      { // pkv/909/F8
        Standard_Real aR, dT;
        //
        gp_Cylinder aCyl = aGAS.Cylinder();
        aR               = aCyl.Radius();
        dT               = 1. - transVal / aR;
        if (dT >= -1 && dT <= 1)
        {
          dT       = acos(dT);
          transVal = dT;
        }
      }
      //
      transVec.Multiply(transVal);
      aPx2DNear = aPx2D.Translated(transVec);
    }
    else
    {
      aPx2DNear.SetCoord(aPx2D.X() + aDt2D * aDP.X(), aPx2D.Y() + aDt2D * aDP.Y());
    }
  }
  else
  {
    aPx2DNear.SetCoord(aPx2D.X() + aDt2D * aDP.X(), aPx2D.Y() + aDt2D * aDP.Y());
  }
  //
  aS->D0(aPx2DNear.X(), aPx2DNear.Y(), aPxNear);
  return iErr;
}

//=================================================================================================

Standard_Integer BOPTools_AlgoTools3D::PointNearEdge(const TopoDS_Edge&              aE,
                                                     const TopoDS_Face&              aF,
                                                     const Standard_Real             aT,
                                                     gp_Pnt2d&                       aPx2DNear,
                                                     gp_Pnt&                         aPxNear,
                                                     const Handle(IntTools_Context)& theContext)
{
  Standard_Real        aTolE, aTolF, dTx, dT2D;
  Handle(Geom_Surface) aS;
  GeomAdaptor_Surface  aGAS;
  //
  dT2D = 10. * BOPTools_AlgoTools3D::MinStepIn2d(); //~1.e-5;
  //
  aS = BRep_Tool::Surface(aF);
  aGAS.Load(aS);
  if (aGAS.GetType() == GeomAbs_Cylinder || aGAS.GetType() == GeomAbs_Sphere)
  {
    dT2D = 10. * dT2D;
  }
  //
  aTolE = BRep_Tool::Tolerance(aE);
  aTolF = BRep_Tool::Tolerance(aF);
  dTx   = 2. * (aTolE + aTolF);
  if (dTx > dT2D)
  {
    dT2D = dTx;
  }
  //
  Standard_Integer iErr = BOPTools_AlgoTools3D::PointNearEdge(aE, aF, aT, dT2D, aPx2DNear, aPxNear);
  if ((iErr != 1) && !theContext->IsPointInOnFace(aF, aPx2DNear))
  {
    gp_Pnt   aP;
    gp_Pnt2d aP2d;
    //
    iErr = BOPTools_AlgoTools3D::PointInFace(aF, aE, aT, dT2D, aP, aP2d, theContext);
    if (iErr == 0)
    {
      aPxNear   = aP;
      aPx2DNear = aP2d;
    }
    else
    {
      iErr = 2; // point is out of the face
    }
  }
  //
  return iErr;
}

//=================================================================================================

Standard_Integer BOPTools_AlgoTools3D::PointNearEdge(const TopoDS_Edge&              aE,
                                                     const TopoDS_Face&              aF,
                                                     const Standard_Real             aT,
                                                     const Standard_Real             theStep,
                                                     gp_Pnt2d&                       aPx2DNear,
                                                     gp_Pnt&                         aPxNear,
                                                     const Handle(IntTools_Context)& theContext)
{
  Standard_Integer iErr =
    BOPTools_AlgoTools3D::PointNearEdge(aE, aF, aT, theStep, aPx2DNear, aPxNear);
  if ((iErr != 1) && !theContext->IsPointInOnFace(aF, aPx2DNear))
  {
    gp_Pnt   aP;
    gp_Pnt2d aP2d;
    //
    iErr = BOPTools_AlgoTools3D::PointInFace(aF, aE, aT, theStep, aP, aP2d, theContext);
    if (iErr == 0)
    {
      aPxNear   = aP;
      aPx2DNear = aP2d;
    }
    else
    {
      iErr = 2; // point is out of the face
    }
  }
  //
  return iErr;
}

//=================================================================================================

Standard_Integer BOPTools_AlgoTools3D::PointNearEdge(const TopoDS_Edge&              aE,
                                                     const TopoDS_Face&              aF,
                                                     gp_Pnt2d&                       aPInFace2D,
                                                     gp_Pnt&                         aPInFace,
                                                     const Handle(IntTools_Context)& theContext)
{
  Standard_Real aT, aT1, aT2;
  //
  // 1. compute parameter on edge
  BRep_Tool::Range(aE, aT1, aT2);
  aT = BOPTools_AlgoTools2D::IntermediatePoint(aT1, aT2);
  //
  // 2. compute point inside the face near the edge
  TopoDS_Face aFF = aF;
  TopoDS_Edge aERight;
  aFF.Orientation(TopAbs_FORWARD);
  BOPTools_AlgoTools3D::OrientEdgeOnFace(aE, aFF, aERight);
  //
  Standard_Integer iErr =
    BOPTools_AlgoTools3D::PointNearEdge(aERight, aFF, aT, aPInFace2D, aPInFace, theContext);
  //
  return iErr;
}

//=================================================================================================

Standard_Real BOPTools_AlgoTools3D::MinStepIn2d()
{
  Standard_Real dt = 1.e-5;
  return dt;
}

//=================================================================================================

Standard_Boolean BOPTools_AlgoTools3D::IsEmptyShape(const TopoDS_Shape& aS)
{
  Standard_Boolean bHasGeometry = Standard_False;
  //
  TopTools_IndexedMapOfShape myShapes;
  //
  Add(aS, myShapes, bHasGeometry);

  return !bHasGeometry;
}

//=================================================================================================

void Add(const TopoDS_Shape&         aS,
         TopTools_IndexedMapOfShape& myShapes,
         Standard_Boolean&           bHasGeometry)
{
  Standard_Integer anIndex;
  //
  if (bHasGeometry)
  {
    return;
  }
  //
  if (aS.IsNull())
  {
    return;
  }
  //
  const TopoDS_Shape& aSx = aS;
  //
  anIndex = myShapes.FindIndex(aSx);
  if (!anIndex)
  {
    bHasGeometry = HasGeometry(aSx);
    if (bHasGeometry)
    {
      return;
    }
    //
    TopoDS_Iterator anIt(aSx, Standard_False, Standard_False);
    for (; anIt.More(); anIt.Next())
    {
      const TopoDS_Shape& aSy = anIt.Value();
      Add(aSy, myShapes, bHasGeometry);
      //
      if (bHasGeometry)
      {
        return;
      }
      //
      myShapes.Add(aSx);
    }
  }
}

//=================================================================================================

Standard_Boolean HasGeometry(const TopoDS_Shape& aS)
{
  Standard_Boolean bHasGeometry = Standard_True;
  TopAbs_ShapeEnum aType        = aS.ShapeType();

  if (aType == TopAbs_VERTEX)
  {
    return bHasGeometry;
  }
  //
  else if (aType == TopAbs_EDGE)
  {
    Handle(BRep_TEdge)                           TE = Handle(BRep_TEdge)::DownCast(aS.TShape());
    BRep_ListIteratorOfListOfCurveRepresentation itrc(TE->Curves());

    while (itrc.More())
    {
      const Handle(BRep_CurveRepresentation)& CR = itrc.Value();
      if (CR->IsCurve3D())
      {
        if (!CR->Curve3D().IsNull())
        {
          return bHasGeometry;
        }
      }
      else if (CR->IsCurveOnSurface())
      {
        return bHasGeometry;
      }
      else if (CR->IsRegularity())
      {
        return bHasGeometry;
      }
      else if (!CR->Polygon3D().IsNull())
      {
        return bHasGeometry;
      }
      else if (CR->IsPolygonOnTriangulation())
      {
        return bHasGeometry;
      }
      else if (CR->IsPolygonOnSurface())
      {
        return bHasGeometry;
      }
      itrc.Next();
    }
  }
  //
  else if (aType == TopAbs_FACE)
  {
    Handle(BRep_TFace) TF = Handle(BRep_TFace)::DownCast(aS.TShape());
    if (!TF->Surface().IsNull())
    {
      return bHasGeometry;
    }
    Handle(Poly_Triangulation) Tr = TF->Triangulation();
    if (!Tr.IsNull())
    {
      return bHasGeometry;
    }
  }

  return !bHasGeometry;
}

//=================================================================================================

void BOPTools_AlgoTools3D::OrientEdgeOnFace(const TopoDS_Edge& aE,
                                            const TopoDS_Face& aF,
                                            TopoDS_Edge&       aERight)
{
  if (BRep_Tool::IsClosed(aE, aF))
  {
    aERight = aE;
    aERight.Orientation(aE.Orientation());

    Standard_Integer iFoundCount = 0;
    TopoDS_Edge      anEdge      = aE;
    TopExp_Explorer  anExp(aF, TopAbs_EDGE);

    for (; anExp.More(); anExp.Next())
    {
      const TopoDS_Shape& aSS = anExp.Current();

      if (aSS.IsSame(aE))
      {
        anEdge = TopoDS::Edge(aSS);
        iFoundCount++;
      }
    }

    if (iFoundCount == 1)
    {
      aERight = anEdge;
    }
    return;
  }

  TopExp_Explorer anExp(aF, TopAbs_EDGE);
  for (; anExp.More(); anExp.Next())
  {
    const TopoDS_Shape& aSS = anExp.Current();
    if (aSS.IsSame(aE))
    {
      aERight = aE;
      aERight.Orientation(aSS.Orientation());
      return;
    }
  }
  aERight = aE;
  aERight.Orientation(aE.Orientation());
}

//=================================================================================================

Standard_Integer BOPTools_AlgoTools3D::PointInFace(const TopoDS_Face&              theF,
                                                   gp_Pnt&                         theP,
                                                   gp_Pnt2d&                       theP2D,
                                                   const Handle(IntTools_Context)& theContext)
{
  Standard_Integer i, iErr = 1;
  Standard_Real    aUMin, aUMax, aVMin, aVMax, aUx;
  //
  theContext->UVBounds(theF, aUMin, aUMax, aVMin, aVMax);
  //
  gp_Dir2d aD2D(0., 1.);
  aUx = IntTools_Tools::IntermediatePoint(aUMin, aUMax);
  //
  for (i = 0; i < 2; ++i)
  {
    gp_Pnt2d            aP2D(aUx, 0.);
    Handle(Geom2d_Line) aL2D = new Geom2d_Line(aP2D, aD2D);
    iErr = BOPTools_AlgoTools3D::PointInFace(theF, aL2D, theP, theP2D, theContext);
    if (iErr == 0)
    {
      // done
      break;
    }
    else
    {
      // possible reason - incorrect computation of the 2d box of the face.
      // try to compute the point with the translated line.
      aUx = aUMax - (aUx - aUMin);
    }
  }
  //
  return iErr;
}

//=================================================================================================

Standard_Integer BOPTools_AlgoTools3D::PointInFace(const TopoDS_Face&              theF,
                                                   const TopoDS_Edge&              theE,
                                                   const Standard_Real             theT,
                                                   const Standard_Real             theDt2D,
                                                   gp_Pnt&                         theP,
                                                   gp_Pnt2d&                       theP2D,
                                                   const Handle(IntTools_Context)& theContext)
{
  Standard_Integer     iErr;
  Standard_Real        f, l;
  Handle(Geom2d_Curve) aC2D;
  //
  iErr = 0;
  aC2D = BRep_Tool::CurveOnSurface(theE, theF, f, l);
  if (aC2D.IsNull())
  {
    iErr = 5;
    return iErr;
  }
  //
  gp_Pnt2d aP2D;
  gp_Vec2d aV2D;
  //
  aC2D->D1(theT, aP2D, aV2D);
  gp_Dir2d aD2Dx(aV2D);
  //
  gp_Dir2d aD2D;
  aD2D.SetCoord(-aD2Dx.Y(), aD2Dx.X());
  //
  if (theE.Orientation() == TopAbs_REVERSED)
  {
    aD2D.Reverse();
  }
  //
  if (theF.Orientation() == TopAbs_REVERSED)
  {
    aD2D.Reverse();
  }
  //
  Handle(Geom2d_Line)         aL2D     = new Geom2d_Line(aP2D, aD2D);
  Handle(Geom2d_TrimmedCurve) aL2DTrim = new Geom2d_TrimmedCurve(aL2D, 0., Precision::Infinite());
  //
  iErr = BOPTools_AlgoTools3D::PointInFace(theF, aL2DTrim, theP, theP2D, theContext, theDt2D);
  //
  return iErr;
}

//=================================================================================================

Standard_Integer BOPTools_AlgoTools3D::PointInFace(const TopoDS_Face&              theF,
                                                   const Handle(Geom2d_Curve)&     theL2D,
                                                   gp_Pnt&                         theP,
                                                   gp_Pnt2d&                       theP2D,
                                                   const Handle(IntTools_Context)& theContext,
                                                   const Standard_Real             theDt2D)
{
  Standard_Boolean bIsDone, bHasFirstPoint, bHasSecondPoint;
  Standard_Integer iErr, aIH, aNbDomains;
  Standard_Real    aVx, aV1, aV2;
  //
  Geom2dHatch_Hatcher& aHatcher = theContext->Hatcher(theF);
  //
  Geom2dAdaptor_Curve aHCur(theL2D);
  //
  aHatcher.ClrHatchings();
  aIH = aHatcher.AddHatching(aHCur);
  //
  iErr = 0;
  for (;;)
  {
    aHatcher.Trim();
    bIsDone = aHatcher.TrimDone(aIH);
    if (!bIsDone)
    {
      iErr = 1;
      break;
    }
    //
    aHatcher.ComputeDomains(aIH);
    bIsDone = aHatcher.IsDone(aIH);
    if (!bIsDone)
    {
      iErr = 2;
      break;
    }
    //
    aNbDomains = aHatcher.NbDomains(aIH);
    if (aNbDomains == 0)
    {
      iErr = 2;
      break;
    }
    //
    const HatchGen_Domain& aDomain = aHatcher.Domain(aIH, 1);
    bHasFirstPoint                 = aDomain.HasFirstPoint();
    if (!bHasFirstPoint)
    {
      iErr = 3;
      break;
    }
    //
    bHasSecondPoint = aDomain.HasSecondPoint();
    if (!bHasSecondPoint)
    {
      iErr = 4;
      break;
    }
    //
    aV1 = aDomain.FirstPoint().Parameter();
    aV2 = aDomain.SecondPoint().Parameter();
    //
    aVx = (theDt2D > 0. && (aV2 - aV1) > theDt2D) ? (aV1 + theDt2D)
                                                  : IntTools_Tools::IntermediatePoint(aV1, aV2);
    //
    Handle(Geom_Surface) aS = BRep_Tool::Surface(theF);
    //
    theL2D->D0(aVx, theP2D);
    aS->D0(theP2D.X(), theP2D.Y(), theP);
    break;
  }
  //
  aHatcher.RemHatching(aIH);
  return iErr;
}
