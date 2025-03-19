// Created: 2009-01-21
//
// Copyright (c) 2009-2013 OPEN CASCADE SAS
//
// This file is part of commercial software by OPEN CASCADE SAS,
// furnished in accordance with the terms and conditions of the contract
// and with the inclusion of this copyright notice.
// This file or any part thereof may not be provided or otherwise
// made available to any third party.
//
// No ownership title to the software is transferred hereby.
//
// OPEN CASCADE SAS makes no representation or warranties with respect to the
// performance of this software, and specifically disclaims any responsibility
// for any damages, special or consequential, connected with its use.

#include <Geom2dConvert_ApproxArcsSegments.hxx>

#include <Adaptor2d_Curve2d.hxx>
#include <ElCLib.hxx>
#include <GCE2d_MakeArcOfCircle.hxx>
#include <GCE2d_MakeSegment.hxx>
#include <GCPnts_QuasiUniformDeflection.hxx>
#include <Geom2d_Circle.hxx>
#include <Geom2d_Line.hxx>
#include <Geom2d_TrimmedCurve.hxx>
#include <NCollection_IncAllocator.hxx>
#include <Precision.hxx>
#include <Standard_Version.hxx>
#include <gp.hxx>
#include <gp_Ax2d.hxx>
#include <gp_Lin2d.hxx>

static const Standard_Integer MAXPOINTS            = 100;
static const Standard_Real    MyCurvatureTolerance = 0.0001;

static Standard_Boolean checkContinuity(const Handle(Geom2d_Curve)& theCurve1,
                                        const Handle(Geom2d_Curve)& theCurve2,
                                        const Standard_Real         theAnglTol);

static Geom2dConvert_PPoint getParameter(const gp_XY&             theXY1,
                                         const Standard_Real      theFirstPar,
                                         const Standard_Real      theLastPar,
                                         const Adaptor2d_Curve2d& theCurve);

static Standard_Boolean isInflectionPoint(const Standard_Real      theParam,
                                          const Adaptor2d_Curve2d& theCurve);

static Standard_Boolean isInflectionPoint(const Standard_Real         theParam,
                                          const Geom2dConvert_PPoint& theFirstInf,
                                          const Adaptor2d_Curve2d&    theCurve,
                                          const Standard_Real         theAnglTol);

//=================================================================================================

Geom2dConvert_ApproxArcsSegments::Geom2dConvert_ApproxArcsSegments(
  const Adaptor2d_Curve2d& theCurve,
  const Standard_Real      theTolerance,
  const Standard_Real      theAngleTol)
    : myCurve(theCurve),
      myAlloc(new NCollection_IncAllocator(4000)),
      myTolerance(theTolerance),
      myAngleTolerance(theAngleTol),
      mySeqParams(myAlloc),
      myStatus(StatusNotDone)
{
  myExt[0] = Geom2dConvert_PPoint(myCurve.FirstParameter(), myCurve);
  myExt[1] = Geom2dConvert_PPoint(myCurve.LastParameter(), myCurve);

  switch (myCurve.GetType())
  {
    case GeomAbs_Line: {
      // Create a single line segment.
      const Standard_Real aDist = myExt[0].Dist(myExt[1]);
      if (aDist > Precision::Confusion())
      {
        const gp_Ax2d anAx2d(myExt[0].Point(), gp_Vec2d(myExt[0].Point(), myExt[1].Point()));
        const Handle(Geom2d_Line) aLine = new Geom2d_Line(anAx2d);
        mySeqCurves.Append(new Geom2d_TrimmedCurve(aLine, 0., aDist));
        myStatus = StatusOK;
      }
    }
    break;
    case GeomAbs_Circle: {
      // Create a couple of arcs of equal size.
      const Geom2dConvert_PPoint aPP(.5 * (myExt[0].Parameter() + myExt[1].Parameter()), myCurve);
      Handle(Geom2d_Curve)       aCurve = makeCircle(myExt[0], aPP);
      if (aCurve.IsNull() == Standard_False)
      {
        mySeqCurves.Append(aCurve);
        aCurve = makeCircle(aPP, myExt[1]);
        if (aCurve.IsNull() == Standard_False)
          mySeqCurves.Append(aCurve);
      }
    }
    break;
    default:
      makeFreeform();
  }

  // Check status of the calculation
  if (myStatus == StatusNotDone)
  {
    if (mySeqCurves.IsEmpty() == Standard_False)
      myStatus = StatusOK;
    else
    {
      // std::cout << "GeomConv2d_Approx: no geometry converted." << std::endl;
      myStatus = StatusError;
    }
  }
}

//=======================================================================
// function : makeCircle
// purpose  : method for creation of circle
//=======================================================================

Handle(Geom2d_Curve) Geom2dConvert_ApproxArcsSegments::makeCircle(
  const Geom2dConvert_PPoint& theFirst,
  const Geom2dConvert_PPoint& theLast) const
{
  Handle(Geom2d_Curve) aResult;
  gp_Pnt2d             aPointM(0.0, 0.0);
  const Standard_Real  aParaM = (theFirst.Parameter() + theLast.Parameter()) * .5;
  myCurve.D0(aParaM, aPointM);
  GCE2d_MakeArcOfCircle aMakeArc1(theFirst.Point(), aPointM, theLast.Point());

  if (aMakeArc1.IsDone())
    aResult = aMakeArc1.Value();
  // else
  // std::cout << "makeCircle(): Circle not built" << std::endl;
  return aResult;
}

//=======================================================================
// function : makeArc
// purpose  : creation arcs by two points and derivative in the first point
///        : parameter isFirst specified direction of the arc.
//=======================================================================

Standard_Boolean Geom2dConvert_ApproxArcsSegments::makeArc(
  const Geom2dConvert_PPoint&  theParam1,
  Geom2dConvert_PPoint&        theParam2,
  const Standard_Boolean       isFirst,
  Handle(Geom2d_TrimmedCurve)& theCurve) const
{
  const gp_XY aP1(theParam1.Point());
  const gp_XY aP2(theParam2.Point());
  const gp_XY aVec(isFirst ? theParam1.D1() : -theParam1.D1());

  // Detect the sense (CCW means positive)
  const gp_XY   aDelta = aP2 - aP1;
  Standard_Real aSense = aVec ^ aDelta;
  if (aSense > Precision::Angular())
    aSense = 1.;
  else if (aSense < -Precision::Angular())
    aSense = -1.;
  else
  {
    // std::cout << "makeArc(): Arc Not Built" << std::endl;
    return Standard_False;
  }

  // Find the centre of the circle
  const gp_XY         aMiddle     = (aP2 + aP1) * 0.5;
  const Standard_Real prodP1V     = aP1 * aVec;
  const Standard_Real prodDeM     = aDelta * aMiddle;
  const Standard_Real vprodVD     = aVec ^ aDelta;
  const Standard_Real aResolution = gp::Resolution();

  if (vprodVD < -aResolution || vprodVD > aResolution)
  {
    const gp_Pnt2d              aCenter((prodP1V * aDelta.Y() - prodDeM * aVec.Y()) / vprodVD,
                           (prodDeM * aVec.X() - prodP1V * aDelta.X()) / vprodVD);
    const Standard_Real         aRad = (aCenter.Distance(aP1) + aCenter.Distance(aP2)) * 0.5;
    const gp_Ax22d              ax22d(aCenter, gp_Dir2d(1., 0.), gp_Dir2d(0., 1.));
    const gp_Circ2d             aCir(ax22d, aRad);
    const Handle(Geom2d_Circle) Circ = new Geom2d_Circle(aCir);

    // calculation parameters first and last points of arc.
    Standard_Real anAlpha1, anAlpha2;
    if (isFirst)
    {
      anAlpha1 = ElCLib::Parameter(aCir, aP1);
      anAlpha2 = ElCLib::Parameter(aCir, aP2);
    }
    else
    {
      anAlpha1 = ElCLib::Parameter(aCir, aP2);
      anAlpha2 = ElCLib::Parameter(aCir, aP1);
      aSense   = -aSense;
    }

    if (fabs(anAlpha1 - anAlpha2) < 1e-100)
      // very small value, just to avoid exact match
      return Standard_False;

    // Reverse the circle if the sense is negative
    if (aSense < 0.)
    {
      anAlpha1 = Circ->ReversedParameter(anAlpha1);
      anAlpha2 = Circ->ReversedParameter(anAlpha2);
      Circ->Reverse();
    }
    theCurve = new Geom2d_TrimmedCurve(Circ, anAlpha1, anAlpha2);
    // Correct the direction in the opposite point
    const gp_XY aRadV = theParam2.Point() - aCenter.XY();
    theParam2.SetD1(gp_XY(-aRadV.Y() * aSense, aRadV.X() * aSense));
    return Standard_True;
  }

  // Algorithm failed, possibly because aVec is normal to the chorde
  return Standard_False;
}

//=======================================================================
// function : makeLine
// purpose  : method for creation of line
//=======================================================================

Handle(Geom2d_TrimmedCurve) Geom2dConvert_ApproxArcsSegments::makeLine(
  Geom2dConvert_PPoint&  theFirst,
  Geom2dConvert_PPoint&  theLast,
  const Standard_Boolean isCheck) const
{
  Handle(Geom2d_TrimmedCurve) aResult;

  gp_XY aSlope = theLast.Point() - theFirst.Point();
  if (fabs(aSlope.SquareModulus()) < gp::Resolution())
    return aResult;
  gp_Dir2d aDirLine(aSlope);

  if (isCheck)
  {
    if (theFirst.D1().SquareModulus() < gp::Resolution()
        || theLast.D1().SquareModulus() < gp::Resolution())
      return aResult;

    // Angular continuity (G1) is only checked when the end of the line is not
    // on the extremity of the curve
    Standard_Real absAngle[2] = {0., 0.};
    if (theFirst != myExt[0])
    {
      const Standard_Real anAng = aDirLine.Angle(theFirst.D1());
      absAngle[0]               = (anAng > 0. ? anAng : -anAng);
    }
    if (theLast != myExt[1])
    {
      const Standard_Real anAng = aDirLine.Angle(theLast.D1());
      absAngle[1]               = (anAng > 0. ? anAng : -anAng);
    }

    // if the derivatives in the end points differ from the derivative line
    // more than value of the specified continuity tolerance
    // then a biarc should be built instead of a line.
    const Standard_Real aContTolerance = ::Max(myAngleTolerance, 0.01);
    if (absAngle[0] > aContTolerance || absAngle[1] > aContTolerance)
    {
      // std::cout << "makeLine(): Line not built" << std::endl;
      return aResult;
    }
  } // end if (isCheck)

  // building segment of line
  GCE2d_MakeSegment aMakeSeg(theFirst.Point(), theLast.Point());
  if (aMakeSeg.IsDone())
  {
    Handle(Geom2d_TrimmedCurve) aCurve = aMakeSeg.Value();
    if (checkCurve(aCurve, theFirst.Parameter(), theLast.Parameter()))
    {
      aResult = aCurve;
      // correct the derivatives fields in both arguments
      const gp_XY aNewD1(theLast.Point() - theFirst.Point());
      theFirst.SetD1(aNewD1);
      theLast.SetD1(aNewD1);
    }
  }
  // else
  // std::cout << "makeLine(): Line not built" << std::endl;
  return aResult;
}

//=======================================================================
// function : makeFreeform
// purpose  : get a sequence of Geom curves from one curve
//=======================================================================

Standard_Boolean Geom2dConvert_ApproxArcsSegments::makeFreeform()
{
  Geom2dConvert_SequenceOfPPoint seqParamPoints(myAlloc);
  Geom2dConvert_PPoint*          aPrevParam = &myExt[0];

  // calculation of the inflection points.
  getLinearParts(seqParamPoints);
  const Standard_Boolean isNoInfPoints = seqParamPoints.IsEmpty();

  TColGeom2d_SequenceOfCurve aSeqLinearParts;
  Standard_Boolean           isDone(Standard_True);
  Standard_Integer           i;
  for (i = 1; i < seqParamPoints.Length(); i += 2)
  {
    Handle(Geom2d_Curve)  aLineCurve;
    Geom2dConvert_PPoint& aParam0 = seqParamPoints.ChangeValue(i);
    Geom2dConvert_PPoint& aParam1 = seqParamPoints.ChangeValue(i + 1);
    if (aParam0 != aParam1)
      // linear part of the curve lies between odd and even values of i.
      // parameters from parameter's sequence.
      aLineCurve = makeLine(aParam0, aParam1, Standard_False);
    aSeqLinearParts.Append(aLineCurve);
  }

  for (i = 1; i < seqParamPoints.Length(); i += 2)
  {
    // approximation for non-linear part preceding the linear part
    if (seqParamPoints(i) != *aPrevParam)
    {
      const Standard_Integer aLastInd = mySeqCurves.Length();
      isDone                          = makeApproximation(*aPrevParam, seqParamPoints(i));
      if (isDone && aLastInd && mySeqCurves.Length() > aLastInd)
        isDone = checkContinuity(mySeqCurves.Value(aLastInd),
                                 mySeqCurves.Value(aLastInd + 1),
                                 myAngleTolerance);
      if (!isDone)
      {
        myStatus = StatusError;
        break;
      }
    }

    const Handle(Geom2d_Curve)& aCurve = aSeqLinearParts.Value((i + 1) / 2);
    if (aCurve.IsNull() == Standard_False)
      mySeqCurves.Append(aCurve);
    else
    {
      Geom2dConvert_PPoint&  aParam0  = seqParamPoints.ChangeValue(i);
      Geom2dConvert_PPoint&  aParam1  = seqParamPoints.ChangeValue(i + 1);
      const Standard_Integer aLastInd = mySeqCurves.Length();
      isDone                          = makeApproximation(aParam0, aParam1);
      if (isDone && aLastInd && mySeqCurves.Length() > aLastInd)
        isDone = checkContinuity(mySeqCurves.Value(aLastInd),
                                 mySeqCurves.Value(aLastInd + 1),
                                 myAngleTolerance);

      if (!isDone)
      {
        myStatus = StatusError;
        // std::cout << "makeOther: Line not built" << std::endl;
        break;
      }
    }
    aPrevParam = &seqParamPoints(i + 1);
  }

  // approximation for non-linear part following the last linear part
  if (isDone && (*aPrevParam != myExt[1]))
  {
    // Case of a closed edge like an ellipse
    if (isNoInfPoints && (myExt[0].Point() - myExt[1].Point()).Modulus() < myTolerance)
    {
      Geom2dConvert_PPoint aPPoint(0.5 * (myExt[0].Parameter() + myExt[1].Parameter()), myCurve);
      isDone = makeApproximation(myExt[0], aPPoint);
      if (isDone)
        isDone = makeApproximation(aPPoint, myExt[1]);
    }
    else
    {
      isDone = makeApproximation(*aPrevParam, myExt[1]);
    }
    if (!isDone)
    {
      myStatus = StatusError;
      // std::cout << "makeOther: Line not built" << std::endl;
    }
  }

  return (mySeqCurves.Length() && myStatus != StatusError);
}

//=======================================================================
// function : getLinearParts
// purpose  : method for getting inflection points
//=======================================================================

void Geom2dConvert_ApproxArcsSegments::getLinearParts(Geom2dConvert_SequenceOfPPoint& theSeqPar)
{
  Standard_Integer i;
  // Fill the sequences with values along the curve
  mySeqParams.Clear();
  Adaptor2d_Curve2d&            myCurveMut = const_cast<Adaptor2d_Curve2d&>(myCurve);
  GCPnts_QuasiUniformDeflection aQUDefAlgo(myCurveMut, myTolerance * 0.5);
  Standard_Boolean              isUniformDone = aQUDefAlgo.IsDone();

  gp_XY aLastPnt(myExt[0].Point());
  if (isUniformDone)
  {
    for (i = 1; i <= aQUDefAlgo.NbPoints(); i++)
    {
      const Geom2dConvert_PPoint aPP(aQUDefAlgo.Parameter(i), myCurve);
      mySeqParams.Append(aPP);
      aLastPnt = aPP.Point();
    }
  }
  else
  {
    const Standard_Real aParamStep = (myExt[1].Parameter() - myExt[0].Parameter()) / MAXPOINTS;
    for (i = 1; i <= MAXPOINTS; i++)
    {
      const Standard_Real        aParam = myExt[0].Parameter() + aParamStep * i;
      const Geom2dConvert_PPoint aPP(aParam, myCurve);
      mySeqParams.Append(aPP);
      aLastPnt = aPP.Point();
    }
  }

  // check if the curve may be linearised
  gp_XY               aDir  = myExt[1].Point() - myExt[0].Point();
  const Standard_Real aMod2 = aDir.SquareModulus();
  if (aMod2 > Precision::Confusion())
  {
    Standard_Boolean isLinear = Standard_True;
    aDir /= sqrt(aMod2);
    for (i = 1; i <= mySeqParams.Length(); i++)
    {
      // Distance from point "i" to the segment between two extremities
      const Standard_Real aDist = aDir ^ (mySeqParams(i).Point() - myExt[0].Point());
      if (aDist > myTolerance * 0.5 || aDist < -myTolerance * 0.5)
      {
        isLinear = Standard_False;
        break;
      }
    }
    if (isLinear)
    {
      theSeqPar.Append(myExt[0]);
      theSeqPar.Append(myExt[1]);
      return;
    }
  }

  // check if point for First Parameter is inflection point.
  Standard_Integer     indStartLinear(0);
  Geom2dConvert_PPoint aLastInflParam  = myExt[0];
  Geom2dConvert_PPoint aFirstInflParam = myExt[0];

  // Getting further inflection points with step by parameter.
  // The point with index 1 is the same as myExt[0]
  for (i = 1; i <= mySeqParams.Length(); i++)
  {
    const Geom2dConvert_PPoint& aCurParam = mySeqParams(i);
    if (indStartLinear)
    {
      Standard_Boolean isStillInflectionFirst =
        isInflectionPoint(aFirstInflParam.Parameter(), aCurParam, myCurve, myAngleTolerance);
      if (isInflectionPoint(aCurParam.Parameter(), aFirstInflParam, myCurve, myAngleTolerance))
      {
        aLastInflParam = mySeqParams(i);
        while (isStillInflectionFirst == Standard_False)
        {
          if (++indStartLinear >= i)
          {
            indStartLinear = 0;
            break;
          }
          aFirstInflParam = mySeqParams(indStartLinear);
          isStillInflectionFirst =
            isInflectionPoint(aFirstInflParam.Parameter(), aCurParam, myCurve, myAngleTolerance);
        }
      }
      else
      {
        // Add the interval in the output sequence
        // The interval is added only if it is more than 10 times the tolerance
        aLastInflParam = findInflection(aLastInflParam, aCurParam);
        if (!isInflectionPoint(aFirstInflParam.Parameter(),
                               aLastInflParam,
                               myCurve,
                               myAngleTolerance))
        {
          aFirstInflParam = findInflection(aLastInflParam, aFirstInflParam);
        }
        const Standard_Real aDist((aFirstInflParam.Point() - aLastInflParam.Point()).Modulus());
        if (aFirstInflParam.Parameter() < aLastInflParam.Parameter() && aDist > 10 * myTolerance)
        {
          theSeqPar.Append(aFirstInflParam);
          theSeqPar.Append(aLastInflParam);
        }
        indStartLinear = 0;
      }
    }
    else if (isInflectionPoint(aCurParam.Parameter(), myCurve))
    {
      aLastInflParam = aCurParam;
      if (i > 1)
        aFirstInflParam = findInflection(aCurParam, mySeqParams(i - 1));
      indStartLinear = i;
    }
  }

  const Standard_Real aDist((aFirstInflParam.Point() - myExt[1].Point()).Modulus());
  if (indStartLinear && aDist > 10 * myTolerance)
  {
    theSeqPar.Append(aFirstInflParam);
    theSeqPar.Append(myExt[1]);
  }
}

//=======================================================================
// function : findInflection
// purpose  : Dichotomic search of the boundary of inflection interval, between
//           two parameters on the Curve
//=======================================================================

Geom2dConvert_PPoint Geom2dConvert_ApproxArcsSegments::findInflection(
  const Geom2dConvert_PPoint& theParamIsInfl,
  const Geom2dConvert_PPoint& theParamNoInfl) const
{
  Standard_Real aLower(theParamIsInfl.Parameter());
  Standard_Real anUpper(theParamNoInfl.Parameter());
  Standard_Real aTest(0.);
  for (Standard_Integer i = 0; i < 3; i++)
  { // 3 iterations
    aTest = (aLower + anUpper) * 0.5;
    if (isInflectionPoint(aTest, theParamIsInfl, myCurve, myAngleTolerance))
      aLower = aTest;
    else
      anUpper = aTest;
  }
  return Geom2dConvert_PPoint(aTest, myCurve);
}

//=======================================================================
// function : makeApproximation
// purpose  : make approximation non-linear part of the other curve
//=======================================================================

Standard_Boolean Geom2dConvert_ApproxArcsSegments::makeApproximation(
  Geom2dConvert_PPoint& theFirstParam,
  Geom2dConvert_PPoint& theLastParam)
{
  // if difference between parameters is less than Precision::PConfusion
  // approximation was not made.
  Standard_Boolean isDone = Standard_False;
  if (theLastParam != theFirstParam)
  {
    const Standard_Real aDistance = (theFirstParam.Point() - theLastParam.Point()).Modulus();
    if (aDistance < myTolerance)
    {
      const Handle(Geom2d_Curve) aCurve = makeLine(theFirstParam, theLastParam, Standard_True);
      isDone                            = !aCurve.IsNull();
      if (isDone && mySeqCurves.Length())
        isDone = checkContinuity(mySeqCurves.Last(), aCurve, myAngleTolerance);
      if (isDone || aDistance < Precision::Confusion())
      {
        mySeqCurves.Append(aCurve);
        return isDone;
      }
    }
    // calculate biarc
    isDone = calculateBiArcs(theFirstParam, theLastParam);

    // if biarc was not calculated calculation is repeated on half the interval.
    if (!isDone)
    {
      Geom2dConvert_PPoint aParaM(theFirstParam.Parameter()
                                    + (theLastParam.Parameter() - theFirstParam.Parameter()) * 0.55,
                                  myCurve);
      isDone = makeApproximation(theFirstParam, aParaM);
      if (isDone)
        isDone = makeApproximation(aParaM, theLastParam);
    }
  }
  return isDone;
}

//=======================================================================
// function : calculateBiArcs
// purpose  : method for calculation of the biarcs.
//=======================================================================

Standard_Boolean Geom2dConvert_ApproxArcsSegments::calculateBiArcs(
  Geom2dConvert_PPoint& theFirstParam,
  Geom2dConvert_PPoint& theLastParam)
{
  const Standard_Real aResolution = gp::Resolution();

  if (theFirstParam.D1().SquareModulus() < aResolution
      || theLastParam.D1().SquareModulus() < aResolution)
  {
    // std::cout << "calculateBiArcs(): bad initial data" << std::endl;
    return Standard_False;
  }
  const gp_XY aPnt[2] = {theFirstParam.Point(), theLastParam.Point()};
  gp_Dir2d    aDir[2] = {theFirstParam.D1(), theLastParam.D1()};

  // Try to approximate the curve by a single arc. The criterion for that is
  // more rigid if the curve is the entire input curve
  // (possible pb. connecting with other boundaries)
  const gp_Vec2d aDelta(aPnt[1] - aPnt[0]);
  Standard_Real  anAngle1 = aDelta.Angle(gp_Vec2d(aDir[0]));
  if (anAngle1 < 0.)
    anAngle1 = -anAngle1;
  Standard_Real anAngle2 = aDelta.Angle(gp_Vec2d(aDir[1]));
  if (anAngle2 < 0.)
    anAngle2 = -anAngle2;

  // in the case when two angles are equal one arc can be built.
  Standard_Real anAngleThreshold(Precision::Angular() * 10.);
  if (theFirstParam != myExt[0] || theLastParam != myExt[1])
    anAngleThreshold = myAngleTolerance * 0.1;
  if (fabs(anAngle1 - anAngle2) < anAngleThreshold)
  {
    Handle(Geom2d_TrimmedCurve) aCurve;
    // protect the theLastParam from modification of D1, when
    // the created arc is rejected.
    Geom2dConvert_PPoint aLastParam(theLastParam);
    if (!makeArc(theFirstParam, aLastParam, Standard_True, aCurve))
      return Standard_False;
    if (checkCurve(aCurve, theFirstParam.Parameter(), aLastParam.Parameter()))
    {
      theLastParam = aLastParam;
      mySeqCurves.Append(aCurve);
      return Standard_True;
    }
  }

  // if one arc was not built or for other cases biarc will be built
  // method for building biarc was taken from article Ahmad H. Nasri et al.
  // "A Recursive Subdivision Algorithm for Piecewise Circular Spline",
  // Computer Graphics Forum, 2001.

  // definition of point of intersection two tangent directions in the points
  // corresponding FirstParameter and LastParameter.
  aDir[1].Reverse();

  // Direct calculation of intersection point, replaces a class call below
  const Standard_Real aProd[3] = {aPnt[0] ^ aDir[0].XY(),
                                  aPnt[1] ^ aDir[1].XY(),
                                  aDir[1] ^ aDir[0].XY()};
  gp_XY               aIntPoint((aProd[0] * aDir[1].X() - aProd[1] * aDir[0].X()) / aProd[2],
                  (aProd[0] * aDir[1].Y() - aProd[1] * aDir[0].Y()) / aProd[2]);
  const gp_XY         aDiff[2] = {aIntPoint - aPnt[0], aIntPoint - aPnt[1]};
  if (aDiff[0] * aDir[0].XY() < 0. || aDiff[1] * aDir[1].XY() < 0.)
  {
    return Standard_False;
  }

  // calculation middle point for building biarc.
  const Standard_Real ad1  = aDiff[0].Modulus();
  const Standard_Real ad2  = aDiff[1].Modulus();
  const Standard_Real ad12 = aDelta.Magnitude();

  const Standard_Real aB1 = ad1 / (ad1 + ad2);
  if (fabs(aB1 - 0.5) < 0.0001)
    return Standard_False;

  gp_XY aXY[2] = {aPnt[0] + aDir[0].XY() * ad12 * ad1 / (ad12 + ad1 + ad2),
                  aPnt[1] + aDir[1].XY() * ad12 * ad2 / (ad12 + ad1 + ad2)};

  const gp_XY          aXYmidArc(aXY[0] + aB1 * (aXY[1] - aXY[0]));
  Geom2dConvert_PPoint aParamMidArc =
    getParameter(aXYmidArc, theFirstParam.Parameter(), theLastParam.Parameter(), myCurve);

  // building first arc from biarc.
  Handle(Geom2d_TrimmedCurve) aCurve1, aCurve2;
  if (!makeArc(theFirstParam, aParamMidArc, Standard_True, aCurve1))
    return Standard_False;

  if (!checkCurve(aCurve1, theFirstParam.Parameter(), aParamMidArc.Parameter()))
    return Standard_False;

  // building second arc from biarc.
  if (makeArc(theLastParam, aParamMidArc, Standard_False, aCurve2))
  {
    if (checkCurve(aCurve2, aParamMidArc.Parameter(), theLastParam.Parameter()))
    {
      mySeqCurves.Append(aCurve1);
      mySeqCurves.Append(aCurve2);
      return Standard_True;
    }
  }
  return Standard_False;
}

//=======================================================================
// function : calculateLines
// purpose  : method for calculation of the linear interpolation.
//=======================================================================

Standard_Boolean Geom2dConvert_ApproxArcsSegments::calculateLines(
  Geom2dConvert_PPoint& theFirstParam,
  Geom2dConvert_PPoint& theLastParam)
{
  Geom2dConvert_PPoint* aPrevParam = &theFirstParam;
  for (int i = 1; i <= mySeqParams.Length(); i++)
  {
    Geom2dConvert_PPoint& aCurParam = mySeqParams.ChangeValue(i);
    if (aCurParam.Parameter() < (*aPrevParam).Parameter())
    {
      continue;
    }
    if (aCurParam.Parameter() > theLastParam.Parameter())
    {
      break;
    }

    // build line segment
    if (aCurParam != *aPrevParam)
    {
      const Standard_Real aDistance = (aCurParam.Point() - (*aPrevParam).Point()).Modulus();
      if (aDistance > myTolerance)
      {
        const Handle(Geom2d_Curve) aCurve = makeLine(*aPrevParam, aCurParam, Standard_False);
        if (aCurve.IsNull())
        {
          return Standard_False;
        }

        mySeqCurves.Append(aCurve);
        aPrevParam = &mySeqParams(i);
      }
    }
  }
  return Standard_True;
}

//=======================================================================
// function : checkCurve
// purpose  : method for checking max deflection Geom curve from Adaptor Curve
//=======================================================================

Standard_Boolean Geom2dConvert_ApproxArcsSegments::checkCurve(
  const Handle(Geom2d_Curve)& aCurve,
  const Standard_Real         theFirstParam,
  const Standard_Real         theLastParam) const
{
  if (aCurve.IsNull())
    return Standard_False; // check fails on empty input
  Standard_Boolean isUniformDone = !mySeqParams.IsEmpty();
  // calculation sequence of the parameters or step by parameter.
  Standard_Integer aNbPnts    = (isUniformDone ? mySeqParams.Length() : MAXPOINTS);
  Standard_Real    aParamStep = (theLastParam - theFirstParam) / MAXPOINTS;

  Handle(Geom2d_Curve)        aCurve1  = aCurve;
  Handle(Geom2d_TrimmedCurve) aTrCurve = Handle(Geom2d_TrimmedCurve)::DownCast(aCurve);
  if (!aTrCurve.IsNull())
    aCurve1 = aTrCurve->BasisCurve();
  gp_Lin2d            aLin2d;
  gp_Circ2d           aCirc2d;
  Handle(Geom2d_Line) aGeomLine = Handle(Geom2d_Line)::DownCast(aCurve1);
  Standard_Boolean    isLine    = (!aGeomLine.IsNull());
  Standard_Boolean    isCircle  = (!isLine);
  if (isLine)
    aLin2d = aGeomLine->Lin2d();
  else
  {
    Handle(Geom2d_Circle) aGeomCircle = Handle(Geom2d_Circle)::DownCast(aCurve1);
    isCircle                          = (!aGeomCircle.IsNull());
    if (isCircle)
      aCirc2d = aGeomCircle->Circ2d();
    else
      return Standard_False;
  }

  // calculation of the max deflection points from CurveAdaptor from Geom curve.
  Standard_Boolean isLess = Standard_True;
  Standard_Integer i      = 1;
  for (; i <= aNbPnts && isLess; i++)
  {

    Standard_Real aParam =
      (isUniformDone ? mySeqParams.Value(i).Parameter() : (theFirstParam + i * aParamStep));
    if (aParam < (theFirstParam - Precision::PConfusion())
        || aParam > (theLastParam + Precision::PConfusion()))
      continue;

    // getting point from adaptor curve by specified parameter.
    gp_Pnt2d aPointAdaptor(0., 0.);
    gp_Pnt2d aProjPoint(0., 0.);
    myCurve.D0(aParam, aPointAdaptor);
    Standard_Real aParameterCurve = 0.0;

    // getting point from geom curve by specified parameter.
    if (isLine)
    {
      aParameterCurve = ElCLib::Parameter(aLin2d, aPointAdaptor);
      aProjPoint      = ElCLib::Value(aParameterCurve, aLin2d);
    }
    else if (isCircle)
    {

      aParameterCurve = ElCLib::Parameter(aCirc2d, aPointAdaptor);
      aProjPoint      = ElCLib::Value(aParameterCurve, aCirc2d);
    }

    isLess = (aProjPoint.Distance(aPointAdaptor) < myTolerance + Precision::PConfusion());
  }
  return isLess;
}

//=======================================================================
// function : checkContinuity
// purpose  : check continuty first derivative between two curves.
//=======================================================================

Standard_Boolean checkContinuity(const Handle(Geom2d_Curve)& theCurve1,
                                 const Handle(Geom2d_Curve)& theCurve2,
                                 const Standard_Real         theAngleTol)
{
  gp_Vec2d v11, v21;
  gp_Pnt2d p1, p2;
  theCurve1->D1(theCurve1->LastParameter(), p1, v11);
  theCurve2->D1(theCurve2->FirstParameter(), p2, v21);

  // check continuity with the specified tolerance.
  return (v11.IsParallel(v21, theAngleTol));
}

//=======================================================================
// function : getParameter
// purpose  : getting the nearest point on AdaptorCurve to the specified point.
//=======================================================================

Geom2dConvert_PPoint getParameter(const gp_XY&             theXY1,
                                  const Standard_Real      theFirstParam,
                                  const Standard_Real      theLastParam,
                                  const Adaptor2d_Curve2d& theCurve)
{
  Geom2dConvert_PPoint aResult;
  Standard_Real        prevParam = theFirstParam;
  Standard_Real        af1       = theFirstParam;
  Standard_Real        af2       = theLastParam;

  // for finding nearest point use method half division.
  Standard_Real    aMinDist = RealLast();
  Standard_Integer i        = 1;
  for (; i <= MAXPOINTS; i++)
  {
    aResult              = Geom2dConvert_PPoint(af1, theCurve);
    Standard_Real adist1 = (theXY1 - aResult.Point()).Modulus();
    if (adist1 < Precision::Confusion())
    {
      return aResult;
    }

    aResult              = Geom2dConvert_PPoint(af2, theCurve);
    Standard_Real adist2 = (theXY1 - aResult.Point()).Modulus();
    if (adist2 < Precision::Confusion())
    {
      return aResult;
    }

    if (aMinDist <= adist2 - Precision::Confusion() && aMinDist <= adist1 - Precision::Confusion())
    {
      break;
    }

    if (adist1 < adist2 - Precision::Confusion())
    {
      prevParam = af1;
      aMinDist  = adist1;
      af2       = (af1 + af2) * 0.5;
    }
    else
    {
      prevParam = af2;
      aMinDist  = adist2;
      af1       = (af1 + af2) * 0.5;
    }
  }
  aResult = Geom2dConvert_PPoint(prevParam, theCurve);
  return aResult;
}

//=======================================================================
// function : isInflectionPoint
// purpose  : method calculating that point specified by parameter
//           is inflection point
//=======================================================================

Standard_Boolean isInflectionPoint(const Standard_Real theParam, const Adaptor2d_Curve2d& theCurve)
{
  gp_Pnt2d aP1;
  gp_Vec2d aD1, aD2;
  theCurve.D2(theParam, aP1, aD1, aD2);
  const Standard_Real aSqMod     = aD1.XY().SquareModulus();
  const Standard_Real aCurvature = fabs(aD1.XY() ^ aD2.XY()) / (aSqMod * sqrt(aSqMod));
  return (aCurvature < MyCurvatureTolerance);
}

//=======================================================================
// function : isInflectionPoint
// purpose  : method calculating that point specified by parameter
//           is inflection point
//=======================================================================

Standard_Boolean isInflectionPoint(const Standard_Real         theParam,
                                   const Geom2dConvert_PPoint& theFirstInfl,
                                   const Adaptor2d_Curve2d&    theCurve,
                                   const Standard_Real         theAngleTol)
{
  gp_Pnt2d aP1;
  gp_Vec2d aD1, aD2;
  theCurve.D2(theParam, aP1, aD1, aD2);
  const Standard_Real aSqMod     = aD1.XY().SquareModulus();
  const Standard_Real aCurvature = fabs(aD1.XY() ^ aD2.XY()) / (aSqMod * sqrt(aSqMod));
  Standard_Real       aContAngle = fabs(gp_Vec2d(aP1.XY() - theFirstInfl.Point()).Angle(aD1));
  aContAngle                     = ::Min(aContAngle, fabs(M_PI - aContAngle));
  return (aCurvature < MyCurvatureTolerance && aContAngle < theAngleTol);
}
