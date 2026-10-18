// Created on: 1994-04-05
// Created by: Yves FRICAUD
// Copyright (c) 1994-1999 Matra Datavision
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

#ifndef _Bisector_FunctionH_HeaderFile
#define _Bisector_FunctionH_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <gp_Pnt2d.hxx>
#include <gp_Vec2d.hxx>
#include <math_FunctionWithDerivative.hxx>
#include <Standard_Boolean.hxx>
#include <Standard_Real.hxx>
class Geom2d_Curve;
class Geom2dAdaptor_Curve;

//! Circle-tangency equation for a guide point and a second curve.
//! The residual is the chord projected onto a unit normal to the tangent
//! bisector. It has length units and has no parallel-tangent zero factor.
//! The reference parameter selects a continuous orientation of this normal.
//! The continuous tangent-bisector normal must stay within a right angle of
//! its reference direction. Splitting at parallel tangents bounds its rotation.
//! The source curve is retained by a handle and must have regular tangents.
//! @code
//! Bisector_FunctionH aFunction(aCurve, aPoint, aGuideTangent, aMiddle);
//! double aResidual;
//! if (aFunction.Value(aParameter, aResidual))
//! {
//!   // Use the residual within the selected search interval.
//! }
//! @endcode
class Bisector_FunctionH : public math_FunctionWithDerivative
{
public:
  DEFINE_STANDARD_ALLOC

  //! Uses the midpoint of the source parameter range as the reference.
  //! @param[in] C2 source curve, retained by a handle
  //! @param[in] P1 point on the guide curve
  //! @param[in] T1 oriented guide tangent, with magnitude above gp::Resolution()
  Standard_EXPORT Bisector_FunctionH(const occ::handle<Geom2d_Curve>& C2,
                                     const gp_Pnt2d&                  P1,
                                     const gp_Vec2d&                  T1);

  //! Selects the normal orientation using a tangent inside the search interval.
  //! @param[in] theCurve source curve, retained by a handle
  //! @param[in] thePoint point on the guide curve
  //! @param[in] theTangent oriented guide tangent, with magnitude above gp::Resolution()
  //! @param[in] theReferenceParameter parameter inside the search interval on the source curve
  Standard_EXPORT Bisector_FunctionH(const occ::handle<Geom2d_Curve>& theCurve,
                                     const gp_Pnt2d&                  thePoint,
                                     const gp_Vec2d&                  theTangent,
                                     double                           theReferenceParameter);

  //! Reuses a curve adaptor during the root search.
  //! @param[in] theCurve initialized adaptor, which must stay alive and unchanged during use
  //! @param[in] thePoint point on the guide curve
  //! @param[in] theTangent oriented guide tangent, with magnitude above gp::Resolution()
  //! @param[in] theReferenceParameter parameter inside the search interval
  Standard_EXPORT Bisector_FunctionH(const Geom2dAdaptor_Curve& theCurve,
                                     const gp_Pnt2d&            thePoint,
                                     const gp_Vec2d&            theTangent,
                                     double                     theReferenceParameter);

  //! Evaluates the circle-tangency residual in length units.
  //! @param[in] X parameter on C2 within the selected search interval
  //! @param[out] F signed projection of the chord onto the tangent-bisector normal
  //! @return false if a source or reference tangent is unresolved
  Standard_EXPORT bool Value(const double X, double& F) override;

  //! Evaluates the residual derivative with respect to the parameter on C2.
  //! @param[in] X parameter on C2 within the selected search interval
  //! @param[out] D derivative of the residual
  //! @return false if a source or reference tangent is unresolved
  Standard_EXPORT bool Derivative(const double X, double& D) override;

  //! Evaluates the residual and its derivative with respect to the parameter on C2.
  //! @param[in] X parameter on C2 within the selected search interval
  //! @param[out] F residual in length units
  //! @param[out] D derivative of the residual
  //! @return false if a source or reference tangent is unresolved
  Standard_EXPORT bool Values(const double X, double& F, double& D) override;

private:
  occ::handle<Geom2d_Curve>  curve2;      //!< Shared source curve.
  gp_Pnt2d                   p1;          //!< Guide point.
  gp_Vec2d                   t1;          //!< Unit oriented guide tangent.
  gp_Vec2d                   myReference; //!< Unit normal orientation, or zero if unresolved.
  const Geom2dAdaptor_Curve* myAdaptor = nullptr; //!< Borrowed evaluation cache, when provided.
};

#endif // _Bisector_FunctionH_HeaderFile
