// Created on: 1992-10-19
// Created by: Laurent PAINNOT
// Copyright (c) 1992-1999 Matra Datavision
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

#ifndef _Extrema_GGExtPC_HeaderFile
#define _Extrema_GGExtPC_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>

#include <ElCLib.hxx>
#include <GeomAbs_CurveType.hxx>
#include <NCollection_Array1.hxx>
#include <Precision.hxx>
#include <Standard_Failure.hxx>
#include <Standard_OutOfRange.hxx>
#include <StdFail_NotDone.hxx>
#include <TColStd_Array1OfReal.hxx>
#include <TColStd_HArray1OfReal.hxx>
#include <TColStd_SequenceOfBoolean.hxx>
#include <TColStd_SequenceOfReal.hxx>

//! Generic class for computing extremal distances between a point and a curve.
//!
//! This template class provides comprehensive extremum search functionality,
//! handling different curve types (lines, circles, ellipses, parabolas,
//! hyperbolas, Bezier, BSpline, and general curves) with optimized algorithms.
//!
//! @tparam TheCurve       The curve type (e.g., Adaptor3d_Curve, Adaptor2d_Curve2d)
//! @tparam TheCurveTool   The curve tool providing static methods
//! @tparam TheExtPElC     The elementary curve extremum class
//! @tparam ThePoint       The point type (e.g., gp_Pnt, gp_Pnt2d)
//! @tparam TheVector      The vector type (e.g., gp_Vec, gp_Vec2d)
//! @tparam ThePOnC        The point-on-curve type
//! @tparam TheSequenceOfPOnC The sequence of points on curve
//! @tparam TheEPC         The general extremum point-curve class
template <typename TheCurve,
          typename TheCurveTool,
          typename TheExtPElC,
          typename ThePoint,
          typename TheVector,
          typename ThePOnC,
          typename TheSequenceOfPOnC,
          typename TheEPC>
class Extrema_GGExtPC
{
public:
  DEFINE_STANDARD_ALLOC

  //! Default constructor.
  Extrema_GGExtPC()
      : myC(nullptr),
        mydone(false),
        mydist1(RealLast()),
        mydist2(RealLast()),
        mytolu(0.0),
        mytolf(0.0),
        mysample(17),
        myintuinf(Precision::Infinite()),
        myintusup(Precision::Infinite()),
        myuinf(Precision::Infinite()),
        myusup(Precision::Infinite()),
        type(GeomAbs_OtherCurve)
  {
  }

  //! Calculates all extremum distances between point P and curve C
  //! within the specified parameter range.
  //! @param theP    The point
  //! @param theC    The curve
  //! @param theUinf Lower bound of parameter range
  //! @param theUsup Upper bound of parameter range
  //! @param theTolF Tolerance on function value (default 1.0e-10)
  Extrema_GGExtPC(const ThePoint& theP,
                  const TheCurve& theC,
                  const double    theUinf,
                  const double    theUsup,
                  const double    theTolF = 1.0e-10)
  {
    Initialize(theC, theUinf, theUsup, theTolF);
    Perform(theP);
  }

  //! Calculates all extremum distances between point P and curve C
  //! using the full curve parameter range.
  //! @param theP    The point
  //! @param theC    The curve
  //! @param theTolF Tolerance on function value (default 1.0e-10)
  Extrema_GGExtPC(const ThePoint& theP, const TheCurve& theC, const double theTolF = 1.0e-10)
  {
    Initialize(theC,
               TheCurveTool::FirstParameter(theC),
               TheCurveTool::LastParameter(theC),
               theTolF);
    Perform(theP);
  }

  //! Initializes the algorithm with curve and parameter range.
  //! @param theC    The curve
  //! @param theUinf Lower bound of parameter range
  //! @param theUsup Upper bound of parameter range
  //! @param theTolF Tolerance on function value (default 1.0e-10)
  void Initialize(const TheCurve& theC,
                  const double    theUinf,
                  const double    theUsup,
                  const double    theTolF = 1.0e-10)
  {
    myC       = const_cast<TheCurve*>(&theC);
    myintuinf = myuinf = theUinf;
    myintusup = myusup = theUsup;
    mytolf             = theTolF;
    mytolu             = TheCurveTool::Resolution(*myC, Precision::Confusion());
    type               = TheCurveTool::GetType(theC);
    mydone             = false;
    mydist1            = RealLast();
    mydist2            = RealLast();
    mysample           = 17;
  }

  //! Performs the extremum computation for the given point.
  //! @param theP The point to find extrema from
  void Perform(const ThePoint& theP)
  {
    mySqDist.Clear();
    mypoint.Clear();
    myismin.Clear();
    int    i, NbExt, n;
    double U;
    mysample             = 17;
    constexpr double t3d = Precision::Confusion();

    if (Precision::IsInfinite(myuinf))
      mydist1 = RealLast();
    else
    {
      Pf      = TheCurveTool::Value(*myC, myuinf);
      mydist1 = theP.SquareDistance(Pf);
    }

    if (Precision::IsInfinite(myusup))
      mydist2 = RealLast();
    else
    {
      Pl      = TheCurveTool::Value(*myC, myusup);
      mydist2 = theP.SquareDistance(Pl);
    }

    TheCurve& aCurve = *myC;

    switch (type)
    {
      case GeomAbs_Circle: {
        myExtPElC.Perform(theP, TheCurveTool::Circle(aCurve), t3d, myuinf, myusup);
        break;
      }
      case GeomAbs_Ellipse: {
        myExtPElC.Perform(theP, TheCurveTool::Ellipse(aCurve), t3d, myuinf, myusup);
        break;
      }
      case GeomAbs_Parabola: {
        myExtPElC.Perform(theP, TheCurveTool::Parabola(aCurve), t3d, myuinf, myusup);
        break;
      }
      case GeomAbs_Hyperbola: {
        myExtPElC.Perform(theP, TheCurveTool::Hyperbola(aCurve), t3d, myuinf, myusup);
        break;
      }
      case GeomAbs_Line: {
        myExtPElC.Perform(theP, TheCurveTool::Line(aCurve), t3d, myuinf, myusup);
        break;
      }
      case GeomAbs_BezierCurve: {
        myintuinf = myuinf;
        myintusup = myusup;
        mysample  = (TheCurveTool::Bezier(aCurve))->NbPoles() * 2;
        myExtPC.Initialize(aCurve);
        IntervalPerform(theP);
        return;
      }
      case GeomAbs_BSplineCurve: {
        const int aFirstIdx = TheCurveTool::BSpline(aCurve)->FirstUKnotIndex(),
                  aLastIdx  = TheCurveTool::BSpline(aCurve)->LastUKnotIndex();
        TColStd_Array1OfReal aKnots(aFirstIdx, aLastIdx);
        TheCurveTool::BSpline(aCurve)->Knots(aKnots);

        double       aPeriodJump = 0.0;
        const double aTolCoeff   = (myusup - myuinf) * Precision::PConfusion();
        if (TheCurveTool::IsPeriodic(aCurve))
        {
          int aPeriodShift = int((myuinf - aKnots(aFirstIdx)) / TheCurveTool::Period(aCurve));
          if (myuinf < aKnots(aFirstIdx) - aTolCoeff)
            aPeriodShift--;
          aPeriodJump = TheCurveTool::Period(aCurve) * aPeriodShift;
        }

        int anIdx;
        int aFirstUsedKnot = aFirstIdx, aLastUsedKnot = aLastIdx;
        for (anIdx = aFirstIdx; anIdx <= aLastIdx; anIdx++)
        {
          double aKnot = aKnots(anIdx) + aPeriodJump;
          if (myuinf >= aKnot - aTolCoeff)
            aFirstUsedKnot = anIdx;
          else
            break;
        }
        for (anIdx = aLastIdx; anIdx >= aFirstIdx; anIdx--)
        {
          double aKnot = aKnots(anIdx) + aPeriodJump;
          if (myusup <= aKnot + aTolCoeff)
            aLastUsedKnot = anIdx;
          else
            break;
        }

        if (aFirstUsedKnot == aLastUsedKnot)
        {
          aFirstUsedKnot = aFirstIdx;
          aLastUsedKnot  = aFirstIdx + 1;
        }

        mysample = (TheCurveTool::BSpline(aCurve))->Degree() + 1;

        if (mysample == 2)
        {
          ThePoint aPmin;
          double   tmin = 0., distmin = RealLast();
          double   aMin1 = 0., aMin2 = 0.;
          myExtPC.Initialize(aCurve);
          for (anIdx = aFirstUsedKnot; anIdx < aLastUsedKnot; anIdx++)
          {
            double aF = aKnots(anIdx) + aPeriodJump, aL = aKnots(anIdx + 1) + aPeriodJump;

            if (anIdx == aFirstUsedKnot)
              aF = myuinf;
            else if (anIdx == aLastUsedKnot - 1)
              aL = myusup;

            ThePoint aP1, aP2;
            TheCurveTool::D0(aCurve, aF, aP1);
            TheCurveTool::D0(aCurve, aL, aP2);
            TheVector aBase1(theP, aP1), aBase2(theP, aP2);
            TheVector aV(aP2, aP1);
            double    aVal1 = aV.Dot(aBase1);
            double    aVal2 = aV.Dot(aBase2);
            if (anIdx == aFirstUsedKnot)
              aMin1 = theP.SquareDistance(aP1);
            else
            {
              aMin1 = aMin2;
              if (distmin > aMin1)
              {
                distmin = aMin1;
                tmin    = aF;
                aPmin   = aP1;
              }
            }
            aMin2             = theP.SquareDistance(aP2);
            double aMinSqDist = std::min(aMin1, aMin2);
            double aMinDer    = std::min(std::abs(aVal1), std::abs(aVal2));
            if (!(Precision::IsInfinite(aVal1) || Precision::IsInfinite(aVal2)))
            {
              if (aVal1 * aVal2 <= 0.0 || aMinSqDist < 100. * Precision::SquareConfusion()
                  || 2. * aMinDer < Precision::Confusion())
              {
                myintuinf = aF;
                myintusup = aL;
                IntervalPerform(theP);
              }
            }
          }
          if (!Precision::IsInfinite(distmin))
          {
            bool isToAdd = true;
            NbExt        = mypoint.Length();
            for (i = 1; i <= NbExt && isToAdd; i++)
            {
              double t = mypoint.Value(i).Parameter();
              isToAdd  = (distmin < mySqDist(i)) && (std::abs(t - tmin) > mytolu);
            }
            if (isToAdd)
            {
              ThePOnC PC(tmin, aPmin);
              mySqDist.Append(distmin);
              myismin.Append(true);
              mypoint.Append(PC);
            }
          }
        }
        else
        {
          int                        aValIdx = 1;
          NCollection_Array1<double> aVal(1, (mysample) * (aLastUsedKnot - aFirstUsedKnot) + 1);
          NCollection_Array1<double> aParam(1, (mysample) * (aLastUsedKnot - aFirstUsedKnot) + 1);
          for (anIdx = aFirstUsedKnot; anIdx < aLastUsedKnot; anIdx++)
          {
            double aF = aKnots(anIdx) + aPeriodJump, aL = aKnots(anIdx + 1) + aPeriodJump;

            if (anIdx == aFirstUsedKnot)
              aF = myuinf;
            if (anIdx == aLastUsedKnot - 1)
              aL = myusup;

            double aStep = (aL - aF) / mysample;
            for (int aPntIdx = 0; aPntIdx < mysample; aPntIdx++)
            {
              double aCurrentParam = aF + aStep * aPntIdx;
              aVal(aValIdx)   = TheCurveTool::Value(aCurve, aCurrentParam).SquareDistance(theP);
              aParam(aValIdx) = aCurrentParam;
              aValIdx++;
            }
          }
          aVal(aValIdx)   = TheCurveTool::Value(aCurve, myusup).SquareDistance(theP);
          aParam(aValIdx) = myusup;

          myExtPC.Initialize(aCurve);

          for (anIdx = aVal.Lower() + 1; anIdx < aVal.Upper(); anIdx++)
          {
            if (aVal(anIdx) <= Precision::SquareConfusion())
            {
              mySqDist.Append(aVal(anIdx));
              myismin.Append(true);
              mypoint.Append(ThePOnC(aParam(anIdx), TheCurveTool::Value(aCurve, aParam(anIdx))));
            }
            if ((aVal(anIdx) >= aVal(anIdx + 1) && aVal(anIdx) >= aVal(anIdx - 1))
                || (aVal(anIdx) <= aVal(anIdx + 1) && aVal(anIdx) <= aVal(anIdx - 1)))
            {
              myintuinf = aParam(anIdx - 1);
              myintusup = aParam(anIdx + 1);
              IntervalPerform(theP);
            }
          }

          if (mydist1 > Precision::SquareConfusion() && !Precision::IsPositiveInfinite(mydist1))
          {
            ThePoint  aP1, aP2;
            TheVector aV1, aV2;
            TheCurveTool::D1(aCurve, aParam.Value(aParam.Lower()), aP1, aV1);
            TheCurveTool::D1(aCurve, aParam.Value(aParam.Lower() + 1), aP2, aV2);
            TheVector aBase1(theP, aP1), aBase2(theP, aP2);
            double    aVal1 = aV1.Dot(aBase1);
            double    aVal2 = aV2.Dot(aBase2);
            if (!(Precision::IsInfinite(aVal1) || Precision::IsInfinite(aVal2)))
            {
              if (aVal1 * aVal2 <= 0.0 || aBase1.Dot(aBase2) <= 0.0
                  || 2.0 * std::abs(aVal1) < Precision::Confusion())
              {
                myintuinf = aParam(aVal.Lower());
                myintusup = aParam(aVal.Lower() + 1);
                IntervalPerform(theP);
              }
            }
          }

          if (mydist2 > Precision::SquareConfusion() && !Precision::IsPositiveInfinite(mydist2))
          {
            ThePoint  aP1, aP2;
            TheVector aV1, aV2;
            TheCurveTool::D1(aCurve, aParam.Value(aParam.Upper() - 1), aP1, aV1);
            TheCurveTool::D1(aCurve, aParam.Value(aParam.Upper()), aP2, aV2);
            TheVector aBase1(theP, aP1), aBase2(theP, aP2);
            double    aVal1 = aV1.Dot(aBase1);
            double    aVal2 = aV2.Dot(aBase2);

            if (!(Precision::IsInfinite(aVal1) || Precision::IsInfinite(aVal2)))
            {
              if (aVal1 * aVal2 <= 0.0 || aBase1.Dot(aBase2) <= 0.0
                  || 2.0 * std::abs(aVal2) < Precision::Confusion())
              {
                myintuinf = aParam(aVal.Upper() - 1);
                myintusup = aParam(aVal.Upper());
                IntervalPerform(theP);
              }
            }
          }
        }
        mydone = true;
        break;
      }
      default: {
        constexpr int                 aMaxSample   = 17;
        bool                          IntExtIsDone = false;
        bool                          IntIsNotValid;
        Handle(TColStd_HArray1OfReal) theHInter;
        n = TheCurveTool::NbIntervals(aCurve, GeomAbs_C2);
        if (n > 1)
        {
          theHInter = new TColStd_HArray1OfReal(1, n + 1);
          TheCurveTool::Intervals(aCurve, theHInter->ChangeArray1(), GeomAbs_C2);
        }
        else
        {
          theHInter = TheCurveTool::DeflCurvIntervals(aCurve);
          n         = theHInter->Length() - 1;
        }
        mysample      = std::max(mysample / n, aMaxSample);
        double maxint = 0.;
        for (i = 1; i <= n; ++i)
        {
          double dt = theHInter->Value(i + 1) - theHInter->Value(i);
          if (maxint < dt)
            maxint = dt;
        }
        bool      isPeriodic = TheCurveTool::IsPeriodic(aCurve);
        TheVector V1;
        ThePoint  PP;
        double    s1 = 0.0;
        double    s2 = 0.0;
        myExtPC.Initialize(aCurve);
        for (i = 1; i <= n; i++)
        {
          myintuinf = theHInter->Value(i);
          myintusup = theHInter->Value(i + 1);
          mysample  = std::max(RealToInt(aMaxSample * (myintusup - myintuinf) / maxint), 3);

          double anInfToCheck = myintuinf;
          double aSupToCheck  = myintusup;

          if (isPeriodic)
          {
            double aPeriod = TheCurveTool::Period(aCurve);
            anInfToCheck   = ElCLib::InPeriod(myintuinf, myuinf, myuinf + aPeriod);
            aSupToCheck    = myintusup + (anInfToCheck - myintuinf);
          }
          IntIsNotValid = (myuinf > aSupToCheck) || (myusup < anInfToCheck);

          if (IntIsNotValid)
            continue;

          if (myuinf >= anInfToCheck)
            anInfToCheck = myuinf;
          if (myusup <= aSupToCheck)
            aSupToCheck = myusup;
          if ((aSupToCheck - anInfToCheck) <= mytolu)
            continue;

          if (i != 1)
          {
            TheCurveTool::D1(aCurve, myintuinf, PP, V1);
            s1 = (TheVector(theP, PP)) * V1;
            if (s1 * s2 < 0.0)
            {
              mySqDist.Append(PP.SquareDistance(theP));
              myismin.Append((s1 < 0.0));
              mypoint.Append(ThePOnC(myintuinf, PP));
            }
          }
          if (i != n)
          {
            TheCurveTool::D1(aCurve, myintusup, PP, V1);
            s2 = (TheVector(theP, PP)) * V1;
          }

          IntervalPerform(theP);
          IntExtIsDone = IntExtIsDone || mydone;
        }

        mydone = IntExtIsDone;
        break;
      }
    }

    // Postprocessing
    if (type == GeomAbs_BSplineCurve || type == GeomAbs_OffsetCurve || type == GeomAbs_OtherCurve)
    {
      if (mydist1 < Precision::SquareConfusion() || mydist2 < Precision::SquareConfusion())
      {
        bool isFirstAdded = false;
        bool isLastAdded  = false;
        int  aNbPoints    = mypoint.Length();
        for (i = 1; i <= aNbPoints; i++)
        {
          U = mypoint.Value(i).Parameter();
          if (std::abs(U - myuinf) < mytolu)
            isFirstAdded = true;
          else if (std::abs(myusup - U) < mytolu)
            isLastAdded = true;
        }
        if (!isFirstAdded && mydist1 < Precision::SquareConfusion())
        {
          mySqDist.Prepend(mydist1);
          myismin.Prepend(true);
          mypoint.Prepend(ThePOnC(myuinf, Pf));
        }
        if (!isLastAdded && mydist2 < Precision::SquareConfusion())
        {
          mySqDist.Append(mydist2);
          myismin.Append(true);
          mypoint.Append(ThePOnC(myusup, Pl));
        }
        mydone = true;
      }
    }
    else
    {
      mydone = myExtPElC.IsDone();
      if (mydone)
      {
        NbExt = myExtPElC.NbExt();
        for (i = 1; i <= NbExt; i++)
        {
          ThePOnC PC = myExtPElC.Point(i);
          U          = PC.Parameter();
          if (TheCurveTool::IsPeriodic(aCurve))
          {
            U = ElCLib::InPeriod(U, myuinf, myuinf + TheCurveTool::Period(aCurve));
          }
          if ((U >= myuinf - mytolu) && (U <= myusup + mytolu))
          {
            PC.SetValues(U, myExtPElC.Point(i).Value());
            mySqDist.Append(myExtPElC.SquareDistance(i));
            myismin.Append(myExtPElC.IsMin(i));
            mypoint.Append(PC);
          }
        }
      }
    }
  }

  //! Returns true if the distances are found.
  bool IsDone() const { return mydone; }

  //! Returns the Nth extremum square distance.
  //! @param theN Index of the extremum (1-based)
  //! @return Square distance value
  double SquareDistance(const int theN) const
  {
    if ((theN < 1) || (theN > NbExt()))
      throw Standard_OutOfRange();
    return mySqDist.Value(theN);
  }

  //! Returns the number of extremum distances.
  //! @return Number of extrema found
  int NbExt() const
  {
    if (!IsDone())
      throw StdFail_NotDone();
    return mySqDist.Length();
  }

  //! Returns true if the Nth extremum distance is a minimum.
  //! @param theN Index of the extremum (1-based)
  //! @return true if minimum, false if maximum
  bool IsMin(const int theN) const
  {
    if ((theN < 1) || (theN > NbExt()))
      throw Standard_OutOfRange();
    return myismin.Value(theN);
  }

  //! Returns the point of the Nth extremum distance.
  //! @param theN Index of the extremum (1-based)
  //! @return The point on curve
  const ThePOnC& Point(const int theN) const
  {
    if ((theN < 1) || (theN > NbExt()))
      throw Standard_OutOfRange();
    return mypoint.Value(theN);
  }

  //! Returns the distances at curve endpoints.
  //! @param[out] theDist1 Square distance to first point
  //! @param[out] theDist2 Square distance to last point
  //! @param[out] theP1 First point on curve
  //! @param[out] theP2 Last point on curve
  void TrimmedSquareDistances(double&   theDist1,
                              double&   theDist2,
                              ThePoint& theP1,
                              ThePoint& theP2) const
  {
    theDist1 = mydist1;
    theDist2 = mydist2;
    theP1    = Pf;
    theP2    = Pl;
  }

protected:
  //! Performs extremum search on the current interval.
  void IntervalPerform(const ThePoint& theP)
  {
    int    i;
    double U;
    myExtPC.Initialize(mysample, myintuinf, myintusup, mytolu, mytolf);
    myExtPC.Perform(theP);
    mydone = myExtPC.IsDone();
    if (mydone)
    {
      int NbExt = myExtPC.NbExt();
      for (i = 1; i <= NbExt; i++)
      {
        ThePOnC PC = myExtPC.Point(i);
        U          = PC.Parameter();
        if (TheCurveTool::IsPeriodic(*myC))
        {
          U = ElCLib::InPeriod(U, myuinf, myuinf + TheCurveTool::Period(*myC));
        }
        if ((U >= myuinf - mytolu) && (U <= myusup + mytolu))
        {
          AddSol(U, PC.Value(), myExtPC.SquareDistance(i), myExtPC.IsMin(i));
        }
      }
    }
  }

  //! Adds a solution if not already present.
  void AddSol(const double theU, const ThePoint& theP, const double theSqDist, const bool isMin)
  {
    int i, NbExt = mypoint.Length();
    for (i = 1; i <= NbExt; i++)
    {
      double t = mypoint.Value(i).Parameter();
      if (std::abs(t - theU) <= mytolu)
      {
        return;
      }
    }
    ThePOnC PC(theU, theP);
    mySqDist.Append(theSqDist);
    myismin.Append(isMin);
    mypoint.Append(PC);
  }

private:
  TheCurve*                 myC;
  ThePoint                  Pf;
  ThePoint                  Pl;
  TheExtPElC                myExtPElC;
  TheSequenceOfPOnC         mypoint;
  bool                      mydone;
  double                    mydist1;
  double                    mydist2;
  TheEPC                    myExtPC;
  double                    mytolu;
  double                    mytolf;
  int                       mysample;
  double                    myintuinf;
  double                    myintusup;
  double                    myuinf;
  double                    myusup;
  GeomAbs_CurveType         type;
  TColStd_SequenceOfBoolean myismin;
  TColStd_SequenceOfReal    mySqDist;
};

#endif // _Extrema_GGExtPC_HeaderFile
