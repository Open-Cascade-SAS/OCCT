// Created by: Nikolai BUKHALOV
// Copyright (c) 2015 OPEN CASCADE SAS
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

#include <GeomLib_CheckCurveOnSurface.hxx>

#include <Adaptor3d_Curve.hxx>
#include <Adaptor3d_CurveOnSurface.hxx>
#include <Geom_BSplineCurve.hxx>
#include <Geom2d_BSplineCurve.hxx>
#include <Geom2dAdaptor_Curve.hxx>
#include <GeomAdaptor_Curve.hxx>
#include <gp_Pnt.hxx>
#include <math_MultipleVarFunctionWithHessian.hxx>
#include <math_NewtonMinimum.hxx>
#include <math_PSO.hxx>
#include <math_PSOParticlesPool.hxx>
#include <OSD_Parallel.hxx>
#include <Standard_ErrorHandler.hxx>
#include <TColStd_Array1OfReal.hxx>
#include <TColStd_HArray1OfReal.hxx>

typedef NCollection_Array1<Handle(Adaptor3d_Curve)> Array1OfHCurve;

class GeomLib_CheckCurveOnSurface_TargetFunc;

static Standard_Boolean MinComputing(GeomLib_CheckCurveOnSurface_TargetFunc& theFunction,
                                     const Standard_Real                     theEpsilon, // 1.0e-3
                                     const Standard_Integer                  theNbParticles,
                                     Standard_Real&                          theBestValue,
                                     Standard_Real&                          theBestParameter);

static Standard_Integer FillSubIntervals(const Handle(Adaptor3d_Curve)&   theCurve3d,
                                         const Handle(Adaptor2d_Curve2d)& theCurve2d,
                                         const Standard_Real              theFirst,
                                         const Standard_Real              theLast,
                                         Standard_Integer&                theNbParticles,
                                         TColStd_Array1OfReal* const      theSubIntervals = 0);

//=======================================================================
// class   : GeomLib_CheckCurveOnSurface_TargetFunc
// purpose : Target function (to be minimized)
//=======================================================================
class GeomLib_CheckCurveOnSurface_TargetFunc : public math_MultipleVarFunctionWithHessian
{
public:
  GeomLib_CheckCurveOnSurface_TargetFunc(const Adaptor3d_Curve& theC3D,
                                         const Adaptor3d_Curve& theCurveOnSurface,
                                         const Standard_Real    theFirst,
                                         const Standard_Real    theLast)
      : myCurve1(theC3D),
        myCurve2(theCurveOnSurface),
        myFirst(theFirst),
        myLast(theLast)
  {
  }

  // returns the number of parameters of the function
  //(the function is one-dimension).
  virtual Standard_Integer NbVariables() const { return 1; }

  // returns value of the function when parameters are equal to theX
  virtual Standard_Boolean Value(const math_Vector& theX, Standard_Real& theFVal)
  {
    return Value(theX(1), theFVal);
  }

  // returns value of the one-dimension-function when parameter
  // is equal to theX
  Standard_Boolean Value(const Standard_Real theX, Standard_Real& theFVal) const
  {
    try
    {
      OCC_CATCH_SIGNALS
      if (!CheckParameter(theX))
        return Standard_False;

      const gp_Pnt aP1(myCurve1.Value(theX)), aP2(myCurve2.Value(theX));

      theFVal = -1.0 * aP1.SquareDistance(aP2);
    }
    catch (Standard_Failure const&)
    {
      return Standard_False;
    }
    //
    return Standard_True;
  }

  // see analogical method for abstract owner class math_MultipleVarFunction
  virtual Standard_Integer GetStateNumber() { return 0; }

  // returns the gradient of the function when parameters are
  // equal to theX
  virtual Standard_Boolean Gradient(const math_Vector& theX, math_Vector& theGrad)
  {
    return Derive(theX(1), theGrad(1));
  }

  // returns 1st derivative of the one-dimension-function when
  // parameter is equal to theX
  Standard_Boolean Derive(const Standard_Real  theX,
                          Standard_Real&       theDeriv1,
                          Standard_Real* const theDeriv2 = 0) const
  {
    try
    {
      OCC_CATCH_SIGNALS
      if (!CheckParameter(theX))
      {
        return Standard_False;
      }
      //
      gp_Pnt aP1, aP2;
      gp_Vec aDC1, aDC2, aDCC1, aDCC2;
      //
      if (!theDeriv2)
      {
        myCurve1.D1(theX, aP1, aDC1);
        myCurve2.D1(theX, aP2, aDC2);
      }
      else
      {
        myCurve1.D2(theX, aP1, aDC1, aDCC1);
        myCurve2.D2(theX, aP2, aDC2, aDCC2);
      }

      const gp_Vec aVec1(aP1, aP2), aVec2(aDC2 - aDC1);
      //
      theDeriv1 = -2.0 * aVec1.Dot(aVec2);

      if (theDeriv2)
      {
        const gp_Vec aVec3(aDCC2 - aDCC1);
        *theDeriv2 = -2.0 * (aVec2.SquareMagnitude() + aVec1.Dot(aVec3));
      }
    }
    catch (Standard_Failure const&)
    {
      return Standard_False;
    }

    return Standard_True;
  }

  // returns value and gradient
  virtual Standard_Boolean Values(const math_Vector& theX,
                                  Standard_Real&     theVal,
                                  math_Vector&       theGrad)
  {
    if (!Value(theX, theVal))
    {
      return Standard_False;
    }
    //
    if (!Gradient(theX, theGrad))
    {
      return Standard_False;
    }
    //
    return Standard_True;
  }

  // returns value, gradient and hessian
  virtual Standard_Boolean Values(const math_Vector& theX,
                                  Standard_Real&     theVal,
                                  math_Vector&       theGrad,
                                  math_Matrix&       theHessian)
  {
    if (!Value(theX, theVal))
    {
      return Standard_False;
    }
    //
    if (!Derive(theX(1), theGrad(1), &theHessian(1, 1)))
    {
      return Standard_False;
    }
    //
    return Standard_True;
  }

  //
  Standard_Real FirstParameter() const { return myFirst; }

  //
  Standard_Real LastParameter() const { return myLast; }

private:
  GeomLib_CheckCurveOnSurface_TargetFunc operator=(GeomLib_CheckCurveOnSurface_TargetFunc&)
    Standard_DELETE;

  // checks if the function can be computed when its parameter is
  // equal to theParam
  Standard_Boolean CheckParameter(const Standard_Real theParam) const
  {
    return ((myFirst <= theParam) && (theParam <= myLast));
  }

  const Adaptor3d_Curve& myCurve1;
  const Adaptor3d_Curve& myCurve2;
  const Standard_Real    myFirst;
  const Standard_Real    myLast;
};

//=======================================================================
// class   : GeomLib_CheckCurveOnSurface_Local
// purpose : Created for parallelization possibility only
//=======================================================================
class GeomLib_CheckCurveOnSurface_Local
{
public:
  GeomLib_CheckCurveOnSurface_Local(const Array1OfHCurve&       theCurveArray,
                                    const Array1OfHCurve&       theCurveOnSurfaceArray,
                                    const TColStd_Array1OfReal& theIntervalsArr,
                                    const Standard_Real         theEpsilonRange,
                                    const Standard_Integer      theNbParticles)
      : myCurveArray(theCurveArray),
        myCurveOnSurfaceArray(theCurveOnSurfaceArray),
        mySubIntervals(theIntervalsArr),
        myEpsilonRange(theEpsilonRange),
        myNbParticles(theNbParticles),
        myArrOfDist(theIntervalsArr.Lower(), theIntervalsArr.Upper() - 1),
        myArrOfParam(theIntervalsArr.Lower(), theIntervalsArr.Upper() - 1)
  {
  }

  void operator()(Standard_Integer theThreadIndex, Standard_Integer theElemIndex) const
  {
    // For every sub-interval (which is set by mySubIntervals array) this method
    // computes optimal value of GeomLib_CheckCurveOnSurface_TargetFunc function.
    // This optimal value will be put in corresponding (depending on theIndex - the
    // identificator of the current interval in mySubIntervals array) cell of
    // myArrOfDist and myArrOfParam arrays.
    GeomLib_CheckCurveOnSurface_TargetFunc aFunc(
      *(myCurveArray.Value(theThreadIndex).get()),
      *(myCurveOnSurfaceArray.Value(theThreadIndex).get()),
      mySubIntervals.Value(theElemIndex),
      mySubIntervals.Value(theElemIndex + 1));

    Standard_Real aMinDist = RealLast(), aPar = 0.0;
    if (!MinComputing(aFunc, myEpsilonRange, myNbParticles, aMinDist, aPar))
    {
      myArrOfDist(theElemIndex)  = RealLast();
      myArrOfParam(theElemIndex) = aFunc.FirstParameter();
      return;
    }

    myArrOfDist(theElemIndex)  = aMinDist;
    myArrOfParam(theElemIndex) = aPar;
  }

  // Returns optimal value (inverse of square of maximal distance)
  void OptimalValues(Standard_Real& theMinimalValue, Standard_Real& theParameter) const
  {
    // This method looks for the minimal value of myArrOfDist.

    const Standard_Integer aStartInd = myArrOfDist.Lower();
    theMinimalValue                  = myArrOfDist(aStartInd);
    theParameter                     = myArrOfParam(aStartInd);
    for (Standard_Integer i = aStartInd + 1; i <= myArrOfDist.Upper(); i++)
    {
      if (myArrOfDist(i) < theMinimalValue)
      {
        theMinimalValue = myArrOfDist(i);
        theParameter    = myArrOfParam(i);
      }
    }
  }

private:
  GeomLib_CheckCurveOnSurface_Local operator=(const GeomLib_CheckCurveOnSurface_Local&)
    Standard_DELETE;

private:
  const Array1OfHCurve& myCurveArray;
  const Array1OfHCurve& myCurveOnSurfaceArray;

  const TColStd_Array1OfReal&               mySubIntervals;
  const Standard_Real                       myEpsilonRange;
  const Standard_Integer                    myNbParticles;
  mutable NCollection_Array1<Standard_Real> myArrOfDist;
  mutable NCollection_Array1<Standard_Real> myArrOfParam;
};

//=================================================================================================

GeomLib_CheckCurveOnSurface::GeomLib_CheckCurveOnSurface()
    : myErrorStatus(0),
      myMaxDistance(RealLast()),
      myMaxParameter(0.),
      myTolRange(Precision::PConfusion()),
      myIsParallel(Standard_False)
{
}

//=================================================================================================

GeomLib_CheckCurveOnSurface::GeomLib_CheckCurveOnSurface(const Handle(Adaptor3d_Curve)& theCurve,
                                                         const Standard_Real            theTolRange)
    : myCurve(theCurve),
      myErrorStatus(0),
      myMaxDistance(RealLast()),
      myMaxParameter(0.),
      myTolRange(theTolRange),
      myIsParallel(Standard_False)
{
}

//=================================================================================================

void GeomLib_CheckCurveOnSurface::Init()
{
  myCurve.Nullify();
  myErrorStatus  = 0;
  myMaxDistance  = RealLast();
  myMaxParameter = 0.0;
  myTolRange     = Precision::PConfusion();
}

//=================================================================================================

void GeomLib_CheckCurveOnSurface::Init(const Handle(Adaptor3d_Curve)& theCurve,
                                       const Standard_Real            theTolRange)
{
  myCurve        = theCurve;
  myErrorStatus  = 0;
  myMaxDistance  = RealLast();
  myMaxParameter = 0.0;
  myTolRange     = theTolRange;
}

//=================================================================================================

void GeomLib_CheckCurveOnSurface::Perform(const Handle(Adaptor3d_CurveOnSurface)& theCurveOnSurface)
{
  if (myCurve.IsNull() || theCurveOnSurface.IsNull())
  {
    myErrorStatus = 1;
    return;
  }

  if ((myCurve->FirstParameter() - theCurveOnSurface->FirstParameter() > myTolRange)
      || (myCurve->LastParameter() - theCurveOnSurface->LastParameter() < -myTolRange))
  {
    myErrorStatus = 2;
    return;
  }

  const Standard_Real anEpsilonRange = 1.e-3;

  Standard_Integer aNbParticles = 3;

  // Polynomial function with degree n has not more than n-1 maxima and
  // minima (degree of 1st derivative is equal to n-1 => 1st derivative has
  // no greater than n-1 roots). Consequently, this function has
  // maximum n monotonicity intervals. That is a good idea to try to put
  // at least one particle in every monotonicity interval. Therefore,
  // number of particles should be equal to n.

  const Standard_Integer aNbSubIntervals = FillSubIntervals(myCurve,
                                                            theCurveOnSurface->GetCurve(),
                                                            myCurve->FirstParameter(),
                                                            myCurve->LastParameter(),
                                                            aNbParticles);

  if (!aNbSubIntervals)
  {
    myErrorStatus = 3;
    return;
  }

  try
  {
    OCC_CATCH_SIGNALS

    TColStd_Array1OfReal anIntervals(1, aNbSubIntervals + 1);
    FillSubIntervals(myCurve,
                     theCurveOnSurface->GetCurve(),
                     myCurve->FirstParameter(),
                     myCurve->LastParameter(),
                     aNbParticles,
                     &anIntervals);

    const Standard_Integer aNbThreads =
      myIsParallel
        ? Min(anIntervals.Size(), OSD_ThreadPool::DefaultPool()->NbDefaultThreadsToLaunch())
        : 1;
    Array1OfHCurve aCurveArray(0, aNbThreads - 1);
    Array1OfHCurve aCurveOnSurfaceArray(0, aNbThreads - 1);
    for (Standard_Integer anI = 0; anI < aNbThreads; ++anI)
    {
      aCurveArray.SetValue(anI, aNbThreads > 1 ? myCurve->ShallowCopy() : myCurve);
      aCurveOnSurfaceArray.SetValue(
        anI,
        aNbThreads > 1 ? theCurveOnSurface->ShallowCopy()
                       : static_cast<const Handle(Adaptor3d_Curve)&>(theCurveOnSurface));
    }
    GeomLib_CheckCurveOnSurface_Local aComp(aCurveArray,
                                            aCurveOnSurfaceArray,
                                            anIntervals,
                                            anEpsilonRange,
                                            aNbParticles);
    if (aNbThreads > 1)
    {
      const Handle(OSD_ThreadPool)& aThreadPool = OSD_ThreadPool::DefaultPool();
      OSD_ThreadPool::Launcher      aLauncher(*aThreadPool, aNbThreads);
      aLauncher.Perform(anIntervals.Lower(), anIntervals.Upper(), aComp);
    }
    else
    {
      for (Standard_Integer anI = anIntervals.Lower(); anI < anIntervals.Upper(); ++anI)
      {
        aComp(0, anI);
      }
    }
    aComp.OptimalValues(myMaxDistance, myMaxParameter);

    myMaxDistance = sqrt(Abs(myMaxDistance));
  }
  catch (Standard_Failure const&)
  {
    myErrorStatus = 3;
  }
}

//=======================================================================
// Function : FillSubIntervals
// purpose : Divides [theFirst, theLast] interval on parts
//            in order to make searching-algorithm more precisely
//            (fills theSubIntervals array).
//            Returns number of subintervals.
//=======================================================================
Standard_Integer FillSubIntervals(const Handle(Adaptor3d_Curve)&   theCurve3d,
                                  const Handle(Adaptor2d_Curve2d)& theCurve2d,
                                  const Standard_Real              theFirst,
                                  const Standard_Real              theLast,
                                  Standard_Integer&                theNbParticles,
                                  TColStd_Array1OfReal* const      theSubIntervals)
{
  const Standard_Integer     aMaxKnots     = 101;
  const Standard_Real        anArrTempC[2] = {theFirst, theLast};
  const TColStd_Array1OfReal anArrTemp(anArrTempC[0], 1, 2);

  theNbParticles = 3;
  Handle(Geom2d_BSplineCurve) aBS2DCurv;
  Handle(Geom_BSplineCurve)   aBS3DCurv;
  Standard_Boolean            isTrimmed3D = Standard_False, isTrimmed2D = Standard_False;

  //
  if (theCurve3d->GetType() == GeomAbs_BSplineCurve)
  {
    aBS3DCurv = theCurve3d->BSpline();
  }
  if (theCurve2d->GetType() == GeomAbs_BSplineCurve)
  {
    aBS2DCurv = theCurve2d->BSpline();
  }

  Handle(TColStd_HArray1OfReal) anArrKnots3D, anArrKnots2D;

  if (!aBS3DCurv.IsNull())
  {
    if (aBS3DCurv->NbKnots() <= aMaxKnots)
    {
      anArrKnots3D = new TColStd_HArray1OfReal(aBS3DCurv->Knots());
    }
    else
    {
      Standard_Integer KnotCount;
      if (isTrimmed3D)
      {
        Standard_Integer i;
        KnotCount                          = 0;
        const TColStd_Array1OfReal& aKnots = aBS3DCurv->Knots();
        for (i = aBS3DCurv->FirstUKnotIndex(); i <= aBS3DCurv->LastUKnotIndex(); ++i)
        {
          if (aKnots(i) > theFirst && aKnots(i) < theLast)
          {
            ++KnotCount;
          }
        }
        KnotCount += 2;
      }
      else
      {
        KnotCount = aBS3DCurv->LastUKnotIndex() - aBS3DCurv->FirstUKnotIndex() + 1;
      }
      if (KnotCount <= aMaxKnots)
      {
        anArrKnots3D = new TColStd_HArray1OfReal(aBS3DCurv->Knots());
      }
      else
      {
        anArrKnots3D = new TColStd_HArray1OfReal(1, aMaxKnots);
        anArrKnots3D->SetValue(1, theFirst);
        anArrKnots3D->SetValue(aMaxKnots, theLast);
        Standard_Integer i;
        Standard_Real    dt = (theLast - theFirst) / (aMaxKnots - 1);
        Standard_Real    t  = theFirst + dt;
        for (i = 2; i < aMaxKnots; ++i, t += dt)
        {
          anArrKnots3D->SetValue(i, t);
        }
      }
    }
  }
  else
  {
    anArrKnots3D = new TColStd_HArray1OfReal(anArrTemp);
  }
  if (!aBS2DCurv.IsNull())
  {
    if (aBS2DCurv->NbKnots() <= aMaxKnots)
    {
      anArrKnots2D = new TColStd_HArray1OfReal(aBS2DCurv->Knots());
    }
    else
    {
      Standard_Integer KnotCount;
      if (isTrimmed2D)
      {
        Standard_Integer i;
        KnotCount                          = 0;
        const TColStd_Array1OfReal& aKnots = aBS2DCurv->Knots();
        for (i = aBS2DCurv->FirstUKnotIndex(); i <= aBS2DCurv->LastUKnotIndex(); ++i)
        {
          if (aKnots(i) > theFirst && aKnots(i) < theLast)
          {
            ++KnotCount;
          }
        }
        KnotCount += 2;
      }
      else
      {
        KnotCount = aBS2DCurv->LastUKnotIndex() - aBS2DCurv->FirstUKnotIndex() + 1;
      }
      if (KnotCount <= aMaxKnots)
      {
        anArrKnots2D = new TColStd_HArray1OfReal(aBS2DCurv->Knots());
      }
      else
      {
        anArrKnots2D = new TColStd_HArray1OfReal(1, aMaxKnots);
        anArrKnots2D->SetValue(1, theFirst);
        anArrKnots2D->SetValue(aMaxKnots, theLast);
        Standard_Integer i;
        Standard_Real    dt = (theLast - theFirst) / (aMaxKnots - 1);
        Standard_Real    t  = theFirst + dt;
        for (i = 2; i < aMaxKnots; ++i, t += dt)
        {
          anArrKnots2D->SetValue(i, t);
        }
      }
    }
  }
  else
  {
    anArrKnots2D = new TColStd_HArray1OfReal(anArrTemp);
  }

  Standard_Integer aNbSubIntervals = 1;

  try
  {
    OCC_CATCH_SIGNALS
    const Standard_Integer anIndMax3D = anArrKnots3D->Upper(), anIndMax2D = anArrKnots2D->Upper();

    Standard_Integer anIndex3D = anArrKnots3D->Lower(), anIndex2D = anArrKnots2D->Lower();

    if (theSubIntervals)
      theSubIntervals->ChangeValue(aNbSubIntervals) = theFirst;

    while ((anIndex3D <= anIndMax3D) && (anIndex2D <= anIndMax2D))
    {
      const Standard_Real aVal3D = anArrKnots3D->Value(anIndex3D),
                          aVal2D = anArrKnots2D->Value(anIndex2D);
      const Standard_Real aDelta = aVal3D - aVal2D;

      if (aDelta < Precision::PConfusion())
      { // aVal3D <= aVal2D
        if ((aVal3D > theFirst) && (aVal3D < theLast))
        {
          aNbSubIntervals++;

          if (theSubIntervals)
            theSubIntervals->ChangeValue(aNbSubIntervals) = aVal3D;
        }

        anIndex3D++;

        if (-aDelta < Precision::PConfusion())
        { // aVal3D == aVal2D
          anIndex2D++;
        }
      }
      else
      { // aVal2D < aVal3D
        if ((aVal2D > theFirst) && (aVal2D < theLast))
        {
          aNbSubIntervals++;

          if (theSubIntervals)
            theSubIntervals->ChangeValue(aNbSubIntervals) = aVal2D;
        }

        anIndex2D++;
      }
    }

    if (theSubIntervals)
      theSubIntervals->ChangeValue(aNbSubIntervals + 1) = theLast;

    if (!aBS3DCurv.IsNull())
    {
      theNbParticles = Max(theNbParticles, aBS3DCurv->Degree());
    }

    if (!aBS2DCurv.IsNull())
    {
      theNbParticles = Max(theNbParticles, aBS2DCurv->Degree());
    }
  }
  catch (Standard_Failure const&)
  {
#ifdef OCCT_DEBUG
    std::cout << "ERROR! BRepLib_CheckCurveOnSurface.cxx, "
                 "FillSubIntervals(): Incorrect filling!"
              << std::endl;
#endif

    aNbSubIntervals = 0;
  }

  return aNbSubIntervals;
}

//=======================================================================
// class   : PSO_Perform
// purpose : Searches minimal distance with math_PSO class
//=======================================================================
Standard_Boolean PSO_Perform(GeomLib_CheckCurveOnSurface_TargetFunc& theFunction,
                             const math_Vector&                      theParInf,
                             const math_Vector&                      theParSup,
                             const Standard_Real                     theEpsilon,
                             const Standard_Integer                  theNbParticles,
                             Standard_Real&                          theBestValue,
                             math_Vector&                            theOutputParam)
{
  const Standard_Real aDeltaParam = theParSup(1) - theParInf(1);
  if (aDeltaParam < Precision::PConfusion())
    return Standard_False;

  math_Vector aStepPar(1, 1);
  aStepPar(1) = theEpsilon * aDeltaParam;

  math_PSOParticlesPool aParticles(theNbParticles, 1);

  // They are used for finding a position of theNbParticles worst places
  const Standard_Integer aNbControlPoints = 3 * theNbParticles;

  const Standard_Real aStep  = aDeltaParam / (aNbControlPoints - 1);
  Standard_Integer    aCount = 1;
  for (Standard_Real aPrm = theParInf(1); aCount <= aNbControlPoints;
       aCount++, aPrm = (aCount == aNbControlPoints) ? theParSup(1) : aPrm + aStep)
  {
    Standard_Real aVal = RealLast();
    if (!theFunction.Value(aPrm, aVal))
      continue;

    PSO_Particle* aParticle = aParticles.GetWorstParticle();

    if (aVal > aParticle->BestDistance)
      continue;

    aParticle->Position[0]     = aPrm;
    aParticle->BestPosition[0] = aPrm;
    aParticle->Distance        = aVal;
    aParticle->BestDistance    = aVal;
  }

  math_PSO aPSO(&theFunction, theParInf, theParSup, aStepPar);
  aPSO.Perform(aParticles, theNbParticles, theBestValue, theOutputParam);

  return Standard_True;
}

//=======================================================================
// class   : MinComputing
// purpose : Performs computing minimal value
//=======================================================================
Standard_Boolean MinComputing(GeomLib_CheckCurveOnSurface_TargetFunc& theFunction,
                              const Standard_Real                     theEpsilon, // 1.0e-3
                              const Standard_Integer                  theNbParticles,
                              Standard_Real&                          theBestValue,
                              Standard_Real&                          theBestParameter)
{
  try
  {
    OCC_CATCH_SIGNALS

    //
    math_Vector aParInf(1, 1), aParSup(1, 1), anOutputParam(1, 1);
    aParInf(1)       = theFunction.FirstParameter();
    aParSup(1)       = theFunction.LastParameter();
    theBestParameter = aParInf(1);
    theBestValue     = RealLast();

    if (!PSO_Perform(theFunction,
                     aParInf,
                     aParSup,
                     theEpsilon,
                     theNbParticles,
                     theBestValue,
                     anOutputParam))
    {
#ifdef OCCT_DEBUG
      std::cout << "BRepLib_CheckCurveOnSurface::Compute(): math_PSO is failed!" << std::endl;
#endif
      return Standard_False;
    }

    theBestParameter = anOutputParam(1);

    // Here, anOutputParam contains parameter, which is near to optimal.
    // It needs to be more precise. Precision is made by math_NewtonMinimum.
    math_NewtonMinimum aMinSol(theFunction);
    aMinSol.Perform(theFunction, anOutputParam);

    if (aMinSol.IsDone() && (aMinSol.GetStatus() == math_OK))
    { // math_NewtonMinimum has precised the value. We take it.
      aMinSol.Location(anOutputParam);
      theBestParameter = anOutputParam(1);
      theBestValue     = aMinSol.Minimum();
    }
    else
    { // Use math_PSO again but on smaller range.
      const Standard_Real aStep = theEpsilon * (aParSup(1) - aParInf(1));
      aParInf(1)                = theBestParameter - 0.5 * aStep;
      aParSup(1)                = theBestParameter + 0.5 * aStep;

      Standard_Real aValue = RealLast();
      if (PSO_Perform(theFunction,
                      aParInf,
                      aParSup,
                      theEpsilon,
                      theNbParticles,
                      aValue,
                      anOutputParam))
      {
        if (aValue < theBestValue)
        {
          theBestValue     = aValue;
          theBestParameter = anOutputParam(1);
        }
      }
    }
  }
  catch (Standard_Failure const&)
  {
#ifdef OCCT_DEBUG
    std::cout << "BRepLib_CheckCurveOnSurface.cxx: Exception in MinComputing()!" << std::endl;
#endif
    return Standard_False;
  }

  return Standard_True;
}
