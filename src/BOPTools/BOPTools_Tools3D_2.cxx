// File:	BOPTools_Tools3D_2.cxx
// Created:	Thu Jun 10 09:11:01 2004
// Author:	Peter KURNEV
//		<pkv@irinox>

#include <BOPTools_Tools3D.ixx>

#include <math.h>
#include <gp_Dir.hxx>
#include <gp_Cylinder.hxx>
#include <gp_Lin.hxx>
#include <gp_Ax1.hxx>
#include <gp_Cone.hxx>

#include <TopAbs_Orientation.hxx>
#include <Geom_Surface.hxx>
#include <GeomAbs_SurfaceType.hxx>
#include <BRep_Tool.hxx>
#include <GeomAdaptor_Surface.hxx>
#include <IntTools_Tools.hxx>
#include <BOPTools_Tools2D.hxx>
#include <Geom_Curve.hxx>
#include <gp_Vec.hxx>

static
  Standard_Boolean AnalyticState(const TopoDS_Face& aF1,
				 const TopoDS_Face& aFx,
				 const gp_Pnt& aP,
				 const gp_Dir& aDNSx,
				 const Standard_Real aTolR,
				 TopAbs_State& aSt);

static
  gp_Pnt ProjectedPoint (const gp_Pnt&,
			 const gp_Ax1&);

//=======================================================================
//function : TreatedAsAnalytic
//purpose  : 
//=======================================================================
  Standard_Boolean BOPTools_Tools3D::TreatedAsAnalytic(const TopoDS_Face& aFx, 
						       const TopoDS_Edge& aSpE1,
						       const TopoDS_Face& aF1,
						       const Standard_Real aTolTangent,
						       const Standard_Real aTolR,
						       TopAbs_State& aSt,
						       IntTools_Context& )
{ 
  Standard_Boolean bFlag, bIsAnalytic, bIsDirsCoinside;
  Standard_Real aT1, aTb, aTe;
  gp_Dir aDNSx, aDNS1;
  gp_Pnt aP1;
  //
  bFlag=Standard_False;
  //
  bIsAnalytic=BOPTools_Tools3D::HasAnalyticSurfaceType(aFx);
  if (!bIsAnalytic) {
    return bFlag;
  }
  bIsAnalytic=BOPTools_Tools3D::HasAnalyticSurfaceType(aF1);
  if (!bIsAnalytic) {
    return bFlag;
  }
  //
  Handle(Geom_Curve)aC3D =BRep_Tool::Curve(aSpE1, aTb, aTe);
  aT1=BOPTools_Tools2D::IntermediatePoint (aTb, aTe);
  aC3D->D0(aT1, aP1);
  //
  BOPTools_Tools3D::GetNormalToFaceOnEdge(aSpE1, aF1, aT1, aDNS1);
  BOPTools_Tools3D::GetNormalToFaceOnEdge(aSpE1, aFx, aT1, aDNSx);
  //
  bIsDirsCoinside=IntTools_Tools::IsDirsCoinside(aDNSx, aDNS1, aTolTangent);
  if (!bIsDirsCoinside) {
    return bFlag;
  }
  //
  bFlag=AnalyticState(aF1, aFx, aP1, aDNSx, aTolR, aSt);
  //
  return bFlag;
}

//=======================================================================
//function : TreatedAsAnalytic
//purpose  : 
//=======================================================================
  Standard_Boolean BOPTools_Tools3D::TreatedAsAnalytic(const Standard_Real aTx,
						       const gp_Pnt& aPx,
						       const TopoDS_Edge& anEx,
						       const TopoDS_Face& aFx, 
						       const TopoDS_Edge& anE1,
						       const TopoDS_Face& aF1,
						       const Standard_Real aTolTangent,
						       const Standard_Real aTolR,
						       TopAbs_State& aSt,
						       IntTools_Context& aContext)
{ 
  Standard_Boolean bFlag, bIsAnalytic, bIsDirsCoinside;
  Standard_Real  aT1;
  gp_Dir aDNSx, aDNS1;
  //
  bFlag=Standard_False;
  //
  bIsAnalytic=BOPTools_Tools3D::HasAnalyticSurfaceType(aFx);
  if (!bIsAnalytic) {
    return bFlag;
  }
  bIsAnalytic=BOPTools_Tools3D::HasAnalyticSurfaceType(aF1);
  if (!bIsAnalytic) {
    return bFlag;
  }
  //
  BOPTools_Tools3D::GetNormalToFaceOnEdge(anEx, aFx, aTx, aDNSx);
  aContext.ProjectPointOnEdge(aPx, anE1, aT1);
  BOPTools_Tools3D::GetNormalToFaceOnEdge(anE1, aF1, aT1, aDNS1);
  //
  bIsDirsCoinside=IntTools_Tools::IsDirsCoinside(aDNSx, aDNS1, aTolTangent);
  if (!bIsDirsCoinside) {
    return bFlag;
  }
  //
  bFlag=AnalyticState(aF1, aFx, aPx, aDNSx, aTolR, aSt);
  //
  return bFlag;
}

//=======================================================================
//function : AnalyticState
//purpose  : 
//=======================================================================
Standard_Boolean AnalyticState(const TopoDS_Face& aF1,
			       const TopoDS_Face& aFx,
			       const gp_Pnt& aP,
			       const gp_Dir& aDNSx,
			       const Standard_Real aTolR,
			       TopAbs_State& aSt)
{
  Standard_Boolean bFlag;
  Standard_Real aScPr;
  Handle(Geom_Surface) aSF1, aSFx;
  GeomAbs_SurfaceType aTypeF1, aTypeFx;
  TopAbs_Orientation anOrFx;
  gp_Dir aDNFx;
  gp_Pnt aPOnAxis;
  //
  bFlag=Standard_False;
  aSt=TopAbs_OUT;
  //
  aSF1=BRep_Tool::Surface(aF1);
  GeomAdaptor_Surface aGASF1(aSF1);
  aTypeF1=aGASF1.GetType();
  //
  aSFx=BRep_Tool::Surface(aFx);
  GeomAdaptor_Surface aGASFx(aSFx);
  aTypeFx=aGASFx.GetType();
  //
  aDNFx=aDNSx;
  anOrFx=aFx.Orientation();
  if (anOrFx==TopAbs_REVERSED){
    aDNFx.Reverse();
  }
  // 
  // Plane/Cylinder
  if (aTypeF1==GeomAbs_Plane && aTypeFx==GeomAbs_Cylinder) {
    gp_Cylinder aCYx;
    //
    aCYx=aGASFx.Cylinder();
    aPOnAxis=ProjectedPoint(aP, aCYx.Axis());
    gp_Vec aVTC(aP, aPOnAxis);
    gp_Dir aDTC(aVTC);
    //
    aScPr=aDNFx*aDTC;
    if (aScPr>0.) {
      aSt=TopAbs_IN;
    }
    bFlag=!bFlag;
    //
  } 
  //
  // Cylinder/Plane
  else if (aTypeF1==GeomAbs_Cylinder &&  aTypeFx==GeomAbs_Plane) {
    gp_Cylinder aCY1;
    //
    aCY1=aGASF1.Cylinder();
    aPOnAxis=ProjectedPoint(aP, aCY1.Axis());
    gp_Vec aVTC(aP, aPOnAxis);
    gp_Dir aDTC(aVTC);
    //
    aScPr=aDNFx*aDTC;
    if (aScPr<0.) {
      aSt=TopAbs_IN;
    }
    bFlag=!bFlag;
  } //
  //
  // Plane/Cone
  else if ( aTypeF1==GeomAbs_Plane && aTypeFx==GeomAbs_Cone) {
    gp_Cone aCNx;
    //
    aCNx=aGASFx.Cone();
    aPOnAxis=ProjectedPoint(aP, aCNx.Axis());
    gp_Vec aVTC(aP, aPOnAxis);
    gp_Dir aDTC(aVTC);
    //
    aScPr=aDNFx*aDTC;
    if (aScPr>0.) {
      aSt=TopAbs_IN;
    }
    bFlag=!bFlag;
  }
  // Cone/Plane
  else if (aTypeF1==GeomAbs_Cone && aTypeFx==GeomAbs_Plane) {
    gp_Cone aCN1;
    //
    aCN1=aGASF1.Cone();
    aPOnAxis=ProjectedPoint(aP, aCN1.Axis());
    gp_Vec aVTC(aP, aPOnAxis);
    gp_Dir aDTC(aVTC);
    //
    aScPr=aDNFx*aDTC;
    if (aScPr<0.) {
      aSt=TopAbs_IN;
    }
    bFlag=!bFlag;
  } //
  // 
  // Cylinder(Cone)/Cylinder(Cone)
  else if ((aTypeF1==GeomAbs_Cylinder || aTypeF1==GeomAbs_Cone) && 
	   (aTypeFx==GeomAbs_Cylinder || aTypeFx==GeomAbs_Cone)) {
    //
    Standard_Real aPr, aR1, aRx, aSemiAngle, aDist; 
    gp_Pnt aPOnAxis1, aPOnAxisx;
    gp_Cylinder aCY1, aCYx;
    gp_Cone aCN1, aCNx;
    gp_Ax1 anAx1, anAxx;
    //
    // surface 1
    if (aTypeF1==GeomAbs_Cylinder) {
      aCY1=aGASF1.Cylinder();
      anAx1=aCY1.Axis();
      aR1=aCY1.Radius();
      aPOnAxis1=ProjectedPoint(aP, anAx1);
    }
    else {
      aCN1=aGASF1.Cone();
      anAx1=aCN1.Axis();
      aSemiAngle=aCN1.SemiAngle();
      gp_Lin aLin(anAx1);
      aDist=aLin.Distance(aP);
      aR1=aDist/cos(aSemiAngle);
      aPOnAxis1=ProjectedPoint(aP, anAx1);
    }
    // surface x
    if (aTypeFx==GeomAbs_Cylinder) {
      aCYx=aGASFx.Cylinder();
      anAxx=aCYx.Axis();
      aRx=aCYx.Radius();
      aPOnAxisx=ProjectedPoint(aP, anAxx);
    }
    else {
      aCNx=aGASFx.Cone();
      anAxx=aCNx.Axis();
      aSemiAngle=aCNx.SemiAngle();
      gp_Lin aLin(anAxx);
      aDist=aLin.Distance(aP);
      aRx=aDist/cos(aSemiAngle);
      aPOnAxisx=ProjectedPoint(aP, anAxx);
    }
    //
    if (fabs(aRx-aR1) < aTolR) {
      return bFlag;
    }
    //
    gp_Vec aVTC1(aP, aPOnAxis1);
    gp_Vec aVTCx(aP, aPOnAxisx);
    gp_Dir aDTC1(aVTC1);
    gp_Dir aDTCx(aVTCx);
    //
    aPr=aDTC1*aDTCx;
    if (aPr < 0.) {
      // opposite case
      aScPr=aDNFx*aDTCx;
      if (aScPr>0.) {
	aSt=TopAbs_IN;
      }
      
    }
    else {
      // one inside other
      aScPr=aDNFx*aDTC1;
      if (aRx<aR1) {
	if (aScPr>0.) {
	  aSt=TopAbs_IN;
	}
      }
      else {
	if (aScPr<0.) {
	  aSt=TopAbs_IN;
	}
      }
    }
    bFlag=!bFlag;
    //
  }
  //
  return bFlag;
}

//=======================================================================
//function : HasAnalyticSurfaceType
//purpose  : 
//=======================================================================
  Standard_Boolean BOPTools_Tools3D::HasAnalyticSurfaceType(const TopoDS_Face& aF)
{
  Standard_Boolean bFlag=Standard_False;
  GeomAbs_SurfaceType aType;
  //
  Handle(Geom_Surface) aS;
  //
  aS=BRep_Tool::Surface(aF);
  GeomAdaptor_Surface aGAS(aS);
  aType=aGAS.GetType();

  //
  bFlag= (aType==GeomAbs_Plane || 
	  aType==GeomAbs_Cylinder || 
	  aType==GeomAbs_Cone ||
	  aType==GeomAbs_Sphere);
  //
  return bFlag;
}

//=======================================================================
//function :ProjectedPoint
//purpose  : 
//=======================================================================
  gp_Pnt ProjectedPoint (const gp_Pnt& aP,
			 const gp_Ax1& anAx1)
{
  Standard_Real aDist;
  //
  gp_Vec aVDirection(anAx1.Direction());
  gp_Pnt anOrigin=anAx1.Location();
  gp_Vec aV(anOrigin, aP);
  aDist = aVDirection.Dot(aV);
  //
  gp_Pnt aPx= anOrigin.Translated(aDist*aVDirection);
  return aPx;
}
