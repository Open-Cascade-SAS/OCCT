// Created on: 1993-03-09
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

#ifndef _Geom_BezierCurve_HeaderFile
#define _Geom_BezierCurve_HeaderFile

#include <Standard.hxx>

#include <gp_Pnt.hxx>
#include <NCollection_Array1.hxx>
#include <Geom_BoundedCurve.hxx>
#include <GeomAbs_Shape.hxx>
#include <BSplCLib.hxx>

class gp_Trsf;
class Geom_Geometry;
namespace Geom_EvalRepCurveDesc
{
class Base;
}

//! Describes a rational or non-rational Bezier curve
//! - a non-rational Bezier curve is defined by a table of
//! poles (also called control points),
//! - a rational Bezier curve is defined by a table of
//! poles with varying weights.
//! These data are manipulated by two parallel arrays:
//! - the poles table, which is an array of gp_Pnt points, and
//! - the weights table, which is an array of reals.
//! The bounds of these arrays are 1 and "the number of "poles" of the curve.
//! The poles of the curve are "control points" used to deform the curve.
//! The first pole is the start point of the curve, and the
//! last pole is the end point of the curve. The segment
//! that joins the first pole to the second pole is the
//! tangent to the curve at its start point, and the
//! segment that joins the last pole to the
//! second-from-last pole is the tangent to the curve at its end point.
//! It is more difficult to give a geometric signification to
//! the weights but they are useful for providing the exact
//! representations of arcs of a circle or ellipse.
//! Moreover, if the weights of all poles are equal, the
//! curve is polynomial; it is therefore a non-rational
//! curve. The non-rational curve is a special and
//! frequently used case. The weights are defined and
//! used only in the case of a rational curve.
//! The degree of a Bezier curve is equal to the number
//! of poles, minus 1. It must be greater than or equal to
//! 1. However, the degree of a Geom_BezierCurve
//! curve is limited to a value (25) which is defined and
//! controlled by the system. This value is returned by the function MaxDegree.
//! The parameter range for a Bezier curve is [ 0, 1 ].
//! If the first and last control points of the Bezier curve
//! are the same point then the curve is closed. For
//! example, to create a closed Bezier curve with four
//! control points, you have to give the set of control
//! points P1, P2, P3 and P1.
//! The continuity of a Bezier curve is infinite.
//! It is not possible to build a Bezier curve with negative
//! weights. We consider that a weight value is zero if it
//! is less than or equal to gp::Resolution(). We
//! also consider that two weight values W1 and W2 are equal if:
//! |W2 - W1| <= gp::Resolution().
//! Warning
//! - When considering the continuity of a closed Bezier
//! curve at the junction point, remember that a curve
//! of this type is never periodic. This means that the
//! derivatives for the parameter u = 0 have no
//! reason to be the same as the derivatives for the
//! parameter u = 1 even if the curve is closed.
//! - The length of a Bezier curve can be null.
class Geom_BezierCurve : public Geom_BoundedCurve
{

public:
  //! Creates a non rational Bezier curve with a set of poles
  //! CurvePoles. The weights are defaulted to all being 1.
  //! Raises ConstructionError if the number of poles is greater than MaxDegree + 1
  //! or lower than 2.
  Standard_EXPORT Geom_BezierCurve(const NCollection_Array1<gp_Pnt>& CurvePoles);

  //! Creates a rational Bezier curve with the set of poles
  //! CurvePoles and the set of weights PoleWeights.
  //! If all the weights are identical the curve is considered
  //! as non rational. Raises ConstructionError if
  //! the number of poles is greater than MaxDegree + 1 or lower
  //! than 2 or CurvePoles and CurveWeights have not the same length
  //! or one weight value is lower or equal to Resolution from package gp.
  Standard_EXPORT Geom_BezierCurve(const NCollection_Array1<gp_Pnt>& CurvePoles,
                                   const NCollection_Array1<double>& PoleWeights);

  //! Copy constructor for optimized copying without validation.
  //! @param[in] theOther the Bezier curve to copy from
  Standard_EXPORT Geom_BezierCurve(const Geom_BezierCurve& theOther);

  //! Returns true if an evaluation representation is attached.
  bool HasEvalRepresentation() const { return !myEvalRep.IsNull(); }

  //! Returns the current evaluation representation descriptor (may be null).
  const occ::handle<Geom_EvalRepCurveDesc::Base>& EvalRepresentation() const { return myEvalRep; }

  //! Sets a new evaluation representation.
  //! Validates descriptor data and ensures no circular references.
  Standard_EXPORT void SetEvalRepresentation(const occ::handle<Geom_EvalRepCurveDesc::Base>& theDesc);

  //! Removes the evaluation representation.
  void ClearEvalRepresentation() { myEvalRep.Nullify(); }

  //! Increases the degree of a bezier curve. Degree is the new
  //! degree of <me>. Raises ConstructionError
  //! if Degree is greater than MaxDegree or lower than 2
  //! or lower than the initial degree of <me>.
  Standard_EXPORT void Increase(const int Degree);

  //! Inserts a pole P after the pole of range Index.
  //! If the curve <me> is rational the weight value for the new
  //! pole of range Index is 1.0.
  //! raised if Index is not in the range [1, NbPoles]
  //!
  //! raised if the resulting number of poles is greater than
  //! MaxDegree + 1.
  Standard_EXPORT void InsertPoleAfter(const int Index, const gp_Pnt& P);

  //! Inserts a pole with its weight in the set of poles after the
  //! pole of range Index. If the curve was non rational it can
  //! become rational if all the weights are not identical.
  //! Raised if Index is not in the range [1, NbPoles]
  //!
  //! Raised if the resulting number of poles is greater than
  //! MaxDegree + 1.
  //! Raised if Weight is lower or equal to Resolution from package gp.
  Standard_EXPORT void InsertPoleAfter(const int Index, const gp_Pnt& P, const double Weight);

  //! Inserts a pole P before the pole of range Index.
  //! If the curve <me> is rational the weight value for the new
  //! pole of range Index is 1.0.
  //! Raised if Index is not in the range [1, NbPoles]
  //!
  //! Raised if the resulting number of poles is greater than
  //! MaxDegree + 1.
  Standard_EXPORT void InsertPoleBefore(const int Index, const gp_Pnt& P);

  //! Inserts a pole with its weight in the set of poles after
  //! the pole of range Index. If the curve was non rational it
  //! can become rational if all the weights are not identical.
  //! Raised if Index is not in the range [1, NbPoles]
  //!
  //! Raised if the resulting number of poles is greater than
  //! MaxDegree + 1.
  //! Raised if Weight is lower or equal to Resolution from
  //! package gp.
  Standard_EXPORT void InsertPoleBefore(const int Index, const gp_Pnt& P, const double Weight);

  //! Removes the pole of range Index.
  //! If the curve was rational it can become non rational.
  //! Raised if Index is not in the range [1, NbPoles]
  //! Raised if Degree is lower than 2.
  Standard_EXPORT void RemovePole(const int Index);

  //! Reverses the direction of parametrization of <me>
  //! Value (NewU) = Value (1 - OldU)
  Standard_EXPORT void Reverse() final;

  //! Returns the parameter on the reversed curve for
  //! the point of parameter U on <me>.
  //!
  //! returns 1-U
  Standard_EXPORT double ReversedParameter(const double U) const final;

  //! Segments the curve between U1 and U2 which can be out
  //! of the bounds of the curve. The curve is oriented from U1
  //! to U2.
  //! The control points are modified, the first and the last point
  //! are not the same but the parametrization range is [0, 1]
  //! else it could not be a Bezier curve.
  //! Warnings :
  //! Even if <me> is not closed it can become closed after the
  //! segmentation for example if U1 or U2 are out of the bounds
  //! of the curve <me> or if the curve makes loop.
  //! After the segmentation the length of a curve can be null.
  Standard_EXPORT void Segment(const double U1, const double U2);

  //! Substitutes the pole of range index with P.
  //! If the curve <me> is rational the weight of range Index
  //! is not modified.
  //! raiseD if Index is not in the range [1, NbPoles]
  Standard_EXPORT void SetPole(const int Index, const gp_Pnt& P);

  //! Substitutes the pole and the weights of range Index.
  //! If the curve <me> is not rational it can become rational
  //! if all the weights are not identical.
  //! If the curve was rational it can become non rational if
  //! all the weights are identical.
  //! Raised if Index is not in the range [1, NbPoles]
  //! Raised if Weight <= Resolution from package gp
  Standard_EXPORT void SetPole(const int Index, const gp_Pnt& P, const double Weight);

  //! Changes the weight of the pole of range Index.
  //! If the curve <me> is not rational it can become rational
  //! if all the weights are not identical.
  //! If the curve was rational it can become non rational if
  //! all the weights are identical.
  //! Raised if Index is not in the range [1, NbPoles]
  //! Raised if Weight <= Resolution from package gp
  Standard_EXPORT void SetWeight(const int Index, const double Weight);

  //! Returns True if the distance between the first point
  //! and the last point of the curve is lower or equal to
  //! the Resolution from package gp.
  Standard_EXPORT bool IsClosed() const final;

  //! Continuity of the curve, returns True.
  Standard_EXPORT bool IsCN(const int N) const final;

  //! Returns True if the parametrization of a curve is periodic.
  //! (P(u) = P(u + T) T = constante)
  Standard_EXPORT bool IsPeriodic() const final;

  //! Returns false if all the weights are identical. The tolerance
  //! criterion is Resolution from package gp.
  Standard_EXPORT bool IsRational() const;

  //! a Bezier curve is CN
  Standard_EXPORT GeomAbs_Shape Continuity() const final;

  //! Returns the polynomial degree of the curve.
  //! it is the number of poles - 1
  //! point P and derivatives (V1, V2, V3) computation
  //! The Bezier Curve has a Polynomial representation so the
  //! parameter U can be out of the bounds of the curve.
  Standard_EXPORT int Degree() const;

  Standard_EXPORT std::optional<gp_Pnt> EvalD0(const double U) const final;

  Standard_EXPORT std::optional<Geom_Curve::ResD1> EvalD1(const double U) const final;

  Standard_EXPORT std::optional<Geom_Curve::ResD2> EvalD2(const double U) const final;

  //! For this Bezier curve, computes
  //! - the point P of parameter U, or
  //! - the point P and one or more of the following values:
  //! - V1, the first derivative vector,
  //! - V2, the second derivative vector,
  //! - V3, the third derivative vector.
  //! Note: the parameter U can be outside the bounds of the curve.
  Standard_EXPORT std::optional<Geom_Curve::ResD3> EvalD3(const double U) const final;

  //! For the point of parameter U of this Bezier curve,
  //! computes the vector corresponding to the Nth derivative.
  //! Note: the parameter U can be outside the bounds of the curve.
  //! Exceptions Standard_RangeError if N is less than 1.
  Standard_EXPORT std::optional<gp_Vec> EvalDN(const double U, const int N) const final;

  //! Returns Value (U=0.), it is the first control point of the curve.
  Standard_EXPORT gp_Pnt StartPoint() const final;

  //! Returns Value (U=1.), it is the last control point of the Bezier curve.
  Standard_EXPORT gp_Pnt EndPoint() const final;

  //! Returns the value of the first parameter of this
  //! Bezier curve. This is 0.0, which gives the start point of this Bezier curve
  Standard_EXPORT double FirstParameter() const final;

  //! Returns the value of the last parameter of this
  //! Bezier curve. This is 1.0, which gives the end point of this Bezier curve.
  Standard_EXPORT double LastParameter() const final;

  //! Returns the number of poles of this Bezier curve.
  Standard_EXPORT int NbPoles() const;

  //! Returns the pole of range Index.
  //! Raised if Index is not in the range [1, NbPoles]
  Standard_EXPORT const gp_Pnt& Pole(const int Index) const;

  //! Returns all the poles of the curve.
  //!
  //! Raised if the length of P is not equal to the number of poles.
  Standard_DEPRECATED("use Poles() returning const reference instead")
  Standard_EXPORT void Poles(NCollection_Array1<gp_Pnt>& P) const;

  //! Returns all the poles of the curve.
  Standard_EXPORT const NCollection_Array1<gp_Pnt>& Poles() const;

  //! Returns the weight of range Index.
  //! Raised if Index is not in the range [1, NbPoles]
  Standard_EXPORT double Weight(const int Index) const;

  //! Returns all the weights of the curve.
  //!
  //! Raised if the length of W is not equal to the number of poles.
  Standard_DEPRECATED("use Weights() returning const pointer instead")
  Standard_EXPORT void Weights(NCollection_Array1<double>& W) const;

  //! Returns all the weights of the curve.
  const NCollection_Array1<double>* Weights() const
  {
    return myRational ? &myWeights : BSplCLib::NoWeights();
  }

  //! Returns a const reference to the weights array.
  //! For rational curves: the internal owning weights array.
  //! For non-rational curves: a non-owning view of unit weights from BSplCLib.
  //! The array is always sized to match NbPoles().
  //! @warning Do NOT modify elements through the returned reference.
  const NCollection_Array1<double>& WeightsArray() const { return myWeights; }

  //! Applies the transformation T to this Bezier curve.
  Standard_EXPORT void Transform(const gp_Trsf& T) final;

  //! Returns the value of the maximum polynomial degree
  //! of any Geom_BezierCurve curve. This value is 25.
  Standard_EXPORT static int MaxDegree();

  //! Computes for this Bezier curve the parametric
  //! tolerance UTolerance for a given 3D tolerance Tolerance3D.
  //! If f(t) is the equation of this Bezier curve,
  //! UTolerance ensures that:
  //! |t1-t0| < UTolerance ===> |f(t1)-f(t0)| < Tolerance3D
  Standard_EXPORT void Resolution(const double Tolerance3D, double& UTolerance);

  //! Creates a new object which is a copy of this Bezier curve.
  Standard_EXPORT occ::handle<Geom_Geometry> Copy() const final;

  //! Dumps the content of me into the stream
  Standard_EXPORT void DumpJson(Standard_OStream& theOStream, int theDepth = -1) const final;

  //! Returns Bezier knots {0.0, 1.0} as a static array.
  Standard_EXPORT const NCollection_Array1<double>& Knots() const;

  //! Returns Bezier multiplicities for the current degree.
  Standard_EXPORT const NCollection_Array1<int>& Multiplicities() const;

  //! Returns Bezier flat knots for the current degree.
  Standard_EXPORT const NCollection_Array1<double>& KnotSequence() const;

  DEFINE_STANDARD_RTTIEXT(Geom_BezierCurve, Geom_BoundedCurve)

protected:
  //! Set poles to thePoles, weights to theWeights.
  //! If theWeights is null the curve is non rational.
  //! Update rational and closed.
  void init(const NCollection_Array1<gp_Pnt>& thePoles,
            const NCollection_Array1<double>* theWeights);

private:
  NCollection_Array1<gp_Pnt> myPoles;
  NCollection_Array1<double> myWeights;
  occ::handle<Geom_EvalRepCurveDesc::Base> myEvalRep;
  bool                       myRational      = false;
  bool                       myClosed        = false;
  double                     myMaxDerivInv   = 0.0;
  bool                       myMaxDerivInvOk = false;
};

#endif // _Geom_BezierCurve_HeaderFile
