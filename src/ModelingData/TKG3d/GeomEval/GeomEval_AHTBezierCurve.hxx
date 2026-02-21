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

#ifndef _GeomEval_AHTBezierCurve_HeaderFile
#define _GeomEval_AHTBezierCurve_HeaderFile

#include <Geom_BoundedCurve.hxx>
#include <NCollection_Array1.hxx>
#include <gp_Pnt.hxx>

class gp_Trsf;
class Geom_Geometry;

//! 3D Algebraic-Hyperbolic-Trigonometric Bezier curve.
//! Uses a mixed basis: {1, t, ..., t^k, sinh(alpha*t), cosh(alpha*t), sin(beta*t), cos(beta*t)}.
//! The number of basis functions = algDegree + 1 + 2*(alpha>0) + 2*(beta>0) must equal NbPoles.
//! Parameter range: [0, 1].
class GeomEval_AHTBezierCurve : public Geom_BoundedCurve
{
public:
  //! Non-rational constructor.
  //! @param[in] thePoles control points
  //! @param[in] theAlgDegree algebraic polynomial degree (>= 0)
  //! @param[in] theAlpha hyperbolic frequency (>= 0, 0 = no hyperbolic terms)
  //! @param[in] theBeta trigonometric frequency (>= 0, 0 = no trig terms)
  Standard_EXPORT GeomEval_AHTBezierCurve(const NCollection_Array1<gp_Pnt>& thePoles,
                                          int                               theAlgDegree,
                                          double                            theAlpha,
                                          double                            theBeta);

  //! Rational constructor.
  //! @param[in] thePoles control points
  //! @param[in] theWeights weights for each pole (must be > 0)
  //! @param[in] theAlgDegree algebraic polynomial degree (>= 0)
  //! @param[in] theAlpha hyperbolic frequency (>= 0, 0 = no hyperbolic terms)
  //! @param[in] theBeta trigonometric frequency (>= 0, 0 = no trig terms)
  Standard_EXPORT GeomEval_AHTBezierCurve(const NCollection_Array1<gp_Pnt>& thePoles,
                                          const NCollection_Array1<double>& theWeights,
                                          int                               theAlgDegree,
                                          double                            theAlpha,
                                          double                            theBeta);

  //! Returns the array of poles.
  Standard_EXPORT const NCollection_Array1<gp_Pnt>& Poles() const;

  //! Returns the array of weights.
  Standard_EXPORT const NCollection_Array1<double>& Weights() const;

  //! Returns the algebraic polynomial degree.
  Standard_EXPORT int AlgDegree() const;

  //! Returns the hyperbolic frequency parameter.
  Standard_EXPORT double Alpha() const;

  //! Returns the trigonometric frequency parameter.
  Standard_EXPORT double Beta() const;

  //! Returns the number of poles.
  Standard_EXPORT int NbPoles() const;

  //! Returns true if the curve is rational (has explicit weights).
  Standard_EXPORT bool IsRational() const;

  //! Returns the start point of the curve (at parameter 0).
  Standard_EXPORT gp_Pnt StartPoint() const final;

  //! Returns the end point of the curve (at parameter 1).
  Standard_EXPORT gp_Pnt EndPoint() const final;

  //! Reversal is not supported for this eval curve.
  //! @throw Standard_NotImplemented
  Standard_EXPORT void Reverse() final;

  //! Reversal is not supported for this eval curve.
  //! @throw Standard_NotImplemented
  Standard_EXPORT double ReversedParameter(const double U) const final;

  //! Returns the value of the first parameter: 0.0.
  Standard_EXPORT double FirstParameter() const final;

  //! Returns the value of the last parameter: 1.0.
  Standard_EXPORT double LastParameter() const final;

  //! Returns true if the curve is closed.
  Standard_EXPORT bool IsClosed() const final;

  //! Returns false. The AHT-Bezier curve is not periodic.
  Standard_EXPORT bool IsPeriodic() const final;

  //! Returns GeomAbs_CN.
  Standard_EXPORT GeomAbs_Shape Continuity() const final;

  //! Returns true for any N. The AHT-Bezier curve is infinitely differentiable.
  Standard_EXPORT bool IsCN(const int N) const final;

  //! Computes the point at parameter U.
  Standard_EXPORT gp_Pnt EvalD0(const double U) const final;

  //! Computes the point and first derivative at parameter U.
  Standard_EXPORT Geom_Curve::ResD1 EvalD1(const double U) const final;

  //! Computes the point and first two derivatives at parameter U.
  Standard_EXPORT Geom_Curve::ResD2 EvalD2(const double U) const final;

  //! Computes the point and first three derivatives at parameter U.
  Standard_EXPORT Geom_Curve::ResD3 EvalD3(const double U) const final;

  //! Computes the N-th derivative at parameter U.
  //! @param[in] U the parameter value
  //! @param[in] N the derivative order (must be >= 1)
  //! @return the N-th derivative vector
  Standard_EXPORT gp_Vec EvalDN(const double U, const int N) const final;

  //! Transformation is not supported for this eval geometry.
  //! @throw Standard_NotImplemented
  Standard_EXPORT void Transform(const gp_Trsf& T) final;

  //! Creates a new object which is a copy of this curve.
  Standard_EXPORT occ::handle<Geom_Geometry> Copy() const final;

  //! Dumps the content of me into the stream.
  Standard_EXPORT void DumpJson(Standard_OStream& theOStream, int theDepth = -1) const final;

  DEFINE_STANDARD_RTTIEXT(GeomEval_AHTBezierCurve, Geom_BoundedCurve)

private:
  //! Compute the number of basis functions for the given parameters.
  static int basisDimension(int theAlgDegree, double theAlpha, double theBeta);

  NCollection_Array1<gp_Pnt> myPoles;
  NCollection_Array1<double> myWeights;
  int                        myAlgDegree;
  double                     myAlpha;
  double                     myBeta;
  bool                       myRational;
};

#endif // _GeomEval_AHTBezierCurve_HeaderFile
