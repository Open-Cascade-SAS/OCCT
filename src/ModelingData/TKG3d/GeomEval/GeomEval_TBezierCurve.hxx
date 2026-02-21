// Copyright (c) 2025 OPEN CASCADE SAS
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

#ifndef _GeomEval_TBezierCurve_HeaderFile
#define _GeomEval_TBezierCurve_HeaderFile

#include <Geom_BoundedCurve.hxx>
#include <NCollection_Array1.hxx>
#include <gp_Pnt.hxx>

class gp_Trsf;
class Geom_Geometry;

//! 3D Trigonometric Bezier curve.
//! Uses a trigonometric Bernstein-like basis over the space
//! {1, sin(alpha*t), cos(alpha*t), ..., sin(n*alpha*t), cos(n*alpha*t)}.
//!
//! The parameter domain is [0, Pi/alpha].
//! The number of control points is 2*n + 1 for order n.
//!
//! The alpha parameter controls the frequency of the trigonometric basis.
//! A T-Bezier curve of order n with poles P_0, P_1, ..., P_{2n} is:
//! @code
//!   C(t) = P_0 * T_0(t) + P_1 * T_1(t) + ... + P_{2n} * T_{2n}(t)
//! @endcode
//! where:
//! - T_0(t) = 1
//! - T_{2k-1}(t) = sin(k * alpha * t), for k = 1..n
//! - T_{2k}(t) = cos(k * alpha * t), for k = 1..n
//!
//! For rational curves, each pole is weighted:
//! @code
//!   C(t) = sum(w_i * P_i * T_i(t)) / sum(w_i * T_i(t))
//! @endcode
class GeomEval_TBezierCurve : public Geom_BoundedCurve
{
public:
  //! Constructs a non-rational T-Bezier curve from poles and alpha.
  //! @param[in] thePoles control points (1-based, size must be odd >= 3)
  //! @param[in] theAlpha frequency parameter (must be > 0)
  //! @throw Standard_ConstructionError if NbPoles is not odd or < 3 or theAlpha <= 0
  Standard_EXPORT GeomEval_TBezierCurve(const NCollection_Array1<gp_Pnt>& thePoles,
                                        double                            theAlpha);

  //! Constructs a rational T-Bezier curve.
  //! @param[in] thePoles control points (1-based, size must be odd >= 3)
  //! @param[in] theWeights weights (same size as poles, all > 0)
  //! @param[in] theAlpha frequency parameter (must be > 0)
  //! @throw Standard_ConstructionError if validation fails
  Standard_EXPORT GeomEval_TBezierCurve(const NCollection_Array1<gp_Pnt>& thePoles,
                                        const NCollection_Array1<double>& theWeights,
                                        double                            theAlpha);

  //! Returns the poles array.
  Standard_EXPORT const NCollection_Array1<gp_Pnt>& Poles() const;

  //! Returns the weights array (empty if non-rational).
  Standard_EXPORT const NCollection_Array1<double>& Weights() const;

  //! Returns the frequency parameter alpha.
  Standard_EXPORT double Alpha() const;

  //! Returns the number of poles.
  Standard_EXPORT int NbPoles() const;

  //! Returns the trigonometric order n (NbPoles = 2*n + 1).
  Standard_EXPORT int Order() const;

  //! Returns true if the curve is rational.
  Standard_EXPORT bool IsRational() const;

  // -- Geom_BoundedCurve interface --

  //! Returns the start point C(0).
  Standard_EXPORT gp_Pnt StartPoint() const final;

  //! Returns the end point C(Pi/alpha).
  Standard_EXPORT gp_Pnt EndPoint() const final;

  // -- Geom_Curve interface --

  //! Reversal is not supported for this eval curve.
  //! @throw Standard_NotImplemented
  Standard_EXPORT void Reverse() final;

  //! Reversal is not supported for this eval curve.
  //! @throw Standard_NotImplemented
  Standard_EXPORT double ReversedParameter(const double U) const final;

  //! Returns the first parameter value: 0.0.
  Standard_EXPORT double FirstParameter() const final;

  //! Returns the last parameter value: Pi/alpha.
  Standard_EXPORT double LastParameter() const final;

  //! Returns true if StartPoint and EndPoint coincide.
  Standard_EXPORT bool IsClosed() const final;

  //! Returns false. T-Bezier curves are not periodic.
  Standard_EXPORT bool IsPeriodic() const final;

  //! Returns GeomAbs_CN. T-Bezier curves are infinitely differentiable.
  Standard_EXPORT GeomAbs_Shape Continuity() const final;

  //! Returns true for all N. T-Bezier curves are infinitely differentiable.
  Standard_EXPORT bool IsCN(const int N) const final;

  //! Computes the point C(U).
  Standard_EXPORT gp_Pnt EvalD0(const double U) const final;

  //! Computes the point and first derivative at U.
  Standard_EXPORT Geom_Curve::ResD1 EvalD1(const double U) const final;

  //! Computes the point and first two derivatives at U.
  Standard_EXPORT Geom_Curve::ResD2 EvalD2(const double U) const final;

  //! Computes the point and first three derivatives at U.
  Standard_EXPORT Geom_Curve::ResD3 EvalD3(const double U) const final;

  //! Computes the N-th derivative at U.
  //! @param[in] U parameter value
  //! @param[in] N derivative order (must be >= 1)
  //! @return the N-th derivative vector
  //! @throw Geom_UndefinedDerivative if N < 1
  Standard_EXPORT gp_Vec EvalDN(const double U, const int N) const final;

  //! Transformation is not supported for this eval geometry.
  //! @throw Standard_NotImplemented
  Standard_EXPORT void Transform(const gp_Trsf& T) final;

  //! Creates a new object which is a copy of this T-Bezier curve.
  Standard_EXPORT occ::handle<Geom_Geometry> Copy() const final;

  //! Dumps the content of me into the stream.
  Standard_EXPORT void DumpJson(Standard_OStream& theOStream, int theDepth = -1) const final;

  DEFINE_STANDARD_RTTIEXT(GeomEval_TBezierCurve, Geom_BoundedCurve)

private:
  //! Evaluate trigonometric basis functions at parameter theT.
  //! Returns array of size 2*n+1:
  //! {T_0(t), T_1(t), ..., T_{2n}(t)}
  //! where T_0=1, T_{2k-1}=sin(k*alpha*t), T_{2k}=cos(k*alpha*t).
  void evalBasis(double theT, NCollection_Array1<double>& theBasis) const;

  //! Evaluate derivatives of basis functions of order theDerivOrder.
  //! @param[in] theT parameter value
  //! @param[in] theDerivOrder derivative order (1, 2, 3, ...)
  //! @param[out] theBasisDeriv array of derivatives of size 2*n+1
  void evalBasisDeriv(double                      theT,
                      int                         theDerivOrder,
                      NCollection_Array1<double>& theBasisDeriv) const;

  //! Evaluate point on a non-rational curve: sum(P_i * B_i(t)).
  gp_Pnt evalNonRationalPoint(const NCollection_Array1<double>& theBasis) const;

  //! Evaluate derivative vector on a non-rational curve: sum(P_i * B_i'(t)).
  gp_Vec evalNonRationalDeriv(const NCollection_Array1<double>& theBasisDeriv) const;

  NCollection_Array1<gp_Pnt> myPoles;    //!< Control points
  NCollection_Array1<double> myWeights;  //!< Weights (empty if non-rational)
  double                     myAlpha;    //!< Frequency parameter
  bool                       myRational; //!< True if curve is rational
};

#endif // _GeomEval_TBezierCurve_HeaderFile
