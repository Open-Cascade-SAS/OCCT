// Created on: 1993-03-24
// Created by: Philippe DAUTRY
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

#ifndef _Geom2d_Circle_HeaderFile
#define _Geom2d_Circle_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Geom2d_Conic.hxx>
class gp_Circ2d;
class gp_Ax2d;
class gp_Ax22d;
class gp_Trsf2d;
class Geom2d_Geometry;

//! Describes a circle in the plane (2D space).
//! A circle is defined by its radius and, as with any conic
//! curve, is positioned in the plane with a coordinate
//! system (gp_Ax22d object) where the origin is the
//! center of the circle.
//! The coordinate system is the local coordinate
//! system of the circle.
//! The orientation (direct or indirect) of the local
//! coordinate system gives an explicit orientation to the
//! circle, determining the direction in which the
//! parameter increases along the circle.
//! The Geom2d_Circle circle is parameterized by an angle:
//! P(U) = O + R*std::cos(U)*XDir + R*Sin(U)*YDir
//! where:
//! - P is the point of parameter U,
//! - O, XDir and YDir are respectively the origin, "X
//! Direction" and "Y Direction" of its local coordinate system,
//! - R is the radius of the circle.
//! The "X Axis" of the local coordinate system therefore
//! defines the origin of the parameter of the circle. The
//! parameter is the angle with this "X Direction".
//! A circle is a closed and periodic curve. The period is
//! 2.*Pi and the parameter range is [ 0,2.*Pi [.
//! See Also
//! GCE2d_MakeCircle which provides functions for
//! more complex circle constructions
//! gp_Ax22d and gp_Circ2d for an equivalent, non-parameterized data structure.
class Geom2d_Circle : public Geom2d_Conic
{

public:
  //! Constructs a circle by conversion of the gp_Circ2d circle C.
  Standard_EXPORT Geom2d_Circle(const gp_Circ2d& C);

  //! Constructs a circle of radius Radius, whose center is the origin of axis
  //! A; A is the "X Axis" of the local coordinate system
  //! of the circle; this coordinate system is direct if
  //! Sense is true (default value) or indirect if Sense is false.
  //! Note: It is possible to create a circle where Radius is equal to 0.0.
  //! Exceptions Standard_ConstructionError if Radius is negative.
  Standard_EXPORT Geom2d_Circle(const gp_Ax2d& A, const double Radius, const bool Sense = true);

  //! Constructs a circle
  //! of radius Radius, where the coordinate system A
  //! locates the circle and defines its orientation in the plane such that:
  //! - the center of the circle is the origin of A,
  //! - the orientation (direct or indirect) of A gives the
  //! orientation of the circle.
  Standard_EXPORT Geom2d_Circle(const gp_Ax22d& A, const double Radius);

  //! Converts the gp_Circ2d circle C into this circle.
  Standard_EXPORT void SetCirc2d(const gp_Circ2d& C);

  Standard_EXPORT void SetRadius(const double R);

  //! Returns the non persistent circle from gp with the same
  //! geometric properties as <me>.
  Standard_EXPORT gp_Circ2d Circ2d() const;

  //! Returns the radius of this circle.
  Standard_EXPORT double Radius() const;

  //! Computes the parameter on the reversed circle for
  //! the point of parameter U on this circle.
  //! For a circle, the returned value is: 2.*Pi - U.
  Standard_EXPORT double ReversedParameter(const double U) const final;

  //! Returns 0., which is the eccentricity of any circle.
  Standard_EXPORT double Eccentricity() const final;

  //! Returns 0.0
  Standard_EXPORT double FirstParameter() const final;

  //! Returns 2*PI.
  Standard_EXPORT double LastParameter() const final;

  //! returns True.
  Standard_EXPORT bool IsClosed() const final;

  //! returns True. The period of a circle is 2.*Pi.
  Standard_EXPORT bool IsPeriodic() const final;

  //! Returns in P the point of parameter U.
  //! P = C + R * Cos (U) * XDir + R * Sin (U) * YDir
  //! where C is the center of the circle , XDir the XDirection and
  //! YDir the YDirection of the circle's local coordinate system.
  Standard_EXPORT std::optional<gp_Pnt2d> EvalD0(const double U) const final;

  //! Returns the point P of parameter U and the first derivative V1.
  Standard_EXPORT std::optional<Geom2d_Curve::ResD1> EvalD1(const double U) const final;

  //! Returns the point P of parameter U, the first and second
  //! derivatives V1 and V2.
  Standard_EXPORT std::optional<Geom2d_Curve::ResD2> EvalD2(const double U) const final;

  //! Returns the point P of parameter u, the first second and third
  //! derivatives V1 V2 and V3.
  Standard_EXPORT std::optional<Geom2d_Curve::ResD3> EvalD3(const double U) const final;

  //! For the point of parameter U of this circle, computes
  //! the vector corresponding to the Nth derivative.
  //! Exceptions: Standard_RangeError if N is less than 1.
  Standard_EXPORT std::optional<gp_Vec2d> EvalDN(const double U, const int N) const final;

  //! Applies the transformation T to this circle.
  Standard_EXPORT void Transform(const gp_Trsf2d& T) final;

  //! Creates a new object which is a copy of this circle.
  Standard_EXPORT occ::handle<Geom2d_Geometry> Copy() const final;

  //! Dumps the content of me into the stream
  Standard_EXPORT void DumpJson(Standard_OStream& theOStream, int theDepth = -1) const final;

  DEFINE_STANDARD_RTTIEXT(Geom2d_Circle, Geom2d_Conic)

private:
  double radius;
};

#endif // _Geom2d_Circle_HeaderFile
