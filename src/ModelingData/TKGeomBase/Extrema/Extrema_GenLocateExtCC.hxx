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

#ifndef _Extrema_GenLocateExtCC_HeaderFile
#define _Extrema_GenLocateExtCC_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>

#include <math_FunctionSetRoot.hxx>
#include <math_Vector.hxx>
#include <StdFail_NotDone.hxx>

//! Template class for locating local extremum of distance between two curves.
//! Searches for a pair of parameter values (U,V) such that dist(C1(u),C2(v))
//! passes through an extremum, and (U,V) is the solution closest to (U0,V0).
//!
//! @tparam TheCurve   Curve type (e.g., Adaptor3d_Curve, Adaptor2d_Curve2d)
//! @tparam TheTool    Tool for curve operations
//! @tparam ThePOnC    Point on curve type
//! @tparam TheCCLocF  Function type for curve-curve local extremum
template <typename TheCurve, typename TheTool, typename ThePOnC, typename TheCCLocF>
class Extrema_GenLocateExtCC
{
public:
  DEFINE_STANDARD_ALLOC

  //! Calculates the distance between two curves C1 and C2.
  //! Searches for a local extremum starting from initial parameters (U0, V0).
  //! @param theC1   First curve
  //! @param theC2   Second curve
  //! @param theU0   Initial parameter on first curve
  //! @param theV0   Initial parameter on second curve
  //! @param theTolU Tolerance on parameter of first curve
  //! @param theTolV Tolerance on parameter of second curve
  Extrema_GenLocateExtCC(const TheCurve& theC1,
                         const TheCurve& theC2,
                         const double    theU0,
                         const double    theV0,
                         const double    theTolU,
                         const double    theTolV)
  {
    myDone   = false;
    mySqDist = RealLast();

    double Uinf = TheTool::FirstParameter(theC1);
    double Usup = TheTool::LastParameter(theC1);
    double Uu;
    if (Uinf > Usup)
    {
      Uu   = Uinf;
      Uinf = Usup;
      Usup = Uu;
    }

    double Vinf = TheTool::FirstParameter(theC2);
    double Vsup = TheTool::LastParameter(theC2);
    if (Vinf > Vsup)
    {
      Uu   = Vinf;
      Vinf = Vsup;
      Vsup = Uu;
    }

    TheCCLocF   F(theC1, theC2);
    math_Vector Tol(1, 2);
    Tol(1)      = theTolU;
    Tol(2)      = theTolV;
    double Tolf = 1.e-10;

    math_Vector Start(1, 2);
    math_Vector Uuinf(1, 2);
    math_Vector Uusup(1, 2);

    Start(1) = theU0;
    Start(2) = theV0;

    Uuinf(1) = Uinf;
    Uuinf(2) = Vinf;
    Uusup(1) = Usup;
    Uusup(2) = Vsup;

    math_FunctionSetRoot S(F, Tol);
    S.Perform(F, Start, Uuinf, Uusup);

    if (S.IsDone() && F.NbExt() > 0)
    {
      mySqDist = F.SquareDistance(1);
      F.Points(1, myPoint1, myPoint2);
      Start(1) = myPoint1.Parameter();
      Start(2) = myPoint2.Parameter();
      math_Vector Ff(1, 2);
      F.Value(Start, Ff);
      if ((Ff(1) < Tolf) && (Ff(2) < Tolf))
        myDone = true;
    }
  }

  //! Returns True if the distance is found.
  bool IsDone() const { return myDone; }

  //! Returns the value of the extremum square distance.
  double SquareDistance() const
  {
    if (!IsDone())
    {
      throw StdFail_NotDone();
    }
    return mySqDist;
  }

  //! Returns the points of the extremum distance.
  //! @param theP1 Point on first curve
  //! @param theP2 Point on second curve
  void Point(ThePOnC& theP1, ThePOnC& theP2) const
  {
    if (!IsDone())
    {
      throw StdFail_NotDone();
    }
    theP1 = myPoint1;
    theP2 = myPoint2;
  }

private:
  bool    myDone;
  double  mySqDist;
  ThePOnC myPoint1;
  ThePOnC myPoint2;
};

#endif // _Extrema_GenLocateExtCC_HeaderFile
