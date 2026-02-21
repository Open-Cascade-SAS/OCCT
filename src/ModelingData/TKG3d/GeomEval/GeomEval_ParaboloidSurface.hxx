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

#ifndef _GeomEval_ParaboloidSurface_HeaderFile
#define _GeomEval_ParaboloidSurface_HeaderFile

#include <Geom_ElementarySurface.hxx>

//! Describes a circular paraboloid surface of revolution.
//! A paraboloid is defined by its focal distance and is positioned
//! in space by a coordinate system (a gp_Ax3 object), the origin
//! of which is the vertex of the paraboloid.
//!
//! The parametric equation of the paraboloid is:
//! @code
//!   P(u,v) = O + v*cos(u)*XDir + v*sin(u)*YDir + v^2/(4*F)*ZDir
//! @endcode
//! where:
//! - O, XDir, YDir and ZDir are respectively the origin,
//!   the "X Direction", the "Y Direction" and the "Z Direction"
//!   of its local coordinate system, and
//! - F is the focal distance.
//!
//! The parametric range is:
//! - [0, 2*Pi] for u, and
//! - (-inf, +inf) for v.
//!
//! The implicit equation in local coordinates is:
//! @code
//!   X^2 + Y^2 - 4*F*Z = 0
//! @endcode
class GeomEval_ParaboloidSurface : public Geom_ElementarySurface
{
public:

  //! Creates a paraboloid surface with the given local coordinate system
  //! and focal distance.
  //! @param[in] thePosition the local coordinate system
  //! @param[in] theFocal the focal distance (must be > 0)
  //! @throw Standard_ConstructionError if theFocal <= 0
  Standard_EXPORT GeomEval_ParaboloidSurface(const gp_Ax3& thePosition, double theFocal);

  //! Returns the focal distance of this paraboloid.
  Standard_EXPORT double Focal() const;

  //! Assigns the value theFocal to the focal distance of this paraboloid.
  //! @param[in] theFocal the new focal distance (must be > 0)
  //! @throw Standard_ConstructionError if theFocal <= 0
  Standard_EXPORT void SetFocal(double theFocal);

  //! Reversal is not supported for this eval surface.
  //! @throw Standard_NotImplemented
  Standard_EXPORT void UReverse() final;

  //! Reversal is not supported for this eval surface.
  //! @throw Standard_NotImplemented
  Standard_EXPORT void VReverse() final;

  //! Reversal is not supported for this eval surface.
  //! @throw Standard_NotImplemented
  Standard_EXPORT double UReversedParameter(const double U) const final;

  //! Reversal is not supported for this eval surface.
  //! @throw Standard_NotImplemented
  Standard_EXPORT double VReversedParameter(const double V) const final;

  //! Returns the parametric bounds U1, U2, V1 and V2 of this paraboloid.
  //! @param[out] U1 lower U bound (0)
  //! @param[out] U2 upper U bound (2*Pi)
  //! @param[out] V1 lower V bound (-Precision::Infinite())
  //! @param[out] V2 upper V bound (Precision::Infinite())
  Standard_EXPORT void Bounds(double& U1, double& U2, double& V1, double& V2) const final;

  //! Returns True. The paraboloid is closed in U (period 2*Pi).
  Standard_EXPORT bool IsUClosed() const final;

  //! Returns False.
  Standard_EXPORT bool IsVClosed() const final;

  //! Returns True. The paraboloid is periodic in U (period 2*Pi).
  Standard_EXPORT bool IsUPeriodic() const final;

  //! Returns False.
  Standard_EXPORT bool IsVPeriodic() const final;

  //! Computes the U isoparametric curve.
  //! For a paraboloid, the U isoparametric curve is a parabola,
  //! which is not a standard Geom_Curve type.
  //! @throw Standard_NotImplemented
  Standard_EXPORT occ::handle<Geom_Curve> UIso(const double U) const final;

  //! Computes the V isoparametric curve.
  //! For a paraboloid, the V isoparametric curve is a circle of radius |v|,
  //! which degenerates to a point at v=0.
  //! @throw Standard_NotImplemented
  Standard_EXPORT occ::handle<Geom_Curve> VIso(const double V) const final;

  //! Computes the point P(U, V) on the surface.
  //! @code
  //!   P(U, V) = O + V*cos(U)*XDir + V*sin(U)*YDir + V^2/(4*F)*ZDir
  //! @endcode
  Standard_EXPORT gp_Pnt EvalD0(const double U, const double V) const final;

  //! Computes the point and the first partial derivatives at (U, V).
  Standard_EXPORT Geom_Surface::ResD1 EvalD1(const double U, const double V) const final;

  //! Computes the point and partial derivatives up to 2nd order at (U, V).
  Standard_EXPORT Geom_Surface::ResD2 EvalD2(const double U, const double V) const final;

  //! Computes the point and partial derivatives up to 3rd order at (U, V).
  Standard_EXPORT Geom_Surface::ResD3 EvalD3(const double U, const double V) const final;

  //! Computes the derivative of order Nu in the direction u
  //! and Nv in the direction v.
  //! @param[in] U the u parameter
  //! @param[in] V the v parameter
  //! @param[in] Nu derivative order in u (must be >= 0)
  //! @param[in] Nv derivative order in v (must be >= 0)
  //! @return the derivative vector
  //! @throw Geom_UndefinedDerivative if Nu + Nv < 1 or Nu < 0 or Nv < 0
  Standard_EXPORT gp_Vec EvalDN(const double U,
                                const double V,
                                const int    Nu,
                                const int    Nv) const final;

  //! Applies the transformation T to this paraboloid.
  Standard_EXPORT void Transform(const gp_Trsf& T) final;

  //! Creates a new object which is a copy of this paraboloid.
  Standard_EXPORT occ::handle<Geom_Geometry> Copy() const final;

  //! Dumps the content of me into the stream.
  Standard_EXPORT void DumpJson(Standard_OStream& theOStream, int theDepth = -1) const final;

  //! Returns the coefficients of the implicit equation of the
  //! quadric in the absolute Cartesian coordinate system:
  //! @code
  //!   A1*X^2 + A2*Y^2 + A3*Z^2 + 2*(B1*X*Y + B2*X*Z + B3*Y*Z) +
  //!   2*(C1*X + C2*Y + C3*Z) + D = 0
  //! @endcode
  //! In local coordinates the equation is: X^2 + Y^2 - 4*F*Z = 0.
  Standard_EXPORT void Coefficients(double& A1,
                                    double& A2,
                                    double& A3,
                                    double& B1,
                                    double& B2,
                                    double& B3,
                                    double& C1,
                                    double& C2,
                                    double& C3,
                                    double& D) const;

  DEFINE_STANDARD_RTTIEXT(GeomEval_ParaboloidSurface, Geom_ElementarySurface)

private:

  double myFocal;
};

#endif // _GeomEval_ParaboloidSurface_HeaderFile
