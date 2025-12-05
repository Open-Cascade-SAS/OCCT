// Created on: 1991-02-26
// Created by: Isabelle GRIGNON
// Copyright (c) 1991-1999 Matra Datavision
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

#ifndef _Extrema_GFuncExtPC_HeaderFile
#define _Extrema_GFuncExtPC_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>

#include <GeomAbs_CurveType.hxx>
#include <math_FunctionWithDerivative.hxx>
#include <Precision.hxx>
#include <Standard_TypeMismatch.hxx>
#include <TColStd_SequenceOfInteger.hxx>
#include <TColStd_SequenceOfReal.hxx>

#include <cmath>

//! Template class for computing extremal distance function between a point and a curve.
//! Searches for a parameter value u such that dist(P, C(u)) passes through an extremum.
//! Inherits from math_FunctionWithDerivative and is used by math_FunctionRoot and
//! math_FunctionRoots algorithms.
//!
//! If D1c and D2c are the first and second derivatives:
//! F(u) = (C(u)-P).D1c(u) / ||D1c||
//! DF(u) = ||D1c|| + (C(u)-P).D2c(u)/||D1c|| - F(u)*D2c.D1c/||D1c||^2
//!
//! @tparam TheCurve    Curve type (e.g., Adaptor3d_Curve, Adaptor2d_Curve2d)
//! @tparam TheCurveTool Tool for curve operations
//! @tparam ThePOnC     Point on curve type
//! @tparam ThePoint    Point type (e.g., gp_Pnt, gp_Pnt2d)
//! @tparam TheVector   Vector type (e.g., gp_Vec, gp_Vec2d)
//! @tparam TheSeqPOnC  Sequence of points on curve type
template <typename TheCurve,
          typename TheCurveTool,
          typename ThePOnC,
          typename ThePoint,
          typename TheVector,
          typename TheSeqPOnC>
class Extrema_GFuncExtPC : public math_FunctionWithDerivative
{
public:
  DEFINE_STANDARD_ALLOC

  //! Default constructor.
  Extrema_GFuncExtPC()
      : myC(nullptr),
        myU(0.0),
        myD1f(0.0),
        myPinit(false),
        myCinit(false),
        myD1Init(false),
        myTol(MinTol),
        myMaxDerivOrder(0),
        myUinfium(0.0),
        myUsupremum(0.0)
  {
  }

  //! Constructor with point and curve initialization.
  //! @param theP Point to compute distance from
  //! @param theC Curve to compute distance to
  Extrema_GFuncExtPC(const ThePoint& theP, const TheCurve& theC)
      : myP(theP),
        myC(const_cast<TheCurve*>(&theC)),
        myU(0.0),
        myD1f(0.0),
        myPinit(true),
        myCinit(true),
        myD1Init(false)
  {
    SubIntervalInitialize(TheCurveTool::FirstParameter(theC), TheCurveTool::LastParameter(theC));

    switch (TheCurveTool::GetType(theC))
    {
      case GeomAbs_BezierCurve:
      case GeomAbs_BSplineCurve:
      case GeomAbs_OffsetCurve:
      case GeomAbs_OtherCurve:
        myMaxDerivOrder = MaxOrder;
        myTol           = SearchOfTolerance();
        break;
      default:
        myMaxDerivOrder = 0;
        myTol           = MinTol;
        break;
    }
  }

  //! Sets the curve field.
  //! @param theC Curve to set
  void Initialize(const TheCurve& theC)
  {
    myC     = const_cast<TheCurve*>(&theC);
    myCinit = true;
    myPoint.Clear();
    mySqDist.Clear();
    myIsMin.Clear();

    SubIntervalInitialize(TheCurveTool::FirstParameter(theC), TheCurveTool::LastParameter(theC));

    switch (TheCurveTool::GetType(theC))
    {
      case GeomAbs_BezierCurve:
      case GeomAbs_BSplineCurve:
      case GeomAbs_OffsetCurve:
      case GeomAbs_OtherCurve:
        myMaxDerivOrder = MaxOrder;
        myTol           = SearchOfTolerance();
        break;
      default:
        myMaxDerivOrder = 0;
        myTol           = MinTol;
        break;
    }
  }

  //! Sets the point field.
  //! @param theP Point to set
  void SetPoint(const ThePoint& theP)
  {
    myP     = theP;
    myPinit = true;
    myPoint.Clear();
    mySqDist.Clear();
    myIsMin.Clear();
  }

  //! Calculation of F(u).
  //! @param theU Parameter value
  //! @param theF Output function value
  //! @return True if computation succeeded
  bool Value(const double theU, double& theF) override
  {
    if (!myPinit || !myCinit)
    {
      throw Standard_TypeMismatch("No init");
    }

    myU = theU;
    TheVector D1c;
    TheCurveTool::D1(*myC, myU, myPc, D1c);

    if (Precision::IsInfinite(D1c.X()) || Precision::IsInfinite(D1c.Y()))
    {
      theF = Precision::Infinite();
      return false;
    }

    double Ndu = D1c.Magnitude();

    if (myMaxDerivOrder != 0)
    {
      if (Ndu <= myTol) // Singular case
      {
        const double DivisionFactor = 1.e-3;
        double       du;
        if ((myUsupremum >= RealLast()) || (myUinfium <= RealFirst()))
          du = 0.0;
        else
          du = myUsupremum - myUinfium;

        const double aDelta = std::max(du * DivisionFactor, MinStep);
        // Derivative is approximated by Taylor-series

        int       n = 1; // Derivative order
        TheVector V;
        bool      IsDeriveFound;

        do
        {
          V             = TheCurveTool::DN(*myC, myU, ++n);
          Ndu           = V.Magnitude();
          IsDeriveFound = (Ndu > myTol);
        } while (!IsDeriveFound && n < myMaxDerivOrder);

        if (IsDeriveFound)
        {
          double u;

          if (myU - myUinfium < aDelta)
            u = myU + aDelta;
          else
            u = myU - aDelta;

          ThePoint P1, P2;
          TheCurveTool::D0(*myC, std::min(myU, u), P1);
          TheCurveTool::D0(*myC, std::max(myU, u), P2);

          TheVector V1(P1, P2);
          double    aDirFactor = V.Dot(V1);

          if (aDirFactor < 0.0)
            D1c = -V;
          else
            D1c = V;
        }
        else
        {
          // Derivative is approximated by three points
          ThePoint Ptemp;
          ThePoint P1, P2, P3;
          bool     IsParameterGrown;

          if (myU - myUinfium < 2 * aDelta)
          {
            TheCurveTool::D0(*myC, myU, P1);
            TheCurveTool::D0(*myC, myU + aDelta, P2);
            TheCurveTool::D0(*myC, myU + 2 * aDelta, P3);
            IsParameterGrown = true;
          }
          else
          {
            TheCurveTool::D0(*myC, myU - 2 * aDelta, P1);
            TheCurveTool::D0(*myC, myU - aDelta, P2);
            TheCurveTool::D0(*myC, myU, P3);
            IsParameterGrown = false;
          }

          TheVector V1(Ptemp, P1), V2(Ptemp, P2), V3(Ptemp, P3);

          if (IsParameterGrown)
            D1c = -3 * V1 + 4 * V2 - V3;
          else
            D1c = V1 - 4 * V2 + 3 * V3;
        }
        Ndu = D1c.Magnitude();
      }
    }

    if (Ndu <= MinTol)
    {
      // Warning: 1st derivative is equal to zero!
      return false;
    }

    TheVector PPc(myP, myPc);
    theF = PPc.Dot(D1c) / Ndu;
    return true;
  }

  //! Calculation of F'(u).
  //! @param theU Parameter value
  //! @param theDF Output derivative value
  //! @return True if computation succeeded
  bool Derivative(const double theU, double& theDF) override
  {
    if (!myPinit || !myCinit)
    {
      throw Standard_TypeMismatch();
    }
    double F;
    return Values(theU, F, theDF);
  }

  //! Calculation of F(u) and F'(u).
  //! @param theU Parameter value
  //! @param theF Output function value
  //! @param theDF Output derivative value
  //! @return True if computation succeeded
  bool Values(const double theU, double& theF, double& theDF) override
  {
    if (!myPinit || !myCinit)
    {
      throw Standard_TypeMismatch("No init");
    }

    ThePoint myPc_old = myPc, myP_old = myP;

    if (Value(theU, theF) == false)
    {
      myD1Init = false;
      return false;
    }

    myU  = theU;
    myPc = myPc_old;
    myP  = myP_old;

    TheVector D1c, D2c;
    TheCurveTool::D2(*myC, myU, myPc, D1c, D2c);

    double Ndu = D1c.Magnitude();
    if (Ndu <= myTol) // Singular case
    {
      // Derivative is approximated by three points
      const double DivisionFactor = 0.01;
      double       du;
      if ((myUsupremum >= RealLast()) || (myUinfium <= RealFirst()))
        du = 0.0;
      else
        du = myUsupremum - myUinfium;

      const double aDelta = std::max(du * DivisionFactor, MinStep);

      double F1, F2, F3;

      if (myU - myUinfium < 2 * aDelta)
      {
        F1              = theF;
        const double U2 = myU + aDelta;
        const double U3 = myU + aDelta * 2.0;

        if (!((Value(U2, F2)) && (Value(U3, F3))))
        {
          myD1Init = false;
          return false;
        }

        theDF = (-3 * F1 + 4 * F2 - F3) / (2.0 * aDelta);
      }
      else
      {
        F3              = theF;
        const double U1 = myU - aDelta * 2.0;
        const double U2 = myU - aDelta;

        if (!((Value(U2, F2)) && (Value(U1, F1))))
        {
          myD1Init = false;
          return false;
        }

        theDF = (F1 - 4 * F2 + 3 * F3) / (2.0 * aDelta);
      }
      myU  = theU;
      myPc = myPc_old;
      myP  = myP_old;
    }
    else
    {
      TheVector PPc(myP, myPc);
      theDF = Ndu + (PPc.Dot(D2c) / Ndu) - theF * (D1c.Dot(D2c)) / (Ndu * Ndu);
    }

    myD1f = theDF;

    myD1Init = true;
    return true;
  }

  //! Save the found extremum.
  //! @return State number
  int GetStateNumber() override
  {
    if (!myPinit || !myCinit)
    {
      throw Standard_TypeMismatch();
    }
    mySqDist.Append(myPc.SquareDistance(myP));

    // It is necessary to always compute myD1f.
    myD1Init = true;
    double FF, DD;
    Values(myU, FF, DD);

    int IntVal = 0;
    if (myD1f > 0.0)
    {
      IntVal = 1;
    }

    myIsMin.Append(IntVal);
    myPoint.Append(ThePOnC(myU, myPc));
    return 0;
  }

  //! Return the number of found extrema.
  int NbExt() const { return mySqDist.Length(); }

  //! Returns the Nth square distance.
  //! @param theN Index of extremum
  double SquareDistance(const int theN) const
  {
    if (!myPinit || !myCinit)
    {
      throw Standard_TypeMismatch();
    }
    return mySqDist.Value(theN);
  }

  //! Shows if the Nth distance is a minimum.
  //! @param theN Index of extremum
  bool IsMin(const int theN) const
  {
    if (!myPinit || !myCinit)
    {
      throw Standard_TypeMismatch();
    }
    return (myIsMin.Value(theN) == 1);
  }

  //! Returns the Nth extremum point.
  //! @param theN Index of extremum
  const ThePOnC& Point(const int theN) const
  {
    if (!myPinit || !myCinit)
    {
      throw Standard_TypeMismatch();
    }
    return myPoint.Value(theN);
  }

  //! Determines boundaries of subinterval for root finding.
  //! @param theUfirst First parameter bound
  //! @param theUlast Last parameter bound
  void SubIntervalInitialize(const double theUfirst, const double theUlast)
  {
    myUinfium   = theUfirst;
    myUsupremum = theUlast;
  }

  //! Computes a tolerance value. If 1st derivative of curve |D1| < Tol,
  //! it is considered D1=0.
  double SearchOfTolerance()
  {
    const int    NPoint = 10;
    const double aStep  = (myUsupremum - myUinfium) / static_cast<double>(NPoint);

    int    aNum = 0;
    double aMax = -Precision::Infinite();

    do
    {
      double u = myUinfium + aNum * aStep;
      if (u > myUsupremum)
        u = myUsupremum;

      ThePoint  Ptemp;
      TheVector VDer;
      TheCurveTool::D1(*myC, u, Ptemp, VDer);

      if (Precision::IsInfinite(VDer.X()) || Precision::IsInfinite(VDer.Y()))
      {
        continue;
      }

      double vm = VDer.Magnitude();
      if (vm > aMax)
        aMax = vm;
    } while (++aNum < NPoint + 1);

    return std::max(aMax * TolFactor, MinTol);
  }

private:
  static constexpr double TolFactor = 1.e-12;
  static constexpr double MinTol    = 1.e-20;
  static constexpr double MinStep   = 1.e-7;
  static constexpr int    MaxOrder  = 3;

  ThePoint                  myP;
  TheCurve*                 myC;
  double                    myU;
  ThePoint                  myPc;
  double                    myD1f;
  TColStd_SequenceOfReal    mySqDist;
  TColStd_SequenceOfInteger myIsMin;
  TheSeqPOnC                myPoint;
  bool                      myPinit;
  bool                      myCinit;
  bool                      myD1Init;
  double                    myTol;
  int                       myMaxDerivOrder;
  double                    myUinfium;
  double                    myUsupremum;
};

#endif // _Extrema_GFuncExtPC_HeaderFile
