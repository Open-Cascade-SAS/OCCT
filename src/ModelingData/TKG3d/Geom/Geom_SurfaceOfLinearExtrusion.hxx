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

#ifndef _Geom_SurfaceOfLinearExtrusion_HeaderFile
#define _Geom_SurfaceOfLinearExtrusion_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Geom_SweptSurface.hxx>
class Geom_Curve;
class gp_Dir;
class gp_Trsf;
class gp_GTrsf2d;
class Geom_Geometry;

namespace Geom_EvalRepSurfaceDesc
{
class Base;
}

//! Describes a surface of linear extrusion ("extruded
//! surface"), e.g. a generalized cylinder. Such a surface
//! is obtained by sweeping a curve (called the "extruded
//! curve" or "basis") in a given direction (referred to as
//! the "direction of extrusion" and defined by a unit vector).
//! The u parameter is along the extruded curve. The v
//! parameter is along the direction of extrusion.
//! The parameter range for the u parameter is defined
//! by the reference curve.
//! The parameter range for the v parameter is ] -
//! infinity, + infinity [.
//! The position of the curve gives the origin of the v parameter.
//! The surface is "CN" in the v parametric direction.
//! The form of a surface of linear extrusion is generally a
//! ruled surface (GeomAbs_RuledForm). It can be:
//! - a cylindrical surface, if the extruded curve is a circle,
//! or a trimmed circle, with an axis parallel to the
//! direction of extrusion (GeomAbs_CylindricalForm), or
//! - a planar surface, if the extruded curve is a line
//! (GeomAbs_PlanarForm).
//! Note: The surface of extrusion is built from a copy of
//! the original basis curve, so the original curve is not
//! modified when the surface is modified.
//! Warning
//! Degenerate surfaces are not detected. A degenerate
//! surface is obtained, for example, when the extruded
//! curve is a line and the direction of extrusion is parallel
//! to that line.
class Geom_SurfaceOfLinearExtrusion : public Geom_SweptSurface
{

public:
  //! V is the direction of extrusion.
  //! C is the extruded curve.
  //! The form of a SurfaceOfLinearExtrusion can be :
  //! . ruled surface (RuledForm),
  //! . a cylindrical surface if the extruded curve is a circle or
  //! a trimmed circle (CylindricalForm),
  //! . a plane surface if the extruded curve is a Line (PlanarForm).
  //! Warnings :
  //! Degenerated surface cases are not detected. For example if the
  //! curve C is a line and V is parallel to the direction of this
  //! line.
  Standard_EXPORT Geom_SurfaceOfLinearExtrusion(const occ::handle<Geom_Curve>& C, const gp_Dir& V);

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

  //! Assigns V as the "direction of extrusion" for this
  //! surface of linear extrusion.
  Standard_EXPORT void SetDirection(const gp_Dir& V);

  //! Modifies this surface of linear extrusion by redefining
  //! its "basis curve" (the "extruded curve").
  Standard_EXPORT void SetBasisCurve(const occ::handle<Geom_Curve>& C);

  //! Changes the orientation of this surface of linear
  //! extrusion in the u parametric direction. The
  //! bounds of the surface are not changed, but the given
  //! parametric direction is reversed. Hence the
  //! orientation of the surface is reversed.
  //! In the case of a surface of linear extrusion:
  //! - UReverse reverses the basis curve, and
  //! - VReverse reverses the direction of linear extrusion.
  Standard_EXPORT void UReverse() final;

  //! Computes the u parameter on the modified
  //! surface, produced by reversing its u parametric
  //! direction, for any point of u parameter U on this surface of linear extrusion.
  //! In the case of an extruded surface:
  //! - UReverseParameter returns the reversed
  //! parameter given by the function
  //! ReversedParameter called with U on the basis curve,
  Standard_EXPORT double UReversedParameter(const double U) const final;

  //! Changes the orientation of this surface of linear
  //! extrusion in the v parametric direction. The
  //! bounds of the surface are not changed, but the given
  //! parametric direction is reversed. Hence the
  //! orientation of the surface is reversed.
  //! In the case of a surface of linear extrusion:
  //! - UReverse reverses the basis curve, and
  //! - VReverse reverses the direction of linear extrusion.
  Standard_EXPORT void VReverse() final;

  //! Computes the v parameter on the modified
  //! surface, produced by reversing its u v parametric
  //! direction, for any point of v parameter V on this surface of linear extrusion.
  //! In the case of an extruded surface VReverse returns -V.
  Standard_EXPORT double VReversedParameter(const double V) const final;

  //! Returns the parametric bounds U1, U2, V1 and V2 of
  //! this surface of linear extrusion.
  //! A surface of linear extrusion is infinite in the v
  //! parametric direction, so:
  //! - V1 = double::RealFirst()
  //! - V2 = double::RealLast().
  Standard_EXPORT void Bounds(double& U1, double& U2, double& V1, double& V2) const final;

  //! IsUClosed returns true if the "basis curve" of this
  //! surface of linear extrusion is closed.
  Standard_EXPORT bool IsUClosed() const final;

  //! IsVClosed always returns false.
  Standard_EXPORT bool IsVClosed() const final;

  //! IsCNu returns true if the degree of continuity for the
  //! "basis curve" of this surface of linear extrusion is at least N.
  //! Raises RangeError if N < 0.
  Standard_EXPORT bool IsCNu(const int N) const final;

  //! IsCNv always returns true.
  Standard_EXPORT bool IsCNv(const int N) const final;

  //! IsUPeriodic returns true if the "basis curve" of this
  //! surface of linear extrusion is periodic.
  Standard_EXPORT bool IsUPeriodic() const final;

  //! IsVPeriodic always returns false.
  Standard_EXPORT bool IsVPeriodic() const final;

  //! Computes the U isoparametric curve of this surface
  //! of linear extrusion. This is the line parallel to the
  //! direction of extrusion, passing through the point of
  //! parameter U of the basis curve.
  Standard_EXPORT occ::handle<Geom_Curve> UIso(const double U) const final;

  //! Computes the V isoparametric curve of this surface
  //! of linear extrusion. This curve is obtained by
  //! translating the extruded curve in the direction of
  //! extrusion, with the magnitude V.
  Standard_EXPORT occ::handle<Geom_Curve> VIso(const double V) const final;

  //! Computes the point P (U, V) on the surface.
  //! The parameter U is the parameter on the extruded curve.
  //! The parametrization V is a linear parametrization, and
  //! the direction of parametrization is the direction of
  //! extrusion. If the point is on the extruded curve, V = 0.0
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
  //! Raises RangeError if Nu + Nv < 1 or Nu < 0 or Nv < 0.
  Standard_EXPORT gp_Vec EvalDN(const double U,
                                               const double V,
                                               const int    Nu,
                                               const int    Nv) const final;

  //! Applies the transformation T to this surface of linear extrusion.
  Standard_EXPORT void Transform(const gp_Trsf& T) final;

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
  //! This method multiplies:
  //! U by BasisCurve()->ParametricTransformation(T)
  //! V by T.ScaleFactor()
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
  //! This method returns a scale
  //! U by BasisCurve()->ParametricTransformation(T)
  //! V by T.ScaleFactor()
  Standard_EXPORT gp_GTrsf2d ParametricTransformation(const gp_Trsf& T) const final;

  //! Creates a new object which is a copy of this surface of linear extrusion.
  Standard_EXPORT occ::handle<Geom_Geometry> Copy() const final;

  //! Dumps the content of me into the stream
  Standard_EXPORT void DumpJson(Standard_OStream& theOStream, int theDepth = -1) const final;

  DEFINE_STANDARD_RTTIEXT(Geom_SurfaceOfLinearExtrusion, Geom_SweptSurface)

private:
  occ::handle<Geom_EvalRepSurfaceDesc::Base> myEvalRep;
};

#endif // _Geom_SurfaceOfLinearExtrusion_HeaderFile
