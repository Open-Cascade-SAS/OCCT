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

#include <ChFi2d_FilletAlgo.hxx>

#include <GeomProjLib.hxx>
#include <BRep_Tool.hxx>
#include <Precision.hxx>
#include <ElSLib.hxx>
#include <ElCLib.hxx>

#include <Geom2dAPI_ProjectPointOnCurve.hxx>
#include <GeomAPI_ProjectPointOnCurve.hxx>
#include <Geom2dAPI_InterCurveCurve.hxx>

#include <TopoDS.hxx>
#include <TopoDS_Iterator.hxx>
#include <TColStd_ListIteratorOfListOfReal.hxx>

#include <gp_Circ.hxx>
#include <Geom_Circle.hxx>
#include <Geom2d_Line.hxx>

#include <BRepBuilderAPI_MakeEdge.hxx>
#include <BRepAdaptor_Curve.hxx>

ChFi2d_FilletAlgo::ChFi2d_FilletAlgo()
: myStart1(0.0),
  myEnd1  (0.0),
  myStart2(0.0),
  myEnd2  (0.0),
  myRadius(0.0),
  myStartSide    (Standard_False),
  myEdgesExchnged(Standard_False),
  myDegreeOfRecursion(0)
{
}

ChFi2d_FilletAlgo::ChFi2d_FilletAlgo(const TopoDS_Wire& theWire, const gp_Pln& thePlane)
: myStart1(0.0),
  myEnd1  (0.0),
  myStart2(0.0),
  myEnd2  (0.0),
  myRadius(0.0),
  myStartSide    (Standard_False),
  myEdgesExchnged(Standard_False),
  myDegreeOfRecursion(0)
{
  Init(theWire, thePlane);
}

ChFi2d_FilletAlgo::ChFi2d_FilletAlgo(const TopoDS_Edge& theEdge1, 
                                     const TopoDS_Edge& theEdge2, 
                                     const gp_Pln& thePlane) 
: myEdge1(theEdge1),
  myEdge2(theEdge2),
  myStart1(0.0),
  myEnd1  (0.0),
  myStart2(0.0),
  myEnd2  (0.0),
  myRadius(0.0),
  myStartSide    (Standard_False),
  myEdgesExchnged(Standard_False),
  myDegreeOfRecursion(0)
{
  Init(theEdge1, theEdge2, thePlane);
}

void ChFi2d_FilletAlgo::Init(const TopoDS_Wire& theWire, const gp_Pln& thePlane) 
{
  TopoDS_Edge theEdge1, theEdge2;
  TopoDS_Iterator itr(theWire);
  for (; itr.More(); itr.Next())
  {
    if (theEdge1.IsNull())
      theEdge1 = TopoDS::Edge(itr.Value());
    else if (theEdge2.IsNull())
      theEdge2 = TopoDS::Edge(itr.Value());
    else
      break;
  }
  if (theEdge1.IsNull() || theEdge2.IsNull())
    throw Standard_ConstructionError("The fillet algorithms expects a wire consisting of two edges.");
  Init(theEdge1, theEdge2, thePlane);
}

void ChFi2d_FilletAlgo::Init(const TopoDS_Edge& theEdge1, 
                             const TopoDS_Edge& theEdge2, 
                             const gp_Pln& thePlane)
{
  myPlane = new Geom_Plane(thePlane);

  myEdgesExchnged = Standard_False;

  BRepAdaptor_Curve aBAC1(theEdge1);
  BRepAdaptor_Curve aBAC2(theEdge2);
  if (aBAC1.GetType() < aBAC2.GetType()) 
  { // first curve must be more complicated
    myEdge1 = theEdge2;
    myEdge2 = theEdge1;
    myEdgesExchnged = Standard_True;
  }	 
  else 
  {
    myEdge1 = theEdge1;
    myEdge2 = theEdge2;
  }

  Handle(Geom_Curve) aCurve1 = BRep_Tool::Curve(myEdge1, myStart1, myEnd1);
  Handle(Geom_Curve) aCurve2 = BRep_Tool::Curve(myEdge2, myStart2, myEnd2);

  myCurve1 = GeomProjLib::Curve2d(aCurve1, myStart1, myEnd1, myPlane);
  myCurve2 = GeomProjLib::Curve2d(aCurve2, myStart2, myEnd2, myPlane);

  while (myCurve1->IsPeriodic() && myStart1 >= myEnd1)
    myEnd1 += myCurve1->Period();
  while (myCurve2->IsPeriodic() && myStart2 >= myEnd2)
    myEnd2 += myCurve2->Period();
 
  if (aBAC1.GetType() == aBAC2.GetType()) 
  {
    if (myEnd2 - myStart2 < myEnd1 - myStart1) 
    { // first curve must be parametrically shorter
      TopoDS_Edge anEdge = myEdge1;
      myEdge1 = myEdge2;
      myEdge2 = anEdge;
      Handle(Geom2d_Curve) aCurve = myCurve1;
      myCurve1 = myCurve2;
      myCurve2 = aCurve;
      Standard_Real a = myStart1;
      myStart1 = myStart2;
      myStart2 = a;
      a = myEnd1;
      myEnd1 = myEnd2;
      myEnd2 = a;
      myEdgesExchnged = Standard_True;
    }
  }
}

//! This function returns true if linear segment from start point of the 
//! fillet arc to the end point is intersected by the first or second 
//! curve: in this case fillet is invalid.
static Standard_Boolean IsRadiusIntersected(const Handle(Geom2d_Curve)& theCurve, const Standard_Real theCurveMin, const double theCurveMax,
                                            const gp_Pnt2d theStart, const gp_Pnt2d theEnd, const Standard_Boolean theStartConnected) 
{
  //Check the given start and end if they are identical. If yes
  //return false
  if (theStart.SquareDistance(theEnd) < Precision::SquareConfusion())
  {
    return Standard_False;
  }
  Handle(Geom2d_Line) line = new Geom2d_Line(theStart, gp_Dir2d(gp_Vec2d(theStart, theEnd)));
  Geom2dAPI_InterCurveCurve anInter(theCurve, line, Precision::Confusion());
  Standard_Integer a;
  gp_Pnt2d aPoint;
  for(a = anInter.NbPoints(); a > 0; a--) 
  {
    aPoint = anInter.Point(a);
    Geom2dAPI_ProjectPointOnCurve aProjInt(aPoint, theCurve, theCurveMin, theCurveMax);
    if (aProjInt.NbPoints() < 1 || aProjInt.LowerDistanceParameter() > Precision::Confusion()) 
      continue; // point is not on edge

    if (aPoint.Distance(theStart) < Precision::Confusion())
    {
      if (!theStartConnected) 
        return Standard_True;
    }
    if (aPoint.Distance(theEnd) < Precision::Confusion()) 
      return Standard_True;
    if (gp_Vec2d(aPoint, theStart).IsOpposite(gp_Vec2d(aPoint, theEnd), Precision::Angular())) 
      return Standard_True;
  }
  Handle(Geom2d_Curve) aCurve = theCurve;
  for(a = anInter.NbSegments(); a > 0; a--) 
  {
    //anInter.Segment(a, aCurve); //not implemented (bug in OCC)
    aPoint = aCurve->Value(aCurve->FirstParameter());

    Geom2dAPI_ProjectPointOnCurve aProjInt(aPoint, theCurve, theCurveMin, theCurveMax);
    if (aProjInt.NbPoints() && aProjInt.LowerDistanceParameter() < Precision::Confusion()) 
    { // point is on edge
      if (aPoint.Distance(theStart) < Precision::Confusion()) 
        if (!theStartConnected) 
          return Standard_True;
      if (aPoint.Distance(theEnd) < Precision::Confusion()) 
        return Standard_True;
      if (gp_Vec2d(aPoint, theStart).IsOpposite(gp_Vec2d(aPoint, theEnd), Precision::Angular())) 
        return Standard_True;
    }
    aPoint = aCurve->Value(aCurve->LastParameter());

    aProjInt.Init(aPoint, theCurve, theCurveMin, theCurveMax);
    if (aProjInt.NbPoints() && aProjInt.LowerDistanceParameter() < Precision::Confusion()) 
    { // point is on edge
      if (aPoint.Distance(theStart) < Precision::Confusion()) 
        if (!theStartConnected) 
          return Standard_True;
      if (aPoint.Distance(theEnd) < Precision::Confusion()) 
        return Standard_True;
      if (gp_Vec2d(aPoint, theStart).IsOpposite(gp_Vec2d(aPoint, theEnd), Precision::Angular())) 
        return Standard_True;
    }
  }
  return Standard_False;
}

void ChFi2d_FilletAlgo::FillPoint(FilletPoint* thePoint, const Standard_Real theLimit) 
{
  
  // on the intersection point
  Standard_Boolean aValid = Standard_False;
  Standard_Real aStep = Precision::Confusion();
  gp_Pnt2d aCenter, aPoint; // center of fillet and point on curve1
  Standard_Real aParam = thePoint->getParam();
  if (theLimit < aParam) aStep = -aStep;
  for(aValid = Standard_False; !aValid; aParam += aStep) 
  {
    if ((aParam - aStep - theLimit) * (aParam - theLimit) <= 0) 
      break; // limit was exceeded
    aStep *= 2;
    gp_Vec2d aVec;
    myCurve1->D1(aParam, aPoint, aVec);
    if (aVec.SquareMagnitude() < Precision::Confusion()) 
      continue;

    gp_Vec2d aPerp(((myStartSide)?-1:1) * aVec.Y(), ((myStartSide)?1:-1) * aVec.X());
    aPerp.Normalize();
    aPerp.Multiply(myRadius);
    aCenter = aPoint.Translated(aPerp);


    Geom2dAPI_ProjectPointOnCurve aProjInt(aPoint, myCurve2, myStart2, myEnd2);
    if (aProjInt.NbPoints() == 0 || aPoint.Distance(aProjInt.NearestPoint()) > Precision::Confusion()) 
    {
      aValid = Standard_True;
      break;
    }
  }
  if (aValid) 
  {
    thePoint->setParam(aParam);
    thePoint->setCenter(aCenter);
    aValid = !IsRadiusIntersected(myCurve2, myStart2, myEnd2, aPoint, aCenter, Standard_True);
  }
  
  Geom2dAPI_ProjectPointOnCurve aProj(aCenter, myCurve2);
  int a, aNB = aProj.NbPoints();
  for(a = aNB; a > 0; a--) 
  {
    if (aPoint.SquareDistance(aProj.Point(a)) < Precision::Confusion()) 
      continue;
        
    Standard_Boolean aValid2 = aValid;
    if (aValid2) 
      aValid2 = !IsRadiusIntersected(myCurve1, myStart1, myEnd1, aCenter, aProj.Point(a), Standard_False);

    // checking the right parameter
    Standard_Real aParamProj = aProj.Parameter(a);
    while(myCurve2->IsPeriodic() && aParamProj < myStart2)
      aParamProj += myCurve2->Period();

    const Standard_Real d = aProj.Distance(a);
    thePoint->appendValue(d * d - myRadius * myRadius, (aParamProj >= myStart2 && aParamProj <= myEnd2 && aValid2));
    if (Abs(d - myRadius) < Precision::Confusion())
      thePoint->setParam2(aParamProj);
  }
}

void ChFi2d_FilletAlgo::FillDiff(FilletPoint* thePoint, Standard_Real theDiffStep, Standard_Boolean theFront) 
{
  Standard_Real aDelta = theFront?(theDiffStep):(-theDiffStep);
  FilletPoint* aDiff = new FilletPoint(thePoint->getParam() + aDelta);
  FillPoint(aDiff, aDelta * 999.);
  if (!thePoint->calculateDiff(aDiff)) 
  {
    aDiff->setParam(thePoint->getParam() - aDelta);
    FillPoint(aDiff,  - aDelta * 999);
    thePoint->calculateDiff(aDiff);
  }
  delete aDiff;
}

// returns true, if at least one result was found
Standard_Boolean ChFi2d_FilletAlgo::Perform(const Standard_Real theRadius) 
{
  myDegreeOfRecursion = 0;
  myResultParams.Clear();
  myResultOrientation.Clear();

  Standard_Real aNBSteps;
  Geom2dAdaptor_Curve aGAC(myCurve1);
  switch (aGAC.GetType()) 
  {
  case GeomAbs_Line:
    aNBSteps = 2;
    break;
  case GeomAbs_Circle:
    aNBSteps = 4;
    break;
  case GeomAbs_Ellipse:
    aNBSteps = 5;
    break;
  case GeomAbs_BSplineCurve:
    aNBSteps = 2 + aGAC.Degree() * aGAC.NbPoles();
    break;
  default: // unknown: maximum
    aNBSteps = 100;
  }
  //cout<<"aNBSteps = "<<aNBSteps<<endl;

  myRadius = theRadius;
  Standard_Real aParam, aStep, aDStep;
  aStep = (myEnd1 - myStart1) / aNBSteps;
  aDStep = 1.e-4 * aStep;

  int aCycle;
  for(aCycle = 2, myStartSide = Standard_False; aCycle; myStartSide = !myStartSide, aCycle--) 
  {
    FilletPoint *aLeft = NULL, *aRight;
    
    for(aParam = myStart1 + aStep; aParam < myEnd1 || Abs(myEnd1 - aParam) < Precision::Confusion(); aParam += aStep) 
    {
      if (!aLeft) 
      {
        aLeft = new FilletPoint(aParam - aStep);
        FillPoint(aLeft, aParam);
        FillDiff(aLeft, aDStep, Standard_True);
      }
      
      aRight = new FilletPoint(aParam);
      FillPoint(aRight, aParam - aStep);
      FillDiff(aRight, aDStep, Standard_False);
      
      aLeft->FilterPoints(aRight);
      PerformNewton(aLeft, aRight);
      
      delete aLeft;
      aLeft = aRight;
    }//for
    delete aLeft;
  }//for

  return !myResultParams.IsEmpty();
}

Standard_Boolean ChFi2d_FilletAlgo::ProcessPoint(FilletPoint* theLeft, FilletPoint* theRight, Standard_Real theParameter) 
{
  if (theParameter >= theLeft->getParam() && theParameter < theRight->getParam()) 
  {
    Standard_Real aDX = (theRight->getParam() - theLeft->getParam());
    if (theParameter - theLeft->getParam() < aDX/100.0) 
    {
      theParameter = theLeft->getParam() + aDX/100.0;
    }
    if (theRight->getParam() - theParameter < aDX/100.0) 
    {
      theParameter = theRight->getParam() - aDX/100.0;
    }

    // Protection on infinite loops.
    myDegreeOfRecursion++;
    Standard_Real diffx = 0.001 * aDX;
    if (myDegreeOfRecursion > 100)
    {
      diffx *= 10.0;
      if (myDegreeOfRecursion > 1000)
      {
        diffx *= 10.0;
        if (myDegreeOfRecursion > 3000)
        {
          return Standard_True;
        }
      }
    }

    FilletPoint* aPoint1 = theLeft->Copy();
    FilletPoint* aPoint2 = new FilletPoint(theParameter);
    FillPoint(aPoint2, aPoint1->getParam());
    FillDiff(aPoint2, diffx, Standard_True);
    
    aPoint1->FilterPoints(aPoint2);
    PerformNewton(aPoint1, aPoint2);
    aPoint2->FilterPoints(theRight);
    PerformNewton(aPoint2, theRight);

    delete aPoint1;
    delete aPoint2;
    return Standard_True;
  }

  return Standard_False;
}

void ChFi2d_FilletAlgo::PerformNewton(FilletPoint* theLeft, FilletPoint* theRight) 
{
  int a;
  // check the left: if this is solution store it and remove it from the list of researching points of theLeft
  a = theLeft->hasSolution(myRadius);
  if (a) 
  {
    if (theLeft->isValid(a)) 
    {
      myResultParams.Append(theLeft->getParam());
      myResultOrientation.Append(myStartSide);
    }
    return;
  }

  Standard_Real aDX = theRight->getParam() - theLeft->getParam();
  if (aDX < 1.e-6 * Precision::Confusion())
  {
    a = theRight->hasSolution(myRadius);
    if (a && theRight->isValid(a)) 
    {
      myResultParams.Append(theRight->getParam());
      myResultOrientation.Append(myStartSide);
    }
    return;
  }
  for(a = 1; a <= theLeft->getNBValues(); a++) 
  {
    int aNear = theLeft->getNear(a);
        
    Standard_Real aA = (theRight->getDiff(aNear) - theLeft->getDiff(a)) / aDX;
    Standard_Real aB = theLeft->getDiff(a) - aA * theLeft->getParam();
    Standard_Real aC = theLeft->getValue(a) - theLeft->getDiff(a) * theLeft->getParam() + aA * theLeft->getParam() * theLeft->getParam() / 2.0;
    Standard_Real aDet = aB * aB - 2.0 * aA * aC;
    
    if (Abs(aA) < Precision::Confusion()) 
    { // linear case
      //cout<<"###"<<endl;
      if (Abs(aB) > 10e-20) 
      {
        Standard_Real aX0 = - aC / aB; // use extremum
        if (aX0 > theLeft->getParam() && aX0 < theRight->getParam())
          ProcessPoint(theLeft, theRight, aX0);
      }
      else 
      {
        ProcessPoint(theLeft, theRight, theLeft->getParam() + aDX / 2.0); // linear division otherwise
      }
    } 
    else
    {
      if (Abs(aB) > Abs(aDet * 1000000.)) 
      {	// possible floating point operations accurancy errors
        //cout<<"*";
        ProcessPoint(theLeft, theRight, theLeft->getParam() + aDX / 2.0); // linear division otherwise
      } 
      else
      {
        if (aDet > 0) 
        { // two solutions
          aDet = sqrt(aDet);
          Standard_Boolean aRes = ProcessPoint(theLeft, theRight, (- aB + aDet) / aA);
          if (!aRes) 
            aRes = ProcessPoint(theLeft, theRight, (- aB - aDet) / aA);
          if (!aRes) 
            ProcessPoint(theLeft, theRight, theLeft->getParam() + aDX / 2.0); // linear division otherwise
        } 
        else 
        {
          //cout<<"%%%"<<endl;
          Standard_Real aX0 = - aB / aA; // use extremum
          if (aX0 > theLeft->getParam() && aX0 < theRight->getParam())
            ProcessPoint(theLeft, theRight, aX0);
          else 
            ProcessPoint(theLeft, theRight, theLeft->getParam() + aDX / 2.0); // linear division otherwise
        }
      }
    }
  }//for
}

// returns number of possible solutions.
int ChFi2d_FilletAlgo::NbResults(const gp_Pnt& thePoint)
{
  Standard_Real aX, aY;
  gp_Pnt2d aTargetPoint2d;
  ElSLib::PlaneParameters(myPlane->Pln().Position(), thePoint, aX, aY);
  aTargetPoint2d.SetCoord(aX, aY);
   
  //iterate through all possible solutions.
  int i = 1, nb = 0;
  TColStd_ListIteratorOfListOfReal anIter(myResultParams);
  for(; anIter.More(); anIter.Next(), i++) 
  {
    myStartSide = (myResultOrientation.Value(i)) ? Standard_True : Standard_False;
    FilletPoint *aPoint = new FilletPoint(anIter.Value());
    FillPoint(aPoint, anIter.Value() + 1.);
    if (aPoint->hasSolution(myRadius)) 
      nb++;
    delete aPoint;
  }//for

  return nb;
}

// returns result (fillet edge, modified edge1, modified edge2), neares to the given point <thePoint>
TopoDS_Edge ChFi2d_FilletAlgo::Result(const gp_Pnt& thePoint, TopoDS_Edge& theEdge1, TopoDS_Edge& theEdge2, const int iSolution) 
{
  TopoDS_Edge aResult;
  gp_Pnt2d aTargetPoint2d;
  Standard_Real aX, aY;
  ElSLib::PlaneParameters(myPlane->Pln().Position(), thePoint, aX, aY);
  aTargetPoint2d.SetCoord(aX, aY);
   
  // choose the nearest circle
  Standard_Real aDistance = 0.0, aP;
  FilletPoint *aNearest;
  int a, iSol = 1;
  TColStd_ListIteratorOfListOfReal anIter(myResultParams);
  for(aNearest = NULL, a = 1; anIter.More(); anIter.Next(), a++) 
  {
    myStartSide = (myResultOrientation.Value(a))?Standard_True:Standard_False;
    FilletPoint *aPoint = new FilletPoint(anIter.Value());
    FillPoint(aPoint, anIter.Value() + 1.);
    if (!aPoint->hasSolution(myRadius))
    {
      delete aPoint;
      continue;
    }
    aP = DBL_MAX;
    if (iSolution == -1)
    {
      aP = Abs(aPoint->getCenter().Distance(aTargetPoint2d) - myRadius);
    }
    else if (iSolution == iSol)
    {
      aP = 0.0;
    }
    if (!aNearest || aP < aDistance) 
    {
      aNearest = aPoint;
      aDistance = aP;
    } 
    else 
    {
      delete aPoint;
    }
    if (iSolution == iSol)
      break;
    iSol++;
  }//for
   
  if (!aNearest) 
    return aResult;
   
  // create circle edge
  gp_Pnt aCenter = ElSLib::PlaneValue(aNearest->getCenter().X(), aNearest->getCenter().Y(), myPlane->Pln().Position());
  Handle(Geom_Circle) aCircle = new Geom_Circle(gp_Ax2(aCenter, myPlane->Pln().Axis().Direction()), myRadius);
  gp_Pnt2d aPoint2d1, aPoint2d2;
  myCurve1->D0(aNearest->getParam(), aPoint2d1);
  myCurve2->D0(aNearest->getParam2(), aPoint2d2);
  gp_Pnt aPoint1 = ElSLib::PlaneValue(aPoint2d1.X(), aPoint2d1.Y(), myPlane->Pln().Position());
  gp_Pnt aPoint2 = ElSLib::PlaneValue(aPoint2d2.X(), aPoint2d2.Y(), myPlane->Pln().Position());

  GeomAPI_ProjectPointOnCurve aProj(thePoint, aCircle);
  Standard_Real aTargetParam = aProj.LowerDistanceParameter();
  gp_Pnt aPointOnCircle = aProj.NearestPoint();

  // There is a bug in Open CASCADE in calculation of nearest point to a circle near the parameter 0.0
  // Therefore I check this extrema point manually:
  gp_Pnt p0 = ElCLib::Value(0.0, aCircle->Circ());
  if (p0.Distance(thePoint) < aPointOnCircle.Distance(thePoint))
  {
    aTargetParam = 0.0;
    aPointOnCircle = p0;
  }

  aProj.Perform(aPoint1);
  Standard_Real aParam1 = aProj.LowerDistanceParameter();
  aProj.Perform(aPoint2);
  Standard_Real aParam2 = aProj.LowerDistanceParameter();
  Standard_Boolean aIsOut = ((aParam1 < aTargetParam && aParam2 < aTargetParam) || (aParam1 > aTargetParam && aParam2 > aTargetParam));
  if (aParam1 > aParam2) 
    aIsOut = !aIsOut;
  BRepBuilderAPI_MakeEdge aBuilder(aCircle->Circ(), aIsOut ? aParam2 : aParam1, aIsOut? aParam1 : aParam2);
  aResult = aBuilder.Edge();

  // divide edges
  Standard_Real aStart, anEnd;
  Handle(Geom_Curve) aCurve = BRep_Tool::Curve(myEdge1, aStart, anEnd);
  gp_Vec aDir;
  aCurve->D1(aNearest->getParam(), aPoint1, aDir);

  gp_Vec aCircleDir;
  aCircle->D1(aParam1, aPoint1, aCircleDir);
    
  if ((aCircleDir.Angle(aDir) > M_PI / 2.0) ^ aIsOut)
    aStart = aNearest->getParam();
  else
    anEnd = aNearest->getParam();

  //Check the case when start and end are identical. This happens
  //when the edge decreases to size 0. Old ww5 allows such
  //cases. So we are again bug compatible
  if (fabs(aStart - anEnd) < Precision::Confusion())
    anEnd = aStart + Precision::Confusion();
  //Divide edge
  BRepBuilderAPI_MakeEdge aDivider1(aCurve, aStart, anEnd);
  if (myEdgesExchnged) 
    theEdge2 = aDivider1.Edge();
  else 
    theEdge1 = aDivider1.Edge();

  aCurve = BRep_Tool::Curve(myEdge2, aStart, anEnd);
  aCurve->D1(aNearest->getParam2(), aPoint2, aDir);
   
  aCircle->D1(aParam2, aPoint2, aCircleDir);

  if ((aCircleDir.Angle(aDir) > M_PI / 2.0) ^ (!aIsOut))
    aStart = aNearest->getParam2();
  else
    anEnd = aNearest->getParam2();

  //Check the case when start and end are identical. This happens
  //when the edge decreases to size 0. Old ww5 allows such
  //cases. So we are again bug compatible
  if (fabs(aStart - anEnd) < Precision::Confusion())
    anEnd = aStart + Precision::Confusion();
  BRepBuilderAPI_MakeEdge aDivider2(aCurve, aStart, anEnd);
  if (myEdgesExchnged) 
    theEdge1 = aDivider2.Edge();
  else 
    theEdge2 = aDivider2.Edge();

  delete aNearest;
  return aResult;
}

FilletPoint::FilletPoint(const Standard_Real theParam)
: myParam (theParam),
  myParam2(0.0)
{
}

void FilletPoint::appendValue(Standard_Real theValue, Standard_Boolean theValid) 
{
  Standard_Integer a;
  for(a = 1; a <= myV.Length(); a++) 
  {
    if (theValue < myV.Value(a)) 
    {
      myV.InsertBefore(a, theValue);
      myValid.InsertBefore(a, theValid);
      return;
    }
  }
  myV.Append(theValue);
  myValid.Append(theValid);
}

Standard_Boolean FilletPoint::calculateDiff(FilletPoint* thePoint) 
{
  Standard_Integer a;
  Standard_Boolean aDiffsSet = (myD.Length() != 0);
  Standard_Real aDX = thePoint->getParam() - myParam, aDY = 0.0;
  if (thePoint->myV.Length() == myV.Length()) 
  { // absolutely the same points
    for(a = 1; a <= myV.Length(); a++) 
    {
      aDY = thePoint->myV.Value(a) - myV.Value(a);
      if (aDiffsSet) 
        myD.SetValue(a, aDY / aDX);
      else 
        myD.Append(aDY / aDX);
    }
    return Standard_True;
  }
  // between the diffeerent points searching for nearest analogs
  Standard_Integer b;
  for(a = 1; a <= myV.Length(); a++) 
  {
    for(b = 1; b <= thePoint->myV.Length(); b++) 
    {
      if (b == 1 || Abs(thePoint->myV.Value(b) - myV.Value(a)) < Abs(aDY))
        aDY = thePoint->myV.Value(b) - myV.Value(a);
    }
    if (aDiffsSet) 
    {
      if (Abs(aDY / aDX) < Abs(myD.Value(a)))
        myD.SetValue(a, aDY / aDX);
    } 
    else 
    {
      myD.Append(aDY / aDX);
    }
  }//for
    
  return Standard_False;
}

void FilletPoint::FilterPoints(FilletPoint* thePoint) 
{
  Standard_Integer a, b;
  TColStd_SequenceOfReal aDiffs;
  Standard_Real aY, aY2, aDX = thePoint->getParam() - myParam;
  for(a = 1; a <= myV.Length(); a++) 
  {
    // searching for near point from thePoint
    Standard_Integer aNear = 0;
    Standard_Real aDiff = aDX * 10000.;
    aY = myV.Value(a) + myD.Value(a) * aDX;
    for(b = 1; b <= thePoint->myV.Length(); b++) 
    {
      // calculate hypothesis value of the Y2 with the constant first and second derivative
      aY2 = aY + aDX * (thePoint->myD.Value(b) - myD.Value(a)) / 2.0;
      if (aNear == 0 || Abs(aY2 - thePoint->myV.Value(b)) < Abs(aDiff)) 
      {
        aNear = b;
        aDiff = aY2 - thePoint->myV.Value(b);
      }
    }//for b...

    if (aNear) 
    {
      if (myV.Value(a) * thePoint->myV.Value(aNear) > 0) 
      {// the same sign at the same sides of the interval
        if (myV.Value(a) * myD.Value(a) > 0) 
        {
          if (Abs(myD.Value(a)) > Precision::Confusion()) 
            aNear = 0;
        } 
        else 
        {
          if (Abs(myV.Value(a)) > Abs(thePoint->myV.Value(aNear)))
            if (thePoint->myV.Value(aNear) * thePoint->myD.Value(aNear) < 0 && Abs(thePoint->myD.Value(aNear)) > Precision::Confusion())
            {
              aNear = 0;
            }
        }
      }
    }//if aNear

    if (aNear) 
    {
      if (myV.Value(a) * thePoint->myV.Value(aNear) > 0) 
      {
        if ((myV.Value(a) + myD.Value(a) * aDX) * myV.Value(a) > Precision::Confusion() &&
            (thePoint->myV.Value(aNear) + thePoint->myD.Value(aNear) * aDX) * thePoint->myV.Value(aNear) > Precision::Confusion())
        {
          aNear = 0;
        }
      }
    }//if aNear
    
    if (aNear)
    {
      if (Abs(aDiff / aDX) > 1.e+7) 
      {
        aNear = 0;
      }
    }

    if (aNear == 0) 
    {	// there is no near: remove it from the list
      myV.Remove(a);
      myD.Remove(a);
      myValid.Remove(a);
      a--;
    } 
    else 
    {
      Standard_Boolean aFound = Standard_False;
      for(b = 1; b <= myNear.Length(); b++) 
      {
        if (myNear.Value(b) == aNear) 
        {
          if (Abs(aDiffs.Value(b)) < Abs(aDiff)) 
          { // return this 'near'
            aFound = Standard_True;
            myV.Remove(a);
            myD.Remove(a);
            myValid.Remove(a);
            a--;
            break;
          } 
          else 
          { // remove the old 'near'
            myV.Remove(b);
            myD.Remove(b);
            myValid.Remove(b);
            myNear.Remove(b);
            aDiffs.Remove(b);
            a--;
            break;
          }
        }
      }//for b...
      if (!aFound) 
      {
        myNear.Append(aNear);
        aDiffs.Append(aDiff);
      }
    }//else
  }//for a...
}

FilletPoint* FilletPoint::Copy() 
{
  FilletPoint* aCopy = new FilletPoint(myParam);
  Standard_Integer a;
  for(a = 1; a <= myV.Length(); a++) 
  {
    aCopy->myV.Append(myV.Value(a));
    aCopy->myD.Append(myD.Value(a));
    aCopy->myValid.Append(myValid.Value(a));
  }
  return aCopy;
}

int FilletPoint::hasSolution(const Standard_Real theRadius) 
{
  Standard_Integer a;
  for(a = 1; a <= myV.Length(); a++) 
  {
    if (Abs(sqrt(Abs(Abs(myV.Value(a)) + theRadius * theRadius)) - theRadius) < Precision::Confusion()) 
      return a;
  }
  return 0;
}

void FilletPoint::remove(int theIndex) 
{
  myV.Remove(theIndex);
  myD.Remove(theIndex);
  myValid.Remove(theIndex);
  myNear.Remove(theIndex);
}
