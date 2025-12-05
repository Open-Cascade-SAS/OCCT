// Copyright (c) 1995-1999 Matra Datavision
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

#ifndef _Extrema_GFuncExtCC_HeaderFile
#define _Extrema_GFuncExtCC_HeaderFile

#include <GeomAbs_CurveType.hxx>
#include <math_FunctionSetWithDerivatives.hxx>
#include <math_Matrix.hxx>
#include <math_Vector.hxx>
#include <Precision.hxx>
#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_OutOfRange.hxx>
#include <TColStd_SequenceOfReal.hxx>

//! Template class for function used to find extremal distance between two curves.
//! This class inherits from math_FunctionSetWithDerivatives and is used by
//! the algorithm math_FunctionSetRoot.
//!
//! @tparam TheCurve1 Type of the first curve (e.g., Adaptor3d_Curve)
//! @tparam TheCurveTool1 Tool class for the first curve
//! @tparam TheCurve2 Type of the second curve
//! @tparam TheCurveTool2 Tool class for the second curve
//! @tparam ThePOnC Point on curve type (e.g., Extrema_POnCurv)
//! @tparam ThePoint Point type (e.g., gp_Pnt)
//! @tparam TheVector Vector type (e.g., gp_Vec)
//! @tparam TheSequenceOfPOnC Sequence of points on curve
template <typename TheCurve1,
          typename TheCurveTool1,
          typename TheCurve2,
          typename TheCurveTool2,
          typename ThePOnC,
          typename ThePoint,
          typename TheVector,
          typename TheSequenceOfPOnC>
class Extrema_GFuncExtCC : public math_FunctionSetWithDerivatives
{
public:
  DEFINE_STANDARD_ALLOC

  //! Default constructor with tolerance.
  Extrema_GFuncExtCC(const double theTol = 1.0e-10);

  //! Constructor with curves.
  Extrema_GFuncExtCC(const TheCurve1& theC1, const TheCurve2& theC2, const double theTol = 1.0e-10);

  //! Sets the curve for the specified rank (1 or 2).
  void SetCurve(const int theRank, const TheCurve1& theC);

  //! Sets the tolerance.
  void SetTolerance(const double theTol) { myTol = theTol; }

  //! Returns the number of variables (2).
  virtual int NbVariables() const Standard_OVERRIDE { return 2; }

  //! Returns the number of equations (2).
  virtual int NbEquations() const Standard_OVERRIDE { return 2; }

  //! Calculate Fi(U,V).
  virtual Standard_Boolean Value(const math_Vector& theUV, math_Vector& theF) Standard_OVERRIDE;

  //! Calculate Fi'(U,V).
  Standard_Boolean Derivatives(const math_Vector& theUV, math_Matrix& theDF) Standard_OVERRIDE;

  //! Calculate Fi(U,V) and Fi'(U,V).
  Standard_Boolean Values(const math_Vector& theUV,
                          math_Vector&       theF,
                          math_Matrix&       theDF) Standard_OVERRIDE;

  //! Save the found extremum.
  virtual int GetStateNumber() Standard_OVERRIDE;

  //! Return the number of found extrema.
  int NbExt() const { return mySqDist.Length(); }

  //! Return the value of the Nth distance.
  double SquareDistance(const int theN) const { return mySqDist.Value(theN); }

  //! Return the points of the Nth extreme distance.
  void Points(const int theN, ThePOnC& theP1, ThePOnC& theP2) const;

  //! Returns a pointer to the curve specified in the constructor or in SetCurve() method.
  Standard_Address CurvePtr(const int theRank) const
  {
    Standard_OutOfRange_Raise_if(theRank < 1 || theRank > 2, "Extrema_GFuncExtCC::CurvePtr()");
    return (theRank == 1 ? myC1 : myC2);
  }

  //! Returns a tolerance specified in the constructor or in SetTolerance() method.
  double Tolerance() const { return myTol; }

  //! Determines boundaries of subinterval for find of root.
  void SubIntervalInitialize(const math_Vector& theUfirst, const math_Vector& theUlast);

  //! Computes a Tol value. If 1st derivative of curve |D1|<Tol, it is considered D1=0.
  double SearchOfTolerance(const Standard_Address theC);

private:
  static constexpr double THE_MIN_TOL    = 1.e-20;
  static constexpr double THE_TOL_FACTOR = 1.e-12;
  static constexpr double THE_MIN_STEP   = 1e-7;
  static constexpr int    THE_MAX_ORDER  = 3;

  Standard_Address       myC1;
  Standard_Address       myC2;
  double                 myTol;
  double                 myU;
  double                 myV;
  ThePoint               myP1;
  ThePoint               myP2;
  TheVector              myDu;
  TheVector              myDv;
  TColStd_SequenceOfReal mySqDist;
  TheSequenceOfPOnC      myPoints;
  double                 myTolC1;
  double                 myTolC2;
  int                    myMaxDerivOrderC1;
  int                    myMaxDerivOrderC2;
  double                 myUinfium;
  double                 myUsupremum;
  double                 myVinfium;
  double                 myVsupremum;
};

//==================================================================================================
// Implementation
//==================================================================================================

//==================================================================================================

template <typename TheCurve1,
          typename TheCurveTool1,
          typename TheCurve2,
          typename TheCurveTool2,
          typename ThePOnC,
          typename ThePoint,
          typename TheVector,
          typename TheSequenceOfPOnC>
double Extrema_GFuncExtCC<TheCurve1,
                          TheCurveTool1,
                          TheCurve2,
                          TheCurveTool2,
                          ThePOnC,
                          ThePoint,
                          TheVector,
                          TheSequenceOfPOnC>::SearchOfTolerance(const Standard_Address theC)
{
  const int NPoint = 10;
  double    aStartParam, anEndParam;

  if (theC == myC1)
  {
    aStartParam = myUinfium;
    anEndParam  = myUsupremum;
  }
  else if (theC == myC2)
  {
    aStartParam = myVinfium;
    anEndParam  = myVsupremum;
  }
  else
  {
    return THE_MIN_TOL;
  }

  const double aStep = (anEndParam - aStartParam) / (double)NPoint;

  int    aNum = 0;
  double aMax = -Precision::Infinite();

  do
  {
    double u = aStartParam + aNum * aStep;
    if (u > anEndParam)
      u = anEndParam;

    ThePoint  Ptemp;
    TheVector VDer;
    TheCurveTool1::D1(*((TheCurve1*)theC), u, Ptemp, VDer);
    double vm = VDer.Magnitude();
    if (vm > aMax)
      aMax = vm;
  } while (++aNum < NPoint + 1);

  return std::max(aMax * THE_TOL_FACTOR, THE_MIN_TOL);
}

//==================================================================================================

template <typename TheCurve1,
          typename TheCurveTool1,
          typename TheCurve2,
          typename TheCurveTool2,
          typename ThePOnC,
          typename ThePoint,
          typename TheVector,
          typename TheSequenceOfPOnC>
Extrema_GFuncExtCC<TheCurve1,
                   TheCurveTool1,
                   TheCurve2,
                   TheCurveTool2,
                   ThePOnC,
                   ThePoint,
                   TheVector,
                   TheSequenceOfPOnC>::Extrema_GFuncExtCC(const double theTol)
    : myC1(0),
      myC2(0),
      myTol(theTol)
{
  math_Vector V1(1, 2), V2(1, 2);
  V1(1) = 0.0;
  V2(1) = 0.0;
  V1(2) = 0.0;
  V2(2) = 0.0;
  SubIntervalInitialize(V1, V2);
  myMaxDerivOrderC1 = 0;
  myTolC1           = THE_MIN_TOL;
  myMaxDerivOrderC2 = 0;
  myTolC2           = THE_MIN_TOL;
}

//==================================================================================================

template <typename TheCurve1,
          typename TheCurveTool1,
          typename TheCurve2,
          typename TheCurveTool2,
          typename ThePOnC,
          typename ThePoint,
          typename TheVector,
          typename TheSequenceOfPOnC>
Extrema_GFuncExtCC<TheCurve1,
                   TheCurveTool1,
                   TheCurve2,
                   TheCurveTool2,
                   ThePOnC,
                   ThePoint,
                   TheVector,
                   TheSequenceOfPOnC>::Extrema_GFuncExtCC(const TheCurve1& theC1,
                                                          const TheCurve2& theC2,
                                                          const double     theTol)
    : myC1((Standard_Address)&theC1),
      myC2((Standard_Address)&theC2),
      myTol(theTol)
{
  math_Vector V1(1, 2), V2(1, 2);
  V1(1) = TheCurveTool1::FirstParameter(*((TheCurve1*)myC1));
  V2(1) = TheCurveTool1::LastParameter(*((TheCurve1*)myC1));
  V1(2) = TheCurveTool2::FirstParameter(*((TheCurve2*)myC2));
  V2(2) = TheCurveTool2::LastParameter(*((TheCurve2*)myC2));
  SubIntervalInitialize(V1, V2);

  switch (TheCurveTool1::GetType(*((TheCurve1*)myC1)))
  {
    case GeomAbs_BezierCurve:
    case GeomAbs_BSplineCurve:
    case GeomAbs_OffsetCurve:
    case GeomAbs_OtherCurve:
      myMaxDerivOrderC1 = THE_MAX_ORDER;
      myTolC1           = SearchOfTolerance((Standard_Address)&theC1);
      break;
    default:
      myMaxDerivOrderC1 = 0;
      myTolC1           = THE_MIN_TOL;
      break;
  }

  switch (TheCurveTool2::GetType(*((TheCurve2*)myC2)))
  {
    case GeomAbs_BezierCurve:
    case GeomAbs_BSplineCurve:
    case GeomAbs_OffsetCurve:
    case GeomAbs_OtherCurve:
      myMaxDerivOrderC2 = THE_MAX_ORDER;
      myTolC2           = SearchOfTolerance((Standard_Address)&theC2);
      break;
    default:
      myMaxDerivOrderC2 = 0;
      myTolC2           = THE_MIN_TOL;
      break;
  }
}

//==================================================================================================

template <typename TheCurve1,
          typename TheCurveTool1,
          typename TheCurve2,
          typename TheCurveTool2,
          typename ThePOnC,
          typename ThePoint,
          typename TheVector,
          typename TheSequenceOfPOnC>
void Extrema_GFuncExtCC<TheCurve1,
                        TheCurveTool1,
                        TheCurve2,
                        TheCurveTool2,
                        ThePOnC,
                        ThePoint,
                        TheVector,
                        TheSequenceOfPOnC>::SetCurve(const int theRank, const TheCurve1& theC)
{
  Standard_OutOfRange_Raise_if(theRank < 1 || theRank > 2, "Extrema_GFuncExtCC::SetCurve()")

    if (theRank == 1)
  {
    myC1 = (Standard_Address)&theC;
    switch (theC.GetType())
    {
      case GeomAbs_BezierCurve:
      case GeomAbs_BSplineCurve:
      case GeomAbs_OffsetCurve:
      case GeomAbs_OtherCurve:
        myMaxDerivOrderC1 = THE_MAX_ORDER;
        myTolC1           = SearchOfTolerance((Standard_Address)&theC);
        break;
      default:
        myMaxDerivOrderC1 = 0;
        myTolC1           = THE_MIN_TOL;
        break;
    }
  }
  else if (theRank == 2)
  {
    myC2 = (Standard_Address)&theC;
    switch (theC.GetType())
    {
      case GeomAbs_BezierCurve:
      case GeomAbs_BSplineCurve:
      case GeomAbs_OffsetCurve:
      case GeomAbs_OtherCurve:
        myMaxDerivOrderC2 = THE_MAX_ORDER;
        myTolC2           = SearchOfTolerance((Standard_Address)&theC);
        break;
      default:
        myMaxDerivOrderC2 = 0;
        myTolC2           = THE_MIN_TOL;
        break;
    }
  }
}

//==================================================================================================

template <typename TheCurve1,
          typename TheCurveTool1,
          typename TheCurve2,
          typename TheCurveTool2,
          typename ThePOnC,
          typename ThePoint,
          typename TheVector,
          typename TheSequenceOfPOnC>
Standard_Boolean Extrema_GFuncExtCC<TheCurve1,
                                    TheCurveTool1,
                                    TheCurve2,
                                    TheCurveTool2,
                                    ThePOnC,
                                    ThePoint,
                                    TheVector,
                                    TheSequenceOfPOnC>::Value(const math_Vector& theUV,
                                                              math_Vector&       theF)
{
  myU = theUV(1);
  myV = theUV(2);
  TheCurveTool1::D1(*((TheCurve1*)myC1), myU, myP1, myDu);
  TheCurveTool2::D1(*((TheCurve2*)myC2), myV, myP2, myDv);

  TheVector P1P2(myP1, myP2);

  double Ndu = myDu.Magnitude();

  if (myMaxDerivOrderC1 != 0)
  {
    if (Ndu <= myTolC1)
    {
      const double DivisionFactor = 1.e-3;
      double       du;
      if ((myUsupremum >= RealLast()) || (myUinfium <= RealFirst()))
        du = 0.0;
      else
        du = myUsupremum - myUinfium;

      const double aDelta = std::max(du * DivisionFactor, THE_MIN_STEP);

      int              n = 1;
      TheVector        V;
      Standard_Boolean IsDeriveFound;

      do
      {
        V             = TheCurveTool1::DN(*((TheCurve1*)myC1), myU, ++n);
        Ndu           = V.Magnitude();
        IsDeriveFound = (Ndu > myTolC1);
      } while (!IsDeriveFound && n < myMaxDerivOrderC1);

      if (IsDeriveFound)
      {
        double u;

        if (myU - myUinfium < aDelta)
          u = myU + aDelta;
        else
          u = myU - aDelta;

        ThePoint P1, P2;
        TheCurveTool1::D0(*((TheCurve1*)myC1), std::min(myU, u), P1);
        TheCurveTool1::D0(*((TheCurve1*)myC1), std::max(myU, u), P2);

        TheVector V1(P1, P2);
        double    aDirFactor = V.Dot(V1);

        if (aDirFactor < 0.0)
          myDu = -V;
        else
          myDu = V;
      }
      else
      {
        ThePoint         Ptemp;
        ThePoint         P1, P2, P3;
        Standard_Boolean IsParameterGrown;

        if (myU - myUinfium < 2 * aDelta)
        {
          TheCurveTool1::D0(*((TheCurve1*)myC1), myU, P1);
          TheCurveTool1::D0(*((TheCurve1*)myC1), myU + aDelta, P2);
          TheCurveTool1::D0(*((TheCurve1*)myC1), myU + 2 * aDelta, P3);
          IsParameterGrown = Standard_True;
        }
        else
        {
          TheCurveTool1::D0(*((TheCurve1*)myC1), myU - 2 * aDelta, P1);
          TheCurveTool1::D0(*((TheCurve1*)myC1), myU - aDelta, P2);
          TheCurveTool1::D0(*((TheCurve1*)myC1), myU, P3);
          IsParameterGrown = Standard_False;
        }

        TheVector V1(Ptemp, P1), V2(Ptemp, P2), V3(Ptemp, P3);

        if (IsParameterGrown)
          myDu = -3 * V1 + 4 * V2 - V3;
        else
          myDu = V1 - 4 * V2 + 3 * V3;
      }
      Ndu = myDu.Magnitude();
    }
  }

  if (Ndu <= THE_MIN_TOL)
  {
    return Standard_False;
  }

  double Ndv = myDv.Magnitude();

  if (myMaxDerivOrderC2 != 0)
  {
    if (Ndv <= myTolC2)
    {
      const double DivisionFactor = 1.e-3;
      double       dv;
      if ((myVsupremum >= RealLast()) || (myVinfium <= RealFirst()))
        dv = 0.0;
      else
        dv = myVsupremum - myVinfium;

      const double aDelta = std::max(dv * DivisionFactor, THE_MIN_STEP);

      int              n = 1;
      TheVector        V;
      Standard_Boolean IsDeriveFound;

      do
      {
        V             = TheCurveTool2::DN(*((TheCurve2*)myC2), myV, ++n);
        Ndv           = V.Magnitude();
        IsDeriveFound = (Ndv > myTolC2);
      } while (!IsDeriveFound && n < myMaxDerivOrderC2);

      if (IsDeriveFound)
      {
        double v;

        if (myV - myVinfium < aDelta)
          v = myV + aDelta;
        else
          v = myV - aDelta;

        ThePoint P1, P2;
        TheCurveTool2::D0(*((TheCurve2*)myC2), std::min(myV, v), P1);
        TheCurveTool2::D0(*((TheCurve2*)myC2), std::max(myV, v), P2);

        TheVector V1(P1, P2);
        double    aDirFactor = V.Dot(V1);

        if (aDirFactor < 0.0)
          myDv = -V;
        else
          myDv = V;
      }
      else
      {
        ThePoint         Ptemp;
        ThePoint         P1, P2, P3;
        Standard_Boolean IsParameterGrown;

        if (myV - myVinfium < 2 * aDelta)
        {
          TheCurveTool2::D0(*((TheCurve2*)myC2), myV, P1);
          TheCurveTool2::D0(*((TheCurve2*)myC2), myV + aDelta, P2);
          TheCurveTool2::D0(*((TheCurve2*)myC2), myV + 2 * aDelta, P3);
          IsParameterGrown = Standard_True;
        }
        else
        {
          TheCurveTool2::D0(*((TheCurve2*)myC2), myV - 2 * aDelta, P1);
          TheCurveTool2::D0(*((TheCurve2*)myC2), myV - aDelta, P2);
          TheCurveTool2::D0(*((TheCurve2*)myC2), myV, P3);
          IsParameterGrown = Standard_False;
        }

        TheVector V1(Ptemp, P1), V2(Ptemp, P2), V3(Ptemp, P3);

        if (IsParameterGrown)
          myDv = -3 * V1 + 4 * V2 - V3;
        else
          myDv = V1 - 4 * V2 + 3 * V3;
      }

      Ndv = myDv.Magnitude();
    }
  }

  if (Ndv <= THE_MIN_TOL)
  {
    return Standard_False;
  }

  theF(1) = P1P2.Dot(myDu) / Ndu;
  theF(2) = P1P2.Dot(myDv) / Ndv;
  return Standard_True;
}

//==================================================================================================

template <typename TheCurve1,
          typename TheCurveTool1,
          typename TheCurve2,
          typename TheCurveTool2,
          typename ThePOnC,
          typename ThePoint,
          typename TheVector,
          typename TheSequenceOfPOnC>
Standard_Boolean Extrema_GFuncExtCC<TheCurve1,
                                    TheCurveTool1,
                                    TheCurve2,
                                    TheCurveTool2,
                                    ThePOnC,
                                    ThePoint,
                                    TheVector,
                                    TheSequenceOfPOnC>::Derivatives(const math_Vector& theUV,
                                                                    math_Matrix&       theDF)
{
  math_Vector F(1, 2);
  return Values(theUV, F, theDF);
}

//==================================================================================================

template <typename TheCurve1,
          typename TheCurveTool1,
          typename TheCurve2,
          typename TheCurveTool2,
          typename ThePOnC,
          typename ThePoint,
          typename TheVector,
          typename TheSequenceOfPOnC>
Standard_Boolean Extrema_GFuncExtCC<TheCurve1,
                                    TheCurveTool1,
                                    TheCurve2,
                                    TheCurveTool2,
                                    ThePOnC,
                                    ThePoint,
                                    TheVector,
                                    TheSequenceOfPOnC>::Values(const math_Vector& theUV,
                                                               math_Vector&       theF,
                                                               math_Matrix&       theDF)
{
  myU = theUV(1);
  myV = theUV(2);

  if (Value(theUV, theF) == Standard_False)
  {
    return Standard_False;
  }

  TheVector Du, Dv, Duu, Dvv;
  TheCurveTool1::D2(*((TheCurve1*)myC1), myU, myP1, Du, Duu);
  TheCurveTool2::D2(*((TheCurve2*)myC2), myV, myP2, Dv, Dvv);

  const double    myU_old = myU, myV_old = myV;
  const ThePoint  myP1_old = myP1, myP2_old = myP2;
  const TheVector myDu_old = myDu, myDv_old = myDv;

  const double DivisionFactor = 0.01;

  double du;
  if ((myUsupremum >= RealLast()) || (myUinfium <= RealFirst()))
    du = 0.0;
  else
    du = myUsupremum - myUinfium;

  const double aDeltaU = std::max(du * DivisionFactor, THE_MIN_STEP);

  double dv;
  if ((myVsupremum >= RealLast()) || (myVinfium <= RealFirst()))
    dv = 0.0;
  else
    dv = myVsupremum - myVinfium;

  const double aDeltaV = std::max(dv * DivisionFactor, THE_MIN_STEP);

  TheVector P1P2(myP1, myP2);

  if ((myMaxDerivOrderC1 != 0) && (Du.Magnitude() <= myTolC1))
  {
    math_Vector FF1(1, 2), FF2(1, 2), FF3(1, 2);
    double      F1, F2, F3;

    if (myU - myUinfium < 2 * aDeltaU)
    {
      F1 = theF(1);
      math_Vector UV2(1, 2), UV3(1, 2);
      UV2(1) = myU + aDeltaU;
      UV2(2) = myV;
      UV3(1) = myU + 2 * aDeltaU;
      UV3(2) = myV;
      if (!((Value(UV2, FF2)) && (Value(UV3, FF3))))
      {
        return Standard_False;
      }

      F2 = FF2(1);
      F3 = FF3(1);

      theDF(1, 1) = (-3 * F1 + 4 * F2 - F3) / (2.0 * aDeltaU);
    }
    else
    {
      F3 = theF(1);
      math_Vector UV2(1, 2), UV1(1, 2);
      UV2(1) = myU - aDeltaU;
      UV2(2) = myV;
      UV1(1) = myU - 2 * aDeltaU;
      UV1(2) = myV;

      if (!((Value(UV2, FF2)) && (Value(UV1, FF1))))
      {
        return Standard_False;
      }

      F1 = FF1(1);
      F2 = FF2(1);

      theDF(1, 1) = (F1 - 4 * F2 + 3 * F3) / (2.0 * aDeltaU);
    }

    myU = myU_old;
    myV = myV_old;

    if (myV - myVinfium < 2 * aDeltaV)
    {
      F1 = theF(1);
      math_Vector UV2(1, 2), UV3(1, 2);
      UV2(1) = myU;
      UV2(2) = myV + aDeltaV;
      UV3(1) = myU;
      UV3(2) = myV + 2 * aDeltaV;

      if (!((Value(UV2, FF2)) && (Value(UV3, FF3))))
      {
        return Standard_False;
      }
      F2 = FF2(1);
      F3 = FF3(1);

      theDF(1, 2) = (-3 * F1 + 4 * F2 - F3) / (2.0 * aDeltaV);
    }
    else
    {
      F3 = theF(1);
      math_Vector UV2(1, 2), UV1(1, 2);
      UV2(1) = myU;
      UV2(2) = myV - aDeltaV;
      UV1(1) = myU;
      UV1(2) = myV - 2 * aDeltaV;
      if (!((Value(UV2, FF2)) && (Value(UV1, FF1))))
      {
        return Standard_False;
      }

      F1 = FF1(1);
      F2 = FF2(1);

      theDF(1, 2) = (F1 - 4 * F2 + 3 * F3) / (2.0 * aDeltaV);
    }

    myU  = myU_old;
    myV  = myV_old;
    myP1 = myP1_old, myP2 = myP2_old;
    myDu = myDu_old, myDv = myDv_old;
  }
  else
  {
    const double Ndu = myDu.Magnitude();
    theDF(1, 1)      = -Ndu + (P1P2.Dot(Duu) / Ndu) - theF(1) * (myDu.Dot(Duu) / (Ndu * Ndu));
    theDF(1, 2)      = myDv.Dot(myDu) / Ndu;
  }

  if ((myMaxDerivOrderC2 != 0) && (Dv.Magnitude() <= myTolC2))
  {
    math_Vector FF1(1, 2), FF2(1, 2), FF3(1, 2);
    double      F1, F2, F3;

    if (myV - myVinfium < 2 * aDeltaV)
    {
      F1 = theF(2);
      math_Vector UV2(1, 2), UV3(1, 2);
      UV2(1) = myU;
      UV2(2) = myV + aDeltaV;
      UV3(1) = myU;
      UV3(2) = myV + 2 * aDeltaV;

      if (!((Value(UV2, FF2)) && (Value(UV3, FF3))))
      {
        return Standard_False;
      }

      F2 = FF2(2);
      F3 = FF3(2);

      theDF(2, 2) = (-3 * F1 + 4 * F2 - F3) / (2.0 * aDeltaV);
    }
    else
    {
      F3 = theF(2);
      math_Vector UV2(1, 2), UV1(1, 2);
      UV2(1) = myU;
      UV2(2) = myV - aDeltaV;
      UV1(1) = myU;
      UV1(2) = myV - 2 * aDeltaV;

      if (!((Value(UV2, FF2)) && (Value(UV1, FF1))))
      {
        return Standard_False;
      }

      F1 = FF1(2);
      F2 = FF2(2);

      theDF(2, 2) = (F1 - 4 * F2 + 3 * F3) / (2.0 * aDeltaV);
    }

    myU = myU_old;
    myV = myV_old;

    if (myU - myUinfium < 2 * aDeltaU)
    {
      F1 = theF(2);
      math_Vector UV2(1, 2), UV3(1, 2);
      UV2(1) = myU + aDeltaU;
      UV2(2) = myV;
      UV3(1) = myU + 2 * aDeltaU;
      UV3(2) = myV;
      if (!((Value(UV2, FF2)) && (Value(UV3, FF3))))
      {
        return Standard_False;
      }

      F2 = FF2(2);
      F3 = FF3(2);

      theDF(2, 1) = (-3 * F1 + 4 * F2 - F3) / (2.0 * aDeltaU);
    }
    else
    {
      F3 = theF(2);
      math_Vector UV2(1, 2), UV1(1, 2);
      UV2(1) = myU - aDeltaU;
      UV2(2) = myV;
      UV1(1) = myU - 2 * aDeltaU;
      UV1(2) = myV;

      if (!((Value(UV2, FF2)) && (Value(UV1, FF1))))
      {
        return Standard_False;
      }

      F1 = FF1(2);
      F2 = FF2(2);

      theDF(2, 1) = (F1 - 4 * F2 + 3 * F3) / (2.0 * aDeltaU);
    }

    myU  = myU_old;
    myV  = myV_old;
    myP1 = myP1_old;
    myP2 = myP2_old;
    myDu = myDu_old;
    myDv = myDv_old;
  }
  else
  {
    double Ndv  = myDv.Magnitude();
    theDF(2, 2) = Ndv + (P1P2.Dot(Dvv) / Ndv) - theF(2) * (myDv.Dot(Dvv) / (Ndv * Ndv));
    theDF(2, 1) = -myDu.Dot(myDv) / Ndv;
  }

  return Standard_True;
}

//==================================================================================================

template <typename TheCurve1,
          typename TheCurveTool1,
          typename TheCurve2,
          typename TheCurveTool2,
          typename ThePOnC,
          typename ThePoint,
          typename TheVector,
          typename TheSequenceOfPOnC>
int Extrema_GFuncExtCC<TheCurve1,
                       TheCurveTool1,
                       TheCurve2,
                       TheCurveTool2,
                       ThePOnC,
                       ThePoint,
                       TheVector,
                       TheSequenceOfPOnC>::GetStateNumber()
{
  TheVector Du(myDu), Dv(myDv);
  TheVector P1P2(myP1, myP2);

  double mod = Du.Magnitude();
  if (mod > myTolC1)
  {
    Du /= mod;
  }
  mod = Dv.Magnitude();
  if (mod > myTolC2)
  {
    Dv /= mod;
  }

  if (std::abs(P1P2.Dot(Du)) <= myTol && std::abs(P1P2.Dot(Dv)) <= myTol)
  {
    mySqDist.Append(myP1.SquareDistance(myP2));
    myPoints.Append(ThePOnC(myU, myP1));
    myPoints.Append(ThePOnC(myV, myP2));
  }
  return 0;
}

//==================================================================================================

template <typename TheCurve1,
          typename TheCurveTool1,
          typename TheCurve2,
          typename TheCurveTool2,
          typename ThePOnC,
          typename ThePoint,
          typename TheVector,
          typename TheSequenceOfPOnC>
void Extrema_GFuncExtCC<TheCurve1,
                        TheCurveTool1,
                        TheCurve2,
                        TheCurveTool2,
                        ThePOnC,
                        ThePoint,
                        TheVector,
                        TheSequenceOfPOnC>::Points(const int theN,
                                                   ThePOnC&  theP1,
                                                   ThePOnC&  theP2) const
{
  theP1 = myPoints.Value(2 * theN - 1);
  theP2 = myPoints.Value(2 * theN);
}

//==================================================================================================

template <typename TheCurve1,
          typename TheCurveTool1,
          typename TheCurve2,
          typename TheCurveTool2,
          typename ThePOnC,
          typename ThePoint,
          typename TheVector,
          typename TheSequenceOfPOnC>
void Extrema_GFuncExtCC<TheCurve1,
                        TheCurveTool1,
                        TheCurve2,
                        TheCurveTool2,
                        ThePOnC,
                        ThePoint,
                        TheVector,
                        TheSequenceOfPOnC>::SubIntervalInitialize(const math_Vector& theInfBound,
                                                                  const math_Vector& theSupBound)
{
  myUinfium   = theInfBound(1);
  myUsupremum = theSupBound(1);
  myVinfium   = theInfBound(2);
  myVsupremum = theSupBound(2);
}

#endif // _Extrema_GFuncExtCC_HeaderFile
