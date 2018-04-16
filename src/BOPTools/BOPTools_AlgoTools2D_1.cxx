// Created by: Peter KURNEV
// Copyright (c) 1999-2015 OPEN CASCADE SAS
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

#include <BOPTools_AlgoTools2D.hxx>
#include <Precision.hxx>

#include <gp_Vec2d.hxx>
#include <gp_Dir2d.hxx>

#include <TopLoc_Location.hxx>

#include <Geom2d_Curve.hxx>
#include <Geom2d_TrimmedCurve.hxx>
#include <Geom_Surface.hxx>
#include <GeomLib.hxx>

#include <GeomAPI_ProjectPointOnCurve.hxx>

#include <TopoDS.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>

#include <BRep_Builder.hxx>
#include <BRep_Tool.hxx>
#include <BRepLib.hxx>

#include <TopExp_Explorer.hxx>

#include <IntTools_Context.hxx>
#include <IntTools_Tools.hxx>

#include <BOPTools_AlgoTools.hxx>



static
  Standard_Integer UpdateClosedPCurve(const TopoDS_Edge& ,
                                      const TopoDS_Edge& ,
                                      const TopoDS_Face& , 
                                      const Handle(IntTools_Context)& );
static
  Standard_Boolean IsClosed(const TopoDS_Edge& ,
                            const TopoDS_Face& );

//=======================================================================
//function : AttachExistingPCurve
//purpose  : 
//=======================================================================
Standard_Integer BOPTools_AlgoTools2D::AttachExistingPCurve
  (const TopoDS_Edge& aE2, // old
   const TopoDS_Edge& aE1, // new
   const TopoDS_Face& aF, 
   const Handle(IntTools_Context)& aCtx)
{
  Standard_Boolean bIsToReverse, bIsClosed, bComp;
  Standard_Integer iRet;
  Standard_Real aTol, aT11, aT12, aT21, aT22, aTolPPC;
  Standard_Real aTolSP, aTMax;
  Handle(Geom2d_Curve) aC2Dold, aC2DoldC;
  Handle(Geom2d_Curve) aC2DT;
  BRep_Builder aBB;
  //
  iRet=0;
  //
  aC2Dold=BRep_Tool::CurveOnSurface(aE2, aF, aT21, aT22);
  if (aC2Dold.IsNull()){
    iRet=1;
    return iRet;
  }
  //
  aC2DoldC=Handle(Geom2d_Curve)::DownCast(aC2Dold->Copy());
  //
  bIsToReverse = BOPTools_AlgoTools::IsSplitToReverse(aE1, aE2, aCtx);
  if (bIsToReverse) {
    Standard_Real aT21r, aT22r;
    //
    aC2DoldC->Reverse();
    //
    aT21r=aC2DoldC->ReversedParameter(aT21);
    aT22r=aC2DoldC->ReversedParameter(aT22);
    aT21=aT22r;
    aT22=aT21r;
  }
  //
  aC2DT=new Geom2d_TrimmedCurve(aC2DoldC, aT21, aT22);
  //
  aTolPPC=Precision::PConfusion();
  //
  Handle(Geom_Curve) aCE1 = BRep_Tool::Curve(aE1, aT11, aT12);
  //
  GeomLib::SameRange(aTolPPC, aC2DT, aT21, aT22, aT11, aT12, aC2DT);
  //
  if (aC2DT.IsNull()){
    iRet=2;
    return iRet;
  }
  //
  // check the curves on same parameter to prevent 
  // big tolerance increasing
  Handle(Geom_Surface) aSF = BRep_Tool::Surface(aF);
  //
  bComp = IntTools_Tools::ComputeTolerance
    (aCE1, aC2DT, aSF, aT11, aT12, aTolSP, aTMax, aTolPPC);
  if (!bComp) {
    iRet = 3;
    return iRet;
  }
  //
  aTol = BRep_Tool::Tolerance(aE1);
  //
  if ((aTolSP > 10.*aTol) && aTolSP > 0.1) {
    iRet = 4;
    return iRet;
  }
  //
  // create a temporary edge to make same parameter pcurve
  TopoDS_Edge aE1T;
  aBB.MakeEdge(aE1T, aCE1, aTol);
  aBB.Range(aE1T, aT11, aT12);
  aBB.SameRange(aE1T, Standard_False);
  aBB.SameParameter(aE1T, Standard_False);
  //
  aBB.UpdateEdge(aE1T, aC2DT, aF, aTol);
  try {
    BRepLib::SameParameter(aE1T);
    BRepLib::SameRange(aE1T);
  }
  catch (Standard_Failure)
  {
    iRet = 6;
    return iRet;
  }
  //
  bIsClosed = IsClosed(aE2, aF);
  if (bIsClosed) {
    iRet = UpdateClosedPCurve(aE2, aE1T, aF, aCtx);
    if(iRet) {
      iRet = 5;
      return iRet;
    }
  }
  // transfer pcurve(s) from the temporary edge to the new edge
  aBB.Transfert(aE1T, aE1);
  // update tolerance of vertices
  Standard_Real aNewTol = BRep_Tool::Tolerance(aE1T);
  TopoDS_Iterator it(aE1);
  for (; it.More(); it.Next())
    aBB.UpdateVertex(TopoDS::Vertex(it.Value()), aNewTol);
  return iRet;
}
//=======================================================================
//function : UpdateClosedPCurve
//purpose  : 
//=======================================================================
Standard_Integer UpdateClosedPCurve(const TopoDS_Edge& aEold,
                                    const TopoDS_Edge& aEnew,
                                    const TopoDS_Face& aF, 
                                    const Handle(IntTools_Context)& aCtx)
{
  Standard_Boolean bUClosed, bRevOrder;
  Standard_Integer aNbPoints, iRet;
  Standard_Real aTS1, aTS2, aTS, aScPr, aUS1, aVS1, aUS2, aVS2, aT, aU, aV;
  Standard_Real aT1, aT2, aTol;
  gp_Pnt2d aP2DS1, aP2DS2, aP2D; 
  gp_Vec2d aV2DT, aV2D, aV2DS1, aV2DS2;
  gp_Pnt aP;
  Handle(Geom2d_Curve) aC2D, aC2DS1, aC2DS2, aC2Dnew, aC2DoldCT;         
  Handle(Geom2d_Curve) aC2Dold;         
  Handle(Geom2d_TrimmedCurve) aC2DTnew;
  Handle(Geom_Surface) aS;
  TopoDS_Edge aES;
  BRep_Builder aBB;
  //
  iRet=0;
  aTol=BRep_Tool::Tolerance(aEnew);
  //
  // aC2DoldCT is alone p-curve of aEnew that we've built
  // The task is to build closed p-curves for aEnew
  aC2DoldCT=BRep_Tool::CurveOnSurface(aEnew, aF, aT1, aT2);
  //
  // aC2Dold is p-curve of aEold
  aC2Dold=BRep_Tool::CurveOnSurface(aEold, aF, aT1, aT2);
  //
  // As aEold is closed on aF, it is possible to retrieve
  // the two p-curves:  
  //  aC2DS1 -first p-curve 
  //  aC2DS2 -second p-curve
  aES=aEold;
  aES.Orientation(TopAbs_FORWARD);
  aC2DS1=BRep_Tool::CurveOnSurface(aES, aF, aTS1, aTS2);
  //
  aES.Orientation(TopAbs_REVERSED);
  aC2DS2=BRep_Tool::CurveOnSurface(aES, aF, aTS1, aTS2);
  //
  aTS=BOPTools_AlgoTools2D::IntermediatePoint(aTS1, aTS2);
  //
  aC2DS1->D1(aTS, aP2DS1, aV2DS1);
  aC2DS2->D1(aTS, aP2DS2, aV2DS2);
  //
  // aV2DS12 - translation vector 
  gp_Vec2d aV2DS12(aP2DS1, aP2DS2);
  gp_Dir2d aD2DS12(aV2DS12);
  const gp_Dir2d& aD2DX=gp::DX2d();
  //
  // Directoion of closeness: U-Closed or V-Closed 
  aScPr=aD2DS12*aD2DX;
  bUClosed=Standard_True;
  if (fabs(aScPr) < aTol) {
    bUClosed=!bUClosed;
  }
  //
  aP2DS1.Coord(aUS1, aVS1);
  aP2DS2.Coord(aUS2, aVS2);
  //
  // aP - some 3D-point on seam edge of the surface aS
  aS=BRep_Tool::Surface(aF);
  aS->D0(aUS1, aVS1, aP);
  //
  GeomAPI_ProjectPointOnCurve& aProjPC=aCtx->ProjPC(aEnew);
  //
  aProjPC.Perform(aP);
  aNbPoints=aProjPC.NbPoints();
  if (!aNbPoints) {
    iRet=1;
    return iRet;
  }
  //
  // aT - parameter for aP on the curves of aEnew
  aT=aProjPC.LowerDistanceParameter();
  //
  aC2D=aC2DoldCT;
  aC2D->D1(aT, aP2D, aV2D);
  aP2D.Coord(aU, aV);
  //
  aC2Dnew=Handle(Geom2d_Curve)::DownCast(aC2D->Copy());
  aC2DTnew=new Geom2d_TrimmedCurve(aC2Dnew, aT1, aT2);
  //
  aV2DT=aV2DS12;
  if (!bUClosed) {    // V Closed
    if (fabs(aV-aVS2)<aTol) {
      aV2DT.Reverse();
    }
  }
  else {   // U Closed
    if (fabs(aU-aUS2)<aTol) {
      aV2DT.Reverse();
    }
  }
  //
  // Translate aC2DTnew
  aC2DTnew->Translate(aV2DT);
  //
  // 4 Order the 2D curves
  bRevOrder=Standard_False;
  aScPr=aV2D*aV2DS1;
  if(aScPr<0.) {
    bRevOrder=!bRevOrder;
  }
  //
  if (!bRevOrder) {
    aBB.UpdateEdge(aEnew, aC2D, aC2DTnew, aF, aTol);
  }
  else {
    aBB.UpdateEdge(aEnew, aC2DTnew, aC2D , aF, aTol);
  }
  return iRet;
}
//=======================================================================
//function : IsClosed
//purpose  :
//=======================================================================
Standard_Boolean IsClosed(const TopoDS_Edge& aE,
                          const TopoDS_Face& aF)
{
  Standard_Boolean bRet;
  //
  bRet=BRep_Tool::IsClosed(aE, aF);
  if (bRet) {
    Standard_Integer iCnt;
    //
    iCnt=0;
    TopExp_Explorer aExp(aF, TopAbs_EDGE);
    for (; (aExp.More() || iCnt==2); aExp.Next()) {
      const TopoDS_Shape& aEx=aExp.Current();
      if(aEx.IsSame(aE)) {
        ++iCnt;
      }
    }
    bRet=(iCnt==2);
  }
  return bRet;
}
