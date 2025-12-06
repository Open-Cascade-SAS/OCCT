// Created on: 1995-07-18
// Created by: Modelistation
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

#ifndef _Extrema_GGenExtCC_HeaderFile
#define _Extrema_GGenExtCC_HeaderFile

#include <algorithm>

#include <Extrema_GlobOptFuncCC.hxx>
#include <GCPnts_AbscissaPoint.hxx>
#include <math_GlobOptMin.hxx>
#include <math_Vector.hxx>
#include <NCollection_CellFilter.hxx>
#include <NCollection_Vector.hxx>
#include <Precision.hxx>
#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_NullObject.hxx>
#include <Standard_OutOfRange.hxx>
#include <StdFail_NotDone.hxx>
#include <TColStd_Array1OfReal.hxx>
#include <TColStd_HArray1OfReal.hxx>
#include <TColStd_ListOfInteger.hxx>
#include <TColStd_SequenceOfReal.hxx>

#ifndef M_SQRT2
  #define M_SQRT2 1.41421356237309504880168872420969808
#endif

//! Template class for computing extremal distances between two curves.
//! The function F(u,v)=distance(C1(u),C2(v)) has an extremum when gradient(f)=0.
//! The algorithm uses Evtushenko's global optimization solver.
//!
//! @tparam TheCurve1 Type of the first curve (e.g., Adaptor3d_Curve)
//! @tparam TheCurveTool1 Tool class for the first curve
//! @tparam TheCurve2 Type of the second curve
//! @tparam TheCurveTool2 Tool class for the second curve
//! @tparam ThePOnC Point on curve type (e.g., Extrema_POnCurv)
//! @tparam ThePoint Point type (e.g., gp_Pnt)
//! @tparam TheExtPC Point-to-curve extremum class (e.g., Extrema_ExtPC)
template <typename TheCurve1,
          typename TheCurveTool1,
          typename TheCurve2,
          typename TheCurveTool2,
          typename ThePOnC,
          typename ThePoint,
          typename TheExtPC>
class Extrema_GGenExtCC
{
public:
  DEFINE_STANDARD_ALLOC

  //! Default constructor.
  Extrema_GGenExtCC();

  //! Constructor with two curves.
  Extrema_GGenExtCC(const TheCurve1& theC1, const TheCurve2& theC2);

  //! Constructor with two curves and parameter bounds.
  Extrema_GGenExtCC(const TheCurve1& theC1,
                    const TheCurve2& theC2,
                    const double     theUinf,
                    const double     theUsup,
                    const double     theVinf,
                    const double     theVsup);

  //! Sets parameters for computation.
  void SetParams(const TheCurve1& theC1,
                 const TheCurve2& theC2,
                 const double     theUinf,
                 const double     theUsup,
                 const double     theVinf,
                 const double     theVsup);

  //! Sets the tolerance.
  void SetTolerance(const double theTol);

  //! Set flag for single extrema computation.
  void SetSingleSolutionFlag(const Standard_Boolean theFlag);

  //! Get flag for single extrema computation.
  Standard_Boolean GetSingleSolutionFlag() const;

  //! Performs calculations.
  void Perform();

  //! Returns True if the distances are found.
  Standard_Boolean IsDone() const;

  //! Returns state of myParallel flag.
  Standard_Boolean IsParallel() const;

  //! Returns the number of extremum distances.
  int NbExt() const;

  //! Returns the value of the Nth square extremum distance.
  double SquareDistance(const int theN = 1) const;

  //! Returns the points of the Nth extremum distance.
  void Points(const int theN, ThePOnC& theP1, ThePOnC& theP2) const;

private:
  Standard_Boolean       myIsFindSingleSolution;
  Standard_Boolean       myParallel;
  double                 myCurveMinTol;
  math_Vector            myLowBorder;
  math_Vector            myUppBorder;
  TColStd_SequenceOfReal myPoints1;
  TColStd_SequenceOfReal myPoints2;
  Standard_Address       myC[2];
  Standard_Boolean       myDone;
};

//==================================================================================================
// Static helper functions
//==================================================================================================

namespace
{
// Comparator, used in std::sort.
inline Standard_Boolean Extrema_GGenExtCC_comp(const gp_XY& theA, const gp_XY& theB)
{
  if (theA.X() < theB.X())
  {
    return Standard_True;
  }
  else
  {
    if (theA.X() == theB.X())
    {
      if (theA.Y() < theB.Y())
        return Standard_True;
    }
  }
  return Standard_False;
}

inline void Extrema_GGenExtCC_ChangeIntervals(Handle(TColStd_HArray1OfReal)& theInts,
                                              const int                      theNbInts)
{
  int                           aNbInts  = theInts->Length() - 1;
  int                           aNbAdd   = theNbInts - aNbInts;
  Handle(TColStd_HArray1OfReal) aNewInts = new TColStd_HArray1OfReal(1, theNbInts + 1);
  int                           aNbLast  = theInts->Length();
  int                           i;
  if (aNbInts == 1)
  {
    aNewInts->SetValue(1, theInts->First());
    aNewInts->SetValue(theNbInts + 1, theInts->Last());
    double dt = (theInts->Last() - theInts->First()) / theNbInts;
    double t  = theInts->First() + dt;
    for (i = 2; i <= theNbInts; ++i, t += dt)
    {
      aNewInts->SetValue(i, t);
    }
    theInts = aNewInts;
    return;
  }
  for (i = 1; i <= aNbLast; ++i)
  {
    aNewInts->SetValue(i, theInts->Value(i));
  }
  while (aNbAdd > 0)
  {
    double anLIntMax = -1.;
    int    aMaxInd   = -1;
    for (i = 1; i < aNbLast; ++i)
    {
      double anL = aNewInts->Value(i + 1) - aNewInts->Value(i);
      if (anL > anLIntMax)
      {
        anLIntMax = anL;
        aMaxInd   = i;
      }
    }

    double t = (aNewInts->Value(aMaxInd + 1) + aNewInts->Value(aMaxInd)) / 2.;
    for (i = aNbLast; i > aMaxInd; --i)
    {
      aNewInts->SetValue(i + 1, aNewInts->Value(i));
    }
    aNbLast++;
    aNbAdd--;
    aNewInts->SetValue(aMaxInd + 1, t);
  }
  theInts = aNewInts;
}

class Extrema_GGenExtCC_PointsInspector : public NCollection_CellFilter_InspectorXY
{
public:
  typedef gp_XY Target;

  Extrema_GGenExtCC_PointsInspector(const double theTol)
  {
    myTol    = theTol * theTol;
    myIsFind = Standard_False;
  }

  void ClearFind() { myIsFind = Standard_False; }

  Standard_Boolean isFind() { return myIsFind; }

  void SetCurrent(const gp_XY& theCurPnt) { myCurrent = theCurPnt; }

  NCollection_CellFilter_Action Inspect(const Target& theObject)
  {
    gp_XY        aPt     = myCurrent.Subtracted(theObject);
    const double aSQDist = aPt.SquareModulus();
    if (aSQDist < myTol)
    {
      myIsFind = Standard_True;
    }
    return CellFilter_Keep;
  }

private:
  double           myTol;
  gp_XY            myCurrent;
  Standard_Boolean myIsFind;
};

template <typename TheCurve, typename TheExtPCType, typename ThePointType>
double Extrema_GGenExtCC_ProjPOnC(const ThePointType& theP, TheExtPCType& theProjTool)
{
  double aDist = ::RealLast();
  theProjTool.Perform(theP);
  if (theProjTool.IsDone() && theProjTool.NbExt())
  {
    for (int i = 1; i <= theProjTool.NbExt(); ++i)
    {
      double aD = theProjTool.SquareDistance(i);
      if (aD < aDist)
        aDist = aD;
    }
  }
  return aDist;
}
} // namespace

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
          typename TheExtPC>
Extrema_GGenExtCC<TheCurve1, TheCurveTool1, TheCurve2, TheCurveTool2, ThePOnC, ThePoint, TheExtPC>::
  Extrema_GGenExtCC()
    : myIsFindSingleSolution(Standard_False),
      myParallel(Standard_False),
      myCurveMinTol(Precision::PConfusion()),
      myLowBorder(1, 2),
      myUppBorder(1, 2),
      myDone(Standard_False)
{
  myC[0] = myC[1] = 0;
}

//==================================================================================================

template <typename TheCurve1,
          typename TheCurveTool1,
          typename TheCurve2,
          typename TheCurveTool2,
          typename ThePOnC,
          typename ThePoint,
          typename TheExtPC>
Extrema_GGenExtCC<TheCurve1, TheCurveTool1, TheCurve2, TheCurveTool2, ThePOnC, ThePoint, TheExtPC>::
  Extrema_GGenExtCC(const TheCurve1& theC1, const TheCurve2& theC2)
    : myIsFindSingleSolution(Standard_False),
      myParallel(Standard_False),
      myCurveMinTol(Precision::PConfusion()),
      myLowBorder(1, 2),
      myUppBorder(1, 2),
      myDone(Standard_False)
{
  myC[0]         = (Standard_Address)&theC1;
  myC[1]         = (Standard_Address)&theC2;
  myLowBorder(1) = theC1.FirstParameter();
  myLowBorder(2) = theC2.FirstParameter();
  myUppBorder(1) = theC1.LastParameter();
  myUppBorder(2) = theC2.LastParameter();
}

//==================================================================================================

template <typename TheCurve1,
          typename TheCurveTool1,
          typename TheCurve2,
          typename TheCurveTool2,
          typename ThePOnC,
          typename ThePoint,
          typename TheExtPC>
Extrema_GGenExtCC<TheCurve1, TheCurveTool1, TheCurve2, TheCurveTool2, ThePOnC, ThePoint, TheExtPC>::
  Extrema_GGenExtCC(const TheCurve1& theC1,
                    const TheCurve2& theC2,
                    const double     theUinf,
                    const double     theUsup,
                    const double     theVinf,
                    const double     theVsup)
    : myIsFindSingleSolution(Standard_False),
      myParallel(Standard_False),
      myCurveMinTol(Precision::PConfusion()),
      myLowBorder(1, 2),
      myUppBorder(1, 2),
      myDone(Standard_False)
{
  myC[0]         = (Standard_Address)&theC1;
  myC[1]         = (Standard_Address)&theC2;
  myLowBorder(1) = theUinf;
  myLowBorder(2) = theVinf;
  myUppBorder(1) = theUsup;
  myUppBorder(2) = theVsup;
}

//==================================================================================================

template <typename TheCurve1,
          typename TheCurveTool1,
          typename TheCurve2,
          typename TheCurveTool2,
          typename ThePOnC,
          typename ThePoint,
          typename TheExtPC>
void Extrema_GGenExtCC<TheCurve1,
                       TheCurveTool1,
                       TheCurve2,
                       TheCurveTool2,
                       ThePOnC,
                       ThePoint,
                       TheExtPC>::SetParams(const TheCurve1& theC1,
                                            const TheCurve2& theC2,
                                            const double     theUinf,
                                            const double     theUsup,
                                            const double     theVinf,
                                            const double     theVsup)
{
  myC[0]         = (Standard_Address)&theC1;
  myC[1]         = (Standard_Address)&theC2;
  myLowBorder(1) = theUinf;
  myLowBorder(2) = theVinf;
  myUppBorder(1) = theUsup;
  myUppBorder(2) = theVsup;
}

//==================================================================================================

template <typename TheCurve1,
          typename TheCurveTool1,
          typename TheCurve2,
          typename TheCurveTool2,
          typename ThePOnC,
          typename ThePoint,
          typename TheExtPC>
void Extrema_GGenExtCC<TheCurve1,
                       TheCurveTool1,
                       TheCurve2,
                       TheCurveTool2,
                       ThePOnC,
                       ThePoint,
                       TheExtPC>::SetTolerance(const double theTol)
{
  myCurveMinTol = theTol;
}

//==================================================================================================

template <typename TheCurve1,
          typename TheCurveTool1,
          typename TheCurve2,
          typename TheCurveTool2,
          typename ThePOnC,
          typename ThePoint,
          typename TheExtPC>
void Extrema_GGenExtCC<TheCurve1,
                       TheCurveTool1,
                       TheCurve2,
                       TheCurveTool2,
                       ThePOnC,
                       ThePoint,
                       TheExtPC>::SetSingleSolutionFlag(const Standard_Boolean theFlag)
{
  myIsFindSingleSolution = theFlag;
}

//==================================================================================================

template <typename TheCurve1,
          typename TheCurveTool1,
          typename TheCurve2,
          typename TheCurveTool2,
          typename ThePOnC,
          typename ThePoint,
          typename TheExtPC>
Standard_Boolean Extrema_GGenExtCC<TheCurve1,
                                   TheCurveTool1,
                                   TheCurve2,
                                   TheCurveTool2,
                                   ThePOnC,
                                   ThePoint,
                                   TheExtPC>::GetSingleSolutionFlag() const
{
  return myIsFindSingleSolution;
}

//==================================================================================================

template <typename TheCurve1,
          typename TheCurveTool1,
          typename TheCurve2,
          typename TheCurveTool2,
          typename ThePOnC,
          typename ThePoint,
          typename TheExtPC>
void Extrema_GGenExtCC<TheCurve1,
                       TheCurveTool1,
                       TheCurve2,
                       TheCurveTool2,
                       ThePOnC,
                       ThePoint,
                       TheExtPC>::Perform()
{
  myDone     = Standard_False;
  myParallel = Standard_False;

  TheCurve1& C1 = *(TheCurve1*)myC[0];
  TheCurve2& C2 = *(TheCurve2*)myC[1];

  int           aNbInter[2];
  GeomAbs_Shape aContinuity = GeomAbs_C2;
  aNbInter[0]               = C1.NbIntervals(aContinuity);
  aNbInter[1]               = C2.NbIntervals(aContinuity);

  if (aNbInter[0] * aNbInter[1] > 100)
  {
    aContinuity = GeomAbs_C1;
    aNbInter[0] = C1.NbIntervals(aContinuity);
    aNbInter[1] = C2.NbIntervals(aContinuity);
  }

  double       anL[2];
  int          indmax = -1, indmin = -1;
  const double mult = 20.;
  if (!(Precision::IsInfinite(C1.FirstParameter()) || Precision::IsInfinite(C1.LastParameter())
        || Precision::IsInfinite(C2.FirstParameter()) || Precision::IsInfinite(C2.LastParameter())))
  {
    anL[0] = GCPnts_AbscissaPoint::Length(C1);
    anL[1] = GCPnts_AbscissaPoint::Length(C2);
    if (anL[0] / aNbInter[0] > mult * anL[1] / aNbInter[1])
    {
      indmax = 0;
      indmin = 1;
    }
    else if (anL[1] / aNbInter[1] > mult * anL[0] / aNbInter[0])
    {
      indmax = 1;
      indmin = 0;
    }
  }
  int aNbIntOpt = 0;
  if (indmax >= 0)
  {
    aNbIntOpt = RealToInt(anL[indmax] * aNbInter[indmin] / anL[indmin] / (mult / 4.)) + 1;
    if (aNbIntOpt > 100 || aNbIntOpt < aNbInter[indmax])
    {
      indmax = -1;
    }
    else
    {
      if (aNbIntOpt * aNbInter[indmin] > 100)
      {
        aNbIntOpt = 100 / aNbInter[indmin];
        if (aNbIntOpt < aNbInter[indmax])
        {
          indmax = -1;
        }
      }
    }
  }

  Handle(TColStd_HArray1OfReal) anIntervals1 = new TColStd_HArray1OfReal(1, aNbInter[0] + 1);
  Handle(TColStd_HArray1OfReal) anIntervals2 = new TColStd_HArray1OfReal(1, aNbInter[1] + 1);
  C1.Intervals(anIntervals1->ChangeArray1(), aContinuity);
  C2.Intervals(anIntervals2->ChangeArray1(), aContinuity);
  if (indmax >= 0)
  {
    if (indmax == 0)
    {
      Extrema_GGenExtCC_ChangeIntervals(anIntervals1, aNbIntOpt);
      aNbInter[0] = anIntervals1->Length() - 1;
    }
    else
    {
      Extrema_GGenExtCC_ChangeIntervals(anIntervals2, aNbIntOpt);
      aNbInter[1] = anIntervals2->Length() - 1;
    }
  }
  if (C1.IsClosed() && aNbInter[0] == 1)
  {
    Extrema_GGenExtCC_ChangeIntervals(anIntervals1, 3);
    aNbInter[0] = anIntervals1->Length() - 1;
  }
  if (C2.IsClosed() && aNbInter[1] == 1)
  {
    Extrema_GGenExtCC_ChangeIntervals(anIntervals2, 3);
    aNbInter[1] = anIntervals2->Length() - 1;
  }

  const double aMaxLC   = 10000.;
  double       aLC      = 100.0;
  const double aMaxDer1 = 1.0 / C1.Resolution(1.0);
  const double aMaxDer2 = 1.0 / C2.Resolution(1.0);
  double       aMaxDer  = std::max(aMaxDer1, aMaxDer2) * M_SQRT2;
  if (aLC > aMaxDer)
    aLC = aMaxDer;

  Standard_Boolean isConstLockedFlag = Standard_False;
  const double     aCR               = 0.001;
  if (aMaxDer1 / aMaxDer < aCR || aMaxDer2 / aMaxDer < aCR)
  {
    isConstLockedFlag = Standard_True;
  }
  if (aMaxDer > aMaxLC)
  {
    aLC               = aMaxLC;
    isConstLockedFlag = Standard_True;
  }
  if (C1.GetType() == GeomAbs_Line)
  {
    aMaxDer = 1.0 / C2.Resolution(1.0);
    if (aLC > aMaxDer)
    {
      isConstLockedFlag = Standard_True;
      aLC               = aMaxDer;
    }
  }
  if (C2.GetType() == GeomAbs_Line)
  {
    aMaxDer = 1.0 / C1.Resolution(1.0);
    if (aLC > aMaxDer)
    {
      isConstLockedFlag = Standard_True;
      aLC               = aMaxDer;
    }
  }

  Extrema_GlobOptFuncCCC2 aFunc(C1, C2);
  if (aLC < aMaxLC || aMaxDer > aMaxLC)
  {
    math_Vector aT(1, 2), aG(1, 2);
    double      aF, aMaxG = 0.;
    double      t1, t2, dt1, dt2;
    int         n1 = 21, n2 = 21, i1, i2;
    dt1 = (C1.LastParameter() - C1.FirstParameter()) / (n1 - 1);
    dt2 = (C2.LastParameter() - C2.FirstParameter()) / (n2 - 1);
    for (i1 = 1, t1 = C1.FirstParameter(); i1 <= n1; ++i1, t1 += dt1)
    {
      aT(1) = t1;
      for (i2 = 1, t2 = C2.FirstParameter(); i2 <= n2; ++i2, t2 += dt2)
      {
        aT(2) = t2;
        aFunc.Values(aT, aF, aG);
        double aMod = aG(1) * aG(1) + aG(2) * aG(2);
        aMaxG       = std::max(aMaxG, aMod);
      }
    }
    aMaxG = std::sqrt(aMaxG);
    if (aMaxG > aMaxDer)
    {
      aLC               = std::min(aMaxG, aMaxLC);
      isConstLockedFlag = Standard_True;
    }
    if (aMaxG > 100. * aMaxLC)
    {
      aLC               = 100. * aMaxLC;
      isConstLockedFlag = Standard_True;
    }
    else if (aMaxG < 0.1 * aMaxDer)
    {
      isConstLockedFlag = Standard_True;
    }
  }
  math_GlobOptMin aFinder(&aFunc, myLowBorder, myUppBorder, aLC);
  aFinder.SetLipConstState(isConstLockedFlag);
  aFinder.SetContinuity(aContinuity == GeomAbs_C2 ? 2 : 1);
  double aDiscTol  = 1.0e-2;
  double aValueTol = 1.0e-2;
  double aSameTol  = myCurveMinTol / (aDiscTol);
  aFinder.SetTol(aDiscTol, aSameTol);
  aFinder.SetFunctionalMinimalValue(0.0);

  const double aCellSize = std::max(std::max(anIntervals1->Last() - anIntervals1->First(),
                                             anIntervals2->Last() - anIntervals2->First())
                                      * Precision::PConfusion() / (2.0 * M_SQRT2),
                                    Precision::PConfusion());
  Extrema_GGenExtCC_PointsInspector                         anInspector(aCellSize);
  NCollection_CellFilter<Extrema_GGenExtCC_PointsInspector> aFilter(aCellSize);
  NCollection_Vector<gp_XY>                                 aPnts;

  int         i, j, k;
  math_Vector aFirstBorderInterval(1, 2);
  math_Vector aSecondBorderInterval(1, 2);
  double      aF     = RealLast();
  double      aCurrF = RealLast();
  for (i = 1; i <= aNbInter[0]; i++)
  {
    for (j = 1; j <= aNbInter[1]; j++)
    {
      aFirstBorderInterval(1)  = anIntervals1->Value(i);
      aFirstBorderInterval(2)  = anIntervals2->Value(j);
      aSecondBorderInterval(1) = anIntervals1->Value(i + 1);
      aSecondBorderInterval(2) = anIntervals2->Value(j + 1);

      aFinder.SetLocalParams(aFirstBorderInterval, aSecondBorderInterval);
      aFinder.Perform(GetSingleSolutionFlag());

      aCurrF = aFinder.GetF();
      if (aCurrF >= aF + aSameTol * aValueTol)
      {
        continue;
      }

      if (aCurrF > aF - aSameTol * aValueTol)
      {
        if (aCurrF < aF)
          aF = aCurrF;
      }
      else
      {
        aF = aCurrF;
        aFilter.Reset(aCellSize);
        aPnts.Clear();
      }

      math_Vector sol(1, 2);
      for (k = 1; k <= aFinder.NbExtrema(); k++)
      {
        aFinder.Points(k, sol);
        gp_XY aPnt2d(sol(1), sol(2));

        gp_XY aXYmin = anInspector.Shift(aPnt2d, -aCellSize);
        gp_XY aXYmax = anInspector.Shift(aPnt2d, aCellSize);

        anInspector.ClearFind();
        anInspector.SetCurrent(aPnt2d);
        aFilter.Inspect(aXYmin, aXYmax, anInspector);
        if (!anInspector.isFind())
        {
          aFilter.Add(aPnt2d, aPnt2d);
          aPnts.Append(gp_XY(sol(1), sol(2)));
        }
      }
    }
  }

  const int aNbSol = aPnts.Length();
  if (aNbSol == 0)
  {
    myDone = Standard_False;
    return;
  }

  myDone = Standard_True;

  if (aNbSol == 1)
  {
    const gp_XY& aSol = aPnts.First();
    myPoints1.Append(aSol.X());
    myPoints2.Append(aSol.Y());
    return;
  }

  std::sort(aPnts.begin(), aPnts.end(), Extrema_GGenExtCC_comp);

  TColStd_ListOfInteger aSolutions;

  Standard_Boolean bSaveSolution       = Standard_True;
  Standard_Boolean bDirsCoinside       = Standard_True;
  Standard_Boolean bDifferentSolutions = Standard_False;

  Standard_Boolean isParallel = Standard_True;
  double           aVal       = 0.0;
  math_Vector      aVec(1, 2, 0.0);

  for (int anIdx = 0; anIdx < aNbSol - 1; anIdx++)
  {
    const gp_XY& aCurrent = aPnts(anIdx);
    const gp_XY& aNext    = aPnts(anIdx + 1);

    aVec(1) = (aCurrent.X() + aNext.X()) * 0.5;
    aVec(2) = (aCurrent.Y() + aNext.Y()) * 0.5;

    aFunc.Value(aVec, aVal);
    if (std::abs(aVal - aF) < Precision::Confusion())
    {
      if (bSaveSolution)
      {
        aSolutions.Append(anIdx);
        bSaveSolution = Standard_False;
      }
    }
    else
    {
      isParallel = Standard_False;
      aSolutions.Append(anIdx);
      bSaveSolution = Standard_True;
    }

    if (!bDifferentSolutions)
    {
      if (aNext.X() > aCurrent.X())
      {
        if (aNext.Y() > aCurrent.Y())
        {
          bDifferentSolutions = Standard_True;
          bDirsCoinside       = Standard_True;
        }
        else if (aNext.Y() < aCurrent.Y())
        {
          bDifferentSolutions = Standard_True;
          bDirsCoinside       = Standard_False;
        }
      }
    }
  }
  aSolutions.Append(aNbSol - 1);

  if (!bDifferentSolutions)
    isParallel = Standard_False;

  if (isParallel)
  {
    double aT1[2] = {myLowBorder(1), myUppBorder(1)};
    double aT2[2] = {bDirsCoinside ? myLowBorder(2) : myUppBorder(2),
                     bDirsCoinside ? myUppBorder(2) : myLowBorder(2)};

    TheExtPC anExtPC1, anExtPC2;
    anExtPC1.Initialize(C1, myLowBorder(1), myUppBorder(1));
    anExtPC2.Initialize(C2, myLowBorder(2), myUppBorder(2));

    for (int iT = 0; isParallel && (iT < 2); ++iT)
    {
      double aDist1 =
        Extrema_GGenExtCC_ProjPOnC<TheCurve2, TheExtPC, ThePoint>(C1.Value(aT1[iT]), anExtPC2);
      double aDist2 =
        Extrema_GGenExtCC_ProjPOnC<TheCurve1, TheExtPC, ThePoint>(C2.Value(aT2[iT]), anExtPC1);
      isParallel = (std::abs(std::min(aDist1, aDist2) - aF * aF) < Precision::Confusion());
    }
  }

  if (isParallel)
  {
    const gp_XY& aSol = aPnts.First();
    myPoints1.Append(aSol.X());
    myPoints2.Append(aSol.Y());
    myParallel = Standard_True;
  }
  else
  {
    TColStd_ListIteratorOfListOfInteger aItSol(aSolutions);
    for (; aItSol.More(); aItSol.Next())
    {
      const gp_XY& aSol = aPnts(aItSol.Value());
      myPoints1.Append(aSol.X());
      myPoints2.Append(aSol.Y());
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
          typename TheExtPC>
Standard_Boolean Extrema_GGenExtCC<TheCurve1,
                                   TheCurveTool1,
                                   TheCurve2,
                                   TheCurveTool2,
                                   ThePOnC,
                                   ThePoint,
                                   TheExtPC>::IsDone() const
{
  return myDone;
}

//==================================================================================================

template <typename TheCurve1,
          typename TheCurveTool1,
          typename TheCurve2,
          typename TheCurveTool2,
          typename ThePOnC,
          typename ThePoint,
          typename TheExtPC>
Standard_Boolean Extrema_GGenExtCC<TheCurve1,
                                   TheCurveTool1,
                                   TheCurve2,
                                   TheCurveTool2,
                                   ThePOnC,
                                   ThePoint,
                                   TheExtPC>::IsParallel() const
{
  if (!IsDone())
    throw StdFail_NotDone();
  return myParallel;
}

//==================================================================================================

template <typename TheCurve1,
          typename TheCurveTool1,
          typename TheCurve2,
          typename TheCurveTool2,
          typename ThePOnC,
          typename ThePoint,
          typename TheExtPC>
int Extrema_GGenExtCC<TheCurve1,
                      TheCurveTool1,
                      TheCurve2,
                      TheCurveTool2,
                      ThePOnC,
                      ThePoint,
                      TheExtPC>::NbExt() const
{
  if (!IsDone())
    throw StdFail_NotDone();
  return myPoints1.Length();
}

//==================================================================================================

template <typename TheCurve1,
          typename TheCurveTool1,
          typename TheCurve2,
          typename TheCurveTool2,
          typename ThePOnC,
          typename ThePoint,
          typename TheExtPC>
double Extrema_GGenExtCC<TheCurve1,
                         TheCurveTool1,
                         TheCurve2,
                         TheCurveTool2,
                         ThePOnC,
                         ThePoint,
                         TheExtPC>::SquareDistance(const int theN) const
{
  if (theN < 1 || theN > NbExt())
  {
    throw Standard_OutOfRange();
  }

  return TheCurveTool1::Value(*((TheCurve1*)myC[0]), myPoints1(theN))
    .SquareDistance(TheCurveTool2::Value(*((TheCurve2*)myC[1]), myPoints2(theN)));
}

//==================================================================================================

template <typename TheCurve1,
          typename TheCurveTool1,
          typename TheCurve2,
          typename TheCurveTool2,
          typename ThePOnC,
          typename ThePoint,
          typename TheExtPC>
void Extrema_GGenExtCC<TheCurve1,
                       TheCurveTool1,
                       TheCurve2,
                       TheCurveTool2,
                       ThePOnC,
                       ThePoint,
                       TheExtPC>::Points(const int theN, ThePOnC& theP1, ThePOnC& theP2) const
{
  if (theN < 1 || theN > NbExt())
  {
    throw Standard_OutOfRange();
  }

  theP1.SetValues(myPoints1(theN), TheCurveTool1::Value(*((TheCurve1*)myC[0]), myPoints1(theN)));
  theP2.SetValues(myPoints2(theN), TheCurveTool2::Value(*((TheCurve2*)myC[1]), myPoints2(theN)));
}

#endif // _Extrema_GGenExtCC_HeaderFile
