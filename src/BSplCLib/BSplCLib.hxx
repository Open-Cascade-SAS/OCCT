// Created on: 1991-08-09
// Created by: Jean Claude VAUTHIER
// Copyright (c) 1991-1999 Matra Datavision
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

#ifndef _BSplCLib_HeaderFile
#define _BSplCLib_HeaderFile

#include <BSplCLib_EvaluatorFunction.hxx>
#include <BSplCLib_KnotDistribution.hxx>
#include <BSplCLib_MultDistribution.hxx>
#include <GeomAbs_BSplKnotDistribution.hxx>
#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>
#include <Standard_Integer.hxx>
#include <Standard_Real.hxx>
#include <TColgp_Array1OfPnt.hxx>
#include <TColgp_Array1OfPnt2d.hxx>
#include <TColStd_Array1OfReal.hxx>
#include <TColStd_Array2OfReal.hxx>
#include <TColStd_HArray1OfInteger.hxx>
#include <TColStd_HArray1OfReal.hxx>

class gp_Pnt;
class gp_Pnt2d;
class gp_Vec;
class gp_Vec2d;
class math_Matrix;

//! BSplCLib   B-spline curve Library.
//!
//! The BSplCLib package is  a basic library  for BSplines. It
//! provides three categories of functions.
//!
//! * Management methods to  process knots and multiplicities.
//!
//! * Multi-Dimensions  spline methods.  BSpline methods where
//! poles have an arbitrary number of dimensions. They divides
//! in two groups :
//!
//! - Global methods modifying the  whole set of  poles. The
//! poles are    described   by an array   of   Reals and  a
//! Dimension. Example : Inserting knots.
//!
//! - Local methods  computing  points and derivatives.  The
//! poles  are described by a pointer  on  a local array  of
//! Reals and a Dimension. The local array is modified.
//!
//! *  2D  and 3D spline   curves  methods.
//!
//! Methods  for 2d and 3d BSplines  curves  rational or not
//! rational.
//!
//! Those methods have the following structure :
//!
//! - They extract the pole information in a working array.
//!
//! -  They      process the  working   array    with   the
//! multi-dimension  methods. (for example  a  3d  rational
//! curve is processed as a 4 dimension curve).
//!
//! - They get back the result in the original dimension.
//!
//! Note that the  bspline   surface methods found   in the
//! package BSplSLib  uses  the same  structure and rely on
//! BSplCLib.
//!
//! In the following list  of methods the  2d and 3d  curve
//! methods   will be  described   with  the  corresponding
//! multi-dimension method.
//!
//! The 3d or 2d B-spline curve is defined with :
//!
//! . its control points : TColgp_Array1OfPnt(2d)        Poles
//! . its weights        : TColStd_Array1OfReal          Weights
//! . its knots          : TColStd_Array1OfReal          Knots
//! . its multiplicities : TColStd_Array1OfInteger       Mults
//! . its degree         : Standard_Integer              Degree
//! . its periodicity    : Standard_Boolean              Periodic
//!
//! Warnings :
//! The bounds of Poles and Weights should be the same.
//! The bounds of Knots and Mults   should be the same.
//!
//! Note: weight and multiplicity arrays can be passed by pointer for
//! some functions so that NULL pointer is valid.
//! That means no weights/no multiplicities passed.
//!
//! No weights (BSplCLib::NoWeights()) means the curve is non rational.
//! No mults (BSplCLib::NoMults()) means the knots are "flat" knots.
//!
//! KeyWords :
//! B-spline curve, Functions, Library
//!
//! References :
//! . A survey of curves and surfaces methods in CADG Wolfgang
//! BOHM CAGD 1 (1984)
//! . On de Boor-like algorithms and blossoming Wolfgang BOEHM
//! cagd 5 (1988)
//! . Blossoming and knot insertion algorithms for B-spline curves
//! Ronald N. GOLDMAN
//! . Modelisation des surfaces en CAO, Henri GIAUME Peugeot SA
//! . Curves and Surfaces for Computer Aided Geometric Design,
//! a practical guide Gerald Farin
class BSplCLib
{
public:
  DEFINE_STANDARD_ALLOC

  //! This routine searches the position of the real value theX
  //! in the monotonically increasing set of real values theArray using bisection algorithm.
  //!
  //! If the given value is out of range or array values, algorithm returns either
  //! theArray.Lower()-1 or theArray.Upper()+1 depending on theX position in the ordered set.
  //!
  //! This routine is used to locate a knot value in a set of knots.
  Standard_EXPORT static void Hunt(const TColStd_Array1OfReal& theArray,
                                   const Standard_Real         theX,
                                   Standard_Integer&           theXPos);

  //! Computes the index of the knots value which gives
  //! the start point of the curve.
  Standard_EXPORT static Standard_Integer FirstUKnotIndex(const Standard_Integer         Degree,
                                                          const TColStd_Array1OfInteger& Mults);

  //! Computes the index of the knots value which gives
  //! the end point of the curve.
  Standard_EXPORT static Standard_Integer LastUKnotIndex(const Standard_Integer         Degree,
                                                         const TColStd_Array1OfInteger& Mults);

  //! Computes the index  of  the  flats knots  sequence
  //! corresponding  to  <Index> in  the  knots sequence
  //! which multiplicities are <Mults>.
  Standard_EXPORT static Standard_Integer FlatIndex(const Standard_Integer         Degree,
                                                    const Standard_Integer         Index,
                                                    const TColStd_Array1OfInteger& Mults,
                                                    const Standard_Boolean         Periodic);

  //! Locates  the parametric value    U  in the knots
  //! sequence  between  the  knot K1   and the knot  K2.
  //! The value return in Index verifies.
  //!
  //! Knots(Index) <= U < Knots(Index + 1)
  //! if U <= Knots (K1) then Index = K1
  //! if U >= Knots (K2) then Index = K2 - 1
  //!
  //! If Periodic is True U  may be  modified  to fit in
  //! the range  Knots(K1), Knots(K2).  In any case  the
  //! correct value is returned in NewU.
  //!
  //! Warnings :Index is used  as input   data to initialize  the
  //! searching  function.
  //! Warning: Knots have to be "with repetitions"
  Standard_EXPORT static void LocateParameter(const Standard_Integer         Degree,
                                              const TColStd_Array1OfReal&    Knots,
                                              const TColStd_Array1OfInteger& Mults,
                                              const Standard_Real            U,
                                              const Standard_Boolean         IsPeriodic,
                                              const Standard_Integer         FromK1,
                                              const Standard_Integer         ToK2,
                                              Standard_Integer&              KnotIndex,
                                              Standard_Real&                 NewU);

  //! Locates  the parametric value    U  in the knots
  //! sequence  between  the  knot K1   and the knot  K2.
  //! The value return in Index verifies.
  //!
  //! Knots(Index) <= U < Knots(Index + 1)
  //! if U <= Knots (K1) then Index = K1
  //! if U >= Knots (K2) then Index = K2 - 1
  //!
  //! If Periodic is True U  may be  modified  to fit in
  //! the range  Knots(K1), Knots(K2).  In any case  the
  //! correct value is returned in NewU.
  //!
  //! Warnings :Index is used  as input   data to initialize  the
  //! searching  function.
  //! Warning: Knots have to be "flat"
  Standard_EXPORT static void LocateParameter(const Standard_Integer      Degree,
                                              const TColStd_Array1OfReal& Knots,
                                              const Standard_Real         U,
                                              const Standard_Boolean      IsPeriodic,
                                              const Standard_Integer      FromK1,
                                              const Standard_Integer      ToK2,
                                              Standard_Integer&           KnotIndex,
                                              Standard_Real&              NewU);

  Standard_EXPORT static void LocateParameter(const Standard_Integer         Degree,
                                              const TColStd_Array1OfReal&    Knots,
                                              const TColStd_Array1OfInteger* Mults,
                                              const Standard_Real            U,
                                              const Standard_Boolean         Periodic,
                                              Standard_Integer&              Index,
                                              Standard_Real&                 NewU);

  //! Finds the greatest multiplicity in a set of knots
  //! between  K1  and K2.   Mults  is  the  multiplicity
  //! associated with each knot value.
  Standard_EXPORT static Standard_Integer MaxKnotMult(const TColStd_Array1OfInteger& Mults,
                                                      const Standard_Integer         K1,
                                                      const Standard_Integer         K2);

  //! Finds the lowest multiplicity in  a  set of knots
  //! between   K1  and K2.   Mults is  the  multiplicity
  //! associated with each knot value.
  Standard_EXPORT static Standard_Integer MinKnotMult(const TColStd_Array1OfInteger& Mults,
                                                      const Standard_Integer         K1,
                                                      const Standard_Integer         K2);

  //! Returns the number of poles of the curve. Returns 0 if
  //! one of the multiplicities is incorrect.
  //!
  //! * Non positive.
  //!
  //! * Greater than Degree,  or  Degree+1  at the first and
  //! last knot of a non periodic curve.
  //!
  //! *  The  last periodicity  on  a periodic  curve is not
  //! equal to the first.
  Standard_EXPORT static Standard_Integer NbPoles(const Standard_Integer         Degree,
                                                  const Standard_Boolean         Periodic,
                                                  const TColStd_Array1OfInteger& Mults);

  //! Returns the length  of the sequence  of knots with
  //! repetition.
  //!
  //! Periodic :
  //!
  //! Sum(Mults(i), i = Mults.Lower(); i <= Mults.Upper());
  //!
  //! Non Periodic :
  //!
  //! Sum(Mults(i); i = Mults.Lower(); i < Mults.Upper())
  //! + 2 * Degree
  Standard_EXPORT static Standard_Integer KnotSequenceLength(const TColStd_Array1OfInteger& Mults,
                                                             const Standard_Integer         Degree,
                                                             const Standard_Boolean Periodic);

  Standard_EXPORT static void KnotSequence(const TColStd_Array1OfReal&    Knots,
                                           const TColStd_Array1OfInteger& Mults,
                                           TColStd_Array1OfReal&          KnotSeq,
                                           const Standard_Boolean Periodic = Standard_False);

  //! Computes  the  sequence   of knots KnotSeq  with
  //! repetition  of the  knots  of multiplicity  greater
  //! than 1.
  //!
  //! Length of KnotSeq must be KnotSequenceLength(Mults,Degree,Periodic)
  Standard_EXPORT static void KnotSequence(const TColStd_Array1OfReal&    Knots,
                                           const TColStd_Array1OfInteger& Mults,
                                           const Standard_Integer         Degree,
                                           const Standard_Boolean         Periodic,
                                           TColStd_Array1OfReal&          KnotSeq);

  //! Returns the  length  of the   sequence of  knots  (and
  //! Mults)  without repetition.
  Standard_EXPORT static Standard_Integer KnotsLength(
    const TColStd_Array1OfReal& KnotSeq,
    const Standard_Boolean      Periodic = Standard_False);

  //! Computes  the  sequence   of knots Knots  without
  //! repetition  of the  knots  of multiplicity  greater
  //! than 1.
  //!
  //! Length  of <Knots> and  <Mults> must be
  //! KnotsLength(KnotSequence,Periodic)
  Standard_EXPORT static void Knots(const TColStd_Array1OfReal& KnotSeq,
                                    TColStd_Array1OfReal&       Knots,
                                    TColStd_Array1OfInteger&    Mults,
                                    const Standard_Boolean      Periodic = Standard_False);

  //! Analyses if the  knots distribution is "Uniform"
  //! or  "NonUniform" between  the  knot  FromK1 and the
  //! knot ToK2.  There is  no repetition of  knot in the
  //! knots'sequence <Knots>.
  Standard_EXPORT static BSplCLib_KnotDistribution KnotForm(const TColStd_Array1OfReal& Knots,
                                                            const Standard_Integer      FromK1,
                                                            const Standard_Integer      ToK2);

  //! Analyses the distribution of multiplicities between
  //! the knot FromK1 and the Knot ToK2.
  Standard_EXPORT static BSplCLib_MultDistribution MultForm(const TColStd_Array1OfInteger& Mults,
                                                            const Standard_Integer         FromK1,
                                                            const Standard_Integer         ToK2);

  //! Analyzes the array of knots.
  //! Returns the form and the maximum knot multiplicity.
  Standard_EXPORT static void KnotAnalysis(const Standard_Integer         Degree,
                                           const Standard_Boolean         Periodic,
                                           const TColStd_Array1OfReal&    CKnots,
                                           const TColStd_Array1OfInteger& CMults,
                                           GeomAbs_BSplKnotDistribution&  KnotForm,
                                           Standard_Integer&              MaxKnotMult);

  //! Reparametrizes a B-spline curve to [U1, U2].
  //! The knot values are recomputed such that Knots (Lower) = U1
  //! and Knots (Upper) = U2   but the knot form is not modified.
  //! Warnings :
  //! In the array Knots the values must be in ascending order.
  //! U1 must not be equal to U2 to avoid division by zero.
  Standard_EXPORT static void Reparametrize(const Standard_Real   U1,
                                            const Standard_Real   U2,
                                            TColStd_Array1OfReal& Knots);

  //! Reverses  the  array   knots  to  become  the knots
  //! sequence of the reversed curve.
  Standard_EXPORT static void Reverse(TColStd_Array1OfReal& Knots);

  //! Reverses  the  array of multiplicities.
  Standard_EXPORT static void Reverse(TColStd_Array1OfInteger& Mults);

  //! Reverses the array of poles. Last is the  index of
  //! the new first pole. On  a  non periodic curve last
  //! is Poles.Upper(). On a periodic curve last is
  //!
  //! (number of flat knots - degree - 1)
  //!
  //! or
  //!
  //! (sum of multiplicities(but  for the last) + degree
  //! - 1)
  Standard_EXPORT static void Reverse(TColgp_Array1OfPnt& Poles, const Standard_Integer Last);

  //! Reverses the array of poles.
  Standard_EXPORT static void Reverse(TColgp_Array1OfPnt2d& Poles, const Standard_Integer Last);

  //! Reverses the array of poles.
  Standard_EXPORT static void Reverse(TColStd_Array1OfReal& Weights, const Standard_Integer Last);

  //! Returns False if all the weights  of the  array <Weights>
  //! between   I1 an I2   are  identic.   Epsilon  is used for
  //! comparing  weights. If Epsilon  is 0. the  Epsilon of the
  //! first weight is used.
  Standard_EXPORT static Standard_Boolean IsRational(const TColStd_Array1OfReal& Weights,
                                                     const Standard_Integer      I1,
                                                     const Standard_Integer      I2,
                                                     const Standard_Real         Epsilon = 0.0);

  //! returns the degree maxima for a BSplineCurve.
  static Standard_Integer MaxDegree();

  //! Perform the Boor  algorithm  to  evaluate a point at
  //! parameter <U>, with <Degree> and <Dimension>.
  //!
  //! Poles is  an array of  Reals of size
  //!
  //! <Dimension> *  <Degree>+1
  //!
  //! Containing  the poles.  At  the end <Poles> contains
  //! the current point.
  Standard_EXPORT static void Eval(const Standard_Real    U,
                                   const Standard_Integer Degree,
                                   Standard_Real&         Knots,
                                   const Standard_Integer Dimension,
                                   Standard_Real&         Poles);

  //! Performs the  Boor Algorithm  at  parameter <U> with
  //! the given <Degree> and the  array of <Knots> on  the
  //! poles <Poles> of dimension  <Dimension>.  The schema
  //! is  computed  until  level  <Depth>  on a   basis of
  //! <Length+1> poles.
  //!
  //! * Knots is an array of reals of length :
  //!
  //! <Length> + <Degree>
  //!
  //! * Poles is an array of reals of length :
  //!
  //! (2 * <Length> + 1) * <Dimension>
  //!
  //! The poles values  must be  set  in the array at the
  //! positions.
  //!
  //! 0..Dimension,
  //!
  //! 2 * Dimension ..
  //! 3 * Dimension
  //!
  //! 4  * Dimension ..
  //! 5  * Dimension
  //!
  //! ...
  //!
  //! The results are found in the array poles depending
  //! on the Depth. (See the method GetPole).
  Standard_EXPORT static void BoorScheme(const Standard_Real    U,
                                         const Standard_Integer Degree,
                                         Standard_Real&         Knots,
                                         const Standard_Integer Dimension,
                                         Standard_Real&         Poles,
                                         const Standard_Integer Depth,
                                         const Standard_Integer Length);

  //! Compute  the content of  Pole before the BoorScheme.
  //! This method is used to remove poles.
  //!
  //! U is the poles to  remove, Knots should contains the
  //! knots of the curve after knot removal.
  //!
  //! The first  and last poles  do not  change, the other
  //! poles are computed by averaging two possible values.
  //! The distance between  the  two   possible  poles  is
  //! computed, if it  is higher than <Tolerance> False is
  //! returned.
  Standard_EXPORT static Standard_Boolean AntiBoorScheme(const Standard_Real    U,
                                                         const Standard_Integer Degree,
                                                         Standard_Real&         Knots,
                                                         const Standard_Integer Dimension,
                                                         Standard_Real&         Poles,
                                                         const Standard_Integer Depth,
                                                         const Standard_Integer Length,
                                                         const Standard_Real    Tolerance);

  //! Computes   the   poles of  the    BSpline  giving the
  //! derivatives of order <Order>.
  //!
  //! The formula for the first order is
  //!
  //! Pole(i) = Degree * (Pole(i+1) - Pole(i)) /
  //! (Knots(i+Degree+1) - Knots(i+1))
  //!
  //! This formula  is repeated  (Degree  is decremented at
  //! each step).
  Standard_EXPORT static void Derivative(const Standard_Integer Degree,
                                         Standard_Real&         Knots,
                                         const Standard_Integer Dimension,
                                         const Standard_Integer Length,
                                         const Standard_Integer Order,
                                         Standard_Real&         Poles);

  //! Performs the Bohm  Algorithm at  parameter <U>. This
  //! algorithm computes the value and all the derivatives
  //! up to order N (N <= Degree).
  //!
  //! <Poles> is the original array of poles.
  //!
  //! The   result in  <Poles>  is    the value and    the
  //! derivatives.  Poles[0] is  the value,  Poles[Degree]
  //! is the last  derivative.
  Standard_EXPORT static void Bohm(const Standard_Real    U,
                                   const Standard_Integer Degree,
                                   const Standard_Integer N,
                                   Standard_Real&         Knots,
                                   const Standard_Integer Dimension,
                                   Standard_Real&         Poles);

  //! Used as argument for a non rational curve.
  static TColStd_Array1OfReal* NoWeights();

  //! Used as argument for a flatknots evaluation.
  static TColStd_Array1OfInteger* NoMults();

  //! Stores in LK the useful knots for the BoorSchem
  //! on the span Knots(Index) - Knots(Index+1)
  Standard_EXPORT static void BuildKnots(const Standard_Integer         Degree,
                                         const Standard_Integer         Index,
                                         const Standard_Boolean         Periodic,
                                         const TColStd_Array1OfReal&    Knots,
                                         const TColStd_Array1OfInteger* Mults,
                                         Standard_Real&                 LK);

  //! Return the index of the  first Pole to  use on the
  //! span  Mults(Index)  - Mults(Index+1).  This  index
  //! must be added to Poles.Lower().
  Standard_EXPORT static Standard_Integer PoleIndex(const Standard_Integer         Degree,
                                                    const Standard_Integer         Index,
                                                    const Standard_Boolean         Periodic,
                                                    const TColStd_Array1OfInteger& Mults);

  Standard_EXPORT static void BuildEval(const Standard_Integer      Degree,
                                        const Standard_Integer      Index,
                                        const TColStd_Array1OfReal& Poles,
                                        const TColStd_Array1OfReal* Weights,
                                        Standard_Real&              LP);

  Standard_EXPORT static void BuildEval(const Standard_Integer      Degree,
                                        const Standard_Integer      Index,
                                        const TColgp_Array1OfPnt&   Poles,
                                        const TColStd_Array1OfReal* Weights,
                                        Standard_Real&              LP);

  //! Copy in <LP>  the poles and  weights for  the Eval
  //! scheme. starting from  Poles(Poles.Lower()+Index)
  Standard_EXPORT static void BuildEval(const Standard_Integer      Degree,
                                        const Standard_Integer      Index,
                                        const TColgp_Array1OfPnt2d& Poles,
                                        const TColStd_Array1OfReal* Weights,
                                        Standard_Real&              LP);

  //! Copy in <LP>  poles for <Dimension>  Boor  scheme.
  //! Starting  from    <Index>     *  <Dimension>, copy
  //! <Length+1> poles.
  Standard_EXPORT static void BuildBoor(const Standard_Integer      Index,
                                        const Standard_Integer      Length,
                                        const Standard_Integer      Dimension,
                                        const TColStd_Array1OfReal& Poles,
                                        Standard_Real&              LP);

  //! Returns the index in  the Boor result array of the
  //! poles <Index>. If  the Boor  algorithm was perform
  //! with <Length> and <Depth>.
  Standard_EXPORT static Standard_Integer BoorIndex(const Standard_Integer Index,
                                                    const Standard_Integer Length,
                                                    const Standard_Integer Depth);

  //! Copy  the  pole at  position  <Index>  in  the Boor
  //! scheme of   dimension <Dimension> to  <Position> in
  //! the array <Pole>. <Position> is updated.
  Standard_EXPORT static void GetPole(const Standard_Integer Index,
                                      const Standard_Integer Length,
                                      const Standard_Integer Depth,
                                      const Standard_Integer Dimension,
                                      Standard_Real&         LocPoles,
                                      Standard_Integer&      Position,
                                      TColStd_Array1OfReal&  Pole);

  //! Returns in <NbPoles, NbKnots> the  new number of poles
  //! and  knots    if  the  sequence   of  knots <AddKnots,
  //! AddMults> is inserted in the sequence <Knots, Mults>.
  //!
  //! Epsilon is used to compare knots for equality.
  //!
  //! If Add is True  the multiplicities on  equal knots are
  //! added.
  //!
  //! If Add is False the max value of the multiplicities is
  //! kept.
  //!
  //! Return False if :
  //! The knew knots are knot increasing.
  //! The new knots are not in the range.
  Standard_EXPORT static Standard_Boolean PrepareInsertKnots(
    const Standard_Integer         Degree,
    const Standard_Boolean         Periodic,
    const TColStd_Array1OfReal&    Knots,
    const TColStd_Array1OfInteger& Mults,
    const TColStd_Array1OfReal&    AddKnots,
    const TColStd_Array1OfInteger* AddMults,
    Standard_Integer&              NbPoles,
    Standard_Integer&              NbKnots,
    const Standard_Real            Epsilon,
    const Standard_Boolean         Add = Standard_True);

  Standard_EXPORT static void InsertKnots(const Standard_Integer         Degree,
                                          const Standard_Boolean         Periodic,
                                          const Standard_Integer         Dimension,
                                          const TColStd_Array1OfReal&    Poles,
                                          const TColStd_Array1OfReal&    Knots,
                                          const TColStd_Array1OfInteger& Mults,
                                          const TColStd_Array1OfReal&    AddKnots,
                                          const TColStd_Array1OfInteger* AddMults,
                                          TColStd_Array1OfReal&          NewPoles,
                                          TColStd_Array1OfReal&          NewKnots,
                                          TColStd_Array1OfInteger&       NewMults,
                                          const Standard_Real            Epsilon,
                                          const Standard_Boolean         Add = Standard_True);

  Standard_EXPORT static void InsertKnots(const Standard_Integer         Degree,
                                          const Standard_Boolean         Periodic,
                                          const TColgp_Array1OfPnt&      Poles,
                                          const TColStd_Array1OfReal*    Weights,
                                          const TColStd_Array1OfReal&    Knots,
                                          const TColStd_Array1OfInteger& Mults,
                                          const TColStd_Array1OfReal&    AddKnots,
                                          const TColStd_Array1OfInteger* AddMults,
                                          TColgp_Array1OfPnt&            NewPoles,
                                          TColStd_Array1OfReal*          NewWeights,
                                          TColStd_Array1OfReal&          NewKnots,
                                          TColStd_Array1OfInteger&       NewMults,
                                          const Standard_Real            Epsilon,
                                          const Standard_Boolean         Add = Standard_True);

  //! Insert   a  sequence  of  knots <AddKnots> with
  //! multiplicities   <AddMults>. <AddKnots>   must  be a   non
  //! decreasing sequence and verifies :
  //!
  //! Knots(Knots.Lower()) <= AddKnots(AddKnots.Lower())
  //! Knots(Knots.Upper()) >= AddKnots(AddKnots.Upper())
  //!
  //! The NewPoles and NewWeights arrays must have a length :
  //! Poles.Length() + Sum(AddMults())
  //!
  //! When a knot  to insert is identic  to an existing knot the
  //! multiplicities   are added.
  //!
  //! Epsilon is used to test knots for equality.
  //!
  //! When AddMult is negative or null the knot is not inserted.
  //! No multiplicity will becomes higher than the degree.
  //!
  //! The new Knots and Multiplicities  are copied in <NewKnots>
  //! and  <NewMults>.
  //!
  //! All the New arrays should be correctly dimensioned.
  //!
  //! When all  the new knots  are existing knots, i.e. only the
  //! multiplicities  will  change it is   safe to  use the same
  //! arrays as input and output.
  Standard_EXPORT static void InsertKnots(const Standard_Integer         Degree,
                                          const Standard_Boolean         Periodic,
                                          const TColgp_Array1OfPnt2d&    Poles,
                                          const TColStd_Array1OfReal*    Weights,
                                          const TColStd_Array1OfReal&    Knots,
                                          const TColStd_Array1OfInteger& Mults,
                                          const TColStd_Array1OfReal&    AddKnots,
                                          const TColStd_Array1OfInteger* AddMults,
                                          TColgp_Array1OfPnt2d&          NewPoles,
                                          TColStd_Array1OfReal*          NewWeights,
                                          TColStd_Array1OfReal&          NewKnots,
                                          TColStd_Array1OfInteger&       NewMults,
                                          const Standard_Real            Epsilon,
                                          const Standard_Boolean         Add = Standard_True);

  Standard_EXPORT static void InsertKnot(const Standard_Integer         UIndex,
                                         const Standard_Real            U,
                                         const Standard_Integer         UMult,
                                         const Standard_Integer         Degree,
                                         const Standard_Boolean         Periodic,
                                         const TColgp_Array1OfPnt&      Poles,
                                         const TColStd_Array1OfReal*    Weights,
                                         const TColStd_Array1OfReal&    Knots,
                                         const TColStd_Array1OfInteger& Mults,
                                         TColgp_Array1OfPnt&            NewPoles,
                                         TColStd_Array1OfReal*          NewWeights);

  //! Insert a new knot U of multiplicity UMult in the
  //! knot sequence.
  //!
  //! The  location of the new Knot  should be given as an input
  //! data.  UIndex locates the new knot U  in the knot sequence
  //! and Knots (UIndex) < U < Knots (UIndex + 1).
  //!
  //! The new control points corresponding to this insertion are
  //! returned. Knots and Mults are not updated.
  Standard_EXPORT static void InsertKnot(const Standard_Integer         UIndex,
                                         const Standard_Real            U,
                                         const Standard_Integer         UMult,
                                         const Standard_Integer         Degree,
                                         const Standard_Boolean         Periodic,
                                         const TColgp_Array1OfPnt2d&    Poles,
                                         const TColStd_Array1OfReal*    Weights,
                                         const TColStd_Array1OfReal&    Knots,
                                         const TColStd_Array1OfInteger& Mults,
                                         TColgp_Array1OfPnt2d&          NewPoles,
                                         TColStd_Array1OfReal*          NewWeights);

  Standard_EXPORT static void RaiseMultiplicity(const Standard_Integer         KnotIndex,
                                                const Standard_Integer         Mult,
                                                const Standard_Integer         Degree,
                                                const Standard_Boolean         Periodic,
                                                const TColgp_Array1OfPnt&      Poles,
                                                const TColStd_Array1OfReal*    Weights,
                                                const TColStd_Array1OfReal&    Knots,
                                                const TColStd_Array1OfInteger& Mults,
                                                TColgp_Array1OfPnt&            NewPoles,
                                                TColStd_Array1OfReal*          NewWeights);

  //! Raise the multiplicity of knot to <UMult>.
  //!
  //! The new control points  are  returned. Knots and Mults are
  //! not updated.
  Standard_EXPORT static void RaiseMultiplicity(const Standard_Integer         KnotIndex,
                                                const Standard_Integer         Mult,
                                                const Standard_Integer         Degree,
                                                const Standard_Boolean         Periodic,
                                                const TColgp_Array1OfPnt2d&    Poles,
                                                const TColStd_Array1OfReal*    Weights,
                                                const TColStd_Array1OfReal&    Knots,
                                                const TColStd_Array1OfInteger& Mults,
                                                TColgp_Array1OfPnt2d&          NewPoles,
                                                TColStd_Array1OfReal*          NewWeights);

  Standard_EXPORT static Standard_Boolean RemoveKnot(const Standard_Integer         Index,
                                                     const Standard_Integer         Mult,
                                                     const Standard_Integer         Degree,
                                                     const Standard_Boolean         Periodic,
                                                     const Standard_Integer         Dimension,
                                                     const TColStd_Array1OfReal&    Poles,
                                                     const TColStd_Array1OfReal&    Knots,
                                                     const TColStd_Array1OfInteger& Mults,
                                                     TColStd_Array1OfReal&          NewPoles,
                                                     TColStd_Array1OfReal&          NewKnots,
                                                     TColStd_Array1OfInteger&       NewMults,
                                                     const Standard_Real            Tolerance);

  Standard_EXPORT static Standard_Boolean RemoveKnot(const Standard_Integer         Index,
                                                     const Standard_Integer         Mult,
                                                     const Standard_Integer         Degree,
                                                     const Standard_Boolean         Periodic,
                                                     const TColgp_Array1OfPnt&      Poles,
                                                     const TColStd_Array1OfReal*    Weights,
                                                     const TColStd_Array1OfReal&    Knots,
                                                     const TColStd_Array1OfInteger& Mults,
                                                     TColgp_Array1OfPnt&            NewPoles,
                                                     TColStd_Array1OfReal*          NewWeights,
                                                     TColStd_Array1OfReal&          NewKnots,
                                                     TColStd_Array1OfInteger&       NewMults,
                                                     const Standard_Real            Tolerance);

  //! Decrement the  multiplicity  of <Knots(Index)>
  //! to <Mult>. If <Mult>   is  null the   knot  is
  //! removed.
  //!
  //! As there are two ways to compute the new poles
  //! the midlle   will  be used  as  long    as the
  //! distance is lower than Tolerance.
  //!
  //! If a  distance is  bigger  than  tolerance the
  //! methods returns False  and  the new arrays are
  //! not modified.
  //!
  //! A low  tolerance can be  used  to test  if the
  //! knot  can be  removed  without  modifying  the
  //! curve.
  //!
  //! A high tolerance  can be used  to "smooth" the
  //! curve.
  Standard_EXPORT static Standard_Boolean RemoveKnot(const Standard_Integer         Index,
                                                     const Standard_Integer         Mult,
                                                     const Standard_Integer         Degree,
                                                     const Standard_Boolean         Periodic,
                                                     const TColgp_Array1OfPnt2d&    Poles,
                                                     const TColStd_Array1OfReal*    Weights,
                                                     const TColStd_Array1OfReal&    Knots,
                                                     const TColStd_Array1OfInteger& Mults,
                                                     TColgp_Array1OfPnt2d&          NewPoles,
                                                     TColStd_Array1OfReal*          NewWeights,
                                                     TColStd_Array1OfReal&          NewKnots,
                                                     TColStd_Array1OfInteger&       NewMults,
                                                     const Standard_Real            Tolerance);

  //! Returns the   number   of  knots   of  a  curve   with
  //! multiplicities <Mults> after elevating the degree from
  //! <Degree> to <NewDegree>. See the IncreaseDegree method
  //! for more comments.
  Standard_EXPORT static Standard_Integer IncreaseDegreeCountKnots(
    const Standard_Integer         Degree,
    const Standard_Integer         NewDegree,
    const Standard_Boolean         Periodic,
    const TColStd_Array1OfInteger& Mults);

  Standard_EXPORT static void IncreaseDegree(const Standard_Integer         Degree,
                                             const Standard_Integer         NewDegree,
                                             const Standard_Boolean         Periodic,
                                             const Standard_Integer         Dimension,
                                             const TColStd_Array1OfReal&    Poles,
                                             const TColStd_Array1OfReal&    Knots,
                                             const TColStd_Array1OfInteger& Mults,
                                             TColStd_Array1OfReal&          NewPoles,
                                             TColStd_Array1OfReal&          NewKnots,
                                             TColStd_Array1OfInteger&       NewMults);

  Standard_EXPORT static void IncreaseDegree(const Standard_Integer         Degree,
                                             const Standard_Integer         NewDegree,
                                             const Standard_Boolean         Periodic,
                                             const TColgp_Array1OfPnt&      Poles,
                                             const TColStd_Array1OfReal*    Weights,
                                             const TColStd_Array1OfReal&    Knots,
                                             const TColStd_Array1OfInteger& Mults,
                                             TColgp_Array1OfPnt&            NewPoles,
                                             TColStd_Array1OfReal*          NewWeights,
                                             TColStd_Array1OfReal&          NewKnots,
                                             TColStd_Array1OfInteger&       NewMults);

  Standard_EXPORT static void IncreaseDegree(const Standard_Integer         Degree,
                                             const Standard_Integer         NewDegree,
                                             const Standard_Boolean         Periodic,
                                             const TColgp_Array1OfPnt2d&    Poles,
                                             const TColStd_Array1OfReal*    Weights,
                                             const TColStd_Array1OfReal&    Knots,
                                             const TColStd_Array1OfInteger& Mults,
                                             TColgp_Array1OfPnt2d&          NewPoles,
                                             TColStd_Array1OfReal*          NewWeights,
                                             TColStd_Array1OfReal&          NewKnots,
                                             TColStd_Array1OfInteger&       NewMults);

  Standard_EXPORT static void IncreaseDegree(const Standard_Integer      NewDegree,
                                             const TColgp_Array1OfPnt&   Poles,
                                             const TColStd_Array1OfReal* Weights,
                                             TColgp_Array1OfPnt&         NewPoles,
                                             TColStd_Array1OfReal*       NewWeights);

  //! Increase the degree of a bspline (or bezier) curve
  //! of dimension theDimension form theDegree to theNewDegree.
  //!
  //! The number of poles in the new curve is:
  //! @code
  //!   Poles.Length() + (NewDegree - Degree) * Number of spans
  //! @endcode
  //! Where the number of spans is:
  //! @code
  //!   LastUKnotIndex(Mults) - FirstUKnotIndex(Mults) + 1
  //! @endcode
  //! for a non-periodic curve, and
  //! @code
  //!   Knots.Length() - 1
  //! @endcode
  //! for a periodic curve.
  //!
  //! The multiplicities of all knots are increased by the degree elevation.
  //!
  //! The new knots are usually the same knots with the
  //! exception of a non-periodic curve with the first
  //! and last multiplicity not  equal to Degree+1 where
  //! knots are removed form the start and the bottom
  //! until the sum of the multiplicities is equal to
  //! NewDegree+1  at the knots corresponding to the
  //! first and last parameters of the curve.
  //!
  //! Example: Suppose a curve of degree 3 starting
  //! with following knots and multiplicities:
  //! @code
  //!   knot : 0.  1.  2.
  //!   mult : 1   2   1
  //! @endcode
  //!
  //! The FirstUKnot is 2.0 because the sum of multiplicities is
  //! @code
  //!   Degree+1 : 1 + 2 + 1 = 4 = 3 + 1
  //! @endcode
  //! i.e. the first parameter of the curve is 2.0 and
  //! will still be 2.0 after degree elevation.
  //! Let raise this curve to degree 4.
  //! The multiplicities are increased by 2.
  //!
  //! They  become 2 3 2.
  //! But we need a sum of multiplicities of 5 at knot 2.
  //! So the first knot is removed and the new knots are:
  //! @code
  //!   knot : 1.  2.
  //!   mult : 3   2
  //! @endcode
  //! The multiplicity of the first knot may also be reduced if the sum is still too big.
  //!
  //! In the most common situations (periodic curve or curve with first
  //! and last multiplicities equals to Degree+1) the knots are knot changes.
  //!
  //! The method IncreaseDegreeCountKnots can be used to compute the new number of knots.
  Standard_EXPORT static void IncreaseDegree(const Standard_Integer      theNewDegree,
                                             const TColgp_Array1OfPnt2d& thePoles,
                                             const TColStd_Array1OfReal* theWeights,
                                             TColgp_Array1OfPnt2d&       theNewPoles,
                                             TColStd_Array1OfReal*       theNewWeights);

  //! Set in <NbKnots> and <NbPolesToAdd> the number of Knots and
  //! Poles   of  the NotPeriodic  Curve   identical  at the
  //! periodic     curve with    a  degree    <Degree>  ,  a
  //! knots-distribution with Multiplicities <Mults>.
  Standard_EXPORT static void PrepareUnperiodize(const Standard_Integer         Degree,
                                                 const TColStd_Array1OfInteger& Mults,
                                                 Standard_Integer&              NbKnots,
                                                 Standard_Integer&              NbPoles);

  Standard_EXPORT static void Unperiodize(const Standard_Integer         Degree,
                                          const Standard_Integer         Dimension,
                                          const TColStd_Array1OfInteger& Mults,
                                          const TColStd_Array1OfReal&    Knots,
                                          const TColStd_Array1OfReal&    Poles,
                                          TColStd_Array1OfInteger&       NewMults,
                                          TColStd_Array1OfReal&          NewKnots,
                                          TColStd_Array1OfReal&          NewPoles);

  Standard_EXPORT static void Unperiodize(const Standard_Integer         Degree,
                                          const TColStd_Array1OfInteger& Mults,
                                          const TColStd_Array1OfReal&    Knots,
                                          const TColgp_Array1OfPnt&      Poles,
                                          const TColStd_Array1OfReal*    Weights,
                                          TColStd_Array1OfInteger&       NewMults,
                                          TColStd_Array1OfReal&          NewKnots,
                                          TColgp_Array1OfPnt&            NewPoles,
                                          TColStd_Array1OfReal*          NewWeights);

  Standard_EXPORT static void Unperiodize(const Standard_Integer         Degree,
                                          const TColStd_Array1OfInteger& Mults,
                                          const TColStd_Array1OfReal&    Knots,
                                          const TColgp_Array1OfPnt2d&    Poles,
                                          const TColStd_Array1OfReal*    Weights,
                                          TColStd_Array1OfInteger&       NewMults,
                                          TColStd_Array1OfReal&          NewKnots,
                                          TColgp_Array1OfPnt2d&          NewPoles,
                                          TColStd_Array1OfReal*          NewWeights);

  //! Set in <NbKnots> and <NbPoles> the number of Knots and
  //! Poles of the curve resulting from  the trimming of the
  //! BSplinecurve defined with <degree>, <knots>, <mults>
  Standard_EXPORT static void PrepareTrimming(const Standard_Integer         Degree,
                                              const Standard_Boolean         Periodic,
                                              const TColStd_Array1OfReal&    Knots,
                                              const TColStd_Array1OfInteger& Mults,
                                              const Standard_Real            U1,
                                              const Standard_Real            U2,
                                              Standard_Integer&              NbKnots,
                                              Standard_Integer&              NbPoles);

  Standard_EXPORT static void Trimming(const Standard_Integer         Degree,
                                       const Standard_Boolean         Periodic,
                                       const Standard_Integer         Dimension,
                                       const TColStd_Array1OfReal&    Knots,
                                       const TColStd_Array1OfInteger& Mults,
                                       const TColStd_Array1OfReal&    Poles,
                                       const Standard_Real            U1,
                                       const Standard_Real            U2,
                                       TColStd_Array1OfReal&          NewKnots,
                                       TColStd_Array1OfInteger&       NewMults,
                                       TColStd_Array1OfReal&          NewPoles);

  Standard_EXPORT static void Trimming(const Standard_Integer         Degree,
                                       const Standard_Boolean         Periodic,
                                       const TColStd_Array1OfReal&    Knots,
                                       const TColStd_Array1OfInteger& Mults,
                                       const TColgp_Array1OfPnt&      Poles,
                                       const TColStd_Array1OfReal*    Weights,
                                       const Standard_Real            U1,
                                       const Standard_Real            U2,
                                       TColStd_Array1OfReal&          NewKnots,
                                       TColStd_Array1OfInteger&       NewMults,
                                       TColgp_Array1OfPnt&            NewPoles,
                                       TColStd_Array1OfReal*          NewWeights);

  Standard_EXPORT static void Trimming(const Standard_Integer         Degree,
                                       const Standard_Boolean         Periodic,
                                       const TColStd_Array1OfReal&    Knots,
                                       const TColStd_Array1OfInteger& Mults,
                                       const TColgp_Array1OfPnt2d&    Poles,
                                       const TColStd_Array1OfReal*    Weights,
                                       const Standard_Real            U1,
                                       const Standard_Real            U2,
                                       TColStd_Array1OfReal&          NewKnots,
                                       TColStd_Array1OfInteger&       NewMults,
                                       TColgp_Array1OfPnt2d&          NewPoles,
                                       TColStd_Array1OfReal*          NewWeights);

  Standard_EXPORT static void D0(const Standard_Real            U,
                                 const Standard_Integer         Index,
                                 const Standard_Integer         Degree,
                                 const Standard_Boolean         Periodic,
                                 const TColStd_Array1OfReal&    Poles,
                                 const TColStd_Array1OfReal*    Weights,
                                 const TColStd_Array1OfReal&    Knots,
                                 const TColStd_Array1OfInteger* Mults,
                                 Standard_Real&                 P);

  Standard_EXPORT static void D0(const Standard_Real            U,
                                 const Standard_Integer         Index,
                                 const Standard_Integer         Degree,
                                 const Standard_Boolean         Periodic,
                                 const TColgp_Array1OfPnt&      Poles,
                                 const TColStd_Array1OfReal*    Weights,
                                 const TColStd_Array1OfReal&    Knots,
                                 const TColStd_Array1OfInteger* Mults,
                                 gp_Pnt&                        P);

  Standard_EXPORT static void D0(const Standard_Real            U,
                                 const Standard_Integer         UIndex,
                                 const Standard_Integer         Degree,
                                 const Standard_Boolean         Periodic,
                                 const TColgp_Array1OfPnt2d&    Poles,
                                 const TColStd_Array1OfReal*    Weights,
                                 const TColStd_Array1OfReal&    Knots,
                                 const TColStd_Array1OfInteger* Mults,
                                 gp_Pnt2d&                      P);

  Standard_EXPORT static void D0(const Standard_Real         U,
                                 const TColgp_Array1OfPnt&   Poles,
                                 const TColStd_Array1OfReal* Weights,
                                 gp_Pnt&                     P);

  Standard_EXPORT static void D0(const Standard_Real         U,
                                 const TColgp_Array1OfPnt2d& Poles,
                                 const TColStd_Array1OfReal* Weights,
                                 gp_Pnt2d&                   P);

  Standard_EXPORT static void D1(const Standard_Real            U,
                                 const Standard_Integer         Index,
                                 const Standard_Integer         Degree,
                                 const Standard_Boolean         Periodic,
                                 const TColStd_Array1OfReal&    Poles,
                                 const TColStd_Array1OfReal*    Weights,
                                 const TColStd_Array1OfReal&    Knots,
                                 const TColStd_Array1OfInteger* Mults,
                                 Standard_Real&                 P,
                                 Standard_Real&                 V);

  Standard_EXPORT static void D1(const Standard_Real            U,
                                 const Standard_Integer         Index,
                                 const Standard_Integer         Degree,
                                 const Standard_Boolean         Periodic,
                                 const TColgp_Array1OfPnt&      Poles,
                                 const TColStd_Array1OfReal*    Weights,
                                 const TColStd_Array1OfReal&    Knots,
                                 const TColStd_Array1OfInteger* Mults,
                                 gp_Pnt&                        P,
                                 gp_Vec&                        V);

  Standard_EXPORT static void D1(const Standard_Real            U,
                                 const Standard_Integer         UIndex,
                                 const Standard_Integer         Degree,
                                 const Standard_Boolean         Periodic,
                                 const TColgp_Array1OfPnt2d&    Poles,
                                 const TColStd_Array1OfReal*    Weights,
                                 const TColStd_Array1OfReal&    Knots,
                                 const TColStd_Array1OfInteger* Mults,
                                 gp_Pnt2d&                      P,
                                 gp_Vec2d&                      V);

  Standard_EXPORT static void D1(const Standard_Real         U,
                                 const TColgp_Array1OfPnt&   Poles,
                                 const TColStd_Array1OfReal* Weights,
                                 gp_Pnt&                     P,
                                 gp_Vec&                     V);

  Standard_EXPORT static void D1(const Standard_Real         U,
                                 const TColgp_Array1OfPnt2d& Poles,
                                 const TColStd_Array1OfReal* Weights,
                                 gp_Pnt2d&                   P,
                                 gp_Vec2d&                   V);

  Standard_EXPORT static void D2(const Standard_Real            U,
                                 const Standard_Integer         Index,
                                 const Standard_Integer         Degree,
                                 const Standard_Boolean         Periodic,
                                 const TColStd_Array1OfReal&    Poles,
                                 const TColStd_Array1OfReal*    Weights,
                                 const TColStd_Array1OfReal&    Knots,
                                 const TColStd_Array1OfInteger* Mults,
                                 Standard_Real&                 P,
                                 Standard_Real&                 V1,
                                 Standard_Real&                 V2);

  Standard_EXPORT static void D2(const Standard_Real            U,
                                 const Standard_Integer         Index,
                                 const Standard_Integer         Degree,
                                 const Standard_Boolean         Periodic,
                                 const TColgp_Array1OfPnt&      Poles,
                                 const TColStd_Array1OfReal*    Weights,
                                 const TColStd_Array1OfReal&    Knots,
                                 const TColStd_Array1OfInteger* Mults,
                                 gp_Pnt&                        P,
                                 gp_Vec&                        V1,
                                 gp_Vec&                        V2);

  Standard_EXPORT static void D2(const Standard_Real            U,
                                 const Standard_Integer         UIndex,
                                 const Standard_Integer         Degree,
                                 const Standard_Boolean         Periodic,
                                 const TColgp_Array1OfPnt2d&    Poles,
                                 const TColStd_Array1OfReal*    Weights,
                                 const TColStd_Array1OfReal&    Knots,
                                 const TColStd_Array1OfInteger* Mults,
                                 gp_Pnt2d&                      P,
                                 gp_Vec2d&                      V1,
                                 gp_Vec2d&                      V2);

  Standard_EXPORT static void D2(const Standard_Real         U,
                                 const TColgp_Array1OfPnt&   Poles,
                                 const TColStd_Array1OfReal* Weights,
                                 gp_Pnt&                     P,
                                 gp_Vec&                     V1,
                                 gp_Vec&                     V2);

  Standard_EXPORT static void D2(const Standard_Real         U,
                                 const TColgp_Array1OfPnt2d& Poles,
                                 const TColStd_Array1OfReal* Weights,
                                 gp_Pnt2d&                   P,
                                 gp_Vec2d&                   V1,
                                 gp_Vec2d&                   V2);

  Standard_EXPORT static void D3(const Standard_Real            U,
                                 const Standard_Integer         Index,
                                 const Standard_Integer         Degree,
                                 const Standard_Boolean         Periodic,
                                 const TColStd_Array1OfReal&    Poles,
                                 const TColStd_Array1OfReal*    Weights,
                                 const TColStd_Array1OfReal&    Knots,
                                 const TColStd_Array1OfInteger* Mults,
                                 Standard_Real&                 P,
                                 Standard_Real&                 V1,
                                 Standard_Real&                 V2,
                                 Standard_Real&                 V3);

  Standard_EXPORT static void D3(const Standard_Real            U,
                                 const Standard_Integer         Index,
                                 const Standard_Integer         Degree,
                                 const Standard_Boolean         Periodic,
                                 const TColgp_Array1OfPnt&      Poles,
                                 const TColStd_Array1OfReal*    Weights,
                                 const TColStd_Array1OfReal&    Knots,
                                 const TColStd_Array1OfInteger* Mults,
                                 gp_Pnt&                        P,
                                 gp_Vec&                        V1,
                                 gp_Vec&                        V2,
                                 gp_Vec&                        V3);

  Standard_EXPORT static void D3(const Standard_Real            U,
                                 const Standard_Integer         UIndex,
                                 const Standard_Integer         Degree,
                                 const Standard_Boolean         Periodic,
                                 const TColgp_Array1OfPnt2d&    Poles,
                                 const TColStd_Array1OfReal*    Weights,
                                 const TColStd_Array1OfReal&    Knots,
                                 const TColStd_Array1OfInteger* Mults,
                                 gp_Pnt2d&                      P,
                                 gp_Vec2d&                      V1,
                                 gp_Vec2d&                      V2,
                                 gp_Vec2d&                      V3);

  Standard_EXPORT static void D3(const Standard_Real         U,
                                 const TColgp_Array1OfPnt&   Poles,
                                 const TColStd_Array1OfReal* Weights,
                                 gp_Pnt&                     P,
                                 gp_Vec&                     V1,
                                 gp_Vec&                     V2,
                                 gp_Vec&                     V3);

  Standard_EXPORT static void D3(const Standard_Real         U,
                                 const TColgp_Array1OfPnt2d& Poles,
                                 const TColStd_Array1OfReal* Weights,
                                 gp_Pnt2d&                   P,
                                 gp_Vec2d&                   V1,
                                 gp_Vec2d&                   V2,
                                 gp_Vec2d&                   V3);

  Standard_EXPORT static void DN(const Standard_Real            U,
                                 const Standard_Integer         N,
                                 const Standard_Integer         Index,
                                 const Standard_Integer         Degree,
                                 const Standard_Boolean         Periodic,
                                 const TColStd_Array1OfReal&    Poles,
                                 const TColStd_Array1OfReal*    Weights,
                                 const TColStd_Array1OfReal&    Knots,
                                 const TColStd_Array1OfInteger* Mults,
                                 Standard_Real&                 VN);

  Standard_EXPORT static void DN(const Standard_Real            U,
                                 const Standard_Integer         N,
                                 const Standard_Integer         Index,
                                 const Standard_Integer         Degree,
                                 const Standard_Boolean         Periodic,
                                 const TColgp_Array1OfPnt&      Poles,
                                 const TColStd_Array1OfReal*    Weights,
                                 const TColStd_Array1OfReal&    Knots,
                                 const TColStd_Array1OfInteger* Mults,
                                 gp_Vec&                        VN);

  Standard_EXPORT static void DN(const Standard_Real            U,
                                 const Standard_Integer         N,
                                 const Standard_Integer         UIndex,
                                 const Standard_Integer         Degree,
                                 const Standard_Boolean         Periodic,
                                 const TColgp_Array1OfPnt2d&    Poles,
                                 const TColStd_Array1OfReal*    Weights,
                                 const TColStd_Array1OfReal&    Knots,
                                 const TColStd_Array1OfInteger* Mults,
                                 gp_Vec2d&                      V);

  Standard_EXPORT static void DN(const Standard_Real         U,
                                 const Standard_Integer      N,
                                 const TColgp_Array1OfPnt&   Poles,
                                 const TColStd_Array1OfReal& Weights,
                                 gp_Pnt&                     P,
                                 gp_Vec&                     VN);

  //! The  above  functions  compute   values and
  //! derivatives in the following situations :
  //!
  //! * 3D, 2D and 1D
  //!
  //! * Rational or not Rational.
  //!
  //! * Knots  and multiplicities or "flat knots" without
  //! multiplicities.
  //!
  //! * The  <Index>  is   the localization  of  the
  //! parameter in the knot sequence.  If <Index> is  out
  //! of range the correct value will be searched.
  //!
  //! VERY IMPORTANT!!!
  //! USE  BSplCLib::NoWeights()  as Weights argument for non
  //! rational curves computations.
  Standard_EXPORT static void DN(const Standard_Real         U,
                                 const Standard_Integer      N,
                                 const TColgp_Array1OfPnt2d& Poles,
                                 const TColStd_Array1OfReal& Weights,
                                 gp_Pnt2d&                   P,
                                 gp_Vec2d&                   VN);

  //! This  evaluates  the Bspline  Basis  at  a
  //! given  parameter  Parameter   up   to  the
  //! requested   DerivativeOrder  and store the
  //! result  in the  array BsplineBasis  in the
  //! following   fashion
  //! BSplineBasis(1,1)   =
  //! value of first non vanishing
  //! Bspline function which has Index FirstNonZeroBsplineIndex
  //! BsplineBasis(1,2)   =
  //! value of second non vanishing
  //! Bspline   function which  has   Index
  //! FirstNonZeroBsplineIndex + 1
  //! BsplineBasis(1,n)   =
  //! value of second non vanishing non vanishing
  //! Bspline   function which  has   Index
  //! FirstNonZeroBsplineIndex + n (n <= Order)
  //! BSplineBasis(2,1)   =
  //! value of derivative of first non vanishing
  //! Bspline function which has Index FirstNonZeroBsplineIndex
  //! BSplineBasis(N,1)   =
  //! value of Nth derivative of first non vanishing
  //! Bspline function which has Index FirstNonZeroBsplineIndex
  //! if N <= DerivativeOrder + 1
  Standard_EXPORT static Standard_Integer EvalBsplineBasis(
    const Standard_Integer      DerivativeOrder,
    const Standard_Integer      Order,
    const TColStd_Array1OfReal& FlatKnots,
    const Standard_Real         Parameter,
    Standard_Integer&           FirstNonZeroBsplineIndex,
    math_Matrix&                BsplineBasis,
    const Standard_Boolean      isPeriodic = Standard_False);

  //! This Builds   a fully  blown   Matrix of
  //! (ni)
  //! Bi    (tj)
  //!
  //! with i  and j within 1..Order + NumPoles
  //! The  integer ni is   the ith slot of the
  //! array OrderArray, tj is the jth slot of
  //! the array Parameters
  Standard_EXPORT static Standard_Integer BuildBSpMatrix(const TColStd_Array1OfReal&    Parameters,
                                                         const TColStd_Array1OfInteger& OrderArray,
                                                         const TColStd_Array1OfReal&    FlatKnots,
                                                         const Standard_Integer         Degree,
                                                         math_Matrix&                   Matrix,
                                                         Standard_Integer& UpperBandWidth,
                                                         Standard_Integer& LowerBandWidth);

  //! this  factors  the Banded Matrix in
  //! the LU form with a Banded storage of
  //! components of the L matrix
  //! WARNING : do not use if the Matrix is
  //! totally positive (It is the case for
  //! Bspline matrices build as above with
  //! parameters being the Schoenberg points
  Standard_EXPORT static Standard_Integer FactorBandedMatrix(math_Matrix&           Matrix,
                                                             const Standard_Integer UpperBandWidth,
                                                             const Standard_Integer LowerBandWidth,
                                                             Standard_Integer& PivotIndexProblem);

  //! This solves  the system Matrix.X =  B
  //! with when Matrix is factored in LU form
  //! The  Array   is    an   seen   as    an
  //! Array[1..N][1..ArrayDimension] with N =
  //! the  rank  of the  matrix  Matrix.  The
  //! result is stored   in Array  when  each
  //! coordinate is  solved that is  B is the
  //! array whose values are
  //! B[i] = Array[i][p] for each p in 1..ArrayDimension
  Standard_EXPORT static Standard_Integer SolveBandedSystem(const math_Matrix&     Matrix,
                                                            const Standard_Integer UpperBandWidth,
                                                            const Standard_Integer LowerBandWidth,
                                                            const Standard_Integer ArrayDimension,
                                                            Standard_Real&         Array);

  //! This solves  the system Matrix.X =  B
  //! with when Matrix is factored in LU form
  //! The  Array   has the length of
  //! the  rank  of the  matrix  Matrix.  The
  //! result is stored   in Array  when  each
  //! coordinate is  solved that is  B is the
  //! array whose values are
  //! B[i] = Array[i][p] for each p in 1..ArrayDimension
  Standard_EXPORT static Standard_Integer SolveBandedSystem(const math_Matrix&     Matrix,
                                                            const Standard_Integer UpperBandWidth,
                                                            const Standard_Integer LowerBandWidth,
                                                            TColgp_Array1OfPnt2d&  Array);

  //! This solves  the system Matrix.X =  B
  //! with when Matrix is factored in LU form
  //! The  Array   has the length of
  //! the  rank  of the  matrix  Matrix.  The
  //! result is stored   in Array  when  each
  //! coordinate is  solved that is  B is the
  //! array whose values are
  //! B[i] = Array[i][p] for each p in 1..ArrayDimension
  Standard_EXPORT static Standard_Integer SolveBandedSystem(const math_Matrix&     Matrix,
                                                            const Standard_Integer UpperBandWidth,
                                                            const Standard_Integer LowerBandWidth,
                                                            TColgp_Array1OfPnt&    Array);

  Standard_EXPORT static Standard_Integer SolveBandedSystem(const math_Matrix&     Matrix,
                                                            const Standard_Integer UpperBandWidth,
                                                            const Standard_Integer LowerBandWidth,
                                                            const Standard_Boolean HomogenousFlag,
                                                            const Standard_Integer ArrayDimension,
                                                            Standard_Real&         Array,
                                                            Standard_Real&         Weights);

  //! This solves the  system Matrix.X =  B
  //! with when Matrix is factored in LU form
  //! The    Array   is    an   seen  as   an
  //! Array[1..N][1..ArrayDimension] with N =
  //! the  rank  of  the  matrix Matrix.  The
  //! result is  stored   in Array when  each
  //! coordinate is  solved that is B  is the
  //! array  whose   values     are   B[i]  =
  //! Array[i][p]       for     each  p    in
  //! 1..ArrayDimension. If  HomogeneousFlag ==
  //! 0  the  Poles  are  multiplied by   the
  //! Weights   upon   Entry   and      once
  //! interpolation   is    carried  over the
  //! result of the  poles are divided by the
  //! result of   the   interpolation of  the
  //! weights. Otherwise if HomogenousFlag == 1
  //! the Poles and Weights are treated homogeneously
  //! that is that those are interpolated as they
  //! are and result is returned without division
  //! by the interpolated weights.
  Standard_EXPORT static Standard_Integer SolveBandedSystem(const math_Matrix&     Matrix,
                                                            const Standard_Integer UpperBandWidth,
                                                            const Standard_Integer LowerBandWidth,
                                                            const Standard_Boolean HomogenousFlag,
                                                            TColgp_Array1OfPnt2d&  Array,
                                                            TColStd_Array1OfReal&  Weights);

  //! This solves  the system Matrix.X =  B
  //! with when Matrix is factored in LU form
  //! The  Array   is    an   seen   as    an
  //! Array[1..N][1..ArrayDimension] with N =
  //! the  rank  of the  matrix  Matrix.  The
  //! result is stored   in Array  when  each
  //! coordinate is  solved that is  B is the
  //! array whose values are
  //! B[i] = Array[i][p] for each p in 1..ArrayDimension
  //! If  HomogeneousFlag ==
  //! 0  the  Poles  are  multiplied by   the
  //! Weights   upon   Entry   and      once
  //! interpolation   is    carried  over the
  //! result of the  poles are divided by the
  //! result of   the   interpolation of  the
  //! weights. Otherwise if HomogenousFlag == 1
  //! the Poles and Weights are treated homogeneously
  //! that is that those are interpolated as they
  //! are and result is returned without division
  //! by the interpolated weights.
  Standard_EXPORT static Standard_Integer SolveBandedSystem(const math_Matrix&     Matrix,
                                                            const Standard_Integer UpperBandWidth,
                                                            const Standard_Integer LowerBandWidth,
                                                            const Standard_Boolean HomogeneousFlag,
                                                            TColgp_Array1OfPnt&    Array,
                                                            TColStd_Array1OfReal&  Weights);

  //! Merges  two knot vector by   setting the starting and
  //! ending values to StartValue and EndValue
  Standard_EXPORT static void MergeBSplineKnots(const Standard_Real               Tolerance,
                                                const Standard_Real               StartValue,
                                                const Standard_Real               EndValue,
                                                const Standard_Integer            Degree1,
                                                const TColStd_Array1OfReal&       Knots1,
                                                const TColStd_Array1OfInteger&    Mults1,
                                                const Standard_Integer            Degree2,
                                                const TColStd_Array1OfReal&       Knots2,
                                                const TColStd_Array1OfInteger&    Mults2,
                                                Standard_Integer&                 NumPoles,
                                                Handle(TColStd_HArray1OfReal)&    NewKnots,
                                                Handle(TColStd_HArray1OfInteger)& NewMults);

  //! This function will compose  a given Vectorial BSpline F(t)
  //! defined  by its  BSplineDegree and BSplineFlatKnotsl,
  //! its Poles  array which are coded as  an array of Real
  //! of  the  form  [1..NumPoles][1..PolesDimension] with  a
  //! function     a(t) which is   assumed to   satisfy the
  //! following:
  //!
  //! 1. F(a(t))  is a polynomial BSpline
  //! that can be expressed  exactly as a BSpline of degree
  //! NewDegree on the knots FlatKnots
  //!
  //! 2. a(t) defines a differentiable
  //! isomorphism between the range of FlatKnots to the range
  //! of BSplineFlatKnots which is the
  //! same as the  range of F(t)
  //!
  //! Warning: it is
  //! the caller's responsibility to insure that conditions
  //! 1. and  2. above are  satisfied : no check whatsoever
  //! is made in this method
  //!
  //! theStatus will return 0 if OK else it will return the pivot index
  //! of the matrix that was inverted to compute the multiplied
  //! BSpline : the method used is interpolation at Schoenenberg
  //! points of F(a(t))
  Standard_EXPORT static void FunctionReparameterise(const BSplCLib_EvaluatorFunction& Function,
                                                     const Standard_Integer      BSplineDegree,
                                                     const TColStd_Array1OfReal& BSplineFlatKnots,
                                                     const Standard_Integer      PolesDimension,
                                                     Standard_Real&              Poles,
                                                     const TColStd_Array1OfReal& FlatKnots,
                                                     const Standard_Integer      NewDegree,
                                                     Standard_Real&              NewPoles,
                                                     Standard_Integer&           theStatus);

  //! This function will compose  a given Vectorial BSpline F(t)
  //! defined  by its  BSplineDegree and BSplineFlatKnotsl,
  //! its Poles  array which are coded as  an array of Real
  //! of  the  form  [1..NumPoles][1..PolesDimension] with  a
  //! function     a(t) which is   assumed to   satisfy the
  //! following:
  //!
  //! 1. F(a(t))  is a polynomial BSpline
  //! that can be expressed  exactly as a BSpline of degree
  //! NewDegree on the knots FlatKnots
  //!
  //! 2. a(t) defines a differentiable
  //! isomorphism between the range of FlatKnots to the range
  //! of BSplineFlatKnots which is the
  //! same as the  range of F(t)
  //!
  //! Warning: it is
  //! the caller's responsibility to insure that conditions
  //! 1. and  2. above are  satisfied : no check whatsoever
  //! is made in this method
  //!
  //! theStatus will return 0 if OK else it will return the pivot index
  //! of the matrix that was inverted to compute the multiplied
  //! BSpline : the method used is interpolation at Schoenenberg
  //! points of F(a(t))
  Standard_EXPORT static void FunctionReparameterise(const BSplCLib_EvaluatorFunction& Function,
                                                     const Standard_Integer      BSplineDegree,
                                                     const TColStd_Array1OfReal& BSplineFlatKnots,
                                                     const TColStd_Array1OfReal& Poles,
                                                     const TColStd_Array1OfReal& FlatKnots,
                                                     const Standard_Integer      NewDegree,
                                                     TColStd_Array1OfReal&       NewPoles,
                                                     Standard_Integer&           theStatus);

  //! this will compose  a given Vectorial BSpline F(t)
  //! defined  by its  BSplineDegree and BSplineFlatKnotsl,
  //! its Poles  array which are coded as  an array of Real
  //! of  the  form  [1..NumPoles][1..PolesDimension] with  a
  //! function     a(t) which is   assumed to   satisfy the
  //! following  : 1. F(a(t))  is a polynomial BSpline
  //! that can be expressed  exactly as a BSpline of degree
  //! NewDegree on the knots FlatKnots
  //! 2. a(t) defines a differentiable
  //! isomorphism between the range of FlatKnots to the range
  //! of BSplineFlatKnots which is the
  //! same as the  range of F(t)
  //! Warning: it is
  //! the caller's responsibility to insure that conditions
  //! 1. and  2. above are  satisfied : no check whatsoever
  //! is made in this method
  //! theStatus will return 0 if OK else it will return the pivot index
  //! of the matrix that was inverted to compute the multiplied
  //! BSpline : the method used is interpolation at Schoenenberg
  //! points of F(a(t))
  Standard_EXPORT static void FunctionReparameterise(const BSplCLib_EvaluatorFunction& Function,
                                                     const Standard_Integer      BSplineDegree,
                                                     const TColStd_Array1OfReal& BSplineFlatKnots,
                                                     const TColgp_Array1OfPnt&   Poles,
                                                     const TColStd_Array1OfReal& FlatKnots,
                                                     const Standard_Integer      NewDegree,
                                                     TColgp_Array1OfPnt&         NewPoles,
                                                     Standard_Integer&           theStatus);

  //! this will compose  a given Vectorial BSpline F(t)
  //! defined  by its  BSplineDegree and BSplineFlatKnotsl,
  //! its Poles  array which are coded as  an array of Real
  //! of  the  form  [1..NumPoles][1..PolesDimension] with  a
  //! function     a(t) which is   assumed to   satisfy the
  //! following  : 1. F(a(t))  is a polynomial BSpline
  //! that can be expressed  exactly as a BSpline of degree
  //! NewDegree on the knots FlatKnots
  //! 2. a(t) defines a differentiable
  //! isomorphism between the range of FlatKnots to the range
  //! of BSplineFlatKnots which is the
  //! same as the  range of F(t)
  //! Warning: it is
  //! the caller's responsibility to insure that conditions
  //! 1. and  2. above are  satisfied : no check whatsoever
  //! is made in this method
  //! theStatus will return 0 if OK else it will return the pivot index
  //! of the matrix that was inverted to compute the multiplied
  //! BSpline : the method used is interpolation at Schoenenberg
  //! points of F(a(t))
  Standard_EXPORT static void FunctionReparameterise(const BSplCLib_EvaluatorFunction& Function,
                                                     const Standard_Integer      BSplineDegree,
                                                     const TColStd_Array1OfReal& BSplineFlatKnots,
                                                     const TColgp_Array1OfPnt2d& Poles,
                                                     const TColStd_Array1OfReal& FlatKnots,
                                                     const Standard_Integer      NewDegree,
                                                     TColgp_Array1OfPnt2d&       NewPoles,
                                                     Standard_Integer&           theStatus);

  //! this will  multiply a given Vectorial BSpline F(t)
  //! defined  by its  BSplineDegree and BSplineFlatKnotsl,
  //! its Poles  array which are coded as  an array of Real
  //! of  the  form  [1..NumPoles][1..PolesDimension] by  a
  //! function     a(t) which is   assumed to   satisfy the
  //! following  : 1. a(t)  * F(t)  is a polynomial BSpline
  //! that can be expressed  exactly as a BSpline of degree
  //! NewDegree on the knots FlatKnots 2. the range of a(t)
  //! is the same as the  range of F(t)
  //! Warning: it is
  //! the caller's responsibility to insure that conditions
  //! 1. and  2. above are  satisfied : no check whatsoever
  //! is made in this method
  //! theStatus will return 0 if OK else it will return the pivot index
  //! of the matrix that was inverted to compute the multiplied
  //! BSpline : the method used is interpolation at Schoenenberg
  //! points of a(t)*F(t)
  Standard_EXPORT static void FunctionMultiply(const BSplCLib_EvaluatorFunction& Function,
                                               const Standard_Integer            BSplineDegree,
                                               const TColStd_Array1OfReal&       BSplineFlatKnots,
                                               const Standard_Integer            PolesDimension,
                                               Standard_Real&                    Poles,
                                               const TColStd_Array1OfReal&       FlatKnots,
                                               const Standard_Integer            NewDegree,
                                               Standard_Real&                    NewPoles,
                                               Standard_Integer&                 theStatus);

  //! this will  multiply a given Vectorial BSpline F(t)
  //! defined  by its  BSplineDegree and BSplineFlatKnotsl,
  //! its Poles  array which are coded as  an array of Real
  //! of  the  form  [1..NumPoles][1..PolesDimension] by  a
  //! function     a(t) which is   assumed to   satisfy the
  //! following  : 1. a(t)  * F(t)  is a polynomial BSpline
  //! that can be expressed  exactly as a BSpline of degree
  //! NewDegree on the knots FlatKnots 2. the range of a(t)
  //! is the same as the  range of F(t)
  //! Warning: it is
  //! the caller's responsibility to insure that conditions
  //! 1. and  2. above are  satisfied : no check whatsoever
  //! is made in this method
  //! theStatus will return 0 if OK else it will return the pivot index
  //! of the matrix that was inverted to compute the multiplied
  //! BSpline : the method used is interpolation at Schoenenberg
  //! points of a(t)*F(t)
  Standard_EXPORT static void FunctionMultiply(const BSplCLib_EvaluatorFunction& Function,
                                               const Standard_Integer            BSplineDegree,
                                               const TColStd_Array1OfReal&       BSplineFlatKnots,
                                               const TColStd_Array1OfReal&       Poles,
                                               const TColStd_Array1OfReal&       FlatKnots,
                                               const Standard_Integer            NewDegree,
                                               TColStd_Array1OfReal&             NewPoles,
                                               Standard_Integer&                 theStatus);

  //! this will  multiply a given Vectorial BSpline F(t)
  //! defined  by its  BSplineDegree and BSplineFlatKnotsl,
  //! its Poles  array which are coded as  an array of Real
  //! of  the  form  [1..NumPoles][1..PolesDimension] by  a
  //! function     a(t) which is   assumed to   satisfy the
  //! following  : 1. a(t)  * F(t)  is a polynomial BSpline
  //! that can be expressed  exactly as a BSpline of degree
  //! NewDegree on the knots FlatKnots 2. the range of a(t)
  //! is the same as the  range of F(t)
  //! Warning: it is
  //! the caller's responsibility to insure that conditions
  //! 1. and  2. above are  satisfied : no check whatsoever
  //! is made in this method
  //! theStatus will return 0 if OK else it will return the pivot index
  //! of the matrix that was inverted to compute the multiplied
  //! BSpline : the method used is interpolation at Schoenenberg
  //! points of a(t)*F(t)
  Standard_EXPORT static void FunctionMultiply(const BSplCLib_EvaluatorFunction& Function,
                                               const Standard_Integer            BSplineDegree,
                                               const TColStd_Array1OfReal&       BSplineFlatKnots,
                                               const TColgp_Array1OfPnt2d&       Poles,
                                               const TColStd_Array1OfReal&       FlatKnots,
                                               const Standard_Integer            NewDegree,
                                               TColgp_Array1OfPnt2d&             NewPoles,
                                               Standard_Integer&                 theStatus);

  //! this will  multiply a given Vectorial BSpline F(t)
  //! defined  by its  BSplineDegree and BSplineFlatKnotsl,
  //! its Poles  array which are coded as  an array of Real
  //! of  the  form  [1..NumPoles][1..PolesDimension] by  a
  //! function     a(t) which is   assumed to   satisfy the
  //! following  : 1. a(t)  * F(t)  is a polynomial BSpline
  //! that can be expressed  exactly as a BSpline of degree
  //! NewDegree on the knots FlatKnots 2. the range of a(t)
  //! is the same as the  range of F(t)
  //! Warning: it is
  //! the caller's responsibility to insure that conditions
  //! 1. and  2. above are  satisfied : no check whatsoever
  //! is made in this method
  //! theStatus will return 0 if OK else it will return the pivot index
  //! of the matrix that was inverted to compute the multiplied
  //! BSpline : the method used is interpolation at Schoenenberg
  //! points of a(t)*F(t)
  Standard_EXPORT static void FunctionMultiply(const BSplCLib_EvaluatorFunction& Function,
                                               const Standard_Integer            BSplineDegree,
                                               const TColStd_Array1OfReal&       BSplineFlatKnots,
                                               const TColgp_Array1OfPnt&         Poles,
                                               const TColStd_Array1OfReal&       FlatKnots,
                                               const Standard_Integer            NewDegree,
                                               TColgp_Array1OfPnt&               NewPoles,
                                               Standard_Integer&                 theStatus);

  //! Perform the De Boor   algorithm  to  evaluate a point at
  //! parameter <U>, with <Degree> and <Dimension>.
  //!
  //! Poles is  an array of  Reals of size
  //!
  //! <Dimension> *  <Degree>+1
  //!
  //! Containing the  poles.  At  the end <Poles> contains
  //! the current point.   Poles Contain all  the poles of
  //! the BsplineCurve, Knots  also Contains all the knots
  //! of the BsplineCurve.  ExtrapMode has two slots [0] =
  //! Degree used to extrapolate before the first knot [1]
  //! = Degre used to  extrapolate after the last knot has
  //! to be between 1 and  Degree
  Standard_EXPORT static void Eval(const Standard_Real         U,
                                   const Standard_Boolean      PeriodicFlag,
                                   const Standard_Integer      DerivativeRequest,
                                   Standard_Integer&           ExtrapMode,
                                   const Standard_Integer      Degree,
                                   const TColStd_Array1OfReal& FlatKnots,
                                   const Standard_Integer      ArrayDimension,
                                   Standard_Real&              Poles,
                                   Standard_Real&              Result);

  //! Perform the  De Boor algorithm  to evaluate a point at
  //! parameter   <U>,  with   <Degree>    and  <Dimension>.
  //! Evaluates by multiplying the  Poles by the Weights and
  //! gives  the homogeneous  result  in PolesResult that is
  //! the results of the evaluation of the numerator once it
  //! has     been  multiplied   by  the     weights and  in
  //! WeightsResult one has  the result of the evaluation of
  //! the denominator
  //!
  //! Warning:   <PolesResult> and <WeightsResult>  must be   dimensioned
  //! properly.
  Standard_EXPORT static void Eval(const Standard_Real         U,
                                   const Standard_Boolean      PeriodicFlag,
                                   const Standard_Integer      DerivativeRequest,
                                   Standard_Integer&           ExtrapMode,
                                   const Standard_Integer      Degree,
                                   const TColStd_Array1OfReal& FlatKnots,
                                   const Standard_Integer      ArrayDimension,
                                   Standard_Real&              Poles,
                                   Standard_Real&              Weights,
                                   Standard_Real&              PolesResult,
                                   Standard_Real&              WeightsResult);

  //! Perform the evaluation of the Bspline Basis
  //! and then multiplies by the weights
  //! this just evaluates the current point
  Standard_EXPORT static void Eval(const Standard_Real         U,
                                   const Standard_Boolean      PeriodicFlag,
                                   const Standard_Boolean      HomogeneousFlag,
                                   Standard_Integer&           ExtrapMode,
                                   const Standard_Integer      Degree,
                                   const TColStd_Array1OfReal& FlatKnots,
                                   const TColgp_Array1OfPnt&   Poles,
                                   const TColStd_Array1OfReal& Weights,
                                   gp_Pnt&                     Point,
                                   Standard_Real&              Weight);

  //! Perform the evaluation of the Bspline Basis
  //! and then multiplies by the weights
  //! this just evaluates the current point
  Standard_EXPORT static void Eval(const Standard_Real         U,
                                   const Standard_Boolean      PeriodicFlag,
                                   const Standard_Boolean      HomogeneousFlag,
                                   Standard_Integer&           ExtrapMode,
                                   const Standard_Integer      Degree,
                                   const TColStd_Array1OfReal& FlatKnots,
                                   const TColgp_Array1OfPnt2d& Poles,
                                   const TColStd_Array1OfReal& Weights,
                                   gp_Pnt2d&                   Point,
                                   Standard_Real&              Weight);

  //! Extend a BSpline nD using the tangency map
  //! <C1Coefficient> is the coefficient of reparametrisation
  //! <Continuity> must be equal to 1, 2 or 3.
  //! <Degree> must be greater or equal than <Continuity> + 1.
  //!
  //! Warning:   <KnotsResult> and <PolesResult>  must be   dimensioned
  //! properly.
  Standard_EXPORT static void TangExtendToConstraint(const TColStd_Array1OfReal& FlatKnots,
                                                     const Standard_Real         C1Coefficient,
                                                     const Standard_Integer      NumPoles,
                                                     Standard_Real&              Poles,
                                                     const Standard_Integer      Dimension,
                                                     const Standard_Integer      Degree,
                                                     const TColStd_Array1OfReal& ConstraintPoint,
                                                     const Standard_Integer      Continuity,
                                                     const Standard_Boolean      After,
                                                     Standard_Integer&           NbPolesResult,
                                                     Standard_Integer&           NbKnotsRsult,
                                                     Standard_Real&              KnotsResult,
                                                     Standard_Real&              PolesResult);

  //! Perform the evaluation of the of the cache
  //! the parameter must be normalized between
  //! the 0 and 1 for the span.
  //! The Cache must be valid when calling this
  //! routine. Geom Package will insure that.
  //! and then multiplies by the weights
  //! this just evaluates the current point
  //! the CacheParameter is where the Cache was
  //! constructed the SpanLength is to normalize
  //! the polynomial in the cache to avoid bad conditioning
  //! effects
  Standard_EXPORT static void CacheD0(const Standard_Real         U,
                                      const Standard_Integer      Degree,
                                      const Standard_Real         CacheParameter,
                                      const Standard_Real         SpanLenght,
                                      const TColgp_Array1OfPnt&   Poles,
                                      const TColStd_Array1OfReal* Weights,
                                      gp_Pnt&                     Point);

  //! Perform the evaluation of the Bspline Basis
  //! and then multiplies by the weights
  //! this just evaluates the current point
  //! the parameter must be normalized between
  //! the 0 and 1 for the span.
  //! The Cache must be valid when calling this
  //! routine. Geom Package will insure that.
  //! and then multiplies by the weights
  //! ththe CacheParameter is where the Cache was
  //! constructed the SpanLength is to normalize
  //! the polynomial in the cache to avoid bad conditioning
  //! effectsis just evaluates the current point
  Standard_EXPORT static void CacheD0(const Standard_Real         U,
                                      const Standard_Integer      Degree,
                                      const Standard_Real         CacheParameter,
                                      const Standard_Real         SpanLenght,
                                      const TColgp_Array1OfPnt2d& Poles,
                                      const TColStd_Array1OfReal* Weights,
                                      gp_Pnt2d&                   Point);

  //! Calls CacheD0 for Bezier  Curves Arrays computed with
  //! the method PolesCoefficients.
  //! Warning: To be used for Beziercurves ONLY!!!
  static void CoefsD0(const Standard_Real         U,
                      const TColgp_Array1OfPnt&   Poles,
                      const TColStd_Array1OfReal* Weights,
                      gp_Pnt&                     Point);

  //! Calls CacheD0 for Bezier  Curves Arrays computed with
  //! the method PolesCoefficients.
  //! Warning: To be used for Beziercurves ONLY!!!
  static void CoefsD0(const Standard_Real         U,
                      const TColgp_Array1OfPnt2d& Poles,
                      const TColStd_Array1OfReal* Weights,
                      gp_Pnt2d&                   Point);

  //! Perform the evaluation of the of the cache
  //! the parameter must be normalized between
  //! the 0 and 1 for the span.
  //! The Cache must be valid when calling this
  //! routine. Geom Package will insure that.
  //! and then multiplies by the weights
  //! this just evaluates the current point
  //! the CacheParameter is where the Cache was
  //! constructed the SpanLength is to normalize
  //! the polynomial in the cache to avoid bad conditioning
  //! effects
  Standard_EXPORT static void CacheD1(const Standard_Real         U,
                                      const Standard_Integer      Degree,
                                      const Standard_Real         CacheParameter,
                                      const Standard_Real         SpanLenght,
                                      const TColgp_Array1OfPnt&   Poles,
                                      const TColStd_Array1OfReal* Weights,
                                      gp_Pnt&                     Point,
                                      gp_Vec&                     Vec);

  //! Perform the evaluation of the Bspline Basis
  //! and then multiplies by the weights
  //! this just evaluates the current point
  //! the parameter must be normalized between
  //! the 0 and 1 for the span.
  //! The Cache must be valid when calling this
  //! routine. Geom Package will insure that.
  //! and then multiplies by the weights
  //! ththe CacheParameter is where the Cache was
  //! constructed the SpanLength is to normalize
  //! the polynomial in the cache to avoid bad conditioning
  //! effectsis just evaluates the current point
  Standard_EXPORT static void CacheD1(const Standard_Real         U,
                                      const Standard_Integer      Degree,
                                      const Standard_Real         CacheParameter,
                                      const Standard_Real         SpanLenght,
                                      const TColgp_Array1OfPnt2d& Poles,
                                      const TColStd_Array1OfReal* Weights,
                                      gp_Pnt2d&                   Point,
                                      gp_Vec2d&                   Vec);

  //! Calls CacheD1 for Bezier  Curves Arrays computed with
  //! the method PolesCoefficients.
  //! Warning: To be used for Beziercurves ONLY!!!
  static void CoefsD1(const Standard_Real         U,
                      const TColgp_Array1OfPnt&   Poles,
                      const TColStd_Array1OfReal* Weights,
                      gp_Pnt&                     Point,
                      gp_Vec&                     Vec);

  //! Calls CacheD1 for Bezier  Curves Arrays computed with
  //! the method PolesCoefficients.
  //! Warning: To be used for Beziercurves ONLY!!!
  static void CoefsD1(const Standard_Real         U,
                      const TColgp_Array1OfPnt2d& Poles,
                      const TColStd_Array1OfReal* Weights,
                      gp_Pnt2d&                   Point,
                      gp_Vec2d&                   Vec);

  //! Perform the evaluation of the of the cache
  //! the parameter must be normalized between
  //! the 0 and 1 for the span.
  //! The Cache must be valid when calling this
  //! routine. Geom Package will insure that.
  //! and then multiplies by the weights
  //! this just evaluates the current point
  //! the CacheParameter is where the Cache was
  //! constructed the SpanLength is to normalize
  //! the polynomial in the cache to avoid bad conditioning
  //! effects
  Standard_EXPORT static void CacheD2(const Standard_Real         U,
                                      const Standard_Integer      Degree,
                                      const Standard_Real         CacheParameter,
                                      const Standard_Real         SpanLenght,
                                      const TColgp_Array1OfPnt&   Poles,
                                      const TColStd_Array1OfReal* Weights,
                                      gp_Pnt&                     Point,
                                      gp_Vec&                     Vec1,
                                      gp_Vec&                     Vec2);

  //! Perform the evaluation of the Bspline Basis
  //! and then multiplies by the weights
  //! this just evaluates the current point
  //! the parameter must be normalized between
  //! the 0 and 1 for the span.
  //! The Cache must be valid when calling this
  //! routine. Geom Package will insure that.
  //! and then multiplies by the weights
  //! ththe CacheParameter is where the Cache was
  //! constructed the SpanLength is to normalize
  //! the polynomial in the cache to avoid bad conditioning
  //! effectsis just evaluates the current point
  Standard_EXPORT static void CacheD2(const Standard_Real         U,
                                      const Standard_Integer      Degree,
                                      const Standard_Real         CacheParameter,
                                      const Standard_Real         SpanLenght,
                                      const TColgp_Array1OfPnt2d& Poles,
                                      const TColStd_Array1OfReal* Weights,
                                      gp_Pnt2d&                   Point,
                                      gp_Vec2d&                   Vec1,
                                      gp_Vec2d&                   Vec2);

  //! Calls CacheD1 for Bezier  Curves Arrays computed with
  //! the method PolesCoefficients.
  //! Warning: To be used for Beziercurves ONLY!!!
  static void CoefsD2(const Standard_Real         U,
                      const TColgp_Array1OfPnt&   Poles,
                      const TColStd_Array1OfReal* Weights,
                      gp_Pnt&                     Point,
                      gp_Vec&                     Vec1,
                      gp_Vec&                     Vec2);

  //! Calls CacheD1 for Bezier  Curves Arrays computed with
  //! the method PolesCoefficients.
  //! Warning: To be used for Beziercurves ONLY!!!
  static void CoefsD2(const Standard_Real         U,
                      const TColgp_Array1OfPnt2d& Poles,
                      const TColStd_Array1OfReal* Weights,
                      gp_Pnt2d&                   Point,
                      gp_Vec2d&                   Vec1,
                      gp_Vec2d&                   Vec2);

  //! Perform the evaluation of the of the cache
  //! the parameter must be normalized between
  //! the 0 and 1 for the span.
  //! The Cache must be valid when calling this
  //! routine. Geom Package will insure that.
  //! and then multiplies by the weights
  //! this just evaluates the current point
  //! the CacheParameter is where the Cache was
  //! constructed the SpanLength is to normalize
  //! the polynomial in the cache to avoid bad conditioning
  //! effects
  Standard_EXPORT static void CacheD3(const Standard_Real         U,
                                      const Standard_Integer      Degree,
                                      const Standard_Real         CacheParameter,
                                      const Standard_Real         SpanLenght,
                                      const TColgp_Array1OfPnt&   Poles,
                                      const TColStd_Array1OfReal* Weights,
                                      gp_Pnt&                     Point,
                                      gp_Vec&                     Vec1,
                                      gp_Vec&                     Vec2,
                                      gp_Vec&                     Vec3);

  //! Perform the evaluation of the Bspline Basis
  //! and then multiplies by the weights
  //! this just evaluates the current point
  //! the parameter must be normalized between
  //! the 0 and 1 for the span.
  //! The Cache must be valid when calling this
  //! routine. Geom Package will insure that.
  //! and then multiplies by the weights
  //! ththe CacheParameter is where the Cache was
  //! constructed the SpanLength is to normalize
  //! the polynomial in the cache to avoid bad conditioning
  //! effectsis just evaluates the current point
  Standard_EXPORT static void CacheD3(const Standard_Real         U,
                                      const Standard_Integer      Degree,
                                      const Standard_Real         CacheParameter,
                                      const Standard_Real         SpanLenght,
                                      const TColgp_Array1OfPnt2d& Poles,
                                      const TColStd_Array1OfReal* Weights,
                                      gp_Pnt2d&                   Point,
                                      gp_Vec2d&                   Vec1,
                                      gp_Vec2d&                   Vec2,
                                      gp_Vec2d&                   Vec3);

  //! Calls CacheD1 for Bezier  Curves Arrays computed with
  //! the method PolesCoefficients.
  //! Warning: To be used for Beziercurves ONLY!!!
  static void CoefsD3(const Standard_Real         U,
                      const TColgp_Array1OfPnt&   Poles,
                      const TColStd_Array1OfReal* Weights,
                      gp_Pnt&                     Point,
                      gp_Vec&                     Vec1,
                      gp_Vec&                     Vec2,
                      gp_Vec&                     Vec3);

  //! Calls CacheD1 for Bezier  Curves Arrays computed with
  //! the method PolesCoefficients.
  //! Warning: To be used for Beziercurves ONLY!!!
  static void CoefsD3(const Standard_Real         U,
                      const TColgp_Array1OfPnt2d& Poles,
                      const TColStd_Array1OfReal* Weights,
                      gp_Pnt2d&                   Point,
                      gp_Vec2d&                   Vec1,
                      gp_Vec2d&                   Vec2,
                      gp_Vec2d&                   Vec3);

  //! Perform the evaluation of the Taylor expansion
  //! of the Bspline normalized between 0 and 1.
  //! If rational computes the homogeneous Taylor expansion
  //! for the numerator and stores it in CachePoles
  Standard_EXPORT static void BuildCache(const Standard_Real         U,
                                         const Standard_Real         InverseOfSpanDomain,
                                         const Standard_Boolean      PeriodicFlag,
                                         const Standard_Integer      Degree,
                                         const TColStd_Array1OfReal& FlatKnots,
                                         const TColgp_Array1OfPnt&   Poles,
                                         const TColStd_Array1OfReal* Weights,
                                         TColgp_Array1OfPnt&         CachePoles,
                                         TColStd_Array1OfReal*       CacheWeights);

  //! Perform the evaluation of the Taylor expansion
  //! of the Bspline normalized between 0 and 1.
  //! If rational computes the homogeneous Taylor expansion
  //! for the numerator and stores it in CachePoles
  Standard_EXPORT static void BuildCache(const Standard_Real         U,
                                         const Standard_Real         InverseOfSpanDomain,
                                         const Standard_Boolean      PeriodicFlag,
                                         const Standard_Integer      Degree,
                                         const TColStd_Array1OfReal& FlatKnots,
                                         const TColgp_Array1OfPnt2d& Poles,
                                         const TColStd_Array1OfReal* Weights,
                                         TColgp_Array1OfPnt2d&       CachePoles,
                                         TColStd_Array1OfReal*       CacheWeights);

  //! Perform the evaluation of the Taylor expansion
  //! of the Bspline normalized between 0 and 1.
  //! Structure of result optimized for BSplCLib_Cache.
  Standard_EXPORT static void BuildCache(const Standard_Real         theParameter,
                                         const Standard_Real         theSpanDomain,
                                         const Standard_Boolean      thePeriodicFlag,
                                         const Standard_Integer      theDegree,
                                         const Standard_Integer      theSpanIndex,
                                         const TColStd_Array1OfReal& theFlatKnots,
                                         const TColgp_Array1OfPnt&   thePoles,
                                         const TColStd_Array1OfReal* theWeights,
                                         TColStd_Array2OfReal&       theCacheArray);

  //! Perform the evaluation of the Taylor expansion
  //! of the Bspline normalized between 0 and 1.
  //! Structure of result optimized for BSplCLib_Cache.
  Standard_EXPORT static void BuildCache(const Standard_Real         theParameter,
                                         const Standard_Real         theSpanDomain,
                                         const Standard_Boolean      thePeriodicFlag,
                                         const Standard_Integer      theDegree,
                                         const Standard_Integer      theSpanIndex,
                                         const TColStd_Array1OfReal& theFlatKnots,
                                         const TColgp_Array1OfPnt2d& thePoles,
                                         const TColStd_Array1OfReal* theWeights,
                                         TColStd_Array2OfReal&       theCacheArray);

  static void PolesCoefficients(const TColgp_Array1OfPnt2d& Poles,
                                TColgp_Array1OfPnt2d&       CachePoles);

  Standard_EXPORT static void PolesCoefficients(const TColgp_Array1OfPnt2d& Poles,
                                                const TColStd_Array1OfReal* Weights,
                                                TColgp_Array1OfPnt2d&       CachePoles,
                                                TColStd_Array1OfReal*       CacheWeights);

  static void PolesCoefficients(const TColgp_Array1OfPnt& Poles, TColgp_Array1OfPnt& CachePoles);

  //! Encapsulation   of  BuildCache    to   perform   the
  //! evaluation  of the Taylor expansion for beziercurves
  //! at parameter 0.
  //! Warning: To be used for Beziercurves ONLY!!!
  Standard_EXPORT static void PolesCoefficients(const TColgp_Array1OfPnt&   Poles,
                                                const TColStd_Array1OfReal* Weights,
                                                TColgp_Array1OfPnt&         CachePoles,
                                                TColStd_Array1OfReal*       CacheWeights);

  //! Returns pointer to statically allocated array representing
  //! flat knots for bezier curve of the specified degree.
  //! Raises OutOfRange if Degree > MaxDegree()
  Standard_EXPORT static const Standard_Real& FlatBezierKnots(const Standard_Integer Degree);

  //! builds the Schoenberg points from the flat knot
  //! used to interpolate a BSpline since the
  //! BSpline matrix is invertible.
  Standard_EXPORT static void BuildSchoenbergPoints(const Standard_Integer      Degree,
                                                    const TColStd_Array1OfReal& FlatKnots,
                                                    TColStd_Array1OfReal&       Parameters);

  //! Performs the interpolation of  the data given in
  //! the Poles  array  according  to the  requests in
  //! ContactOrderArray    that is      :           if
  //! ContactOrderArray(i) has value  d it means  that
  //! Poles(i)   contains the dth  derivative of  the
  //! function to be interpolated. The length L of the
  //! following arrays must be the same :
  //! Parameters, ContactOrderArray, Poles,
  //! The length of FlatKnots is Degree + L + 1
  //! Warning:
  //! the method used to do that interpolation is
  //! gauss elimination WITHOUT pivoting. Thus if the
  //! diagonal is not dominant there is no guarantee
  //! that the algorithm will work. Nevertheless for
  //! Cubic interpolation or interpolation at Scheonberg
  //! points the method will work
  //! The InversionProblem will report 0 if there was no
  //! problem else it will give the index of the faulty
  //! pivot
  Standard_EXPORT static void Interpolate(const Standard_Integer         Degree,
                                          const TColStd_Array1OfReal&    FlatKnots,
                                          const TColStd_Array1OfReal&    Parameters,
                                          const TColStd_Array1OfInteger& ContactOrderArray,
                                          TColgp_Array1OfPnt&            Poles,
                                          Standard_Integer&              InversionProblem);

  //! Performs the interpolation of  the data given in
  //! the Poles  array  according  to the  requests in
  //! ContactOrderArray    that is      :           if
  //! ContactOrderArray(i) has value  d it means  that
  //! Poles(i)   contains the dth  derivative of  the
  //! function to be interpolated. The length L of the
  //! following arrays must be the same :
  //! Parameters, ContactOrderArray, Poles,
  //! The length of FlatKnots is Degree + L + 1
  //! Warning:
  //! the method used to do that interpolation is
  //! gauss elimination WITHOUT pivoting. Thus if the
  //! diagonal is not dominant there is no guarantee
  //! that the algorithm will work. Nevertheless for
  //! Cubic interpolation at knots or interpolation at Scheonberg
  //! points the method will work.
  //! The InversionProblem w
  //! ll report 0 if there was no
  //! problem else it will give the index of the faulty
  //! pivot
  Standard_EXPORT static void Interpolate(const Standard_Integer         Degree,
                                          const TColStd_Array1OfReal&    FlatKnots,
                                          const TColStd_Array1OfReal&    Parameters,
                                          const TColStd_Array1OfInteger& ContactOrderArray,
                                          TColgp_Array1OfPnt2d&          Poles,
                                          Standard_Integer&              InversionProblem);

  //! Performs the interpolation of  the data given in
  //! the Poles  array  according  to the  requests in
  //! ContactOrderArray    that is      :           if
  //! ContactOrderArray(i) has value  d it means  that
  //! Poles(i)   contains the dth  derivative of  the
  //! function to be interpolated. The length L of the
  //! following arrays must be the same :
  //! Parameters, ContactOrderArray, Poles,
  //! The length of FlatKnots is Degree + L + 1
  //! Warning:
  //! the method used to do that interpolation is
  //! gauss elimination WITHOUT pivoting. Thus if the
  //! diagonal is not dominant there is no guarantee
  //! that the algorithm will work. Nevertheless for
  //! Cubic interpolation at knots or interpolation at Scheonberg
  //! points the method will work.
  //! The InversionProblem will report 0 if there was no
  //! problem else it will give the index of the faulty
  //! pivot
  Standard_EXPORT static void Interpolate(const Standard_Integer         Degree,
                                          const TColStd_Array1OfReal&    FlatKnots,
                                          const TColStd_Array1OfReal&    Parameters,
                                          const TColStd_Array1OfInteger& ContactOrderArray,
                                          TColgp_Array1OfPnt&            Poles,
                                          TColStd_Array1OfReal&          Weights,
                                          Standard_Integer&              InversionProblem);

  //! Performs the interpolation of  the data given in
  //! the Poles  array  according  to the  requests in
  //! ContactOrderArray    that is      :           if
  //! ContactOrderArray(i) has value  d it means  that
  //! Poles(i)   contains the dth  derivative of  the
  //! function to be interpolated. The length L of the
  //! following arrays must be the same :
  //! Parameters, ContactOrderArray, Poles,
  //! The length of FlatKnots is Degree + L + 1
  //! Warning:
  //! the method used to do that interpolation is
  //! gauss elimination WITHOUT pivoting. Thus if the
  //! diagonal is not dominant there is no guarantee
  //! that the algorithm will work. Nevertheless for
  //! Cubic interpolation at knots or interpolation at Scheonberg
  //! points the method will work.
  //! The InversionProblem w
  //! ll report 0 if there was no
  //! problem else it will give the i
  Standard_EXPORT static void Interpolate(const Standard_Integer         Degree,
                                          const TColStd_Array1OfReal&    FlatKnots,
                                          const TColStd_Array1OfReal&    Parameters,
                                          const TColStd_Array1OfInteger& ContactOrderArray,
                                          TColgp_Array1OfPnt2d&          Poles,
                                          TColStd_Array1OfReal&          Weights,
                                          Standard_Integer&              InversionProblem);

  //! Performs the interpolation of  the data given in
  //! the Poles  array  according  to the  requests in
  //! ContactOrderArray    that is      :           if
  //! ContactOrderArray(i) has value  d it means  that
  //! Poles(i)   contains the dth  derivative of  the
  //! function to be interpolated. The length L of the
  //! following arrays must be the same :
  //! Parameters, ContactOrderArray
  //! The length of FlatKnots is Degree + L + 1
  //! The  PolesArray   is    an   seen   as    an
  //! Array[1..N][1..ArrayDimension] with N = tge length
  //! of the parameters array
  //! Warning:
  //! the method used to do that interpolation is
  //! gauss elimination WITHOUT pivoting. Thus if the
  //! diagonal is not dominant there is no guarantee
  //! that the algorithm will work. Nevertheless for
  //! Cubic interpolation or interpolation at Scheonberg
  //! points the method will work
  //! The InversionProblem will report 0 if there was no
  //! problem else it will give the index of the faulty
  //! pivot
  Standard_EXPORT static void Interpolate(const Standard_Integer         Degree,
                                          const TColStd_Array1OfReal&    FlatKnots,
                                          const TColStd_Array1OfReal&    Parameters,
                                          const TColStd_Array1OfInteger& ContactOrderArray,
                                          const Standard_Integer         ArrayDimension,
                                          Standard_Real&                 Poles,
                                          Standard_Integer&              InversionProblem);

  Standard_EXPORT static void Interpolate(const Standard_Integer         Degree,
                                          const TColStd_Array1OfReal&    FlatKnots,
                                          const TColStd_Array1OfReal&    Parameters,
                                          const TColStd_Array1OfInteger& ContactOrderArray,
                                          const Standard_Integer         ArrayDimension,
                                          Standard_Real&                 Poles,
                                          Standard_Real&                 Weights,
                                          Standard_Integer&              InversionProblem);

  //! Find the new poles which allows  an old point (with a
  //! given  u as parameter) to reach a new position
  //! Index1 and Index2 indicate the range of poles we can move
  //! (1, NbPoles-1) or (2, NbPoles) -> no constraint for one side
  //! don't enter (1,NbPoles) -> error: rigid move
  //! (2, NbPoles-1) -> the ends are enforced
  //! (3, NbPoles-2) -> the ends and the tangency are enforced
  //! if Problem in BSplineBasis calculation, no change for the curve
  //! and FirstIndex, LastIndex = 0
  Standard_EXPORT static void MovePoint(const Standard_Real         U,
                                        const gp_Vec2d&             Displ,
                                        const Standard_Integer      Index1,
                                        const Standard_Integer      Index2,
                                        const Standard_Integer      Degree,
                                        const TColgp_Array1OfPnt2d& Poles,
                                        const TColStd_Array1OfReal* Weights,
                                        const TColStd_Array1OfReal& FlatKnots,
                                        Standard_Integer&           FirstIndex,
                                        Standard_Integer&           LastIndex,
                                        TColgp_Array1OfPnt2d&       NewPoles);

  //! Find the new poles which allows  an old point (with a
  //! given  u as parameter) to reach a new position
  //! Index1 and Index2 indicate the range of poles we can move
  //! (1, NbPoles-1) or (2, NbPoles) -> no constraint for one side
  //! don't enter (1,NbPoles) -> error: rigid move
  //! (2, NbPoles-1) -> the ends are enforced
  //! (3, NbPoles-2) -> the ends and the tangency are enforced
  //! if Problem in BSplineBasis calculation, no change for the curve
  //! and FirstIndex, LastIndex = 0
  Standard_EXPORT static void MovePoint(const Standard_Real         U,
                                        const gp_Vec&               Displ,
                                        const Standard_Integer      Index1,
                                        const Standard_Integer      Index2,
                                        const Standard_Integer      Degree,
                                        const TColgp_Array1OfPnt&   Poles,
                                        const TColStd_Array1OfReal* Weights,
                                        const TColStd_Array1OfReal& FlatKnots,
                                        Standard_Integer&           FirstIndex,
                                        Standard_Integer&           LastIndex,
                                        TColgp_Array1OfPnt&         NewPoles);

  //! This is the dimension free version of the utility
  //! U is the parameter  must be within the  first FlatKnots and the
  //! last FlatKnots  Delta is the amount the  curve has  to be moved
  //! DeltaDerivative is the  amount the derivative  has to be moved.
  //! Delta  and   DeltaDerivative   must be    array   of  dimension
  //! ArrayDimension  Degree  is the degree  of   the BSpline and the
  //! FlatKnots are the knots of the BSpline  Starting Condition if =
  //! -1 means the starting point of the curve can move
  //! = 0 means the
  //! starting  point  of the curve  cannot  move but  tangent  starting
  //! point of the curve cannot move
  //! = 1 means the starting point and tangents cannot move
  //! = 2 means the starting point tangent and curvature cannot move
  //! = ...
  //! Same holds for EndingCondition
  //! Poles are the poles of the curve
  //! Weights are the weights of the curve if not NULL
  //! NewPoles are the poles of the deformed curve
  //! ErrorStatus will be 0 if no error happened
  //! 1 if there are not enough knots/poles
  //! the imposed conditions
  //! The way to solve this problem is to add knots to the BSpline
  //! If StartCondition = 1 and EndCondition = 1 then you need at least
  //! 4 + 2 = 6 poles so for example to have a C1 cubic you will need
  //! have at least 2 internal knots.
  Standard_EXPORT static void MovePointAndTangent(const Standard_Real         U,
                                                  const Standard_Integer      ArrayDimension,
                                                  Standard_Real&              Delta,
                                                  Standard_Real&              DeltaDerivative,
                                                  const Standard_Real         Tolerance,
                                                  const Standard_Integer      Degree,
                                                  const Standard_Integer      StartingCondition,
                                                  const Standard_Integer      EndingCondition,
                                                  Standard_Real&              Poles,
                                                  const TColStd_Array1OfReal* Weights,
                                                  const TColStd_Array1OfReal& FlatKnots,
                                                  Standard_Real&              NewPoles,
                                                  Standard_Integer&           ErrorStatus);

  //! This is the dimension free version of the utility
  //! U is the parameter  must be within the  first FlatKnots and the
  //! last FlatKnots  Delta is the amount the  curve has  to be moved
  //! DeltaDerivative is the  amount the derivative  has to be moved.
  //! Delta  and   DeltaDerivative   must be    array   of  dimension
  //! ArrayDimension  Degree  is the degree  of   the BSpline and the
  //! FlatKnots are the knots of the BSpline  Starting Condition if =
  //! -1 means the starting point of the curve can move
  //! = 0 means the
  //! starting  point  of the curve  cannot  move but  tangent  starting
  //! point of the curve cannot move
  //! = 1 means the starting point and tangents cannot move
  //! = 2 means the starting point tangent and curvature cannot move
  //! = ...
  //! Same holds for EndingCondition
  //! Poles are the poles of the curve
  //! Weights are the weights of the curve if not NULL
  //! NewPoles are the poles of the deformed curve
  //! ErrorStatus will be 0 if no error happened
  //! 1 if there are not enough knots/poles
  //! the imposed conditions
  //! The way to solve this problem is to add knots to the BSpline
  //! If StartCondition = 1 and EndCondition = 1 then you need at least
  //! 4 + 2 = 6 poles so for example to have a C1 cubic you will need
  //! have at least 2 internal knots.
  Standard_EXPORT static void MovePointAndTangent(const Standard_Real         U,
                                                  const gp_Vec&               Delta,
                                                  const gp_Vec&               DeltaDerivative,
                                                  const Standard_Real         Tolerance,
                                                  const Standard_Integer      Degree,
                                                  const Standard_Integer      StartingCondition,
                                                  const Standard_Integer      EndingCondition,
                                                  const TColgp_Array1OfPnt&   Poles,
                                                  const TColStd_Array1OfReal* Weights,
                                                  const TColStd_Array1OfReal& FlatKnots,
                                                  TColgp_Array1OfPnt&         NewPoles,
                                                  Standard_Integer&           ErrorStatus);

  //! This is the dimension free version of the utility
  //! U is the parameter  must be within the  first FlatKnots and the
  //! last FlatKnots  Delta is the amount the  curve has  to be moved
  //! DeltaDerivative is the  amount the derivative  has to be moved.
  //! Delta  and   DeltaDerivative   must be    array   of  dimension
  //! ArrayDimension  Degree  is the degree  of   the BSpline and the
  //! FlatKnots are the knots of the BSpline  Starting Condition if =
  //! -1 means the starting point of the curve can move
  //! = 0 means the
  //! starting  point  of the curve  cannot  move but  tangent  starting
  //! point of the curve cannot move
  //! = 1 means the starting point and tangents cannot move
  //! = 2 means the starting point tangent and curvature cannot move
  //! = ...
  //! Same holds for EndingCondition
  //! Poles are the poles of the curve
  //! Weights are the weights of the curve if not NULL
  //! NewPoles are the poles of the deformed curve
  //! ErrorStatus will be 0 if no error happened
  //! 1 if there are not enough knots/poles
  //! the imposed conditions
  //! The way to solve this problem is to add knots to the BSpline
  //! If StartCondition = 1 and EndCondition = 1 then you need at least
  //! 4 + 2 = 6 poles so for example to have a C1 cubic you will need
  //! have at least 2 internal knots.
  Standard_EXPORT static void MovePointAndTangent(const Standard_Real         U,
                                                  const gp_Vec2d&             Delta,
                                                  const gp_Vec2d&             DeltaDerivative,
                                                  const Standard_Real         Tolerance,
                                                  const Standard_Integer      Degree,
                                                  const Standard_Integer      StartingCondition,
                                                  const Standard_Integer      EndingCondition,
                                                  const TColgp_Array1OfPnt2d& Poles,
                                                  const TColStd_Array1OfReal* Weights,
                                                  const TColStd_Array1OfReal& FlatKnots,
                                                  TColgp_Array1OfPnt2d&       NewPoles,
                                                  Standard_Integer&           ErrorStatus);

  //! given a tolerance in 3D space returns a
  //! tolerance    in U parameter space such that
  //! all u1 and u0 in the domain of the curve f(u)
  //! | u1 - u0 | < UTolerance and
  //! we have |f (u1) - f (u0)| < Tolerance3D
  Standard_EXPORT static void Resolution(Standard_Real&              PolesArray,
                                         const Standard_Integer      ArrayDimension,
                                         const Standard_Integer      NumPoles,
                                         const TColStd_Array1OfReal* Weights,
                                         const TColStd_Array1OfReal& FlatKnots,
                                         const Standard_Integer      Degree,
                                         const Standard_Real         Tolerance3D,
                                         Standard_Real&              UTolerance);

  //! given a tolerance in 3D space returns a
  //! tolerance    in U parameter space such that
  //! all u1 and u0 in the domain of the curve f(u)
  //! | u1 - u0 | < UTolerance and
  //! we have |f (u1) - f (u0)| < Tolerance3D
  Standard_EXPORT static void Resolution(const TColgp_Array1OfPnt&   Poles,
                                         const TColStd_Array1OfReal* Weights,
                                         const Standard_Integer      NumPoles,
                                         const TColStd_Array1OfReal& FlatKnots,
                                         const Standard_Integer      Degree,
                                         const Standard_Real         Tolerance3D,
                                         Standard_Real&              UTolerance);

  //! given a tolerance in 3D space returns a
  //! tolerance    in U parameter space such that
  //! all u1 and u0 in the domain of the curve f(u)
  //! | u1 - u0 | < UTolerance and
  //! we have |f (u1) - f (u0)| < Tolerance3D
  Standard_EXPORT static void Resolution(const TColgp_Array1OfPnt2d& Poles,
                                         const TColStd_Array1OfReal* Weights,
                                         const Standard_Integer      NumPoles,
                                         const TColStd_Array1OfReal& FlatKnots,
                                         const Standard_Integer      Degree,
                                         const Standard_Real         Tolerance3D,
                                         Standard_Real&              UTolerance);

  //! Splits the given range to BSpline intervals of given continuity
  //! @param[in] theKnots the knots of BSpline
  //! @param[in] theMults the knots' multiplicities
  //! @param[in] theDegree the degree of BSpline
  //! @param[in] isPeriodic the periodicity of BSpline
  //! @param[in] theContinuity the target interval's continuity
  //! @param[in] theFirst the begin of the target range
  //! @param[in] theLast the end of the target range
  //! @param[in] theTolerance the tolerance
  //! @param[in,out] theIntervals the array to store intervals if isn't nullptr
  //! @return the number of intervals
  Standard_EXPORT static Standard_Integer Intervals(const TColStd_Array1OfReal&    theKnots,
                                                    const TColStd_Array1OfInteger& theMults,
                                                    Standard_Integer               theDegree,
                                                    Standard_Boolean               isPeriodic,
                                                    Standard_Integer               theContinuity,
                                                    Standard_Real                  theFirst,
                                                    Standard_Real                  theLast,
                                                    Standard_Real                  theTolerance,
                                                    TColStd_Array1OfReal*          theIntervals);

protected:
private:
  Standard_EXPORT static void LocateParameter(const TColStd_Array1OfReal& Knots,
                                              const Standard_Real         U,
                                              const Standard_Boolean      Periodic,
                                              const Standard_Integer      K1,
                                              const Standard_Integer      K2,
                                              Standard_Integer&           Index,
                                              Standard_Real&              NewU,
                                              const Standard_Real         Uf,
                                              const Standard_Real         Ue);
};

#include <BSplCLib.lxx>

#endif // _BSplCLib_HeaderFile
