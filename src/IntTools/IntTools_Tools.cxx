// File:	IntTools_Tools.cxx
// Created:	Thu Nov 16 11:42:23 2000
// Author:	Peter KURNEV
//		<pkv@irinox>
//

#include <IntTools_Tools.ixx>

#include <Precision.hxx>

#include <TopExp_Explorer.hxx>
#include <TopTools_IndexedDataMapOfShapeShape.hxx>

#include <TopoDS.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Wire.hxx>
#include <TopLoc_Location.hxx>

#include <BRep_Builder.hxx>
#include <BRep_Tool.hxx>
#include <BRepAdaptor_Surface.hxx>
#include <BRepAdaptor_Curve.hxx>
#include <BRepAdaptor_Surface.hxx>

#include <gp_Pnt.hxx>
#include <gp_Pnt2d.hxx>
#include <gp.hxx>
#include <gp_Lin.hxx>
#include <gp_Dir.hxx>
#include <gp_Ax1.hxx>

#include <Geom_Curve.hxx>
#include <GeomAdaptor_Surface.hxx>
#include <Geom_Surface.hxx>
#include <GeomAPI_ProjectPointOnSurf.hxx>
#include <GeomAPI_ProjectPointOnCurve.hxx>
#include <GeomAdaptor_Curve.hxx>
#include <GeomAbs_CurveType.hxx>
#include <Geom_Line.hxx>
#include <Geom2d_Curve.hxx>
#include <Geom_BoundedCurve.hxx>
#include <Geom_Geometry.hxx>
#include <Geom_TrimmedCurve.hxx>
#include <Geom2d_TrimmedCurve.hxx>

#include <IntTools_FClass2d.hxx>
#include <IntTools_Curve.hxx>
#include <IntTools_SequenceOfCurves.hxx>

static
  void ParabolaTolerance(const Handle(Geom_Curve)& ,
			 const Standard_Real ,
			 const Standard_Real ,
			 const Standard_Real ,
			 Standard_Real& ,
			 Standard_Real& );

//=======================================================================
//function : HasInternalEdge
//purpose  : 
//=======================================================================
  Standard_Boolean IntTools_Tools::HasInternalEdge(const TopoDS_Wire& aW)
{
  Standard_Boolean bFlag=Standard_True;

  TopExp_Explorer anExp(aW, TopAbs_EDGE);
  for (; anExp.More(); anExp.Next()) {
    const TopoDS_Edge& aE=TopoDS::Edge(anExp.Current());
    TopAbs_Orientation anOr=aE.Orientation();
    if (anOr==TopAbs_INTERNAL) {
      return bFlag;
    }
  }
  return !bFlag;
}

//=======================================================================
//function : IsClosed
//purpose  : 
//=======================================================================
  Standard_Boolean IntTools_Tools::IsClosed (const Handle(Geom_Curve)& aC3D)
{
  
  Standard_Real aF, aL, aDist;
  gp_Pnt aP1, aP2;
   
  Handle (Geom_BoundedCurve) aGBC=
      Handle (Geom_BoundedCurve)::DownCast(aC3D);
  if (aGBC.IsNull()) {
    return Standard_False;
  }

  aF=aC3D->FirstParameter();
  aL=aC3D-> LastParameter();
  
  aC3D->D0(aF, aP1);
  aC3D->D0(aL, aP2);

  aDist=aP1.Distance(aP2);
  //return (aDist < 1.e-12); 
  return (aDist < Precision::Confusion()); 
}

//=======================================================================
//function : RejectLines
//purpose  : 
//=======================================================================
   void IntTools_Tools::RejectLines(const IntTools_SequenceOfCurves& aSIn,
				    IntTools_SequenceOfCurves& aSOut)
{
  Standard_Integer i, j, aNb;
  Standard_Boolean bFlag;
  Handle (Geom_Curve) aC3D;

  gp_Dir aD1, aD2;

  aSOut.Clear();

  aNb=aSIn.Length();
  for (i=1; i<=aNb; i++) {
    const IntTools_Curve& IC=aSIn(i);
    aC3D=IC.Curve();
    //
    Handle (Geom_TrimmedCurve) aGTC=
      Handle (Geom_TrimmedCurve)::DownCast(aC3D);
    
    if (!aGTC.IsNull()) {
      aC3D=aGTC->BasisCurve();
      IntTools_Curve* pIC=(IntTools_Curve*) &IC;
      pIC->SetCurve(aC3D);
    }
    //
    Handle (Geom_Line) aGLine=
      Handle (Geom_Line)::DownCast(aC3D);
    
    if (aGLine.IsNull()) {
      aSOut.Clear();
      for (j=1; j<=aNb; j++) {
	aSOut.Append(aSIn(j));
      }
      return;
    }
    //
    gp_Lin aLin=aGLine->Lin();
    aD2=aLin.Direction();
    if (i==1) {
      aSOut.Append(IC);
      aD1=aD2;
      continue;
    }
    
    bFlag=IntTools_Tools::IsDirsCoinside(aD1, aD2);
    if (!bFlag) {
      aSOut.Append(IC);
      return;
    }
  }
}

//=======================================================================
//function : IsDirsCoinside
//purpose  : 
//=======================================================================
  Standard_Boolean IntTools_Tools::IsDirsCoinside  (const gp_Dir& D1, const gp_Dir& D2)
{
  Standard_Boolean bFlag;
  gp_Pnt P1(D1.X(), D1.Y(), D1.Z());  
  gp_Pnt P2(D2.X(), D2.Y(), D2.Z());
  Standard_Real dLim=0.0002, d;
  d=P1.Distance (P2); 
  bFlag= (d<dLim || fabs (2.-d)<dLim); 
  return bFlag;
}
  
//=======================================================================
//function : IsDirsCoinside
//purpose  : 
//=======================================================================
  Standard_Boolean IntTools_Tools::IsDirsCoinside  (const gp_Dir& D1, 
						    const gp_Dir& D2,
						    const Standard_Real dLim)
{
  Standard_Boolean bFlag;
  Standard_Real d;
  //
  gp_Pnt P1(D1.X(), D1.Y(), D1.Z());  
  gp_Pnt P2(D2.X(), D2.Y(), D2.Z());
  d=P1.Distance (P2); 
  bFlag= (d<dLim || fabs (2.-d)<dLim); 
  return bFlag;
}
//=======================================================================
//function : SplitCurve
//purpose  : 
//=======================================================================
  Standard_Integer IntTools_Tools::SplitCurve(const IntTools_Curve& IC,
					      IntTools_SequenceOfCurves& aCvs)
{
  Handle (Geom_Curve) aC3D =IC.Curve();
  if(aC3D.IsNull())
    return 0;
  //
  Handle (Geom2d_Curve) aC2D1=IC.FirstCurve2d();
  Handle (Geom2d_Curve) aC2D2=IC.SecondCurve2d();
  Standard_Boolean bIsClosed;

  bIsClosed=IntTools_Tools::IsClosed(aC3D);
  if (!bIsClosed) {
    return 0;
  }

  Standard_Real aF, aL, aMid;
  
  //
  aF=aC3D->FirstParameter();
  aL=aC3D->LastParameter();
  aMid=0.5*(aF+aL);
  //modified by NIZNHY-PKV Thu Feb  5 08:26:58 2009 f
  GeomAdaptor_Curve aGAC(aC3D);
  GeomAbs_CurveType aCT=aGAC.GetType();
  if (aCT==GeomAbs_BSplineCurve ||
      aCT==GeomAbs_BezierCurve) {
    //aMid=0.5*aMid;
    aMid=IntTools_Tools::IntermediatePoint(aF, aL);
  }
  //modified by NIZNHY-PKV Thu Feb  5 08:27:00 2009 t
  //
  Handle(Geom_Curve) aC3DNewF, aC3DNewL;
  aC3DNewF =new Geom_TrimmedCurve  (aC3D, aF, aMid);
  aC3DNewL =new Geom_TrimmedCurve  (aC3D, aMid, aL);

  //
  Handle (Geom2d_Curve) aC2D1F, aC2D1L, aC2D2F, aC2D2L;
  //
  if(!aC2D1.IsNull()) {
    aC2D1F=new Geom2d_TrimmedCurve (aC2D1, aF, aMid);
    aC2D1L=new Geom2d_TrimmedCurve (aC2D1, aMid, aL);
  }

  if(!aC2D2.IsNull()) {
    aC2D2F=new Geom2d_TrimmedCurve (aC2D2, aF, aMid);
    aC2D2L=new Geom2d_TrimmedCurve (aC2D2, aMid, aL);
  }
  // 
  
  IntTools_Curve aIC1(aC3DNewF, aC2D1F, aC2D2F);
  IntTools_Curve aIC2(aC3DNewL, aC2D1L, aC2D2L);
  //
  aCvs.Append(aIC1);
  //
  aCvs.Append(aIC2);
  //
  return 2;
}

//=======================================================================
//function : IntermediatePoint
//purpose  : 
//=======================================================================
  Standard_Real IntTools_Tools::IntermediatePoint (const Standard_Real aFirst,
						   const Standard_Real aLast)
{
  //define parameter division number as 10*e^(-PI) = 0.43213918
  const Standard_Real PAR_T = 0.43213918;
  Standard_Real aParm;
  aParm=(1.-PAR_T)*aFirst + PAR_T*aLast;
  return aParm;
}

//=======================================================================
//function : IsVertex
//purpose  : 
//=======================================================================
  Standard_Boolean IntTools_Tools::IsVertex (const gp_Pnt& aP,
					     const Standard_Real aTolPV,
					     const TopoDS_Vertex& aV)
{
  Standard_Real aTolV, aD, dTol;
  gp_Pnt aPv; 
  
  aTolV=BRep_Tool::Tolerance(aV);
  //
  //modified by NIZNHY-PKV Thu Jan 18 17:44:46 2007f
  //aTolV=aTolV+aTolPV;
  dTol=Precision::Confusion();
  aTolV=aTolV+aTolPV+dTol;
  //modified by NIZNHY-PKV Thu Jan 18 17:44:49 2007t
  //
  aPv=BRep_Tool::Pnt(aV);
  aD=aPv.Distance(aP);
  return (aD<=aTolV);
}


//=======================================================================
//function : IsVertex
//purpose  : 
//=======================================================================
  Standard_Boolean IntTools_Tools::IsVertex (const IntTools_CommonPrt& aCmnPrt)
{
  Standard_Boolean anIsVertex;
  Standard_Real aParam;

  const TopoDS_Edge&    aE1=aCmnPrt.Edge1();
  const IntTools_Range& aR1=aCmnPrt.Range1();
  aParam=0.5*(aR1.First()+aR1.Last());
  anIsVertex=IntTools_Tools::IsVertex (aE1, aParam);
  
  if (anIsVertex) {
    return Standard_True;
  }

  const TopoDS_Edge&    aE2=aCmnPrt.Edge2();
  const IntTools_SequenceOfRanges& aRs2=aCmnPrt.Ranges2();
  const IntTools_Range& aR2=aRs2(1);
  aParam=0.5*(aR2.First()+aR2.Last());
  anIsVertex=IntTools_Tools::IsVertex (aE2, aParam);
  if (anIsVertex) {
    return Standard_True;
  }
  return Standard_False;
}

//=======================================================================
//function : IsVertex
//purpose  : 
//=======================================================================
  Standard_Boolean IntTools_Tools::IsVertex (const TopoDS_Edge& aE,
					     const TopoDS_Vertex& aV,
					     const Standard_Real t)
{
  Standard_Real aTolV, aTolV2, d2;
  gp_Pnt aPv, aPt; 
  
  BRepAdaptor_Curve aBAC(aE);
  aBAC.D0(t, aPt);

  aTolV=BRep_Tool::Tolerance(aV);
  aTolV2=aTolV*aTolV;
  aPv=BRep_Tool::Pnt(aV);
  d2=aPv.SquareDistance (aPt);
  if (d2 < aTolV2) {
    return Standard_True;
  }
  return Standard_False;
}
//=======================================================================
//function : IsVertex
//purpose  : 
//=======================================================================
  Standard_Boolean IntTools_Tools::IsVertex (const TopoDS_Edge& aE,
					     const Standard_Real t)
{
  Standard_Real aTolV, aTolV2, d2;
  TopoDS_Vertex aV;
  gp_Pnt aPv, aPt; 
  
  BRepAdaptor_Curve aBAC(aE);
  aBAC.D0(t, aPt);

  TopExp_Explorer anExp(aE, TopAbs_VERTEX);
  for (; anExp.More(); anExp.Next()) {
    aV=TopoDS::Vertex (anExp.Current());
    aTolV=BRep_Tool::Tolerance(aV);
    aTolV2=aTolV*aTolV;
    aTolV2=1.e-12;
     aPv=BRep_Tool::Pnt(aV);
     d2=aPv.SquareDistance (aPt);
     if (d2 < aTolV2) {
       return Standard_True;
     }
  }
  return Standard_False;
}


//=======================================================================
//function : ComputeVV
//purpose  : 
//=======================================================================
  Standard_Integer IntTools_Tools::ComputeVV(const TopoDS_Vertex& aV1, 
					     const TopoDS_Vertex& aV2)
{
  Standard_Real aTolV1, aTolV2, aTolSum, d;
  gp_Pnt aP1, aP2;

  aTolV1=BRep_Tool::Tolerance(aV1);
  aTolV2=BRep_Tool::Tolerance(aV2);
  aTolSum=aTolV1+aTolV2;

  aP1=BRep_Tool::Pnt(aV1);
  aP2=BRep_Tool::Pnt(aV2);
  
  d=aP1.Distance(aP2);
  if (d<aTolSum) {
    return 0;
  }
  return -1;
}

//=======================================================================
//function : MakeFaceFromWireAndFace
//purpose  : 
//=======================================================================
  void IntTools_Tools::MakeFaceFromWireAndFace(const TopoDS_Wire& aW,
					       const TopoDS_Face& aF,
					       TopoDS_Face& aFNew)
{
  TopoDS_Face aFF;
  aFF=aF;
  aFF.Orientation(TopAbs_FORWARD);
  aFNew=TopoDS::Face (aFF.EmptyCopied());
  BRep_Builder BB;
  BB.Add(aFNew, aW);
}

//=======================================================================
//function : ClassifyPointByFace
//purpose  : 
//=======================================================================
  TopAbs_State IntTools_Tools::ClassifyPointByFace(const TopoDS_Face& aF,
						   const gp_Pnt2d& aP2d)
{
  Standard_Real aFaceTolerance;
  TopAbs_State aState;
  
  aFaceTolerance=BRep_Tool::Tolerance(aF);
  IntTools_FClass2d aClass2d(aF, aFaceTolerance);
  aState=aClass2d.Perform(aP2d);
  
  return aState;
}

//=======================================================================
//function : IsMiddlePointsEqual
//purpose  : 
//=======================================================================
  Standard_Boolean IntTools_Tools::IsMiddlePointsEqual(const TopoDS_Edge& aE1,
						       const TopoDS_Edge& aE2)
						       
{
  Standard_Real f1, l1, m1, f2, l2, m2, aTol1, aTol2, aSumTol;
  gp_Pnt aP1, aP2;

  aTol1=BRep_Tool::Tolerance(aE1);
  Handle(Geom_Curve) C1=BRep_Tool::Curve(aE1, f1, l1);
  m1=0.5*(f1+l1);
  C1->D0(m1, aP1);

  aTol2=BRep_Tool::Tolerance(aE2);
  Handle(Geom_Curve) C2=BRep_Tool::Curve(aE2, f2, l2);
  m2=0.5*(f2+l2);
  C2->D0(m2, aP2);

  aSumTol=aTol1+aTol2;
  
  if (aP1.Distance(aP2) < aSumTol) {
    return Standard_True;
  }
  return Standard_False;
}

//=======================================================================
//function : CurveTolerance
//purpose  : 
//=======================================================================
  Standard_Real IntTools_Tools::CurveTolerance(const Handle(Geom_Curve)& aC3D,
					       const Standard_Real aTolBase)
{
  Standard_Real aTolReached, aTf, aTl, aTolMin, aTolMax;

  aTolReached=aTolBase;
  //
  if (aC3D.IsNull()) {
    return aTolReached;
  }
  //
  Handle(Geom_TrimmedCurve) aCT3D=Handle(Geom_TrimmedCurve)::DownCast(aC3D);
  if (aCT3D.IsNull()) {
    return aTolReached;
  }
  //
  aTolMin=aTolBase;
  aTolMax=aTolBase;
  //
  aTf=aCT3D->FirstParameter();
  aTl=aCT3D->LastParameter();
  //
  GeomAdaptor_Curve aGAC(aCT3D);
  GeomAbs_CurveType aCType=aGAC.GetType();
  //
  if (aCType==GeomAbs_Parabola) {
    Handle(Geom_Curve) aC3DBase=aCT3D->BasisCurve();
    ParabolaTolerance(aC3DBase, aTf, aTl, aTolBase, aTolMin, aTolMax);
    aTolReached=aTolMax;
  }
  //
  return aTolReached;
}

#include <Geom_Parabola.hxx>
#include <gp_Parab.hxx>
//=======================================================================
//function : ParabolaTolerance
//purpose  : 
//=======================================================================
void ParabolaTolerance(const Handle(Geom_Curve)& aC3D,
		       const Standard_Real aTf,
		       const Standard_Real aTl,
		       const Standard_Real aTol,
		       Standard_Real& aTolMin,
		       Standard_Real& aTolMax)
{
  
  aTolMin=aTol;
  aTolMax=aTol;

  Handle(Geom_Parabola) aGP=Handle(Geom_Parabola)::DownCast(aC3D);
  if (aGP.IsNull()){
    return;
  }

  Standard_Integer aNbPoints;
  Standard_Real aFocal, aX1, aX2, aTol1, aTol2;
  gp_Pnt aPf, aPl;
  gp_Parab aParab=aGP->Parab();
  gp_Ax1 aXAxis=aParab.XAxis();
  Handle(Geom_Line) aGAxis=new Geom_Line(aXAxis);

  aFocal=aGP->Focal();
  if (aFocal==0.) {
    return;
  }
  //
  // aTol1
  aTol1=aTol;
  aX1=0.;
  aGP->D0(aTf, aPf);
  GeomAPI_ProjectPointOnCurve aProj1(aPf, aGAxis);
  aNbPoints=aProj1.NbPoints();
  if (aNbPoints) {
    aX1=aProj1.LowerDistanceParameter();
  }
  if (aX1>=0.) {
    aTol1=aTol*sqrt(0.5*aX1/aFocal);
  }
  if (aTol1==0.) {
    aTol1=aTol;
  }
  //
  // aTol2
  aTol2=aTol;
  aX2=0.;
  aGP->D0(aTl, aPl);
  GeomAPI_ProjectPointOnCurve aProj2(aPl, aGAxis);
  aNbPoints=aProj2.NbPoints();
  if (aNbPoints) {
    aX2=aProj2.LowerDistanceParameter();
  }
  
  if (aX2>=0.) {
    aTol2=aTol*sqrt(0.5*aX2/aFocal);
  }
  if (aTol2==0.) {
    aTol2=aTol;
  }
  //
  aTolMax=(aTol1>aTol2) ? aTol1 : aTol2;
  aTolMin=(aTol1<aTol2) ? aTol1 : aTol2;
}

