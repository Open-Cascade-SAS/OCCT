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
#include <NCollection_Array1.hxx>
#include <NCollection_HArray1.hxx>
#include <Standard_Transient.hxx>

//! Definition of the 1D B_spline curve.
//!
//! Uniform  or non-uniform
//! Rational or non-rational
//! Periodic or non-periodic
//!
//! A b-spline curve is defined by:
//!
//! The Degree (up to 25)
//!
//! The Poles (and the weights if it is rational)
//!
//! The Knots and Multiplicities
//!
//! The knot vector is an increasing sequence of
//! reals without repetition. The multiplicities are
//! the repetition of the knots.
//!
//! If the knots are regularly spaced (the difference
//! of two consecutive knots is a constant), the
//! knots repartition is:
//!
//! - Uniform if all multiplicities are 1.
//!
//! - Quasi-uniform if all multiplicities are 1
//! but the first and the last which are Degree+1.
//!
//! - PiecewiseBezier if all multiplicities are
//! Degree but the first and the last which are
//! Degree+1.
//!
//! The curve may be periodic.
//!
//! On a periodic curve if there are k knots and p
//! poles. the period is knot(k) - knot(1)
//!
//! the poles and knots are infinite vectors with:
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
  //! Creates a non-rational B_spline curve on the
  //! basis <Knots, Multiplicities> of degree <Degree>.
  Standard_EXPORT Law_BSpline(const NCollection_Array1<double>& Poles,
                              const NCollection_Array1<double>& Knots,
                              const NCollection_Array1<int>&    Multiplicities,
                              const int                         Degree,
                              const bool                        Periodic = false);

  //! Creates a rational B_spline curve on the basis
  //! <Knots, Multiplicities> of degree <Degree>.
  Standard_EXPORT Law_BSpline(const NCollection_Array1<double>& Poles,
                              const NCollection_Array1<double>& Weights,
                              const NCollection_Array1<double>& Knots,
                              const NCollection_Array1<int>&    Multiplicities,
                              const int                         Degree,
                              const bool                        Periodic = false);

  //! Increase the degree to <Degree>. Nothing is done
  //! if <Degree> is lower or equal to the current
  //! degree.
  Standard_EXPORT void IncreaseDegree(const int Degree);

  //! Increases the multiplicity of the knot <Index> to
  //! <M>.
  //!
  //! If <M> is lower or equal to the current multiplicity
  //! nothing is done. If <M> is higher than the degree
  //! the degree is used.
  //! If <Index> is not in [FirstUKnotIndex, LastUKnotIndex]
  Standard_EXPORT void IncreaseMultiplicity(const int Index, const int M);

  //! Increases the multiplicities of the knots in
  //! [I1,I2] to <M>.
  //!
  //! For each knot if <M> is lower or equal to the
  //! current multiplicity nothing is done. If <M> is
  //! higher than the degree the degree is used.
  //! If <I1,I2> are not in [FirstUKnotIndex, LastUKnotIndex]
  Standard_EXPORT void IncreaseMultiplicity(const int I1, const int I2, const int M);

  //! Increment the multiplicities of the knots in
  //! [I1,I2] by <M>.
  //!
  //! If <M> is not positive nothing is done.
  //!
  //! For each knot the resulting multiplicity is
  //! limited to the Degree.
  //! If <I1,I2> are not in [FirstUKnotIndex, LastUKnotIndex]
  Standard_EXPORT void IncrementMultiplicity(const int I1, const int I2, const int M);

  //! Inserts a knot value in the sequence of knots.
  //! If <U> is an existing knot the multiplicity is
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
  Standard_EXPORT void InsertKnot(const double U,
                                  const int    M                   = 1,
                                  const double ParametricTolerance = 0.0,
                                  const bool   Add                 = true);

  //! Inserts a set of knots values in the sequence of
  //! knots.
  //!
  //! For each U = Knots(i), M = Mults(i)
  //!
  //! If <U> is an existing knot the multiplicity is
  //! increased by <M> if <Add> is True, increased to
  //! <M> if <Add> is False.
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
  Standard_EXPORT void InsertKnots(const NCollection_Array1<double>& Knots,
                                   const NCollection_Array1<int>&    Mults,
                                   const double                      ParametricTolerance = 0.0,
                                   const bool                        Add                 = false);

  //! Decrement the knots multiplicity to <M>. If M is
  //! 0 the knot is removed. The Poles sequence is
  //! modified.
  //!
  //! As there are two ways to compute the new poles the
  //! average is computed if the distance is lower than
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
  //! BSplineCurve. The knot values are modified. If the BSpline is
  //! NonUniform or Piecewise Bezier an exception Construction error
  //! is raised.
  Standard_EXPORT bool RemoveKnot(const int Index, const int M, const double Tolerance);

  //! Changes the direction of parametrization of <me>. The Knot
  //! sequence is modified, the FirstParameter and the
  //! LastParameter are not modified. The StartPoint of the
  //! initial curve becomes the EndPoint of the reversed curve
  //! and the EndPoint of the initial curve becomes the StartPoint
  //! of the reversed curve.
  Standard_EXPORT void Reverse();

  //! Returns the parameter on the reversed curve for
  //! the point of parameter U on <me>.
  //!
  //! returns UFirst + ULast - U
  Standard_EXPORT double ReversedParameter(const double U) const;

  //! Segments the curve between U1 and U2.
  //! The control points are modified, the first and the last point
  //! are not the same.
  //! Warnings :
  //! Even if <me> is not closed it can become closed after the
  //! segmentation for example if U1 or U2 are out of the bounds
  //! of the curve <me> or if the curve makes loop.
  //! After the segmentation the length of a curve can be null.
  //! raises if U2 < U1.
  Standard_EXPORT void Segment(const double U1, const double U2);

  //! Changes the knot of range Index.
  //! The multiplicity of the knot is not modified.
  //! Raised if K >= Knots(Index+1) or K <= Knots(Index-1).
  //! Raised if Index < 1 || Index > NbKnots
  Standard_EXPORT void SetKnot(const int Index, const double K);

  //! Changes all the knots of the curve
  //! The multiplicity of the knots are not modified.
  //!
  //! Raised if there is an index such that K (Index+1) <= K (Index).
  //!
  //! Raised if K.Lower() < 1 or K.Upper() > NbKnots
  Standard_EXPORT void SetKnots(const NCollection_Array1<double>& K);

  //! Changes the knot of range Index with its multiplicity.
  //! You can increase the multiplicity of a knot but it is
  //! not allowed to decrease the multiplicity of an existing knot.
  //!
  //! Raised if K >= Knots(Index+1) or K <= Knots(Index-1).
  //! Raised if M is greater than Degree or lower than the previous
  //! multiplicity of knot of range Index.
  //! Raised if Index < 1 || Index > NbKnots
  Standard_EXPORT void SetKnot(const int Index, const double K, const int M);

  //! returns the parameter normalized within
  //! the period if the curve is periodic : otherwise
  //! does not do anything
  Standard_EXPORT void PeriodicNormalization(double& U) const;

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
  Standard_EXPORT void SetOrigin(const int Index);

  //! Makes a non periodic curve. If the curve was non periodic
  //! the curve is not modified.
  Standard_EXPORT void SetNotPeriodic();

  //! Substitutes the Pole of range Index with P.
  //!
  //! Raised if Index < 1 || Index > NbPoles
  Standard_EXPORT void SetPole(const int Index, const double P);

  //! Substitutes the pole and the weight of range Index.
  //! If the curve <me> is not rational it can become rational
  //! If the curve was rational it can become non rational
  //!
  //! Raised if Index < 1 || Index > NbPoles
  //! Raised if Weight <= 0.0
  Standard_EXPORT void SetPole(const int Index, const double P, const double Weight);

  //! Changes the weight for the pole of range Index.
  //! If the curve was non rational it can become rational.
  //! If the curve was rational it can become non rational.
  //!
  //! Raised if Index < 1 || Index > NbPoles
  //! Raised if Weight <= 0.0
  Standard_EXPORT void SetWeight(const int Index, const double Weight);

  //! Returns the continuity of the curve, the curve is at least C0.
  //! Raised if N < 0.
  Standard_EXPORT bool IsCN(const int N) const;

  //! Returns true if the distance between the first point and the
  //! last point of the curve is lower or equal to Resolution
  //! from package gp.
  //! Warnings :
  //! The first and the last point can be different from the first
  //! pole and the last pole of the curve.
  Standard_EXPORT bool IsClosed() const;

  //! Returns True if the curve is periodic.
  Standard_EXPORT bool IsPeriodic() const;

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
  Standard_EXPORT GeomAbs_Shape Continuity() const;

  //! Computation of value and derivatives
  Standard_EXPORT int Degree() const;

  Standard_EXPORT double Value(const double U) const;

  Standard_EXPORT void D0(const double U, double& P) const;

  Standard_EXPORT void D1(const double U, double& P, double& V1) const;

  Standard_EXPORT void D2(const double U, double& P, double& V1, double& V2) const;

  Standard_EXPORT void D3(const double U, double& P, double& V1, double& V2, double& V3) const;

  //! The following functions computes the point of parameter U and
  //! the derivatives at this point on the B-spline curve arc
  //! defined between the knot FromK1 and the knot ToK2. U can be
  //! out of bounds [Knot (FromK1), Knot (ToK2)] but for the
  //! computation we only use the definition of the curve between
  //! these two knots. This method is useful to compute local
  //! derivative, if the order of continuity of the whole curve is
  //! not greater enough. Inside the parametric domain Knot
  //! (FromK1), Knot (ToK2) the evaluations are the same as if we
  //! consider the whole definition of the curve. Of course the
  //! evaluations are different outside this parametric domain.
  Standard_EXPORT double DN(const double U, const int N) const;

  Standard_EXPORT double LocalValue(const double U, const int FromK1, const int ToK2) const;

  Standard_EXPORT void LocalD0(const double U, const int FromK1, const int ToK2, double& P) const;

  Standard_EXPORT void LocalD1(const double U,
                               const int    FromK1,
                               const int    ToK2,
                               double&      P,
                               double&      V1) const;

  Standard_EXPORT void LocalD2(const double U,
                               const int    FromK1,
                               const int    ToK2,
                               double&      P,
                               double&      V1,
                               double&      V2) const;

  Standard_EXPORT void LocalD3(const double U,
                               const int    FromK1,
                               const int    ToK2,
                               double&      P,
                               double&      V1,
                               double&      V2,
                               double&      V3) const;

  Standard_EXPORT double LocalDN(const double U,
                                 const int    FromK1,
                                 const int    ToK2,
                                 const int    N) const;

  //! Returns the last point of the curve.
  //! Warnings :
  //! The last point of the curve is different from the last
  //! pole of the curve if the multiplicity of the last knot
  //! is lower than Degree.
  Standard_EXPORT double EndPoint() const;

  //! For a B-spline curve the first parameter (which gives the start
  //! point of the curve) is a knot value but if the multiplicity of
  //! the first knot index is lower than Degree + 1 it is not the
  //! first knot of the curve. This method computes the index of the
  //! knot corresponding to the first parameter.
  Standard_EXPORT int FirstUKnotIndex() const;

  //! Computes the parametric value of the start point of the curve.
  //! It is a knot value.
  Standard_EXPORT double FirstParameter() const;

  //! Returns the knot of range Index. When there is a knot
  //! with a multiplicity greater than 1 the knot is not repeated.
  //! The method Multiplicity can be used to get the multiplicity
  //! of the Knot.
  //! Raised if Index < 1 or Index > NbKnots
  Standard_EXPORT double Knot(const int Index) const;

  //! returns the knot values of the B-spline curve;
  //!
  //! Raised if the length of K is not equal to the number of knots.
  Standard_EXPORT void Knots(NCollection_Array1<double>& K) const;

  //! Returns the knots sequence.
  //! In this sequence the knots with a multiplicity greater than 1
  //! are repeated.
  //! Example :
  //! K = {k1, k1, k1, k2, k3, k3, k4, k4, k4}
  //!
  //! Raised if the length of K is not equal to NbPoles + Degree + 1
  Standard_EXPORT void KnotSequence(NCollection_Array1<double>& K) const;

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
  Standard_EXPORT double LastParameter() const;

  //! Locates the parametric value U in the sequence of knots.
  //! If "WithKnotRepetition" is True we consider the knot's
  //! representation with repetition of multiple knot value,
  //! otherwise we consider the knot's representation with
  //! no repetition of multiple knot values.
  //! Knots (I1) <= U <= Knots (I2)
  //! . if I1 = I2 U is a knot value (the tolerance criterion
  //! ParametricTolerance is used).
  //! . if I1 < 1 => U < Knots (1) - std::abs(ParametricTolerance)
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
  Standard_EXPORT void Multiplicities(NCollection_Array1<int>& M) const;

  //! Returns the number of knots. This method returns the number of
  //! knot without repetition of multiple knots.
  Standard_EXPORT int NbKnots() const;

  //! Returns the number of poles
  Standard_EXPORT int NbPoles() const;

  //! Returns the pole of range Index.
  //! Raised if Index < 1 or Index > NbPoles.
  Standard_EXPORT double Pole(const int Index) const;

  //! Returns the poles of the B-spline curve;
  //!
  //! Raised if the length of P is not equal to the number of poles.
  Standard_EXPORT void Poles(NCollection_Array1<double>& P) const;

  //! Returns the start point of the curve.
  //! Warnings :
  //! This point is different from the first pole of the curve if the
  //! multiplicity of the first knot is lower than Degree.
  Standard_EXPORT double StartPoint() const;

  //! Returns the weight of the pole of range Index .
  //! Raised if Index < 1 or Index > NbPoles.
  Standard_EXPORT double Weight(const int Index) const;

  //! Returns the weights of the B-spline curve;
  //!
  //! Raised if the length of W is not equal to NbPoles.
  Standard_EXPORT void Weights(NCollection_Array1<double>& W) const;

  //! Returns the value of the maximum degree of the normalized
  //! B-spline basis functions in this package.
  Standard_EXPORT static int MaxDegree();

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
  Standard_EXPORT void MovePointAndTangent(const double U,
                                           const double NewValue,
                                           const double Derivative,
                                           const double Tolerance,
                                           const int    StartingCondition,
                                           const int    EndingCondition,
                                           int&         ErrorStatus);

  //! given Tolerance3D returns UTolerance
  //! such that if f(t) is the curve we have
  //! | t1 - t0| < Utolerance ===>
  //! |f(t1) - f(t0)| < Tolerance3D
  Standard_EXPORT void Resolution(const double Tolerance3D, double& UTolerance) const;

  Standard_EXPORT occ::handle<Law_BSpline> Copy() const;

  DEFINE_STANDARD_RTTIEXT(Law_BSpline, Standard_Transient)

private:
  //! Tells whether the Cache is valid for the
  //! given parameter
  //! Warnings : the parameter must be normalized within
  //! the period if the curve is periodic. Otherwise
  //! the answer will be false
  Standard_EXPORT bool IsCacheValid(const double Parameter) const;

  //! Recompute the flatknots, the knotsdistribution, the
  //! continuity.
  Standard_EXPORT void UpdateKnots();

  bool                                     rational;
  bool                                     periodic;
  GeomAbs_BSplKnotDistribution             knotSet;
  GeomAbs_Shape                            smooth;
  int                                      deg;
  occ::handle<NCollection_HArray1<double>> poles;
  occ::handle<NCollection_HArray1<double>> weights;
  occ::handle<NCollection_HArray1<double>> flatknots;
  occ::handle<NCollection_HArray1<double>> knots;
  occ::handle<NCollection_HArray1<int>>    mults;
};

#endif // _Law_BSpline_HeaderFile
