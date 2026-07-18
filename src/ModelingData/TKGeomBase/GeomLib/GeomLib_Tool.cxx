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
#include <Geom_Circle.hxx>
#include <Geom_Ellipse.hxx>
#include <Geom_Hyperbola.hxx>
#include <Geom_Line.hxx>
#include <Geom_Parabola.hxx>
#include <Geom_TrimmedCurve.hxx>
#include <GeomAdaptor_Curve.hxx>
#include <GeomAdaptor_Surface.hxx>
#include <math_PSO.hxx>

// The functions Parameter(s) are used to compute parameter(s) of point
// on curves and surfaces. The main rule is that tested point must lied
// on curves or surfaces otherwise the resulted parameter(s) may be wrong.
// To make search process more common the MaxDist value is used to define
// the proximity of point to curve or surface. It is clear that this MaxDist
// value can't be too high to be not in conflict with previous rule.
static const double PARTOLERANCE = 1.e-9;

//=================================================================================================

bool GeomLib_Tool::Parameter(const occ::handle<Geom_Curve>& Curve,
                             const gp_Pnt&                  Point,
                             const double                   MaxDist,
                             double&                        U)
{
  if (Curve.IsNull())
  {
    return false;
  }
  //
  U           = 0.;
  double aTol = MaxDist * MaxDist;

  // Analytic curve types already have an O(1) closed-form parameter
  // computation in ElCLib, so use that instead of the iterative
  // Extrema_ExtPC search below when the curve (or its basis curve, if
  // trimmed) is a Line, Circle, Ellipse, Hyperbola or Parabola. The
  // closed-form result is only kept if it lands within the curve's own
  // range, accounting for periodicity. A trimmed arc whose true closest
  // point falls outside the trimmed portion still goes through the
  // generic search below.
  {
    occ::handle<Geom_Curve> aBasisCurve = Curve;
    while (aBasisCurve->IsKind(STANDARD_TYPE(Geom_TrimmedCurve)))
    {
      aBasisCurve = occ::down_cast<Geom_TrimmedCurve>(aBasisCurve)->BasisCurve();
    }

    bool   hasFastU = true;
    double aFastU   = 0.0;
    if (aBasisCurve->IsKind(STANDARD_TYPE(Geom_Line)))
    {
      aFastU = ElCLib::Parameter(occ::down_cast<Geom_Line>(aBasisCurve)->Lin(), Point);
    }
    else if (aBasisCurve->IsKind(STANDARD_TYPE(Geom_Circle)))
    {
      aFastU = ElCLib::Parameter(occ::down_cast<Geom_Circle>(aBasisCurve)->Circ(), Point);
    }
    else if (aBasisCurve->IsKind(STANDARD_TYPE(Geom_Ellipse)))
    {
      aFastU = ElCLib::Parameter(occ::down_cast<Geom_Ellipse>(aBasisCurve)->Elips(), Point);
    }
    else if (aBasisCurve->IsKind(STANDARD_TYPE(Geom_Hyperbola)))
    {
      aFastU = ElCLib::Parameter(occ::down_cast<Geom_Hyperbola>(aBasisCurve)->Hypr(), Point);
    }
    else if (aBasisCurve->IsKind(STANDARD_TYPE(Geom_Parabola)))
    {
      aFastU = ElCLib::Parameter(occ::down_cast<Geom_Parabola>(aBasisCurve)->Parab(), Point);
    }
    else
    {
      hasFastU = false;
    }

    if (hasFastU)
    {
      if (Curve->IsPeriodic())
      {
        aFastU = ElCLib::InPeriod(aFastU,
                                  Curve->FirstParameter(),
                                  Curve->FirstParameter() + Curve->Period());
      }
      if (aFastU >= Curve->FirstParameter() - PARTOLERANCE
          && aFastU <= Curve->LastParameter() + PARTOLERANCE)
      {
        const gp_Pnt aFastP = Curve->Value(aFastU);
        if (aFastP.SquareDistance(Point) <= aTol)
        {
          U = aFastU;
          return true;
        }
        return false;
      }
    }
  }
  //
  GeomAdaptor_Curve aGAC(Curve);
  Extrema_ExtPC     extrema(Point, aGAC);
  //
  if (!extrema.IsDone())
  {
    return false;
  }
  //
  int n = extrema.NbExt();
  if (n <= 0)
  {
    return false;
  }
  //
  int    i = 0, iMin = 0;
  double Dist2Min = RealLast();
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
    return false;
  }

  return true;
}

//=================================================================================================

bool GeomLib_Tool::Parameters(const occ::handle<Geom_Surface>& Surface,
                              const gp_Pnt&                    Point,
                              const double                     MaxDist,
                              double&                          U,
                              double&                          V)
{
  if (Surface.IsNull())
  {
    return false;
  }
  //
  U           = 0.;
  V           = 0.;
  double aTol = MaxDist * MaxDist;
  //
  GeomAdaptor_Surface aGAS(Surface);
  double              aTolU = PARTOLERANCE, aTolV = PARTOLERANCE;
  //
  Extrema_ExtPS extrema(Point, aGAS, aTolU, aTolV, Extrema_ExtFlag_MIN);
  //
  if (!extrema.IsDone())
  {
    return false;
  }
  //
  int n = extrema.NbExt();
  if (n <= 0)
  {
    return false;
  }
  //
  double Dist2Min = RealLast();
  int    i = 0, iMin = 0;
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
    return false;
  }

  return true;
}

//=================================================================================================

bool GeomLib_Tool::Parameter(const occ::handle<Geom2d_Curve>& Curve,
                             const gp_Pnt2d&                  Point,
                             const double                     MaxDist,
                             double&                          U)
{
  if (Curve.IsNull())
  {
    return false;
  }
  //
  U           = 0.;
  double aTol = MaxDist * MaxDist;
  //
  Geom2dAdaptor_Curve aGAC(Curve);
  Extrema_ExtPC2d     extrema(Point, aGAC);
  if (!extrema.IsDone())
  {
    return false;
  }
  int n = extrema.NbExt();
  if (n <= 0)
  {
    return false;
  }
  int    i = 0, iMin = 0;
  double Dist2Min = RealLast();
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
    return false;
  }

  return true;
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
  void UpdateFields(const double theParam)
  {
    myCurve.D0(theParam, myPointOnCurve);
    const gp_XY aVt = myPointOnCurve.XY() - myPRef;
    myVecCurvLine   = aVt.Dot(myDirRef) * myDirRef / mySqMod - aVt;
  }

  //! Returns value of *this (square deviation) and its 1st and 2nd derivative.
  void ValueAndDerives(const double theParam, double& theVal, double& theD1, double& theD2)
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
  bool IsValid() const { return myIsValid; }

  //! Returns number of variables
  int NbVariables() const override { return 1; }

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
  bool Value(const math_Vector& thePrm, double& theVal) override
  {
    double aD1;
    double aD2;
    ValueAndDerives(thePrm.Value(thePrm.Lower()), theVal, aD1, aD2);
    theVal = -theVal;
    return true;
  }

  //! Always returns 0. It is used for compatibility with the parent class.
  int GetStateNumber() override { return 0; }

private:
  //! The curve
  Geom2dAdaptor_Curve myCurve;

  //! Square modulus of myDirRef (it is constant)
  double mySqMod;

  //! TRUE if *this is initialized correctly
  bool myIsValid;

  //! Sets the given line
  gp_XY myPRef, myDirRef;

  //! Last computed point in the curve
  gp_Pnt2d myPointOnCurve;

  //! Always directed from myPointOnCurve to the line
  gp_Vec2d myVecCurvLine;
};
} // namespace

//=================================================================================================
double GeomLib_Tool::ComputeDeviation(const Geom2dAdaptor_Curve& theCurve,
                                      const double               theFPar,
                                      const double               theLPar,
                                      const double               theStartParameter,
                                      const int                  theNbIters,
                                      double* const              thePrmOnCurve,
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

  constexpr double aTolDefl = Precision::PConfusion();

  double aD1   = 0.0;
  double aD2   = 0.0;
  double aU0   = theStartParameter;
  double aUmax = theStartParameter;
  double aSqDefl;
  aFunc.ValueAndDerives(aU0, aSqDefl, aD1, aD2);
  for (int anItr = 1; anItr <= theNbIters; anItr++)
  {
    if (std::abs(aD2) < Precision::PConfusion())
    {
      break;
    }
    const double aDelta = aD1 / aD2;
    const double aU1    = aU0 - aDelta;

    if ((aU1 < theFPar) || (aU1 > theLPar))
    {
      break;
    }
    double aSqD = aSqDefl;
    aFunc.ValueAndDerives(aU1, aSqD, aD1, aD2);
    if (aSqD > aSqDefl)
    {
      aUmax = aU1;
      const double aDD =
        aSqDefl > 0.0 ? std::abs(std::sqrt(aSqD) - std::sqrt(aSqDefl)) : std::sqrt(aSqD);
      aSqDefl = aSqD;
      if (aDD < aTolDefl)
      {
        break;
      }
    }

    if (std::abs(aU0 - aU1) < Precision::PConfusion())
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
  return std::sqrt(aSqDefl);
}

//=================================================================================================
double GeomLib_Tool::ComputeDeviation(const Geom2dAdaptor_Curve& theCurve,
                                      const double               theFPar,
                                      const double               theLPar,
                                      const int                  theNbSubIntervals,
                                      const int                  theNbIters,
                                      double* const              thePrmOnCurve)
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

  double aSqDefl = RealLast();
  aMPSO.Perform(aStep, aSqDefl, anOutputPnt, theNbIters);

  if (aSqDefl == RealLast())
  {
    return -1.0;
  }
  if (thePrmOnCurve)
  {
    *thePrmOnCurve = anOutputPnt(1);
  }
  return std::sqrt(std::abs(aSqDefl));
}