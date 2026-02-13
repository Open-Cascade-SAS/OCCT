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

#ifndef _Geom_CylindricalSurface_HeaderFile
#define _Geom_CylindricalSurface_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Geom_ElementarySurface.hxx>

class gp_Ax3;
class gp_Cylinder;
class gp_Trsf;
class gp_GTrsf2d;
class Geom_Geometry;

//! This class defines the infinite cylindrical surface.
//!
//! Every cylindrical surface is set by the following equation:
//! @code
//!   S(U,V) = Location + R*cos(U)*XAxis + R*sin(U)*YAxis + V*ZAxis,
//! @endcode
//! where R is cylinder radius.
//!
//! The local coordinate system of the CylindricalSurface is defined
//! with an axis placement (see class ElementarySurface).
//!
//! The "ZAxis" is the symmetry axis of the CylindricalSurface,
//! it gives the direction of increasing parametric value V.
//!
//! The parametrization range is :
//! @code
//!   U [0, 2*PI], V ]- infinite, + infinite[
//! @endcode
//!
//! The "XAxis" and the "YAxis" define the placement plane of the
//! surface (Z = 0, and parametric value V = 0) perpendicular to
//! the symmetry axis. The "XAxis" defines the origin of the
//! parameter U = 0. The trigonometric sense gives the positive
//! orientation for the parameter U.
//!
//! When you create a CylindricalSurface the U and V directions of
//! parametrization are such that at each point of the surface the
//! normal is oriented towards the "outside region".
//!
//! The methods UReverse VReverse change the orientation of the
//! surface.
class Geom_CylindricalSurface : public Geom_ElementarySurface
{

public:
  //! A3 defines the local coordinate system of the cylindrical surface.
  //! The "ZDirection" of A3 defines the direction of the surface's axis of symmetry.
  //! At the creation the parametrization of the surface is defined
  //! such that the normal Vector (N = D1U ^ D1V) is oriented towards
  //! the "outside region" of the surface.
  //! Warnings:
  //! It is not forbidden to create a cylindrical surface with
  //! Radius = 0.0
  //! Raised if Radius < 0.0
  Standard_EXPORT Geom_CylindricalSurface(const gp_Ax3& A3, const double Radius);

  //! Creates a CylindricalSurface from a non transient gp_Cylinder.
  Standard_EXPORT Geom_CylindricalSurface(const gp_Cylinder& C);

  //! Set <me> so that <me> has the same geometric properties as C.
  Standard_EXPORT void SetCylinder(const gp_Cylinder& C);

  //! Changes the radius of the cylinder.
  //! Raised if R < 0.0
  Standard_EXPORT void SetRadius(const double R);

  //! returns a non transient cylinder with the same geometric properties as <me>.
  Standard_EXPORT gp_Cylinder Cylinder() const;

  //! Return the parameter on the Ureversed surface for
  //! the point of parameter U on <me>.
  //! Return 2.PI - U.
  Standard_EXPORT double UReversedParameter(const double U) const final;

  //! Return the parameter on the Vreversed surface for
  //! the point of parameter V on <me>.
  //! Return -V
  Standard_EXPORT double VReversedParameter(const double V) const final;

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

  //! The CylindricalSurface is infinite in the V direction so
  //! V1 = Realfirst, V2 = RealLast from package Standard.
  //! U1 = 0 and U2 = 2*PI.
  Standard_EXPORT void Bounds(double& U1, double& U2, double& V1, double& V2) const final;

  //! Returns the coefficients of the implicit equation of the quadric
  //! in the absolute cartesian coordinate system :
  //! These coefficients are normalized.
  //! @code
  //! A1.X**2 + A2.Y**2 + A3.Z**2 + 2.(B1.X.Y + B2.X.Z + B3.Y.Z) + 2.(C1.X + C2.Y + C3.Z) + D = 0.0
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

  //! Returns the radius of this cylinder.
  Standard_EXPORT double Radius() const;

  //! Returns True.
  Standard_EXPORT bool IsUClosed() const final;

  //! Returns False.
  Standard_EXPORT bool IsVClosed() const final;

  //! Returns True.
  Standard_EXPORT bool IsUPeriodic() const final;

  //! Returns False.
  Standard_EXPORT bool IsVPeriodic() const final;

  //! The UIso curve is a Line. The location point of this line is
  //! on the placement plane (XAxis, YAxis) of the surface.
  //! This line is parallel to the axis of symmetry of the surface.
  Standard_EXPORT occ::handle<Geom_Curve> UIso(const double U) const final;

  //! The VIso curve is a circle. The start point of this circle
  //! (U = 0) is defined with the "XAxis" of the surface.
  //! The center of the circle is on the symmetry axis.
  Standard_EXPORT occ::handle<Geom_Curve> VIso(const double V) const final;

  //! Computes the point P (U, V) on the surface.
  //! P (U, V) = Loc + Radius * (cos (U) * XDir + sin (U) * YDir) +
  //! V * ZDir
  //! where Loc is the origin of the placement plane (XAxis, YAxis)
  //! XDir is the direction of the XAxis and YDir the direction of
  //! the YAxis.
  Standard_EXPORT std::optional<gp_Pnt> EvalD0(const double U, const double V) const final;

  //! Computes the current point and the first derivatives in the
  //! directions U and V.
  Standard_EXPORT std::optional<Geom_Surface::ResD1> EvalD1(const double U, const double V) const final;

  //! Computes the current point, the first and the second derivatives
  //! in the directions U and V.
  Standard_EXPORT std::optional<Geom_Surface::ResD2> EvalD2(const double U, const double V) const final;

  //! Computes the current point, the first, the second and the
  //! third derivatives in the directions U and V.
  Standard_EXPORT std::optional<Geom_Surface::ResD3> EvalD3(const double U, const double V) const final;

  //! Computes the derivative of order Nu in the direction u and Nv
  //! in the direction v.
  //! Raised if Nu + Nv < 1 or Nu < 0 or Nv < 0.
  Standard_EXPORT std::optional<gp_Vec> EvalDN(const double U,
                                               const double V,
                                               const int    Nu,
                                               const int    Nv) const final;

  //! Applies the transformation T to this cylinder.
  Standard_EXPORT void Transform(const gp_Trsf& T) final;

  //! Creates a new object which is a copy of this cylinder.
  Standard_EXPORT occ::handle<Geom_Geometry> Copy() const final;

  //! Dumps the content of me into the stream
  Standard_EXPORT void DumpJson(Standard_OStream& theOStream, int theDepth = -1) const final;

  DEFINE_STANDARD_RTTIEXT(Geom_CylindricalSurface, Geom_ElementarySurface)

private:
  double radius;
};

#endif // _Geom_CylindricalSurface_HeaderFile
