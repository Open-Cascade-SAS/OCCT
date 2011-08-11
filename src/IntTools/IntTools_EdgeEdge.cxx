// File:	IntTools_EdgeEdge.cxx
// Created:	Thu Oct 26 10:14:32 2000
// Author:	Peter KURNEV
//		<pkv@irinox>
//

#include <IntTools_EdgeEdge.ixx>

#include <BRep_Tool.hxx>

#include <GCPnts_QuasiUniformDeflection.hxx>

#include <GeomAPI_ProjectPointOnCurve.hxx>
#include <GeomAPI_ExtremaCurveCurve.hxx>

#include <TColStd_ListOfReal.hxx>
#include <TColStd_ListIteratorOfListOfReal.hxx>
#include <TColStd_MapOfInteger.hxx>
#include <TColStd_ListOfReal.hxx>
#include <TColStd_SequenceOfReal.hxx>

#include <IntTools.hxx>
#include <IntTools_Range.hxx>
#include <IntTools_CArray1OfInteger.hxx>
#include <IntTools_CArray1OfReal.hxx>
#include <IntTools_Array1OfRange.hxx>
#include <IntTools_QuickSortRange.hxx>
#include <IntTools_CompareRange.hxx>
#include <IntTools_Root.hxx>
#include <IntTools_Array1OfRoots.hxx>
#include <IntTools_Compare.hxx>
#include <IntTools_QuickSort.hxx>
#include <IntTools_Root.hxx>
#include <IntTools_CommonPrt.hxx>
#include <IntTools_SequenceOfRanges.hxx>

#include <gp_Circ.hxx>
#include <gp_Ax1.hxx>
#include <gp_Lin.hxx>
#include <gp_Dir.hxx>
#include <gp_Pnt.hxx>

#include <gce_MakeCirc.hxx>
#include <Precision.hxx>
#include <IntTools.hxx>
//
#include <Bnd_Box.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopExp.hxx>
#include <BndLib_Add3dCurve.hxx>
#include <IntTools_BeanBeanIntersector.hxx>
#include <IntTools_Tools.hxx>

#include <CPnts_AbscissaPoint.hxx>
#include <gp_Circ.hxx>
#include <Extrema_ExtElC.hxx>
#include <Extrema_POnCurv.hxx>
#include <TopoDS_Iterator.hxx>

//=======================================================================
//function : IntTools_EdgeEdge::IntTools_EdgeEdge
//purpose  : 
//=======================================================================
  IntTools_EdgeEdge::IntTools_EdgeEdge()
{
  myTol1=1.e-7;
  myTol2=1.e-7;
  myDiscret=30;
  myEpsT=1e-12;
  myEpsNull=1e-12;
  myDeflection=0.01;
  myIsDone=Standard_False;
  myErrorStatus=1;
  myOrder=Standard_False;

  myPar1=0.;
  myParallel=Standard_False;
}

//=======================================================================
//function : SetEdge1
//purpose  : 
//=======================================================================
  void IntTools_EdgeEdge::SetEdge1(const TopoDS_Edge& anEdge)
{
  myEdge1=anEdge;
}
//=======================================================================
//function : SetEdge2
//purpose  : 
//=======================================================================
  void IntTools_EdgeEdge::SetEdge2(const TopoDS_Edge& anEdge)
{
  myEdge2=anEdge;
}

//=======================================================================
//function : SetTolerance1
//purpose  : 
//=======================================================================
  void IntTools_EdgeEdge::SetTolerance1(const Standard_Real aTol) 
{
  myTol1=aTol;
} 
//=======================================================================
//function : SetTolerance2
//purpose  : 
//=======================================================================
  void IntTools_EdgeEdge::SetTolerance2(const Standard_Real aTol) 
{
  myTol2=aTol;
} 

//=======================================================================
//function : SetDiscretize
//purpose  : 
//=======================================================================
  void IntTools_EdgeEdge::SetDiscretize(const Standard_Integer aDiscret)
{
  myDiscret=aDiscret;
}
//=======================================================================
//function : SetDeflection
//purpose  : 
//=======================================================================
  void IntTools_EdgeEdge::SetDeflection(const Standard_Real aDefl) 
{
  myDeflection=aDefl;
} 
//=======================================================================
//function : SetEpsilonT
//purpose  : 
//=======================================================================
  void IntTools_EdgeEdge::SetEpsilonT(const Standard_Real anEpsT) 
{
  myEpsT=anEpsT;
} 
//=======================================================================
//function : SetEpsilonNull
//purpose  : 
//=======================================================================
  void IntTools_EdgeEdge::SetEpsilonNull(const Standard_Real anEpsNull) 
{
  myEpsNull=anEpsNull;
} 

//=======================================================================
//function : SetRange1
//purpose  : 
//=======================================================================
  void IntTools_EdgeEdge::SetRange1(const Standard_Real aFirst,
				    const Standard_Real aLast) 
{
  myRange1.SetFirst (aFirst);
  myRange1.SetLast  (aLast);
} 
//=======================================================================
//function : SetRange2
//purpose  : 
//=======================================================================
  void IntTools_EdgeEdge::SetRange2(const Standard_Real aFirst,
				    const Standard_Real aLast) 
{
  myRange2.SetFirst (aFirst);
  myRange2.SetLast  (aLast);
} 

//=======================================================================
//function : SetRange1
//purpose  : 
//=======================================================================
  void IntTools_EdgeEdge::SetRange1(const IntTools_Range& aRange) 
{
  myRange1.SetFirst (aRange.First());
  myRange1.SetLast  (aRange.Last());
} 
//=======================================================================
//function : SetRange2
//purpose  : 
//=======================================================================
  void IntTools_EdgeEdge::SetRange2(const IntTools_Range& aRange) 
{
  myRange2.SetFirst (aRange.First());
  myRange2.SetLast  (aRange.Last());
} 

//=======================================================================
//function : Order
//purpose  : 
//=======================================================================
  Standard_Boolean IntTools_EdgeEdge::Order()const 
{
  return myOrder;
} 
//=======================================================================
//function : IsDone
//purpose  : 
//=======================================================================
  Standard_Boolean IntTools_EdgeEdge::IsDone()const 
{
  return myIsDone;
} 
//=======================================================================
//function : ErrorStatus
//purpose  : 
//=======================================================================
  Standard_Integer IntTools_EdgeEdge::ErrorStatus()const 
{
  return myErrorStatus;
} 

//=======================================================================
//function : CommonParts
//purpose  : 
//=======================================================================
  const IntTools_SequenceOfCommonPrts& IntTools_EdgeEdge::CommonParts() const 
{
  return mySeqOfCommonPrts;
}
//=======================================================================
//function : Range1
//purpose  : 
//=======================================================================
  const IntTools_Range&  IntTools_EdgeEdge::Range1() const
{
  return myRange1;
} 
//=======================================================================
//function : Range2
//purpose  : 
//=======================================================================
  const IntTools_Range&  IntTools_EdgeEdge::Range2() const
{
  return myRange2;
}
//=======================================================================
//function : Perform
//purpose  : 
//=======================================================================
  void IntTools_EdgeEdge::Perform() 
{
  Standard_Integer i, pri;
  IntTools_CommonPrt aCommonPrt;
  GeomAbs_CurveType   aCTFrom, aCTTo;
  
  myErrorStatus=0;
  CheckData();
  if (myErrorStatus)
    return;
  //
  // ProjectableRanges, etc
  Prepare();
  if (myErrorStatus) {
    return;
  }
  //

  if(myCFrom.GetType() == GeomAbs_Line && myCTo.GetType() == GeomAbs_Line) {
    ComputeLineLine();
    if (myOrder) {
      TopoDS_Edge aTmp;
      aTmp=myEdge1;
      myEdge1=myEdge2;
      myEdge2=aTmp;
    }
    return;
  }

  IntTools_BeanBeanIntersector anIntersector(myCFrom, myCTo, myTolFrom, myTolTo);
  anIntersector.SetBeanParameters(Standard_True, myTminFrom, myTmaxFrom);
  anIntersector.SetBeanParameters(Standard_False, myTminTo, myTmaxTo);
  anIntersector.Perform();
  
  if(!anIntersector.IsDone()) {
    myIsDone = Standard_False;
    return;
  }
  aCTFrom = myCFrom.GetType();
  aCTTo   = myCTo.GetType();
  
  aCommonPrt.SetEdge1(myCFrom.Edge());
  aCommonPrt.SetEdge2(myCTo.Edge());

  for(Standard_Integer r = 1; r <= anIntersector.Result().Length(); r++) {
    const IntTools_Range& aRange = anIntersector.Result().Value(r);
    
    if(IsProjectable(IntTools_Tools::IntermediatePoint(aRange.First(), aRange.Last()))) {
      aCommonPrt.SetRange1(aRange.First(), aRange.Last());

      if(((aRange.First() - myTminFrom) < Precision::PConfusion()) &&
	 ((myTmaxFrom  - aRange.Last()) < Precision::PConfusion())) {
	aCommonPrt.SetAllNullFlag(Standard_True);
      }
      mySeqOfCommonPrts.Append(aCommonPrt);
    }
  }
  
  Standard_Integer aNbCommonPrts;
  aNbCommonPrts=mySeqOfCommonPrts.Length();
  
  for (i=1; i<=aNbCommonPrts; i++) {
    IntTools_CommonPrt& aCmnPrt=mySeqOfCommonPrts.ChangeValue(i);
    pri=FindRangeOnCurve2 (aCmnPrt); 
    
    if (pri) {
      myErrorStatus=10;
      return;
    }
  }
  //
  
  {
    // Line Circle's Common Parts treatement
    if ((aCTFrom==GeomAbs_Line   && aCTTo==GeomAbs_Circle) || 
	(aCTFrom==GeomAbs_Circle && aCTTo==GeomAbs_Line) ||
	(aCTFrom==GeomAbs_Ellipse && aCTTo==GeomAbs_Ellipse) ||
	(aCTFrom==GeomAbs_Circle && aCTTo==GeomAbs_Circle))  {
      for (i=1; i<=aNbCommonPrts; i++) {
	IntTools_CommonPrt& aCP=mySeqOfCommonPrts(i);
	TopAbs_ShapeEnum aType=aCP.Type();
	Standard_Boolean bIsTouch;
	Standard_Real aTx1, aTx2;
	//
	if ((aType==TopAbs_EDGE) && !aCommonPrt.AllNullFlag()) {
	  bIsTouch=CheckTouch (aCP, aTx1, aTx2);
	  if (bIsTouch) {
	    aCP.SetType(TopAbs_VERTEX);
	    aCP.SetVertexParameter1(aTx1);
	    aCP.SetRange1 (aTx1, aTx1);
	    IntTools_Range& aRange2=(aCP.ChangeRanges2()).ChangeValue(1);
	    aRange2.SetFirst(aTx2);
	    aRange2.SetLast (aTx2);
	  }
	}
	//
	if (aType==TopAbs_VERTEX) {
	  if(aCTFrom==GeomAbs_Line || aCTTo==GeomAbs_Line) {
	    bIsTouch=CheckTouchVertex (aCP, aTx1, aTx2);
	    if (bIsTouch) {
	      aCP.SetVertexParameter1(aTx1);
	      aCP.SetRange1 (aTx1, aTx1);
	      IntTools_Range& aRange2=(aCP.ChangeRanges2()).ChangeValue(1);
	      aRange2.SetFirst(aTx2);
	      aRange2.SetLast (aTx2);
	    }
	  }
	}
      }
    }
  }

  if (myOrder) {
    TopoDS_Edge aTmp;
    aTmp=myEdge1;
    myEdge1=myEdge2;
    myEdge2=aTmp;
  }
    
  myIsDone=Standard_True;
} 
//=======================================================================
//function : CheckData
//purpose  : 
//=======================================================================
  void IntTools_EdgeEdge::CheckData()
{
  if (BRep_Tool::Degenerated(myEdge1)) {
    myErrorStatus=2;
  }
  if (!BRep_Tool::IsGeometric(myEdge1)) { 
     myErrorStatus=3;
  }
  if (BRep_Tool::Degenerated(myEdge2)) {
    myErrorStatus=4;
  }
  if (!BRep_Tool::IsGeometric(myEdge2)) { 
     myErrorStatus=5;
  } 
}
//=======================================================================
//function : Prepare
//purpose  : 
//=======================================================================
  void IntTools_EdgeEdge::Prepare() 
{
  Standard_Real aLE1, aLE2;
  
  //
  // 1.Prepare Curves' data
  aLE1 = 0.;

  if (!BRep_Tool::Degenerated(myEdge1) &&
      BRep_Tool::IsGeometric(myEdge1)) {
    Standard_Real f, l;
    const Handle(Geom_Curve)&  aCurve   =BRep_Tool::Curve  (myEdge1, f, l);
    GeomAdaptor_Curve   aGACurve   (aCurve, myRange1.First(), myRange1.Last());
    aLE1 = CPnts_AbscissaPoint::Length(aGACurve, myRange1.First(), myRange1.Last());
  }
  aLE2 = 0.;

  if (!BRep_Tool::Degenerated(myEdge2) &&
      BRep_Tool::IsGeometric(myEdge2)) {
    Standard_Real f, l;
    const Handle(Geom_Curve)&  aCurve   =BRep_Tool::Curve  (myEdge2, f, l);
    GeomAdaptor_Curve   aGACurve   (aCurve, myRange2.First(), myRange2.Last());
    aLE2 = CPnts_AbscissaPoint::Length(aGACurve, myRange2.First(), myRange2.Last());
  }
  
  if (aLE1 <= aLE2) {
    myCFrom.Initialize(myEdge1);
    myCTo  .Initialize(myEdge2);
    myTolFrom=myTol1;
    myTolTo=myTol2;
    myTminFrom=myRange1.First();
    myTmaxFrom=myRange1.Last ();
    myTminTo  =myRange2.First();
    myTmaxTo  =myRange2.Last ();
  }
  else {
    myCFrom.Initialize(myEdge2);
    myCTo  .Initialize(myEdge1);
    myTolFrom=myTol2;
    myTolTo=myTol1;
    myTminFrom=myRange2.First();
    myTmaxFrom=myRange2.Last ();
    myTminTo  =myRange1.First();
    myTmaxTo  =myRange1.Last ();

    myOrder=Standard_True; // revesed order
  }
  //
  // 2.Prepare myCriteria
  GeomAbs_CurveType aCT1, aCT2;
  aCT1=myCFrom.GetType();
  aCT2=myCTo.GetType()  ;
  
  Standard_Real aTol1, aTol2;
  aTol1=(aCT1==GeomAbs_BSplineCurve||
	 aCT1==GeomAbs_BezierCurve) ? 1.20*myTol1 : myTol1;
  
  aTol2=(aCT2==GeomAbs_BSplineCurve||
	 aCT2==GeomAbs_BezierCurve) ? 1.20*myTol2 : myTol2;
  
  myCriteria=aTol1+aTol2;
}

//=======================================================================
//function : FindProjectableRoot
//purpose  : 
//=======================================================================
  void IntTools_EdgeEdge::FindProjectableRoot (const Standard_Real tt1,
					       const Standard_Real tt2,
					       const Standard_Integer ff1,
					       const Standard_Integer ff2,
					       Standard_Real& tRoot)
{
  Standard_Real tm, t1, t2;
  Standard_Integer anIsProj1, anIsProj2, anIsProjm;
  //
  // Root can be on the ends of [tt1, tt2]
  Standard_Integer anOldErrorStatus=myErrorStatus;

  t1=DistanceFunction(tt1);
  myErrorStatus=anOldErrorStatus;
  if (fabs(t1)<myCriteria) {
    tRoot=tt1;
    return;
  }

  t1=DistanceFunction(tt2);
  myErrorStatus=anOldErrorStatus;
  if (fabs(t1)<myCriteria) {
    tRoot=tt2;
    return;
  }
  
  
  //
  // Root is inside [tt1, tt2]
  t1=tt1;  
  t2=tt2;
  anIsProj1=ff1;
  anIsProj2=ff2;
  
  while (1) {
    if (fabs(t1-t2) < myEpsT) {
      tRoot=.5*(t1+t2);
      return;
    }
    tm=.5*(t1+t2);
    anIsProjm=IsProjectable(tm);
    
    if (anIsProjm != anIsProj1) {
      t2=tm;
      anIsProj2=anIsProjm;
    }
    else {
      t1=tm;
      anIsProj1=anIsProjm;
    }
  }
}

//=======================================================================
//function : IsProjectable
//purpose  : 
//=======================================================================
  Standard_Integer IntTools_EdgeEdge::IsProjectable(const Standard_Real t) const
{ 
  Standard_Integer aNbProj;
  Standard_Real f, l;
  gp_Pnt aPFrom;

  GeomAPI_ProjectPointOnCurve aProjector;
  const TopoDS_Edge& aEFrom=myCFrom.Edge();
  Handle(Geom_Curve)aCurveFrom=BRep_Tool::Curve (aEFrom, f, l);
  aCurveFrom->D0(t, aPFrom);
  
  Handle(Geom_Curve)aCurveTo=BRep_Tool::Curve (myCTo.Edge(), f, l);
  aProjector.Init(aCurveTo, myTminTo, myTmaxTo);
  aProjector.Perform(aPFrom);
  aNbProj=aProjector.NbPoints();
  //
  if (myCTo.GetType()==GeomAbs_Circle) {
    gp_Circ aCirc=myCTo.Circle();
    const gp_Pnt& aCenter=aCirc.Location();
    if (aCenter.SquareDistance(aPFrom) < 1.e-7) {
      aNbProj=1;
    }
  }
  return aNbProj;
}

//=======================================================================
//function : DistanceFunction
//purpose  : 
//=======================================================================
  Standard_Real IntTools_EdgeEdge::DistanceFunction(const Standard_Real t)//const
{
  Standard_Real aD, f, l;
  GeomAPI_ProjectPointOnCurve aProjector;
  gp_Pnt aPFrom; //ZZ , aPTo;

  const TopoDS_Edge& aEFrom=myCFrom.Edge();
  const TopoDS_Edge& aETo  =myCTo.Edge();
  
  Handle(Geom_Curve)aCurveFrom=BRep_Tool::Curve (aEFrom, f, l);
  aCurveFrom->D0 (t, aPFrom);
  Handle(Geom_Curve)aCurveTo=BRep_Tool::Curve (aETo, f, l);

  if (myCTo.GetType()==GeomAbs_Circle) {
    gp_Circ aCirc=myCTo.Circle();
    const gp_Pnt& aCenter=aCirc.Location();
    const gp_Ax1& anAx1  =aCirc.Axis();
    const gp_Dir& aDir   =anAx1.Direction();
    gp_Lin aLin(aCenter, aDir);
    Standard_Real dPFromLin=aLin.Distance(aPFrom);
    if (dPFromLin < 1.e-7) {
      gp_Pnt anAnyPTo;
      aCurveTo->D0 (myTminTo, anAnyPTo);
      aD=aPFrom.Distance(anAnyPTo);
      
      aD=aD-myCriteria;
      return aD; 
    }
  }
  
  aProjector.Init(aCurveTo, myTminTo, myTmaxTo);
  aProjector.Perform(aPFrom);
  //
  Standard_Integer j, aNbPoints;
  //
  aNbPoints =aProjector.NbPoints();
  if (!aNbPoints) {
    for (j=0; j<=1; j++) {
      Standard_Real tt;
      tt=t+myEpsT;
      if (j) {
	tt=t-myEpsT;
      }
	
      aCurveFrom->D0 (tt, aPFrom);
      aProjector.Init(aCurveTo, myTminTo, myTmaxTo);
      aProjector.Perform(aPFrom);
      aNbPoints=aProjector.NbPoints();
      if (aNbPoints) {
	break;
      }
    }
  }
  

  if (!aNbPoints) {
    // Can't find projection.
    myErrorStatus=11;
    aD=100.;
    return aD;
  }
    
  aD=aProjector.LowerDistance();
  //
  aD=aD-myCriteria;
  return aD; 
}

//=======================================================================
//function : DerivativeFunction
//purpose  : 
//=======================================================================
  Standard_Real IntTools_EdgeEdge::DerivativeFunction(const Standard_Real t2)
{
  Standard_Real t1, t3, aD1, aD2, aD3;
  Standard_Real dt=1.e-7;
  t1=t2-dt;
  aD1=DistanceFunction(t1);
  t3=t2+dt;
  aD3=DistanceFunction(t3);
  
  aD2=.5*(aD3-aD1)/dt;
  return aD2; 
}

//=======================================================================
//function : FindGoldRoot
//purpose  : [private]
//=======================================================================
  Standard_Real IntTools_EdgeEdge::FindGoldRoot (const Standard_Real tA,
						 const Standard_Real tB,
						 const Standard_Real coeff)
{
  Standard_Real gs=0.61803399;
  Standard_Real a, b, xp, xl, yp, yl;

  a=tA;  b=tB;
  
  xp=a+(b-a)*gs;
  xl=b-(b-a)*gs;
  yp=coeff*DistanceFunction(xp);
  yl=coeff*DistanceFunction(xl);
  
 
  while (1) {
    
    if (fabs(b-a) < myEpsT) {
      return .5*(b+a);
    }
    
    if (yp < yl) {
      a=xl;
      xl=xp;
      xp=a+(b-a)*gs;
      yp=coeff*DistanceFunction(xp);
    }
    
    else {
      b=xp;
      xp=xl;
      yp=yl;
      xl=b-(b-a)*gs;
      yl=coeff*DistanceFunction(xl);
    }
  }
}  
//=======================================================================
//function : FindSimpleRoot
//purpose  : [private]
//=======================================================================
  Standard_Real IntTools_EdgeEdge::FindSimpleRoot (const Standard_Integer IP,
						   const Standard_Real tA,
						   const Standard_Real tB,
						   const Standard_Real fA)
{
  Standard_Real r, a, b, y, x0, s;
  
  a=tA; b=tB; r=fA;

  Standard_Integer step = 1, stepcheck = 1000, steplimit = 100000;
  Standard_Real value = (IP==1) ? DistanceFunction(0.5*(a+b)) : DerivativeFunction(0.5*(a+b));
  
  while (1) {
    x0=.5*(a+b);

    if (IP==1)
      y=DistanceFunction(x0);
    else 
      y=DerivativeFunction(x0);

    Standard_Real aMaxAB100 = 100.*Max(a, b);
    Standard_Real anEps     = Epsilon(aMaxAB100);
    Standard_Real anEpsT    = Max(anEps, myEpsT);
//    if (fabs(b-a) < myEpsT || y==0.) {
    if (fabs(b-a) < anEpsT || y==0.) {
      return x0;
    }
    
    if( step == stepcheck ) {
      if( Abs(value - y) <= 1.e-9 ) {
	return x0;
      }
      else {
	value = y;
	step = 1;
      }
    }

    if( step == steplimit ) {
      return x0;
    }

    s=y*r;

    if (s<0.) {
      b=x0;
      continue;
    }

    if (s>0.) {
      a=x0; r=y;
    }

    step++;

  }
}

//=======================================================================
//function : FindRangeOnCurve2
//purpose  : 
//=======================================================================
  Standard_Integer IntTools_EdgeEdge::FindRangeOnCurve2(IntTools_CommonPrt&  aCommonPrt)
{
  Standard_Real ttmp, f, l, af1, al1, am1, af2, al2, am2;
  Standard_Integer pri;
  gp_Pnt aPf1, aPl1, aPm1, aPf2, aPl2, aPm2;
  GeomAPI_ProjectPointOnCurve aProjector;

  aCommonPrt.Range1(af1, al1);
  am1=.5*(af1+al1);

  const TopoDS_Edge& anEdgeTo=myCTo.Edge();
  Handle(Geom_Curve)aCurveTo=BRep_Tool::Curve (anEdgeTo, f, l);

  const TopoDS_Edge& anEdgeFrom=myCFrom.Edge();
  Handle(Geom_Curve)aCurveFrom=BRep_Tool::Curve (anEdgeFrom, f, l);
  //
  // af2, aPf2
  aCurveFrom->D0 (af1, aPf1); 
  pri=GetParameterOnCurve2 (af1, af2);
  if (pri) {
    return 1;
  }
  aCurveTo->D0(af2, aPf2);
  //
  // al2, aPl2
  aCurveFrom->D0 (al1, aPl1);
  pri=GetParameterOnCurve2 (al1, al2);
  if (pri) {
    return 1;
  }
  aCurveTo->D0(al2, aPl2);
  //
  // am2, aPm2
  aCurveFrom->D0 (am1, aPm1);
  pri=GetParameterOnCurve2 (am1, am2);
  if (pri) {
    return 1;
  }
  aCurveTo->D0(am2, aPm2);
  //
  // Reverse C2 points if it is necessary  
  Standard_Boolean reverse = (af2 > al2);

  if (reverse) {
    ttmp=af2;
    af2=al2;
    al2=ttmp;
    gp_Pnt aPTmp;
    aPTmp=aPf2;
    aPf2=aPl2;
    aPl2=aPTmp;
  }

  if((Abs(af2 - myTminTo) < Precision::PConfusion()) &&
     (Abs(al2 - myTmaxTo) < Precision::PConfusion())) {
    aCommonPrt.SetAllNullFlag(Standard_True);
  }
  //
  //
  Standard_Boolean aVFlag1, aVFlag2, aGeomFlag1, aGeomFlag2;
  Standard_Real Df2m2, Dm2l2, Df2l2, df2m2, dm2l2, df2l2, df1m1, dm1l1, df1l1;
  Standard_Real tV1, tV2;
  //
  // parametric differences for C2
  Df2m2=fabs(af2-am2);
  Dm2l2=fabs(am2-al2);
  Df2l2=fabs(af2-al2);
  //
  // geometric distances for C2
  df2m2=aPf2.Distance(aPm2);
  dm2l2=aPm2.Distance(aPl2);
  df2l2=aPf2.Distance(aPl2);

  aVFlag1=(Df2m2<myEpsT && Dm2l2<myEpsT);
  aVFlag2=(df2m2 < myCriteria && dm2l2 < myCriteria);

  //
  //  Two perpendicular lines => VERTEX
  if ( aVFlag1 && aVFlag2) { 
    // V e r t e x
    aCommonPrt.SetType(TopAbs_VERTEX);
    pri=TreatVertexType(am1, am2, aCommonPrt);
    
    if (pri) { 
      tV2=.5*(af2+al2);
      aCommonPrt.SetVertexParameter2(tV2);
      aCommonPrt.AppendRange2 (af2, al2);
      
      tV1=.5*(af1+al1);
      aCommonPrt.SetVertexParameter1(tV1);
      aCommonPrt.SetRange1 (af1, al1);
    }
    return 0;
  }
  //
  // geometric distances for C1
  df1m1=aPf1.Distance(aPm1);
  dm1l1=aPm1.Distance(aPl1);
  df1l1=aPf1.Distance(aPl1);
  //
  // if geometric distances between boundaries is less than myCriteria
  // we have VERTEX
  aGeomFlag1=(df1l1 < myCriteria);
  aGeomFlag2=(df2l2 < myCriteria);
  if (aGeomFlag1 && aGeomFlag2) { 
    aCommonPrt.SetType(TopAbs_VERTEX);
    
    tV2=.5*(af2+al2);
    aCommonPrt.SetVertexParameter2(tV2);
    aCommonPrt.AppendRange2 (af2, al2);
      
    tV1=.5*(af1+al1);
    aCommonPrt.SetVertexParameter1(tV1);
    aCommonPrt.SetRange1 (af1, al1);
    return 0;
  }
  //
  //  ???
  if (Df2l2 < myEpsT &&  !aVFlag1) { 
    if (aPf1.Distance(aPl1) < myCriteria && aPf2.Distance(aPl2) < myCriteria) { 
      af1=myTminTo;
      al2=myTmaxTo;
      aCommonPrt.AppendRange2 (af1, al2);
      aCommonPrt.SetType(TopAbs_EDGE);
      return 0;
    }
  }
  //
  aProjector.Init(aCurveFrom, myTminFrom, myTmaxFrom);
  aProjector.Perform(aPm2);
  Standard_Integer aNbPoints=aProjector.NbPoints();
  if (aNbPoints) { 
    Standard_Real aDD=aProjector.LowerDistance();
    if (aDD > myCriteria) { 
      // Passed through 0
      aCommonPrt.SetType(TopAbs_EDGE);
      aCommonPrt.AppendRange2 (myTminTo, af2);
      aCommonPrt.AppendRange2 (al2, myTmaxTo);
      return 0;
    }
  }
  else {
    // Passed through 0
    aCommonPrt.SetType(TopAbs_EDGE); 
    aCommonPrt.AppendRange2 (myTminTo, af2);
    aCommonPrt.AppendRange2 (al2, myTmaxTo);
    return 0;
  }

  IsIntersection (af1, al1);
  if (!myParallel && !aCommonPrt.AllNullFlag()) {
    Standard_Real aPar2;
    GetParameterOnCurve2 (myPar1, aPar2);
    aCommonPrt.SetType(TopAbs_VERTEX);

    Standard_Boolean IsmyPar1 = Standard_True;

    if(Abs(af1-myTminFrom) < Precision::PConfusion()) {
      IsmyPar1 = Standard_False;
      aCommonPrt.SetVertexParameter1(af1);
      if(reverse)
	aCommonPrt.SetVertexParameter2(al2);
      else
	aCommonPrt.SetVertexParameter2(af2);
    }

    if(Abs(al1-myTmaxFrom) < Precision::PConfusion()) {
      IsmyPar1 = Standard_False;
      aCommonPrt.SetVertexParameter1(al1);

      if(reverse)
	aCommonPrt.SetVertexParameter2(af2);
      else
	aCommonPrt.SetVertexParameter2(al2);
    }

    if(Abs(af2-myTminTo) < Precision::PConfusion()) {
      IsmyPar1 = Standard_False;
      aCommonPrt.SetVertexParameter2(af2);

      if(reverse)
	aCommonPrt.SetVertexParameter1(al1);
      else
	aCommonPrt.SetVertexParameter1(af1);
    }

    if(Abs(al2-myTmaxTo) < Precision::PConfusion()) {
      IsmyPar1 = Standard_False;
      aCommonPrt.SetVertexParameter2(al2);

      if(reverse)
	aCommonPrt.SetVertexParameter1(af1);
      else
	aCommonPrt.SetVertexParameter1(al1);
    }
    //     aCommonPrt.SetVertexParameter1(myPar1);
    //     aCommonPrt.SetRange1 (af1, al1);
    
    //     aCommonPrt.SetVertexParameter2(aPar2);
    if(IsmyPar1) {
      aCommonPrt.SetVertexParameter1(myPar1);
      aCommonPrt.SetRange1 (af1, al1);
      
      aCommonPrt.SetVertexParameter2(aPar2);
    }
    aCommonPrt.AppendRange2 (af2, al2);
    return 0;
  }

  

  aCommonPrt.SetType(TopAbs_EDGE);
  aCommonPrt.AppendRange2 (af2, al2);
  return 0;
}
 
//=======================================================================
//function : IsIntersection
//purpose  : 
//=======================================================================
  void IntTools_EdgeEdge::IsIntersection (const Standard_Real ta, 
					  const Standard_Real tb) 
{
  Standard_Integer i, aNb, pri;
  Standard_Real t, f;
  GeomAbs_CurveType aCT1, aCT2;
  IntTools_CArray1OfReal anArgs, aFunc;
  //
  aCT1=myCFrom.GetType();
  aCT2=myCTo.GetType();
  if((aCT1==GeomAbs_Line) && (aCT2==GeomAbs_Line)) {
    const Handle(Geom_Curve)&  Curve1=BRep_Tool::Curve  (myCFrom.Edge(), t, f);
    const Handle(Geom_Curve)&  Curve2=BRep_Tool::Curve  (myCTo.Edge()  , t, f);
    
    GeomAdaptor_Curve   TheCurve1   (Curve1);
    GeomAdaptor_Curve   TheCurve2   (Curve2);
    Extrema_ExtCC anExtrema (TheCurve1, TheCurve2);

    if(anExtrema.IsDone() && anExtrema.IsParallel()) {
      myParallel = Standard_True;
      return;
    }
  }
  //
  if (aCT1==GeomAbs_Circle && aCT2==GeomAbs_Circle) {
    Standard_Boolean bIsDone, bIsParallel;
    Standard_Integer aNbExt;
    Standard_Real aD2, aCriteria2, aT1;
    gp_Circ aCirc1, aCirc2;
    Extrema_POnCurv aPC1, aPC2;
    //
    aCirc1=myCFrom.Circle();
    aCirc2=myCTo.Circle();
    //
    Extrema_ExtElC aExtElC(aCirc1, aCirc2);
    //
    bIsDone=aExtElC.IsDone();
    if (bIsDone) {
      bIsParallel=aExtElC.IsParallel();
      if (!bIsParallel) {
	aCriteria2=myCriteria*myCriteria;
	aNbExt=aExtElC.NbExt();
	for (i=1; i<=aNbExt; ++i) {
	  aD2=aExtElC.SquareDistance(i);
	  if (aD2<aCriteria2) {
	    aExtElC.Points(i, aPC1, aPC2);
	    aT1=aPC1.Parameter();
	    if (aT1>ta && aT1<tb) {
	      myPar1=aT1;
	      myParallel=Standard_False;
	      return;
	    }
	  }
	}
      }
    }
  }
  //
  // Prepare values of arguments for the interval [ta, tb]
  pri=IntTools::PrepareArgs (myCFrom, tb, ta, myDiscret, myDeflection, anArgs);
  aNb=anArgs.Length();

  aFunc.Resize(aNb);
  for (i=0; i<aNb; i++) {
    t=anArgs(i);
    f=DistanceFunction(t);
    if (fabs(f) < myEpsNull) { 
      f=0.;
    }
    aFunc(i)=f;
  }
  FindDerivativeRoot(anArgs, aFunc);
  return ;
}

//=======================================================================
//function : FindDerivativeRoot
//purpose  : 
//=======================================================================
  void IntTools_EdgeEdge::FindDerivativeRoot(const IntTools_CArray1OfReal& t,
					     const IntTools_CArray1OfReal& f)  
{
  Standard_Integer i, n, k;
  Standard_Real fr, tr, anEpsNull;
  IntTools_CArray1OfReal fd;
  TColStd_SequenceOfReal aTSeq, aFSeq;
  
  anEpsNull=100.*myEpsNull;
  myPar1=0.;
  myParallel=Standard_True;
  
  n=t.Length();
  fd.Resize(n+1);
  //
  // Table of derivatives
  fd(0)=(f(1)-f(0))/(t(1)-t(0));
  if (fabs(fd(0)) < anEpsNull) {
    fd(0)=0.;
  }
  k=n-1;
  for (i=1; i<k; i++) {
    fd(i)=.5*(f(i+1)-f(i-1))/(t(i)-t(i-1));
    if (fabs(fd(i)) < anEpsNull) {
      fd(i)=0.;
    }
  }
  fd(n-1)=(f(n-1)-f(n-2))/(t(n-1)-t(n-2));
  if (fabs(fd(n-1)) < anEpsNull) {
    fd(n-1)=0.;
  }
  //
  // Finding the range where the derivatives have different signs
  // for neighbouring points
  for (i=1; i<n; i++) {
    Standard_Real fd1, fd2, t1, t2, fabsfd1, fabsfd2;
    Standard_Boolean bF1, bF2;
    t1 =t(i-1);
    t2 =t(i);
    fd1=fd(i-1);
    fd2=fd(i);

    fabsfd1=fabs(fd1);
    bF1=fabsfd1 < myEpsNull;
    
    fabsfd2=fabs(fd2);
    bF2=fabsfd2 < myEpsNull;

    //aa
    if (fd1*fd2 < 0.) {
      tr=FindSimpleRoot(2, t1, t2, fd1);
      fr=DistanceFunction(tr);
      myPar1=tr;
      myParallel=Standard_False;
      break;
    }
    
    if (!bF1 && bF2) {
      tr=t2;
      fr=fd2;
      myPar1=tr;
      myParallel=Standard_False;
      break;
    }
    
    if (bF1 && !bF2) {
      tr=t1;
      fr=fd1;
      myPar1=tr;
      myParallel=Standard_False;
      break;
    }

  }
}

//=======================================================================
//function : GetParameterOnCurve2
//purpose  : 
//=======================================================================
  Standard_Integer IntTools_EdgeEdge::GetParameterOnCurve2(const Standard_Real aT1, 
							   Standard_Real& aT2) const
{
  Standard_Real f, l;
  Standard_Integer j, found, aNbPoints;
  const TopoDS_Edge& anEdgeTo=myCTo.Edge();
  const TopoDS_Edge& anEdgeFrom=myCFrom.Edge();
  
  Handle(Geom_Curve)aCurveFrom=BRep_Tool::Curve (anEdgeFrom, f, l);
  Handle(Geom_Curve)aCurveTo  =BRep_Tool::Curve (anEdgeTo,   f, l);

  gp_Pnt aP1;
  aCurveFrom->D0 (aT1, aP1);
  GeomAPI_ProjectPointOnCurve aProjector;
  aProjector.Init(aCurveTo, myTminTo, myTmaxTo);
  aProjector.Perform(aP1);
  aNbPoints=aProjector.NbPoints();
  found=1;
  if (!aNbPoints) {
    found=0;
    for (j=0; j<=1; j++) {
      Standard_Real tt;
      tt=aT1+myEpsT;
      if (j) {
	tt=aT1-myEpsT;
      }
      aCurveFrom->D0 (tt, aP1);
      aProjector.Init(aCurveTo, myTminTo, myTmaxTo);
      aProjector.Perform(aP1);
      aNbPoints=aProjector.NbPoints();
      if (aNbPoints) {
	found=1;
	break;
      }
    }
  }

  if (!found) {
    aCurveFrom->D0 (aT1, aP1);
    Standard_Real aDistance = RealLast();

    for(Standard_Integer pIt=0; pIt < 2; pIt++) {
      Standard_Real adist = aDistance;
      if(pIt)
	adist = aP1.Distance(aCurveTo->Value(myTminTo));
      else
	adist = aP1.Distance(aCurveTo->Value(myTmaxTo));
      
      if(adist < myCriteria) {
	found = Standard_True;

	if(adist < aDistance) {
	  aT2 = (pIt) ? myTminTo : myTmaxTo;
	  aDistance = adist;
	}
      }      
    }
    if(found)
      return 0;
  }

  if (!found) {
    aT2=0.;
    return 1;
  }

  for (j=1; j<=aNbPoints; j++) {
    aT2=aProjector.Parameter(j);
    f=aProjector.Distance(j);
  }

  aT2=aProjector.LowerDistanceParameter();
  if (aT2 < myTminTo) {
    aT2=myTminTo;
  }
  if (aT2 > myTmaxTo) {
    aT2=myTmaxTo;
  }
  return 0;
}
 
//=======================================================================
//function : TreatVertexType
//purpose  : 
//=======================================================================
  Standard_Integer IntTools_EdgeEdge::TreatVertexType(const Standard_Real am1,
						      const Standard_Real am2,
						      IntTools_CommonPrt&  aCommonPrt) 
{
  Standard_Real  f1, l1, f2, l2, Alfa , aPeriod;
  gp_Pnt aPm1, aPm2, aP;
  gp_Vec aVm1, aVm2;
  

  const TopoDS_Edge& anEdgeFrom=myCFrom.Edge();
  Handle(Geom_Curve)aCurveFrom=BRep_Tool::Curve (anEdgeFrom, f1, l1);
  aCurveFrom->D1 (am1, aPm1, aVm1);
  aVm1.Normalize();

  const TopoDS_Edge& anEdgeTo=myCTo.Edge();
  Handle(Geom_Curve)aCurveTo=BRep_Tool::Curve (anEdgeTo, f2, l2);
  aCurveTo->D1 (am2, aPm2, aVm2);
  aVm2.Normalize();

  Alfa=aVm1.Angle(aVm2);

  if (Alfa < Precision::Angular()) {
    return 1;
  }

  Standard_Real sinAlfa, cosAlfa, dd, tf1, tl1, tf2, tl2, aL1, aL2;
  Standard_Integer ip;
  
  sinAlfa=sin(Alfa);
  cosAlfa=cos(Alfa);
    
  dd=aPm1.Distance(aPm2);
  // aL2
  if (dd>myCriteria) {
    return 1;
  }
  aL2=(myTolTo*cosAlfa+myTolFrom)/sinAlfa;
  // left point
  aP.SetXYZ(aPm2.XYZ()-aVm2.XYZ()*aL2);
  ip=IntTools::Parameter (aP, aCurveTo, tf2);
  if (ip){
    return ip;
  }
  // 
  if(aP.Distance(aCurveTo->Value(tf2)) > myTolTo)
    return 1;

  // right point
  aP.SetXYZ(aPm2.XYZ()+aVm2.XYZ()*aL2);
  ip=IntTools::Parameter (aP, aCurveTo, tl2);
  if (ip){
    return ip;
  }

  if(aP.Distance(aCurveTo->Value(tl2)) > myTolTo)
    return 1;
  
  // aL1
  if (dd>myCriteria) {
    return 1;
  }
 
  aL1=(myTolFrom*cosAlfa+myTolTo)/sinAlfa;
  // left point
  aP.SetXYZ(aPm1.XYZ()-aVm1.XYZ()*aL1);
  ip=IntTools::Parameter (aP, aCurveFrom, tf1);
  if (ip){
    return ip;
  }

  if(aP.Distance(aCurveFrom->Value(tf1)) > myTolFrom)
    return 1;

  // right point
  aP.SetXYZ(aPm1.XYZ()+aVm1.XYZ()*aL1);
  ip=IntTools::Parameter (aP, aCurveFrom, tl1);
  if (ip){
    return ip;
  }

  if(aP.Distance(aCurveFrom->Value(tl1)) > myTolFrom)
    return 1;

  //
  if (aCurveFrom->IsPeriodic()) {
    aPeriod=aCurveFrom->Period();
    if (tf1<f1 || tf1>l1) {
      tf1=tf1+aPeriod;
    }
    if (tl1<f1 || tl1>l1) {
      tl1=tl1+aPeriod;
    }
  }
  //
  // First range
  aCommonPrt.SetRange1    (tf1, tl1);
  aCommonPrt.SetVertexParameter1((tf1 + tl1) * 0.5);
  //
  // Second Range(s) 
  if (aCurveTo->IsPeriodic() && tf2 > tl2) {
    // aCurveTo is periodic curve and we pass through 0.
    
    aPeriod=aCurveTo->Period();
    aCommonPrt.AppendRange2 (tf2, aPeriod);
    aCommonPrt.AppendRange2 (0., tl2);
    aCommonPrt.SetVertexParameter2((tf2 + aPeriod) * 0.5);
  }
  else {
    // usual cases 
    return 1;
    //
  }
  return 0;
}
 

//
//
// Print block

// myErrorStatus
// 1 - the method Perform() is not invoked  
// 2,3,4,5 -the method CheckData() fails
// 6 - PrepareArgs() problems
// 7 - No Projectable ranges
// 8,9 - PrepareArgs() problems occured inside  projectable Ranges
// 10 - problems in FindRange2
// 11 - can't fill array  aFunc(i) in PrepareArgsFuncArrays: 
//      possible reason is that no points on myCFrom that could be projected
//      on myCTo
//   

#include <Precision.hxx>
#include <Geom_Curve.hxx>
#include <GeomAdaptor_Curve.hxx>
#include <BRep_Tool.hxx>
#include <Extrema_ExtCC.hxx>
#include <ElCLib.hxx>
//=======================================================================
//function : CheckTouchVertex 
//purpose  : line/Circle refinement
//=======================================================================
  Standard_Boolean IntTools_EdgeEdge::CheckTouchVertex (const IntTools_CommonPrt& aCP,
							Standard_Real& aTx1,
							Standard_Real& aTx2) const
{
  Standard_Boolean bFlag;
  Standard_Real aTFR1, aTLR1, aTFR2, aTLR2;
  Standard_Real aTL1, aTL2, aTC1, aTC2;
  Standard_Real aRC, aDLC, aD2, aC2, aTLx, aTCx;
  GeomAbs_CurveType aTFrom, aTTo;
  gp_Circ aCirc;
  gp_Lin  aLine;
  gp_Pnt aPC, aPLx, aPCx;
  //
  bFlag=Standard_False;
  aCP.Range1(aTFR1, aTLR1);
  (aCP.Ranges2())(1).Range(aTFR2, aTLR2);
  //
  aTFrom=myCFrom.GetType();
  aTTo  =myCTo.GetType();
  //
  aTL1=aTFR1;
  aTL2=aTLR1;
  aTC1=aTFR2;
  aTC2=aTLR2;
  if (aTFrom==GeomAbs_Circle) {
    aCirc=myCFrom.Circle();
    aLine=myCTo.Line();
    aTL1=aTFR2;
    aTL2=aTLR2;
    aTC1=aTFR1;
    aTC2=aTLR1;
  }
  else {
    aCirc=myCTo.Circle();
    aLine=myCFrom.Line();
  }
  //
  aPC=aCirc.Location();
  aRC=aCirc.Radius();
  //
  aDLC=aLine.Distance(aPC);
  if (fabs(aDLC-aRC)>myCriteria) {
    return bFlag;
  }
  //
  aTLx=ElCLib::Parameter(aLine, aPC);
  aPLx=ElCLib::Value(aTLx, aLine);
  aTCx=ElCLib::Parameter(aCirc, aPLx);
  aPCx=ElCLib::Value(aTCx, aCirc);
  aD2=aPLx.SquareDistance(aPCx);
  aC2=myCriteria*myCriteria;
  if (aD2>aC2) {
    return bFlag;
  }
  //
  if (aTLx<aTL1 || aTLx>aTL2) {
    return bFlag;
  }
  if (aTCx<aTC1 || aTCx>aTC2) {
    return bFlag;
  }
  //
  aTx1=aTLx;
  aTx2=aTCx;
  if (aTFrom==GeomAbs_Circle) {
    aTx1=aTCx;
    aTx2=aTLx;
  }
  //
  return !bFlag;
}
//=======================================================================
//function : CheckTouch 
//purpose  : 
//=======================================================================
  Standard_Boolean IntTools_EdgeEdge::CheckTouch(const IntTools_CommonPrt& aCP,
						 Standard_Real& aTx1,
						 Standard_Real& aTx2) 
{
  Standard_Real aTF1, aTL1, aTF2, aTL2, Tol, af, al,aDist2, aMinDist2;
  Standard_Boolean theflag=Standard_False;
  Standard_Integer aNbExt, i, iLower;

  aCP.Range1(aTF1, aTL1);
  (aCP.Ranges2())(1).Range(aTF2, aTL2);

  Tol = Precision::PConfusion();

  const Handle(Geom_Curve)&  Curve1   =BRep_Tool::Curve  (myCFrom.Edge(), af, al);
  const Handle(Geom_Curve)&  Curve2   =BRep_Tool::Curve  (myCTo.Edge()  , af, al);
  
  GeomAdaptor_Curve   TheCurve1   (Curve1, aTF1, aTL1);
  GeomAdaptor_Curve   TheCurve2   (Curve2, aTF2, aTL2);

  {
    Standard_Real aTol1 = TheCurve1.Resolution(myCriteria);
    aTol1 = (Tol < aTol1) ? Tol : aTol1;
    
    Standard_Boolean isfirst = (Abs(myTminFrom - aTF1) < aTol1);
    Standard_Boolean islast  = (Abs(myTmaxFrom - aTL1) < aTol1);
    
    if(!isfirst || !islast) {
      if(isfirst) {
	aTx1 = aTF1;
	GeomAPI_ProjectPointOnCurve aProjector;
	aProjector.Init(Curve2, aTF2, aTL2);
	aProjector.Perform(Curve1->Value(aTx1));
	//
	if(aProjector.NbPoints() > 0)
	  aTx2 = aProjector.LowerDistanceParameter();
	else {
	  if(Curve1->Value(aTx1).Distance(Curve2->Value(aTF2)) < myCriteria)
	    aTx2 = aTF2;
	  else
	    aTx2 = aTL2;
	}
	return !theflag;
      }
      
      if(islast) {
	aTx1 = aTL1;
	GeomAPI_ProjectPointOnCurve aProjector;
	aProjector.Init(Curve2, aTF2, aTL2);
	aProjector.Perform(Curve1->Value(aTx1));
	if(aProjector.NbPoints() > 0)
	  aTx2 = aProjector.LowerDistanceParameter();
	else {
	  if(Curve1->Value(aTx1).Distance(Curve2->Value(aTL2)) < myCriteria)
	    aTx2 = aTL2;
	  else
	    aTx2 = aTF2;
	}
	return !theflag;
      }
    }
  }
				 
  Extrema_ExtCC anExtrema (TheCurve1, TheCurve2, aTF1-Tol, aTL1+Tol, aTF2-Tol, aTL2+Tol, Tol, Tol);
  
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

  Standard_Boolean istouch = Standard_True;
  Standard_Integer avalidindex = 0;

  iLower=1;
  aMinDist2=1.e100;
  for (i=1; i<=aNbExt; ++i) {
    aDist2=anExtrema.SquareDistance(i);
    if (aDist2 < aMinDist2) {
      aMinDist2=aDist2;
      iLower=i;
    }

    if(aDist2 < myCriteria * myCriteria) {
      if(avalidindex) {
	Extrema_POnCurv aPOnC1, aPOnC2;
	anExtrema.Points(i, aPOnC1, aPOnC2);
	Standard_Real aPar1 = aPOnC1.Parameter();
	anExtrema.Points(avalidindex, aPOnC1, aPOnC2);
	Standard_Real aPar2 = aPOnC1.Parameter();
	
	if(Abs(aPar1 - aPar2) > Precision::PConfusion()) {
	  istouch = Standard_False;
	}
      }
      avalidindex = i;
    }
  }

  aDist2=anExtrema.SquareDistance(iLower);
  if (aDist2 > myCriteria * myCriteria) {
    return theflag;
  }

  Extrema_POnCurv aPOnC1, aPOnC2;
  anExtrema.Points(iLower, aPOnC1, aPOnC2);
  
  aTx1=aPOnC1.Parameter();
  aTx2=aPOnC2.Parameter();

  if((myCFrom.GetType() == GeomAbs_Line   && myCTo.GetType() == GeomAbs_Circle) || 
     (myCFrom.GetType() == GeomAbs_Circle && myCTo.GetType() == GeomAbs_Line))
  {
    Standard_Real aRadius;
    GeomAbs_CurveType aTFrom, aTTo;
    gp_Circ aCirc;
    gp_Lin  aLine;
    gp_Pnt aPCenter, aPOnLine;

    aTFrom=myCFrom.GetType();
    aTTo  =myCTo.GetType();
    
    if (aTFrom==GeomAbs_Circle) {
      aCirc=myCFrom.Circle();
      aLine=myCTo.Line();
      Curve2->D0(aTx2, aPOnLine);
    }

    else {
      aCirc=myCTo.Circle();
      aLine=myCFrom.Line();
      Curve1->D0(aTx1, aPOnLine);
    }
    

    aPCenter=aCirc.Location();
    aRadius =aCirc.Radius();
  
    aDist2=aPOnLine.SquareDistance(aPCenter);
    aDist2=fabs (sqrt(aDist2)-aRadius);
    aDist2 *= aDist2;
    if (aDist2 < Tol * Tol) {
      return !theflag;
    }
  }
  
  GeomAPI_ProjectPointOnCurve aProjector;
  Standard_Real aMidPar, aMidDist;   
  aMidPar = (aTF1 + aTL1) * 0.5;  
  aProjector.Init(Curve2, aTF2, aTL2);
  aProjector.Perform(Curve1->Value(aMidPar));
  if(aProjector.NbPoints() > 0) {
    aMidDist=aProjector.LowerDistance();
    if(aMidDist * aMidDist < aDist2 || !istouch) {
      aTx1 = aMidPar;
      aTx2 = aProjector.LowerDistanceParameter();
    }
  }
  
  if (fabs (aTx1-aTF1) < Tol) {
    return !theflag;
  }

  if (fabs (aTx1-aTL1) < Tol) {
    return !theflag;
  }

  if (aTx1 > (aTF1-Tol) && aTx1 < (aTL1+Tol) ) {
    return !theflag;
  }

  return theflag;
}
//=======================================================================
//function :  CheckInterval
//purpose  : 
//=======================================================================
  Standard_Boolean IntTools_EdgeEdge::CheckInterval(const Standard_Real aT1,
						    const Standard_Real aT2)
{
  Standard_Boolean bFlag=Standard_False;
  Standard_Integer i, aNb=4;
  Standard_Real aT, dT, aDist;
  
  dT=(aT2-aT1)/aNb;
  
  for (i=1; i<aNb; ++i) {
    aT=aT1+i*dT;
    aDist=DistanceFunction(aT);
    if (aDist>myEpsNull) {
      return bFlag;
    }
  }
  return !bFlag;
}

//=======================================================================
//function : RemoveIdenticalRoots 
//purpose  : 
//=======================================================================
  void IntTools_EdgeEdge::RemoveIdenticalRoots()
{
  Standard_Integer aNbRoots, j, k;

  aNbRoots=mySequenceOfRoots.Length();
  for (j=1; j<=aNbRoots; j++) { 
    const IntTools_Root& aRj=mySequenceOfRoots(j);
    for (k=j+1; k<=aNbRoots; k++) {
      const IntTools_Root& aRk=mySequenceOfRoots(k);
      
      Standard_Real aTj, aTk, aDistance;
      gp_Pnt aPj, aPk;

      aTj=aRj.Root();
      aTk=aRk.Root();

      myCFrom.D0(aTj, aPj);
      myCFrom.D0(aTk, aPk);

      aDistance=aPj.Distance(aPk);
      if (aDistance < myCriteria) {
	mySequenceOfRoots.Remove(k);
	aNbRoots=mySequenceOfRoots.Length();
      }
    }
  }
}

//=======================================================================
//function : ComputeLineLine 
//purpose  : 
//=======================================================================
  void IntTools_EdgeEdge::ComputeLineLine()
{
  Standard_Boolean IsParallel, IsCoincide;
  Standard_Real Tolang2, Tol2;
  gp_Pnt P11, P12, P21, P22;
  //
  myIsDone = Standard_True;
  //
  IsParallel = Standard_False;
  IsCoincide = Standard_False;
  Tolang2 = 1.e-16;
  Tol2 = myCriteria*myCriteria; 
  //
  gp_Lin C1 = myCFrom.Line();
  gp_Lin C2 = myCTo.Line();
  const gp_Dir& D1 = C1.Position().Direction();
  const gp_Dir& D2 = C2.Position().Direction();
  Standard_Real aCos = D1.Dot(D2);
  Standard_Real Ang2;
  if(aCos >= 0. ) {
    Ang2 = 2.*(1. - aCos);
  }
  else {
    Ang2 = 2.*(1. + aCos);
  }

  if(Ang2 <= Tolang2) {
    IsParallel = Standard_True;
    if(C2.SquareDistance(C1.Location()) <= Tol2) {
      IsCoincide = Standard_True;
      P11 = ElCLib::Value(myTminFrom, C1);
      P12 = ElCLib::Value(myTmaxFrom, C1);
    }
  }
  else {
    //Check coincidence of extremity points;
    //Check only shortest line
    P11 = ElCLib::Value(myTminFrom, C1);
    P12 = ElCLib::Value(myTmaxFrom, C1);
    if(C2.SquareDistance(P11) <= Tol2 && C2.SquareDistance(P12) <= Tol2) {
      IsCoincide = Standard_True;
    }
  }

  if(IsCoincide) {
    Standard_Real t21, t22;
    t21 = ElCLib::Parameter(C2, P11);
    t22 = ElCLib::Parameter(C2, P12);

    if((t21 > myTmaxTo && t22 > myTmaxTo) || (t21 < myTminTo && t22 < myTminTo)) {
      return;
    }

    Standard_Real temp;
    if(t21 > t22) {
      temp = t21;
      t21 = t22;
      t22 = temp;
    }

    IntTools_CommonPrt aCommonPrt;
    aCommonPrt.SetEdge1(myCFrom.Edge());
    aCommonPrt.SetEdge2(myCTo.Edge());
    if(t21 >= myTminTo) {
      if(t22 <= myTmaxTo) {
	aCommonPrt.SetRange1(myTminFrom, myTmaxFrom);
	aCommonPrt.SetAllNullFlag(Standard_True);
	aCommonPrt.AppendRange2(t21, t22);
      }
      else {
	aCommonPrt.SetRange1(myTminFrom, myTmaxFrom - (t22 - myTmaxTo));
	aCommonPrt.AppendRange2(t21, myTmaxTo);
      }
    }
    else {
      aCommonPrt.SetRange1(myTminFrom + (myTminTo - t21), myTmaxFrom);
      aCommonPrt.AppendRange2(myTminTo, t22);
    }
    aCommonPrt.SetType(TopAbs_EDGE);  
    mySeqOfCommonPrts.Append(aCommonPrt);
    return;

  }

  if(IsParallel) {
    return;
  }
  //
  //modified by NIZNHY-PKV Tue Mar 29 08:29:14 2011f
  {
    TopoDS_Iterator aIt1, aIt2;
    //
    aIt1.Initialize(myEdge1);
    for (; aIt1.More(); aIt1.Next()) {
      const TopoDS_Shape& aV1=aIt1.Value();
      aIt2.Initialize(myEdge2);
      for (; aIt2.More(); aIt2.Next()) {
	const TopoDS_Shape& aV2=aIt2.Value();
	if (aV2.IsSame(aV1)) {
	  // the two straight lines have commpn vertex
	  return;
	}
      }
    }
  }
  //modified by NIZNHY-PKV Tue Mar 29 08:29:16 2011t
  //
  Standard_Real aSin2 = 1. - aCos*aCos;
  gp_Pnt O1 = C1.Location();
  gp_Pnt O2 = C2.Location();
  gp_Vec O1O2 (O1,O2);
  Standard_Real U2 = (D1.XYZ()*(O1O2.Dot(D1))-(O1O2.XYZ())).Dot(D2.XYZ());
  U2 /= aSin2;
  if(U2 < myTminTo || U2 > myTmaxTo) {
    return;
  }
  
  gp_Pnt P2(ElCLib::Value(U2,C2));
  Standard_Real U1 = (gp_Vec(O1,P2)).Dot(D1);
  if(U1 < myTminFrom || U1 > myTmaxFrom) {
    return;
  }

  gp_Pnt P1(ElCLib::Value(U1,C1));
  Standard_Real d2 = P1.SquareDistance(P2);

  if(d2 > Tol2) {
    return;
  }
  //
  IntTools_CommonPrt aCommonPrt;
  aCommonPrt.SetEdge1(myCFrom.Edge());
  aCommonPrt.SetEdge2(myCTo.Edge());
  aCommonPrt.SetRange1(U1 - myCriteria, U1 + myCriteria);
  aCommonPrt.AppendRange2(U2 - myCriteria, U2 + myCriteria);
  aCommonPrt.SetType(TopAbs_VERTEX);
  aCommonPrt.SetVertexParameter1(U1);
  aCommonPrt.SetVertexParameter2(U2);
  mySeqOfCommonPrts.Append(aCommonPrt);
  
}
/*
//=======================================================================
//function : CheckTouchVertex 
//purpose  : 
//=======================================================================
  Standard_Boolean IntTools_EdgeEdge::CheckTouchVertex (const IntTools_CommonPrt& aCP,
							Standard_Real& aTx1,
							Standard_Real& aTx2) const
{
  Standard_Real aTF1, aTL1, aTF2, aTL2, Tol, af, al, aDist, aDistNew;
  Standard_Real aTFR1, aTLR1, aTFR2, aTLR2;
  Standard_Boolean theflag=Standard_False;
  Standard_Integer iNb, aNbExt, i;

  aCP.Range1(aTFR1, aTLR1);
  (aCP.Ranges2())(1).Range(aTFR2, aTLR2);
  
  Tol = Precision::PConfusion();
  
  GeomAbs_CurveType aTFrom, aTTo;

  aTFrom=myCFrom.GetType();
  aTTo  =myCTo.GetType();
  gp_Circ aCirc;
  gp_Lin  aLine;

  if (aTFrom==GeomAbs_Circle) {
    aCirc=myCFrom.Circle();
    aLine=myCTo.Line();
  }
  else {
    aCirc=myCTo.Circle();
    aLine=myCFrom.Line();
  }
  
  Standard_Real aRadius;
  gp_Pnt aPCenter;
  aPCenter=aCirc.Location();
  aRadius =aCirc.Radius();
  
  aDist=aLine.Distance(aPCenter);
  aDist=fabs (aDist-aRadius);
  if (aDist > Tol) {
    return theflag;
  }
  //
  aTF1=myTminFrom;
  aTL1=myTmaxFrom;
  aTF2=myTminTo;
  aTL2=myTmaxTo;

  const Handle(Geom_Curve)&  Curve1   =BRep_Tool::Curve  (myCFrom.Edge(), af, al);
  const Handle(Geom_Curve)&  Curve2   =BRep_Tool::Curve  (myCTo.Edge()  , af, al);
  
  GeomAdaptor_Curve   TheCurve1   (Curve1, aTF1, aTL1);
  GeomAdaptor_Curve   TheCurve2   (Curve2, aTF2, aTL2);
				 
  Extrema_ExtCC anExtrema (TheCurve1, 
			   TheCurve2, 
			   aTF1-Tol, 
			   aTL1+Tol, 
			   aTF2-Tol, 
			   aTL2+Tol, 
			   Tol, Tol);
  
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
  //
  iNb=0;
  Standard_Real aTx1Av=0., aTx2Av=0.;
  
  gp_Pnt aPC1, aPC2;
  
  for (i=1; i<=aNbExt; ++i) {
    Extrema_POnCurv aPOnC1, aPOnC2;
    anExtrema.Points(i, aPOnC1, aPOnC2);
    
    aTx1=aPOnC1.Parameter();
    aTx2=aPOnC2.Parameter();
    if (fabs (aTx1-aTFR1) < 1.e-4 && fabs (aTx2-aTFR2) < 1.e-4) {
      aTx1Av=aTx1Av+aTx1;
      aTx2Av=aTx2Av+aTx2;
      iNb++;
    }
  }

  if (!iNb) {
    return theflag;
  }

  aTx1=aTx1Av/iNb;
  aTx2=aTx2Av/iNb;
  
  Curve1->D0(aTx1, aPC1);
  Curve2->D0(aTx2, aPC2);
  //
  aDistNew=aPC1.Distance(aPC2);
  if (aDistNew > aDist) {
    aTx1=0.5*(aTFR1+aTLR1);
    aTx2=0.5*(aTFR2+aTLR2);
    return !theflag;
  }
  //

  aDist=aDistNew;
  if (aDist > myCriteria) {
    return theflag;
  }
  
  if (fabs (aTx1-aTF1) < Tol) {
    return !theflag;
  }

  if (fabs (aTx1-aTL1) < Tol) {
    return !theflag;
  }

  if (aTx1 > (aTF1-Tol) && aTx1 < (aTL1+Tol) ) {
    return !theflag;
  }

  return theflag;
}

*/
