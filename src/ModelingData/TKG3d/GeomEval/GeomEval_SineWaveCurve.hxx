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

#ifndef _GeomEval_SineWaveCurve_HeaderFile
#define _GeomEval_SineWaveCurve_HeaderFile

#include <Geom_Curve.hxx>
#include <gp_Ax2.hxx>

class gp_Trsf;
class Geom_Geometry;

//! Describes a 3D sine wave curve.
//! The curve lies in the plane defined by the local coordinate system,
//! oscillating along YDir with propagation along XDir.
//!
//! The parametric equation is:
//! @code
//!   C(t) = O + t*XDir + A*sin(omega*t + phi)*YDir
//! @endcode
//! where:
//! - O, XDir, YDir are from the local coordinate system,
//! - A is the amplitude (> 0),
//! - omega is the angular frequency (> 0),
//! - phi is the phase shift.
//!
//! The parameter range is (-inf, +inf). The curve is periodic with period 2*Pi/omega.
class GeomEval_SineWaveCurve : public Geom_Curve
{
public:

  //! Creates a 3D sine wave curve.
  //! @param[in] thePosition the local coordinate system
  //! @param[in] theAmplitude the wave amplitude (must be > 0)
  //! @param[in] theOmega the angular frequency (must be > 0)
  //! @param[in] thePhase the phase shift (default 0)
  //! @throw Standard_ConstructionError if theAmplitude <= 0 or theOmega <= 0
  Standard_EXPORT GeomEval_SineWaveCurve(const gp_Ax2& thePosition,
                                     double        theAmplitude,
                                     double        theOmega,
                                     double        thePhase = 0.0);

  //! Returns the local coordinate system.
  Standard_EXPORT const gp_Ax2& Position() const;

  //! Returns the amplitude.
  Standard_EXPORT double Amplitude() const;

  //! Returns the angular frequency.
  Standard_EXPORT double Omega() const;

  //! Returns the phase shift.
  Standard_EXPORT double Phase() const;

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

  //! Returns true. Period = 2*Pi/omega.
  Standard_EXPORT bool IsPeriodic() const final;

  //! Returns the period: 2*Pi/omega.
  Standard_EXPORT double Period() const final;

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
  //! @throw Standard_RangeError if N < 1
  Standard_EXPORT gp_Vec EvalDN(const double U, const int N) const final;

  //! Applies the transformation T to this curve.
  Standard_EXPORT void Transform(const gp_Trsf& T) final;

  //! Creates a new object which is a copy of this curve.
  Standard_EXPORT occ::handle<Geom_Geometry> Copy() const final;

  //! Dumps the content of me into the stream.
  Standard_EXPORT void DumpJson(Standard_OStream& theOStream, int theDepth = -1) const final;

  DEFINE_STANDARD_RTTIEXT(GeomEval_SineWaveCurve, Geom_Curve)

private:

  gp_Ax2 myPosition;  //!< Local coordinate system
  double myAmplitude;  //!< Wave amplitude (> 0)
  double myOmega;      //!< Angular frequency (> 0)
  double myPhase;      //!< Phase shift
};

#endif // _GeomEval_SineWaveCurve_HeaderFile
