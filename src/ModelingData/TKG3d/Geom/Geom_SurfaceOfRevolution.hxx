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

#include <Geom_SweptSurface.hxx>
#include <gp_Pnt.hxx>
class Geom_Curve;
class gp_Ax1;
class gp_Dir;
class gp_Ax2;
class gp_Trsf;
class gp_GTrsf2d;
class Geom_Geometry;

namespace Geom_EvalRepSurfaceDesc
{
class Base;
}

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
  //! C : is the meridian or the referenced curve.
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
  Standard_EXPORT Geom_SurfaceOfRevolution(const occ::handle<Geom_Curve>& C, const gp_Ax1& A1);

  //! Returns true if an evaluation representation is attached.
  bool HasEvalRepresentation() const { return !myEvalRep.IsNull(); }

  //! Returns the current evaluation representation descriptor (may be null).
  const occ::handle<Geom_EvalRepSurfaceDesc::Base>& EvalRepresentation() const { return myEvalRep; }

  //! Sets a new evaluation representation.
  //! Validates descriptor data and ensures no circular references.
  Standard_EXPORT void SetEvalRepresentation(
    const occ::handle<Geom_EvalRepSurfaceDesc::Base>& theDesc);

  //! Removes the evaluation representation.
  void ClearEvalRepresentation() { myEvalRep.Nullify(); }

  //! Changes the axis of revolution.
  //! Warnings :
  //! It is not checked that the axis is in the plane of the
  //! revolved curve.
  Standard_EXPORT void SetAxis(const gp_Ax1& A1);

  //! Changes the direction of the revolution axis.
  //! Warnings :
  //! It is not checked that the axis is in the plane of the
  //! revolved curve.
  Standard_EXPORT void SetDirection(const gp_Dir& V);

  //! Changes the revolved curve of the surface.
  //! Warnings :
  //! It is not checked that the curve C is planar and that the
  //! surface axis is in the plane of the curve.
  //! It is not checked that the revolved curve C doesn't
  //! self-intersects.
  Standard_EXPORT void SetBasisCurve(const occ::handle<Geom_Curve>& C);

  //! Changes the location point of the revolution axis.
  //! Warnings :
  //! It is not checked that the axis is in the plane of the
  //! revolved curve.
  Standard_EXPORT void SetLocation(const gp_Pnt& P);

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
  //! in the u parametric direction. The bounds of the
  //! surface are not changed but the given parametric
  //! direction is reversed. Hence the orientation of the
  //! surface is reversed.
  //! As a consequence:
  //! - UReverse reverses the direction of the axis of
  //! revolution of this surface,
  Standard_EXPORT void UReverse() final;

  //! Computes the u parameter on the modified
  //! surface, when reversing its u parametric
  //! direction, for any point of u parameter U on this surface of revolution.
  //! In the case of a revolved surface:
  //! - UReversedParameter returns 2.*Pi - U
  Standard_EXPORT double UReversedParameter(const double U) const final;

  //! Changes the orientation of this surface of revolution
  //! in the v parametric direction. The bounds of the
  //! surface are not changed but the given parametric
  //! direction is reversed. Hence the orientation of the
  //! surface is reversed.
  //! As a consequence:
  //! - VReverse reverses the meridian of this surface of revolution.
  Standard_EXPORT void VReverse() final;

  //! Computes the v parameter on the modified
  //! surface, when reversing its v parametric
  //! direction, for any point of v parameter V on this surface of revolution.
  //! In the case of a revolved surface:
  //! - VReversedParameter returns the reversed
  //! parameter given by the function
  //! ReversedParameter called with V on the meridian.
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
  //! This method multiplies V by BasisCurve()->ParametricTransformation(T)
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
  //! Where U',V' are obtained by transforming U,V with
  //! the 2d transformation returned by
  //! @code
  //!   me->ParametricTransformation(T)
  //! @endcode
  //! This method returns a scale centered on the
  //! U axis with BasisCurve()->ParametricTransformation(T)
  Standard_EXPORT gp_GTrsf2d ParametricTransformation(const gp_Trsf& T) const final;

  //! Returns the parametric bounds U1, U2 , V1 and V2 of this surface.
  //! A surface of revolution is always complete, so U1 = 0, U2 = 2*PI.
  Standard_EXPORT void Bounds(double& U1, double& U2, double& V1, double& V2) const final;

  //! IsUClosed always returns true.
  Standard_EXPORT bool IsUClosed() const final;

  //! IsVClosed returns true if the meridian of this
  //! surface of revolution is closed.
  Standard_EXPORT bool IsVClosed() const final;

  //! IsCNu always returns true.
  Standard_EXPORT bool IsCNu(const int N) const final;

  //! IsCNv returns true if the degree of continuity of the
  //! meridian of this surface of revolution is at least N.
  //! Raised if N < 0.
  Standard_EXPORT bool IsCNv(const int N) const final;

  //! Returns True.
  Standard_EXPORT bool IsUPeriodic() const final;

  //! IsVPeriodic returns true if the meridian of this
  //! surface of revolution is periodic.
  Standard_EXPORT bool IsVPeriodic() const final;

  //! Computes the U isoparametric curve of this surface
  //! of revolution. It is the curve obtained by rotating the
  //! meridian through an angle U about the axis of revolution.
  Standard_EXPORT occ::handle<Geom_Curve> UIso(const double U) const final;

  //! Computes the U isoparametric curve of this surface
  //! of revolution. It is the curve obtained by rotating the
  //! meridian through an angle U about the axis of revolution.
  Standard_EXPORT occ::handle<Geom_Curve> VIso(const double V) const final;

  //! Computes the point P (U, V) on the surface.
  //! U is the angle of the rotation around the revolution axis.
  //! The direction of this axis gives the sense of rotation.
  //! V is the parameter of the revolved curve.
  //! Raises an exception on failure.
  Standard_EXPORT gp_Pnt EvalD0(const double U, const double V) const final;

  //! Computes the point and first partial derivatives at (U, V).
  //! Raises an exception if the surface continuity is not C1.
  Standard_EXPORT Geom_Surface::ResD1 EvalD1(const double U,
                                                            const double V) const final;

  //! Computes the point and partial derivatives up to 2nd order at (U, V).
  //! Raises an exception if the surface continuity is not C2.
  Standard_EXPORT Geom_Surface::ResD2 EvalD2(const double U,
                                                            const double V) const final;

  //! Computes the point and partial derivatives up to 3rd order at (U, V).
  //! Raises an exception if the surface continuity is not C3.
  Standard_EXPORT Geom_Surface::ResD3 EvalD3(const double U,
                                                            const double V) const final;

  //! Computes the derivative of order Nu in U and Nv in V at (U, V).
  //! Raises an exception on failure.
  //!
  //! Raised if the continuity of the surface is not CNu in the u
  //! direction and CNv in the v direction.
  //! Raised if Nu + Nv < 1 or Nu < 0 or Nv < 0.
  Standard_EXPORT gp_Vec EvalDN(const double U,
                                               const double V,
                                               const int    Nu,
                                               const int    Nv) const final;

  //! Applies the transformation T to this surface of revolution.
  Standard_EXPORT void Transform(const gp_Trsf& T) final;

  //! Creates a new object which is a copy of this surface of revolution.
  Standard_EXPORT occ::handle<Geom_Geometry> Copy() const final;

  //! Dumps the content of me into the stream
  Standard_EXPORT void DumpJson(Standard_OStream& theOStream, int theDepth = -1) const final;

  DEFINE_STANDARD_RTTIEXT(Geom_SurfaceOfRevolution, Geom_SweptSurface)

private:
  occ::handle<Geom_EvalRepSurfaceDesc::Base> myEvalRep;
  gp_Pnt                                     loc;
};

#endif // _Geom_SurfaceOfRevolution_HeaderFile
