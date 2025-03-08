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

#ifndef _Geom_SphericalSurface_HeaderFile
#define _Geom_SphericalSurface_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Geom_ElementarySurface.hxx>
#include <Standard_Integer.hxx>
class gp_Ax3;
class gp_Sphere;
class Geom_Curve;
class gp_Pnt;
class gp_Vec;
class gp_Trsf;
class Geom_Geometry;

class Geom_SphericalSurface;
DEFINE_STANDARD_HANDLE(Geom_SphericalSurface, Geom_ElementarySurface)

//! Describes a sphere.
//! A sphere is defined by its radius, and is positioned in
//! space by a coordinate system (a gp_Ax3 object), the
//! origin of which is the center of the sphere.
//! This coordinate system is the "local coordinate
//! system" of the sphere. The following apply:
//! - Rotation around its "main Axis", in the trigonometric
//! sense given by the "X Direction" and the "Y
//! Direction", defines the u parametric direction.
//! - Its "X Axis" gives the origin for the u parameter.
//! - The "reference meridian" of the sphere is a
//! half-circle, of radius equal to the radius of the
//! sphere. It is located in the plane defined by the
//! origin, "X Direction" and "main Direction", centered
//! on the origin, and positioned on the positive side of the "X Axis".
//! - Rotation around the "Y Axis" gives the v parameter
//! on the reference meridian.
//! - The "X Axis" gives the origin of the v parameter on
//! the reference meridian.
//! - The v parametric direction is oriented by the "main
//! Direction", i.e. when v increases, the Z coordinate
//! increases. (This implies that the "Y Direction"
//! orients the reference meridian only when the local
//! coordinate system is indirect.)
//! - The u isoparametric curve is a half-circle obtained
//! by rotating the reference meridian of the sphere
//! through an angle u around the "main Axis", in the
//! trigonometric sense defined by the "X Direction"
//! and the "Y Direction".
//! The parametric equation of the sphere is:
//! P(u,v) = O + R*cos(v)*(cos(u)*XDir + sin(u)*YDir)+R*sin(v)*ZDir
//! where:
//! - O, XDir, YDir and ZDir are respectively the
//! origin, the "X Direction", the "Y Direction" and the "Z
//! Direction" of its local coordinate system, and
//! - R is the radius of the sphere.
//! The parametric range of the two parameters is:
//! - [ 0, 2.*Pi ] for u, and
//! - [ - Pi/2., + Pi/2. ] for v.
class Geom_SphericalSurface : public Geom_ElementarySurface
{

public:
  //! A3 is the local coordinate system of the surface.
  //! At the creation the parametrization of the surface is defined
  //! such as the normal Vector (N = D1U ^ D1V) is directed away from
  //! the center of the sphere.
  //! The direction of increasing parametric value V is defined by the
  //! rotation around the "YDirection" of A2 in the trigonometric sense
  //! and the orientation of increasing parametric value U is defined
  //! by the rotation around the main direction of A2 in the
  //! trigonometric sense.
  //! Warnings :
  //! It is not forbidden to create a spherical surface with
  //! Radius = 0.0
  //! Raised if Radius < 0.0.
  Standard_EXPORT Geom_SphericalSurface(const gp_Ax3& A3, const Standard_Real Radius);

  //! Creates a SphericalSurface from a non persistent Sphere from
  //! package gp.
  Standard_EXPORT Geom_SphericalSurface(const gp_Sphere& S);

  //! Assigns the value R to the radius of this sphere.
  //! Exceptions Standard_ConstructionError if R is less than 0.0.
  Standard_EXPORT void SetRadius(const Standard_Real R);

  //! Converts the gp_Sphere S into this sphere.
  Standard_EXPORT void SetSphere(const gp_Sphere& S);

  //! Returns a non persistent sphere with the same geometric
  //! properties as <me>.
  Standard_EXPORT gp_Sphere Sphere() const;

  //! Computes the u parameter on the modified
  //! surface, when reversing its u  parametric
  //! direction, for any point of u parameter U on this sphere.
  //! In the case of a sphere, these functions returns 2.PI - U.
  Standard_EXPORT Standard_Real UReversedParameter(const Standard_Real U) const Standard_OVERRIDE;

  //! Computes the v parameter on the modified
  //! surface, when reversing its v parametric
  //! direction, for any point of v parameter V on this sphere.
  //! In the case of a sphere, these functions returns   -U.
  Standard_EXPORT Standard_Real VReversedParameter(const Standard_Real V) const Standard_OVERRIDE;

  //! Computes the aera of the spherical surface.
  Standard_EXPORT Standard_Real Area() const;

  //! Returns the parametric bounds U1, U2, V1 and V2 of this sphere.
  //! For a sphere: U1 = 0, U2 = 2*PI, V1 = -PI/2, V2 = PI/2.
  Standard_EXPORT void Bounds(Standard_Real& U1,
                              Standard_Real& U2,
                              Standard_Real& V1,
                              Standard_Real& V2) const Standard_OVERRIDE;

  //! Returns the coefficients of the implicit equation of the
  //! quadric in the absolute cartesian coordinates system :
  //! These coefficients are normalized.
  //! A1.X**2 + A2.Y**2 + A3.Z**2 + 2.(B1.X.Y + B2.X.Z + B3.Y.Z) +
  //! 2.(C1.X + C2.Y + C3.Z) + D = 0.0
  Standard_EXPORT void Coefficients(Standard_Real& A1,
                                    Standard_Real& A2,
                                    Standard_Real& A3,
                                    Standard_Real& B1,
                                    Standard_Real& B2,
                                    Standard_Real& B3,
                                    Standard_Real& C1,
                                    Standard_Real& C2,
                                    Standard_Real& C3,
                                    Standard_Real& D) const;

  //! Computes the coefficients of the implicit equation of
  //! this quadric in the absolute Cartesian coordinate system:
  //! A1.X**2 + A2.Y**2 + A3.Z**2 + 2.(B1.X.Y + B2.X.Z + B3.Y.Z) +
  //! 2.(C1.X + C2.Y + C3.Z) + D = 0.0
  //! An implicit normalization is applied (i.e. A1 = A2 = 1.
  //! in the local coordinate system of this sphere).
  Standard_EXPORT Standard_Real Radius() const;

  //! Computes the volume of the spherical surface.
  Standard_EXPORT Standard_Real Volume() const;

  //! Returns True.
  Standard_EXPORT Standard_Boolean IsUClosed() const Standard_OVERRIDE;

  //! Returns False.
  Standard_EXPORT Standard_Boolean IsVClosed() const Standard_OVERRIDE;

  //! Returns True.
  Standard_EXPORT Standard_Boolean IsUPeriodic() const Standard_OVERRIDE;

  //! Returns False.
  Standard_EXPORT Standard_Boolean IsVPeriodic() const Standard_OVERRIDE;

  //! Computes the U isoparametric curve.
  //! The U isoparametric curves of the surface are defined by the
  //! section of the spherical surface with plane obtained by rotation
  //! of the plane (Location, XAxis, ZAxis) around ZAxis. This plane
  //! defines the origin of parametrization u.
  //! For a SphericalSurface the UIso curve is a Circle.
  //! Warnings : The radius of this circle can be zero.
  Standard_EXPORT Handle(Geom_Curve) UIso(const Standard_Real U) const Standard_OVERRIDE;

  //! Computes the V isoparametric curve.
  //! The V isoparametric curves of the surface  are defined by
  //! the section of the spherical surface with plane parallel to the
  //! plane (Location, XAxis, YAxis). This plane defines the origin of
  //! parametrization V.
  //! Be careful if  V is close to PI/2 or 3*PI/2 the radius of the
  //! circle becomes tiny. It is not forbidden in this toolkit to
  //! create circle with radius = 0.0
  //! For a SphericalSurface the VIso curve is a Circle.
  //! Warnings : The radius of this circle can be zero.
  Standard_EXPORT Handle(Geom_Curve) VIso(const Standard_Real V) const Standard_OVERRIDE;

  //! Computes the  point P (U, V) on the surface.
  //! P (U, V) = Loc + Radius * Sin (V) * Zdir +
  //! Radius * Cos (V) * (cos (U) * XDir + sin (U) * YDir)
  //! where Loc is the origin of the placement plane (XAxis, YAxis)
  //! XDir is the direction of the XAxis and YDir the direction of
  //! the YAxis and ZDir the direction of the ZAxis.
  Standard_EXPORT void D0(const Standard_Real U,
                          const Standard_Real V,
                          gp_Pnt&             P) const Standard_OVERRIDE;

  //! Computes the current point and the first derivatives in the
  //! directions U and V.
  Standard_EXPORT void D1(const Standard_Real U,
                          const Standard_Real V,
                          gp_Pnt&             P,
                          gp_Vec&             D1U,
                          gp_Vec&             D1V) const Standard_OVERRIDE;

  //! Computes the current point, the first and the second derivatives
  //! in the directions U and V.
  Standard_EXPORT void D2(const Standard_Real U,
                          const Standard_Real V,
                          gp_Pnt&             P,
                          gp_Vec&             D1U,
                          gp_Vec&             D1V,
                          gp_Vec&             D2U,
                          gp_Vec&             D2V,
                          gp_Vec&             D2UV) const Standard_OVERRIDE;

  //! Computes the current point, the first,the second and the third
  //! derivatives in the directions U and V.
  Standard_EXPORT void D3(const Standard_Real U,
                          const Standard_Real V,
                          gp_Pnt&             P,
                          gp_Vec&             D1U,
                          gp_Vec&             D1V,
                          gp_Vec&             D2U,
                          gp_Vec&             D2V,
                          gp_Vec&             D2UV,
                          gp_Vec&             D3U,
                          gp_Vec&             D3V,
                          gp_Vec&             D3UUV,
                          gp_Vec&             D3UVV) const Standard_OVERRIDE;

  //! Computes the derivative of order Nu in the direction u
  //! and Nv in the direction v.
  //! Raised if Nu + Nv < 1 or Nu < 0 or Nv < 0.
  Standard_EXPORT gp_Vec DN(const Standard_Real    U,
                            const Standard_Real    V,
                            const Standard_Integer Nu,
                            const Standard_Integer Nv) const Standard_OVERRIDE;

  //! Applies the transformation T to this sphere.
  Standard_EXPORT void Transform(const gp_Trsf& T) Standard_OVERRIDE;

  //! Creates a new object which is a copy of this sphere.
  Standard_EXPORT Handle(Geom_Geometry) Copy() const Standard_OVERRIDE;

  //! Dumps the content of me into the stream
  Standard_EXPORT virtual void DumpJson(Standard_OStream& theOStream,
                                        Standard_Integer  theDepth = -1) const Standard_OVERRIDE;

  DEFINE_STANDARD_RTTIEXT(Geom_SphericalSurface, Geom_ElementarySurface)

protected:
private:
  Standard_Real radius;
};

#endif // _Geom_SphericalSurface_HeaderFile
