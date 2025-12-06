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

#ifndef _Extrema_GenLocateExtPC_HeaderFile
#define _Extrema_GenLocateExtPC_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>

#include <math_FunctionRoot.hxx>
#include <StdFail_NotDone.hxx>

#include <cmath>

//! Template class for local extremum search between a point and a curve.
//! Searches for a local extremum of distance between a point and a curve
//! near an initial parameter value.
//!
//! @tparam TheCurve   Curve type (e.g., Adaptor3d_Curve, Adaptor2d_Curve2d, Standard_Address)
//! @tparam TheTool    Tool for curve operations (e.g., Extrema_CurveTool, Extrema_Curve2dTool)
//! @tparam ThePOnC    Point on curve type (e.g., Extrema_POnCurv, Extrema_POnCurv2d)
//! @tparam ThePnt     Point type (e.g., gp_Pnt, gp_Pnt2d)
//! @tparam ThePCLocF  Function type for root finding
template <typename TheCurve,
          typename TheTool,
          typename ThePOnC,
          typename ThePnt,
          typename ThePCLocF>
class Extrema_GenLocateExtPC
{
public:
  DEFINE_STANDARD_ALLOC

  //! Default constructor.
  Extrema_GenLocateExtPC()
      : myDone(false),
        mytolU(0.0),
        myumin(0.0),
        myusup(0.0)
  {
  }

  //! Calculates the distance with a close point.
  //! The close point is defined by the parameter value U0.
  //! The function F(u)=distance(P,C(u)) has an extremum
  //! when g(u)=dF/du=0. The algorithm searches a zero
  //! near the close point.
  //! TolU is used to decide to stop the iterations.
  //! At the nth iteration, the criteria is:
  //! abs(Un - Un-1) < TolU.
  Extrema_GenLocateExtPC(const ThePnt&   theP,
                         const TheCurve& theC,
                         const double    theU0,
                         const double    theTolU)
  {
    Initialize(theC, TheTool::FirstParameter(theC), TheTool::LastParameter(theC), theTolU);
    Perform(theP, theU0);
  }

  //! Calculates the distance with a close point.
  //! The close point is defined by the parameter value U0.
  //! The function F(u)=distance(P,C(u)) has an extremum
  //! when g(u)=dF/du=0. The algorithm searches a zero
  //! near the close point.
  //! Zeros are searched between Umin and Usup.
  //! TolU is used to decide to stop the iterations.
  //! At the nth iteration, the criteria is:
  //! abs(Un - Un-1) < TolU.
  Extrema_GenLocateExtPC(const ThePnt&   theP,
                         const TheCurve& theC,
                         const double    theU0,
                         const double    theUmin,
                         const double    theUsup,
                         const double    theTolU)
  {
    Initialize(theC, theUmin, theUsup, theTolU);
    Perform(theP, theU0);
  }

  //! Sets the fields of the algorithm.
  void Initialize(const TheCurve& theC,
                  const double    theUmin,
                  const double    theUsup,
                  const double    theTolU)
  {
    myDone = false;
    myF.Initialize(theC);
    myumin = theUmin;
    myusup = theUsup;
    mytolU = theTolU;
  }

  //! The algorithm is done with the point P.
  //! An exception is raised if the fields have not been initialized.
  void Perform(const ThePnt& theP, const double theU0)
  {
    myF.SetPoint(theP);
    math_FunctionRoot S(myF, theU0, mytolU, myumin, myusup);
    myDone = S.IsDone();
    if (myDone)
    {
      double         uu, ff;
      const ThePOnC& PP = Point();
      uu                = PP.Parameter();
      if (myF.Value(uu, ff))
      {
        if (std::abs(ff) >= 1.e-07)
          myDone = false;
      }
      else
        myDone = false;
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
    return myF.SquareDistance(1);
  }

  //! Returns True if the extremum distance is a minimum.
  bool IsMin() const
  {
    if (!IsDone())
    {
      throw StdFail_NotDone();
    }
    return myF.IsMin(1);
  }

  //! Returns the point of the extremum distance.
  const ThePOnC& Point() const
  {
    if (!IsDone())
    {
      throw StdFail_NotDone();
    }
    return myF.Point(1);
  }

private:
  bool      myDone;
  double    mytolU;
  double    myumin;
  double    myusup;
  ThePCLocF myF;
};

#endif // _Extrema_GenLocateExtPC_HeaderFile
