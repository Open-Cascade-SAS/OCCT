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

#ifndef _GeomEval_HyperboloidSurface_HeaderFile
#define _GeomEval_HyperboloidSurface_HeaderFile

#include <Geom_ElementarySurface.hxx>

//! Describes a hyperboloid of revolution surface (one-sheet or two-sheet).
//!
//! A hyperboloid is defined by two semi-axis radii R1 and R2, a sheet mode,
//! and is positioned in space by a coordinate system (a gp_Ax3 object),
//! the origin of which is the center of the hyperboloid.
//!
//! **One-sheet parametrization:**
//! @code
//!   P(u,v) = O + R1*cosh(v)*cos(u)*XDir + R1*cosh(v)*sin(u)*YDir + R2*sinh(v)*ZDir
//! @endcode
//! Implicit equation in local coordinates:
//! @code
//!   X^2/R1^2 + Y^2/R1^2 - Z^2/R2^2 = 1
//! @endcode
//!
//! **Two-sheet parametrization** (covers one sheet only):
//! @code
//!   P(u,v) = O + R2*sinh(v)*cos(u)*XDir + R2*sinh(v)*sin(u)*YDir + R1*cosh(v)*ZDir
//! @endcode
//! Implicit equation in local coordinates:
//! @code
//!   X^2/R2^2 + Y^2/R2^2 - Z^2/R1^2 = -1
//! @endcode
//! The second sheet is not represented by this class.
//!
//! The parametric range is:
//! - [0, 2*Pi] for u (periodic, closed), and
//! - (-inf, +inf) for v (not periodic, not closed).
class GeomEval_HyperboloidSurface : public Geom_ElementarySurface
{
public:
  //! Sheet mode selector.
  enum class SheetMode
  {
    OneSheet, //!< hyperboloid of one sheet
    TwoSheets //!< hyperboloid of two sheets (one sheet only)
  };

  //! Creates a hyperboloid surface with the given local coordinate system,
  //! semi-axis radii, and sheet mode.
  //! @param[in] thePosition local coordinate system
  //! @param[in] theR1 first semi-axis radius (must be > 0)
  //! @param[in] theR2 second semi-axis radius (must be > 0)
  //! @param[in] theMode one-sheet or two-sheet mode
  //! @throw Standard_ConstructionError if theR1 <= 0 or theR2 <= 0
  Standard_EXPORT GeomEval_HyperboloidSurface(const gp_Ax3& thePosition,
                                              double        theR1,
                                              double        theR2,
                                              SheetMode     theMode = SheetMode::OneSheet);

  //! Returns the first semi-axis radius.
  Standard_EXPORT double R1() const;

  //! Returns the second semi-axis radius.
  Standard_EXPORT double R2() const;

  //! Returns the sheet mode.
  Standard_EXPORT SheetMode Mode() const;

  //! Assigns the value theR1 to the first semi-axis radius.
  //! @param[in] theR1 the new first semi-axis radius (must be > 0)
  //! @throw Standard_ConstructionError if theR1 <= 0
  Standard_EXPORT void SetR1(double theR1);

  //! Assigns the value theR2 to the second semi-axis radius.
  //! @param[in] theR2 the new second semi-axis radius (must be > 0)
  //! @throw Standard_ConstructionError if theR2 <= 0
  Standard_EXPORT void SetR2(double theR2);

  //! Sets the sheet mode.
  //! @param[in] theMode one-sheet or two-sheet mode
  Standard_EXPORT void SetMode(SheetMode theMode);

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

  //! Returns the parametric bounds U1, U2, V1 and V2 of this hyperboloid.
  //! @param[out] U1 lower U bound (0)
  //! @param[out] U2 upper U bound (2*Pi)
  //! @param[out] V1 lower V bound (-Precision::Infinite())
  //! @param[out] V2 upper V bound (Precision::Infinite())
  Standard_EXPORT void Bounds(double& U1, double& U2, double& V1, double& V2) const final;

  //! Returns True. The hyperboloid is closed in U (period 2*Pi).
  Standard_EXPORT bool IsUClosed() const final;

  //! Returns False.
  Standard_EXPORT bool IsVClosed() const final;

  //! Returns True. The hyperboloid is periodic in U (period 2*Pi).
  Standard_EXPORT bool IsUPeriodic() const final;

  //! Returns False.
  Standard_EXPORT bool IsVPeriodic() const final;

  //! Computes the U isoparametric curve.
  //! For a hyperboloid, no standard Geom_Curve representation is available.
  //! @throw Standard_NotImplemented
  Standard_EXPORT occ::handle<Geom_Curve> UIso(const double U) const final;

  //! Computes the V isoparametric curve.
  //! For a hyperboloid, no standard Geom_Curve representation is available.
  //! @throw Standard_NotImplemented
  Standard_EXPORT occ::handle<Geom_Curve> VIso(const double V) const final;

  //! Computes the point P(U, V) on the surface.
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

  //! Transformation is not supported for this eval geometry.
  //! @throw Standard_NotImplemented
  Standard_EXPORT void Transform(const gp_Trsf& T) final;

  //! Creates a new object which is a copy of this hyperboloid.
  Standard_EXPORT occ::handle<Geom_Geometry> Copy() const final;

  //! Dumps the content of me into the stream.
  Standard_EXPORT void DumpJson(Standard_OStream& theOStream, int theDepth = -1) const final;

  //! Returns the coefficients of the implicit equation of the
  //! quadric in the absolute Cartesian coordinate system:
  //! @code
  //!   A1*X^2 + A2*Y^2 + A3*Z^2 + 2*(B1*X*Y + B2*X*Z + B3*Y*Z) +
  //!   2*(C1*X + C2*Y + C3*Z) + D = 0
  //! @endcode
  //! For one-sheet (local): X^2/R1^2 + Y^2/R1^2 - Z^2/R2^2 - 1 = 0.
  //! For two-sheet (local): X^2/R2^2 + Y^2/R2^2 - Z^2/R1^2 + 1 = 0.
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

  DEFINE_STANDARD_RTTIEXT(GeomEval_HyperboloidSurface, Geom_ElementarySurface)

private:
  double    myR1;
  double    myR2;
  SheetMode myMode;
};

#endif // _GeomEval_HyperboloidSurface_HeaderFile
