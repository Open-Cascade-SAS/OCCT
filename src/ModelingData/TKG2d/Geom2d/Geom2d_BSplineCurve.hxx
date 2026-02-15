// Created on: 1993-03-24
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

#ifndef _Geom2d_BSplineCurve_HeaderFile
#define _Geom2d_BSplineCurve_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Precision.hxx>
#include <GeomAbs_BSplKnotDistribution.hxx>
#include <GeomAbs_Shape.hxx>
#include <gp_Pnt2d.hxx>
#include <NCollection_Array1.hxx>
#include <Geom2d_BoundedCurve.hxx>
class gp_Trsf2d;
class Geom2d_Geometry;

namespace Geom2d_EvalRepCurveDesc
{
class Base;
}

//! Describes a BSpline curve.
//! A BSpline curve can be:
//! - uniform or non-uniform,
//! - rational or non-rational,
//! - periodic or non-periodic.
//! A BSpline curve is defined by:
//! - its degree; the degree for a
//! Geom2d_BSplineCurve is limited to a value (25)
//! which is defined and controlled by the system. This
//! value is returned by the function MaxDegree;
//! - its periodic or non-periodic nature;
//! - a table of poles (also called control points), with
//! their associated weights if the BSpline curve is
//! rational. The poles of the curve are "control points"
//! used to deform the curve. If the curve is
//! non-periodic, the first pole is the start point of the
//! curve, and the last pole is the end point of the
//! curve. The segment, which joins the first pole to the
//! second pole, is the tangent to the curve at its start
//! point, and the segment, which joins the last pole to
//! the second-from-last pole, is the tangent to the
//! curve at its end point. If the curve is periodic, these
//! geometric properties are not verified. It is more
//! difficult to give a geometric signification to the
//! weights but they are useful for providing exact
//! representations of the arcs of a circle or ellipse.
//! Moreover, if the weights of all the poles are equal,
//! the curve has a polynomial equation; it is
//! therefore a non-rational curve.
//! - a table of knots with their multiplicities. For a
//! Geom2d_BSplineCurve, the table of knots is an
//! increasing sequence of reals without repetition; the
//! multiplicities define the repetition of the knots. A
//! BSpline curve is a piecewise polynomial or rational
//! curve. The knots are the parameters of junction
//! points between two pieces. The multiplicity
//! Mult(i) of the knot Knot(i) of the BSpline
//! curve is related to the degree of continuity of the
//! curve at the knot Knot(i), which is equal to
//! Degree - Mult(i) where Degree is the
//! degree of the BSpline curve.
//! If the knots are regularly spaced (i.e. the difference
//! between two consecutive knots is a constant), three
//! specific and frequently used cases of knot distribution
//! can be identified:
//! - "uniform" if all multiplicities are equal to 1,
//! - "quasi-uniform" if all multiplicities are equal to 1,
//! except the first and the last knot which have a
//! multiplicity of Degree + 1, where Degree is
//! the degree of the BSpline curve,
//! - "Piecewise Bezier" if all multiplicities are equal to
//! Degree except the first and last knot which have
//! a multiplicity of Degree + 1, where Degree is
//! the degree of the BSpline curve. A curve of this
//! type is a concatenation of arcs of Bezier curves.
//! If the BSpline curve is not periodic:
//! - the bounds of the Poles and Weights tables are 1
//! and NbPoles, where NbPoles is the number of
//! poles of the BSpline curve,
//! - the bounds of the Knots and Multiplicities tables are
//! 1 and NbKnots, where NbKnots is the number
//! of knots of the BSpline curve.
//! If the BSpline curve is periodic, and if there are k
//! periodic knots and p periodic poles, the period is:
//! period = Knot(k + 1) - Knot(1)
//! and the poles and knots tables can be considered as
//! infinite tables, such that:
//! - Knot(i+k) = Knot(i) + period
//! - Pole(i+p) = Pole(i)
//! Note: data structures of a periodic BSpline curve are
//! more complex than those of a non-periodic one.
//! Warnings:
//! In this class we consider that a weight value is zero if
//! Weight <= Resolution from package gp.
//! For two parametric values (or two knot values) U1, U2 we
//! consider that U1 = U2 if Abs (U2 - U1) <= Epsilon (U1).
//! For two weights values W1, W2 we consider that W1 = W2 if
//! Abs (W2 - W1) <= Epsilon (W1). The method Epsilon is
//! defined in the class Real from package Standard.
//!
//! References :
//! . A survey of curve and surface methods in CADG Wolfgang BOHM
//! CAGD 1 (1984)
//! . On de Boor-like algorithms and blossoming Wolfgang BOEHM
//! cagd 5 (1988)
//! . Blossoming and knot insertion algorithms for B-spline curves
//! Ronald N. GOLDMAN
//! . Modelisation des surfaces en CAO, Henri GIAUME Peugeot SA
//! . Curves and Surfaces for Computer Aided Geometric Design,
//! a practical guide Gerald Farin
class Geom2d_BSplineCurve : public Geom2d_BoundedCurve
{

public:
  //! Creates a non-rational B_spline curve on the
  //! basis <Knots, Multiplicities> of degree <Degree>.
  //! The following conditions must be verified.
  //! 0 < Degree <= MaxDegree.
  //!
  //! Knots.Length() == Mults.Length() >= 2
  //!
  //! Knots(i) < Knots(i+1) (Knots are increasing)
  //!
  //! 1 <= Mults(i) <= Degree
  //!
  //! On a non periodic curve the first and last multiplicities
  //! may be Degree+1 (this is even recommended if you want the
  //! curve to start and finish on the first and last pole).
  //!
  //! On a periodic curve the first and the last multicities
  //! must be the same.
  //!
  //! on non-periodic curves
  //!
  //! Poles.Length() == Sum(Mults(i)) - Degree - 1 >= 2
  //!
  //! on periodic curves
  //!
  //! Poles.Length() == Sum(Mults(i)) except the first or last
  Standard_EXPORT Geom2d_BSplineCurve(const NCollection_Array1<gp_Pnt2d>& Poles,
                                      const NCollection_Array1<double>&   Knots,
                                      const NCollection_Array1<int>&      Multiplicities,
                                      const int                           Degree,
                                      const bool                          Periodic = false);

  //! Creates a rational B_spline curve on the basis
  //! <Knots, Multiplicities> of degree <Degree>.
  //! The following conditions must be verified.
  //! 0 < Degree <= MaxDegree.
  //!
  //! Knots.Length() == Mults.Length() >= 2
  //!
  //! Knots(i) < Knots(i+1) (Knots are increasing)
  //!
  //! 1 <= Mults(i) <= Degree
  //!
  //! On a non periodic curve the first and last multiplicities
  //! may be Degree+1 (this is even recommended if you want the
  //! curve to start and finish on the first and last pole).
  //!
  //! On a periodic curve the first and the last multicities
  //! must be the same.
  //!
  //! on non-periodic curves
  //!
  //! Poles.Length() == Sum(Mults(i)) - Degree - 1 >= 2
  //!
  //! on periodic curves
  //!
  //! Poles.Length() == Sum(Mults(i)) except the first or last
  Standard_EXPORT Geom2d_BSplineCurve(const NCollection_Array1<gp_Pnt2d>& Poles,
                                      const NCollection_Array1<double>&   Weights,
                                      const NCollection_Array1<double>&   Knots,
                                      const NCollection_Array1<int>&      Multiplicities,
                                      const int                           Degree,
                                      const bool                          Periodic = false);

  //! Copy constructor for optimized copying without validation.
  Standard_EXPORT Geom2d_BSplineCurve(const Geom2d_BSplineCurve& theOther);

  //! Returns true if an evaluation representation is attached.
  bool HasEvalRepresentation() const { return !myEvalRep.IsNull(); }

  //! Returns the current evaluation representation descriptor (may be null).
  const occ::handle<Geom2d_EvalRepCurveDesc::Base>& EvalRepresentation() const { return myEvalRep; }

  //! Sets a new evaluation representation.
  //! Validates descriptor data and ensures no circular references.
  Standard_EXPORT void SetEvalRepresentation(
    const occ::handle<Geom2d_EvalRepCurveDesc::Base>& theDesc);

  //! Removes the evaluation representation.
  void ClearEvalRepresentation() { myEvalRep.Nullify(); }

  //! Increases the degree of this BSpline curve to
  //! Degree. As a result, the poles, weights and
  //! multiplicities tables are modified; the knots table is
  //! not changed. Nothing is done if Degree is less than
  //! or equal to the current degree.
  //! Exceptions
  //! Standard_ConstructionError if Degree is greater than
  //! Geom2d_BSplineCurve::MaxDegree().
  Standard_EXPORT void IncreaseDegree(const int Degree);

  //! Increases the multiplicity of the knot <Index> to
  //! <M>.
  //!
  //! If <M> is lower or equal to the current multiplicity
  //! nothing is done. If <M> is higher than the degree,
  //! the degree is used.
  //! If <Index> is not in [FirstUKnotIndex, LastUKnotIndex]
  Standard_EXPORT void IncreaseMultiplicity(const int Index, const int M);

  //! Increases the multiplicities of the knots in
  //! [I1,I2] to <M>.
  //!
  //! For each knot if <M> is lower or equal to the
  //! current multiplicity nothing is done. If <M> is
  //! higher than the degree the degree is used.
  //! As a result, the poles and weights tables of this curve are modified.
  //! Warning
  //! It is forbidden to modify the multiplicity of the first or
  //! last knot of a non-periodic curve. Be careful as
  //! Geom2d does not protect against this.
  //! Exceptions
  //! Standard_OutOfRange if either Index, I1 or I2 is
  //! outside the bounds of the knots table.
  Standard_EXPORT void IncreaseMultiplicity(const int I1, const int I2, const int M);

  //! Increases by M the multiplicity of the knots of indexes
  //! I1 to I2 in the knots table of this BSpline curve. For
  //! each knot, the resulting multiplicity is limited to the
  //! degree of this curve. If M is negative, nothing is done.
  //! As a result, the poles and weights tables of this
  //! BSpline curve are modified.
  //! Warning
  //! It is forbidden to modify the multiplicity of the first or
  //! last knot of a non-periodic curve. Be careful as
  //! Geom2d does not protect against this.
  //! Exceptions
  //! Standard_OutOfRange if I1 or I2 is outside the
  //! bounds of the knots table.
  Standard_EXPORT void IncrementMultiplicity(const int I1, const int I2, const int M);

  //! Inserts a knot value in the sequence of knots. If
  //! <U> is an existing knot the multiplicity is
  //! increased by <M>.
  //!
  //! If U is not on the parameter range nothing is
  //! done.
  //!
  //! If the multiplicity is negative or null nothing is
  //! done. The new multiplicity is limited to the
  //! degree.
  //!
  //! The tolerance criterion for knots equality is
  //! the max of Epsilon(U) and ParametricTolerance.
  //! Warning
  //! - If U is less than the first parameter or greater than
  //! the last parameter of this BSpline curve, nothing is done.
  //! - If M is negative or null, nothing is done.
  //! - The multiplicity of a knot is limited to the degree of
  //! this BSpline curve.
  Standard_EXPORT void InsertKnot(const double U,
                                  const int    M                   = 1,
                                  const double ParametricTolerance = 0.0);

  //! Inserts the values of the array Knots, with the
  //! respective multiplicities given by the array Mults, into
  //! the knots table of this BSpline curve.
  //! If a value of the array Knots is an existing knot, its multiplicity is:
  //! - increased by M, if Add is true, or
  //! - increased to M, if Add is false (default value).
  //! The tolerance criterion used for knot equality is the
  //! larger of the values ParametricTolerance (defaulted
  //! to 0.) and double::Epsilon(U),
  //! where U is the current knot value.
  //! Warning
  //! - For a value of the array Knots which is less than
  //! the first parameter or greater than the last
  //! parameter of this BSpline curve, nothing is done.
  //! - For a value of the array Mults which is negative or
  //! null, nothing is done.
  //! - The multiplicity of a knot is limited to the degree of
  //! this BSpline curve.
  Standard_EXPORT void InsertKnots(const NCollection_Array1<double>& Knots,
                                   const NCollection_Array1<int>&    Mults,
                                   const double                      ParametricTolerance = 0.0,
                                   const bool                        Add                 = false);

  //! Reduces the multiplicity of the knot of index Index
  //! to M. If M is equal to 0, the knot is removed.
  //! With a modification of this type, the array of poles is also modified.
  //! Two different algorithms are systematically used to
  //! compute the new poles of the curve. If, for each
  //! pole, the distance between the pole calculated
  //! using the first algorithm and the same pole
  //! calculated using the second algorithm, is less than
  //! Tolerance, this ensures that the curve is not
  //! modified by more than Tolerance. Under these
  //! conditions, true is returned; otherwise, false is returned.
  //! A low tolerance is used to prevent modification of
  //! the curve. A high tolerance is used to "smooth" the curve.
  //! Exceptions
  //! Standard_OutOfRange if Index is outside the
  //! bounds of the knots table.
  Standard_EXPORT bool RemoveKnot(const int Index, const int M, const double Tolerance);

  //! The new pole is inserted after the pole of range Index.
  //! If the curve was non rational it can become rational.
  //!
  //! Raised if the B-spline is NonUniform or PiecewiseBezier or if
  //! Weight <= 0.0
  //! Raised if Index is not in the range [1, Number of Poles]
  Standard_EXPORT void InsertPoleAfter(const int       Index,
                                       const gp_Pnt2d& P,
                                       const double    Weight = 1.0);

  //! The new pole is inserted before the pole of range Index.
  //! If the curve was non rational it can become rational.
  //!
  //! Raised if the B-spline is NonUniform or PiecewiseBezier or if
  //! Weight <= 0.0
  //! Raised if Index is not in the range [1, Number of Poles]
  Standard_EXPORT void InsertPoleBefore(const int       Index,
                                        const gp_Pnt2d& P,
                                        const double    Weight = 1.0);

  //! Removes the pole of range Index
  //! If the curve was rational it can become non rational.
  //!
  //! Raised if the B-spline is NonUniform or PiecewiseBezier.
  //! Raised if the number of poles of the B-spline curve is lower or
  //! equal to 2 before removing.
  //! Raised if Index is not in the range [1, Number of Poles]
  Standard_EXPORT void RemovePole(const int Index);

  //! Reverses the orientation of this BSpline curve. As a result
  //! - the knots and poles tables are modified;
  //! - the start point of the initial curve becomes the end
  //! point of the reversed curve;
  //! - the end point of the initial curve becomes the start
  //! point of the reversed curve.
  Standard_EXPORT void Reverse() final;

  //! Computes the parameter on the reversed curve for
  //! the point of parameter U on this BSpline curve.
  //! The returned value is: UFirst + ULast - U,
  //! where UFirst and ULast are the values of the
  //! first and last parameters of this BSpline curve.
  Standard_EXPORT double ReversedParameter(const double U) const final;

  //! Modifies this BSpline curve by segmenting it
  //! between U1 and U2. Either of these values can be
  //! outside the bounds of the curve, but U2 must be greater than U1.
  //! All data structure tables of this BSpline curve are
  //! modified, but the knots located between U1 and U2
  //! are retained. The degree of the curve is not modified.
  //!
  //! Parameter theTolerance defines the possible proximity of the segment
  //! boundaries and B-spline knots to treat them as equal.
  //!
  //! Warnings:
  //! Even if <me> is not closed it can become closed after the
  //! segmentation for example if U1 or U2 are out of the bounds
  //! of the curve <me> or if the curve makes loop.
  //! After the segmentation the length of a curve can be null.
  //! - The segmentation of a periodic curve over an
  //! interval corresponding to its period generates a
  //! non-periodic curve with equivalent geometry.
  //! Exceptions
  //! Standard_DomainError if U2 is less than U1.
  //! raises if U2 < U1.
  //! Standard_DomainError if U2 - U1 exceeds the period for periodic curves.
  //! i.e. ((U2 - U1) - Period) > Precision::PConfusion().
  Standard_EXPORT void Segment(const double U1,
                               const double U2,
                               const double theTolerance = Precision::PConfusion());

  //! Modifies this BSpline curve by assigning the value K
  //! to the knot of index Index in the knots table. This is a
  //! relatively local modification because K must be such that:
  //! Knots(Index - 1) < K < Knots(Index + 1)
  //! Exceptions
  //! Standard_ConstructionError if:
  //! - K is not such that:
  //! Knots(Index - 1) < K < Knots(Index + 1)
  //! - M is greater than the degree of this BSpline curve
  //! or lower than the previous multiplicity of knot of
  //! index Index in the knots table.
  //! Standard_OutOfRange if Index is outside the bounds of the knots table.
  Standard_EXPORT void SetKnot(const int Index, const double K);

  //! Modifies this BSpline curve by assigning the array
  //! K to its knots table. The multiplicity of the knots is not modified.
  //! Exceptions
  //! Standard_ConstructionError if the values in the
  //! array K are not in ascending order.
  //! Standard_OutOfRange if the bounds of the array
  //! K are not respectively 1 and the number of knots of this BSpline curve.
  Standard_EXPORT void SetKnots(const NCollection_Array1<double>& K);

  //! Modifies this BSpline curve by assigning the value K
  //! to the knot of index Index in the knots table. This is a
  //! relatively local modification because K must be such that:
  //! Knots(Index - 1) < K < Knots(Index + 1)
  //! The second syntax allows you also to increase the
  //! multiplicity of the knot to M (but it is not possible to
  //! decrease the multiplicity of the knot with this function).
  //! Exceptions
  //! Standard_ConstructionError if:
  //! - K is not such that:
  //! Knots(Index - 1) < K < Knots(Index + 1)
  //! - M is greater than the degree of this BSpline curve
  //! or lower than the previous multiplicity of knot of
  //! index Index in the knots table.
  //! Standard_OutOfRange if Index is outside the bounds of the knots table.
  Standard_EXPORT void SetKnot(const int Index, const double K, const int M);

  //! Computes the parameter normalized within the
  //! "first" period of this BSpline curve, if it is periodic:
  //! the returned value is in the range Param1 and
  //! Param1 + Period, where:
  //! - Param1 is the "first parameter", and
  //! - Period the period of this BSpline curve.
  //! Note: If this curve is not periodic, U is not modified.
  Standard_EXPORT void PeriodicNormalization(double& U) const;

  //! Changes this BSpline curve into a periodic curve.
  //! To become periodic, the curve must first be closed.
  //! Next, the knot sequence must be periodic. For this,
  //! FirstUKnotIndex and LastUKnotIndex are used to
  //! compute I1 and I2, the indexes in the knots array
  //! of the knots corresponding to the first and last
  //! parameters of this BSpline curve.
  //! The period is therefore Knot(I2) - Knot(I1).
  //! Consequently, the knots and poles tables are modified.
  //! Exceptions
  //! Standard_ConstructionError if this BSpline curve is not closed.
  Standard_EXPORT void SetPeriodic();

  //! Assigns the knot of index Index in the knots table as
  //! the origin of this periodic BSpline curve. As a
  //! consequence, the knots and poles tables are modified.
  //! Exceptions
  //! Standard_NoSuchObject if this curve is not periodic.
  //! Standard_DomainError if Index is outside the
  //! bounds of the knots table.
  Standard_EXPORT void SetOrigin(const int Index);

  //! Changes this BSpline curve into a non-periodic
  //! curve. If this curve is already non-periodic, it is not modified.
  //! Note that the poles and knots tables are modified.
  //! Warning
  //! If this curve is periodic, as the multiplicity of the first
  //! and last knots is not modified, and is not equal to
  //! Degree + 1, where Degree is the degree of
  //! this BSpline curve, the start and end points of the
  //! curve are not its first and last poles.
  Standard_EXPORT void SetNotPeriodic();

  //! Modifies this BSpline curve by assigning P to the
  //! pole of index Index in the poles table.
  //! Exceptions
  //! Standard_OutOfRange if Index is outside the
  //! bounds of the poles table.
  //! Standard_ConstructionError if Weight is negative or null.
  Standard_EXPORT void SetPole(const int Index, const gp_Pnt2d& P);

  //! Modifies this BSpline curve by assigning P to the
  //! pole of index Index in the poles table.
  //! The second syntax also allows you to modify the
  //! weight of the modified pole, which becomes Weight.
  //! In this case, if this BSpline curve is non-rational, it
  //! can become rational and vice versa.
  //! Exceptions
  //! Standard_OutOfRange if Index is outside the
  //! bounds of the poles table.
  //! Standard_ConstructionError if Weight is negative or null.
  Standard_EXPORT void SetPole(const int Index, const gp_Pnt2d& P, const double Weight);

  //! Assigns the weight Weight to the pole of index Index of the poles table.
  //! If the curve was non rational it can become rational.
  //! If the curve was rational it can become non rational.
  //! Exceptions
  //! Standard_OutOfRange if Index is outside the
  //! bounds of the poles table.
  //! Standard_ConstructionError if Weight is negative or null.
  Standard_EXPORT void SetWeight(const int Index, const double Weight);

  //! Moves the point of parameter U of this BSpline
  //! curve to P. Index1 and Index2 are the indexes in the
  //! table of poles of this BSpline curve of the first and
  //! last poles designated to be moved.
  //! FirstModifiedPole and LastModifiedPole are the
  //! indexes of the first and last poles, which are
  //! effectively modified.
  //! In the event of incompatibility between Index1,
  //! Index2 and the value U:
  //! - no change is made to this BSpline curve, and
  //! - the FirstModifiedPole and LastModifiedPole are returned null.
  //! Exceptions
  //! Standard_OutOfRange if:
  //! - Index1 is greater than or equal to Index2, or
  //! - Index1 or Index2 is less than 1 or greater than the
  //! number of poles of this BSpline curve.
  Standard_EXPORT void MovePoint(const double    U,
                                 const gp_Pnt2d& P,
                                 const int       Index1,
                                 const int       Index2,
                                 int&            FirstModifiedPole,
                                 int&            LastModifiedPole);

  //! Move a point with parameter U to P.
  //! and makes it tangent at U be Tangent.
  //! StartingCondition = -1 means first can move
  //! EndingCondition   = -1 means last point can move
  //! StartingCondition = 0 means the first point cannot move
  //! EndingCondition   = 0 means the last point cannot move
  //! StartingCondition = 1 means the first point and tangent cannot move
  //! EndingCondition   = 1 means the last point and tangent cannot move
  //! and so forth
  //! ErrorStatus != 0 means that there are not enough degree of freedom
  //! with the constrain to deform the curve accordingly
  Standard_EXPORT void MovePointAndTangent(const double    U,
                                           const gp_Pnt2d& P,
                                           const gp_Vec2d& Tangent,
                                           const double    Tolerance,
                                           const int       StartingCondition,
                                           const int       EndingCondition,
                                           int&            ErrorStatus);

  //! Returns true if the degree of continuity of this
  //! BSpline curve is at least N. A BSpline curve is at least GeomAbs_C0.
  //! Exceptions Standard_RangeError if N is negative.
  Standard_EXPORT bool IsCN(const int N) const final;

  //! Check if curve has at least G1 continuity in interval [theTf, theTl]
  //! Returns true if IsCN(1)
  //! or
  //! angle between "left" and "right" first derivatives at
  //! knots with C0 continuity is less then theAngTol
  //! only knots in interval [theTf, theTl] is checked
  Standard_EXPORT bool IsG1(const double theTf, const double theTl, const double theAngTol) const;

  //! Returns true if the distance between the first point and the
  //! last point of the curve is lower or equal to Resolution
  //! from package gp.
  //! Warnings :
  //! The first and the last point can be different from the first
  //! pole and the last pole of the curve.
  Standard_EXPORT bool IsClosed() const final;

  //! Returns True if the curve is periodic.
  Standard_EXPORT bool IsPeriodic() const final;

  //! Returns True if the weights are not identical.
  //! The tolerance criterion is Epsilon of the class Real.
  Standard_EXPORT bool IsRational() const;

  //! Returns the global continuity of the curve :
  //! C0 : only geometric continuity,
  //! C1 : continuity of the first derivative all along the Curve,
  //! C2 : continuity of the second derivative all along the Curve,
  //! C3 : continuity of the third derivative all along the Curve,
  //! CN : the order of continuity is infinite.
  //! For a B-spline curve of degree d if a knot Ui has a
  //! multiplicity p the B-spline curve is only Cd-p continuous
  //! at Ui. So the global continuity of the curve can't be greater
  //! than Cd-p where p is the maximum multiplicity of the interior
  //! Knots. In the interior of a knot span the curve is infinitely
  //! continuously differentiable.
  Standard_EXPORT GeomAbs_Shape Continuity() const final;

  //! Returns the degree of this BSpline curve.
  //! In this class the degree of the basis normalized B-spline
  //! functions cannot be greater than "MaxDegree"
  //! Computation of value and derivatives
  Standard_EXPORT int Degree() const;

  Standard_EXPORT gp_Pnt2d EvalD0(const double U) const final;

  //! Raised if the continuity of the curve is not C1.
  Standard_EXPORT Geom2d_Curve::ResD1 EvalD1(const double U) const final;

  //! Raised if the continuity of the curve is not C2.
  Standard_EXPORT Geom2d_Curve::ResD2 EvalD2(const double U) const final;

  //! For this BSpline curve, computes
  //! - the point P of parameter U, or
  //! - the point P and one or more of the following values:
  //! - V1, the first derivative vector,
  //! - V2, the second derivative vector,
  //! - V3, the third derivative vector.
  //! Warning
  //! On a point where the continuity of the curve is not the
  //! one requested, these functions impact the part
  //! defined by the parameter with a value greater than U,
  //! i.e. the part of the curve to the "right" of the singularity.
  //! Raises UndefinedDerivative if the continuity of the curve is not C3.
  Standard_EXPORT Geom2d_Curve::ResD3 EvalD3(const double U) const final;

  //! For the point of parameter U of this BSpline curve,
  //! computes the vector corresponding to the Nth derivative.
  //! Warning
  //! On a point where the continuity of the curve is not the
  //! one requested, this function impacts the part defined
  //! by the parameter with a value greater than U, i.e. the
  //! part of the curve to the "right" of the singularity.
  //! Raises UndefinedDerivative if the continuity of the curve is not CN.
  //! RangeError if N < 1.
  //! The following functions computes the point of parameter U
  //! and the derivatives at this point on the B-spline curve
  //! arc defined between the knot FromK1 and the knot ToK2.
  //! U can be out of bounds [Knot (FromK1), Knot (ToK2)] but
  //! for the computation we only use the definition of the curve
  //! between these two knots. This method is useful to compute
  //! local derivative, if the order of continuity of the whole
  //! curve is not greater enough. Inside the parametric
  //! domain Knot (FromK1), Knot (ToK2) the evaluations are
  //! the same as if we consider the whole definition of the
  //! curve. Of course the evaluations are different outside
  //! this parametric domain.
  Standard_EXPORT gp_Vec2d EvalDN(const double U, const int N) const final;

  //! Raised if FromK1 = ToK2.
  Standard_EXPORT gp_Pnt2d LocalValue(const double U, const int FromK1, const int ToK2) const;

  //! Raised if FromK1 = ToK2.
  Standard_EXPORT void LocalD0(const double U, const int FromK1, const int ToK2, gp_Pnt2d& P) const;

  //! Raised if the local continuity of the curve is not C1
  //! between the knot K1 and the knot K2.
  //! Raised if FromK1 = ToK2.
  Standard_EXPORT void LocalD1(const double U,
                               const int    FromK1,
                               const int    ToK2,
                               gp_Pnt2d&    P,
                               gp_Vec2d&    V1) const;

  //! Raised if the local continuity of the curve is not C2
  //! between the knot K1 and the knot K2.
  //! Raised if FromK1 = ToK2.
  Standard_EXPORT void LocalD2(const double U,
                               const int    FromK1,
                               const int    ToK2,
                               gp_Pnt2d&    P,
                               gp_Vec2d&    V1,
                               gp_Vec2d&    V2) const;

  //! Raised if the local continuity of the curve is not C3
  //! between the knot K1 and the knot K2.
  //! Raised if FromK1 = ToK2.
  Standard_EXPORT void LocalD3(const double U,
                               const int    FromK1,
                               const int    ToK2,
                               gp_Pnt2d&    P,
                               gp_Vec2d&    V1,
                               gp_Vec2d&    V2,
                               gp_Vec2d&    V3) const;

  //! Raised if the local continuity of the curve is not CN
  //! between the knot K1 and the knot K2.
  //! Raised if FromK1 = ToK2.
  //! Raised if N < 1.
  Standard_EXPORT gp_Vec2d LocalDN(const double U,
                                   const int    FromK1,
                                   const int    ToK2,
                                   const int    N) const;

  //! Returns the last point of the curve.
  //! Warnings :
  //! The last point of the curve is different from the last
  //! pole of the curve if the multiplicity of the last knot
  //! is lower than Degree.
  Standard_EXPORT gp_Pnt2d EndPoint() const final;

  //! For a B-spline curve the first parameter (which gives the start
  //! point of the curve) is a knot value but if the multiplicity of
  //! the first knot index is lower than Degree + 1 it is not the
  //! first knot of the curve. This method computes the index of the
  //! knot corresponding to the first parameter.
  Standard_EXPORT int FirstUKnotIndex() const;

  //! Computes the parametric value of the start point of the curve.
  //! It is a knot value.
  Standard_EXPORT double FirstParameter() const final;

  //! Returns the knot of range Index. When there is a knot
  //! with a multiplicity greater than 1 the knot is not repeated.
  //! The method Multiplicity can be used to get the multiplicity
  //! of the Knot.
  //! Raised if Index < 1 or Index > NbKnots
  Standard_EXPORT double Knot(const int Index) const;

  //! returns the knot values of the B-spline curve;
  //!
  //! Raised K.Lower() is less than number of first knot or
  //! K.Upper() is more than number of last knot.
  Standard_DEPRECATED("use Knots() returning const reference instead")
  Standard_EXPORT void Knots(NCollection_Array1<double>& K) const;

  //! returns the knot values of the B-spline curve;
  Standard_EXPORT const NCollection_Array1<double>& Knots() const;

  //! Returns the knots sequence.
  //! In this sequence the knots with a multiplicity greater than 1
  //! are repeated.
  //! Example :
  //! K = {k1, k1, k1, k2, k3, k3, k4, k4, k4}
  //!
  //! Raised if K.Lower() is less than number of first knot
  //! in knot sequence with repetitions or K.Upper() is more
  //! than number of last knot in knot sequence with repetitions.
  Standard_DEPRECATED("use KnotSequence() returning const reference instead")
  Standard_EXPORT void KnotSequence(NCollection_Array1<double>& K) const;

  //! Returns the knots sequence.
  //! In this sequence the knots with a multiplicity greater than 1
  //! are repeated.
  //! Example :
  //! K = {k1, k1, k1, k2, k3, k3, k4, k4, k4}
  Standard_EXPORT const NCollection_Array1<double>& KnotSequence() const;

  //! Returns NonUniform or Uniform or QuasiUniform or PiecewiseBezier.
  //! If all the knots differ by a positive constant from the
  //! preceding knot the BSpline Curve can be :
  //! - Uniform if all the knots are of multiplicity 1,
  //! - QuasiUniform if all the knots are of multiplicity 1 except for
  //! the first and last knot which are of multiplicity Degree + 1,
  //! - PiecewiseBezier if the first and last knots have multiplicity
  //! Degree + 1 and if interior knots have multiplicity Degree
  //! A piecewise Bezier with only two knots is a BezierCurve.
  //! else the curve is non uniform.
  //! The tolerance criterion is Epsilon from class Real.
  Standard_EXPORT GeomAbs_BSplKnotDistribution KnotDistribution() const;

  //! For a BSpline curve the last parameter (which gives the
  //! end point of the curve) is a knot value but if the
  //! multiplicity of the last knot index is lower than
  //! Degree + 1 it is not the last knot of the curve. This
  //! method computes the index of the knot corresponding to
  //! the last parameter.
  Standard_EXPORT int LastUKnotIndex() const;

  //! Computes the parametric value of the end point of the curve.
  //! It is a knot value.
  Standard_EXPORT double LastParameter() const final;

  //! Locates the parametric value U in the sequence of knots.
  //! If "WithKnotRepetition" is True we consider the knot's
  //! representation with repetition of multiple knot value,
  //! otherwise we consider the knot's representation with
  //! no repetition of multiple knot values.
  //! Knots (I1) <= U <= Knots (I2)
  //! . if I1 = I2  U is a knot value (the tolerance criterion
  //! ParametricTolerance is used).
  //! . if I1 < 1  => U < Knots (1) - std::abs(ParametricTolerance)
  //! . if I2 > NbKnots => U > Knots (NbKnots) + std::abs(ParametricTolerance)
  Standard_EXPORT void LocateU(const double U,
                               const double ParametricTolerance,
                               int&         I1,
                               int&         I2,
                               const bool   WithKnotRepetition = false) const;

  //! Returns the multiplicity of the knots of range Index.
  //! Raised if Index < 1 or Index > NbKnots
  Standard_EXPORT int Multiplicity(const int Index) const;

  //! Returns the multiplicity of the knots of the curve.
  //!
  //! Raised if the length of M is not equal to NbKnots.
  Standard_DEPRECATED("use Multiplicities() returning const reference instead")
  Standard_EXPORT void Multiplicities(NCollection_Array1<int>& M) const;

  //! returns the multiplicity of the knots of the curve.
  Standard_EXPORT const NCollection_Array1<int>& Multiplicities() const;

  //! Returns the number of knots. This method returns the number of
  //! knot without repetition of multiple knots.
  Standard_EXPORT int NbKnots() const;

  //! Returns the number of poles
  Standard_EXPORT int NbPoles() const;

  //! Returns the pole of range Index.
  //! Raised if Index < 1 or Index > NbPoles.
  Standard_EXPORT const gp_Pnt2d& Pole(const int Index) const;

  //! Returns the poles of the B-spline curve;
  //!
  //! Raised if the length of P is not equal to the number of poles.
  Standard_DEPRECATED("use Poles() returning const reference instead")
  Standard_EXPORT void Poles(NCollection_Array1<gp_Pnt2d>& P) const;

  //! Returns the poles of the B-spline curve;
  Standard_EXPORT const NCollection_Array1<gp_Pnt2d>& Poles() const;

  //! Returns the start point of the curve.
  //! Warnings :
  //! This point is different from the first pole of the curve if the
  //! multiplicity of the first knot is lower than Degree.
  Standard_EXPORT gp_Pnt2d StartPoint() const final;

  //! Returns the weight of the pole of range Index .
  //! Raised if Index < 1 or Index > NbPoles.
  Standard_EXPORT double Weight(const int Index) const;

  //! Returns the weights of the B-spline curve;
  //!
  //! Raised if the length of W is not equal to NbPoles.
  Standard_DEPRECATED("use Weights() returning const pointer instead")
  Standard_EXPORT void Weights(NCollection_Array1<double>& W) const;

  //! Returns the weights of the B-spline curve;
  Standard_EXPORT const NCollection_Array1<double>* Weights() const;

  //! Returns a const reference to the weights array.
  //! For rational curves: the internal owning weights array.
  //! For non-rational curves: a non-owning view of unit weights from BSplCLib.
  //! The array is always sized to match NbPoles().
  //! @warning Do NOT modify elements through the returned reference.
  const NCollection_Array1<double>& WeightsArray() const { return myWeights; }

  //! Applies the transformation T to this BSpline curve.
  Standard_EXPORT void Transform(const gp_Trsf2d& T) final;

  //! Returns the value of the maximum degree of the normalized
  //! B-spline basis functions in this package.
  Standard_EXPORT static int MaxDegree();

  //! Computes for this BSpline curve the parametric
  //! tolerance UTolerance for a given tolerance
  //! Tolerance3D (relative to dimensions in the plane).
  //! If f(t) is the equation of this BSpline curve,
  //! UTolerance ensures that:
  //! | t1 - t0| < Utolerance ===>
  //! |f(t1) - f(t0)| < ToleranceUV
  Standard_EXPORT void Resolution(const double ToleranceUV, double& UTolerance);

  //! Creates a new object which is a copy of this BSpline curve.
  Standard_EXPORT occ::handle<Geom2d_Geometry> Copy() const final;

  //! Dumps the content of me into the stream
  Standard_EXPORT void DumpJson(Standard_OStream& theOStream, int theDepth = -1) const final;

  DEFINE_STANDARD_RTTIEXT(Geom2d_BSplineCurve, Geom2d_BoundedCurve)

protected:
  //! Recompute the flatknots, the knotsdistribution, the continuity.
  void updateKnots();

private:
  NCollection_Array1<gp_Pnt2d>               myPoles;
  NCollection_Array1<double>                 myWeights;
  NCollection_Array1<double>                 myKnots;
  NCollection_Array1<double>                 myFlatKnots;
  NCollection_Array1<int>                    myMults;
  occ::handle<Geom2d_EvalRepCurveDesc::Base> myEvalRep;
  int                                        myDeg           = 0;
  bool                                       myPeriodic      = false;
  bool                                       myRational      = false;
  GeomAbs_BSplKnotDistribution               myKnotSet       = GeomAbs_NonUniform;
  GeomAbs_Shape                              mySmooth        = GeomAbs_C0;
  double                                     myMaxDerivInv   = 0.0;
  bool                                       myMaxDerivInvOk = false;
};

#endif // _Geom2d_BSplineCurve_HeaderFile
