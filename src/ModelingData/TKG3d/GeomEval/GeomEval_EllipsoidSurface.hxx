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

#ifndef _GeomEval_EllipsoidSurface_HeaderFile
#define _GeomEval_EllipsoidSurface_HeaderFile

#include <Geom_ElementarySurface.hxx>

//! Describes a triaxial ellipsoid surface.
//! An ellipsoid is defined by three semi-axes A, B, C (all > 0)
//! and is positioned in space by a coordinate system (a gp_Ax3 object),
//! the origin of which is the center of the ellipsoid.
//!
//! The parametric equation of the ellipsoid is:
//! @code
//!   P(u,v) = O + A*cos(v)*cos(u)*XDir + B*cos(v)*sin(u)*YDir + C*sin(v)*ZDir
//! @endcode
//! where:
//! - O, XDir, YDir and ZDir are respectively the origin,
//!   the "X Direction", the "Y Direction" and the "Z Direction"
//!   of its local coordinate system, and
//! - A, B, C are the three semi-axes.
//!
//! The parametric range is:
//! - [0, 2*Pi] for u, and
//! - [-Pi/2, Pi/2] for v.
//!
//! When A == B the surface degenerates to a spheroid (ellipsoid of revolution).
//!
//! The implicit equation in local coordinates is:
//! @code
//!   X^2/A^2 + Y^2/B^2 + Z^2/C^2 - 1 = 0
//! @endcode
class GeomEval_EllipsoidSurface : public Geom_ElementarySurface
{
public:

  //! Creates a triaxial ellipsoid surface with the given local coordinate system
  //! and three semi-axes.
  //! @param[in] thePosition the local coordinate system
  //! @param[in] theA the semi-axis along XDir (must be > 0)
  //! @param[in] theB the semi-axis along YDir (must be > 0)
  //! @param[in] theC the semi-axis along ZDir (must be > 0)
  //! @throw Standard_ConstructionError if any semi-axis <= 0
  Standard_EXPORT GeomEval_EllipsoidSurface(const gp_Ax3& thePosition,
                                            double        theA,
                                            double        theB,
                                            double        theC);

  //! Returns the semi-axis A (along XDir).
  Standard_EXPORT double SemiAxisA() const;

  //! Returns the semi-axis B (along YDir).
  Standard_EXPORT double SemiAxisB() const;

  //! Returns the semi-axis C (along ZDir).
  Standard_EXPORT double SemiAxisC() const;

  //! Assigns the value theA to the semi-axis A.
  //! @param[in] theA the new semi-axis value (must be > 0)
  //! @throw Standard_ConstructionError if theA <= 0
  Standard_EXPORT void SetSemiAxisA(double theA);

  //! Assigns the value theB to the semi-axis B.
  //! @param[in] theB the new semi-axis value (must be > 0)
  //! @throw Standard_ConstructionError if theB <= 0
  Standard_EXPORT void SetSemiAxisB(double theB);

  //! Assigns the value theC to the semi-axis C.
  //! @param[in] theC the new semi-axis value (must be > 0)
  //! @throw Standard_ConstructionError if theC <= 0
  Standard_EXPORT void SetSemiAxisC(double theC);

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

  //! Returns the parametric bounds U1, U2, V1 and V2 of this ellipsoid.
  //! @param[out] U1 lower U bound (0)
  //! @param[out] U2 upper U bound (2*Pi)
  //! @param[out] V1 lower V bound (-Pi/2)
  //! @param[out] V2 upper V bound (Pi/2)
  Standard_EXPORT void Bounds(double& U1, double& U2, double& V1, double& V2) const final;

  //! Returns True. The ellipsoid is closed in U (period 2*Pi).
  Standard_EXPORT bool IsUClosed() const final;

  //! Returns False.
  Standard_EXPORT bool IsVClosed() const final;

  //! Returns True. The ellipsoid is periodic in U (period 2*Pi).
  Standard_EXPORT bool IsUPeriodic() const final;

  //! Returns False.
  Standard_EXPORT bool IsVPeriodic() const final;

  //! Computes the U isoparametric curve.
  //! For a triaxial ellipsoid, the U isoparametric curve is not
  //! a standard Geom_Curve type.
  //! @throw Standard_NotImplemented
  Standard_EXPORT occ::handle<Geom_Curve> UIso(const double U) const final;

  //! Computes the V isoparametric curve.
  //! For a triaxial ellipsoid, the V isoparametric curve is not
  //! a standard Geom_Curve type (it is an ellipse only when A == B).
  //! @throw Standard_NotImplemented
  Standard_EXPORT occ::handle<Geom_Curve> VIso(const double V) const final;

  //! Computes the point P(U, V) on the surface.
  //! @code
  //!   P(U, V) = O + A*cos(V)*cos(U)*XDir + B*cos(V)*sin(U)*YDir + C*sin(V)*ZDir
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

  //! Applies the transformation T to this ellipsoid.
  //! Scales all three semi-axes by Abs(T.ScaleFactor()) and transforms
  //! the local coordinate system.
  Standard_EXPORT void Transform(const gp_Trsf& T) final;

  //! Creates a new object which is a copy of this ellipsoid.
  Standard_EXPORT occ::handle<Geom_Geometry> Copy() const final;

  //! Dumps the content of me into the stream.
  Standard_EXPORT void DumpJson(Standard_OStream& theOStream, int theDepth = -1) const final;

  //! Returns the coefficients of the implicit equation of the
  //! quadric in the absolute Cartesian coordinate system:
  //! @code
  //!   A1*X^2 + A2*Y^2 + A3*Z^2 + 2*(B1*X*Y + B2*X*Z + B3*Y*Z) +
  //!   2*(C1*X + C2*Y + C3*Z) + D = 0
  //! @endcode
  //! In local coordinates the equation is: X^2/A^2 + Y^2/B^2 + Z^2/C^2 - 1 = 0.
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

  DEFINE_STANDARD_RTTIEXT(GeomEval_EllipsoidSurface, Geom_ElementarySurface)

private:

  double myA; //!< Semi-axis along XDir
  double myB; //!< Semi-axis along YDir
  double myC; //!< Semi-axis along ZDir
};

#endif // _GeomEval_EllipsoidSurface_HeaderFile
