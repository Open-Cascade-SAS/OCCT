// Created by: Eugeny MALTCHIKOV
// Copyright (c) 2014 OPEN CASCADE SAS
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

#include <BRepLib_CheckCurveOnSurface.ixx>

#include <math_GlobOptMin.hxx>
#include <math_MultipleVarFunctionWithHessian.hxx>
#include <math_Matrix.hxx>

#include <Geom_Plane.hxx>
#include <Geom_RectangularTrimmedSurface.hxx>
#include <Geom_TrimmedCurve.hxx>

#include <Geom2dAdaptor.hxx>

#include <GeomAdaptor_HSurface.hxx>
#include <GeomAdaptor_HCurve.hxx>

#include <GeomProjLib.hxx>

#include <BRep_Tool.hxx>
#include <BRep_TEdge.hxx>
#include <BRep_CurveRepresentation.hxx>
#include <BRep_ListIteratorOfListOfCurveRepresentation.hxx>

#include <TopLoc_Location.hxx>

#include <ProjLib_ProjectedCurve.hxx>


//=======================================================================
//class   : BRepLib_CheckCurveOnSurface_GlobOptFunc
//purpose : provides necessary methods to be used in math_GlobOptMin
//=======================================================================
class BRepLib_CheckCurveOnSurface_GlobOptFunc :
  public math_MultipleVarFunctionWithHessian
{
 public:
  BRepLib_CheckCurveOnSurface_GlobOptFunc
    (BRepLib_CheckCurveOnSurface_GlobOptFunc&);
  BRepLib_CheckCurveOnSurface_GlobOptFunc
    (const Handle(Geom_Curve)& theC3D,
     const Handle(Geom2d_Curve)& theC2D,
     const Handle(Geom_Surface)& theSurf,
     const Standard_Real theFirst,
     const Standard_Real theLast)
    :
      myCurve(theC3D),
      myPCurve(theC2D),
      mySurf(theSurf),
      myFirst(theFirst),
      myLast(theLast)
  {
  }
  //
  virtual Standard_Integer NbVariables() const {
    return 1;
  }
  //
  virtual Standard_Boolean Value(const math_Vector& theX,
                                 Standard_Real& theFVal) {
    try {
      const Standard_Real aPar = theX(1);
      if (!CheckParameter(aPar))
        return Standard_False;
      gp_Pnt aP1, aP2;
      gp_Pnt2d aP2d;
      //
      myCurve->D0(aPar, aP1);
      myPCurve->D0(aPar, aP2d);
      mySurf->D0(aP2d.X(), aP2d.Y(), aP2);
      //
      theFVal = -1.0*aP1.SquareDistance(aP2);
    }
    catch(Standard_Failure) {
      return Standard_False;
    }
    //
    return Standard_True;
  }
  //
  virtual Standard_Integer GetStateNumber() {
    return 0;
  }
  //
  virtual Standard_Boolean Gradient(const math_Vector& theX,
                                    math_Vector& theGrad) {
    try {
      const Standard_Real aPar = theX(1);
      if (!CheckParameter(aPar)) {
        return Standard_False;
      }
      //
      gp_Pnt aP1, aP2;
      gp_Vec aDC3D, aDSU, aDSV;
      gp_Pnt2d aP2d;
      gp_Vec2d aDC2D;
      //
      myCurve->D1(aPar, aP1, aDC3D);
      myPCurve->D1(aPar, aP2d, aDC2D);
      mySurf->D1(aP2d.X(), aP2d.Y(), aP2, aDSU, aDSV);
      //
      aP1.SetXYZ(aP1.XYZ() - aP2.XYZ());
      aP2.SetXYZ(aDC3D.XYZ() - aDC2D.X()*aDSU.XYZ() - aDC2D.Y()*aDSV.XYZ());
      //
      theGrad(1) = -2.0*aP1.XYZ().Dot(aP2.XYZ());
    }
    catch(Standard_Failure) {
      return Standard_False;
    }
    //
    return Standard_True;
  }
  //   
  virtual Standard_Boolean Values(const math_Vector& theX,
                                  Standard_Real& theVal,
                                  math_Vector& theGrad) {
    if (!Value(theX, theVal)) {
      return Standard_False;
    }
    //
    if (!Gradient(theX, theGrad)) {
      return Standard_False;
    }
    //
    return Standard_True;
  }
  //
  virtual Standard_Boolean Values(const math_Vector& theX,
                                  Standard_Real& theVal,
                                  math_Vector& theGrad,
                                  math_Matrix& theHessian) {
    if (!Value(theX, theVal)) {
      return Standard_False;
    }
    //    
    if (!Gradient(theX, theGrad)) {
      return Standard_False;
    }
    //
    theHessian(1,1) = theGrad(1);
    //
    return Standard_True;
  }
  //
 private:

  Standard_Boolean CheckParameter(const Standard_Real theParam) {
    return ((myFirst <= theParam) && (theParam <= myLast));
  }

  Handle(Geom_Curve) myCurve;
  Handle(Geom2d_Curve) myPCurve;
  Handle(Geom_Surface) mySurf;
  Standard_Real myFirst;
  Standard_Real myLast;
};

static 
  void MinComputing(BRepLib_CheckCurveOnSurface_GlobOptFunc& theFunction,
                    const Standard_Real theFirst,
                    const Standard_Real theLast,
                    const Standard_Real theEpsilon,
                    Standard_Real& theBestValue,
                    Standard_Real& theBestParameter);


//=======================================================================
//function : BRepLib_CheckCurveOnSurface
//purpose  : 
//=======================================================================
BRepLib_CheckCurveOnSurface::BRepLib_CheckCurveOnSurface()
:
  myFirst(0.),
  myLast(0.),
  myErrorStatus(0),
  myMaxDistance(0.),
  myMaxParameter(0.)
{
}

//=======================================================================
//function : BRepLib_CheckCurveOnSurface
//purpose  : 
//=======================================================================
BRepLib_CheckCurveOnSurface::BRepLib_CheckCurveOnSurface
  (const TopoDS_Edge& theEdge,
   const TopoDS_Face& theFace)
:
  myErrorStatus(0),
  myMaxDistance(0.),
  myMaxParameter(0.)
{
  Init(theEdge, theFace);
}

//=======================================================================
//function : BRepLib_CheckCurveOnSurface
//purpose  : 
//=======================================================================
BRepLib_CheckCurveOnSurface::BRepLib_CheckCurveOnSurface
  (const Handle(Geom_Curve)& the3DCurve,
   const Handle(Geom2d_Curve)& the2DCurve,
   const Handle(Geom_Surface)& theSurface,
   const Standard_Real theFirst,
   const Standard_Real theLast)
:
  myErrorStatus(0),
  myMaxDistance(0.),
  myMaxParameter(0.)
{
  Init(the3DCurve, the2DCurve, theSurface, theFirst, theLast);
}

//=======================================================================
//function : Init
//purpose  : 
//=======================================================================
void BRepLib_CheckCurveOnSurface::Init
  (const TopoDS_Edge& theEdge,
   const TopoDS_Face& theFace)
{
  if (theEdge.IsNull() || theFace.IsNull()) {
    return;
  }
  //
  if (BRep_Tool::Degenerated(theEdge) ||
      !BRep_Tool::IsGeometric(theEdge)) {
    return;
  }
  //
  Standard_Boolean isPCurveFound;
  TopLoc_Location aLocE, aLocF, aLocC2D;
  //
  // 3D curve initialization
  myCurve = Handle(Geom_Curve)::
    DownCast(BRep_Tool::Curve(theEdge, aLocE, myFirst, myLast)->Copy());
  myCurve->Transform(aLocE.Transformation());
  //
  // Surface initialization
  const Handle(Geom_Surface)& aS = BRep_Tool::Surface(theFace, aLocF);
  mySurface = Handle(Geom_Surface)::
    DownCast(aS->Copy()->Transformed(aLocF.Transformation()));
  //
  // 2D curves initialization 
  isPCurveFound = Standard_False;
  aLocC2D = aLocF.Predivided(aLocE);
  const Handle(BRep_TEdge)& aTE = *((Handle(BRep_TEdge)*)&theEdge.TShape());
  BRep_ListIteratorOfListOfCurveRepresentation itcr(aTE->Curves());
  //
  for (; itcr.More(); itcr.Next()) {
    const Handle(BRep_CurveRepresentation)& cr = itcr.Value();
    if (cr->IsCurveOnSurface(aS, aLocC2D)) {
      isPCurveFound = Standard_True;
      myPCurve = cr->PCurve();
      //
      if (cr->IsCurveOnClosedSurface()) {
        myPCurve2 = cr->PCurve2();
      }
      break;
    }
  }
  //
  if (isPCurveFound) {
    return;
  }
  //
  Handle(Geom_Plane) aPlane;
  Handle(Standard_Type) dtyp = mySurface->DynamicType();
  //
  if (dtyp == STANDARD_TYPE(Geom_RectangularTrimmedSurface)) {
    aPlane = Handle(Geom_Plane)::
      DownCast(Handle(Geom_RectangularTrimmedSurface)::
               DownCast(mySurface)->BasisSurface()->Copy());
  }
  else {
    aPlane = Handle(Geom_Plane)::DownCast(mySurface->Copy());
  }
  //
  if (aPlane.IsNull()) { // not a plane
    return;
  }
  //
  aPlane = Handle(Geom_Plane)::DownCast(aPlane);
  //
  Handle(GeomAdaptor_HSurface) aGAHS = new GeomAdaptor_HSurface(aPlane);
  Handle(Geom_Curve) aProjOnPlane = 
    GeomProjLib::ProjectOnPlane (new Geom_TrimmedCurve(myCurve, myFirst, myLast), 
                                 aPlane, aPlane->Position().Direction(), 
                                 Standard_True);
  Handle(GeomAdaptor_HCurve) aHCurve = new GeomAdaptor_HCurve(aProjOnPlane);
  //
  ProjLib_ProjectedCurve aProj(aGAHS, aHCurve);
  myPCurve = Geom2dAdaptor::MakeCurve(aProj);
}

//=======================================================================
//function : Init
//purpose  : 
//=======================================================================
void BRepLib_CheckCurveOnSurface::Init
  (const Handle(Geom_Curve)& the3DCurve,
   const Handle(Geom2d_Curve)& the2DCurve,
   const Handle(Geom_Surface)& theSurface,
   const Standard_Real theFirst,
   const Standard_Real theLast)
{
  myCurve = the3DCurve;
  myPCurve = the2DCurve;
  mySurface = theSurface;
  myFirst = theFirst;
  myLast  = theLast;
}

//=======================================================================
//function : Perform
//purpose  : 
//=======================================================================
void BRepLib_CheckCurveOnSurface::Perform()
{
  try {
    //
    // 1. Check data
    CheckData();
    if (myErrorStatus) {
      return;
    }
    //
    // 2. Compute the max distance
    Compute(myPCurve);
    //
    if (!myPCurve2.IsNull()) {
      // compute max distance for myPCurve2
      // (for the second curve on closed surface)
      Compute(myPCurve2);
    }
  }
  catch (Standard_Failure) {
    myErrorStatus = 3;
  }
}

//=======================================================================
//function : Compute
//purpose  : 
//=======================================================================
void BRepLib_CheckCurveOnSurface::Compute
  (const Handle(Geom2d_Curve)& thePCurve)
{
  Standard_Integer aNbIt, aStatus;
  Standard_Real anEpsilonRange, aMinDelta;
  Standard_Real aFirst, aLast;
  Standard_Real aValue, aParam, aBP;
  Standard_Real theMaxDist, theMaxPar;
  //
  anEpsilonRange = 1.e-3;
  aMinDelta = 1.e-5;
  aFirst = myFirst;
  aLast  = myLast;
  //
  BRepLib_CheckCurveOnSurface_GlobOptFunc aFunc
    (myCurve, thePCurve, mySurface, myFirst, myLast);
  //
  math_Vector anOutputParam(1, 1);
  anOutputParam(1) = aFirst;
  theMaxDist = 0.;
  theMaxPar = aFirst;
  aNbIt = 100;
  aStatus = Standard_True;
  //
  MinComputing(aFunc, aFirst, aLast, anEpsilonRange, theMaxDist, theMaxPar);
  //
  while((aNbIt-- >= 0) && aStatus) {
    aValue = theMaxDist;
    aParam = theMaxPar;
    aBP = theMaxPar - aMinDelta;

    if((aBP - aFirst) > Precision::PConfusion())
      MinComputing(aFunc, aFirst, aBP, anEpsilonRange, theMaxDist, theMaxPar);
    //
    if(theMaxDist < aValue) {
      aLast = aBP;
      aStatus = Standard_True;
    }
    else {
      theMaxDist = aValue;
      theMaxPar = aParam;
      aStatus = Standard_False;
    }
    //
    if(!aStatus) {
      aBP = theMaxPar + aMinDelta;

      if((aLast - aBP) > Precision::PConfusion())
        MinComputing(aFunc, aBP, aLast, 1.0e-3, theMaxDist, theMaxPar);
      //
      if(theMaxDist < aValue) {
        aFirst = aBP;
        aStatus = Standard_True;
      }
      else {
        theMaxDist = aValue;
        theMaxPar = aParam;
        aStatus = Standard_False;
      }
    }
  }
  //
  theMaxDist = sqrt(Abs(theMaxDist));
  if (theMaxDist > myMaxDistance) {
    myMaxDistance = theMaxDist;
    myMaxParameter = theMaxPar;
  }  
}

//=======================================================================
// Function : MinComputing
// purpose : 
//=======================================================================
void MinComputing
  (BRepLib_CheckCurveOnSurface_GlobOptFunc& theFunction,
   const Standard_Real theFirst,
   const Standard_Real theLast,
   const Standard_Real theEpsilon, //1.0e-3
   Standard_Real& theBestValue,
   Standard_Real& theBestParameter)
{
  const Standard_Real aStepMin = 1.0e-2;
  math_Vector aFirstV(1, 1), aLastV(1, 1), anOutputParam(1, 1);
  aFirstV(1) = theFirst;
  aLastV(1) = theLast;
  //
  math_GlobOptMin aFinder(&theFunction, aFirstV, aLastV);
  aFinder.SetTol(aStepMin, theEpsilon);
  aFinder.Perform();
  //
  const Standard_Integer aNbExtr = aFinder.NbExtrema();
  for(Standard_Integer i = 1; i <= aNbExtr; i++)
  {
    Standard_Real aValue = 0.0;
    aFinder.Points(i, anOutputParam);
    theFunction.Value(anOutputParam, aValue);
    //
    if(aValue < theBestValue) {
      theBestValue = aValue;
      theBestParameter = anOutputParam(1);
    }
  }
}
