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

#ifndef _Geom_ToroidalSurface_HeaderFile
#define _Geom_ToroidalSurface_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Geom_ElementarySurface.hxx>
#include <TColStd_Array1OfReal.hxx>
#include <Standard_Integer.hxx>
class gp_Ax3;
class gp_Torus;
class Geom_Curve;
class gp_Pnt;
class gp_Vec;
class gp_Trsf;
class Geom_Geometry;

class Geom_ToroidalSurface;
DEFINE_STANDARD_HANDLE(Geom_ToroidalSurface, Geom_ElementarySurface)

//! Describes a torus.
//! A torus is defined by its major and minor radii, and
//! positioned in space with a coordinate system (a
//! gp_Ax3 object) as follows:
//! - The origin is the center of the torus.
//! - The surface is obtained by rotating a circle around
//! the "main Direction". This circle has a radius equal
//! to the minor radius, and is located in the plane
//! defined by the origin, "X Direction" and "main
//! Direction". It is centered on the "X Axis", on its
//! positive side, and positioned at a distance from the
//! origin equal to the major radius. This circle is the
//! "reference circle" of the torus.
//! - The plane defined by the origin, the "X Direction"
//! and the "Y Direction" is called the "reference plane" of the torus.
//! This coordinate system is the "local coordinate
//! system" of the torus. The following apply:
//! - Rotation around its "main Axis", in the trigonometric
//! sense given by "X Direction" and "Y Direction",
//! defines the u parametric direction.
//! - The "X Axis" gives the origin for the u parameter.
//! - Rotation around an axis parallel to the "Y Axis" and
//! passing through the center of the "reference circle"
//! gives the v parameter on the "reference circle".
//! - The "X Axis" gives the origin of the v parameter on
//! the "reference circle".
//! - The v parametric direction is oriented by the
//! inverse of the "main Direction", i.e. near 0, as v
//! increases, the Z coordinate decreases. (This
//! implies that the "Y Direction" orients the reference
//! circle only when the local coordinate system is direct.)
//! - The u isoparametric curve is a circle obtained by
//! rotating the "reference circle" of the torus through
//! an angle u about the "main Axis".
//! The parametric equation of the torus is :
//! P(u, v) = O + (R + r*cos(v)) * (cos(u)*XDir +
//! sin(u)*YDir ) + r*sin(v)*ZDir, where:
//! - O, XDir, YDir and ZDir are respectively the
//! origin, the "X Direction", the "Y Direction" and the "Z
//! Direction" of the local coordinate system,
//! - r and R are, respectively, the minor and major radius.
//! The parametric range of the two parameters is:
//! - [ 0, 2.*Pi ] for u
//! - [ 0, 2.*Pi ] for v
class Geom_ToroidalSurface : public Geom_ElementarySurface
{

public:
  //! A3 is the local coordinate system of the surface.
  //! The orientation of increasing V parametric value is defined
  //! by the rotation around the main axis (ZAxis) in the
  //! trigonometric sense. The parametrization of the surface in the
  //! U direction is defined such as the normal Vector (N = D1U ^ D1V)
  //! is oriented towards the "outside region" of the surface.
  //! Warnings :
  //! It is not forbidden to create a toroidal surface with
  //! MajorRadius = MinorRadius = 0.0
  //!
  //! Raised if MinorRadius < 0.0 or if MajorRadius < 0.0
  Standard_EXPORT Geom_ToroidalSurface(const gp_Ax3&       A3,
                                       const Standard_Real MajorRadius,
                                       const Standard_Real MinorRadius);

  //! Creates a ToroidalSurface from a non transient Torus from
  //! package gp.
  Standard_EXPORT Geom_ToroidalSurface(const gp_Torus& T);

  //! Modifies this torus by changing its major radius.
  //! Exceptions
  //! Standard_ConstructionError if:
  //! - MajorRadius is negative, or
  //! - MajorRadius - r is less than or equal to
  //! gp::Resolution(), where r is the minor radius of this torus.
  Standard_EXPORT void SetMajorRadius(const Standard_Real MajorRadius);

  //! Modifies this torus by changing its minor radius.
  //! Exceptions
  //! Standard_ConstructionError if:
  //! - MinorRadius is negative, or
  //! - R - MinorRadius is less than or equal to
  //! gp::Resolution(), where R is the major radius of this torus.
  Standard_EXPORT void SetMinorRadius(const Standard_Real MinorRadius);

  //! Converts the gp_Torus torus T into this torus.
  Standard_EXPORT void SetTorus(const gp_Torus& T);

  //! Returns the non transient torus with the same geometric
  //! properties as <me>.
  Standard_EXPORT gp_Torus Torus() const;

  //! Return the  parameter on the  Ureversed surface for
  //! the point of parameter U on <me>.
  //! Return 2.PI - U.
  Standard_EXPORT Standard_Real UReversedParameter(const Standard_Real U) const Standard_OVERRIDE;

  //! Return the  parameter on the  Ureversed surface for
  //! the point of parameter U on <me>.
  //! Return 2.PI - U.
  Standard_EXPORT Standard_Real VReversedParameter(const Standard_Real U) const Standard_OVERRIDE;

  //! Computes the aera of the surface.
  Standard_EXPORT Standard_Real Area() const;

  //! Returns the parametric bounds U1, U2, V1 and V2 of this torus.
  //! For a torus: U1 = V1 = 0 and U2 = V2 = 2*PI .
  Standard_EXPORT void Bounds(Standard_Real& U1,
                              Standard_Real& U2,
                              Standard_Real& V1,
                              Standard_Real& V2) const Standard_OVERRIDE;

  //! Returns the coefficients of the implicit equation of the surface
  //! in the absolute cartesian coordinate system :
  //! Coef(1) * X**4 + Coef(2) * Y**4 + Coef(3) * Z**4 +
  //! Coef(4) * X**3 * Y + Coef(5) * X**3 * Z + Coef(6) * Y**3 * X +
  //! Coef(7) * Y**3 * Z + Coef(8) * Z**3 * X + Coef(9) * Z**3 * Y +
  //! Coef(10) * X**2 * Y**2 + Coef(11) * X**2 * Z**2 +
  //! Coef(12) * Y**2 * Z**2 + Coef(13) * X**3 + Coef(14) * Y**3 +
  //! Coef(15) * Z**3 + Coef(16) * X**2 * Y + Coef(17) * X**2 * Z +
  //! Coef(18) * Y**2 * X + Coef(19) * Y**2 * Z + Coef(20) * Z**2 * X +
  //! Coef(21) * Z**2 * Y + Coef(22) * X**2 + Coef(23) * Y**2 +
  //! Coef(24) * Z**2 + Coef(25) * X * Y + Coef(26) * X * Z +
  //! Coef(27) * Y * Z + Coef(28) * X + Coef(29) * Y + Coef(30) *  Z +
  //! Coef(31) = 0.0
  //! Raised if the length of Coef is lower than 31.
  Standard_EXPORT void Coefficients(TColStd_Array1OfReal& Coef) const;

  //! Returns the major radius, or the minor radius, of this torus.
  Standard_EXPORT Standard_Real MajorRadius() const;

  //! Returns the major radius, or the minor radius, of this torus.
  Standard_EXPORT Standard_Real MinorRadius() const;

  //! Computes the volume.
  Standard_EXPORT Standard_Real Volume() const;

  //! Returns True.
  Standard_EXPORT Standard_Boolean IsUClosed() const Standard_OVERRIDE;

  //! Returns True.
  Standard_EXPORT Standard_Boolean IsVClosed() const Standard_OVERRIDE;

  //! Returns True.
  Standard_EXPORT Standard_Boolean IsUPeriodic() const Standard_OVERRIDE;

  //! Returns True.
  Standard_EXPORT Standard_Boolean IsVPeriodic() const Standard_OVERRIDE;

  //! Computes the U isoparametric curve.
  //!
  //! For a toroidal surface the UIso curve is a circle.
  //! The center of the Uiso circle is at the distance MajorRadius
  //! from the location point of the toroidal surface.
  //! Warnings :
  //! The radius of the circle can be zero if for the surface
  //! MinorRadius = 0.0
  Standard_EXPORT Handle(Geom_Curve) UIso(const Standard_Real U) const Standard_OVERRIDE;

  //! Computes the V isoparametric curve.
  //!
  //! For a ToroidalSurface the VIso curve is a circle.
  //! The axis of the circle is the main axis (ZAxis) of the
  //! toroidal  surface.
  //! Warnings :
  //! The radius of the circle can be zero if for the surface
  //! MajorRadius = MinorRadius
  Standard_EXPORT Handle(Geom_Curve) VIso(const Standard_Real V) const Standard_OVERRIDE;

  //! Computes the  point P (U, V) on the surface.
  //! P (U, V) = Loc + MinorRadius * Sin (V) * Zdir +
  //! (MajorRadius + MinorRadius * Cos(V)) *
  //! (cos (U) * XDir + sin (U) * YDir)
  //! where Loc is the origin of the placement plane (XAxis, YAxis)
  //! XDir is the direction of the XAxis and YDir the direction of
  //! the YAxis and ZDir the direction of the ZAxis.
  Standard_EXPORT void D0(const Standard_Real U,
                          const Standard_Real V,
                          gp_Pnt&             P) const Standard_OVERRIDE;

  //! Computes the current point and the first derivatives in
  //! the directions U and V.
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

  //! Computes the current point, the first,the second and the
  //! third derivatives in the directions U and V.
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

  //! Computes the derivative of order Nu in the direction u and
  //! Nv in the direction v.
  //! Raised if Nu + Nv < 1 or Nu < 0 or Nv < 0.
  Standard_EXPORT gp_Vec DN(const Standard_Real    U,
                            const Standard_Real    V,
                            const Standard_Integer Nu,
                            const Standard_Integer Nv) const Standard_OVERRIDE;

  //! Applies the transformation T to this torus.
  Standard_EXPORT void Transform(const gp_Trsf& T) Standard_OVERRIDE;

  //! Creates a new object which is a copy of this torus.
  Standard_EXPORT Handle(Geom_Geometry) Copy() const Standard_OVERRIDE;

  //! Dumps the content of me into the stream
  Standard_EXPORT virtual void DumpJson(Standard_OStream& theOStream,
                                        Standard_Integer  theDepth = -1) const Standard_OVERRIDE;

  DEFINE_STANDARD_RTTIEXT(Geom_ToroidalSurface, Geom_ElementarySurface)

protected:
private:
  Standard_Real majorRadius;
  Standard_Real minorRadius;
};

#endif // _Geom_ToroidalSurface_HeaderFile
