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

#ifndef _Geom2dEval_LogarithmicSpiralCurve_HeaderFile
#define _Geom2dEval_LogarithmicSpiralCurve_HeaderFile

#include <Geom2d_Curve.hxx>
#include <gp_Ax2d.hxx>

class gp_Trsf2d;
class Geom2d_Geometry;

//! Describes a 2D logarithmic (equiangular) spiral curve.
//! The polar equation is r = a*exp(b*t).
//!
//! The parametric equation is:
//! @code
//!   C(t) = O + a*exp(b*t)*cos(t)*XDir + a*exp(b*t)*sin(t)*YDir
//! @endcode
//! where:
//! - O, XDir are from the local coordinate system,
//! - YDir is the perpendicular to XDir,
//! - a is the scale factor (> 0),
//! - b is the growth exponent (> 0).
//!
//! The parameter range is (-inf, +inf).
//! The angle between tangent and radial direction is constant = atan(1/b).
class Geom2dEval_LogarithmicSpiralCurve : public Geom2d_Curve
{
public:

  //! Creates a logarithmic spiral.
  //! @param[in] thePosition the local coordinate system
  //! @param[in] theScale the scale factor (must be > 0)
  //! @param[in] theGrowthExponent the growth exponent (must be > 0)
  //! @throw Standard_ConstructionError if theScale <= 0 or theGrowthExponent <= 0
  Standard_EXPORT Geom2dEval_LogarithmicSpiralCurve(const gp_Ax2d& thePosition,
                                                    double         theScale,
                                                    double         theGrowthExponent);

  //! Returns the local coordinate system.
  Standard_EXPORT const gp_Ax2d& Position() const;

  //! Returns the scale factor.
  Standard_EXPORT double Scale() const;

  //! Returns the growth exponent.
  Standard_EXPORT double GrowthExponent() const;

  //! Reverses the direction of parametrization.
  Standard_EXPORT void Reverse() final;

  //! @return -U
  Standard_EXPORT double ReversedParameter(const double U) const final;

  //! Returns -Precision::Infinite().
  Standard_EXPORT double FirstParameter() const final;

  //! Returns Precision::Infinite().
  Standard_EXPORT double LastParameter() const final;

  //! Returns false.
  Standard_EXPORT bool IsClosed() const final;

  //! Returns false.
  Standard_EXPORT bool IsPeriodic() const final;

  //! Returns GeomAbs_CN.
  Standard_EXPORT GeomAbs_Shape Continuity() const final;

  //! Returns true for any N >= 0.
  Standard_EXPORT bool IsCN(const int N) const final;

  //! Computes the point at parameter U.
  Standard_EXPORT gp_Pnt2d EvalD0(const double U) const final;

  //! Computes the point and first derivative at parameter U.
  Standard_EXPORT Geom2d_Curve::ResD1 EvalD1(const double U) const final;

  //! Computes the point and first two derivatives at parameter U.
  Standard_EXPORT Geom2d_Curve::ResD2 EvalD2(const double U) const final;

  //! Computes the point and first three derivatives at parameter U.
  Standard_EXPORT Geom2d_Curve::ResD3 EvalD3(const double U) const final;

  //! Computes the N-th derivative at parameter U.
  //! @throw Standard_RangeError if N < 1
  Standard_EXPORT gp_Vec2d EvalDN(const double U, const int N) const final;

  //! Applies the transformation T to this curve.
  Standard_EXPORT void Transform(const gp_Trsf2d& T) final;

  //! Creates a new object which is a copy of this curve.
  Standard_EXPORT occ::handle<Geom2d_Geometry> Copy() const final;

  //! Dumps the content of me into the stream.
  Standard_EXPORT void DumpJson(Standard_OStream& theOStream, int theDepth = -1) const final;

  DEFINE_STANDARD_RTTIEXT(Geom2dEval_LogarithmicSpiralCurve, Geom2d_Curve)

private:

  gp_Ax2d myPosition;       //!< Local coordinate system
  double  myScale;           //!< Scale factor (> 0)
  double  myGrowthExponent;  //!< Growth exponent (> 0)
};

#endif // _Geom2dEval_LogarithmicSpiralCurve_HeaderFile
