// Created on: 2003-03-18
// Created by: Oleg FEDYAEV
// Copyright (c) 2003-2014 OPEN CASCADE SAS
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

#include <GeomLib_Tool.hxx>

#include <ElCLib.hxx>
#include <Extrema_ExtPC.hxx>
#include <Extrema_ExtPC2d.hxx>
#include <Extrema_ExtPS.hxx>
#include <Geom2dAdaptor_Curve.hxx>
#include <GeomAdaptor_Curve.hxx>
#include <GeomAdaptor_Surface.hxx>
#include <math_PSO.hxx>

// The functions Parameter(s) are used to compute parameter(s) of point
// on curves and surfaces. The main rule is that tested point must lied
// on curves or surfaces otherwise the resulted parameter(s) may be wrong.
// To make search process more common the MaxDist value is used to define
// the proximity of point to curve or surface. It is clear that this MaxDist
// value can't be too high to be not in conflict with previous rule.
static const Standard_Real PARTOLERANCE = 1.e-9;

//=======================================================================
// function : Parameter
// purpose  : Get parameter on curve of given point
//           return FALSE if point is far from curve than MaxDist
//           or computation fails
//=======================================================================

Standard_Boolean GeomLib_Tool::Parameter(const Handle(Geom_Curve)& Curve,
                                         const gp_Pnt&             Point,
                                         const Standard_Real       MaxDist,
                                         Standard_Real&            U)
{
  if (Curve.IsNull())
    return Standard_False;
  //
  U                  = 0.;
  Standard_Real aTol = MaxDist * MaxDist;
  //
  GeomAdaptor_Curve aGAC(Curve);
  Extrema_ExtPC     extrema(Point, aGAC);
  //
  if (!extrema.IsDone())
    return Standard_False;
  //
  Standard_Integer n = extrema.NbExt();
  if (n <= 0)
    return Standard_False;
  //
  Standard_Integer i = 0, iMin = 0;
  Standard_Real    Dist2Min = RealLast();
  for (i = 1; i <= n; i++)
  {
    if (extrema.SquareDistance(i) < Dist2Min)
    {
      iMin     = i;
      Dist2Min = extrema.SquareDistance(i);
    }
  }
  if (iMin != 0 && Dist2Min <= aTol)
  {
    U = (extrema.Point(iMin)).Parameter();
  }
  else
  {
    return Standard_False;
  }

  return Standard_True;
}

//=======================================================================
// function : Parameters
// purpose  : Get parameters on surface of given point
//           return FALSE if point is far from surface than MaxDist
//           or computation fails
//=======================================================================

Standard_Boolean GeomLib_Tool::Parameters(const Handle(Geom_Surface)& Surface,
                                          const gp_Pnt&               Point,
                                          const Standard_Real         MaxDist,
                                          Standard_Real&              U,
                                          Standard_Real&              V)
{
  if (Surface.IsNull())
    return Standard_False;
  //
  U                  = 0.;
  V                  = 0.;
  Standard_Real aTol = MaxDist * MaxDist;
  //
  GeomAdaptor_Surface aGAS(Surface);
  Standard_Real       aTolU = PARTOLERANCE, aTolV = PARTOLERANCE;
  //
  Extrema_ExtPS extrema(Point, aGAS, aTolU, aTolV);
  //
  if (!extrema.IsDone())
    return Standard_False;
  //
  Standard_Integer n = extrema.NbExt();
  if (n <= 0)
    return Standard_False;
  //
  Standard_Real    Dist2Min = RealLast();
  Standard_Integer i = 0, iMin = 0;
  for (i = 1; i <= n; i++)
  {
    if (extrema.SquareDistance(i) < Dist2Min)
    {
      Dist2Min = extrema.SquareDistance(i);
      iMin     = i;
    }
  }
  if (iMin != 0 && Dist2Min <= aTol)
  {
    extrema.Point(iMin).Parameter(U, V);
  }
  else
  {
    return Standard_False;
  }

  return Standard_True;
}

//=======================================================================
// function : Parameter
// purpose  : Get parameter on curve of given point
//           return FALSE if point is far from curve than MaxDist
//           or computation fails
//=======================================================================

Standard_Boolean GeomLib_Tool::Parameter(const Handle(Geom2d_Curve)& Curve,
                                         const gp_Pnt2d&             Point,
                                         const Standard_Real         MaxDist,
                                         Standard_Real&              U)
{
  if (Curve.IsNull())
    return Standard_False;
  //
  U                  = 0.;
  Standard_Real aTol = MaxDist * MaxDist;
  //
  Geom2dAdaptor_Curve aGAC(Curve);
  Extrema_ExtPC2d     extrema(Point, aGAC);
  if (!extrema.IsDone())
    return Standard_False;
  Standard_Integer n = extrema.NbExt();
  if (n <= 0)
    return Standard_False;
  Standard_Integer i = 0, iMin = 0;
  Standard_Real    Dist2Min = RealLast();
  for (i = 1; i <= n; i++)
  {
    if (extrema.SquareDistance(i) < Dist2Min)
    {
      Dist2Min = extrema.SquareDistance(i);
      iMin     = i;
    }
  }
  if (iMin != 0 && Dist2Min <= aTol)
  {
    U = (extrema.Point(iMin)).Parameter();
  }
  else
  {
    return Standard_False;
  }

  return Standard_True;
}

namespace
{
//! Target function to compute deviation of the source 2D-curve.
//! It is one-variate function. Its parameter is a parameter
//! on the curve. Deviation is a maximal distance between
//! any point in the curve and the given line.
class FuncSolveDeviation : public math_MultipleVarFunction
{
public:
  //! Constructor. Initializes the curve and the line
  //! going through two given points.
  FuncSolveDeviation(const Geom2dAdaptor_Curve& theCurve, const gp_XY& thePf, const gp_XY& thePl)
      : myCurve(theCurve),
        myPRef(thePf)
  {
    myDirRef  = thePl - thePf;
    mySqMod   = myDirRef.SquareModulus();
    myIsValid = (mySqMod > Precision::SquarePConfusion());
  }

  //! Compute additional parameters depending on the argument
  //! of *this
  void UpdateFields(const Standard_Real theParam)
  {
    myCurve.D0(theParam, myPointOnCurve);
    const gp_XY aVt = myPointOnCurve.XY() - myPRef;
    myVecCurvLine   = aVt.Dot(myDirRef) * myDirRef / mySqMod - aVt;
  }

  //! Returns value of *this (square deviation) and its 1st and 2nd derivative.
  void ValueAndDerives(const Standard_Real theParam,
                       Standard_Real&      theVal,
                       Standard_Real&      theD1,
                       Standard_Real&      theD2)
  {
    gp_Vec2d aD1;
    gp_Vec2d aD2;
    myCurve.D2(theParam, myPointOnCurve, aD1, aD2);

    const gp_XY aVt = myPointOnCurve.XY() - myPRef;
    theVal          = aVt.Crossed(myDirRef);
    theD1           = aD1.Crossed(myDirRef);
    theD2           = 2.0 * (theD1 * theD1 + theVal * aD2.Crossed(myDirRef));
    theD1 *= 2.0 * theVal;
    theVal *= theVal / mySqMod;
  }

  //! Returns TRUE if the function has been initializes correctly.
  Standard_Boolean IsValid() const { return myIsValid; }

  //! Returns number of variables
  virtual Standard_Integer NbVariables() const Standard_OVERRIDE { return 1; }

  //! Returns last computed Point in the given curve.
  //! Its value will be recomputed after calling UpdateFields(...) method,
  //! which sets this point correspond to the input parameter.
  const gp_Pnt2d& PointOnCurve() const { return myPointOnCurve; }

  //! Returns last computed vector directed from some point on the curve
  //! to the given line. This vector is correspond to the found deviation.
  //! Its value will be recomputed after calling UpdateFields(...) method,
  //! which set this vector correspond to the input parameter.
  const gp_Vec2d& VecCurveLine() const { return myVecCurvLine; }

  //! Returns the given line
  void GetLine(gp_Lin2d* const theLine) const
  {
    if (theLine == nullptr)
    {
      return;
    }
    theLine->SetDirection(myDirRef);
    theLine->SetLocation(myPRef);
  }

  //! Returns value of *this (square deviation)
  virtual Standard_Boolean Value(const math_Vector& thePrm, Standard_Real& theVal) Standard_OVERRIDE
  {
    Standard_Real aD1;
    Standard_Real aD2;
    ValueAndDerives(thePrm.Value(thePrm.Lower()), theVal, aD1, aD2);
    theVal = -theVal;
    return Standard_True;
  }

  //! Always returns 0. It is used for compatibility with the parent class.
  virtual Standard_Integer GetStateNumber() Standard_OVERRIDE { return 0; }

private:
  //! The curve
  Geom2dAdaptor_Curve myCurve;

  //! Square modulus of myDirRef (it is constant)
  Standard_Real mySqMod;

  //! TRUE if *this is initialized correctly
  Standard_Boolean myIsValid;

  //! Sets the given line
  gp_XY myPRef, myDirRef;

  //! Last computed point in the curve
  gp_Pnt2d myPointOnCurve;

  //! Always directed from myPointOnCurve to the line
  gp_Vec2d myVecCurvLine;
};
} // namespace

//=======================================================================
// function : ComputeDeviation
// purpose  : Computes parameter on curve (*thePrmOnCurve) where maximal deviation
//           (maximal value of correspond function FuncSolveDeviation) is obtained.
//           ALGORITHM!
//           The point is looked for where 1st derivative of the function
//            FuncSolveDeviation is equal to 0. It is made by iterative formula:
//
//                U(n+1)=U(n) - D1/D2,
//
//            where D1 and D2 are 1st and 2nd derivative of the function, computed in
//            the point U(n). U(0) = theStartParameter.
//=======================================================================
Standard_Real GeomLib_Tool::ComputeDeviation(const Geom2dAdaptor_Curve& theCurve,
                                             const Standard_Real        theFPar,
                                             const Standard_Real        theLPar,
                                             const Standard_Real        theStartParameter,
                                             const Standard_Integer     theNbIters,
                                             Standard_Real* const       thePrmOnCurve,
                                             gp_Pnt2d* const            thePtOnCurve,
                                             gp_Vec2d* const            theVecCurvLine,
                                             gp_Lin2d* const            theLine)
{
  // Computed maximal deflection
  if ((theStartParameter < theFPar) || (theStartParameter > theLPar))
  {
    return -1.0;
  }

  const gp_Pnt2d aPf(theCurve.Value(theFPar));
  const gp_Pnt2d aPl(theCurve.Value(theLPar));

  FuncSolveDeviation aFunc(theCurve, aPf.XY(), aPl.XY());

  if (!aFunc.IsValid())
  {
    return -1.0;
  }

  aFunc.GetLine(theLine);

  constexpr Standard_Real aTolDefl = Precision::PConfusion();

  Standard_Real aD1   = 0.0;
  Standard_Real aD2   = 0.0;
  Standard_Real aU0   = theStartParameter;
  Standard_Real aUmax = theStartParameter;
  Standard_Real aSqDefl;
  aFunc.ValueAndDerives(aU0, aSqDefl, aD1, aD2);
  for (Standard_Integer anItr = 1; anItr <= theNbIters; anItr++)
  {
    if (Abs(aD2) < Precision::PConfusion())
    {
      break;
    }
    const Standard_Real aDelta = aD1 / aD2;
    const Standard_Real aU1    = aU0 - aDelta;

    if ((aU1 < theFPar) || (aU1 > theLPar))
    {
      break;
    }
    Standard_Real aSqD = aSqDefl;
    aFunc.ValueAndDerives(aU1, aSqD, aD1, aD2);
    if (aSqD > aSqDefl)
    {
      aUmax                   = aU1;
      const Standard_Real aDD = aSqDefl > 0.0 ? Abs(Sqrt(aSqD) - Sqrt(aSqDefl)) : Sqrt(aSqD);
      aSqDefl                 = aSqD;
      if (aDD < aTolDefl)
      {
        break;
      }
    }

    if (Abs(aU0 - aU1) < Precision::PConfusion())
    {
      break;
    }
    aU0 = aU1;
  }
  if (aSqDefl < 0.0)
  {
    return aSqDefl;
  }
  if (thePrmOnCurve)
  {
    *thePrmOnCurve = aUmax;
  }
  if ((thePtOnCurve != nullptr) || (theVecCurvLine != nullptr))
  {
    aFunc.UpdateFields(aUmax);

    if (thePtOnCurve != nullptr)
    {
      thePtOnCurve->SetXY(aFunc.PointOnCurve().XY());
    }

    if (theVecCurvLine != nullptr)
    {
      theVecCurvLine->SetXY(aFunc.VecCurveLine().XY());
    }
  }
  return Sqrt(aSqDefl);
}

//=======================================================================
// function : ComputeDeviation
// purpose  : Computes parameter on curve (*thePrmOnCurve) where maximal deviation
//           (maximal value of correspond function FuncSolveDeviation) is obtained
//           (fast but not precisely).
//           math_PSO Algorithm is used.
//=======================================================================
Standard_Real GeomLib_Tool::ComputeDeviation(const Geom2dAdaptor_Curve& theCurve,
                                             const Standard_Real        theFPar,
                                             const Standard_Real        theLPar,
                                             const Standard_Integer     theNbSubIntervals,
                                             const Standard_Integer     theNbIters,
                                             Standard_Real* const       thePrmOnCurve)
{
  // Computed maximal deflection
  const gp_Pnt2d aPf(theCurve.Value(theFPar));
  const gp_Pnt2d aPl(theCurve.Value(theLPar));

  FuncSolveDeviation aFunc(theCurve, aPf.XY(), aPl.XY());

  if (!aFunc.IsValid())
  {
    return -1.0;
  }
  const math_Vector aFPar(1, 1, theFPar);
  const math_Vector aLPar(1, 1, theLPar);
  const math_Vector aStep(1, 1, (theLPar - theFPar) / (10.0 * theNbSubIntervals));
  math_Vector       anOutputPnt(1, 1, theFPar);
  math_PSO          aMPSO(&aFunc, aFPar, aLPar, aStep, theNbSubIntervals, theNbIters);

  Standard_Real aSqDefl = RealLast();
  aMPSO.Perform(aStep, aSqDefl, anOutputPnt, theNbIters);

  if (aSqDefl == RealLast())
  {
    return -1.0;
  }
  if (thePrmOnCurve)
  {
    *thePrmOnCurve = anOutputPnt(1);
  }
  return Sqrt(Abs(aSqDefl));
}