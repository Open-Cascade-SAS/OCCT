// File:	IntTools_Context.cxx
// Created:	Wed Apr  3 16:57:54 2002
// Author:	Peter KURNEV
//		<pkv@irinox>


#include <IntTools_Context.ixx>

#include <Precision.hxx>

#include <Geom_Curve.hxx>
#include <Geom_BoundedCurve.hxx>
#include <GeomAPI_ProjectPointOnCurve.hxx>
#include <GeomAPI_ProjectPointOnSurf.hxx>
#include <GeomAdaptor_Curve.hxx>

#include <TopAbs_State.hxx>
#include <TopoDS.hxx>
#include <TopExp_Explorer.hxx>

#include <BRep_Tool.hxx>
#include <BRepAdaptor_Surface.hxx>

#include <IntTools_Tools.hxx>
#include <IntTools_FClass2d.hxx>
// 
#include <Extrema_LocateExtPC.hxx>

#include <Geom2d_Curve.hxx>

//=======================================================================
//function : 
//purpose  : 
//=======================================================================
  IntTools_Context::IntTools_Context()
{
}
//=======================================================================
//function : ~
//purpose  : 
//=======================================================================
  IntTools_Context::~IntTools_Context()
{
  Standard_Address anAdr;
  Standard_Integer i, aNb;
  //
  IntTools_FClass2d* pFClass2d;
  aNb=myFClass2dMap.Extent();
  for (i=1; i<=aNb; ++i) {
    anAdr=myFClass2dMap(i);
    pFClass2d=(IntTools_FClass2d*)anAdr;
    delete pFClass2d;
  }
  myFClass2dMap.Clear();
  //
  GeomAPI_ProjectPointOnSurf* pProjPS;
  aNb=myProjPSMap.Extent();
  for (i=1; i<=aNb; ++i) {
    anAdr=myProjPSMap(i);
    pProjPS=(GeomAPI_ProjectPointOnSurf*)anAdr;
    delete pProjPS;
  }
  myProjPSMap.Clear();
  //
  GeomAPI_ProjectPointOnCurve* pProjPC;
  aNb=myProjPCMap.Extent();
  for (i=1; i<=aNb; ++i) {
    anAdr=myProjPCMap(i);
    pProjPC=(GeomAPI_ProjectPointOnCurve*)anAdr;
    delete pProjPC;
  }
  myProjPCMap.Clear();
  //
  GeomAPI_ProjectPointOnCurve* pProjPT;
  aNb=myProjPTMap.Extent();
  for (i=1; i<=aNb; ++i) {
    anAdr=myProjPTMap(i);
    pProjPT=(GeomAPI_ProjectPointOnCurve*)anAdr;
    delete pProjPT;
  }
  myProjPTMap.Clear();
  //
  BRepClass3d_SolidClassifier* pSC;
  aNb=mySClassMap.Extent();
  for (i=1; i<=aNb; ++i) {
    anAdr=mySClassMap(i);
    pSC=(BRepClass3d_SolidClassifier*)anAdr;
    delete pSC;
  }
  mySClassMap.Clear();
  //
  IntTools_SurfaceRangeLocalizeData* pSData = NULL;
  aNb = myProjSDataMap.Extent();
  for (i=1; i<=aNb; ++i) {
    anAdr=myProjSDataMap(i);
    pSData = (IntTools_SurfaceRangeLocalizeData*)anAdr;
    if(pSData)
      delete pSData;
    pSData = NULL;
  }
  myProjSDataMap.Clear();
}
//=======================================================================
//function : FClass2d
//purpose  : 
//=======================================================================
  IntTools_FClass2d& IntTools_Context::FClass2d(const TopoDS_Face& aF)
{
  Standard_Address anAdr;
  IntTools_FClass2d* pFClass2d;
 
  if (!myFClass2dMap.Contains(aF)) {
    Standard_Real aTolF;
    TopoDS_Face aFF=aF;
    aFF.Orientation(TopAbs_FORWARD);
    aTolF=BRep_Tool::Tolerance(aFF);
    //
    pFClass2d=new IntTools_FClass2d(aFF, aTolF);
    //
    anAdr=(Standard_Address)pFClass2d;
    myFClass2dMap.Add(aFF, anAdr);
  }

  else {
    anAdr=myFClass2dMap.FindFromKey(aF);
    pFClass2d=(IntTools_FClass2d*)anAdr;
  }

  return *pFClass2d;
}
//=======================================================================
//function : ProjPS
//purpose  : 
//=======================================================================
  GeomAPI_ProjectPointOnSurf& IntTools_Context::ProjPS(const TopoDS_Face& aF)
{
  Standard_Address anAdr;
  GeomAPI_ProjectPointOnSurf* pProjPS;
 
  if (!myProjPSMap.Contains(aF)) {
    Standard_Real Umin, Usup, Vmin, Vsup, anEpsT=1.e-12 ;
    BRepAdaptor_Surface aBAS;
    //
    const Handle(Geom_Surface)& aS=BRep_Tool::Surface(aF);
    aBAS.Initialize (aF, Standard_True);
    //
    Umin=aBAS.FirstUParameter();
    Usup=aBAS.LastUParameter ();
    Vmin=aBAS.FirstVParameter();
    Vsup=aBAS.LastVParameter ();
    //
    pProjPS=new GeomAPI_ProjectPointOnSurf;
    pProjPS->Init(aS ,Umin, Usup, Vmin, Vsup, anEpsT);
    //
    anAdr=(Standard_Address)pProjPS;
    myProjPSMap.Add(aF, anAdr);
  }
  
  else {
    anAdr=myProjPSMap.FindFromKey(aF);
    pProjPS=(GeomAPI_ProjectPointOnSurf*)anAdr;
  }
  return *pProjPS;
}
//=======================================================================
//function : ProjPC
//purpose  : 
//=======================================================================
  GeomAPI_ProjectPointOnCurve& IntTools_Context::ProjPC(const TopoDS_Edge& aE)
{
  Standard_Address anAdr;
  GeomAPI_ProjectPointOnCurve* pProjPC;
 
  if (!myProjPCMap.Contains(aE)) {
    Standard_Real f, l;
    //
    Handle(Geom_Curve)aC3D=BRep_Tool::Curve (aE, f, l);
    //
    pProjPC=new GeomAPI_ProjectPointOnCurve;
    pProjPC->Init(aC3D, f, l);
    //
    anAdr=(Standard_Address)pProjPC;
    myProjPCMap.Add(aE, anAdr);
  }
  
  else {
    anAdr=myProjPCMap.FindFromKey(aE);
    pProjPC=(GeomAPI_ProjectPointOnCurve*)anAdr;
  }
  return *pProjPC;
}
//=======================================================================
//function : ProjPT
//purpose  : 
//=======================================================================
  GeomAPI_ProjectPointOnCurve& IntTools_Context::ProjPT(const Handle(Geom_Curve)& aC3D)
							
{
  Standard_Address anAdr;
  GeomAPI_ProjectPointOnCurve* pProjPT;
 
  if (!myProjPTMap.Contains(aC3D)) {
    Standard_Real f, l;
    f=aC3D->FirstParameter();
    l=aC3D->LastParameter();
    //
    pProjPT=new GeomAPI_ProjectPointOnCurve;
    pProjPT->Init(aC3D, f, l);
    //
    anAdr=(Standard_Address)pProjPT;
    myProjPTMap.Add(aC3D, anAdr);
  }
  
  else {
    anAdr=myProjPTMap.FindFromKey(aC3D);
    pProjPT=(GeomAPI_ProjectPointOnCurve*)anAdr;
  }
  return *pProjPT;
}
//=======================================================================
//function : SurfaceData
//purpose  : 
//=======================================================================
  IntTools_SurfaceRangeLocalizeData& IntTools_Context::SurfaceData(const TopoDS_Face& aF) 
{
  Standard_Address anAdr;
  IntTools_SurfaceRangeLocalizeData* pSData;
 
  if (!myProjSDataMap.Contains(aF)) {
    //
    pSData=new IntTools_SurfaceRangeLocalizeData(3, 
						 3, 
						 10. * Precision::PConfusion(), 
						 10. * Precision::PConfusion());
    //
    anAdr=(Standard_Address)pSData;
    myProjSDataMap.Add(aF, anAdr);
  }
  
  else {
    anAdr=myProjSDataMap.FindFromKey(aF);
    pSData=(IntTools_SurfaceRangeLocalizeData*)anAdr;
  }
  return *pSData;

}
//=======================================================================
//function : SolidClassifier
//purpose  : 
//=======================================================================
  BRepClass3d_SolidClassifier& IntTools_Context::SolidClassifier(const TopoDS_Solid& aSolid)
{
  Standard_Address anAdr;
  BRepClass3d_SolidClassifier* pSC;
 
  if (!mySClassMap.Contains(aSolid)) {
    //
    pSC=new BRepClass3d_SolidClassifier(aSolid);
    //
    anAdr=(Standard_Address)pSC;
    mySClassMap.Add(aSolid, anAdr);
  }
  
  else {
    anAdr=mySClassMap.FindFromKey(aSolid);
    pSC =(BRepClass3d_SolidClassifier*)anAdr;
  }
  return *pSC;
}


//modified by NIZNHY-PKV Tue Feb  2 08:33:16 2010f
//=======================================================================
//function : ComputeVE
//purpose  : 
//=======================================================================
  Standard_Integer IntTools_Context::ComputeVE(const TopoDS_Vertex& aV1, 
					       const TopoDS_Edge&   aE2,
					       Standard_Real& aT)
{
  Standard_Boolean bToUpdate;
  Standard_Integer iFlag;
  Standard_Real aDist;
  //
  iFlag= IntTools_Context::ComputeVE(aV1, aE2, aT, bToUpdate, aDist);
  //
  return iFlag;
}
//=======================================================================
//function : ComputeVE
//purpose  : 
//=======================================================================
  Standard_Integer IntTools_Context::ComputeVE(const TopoDS_Vertex& aV1, 
					       const TopoDS_Edge&   aE2,
					       Standard_Real& aT,
					       Standard_Boolean& bToUpdateVertex, 
					       Standard_Real& aDist)
{
  bToUpdateVertex=Standard_False;
  aDist=0.;
  //
  if (BRep_Tool::Degenerated(aE2)) {
    return -1;
  }
  if (!BRep_Tool::IsGeometric(aE2)) { 
    return -2;
  }
  //
  Standard_Real aTolV1, aTolE2, aTolSum, aTolVx;
  Standard_Integer aNbProj;
  gp_Pnt aP;
  //
  aP=BRep_Tool::Pnt(aV1);
  //
  GeomAPI_ProjectPointOnCurve& aProjector=ProjPC(aE2);
  aProjector.Perform(aP);
  aNbProj=aProjector.NbPoints();
  if (!aNbProj) {
    return -3;
  }
  //
  aDist=aProjector.LowerDistance();
  aTolV1=BRep_Tool::Tolerance(aV1);
  aTolE2=BRep_Tool::Tolerance(aE2);
  aTolSum=aTolV1+aTolE2;
  //
  aT=aProjector.LowerDistanceParameter();
  if (aDist > aTolSum) {
    return -4;
  }
  //
  aTolVx=aDist+aTolE2;
  if (aTolVx>aTolV1) {
    bToUpdateVertex=!bToUpdateVertex;
    aDist=aTolVx;
  }
  //
  return 0;
}
//modified by NIZNHY-PKV Tue Feb  2 08:33:21 2010t
//=======================================================================
//function : ComputeVS
//purpose  : 
//=======================================================================
  Standard_Integer IntTools_Context::ComputeVS(const TopoDS_Vertex& aV1, 
					       const TopoDS_Face&   aF2,
					       Standard_Real& U,
					       Standard_Real& V)
{
  Standard_Real aTolV1, aTolF2, aTolSum, aDist;
  gp_Pnt aP;

  aP=BRep_Tool::Pnt(aV1);
  //
  // 1. Check if the point is projectable on the surface
  GeomAPI_ProjectPointOnSurf& aProjector=ProjPS(aF2);
  aProjector.Perform(aP);
  //
  if (!aProjector.IsDone()) {
    // the point is not  projectable on the surface
    return -1;
  }
  //
  // 2. Check the distance between the projection point and 
  //    the original point
  aDist=aProjector.LowerDistance();

  aTolV1=BRep_Tool::Tolerance(aV1);
  aTolF2=BRep_Tool::Tolerance(aF2);
  aTolSum=aTolV1+aTolF2;
  if (aDist > aTolSum) {
    // the distance is too large
    return -2;
  }
  aProjector.LowerDistanceParameters(U, V);
  //
  gp_Pnt2d aP2d(U, V);
  Standard_Boolean pri=IsPointInFace (aF2, aP2d);
  if (!pri) {
    //  the point lays on the surface but out of the face 
    return -3;
  }
  return 0;
}
//=======================================================================
//function : StatePointFace
//purpose  : 
//=======================================================================
  TopAbs_State IntTools_Context::StatePointFace(const TopoDS_Face& aF,
						const gp_Pnt2d& aP2d)
{
  TopAbs_State aState;
  IntTools_FClass2d& aClass2d=FClass2d(aF);
  aState=aClass2d.Perform(aP2d);
  return aState;
}
//=======================================================================
//function : IsPointInFace
//purpose  : 
//=======================================================================
  Standard_Boolean IntTools_Context::IsPointInFace(const TopoDS_Face& aF,
						   const gp_Pnt2d& aP2d)
{
  TopAbs_State aState=StatePointFace(aF, aP2d);
  if (aState==TopAbs_OUT || aState==TopAbs_ON) {
    return Standard_False;
  }
  return Standard_True;
}
//=======================================================================
//function : IsPointInOnFace
//purpose  : 
//=======================================================================
  Standard_Boolean IntTools_Context::IsPointInOnFace(const TopoDS_Face& aF,
						     const gp_Pnt2d& aP2d)
{ 
  TopAbs_State aState=StatePointFace(aF, aP2d);
  if (aState==TopAbs_OUT) {
    return Standard_False;
  }
  return Standard_True;
}
//=======================================================================
//function : IsValidPointForFace
//purpose  : 
//=======================================================================
  Standard_Boolean IntTools_Context::IsValidPointForFace(const gp_Pnt& aP,
							 const TopoDS_Face& aF,
							 const Standard_Real aTol) 
{
  Standard_Boolean bFlag;
  Standard_Real Umin, myEpsT, U, V;
  myEpsT=1.e-12;

  GeomAPI_ProjectPointOnSurf& aProjector=ProjPS(aF);
  aProjector.Perform(aP);
  
  bFlag=aProjector.IsDone();
  if (bFlag) {
    
    Umin=aProjector.LowerDistance();
    //if (Umin > 1.e-3) { // it was 
    if (Umin > aTol) {
      return !bFlag; 
    }
    //
    aProjector.LowerDistanceParameters(U, V);
    gp_Pnt2d aP2D(U, V);
    bFlag=IsPointInOnFace (aF, aP2D);
  }
  return bFlag;
}
//=======================================================================
//function : IsValidPointForFaces
//purpose  : 
//=======================================================================
  Standard_Boolean IntTools_Context::IsValidPointForFaces (const gp_Pnt& aP,
							   const TopoDS_Face& aF1,
							   const TopoDS_Face& aF2,
							   const Standard_Real aTol) 
{
  Standard_Boolean bFlag1, bFlag2;

  bFlag1=IsValidPointForFace(aP, aF1, aTol);
  if (!bFlag1) {
    return bFlag1;
  }
  bFlag2=IsValidPointForFace(aP, aF2, aTol);  
  return  bFlag2;
}
//=======================================================================
//function : IsValidBlockForFace
//purpose  : 
//=======================================================================
  Standard_Boolean IntTools_Context::IsValidBlockForFace (const Standard_Real aT1,
							  const Standard_Real aT2,
							  const IntTools_Curve& aC, 
							  const TopoDS_Face& aF,
							  const Standard_Real aTol) 
{
  Standard_Boolean bFlag;
  Standard_Real aTInterm, aFirst, aLast;
  gp_Pnt aPInterm;

  aTInterm=IntTools_Tools::IntermediatePoint(aT1, aT2);

  Handle(Geom_Curve) aC3D=aC.Curve();
  aFirst=aC3D->FirstParameter();
  aLast =aC3D->LastParameter();
  // point 3D
  aC3D->D0(aTInterm, aPInterm);
  //
  bFlag=IsValidPointForFace (aPInterm, aF, aTol);
  return bFlag;
}
//=======================================================================
//function : IsValidBlockForFaces
//purpose  : 
//=======================================================================
  Standard_Boolean IntTools_Context::IsValidBlockForFaces (const Standard_Real aT1,
							   const Standard_Real aT2,
							   const IntTools_Curve& aC, 
							   const TopoDS_Face& aF1,
							   const TopoDS_Face& aF2,
							   const Standard_Real aTol) 
{
  Standard_Boolean bFlag1, bFlag2;
  //
  Handle(Geom2d_Curve) aPC1 = aC.FirstCurve2d();
  Handle(Geom2d_Curve) aPC2 = aC.SecondCurve2d();
  if( !aPC1.IsNull() && !aPC2.IsNull() ) {
    Standard_Real aMidPar = IntTools_Tools::IntermediatePoint(aT1, aT2);
    gp_Pnt2d aPnt2D;


    aPC1->D0(aMidPar, aPnt2D);
    bFlag1 = IsPointInOnFace(aF1, aPnt2D);

    if( !bFlag1 )
      return bFlag1;

    aPC2->D0(aMidPar, aPnt2D);
    bFlag2 = IsPointInOnFace(aF2, aPnt2D);
    return bFlag2;
  }
  //

  bFlag1=IsValidBlockForFace (aT1, aT2, aC, aF1, aTol);
  if (!bFlag1) {
    return bFlag1;
  }
  bFlag2=IsValidBlockForFace (aT1, aT2, aC, aF2, aTol);
  return bFlag2;
}
//=======================================================================
//function : IsVertexOnLine
//purpose  : 
//=======================================================================
  Standard_Boolean IntTools_Context::IsVertexOnLine (const TopoDS_Vertex& aV,
						     const IntTools_Curve& aC, 
						     const Standard_Real aTolC,
						     Standard_Real& aT)
{
  Standard_Boolean bRet;
  Standard_Real aTolV;
  //
  aTolV=BRep_Tool::Tolerance(aV);
  bRet=IntTools_Context::IsVertexOnLine(aV, aTolV, aC, aTolC , aT);
  //
  return bRet;
}
//=======================================================================
//function : IsVertexOnLine
//purpose  : 
//=======================================================================
  Standard_Boolean IntTools_Context::IsVertexOnLine (const TopoDS_Vertex& aV,
						     const Standard_Real aTolV,
						     const IntTools_Curve& aC, 
						     const Standard_Real aTolC,
						     Standard_Real& aT)
{
  Standard_Real aFirst, aLast, aDist, aTolSum;
  Standard_Integer aNbProj;
  gp_Pnt aPv; 
  
  aPv=BRep_Tool::Pnt(aV);

  Handle(Geom_Curve) aC3D=aC.Curve();
  
  
  aTolSum=aTolV+aTolC;
  //
  GeomAdaptor_Curve aGAC(aC3D);
  GeomAbs_CurveType aType=aGAC.GetType();
  if (aType==GeomAbs_BSplineCurve ||
      aType==GeomAbs_BezierCurve) {
    aTolSum=2.*aTolSum;
    if (aTolSum<1.e-5) {
      aTolSum=1.e-5;
    }
  }
  else {
    aTolSum=2.*aTolSum;//xft
    if(aTolSum < 1.e-6)
      aTolSum = 1.e-6;
  }
  //
  aFirst=aC3D->FirstParameter();
  aLast =aC3D->LastParameter();
  //
  //Checking extermities first
  if (!Precision::IsInfinite(aFirst)) {
    gp_Pnt aPCFirst=aC3D->Value(aFirst);
    aDist=aPv.Distance(aPCFirst);
    if (aDist < aTolSum) {
      aT=aFirst;
      //
      if(aDist > aTolV) {
	Extrema_LocateExtPC anExt(aPv, aGAC, aFirst, 1.e-10);

	if(anExt.IsDone()) {
	  Extrema_POnCurv aPOncurve = anExt.Point();
	  aT = aPOncurve.Parameter();

	  if((aT > (aLast + aFirst) * 0.5) ||
	     (aPv.Distance(aPOncurve.Value()) > aTolSum) ||
	     (aPCFirst.Distance(aPOncurve.Value()) < Precision::Confusion()))
	    aT = aFirst;
	}
      }
      //
      return Standard_True;
    }
  }
  //
  if (!Precision::IsInfinite(aLast)) {
    gp_Pnt aPCLast=aC3D->Value(aLast);
    aDist=aPv.Distance(aPCLast);
    if (aDist < aTolSum) {
      aT=aLast;
      //
      if(aDist > aTolV) {
	Extrema_LocateExtPC anExt(aPv, aGAC, aLast, 1.e-10);

	if(anExt.IsDone()) {
	  Extrema_POnCurv aPOncurve = anExt.Point();
	  aT = aPOncurve.Parameter();

	  if((aT < (aLast + aFirst) * 0.5) ||
	     (aPv.Distance(aPOncurve.Value()) > aTolSum) ||
	     (aPCLast.Distance(aPOncurve.Value()) < Precision::Confusion()))
	    aT = aLast;
	}
      }
      //
      return Standard_True;
    }
  }
  //
  GeomAPI_ProjectPointOnCurve& aProjector=ProjPT(aC3D);
  aProjector.Perform(aPv);
  
  aNbProj=aProjector.NbPoints();
  if (!aNbProj) {
    Handle(Geom_BoundedCurve) aBC=
      Handle(Geom_BoundedCurve)::DownCast(aC3D);
    if (!aBC.IsNull()) {
      gp_Pnt aPStart=aBC->StartPoint();
      gp_Pnt aPEnd  =aBC->EndPoint();
      
      aDist=aPv.Distance(aPStart);
      if (aDist < aTolSum) {
	aT=aFirst;
	return Standard_True;
      }
      
      aDist=aPv.Distance(aPEnd);
      if (aDist < aTolSum) {
	aT=aLast;
	return Standard_True;
      }
    }
    
    return Standard_False;
  }
  
  aDist=aProjector.LowerDistance();
  
  if (aDist > aTolSum) {
    return Standard_False;
  }

  aT=aProjector.LowerDistanceParameter();

  return Standard_True; 
}
//=======================================================================
//function : ProjectPointOnEdge
//purpose  : 
//=======================================================================
  Standard_Boolean IntTools_Context::ProjectPointOnEdge(const gp_Pnt& aP,
							const TopoDS_Edge& anEdge,
							Standard_Real& aT)
{
  Standard_Integer aNbPoints;

  GeomAPI_ProjectPointOnCurve& aProjector=ProjPC(anEdge);
  aProjector.Perform(aP);

  aNbPoints=aProjector.NbPoints();
  if (aNbPoints) {
    aT=aProjector.LowerDistanceParameter();
    return Standard_True;
  }
  return Standard_False;
}

