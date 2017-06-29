// Created on: 2001-02-26
// Created by: Peter KURNEV
// Copyright (c) 2001-2014 OPEN CASCADE SAS
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
#include <BndLib_AddSurface.hxx>
#include <BRep_Tool.hxx>
#include <BRepAdaptor_Surface.hxx>
#include <Extrema_ExtCS.hxx>
#include <Extrema_POnCurv.hxx>
#include <Extrema_POnSurf.hxx>
#include <Geom_Curve.hxx>
#include <Geom_Surface.hxx>
#include <GeomAdaptor_Curve.hxx>
#include <GeomAdaptor_HCurve.hxx>
#include <GeomAdaptor_HSurface.hxx>
#include <GeomAdaptor_Surface.hxx>
#include <GeomAPI_ProjectPointOnSurf.hxx>
#include <gp_Ax1.hxx>
#include <gp_Circ.hxx>
#include <gp_Cone.hxx>
#include <gp_Cylinder.hxx>
#include <gp_Lin.hxx>
#include <gp_Pln.hxx>
#include <gp_Pnt.hxx>
#include <gp_Torus.hxx>
#include <IntCurveSurface_HInter.hxx>
#include <IntCurveSurface_IntersectionPoint.hxx>
#include <IntTools.hxx>
#include <IntTools_BeanFaceIntersector.hxx>
#include <IntTools_CArray1OfInteger.hxx>
#include <IntTools_CommonPrt.hxx>
#include <IntTools_Context.hxx>
#include <IntTools_EdgeFace.hxx>
#include <IntTools_FClass2d.hxx>
#include <IntTools_Range.hxx>
#include <IntTools_Root.hxx>
#include <IntTools_Tools.hxx>
#include <Precision.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>

#include <algorithm>
static
  Standard_Boolean IsCoplanar (const BRepAdaptor_Curve&  ,
                               const BRepAdaptor_Surface& );
static
  Standard_Boolean IsRadius (const BRepAdaptor_Curve& aCurve ,
                             const BRepAdaptor_Surface& aSurface,
                             const Standard_Real aCriteria);

//=======================================================================
//function : IntTools_EdgeFace::IntTools_EdgeFace
//purpose  : 
//=======================================================================
  IntTools_EdgeFace::IntTools_EdgeFace()
{
  myFuzzyValue = Precision::Confusion();
  myDiscret = 30;
  myEpsT   =1e-12;
  myDeflection=0.01;
  myIsDone=Standard_False;
  myErrorStatus=1;
  myQuickCoincidenceCheck=Standard_False;
}
//=======================================================================
//function : SetContext
//purpose  : 
//=======================================================================
void IntTools_EdgeFace::SetContext(const Handle(IntTools_Context)& theContext) 
{
  myContext = theContext;
}

//=======================================================================
//function : Context
//purpose  : 
//=======================================================================
const Handle(IntTools_Context)& IntTools_EdgeFace::Context()const 
{
  return myContext;
}
//=======================================================================
//function : SetEdge
//purpose  : 
//=======================================================================
void IntTools_EdgeFace::SetEdge(const TopoDS_Edge& anEdge)
{
  myEdge=anEdge;
}
//=======================================================================
//function : SetFace
//purpose  : 
//=======================================================================
void IntTools_EdgeFace::SetFace(const TopoDS_Face& aFace)
{
  myFace=aFace;
}
//=======================================================================
//function : Edge
//purpose  : 
//=======================================================================
const TopoDS_Edge& IntTools_EdgeFace::Edge()const 
{
  return myEdge;
}
//=======================================================================
//function : Face
//purpose  : 
//=======================================================================
const TopoDS_Face& IntTools_EdgeFace::Face()const 
{
  return myFace;
}
//=======================================================================
//function : SetFuzzyValue
//purpose  : 
//=======================================================================
void IntTools_EdgeFace::SetFuzzyValue(const Standard_Real theFuzz)
{
  myFuzzyValue = Max(theFuzz, Precision::Confusion());
}
//=======================================================================
//function : SetDiscretize
//purpose  : 
//=======================================================================
void IntTools_EdgeFace::SetDiscretize(const Standard_Integer aDiscret)
{
  myDiscret=aDiscret;
}
//=======================================================================
//function : SetDeflection
//purpose  : 
//=======================================================================
void IntTools_EdgeFace::SetDeflection(const Standard_Real aDefl) 
{
  myDeflection=aDefl;
} 
//=======================================================================
//function : SetEpsilonT
//purpose  : 
//=======================================================================
void IntTools_EdgeFace::SetEpsilonT(const Standard_Real anEpsT) 
{
  myEpsT=anEpsT;
} 
//=======================================================================
//function : SetRange
//purpose  : 
//=======================================================================
void IntTools_EdgeFace::SetRange(const Standard_Real aFirst,
                                 const Standard_Real aLast) 
{
  myRange.SetFirst (aFirst);
  myRange.SetLast  (aLast);
} 

//=======================================================================
//function : SetRange
//purpose  : 
//=======================================================================
void IntTools_EdgeFace::SetRange(const IntTools_Range& aRange) 
{
  SetRange(aRange.First(), aRange.Last());
} 
//=======================================================================
//function : IsDone
//purpose  : 
//=======================================================================
Standard_Boolean IntTools_EdgeFace::IsDone()const 
{
  return myIsDone;
} 
//=======================================================================
//function : ErrorStatus
//purpose  : 
//=======================================================================
Standard_Integer IntTools_EdgeFace::ErrorStatus()const 
{
  return myErrorStatus;
}
//=======================================================================
//function : CommonParts
//purpose  : 
//=======================================================================
const IntTools_SequenceOfCommonPrts& IntTools_EdgeFace::CommonParts() const 
{
  return mySeqOfCommonPrts;
}
//=======================================================================
//function : Range
//purpose  : 
//=======================================================================
const IntTools_Range&  IntTools_EdgeFace::Range() const
{
  return myRange;
} 
//=======================================================================
//function :  IsCoincident
//purpose  : 
//=======================================================================
Standard_Boolean IntTools_EdgeFace::IsCoincident() 
{
  Standard_Integer i, iCnt;
  Standard_Real dT, aT, aD, aT1, aT2, aU, aV;

  gp_Pnt aP;
  TopAbs_State aState;
  gp_Pnt2d aP2d;
  //
  GeomAPI_ProjectPointOnSurf& aProjector=myContext->ProjPS(myFace);

  const Standard_Integer aNbSeg=23;
  const Standard_Real aTresh=0.5;
  const Standard_Integer aTreshIdxF = RealToInt((aNbSeg+1)*0.25),
                         aTreshIdxL = RealToInt((aNbSeg+1)*0.75);
  const Handle(Geom_Surface) aSurf = BRep_Tool::Surface(myFace);

  aT1=myRange.First();
  aT2=myRange.Last();
  dT=(aT2-aT1)/aNbSeg;
  //
  Standard_Boolean isClassified = Standard_False;
  iCnt=0;
  for(i=0; i <= aNbSeg; ++i) {
    aT = aT1+i*dT;
    aP=myC.Value(aT);
    //
    aProjector.Perform(aP);
    if (!aProjector.IsDone()) {
      continue;
    }
    //
    
    aD=aProjector.LowerDistance();
    if (aD>myCriteria) {
      continue;
    }
    //

    ++iCnt; 

    //We classify only three points: in the begin, in the 
    //end and in the middle of the edge.
    //However, exact middle point (when i == (aNbSeg + 1)/2)
    //can be unprojectable. Therefore, it will not be able to
    //be classified. Therefore, points with indexes in 
    //[aTreshIdxF, aTreshIdxL] range are made available 
    //for classification.
    //isClassified == TRUE if MIDDLE point has been choosen and
    //classified correctly.

    if(((0 < i) && (i < aTreshIdxF)) || ((aTreshIdxL < i ) && (i < aNbSeg)))
      continue;

    if(isClassified && (i != aNbSeg))
      continue;

    aProjector.LowerDistanceParameters(aU, aV);
    aP2d.SetX(aU);
    aP2d.SetY(aV);

    IntTools_FClass2d& aClass2d=myContext->FClass2d(myFace);
    aState = aClass2d.Perform(aP2d);
    
    if(aState == TopAbs_OUT)
      return Standard_False;

    if(i != 0)
      isClassified = Standard_True;
  }
  //
  const Standard_Real aCoeff=(Standard_Real)iCnt/((Standard_Real)aNbSeg+1);
  return (aCoeff > aTresh);
}
//=======================================================================
//function : CheckData
//purpose  : 
//=======================================================================
void IntTools_EdgeFace::CheckData()
{
  if (BRep_Tool::Degenerated(myEdge)) {
    myErrorStatus=2;
  }
  if (!BRep_Tool::IsGeometric(myEdge)) { 
     myErrorStatus=3;
  }
}

//=======================================================================
//function : IsProjectable
//purpose  : 
//=======================================================================
Standard_Boolean IntTools_EdgeFace::IsProjectable
  (const Standard_Real aT) const
{
  Standard_Boolean bFlag; 
  gp_Pnt aPC;
  //
  myC.D0(aT, aPC);
  bFlag=myContext->IsValidPointForFace(aPC, myFace, myCriteria);
  //
  return bFlag;
}
//=======================================================================
//function : DistanceFunction
//purpose  : 
//=======================================================================
Standard_Real IntTools_EdgeFace::DistanceFunction
  (const Standard_Real t)
{
  Standard_Real aD;

  //
  gp_Pnt P;
  myC.D0(t, P);
  //
  Standard_Boolean bIsEqDistance;

  bIsEqDistance= IntTools_EdgeFace::IsEqDistance(P, myS, 1.e-7, aD); 
  if (bIsEqDistance) {
    aD=aD-myCriteria;
    return aD; 
  }
  
  //
  Standard_Boolean bFlag = Standard_False;

  GeomAPI_ProjectPointOnSurf& aLocProj = myContext->ProjPS(myFace);
  aLocProj.Perform(P);
  bFlag = aLocProj.IsDone();
  
  if(bFlag) {
    aD = aLocProj.LowerDistance();
  }
  //
  
  if (!bFlag) {
    myErrorStatus=11;
    return 99.;
  }
  
  // 
  //   aD=aProjector.LowerDistance();
  // 
  aD=aD-myCriteria;
  return aD; 
}
//
//=======================================================================
//function : IsEqDistance
//purpose  : 
//=======================================================================
Standard_Boolean IntTools_EdgeFace::IsEqDistance
  (const gp_Pnt& aP,
   const BRepAdaptor_Surface& aBAS,
   const Standard_Real aTol,
   Standard_Real& aD)
{
  Standard_Boolean bRetFlag=Standard_True;

  GeomAbs_SurfaceType aSurfType=aBAS.GetType();

  if (aSurfType==GeomAbs_Cylinder) {
    gp_Cylinder aCyl=aBAS.Cylinder();
    const gp_Ax1& anAx1  =aCyl.Axis();
    gp_Lin aLinAxis(anAx1);
    Standard_Real aDC, aRadius=aCyl.Radius();
    aDC=aLinAxis.Distance(aP);
    if (aDC < aTol) {
      aD=aRadius;
      return bRetFlag; 
    }
  }

  if (aSurfType==GeomAbs_Cone) {
    gp_Cone aCone=aBAS.Cone();
    const gp_Ax1& anAx1  =aCone.Axis();
    gp_Lin aLinAxis(anAx1);
    Standard_Real aDC, aRadius, aDS, aSemiAngle;
    aDC=aLinAxis.Distance(aP);
    if (aDC < aTol) {
      gp_Pnt anApex=aCone.Apex();
      aSemiAngle=aCone.SemiAngle();
      aDS=aP.Distance(anApex);
      
      aRadius=aDS*tan(aSemiAngle);
      aD=aRadius;
      return bRetFlag; 
    }
  }

  if (aSurfType==GeomAbs_Torus) {
    Standard_Real aMajorRadius, aMinorRadius, aDC;

    gp_Torus aTorus=aBAS.Torus();
    gp_Pnt aPLoc=aTorus.Location();
    aMajorRadius=aTorus.MajorRadius();
    
    aDC=fabs(aPLoc.Distance(aP)-aMajorRadius);
    if (aDC < aTol) {
      aMinorRadius=aTorus.MinorRadius();
      aD=aMinorRadius;
      return bRetFlag; 
    }
  }
  return !bRetFlag; 
}
//
//=======================================================================
//function : MakeType
//purpose  : 
//=======================================================================
Standard_Integer IntTools_EdgeFace::MakeType
  (IntTools_CommonPrt&  aCommonPrt)
{
  Standard_Real  af1, al1;
  Standard_Real  df1, tm;
  Standard_Boolean bAllNullFlag;
  //
  bAllNullFlag=aCommonPrt.AllNullFlag();
  if (bAllNullFlag) {
    aCommonPrt.SetType(TopAbs_EDGE);
    return 0;
  }
  //
  aCommonPrt.Range1(af1, al1);

  {
    gp_Pnt aPF, aPL;
    myC.D0(af1, aPF);
    myC.D0(al1, aPL);
    df1=aPF.Distance(aPL);
    Standard_Boolean isWholeRange = Standard_False;
    
    if((Abs(af1 - myRange.First()) < myC.Resolution(myCriteria)) &&
       (Abs(al1 - myRange.Last()) < myC.Resolution(myCriteria)))
      isWholeRange = Standard_True;
    
    
    if ((df1 > myCriteria * 2.) && isWholeRange) {
      aCommonPrt.SetType(TopAbs_EDGE);
    }
    else {
      if(isWholeRange) {
        tm = (af1 + al1) * 0.5;
        
        if(aPF.Distance(myC.Value(tm)) > myCriteria * 2.) {
          aCommonPrt.SetType(TopAbs_EDGE);
          return 0;
        }
      }
      
      if(!CheckTouch(aCommonPrt, tm)) {
        tm = (af1 + al1) * 0.5;
      }
      aCommonPrt.SetType(TopAbs_VERTEX);
      aCommonPrt.SetVertexParameter1(tm);
      aCommonPrt.SetRange1 (af1, al1);
    }
  }
 return 0;
}


//=======================================================================
//function : CheckTouch 
//purpose  : 
//=======================================================================
Standard_Boolean IntTools_EdgeFace::CheckTouch
  (const IntTools_CommonPrt& aCP,
   Standard_Real&            aTx) 
{
  if (myC.GetType() == GeomAbs_Line &&
      myS.GetType() == GeomAbs_Plane) {
    return Standard_False;
  }
  //
  Standard_Real aTF, aTL, Tol, U1f, U1l, V1f, V1l, af, al,aDist2, aMinDist2;
  Standard_Boolean theflag=Standard_False;
  Standard_Integer aNbExt, iLower;

  aCP.Range1(aTF, aTL);

  //
  Standard_Real aCR;
  aCR=myC.Resolution(myCriteria);
  if((Abs(aTF - myRange.First()) < aCR) &&
     (Abs(aTL - myRange.Last())  < aCR)) {
    return theflag; // EDGE 
  }
  //

  Tol = Precision::PConfusion();

  const Handle(Geom_Curve)&  Curve   =BRep_Tool::Curve  (myC.Edge(), af, al);
  const Handle(Geom_Surface)& Surface=BRep_Tool::Surface(myS.Face());
  //   Surface->Bounds(U1f,U1l,V1f,V1l);
  U1f = myS.FirstUParameter();
  U1l = myS.LastUParameter();
  V1f = myS.FirstVParameter();
  V1l = myS.LastVParameter();
  
  GeomAdaptor_Curve   TheCurve   (Curve,aTF, aTL);
  GeomAdaptor_Surface TheSurface (Surface, U1f, U1l, V1f, V1l); 
     
  Extrema_ExtCS anExtrema (TheCurve, TheSurface, Tol, Tol);

  aDist2 = 1.e100;

  if(anExtrema.IsDone()) {
    aMinDist2 = aDist2;

    if(!anExtrema.IsParallel()) {
      aNbExt=anExtrema.NbExt();
      
      if(aNbExt > 0) {
 iLower=1;
 for (Standard_Integer i=1; i<=aNbExt; i++) {
   aDist2=anExtrema.SquareDistance(i);
   if (aDist2 < aMinDist2) {
     aMinDist2=aDist2;
     iLower=i;
   }
 }
 aDist2=anExtrema.SquareDistance(iLower);
 Extrema_POnCurv aPOnC;
 Extrema_POnSurf aPOnS;
 anExtrema.Points(iLower, aPOnC, aPOnS);
 aTx=aPOnC.Parameter();
      }
      else {
 // modified by NIZHNY-MKK  Thu Jul 21 11:35:32 2005.BEGIN
 IntCurveSurface_HInter anExactIntersector;
  
 Handle(GeomAdaptor_HCurve) aCurve     = new GeomAdaptor_HCurve(TheCurve);
 Handle(GeomAdaptor_HSurface) aSurface = new GeomAdaptor_HSurface(TheSurface);
 
 anExactIntersector.Perform(aCurve, aSurface);

 if(anExactIntersector.IsDone()) {
   for(Standard_Integer i = 1; i <= anExactIntersector.NbPoints(); i++) {
     const IntCurveSurface_IntersectionPoint& aPoint = anExactIntersector.Point(i);
      
     if((aPoint.W() >= aTF) && (aPoint.W() <= aTL)) {
       aDist2=0.;
       aTx = aPoint.W();
     }
   }
 }
 // modified by NIZHNY-MKK  Thu Jul 21 11:35:40 2005.END
      }
    }
    else {
      return theflag;
    }
  }

  Standard_Real aBoundaryDist;

  aBoundaryDist = DistanceFunction(aTF) + myCriteria;
  if(aBoundaryDist * aBoundaryDist < aDist2) {
    aDist2 = aBoundaryDist * aBoundaryDist;
    aTx = aTF;
  }
  
  aBoundaryDist = DistanceFunction(aTL) + myCriteria;
  if(aBoundaryDist * aBoundaryDist < aDist2) {
    aDist2 = aBoundaryDist * aBoundaryDist;
    aTx = aTL;
  }

  Standard_Real aParameter = (aTF + aTL) * 0.5;
  aBoundaryDist = DistanceFunction(aParameter) + myCriteria;
  if(aBoundaryDist * aBoundaryDist < aDist2) {
    aDist2 = aBoundaryDist * aBoundaryDist;
    aTx = aParameter;
  }

  if(aDist2 > myCriteria * myCriteria) {
    return theflag;
  }
  
  if (fabs (aTx-aTF) < myEpsT) {
    return !theflag;
  }

  if (fabs (aTx-aTL) < myEpsT) {
    return !theflag;
  }

  if (aTx>aTF && aTx<aTL) {
    return !theflag;
  }

  return theflag;
}

//=======================================================================
//function : Perform
//purpose  : 
//=======================================================================
void IntTools_EdgeFace::Perform() 
{
  Standard_Integer i, aNb;
  IntTools_CommonPrt aCommonPrt;
  //
  aCommonPrt.SetEdge1(myEdge);
  //
  myErrorStatus=0;
  CheckData();
  if (myErrorStatus) {
    return;
  }
  //
  if (myContext.IsNull()) {
    myContext=new IntTools_Context;
  }
  //
  myIsDone = Standard_False;
  myC.Initialize(myEdge);
  GeomAbs_CurveType aCurveType;
  aCurveType=myC.GetType();
  //
  // Prepare myCriteria
  Standard_Real aFuzz = myFuzzyValue / 2.;
  Standard_Real aTolF = BRep_Tool::Tolerance(myFace) + aFuzz;
  Standard_Real aTolE = BRep_Tool::Tolerance(myEdge) + aFuzz;
  if (aCurveType == GeomAbs_BSplineCurve ||
      aCurveType==GeomAbs_BezierCurve) {
    //--- 5112
    Standard_Real diff1 = (aTolE/aTolF);
    Standard_Real diff2 = (aTolF/aTolE);
    if( diff1 > 100 || diff2 > 100 ) {
      myCriteria = Max(aTolE,aTolF);
    }
    else //--- 5112
      myCriteria = 1.5*aTolE + aTolF;
  }
  else {
    myCriteria = aTolE + aTolF;
  }
  
  myS = myContext->SurfaceAdaptor(myFace);
  
  if (myQuickCoincidenceCheck) {
    if (IsCoincident()) {
      aCommonPrt.SetType(TopAbs_EDGE);
      aCommonPrt.SetRange1(myRange.First(), myRange.Last());
      mySeqOfCommonPrts.Append(aCommonPrt);
      myIsDone=Standard_True;
      return;
    }
  }
  //
  IntTools_BeanFaceIntersector anIntersector(myC, myS, aTolE, aTolF);
  anIntersector.SetBeanParameters(myRange.First(), myRange.Last());
  //
  anIntersector.SetContext(myContext);
  //
  anIntersector.Perform();
  
  if(!anIntersector.IsDone()) {
    return;
  }
  
  for(Standard_Integer r = 1; r <= anIntersector.Result().Length(); r++) {
    const IntTools_Range& aRange = anIntersector.Result().Value(r);
    
    if(IsProjectable(IntTools_Tools::IntermediatePoint(aRange.First(), aRange.Last()))) {
      aCommonPrt.SetRange1(aRange.First(), aRange.Last());
      mySeqOfCommonPrts.Append(aCommonPrt);
    }
  }

  aNb = mySeqOfCommonPrts.Length();

  for (i=1; i<=aNb; i++) {
    IntTools_CommonPrt& aCP=mySeqOfCommonPrts.ChangeValue(i);
    //
    Standard_Real aTx1, aTx2;
    gp_Pnt aPx1, aPx2;
    //
    aCP.Range1(aTx1, aTx2);
    myC.D0(aTx1, aPx1);
    myC.D0(aTx2, aPx2);
    aCP.SetBoundingPoints(aPx1, aPx2);
    //
    MakeType (aCP); 
  }
  {
    // Line\Cylinder's Common Parts treatement
    GeomAbs_CurveType   aCType;
    GeomAbs_SurfaceType aSType;
    TopAbs_ShapeEnum aType;
    Standard_Boolean bIsTouch;
    Standard_Real aTx;
    
    aCType=myC.GetType();
    aSType=myS.GetType();
    
    if (aCType==GeomAbs_Line && aSType==GeomAbs_Cylinder) {
      for (i=1; i<=aNb; i++) {
        IntTools_CommonPrt& aCP=mySeqOfCommonPrts(i);
        aType=aCP.Type();
        if (aType==TopAbs_EDGE) {
          bIsTouch=CheckTouch (aCP, aTx);
          if (bIsTouch) {
            aCP.SetType(TopAbs_VERTEX);
            aCP.SetVertexParameter1(aTx);
            //aCP.SetRange1 (aTx, aTx);
          }
        }
        else if (aType==TopAbs_VERTEX) {
          bIsTouch=CheckTouchVertex (aCP, aTx);
          if (bIsTouch) {
            aCP.SetVertexParameter1(aTx);
            //aCP.SetRange1 (aTx, aTx);
          }
        }
      }
    }
    
    // Circle\Plane's Common Parts treatement
    
    if (aCType==GeomAbs_Circle && aSType==GeomAbs_Plane) {
      Standard_Boolean bIsCoplanar, bIsRadius;
      bIsCoplanar=IsCoplanar(myC, myS);
      bIsRadius=IsRadius(myC, myS, myCriteria);
      if (!bIsCoplanar && !bIsRadius) {
        for (i=1; i<=aNb; i++) {
          IntTools_CommonPrt& aCP=mySeqOfCommonPrts(i);
          aType=aCP.Type();
          if (aType==TopAbs_EDGE) {
            bIsTouch=CheckTouch (aCP, aTx);
            if (bIsTouch) {
              aCP.SetType(TopAbs_VERTEX);
              aCP.SetVertexParameter1(aTx);
              //aCP.SetRange1 (aTx, aTx);
            }
          }
          else if (aType==TopAbs_VERTEX) {
            bIsTouch=CheckTouchVertex (aCP, aTx);
            if (bIsTouch) {
              aCP.SetVertexParameter1(aTx);
              //aCP.SetRange1 (aTx, aTx);
            }
          }
        }
      }
    }
  }
  myIsDone=Standard_True;
}

//
// myErrorStatus
// 1 - the method Perform() is not invoked  
// 2,3,4,5 -the method CheckData() fails
// 6 - PrepareArgs() problems
// 7 - No Projectable ranges
// 8,9 - PrepareArgs() problems occured inside  projectable Ranges
// 11 - can't fill array  aFunc(i) in PrepareArgsFuncArrays 


//=======================================================================
//function : CheckTouch 
//purpose  : 
//=======================================================================
Standard_Boolean IntTools_EdgeFace::CheckTouchVertex 
  (const IntTools_CommonPrt& aCP,
   Standard_Real& aTx) 
{
  Standard_Real aTF, aTL, Tol, U1f,U1l,V1f,V1l;
  Standard_Real aEpsT, af, al,aDist2, aMinDist2, aTm, aDist2New;
  Standard_Boolean theflag=Standard_False;
  Standard_Integer aNbExt, i, iLower ;
  GeomAbs_CurveType aType;
  //
  aCP.Range1(aTF, aTL);
  aType=myC.GetType();
  //
  aEpsT=8.e-5;
  if (aType==GeomAbs_Line) {
    aEpsT=9.e-5;
  }
  //
  aTm=0.5*(aTF+aTL);
  aDist2=DistanceFunction(aTm);
  aDist2 *= aDist2;

  Tol = Precision::PConfusion();

  const Handle(Geom_Curve)&  Curve =BRep_Tool::Curve  (myC.Edge(), af, al);
  const Handle(Geom_Surface)& Surface=BRep_Tool::Surface(myS.Face());

  Surface->Bounds(U1f,U1l,V1f,V1l);
  
  GeomAdaptor_Curve   TheCurve   (Curve,aTF, aTL);
  GeomAdaptor_Surface TheSurface (Surface, U1f, U1l, V1f, V1l); 
     
  Extrema_ExtCS anExtrema (TheCurve, TheSurface, Tol, Tol);
   
  if(!anExtrema.IsDone()) {
    return theflag;
  }
  if (anExtrema.IsParallel()) {
    return theflag;
  }
  
  aNbExt=anExtrema.NbExt() ;
  if (!aNbExt) {
     return theflag;
  }

  iLower=1;
  aMinDist2=1.e100;
  for (i=1; i<=aNbExt; ++i) {
    aDist2=anExtrema.SquareDistance(i);
    if (aDist2 < aMinDist2) {
      aMinDist2=aDist2;
      iLower=i;
    }
  }

  aDist2New=anExtrema.SquareDistance(iLower);
  
  if (aDist2New > aDist2) {
    aTx=aTm;
    return !theflag;
  }
  
  if (aDist2New > myCriteria * myCriteria) {
    return theflag;
  }

  Extrema_POnCurv aPOnC;
  Extrema_POnSurf aPOnS;
  anExtrema.Points(iLower, aPOnC, aPOnS);

 
  aTx=aPOnC.Parameter();
  ///
  if (fabs (aTx-aTF) < aEpsT) {
    return theflag;
  }

  if (fabs (aTx-aTL) < aEpsT) {
    return theflag;
  }

  if (aTx>aTF && aTx<aTL) {
    return !theflag;
  }

  return theflag;
}


//=======================================================================
//function :  IsCoplanar
//purpose  : 
//=======================================================================
Standard_Boolean IsCoplanar (const BRepAdaptor_Curve& aCurve ,
                             const BRepAdaptor_Surface& aSurface)
{
  Standard_Boolean bFlag=Standard_False;

  GeomAbs_CurveType   aCType;
  GeomAbs_SurfaceType aSType;

  aCType=aCurve.GetType();
  aSType=aSurface.GetType();
    
  if (aCType==GeomAbs_Circle && aSType==GeomAbs_Plane) {
    gp_Circ aCirc=aCurve.Circle();
    const gp_Ax1& anAx1=aCirc.Axis();
    const gp_Dir& aDirAx1=anAx1.Direction();
    
    gp_Pln aPln=aSurface.Plane();
    const gp_Ax1& anAx=aPln.Axis();
    const gp_Dir& aDirPln=anAx.Direction();

    bFlag=IntTools_Tools::IsDirsCoinside(aDirAx1, aDirPln);
  }
  return bFlag;
}
//=======================================================================
//function :  IsRadius
//purpose  : 
//=======================================================================
Standard_Boolean IsRadius (const BRepAdaptor_Curve& aCurve,
                           const BRepAdaptor_Surface& aSurface,
                           const Standard_Real aCriteria)
{
  Standard_Boolean bFlag=Standard_False;

  GeomAbs_CurveType   aCType;
  GeomAbs_SurfaceType aSType;

  aCType=aCurve.GetType();
  aSType=aSurface.GetType();
    
  if (aCType==GeomAbs_Circle && aSType==GeomAbs_Plane) {
    gp_Circ aCirc=aCurve.Circle();
    const gp_Pnt aCenter=aCirc.Location();
    Standard_Real aR=aCirc.Radius();
    gp_Pln aPln=aSurface.Plane();
    Standard_Real aD=aPln.Distance(aCenter);
    if (fabs (aD-aR) < aCriteria) {
      return !bFlag;
    }
  }
  return bFlag;
}
//
//=======================================================================
//function :  AdaptiveDiscret
//purpose  : 
//=======================================================================
Standard_Integer AdaptiveDiscret (const Standard_Integer iDiscret,
                                  const BRepAdaptor_Curve& aCurve ,
                                  const BRepAdaptor_Surface& aSurface)
{
  Standard_Integer iDiscretNew;

  iDiscretNew=iDiscret;

  GeomAbs_SurfaceType aSType;

  aSType=aSurface.GetType();
    
  if (aSType==GeomAbs_Cylinder) {
   Standard_Real aELength, aRadius, dLR;

   aELength=IntTools::Length(aCurve.Edge());
   
   gp_Cylinder aCylinder=aSurface.Cylinder();
   aRadius=aCylinder.Radius();
   dLR=2*aRadius;

   iDiscretNew=(Standard_Integer)(aELength/dLR);
   
   if (iDiscretNew<iDiscret) {
     iDiscretNew=iDiscret;
   }
     
  }
  return iDiscretNew;
}
