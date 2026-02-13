// Created on: 1993-03-10
// Created by: JCV
// Copyright (c) 1993-1999 Matra Datavision
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

#ifndef _Geom_ConicalSurface_HeaderFile
#define _Geom_ConicalSurface_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Geom_ElementarySurface.hxx>

class gp_Ax3;
class gp_Cone;
class gp_Trsf;
class gp_GTrsf2d;
class Geom_Geometry;

//! Describes a cone.
//! A cone is defined by the half-angle (can be negative) at its apex, and
//! is positioned in space by a coordinate system (a
//! gp_Ax3 object) and a reference radius as follows:
//! - The "main Axis" of the coordinate system is the
//! axis of revolution of the cone.
//! - The plane defined by the origin, the "X Direction"
//! and the "Y Direction" of the coordinate system is
//! the reference plane of the cone. The intersection
//! of the cone with this reference plane is a circle of
//! radius equal to the reference radius.
//! - The apex of the cone is on the negative side of
//! the "main Axis" of the coordinate system if the
//! half-angle is positive, and on the positive side if
//! the half-angle is negative.
//! This coordinate system is the "local coordinate
//! system" of the cone. The following apply:
//! - Rotation around its "main Axis", in the
//! trigonometric sense given by the "X Direction"
//! and the "Y Direction", defines the u parametric direction.
//! - Its "X Axis" gives the origin for the u parameter.
//! - Its "main Direction" is the v parametric direction of the cone.
//! - Its origin is the origin of the v parameter.
//! The parametric range of the two parameters is:
//! @code
//!  - [ 0, 2.*Pi ] for u, and
//!  - ] -infinity, +infinity [ for v
//! @endcode
//! The parametric equation of the cone is:
//! @code
//! P(u, v) = O + (R + v*sin(Ang)) * (cos(u)*XDir + sin(u)*YDir) + v*cos(Ang)*ZDir
//! @endcode
//! where:
//! - O, XDir, YDir and ZDir are respectively
//! the origin, the "X Direction", the "Y Direction" and
//! the "Z Direction" of the cone's local coordinate system,
//! - Ang is the half-angle at the apex of the cone, and
//! - R is the reference radius.
class Geom_ConicalSurface : public Geom_ElementarySurface
{

public:
  //! A3 defines the local coordinate system of the conical surface.
  //! Ang is the conical surface semi-angle. Its absolute value is in range
  //! ]0, PI/2[.
  //! Radius is the radius of the circle Viso in the placement plane
  //! of the conical surface defined with "XAxis" and "YAxis".
  //! The "ZDirection" of A3 defines the direction of the surface's
  //! axis of symmetry.
  //! If the location point of A3 is the apex of the surface
  //! Radius = 0 .
  //! At the creation the parametrization of the surface is defined
  //! such that the normal Vector (N = D1U ^ D1V) is oriented towards
  //! the "outside region" of the surface.
  //!
  //! Raised if Radius < 0.0 or std::abs(Ang) < Resolution from gp or
  //! std::abs(Ang) >= PI/2 - Resolution
  Standard_EXPORT Geom_ConicalSurface(const gp_Ax3& A3, const double Ang, const double Radius);

  //! Creates a ConicalSurface from a non transient gp_Cone.
  Standard_EXPORT Geom_ConicalSurface(const gp_Cone& C);

  //! Set <me> so that <me> has the same geometric properties as C.
  Standard_EXPORT void SetCone(const gp_Cone& C);

  //! Changes the radius of the conical surface in the placement plane (Z = 0, V = 0).
  //! The local coordinate system is not modified.
  //! Raised if R < 0.0
  Standard_EXPORT void SetRadius(const double R);

  //! Changes the semi angle of the conical surface.
  //! Semi-angle can be negative. Its absolute value
  //! std::abs(Ang) is in range ]0,PI/2[.
  //! Raises ConstructionError if std::abs(Ang) < Resolution from gp or
  //! std::abs(Ang) >= PI/2 - Resolution
  Standard_EXPORT void SetSemiAngle(const double Ang);

  //! Returns a non transient cone with the same geometric properties as <me>.
  Standard_EXPORT gp_Cone Cone() const;

  //! Eeturn 2.PI - U.
  Standard_EXPORT double UReversedParameter(const double U) const final;

  //! Computes the u (or v) parameter on the modified surface,
  //! when reversing its u (or v) parametric direction,
  //! for any point of u parameter U (or of v parameter V) on this cone.
  //! In the case of a cone, these functions return respectively:
  //! - 2.*Pi - U, -V.
  Standard_EXPORT double VReversedParameter(const double V) const final;

  //! Changes the orientation of this cone in the v parametric direction.
  //! The bounds of the surface are not changed but the v parametric direction is reversed.
  //! As a consequence, for a cone:
  //! - the "main Direction" of the local coordinate system
  //! is reversed, and
  //! - the half-angle at the apex is inverted.
  Standard_EXPORT void VReverse() final;

  //! Computes the parameters on the transformed surface for
  //! the transform of the point of parameters U,V on <me>.
  //! @code
  //!   me->Transformed(T)->Value(U',V')
  //! @endcode
  //! is the same point as
  //! @code
  //!   me->Value(U,V).Transformed(T)
  //! @endcode
  //! Where U',V' are the new values of U,V after calling
  //! @code
  //!   me->TransformParameters(U,V,T)
  //! @endcode
  //! This method multiplies V by T.ScaleFactor()
  Standard_EXPORT void TransformParameters(double& U, double& V, const gp_Trsf& T) const final;

  //! Returns a 2d transformation used to find the new
  //! parameters of a point on the transformed surface.
  //! @code
  //!   me->Transformed(T)->Value(U',V')
  //! @endcode
  //! is the same point as
  //! @code
  //!   me->Value(U,V).Transformed(T)
  //! @endcode
  //! Where U',V' are obtained by transforming U,V with the 2d transformation returned by
  //! @code
  //!   me->ParametricTransformation(T)
  //! @endcode
  //! This method returns a scale centered on the U axis with T.ScaleFactor
  Standard_EXPORT gp_GTrsf2d ParametricTransformation(const gp_Trsf& T) const final;

  //! Computes the apex of this cone. It is on the negative
  //! side of the axis of revolution of this cone if the
  //! half-angle at the apex is positive, and on the positive
  //! side of the "main Axis" if the half-angle is negative.
  Standard_EXPORT gp_Pnt Apex() const;

  //! The conical surface is infinite in the V direction so
  //! V1 = Realfirst from Standard and V2 = RealLast.
  //! U1 = 0 and U2 = 2*PI.
  Standard_EXPORT void Bounds(double& U1, double& U2, double& V1, double& V2) const final;

  //! Returns the coefficients of the implicit equation of the
  //! quadric in the absolute cartesian coordinate system :
  //! These coefficients are normalized.
  //! @code
  //!   A1.X**2 + A2.Y**2 + A3.Z**2 + 2.(B1.X.Y + B2.X.Z + B3.Y.Z) + 2.(C1.X + C2.Y + C3.Z) + D =
  //!   0.0
  //! @endcode
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

  //! Returns the reference radius of this cone.
  //! The reference radius is the radius of the circle formed
  //! by the intersection of this cone and its reference
  //! plane (i.e. the plane defined by the origin, "X
  //! Direction" and "Y Direction" of the local coordinate
  //! system of this cone).
  //! If the apex of this cone is on the origin of the local
  //! coordinate system of this cone, the returned value is 0.
  Standard_EXPORT double RefRadius() const;

  //! Returns the semi-angle at the apex of this cone.
  //! Attention! Semi-angle can be negative.
  Standard_EXPORT double SemiAngle() const;

  //! returns True.
  Standard_EXPORT bool IsUClosed() const final;

  //! returns False.
  Standard_EXPORT bool IsVClosed() const final;

  //! Returns True.
  Standard_EXPORT bool IsUPeriodic() const final;

  //! Returns False.
  Standard_EXPORT bool IsVPeriodic() const final;

  //! Builds the U isoparametric line of this cone.
  //! The origin of this line is on the reference plane of this cone
  //! (i.e. the plane defined by the origin, "X Direction"
  //! and "Y Direction" of the local coordinate system of this cone).
  Standard_EXPORT occ::handle<Geom_Curve> UIso(const double U) const final;

  //! Builds the V isoparametric circle of this cone.
  //! It is the circle on this cone, located in the plane of Z
  //! coordinate V*cos(Semi-Angle) in the local coordinate system of this cone.
  //! The "Axis" of this circle is the axis of revolution of this cone.
  //! Its starting point is defined by the "X Direction" of this cone.
  //! Warning
  //! If the V isoparametric circle is close to the apex of
  //! this cone, the radius of the circle becomes very small.
  //! It is possible to have a circle with radius equal to 0.0.
  Standard_EXPORT occ::handle<Geom_Curve> VIso(const double V) const final;

  //! Computes the point P (U, V) on the surface.
  //! @code
  //!   P (U, V) = Loc +
  //!              (RefRadius + V * sin (Semi-Angle)) * (cos (U) * XDir + sin (U) * YDir) +
  //!              V * cos (Semi-Angle) * ZDir
  //! @endcode
  //! where Loc is the origin of the placement plane (XAxis, YAxis)
  //! XDir is the direction of the XAxis and YDir the direction of the YAxis.
  Standard_EXPORT std::optional<gp_Pnt> EvalD0(const double U, const double V) const final;

  //! Computes the current point and the first derivatives in the directions U and V.
  Standard_EXPORT std::optional<Geom_Surface::ResD1> EvalD1(const double U, const double V) const final;

  //! Computes the current point, the first and the second derivatives in the directions U and V.
  Standard_EXPORT std::optional<Geom_Surface::ResD2> EvalD2(const double U, const double V) const final;

  //! Computes the current point, the first,the second and the third
  //! derivatives in the directions U and V.
  Standard_EXPORT std::optional<Geom_Surface::ResD3> EvalD3(const double U, const double V) const final;

  //! Computes the derivative of order Nu in the u
  //! parametric direction, and Nv in the v parametric
  //! direction at the point of parameters (U, V) of this cone.
  //! Exceptions
  //! Standard_RangeError if:
  //! - Nu + Nv is less than 1,
  //! - Nu or Nv is negative.
  Standard_EXPORT std::optional<gp_Vec> EvalDN(const double U,
                                               const double V,
                                               const int    Nu,
                                               const int    Nv) const final;

  //! Applies the transformation T to this cone.
  Standard_EXPORT void Transform(const gp_Trsf& T) final;

  //! Creates a new object which is a copy of this cone.
  Standard_EXPORT occ::handle<Geom_Geometry> Copy() const final;
  //! Dumps the content of me into the stream
  Standard_EXPORT void DumpJson(Standard_OStream& theOStream, int theDepth = -1) const final;

  DEFINE_STANDARD_RTTIEXT(Geom_ConicalSurface, Geom_ElementarySurface)

private:
  double radius;
  double semiAngle;
};

#endif // _Geom_ConicalSurface_HeaderFile
