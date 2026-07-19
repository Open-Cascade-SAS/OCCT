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

#ifndef _Geom2d_OffsetCurve_HeaderFile
#define _Geom2d_OffsetCurve_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <GeomAbs_Shape.hxx>
#include <Geom2d_Curve.hxx>

class gp_Trsf2d;
class Geom2d_Geometry;

namespace Geom2dEval_RepCurveDesc
{
class Base;
}

//! This class implements the basis services for the creation,
//! edition, modification and evaluation of planar offset curve.
//! The offset curve is obtained by offsetting by distance along
//! the normal to a basis curve defined in 2D space.
//! The offset curve in this package can be a self intersecting
//! curve even if the basis curve does not self-intersect.
//! The self intersecting portions are not deleted at the
//! construction time.
//! An offset curve is a curve at constant distance (Offset) from a
//! basis curve and the offset curve takes its parametrization from
//! the basis curve. The Offset curve is in the direction of the
//! normal to the basis curve N.
//! The distance offset may be positive or negative to indicate the
//! preferred side of the curve :
//! . distance offset >0 => the curve is in the direction of N
//! . distance offset >0 => the curve is in the direction of - N
//! On the Offset curve :
//! Value(u) = BasisCurve.Value(U) + (Offset * (T ^ Z)) / ||T ^ Z||
//! where T is the tangent vector to the basis curve and Z the
//! direction of the normal vector to the plane of the curve,
//! N = T ^ Z defines the offset direction and should not have
//! null length.
//!
//! Warnings :
//! In this package we suppose that the continuity of the offset
//! curve is one degree less than the continuity of the
//! basis curve and we don't check that at any point ||T^Z|| != 0.0
//!
//! So to evaluate the curve it is better to check that the offset
//! curve is well defined at any point because an exception could
//! be raised. The check is not done in this package at the creation
//! of the offset curve because the control needs the use of an
//! algorithm which cannot be implemented in this package.
//! The OffsetCurve is closed if the first point and the last point
//! are the same (The distance between these two points is lower or
//! equal to the Resolution sea package gp) . The OffsetCurve can be
//! closed even if the basis curve is not closed.
class Geom2d_OffsetCurve : public Geom2d_Curve
{

public:
  //! Constructs a curve offset from the basis curve C,
  //! where Offset is the distance between the offset
  //! curve and the basis curve at any point.
  //! A point on the offset curve is built by measuring the
  //! offset value along a normal vector at a point on C.
  //! This normal vector is obtained by rotating the
  //! vector tangential to C at 90 degrees in the
  //! anti-trigonometric sense. The side of C on which
  //! the offset value is measured is indicated by this
  //! normal vector if Offset is positive, or in the inverse
  //! sense if Offset is negative.
  //! If isNotCheckC0 = TRUE checking if basis curve has C0-continuity
  //! is not made.
  //! Warnings :
  //! In this package the entities are not shared. The OffsetCurve is
  //! built with a copy of the curve C. So when C is modified the
  //! OffsetCurve is not modified
  //! Warning! if isNotCheckC0 = false,
  //! ConstructionError raised if the basis curve C is not at least C1.
  //! No check is done to know if ||V^Z|| != 0.0 at any point.
  Standard_EXPORT Geom2d_OffsetCurve(const occ::handle<Geom2d_Curve>& C,
                                     const double                     Offset,
                                     const bool                       isNotCheckC0 = false);

  //! Copy constructor for optimized copying without validation.
  Standard_EXPORT Geom2d_OffsetCurve(const Geom2d_OffsetCurve& theOther);

  //! Returns true if an evaluation representation is attached.
  bool HasEvalRepresentation() const { return !myEvalRep.IsNull(); }

  //! Returns the current evaluation representation descriptor (may be null).
  const occ::handle<Geom2dEval_RepCurveDesc::Base>& EvalRepresentation() const { return myEvalRep; }

  //! Sets a new evaluation representation.
  //! Validates descriptor data and ensures no circular references.
  Standard_EXPORT void SetEvalRepresentation(
    const occ::handle<Geom2dEval_RepCurveDesc::Base>& theDesc);

  //! Removes the evaluation representation.
  void ClearEvalRepresentation() { myEvalRep.Nullify(); }

  //! Changes the direction of parametrization of <me>.
  //! As a result:
  //! - the basis curve is reversed,
  //! - the start point of the initial curve becomes the end
  //! point of the reversed curve,
  //! - the end point of the initial curve becomes the start
  //! point of the reversed curve, and
  //! - the first and last parameters are recomputed.
  Standard_EXPORT void Reverse() final;

  //! Computes the parameter on the reversed curve for
  //! the point of parameter U on this offset curve.
  Standard_EXPORT double ReversedParameter(const double U) const final;

  //! Changes this offset curve by assigning C as the
  //! basis curve from which it is built.
  //! If isNotCheckC0 = TRUE checking if basis curve has C0-continuity
  //! is not made.
  //! Exceptions
  //! if isNotCheckC0 = false,
  //! Standard_ConstructionError if the curve C is not at least "C1" continuous.
  Standard_EXPORT void SetBasisCurve(const occ::handle<Geom2d_Curve>& C,
                                     const bool                       isNotCheckC0 = false);

  //! Changes this offset curve by assigning D as the offset value.
  Standard_EXPORT void SetOffsetValue(const double D);

  //! Returns the basis curve of this offset curve. The basis curve can be an offset curve.
  Standard_EXPORT occ::handle<Geom2d_Curve> BasisCurve() const;

  //! Continuity of the Offset curve :
  //! C0 : only geometric continuity,
  //! C1 : continuity of the first derivative all along the Curve,
  //! C2 : continuity of the second derivative all along the Curve,
  //! C3 : continuity of the third derivative all along the Curve,
  //! G1 : tangency continuity all along the Curve,
  //! G2 : curvature continuity all along the Curve,
  //! CN : the order of continuity is infinite.
  //! Warnings :
  //! Returns the continuity of the basis curve - 1.
  //! The offset curve must have a unique normal direction defined
  //! at any point.
  //! Value and derivatives
  //!
  //! Warnings :
  //! The exception UndefinedValue or UndefinedDerivative is
  //! raised if it is not possible to compute a unique offset
  //! direction.
  //! If T is the first derivative with not null length and
  //! Z the direction normal to the plane of the curve, the
  //! relation ||T(U) ^ Z|| != 0 must be satisfied to evaluate
  //! the offset curve.
  //! No check is done at the creation time and we suppose
  //! in this package that the offset curve is well defined.
  Standard_EXPORT GeomAbs_Shape Continuity() const final;

  //! Warning! this should not be called
  //! if the basis curve is not at least C1. Nevertheless
  //! if used on portion where the curve is C1, it is OK
  Standard_EXPORT gp_Pnt2d EvalD0(const double U) const final;

  //! Warning! this should not be called
  //! if the continuity of the basis curve is not C2.
  //! Nevertheless, it's OK to use it on portion
  //! where the curve is C2
  Standard_EXPORT Geom2d_Curve::ResD1 EvalD1(const double U) const final;

  //! Warning! This should not be called
  //! if the continuity of the basis curve is not C3.
  //! Nevertheless, it's OK to use it on portion
  //! where the curve is C3
  Standard_EXPORT Geom2d_Curve::ResD2 EvalD2(const double U) const final;

  //! Warning! This should not be called
  //! if the continuity of the basis curve is not C4.
  //! Nevertheless, it's OK to use it on portion
  //! where the curve is C4
  Standard_EXPORT Geom2d_Curve::ResD3 EvalD3(const double U) const final;

  //! The returned vector gives the value of the derivative
  //! for the order of derivation N.
  //! Warning! this should not be called
  //! raises UndefunedDerivative if the continuity of the basis curve is not CN+1.
  //! Nevertheless, it's OK to use it on portion
  //! where the curve is CN+1
  //! raises RangeError if N < 1.
  //! raises NotImplemented if N > 3.
  //! The following functions compute the value and derivatives
  //! on the offset curve and returns the derivatives on the
  //! basis curve too.
  //! The computation of the value and derivatives on the basis
  //! curve are used to evaluate the offset curve
  //! Warnings :
  //! The exception UndefinedValue or UndefinedDerivative is
  //! raised if it is not possible to compute a unique offset direction.
  Standard_EXPORT gp_Vec2d EvalDN(const double U, const int N) const final;

  //! Returns the value of the first parameter of this
  //! offset curve. The first parameter corresponds to the
  //! start point of the curve.
  //! Note: the first and last parameters of this offset curve
  //! are also the ones of its basis curve.
  Standard_EXPORT double FirstParameter() const final;

  //! Returns the value of the last parameter of this
  //! offset curve. The last parameter
  //! corresponds to the end point.
  //! Note: the first and last parameters of this offset curve
  //! are also the ones of its basis curve.
  Standard_EXPORT double LastParameter() const final;

  //! Returns the offset value of this offset curve.
  Standard_EXPORT double Offset() const;

  //! Returns True if the distance between the start point
  //! and the end point of the curve is lower or equal to
  //! Resolution from package gp.
  Standard_EXPORT bool IsClosed() const final;

  //! Is the order of continuity of the curve N ?
  //! Warnings :
  //! This method answer True if the continuity of the basis curve
  //! is N + 1. We suppose in this class that a normal direction
  //! to the basis curve (used to compute the offset curve) is
  //! defined at any point on the basis curve.
  //! Raised if N < 0.
  Standard_EXPORT bool IsCN(const int N) const final;

  //! Is the parametrization of a curve is periodic ?
  //! If the basis curve is a circle or an ellipse the corresponding
  //! OffsetCurve is periodic. If the basis curve can't be periodic
  //! (for example BezierCurve) the OffsetCurve can't be periodic.
  Standard_EXPORT bool IsPeriodic() const final;

  //! Returns the period of this offset curve, i.e. the period
  //! of the basis curve of this offset curve.
  //! Exceptions
  //! Standard_NoSuchObject if the basis curve is not periodic.
  Standard_EXPORT double Period() const final;

  //! Applies the transformation T to this offset curve.
  //! Note: the basis curve is also modified.
  Standard_EXPORT void Transform(const gp_Trsf2d& T) final;

  //! Returns the parameter on the transformed curve for
  //! the transform of the point of parameter U on <me>.
  //!
  //! me->Transformed(T)->Value(me->TransformedParameter(U,T))
  //!
  //! is the same point as
  //!
  //! me->Value(U).Transformed(T)
  //!
  //! This methods calls the basis curve method.
  Standard_EXPORT double TransformedParameter(const double U, const gp_Trsf2d& T) const final;

  //! Returns a coefficient to compute the parameter on
  //! the transformed curve for the transform of the
  //! point on <me>.
  //!
  //! Transformed(T)->Value(U * ParametricTransformation(T))
  //!
  //! is the same point as
  //!
  //! Value(U).Transformed(T)
  //!
  //! This methods calls the basis curve method.
  Standard_EXPORT double ParametricTransformation(const gp_Trsf2d& T) const final;

  //! Creates a new object, which is a copy of this offset curve.
  Standard_EXPORT occ::handle<Geom2d_Geometry> Copy() const final;

  //! Returns continuity of the basis curve.
  Standard_EXPORT GeomAbs_Shape GetBasisCurveContinuity() const;

  //! Dumps the content of me into the stream
  Standard_EXPORT void DumpJson(Standard_OStream& theOStream, int theDepth = -1) const final;

  DEFINE_STANDARD_RTTIEXT(Geom2d_OffsetCurve, Geom2d_Curve)

private:
  occ::handle<Geom2d_Curve>                  basisCurve;
  occ::handle<Geom2dEval_RepCurveDesc::Base> myEvalRep;
  double                                     offsetValue;
  GeomAbs_Shape                              myBasisCurveContinuity;
};

#endif // _Geom2d_OffsetCurve_HeaderFile
