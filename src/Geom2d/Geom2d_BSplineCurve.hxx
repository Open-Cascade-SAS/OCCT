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
#include <Standard_Integer.hxx>
#include <TColgp_HArray1OfPnt2d.hxx>
#include <TColStd_HArray1OfReal.hxx>
#include <TColStd_HArray1OfInteger.hxx>
#include <Geom2d_BoundedCurve.hxx>
#include <TColgp_Array1OfPnt2d.hxx>
#include <TColStd_Array1OfReal.hxx>
#include <TColStd_Array1OfInteger.hxx>
class gp_Pnt2d;
class gp_Vec2d;
class gp_Trsf2d;
class Geom2d_Geometry;


class Geom2d_BSplineCurve;
DEFINE_STANDARD_HANDLE(Geom2d_BSplineCurve, Geom2d_BoundedCurve)

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
//! Warnings :
//! In this class we consider that a weight value is zero if
//! Weight <= Resolution from package gp.
//! For two parametric values (or two knot values) U1, U2 we
//! consider that U1 = U2 if Abs (U2 - U1) <= Epsilon (U1).
//! For two weights values W1, W2 we consider that W1 = W2 if
//! Abs (W2 - W1) <= Epsilon (W1).  The method Epsilon is
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

  
  //! Creates a  non-rational B_spline curve   on  the
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
  //! On a periodic  curve the first  and  the last multicities
  //! must be the same.
  //!
  //! on non-periodic curves
  //!
  //! Poles.Length() == Sum(Mults(i)) - Degree - 1 >= 2
  //!
  //! on periodic curves
  //!
  //! Poles.Length() == Sum(Mults(i)) except the first or last
  Standard_EXPORT Geom2d_BSplineCurve(const TColgp_Array1OfPnt2d& Poles, const TColStd_Array1OfReal& Knots, const TColStd_Array1OfInteger& Multiplicities, const Standard_Integer Degree, const Standard_Boolean Periodic = Standard_False);
  
  //! Creates  a rational B_spline  curve  on the basis
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
  //! On a periodic  curve the first  and  the last multicities
  //! must be the same.
  //!
  //! on non-periodic curves
  //!
  //! Poles.Length() == Sum(Mults(i)) - Degree - 1 >= 2
  //!
  //! on periodic curves
  //!
  //! Poles.Length() == Sum(Mults(i)) except the first or last
  Standard_EXPORT Geom2d_BSplineCurve(const TColgp_Array1OfPnt2d& Poles, const TColStd_Array1OfReal& Weights, const TColStd_Array1OfReal& Knots, const TColStd_Array1OfInteger& Multiplicities, const Standard_Integer Degree, const Standard_Boolean Periodic = Standard_False);
  
  //! Increases the degree of this BSpline curve to
  //! Degree. As a result, the poles, weights and
  //! multiplicities tables are modified; the knots table is
  //! not changed. Nothing is done if Degree is less than
  //! or equal to the current degree.
  //! Exceptions
  //! Standard_ConstructionError if Degree is greater than
  //! Geom2d_BSplineCurve::MaxDegree().
  Standard_EXPORT void IncreaseDegree (const Standard_Integer Degree);
  
  //! Increases the multiplicity  of the knot <Index> to
  //! <M>.
  //!
  //! If   <M>   is   lower   or  equal   to  the current
  //! multiplicity nothing is done. If <M> is higher than
  //! the degree the degree is used.
  //! If <Index> is not in [FirstUKnotIndex, LastUKnotIndex]
  Standard_EXPORT void IncreaseMultiplicity (const Standard_Integer Index, const Standard_Integer M);
  
  //! Increases  the  multiplicities   of  the knots  in
  //! [I1,I2] to <M>.
  //!
  //! For each knot if  <M>  is  lower  or equal  to  the
  //! current multiplicity  nothing  is  done. If <M>  is
  //! higher than the degree the degree is used.
  //! As a result, the poles and weights tables of this curve are modified.
  //! Warning
  //! It is forbidden to modify the multiplicity of the first or
  //! last knot of a non-periodic curve. Be careful as
  //! Geom2d does not protect against this.
  //! Exceptions
  //! Standard_OutOfRange if either Index, I1 or I2 is
  //! outside the bounds of the knots table.
  Standard_EXPORT void IncreaseMultiplicity (const Standard_Integer I1, const Standard_Integer I2, const Standard_Integer M);
  
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
  Standard_EXPORT void IncrementMultiplicity (const Standard_Integer I1, const Standard_Integer I2, const Standard_Integer M);
  
  //! Inserts a knot value in the sequence of knots.  If
  //! <U>  is an  existing knot     the multiplicity  is
  //! increased by <M>.
  //!
  //! If U  is  not  on the parameter  range  nothing is
  //! done.
  //!
  //! If the multiplicity is negative or null nothing is
  //! done. The  new   multiplicity  is limited  to  the
  //! degree.
  //!
  //! The  tolerance criterion  for  knots  equality  is
  //! the max of Epsilon(U) and ParametricTolerance.
  //! Warning
  //! - If U is less than the first parameter or greater than
  //! the last parameter of this BSpline curve, nothing is done.
  //! - If M is negative or null, nothing is done.
  //! - The multiplicity of a knot is limited to the degree of
  //! this BSpline curve.
  Standard_EXPORT void InsertKnot (const Standard_Real U, const Standard_Integer M = 1, const Standard_Real ParametricTolerance = 0.0);
  
  //! Inserts the values of the array Knots, with the
  //! respective multiplicities given by the array Mults, into
  //! the knots table of this BSpline curve.
  //! If a value of the array Knots is an existing knot, its multiplicity is:
  //! - increased by M, if Add is true, or
  //! - increased to M, if Add is false (default value).
  //! The tolerance criterion used for knot equality is the
  //! larger of the values ParametricTolerance (defaulted
  //! to 0.) and Standard_Real::Epsilon(U),
  //! where U is the current knot value.
  //! Warning
  //! - For a value of the array Knots which is less than
  //! the first parameter or greater than the last
  //! parameter of this BSpline curve, nothing is done.
  //! - For a value of the array Mults which is negative or
  //! null, nothing is done.
  //! - The multiplicity of a knot is limited to the degree of
  //! this BSpline curve.
  Standard_EXPORT void InsertKnots (const TColStd_Array1OfReal& Knots, const TColStd_Array1OfInteger& Mults, const Standard_Real ParametricTolerance = 0.0, const Standard_Boolean Add = Standard_False);
  
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
  Standard_EXPORT Standard_Boolean RemoveKnot (const Standard_Integer Index, const Standard_Integer M, const Standard_Real Tolerance);
  

  //! The new pole is inserted after the pole of range Index.
  //! If the curve was non rational it can become rational.
  //!
  //! Raised if the B-spline is NonUniform or PiecewiseBezier or if
  //! Weight <= 0.0
  //! Raised if Index is not in the range [1, Number of Poles]
  Standard_EXPORT void InsertPoleAfter (const Standard_Integer Index, const gp_Pnt2d& P, const Standard_Real Weight = 1.0);
  

  //! The new pole is inserted before the pole of range Index.
  //! If the curve was non rational it can become rational.
  //!
  //! Raised if the B-spline is NonUniform or PiecewiseBezier or if
  //! Weight <= 0.0
  //! Raised if Index is not in the range [1, Number of Poles]
  Standard_EXPORT void InsertPoleBefore (const Standard_Integer Index, const gp_Pnt2d& P, const Standard_Real Weight = 1.0);
  

  //! Removes the pole of range Index
  //! If the curve was rational it can become non rational.
  //!
  //! Raised if the B-spline is NonUniform or PiecewiseBezier.
  //! Raised if the number of poles of the B-spline curve is lower or
  //! equal to 2 before removing.
  //! Raised if Index is not in the range [1, Number of Poles]
  Standard_EXPORT void RemovePole (const Standard_Integer Index);
  
  //! Reverses the orientation of this BSpline curve. As a result
  //! - the knots and poles tables are modified;
  //! - the start point of the initial curve becomes the end
  //! point of the reversed curve;
  //! - the end point of the initial curve becomes the start
  //! point of the reversed curve.
  Standard_EXPORT void Reverse() Standard_OVERRIDE;
  
  //! Computes the parameter on the reversed curve for
  //! the point of parameter U on this BSpline curve.
  //! The returned value is: UFirst + ULast - U,
  //! where UFirst and ULast are the values of the
  //! first and last parameters of this BSpline curve.
  Standard_EXPORT Standard_Real ReversedParameter (const Standard_Real U) const Standard_OVERRIDE;
  
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
  //! Warnings :
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
  Standard_EXPORT void Segment (const Standard_Real U1, const Standard_Real U2,
                                const Standard_Real theTolerance = Precision::PConfusion());
  
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
  Standard_EXPORT void SetKnot (const Standard_Integer Index, const Standard_Real K);
  
  //! Modifies this BSpline curve by assigning the array
  //! K to its knots table. The multiplicity of the knots is not modified.
  //! Exceptions
  //! Standard_ConstructionError if the values in the
  //! array K are not in ascending order.
  //! Standard_OutOfRange if the bounds of the array
  //! K are not respectively 1 and the number of knots of this BSpline curve.
  Standard_EXPORT void SetKnots (const TColStd_Array1OfReal& K);
  
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
  Standard_EXPORT void SetKnot (const Standard_Integer Index, const Standard_Real K, const Standard_Integer M);
  
  //! Computes the parameter normalized within the
  //! "first" period of this BSpline curve, if it is periodic:
  //! the returned value is in the range Param1 and
  //! Param1 + Period, where:
  //! - Param1 is the "first parameter", and
  //! - Period the period of this BSpline curve.
  //! Note: If this curve is not periodic, U is not modified.
  Standard_EXPORT void PeriodicNormalization (Standard_Real& U) const;
  
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
  Standard_EXPORT void SetOrigin (const Standard_Integer Index);
  
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
  Standard_EXPORT void SetPole (const Standard_Integer Index, const gp_Pnt2d& P);
  
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
  Standard_EXPORT void SetPole (const Standard_Integer Index, const gp_Pnt2d& P, const Standard_Real Weight);
  
  //! Assigns the weight Weight to the pole of index Index of the poles table.
  //! If the curve was non rational it can become rational.
  //! If the curve was rational it can become non rational.
  //! Exceptions
  //! Standard_OutOfRange if Index is outside the
  //! bounds of the poles table.
  //! Standard_ConstructionError if Weight is negative or null.
  Standard_EXPORT void SetWeight (const Standard_Integer Index, const Standard_Real Weight);
  
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
  Standard_EXPORT void MovePoint (const Standard_Real U, const gp_Pnt2d& P, const Standard_Integer Index1, const Standard_Integer Index2, Standard_Integer& FirstModifiedPole, Standard_Integer& LastModifiedPole);
  
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
  Standard_EXPORT void MovePointAndTangent (const Standard_Real U, const gp_Pnt2d& P, const gp_Vec2d& Tangent, const Standard_Real Tolerance, const Standard_Integer StartingCondition, const Standard_Integer EndingCondition, Standard_Integer& ErrorStatus);
  
  //! Returns true if the degree of continuity of this
  //! BSpline curve is at least N. A BSpline curve is at least GeomAbs_C0.
  //! Exceptions Standard_RangeError if N is negative.
  Standard_EXPORT Standard_Boolean IsCN (const Standard_Integer N) const Standard_OVERRIDE;
  

  //! Check if curve has at least G1 continuity in interval [theTf, theTl]
  //! Returns true if IsCN(1)
  //! or
  //! angle between "left" and "right" first derivatives at
  //! knots with C0 continuity is less then theAngTol
  //! only knots in interval [theTf, theTl] is checked
  Standard_EXPORT Standard_Boolean IsG1 (const Standard_Real theTf, const Standard_Real theTl, const Standard_Real theAngTol) const;
  

  //! Returns true if the distance between the first point and the
  //! last point of the curve is lower or equal to Resolution
  //! from package gp.
  //! Warnings :
  //! The first and the last point can be different from the first
  //! pole and the last pole of the curve.
  Standard_EXPORT Standard_Boolean IsClosed() const Standard_OVERRIDE;
  
  //! Returns True if the curve is periodic.
  Standard_EXPORT Standard_Boolean IsPeriodic() const Standard_OVERRIDE;
  

  //! Returns True if the weights are not identical.
  //! The tolerance criterion is Epsilon of the class Real.
  Standard_EXPORT Standard_Boolean IsRational() const;
  

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
  Standard_EXPORT GeomAbs_Shape Continuity() const Standard_OVERRIDE;
  
  //! Returns the degree of this BSpline curve.
  //! In this class the degree of the basis normalized B-spline
  //! functions cannot be greater than "MaxDegree"
  //! Computation of value and derivatives
  Standard_EXPORT Standard_Integer Degree() const;
  
  Standard_EXPORT void D0 (const Standard_Real U, gp_Pnt2d& P) const Standard_OVERRIDE;
  
  //! Raised if the continuity of the curve is not C1.
  Standard_EXPORT void D1 (const Standard_Real U, gp_Pnt2d& P, gp_Vec2d& V1) const Standard_OVERRIDE;
  
  //! Raised if the continuity of the curve is not C2.
  Standard_EXPORT void D2 (const Standard_Real U, gp_Pnt2d& P, gp_Vec2d& V1, gp_Vec2d& V2) const Standard_OVERRIDE;
  
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
  Standard_EXPORT void D3 (const Standard_Real U, gp_Pnt2d& P, gp_Vec2d& V1, gp_Vec2d& V2, gp_Vec2d& V3) const Standard_OVERRIDE;
  
  //! For the point of parameter U of this BSpline curve,
  //! computes the vector corresponding to the Nth derivative.
  //! Warning
  //! On a point where the continuity of the curve is not the
  //! one requested, this function impacts the part defined
  //! by the parameter with a value greater than U, i.e. the
  //! part of the curve to the "right" of the singularity.
  //! Raises  UndefinedDerivative if the continuity of the curve is not CN.
  //! RangeError if N < 1.
  //! The following functions computes the point of parameter U
  //! and the derivatives at this point on the B-spline curve
  //! arc defined between the knot FromK1 and the knot ToK2.
  //! U can be out of bounds [Knot (FromK1),  Knot (ToK2)] but
  //! for the computation we only use the definition of the curve
  //! between these two knots. This method is useful to compute
  //! local derivative, if the order of continuity of the whole
  //! curve is not greater enough.    Inside the parametric
  //! domain Knot (FromK1), Knot (ToK2) the evaluations are
  //! the same as if we consider the whole definition of the
  //! curve. Of course the evaluations are different outside
  //! this parametric domain.
  Standard_EXPORT gp_Vec2d DN (const Standard_Real U, const Standard_Integer N) const Standard_OVERRIDE;
  
  //! Raised if FromK1 = ToK2.
  Standard_EXPORT gp_Pnt2d LocalValue (const Standard_Real U, const Standard_Integer FromK1, const Standard_Integer ToK2) const;
  
  //! Raised if FromK1 = ToK2.
  Standard_EXPORT void LocalD0 (const Standard_Real U, const Standard_Integer FromK1, const Standard_Integer ToK2, gp_Pnt2d& P) const;
  

  //! Raised if the local continuity of the curve is not C1
  //! between the knot K1 and the knot K2.
  //! Raised if FromK1 = ToK2.
  Standard_EXPORT void LocalD1 (const Standard_Real U, const Standard_Integer FromK1, const Standard_Integer ToK2, gp_Pnt2d& P, gp_Vec2d& V1) const;
  

  //! Raised if the local continuity of the curve is not C2
  //! between the knot K1 and the knot K2.
  //! Raised if FromK1 = ToK2.
  Standard_EXPORT void LocalD2 (const Standard_Real U, const Standard_Integer FromK1, const Standard_Integer ToK2, gp_Pnt2d& P, gp_Vec2d& V1, gp_Vec2d& V2) const;
  

  //! Raised if the local continuity of the curve is not C3
  //! between the knot K1 and the knot K2.
  //! Raised if FromK1 = ToK2.
  Standard_EXPORT void LocalD3 (const Standard_Real U, const Standard_Integer FromK1, const Standard_Integer ToK2, gp_Pnt2d& P, gp_Vec2d& V1, gp_Vec2d& V2, gp_Vec2d& V3) const;
  

  //! Raised if the local continuity of the curve is not CN
  //! between the knot K1 and the knot K2.
  //! Raised if FromK1 = ToK2.
  //! Raised if N < 1.
  Standard_EXPORT gp_Vec2d LocalDN (const Standard_Real U, const Standard_Integer FromK1, const Standard_Integer ToK2, const Standard_Integer N) const;
  

  //! Returns the last point of the curve.
  //! Warnings :
  //! The last point of the curve is different from the last
  //! pole of the curve if the multiplicity of the last knot
  //! is lower than Degree.
  Standard_EXPORT gp_Pnt2d EndPoint() const Standard_OVERRIDE;
  

  //! For a B-spline curve the first parameter (which gives the start
  //! point of the curve) is a knot value but if the multiplicity of
  //! the first knot index is lower than Degree + 1 it is not the
  //! first knot of the curve. This method computes the index of the
  //! knot corresponding to the first parameter.
  Standard_EXPORT Standard_Integer FirstUKnotIndex() const;
  

  //! Computes the parametric value of the start point of the curve.
  //! It is a knot value.
  Standard_EXPORT Standard_Real FirstParameter() const Standard_OVERRIDE;
  

  //! Returns the knot of range Index. When there is a knot
  //! with a multiplicity greater than 1 the knot is not repeated.
  //! The method Multiplicity can be used to get the multiplicity
  //! of the Knot.
  //! Raised if Index < 1 or Index > NbKnots
  Standard_EXPORT Standard_Real Knot (const Standard_Integer Index) const;
  
  //! returns the knot values of the B-spline curve;
  //!
  //! Raised K.Lower() is less than number of first knot or
  //! K.Upper() is more than number of last knot.
  Standard_EXPORT void Knots (TColStd_Array1OfReal& K) const;
  
  //! returns the knot values of the B-spline curve;
  Standard_EXPORT const TColStd_Array1OfReal& Knots() const;
  
  //! Returns the knots sequence.
  //! In this sequence the knots with a multiplicity greater than 1
  //! are repeated.
  //! Example :
  //! K = {k1, k1, k1, k2, k3, k3, k4, k4, k4}
  //!
  //! Raised if K.Lower() is less than number of first knot
  //! in knot sequence with repetitions or K.Upper() is more
  //! than number of last knot in knot sequence with repetitions.
  Standard_EXPORT void KnotSequence (TColStd_Array1OfReal& K) const;
  
  //! Returns the knots sequence.
  //! In this sequence the knots with a multiplicity greater than 1
  //! are repeated.
  //! Example :
  //! K = {k1, k1, k1, k2, k3, k3, k4, k4, k4}
  Standard_EXPORT const TColStd_Array1OfReal& KnotSequence() const;
  

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
  Standard_EXPORT Standard_Integer LastUKnotIndex() const;
  

  //! Computes the parametric value of the end point of the curve.
  //! It is a knot value.
  Standard_EXPORT Standard_Real LastParameter() const Standard_OVERRIDE;
  

  //! Locates the parametric value U in the sequence of knots.
  //! If "WithKnotRepetition" is True we consider the knot's
  //! representation with repetition of multiple knot value,
  //! otherwise  we consider the knot's representation with
  //! no repetition of multiple knot values.
  //! Knots (I1) <= U <= Knots (I2)
  //! . if I1 = I2  U is a knot value (the tolerance criterion
  //! ParametricTolerance is used).
  //! . if I1 < 1  => U < Knots (1) - Abs(ParametricTolerance)
  //! . if I2 > NbKnots => U > Knots (NbKnots) + Abs(ParametricTolerance)
  Standard_EXPORT void LocateU (const Standard_Real U, const Standard_Real ParametricTolerance, Standard_Integer& I1, Standard_Integer& I2, const Standard_Boolean WithKnotRepetition = Standard_False) const;
  

  //! Returns the multiplicity of the knots of range Index.
  //! Raised if Index < 1 or Index > NbKnots
  Standard_EXPORT Standard_Integer Multiplicity (const Standard_Integer Index) const;
  

  //! Returns the multiplicity of the knots of the curve.
  //!
  //! Raised if the length of M is not equal to NbKnots.
  Standard_EXPORT void Multiplicities (TColStd_Array1OfInteger& M) const;
  
  //! returns the multiplicity of the knots of the curve.
  Standard_EXPORT const TColStd_Array1OfInteger& Multiplicities() const;
  

  //! Returns the number of knots. This method returns the number of
  //! knot without repetition of multiple knots.
  Standard_EXPORT Standard_Integer NbKnots() const;
  
  //! Returns the number of poles
  Standard_EXPORT Standard_Integer NbPoles() const;
  
  //! Returns the pole of range Index.
  //! Raised if Index < 1 or Index > NbPoles.
  Standard_EXPORT const gp_Pnt2d& Pole (const Standard_Integer Index) const;
  
  //! Returns the poles of the B-spline curve;
  //!
  //! Raised if the length of P is not equal to the number of poles.
  Standard_EXPORT void Poles (TColgp_Array1OfPnt2d& P) const;
  
  //! Returns the poles of the B-spline curve;
  Standard_EXPORT const TColgp_Array1OfPnt2d& Poles() const;
  

  //! Returns the start point of the curve.
  //! Warnings :
  //! This point is different from the first pole of the curve if the
  //! multiplicity of the first knot is lower than Degree.
  Standard_EXPORT gp_Pnt2d StartPoint() const Standard_OVERRIDE;
  
  //! Returns the weight of the pole of range Index .
  //! Raised if Index < 1 or Index > NbPoles.
  Standard_EXPORT Standard_Real Weight (const Standard_Integer Index) const;
  
  //! Returns the weights of the B-spline curve;
  //!
  //! Raised if the length of W is not equal to NbPoles.
  Standard_EXPORT void Weights (TColStd_Array1OfReal& W) const;
  
  //! Returns the weights of the B-spline curve;
  Standard_EXPORT const TColStd_Array1OfReal* Weights() const;
  
  //! Applies the transformation T to this BSpline curve.
  Standard_EXPORT void Transform (const gp_Trsf2d& T) Standard_OVERRIDE;
  

  //! Returns the value of the maximum degree of the normalized
  //! B-spline basis functions in this package.
  Standard_EXPORT static Standard_Integer MaxDegree();
  
  //! Computes for this BSpline curve the parametric
  //! tolerance UTolerance for a given tolerance
  //! Tolerance3D (relative to dimensions in the plane).
  //! If f(t) is the equation of this BSpline curve,
  //! UTolerance ensures that:
  //! | t1 - t0| < Utolerance ===>
  //! |f(t1) - f(t0)| < ToleranceUV
  Standard_EXPORT void Resolution (const Standard_Real ToleranceUV, Standard_Real& UTolerance);
  
  //! Creates a new object which is a copy of this BSpline curve.
  Standard_EXPORT Handle(Geom2d_Geometry) Copy() const Standard_OVERRIDE;

  //! Dumps the content of me into the stream
  Standard_EXPORT virtual void DumpJson (Standard_OStream& theOStream, Standard_Integer theDepth = -1) const Standard_OVERRIDE;




  DEFINE_STANDARD_RTTIEXT(Geom2d_BSplineCurve,Geom2d_BoundedCurve)

protected:




private:

  
  //! Recompute  the  flatknots,  the knotsdistribution, the continuity.
  Standard_EXPORT void UpdateKnots();

  Standard_Boolean rational;
  Standard_Boolean periodic;
  GeomAbs_BSplKnotDistribution knotSet;
  GeomAbs_Shape smooth;
  Standard_Integer deg;
  Handle(TColgp_HArray1OfPnt2d) poles;
  Handle(TColStd_HArray1OfReal) weights;
  Handle(TColStd_HArray1OfReal) flatknots;
  Handle(TColStd_HArray1OfReal) knots;
  Handle(TColStd_HArray1OfInteger) mults;
  Standard_Real maxderivinv;
  Standard_Boolean maxderivinvok;


};







#endif // _Geom2d_BSplineCurve_HeaderFile
