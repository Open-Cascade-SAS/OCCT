//
//
//
#include <IntTools_BeanBeanIntersector.ixx>

#include <IntTools_Root.hxx>
#include <Precision.hxx>
#include <Extrema_POnCurv.hxx>
#include <BRep_Tool.hxx>
#include <Geom_Curve.hxx>
#include <TColStd_Array1OfReal.hxx>
#include <TColStd_Array1OfInteger.hxx>
#include <IntTools_CArray1OfReal.hxx>
#include <gp_Lin.hxx>
#include <Intf_Array1OfLin.hxx>
#include <Bnd_Box.hxx>
#include <Extrema_ExtCC.hxx>
#include <Extrema_ExtElC.hxx>
#include <Extrema_LocateExtPC.hxx>
#include <gp_Circ.hxx>
#include <gp_Elips.hxx>
#include <IntTools.hxx>
#include <ElCLib.hxx>
#include <Geom_Line.hxx>
#include <GeomAdaptor_HCurve.hxx>

static void LocalPrepareArgs(BRepAdaptor_Curve& theCurve,
			     const Standard_Real      theFirstParameter,
			     const Standard_Real      theLastParameter,
			     Standard_Real&           theDeflection,
			     IntTools_CArray1OfReal&  theArgs);

static Standard_Boolean SetEmptyResultRange(const Standard_Real      theParameter, 
					    IntTools_MarkedRangeSet& theMarkedRange);

// ==================================================================================
// function: IntTools_BeanBeanIntersector
// purpose: 
// ==================================================================================
IntTools_BeanBeanIntersector::IntTools_BeanBeanIntersector() :

myFirstParameter1(0.),
myLastParameter1(0.),
myFirstParameter2(0.),
myLastParameter2(0.),
myBeanTolerance1(0.),
myBeanTolerance2(0.),
myCurveResolution1(0.),
myCriteria(0.),
myIsDone(Standard_False)
{
}

// ==================================================================================
// function: IntTools_BeanBeanIntersector
// purpose: 
// ==================================================================================
IntTools_BeanBeanIntersector::IntTools_BeanBeanIntersector(const TopoDS_Edge& theEdge1,
							   const TopoDS_Edge& theEdge2) :

myFirstParameter1(0.),
myLastParameter1(0.),
myFirstParameter2(0.),
myLastParameter2(0.),
myBeanTolerance1(0.),
myBeanTolerance2(0.),
myCurveResolution1(0.),
myCriteria(0.),
myIsDone(Standard_False)
{
  Init(theEdge1, theEdge2);
}

// ==================================================================================
// function: IntTools_BeanBeanIntersector
// purpose: 
// ==================================================================================
IntTools_BeanBeanIntersector::IntTools_BeanBeanIntersector(const BRepAdaptor_Curve& theCurve1,
							   const BRepAdaptor_Curve& theCurve2,
							   const Standard_Real      theBeanTolerance1,
							   const Standard_Real      theBeanTolerance2) :

myFirstParameter1(0.),
myLastParameter1(0.),
myFirstParameter2(0.),
myLastParameter2(0.),
myBeanTolerance1(0.),
myBeanTolerance2(0.),
myCurveResolution1(0.),
myCriteria(0.),
myIsDone(Standard_False)
{
  Init(theCurve1, theCurve2, theBeanTolerance1, theBeanTolerance2);
}

// ==================================================================================
// function: IntTools_BeanBeanIntersector
// purpose: 
// ==================================================================================
IntTools_BeanBeanIntersector::IntTools_BeanBeanIntersector(const BRepAdaptor_Curve& theCurve1,
							   const BRepAdaptor_Curve& theCurve2,
							   const Standard_Real       theFirstParOnCurve1,
							   const Standard_Real       theLastParOnCurve1,
							   const Standard_Real       theFirstParOnCurve2,
							   const Standard_Real       theLastParOnCurve2,
							   const Standard_Real       theBeanTolerance1,
							   const Standard_Real       theBeanTolerance2) :

myFirstParameter1(0.),
myLastParameter1(0.),
myFirstParameter2(0.),
myLastParameter2(0.),
myBeanTolerance1(0.),
myBeanTolerance2(0.),
myCurveResolution1(0.),
myCriteria(0.),
myIsDone(Standard_False)
{
  Init(theCurve1, theCurve2, theFirstParOnCurve1, theLastParOnCurve1, 
       theFirstParOnCurve2, theLastParOnCurve2, 
       theBeanTolerance1, theBeanTolerance2);
}

// ==================================================================================
// function: Init
// purpose: 
// ==================================================================================
void IntTools_BeanBeanIntersector::Init(const TopoDS_Edge& theEdge1,
					const TopoDS_Edge& theEdge2) 
{
  myCurve1.Initialize(theEdge1);
  myCurve2.Initialize(theEdge2);

  myTrsfCurve1 = Handle(Geom_Curve)::DownCast(myCurve1.Curve().Curve()->Transformed(myCurve1.Trsf()));
  myTrsfCurve2 = Handle(Geom_Curve)::DownCast(myCurve2.Curve().Curve()->Transformed(myCurve2.Trsf()));

  SetBeanParameters(Standard_True, myCurve1.FirstParameter(), myCurve1.LastParameter());
  SetBeanParameters(Standard_False, myCurve2.FirstParameter(), myCurve2.LastParameter());

  myBeanTolerance1 = BRep_Tool::Tolerance(theEdge1);
  myBeanTolerance2 = BRep_Tool::Tolerance(theEdge2);
  myCriteria = myBeanTolerance1 + myBeanTolerance2;
  myCurveResolution1 = myCurve1.Resolution(myCriteria);
}

// ==================================================================================
// function: Init
// purpose: 
// ==================================================================================
void IntTools_BeanBeanIntersector::Init(const BRepAdaptor_Curve& theCurve1,
					const BRepAdaptor_Curve& theCurve2,
					const Standard_Real      theBeanTolerance1,
					const Standard_Real      theBeanTolerance2) 
{
  myCurve1 = theCurve1;
  myCurve2 = theCurve2;

  SetBeanParameters(Standard_True, myCurve1.FirstParameter(), myCurve1.LastParameter());
  SetBeanParameters(Standard_False, myCurve2.FirstParameter(), myCurve2.LastParameter());

  myTrsfCurve1 = Handle(Geom_Curve)::DownCast(myCurve1.Curve().Curve()->Transformed(myCurve1.Trsf()));
  myTrsfCurve2 = Handle(Geom_Curve)::DownCast(myCurve2.Curve().Curve()->Transformed(myCurve2.Trsf()));

  myBeanTolerance1 = theBeanTolerance1;
  myBeanTolerance2 = theBeanTolerance2;
  myCriteria = myBeanTolerance1 + myBeanTolerance2;
  myCurveResolution1 = myCurve1.Resolution(myCriteria);
}

// ==================================================================================
// function: Init
// purpose: 
// ==================================================================================
void IntTools_BeanBeanIntersector::Init(const BRepAdaptor_Curve& theCurve1,
					const BRepAdaptor_Curve& theCurve2,
					const Standard_Real      theFirstParOnCurve1,
					const Standard_Real      theLastParOnCurve1,
					const Standard_Real      theFirstParOnCurve2,
					const Standard_Real      theLastParOnCurve2,
					const Standard_Real      theBeanTolerance1,
					const Standard_Real      theBeanTolerance2) 
{
  myCurve1 = theCurve1;
  myCurve2 = theCurve2;

  myTrsfCurve1 = Handle(Geom_Curve)::DownCast(myCurve1.Curve().Curve()->Transformed(myCurve1.Trsf()));
  myTrsfCurve2 = Handle(Geom_Curve)::DownCast(myCurve2.Curve().Curve()->Transformed(myCurve2.Trsf()));

  SetBeanParameters(Standard_True, theFirstParOnCurve1, theLastParOnCurve1);
  SetBeanParameters(Standard_False, theFirstParOnCurve2, theLastParOnCurve2);

  myBeanTolerance1 = theBeanTolerance1;
  myBeanTolerance2 = theBeanTolerance2;
  myCriteria = myBeanTolerance1 + myBeanTolerance2;
  myCurveResolution1 = myCurve1.Resolution(myCriteria);
}

// ==================================================================================
// function: SetBeanParameters
// purpose: 
// ==================================================================================
void IntTools_BeanBeanIntersector::SetBeanParameters(const Standard_Boolean IsFirstBean,
						     const Standard_Real    theFirstParOnCurve,
						     const Standard_Real    theLastParOnCurve) 
{
  if(IsFirstBean) {
    myFirstParameter1 = theFirstParOnCurve;
    myLastParameter1  = theLastParOnCurve;
  }
  else {
    myFirstParameter2 = theFirstParOnCurve;
    myLastParameter2  = theLastParOnCurve;
  }
}
// ==================================================================================
// function: Result
// purpose: 
// ==================================================================================
const IntTools_SequenceOfRanges& IntTools_BeanBeanIntersector::Result() const
{
  return myResults;
}

// ==================================================================================
// function: Result
// purpose: 
// ==================================================================================
 void IntTools_BeanBeanIntersector::Result(IntTools_SequenceOfRanges& theResults) const
{
  theResults = myResults;
}
// ==================================================================================
// function: Perform
// purpose: 
// ==================================================================================
void IntTools_BeanBeanIntersector::Perform() 
{
  Standard_Boolean bFastComputed;
  Standard_Integer k, i, iFlag, aNbRanges, aNbResults;
  Standard_Real aMidParameter, aCoeff, aParamDist, aPPC;
  Standard_Real aCriteria2, aD2;
  gp_Pnt aPi, aPh;
  IntTools_CArray1OfReal aParams;
  IntTools_Range aRange2, aRange;
  // 
  myIsDone = Standard_False;
  myResults.Clear();
  //
  LocalPrepareArgs(myCurve1, myFirstParameter1, myLastParameter1, myDeflection, aParams);
  //
  myRangeManager.SetRanges(aParams, 0);
  //
  aNbRanges=myRangeManager.Length();
  if(!aNbRanges) {
    return;
  }
  //
  bFastComputed=FastComputeIntersection();
  if(bFastComputed) {
    aRange.SetFirst(myFirstParameter1);
    aRange.SetLast (myLastParameter1);
    myResults.Append(aRange);
    myIsDone = Standard_True;
    return;
  }
  //
  ComputeRoughIntersection();
  
  //Standard_Real aMidParameter = (myFirstParameter2 + myLastParameter2) * 0.5;
  aCoeff=0.5753;
  aMidParameter = myFirstParameter2+(myLastParameter2-myFirstParameter2)*aCoeff;
  //
  for(k = 0; k < 2; ++k) {
    if(!k) {
      aRange2.SetFirst(myFirstParameter2);
      aRange2.SetLast(aMidParameter);
    }
    else {
      aRange2.SetFirst(aMidParameter);
      aRange2.SetLast(myLastParameter2);
    }
    
    ComputeUsingExtrema(aRange2);
    
    ComputeNearRangeBoundaries(aRange2);
  }
  //
  //  Legend iFlag
  //
  // 0 - just initialized 
  // 1 - non-intersected
  // 2 - roughly intersected
  // 3 - intersection is not done
  // 4 - coincided range
  //
  aPPC=Precision::PConfusion();
  aCriteria2=myCriteria*myCriteria;
  aNbRanges=myRangeManager.Length();
  //
  for(i=1; i<=aNbRanges; ++i) {
    iFlag=myRangeManager.Flag(i);
    //
    if(iFlag==4) {
      aRange=myRangeManager.Range(i);
      aNbResults=myResults.Length();
      if(aNbResults>0) {
	const IntTools_Range& aLastRange = myResults.Last();
	//
	aParamDist = Abs(aRange.First() - aLastRange.Last());
	if(aParamDist > myCurveResolution1) {
	  myResults.Append(aRange);
	}
	else {
	  aPi=myCurve1.Value(aRange.First());
	  aPh=myCurve1.Value(aLastRange.Last());
	  aD2=aPi.SquareDistance(aPh);
	  if(aParamDist<aPPC || aD2<aCriteria2) { 
	    myResults.ChangeValue(aNbResults).SetLast(aRange.Last());
	  }
	  else {
	    myResults.Append(aRange);
	  }
	}
      }// if(aNbR>0) {
      else {
	myResults.Append(aRange);
      }
    } //if(iFlag==4) {
  } // for(i = 1; i <= myRangeManager.Length(); i++) {
  myIsDone = Standard_True;
}
// ==================================================================================
// function: FastComputeIntersection
// purpose: 
// ==================================================================================
Standard_Boolean IntTools_BeanBeanIntersector::FastComputeIntersection() 
{
  Standard_Boolean aresult;
  GeomAbs_CurveType aCT1, aCT2;
  //
  aresult = Standard_False;
  //
  aCT1=myCurve1.GetType();
  aCT2=myCurve2.GetType();
  //
  if(aCT1 != aCT2) {
    return aresult;
  }
  //
  // Line
  if(aCT1==GeomAbs_Line) {
    Standard_Real par1, par2;
    
    if((Distance(myFirstParameter1, par1) < myCriteria) &&
       (Distance(myLastParameter1, par2) < myCriteria)) {

      if((par1  >= myFirstParameter2) && (par1 <= myLastParameter2) &&
	 (par2  >= myFirstParameter2) && (par2 <= myLastParameter2)) {
	myRangeManager.InsertRange(myFirstParameter1, myLastParameter1, 4);
	aresult = Standard_True;
      }
    }
    return aresult;
  } 
  //
  // Circle
  if(aCT1==GeomAbs_Circle) {
    Standard_Real anAngle, aPA, aDistLoc, aDist, aDiff, aR1, aR2;
    gp_Circ aCirc1, aCirc2;
    gp_Dir aDir1, aDir2;
    //
    aCirc1=myCurve1.Circle();
    aCirc2=myCurve2.Circle();
    aR1=aCirc1.Radius();
    aR2=aCirc2.Radius();
    //
    aPA=Precision::Angular();
    aDir1 = aCirc1.Axis().Direction();
    aDir2 = aCirc2.Axis().Direction();
    //
    anAngle = aDir1.Angle(aDir2);
    if(anAngle > aPA) {
      return aresult; //->
    }
    //
    const gp_Pnt& aPLoc1=aCirc1.Location();
    const gp_Pnt& aPLoc2=aCirc2.Location();
    aDistLoc = aPLoc1.Distance(aPLoc2);
    aDist=aDistLoc;
    aDiff=aR1 - aR2;
    aDist+=Abs(aDiff);
    if(Abs(aDist) > myCriteria) {
      return aresult; //->
    }
    //
    Standard_Real aSinPA, atmpvalue, aprojectedradius;
    //
    aSinPA=sin(aPA);
    atmpvalue = aR1*aSinPA;
    atmpvalue *= atmpvalue;
    aprojectedradius = sqrt(aR1*aR1 - atmpvalue);

    aDiff = aprojectedradius - aR2;
    aDist = aDistLoc + sqrt((aDiff * aDiff) + atmpvalue);
    if(Abs(aDist) > myCriteria) {
      return aresult; //->
    }
    //
    Standard_Boolean newparfound;
    Standard_Integer i;
    Standard_Real afirstpar, alastpar, par1, par2, apar;
    //
    afirstpar = myFirstParameter1;
    alastpar  = myLastParameter1;
    
    for(i = 0; i < 2; i++) {
      if((Distance(afirstpar, par1) < myCriteria) &&
	 (Distance(alastpar , par2) < myCriteria)) {
	
	if(i || Distance((myFirstParameter1 + myLastParameter2) * 0.5, apar) < myCriteria) {
	  myRangeManager.InsertRange(afirstpar, alastpar, 4);
	  
	  if(!i) {
	    aresult = Standard_True;
	  }
	}
	break;
      }
      //
      if(i) {
	break;
      }
      // i=0 :
      newparfound = Standard_False;
      
      if(Distance((myFirstParameter1 + myLastParameter2) * 0.5, apar) < myCriteria) {
	afirstpar = myFirstParameter1 + myCriteria;
	alastpar  = myLastParameter1  - myCriteria;
	newparfound = Standard_True;
	
	if(alastpar <= afirstpar) {
	  newparfound = Standard_False;
	}
      }
      
      if(!newparfound) {
	break;
      }
    }// for(i = 0; i < 2; i++) {
  } // if(aCT1==GeomAbs_Circle)
  return aresult;
}
// ==================================================================================
// function: Distance
// purpose: 
// ==================================================================================
Standard_Real IntTools_BeanBeanIntersector::Distance(const Standard_Real theArg,
						     Standard_Real&      theArgOnOtherBean) 
{
  Standard_Real aDistance;
  Standard_Integer aNbPoints;
  gp_Pnt aPoint;
  //
  aDistance=RealLast();
  //
  aPoint=myCurve1.Value(theArg);
  //modified by NIZNHY-PKV Thu Jul 01 12:20:52 2010f
  myProjector.Init(myTrsfCurve2, myFirstParameter2, myLastParameter2);
  myProjector.Perform(aPoint);
  //myProjector.Init(aPoint, myTrsfCurve2, myFirstParameter2, myLastParameter2);
  //modified by NIZNHY-PKV Thu Jul 01 12:21:05 2010t
  //
  aNbPoints=myProjector.NbPoints();
  if(aNbPoints > 0) {
    theArgOnOtherBean = myProjector.LowerDistanceParameter();
    aDistance=myProjector.LowerDistance();
  }
  //
  else {
    Standard_Real aDistance1, aDistance2;
    //
    aDistance1 = aPoint.Distance(myCurve2.Value(myFirstParameter2));
    aDistance2 = aPoint.Distance(myCurve2.Value(myLastParameter2));
    //
    theArgOnOtherBean = myLastParameter2;
    aDistance=aDistance2;  
    if(aDistance1 < aDistance2) {
      theArgOnOtherBean = myFirstParameter2;
      aDistance=aDistance1;
    }
  }
  return aDistance;
}
// ==================================================================================
// function: ComputeRoughIntersection
// purpose: 
// ==================================================================================
void IntTools_BeanBeanIntersector::ComputeRoughIntersection() 
{
  Standard_Boolean isintersection;
  Standard_Integer i, aNbArgs, aNbArgs1, aNbRanges, j, aNbLines, k, pIt, extIt, iFlag;
  Standard_Real aDeflection, aGPR, aCurDeflection, aT1, aT2, aD;
  Standard_Real aMaxDistance, aDistance, aPPC, aPrm1, aPrm2, aPPA;
  GeomAbs_CurveType aCT1, aCT2;
  gp_Pnt aPoint1, aPoint2, aMidPOnCurve, aMidPOnLine, aP1, aP2;
  IntTools_CArray1OfReal anArgs;
  LocalPrepareArgs(myCurve2, myFirstParameter2, myLastParameter2, aDeflection, anArgs);
  //
  aNbArgs=anArgs.Length();
  if(!aNbArgs) {
    return;
  }
  //
  aCT1=myCurve1.GetType();
  aCT2=myCurve2.GetType();
  aGPR=gp::Resolution();
  aPPC=Precision::PConfusion();
  aPPA=Precision::Angular();
  aNbArgs1=aNbArgs-1;
  //
  Intf_Array1OfLin aLines(1, aNbArgs1);
  TColStd_Array1OfInteger aLineFlags(1, aNbArgs1);
  TColStd_Array1OfReal aDistances(1, aNbArgs1);
  //
  aT1=anArgs(0);
  aPoint1 = myCurve2.Value(aT1);
  for(i=1; i<aNbArgs; ++i) {
    aT2=anArgs(i);
    aPoint2 = myCurve2.Value(aT2);
    gp_Vec aVec(aPoint1, aPoint2);
    aD=aVec.Magnitude();
    aDistances.SetValue(i, aD);
    //
    if(aD<=aGPR) {
      aLineFlags.SetValue(i, 0);
    }
    else {
      aLineFlags.SetValue(i, 1);
      gp_Lin aLine(aPoint1, gp_Dir(aVec));
      aLines.SetValue(i, aLine);
      //
      if((aCT2 == GeomAbs_BezierCurve) ||
	 (aCT2 == GeomAbs_BSplineCurve)) {
	aMidPOnCurve = myCurve2.Value((aT1+aT2) * 0.5);
	aMidPOnLine = ElCLib::Value((aDistances(i)*0.5), aLine);
	aCurDeflection = aMidPOnCurve.Distance(aMidPOnLine);
	if(aCurDeflection > aDeflection) {
	  aDeflection = aCurDeflection;
	}
      }
    }
    aT1=aT2;
    aPoint1 = aPoint2;
  }
  //
  aNbLines=aLines.Upper();
  aMaxDistance = myCriteria + myDeflection + aDeflection;
  
  aT1=myRangeManager.Range(1).First();
  aPoint1 = myCurve1.Value(aT1);
  aNbRanges=myRangeManager.Length();
  for(i = 1; i <= aNbRanges; ++i) {
    const IntTools_Range& aRange = myRangeManager.Range(i);
    aT2=aRange.Last();
    aPoint2 = myCurve1.Value(aT2);
    //
    iFlag=myRangeManager.Flag(i);
    if(iFlag==4) {// coincided
      aT1=aT2;
      aPoint1 = aPoint2;
      continue;
    }
    //
    myRangeManager.SetFlag(i, 1); // not intersected
    
    Bnd_Box aBox1;
    aBox1.Add(aPoint1);
    aBox1.Add(aPoint2);
    aBox1.Enlarge(myBeanTolerance1 + myDeflection);

    gp_Vec aVec(aPoint1, aPoint2);

    aDistance=aVec.Magnitude();
    if(aDistance <= aGPR) {
      myRangeManager.SetFlag(i, 0);
      continue;
    }
    //
    gp_Lin aLine(aPoint1, gp_Dir(aVec));
    //
    if((aCT1 == GeomAbs_BezierCurve) ||
       (aCT1 == GeomAbs_BSplineCurve)) {
      aMidPOnCurve = myCurve1.Value((aRange.First() + aRange.Last()) * 0.5);
      aMidPOnLine = ElCLib::Value((aDistance*0.5), aLine);
      aCurDeflection = aMidPOnCurve.Distance(aMidPOnLine);
      if(myDeflection < aCurDeflection) {
	aMaxDistance += aCurDeflection - myDeflection;
	myDeflection = aCurDeflection;	
      }
    }
    //
    for(j=1; j<=aNbLines; ++j) {
      if(!aLineFlags(j)) {
	myRangeManager.SetFlag(i, 0);
	continue;
      }
      //
      const gp_Lin& aL2=aLines(j);
      //Handle(Geom_Line) aC2=new Geom_Line(aL2); // DEB ft
      aD=aDistances(j);
      aP1=ElCLib::Value(0., aL2);
      aP2=ElCLib::Value(aD, aL2);
      //
      Extrema_ExtElC anExtrema(aLine, aL2, aPPA);
      if(anExtrema.IsDone() && (anExtrema.IsParallel() || (anExtrema.NbExt() > 0))) {
	isintersection = Standard_False;

	if(anExtrema.IsParallel()) {
	  isintersection = (anExtrema.SquareDistance(1) < aMaxDistance * aMaxDistance);
	}
	else { //1
	  for(k = 1; !isintersection && k <= anExtrema.NbExt(); ++k) {
	    if(anExtrema.SquareDistance(k) < aMaxDistance * aMaxDistance) {
	      Extrema_POnCurv P1, P2;
	      anExtrema.Points(k, P1, P2);
	      aPrm1=P1.Parameter();
	      aPrm2=P2.Parameter();
	      if((aPrm1 >= -aMaxDistance) && (aPrm1 <= aDistance+aMaxDistance) &&
		 (aPrm2 >= -aMaxDistance) && (aPrm2 <= aD+aMaxDistance)) {
		isintersection = Standard_True;
	      }
	      else { // 2
		Extrema_ExtPElC aPointProjector;

		for(pIt = 0; !isintersection && (pIt < 4); ++pIt) {
		  switch (pIt) {
		    case 0: {
		      aPointProjector = 
			//Extrema_ExtPElC(aPoint1, aLines(j), aPPC, 0., aDistances(j));
			Extrema_ExtPElC(aPoint1, aL2, aPPC, -aMaxDistance, aD+aMaxDistance);
		      break;
		    }
		    case 1: {
		      aPointProjector =
			//Extrema_ExtPElC(aPoint2, aLines(j), aPPC, 0., aD);
			Extrema_ExtPElC(aPoint2, aL2, aPPC, -aMaxDistance, aD+aMaxDistance);
		      break;
		    }
		    case 2: {
		      aPointProjector = 
			//Extrema_ExtPElC(ElCLib::Value(0., aLines(j)), aLine, aPPC, 0., aDistance);
			Extrema_ExtPElC(aP1, aLine, aPPC, -aMaxDistance, aDistance+aMaxDistance);
		      break;
		    }
		    case 3: {
		      aPointProjector =
			//Extrema_ExtPElC(ElCLib::Value(aDistances(j), aLines(j)), aLine, aPPC, 0., aDistance);
			Extrema_ExtPElC(aP2, aLine, aPPC, -aMaxDistance, aDistance+aMaxDistance);
		      break;
		    }
		    default: {
		      break;
		    }
		  }
		  //
		  if(aPointProjector.IsDone()) {
            Standard_Real aMaxDistance2 = aMaxDistance * aMaxDistance;
		    for(extIt = 1; extIt <= aPointProjector.NbExt(); extIt++) {
		      if(aPointProjector.SquareDistance(extIt) < aMaxDistance2) {
			isintersection = Standard_True;
		      }
		    }
		  }
		} // end for
	      }// else { // 2
	    }//if(anExtrema.Value(k) < aMaxDistance) {
	  }//for(k = 1; !isintersection && k <= anExtrema.NbExt(); k++) {
	}//else { //1
	
	if(isintersection) {
	  myRangeManager.SetFlag(i, 2); // roughly intersected
	  break;
	}
      }//if(anExtrema.IsDone() && (anExtrema.IsParallel() || (anExtrema.NbExt() > 0))) {
      else {
	Bnd_Box aBox2;
	aBox2.Add(myCurve2.Value(anArgs(j-1)));
	aBox2.Add(myCurve2.Value(anArgs(j)));
	aBox2.Enlarge(myBeanTolerance2 + aDeflection);
	//
	if(!aBox1.IsOut(aBox2)) {
	  myRangeManager.SetFlag(i, 2); // roughly intersected
	  break;
	}
      }
    }
    aT1=aT2;
    aPoint1 = aPoint2;
  }
}

// ==================================================================================
// function: ComputeUsingExtrema
// purpose: 
// ==================================================================================
void IntTools_BeanBeanIntersector::ComputeUsingExtrema(const IntTools_Range& theRange2) 
{
  //rln Dec 2008.
  //Extrema_ExtCC is reused throughout this method to store caches computed on
  //theRange2. However it is actually used only in a few calls of
  //ComputeUsingExtrema(), so using a default constructor would add unnecessary overhead
  //of initialization its internal fields. Since it is not manipulated by Handle then
  //we will use a pointer to it and initialize it only when needed.
  Extrema_ExtCC *apExtrema = 0; 
  Handle(GeomAdaptor_HCurve) aHCurve1, aHCurve2; //will be initialized later, only if needed
  //handles are used to guard pointers to GeomAdaptor_Curve inside Extrema
  Standard_Real aCriteria2 = myCriteria * myCriteria;
  for(Standard_Integer i = 1; i <= myRangeManager.Length(); i++) {

    if(myRangeManager.Flag(i) == 2 || myRangeManager.Flag(i) == 0) {
      const IntTools_Range& aParamRange = myRangeManager.Range(i);
      
      if(aParamRange.Last() - aParamRange.First() < Precision::PConfusion()) {

	if(((i > 1) && (myRangeManager.Flag(i-1) == 4)) ||
	   ((i <  myRangeManager.Length()) && (myRangeManager.Flag(i+1) == 4))) {
	  myRangeManager.SetFlag(i, 4);
	  continue;
	}
      }
      if (aHCurve2.IsNull()) {
        //initialize only once 
        apExtrema = new Extrema_ExtCC;
        Standard_Real ftmp = theRange2.First() - Precision::PConfusion();
        Standard_Real ltmp = theRange2.Last()  + Precision::PConfusion();
        ftmp = (ftmp < myFirstParameter2) ? myFirstParameter2 : ftmp;
        ltmp = (ltmp > myLastParameter2)  ? myLastParameter2  : ltmp;
        aHCurve2 = new GeomAdaptor_HCurve (myTrsfCurve2, ftmp, ltmp);
        apExtrema->SetCurve (2, aHCurve2->Curve(), theRange2.First(), theRange2.Last());
      }
      Extrema_ExtCC& anExtrema = *apExtrema;

      Standard_Real ftmp = aParamRange.First() - Precision::PConfusion();
      Standard_Real ltmp = aParamRange.Last()  + Precision::PConfusion();
      ftmp = (ftmp < myFirstParameter1) ? myFirstParameter1 : ftmp;
      ltmp = (ltmp > myLastParameter1)  ? myLastParameter1  : ltmp;
      aHCurve1 = new GeomAdaptor_HCurve (myTrsfCurve1, ftmp, ltmp);
      anExtrema.SetCurve (1, aHCurve1->Curve(), aParamRange.First(), aParamRange.Last());

      anExtrema.Perform();

      if(anExtrema.IsDone() && (anExtrema.IsParallel() || (anExtrema.NbExt() > 0))) {
	Standard_Integer anOldNbRanges = myRangeManager.Length();

	if (anExtrema.IsParallel()) {
	  if(anExtrema.SquareDistance(1) < aCriteria2) {
	    Standard_Real theParameter1, theParameter2;
	    Standard_Real adistance1 = Distance(aParamRange.First(), theParameter1);
	    Standard_Real adistance2 = Distance(aParamRange.Last(),  theParameter2);
	    Standard_Boolean validdistance1 = (adistance1 < myCriteria);
	    Standard_Boolean validdistance2 = (adistance2 < myCriteria);

	    if (validdistance1 && validdistance2) {
	      myRangeManager.InsertRange(aParamRange.First(), aParamRange.Last(), 4);
	      continue;
	    }
	    else {
	      if(validdistance1) {
		ComputeRangeFromStartPoint(Standard_True, aParamRange.First(), i, theParameter1, theRange2);
	      }
	      else {
		if(validdistance2) {
		  ComputeRangeFromStartPoint(Standard_False, aParamRange.Last(), i, theParameter2, theRange2);
		}
		else {
		  Standard_Real a  = aParamRange.First();
		  Standard_Real b  = aParamRange.Last();
		  Standard_Real da = adistance1;
		  Standard_Real db = adistance2;
		  Standard_Real asolution = a;
		  Standard_Boolean found = Standard_False;
		  
		  while(((b - a) > myCurveResolution1) && !found) {
		    asolution = (a+b)*0.5;
		    Standard_Real adist = Distance(asolution, theParameter1);
		    
		    if(adist < myCriteria) {
		      found = Standard_True;
		    }
		    else {
		      if(da < db) {
			b = asolution;
			db = adist;
		      }
		      else {
			a = asolution;
			da = adist;
		      }
		    }
		  } // end while

		  if(found) {
		    ComputeRangeFromStartPoint(Standard_False, asolution, i, theParameter1, theRange2);
		    ComputeRangeFromStartPoint(Standard_True,  asolution, i, theParameter1, theRange2);
		  }
		  else {
		    myRangeManager.SetFlag(i, 2);
		  }
		}
	      }
	    }
	  }
	}
	else {

	  for(Standard_Integer j = 1 ; j <= anExtrema.NbExt(); j++) {
	    if(anExtrema.SquareDistance(j) < aCriteria2) {
	      Extrema_POnCurv p1, p2;
	      anExtrema.Points(j, p1, p2);

	      Standard_Integer aNbRanges = myRangeManager.Length();

	      Standard_Integer anIndex = myRangeManager.GetIndex(p1.Parameter(), Standard_False);
	      if(anIndex > 0) {
		ComputeRangeFromStartPoint(Standard_False, p1.Parameter(), anIndex, p2.Parameter(), theRange2);
	      }

	      anIndex = myRangeManager.GetIndex(p1.Parameter(), Standard_True);

	      if(anIndex > 0) {
		ComputeRangeFromStartPoint(Standard_True, p1.Parameter(), anIndex, p2.Parameter(), theRange2);
	      }

	      if(aNbRanges == myRangeManager.Length()) {
		SetEmptyResultRange(p1.Parameter(), myRangeManager);
	      }
	    }
	  } // end for
	}
	Standard_Integer adifference = myRangeManager.Length() - anOldNbRanges;

	if(adifference > 0) {
	  i+=adifference;
	}
      }
      else {
	myRangeManager.SetFlag(i, 3); // intersection not done.
      }
    }
  }
  if (apExtrema) delete apExtrema;
}

// ==================================================================================
// function: ComputeNearRangeBoundaries
// purpose: 
// ==================================================================================
void IntTools_BeanBeanIntersector::ComputeNearRangeBoundaries(const IntTools_Range& theRange2) 
{
  Standard_Real theParameter = theRange2.First();

  for(Standard_Integer i = 1; i <= myRangeManager.Length(); i++) {
    if(myRangeManager.Flag(i) != 3)
      continue;

    if((i > 1) && ((myRangeManager.Flag(i-1) == 1) || (myRangeManager.Flag(i-1) == 4))) {
      myRangeManager.SetFlag(i, 2);
      continue;
    }

    const IntTools_Range& aParamRange = myRangeManager.Range(i);
    
    if(Distance(aParamRange.First(), theParameter) < myCriteria) {
      Standard_Integer aNbRanges = myRangeManager.Length();
      
      if(i > 1) {
	ComputeRangeFromStartPoint(Standard_False, aParamRange.First(), i-1, theParameter, theRange2);
      }
      ComputeRangeFromStartPoint(Standard_True, aParamRange.First(), i + (myRangeManager.Length() - aNbRanges), theParameter, theRange2);

      if(aNbRanges == myRangeManager.Length()) {
	SetEmptyResultRange(aParamRange.First(), myRangeManager);
      }
    }
    else {
      myRangeManager.SetFlag(i, 2);
    }
  }

  if((myRangeManager.Flag(myRangeManager.Length()) == 3) || 
     (myRangeManager.Flag(myRangeManager.Length()) == 2)) {
    const IntTools_Range& aParamRange = myRangeManager.Range(myRangeManager.Length());
    
    if(Distance(aParamRange.Last(), theParameter) < myCriteria) {
      Standard_Integer aNbRanges = myRangeManager.Length();
      myRangeManager.SetFlag(myRangeManager.Length(), 2);      

      ComputeRangeFromStartPoint(Standard_False, aParamRange.Last(), myRangeManager.Length(), theParameter, theRange2);
      
      if(aNbRanges == myRangeManager.Length()) {
	SetEmptyResultRange(aParamRange.Last(), myRangeManager);
      }
    }
    else {
      myRangeManager.SetFlag(myRangeManager.Length(), 2);
    }
  }
}

// ==================================================================================
// function: ComputeRangeFromStartPoint
// purpose: 
// ==================================================================================
void IntTools_BeanBeanIntersector::ComputeRangeFromStartPoint(const Standard_Boolean ToIncreaseParameter,
							      const Standard_Real    theParameter,
							      const Standard_Integer theIndex,
							      const Standard_Real    theParameter2,
							      const IntTools_Range&  theRange2) 
{

  if(myRangeManager.Flag(theIndex) == 4 ||
     myRangeManager.Flag(theIndex) == 1)
    return;

  Standard_Integer aValidIndex = theIndex;
  Standard_Real aMinDelta        = myCurveResolution1 * 0.5;
  Standard_Real aDeltaRestrictor = myLastParameter1 - myFirstParameter1;

  if(Abs(aDeltaRestrictor) < Precision::PConfusion()) {
    return;
  }

  if(aMinDelta > aDeltaRestrictor) {
    aMinDelta = aDeltaRestrictor;
  }
  Standard_Real tenOfMinDelta    = aMinDelta * 10.;
  Standard_Real aDelta           = myCurveResolution1;
  Standard_Real aCurPar  = (ToIncreaseParameter) ? (theParameter + aDelta) : (theParameter - aDelta);
  Standard_Real aPrevPar = theParameter;
  IntTools_Range aCurrentRange = myRangeManager.Range(aValidIndex);

  Standard_Boolean BoundaryCondition  = (ToIncreaseParameter) ? (aCurPar > aCurrentRange.Last()) : (aCurPar < aCurrentRange.First());
  
  if(BoundaryCondition) {
    aCurPar = (ToIncreaseParameter) ? aCurrentRange.Last() : aCurrentRange.First();
    BoundaryCondition = Standard_False;
  }

  Standard_Integer loopcounter = 0; // neccesary to have no infinite loop
  Standard_Real aParameter = theParameter2;
  Standard_Boolean anotherSolutionFound = Standard_False;

  Standard_Boolean isboundaryindex = Standard_False;
  Standard_Boolean isvalidindex = Standard_True;

  Standard_Real aCriteria2 = myCriteria * myCriteria;

  while((aDelta >= aMinDelta) && (loopcounter <= 10)) {
    Standard_Boolean pointfound = Standard_False;

    gp_Pnt aPoint = myCurve1.Value(aCurPar);
    GeomAdaptor_Curve aCurve2(myTrsfCurve2, theRange2.First(), theRange2.Last());

    Extrema_LocateExtPC anExtrema(aPoint, aCurve2, aParameter, theRange2.First(), theRange2.Last(), 1.e-10);

    if(anExtrema.IsDone()) {
      if(anExtrema.SquareDistance() < aCriteria2) {
	Extrema_POnCurv aPOnCurv = anExtrema.Point();
	aParameter = aPOnCurv.Parameter();
	pointfound = Standard_True;
      }
    }
    else {
      //       pointfound = (Distance(aCurPar, aParameter) < myCriteria);
      Standard_Real afoundparam = aParameter;

      if(Distance(aCurPar, afoundparam) < myCriteria) {
	aParameter = afoundparam;
	pointfound = Standard_True;
      }
    }

    if(pointfound) {
      aPrevPar = aCurPar;
      anotherSolutionFound = Standard_True;

      if(BoundaryCondition && (isboundaryindex || !isvalidindex))
	break;
    }
    else {
      aDeltaRestrictor = aDelta;
    }

    // if pointfound decide to increase aDelta using derivative of distance function
    //

    aDelta = (pointfound) ? (aDelta * 2.) : (aDelta * 0.5);
    aDelta = (aDelta < aDeltaRestrictor) ? aDelta : aDeltaRestrictor;
    aCurPar = (ToIncreaseParameter) ? (aPrevPar + aDelta) : (aPrevPar - aDelta);

    BoundaryCondition  = (ToIncreaseParameter) ? (aCurPar > aCurrentRange.Last()) : (aCurPar < aCurrentRange.First());

    isboundaryindex = Standard_False;
    isvalidindex = Standard_True;

    if(BoundaryCondition) {
      isboundaryindex = ((!ToIncreaseParameter && (aValidIndex == 1)) ||
			 (ToIncreaseParameter && (aValidIndex == myRangeManager.Length())));

      if(!isboundaryindex) {
	if(pointfound) {
	  Standard_Integer aFlag = (ToIncreaseParameter) ? myRangeManager.Flag(aValidIndex + 1) : myRangeManager.Flag(aValidIndex - 1);
	  
	  if(aFlag != 1 && aFlag != 4) {
	    aValidIndex = (ToIncreaseParameter) ? (aValidIndex + 1) : (aValidIndex - 1);
	    aCurrentRange = myRangeManager.Range(aValidIndex);

	    if((ToIncreaseParameter && (aCurPar > aCurrentRange.Last())) ||
	       (!ToIncreaseParameter && (aCurPar < aCurrentRange.First()))) {
	      aCurPar = (aCurrentRange.First() + aCurrentRange.Last()) * 0.5;
	      aDelta*=0.5;
	    }
	  }
	  else {
	    isvalidindex = Standard_False;
	    aCurPar = (ToIncreaseParameter) ? aCurrentRange.Last() : aCurrentRange.First();
	  }
	}
      }
      else {
	aCurPar = (ToIncreaseParameter) ? aCurrentRange.Last() : aCurrentRange.First();
      }

      if(aDelta < tenOfMinDelta) {
	loopcounter++;
      }
      else {
	loopcounter = 0;
      }
    } // end if(BoundaryCondition)
  }

  if(anotherSolutionFound) {
    if(ToIncreaseParameter)
      myRangeManager.InsertRange(theParameter, aPrevPar, 4);
    else
      myRangeManager.InsertRange(aPrevPar, theParameter, 4);
  }
}

// ---------------------------------------------------------------------------------
// static function: LocalPrepareArgs
// purpose: 
// ---------------------------------------------------------------------------------
static void LocalPrepareArgs(BRepAdaptor_Curve& theCurve,
			     const Standard_Real      theFirstParameter,
			     const Standard_Real      theLastParameter,
			     Standard_Real&           theDeflection,
			     IntTools_CArray1OfReal&  theArgs) {
  Standard_Integer aDiscretization = 30;
  Standard_Real aRelativeDeflection = 0.01;
  theDeflection = aRelativeDeflection;
  Standard_Boolean prepareargs = Standard_True;
  
  switch(theCurve.GetType()) {
  case GeomAbs_Line: {
    prepareargs = Standard_False;
    aDiscretization = 3;
    theArgs.Append(theFirstParameter);

    if((theLastParameter - theFirstParameter) > Precision::PConfusion()) {
      theArgs.Append((theFirstParameter + theLastParameter)*0.5);
    }
    theArgs.Append(theLastParameter);
    theDeflection = Precision::Confusion();
    break;
  }
  case GeomAbs_Circle: {
    aDiscretization = 23;
    theDeflection = aRelativeDeflection * theCurve.Circle().Radius();
    break;
  }
  case GeomAbs_Ellipse: {
    aDiscretization = 40;
    theDeflection = 2 * aRelativeDeflection * theCurve.Ellipse().MajorRadius();
    break;
  }
  case GeomAbs_Hyperbola:
  case GeomAbs_Parabola: {
    aDiscretization = 40;
    theDeflection = aRelativeDeflection;
    break;
  }
  case GeomAbs_BezierCurve: {
    aDiscretization = 30;
    theDeflection = aRelativeDeflection;
    break;
  }
  case GeomAbs_BSplineCurve: {
    aDiscretization = 30;
    theDeflection = aRelativeDeflection;
    break;
  }
  default: {
    aDiscretization = 30;
    theDeflection = aRelativeDeflection;
  }
  }

  if(prepareargs) {
    IntTools::PrepareArgs(theCurve, theLastParameter, theFirstParameter, aDiscretization, aRelativeDeflection, theArgs);
  }
}

// ---------------------------------------------------------------------------------
// static function: SetEmptyResultRange
// purpose:  
// ---------------------------------------------------------------------------------
static Standard_Boolean SetEmptyResultRange(const Standard_Real      theParameter, 
					    IntTools_MarkedRangeSet& theMarkedRange) {

  const TColStd_SequenceOfInteger& anIndices = theMarkedRange.GetIndices(theParameter);
  Standard_Boolean add = (anIndices.Length() > 0);
  
  for(Standard_Integer k = 1; k <= anIndices.Length(); k++) {
    if(theMarkedRange.Flag(anIndices(k)) == 4) {
      add = Standard_False;
      break;
    }
  }
  
  if(add) {
    theMarkedRange.InsertRange(theParameter, theParameter, 4);
  }
  
  return add;
}

