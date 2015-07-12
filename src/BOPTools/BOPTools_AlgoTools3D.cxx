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
#include <BOPCol_IndexedDataMapOfShapeListOfShape.hxx>
#include <BOPCol_IndexedMapOfShape.hxx>
#include <BOPTools_AlgoTools2D.hxx>
#include <BOPTools_AlgoTools3D.hxx>
#include <BRep_Builder.hxx>
#include <BRep_CurveRepresentation.hxx>
#include <BRep_ListIteratorOfListOfCurveRepresentation.hxx>
#include <BRep_ListIteratorOfListOfPointRepresentation.hxx>
#include <BRep_PointRepresentation.hxx>
#include <BRep_TEdge.hxx>
#include <BRep_TFace.hxx>
#include <BRep_Tool.hxx>
#include <BRep_TVertex.hxx>
#include <BRepAdaptor_Surface.hxx>
#include <BRepBndLib.hxx>
#include <BRepGProp.hxx>
#include <BRepTools.hxx>
#include <Geom2d_Curve.hxx>
#include <Geom2d_Line.hxx>
#include <Geom2d_TrimmedCurve.hxx>
#include <Geom2dAdaptor_Curve.hxx>
#include <Geom2dHatch_Hatcher.hxx>
#include <Geom_BezierSurface.hxx>
#include <Geom_BSplineSurface.hxx>
#include <Geom_Curve.hxx>
#include <Geom_RectangularTrimmedSurface.hxx>
#include <Geom_Surface.hxx>
#include <GeomAdaptor_Surface.hxx>
#include <gp_Cylinder.hxx>
#include <gp_Dir.hxx>
#include <gp_Dir2d.hxx>
#include <gp_Lin2d.hxx>
#include <gp_Pln.hxx>
#include <gp_Pnt.hxx>
#include <gp_Pnt2d.hxx>
#include <gp_Vec.hxx>
#include <gp_Vec2d.hxx>
#include <GProp_GProps.hxx>
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
#include <TopoDS_Vertex.hxx>

static void Add(const TopoDS_Shape& aS,
                BOPCol_IndexedMapOfShape& myShapes, 
                Standard_Boolean& bHasGeometry);
static 
  Standard_Boolean HasGeometry(const TopoDS_Shape& aS);

//=======================================================================
//function : DoSplitSEAMOnFace
//purpose  : 
//=======================================================================
void BOPTools_AlgoTools3D::DoSplitSEAMOnFace (const TopoDS_Edge& aSplit,
                                              const TopoDS_Face& aF)
{
  Standard_Boolean bIsUPeriodic, bIsVPeriodic, bIsLeft;
  Standard_Real aTol, a, b, anUPeriod, anVPeriod, aT, anU, dU, anU1;
  Standard_Real aScPr, anV, dV, anV1;
  Standard_Real aUmin, aUmax, aVmin, aVmax;
  gp_Pnt2d aP2D;
  gp_Vec2d aVec2D;
  Handle(Geom2d_Curve) aTmpC1, aTmpC2;
  Handle(Geom2d_Curve) C2D1;
  Handle(Geom2d_Line) aLD1;
  Handle(Geom_Surface) aS;
  BRep_Builder BB;
  TopoDS_Edge aSp;
  //
  bIsLeft = Standard_False;
  aSp=aSplit;
  aSp.Orientation(TopAbs_FORWARD);
  aTol=BRep_Tool::Tolerance(aSp);
  //
  aS=BRep_Tool::Surface(aF);
  //
  aS->Bounds(aUmin, aUmax, aVmin, aVmax);
  //
  bIsUPeriodic=aS->IsUPeriodic();
  bIsVPeriodic=aS->IsVPeriodic();
  //
  anUPeriod = bIsUPeriodic ? aS->UPeriod() : 0.;
  anVPeriod = bIsVPeriodic ? aS->VPeriod() : 0.;
  //
  if (!bIsUPeriodic && !bIsVPeriodic) {
    Standard_Boolean bIsUClosed, bIsVClosed;
    Handle(Geom_BSplineSurface) aBS;
    Handle(Geom_BezierSurface) aBZ;
    Handle(Geom_RectangularTrimmedSurface) aRTS;
    //
    bIsUClosed=Standard_False;
    bIsVClosed=Standard_False;
    aBS=Handle(Geom_BSplineSurface)::DownCast(aS);
    aBZ=Handle(Geom_BezierSurface) ::DownCast(aS);
    aRTS=Handle(Geom_RectangularTrimmedSurface)::DownCast(aS);
    //
    if (!aBS.IsNull()) {
      bIsUClosed=aBS->IsUClosed();
      bIsVClosed=aBS->IsVClosed();
    }
    else if (!aBZ.IsNull()) {
      bIsUClosed=aBZ->IsUClosed();
      bIsVClosed=aBZ->IsVClosed();
    }
    else if (!aRTS.IsNull()) {
      Handle(Geom_Surface) aSB;
      //
      aSB=aRTS->BasisSurface();
      bIsUPeriodic=aSB->IsUPeriodic();
      bIsVPeriodic=aSB->IsVPeriodic();
      //
      if (!(bIsUPeriodic || bIsVPeriodic)) {
        return;
      }
      anUPeriod = bIsUPeriodic ? aSB->UPeriod() : 0.;
      anVPeriod = bIsVPeriodic ? aSB->VPeriod() : 0.;
    }
    //
    if (aRTS.IsNull()) {
      if (!bIsUClosed && !bIsVClosed) {
        return;
      }
      //
      if (bIsUClosed) {
        anUPeriod=aUmax-aUmin;
      }
      if (bIsVClosed) {
        anVPeriod=aVmax-aVmin;
      }
    }
  }
  //
  //---------------------------------------------------
  C2D1=BRep_Tool::CurveOnSurface(aSp, aF, a, b);
  //
  aT=BOPTools_AlgoTools2D::IntermediatePoint(a, b);
  C2D1->D1(aT, aP2D, aVec2D);
  gp_Dir2d aDir2D1(aVec2D), aDOX(-1.,0.), aDOY(0.,1.);
  //
  anU=aP2D.X();
  anV=aP2D.Y();
  //
  anU1=anU;
  anV1=anV;
  //
  GeomAdaptor_Surface aGAS(aS);
  dU = aGAS.UResolution(aTol);
  dV = aGAS.VResolution(aTol);
  //
  if (anUPeriod > 0.){
    if (fabs (anU-aUmin) < dU) {
      bIsLeft=Standard_True;
      anU1=anU+anUPeriod;
    }
    else if (fabs (anU-aUmax) < dU) {
      bIsLeft=Standard_False;
      anU1=anU-anUPeriod;
    }
  }
  //
  if (anVPeriod > 0.) {
    if (fabs (anV-aVmin) < dV) {
      bIsLeft=Standard_True;
      anV1=anV+anVPeriod;
    }
    else if (fabs (anV-aVmax) < dV) {
      bIsLeft=Standard_False;
      anV1=anV-anVPeriod;
    }
  }
  //
  if (anU1==anU && anV1==anV) {
    return;
  }
  //
  aScPr = (anU1==anU) ? aDir2D1*aDOX : aDir2D1*aDOY;
  //
  aTmpC1=Handle(Geom2d_Curve)::DownCast(C2D1->Copy());
  Handle(Geom2d_TrimmedCurve) aC1 = 
    new Geom2d_TrimmedCurve(aTmpC1, a, b);
  //
  aTmpC2=Handle(Geom2d_Curve)::DownCast(C2D1->Copy());
  Handle(Geom2d_TrimmedCurve) aC2 = 
    new Geom2d_TrimmedCurve(aTmpC2, a, b);
  gp_Vec2d aTrV(anU1-anU, anV1-anV);
  aC2->Translate(aTrV);
  //
  if (!bIsLeft) {
    if (aScPr<0.) {
      BB.UpdateEdge(aSp, aC2, aC1, aF, aTol);
    }
    else {
      BB.UpdateEdge(aSp, aC1, aC2, aF, aTol);
    }
  }
  else {
    if (aScPr<0.) {
      BB.UpdateEdge(aSp, aC1, aC2, aF, aTol);
    }
    else {
      BB.UpdateEdge(aSp, aC2, aC1, aF, aTol);
    }
  }
}
//=======================================================================
//function : GetNormalToFaceOnEdge
//purpose  : 
//=======================================================================
void BOPTools_AlgoTools3D::GetNormalToFaceOnEdge (const TopoDS_Edge& aE,
                                                  const TopoDS_Face& aF,
                                                  gp_Dir& aDNF)
{
  Standard_Real aT, aT1, aT2;
  
  BRep_Tool::CurveOnSurface(aE, aF, aT1, aT2);
  aT=BOPTools_AlgoTools2D::IntermediatePoint(aT1, aT2);

  BOPTools_AlgoTools3D::GetNormalToFaceOnEdge (aE, aF, aT, aDNF);

  if (aF.Orientation()==TopAbs_REVERSED){
    aDNF.Reverse();
  }
}
//=======================================================================
//function : GetNormalToFaceOnEdge
//purpose  : 
//=======================================================================
void BOPTools_AlgoTools3D::GetNormalToFaceOnEdge (const TopoDS_Edge& aE,
                                                  const TopoDS_Face& aF1,
                                                  const Standard_Real aT, 
                                                  gp_Dir& aDNF1)
{
  Standard_Real U, V, aTolPC;
  gp_Pnt2d aP2D;
  gp_Pnt aP;
  gp_Vec aD1U, aD1V;

  Handle(Geom_Surface) aS1=BRep_Tool::Surface(aF1);
  
  Handle(Geom2d_Curve)aC2D1;
  BOPTools_AlgoTools2D::CurveOnSurface(aE, aF1, aC2D1, aTolPC);

  aC2D1->D0(aT, aP2D);
  U=aP2D.X();
  V=aP2D.Y();
  
  aS1->D1(U, V, aP, aD1U, aD1V);
  gp_Dir aDD1U(aD1U); 
  gp_Dir aDD1V(aD1V); 
  
  aDNF1=aDD1U^aDD1V; 
}
//=======================================================================
//function : SenseFlag
//purpose  :
//=======================================================================
Standard_Integer BOPTools_AlgoTools3D::SenseFlag (const gp_Dir& aDNF1,
                                                  const gp_Dir& aDNF2)
{
  Standard_Boolean bIsDirsCoinside;
  //
  bIsDirsCoinside=IntTools_Tools::IsDirsCoinside(aDNF1, aDNF2);
  if (!bIsDirsCoinside) {
    return 0;
  }
  
  Standard_Real aScPr;
  
  aScPr=aDNF1*aDNF2;
  if (aScPr<0.) {
    return -1;
  }
  else if (aScPr>0.) {
    return 1;
  }
  return -1;
}
//=======================================================================
//function : GetNormalToSurface
//purpose  :
//=======================================================================
Standard_Boolean BOPTools_AlgoTools3D::GetNormalToSurface
  (const Handle(Geom_Surface)& aS,
   const Standard_Real U,
   const Standard_Real V,
   gp_Dir& aDNS)
{
  Standard_Boolean bFlag;
  
  gp_Pnt aP;
  gp_Vec aD1U, aD1V;

  aS->D1(U, V, aP, aD1U, aD1V);
  
  gp_Dir aDD1U(aD1U); 
  gp_Dir aDD1V(aD1V); 
  
  bFlag=IntTools_Tools::IsDirsCoinside(aDD1U, aDD1U);
  if (!bFlag) {
    return bFlag;
  }
  
  aDNS=aDD1U^aDD1V;
  return bFlag;
}
//=======================================================================
//function : GetApproxNormalToFaceOnEdge
//purpose  : 
//=======================================================================
void BOPTools_AlgoTools3D::GetApproxNormalToFaceOnEdge
  (const TopoDS_Edge& aE,
   const TopoDS_Face& aF,
   const Standard_Real aT,
   gp_Pnt& aPNear,
   gp_Dir& aDNF,
   Standard_Real aDt2D)
{
  Standard_Real aFirst, aLast;
  Handle(Geom2d_Curve) aC2D= 
    BRep_Tool::CurveOnSurface (aE, aF, aFirst, aLast);
  
  if (aC2D.IsNull()) {
    return;
  }
  gp_Pnt2d aPx2DNear;
  PointNearEdge (aE, aF, aT, aDt2D, aPx2DNear, aPNear);
  Handle(Geom_Surface) aS=BRep_Tool::Surface(aF);
  
  BOPTools_AlgoTools3D::GetNormalToSurface 
    (aS, aPx2DNear.X(), aPx2DNear.Y(), aDNF);
  
  if (aF.Orientation()==TopAbs_REVERSED){
    aDNF.Reverse();
  }
}
//=======================================================================
//function : GetApproxNormalToFaceOnEdge
//purpose  : 
//=======================================================================
void BOPTools_AlgoTools3D::GetApproxNormalToFaceOnEdge 
  (const TopoDS_Edge& aE,
   const TopoDS_Face& aF,
   const Standard_Real aT,
   gp_Pnt& aPNear,
   gp_Dir& aDNF,
   Handle(IntTools_Context)& theContext)
{
  Standard_Real aFirst, aLast;
  Handle(Geom2d_Curve) aC2D= 
    BRep_Tool::CurveOnSurface (aE, aF, aFirst, aLast);
  
  if (aC2D.IsNull()) {
    return;
  }
  //gp_Pnt aPNear;
  gp_Pnt2d aPx2DNear;
  BOPTools_AlgoTools3D::PointNearEdge 
    (aE, aF, aT, aPx2DNear, aPNear, theContext);
  
  Handle(Geom_Surface) aS=BRep_Tool::Surface(aF);
  
  BOPTools_AlgoTools3D::GetNormalToSurface 
    (aS, aPx2DNear.X(), aPx2DNear.Y(), aDNF);
  
  if (aF.Orientation()==TopAbs_REVERSED){
    aDNF.Reverse();
  }
}
//=======================================================================
//function : PointNearEdge
//purpose  : 
//=======================================================================
void BOPTools_AlgoTools3D::PointNearEdge (const TopoDS_Edge& aE,
                                          const TopoDS_Face& aF,
                                          const Standard_Real aT, 
                                          const Standard_Real aDt2D, 
                                          gp_Pnt2d& aPx2DNear,
                                          gp_Pnt& aPxNear)
{
  Standard_Real aFirst, aLast, aETol, aFTol, transVal;
  GeomAbs_SurfaceType aTS;
  Handle(Geom2d_Curve) aC2D;
  Handle(Geom_Surface) aS;
  //
  aC2D= BRep_Tool::CurveOnSurface (aE, aF, aFirst, aLast);
  if (aC2D.IsNull()) {
    aPx2DNear.SetCoord (99., 99);
    return;
  }
  //
  aS=BRep_Tool::Surface(aF);
  //
  gp_Pnt2d aPx2D;
  gp_Vec2d aVx2D;
  aC2D->D1 (aT, aPx2D, aVx2D);
  gp_Dir2d aDx2D(aVx2D);
  
  gp_Dir2d aDP;
  aDP.SetCoord (-aDx2D.Y(), aDx2D.X());
  
  if (aE.Orientation()==TopAbs_REVERSED){
    aDP.Reverse();
  }

  if (aF.Orientation()==TopAbs_REVERSED) {
    aDP.Reverse();
  }
  //
  aETol = BRep_Tool::Tolerance(aE);
  aFTol = BRep_Tool::Tolerance(aF);
  // NPAL19220
  GeomAdaptor_Surface aGAS(aS);
  aTS=aGAS.GetType();
  if (aTS==GeomAbs_BSplineSurface) {
    if (aETol > 1.e-5) {
      aFTol=aETol;
    }
  }
  if( aETol > 1.e-5 || aFTol > 1.e-5 ) {
    //
    if(aTS!=GeomAbs_Sphere) {
      gp_Vec2d transVec( aDP );
      transVal = aDt2D + aETol + aFTol;
      if (aTS==GeomAbs_Cylinder) {// pkv/909/F8
        Standard_Real aR, dT;
        //
        gp_Cylinder aCyl=aGAS.Cylinder();
        aR=aCyl.Radius();
        dT=1.-transVal/aR;
        if (dT>=-1 && dT<=1) {
          dT=acos(dT);
          transVal=dT;
        }
      }
      //
      transVec.Multiply(transVal);
      aPx2DNear = aPx2D.Translated( transVec );
    }
    else {
      aPx2DNear.SetCoord 
        (aPx2D.X()+aDt2D*aDP.X(), aPx2D.Y()+aDt2D*aDP.Y());
    }
  }
  else {
    aPx2DNear.SetCoord 
      (aPx2D.X()+aDt2D*aDP.X(), aPx2D.Y()+aDt2D*aDP.Y());
  }
  //
  aS->D0(aPx2DNear.X(), aPx2DNear.Y(), aPxNear);
}
//=======================================================================
//function : PointNearEdge
//purpose  : 
//=======================================================================
void BOPTools_AlgoTools3D::PointNearEdge 
  (const TopoDS_Edge& aE,
   const TopoDS_Face& aF,
   const Standard_Real aT, 
   gp_Pnt2d& aPx2DNear,
   gp_Pnt& aPxNear,
   Handle(IntTools_Context)& theContext)
{
  Standard_Real aTolE, aTolF, dTx, dT2D;
  Handle(Geom_Surface) aS;
  GeomAdaptor_Surface aGAS;
  //
  dT2D=10.*BOPTools_AlgoTools3D::MinStepIn2d();//~1.e-5;
  //
  aS = BRep_Tool::Surface(aF);
  aGAS.Load(aS);
  if (aGAS.GetType()==GeomAbs_Cylinder ||
      aGAS.GetType()==GeomAbs_Sphere) {
    dT2D=10.*dT2D;
  } 
  //
  aTolE = BRep_Tool::Tolerance(aE);
  aTolF = BRep_Tool::Tolerance(aF);
  dTx = 2.*(aTolE + aTolF);
  if (dTx > dT2D) {
    dT2D=dTx;
  }
  //
  BOPTools_AlgoTools3D::PointNearEdge 
    (aE, aF, aT, dT2D, aPx2DNear, aPxNear);
  if (!theContext->IsPointInOnFace(aF, aPx2DNear)) {
    Standard_Integer iErr;
    Standard_Real aU1, aU2, aV1, aV2, dV, dU, dTresh;
    gp_Pnt aP;
    gp_Pnt2d aP2d;
    //
    BRepTools::UVBounds(aF, aU1, aU2, aV1, aV2);
    // 
    dU=aU2-aU1;
    dV=aV2-aV1;
    //
    dTresh=1.e-4;
    if (dT2D > dTresh) {
      dTresh=dT2D;
    }
    //
    if (dU < dTresh || dV < dTresh) {
      iErr = BOPTools_AlgoTools3D::PointInFace
        (aF, aP, aP2d, theContext);
      if (!iErr) {
        aPxNear = aP;
        aPx2DNear = aP2d;
      }
    }
  }
}
//=======================================================================
// function: PointNearEdge
// purpose: 
//=======================================================================
void  BOPTools_AlgoTools3D::PointNearEdge 
  (const TopoDS_Edge& aE,
   const TopoDS_Face& aF, 
   gp_Pnt2d& aPInFace2D, 
   gp_Pnt& aPInFace,
   Handle(IntTools_Context)& theContext)
{
  Standard_Real aT, aT1, aT2;
  //
  // 1. 
  BRep_Tool::Range(aE, aT1, aT2);
  aT=BOPTools_AlgoTools2D::IntermediatePoint(aT1, aT2);
  //
  // 2. a Point inside Face near aPOnEdge aPInFace;
  TopoDS_Face aFF=aF;
  TopoDS_Edge aERight;
  aFF.Orientation(TopAbs_FORWARD);
  BOPTools_AlgoTools3D::OrientEdgeOnFace (aE, aFF, aERight);
  
  BOPTools_AlgoTools3D::PointNearEdge 
    (aERight, aFF, aT, aPInFace2D, aPInFace, theContext);
}
//=======================================================================
//function : MinStepIn2d
//purpose  : 
//=======================================================================
Standard_Real BOPTools_AlgoTools3D::MinStepIn2d()
{
  Standard_Real dt=1.e-5;
  return dt;
} 
//=======================================================================
//function : IsEmptyShape
//purpose  : 
//=======================================================================
Standard_Boolean BOPTools_AlgoTools3D::IsEmptyShape
  (const TopoDS_Shape& aS)
{
  Standard_Boolean bHasGeometry=Standard_False;
  //
  BOPCol_IndexedMapOfShape myShapes;
  //
  Add(aS, myShapes, bHasGeometry);

  return !bHasGeometry;
}
//=======================================================================
//function : Add
//purpose  : 
//=======================================================================
void Add(const TopoDS_Shape& aS,
         BOPCol_IndexedMapOfShape& myShapes, 
         Standard_Boolean& bHasGeometry)
{
  Standard_Integer anIndex; 
  //
  if (bHasGeometry) {
    return;
  }
  //
  if (aS.IsNull()) {
    return;
  }
  //
  TopoDS_Shape aSx = aS;
  //
  anIndex=myShapes.FindIndex(aSx);
  if (!anIndex) {
    bHasGeometry=HasGeometry (aSx);
    if (bHasGeometry) {
      return;
    }
    //
    TopoDS_Iterator anIt(aSx, Standard_False, Standard_False);
    for(; anIt.More(); anIt.Next()) {
      const TopoDS_Shape& aSy=anIt.Value();
      Add(aSy, myShapes, bHasGeometry);
      //
      if (bHasGeometry) {
        return;
      }
      //
      myShapes.Add(aSx);
    }
  }
}
//=======================================================================
//function : HasGeometry
//purpose  : 
//=======================================================================
Standard_Boolean HasGeometry(const TopoDS_Shape& aS)
{
  Standard_Boolean bHasGeometry=Standard_True;
  TopAbs_ShapeEnum aType= aS.ShapeType();

  if (aType == TopAbs_VERTEX) {
    return bHasGeometry;
  }
  //
  else if (aType == TopAbs_EDGE) {
    Handle(BRep_TEdge) TE = Handle(BRep_TEdge)::DownCast(aS.TShape());
    BRep_ListIteratorOfListOfCurveRepresentation itrc(TE->Curves());

    while (itrc.More()) {
      const Handle(BRep_CurveRepresentation)& CR = itrc.Value();
      if (CR->IsCurve3D()) {
        if (!CR->Curve3D().IsNull()) {
          return bHasGeometry;
        }
      }
      else if (CR->IsCurveOnSurface()) {
        return bHasGeometry;
      }
      else if (CR->IsRegularity()) {
        return bHasGeometry;
      }
      else if (!CR->Polygon3D().IsNull()) {
        return bHasGeometry;
      }
      else if (CR->IsPolygonOnTriangulation()) {
        return bHasGeometry;
      }
      else if (CR->IsPolygonOnSurface()) {
        return bHasGeometry;
      }
      itrc.Next();
    }
  }
  //
  else if (aType == TopAbs_FACE) {
    Handle(BRep_TFace) TF = Handle(BRep_TFace)::DownCast(aS.TShape());
    if (!TF->Surface().IsNull())  {
      return bHasGeometry;
    }
    Handle(Poly_Triangulation) Tr = TF->Triangulation();
    if (!Tr.IsNull()) {
      return bHasGeometry;
    }
  }
  
  return !bHasGeometry;
}
//=======================================================================
//function : OrientEdgeOnFace
//purpose  : 
//=======================================================================
void BOPTools_AlgoTools3D::OrientEdgeOnFace (const TopoDS_Edge& aE,
                                             const TopoDS_Face& aF,
                                             TopoDS_Edge& aERight)
{
  if (BRep_Tool::IsClosed(aE, aF)) {
    aERight=aE;
    aERight.Orientation(aE.Orientation());

    Standard_Integer iFoundCount = 0;
    TopoDS_Edge anEdge = aE;
    TopExp_Explorer anExp(aF, TopAbs_EDGE);

    for (; anExp.More(); anExp.Next()) {
      const TopoDS_Shape& aSS=anExp.Current();
      
      if (aSS.IsSame(aE)) {
        anEdge = TopoDS::Edge(aSS);
        iFoundCount++;
      }
    }
    
    if(iFoundCount == 1) {
      aERight = anEdge;
    }
    return;
  }
  
  TopExp_Explorer anExp(aF, TopAbs_EDGE);
  for (; anExp.More(); anExp.Next()) {
    const TopoDS_Shape& aSS=anExp.Current();
    if (aSS.IsSame(aE)) {
      aERight=aE;
      aERight.Orientation(aSS.Orientation());
      return;
    }
  }
  aERight=aE;
  aERight.Orientation(aE.Orientation());
}
//=======================================================================
//function : PointInFace
//purpose  :
//=======================================================================
Standard_Integer BOPTools_AlgoTools3D::PointInFace
  (const TopoDS_Face& aF,
   gp_Pnt& theP,
   gp_Pnt2d& theP2D,
   Handle(IntTools_Context)& theContext)
{
  Standard_Boolean bIsDone, bHasFirstPoint, bHasSecondPoint;
  Standard_Integer iErr, aIx = 0, aNbDomains = 0;
  Standard_Real aUMin, aUMax, aVMin, aVMax;
  Standard_Real aVx = 0., aUx, aV1, aV2;
  gp_Dir2d aD2D (0., 1.);
  gp_Pnt2d aP2D;
  gp_Pnt aPx;
  Handle(Geom2d_Curve) aC2D;
  Handle(Geom2d_Line) aL2D;
  Handle(Geom_Surface) aS;
  TopoDS_Face aFF;
  //
  Geom2dHatch_Hatcher& aHatcher = theContext->Hatcher(aF);
  //
  iErr=0;
  //
  aFF=aF;
  aFF.Orientation (TopAbs_FORWARD);
  //
  aS=BRep_Tool::Surface(aFF);
  BRepTools::UVBounds(aFF, aUMin, aUMax, aVMin, aVMax);
  //
  aUx=IntTools_Tools::IntermediatePoint(aUMin, aUMax);
  Standard_Integer i;
  for(i = 1; i <= 2; ++i)
  {
    aP2D.SetCoord(aUx, 0.);
    aL2D=new Geom2d_Line (aP2D, aD2D);
    Geom2dAdaptor_Curve aHCur(aL2D);
    //
    aIx=aHatcher.AddHatching(aHCur) ;
    //
    aHatcher.Trim(aIx);
    bIsDone=aHatcher.TrimDone(aIx);
    if (!bIsDone) {
      iErr=1;
      return iErr;
    }
    //
    if(aHatcher.NbPoints(aIx) > 1)
    {
      aHatcher.ComputeDomains(aIx);
      bIsDone=aHatcher.IsDone(aIx);
      if (!bIsDone) {
        iErr=2;
        return iErr;
      }
      break;
    }
    else
    {
      aUx = aUMax - (aUx - aUMin);
    }
  }
  //
  if(!aHatcher.IsDone(aIx))
  {
    iErr=2;
    return iErr;
  }

  aNbDomains=aHatcher.NbDomains(aIx);
  if (aNbDomains > 0) {
    const HatchGen_Domain& aDomain=aHatcher.Domain (aIx, 1);
    bHasFirstPoint=aDomain.HasFirstPoint();
    if (!bHasFirstPoint) {
      iErr=3;
      return iErr;
    }
    //
    aV1=aDomain.FirstPoint().Parameter();
    //
    bHasSecondPoint=aDomain.HasSecondPoint();
    if (!bHasSecondPoint) {
      iErr=4;
      return iErr;
    }
    //
    aV2=aDomain.SecondPoint().Parameter();
    //
    aVx=IntTools_Tools::IntermediatePoint(aV1, aV2);
    //
  }
  else {
    iErr=2;
    return iErr;
  }
  //
  aS->D0(aUx, aVx, aPx);
  //
  theP2D.SetCoord(aUx, aVx);
  theP=aPx;
  //
  return iErr;
}
