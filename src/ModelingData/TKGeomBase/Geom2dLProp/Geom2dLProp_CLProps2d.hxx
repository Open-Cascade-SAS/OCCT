// Created on: 1992-03-26
// Created by: Herve LEGRAND
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

#ifndef _Geom2dLProp_CLProps2d_HeaderFile
#define _Geom2dLProp_CLProps2d_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>
#include <Standard_OutOfRange.hxx>

#include <gp_Pnt2d.hxx>
#include <gp_Vec2d.hxx>
#include <gp_Dir2d.hxx>
#include <LProp_Status.hxx>

#include <Geom2d_Curve.hxx>
#include <LProp_CurveUtils.hxx>

//! Template class for computing local properties of a 2D curve:
//! point, derivatives up to order 3, tangent, curvature, normal,
//! and centre of curvature.
//! @tparam CurveType the curve storage type (e.g. occ::handle<Geom2d_Curve>,
//!         const HLRBRep_Curve*)
//! @tparam Access the access policy for evaluating curve derivatives
template <typename CurveType = occ::handle<Geom2d_Curve>,
          typename Access    = LProp_CurveUtils::DirectAccess>
class Geom2dLProp_CLProps2d
{
public:
  DEFINE_STANDARD_ALLOC

  //! Initializes the local properties of the curve <C>
  //! The current point and the derivatives are
  //! computed at the same time, which allows an
  //! optimization of the computation time.
  //! <N> indicates the maximum number of derivations to
  //! be done (0, 1, 2 or 3). For example, to compute
  //! only the tangent, N should be equal to 1.
  //! <Resolution> is the linear tolerance (it is used to test
  //! if a vector is null).
  Geom2dLProp_CLProps2d(const CurveType& C, const int N, const double Resolution)
      : myCurve(C),
        myU(RealLast()),
        myDerOrder(N),
        myCN(4),
        myLinTol(Resolution),
        myTangentStatus(LProp_Undecided)
  {
    Standard_OutOfRange_Raise_if(N < 0 || N > 3,
                                 "Geom2dLProp_CLProps2d::Geom2dLProp_CLProps2d()");
  }

  //! Same as previous constructor but here the parameter is
  //! set to the value <U>.
  //! All the computations done will be related to <C> and <U>.
  Geom2dLProp_CLProps2d(const CurveType& C,
                        const double     U,
                        const int        N,
                        const double     Resolution)
      : myCurve(C),
        myDerOrder(N),
        myCN(4),
        myLinTol(Resolution),
        myTangentStatus(LProp_Undecided)
  {
    Standard_OutOfRange_Raise_if(N < 0 || N > 3,
                                 "Geom2dLProp_CLProps2d::Geom2dLProp_CLProps2d()");
    SetParameter(U);
  }

  //! Same as previous constructor but here the parameter is
  //! set to the value <U> and the curve is set
  //! with SetCurve.
  //! the curve can have a empty constructor
  //! All the computations done will be related to <C> and <U>
  //! when the functions "set" will be done.
  Geom2dLProp_CLProps2d(const int N, const double Resolution)
      : myCurve{},
        myU(RealLast()),
        myDerOrder(N),
        myCN(0),
        myLinTol(Resolution),
        myTangentStatus(LProp_Undecided)
  {
    Standard_OutOfRange_Raise_if(N < 0 || N > 3, "Geom2dLProp_CLProps2d() - invalid input");
  }

  //! Initializes the local properties of the curve
  //! for the parameter value <U>.
  void SetParameter(const double U)
  {
    LProp_CurveUtils::SetParameter<Access>(myCurve,
                                           U,
                                           myU,
                                           myDerOrder,
                                           myPnt,
                                           myDerivArr,
                                           myTangentStatus);
  }

  //! Initializes the local properties of the curve
  //! for the new curve.
  void SetCurve(const CurveType& C)
  {
    myCurve = C;
    myCN    = 4;
  }

  //! Returns the Point.
  const gp_Pnt2d& Value() const { return myPnt; }

  //! Returns the first derivative.
  //! The derivative is computed if it has not been yet.
  const gp_Vec2d& D1()
  {
    return LProp_CurveUtils::EnsureDeriv<Access>(myCurve, myU, myDerOrder, 1, myPnt, myDerivArr);
  }

  //! Returns the second derivative.
  //! The derivative is computed if it has not been yet.
  const gp_Vec2d& D2()
  {
    return LProp_CurveUtils::EnsureDeriv<Access>(myCurve, myU, myDerOrder, 2, myPnt, myDerivArr);
  }

  //! Returns the third derivative.
  //! The derivative is computed if it has not been yet.
  const gp_Vec2d& D3()
  {
    return LProp_CurveUtils::EnsureDeriv<Access>(myCurve, myU, myDerOrder, 3, myPnt, myDerivArr);
  }

  //! Returns True if the tangent is defined.
  //! For example, the tangent is not defined if the
  //! three first derivatives are all null.
  bool IsTangentDefined()
  {
    return LProp_CurveUtils::IsTangentDefined<gp_Vec2d>(*this,
                                                         myCN,
                                                         myLinTol,
                                                         mySignificantFirstDerivativeOrder,
                                                         myTangentStatus);
  }

  //! output the tangent direction <D>
  void Tangent(gp_Dir2d& D)
  {
    LProp_CurveUtils::Tangent<Access>(*this,
                                      myCurve,
                                      myU,
                                      myDerivArr,
                                      myPnt,
                                      mySignificantFirstDerivativeOrder,
                                      D);
  }

  //! Returns the curvature.
  double Curvature()
  {
    return LProp_CurveUtils::Curvature(*this,
                                       myDerivArr[0],
                                       myDerivArr[1],
                                       myLinTol,
                                       mySignificantFirstDerivativeOrder,
                                       myCurvature);
  }

  //! Returns the normal direction <N>.
  void Normal(gp_Dir2d& N)
  {
    LProp_CurveUtils::Normal(*this, myDerivArr[0], myDerivArr[1], myLinTol, N);
  }

  //! Returns the centre of curvature <P>.
  void CentreOfCurvature(gp_Pnt2d& P)
  {
    LProp_CurveUtils::CentreOfCurvature(*this,
                                        myPnt,
                                        myDerivArr[0],
                                        myDerivArr[1],
                                        myLinTol,
                                        myCurvature,
                                        P);
  }

private:
  CurveType    myCurve;
  double       myU;
  int          myDerOrder;
  int          myCN;
  double       myLinTol;
  gp_Pnt2d     myPnt;
  gp_Vec2d     myDerivArr[3];
  gp_Dir2d     myTangent;
  double       myCurvature = 0.0;
  LProp_Status myTangentStatus;
  int          mySignificantFirstDerivativeOrder = 0;
};

#endif // _Geom2dLProp_CLProps2d_HeaderFile
