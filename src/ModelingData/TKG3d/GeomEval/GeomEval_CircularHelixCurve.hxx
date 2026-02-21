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

#ifndef _GeomEval_CircularHelixCurve_HeaderFile
#define _GeomEval_CircularHelixCurve_HeaderFile

#include <Geom_Curve.hxx>
#include <gp_Ax2.hxx>

class gp_Trsf;
class Geom_Geometry;

//! Describes a circular helix in 3D space.
//! A circular helix is an unbounded curve defined by a radius R,
//! a pitch P (axial advance per full 2*Pi turn), and a coordinate system.
//!
//! The parametric equation is:
//! @code
//!   C(t) = O + R*cos(t)*XDir + R*sin(t)*YDir + (P*t/(2*Pi))*ZDir
//! @endcode
//! where:
//! - O, XDir, YDir, ZDir are the origin and directions of the local coordinate system,
//! - R is the radius (> 0),
//! - P is the pitch (can be negative for left-handed helix).
//!
//! The parameter range is (-inf, +inf). The curve is neither periodic nor closed.
//! Continuity is GeomAbs_CN.
class GeomEval_CircularHelixCurve : public Geom_Curve
{
public:

  //! Creates a circular helix with the given coordinate system, radius, and pitch.
  //! @param[in] thePosition the local coordinate system
  //! @param[in] theRadius the helix radius (must be > 0)
  //! @param[in] thePitch the axial advance per 2*Pi turn (can be negative)
  //! @throw Standard_ConstructionError if theRadius <= 0
  Standard_EXPORT GeomEval_CircularHelixCurve(const gp_Ax2& thePosition,
                                              double        theRadius,
                                              double        thePitch);

  //! Returns the local coordinate system.
  Standard_EXPORT const gp_Ax2& Position() const;

  //! Returns the helix radius.
  Standard_EXPORT double Radius() const;

  //! Returns the pitch (axial advance per 2*Pi turn).
  Standard_EXPORT double Pitch() const;

  //! Reverses the direction of parametrization.
  //! Negates XDir and the pitch.
  Standard_EXPORT void Reverse() final;

  //! Returns the parameter on the reversed curve for
  //! the point of parameter U on this curve.
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
  //! @throw Standard_RangeError if N < 1
  Standard_EXPORT gp_Vec EvalDN(const double U, const int N) const final;

  //! Transformation is not supported for this eval geometry.
  //! @throw Standard_NotImplemented
  Standard_EXPORT void Transform(const gp_Trsf& T) final;

  //! Creates a new object which is a copy of this curve.
  Standard_EXPORT occ::handle<Geom_Geometry> Copy() const final;

  //! Dumps the content of me into the stream.
  Standard_EXPORT void DumpJson(Standard_OStream& theOStream, int theDepth = -1) const final;

  DEFINE_STANDARD_RTTIEXT(GeomEval_CircularHelixCurve, Geom_Curve)

private:

  gp_Ax2 myPosition; //!< Local coordinate system
  double myRadius;    //!< Helix radius (> 0)
  double myPitch;     //!< Axial advance per 2*Pi turn
};

#endif // _GeomEval_CircularHelixCurve_HeaderFile
