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

#ifndef _Extrema_GGenExtPC_HeaderFile
#define _Extrema_GGenExtPC_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>

#include <math_FunctionRoots.hxx>
#include <StdFail_NotDone.hxx>
#include <Standard_OutOfRange.hxx>

//! Generic class for finding extremal distances between a point and a curve.
//!
//! This template class searches for all parameter values u where the distance
//! function F(u) = distance(P, C(u)) has an extremum, i.e., where dF/du = 0.
//!
//! @tparam TheCurve   The curve type (e.g., Adaptor3d_Curve, Adaptor2d_Curve2d)
//! @tparam TheTool    The curve tool providing static methods (FirstParameter, LastParameter)
//! @tparam ThePOnC    The point-on-curve type (e.g., Extrema_POnCurv, Extrema_POnCurv2d)
//! @tparam ThePoint   The point type (e.g., gp_Pnt, gp_Pnt2d)
//! @tparam ThePCF     The point-curve function type for extremum computation
template <typename TheCurve, typename TheTool, typename ThePOnC, typename ThePoint, typename ThePCF>
class Extrema_GGenExtPC
{
public:
  DEFINE_STANDARD_ALLOC

  //! Default constructor.
  Extrema_GGenExtPC()
      : myDone(false),
        myInit(false),
        mynbsample(0),
        myumin(0.0),
        myusup(0.0),
        mytolu(0.0),
        mytolF(0.0)
  {
  }

  //! Calculates all extremum distances between point P and curve C.
  //! @param theP       The point
  //! @param theC       The curve
  //! @param theNbSample Number of sample points for root finding
  //! @param theTolU    Tolerance on parameter u
  //! @param theTolF    Tolerance on function value
  Extrema_GGenExtPC(const ThePoint& theP,
                    const TheCurve& theC,
                    const int       theNbSample,
                    const double    theTolU,
                    const double    theTolF)
      : myF(theP, theC)
  {
    Initialize(theC, theNbSample, theTolU, theTolF);
    Perform(theP);
  }

  //! Calculates all extremum distances in a given parameter range.
  //! @param theP       The point
  //! @param theC       The curve
  //! @param theNbSample Number of sample points for root finding
  //! @param theUmin    Lower bound of parameter range
  //! @param theUsup    Upper bound of parameter range
  //! @param theTolU    Tolerance on parameter u
  //! @param theTolF    Tolerance on function value
  Extrema_GGenExtPC(const ThePoint& theP,
                    const TheCurve& theC,
                    const int       theNbSample,
                    const double    theUmin,
                    const double    theUsup,
                    const double    theTolU,
                    const double    theTolF)
      : myF(theP, theC)
  {
    Initialize(theC, theNbSample, theUmin, theUsup, theTolU, theTolF);
    Perform(theP);
  }

  //! Initializes the algorithm with the full curve parameter range.
  //! @param theC       The curve
  //! @param theNbU     Number of sample points
  //! @param theTolU    Tolerance on parameter u
  //! @param theTolF    Tolerance on function value
  void Initialize(const TheCurve& theC,
                  const int       theNbU,
                  const double    theTolU,
                  const double    theTolF)
  {
    myInit     = true;
    mynbsample = theNbU;
    mytolu     = theTolU;
    mytolF     = theTolF;
    myF.Initialize(theC);
    myumin = TheTool::FirstParameter(theC);
    myusup = TheTool::LastParameter(theC);
  }

  //! Initializes the algorithm with a specified parameter range.
  //! @param theC       The curve
  //! @param theNbU     Number of sample points
  //! @param theUmin    Lower bound of parameter range
  //! @param theUsup    Upper bound of parameter range
  //! @param theTolU    Tolerance on parameter u
  //! @param theTolF    Tolerance on function value
  void Initialize(const TheCurve& theC,
                  const int       theNbU,
                  const double    theUmin,
                  const double    theUsup,
                  const double    theTolU,
                  const double    theTolF)
  {
    myInit     = true;
    mynbsample = theNbU;
    mytolu     = theTolU;
    mytolF     = theTolF;
    myF.Initialize(theC);
    myumin = theUmin;
    myusup = theUsup;
  }

  //! Initializes only the parameter range and tolerances.
  //! @param theNbU     Number of sample points
  //! @param theUmin    Lower bound of parameter range
  //! @param theUsup    Upper bound of parameter range
  //! @param theTolU    Tolerance on parameter u
  //! @param theTolF    Tolerance on function value
  void Initialize(const int    theNbU,
                  const double theUmin,
                  const double theUsup,
                  const double theTolU,
                  const double theTolF)
  {
    mynbsample = theNbU;
    mytolu     = theTolU;
    mytolF     = theTolF;
    myumin     = theUmin;
    myusup     = theUsup;
  }

  //! Initializes the curve for the function.
  //! @param theC The curve
  void Initialize(const TheCurve& theC) { myF.Initialize(theC); }

  //! Performs the extremum search for the given point.
  //! @param theP The point to find extrema from
  void Perform(const ThePoint& theP)
  {
    myF.SetPoint(theP);
    myF.SubIntervalInitialize(myumin, myusup);
    myDone = false;

    math_FunctionRoots S(myF, myumin, myusup, mynbsample, mytolu, mytolF, mytolF);
    if (!S.IsDone() || S.IsAllNull())
    {
      return;
    }

    myDone = true;
  }

  //! Returns true if the distances are found.
  bool IsDone() const { return myDone; }

  //! Returns the number of extremum distances.
  //! @return Number of extrema found
  int NbExt() const
  {
    if (!IsDone())
    {
      throw StdFail_NotDone();
    }
    return myF.NbExt();
  }

  //! Returns the Nth extremum square distance.
  //! @param theN Index of the extremum (1-based)
  //! @return Square distance value
  double SquareDistance(const int theN) const
  {
    if ((theN < 1) || (theN > NbExt()))
    {
      throw Standard_OutOfRange();
    }
    return myF.SquareDistance(theN);
  }

  //! Returns true if the Nth extremum distance is a minimum.
  //! @param theN Index of the extremum (1-based)
  //! @return true if minimum, false if maximum
  bool IsMin(const int theN) const
  {
    if ((theN < 1) || (theN > NbExt()))
    {
      throw Standard_OutOfRange();
    }
    return myF.IsMin(theN);
  }

  //! Returns the point of the Nth extremum distance.
  //! @param theN Index of the extremum (1-based)
  //! @return The point on curve
  const ThePOnC& Point(const int theN) const
  {
    if ((theN < 1) || (theN > NbExt()))
    {
      throw Standard_OutOfRange();
    }
    return myF.Point(theN);
  }

private:
  bool   myDone;
  bool   myInit;
  int    mynbsample;
  double myumin;
  double myusup;
  double mytolu;
  double mytolF;
  ThePCF myF;
};

#endif // _Extrema_GGenExtPC_HeaderFile
