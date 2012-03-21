// Created on: 2001-06-18
// Created by: Peter KURNEV
// Copyright (c) 2001-2012 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.


#include <BOPTools_Tools3D.ixx>
#include <math.h>

#include <gp.hxx>
#include <gp_Cylinder.hxx>
#include <gp_Pnt2d.hxx>
#include <gp_Vec2d.hxx>
#include <gp_Dir2d.hxx>

#include <gp_Dir.hxx>
#include <gp_Vec.hxx>

#include <Geom2d_Curve.hxx>
#include <Geom_Curve.hxx>
#include <Geom_Surface.hxx>

#include <GeomAdaptor_Surface.hxx>
#include <GeomAPI_ProjectPointOnSurf.hxx>

#include <BRepTools.hxx>
#include <BRepClass3d_SolidClassifier.hxx>

#include <BRep_Tool.hxx>
#include <BRepAdaptor_Surface.hxx>

#include <TopTools_ListIteratorOfListOfShape.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopTools_ListOfShape.hxx>

#include <BOPTools_Tools2D.hxx>


//=======================================================================
//function : GetApproxNormalToFaceOnEdge
//purpose  : 
//=======================================================================
void BOPTools_Tools3D::GetApproxNormalToFaceOnEdge (const TopoDS_Edge& aE,
						      const TopoDS_Face& aF,
						      const Standard_Real aT,
						      gp_Pnt& aPNear,
						      gp_Dir& aDNF)
{
  Standard_Real aFirst, aLast;
  Handle(Geom2d_Curve) aC2D= BRep_Tool::CurveOnSurface (aE, aF, aFirst, aLast);
  
  if (aC2D.IsNull()) {
    return;
  }
  //gp_Pnt aPNear;
  gp_Pnt2d aPx2DNear;
  BOPTools_Tools3D::PointNearEdge (aE, aF, aT, aPx2DNear, aPNear);
  
  Handle(Geom_Surface) aS=BRep_Tool::Surface(aF);
  
  BOPTools_Tools3D::GetNormalToSurface (aS, aPx2DNear.X(), aPx2DNear.Y(), aDNF);
  
  if (aF.Orientation()==TopAbs_REVERSED){
    aDNF.Reverse();
  }
}
//=======================================================================
//function : PointNearEdge
//purpose  : 
//=======================================================================
  void BOPTools_Tools3D::PointNearEdge (const TopoDS_Edge& aE,
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
  // pkv NPAL19220
  GeomAdaptor_Surface aGAS(aS);
  aTS=aGAS.GetType();
  if (aTS==GeomAbs_BSplineSurface) {
    if (aETol > 1.e-5) {
      aFTol=aETol;
    }
  }
  //modified by NIZNHY-PKV Thu Mar 19 14:15:15 2009f
  if( aETol > 1.e-5 || aFTol > 1.e-5 ) {
  //if( aETol > 1.e-5 && aFTol > 1.e-5 ) {
    //modified by NIZNHY-PKV Thu Mar 19 14:15:24 2009t
    //pkv/103/D7
    if(aTS!=GeomAbs_Sphere) {
      gp_Vec2d transVec( aDP );
      transVal = aDt2D + aETol + aFTol;
      if (aTS==GeomAbs_Cylinder) {// pkv/909/F8
	Standard_Real aR, dT;
	//
	gp_Cylinder aCyl=aGAS.Cylinder();
	aR=aCyl.Radius();
	dT=1.-transVal/aR;
	dT=acos(dT);
	transVal=dT;
      }
      //
      transVec.Multiply(transVal);
      //
      aPx2DNear = aPx2D.Translated( transVec );
    }
    else {
      aPx2DNear.SetCoord (aPx2D.X()+aDt2D*aDP.X(), aPx2D.Y()+aDt2D*aDP.Y());
    }
  }
  else {
    aPx2DNear.SetCoord (aPx2D.X()+aDt2D*aDP.X(), aPx2D.Y()+aDt2D*aDP.Y());
  }
  //
  aS->D0(aPx2DNear.X(), aPx2DNear.Y(), aPxNear);
}

//=======================================================================
//function : PointNearEdge
//purpose  : 
//=======================================================================
  void BOPTools_Tools3D::PointNearEdge (const TopoDS_Edge& aE,
					const TopoDS_Face& aF,
					const Standard_Real aT, 
					gp_Pnt2d& aPx2DNear,
					gp_Pnt& aPxNear)
{
  Standard_Real dt2D=BOPTools_Tools3D::MinStepIn2d();//~1.e-5;
  //
  BOPTools_Tools3D::PointNearEdge (aE, aF, aT, dt2D, aPx2DNear, aPxNear);
}

//=======================================================================
// function: PointNearEdge
// purpose: 
//=======================================================================
  void  BOPTools_Tools3D::PointNearEdge (const TopoDS_Edge& aE,
					 const TopoDS_Face& aF, 
					 gp_Pnt2d& aPInFace2D, 
					 gp_Pnt& aPInFace)
				  
{
  Standard_Real aT, aT1, aT2;
  //
  // 1. 
  BRep_Tool::Range(aE, aT1, aT2);
  aT=BOPTools_Tools2D::IntermediatePoint(aT1, aT2);
  //
  // 2. a Point inside Face near aPOnEdge aPInFace;
  TopoDS_Face aFF=aF;
  TopoDS_Edge aERight;
  aFF.Orientation(TopAbs_FORWARD);
  BOPTools_Tools3D::OrientEdgeOnFace (aE, aFF, aERight);
  
  BOPTools_Tools3D::PointNearEdge (aERight, aFF, aT, aPInFace2D, aPInFace);
}


//=======================================================================
//function : PointToCompare
//purpose  : 
//=======================================================================
  void BOPTools_Tools3D::PointToCompare (const gp_Pnt& aP1,
					 const gp_Pnt& aP2,
					 const TopoDS_Face& aF,
					 gp_Pnt& aPF,
					 const Handle(IntTools_Context)& aContext)
{
  Standard_Boolean bFlag;
  Standard_Real aD, aTolF,  U, V;

  Handle(Geom_Surface) aS=BRep_Tool::Surface(aF);

  aTolF=BRep_Tool::Tolerance(aF);
  //
  GeomAPI_ProjectPointOnSurf& aProjector=aContext->ProjPS(aF);
  //
  aProjector.Perform(aP1);
  //
  bFlag=aProjector.IsDone();
  
  if (bFlag) {
    aD=aProjector.LowerDistance();
    if (aD<aTolF) {
      aProjector.LowerDistanceParameters (U, V);
      aS->D0(U, V, aPF);
      return;
    }
  }
  //
  aProjector.Perform(aP2);
  //
  bFlag=aProjector.IsDone();

  if (bFlag) {
    aD=aProjector.LowerDistance();
    if (aD<aTolF) {
      aProjector.LowerDistanceParameters (U, V);
      aS->D0(U, V, aPF);
      return;
    }
  }

  aPF=aP1;
}

//=======================================================================
//function : GetPlane
//purpose  : 
//=======================================================================
  void BOPTools_Tools3D::GetPlane (const TopoDS_Edge& aSpEF1,
				   const TopoDS_Edge& aEF1,
				   const TopoDS_Face& aF1,
				   const TopoDS_Face& aF2,
				   TopAbs_State& aStPF,
				   const Handle(IntTools_Context)& aContext)
{
  Standard_Real aT1, aT2, aT, aTolF2, aDt2D;
  gp_Pnt2d aPx2DNear;
  gp_Pnt aPxNear, aPxF2;
  gp_Dir aDNF2;

  Handle(Geom_Curve)aC3D =BRep_Tool::Curve(aSpEF1, aT1, aT2);
  aT=BOPTools_Tools2D::IntermediatePoint(aT1, aT2);

  Handle(Geom2d_Curve) aC2D= BRep_Tool::CurveOnSurface (aEF1, aF1, aT1, aT2);

  aStPF=TopAbs_OUT;
  //
  aDt2D=BOPTools_Tools3D::MinStepIn2d();//~1.e-5;
  aTolF2=BRep_Tool::Tolerance(aF2);
  //
  {
    GeomAbs_SurfaceType aType1;
    
    Handle(Geom_Surface) aS1=BRep_Tool::Surface(aF1);
    GeomAdaptor_Surface aGASF1(aS1);
    aType1=aGASF1.GetType();
    
    if (aType1==GeomAbs_Cylinder) {
      Standard_Real aCT, aDt2Dx, aPC=0.99;
      
      aCT=1.-aGASF1.UResolution(aTolF2);
      aDt2Dx=aPC*acos(aCT);
      if (aDt2Dx>aDt2D) {
	aDt2D=aDt2Dx;
      }
    }
  }
  //
  BOPTools_Tools3D::PointNearEdge (aEF1, aF1, aT, aDt2D, aPx2DNear, aPxNear);
  //

  //-- EJG
  Standard_Boolean isIn = aContext->IsPointInFace(aF1,aPx2DNear);
  if( !isIn ) {
    Standard_Real aEF1Tol = BRep_Tool::Tolerance(aEF1);
    Standard_Real aF1Tol = BRep_Tool::Tolerance(aF1);
    if( aEF1Tol > 1.e-5 || ( aF1Tol > 1.e-5 || aTolF2 > 1.e-5 ) ) {
      gp_Pnt2d aP2DOnC; 
      aC2D->D0(aT, aP2DOnC);
      gp_Vec2d aP2Dir( aPx2DNear, aP2DOnC );
      Standard_Real transVal = aP2Dir.Magnitude();
      gp_Vec2d aP2DirN = aP2Dir.Normalized();
      if( aF1Tol > 1.e-5 && aTolF2 > 1.e-5 ) {
	transVal = 2.*transVal +aEF1Tol + aF1Tol + aTolF2;
      }
      else {
	transVal *= 2.;
      }
      aPx2DNear.Translate( (transVal*aP2DirN) );
      Handle(Geom_Surface) aS1 = BRep_Tool::Surface(aF1);
      aS1->D0(aPx2DNear.X(), aPx2DNear.Y(), aPxNear);

    }
  }
  //-- EJG

  Standard_Boolean bFlag;
  Standard_Real aD, U, V;
  //
  GeomAPI_ProjectPointOnSurf& aProjector=aContext->ProjPS(aF2);
  //
  Handle(Geom_Surface) aS2=BRep_Tool::Surface(aF2);
  //
  aProjector.Perform(aPxNear);
  //
  bFlag=aProjector.IsDone();
  
  if (!bFlag) {
    return;
  }
  //
  aD=aProjector.LowerDistance();
  if (aD<aTolF2) {
    // aPxF2, aDNF2, aPlnF2
    aProjector.LowerDistanceParameters (U, V);
    aS2->D0(U, V, aPxF2);
    
    BOPTools_Tools3D::GetNormalToSurface (aS2, U, V, aDNF2);
  
    if (aF2.Orientation()==TopAbs_REVERSED){
      aDNF2.Reverse();
    }

    gp_Pln aPlnF2(aPxF2, aDNF2);
    //
    aD=BOPTools_Tools3D::SignDistance(aPxNear, aPlnF2);
    
    if (aD<=0.) {
      aStPF=TopAbs_IN;
    }
  }
}

//=======================================================================
//function : GetPointState
//purpose  : 
//=======================================================================
  void BOPTools_Tools3D::GetPointState (const TopoDS_Edge& aSpEF2,
					const TopoDS_Edge& aEF2,
					const TopoDS_Face& aF2adj,
					const TopoDS_Face& aF1,
					TopAbs_State& aStPF)
{
  Standard_Real aT1, aT2, aT, aTolEF2;
  gp_Dir aDNF1;
  gp_Pnt2d aPxOnF1, aPxOnF2;
  //
  TopoDS_Face aF2adjF=aF2adj;
  aF2adjF.Orientation(TopAbs_FORWARD);
  //
  aTolEF2=BRep_Tool::Tolerance(aEF2);
  //
  aStPF=TopAbs_OUT;
  //
  Handle(Geom_Curve)aC3D =BRep_Tool::Curve(aSpEF2, aT1, aT2);
  aT=BOPTools_Tools2D::IntermediatePoint(aT1, aT2);
  //
  // 2D-curves of aSpEF2 for aF1, aF2adj
  Handle(Geom2d_Curve) aC2DF1= BRep_Tool::CurveOnSurface (aSpEF2, aF1, aT1, aT2);
  aC2DF1->D0(aT, aPxOnF1);

  Handle(Geom2d_Curve) aC2DF2= BRep_Tool::CurveOnSurface (aSpEF2, aF2adjF, aT1, aT2);
  aC2DF2->D0(aT, aPxOnF2);
  //
  // Surfaces
  Handle(Geom_Surface) aS1=BRep_Tool::Surface(aF1);
  Handle(Geom_Surface) aS2=BRep_Tool::Surface(aF2adjF);
  
  //
  // Step aDt2D 
  Standard_Real aDt2D, aDt2dMin, aURes, aVRes;
  //
  GeomAdaptor_Surface aGASF2adjF(aS2);
  //
  aURes=aGASF2adjF.UResolution(aTolEF2);
  aVRes=aGASF2adjF.VResolution(aTolEF2);
  aDt2D=(aURes>aVRes) ? aURes : aVRes;
  //
  aDt2dMin=BOPTools_Tools3D::MinStepIn2d();
  //
  if (aDt2D < aDt2dMin) {
    aDt2D=aDt2dMin;
  }
  //
  // Point aPxNear on aF2adjF that is Near Edge aEF2Right at parameter aT
  gp_Pnt2d aPx2DNear;
  gp_Pnt aPxNear;
  TopoDS_Edge aEF2Right;
  //
  BOPTools_Tools3D::OrientEdgeOnFace (aEF2, aF2adjF, aEF2Right);
  //
  BOPTools_Tools3D::PointNearEdge (aEF2Right, aF2adjF, aT, aDt2D, aPx2DNear, aPxNear);
  //
  // Normal to the face aF1 at the point aPxOnF1
  BOPTools_Tools3D::GetNormalToSurface (aS1, aPxOnF1.X(), aPxOnF1.Y(), aDNF1);
  if (aF1.Orientation()==TopAbs_REVERSED){
    aDNF1.Reverse();
  }
  //
  // Plane to compare aPlnF1
  gp_Pnt aPxF1, aPxF2;
  //
  aS1->D0(aPxOnF1.X(), aPxOnF1.Y(), aPxF1);
  //
  gp_Pln aPlnF1(aPxF1, aDNF1);
  //
  // Correction on shifting vector aVx
  {
    Standard_Real aX, aY, aZ;
    //
    aS2->D0(aPxOnF2.X(), aPxOnF2.Y(), aPxF2);
    gp_Vec aVx(aPxF2, aPxF1);
    //
    aX=aPxNear.X()+aVx.X();
    aY=aPxNear.Y()+aVx.Y();
    aZ=aPxNear.Z()+aVx.Z();
    //
    aPxNear.SetCoord(aX, aY, aZ);
  }
  //
  // Signed Distance between aPxNear, aPlnF1
  Standard_Real aD;
  aD=BOPTools_Tools3D::SignDistance(aPxNear, aPlnF1);
  if (aD<=0.) {
    aStPF=TopAbs_IN;
  }
}

//=======================================================================
//function : OrientEdgeOnFace
//purpose  : 
//=======================================================================
  void BOPTools_Tools3D::OrientEdgeOnFace (const TopoDS_Edge& aE,
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
//function : OrientTouchEdgeOnF1
//purpose  : 
//=======================================================================
  TopAbs_Orientation BOPTools_Tools3D::OrientTouchEdgeOnF1 (const TopoDS_Edge& aSpEF2,
							    const TopoDS_Edge& aEF2,
							    const TopoDS_Face& aF2adj,
							    const TopoDS_Face& aF1)
{
  Standard_Real aT1, aT2, aT;
  //
  Handle(Geom_Curve)aC3D =BRep_Tool::Curve(aSpEF2, aT1, aT2);
  //
  // point on edge aEF2 inside range of aSpEF2:
  gp_Pnt aPx;
  aT=BOPTools_Tools2D::IntermediatePoint(aT1, aT2);
  aC3D->D0(aT, aPx);
  //
  // Normal to aF1
  gp_Dir aDNF1;
  Handle(Geom2d_Curve) aC2DF1= BRep_Tool::CurveOnSurface (aSpEF2, aF1, aT1, aT2);
  gp_Pnt2d aPxOnF1;
  aC2DF1->D0(aT, aPxOnF1);

  Handle(Geom_Surface) aS1=BRep_Tool::Surface(aF1);
  BOPTools_Tools3D::GetNormalToSurface (aS1, aPxOnF1.X(), aPxOnF1.Y(), aDNF1);
  
  if (aF1.Orientation()==TopAbs_REVERSED){
    aDNF1.Reverse();
  }
  //
  // Point  aPxNear that is ON F2adj and near Px
  gp_Pnt2d aPx2DNear;
  gp_Pnt aPxNear;
  
  Handle(Geom2d_Curve) aC2D= BRep_Tool::CurveOnSurface (aEF2, aF2adj, aT1, aT2);
  
  TopoDS_Face aF2adjF=aF2adj;
  aF2adjF.Orientation(TopAbs_FORWARD);
  TopoDS_Edge aEF2Right;
  BOPTools_Tools3D::OrientEdgeOnFace (aEF2, aF2adjF, aEF2Right);
  BOPTools_Tools3D::PointNearEdge (aEF2Right, aF2adjF, aT, aPx2DNear, aPxNear);
  //
  // Normal to aF2adj
  gp_Dir aDNF2;
  Handle(Geom_Surface) aS2=BRep_Tool::Surface(aF2adj);
  BOPTools_Tools3D::GetNormalToSurface (aS2, aPx2DNear.X(), aPx2DNear.Y(), aDNF2);
  
  if (aF2adj.Orientation()==TopAbs_REVERSED){
    aDNF2.Reverse();
  }
  //
  // vector product
  gp_Dir aDTN=aDNF1^aDNF2;
  // Tangent for split
  gp_Vec aTE;
  BOPTools_Tools2D::TangentOnEdge(aT, aSpEF2, aTE);
  gp_Dir aDTE(aTE); 
  
  Standard_Real aScPr;
  aScPr=aDTN*aDTE;
  
  TopAbs_Orientation anOr;
  anOr=TopAbs_FORWARD;
  if (aScPr<0.) {
    anOr=TopAbs_REVERSED;
  }
  return anOr;
}

//=======================================================================
// function: GetSeams
// purpose: 
//=======================================================================
  void BOPTools_Tools3D::GetSeams (const TopoDS_Face& aF,
				   TopoDS_Edge& aSim1,
				   TopoDS_Edge& aSim2)
{
  TopTools_ListOfShape aLS;
  TopExp_Explorer anExpEdges (aF, TopAbs_EDGE);
  for (; anExpEdges.More(); anExpEdges.Next()) {
    const TopoDS_Edge& aE= TopoDS::Edge(anExpEdges.Current());
    if (BRep_Tool::IsClosed(aE, aF)) {
      aLS.Append(aE);
    }
  }
  aSim1=TopoDS::Edge(aLS.First());
  aSim2=TopoDS::Edge(aLS.Last ());
}

//=======================================================================
// function: GetSeam
// purpose: 
//=======================================================================
  void BOPTools_Tools3D::GetSeam (const TopoDS_Face& aF,
				  const TopoDS_Edge& aSim1,
				  TopoDS_Edge& aSim2)
{
  TopExp_Explorer anExpEdges (aF, TopAbs_EDGE);
  for (; anExpEdges.More(); anExpEdges.Next()) {
    const TopoDS_Edge& aE= TopoDS::Edge(anExpEdges.Current());
    if (BRep_Tool::IsClosed(aE, aF)) {
      if (aE.IsSame(aSim1)) {
	if (aE!=aSim1) {
	  aSim2=aE;
	  return;
	}
      }
    }
  }
}

//=======================================================================
//function : MinStepIn2d
//purpose  : 
//=======================================================================
  Standard_Real BOPTools_Tools3D::MinStepIn2d()
{
  Standard_Real dt=1.e-5;
  return dt;
} 


#include <TopTools_IndexedMapOfShape.hxx>
#include <TopoDS_Iterator.hxx>
#include <BRep_TVertex.hxx>
#include <BRep_ListIteratorOfListOfPointRepresentation.hxx>
#include <BRep_PointRepresentation.hxx>
#include <BRep_TEdge.hxx>
#include <BRep_ListIteratorOfListOfCurveRepresentation.hxx>
#include <BRep_CurveRepresentation.hxx>
#include <BRep_TFace.hxx>
#include <Poly_Triangulation.hxx>
#include <BRep_Builder.hxx>

static 
  Standard_Boolean HasGeometry(const TopoDS_Shape& aS);

static
  void Add(const TopoDS_Shape& aS,
	   TopTools_IndexedMapOfShape& myShapes, 
	   Standard_Boolean& bHasGeometry);

//=======================================================================
//function : IsEmptyShape
//purpose  : 
//=======================================================================
  Standard_Boolean BOPTools_Tools3D::IsEmptyShape(const TopoDS_Shape& aS)
{
  Standard_Boolean bHasGeometry=Standard_False;
  //
  TopTools_IndexedMapOfShape myShapes;
  //
  Add(aS, myShapes, bHasGeometry);

  return !bHasGeometry;
}
//=======================================================================
//function : Add
//purpose  : 
//=======================================================================
void Add(const TopoDS_Shape& aS,
	 TopTools_IndexedMapOfShape& myShapes, 
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
    
    Handle(BRep_TVertex) TV = Handle(BRep_TVertex)::DownCast(aS.TShape());
    BRep_ListIteratorOfListOfPointRepresentation itrp(TV->Points());
    
    while (itrp.More()) {
      const Handle(BRep_PointRepresentation)& PR = itrp.Value();

      if (PR->IsPointOnCurve()) {
        return bHasGeometry;
      }

      else if (PR->IsPointOnCurveOnSurface()) {
	return bHasGeometry;
      }

      else if (PR->IsPointOnSurface()) {
        return bHasGeometry;
      }
      itrp.Next();
    }
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
// function: InvertShape
// purpose: 
//=======================================================================
  void BOPTools_Tools3D::InvertShape(const TopoDS_Shape& aS,
				     TopoDS_Shape& aSInvert)
{
  BRep_Builder aBB;
  
  aSInvert=aS.EmptyCopied();
  
  TopoDS_Iterator anIt(aS);

  while (anIt.More()) {
    aBB.Add(aSInvert, anIt.Value().Reversed());
    anIt.Next();
  }
}

//=======================================================================
// function: GetStatePartIN2D
// purpose: 
//=======================================================================
  TopAbs_State  BOPTools_Tools3D::GetStatePartIN2D(const TopoDS_Edge& aSp,
						   const TopoDS_Edge& aEF1,
						   const TopoDS_Face& aF1,
						   const TopoDS_Face& aF2,
						   const Handle(IntTools_Context)& aContext)
{
  gp_Dir aDBF1, aDNF2;
  TopAbs_State aStPF;

  BOPTools_Tools3D::GetBiNormal (aSp, aF1, aDBF1);
  BOPTools_Tools3D::GetNormalToFaceOnEdge (aSp, aF2, aDNF2);

  Standard_Real aTolScPr, aScPr;

  aTolScPr=1.e-7;
  aScPr=aDBF1*aDNF2;

  //XX 909/G2
  BRepAdaptor_Surface aBAS1, aBAS2;
  GeomAbs_SurfaceType aType1, aType2;

  aBAS1.Initialize (aF1, Standard_False);
  aBAS2.Initialize (aF2, Standard_False);
  aType1=aBAS1.GetType();
  aType2=aBAS2.GetType();
  
  if (aType1==GeomAbs_BSplineSurface
      ||
      aType2==GeomAbs_BSplineSurface) {
    //aTolScPr=1.e-5;
    aTolScPr=5.5e-5;
  }
  //XX

  if (fabs(aScPr) < aTolScPr) {
    
    
    BOPTools_Tools3D::GetPlane(aSp, aEF1, aF1, aF2, aStPF, aContext);
    aScPr=1.; // >0.
    if (aStPF==TopAbs_IN) {
      aScPr=-1.; // <0.
    }
  }

  aStPF=TopAbs_OUT;
  if (aScPr<0.) {
    aStPF=TopAbs_IN;
  }
  return aStPF;
}
// ===========================================================================================
// function: ComputeFaceState
// purpose: 
// ===========================================================================================
Standard_Boolean BOPTools_Tools3D::ComputeFaceState(const TopoDS_Face&  theFace,
						    const TopoDS_Solid& theRef,
						    const Handle(IntTools_Context)& theContext,
						    TopAbs_State&       theState) 
{
  TopAbs_State aState = TopAbs_ON;

  Standard_Real umin = 0., umax = 0., vmin = 0., vmax = 0.;
  BRepTools::UVBounds(theFace, umin, umax, vmin, vmax);
  Handle(Geom_Surface) aSurface = BRep_Tool::Surface(theFace);
  Standard_Real aTolerance = BRep_Tool::Tolerance(theFace);

  Standard_Integer nbpoints = 5;
  Standard_Real adeltau = (umax - umin) / (nbpoints + 1);
  Standard_Real adeltav = (vmax - vmin) / (nbpoints + 1);
  Standard_Real U = umin + adeltau;
  Standard_Boolean bFoundValidPoint = Standard_False;
  Standard_Boolean bFoundInFacePoint = Standard_False;
  BRepClass3d_SolidClassifier& aSolidClassifier = theContext->SolidClassifier(theRef);
  Standard_Integer i = 0, j = 0;

  for(i = 1; !bFoundValidPoint && (i <= nbpoints); i++, U+=adeltau) {
    Standard_Real V = vmin + adeltav;

    for(j = 1; !bFoundValidPoint && (j <= nbpoints); j++, V+=adeltav) {
      gp_Pnt2d aPoint(U,V);

      if(theContext->IsPointInFace(theFace, aPoint)) {
	bFoundInFacePoint = Standard_True;
	gp_Pnt aP3d = aSurface->Value(U, V);

	aSolidClassifier.Perform(aP3d, aTolerance);
	aState = aSolidClassifier.State();

	if(aState != TopAbs_ON) {
	  
	  if(!aSolidClassifier.Rejected()) {
	    TopoDS_Face aFace2 = aSolidClassifier.Face();

	    if(!aFace2.IsNull()) {
	      if(BOPTools_Tools3D::CheckSameDomainFaceInside(theFace, aFace2, theContext))
		aState = TopAbs_ON;
	      else {
		break;
	      }
	    }
	  }
	}
      }
    }
  }

  if(!bFoundInFacePoint) {
    U = (umin + umax) * 0.5;
    nbpoints /= 2;

    for(i = 1; !bFoundValidPoint && (i <= nbpoints); i++, U+=adeltau) {
      Standard_Real V = (vmin + vmax) * 0.5;

      for(j = 1; !bFoundValidPoint && (j <= nbpoints); j++, V+=adeltav) {
	gp_Pnt2d aPoint(U,V);

	if(theContext->IsPointInOnFace(theFace, aPoint)) {
	  bFoundInFacePoint = Standard_True;
	  gp_Pnt aP3d = aSurface->Value(U, V);
    
	  aSolidClassifier.Perform(aP3d, aTolerance);
	  aState = aSolidClassifier.State();

	  if(aState != TopAbs_ON) {

	    if(!aSolidClassifier.Rejected()) {
	      TopoDS_Face aFace2 = aSolidClassifier.Face();

	      if(!aFace2.IsNull()) {
		GeomAPI_ProjectPointOnSurf& aProjector = theContext->ProjPS(aFace2);
		aProjector.Perform(aP3d);

		if(aProjector.IsDone()) {
		  Standard_Real U2 = 0., V2 = 0.;
		  aProjector.LowerDistanceParameters(U2, V2);
		  gp_Pnt2d aPoint2(U2, V2);

		  if(aProjector.LowerDistance() < aTolerance) {
		    if(theContext->IsPointInFace(aFace2, aPoint2)) 
		      aState = TopAbs_ON;
		  }
		}
		bFoundValidPoint = Standard_True;
		break;
	      }
	    }
	    else {
	      bFoundInFacePoint = Standard_False;
	    }
	  }
	}
      }
    }
  }

  if(!bFoundInFacePoint)
    return Standard_False;

  theState = aState;

  return Standard_True;
}
//modified by NIZNHY-PKV Thu Sep 22 10:55:14 2011f
// ===========================================================================================
// function: CheckSameDomainFaceInside
// purpose: Check if distance between several points of theFace1 and
//          theFace2 
// ===========================================================================================
Standard_Boolean BOPTools_Tools3D::CheckSameDomainFaceInside(const TopoDS_Face& theFace1,
							     const TopoDS_Face& theFace2,
							     const Handle(IntTools_Context)& theContext) 
{
  Standard_Boolean bFoundON, bPointInFace; 
  Standard_Integer nbpoints, i, j;
  Standard_Real umin, umax, vmin, vmax, aTol, adeltau, adeltav, U, V, U2, V2, aD, aTolE;
  gp_Pnt2d aP2D;
  gp_Pnt aP3D; 
  TopExp_Explorer aExp;
  //
  BRepTools::UVBounds(theFace1, umin, umax, vmin, vmax);
  Handle(Geom_Surface) aS1=BRep_Tool::Surface(theFace1);
  //
  aTol=BRep_Tool::Tolerance(theFace1);
  aExp.Init(theFace1, TopAbs_EDGE);
  for(; aExp.More(); aExp.Next()) {
    const TopoDS_Edge& aE = TopoDS::Edge(aExp.Current());
    aTolE = BRep_Tool::Tolerance(aE);
    aTol=(aTol < aTolE) ? aTolE : aTol;
  }
  aTol=aTol+BRep_Tool::Tolerance(theFace2);
  //
  nbpoints=5;
  adeltau=(umax - umin) / (nbpoints + 1);
  adeltav=(vmax - vmin) / (nbpoints + 1);
  bFoundON = Standard_False;
  //
  GeomAPI_ProjectPointOnSurf& aProjector = theContext->ProjPS(theFace2);
  //
  for(i=1; i<=nbpoints; ++i){
    U=umin+i*adeltau;
    for(j=1; j<=nbpoints; ++j) {
      V=vmin+j*adeltav;
      aP2D.SetCoord(U,V);
      bPointInFace=theContext->IsPointInFace(theFace1, aP2D);
      if(bPointInFace) {
	aP3D=aS1->Value(U, V);
	aProjector.Perform(aP3D);
	if(aProjector.IsDone()) {
	  aProjector.LowerDistanceParameters(U2, V2);
	  aP2D.SetCoord(U2, V2);
	  //
	  aD=aProjector.LowerDistance();
	  if(aD > aTol) {
	    return Standard_False;
	  }
	  //
	  bPointInFace=theContext->IsPointInFace(theFace2, aP2D);
	  if (bPointInFace) {
	    bFoundON = Standard_True;
	  }
	}
      }
    }
  }
  return bFoundON;
}
//modified by NIZNHY-PKV Thu Sep 22 10:55:19 2011t
