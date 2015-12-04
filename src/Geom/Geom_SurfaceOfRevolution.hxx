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

#ifndef _Geom_SurfaceOfRevolution_HeaderFile
#define _Geom_SurfaceOfRevolution_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <gp_Pnt.hxx>
#include <Geom_SweptSurface.hxx>
#include <GeomEvaluator_SurfaceOfRevolution.hxx>
#include <Standard_Real.hxx>
#include <Standard_Boolean.hxx>
#include <Standard_Integer.hxx>
class Standard_ConstructionError;
class Standard_RangeError;
class Geom_UndefinedDerivative;
class Geom_Curve;
class gp_Ax1;
class gp_Dir;
class gp_Pnt;
class gp_Ax2;
class gp_Trsf;
class gp_GTrsf2d;
class gp_Vec;
class Geom_Geometry;


class Geom_SurfaceOfRevolution;
DEFINE_STANDARD_HANDLE(Geom_SurfaceOfRevolution, Geom_SweptSurface)

//! Describes a surface of revolution (revolved surface).
//! Such a surface is obtained by rotating a curve (called
//! the "meridian") through a complete revolution about
//! an axis (referred to as the "axis of revolution"). The
//! curve and the axis must be in the same plane (the
//! "reference plane" of the surface).
//! Rotation around the axis of revolution in the
//! trigonometric sense defines the u parametric
//! direction. So the u parameter is an angle, and its
//! origin is given by the position of the meridian on the surface.
//! The parametric range for the u parameter is: [ 0, 2.*Pi ]
//! The v parameter is that of the meridian.
//! Note: A surface of revolution is built from a copy of the
//! original meridian. As a result the original meridian is
//! not modified when the surface is modified.
//! The form of a surface of revolution is typically a
//! general revolution surface
//! (GeomAbs_RevolutionForm). It can be:
//! - a conical surface, if the meridian is a line or a
//! trimmed line (GeomAbs_ConicalForm),
//! - a cylindrical surface, if the meridian is a line or a
//! trimmed line parallel to the axis of revolution
//! (GeomAbs_CylindricalForm),
//! - a planar surface if the meridian is a line or a
//! trimmed line perpendicular to the axis of revolution
//! of the surface (GeomAbs_PlanarForm),
//! - a toroidal surface, if the meridian is a circle or a
//! trimmed circle (GeomAbs_ToroidalForm), or
//! - a spherical surface, if the meridian is a circle, the
//! center of which is located on the axis of the
//! revolved surface (GeomAbs_SphericalForm).
//! Warning
//! Be careful not to construct a surface of revolution
//! where the curve and the axis or revolution are not
//! defined in the same plane. If you do not have a
//! correct configuration, you can correct your initial
//! curve, using a cylindrical projection in the reference plane.
class Geom_SurfaceOfRevolution : public Geom_SweptSurface
{

public:

  

  //! C : is the meridian  or the referenced curve.
  //! A1 is the axis of revolution.
  //! The form of a SurfaceOfRevolution can be :
  //! . a general revolution surface (RevolutionForm),
  //! . a conical surface if the meridian is a line or a trimmed line
  //! (ConicalForm),
  //! . a cylindrical surface if the meridian is a line or a trimmed
  //! line parallel to the revolution axis (CylindricalForm),
  //! . a planar surface if the meridian is a line perpendicular to
  //! the revolution axis of the surface (PlanarForm).
  //! . a spherical surface,
  //! . a toroidal surface,
  //! . a quadric surface.
  //! Warnings :
  //! It is not checked that the curve C is planar and that the
  //! surface axis is in the plane of the curve.
  //! It is not checked that the revolved curve C doesn't
  //! self-intersects.
  Standard_EXPORT Geom_SurfaceOfRevolution(const Handle(Geom_Curve)& C, const gp_Ax1& A1);
  
  //! Changes the axis of revolution.
  //! Warnings :
  //! It is not checked that the axis is in the plane of the
  //! revolved curve.
  Standard_EXPORT void SetAxis (const gp_Ax1& A1);
  
  //! Changes the direction of the revolution axis.
  //! Warnings :
  //! It is not checked that the axis is in the plane of the
  //! revolved curve.
  Standard_EXPORT void SetDirection (const gp_Dir& V);
  
  //! Changes the revolved curve of the surface.
  //! Warnings :
  //! It is not checked that the curve C is planar and that the
  //! surface axis is in the plane of the curve.
  //! It is not checked that the revolved curve C doesn't
  //! self-intersects.
  Standard_EXPORT void SetBasisCurve (const Handle(Geom_Curve)& C);
  
  //! Changes the location point of the revolution axis.
  //! Warnings :
  //! It is not checked that the axis is in the plane of the
  //! revolved curve.
  Standard_EXPORT void SetLocation (const gp_Pnt& P);
  
  //! Returns the revolution axis of the surface.
  Standard_EXPORT gp_Ax1 Axis() const;
  

  //! Returns the location point of the axis of revolution.
  Standard_EXPORT const gp_Pnt& Location() const;
  

  //! Computes the position of the reference plane of the surface
  //! defined by the basis curve and the symmetry axis.
  //! The location point is the location point of the revolution's
  //! axis, the XDirection of the plane is given by the revolution's
  //! axis and the orientation of the normal to the plane is given
  //! by the sense of revolution.
  //!
  //! Raised if the revolved curve is not planar or if the revolved
  //! curve and the symmetry axis are not in the same plane or if
  //! the maximum of distance between the axis and the revolved
  //! curve is lower or equal to Resolution from gp.
  Standard_EXPORT gp_Ax2 ReferencePlane() const;
  
  //! Changes the orientation of this surface of revolution
  //! in the u  parametric direction. The bounds of the
  //! surface are not changed but the given parametric
  //! direction is reversed. Hence the orientation of the
  //! surface is reversed.
  //! As a consequence:
  //! - UReverse reverses the direction of the axis of
  //! revolution of this surface,
  Standard_EXPORT void UReverse() Standard_OVERRIDE;
  
  //! Computes the u  parameter on the modified
  //! surface, when reversing its u  parametric
  //! direction, for any point of u parameter U  on this surface of revolution.
  //! In the case of a revolved surface:
  //! - UReversedParameter returns 2.*Pi - U
  Standard_EXPORT Standard_Real UReversedParameter (const Standard_Real U) const Standard_OVERRIDE;
  
  //! Changes the orientation of this surface of revolution
  //! in the v parametric direction. The bounds of the
  //! surface are not changed but the given parametric
  //! direction is reversed. Hence the orientation of the
  //! surface is reversed.
  //! As a consequence:
  //! - VReverse reverses the meridian of this surface of revolution.
  Standard_EXPORT void VReverse() Standard_OVERRIDE;
  
  //! Computes the  v parameter on the modified
  //! surface, when reversing its  v parametric
  //! direction, for any point of v parameter V on this surface of revolution.
  //! In the case of a revolved surface:
  //! - VReversedParameter returns the reversed
  //! parameter given by the function
  //! ReversedParameter called with V on the meridian.
  Standard_EXPORT Standard_Real VReversedParameter (const Standard_Real V) const Standard_OVERRIDE;
  
  //! Computes the  parameters on the  transformed  surface for
  //! the transform of the point of parameters U,V on <me>.
  //!
  //! me->Transformed(T)->Value(U',V')
  //!
  //! is the same point as
  //!
  //! me->Value(U,V).Transformed(T)
  //!
  //! Where U',V' are the new values of U,V after calling
  //!
  //! me->TranformParameters(U,V,T)
  //!
  //! This methods multiplies V by
  //! BasisCurve()->ParametricTransformation(T)
  Standard_EXPORT virtual void TransformParameters (Standard_Real& U, Standard_Real& V, const gp_Trsf& T) const Standard_OVERRIDE;
  
  //! Returns a 2d transformation  used to find the  new
  //! parameters of a point on the transformed surface.
  //!
  //! me->Transformed(T)->Value(U',V')
  //!
  //! is the same point as
  //!
  //! me->Value(U,V).Transformed(T)
  //!
  //! Where U',V' are  obtained by transforming U,V with
  //! th 2d transformation returned by
  //!
  //! me->ParametricTransformation(T)
  //!
  //! This  methods  returns  a scale  centered  on  the
  //! U axis with BasisCurve()->ParametricTransformation(T)
  Standard_EXPORT virtual gp_GTrsf2d ParametricTransformation (const gp_Trsf& T) const Standard_OVERRIDE;
  
  //! Returns the parametric bounds U1, U2 , V1 and V2 of this surface.
  //! A surface of revolution is always complete, so U1 = 0, U2 = 2*PI.
  Standard_EXPORT void Bounds (Standard_Real& U1, Standard_Real& U2, Standard_Real& V1, Standard_Real& V2) const Standard_OVERRIDE;
  
  //! IsUClosed always returns true.
  Standard_EXPORT Standard_Boolean IsUClosed() const Standard_OVERRIDE;
  
  //! IsVClosed returns true if the meridian of this
  //! surface of revolution is closed.
  Standard_EXPORT Standard_Boolean IsVClosed() const Standard_OVERRIDE;
  
  //! IsCNu always returns true.
  Standard_EXPORT Standard_Boolean IsCNu (const Standard_Integer N) const Standard_OVERRIDE;
  
  //! IsCNv returns true if the degree of continuity of the
  //! meridian of this surface of revolution is at least N.
  //! Raised if N < 0.
  Standard_EXPORT Standard_Boolean IsCNv (const Standard_Integer N) const Standard_OVERRIDE;
  
  //! Returns True.
  Standard_EXPORT Standard_Boolean IsUPeriodic() const Standard_OVERRIDE;
  
  //! IsVPeriodic returns true if the meridian of this
  //! surface of revolution is periodic.
  Standard_EXPORT Standard_Boolean IsVPeriodic() const Standard_OVERRIDE;
  
  //! Computes the U isoparametric curve of this surface
  //! of revolution. It is the curve obtained by rotating the
  //! meridian through an angle U about the axis of revolution.
  Standard_EXPORT Handle(Geom_Curve) UIso (const Standard_Real U) const Standard_OVERRIDE;
  
  //! Computes the U isoparametric curve of this surface
  //! of revolution. It is the curve obtained by rotating the
  //! meridian through an angle U about the axis of revolution.
  Standard_EXPORT Handle(Geom_Curve) VIso (const Standard_Real V) const Standard_OVERRIDE;
  
  //! Computes the  point P (U, V) on the surface.
  //! U is the angle of the rotation around the revolution axis.
  //! The direction of this axis gives the sense of rotation.
  //! V is the parameter of the revolved curve.
  Standard_EXPORT void D0 (const Standard_Real U, const Standard_Real V, gp_Pnt& P) const Standard_OVERRIDE;

  //! Computes the current point and the first derivatives
  //! in the directions U and V.
  //! Raised if the continuity of the surface is not C1.
  Standard_EXPORT void D1 (const Standard_Real U, const Standard_Real V, gp_Pnt& P, gp_Vec& D1U, gp_Vec& D1V) const Standard_OVERRIDE;

  //! Computes the current point, the first and the second derivatives
  //! in the directions U and V.
  //! Raised if the continuity of the surface is not C2.
  Standard_EXPORT void D2 (const Standard_Real U, const Standard_Real V, gp_Pnt& P, gp_Vec& D1U, gp_Vec& D1V, gp_Vec& D2U, gp_Vec& D2V, gp_Vec& D2UV) const Standard_OVERRIDE;

  //! Computes the current point, the first,the second and the third
  //! derivatives in the directions U and V.
  //! Raised if the continuity of the surface is not C3.
  Standard_EXPORT void D3 (const Standard_Real U, const Standard_Real V, gp_Pnt& P, gp_Vec& D1U, gp_Vec& D1V, gp_Vec& D2U, gp_Vec& D2V, gp_Vec& D2UV, gp_Vec& D3U, gp_Vec& D3V, gp_Vec& D3UUV, gp_Vec& D3UVV) const Standard_OVERRIDE;

  //! Computes the derivative of order Nu in the direction u and
  //! Nv in the direction v.
  //!
  //! Raised if the continuity of the surface is not CNu in the u
  //! direction and CNv in the v direction.
  //! Raised if Nu + Nv < 1 or Nu < 0 or Nv < 0.
  //! The following  functions  evaluates the  local
  //! derivatives on surface. Useful to manage discontinuities
  //! on the surface.
  //! if    Side  =  1  ->  P  =  S( U+,V )
  //! if    Side  = -1  ->  P  =  S( U-,V )
  //! else  P  is betveen discontinuities
  //! can be evaluated using methods  of
  //! global evaluations    P  =  S( U ,V )
  Standard_EXPORT gp_Vec DN (const Standard_Real U, const Standard_Real V, const Standard_Integer Nu, const Standard_Integer Nv) const Standard_OVERRIDE;

  //! Applies the transformation T to this surface of revolution.
  Standard_EXPORT void Transform (const gp_Trsf& T) Standard_OVERRIDE;
  
  //! Creates a new object which is a copy of this surface of revolution.
  Standard_EXPORT Handle(Geom_Geometry) Copy() const Standard_OVERRIDE;


  DEFINE_STANDARD_RTTIEXT(Geom_SurfaceOfRevolution,Geom_SweptSurface)

private:
  Handle(GeomEvaluator_SurfaceOfRevolution) myEvaluator;
  gp_Pnt loc;
};

#endif // _Geom_SurfaceOfRevolution_HeaderFile
