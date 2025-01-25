// Created on: 1995-10-20
// Created by: Laurent BOURESCHE
// Copyright (c) 1995-1999 Matra Datavision
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

#ifndef _Law_BSpline_HeaderFile
#define _Law_BSpline_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <GeomAbs_BSplKnotDistribution.hxx>
#include <GeomAbs_Shape.hxx>
#include <Standard_Integer.hxx>
#include <TColStd_HArray1OfReal.hxx>
#include <TColStd_HArray1OfInteger.hxx>
#include <Standard_Transient.hxx>
#include <TColStd_Array1OfReal.hxx>
#include <TColStd_Array1OfInteger.hxx>

class Law_BSpline;
DEFINE_STANDARD_HANDLE(Law_BSpline, Standard_Transient)

//! Definition of the 1D B_spline curve.
//!
//! Uniform  or non-uniform
//! Rational or non-rational
//! Periodic or non-periodic
//!
//! a b-spline curve is defined by :
//!
//! The Degree (up to 25)
//!
//! The Poles  (and the weights if it is rational)
//!
//! The Knots and Multiplicities
//!
//! The knot vector   is an  increasing  sequence  of
//! reals without  repetition. The multiplicities are
//! the repetition of the knots.
//!
//! If the knots are regularly spaced (the difference
//! of two  consecutive  knots  is a   constant), the
//! knots repartition is :
//!
//! - Uniform if all multiplicities are 1.
//!
//! -  Quasi-uniform if  all multiplicities are  1
//! but the first and the last which are Degree+1.
//!
//! -   PiecewiseBezier if  all multiplicities are
//! Degree but the   first and the  last which are
//! Degree+1.
//!
//! The curve may be periodic.
//!
//! On a periodic curve if there are k knots and p
//! poles. the period is knot(k) - knot(1)
//!
//! the poles and knots are infinite vectors with :
//!
//! knot(i+k) = knot(i) + period
//!
//! pole(i+p) = pole(i)
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
class Law_BSpline : public Standard_Transient
{

public:
  //! Creates a  non-rational B_spline curve   on  the
  //! basis <Knots, Multiplicities> of degree <Degree>.
  Standard_EXPORT Law_BSpline(const TColStd_Array1OfReal&    Poles,
                              const TColStd_Array1OfReal&    Knots,
                              const TColStd_Array1OfInteger& Multiplicities,
                              const Standard_Integer         Degree,
                              const Standard_Boolean         Periodic = Standard_False);

  //! Creates  a rational B_spline  curve  on the basis
  //! <Knots, Multiplicities> of degree <Degree>.
  Standard_EXPORT Law_BSpline(const TColStd_Array1OfReal&    Poles,
                              const TColStd_Array1OfReal&    Weights,
                              const TColStd_Array1OfReal&    Knots,
                              const TColStd_Array1OfInteger& Multiplicities,
                              const Standard_Integer         Degree,
                              const Standard_Boolean         Periodic = Standard_False);

  //! Increase the degree to  <Degree>. Nothing is  done
  //! if  <Degree>   is lower or  equal  to the  current
  //! degree.
  Standard_EXPORT void IncreaseDegree(const Standard_Integer Degree);

  //! Increases the multiplicity  of the knot <Index> to
  //! <M>.
  //!
  //! If   <M>   is   lower   or  equal   to  the current
  //! multiplicity nothing is done. If <M> is higher than
  //! the degree the degree is used.
  //! If <Index> is not in [FirstUKnotIndex, LastUKnotIndex]
  Standard_EXPORT void IncreaseMultiplicity(const Standard_Integer Index, const Standard_Integer M);

  //! Increases  the  multiplicities   of  the knots  in
  //! [I1,I2] to <M>.
  //!
  //! For each knot if  <M>  is  lower  or equal  to  the
  //! current multiplicity  nothing  is  done. If <M>  is
  //! higher than the degree the degree is used.
  //! If <I1,I2> are not in [FirstUKnotIndex, LastUKnotIndex]
  Standard_EXPORT void IncreaseMultiplicity(const Standard_Integer I1,
                                            const Standard_Integer I2,
                                            const Standard_Integer M);

  //! Increment  the  multiplicities   of  the knots  in
  //! [I1,I2] by <M>.
  //!
  //! If <M> is not positive nithing is done.
  //!
  //! For   each  knot   the resulting   multiplicity  is
  //! limited to the Degree.
  //! If <I1,I2> are not in [FirstUKnotIndex, LastUKnotIndex]
  Standard_EXPORT void IncrementMultiplicity(const Standard_Integer I1,
                                             const Standard_Integer I2,
                                             const Standard_Integer M);

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
  Standard_EXPORT void InsertKnot(const Standard_Real    U,
                                  const Standard_Integer M                   = 1,
                                  const Standard_Real    ParametricTolerance = 0.0,
                                  const Standard_Boolean Add                 = Standard_True);

  //! Inserts a set of knots  values in  the sequence of
  //! knots.
  //!
  //! For each U = Knots(i), M = Mults(i)
  //!
  //! If <U>  is an existing  knot  the  multiplicity is
  //! increased by  <M> if  <Add>  is True, increased to
  //! <M> if <Add> is False.
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
  Standard_EXPORT void InsertKnots(const TColStd_Array1OfReal&    Knots,
                                   const TColStd_Array1OfInteger& Mults,
                                   const Standard_Real            ParametricTolerance = 0.0,
                                   const Standard_Boolean         Add = Standard_False);

  //! Decrement the knots multiplicity to <M>. If  M is
  //! 0 the knot   is  removed. The  Poles  sequence   is
  //! modified.
  //!
  //! As there are two ways to  compute the new poles the
  //! average is  computed if  the distance is lower than
  //! the <Tolerance>, else False is returned.
  //!
  //! A low tolerance is used to prevent the modification
  //! of the curve.
  //!
  //! A high tolerance is used to "smooth" the curve.
  //!
  //! Raised if Index is not in the range
  //! [FirstUKnotIndex, LastUKnotIndex]
  //! pole insertion and pole removing
  //! this operation is limited to the Uniform or QuasiUniform
  //! BSplineCurve. The knot values are modified . If the BSpline is
  //! NonUniform or Piecewise Bezier an exception Construction error
  //! is raised.
  Standard_EXPORT Standard_Boolean RemoveKnot(const Standard_Integer Index,
                                              const Standard_Integer M,
                                              const Standard_Real    Tolerance);

  //! Changes the direction of parametrization of <me>. The Knot
  //! sequence is modified, the FirstParameter and the
  //! LastParameter are not modified. The StartPoint of the
  //! initial curve becomes the EndPoint of the reversed curve
  //! and the EndPoint of the initial curve becomes the StartPoint
  //! of the reversed curve.
  Standard_EXPORT void Reverse();

  //! Returns the  parameter on the  reversed  curve for
  //! the point of parameter U on <me>.
  //!
  //! returns UFirst + ULast - U
  Standard_EXPORT Standard_Real ReversedParameter(const Standard_Real U) const;

  //! Segments the curve between U1 and U2.
  //! The control points are modified, the first and the last point
  //! are not the same.
  //! Warnings :
  //! Even if <me> is not closed it can become closed after the
  //! segmentation for example if U1 or U2 are out of the bounds
  //! of the curve <me> or if the curve makes loop.
  //! After the segmentation the length of a curve can be null.
  //! raises if U2 < U1.
  Standard_EXPORT void Segment(const Standard_Real U1, const Standard_Real U2);

  //! Changes the knot of range Index.
  //! The multiplicity of the knot is not modified.
  //! Raised if K >= Knots(Index+1) or K <= Knots(Index-1).
  //! Raised if Index < 1 || Index > NbKnots
  Standard_EXPORT void SetKnot(const Standard_Integer Index, const Standard_Real K);

  //! Changes all the knots of the curve
  //! The multiplicity of the knots are not modified.
  //!
  //! Raised if there is an index such that K (Index+1) <= K (Index).
  //!
  //! Raised if  K.Lower() < 1 or K.Upper() > NbKnots
  Standard_EXPORT void SetKnots(const TColStd_Array1OfReal& K);

  //! Changes the knot of range Index with its multiplicity.
  //! You can increase the multiplicity of a knot but it is
  //! not allowed to decrease the multiplicity of an existing knot.
  //!
  //! Raised if K >= Knots(Index+1) or K <= Knots(Index-1).
  //! Raised if M is greater than Degree or lower than the previous
  //! multiplicity of knot of range Index.
  //! Raised if Index < 1 || Index > NbKnots
  Standard_EXPORT void SetKnot(const Standard_Integer Index,
                               const Standard_Real    K,
                               const Standard_Integer M);

  //! returns the parameter normalized within
  //! the period if the curve is periodic : otherwise
  //! does not do anything
  Standard_EXPORT void PeriodicNormalization(Standard_Real& U) const;

  //! Makes a closed B-spline into a periodic curve. The curve is
  //! periodic if the knot sequence is periodic and if the curve is
  //! closed (The tolerance criterion is Resolution from gp).
  //! The period T is equal to Knot(LastUKnotIndex) -
  //! Knot(FirstUKnotIndex). A periodic B-spline can be uniform
  //! or not.
  //! Raised if the curve is not closed.
  Standard_EXPORT void SetPeriodic();

  //! Set the origin of a periodic curve at Knot(index)
  //! KnotVector and poles are modified.
  //! Raised if the curve is not periodic
  //! Raised if index not in the range
  //! [FirstUKnotIndex , LastUKnotIndex]
  Standard_EXPORT void SetOrigin(const Standard_Integer Index);

  //! Makes a non periodic curve. If the curve was non periodic
  //! the curve is not modified.
  Standard_EXPORT void SetNotPeriodic();

  //! Substitutes the Pole of range Index with P.
  //!
  //! Raised if Index < 1 || Index > NbPoles
  Standard_EXPORT void SetPole(const Standard_Integer Index, const Standard_Real P);

  //! Substitutes the pole and the weight of range Index.
  //! If the curve <me> is not rational it can become rational
  //! If the curve was rational it can become non rational
  //!
  //! Raised if Index < 1 || Index > NbPoles
  //! Raised if Weight <= 0.0
  Standard_EXPORT void SetPole(const Standard_Integer Index,
                               const Standard_Real    P,
                               const Standard_Real    Weight);

  //! Changes the weight for the pole of range Index.
  //! If the curve was non rational it can become rational.
  //! If the curve was rational it can become non rational.
  //!
  //! Raised if Index < 1 || Index > NbPoles
  //! Raised if Weight <= 0.0
  Standard_EXPORT void SetWeight(const Standard_Integer Index, const Standard_Real Weight);

  //! Returns the continuity of the curve, the curve is at least C0.
  //! Raised if N < 0.
  Standard_EXPORT Standard_Boolean IsCN(const Standard_Integer N) const;

  //! Returns true if the distance between the first point and the
  //! last point of the curve is lower or equal to Resolution
  //! from package gp.
  //! Warnings :
  //! The first and the last point can be different from the first
  //! pole and the last pole of the curve.
  Standard_EXPORT Standard_Boolean IsClosed() const;

  //! Returns True if the curve is periodic.
  Standard_EXPORT Standard_Boolean IsPeriodic() const;

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
  Standard_EXPORT GeomAbs_Shape Continuity() const;

  //! Computation of value and derivatives
  Standard_EXPORT Standard_Integer Degree() const;

  Standard_EXPORT Standard_Real Value(const Standard_Real U) const;

  Standard_EXPORT void D0(const Standard_Real U, Standard_Real& P) const;

  Standard_EXPORT void D1(const Standard_Real U, Standard_Real& P, Standard_Real& V1) const;

  Standard_EXPORT void D2(const Standard_Real U,
                          Standard_Real&      P,
                          Standard_Real&      V1,
                          Standard_Real&      V2) const;

  Standard_EXPORT void D3(const Standard_Real U,
                          Standard_Real&      P,
                          Standard_Real&      V1,
                          Standard_Real&      V2,
                          Standard_Real&      V3) const;

  //! The following functions computes the point  of parameter U and
  //! the  derivatives at   this  point on  the  B-spline curve  arc
  //! defined between the knot FromK1  and the knot  ToK2.  U can be
  //! out of bounds   [Knot  (FromK1), Knot   (ToK2)] but   for  the
  //! computation we only  use  the definition of the  curve between
  //! these  two  knots. This  method is  useful  to  compute  local
  //! derivative,  if the order of  continuity of the whole curve is
  //! not   greater  enough.   Inside   the parametric   domain Knot
  //! (FromK1), Knot (ToK2)  the evaluations are the  same as if  we
  //! consider  the whole  definition of the  curve.   Of course the
  //! evaluations are different outside this parametric domain.
  Standard_EXPORT Standard_Real DN(const Standard_Real U, const Standard_Integer N) const;

  Standard_EXPORT Standard_Real LocalValue(const Standard_Real    U,
                                           const Standard_Integer FromK1,
                                           const Standard_Integer ToK2) const;

  Standard_EXPORT void LocalD0(const Standard_Real    U,
                               const Standard_Integer FromK1,
                               const Standard_Integer ToK2,
                               Standard_Real&         P) const;

  Standard_EXPORT void LocalD1(const Standard_Real    U,
                               const Standard_Integer FromK1,
                               const Standard_Integer ToK2,
                               Standard_Real&         P,
                               Standard_Real&         V1) const;

  Standard_EXPORT void LocalD2(const Standard_Real    U,
                               const Standard_Integer FromK1,
                               const Standard_Integer ToK2,
                               Standard_Real&         P,
                               Standard_Real&         V1,
                               Standard_Real&         V2) const;

  Standard_EXPORT void LocalD3(const Standard_Real    U,
                               const Standard_Integer FromK1,
                               const Standard_Integer ToK2,
                               Standard_Real&         P,
                               Standard_Real&         V1,
                               Standard_Real&         V2,
                               Standard_Real&         V3) const;

  Standard_EXPORT Standard_Real LocalDN(const Standard_Real    U,
                                        const Standard_Integer FromK1,
                                        const Standard_Integer ToK2,
                                        const Standard_Integer N) const;

  //! Returns the last point of the curve.
  //! Warnings :
  //! The last point of the curve is different from the last
  //! pole of the curve if the multiplicity of the last knot
  //! is lower than Degree.
  Standard_EXPORT Standard_Real EndPoint() const;

  //! For a B-spline curve the first parameter (which gives the start
  //! point of the curve) is a knot value but if the multiplicity of
  //! the first knot index is lower than Degree + 1 it is not the
  //! first knot of the curve. This method computes the index of the
  //! knot corresponding to the first parameter.
  Standard_EXPORT Standard_Integer FirstUKnotIndex() const;

  //! Computes the parametric value of the start point of the curve.
  //! It is a knot value.
  Standard_EXPORT Standard_Real FirstParameter() const;

  //! Returns the knot of range Index. When there is a knot
  //! with a multiplicity greater than 1 the knot is not repeated.
  //! The method Multiplicity can be used to get the multiplicity
  //! of the Knot.
  //! Raised if Index < 1 or Index > NbKnots
  Standard_EXPORT Standard_Real Knot(const Standard_Integer Index) const;

  //! returns the knot values of the B-spline curve;
  //!
  //! Raised if the length of K is not equal to the number of knots.
  Standard_EXPORT void Knots(TColStd_Array1OfReal& K) const;

  //! Returns the knots sequence.
  //! In this sequence the knots with a multiplicity greater than 1
  //! are repeated.
  //! Example :
  //! K = {k1, k1, k1, k2, k3, k3, k4, k4, k4}
  //!
  //! Raised if the length of K is not equal to NbPoles + Degree + 1
  Standard_EXPORT void KnotSequence(TColStd_Array1OfReal& K) const;

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
  Standard_EXPORT Standard_Real LastParameter() const;

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
  Standard_EXPORT void LocateU(const Standard_Real    U,
                               const Standard_Real    ParametricTolerance,
                               Standard_Integer&      I1,
                               Standard_Integer&      I2,
                               const Standard_Boolean WithKnotRepetition = Standard_False) const;

  //! Returns the multiplicity of the knots of range Index.
  //! Raised if Index < 1 or Index > NbKnots
  Standard_EXPORT Standard_Integer Multiplicity(const Standard_Integer Index) const;

  //! Returns the multiplicity of the knots of the curve.
  //!
  //! Raised if the length of M is not equal to NbKnots.
  Standard_EXPORT void Multiplicities(TColStd_Array1OfInteger& M) const;

  //! Returns the number of knots. This method returns the number of
  //! knot without repetition of multiple knots.
  Standard_EXPORT Standard_Integer NbKnots() const;

  //! Returns the number of poles
  Standard_EXPORT Standard_Integer NbPoles() const;

  //! Returns the pole of range Index.
  //! Raised if Index < 1 or Index > NbPoles.
  Standard_EXPORT Standard_Real Pole(const Standard_Integer Index) const;

  //! Returns the poles of the B-spline curve;
  //!
  //! Raised if the length of P is not equal to the number of poles.
  Standard_EXPORT void Poles(TColStd_Array1OfReal& P) const;

  //! Returns the start point of the curve.
  //! Warnings :
  //! This point is different from the first pole of the curve if the
  //! multiplicity of the first knot is lower than Degree.
  Standard_EXPORT Standard_Real StartPoint() const;

  //! Returns the weight of the pole of range Index .
  //! Raised if Index < 1 or Index > NbPoles.
  Standard_EXPORT Standard_Real Weight(const Standard_Integer Index) const;

  //! Returns the weights of the B-spline curve;
  //!
  //! Raised if the length of W is not equal to NbPoles.
  Standard_EXPORT void Weights(TColStd_Array1OfReal& W) const;

  //! Returns the value of the maximum degree of the normalized
  //! B-spline basis functions in this package.
  Standard_EXPORT static Standard_Integer MaxDegree();

  //! Changes the value of the Law at parameter U to NewValue.
  //! and makes its derivative at U be derivative.
  //! StartingCondition = -1 means first can move
  //! EndingCondition   = -1 means last point can move
  //! StartingCondition = 0 means the first point cannot move
  //! EndingCondition   = 0 means the last point cannot move
  //! StartingCondition = 1 means the first point and tangent cannot move
  //! EndingCondition   = 1 means the last point and tangent cannot move
  //! and so forth
  //! ErrorStatus != 0 means that there are not enough degree of freedom
  //! with the constrain to deform the curve accordingly
  Standard_EXPORT void MovePointAndTangent(const Standard_Real    U,
                                           const Standard_Real    NewValue,
                                           const Standard_Real    Derivative,
                                           const Standard_Real    Tolerance,
                                           const Standard_Integer StartingCondition,
                                           const Standard_Integer EndingCondition,
                                           Standard_Integer&      ErrorStatus);

  //! given Tolerance3D returns UTolerance
  //! such that if f(t) is the curve we have
  //! | t1 - t0| < Utolerance ===>
  //! |f(t1) - f(t0)| < Tolerance3D
  Standard_EXPORT void Resolution(const Standard_Real Tolerance3D, Standard_Real& UTolerance) const;

  Standard_EXPORT Handle(Law_BSpline) Copy() const;

  DEFINE_STANDARD_RTTIEXT(Law_BSpline, Standard_Transient)

protected:
private:
  //! Tells whether the Cache is valid for the
  //! given parameter
  //! Warnings : the parameter must be normalized within
  //! the period if the curve is periodic. Otherwise
  //! the answer will be false
  Standard_EXPORT Standard_Boolean IsCacheValid(const Standard_Real Parameter) const;

  //! Recompute  the  flatknots,  the knotsdistribution, the
  //! continuity.
  Standard_EXPORT void UpdateKnots();

  Standard_Boolean                 rational;
  Standard_Boolean                 periodic;
  GeomAbs_BSplKnotDistribution     knotSet;
  GeomAbs_Shape                    smooth;
  Standard_Integer                 deg;
  Handle(TColStd_HArray1OfReal)    poles;
  Handle(TColStd_HArray1OfReal)    weights;
  Handle(TColStd_HArray1OfReal)    flatknots;
  Handle(TColStd_HArray1OfReal)    knots;
  Handle(TColStd_HArray1OfInteger) mults;
};

#endif // _Law_BSpline_HeaderFile
