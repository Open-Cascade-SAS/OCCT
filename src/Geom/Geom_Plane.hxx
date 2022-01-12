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

#ifndef _Geom_Plane_HeaderFile
#define _Geom_Plane_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Geom_ElementarySurface.hxx>
#include <Standard_Integer.hxx>
class gp_Ax3;
class gp_Pln;
class gp_Pnt;
class gp_Dir;
class gp_Trsf;
class gp_GTrsf2d;
class Geom_Curve;
class gp_Vec;
class Geom_Geometry;


class Geom_Plane;
DEFINE_STANDARD_HANDLE(Geom_Plane, Geom_ElementarySurface)

//! Describes a plane in 3D space.
//! A plane is positioned in space by a coordinate system
//! (a gp_Ax3 object) such that the plane is defined by
//! the origin, "X Direction" and "Y Direction" of this
//! coordinate system.
//! This coordinate system is the "local coordinate
//! system" of the plane. The following apply:
//! - Its "X Direction" and "Y Direction" are respectively
//! the u and v parametric directions of the plane.
//! - Its origin is the origin of the u and v parameters
//! (also called the "origin" of the plane).
//! - Its "main Direction" is a vector normal to the plane.
//! This normal vector gives the orientation of the
//! plane only if the local coordinate system is "direct".
//! (The orientation of the plane is always defined by
//! the "X Direction" and the "Y Direction" of its local
//! coordinate system.)
//! The parametric equation of the plane is:
//! @code
//!   P(u, v) = O + u*XDir + v*YDir
//! @endcode
//! where O, XDir and YDir are respectively the
//! origin, the "X Direction" and the "Y Direction" of the
//! local coordinate system of the plane.
//! The parametric range of the two parameters u and v
//! is ] -infinity, +infinity [.
class Geom_Plane : public Geom_ElementarySurface
{

public:

  //! Creates a plane located in 3D space with an axis placement three axis.
  //! The "ZDirection" of "A3" is the direction normal
  //! to the plane.  The "Location" point of "A3" is the origin of the plane.
  //! The "XDirection" and "YDirection" of "A3" define
  //! the directions of the U isoparametric and V isoparametric curves.
  Standard_EXPORT Geom_Plane(const gp_Ax3& A3);


  //! Creates a plane from a non transient plane from package gp.
  Standard_EXPORT Geom_Plane(const gp_Pln& Pl);

  //! P is the "Location" point or origin of the plane.
  //! V is the direction normal to the plane.
  Standard_EXPORT Geom_Plane(const gp_Pnt& P, const gp_Dir& V);

  //! Creates a plane from its cartesian equation:
  //! @code
  //!   Ax + By + Cz + D = 0.0
  //! @endcode
  //! Raised if Sqrt (A*A + B*B + C*C) <= Resolution from gp
  Standard_EXPORT Geom_Plane(const Standard_Real A, const Standard_Real B, const Standard_Real C, const Standard_Real D);


  //! Set <me> so that <me> has the same geometric properties as Pl.
  Standard_EXPORT void SetPln (const gp_Pln& Pl);

  //! Converts this plane into a gp_Pln plane.
  Standard_EXPORT gp_Pln Pln() const;

  //! Changes the orientation of this plane in the u (or v) parametric direction.
  //! The bounds of the plane are not changed but the given parametric direction is reversed.
  //! Hence the orientation of the surface is reversed.
  Standard_EXPORT virtual void UReverse() Standard_OVERRIDE;

  //! Computes the u  parameter on the modified plane,
  //! produced when reversing the u parametric of this plane,
  //! for any point of u parameter U on this plane.
  //! In the case of a plane, these methods return - -U.
  Standard_EXPORT Standard_Real UReversedParameter (const Standard_Real U) const Standard_OVERRIDE;


  //! Changes the orientation of this plane in the u (or v) parametric direction.
  //! The bounds of the plane are not changed but the given parametric direction is reversed.
  //! Hence the orientation of the surface is reversed.
  Standard_EXPORT virtual void VReverse() Standard_OVERRIDE;

  //! Computes the v parameter on the modified plane,
  //! produced when reversing the v parametric of this plane,
  //! for any point of v parameter V on this plane.
  //! In the case of a plane, these methods return -V.
  Standard_EXPORT Standard_Real VReversedParameter (const Standard_Real V) const Standard_OVERRIDE;

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
  //! This method multiplies U and V by T.ScaleFactor()
  Standard_EXPORT virtual void TransformParameters (Standard_Real& U, Standard_Real& V, const gp_Trsf& T) const Standard_OVERRIDE;

  //! Returns a 2d transformation used to find the new
  //! parameters of a point on the transformed surface.
  //! @code
  //!   me->Transformed(T)->Value(U',V')
  //! @endcode
  //! is the same point as
  //! @code
  //!   me->Value(U,V).Transformed(T)
  //! @endcode
  //! Where U',V' are  obtained by transforming U,V with the 2d transformation returned by
  //! @code
  //!   me->ParametricTransformation(T)
  //! @endcode
  //! This method returns a scale centered on the origin with T.ScaleFactor
  Standard_EXPORT virtual gp_GTrsf2d ParametricTransformation (const gp_Trsf& T) const Standard_OVERRIDE;

  //! Returns the parametric bounds U1, U2, V1 and V2 of this plane.
  //! Because a plane is an infinite surface, the following is always true:
  //! - U1 = V1 =   Standard_Real::RealFirst()
  //! - U2 = V2 =   Standard_Real::RealLast().
  Standard_EXPORT void Bounds (Standard_Real& U1, Standard_Real& U2, Standard_Real& V1, Standard_Real& V2) const Standard_OVERRIDE;

  //! Computes the normalized coefficients of the plane's cartesian equation:
  //! @code
  //!   Ax + By + Cz + D = 0.0
  //! @endcode
  Standard_EXPORT void Coefficients (Standard_Real& A, Standard_Real& B, Standard_Real& C, Standard_Real& D) const;

  //! return False
  Standard_EXPORT Standard_Boolean IsUClosed() const Standard_OVERRIDE;

  //! return False
  Standard_EXPORT Standard_Boolean IsVClosed() const Standard_OVERRIDE;

  //! return False.
  Standard_EXPORT Standard_Boolean IsUPeriodic() const Standard_OVERRIDE;

  //! return False.
  Standard_EXPORT Standard_Boolean IsVPeriodic() const Standard_OVERRIDE;

  //! Computes the U isoparametric curve.
  //! This is a Line parallel to the YAxis of the plane.
  Standard_EXPORT Handle(Geom_Curve) UIso (const Standard_Real U) const Standard_OVERRIDE;

  //! Computes the V isoparametric curve.
  //! This is a Line parallel to the XAxis of the plane.
  Standard_EXPORT Handle(Geom_Curve) VIso (const Standard_Real V) const Standard_OVERRIDE;

  //! Computes the point P (U, V) on <me>.
  //! @code
  //!   P = O + U * XDir + V * YDir.
  //! @endcode
  //! where O is the "Location" point of the plane, XDir the
  //! "XDirection" and YDir the "YDirection" of the plane's local coordinate system.
  Standard_EXPORT void D0 (const Standard_Real U, const Standard_Real V, gp_Pnt& P) const Standard_OVERRIDE;

  //! Computes the current point and the first derivatives in the directions U and V.
  Standard_EXPORT void D1 (const Standard_Real U, const Standard_Real V, gp_Pnt& P, gp_Vec& D1U, gp_Vec& D1V) const Standard_OVERRIDE;

  //! Computes the current point, the first and the second
  //! derivatives in the directions U and V.
  Standard_EXPORT void D2 (const Standard_Real U, const Standard_Real V, gp_Pnt& P, gp_Vec& D1U, gp_Vec& D1V, gp_Vec& D2U, gp_Vec& D2V, gp_Vec& D2UV) const Standard_OVERRIDE;

  //! Computes the current point, the first,the second and the
  //! third derivatives in the directions U and V.
  Standard_EXPORT void D3 (const Standard_Real U, const Standard_Real V, gp_Pnt& P, gp_Vec& D1U, gp_Vec& D1V, gp_Vec& D2U, gp_Vec& D2V, gp_Vec& D2UV, gp_Vec& D3U, gp_Vec& D3V, gp_Vec& D3UUV, gp_Vec& D3UVV) const Standard_OVERRIDE;

  //! Computes the derivative of order Nu in the direction u
  //! and Nv in the direction v.
  //! Raised if Nu + Nv < 1 or Nu < 0 or Nv < 0.
  Standard_EXPORT gp_Vec DN (const Standard_Real U, const Standard_Real V, const Standard_Integer Nu, const Standard_Integer Nv) const Standard_OVERRIDE;

  //! Applies the transformation T to this plane.
  Standard_EXPORT void Transform (const gp_Trsf& T) Standard_OVERRIDE;

  //! Creates a new object which is a copy of this plane.
  Standard_EXPORT Handle(Geom_Geometry) Copy() const Standard_OVERRIDE;

  //! Dumps the content of me into the stream
  Standard_EXPORT virtual void DumpJson (Standard_OStream& theOStream, Standard_Integer theDepth = -1) const Standard_OVERRIDE;


  DEFINE_STANDARD_RTTIEXT(Geom_Plane,Geom_ElementarySurface)

};

#endif // _Geom_Plane_HeaderFile
