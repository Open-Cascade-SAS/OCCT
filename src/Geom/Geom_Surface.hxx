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

#ifndef _Geom_Surface_HeaderFile
#define _Geom_Surface_HeaderFile

#include <Geom_Curve.hxx>

class gp_Trsf;
class gp_GTrsf2d;
class gp_Pnt;
class gp_Vec;

class Geom_Surface;
DEFINE_STANDARD_HANDLE(Geom_Surface, Geom_Geometry)

//! Describes the common behavior of surfaces in 3D space.
//! The Geom package provides many implementations of concrete derived surfaces,
//! such as planes, cylinders, cones, spheres and tori, surfaces of linear extrusion,
//! surfaces of revolution, Bezier and BSpline surfaces, and so on.
//! The key characteristic of these surfaces is that they are parameterized.
//! Geom_Surface demonstrates:
//! - how to work with the parametric equation of a surface
//!   to compute the point of parameters (u, v), and, at this point, the 1st, 2nd ... Nth derivative;
//! - how to find global information about a surface in
//!   each parametric direction (for example, level of continuity, whether the surface is closed,
//!   its periodicity, the bounds of the parameters and so on);
//! - how the parameters change when geometric transformations are applied to the surface,
//!   or the orientation is modified.
//!
//! Note that all surfaces must have a geometric continuity, and any surface is at least "C0".
//! Generally, continuity is checked at construction time or when the curve is edited.
//! Where this is not the case, the documentation makes this explicit.
//!
//! Warning
//! The Geom package does not prevent the construction of
//! surfaces with null areas, or surfaces which self-intersect.
class Geom_Surface : public Geom_Geometry
{

public:

  //! Reverses the U direction of parametrization of <me>.
  //! The bounds of the surface are not modified.
  Standard_EXPORT virtual void UReverse() = 0;

  //! Reverses the U direction of parametrization of <me>.
  //! The bounds of the surface are not modified.
  //! A copy of <me> is returned.
  Standard_NODISCARD Standard_EXPORT Handle(Geom_Surface) UReversed() const;

  //! Returns the  parameter on the  Ureversed surface for
  //! the point of parameter U on <me>.
  //! @code
  //!   me->UReversed()->Value(me->UReversedParameter(U),V)
  //! @endcode
  //! is the same point as
  //! @code
  //!   me->Value(U,V)
  //! @endcode
  Standard_EXPORT virtual Standard_Real UReversedParameter (const Standard_Real U) const = 0;

  //! Reverses the V direction of parametrization of <me>.
  //! The bounds of the surface are not modified.
  Standard_EXPORT virtual void VReverse() = 0;

  //! Reverses the V direction of parametrization of <me>.
  //! The bounds of the surface are not modified.
  //! A copy of <me> is returned.
  Standard_NODISCARD Standard_EXPORT Handle(Geom_Surface) VReversed() const;

  //! Returns the  parameter on the  Vreversed surface for
  //! the point of parameter V on <me>.
  //! @code
  //!   me->VReversed()->Value(U,me->VReversedParameter(V))
  //! @endcode
  //! is the same point as
  //! @code
  //!   me->Value(U,V)
  //! @endcode
  Standard_EXPORT virtual Standard_Real VReversedParameter (const Standard_Real V) const = 0;

  //! Computes the  parameters on the  transformed  surface for
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
  //! This method does not change <U> and <V>
  //!
  //! It  can be redefined.  For  example on  the Plane,
  //! Cylinder, Cone, Revolved and Extruded surfaces.
  Standard_EXPORT virtual void TransformParameters (Standard_Real& U, Standard_Real& V, const gp_Trsf& T) const;
  
  //! Returns a 2d transformation  used to find the  new
  //! parameters of a point on the transformed surface.
  //! @code
  //!   me->Transformed(T)->Value(U',V')
  //! @endcode
  //! is the same point as
  //! @code
  //!   me->Value(U,V).Transformed(T)
  //! @endcode
  //! Where U',V' are  obtained by transforming U,V with
  //! the 2d transformation returned by
  //! @code
  //!   me->ParametricTransformation(T)
  //! @endcode
  //! This method returns an identity transformation
  //!
  //! It  can be redefined.  For  example on  the Plane,
  //! Cylinder, Cone, Revolved and Extruded surfaces.
  Standard_EXPORT virtual gp_GTrsf2d ParametricTransformation (const gp_Trsf& T) const;

  //! Returns the parametric bounds U1, U2, V1 and V2 of this surface.
  //! If the surface is infinite, this function can return a value
  //! equal to Precision::Infinite: instead of Standard_Real::LastReal.
  Standard_EXPORT virtual void Bounds (Standard_Real& U1, Standard_Real& U2, Standard_Real& V1, Standard_Real& V2) const = 0;

  //! Checks whether this surface is closed in the u parametric direction.
  //! Returns true if, in the u parametric direction:
  //! taking uFirst and uLast as the parametric bounds in
  //! the u parametric direction, for each parameter v,
  //! the distance between the points P(uFirst, v) and
  //! P(uLast, v) is less than or equal to gp::Resolution().
  Standard_EXPORT virtual Standard_Boolean IsUClosed() const = 0;

  //! Checks whether this surface is closed in the u parametric direction.
  //! Returns true if, in the v parametric direction:
  //! taking vFirst and vLast as the parametric bounds in the v parametric direction,
  //! for each parameter u, the distance between the points
  //! P(u, vFirst) and P(u, vLast) is less than or equal to gp::Resolution().
  Standard_EXPORT virtual Standard_Boolean IsVClosed() const = 0;

  //! Checks if this surface is periodic in the u parametric direction.
  //! Returns true if:
  //! - this surface is closed in the u parametric direction, and
  //! - there is a constant T such that the distance
  //!   between the points P (u, v) and P (u + T, v)
  //!   (or the points P (u, v) and P (u, v + T)) is less than or equal to gp::Resolution().
  //!
  //! Note: T is the parametric period in the u parametric direction.
  Standard_EXPORT virtual Standard_Boolean IsUPeriodic() const = 0;

  //! Returns the period of this surface in the u parametric direction.
  //! Raises if the surface is not uperiodic.
  Standard_EXPORT virtual Standard_Real UPeriod() const;

  //! Checks if this surface is periodic in the v parametric direction.
  //! Returns true if:
  //! - this surface is closed in the v parametric direction, and
  //! - there is a constant T such that the distance
  //!   between the points P (u, v) and P (u + T, v)
  //!   (or the points P (u, v) and P (u, v + T)) is less than or equal to gp::Resolution().
  //!
  //! Note: T is the parametric period in the v parametric direction.
  Standard_EXPORT virtual Standard_Boolean IsVPeriodic() const = 0;

  //! Returns the period of this surface in the v parametric direction.
  //! raises if the surface is not vperiodic.
  Standard_EXPORT virtual Standard_Real VPeriod() const;

  //! Computes the U isoparametric curve.
  Standard_EXPORT virtual Handle(Geom_Curve) UIso (const Standard_Real U) const = 0;

  //! Computes the V isoparametric curve.
  Standard_EXPORT virtual Handle(Geom_Curve) VIso (const Standard_Real V) const = 0;

  //! Returns the Global Continuity of the surface in direction U and V :
  //! - C0: only geometric continuity,
  //! - C1: continuity of the first derivative all along the surface,
  //! - C2: continuity of the second derivative all along the surface,
  //! - C3: continuity of the third derivative all along the surface,
  //! - G1: tangency continuity all along the surface,
  //! - G2: curvature continuity all along the surface,
  //! - CN: the order of continuity is infinite.
  //!
  //! Example:
  //! If the surface is C1 in the V parametric direction and C2
  //! in the U parametric direction Shape = C1.
  Standard_EXPORT virtual GeomAbs_Shape Continuity() const = 0;

  //! Returns the order of continuity of the surface in the U parametric direction.
  //! Raised if N < 0.
  Standard_EXPORT virtual Standard_Boolean IsCNu (const Standard_Integer N) const = 0;

  //! Returns the order of continuity of the surface in the V parametric direction.
  //! Raised if N < 0.
  Standard_EXPORT virtual Standard_Boolean IsCNv (const Standard_Integer N) const = 0;

  //! Computes the point of parameter U,V on the surface.
  //!
  //! Raised only for an "OffsetSurface" if it is not possible to
  //! compute the current point.
  Standard_EXPORT virtual void D0 (const Standard_Real U, const Standard_Real V, gp_Pnt& P) const = 0;


  //! Computes the point P and the first derivatives in the directions U and V at this point.
  //! Raised if the continuity of the surface is not C1.
  //!
  //! Tip: use GeomLib::NormEstim() to calculate surface normal at specified (U, V) point.
  Standard_EXPORT virtual void D1 (const Standard_Real U, const Standard_Real V, gp_Pnt& P, gp_Vec& D1U, gp_Vec& D1V) const = 0;

  //! Computes the point P, the first and the second derivatives in
  //! the directions U and V at this point.
  //! Raised if the continuity of the surface is not C2.
  Standard_EXPORT virtual void D2 (const Standard_Real U, const Standard_Real V, gp_Pnt& P, gp_Vec& D1U, gp_Vec& D1V, gp_Vec& D2U, gp_Vec& D2V, gp_Vec& D2UV) const = 0;

  //! Computes the point P, the first,the second and the third
  //! derivatives in the directions U and V at this point.
  //! Raised if the continuity of the surface is not C2.
  Standard_EXPORT virtual void D3 (const Standard_Real U, const Standard_Real V, gp_Pnt& P, gp_Vec& D1U, gp_Vec& D1V, gp_Vec& D2U, gp_Vec& D2V, gp_Vec& D2UV, gp_Vec& D3U, gp_Vec& D3V, gp_Vec& D3UUV, gp_Vec& D3UVV) const = 0;

  //! Computes the derivative of order Nu in the direction U and Nv in the direction V at the point P(U, V).
  //!
  //! Raised if the continuity of the surface is not CNu in the U direction or not CNv in the V direction.
  //! Raised if Nu + Nv < 1 or Nu < 0 or Nv < 0.
  Standard_EXPORT virtual gp_Vec DN (const Standard_Real U, const Standard_Real V, const Standard_Integer Nu, const Standard_Integer Nv) const = 0;

  //! Computes the point of parameter (U, V) on the surface.
  //!
  //! It is implemented with D0.
  //! Tip: use GeomLib::NormEstim() to calculate surface normal at specified (U, V) point.
  //!
  //! Raised only for an "OffsetSurface" if it is not possible to compute the current point.
  Standard_EXPORT gp_Pnt Value (const Standard_Real U, const Standard_Real V) const;

  //! Dumps the content of me into the stream
  Standard_EXPORT virtual void DumpJson (Standard_OStream& theOStream, Standard_Integer theDepth = -1) const Standard_OVERRIDE;

  DEFINE_STANDARD_RTTIEXT(Geom_Surface,Geom_Geometry)

};

#endif // _Geom_Surface_HeaderFile
