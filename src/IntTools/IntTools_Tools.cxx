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
#include <BRepAdaptor_Surface.hxx>
#include <Geom2d_Curve.hxx>
#include <Geom2d_TrimmedCurve.hxx>
#include <Geom_BoundedCurve.hxx>
#include <Geom_Curve.hxx>
#include <Geom_Geometry.hxx>
#include <Geom_Line.hxx>
#include <Geom_Surface.hxx>
#include <Geom_TrimmedCurve.hxx>
#include <GeomAbs_CurveType.hxx>
#include <GeomAdaptor_Curve.hxx>
#include <GeomAdaptor_Surface.hxx>
#include <GeomAPI_ProjectPointOnCurve.hxx>
#include <GeomAPI_ProjectPointOnSurf.hxx>
#include <gp.hxx>
#include <gp_Ax1.hxx>
#include <gp_Dir.hxx>
#include <gp_Lin.hxx>
#include <gp_Pln.hxx>
#include <gp_Pnt.hxx>
#include <gp_Pnt2d.hxx>
#include <IntTools_CommonPrt.hxx>
#include <IntTools_Curve.hxx>
#include <IntTools_FClass2d.hxx>
#include <IntTools_Range.hxx>
#include <IntTools_SequenceOfCurves.hxx>
#include <IntTools_Tools.hxx>
#include <Precision.hxx>
#include <TopExp_Explorer.hxx>
#include <TopLoc_Location.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopoDS_Wire.hxx>
#include <TopTools_IndexedDataMapOfShapeShape.hxx>

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
  Standard_Boolean bRet;
  Standard_Real aF, aL, aDist, aPC;
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

  
  //
  aPC=Precision::Confusion();
  aPC=aPC*aPC;
  aDist=aP1.SquareDistance(aP2);
  bRet=aDist<aPC;
  return bRet;
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
  GeomAdaptor_Curve aGAC(aC3D);
  GeomAbs_CurveType aCT=aGAC.GetType();
  if (aCT==GeomAbs_BSplineCurve ||
      aCT==GeomAbs_BezierCurve) {
    //aMid=0.5*aMid;
    aMid=IntTools_Tools::IntermediatePoint(aF, aL);
  }
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
  IntTools_Curve aIC1(aC3DNewF, aC2D1F, aC2D2F, IC.Tolerance(), IC.TangentialTolerance());
  IntTools_Curve aIC2(aC3DNewL, aC2D1L, aC2D2L, IC.Tolerance(), IC.TangentialTolerance());
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
  //define parameter division number as 10*e^(-M_PI) = 0.43213918
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
  Standard_Boolean bRet;
  Standard_Real aTolV, aD, dTol;
  gp_Pnt aPv; 
  
  aTolV=BRep_Tool::Tolerance(aV);
  //
  dTol=Precision::Confusion();
  aTolV=aTolV+aTolPV+dTol;
  //
  aPv=BRep_Tool::Pnt(aV);
  //
  aD=aPv.SquareDistance(aP);
  aTolV=aTolV*aTolV;
  bRet=(aD<=aTolV);
  return bRet;
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
  aTolSum=aTolSum*aTolSum;
  d=aP1.SquareDistance(aP2);
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
  Standard_Boolean bRet;
  Standard_Real f1, l1, m1, f2, l2, m2, aTol1, aTol2, aSumTol, aD2;
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
  aSumTol=aSumTol*aSumTol;
  aD2=aP1.SquareDistance(aP2);
  bRet=aD2<aSumTol;
  return bRet;
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
#include <BndLib_Add3dCurve.hxx>
#include <BRepLib_CheckCurveOnSurface.hxx>
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
/////////////////////////////////////////////////////////////////////////
//=======================================================================
//function : CheckCurve
//purpose  : 
//=======================================================================
Standard_Boolean IntTools_Tools::CheckCurve(const IntTools_Curve& theCurve,
                                            Bnd_Box& theBox)
{
  const Handle(Geom_Curve)& aC3D = theCurve.Curve();
  Standard_Boolean bValid = !aC3D.IsNull();
  if (!bValid) {
    return bValid;
  }
  //
  // Build bounding box for the curve
  BndLib_Add3dCurve::Add(GeomAdaptor_Curve(aC3D),
                         Max(theCurve.Tolerance(), theCurve.TangentialTolerance()),
                         theBox);
  //
  // Estimate the bounding box of the curve comparing it with the
  // minimal length for the curve from which the valid edge can be built -
  // 3*Precision::Confusion():
  // - 2 vertices with the Precision::Confusion() tolerance;
  // - plus Precision::Confusion() as the minimal distance between vertices.
  Standard_Real aTolCmp = 3*Precision::Confusion();
  //
  // Check the size of the box using the Bnd_Box::IsThin() method
  // which does not use the gap of the box.
  bValid = !theBox.IsThin(aTolCmp);
  //
  return bValid;
}
//=======================================================================
//function : IsOnPave
//purpose  : 
//=======================================================================
Standard_Boolean IntTools_Tools::IsOnPave(const Standard_Real aT1,
                                          const IntTools_Range& aRange,
                                          const Standard_Real aTolerance)
{
  Standard_Boolean firstisonpave1, firstisonpave2, bIsOnPave;
  //
  firstisonpave1  = (Abs(aRange.First() - aT1) < aTolerance);
  firstisonpave2  = (Abs(aRange.Last()  - aT1) < aTolerance);
  bIsOnPave=(firstisonpave1 || firstisonpave2);
  return bIsOnPave;
}
//=======================================================================
// function: VertexParameters
// purpose: 
//=======================================================================
void IntTools_Tools::VertexParameters(const IntTools_CommonPrt& aCPart,
                                      Standard_Real& aT1, 
                                      Standard_Real& aT2)
{
  const IntTools_Range& aR1=aCPart.Range1();
  aT1=0.5*(aR1.First()+aR1.Last());
  //
  if((aCPart.VertexParameter1() >= aR1.First()) &&
     (aCPart.VertexParameter1() <= aR1.Last())) {
    aT1 = aCPart.VertexParameter1();
  }
  //
  const IntTools_SequenceOfRanges& aRanges2=aCPart.Ranges2();
  const IntTools_Range& aR2=aRanges2(1);
  aT2=0.5*(aR2.First()+aR2.Last());
  //
  if((aCPart.VertexParameter2() >= aR2.First()) &&
     (aCPart.VertexParameter2() <= aR2.Last())) {
    aT2 = aCPart.VertexParameter2();
  }
}
//=======================================================================
// function: VertexParameter
// purpose: 
//=======================================================================
void IntTools_Tools::VertexParameter(const IntTools_CommonPrt& aCPart,
                                     Standard_Real& aT)
{
  const IntTools_Range& aR=aCPart.Range1();
  aT=0.5*(aR.First()+aR.Last());
  if((aCPart.VertexParameter1() >= aR.First()) &&
     (aCPart.VertexParameter1() <= aR.Last())) {
    aT = aCPart.VertexParameter1();
  }
}
//=======================================================================
// function: IsOnPave1
// purpose: 
//=======================================================================
Standard_Boolean IntTools_Tools::IsOnPave1(const Standard_Real aTR,
                                           const IntTools_Range& aCPRange,
                                           const Standard_Real aTolerance)
{
  Standard_Boolean bIsOnPave;
  Standard_Real aT1, aT2, dT1, dT2;
  //
  aT1=aCPRange.First();
  aT2=aCPRange.Last();
  bIsOnPave=(aTR>=aT1 && aTR<=aT2);
  if (bIsOnPave) {
    return bIsOnPave;
  }
  //
  dT1=Abs(aTR-aT1);  
  dT2=Abs(aTR-aT2);
  bIsOnPave=(dT1<=aTolerance || dT2<=aTolerance);
  return bIsOnPave;
}
//=======================================================================
// function: IsInRange
// purpose: 
//=======================================================================
Standard_Boolean IntTools_Tools::IsInRange(const IntTools_Range& aRRef,
                                           const IntTools_Range& aR,
                                           const Standard_Real aTolerance)
{
  Standard_Boolean bIsIn;
  Standard_Real aT1, aT2, aTRef1, aTRef2;
  //
  aR.Range(aT1, aT2);
  aRRef.Range(aTRef1, aTRef2);
  //
  aTRef1-=aTolerance;
  aTRef2+=aTolerance;
  //
  bIsIn = (aT1>=aTRef1 && aT1<=aTRef2) ||
          (aT2>=aTRef1 && aT2<=aTRef2);
  //
  return bIsIn;
}
//=======================================================================
//function : SegPln
//purpose  : 
//=======================================================================
Standard_Integer IntTools_Tools::SegPln(const gp_Lin& theLin,
                                        const Standard_Real theTLin1,
                                        const Standard_Real theTLin2,
                                        const Standard_Real theTolLin,
                                        const gp_Pln& thePln,
                                        const Standard_Real theTolPln,
                                        gp_Pnt& theP,
                                        Standard_Real& theTP,
                                        Standard_Real& theTolP,
                                        Standard_Real& theTPmin,
                                        Standard_Real& theTPmax)
{
  Standard_Integer iRet;
  Standard_Real aTol, aA, aB, aC, aD, aE, aH, aTP, aDist1, aDist2;
  gp_Pnt aP1, aP2;
  //
  iRet=0;
  aTol=theTolLin+theTolPln;
  //
  const gp_Ax3& aPosPln=thePln.Position();
  const gp_Dir& aDirPln=aPosPln.Direction();
  const gp_Pnt& aLocPln=aPosPln.Location();
  //
  const gp_Dir& aDirLin=theLin.Direction();
  const gp_Pnt& aLocLin=theLin.Location();
  //
  aP1.SetXYZ(aLocLin.XYZ()+theTLin1*aDirLin.XYZ());
  aDist1=aDirPln.X()*(aP1.X()-aLocPln.X())+
         aDirPln.Y()*(aP1.Y()-aLocPln.Y())+
         aDirPln.Z()*(aP1.Z()-aLocPln.Z());
  //
  aP2.SetXYZ(aLocLin.XYZ()+theTLin2*aDirLin.XYZ());
  aDist2=aDirPln.X()*(aP2.X()-aLocPln.X())+
         aDirPln.Y()*(aP2.Y()-aLocPln.Y())+
         aDirPln.Z()*(aP2.Z()-aLocPln.Z());
  //
  if (aDist1<aTol && aDist2<aTol){
    iRet=1; // common block
    return iRet;
  }
  //
  if (aDist1*aDist2 > 0.) {
    iRet=2; // segment lays on one side to the Plane
    return iRet;
  } 
  //
  thePln.Coefficients(aA, aB, aC, aD);
  aE=aA*aLocLin.X()+aB*aLocLin.Y()+aC*aLocLin.Z()+aD;
  aH=aA*aDirLin.X()+aB*aDirLin.Y()+aC*aDirLin.Z();
  aTP=-aE/aH;
  if (aTP < theTLin1-aTol || aTP > theTLin2+aTol) {
    iRet=3; // no intersections due to range of the Line
    return iRet;
  }
  //
  theTP=aTP;
  theP.SetXYZ(aLocLin.XYZ()+aTP*aDirLin.XYZ());
  theTolP=aTol;
  theTPmin=theTP-theTolPln;
  theTPmax=theTP+theTolPln;
  iRet=0; // intersection point
  return iRet;
}

//=======================================================================
// Function : ComputeTolerance
// purpose : 
//=======================================================================
Standard_Boolean IntTools_Tools::ComputeTolerance
  (const Handle(Geom_Curve)& theCurve3D,
   const Handle(Geom2d_Curve)& theCurve2D,
   const Handle(Geom_Surface)& theSurf,
   const Standard_Real theFirst,
   const Standard_Real theLast,
   Standard_Real& theMaxDist,
   Standard_Real& theMaxPar,
   const Standard_Real theTolRange)
{
  GeomLib_CheckCurveOnSurface aCS;
  //
  aCS.Init(theCurve3D, theSurf, theFirst, theLast, theTolRange);
  aCS.Perform(theCurve2D);
  if (!aCS.IsDone()) {
    return Standard_False;
  }

  //Obtaining precise result is impossible if we use 
  //numeric methods for solution. Therefore, we must provide
  //some margin. Otherwise, in the future
  //(when geometrical properties of the curve will be changed,
  //e.g. after trimming) we will be able to come
  //to the more precise minimum point. As result, this curve with the
  //tolerance computed earlier will become invalid.
  const Standard_Real anEps = (1.0+1.0e-5);
  theMaxDist = anEps*aCS.MaxDistance();
  theMaxPar  = aCS.MaxParameter();
  //
  return Standard_True;
}

//=======================================================================
// Function : ComputeIntRange
// purpose : 
//=======================================================================
Standard_Real IntTools_Tools::ComputeIntRange(const Standard_Real theTol1,
                                              const Standard_Real theTol2,
                                              const Standard_Real theAngle)
{
  Standard_Real aDt;
  //
  if (Abs(M_PI_2 - theAngle) < Precision::Angular()) {
    aDt = theTol2;
  }
  else {
    Standard_Real a1, a2, anAngle;
    //
    anAngle = (theAngle > M_PI_2) ? (M_PI - theAngle) : theAngle;
    a1 = theTol1 * tan(M_PI_2 - anAngle);
    a2 = theTol2 / sin(anAngle);
    aDt = a1 + a2;
  }
  //
  return aDt;
}
