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
#include <gp_Pnt.hxx>
#include <NCollection_Array1.hxx>
#include <gp_Pnt2d.hxx>
#include <NCollection_Array2.hxx>
#include <NCollection_HArray1.hxx>

class gp_Pnt;
class gp_Pnt2d;
class gp_Vec;
class gp_Vec2d;
class math_Matrix;

//! BSplCLib B-spline curve Library.
//!
//! The BSplCLib package is a basic library for BSplines. It
//! provides three categories of functions.
//!
//! * Management methods to process knots and multiplicities.
//!
//! * Multi-Dimensions spline methods. BSpline methods where
//! poles have an arbitrary number of dimensions. They divides
//! in two groups:
//!
//! - Global methods modifying the whole set of poles. The
//! poles are described by an array of Reals and a
//! Dimension. Example: Inserting knots.
//!
//! - Local methods computing points and derivatives. The
//! poles are described by a pointer on a local array of
//! Reals and a Dimension. The local array is modified.
//!
//! * 2D and 3D spline curves methods.
//!
//! Methods for 2d and 3d BSplines curves rational or not
//! rational.
//!
//! Those methods have the following structure:
//!
//! - They extract the pole information in a working array.
//!
//! - They process the working array with the multi-
//! dimension methods. (for example a 3d rational curve
//! is processed as a 4 dimension curve).
//!
//! - They get back the result in the original dimension.
//!
//! Note that the bspline surface methods found in the
//! package BSplSLib uses the same structure and rely on
//! BSplCLib.
//!
//! In the following list of methods the 2d and 3d curve
//! methods will be described with the corresponding
//! multi-dimension method.
//!
//! The 3d or 2d B-spline curve is defined with:
//!
//! . its control points : NCollection_Array1<gp_Pnt>(2d)        Poles
//! . its weights        : NCollection_Array1<double>          Weights
//! . its knots          : NCollection_Array1<double>          Knots
//! . its multiplicities : NCollection_Array1<int>       Mults
//! . its degree         : int              Degree
//! . its periodicity    : bool              Periodic
//!
//! Warnings :
//! The bounds of Poles and Weights should be the same.
//! The bounds of Knots and Mults should be the same.
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
  Standard_EXPORT static void Hunt(const NCollection_Array1<double>& theArray,
                                   const double                      theX,
                                   int&                              theXPos);

  //! Computes the index of the knots value which gives
  //! the start point of the curve.
  Standard_EXPORT static int FirstUKnotIndex(const int                      Degree,
                                             const NCollection_Array1<int>& Mults);

  //! Computes the index of the knots value which gives
  //! the end point of the curve.
  Standard_EXPORT static int LastUKnotIndex(const int Degree, const NCollection_Array1<int>& Mults);

  //! Computes the index of the flats knots sequence
  //! corresponding to <Index> in the knots sequence
  //! which multiplicities are <Mults>.
  Standard_EXPORT static int FlatIndex(const int                      Degree,
                                       const int                      Index,
                                       const NCollection_Array1<int>& Mults,
                                       const bool                     Periodic);

  //! Locates the parametric value U in the knots
  //! sequence between the knot K1 and the knot K2.
  //! The value return in Index verifies.
  //!
  //! Knots(Index) <= U < Knots(Index + 1)
  //! if U <= Knots (K1) then Index = K1
  //! if U >= Knots (K2) then Index = K2 - 1
  //!
  //! If Periodic is True U may be modified to fit in
  //! the range Knots(K1), Knots(K2). In any case the
  //! correct value is returned in NewU.
  //!
  //! Warnings: Index is used as input data to initialize the
  //! searching function.
  //! Warning: Knots have to be "with repetitions"
  Standard_EXPORT static void LocateParameter(const int                         Degree,
                                              const NCollection_Array1<double>& Knots,
                                              const NCollection_Array1<int>&    Mults,
                                              const double                      U,
                                              const bool                        IsPeriodic,
                                              const int                         FromK1,
                                              const int                         ToK2,
                                              int&                              KnotIndex,
                                              double&                           NewU);

  //! Locates the parametric value U in the knots
  //! sequence between the knot K1 and the knot K2.
  //! The value return in Index verifies.
  //!
  //! Knots(Index) <= U < Knots(Index + 1)
  //! if U <= Knots (K1) then Index = K1
  //! if U >= Knots (K2) then Index = K2 - 1
  //!
  //! If Periodic is True U may be modified to fit in
  //! the range Knots(K1), Knots(K2). In any case the
  //! correct value is returned in NewU.
  //!
  //! Warnings: Index is used as input data to initialize the
  //! searching function.
  //! Warning: Knots have to be "flat"
  Standard_EXPORT static void LocateParameter(const int                         Degree,
                                              const NCollection_Array1<double>& Knots,
                                              const double                      U,
                                              const bool                        IsPeriodic,
                                              const int                         FromK1,
                                              const int                         ToK2,
                                              int&                              KnotIndex,
                                              double&                           NewU);

  Standard_EXPORT static void LocateParameter(const int                         Degree,
                                              const NCollection_Array1<double>& Knots,
                                              const NCollection_Array1<int>*    Mults,
                                              const double                      U,
                                              const bool                        Periodic,
                                              int&                              Index,
                                              double&                           NewU);

  //! Finds the greatest multiplicity in a set of knots
  //! between K1 and K2. Mults is the multiplicity
  //! associated with each knot value.
  Standard_EXPORT static int MaxKnotMult(const NCollection_Array1<int>& Mults,
                                         const int                      K1,
                                         const int                      K2);

  //! Finds the lowest multiplicity in a set of knots
  //! between K1 and K2. Mults is the multiplicity
  //! associated with each knot value.
  Standard_EXPORT static int MinKnotMult(const NCollection_Array1<int>& Mults,
                                         const int                      K1,
                                         const int                      K2);

  //! Returns the number of poles of the curve. Returns 0 if
  //! one of the multiplicities is incorrect.
  //!
  //! * Non positive.
  //!
  //! * Greater than Degree, or Degree+1 at the first and
  //! last knot of a non periodic curve.
  //!
  //! * The last periodicity on a periodic curve is not
  //! equal to the first.
  Standard_EXPORT static int NbPoles(const int                      Degree,
                                     const bool                     Periodic,
                                     const NCollection_Array1<int>& Mults);

  //! Returns the length of the sequence of knots with
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
  Standard_EXPORT static int KnotSequenceLength(const NCollection_Array1<int>& Mults,
                                                const int                      Degree,
                                                const bool                     Periodic);

  Standard_EXPORT static void KnotSequence(const NCollection_Array1<double>& Knots,
                                           const NCollection_Array1<int>&    Mults,
                                           NCollection_Array1<double>&       KnotSeq,
                                           const bool                        Periodic = false);

  //! Computes the sequence of knots KnotSeq with
  //! repetition of the knots of multiplicity greater
  //! than 1.
  //!
  //! Length of KnotSeq must be KnotSequenceLength(Mults,Degree,Periodic)
  Standard_EXPORT static void KnotSequence(const NCollection_Array1<double>& Knots,
                                           const NCollection_Array1<int>&    Mults,
                                           const int                         Degree,
                                           const bool                        Periodic,
                                           NCollection_Array1<double>&       KnotSeq);

  //! Returns thelength of the sequence of knots (and
  //! Mults) without repetition.
  Standard_EXPORT static int KnotsLength(const NCollection_Array1<double>& KnotSeq,
                                         const bool                        Periodic = false);

  //! Computes the sequence of knots Knots without
  //! repetition of the knots of multiplicity greater
  //! than 1.
  //!
  //! Length of <Knots> and <Mults> must be
  //! KnotsLength(KnotSequence,Periodic)
  Standard_EXPORT static void Knots(const NCollection_Array1<double>& KnotSeq,
                                    NCollection_Array1<double>&       Knots,
                                    NCollection_Array1<int>&          Mults,
                                    const bool                        Periodic = false);

  //! Analyses if the knots distribution is "Uniform"
  //! or "NonUniform" between the knot FromK1 and the
  //! knot ToK2. There is no repetition of knot in the
  //! knots'sequence <Knots>.
  Standard_EXPORT static BSplCLib_KnotDistribution KnotForm(const NCollection_Array1<double>& Knots,
                                                            const int FromK1,
                                                            const int ToK2);

  //! Analyses the distribution of multiplicities between
  //! the knot FromK1 and the Knot ToK2.
  Standard_EXPORT static BSplCLib_MultDistribution MultForm(const NCollection_Array1<int>& Mults,
                                                            const int                      FromK1,
                                                            const int                      ToK2);

  //! Analyzes the array of knots.
  //! Returns the form and the maximum knot multiplicity.
  Standard_EXPORT static void KnotAnalysis(const int                         Degree,
                                           const bool                        Periodic,
                                           const NCollection_Array1<double>& CKnots,
                                           const NCollection_Array1<int>&    CMults,
                                           GeomAbs_BSplKnotDistribution&     KnotForm,
                                           int&                              MaxKnotMult);

  //! Reparametrizes a B-spline curve to [U1, U2].
  //! The knot values are recomputed such that Knots (Lower) = U1
  //! and Knots (Upper) = U2 but the knot form is not modified.
  //! Warnings:
  //! In the array Knots the values must be in ascending order.
  //! U1 must not be equal to U2 to avoid division by zero.
  Standard_EXPORT static void Reparametrize(const double                U1,
                                            const double                U2,
                                            NCollection_Array1<double>& Knots);

  //! Reverses the array knots to become the knots
  //! sequence of the reversed curve.
  Standard_EXPORT static void Reverse(NCollection_Array1<double>& Knots);

  //! Reverses the array of multiplicities.
  Standard_EXPORT static void Reverse(NCollection_Array1<int>& Mults);

  //! Reverses the array of poles. Last is the index of
  //! the new first pole. On a non periodic curve last
  //! is Poles.Upper(). On a periodic curve last is
  //!
  //! (number of flat knots - degree - 1)
  //!
  //! or
  //!
  //! (sum of multiplicities(but for the last) + degree - 1)
  Standard_EXPORT static void Reverse(NCollection_Array1<gp_Pnt>& Poles, const int Last);

  //! Reverses the array of poles.
  Standard_EXPORT static void Reverse(NCollection_Array1<gp_Pnt2d>& Poles, const int Last);

  //! Reverses the array of poles.
  Standard_EXPORT static void Reverse(NCollection_Array1<double>& Weights, const int Last);

  //! Returns False if all the weights of the array <Weights>
  //! between I1 an I2 are identic. Epsilon is used for
  //! comparing weights. If Epsilon is 0. the Epsilon of the
  //! first weight is used.
  Standard_EXPORT static bool IsRational(const NCollection_Array1<double>& Weights,
                                         const int                         I1,
                                         const int                         I2,
                                         const double                      Epsilon = 0.0);

  //! returns the degree maxima for a BSplineCurve.
  static inline constexpr int MaxDegree();

  //! Perform the Boor algorithm to evaluate a point at
  //! parameter <U>, with <Degree> and <Dimension>.
  //!
  //! Poles is an array of Reals of size
  //!
  //! <Dimension> * <Degree>+1
  //!
  //! Containing the poles. At the end <Poles> contains
  //! the current point.
  Standard_EXPORT static void Eval(const double U,
                                   const int    Degree,
                                   double&      Knots,
                                   const int    Dimension,
                                   double&      Poles);

  //! Performs the Boor Algorithm at parameter <U> with
  //! the given <Degree> and the array of <Knots> on the
  //! poles <Poles> of dimension <Dimension>. The schema
  //! is computed until level <Depth> on a basis of
  //! <Length+1> poles.
  //!
  //! * Knots is an array of reals of length:
  //!
  //! <Length> + <Degree>
  //!
  //! * Poles is an array of reals of length:
  //!
  //! (2 * <Length> + 1) * <Dimension>
  //!
  //! The poles values must be set in the array at the
  //! positions.
  //!
  //! 0..Dimension,
  //!
  //! 2 * Dimension ..
  //! 3 * Dimension
  //!
  //! 4 * Dimension ..
  //! 5 * Dimension
  //!
  //! ...
  //!
  //! The results are found in the array poles depending
  //! on the Depth. (See the method GetPole).
  Standard_EXPORT static void BoorScheme(const double U,
                                         const int    Degree,
                                         double&      Knots,
                                         const int    Dimension,
                                         double&      Poles,
                                         const int    Depth,
                                         const int    Length);

  //! Compute the content of Pole before the BoorScheme.
  //! This method is used to remove poles.
  //!
  //! U is the poles to remove, Knots should contains the
  //! knots of the curve after knot removal.
  //!
  //! The first and last poles do not change, the other
  //! poles are computed by averaging two possible values.
  //! The distance between the two possible poles is
  //! computed, if it is higher than <Tolerance> False is
  //! returned.
  Standard_EXPORT static bool AntiBoorScheme(const double U,
                                             const int    Degree,
                                             double&      Knots,
                                             const int    Dimension,
                                             double&      Poles,
                                             const int    Depth,
                                             const int    Length,
                                             const double Tolerance);

  //! Computes the poles of the BSpline giving the
  //! derivatives of order <Order>.
  //!
  //! The formula for the first order is
  //!
  //! Pole(i) = Degree * (Pole(i+1) - Pole(i)) /
  //! (Knots(i+Degree+1) - Knots(i+1))
  //!
  //! This formula is repeated (Degree is decremented at
  //! each step).
  Standard_EXPORT static void Derivative(const int Degree,
                                         double&   Knots,
                                         const int Dimension,
                                         const int Length,
                                         const int Order,
                                         double&   Poles);

  //! Performs the Bohm Algorithm at parameter <U>. This
  //! algorithm computes the value and all the derivatives
  //! up to order N (N <= Degree).
  //!
  //! <Poles> is the original array of poles.
  //!
  //! The result in <Poles> is the value and the
  //! derivatives. Poles[0] is the value, Poles[Degree]
  //! is the last derivative.
  Standard_EXPORT static void Bohm(const double U,
                                   const int    Degree,
                                   const int    N,
                                   double&      Knots,
                                   const int    Dimension,
                                   double&      Poles);

  //! Used as argument for a non rational curve.
  static NCollection_Array1<double>* NoWeights();

  //! Used as argument for a flatknots evaluation.
  static NCollection_Array1<int>* NoMults();

  //! Returns the maximum number of elements supported by the pre-allocated
  //! unit weights array (2049). For sizes larger than this, UnitWeights()
  //! will allocate a new array.
  static constexpr int MaxUnitWeightsSize() { return 2049; }

  //! Returns an NCollection_Array1<double> filled with 1.0 values.
  //! If theNbElems <= MaxUnitWeightsSize(), references a pre-allocated global array
  //! (zero allocation). Otherwise, allocates a new array and fills with 1.0.
  //! @warning The returned array may reference global static memory -- do NOT modify elements.
  //! @param[in] theNbElems the number of elements in the returned array
  //! @return array of unit weights with bounds [1, theNbElems]
  Standard_EXPORT static NCollection_Array1<double> UnitWeights(const int theNbElems);

  //! Returns a pointer to the pre-allocated unit weights static array.
  //! The array contains MaxUnitWeightsSize() elements, all equal to 1.0.
  //! @warning Do NOT modify elements through this pointer.
  //! @return pointer to the first element of the static unit weights array
  Standard_EXPORT static const double* UnitWeightsData();

  //! Stores in LK the useful knots for the BoorSchem
  //! on the span Knots(Index) - Knots(Index+1)
  Standard_EXPORT static void BuildKnots(const int                         Degree,
                                         const int                         Index,
                                         const bool                        Periodic,
                                         const NCollection_Array1<double>& Knots,
                                         const NCollection_Array1<int>*    Mults,
                                         double&                           LK);

  //! Return the index of the first Pole to use on the
  //! span Mults(Index) - Mults(Index+1). This index
  //! must be added to Poles.Lower().
  Standard_EXPORT static int PoleIndex(const int                      Degree,
                                       const int                      Index,
                                       const bool                     Periodic,
                                       const NCollection_Array1<int>& Mults);

  Standard_EXPORT static void BuildEval(const int                         Degree,
                                        const int                         Index,
                                        const NCollection_Array1<double>& Poles,
                                        const NCollection_Array1<double>* Weights,
                                        double&                           LP);

  Standard_EXPORT static void BuildEval(const int                         Degree,
                                        const int                         Index,
                                        const NCollection_Array1<gp_Pnt>& Poles,
                                        const NCollection_Array1<double>* Weights,
                                        double&                           LP);

  //! Copy in <LP> the poles and weights for the Eval
  //! scheme. starting from Poles(Poles.Lower()+Index)
  Standard_EXPORT static void BuildEval(const int                           Degree,
                                        const int                           Index,
                                        const NCollection_Array1<gp_Pnt2d>& Poles,
                                        const NCollection_Array1<double>*   Weights,
                                        double&                             LP);

  //! Copy in <LP> poles for <Dimension> Boor scheme.
  //! Starting from <Index> * <Dimension>, copy
  //! <Length+1> poles.
  Standard_EXPORT static void BuildBoor(const int                         Index,
                                        const int                         Length,
                                        const int                         Dimension,
                                        const NCollection_Array1<double>& Poles,
                                        double&                           LP);

  //! Returns the index in the Boor result array of the
  //! poles <Index>. If the Boor algorithm was perform
  //! with <Length> and <Depth>.
  Standard_EXPORT static int BoorIndex(const int Index, const int Length, const int Depth);

  //! Copy the pole at position <Index> in the Boor
  //! scheme of dimension <Dimension> to <Position> in
  //! the array <Pole>. <Position> is updated.
  Standard_EXPORT static void GetPole(const int                   Index,
                                      const int                   Length,
                                      const int                   Depth,
                                      const int                   Dimension,
                                      double&                     LocPoles,
                                      int&                        Position,
                                      NCollection_Array1<double>& Pole);

  //! Returns in <NbPoles, NbKnots> the new number of poles
  //! and knots if the sequence of knots <AddKnots,
  //! AddMults> is inserted in the sequence <Knots, Mults>.
  //!
  //! Epsilon is used to compare knots for equality.
  //!
  //! If Add is True the multiplicities on equal knots are
  //! added.
  //!
  //! If Add is False the max value of the multiplicities is
  //! kept.
  //!
  //! Return False if:
  //! The knew knots are knot increasing.
  //! The new knots are not in the range.
  Standard_EXPORT static bool PrepareInsertKnots(const int                         Degree,
                                                 const bool                        Periodic,
                                                 const NCollection_Array1<double>& Knots,
                                                 const NCollection_Array1<int>&    Mults,
                                                 const NCollection_Array1<double>& AddKnots,
                                                 const NCollection_Array1<int>*    AddMults,
                                                 int&                              NbPoles,
                                                 int&                              NbKnots,
                                                 const double                      Epsilon,
                                                 const bool                        Add = true);

  Standard_EXPORT static void InsertKnots(const int                         Degree,
                                          const bool                        Periodic,
                                          const int                         Dimension,
                                          const NCollection_Array1<double>& Poles,
                                          const NCollection_Array1<double>& Knots,
                                          const NCollection_Array1<int>&    Mults,
                                          const NCollection_Array1<double>& AddKnots,
                                          const NCollection_Array1<int>*    AddMults,
                                          NCollection_Array1<double>&       NewPoles,
                                          NCollection_Array1<double>&       NewKnots,
                                          NCollection_Array1<int>&          NewMults,
                                          const double                      Epsilon,
                                          const bool                        Add = true);

  Standard_EXPORT static void InsertKnots(const int                         Degree,
                                          const bool                        Periodic,
                                          const NCollection_Array1<gp_Pnt>& Poles,
                                          const NCollection_Array1<double>* Weights,
                                          const NCollection_Array1<double>& Knots,
                                          const NCollection_Array1<int>&    Mults,
                                          const NCollection_Array1<double>& AddKnots,
                                          const NCollection_Array1<int>*    AddMults,
                                          NCollection_Array1<gp_Pnt>&       NewPoles,
                                          NCollection_Array1<double>*       NewWeights,
                                          NCollection_Array1<double>&       NewKnots,
                                          NCollection_Array1<int>&          NewMults,
                                          const double                      Epsilon,
                                          const bool                        Add = true);

  //! Insert a sequence of knots <AddKnots> with
  //! multiplicities <AddMults>. <AddKnots> must be a non
  //! decreasing sequence and verifies:
  //!
  //! Knots(Knots.Lower()) <= AddKnots(AddKnots.Lower())
  //! Knots(Knots.Upper()) >= AddKnots(AddKnots.Upper())
  //!
  //! The NewPoles and NewWeights arrays must have a length:
  //! Poles.Length() + Sum(AddMults())
  //!
  //! When a knot to insert is identic to an existing knot the
  //! multiplicities are added.
  //!
  //! Epsilon is used to test knots for equality.
  //!
  //! When AddMult is negative or null the knot is not inserted.
  //! No multiplicity will becomes higher than the degree.
  //!
  //! The new Knots and Multiplicities are copied in <NewKnots>
  //! and <NewMults>.
  //!
  //! All the New arrays should be correctly dimensioned.
  //!
  //! When all the new knots are existing knots, i.e. only the
  //! multiplicities will change it is safe to use the same
  //! arrays as input and output.
  Standard_EXPORT static void InsertKnots(const int                           Degree,
                                          const bool                          Periodic,
                                          const NCollection_Array1<gp_Pnt2d>& Poles,
                                          const NCollection_Array1<double>*   Weights,
                                          const NCollection_Array1<double>&   Knots,
                                          const NCollection_Array1<int>&      Mults,
                                          const NCollection_Array1<double>&   AddKnots,
                                          const NCollection_Array1<int>*      AddMults,
                                          NCollection_Array1<gp_Pnt2d>&       NewPoles,
                                          NCollection_Array1<double>*         NewWeights,
                                          NCollection_Array1<double>&         NewKnots,
                                          NCollection_Array1<int>&            NewMults,
                                          const double                        Epsilon,
                                          const bool                          Add = true);

  Standard_EXPORT static void InsertKnot(const int                         UIndex,
                                         const double                      U,
                                         const int                         UMult,
                                         const int                         Degree,
                                         const bool                        Periodic,
                                         const NCollection_Array1<gp_Pnt>& Poles,
                                         const NCollection_Array1<double>* Weights,
                                         const NCollection_Array1<double>& Knots,
                                         const NCollection_Array1<int>&    Mults,
                                         NCollection_Array1<gp_Pnt>&       NewPoles,
                                         NCollection_Array1<double>*       NewWeights);

  //! Insert a new knot U of multiplicity UMult in the knot
  //! sequence.
  //!
  //! The location of the new Knot should be given as an input
  //! data. UIndex locates the new knot U in the knot sequence
  //! and Knots (UIndex) < U < Knots (UIndex + 1).
  //!
  //! The new control points corresponding to this insertion are
  //! returned. Knots and Mults are not updated.
  Standard_EXPORT static void InsertKnot(const int                           UIndex,
                                         const double                        U,
                                         const int                           UMult,
                                         const int                           Degree,
                                         const bool                          Periodic,
                                         const NCollection_Array1<gp_Pnt2d>& Poles,
                                         const NCollection_Array1<double>*   Weights,
                                         const NCollection_Array1<double>&   Knots,
                                         const NCollection_Array1<int>&      Mults,
                                         NCollection_Array1<gp_Pnt2d>&       NewPoles,
                                         NCollection_Array1<double>*         NewWeights);

  Standard_EXPORT static void RaiseMultiplicity(const int                         KnotIndex,
                                                const int                         Mult,
                                                const int                         Degree,
                                                const bool                        Periodic,
                                                const NCollection_Array1<gp_Pnt>& Poles,
                                                const NCollection_Array1<double>* Weights,
                                                const NCollection_Array1<double>& Knots,
                                                const NCollection_Array1<int>&    Mults,
                                                NCollection_Array1<gp_Pnt>&       NewPoles,
                                                NCollection_Array1<double>*       NewWeights);

  //! Raise the multiplicity of knot to <UMult>.
  //!
  //! The new control points are returned. Knots and Mults are
  //! not updated.
  Standard_EXPORT static void RaiseMultiplicity(const int                           KnotIndex,
                                                const int                           Mult,
                                                const int                           Degree,
                                                const bool                          Periodic,
                                                const NCollection_Array1<gp_Pnt2d>& Poles,
                                                const NCollection_Array1<double>*   Weights,
                                                const NCollection_Array1<double>&   Knots,
                                                const NCollection_Array1<int>&      Mults,
                                                NCollection_Array1<gp_Pnt2d>&       NewPoles,
                                                NCollection_Array1<double>*         NewWeights);

  Standard_EXPORT static bool RemoveKnot(const int                         Index,
                                         const int                         Mult,
                                         const int                         Degree,
                                         const bool                        Periodic,
                                         const int                         Dimension,
                                         const NCollection_Array1<double>& Poles,
                                         const NCollection_Array1<double>& Knots,
                                         const NCollection_Array1<int>&    Mults,
                                         NCollection_Array1<double>&       NewPoles,
                                         NCollection_Array1<double>&       NewKnots,
                                         NCollection_Array1<int>&          NewMults,
                                         const double                      Tolerance);

  Standard_EXPORT static bool RemoveKnot(const int                         Index,
                                         const int                         Mult,
                                         const int                         Degree,
                                         const bool                        Periodic,
                                         const NCollection_Array1<gp_Pnt>& Poles,
                                         const NCollection_Array1<double>* Weights,
                                         const NCollection_Array1<double>& Knots,
                                         const NCollection_Array1<int>&    Mults,
                                         NCollection_Array1<gp_Pnt>&       NewPoles,
                                         NCollection_Array1<double>*       NewWeights,
                                         NCollection_Array1<double>&       NewKnots,
                                         NCollection_Array1<int>&          NewMults,
                                         const double                      Tolerance);

  //! Decrement the multiplicity of <Knots(Index)>
  //! to <Mult>. If <Mult> is null the knot is
  //! removed.
  //!
  //! As there are two ways to compute the new poles
  //! the midlle will be used as long as the
  //! distance is lower than Tolerance.
  //!
  //! If a distance is bigger than tolerance the
  //! methods returns False and the new arrays are
  //! not modified.
  //!
  //! A low tolerance can be used to test if the
  //! knot can be removed without modifying the
  //! curve.
  //!
  //! A high tolerance can be used to "smooth" the
  //! curve.
  Standard_EXPORT static bool RemoveKnot(const int                           Index,
                                         const int                           Mult,
                                         const int                           Degree,
                                         const bool                          Periodic,
                                         const NCollection_Array1<gp_Pnt2d>& Poles,
                                         const NCollection_Array1<double>*   Weights,
                                         const NCollection_Array1<double>&   Knots,
                                         const NCollection_Array1<int>&      Mults,
                                         NCollection_Array1<gp_Pnt2d>&       NewPoles,
                                         NCollection_Array1<double>*         NewWeights,
                                         NCollection_Array1<double>&         NewKnots,
                                         NCollection_Array1<int>&            NewMults,
                                         const double                        Tolerance);

  //! Returns the number of knots of a curve with
  //! multiplicities <Mults> after elevating the degree from
  //! <Degree> to <NewDegree>. See the IncreaseDegree method
  //! for more comments.
  Standard_EXPORT static int IncreaseDegreeCountKnots(const int                      Degree,
                                                      const int                      NewDegree,
                                                      const bool                     Periodic,
                                                      const NCollection_Array1<int>& Mults);

  Standard_EXPORT static void IncreaseDegree(const int                         Degree,
                                             const int                         NewDegree,
                                             const bool                        Periodic,
                                             const int                         Dimension,
                                             const NCollection_Array1<double>& Poles,
                                             const NCollection_Array1<double>& Knots,
                                             const NCollection_Array1<int>&    Mults,
                                             NCollection_Array1<double>&       NewPoles,
                                             NCollection_Array1<double>&       NewKnots,
                                             NCollection_Array1<int>&          NewMults);

  Standard_EXPORT static void IncreaseDegree(const int                         Degree,
                                             const int                         NewDegree,
                                             const bool                        Periodic,
                                             const NCollection_Array1<gp_Pnt>& Poles,
                                             const NCollection_Array1<double>* Weights,
                                             const NCollection_Array1<double>& Knots,
                                             const NCollection_Array1<int>&    Mults,
                                             NCollection_Array1<gp_Pnt>&       NewPoles,
                                             NCollection_Array1<double>*       NewWeights,
                                             NCollection_Array1<double>&       NewKnots,
                                             NCollection_Array1<int>&          NewMults);

  Standard_EXPORT static void IncreaseDegree(const int                           Degree,
                                             const int                           NewDegree,
                                             const bool                          Periodic,
                                             const NCollection_Array1<gp_Pnt2d>& Poles,
                                             const NCollection_Array1<double>*   Weights,
                                             const NCollection_Array1<double>&   Knots,
                                             const NCollection_Array1<int>&      Mults,
                                             NCollection_Array1<gp_Pnt2d>&       NewPoles,
                                             NCollection_Array1<double>*         NewWeights,
                                             NCollection_Array1<double>&         NewKnots,
                                             NCollection_Array1<int>&            NewMults);

  Standard_EXPORT static void IncreaseDegree(const int                         NewDegree,
                                             const NCollection_Array1<gp_Pnt>& Poles,
                                             const NCollection_Array1<double>* Weights,
                                             NCollection_Array1<gp_Pnt>&       NewPoles,
                                             NCollection_Array1<double>*       NewWeights);

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
  //! and last multiplicity not equal to Degree+1 where
  //! knots are removed form the start and the bottom
  //! until the sum of the multiplicities is equal to
  //! NewDegree+1 at the knots corresponding to the
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
  //! They become 2 3 2.
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
  Standard_EXPORT static void IncreaseDegree(const int                           theNewDegree,
                                             const NCollection_Array1<gp_Pnt2d>& thePoles,
                                             const NCollection_Array1<double>*   theWeights,
                                             NCollection_Array1<gp_Pnt2d>&       theNewPoles,
                                             NCollection_Array1<double>*         theNewWeights);

  //! Set in <NbKnots> and <NbPolesToAdd> the number of Knots and
  //! Poles of the NotPeriodic Curve identical at the
  //! periodic curve with a degree <Degree>, a
  //! knots-distribution with Multiplicities <Mults>.
  Standard_EXPORT static void PrepareUnperiodize(const int                      Degree,
                                                 const NCollection_Array1<int>& Mults,
                                                 int&                           NbKnots,
                                                 int&                           NbPoles);

  Standard_EXPORT static void Unperiodize(const int                         Degree,
                                          const int                         Dimension,
                                          const NCollection_Array1<int>&    Mults,
                                          const NCollection_Array1<double>& Knots,
                                          const NCollection_Array1<double>& Poles,
                                          NCollection_Array1<int>&          NewMults,
                                          NCollection_Array1<double>&       NewKnots,
                                          NCollection_Array1<double>&       NewPoles);

  Standard_EXPORT static void Unperiodize(const int                         Degree,
                                          const NCollection_Array1<int>&    Mults,
                                          const NCollection_Array1<double>& Knots,
                                          const NCollection_Array1<gp_Pnt>& Poles,
                                          const NCollection_Array1<double>* Weights,
                                          NCollection_Array1<int>&          NewMults,
                                          NCollection_Array1<double>&       NewKnots,
                                          NCollection_Array1<gp_Pnt>&       NewPoles,
                                          NCollection_Array1<double>*       NewWeights);

  Standard_EXPORT static void Unperiodize(const int                           Degree,
                                          const NCollection_Array1<int>&      Mults,
                                          const NCollection_Array1<double>&   Knots,
                                          const NCollection_Array1<gp_Pnt2d>& Poles,
                                          const NCollection_Array1<double>*   Weights,
                                          NCollection_Array1<int>&            NewMults,
                                          NCollection_Array1<double>&         NewKnots,
                                          NCollection_Array1<gp_Pnt2d>&       NewPoles,
                                          NCollection_Array1<double>*         NewWeights);

  //! Set in <NbKnots> and <NbPoles> the number of Knots and
  //! Poles of the curve resulting from the trimming of the
  //! BSplinecurve defined with <degree>, <knots>, <mults>
  Standard_EXPORT static void PrepareTrimming(const int                         Degree,
                                              const bool                        Periodic,
                                              const NCollection_Array1<double>& Knots,
                                              const NCollection_Array1<int>&    Mults,
                                              const double                      U1,
                                              const double                      U2,
                                              int&                              NbKnots,
                                              int&                              NbPoles);

  Standard_EXPORT static void Trimming(const int                         Degree,
                                       const bool                        Periodic,
                                       const int                         Dimension,
                                       const NCollection_Array1<double>& Knots,
                                       const NCollection_Array1<int>&    Mults,
                                       const NCollection_Array1<double>& Poles,
                                       const double                      U1,
                                       const double                      U2,
                                       NCollection_Array1<double>&       NewKnots,
                                       NCollection_Array1<int>&          NewMults,
                                       NCollection_Array1<double>&       NewPoles);

  Standard_EXPORT static void Trimming(const int                         Degree,
                                       const bool                        Periodic,
                                       const NCollection_Array1<double>& Knots,
                                       const NCollection_Array1<int>&    Mults,
                                       const NCollection_Array1<gp_Pnt>& Poles,
                                       const NCollection_Array1<double>* Weights,
                                       const double                      U1,
                                       const double                      U2,
                                       NCollection_Array1<double>&       NewKnots,
                                       NCollection_Array1<int>&          NewMults,
                                       NCollection_Array1<gp_Pnt>&       NewPoles,
                                       NCollection_Array1<double>*       NewWeights);

  Standard_EXPORT static void Trimming(const int                           Degree,
                                       const bool                          Periodic,
                                       const NCollection_Array1<double>&   Knots,
                                       const NCollection_Array1<int>&      Mults,
                                       const NCollection_Array1<gp_Pnt2d>& Poles,
                                       const NCollection_Array1<double>*   Weights,
                                       const double                        U1,
                                       const double                        U2,
                                       NCollection_Array1<double>&         NewKnots,
                                       NCollection_Array1<int>&            NewMults,
                                       NCollection_Array1<gp_Pnt2d>&       NewPoles,
                                       NCollection_Array1<double>*         NewWeights);

  Standard_EXPORT static void D0(const double                      U,
                                 const int                         Index,
                                 const int                         Degree,
                                 const bool                        Periodic,
                                 const NCollection_Array1<double>& Poles,
                                 const NCollection_Array1<double>* Weights,
                                 const NCollection_Array1<double>& Knots,
                                 const NCollection_Array1<int>*    Mults,
                                 double&                           P);

  Standard_EXPORT static void D0(const double                      U,
                                 const int                         Index,
                                 const int                         Degree,
                                 const bool                        Periodic,
                                 const NCollection_Array1<gp_Pnt>& Poles,
                                 const NCollection_Array1<double>* Weights,
                                 const NCollection_Array1<double>& Knots,
                                 const NCollection_Array1<int>*    Mults,
                                 gp_Pnt&                           P);

  Standard_EXPORT static void D0(const double                        U,
                                 const int                           UIndex,
                                 const int                           Degree,
                                 const bool                          Periodic,
                                 const NCollection_Array1<gp_Pnt2d>& Poles,
                                 const NCollection_Array1<double>*   Weights,
                                 const NCollection_Array1<double>&   Knots,
                                 const NCollection_Array1<int>*      Mults,
                                 gp_Pnt2d&                           P);

  Standard_EXPORT static void D0(const double                      U,
                                 const NCollection_Array1<gp_Pnt>& Poles,
                                 const NCollection_Array1<double>* Weights,
                                 gp_Pnt&                           P);

  Standard_EXPORT static void D0(const double                        U,
                                 const NCollection_Array1<gp_Pnt2d>& Poles,
                                 const NCollection_Array1<double>*   Weights,
                                 gp_Pnt2d&                           P);

  Standard_EXPORT static void D1(const double                      U,
                                 const int                         Index,
                                 const int                         Degree,
                                 const bool                        Periodic,
                                 const NCollection_Array1<double>& Poles,
                                 const NCollection_Array1<double>* Weights,
                                 const NCollection_Array1<double>& Knots,
                                 const NCollection_Array1<int>*    Mults,
                                 double&                           P,
                                 double&                           V);

  Standard_EXPORT static void D1(const double                      U,
                                 const int                         Index,
                                 const int                         Degree,
                                 const bool                        Periodic,
                                 const NCollection_Array1<gp_Pnt>& Poles,
                                 const NCollection_Array1<double>* Weights,
                                 const NCollection_Array1<double>& Knots,
                                 const NCollection_Array1<int>*    Mults,
                                 gp_Pnt&                           P,
                                 gp_Vec&                           V);

  Standard_EXPORT static void D1(const double                        U,
                                 const int                           UIndex,
                                 const int                           Degree,
                                 const bool                          Periodic,
                                 const NCollection_Array1<gp_Pnt2d>& Poles,
                                 const NCollection_Array1<double>*   Weights,
                                 const NCollection_Array1<double>&   Knots,
                                 const NCollection_Array1<int>*      Mults,
                                 gp_Pnt2d&                           P,
                                 gp_Vec2d&                           V);

  Standard_EXPORT static void D1(const double                      U,
                                 const NCollection_Array1<gp_Pnt>& Poles,
                                 const NCollection_Array1<double>* Weights,
                                 gp_Pnt&                           P,
                                 gp_Vec&                           V);

  Standard_EXPORT static void D1(const double                        U,
                                 const NCollection_Array1<gp_Pnt2d>& Poles,
                                 const NCollection_Array1<double>*   Weights,
                                 gp_Pnt2d&                           P,
                                 gp_Vec2d&                           V);

  Standard_EXPORT static void D2(const double                      U,
                                 const int                         Index,
                                 const int                         Degree,
                                 const bool                        Periodic,
                                 const NCollection_Array1<double>& Poles,
                                 const NCollection_Array1<double>* Weights,
                                 const NCollection_Array1<double>& Knots,
                                 const NCollection_Array1<int>*    Mults,
                                 double&                           P,
                                 double&                           V1,
                                 double&                           V2);

  Standard_EXPORT static void D2(const double                      U,
                                 const int                         Index,
                                 const int                         Degree,
                                 const bool                        Periodic,
                                 const NCollection_Array1<gp_Pnt>& Poles,
                                 const NCollection_Array1<double>* Weights,
                                 const NCollection_Array1<double>& Knots,
                                 const NCollection_Array1<int>*    Mults,
                                 gp_Pnt&                           P,
                                 gp_Vec&                           V1,
                                 gp_Vec&                           V2);

  Standard_EXPORT static void D2(const double                        U,
                                 const int                           UIndex,
                                 const int                           Degree,
                                 const bool                          Periodic,
                                 const NCollection_Array1<gp_Pnt2d>& Poles,
                                 const NCollection_Array1<double>*   Weights,
                                 const NCollection_Array1<double>&   Knots,
                                 const NCollection_Array1<int>*      Mults,
                                 gp_Pnt2d&                           P,
                                 gp_Vec2d&                           V1,
                                 gp_Vec2d&                           V2);

  Standard_EXPORT static void D2(const double                      U,
                                 const NCollection_Array1<gp_Pnt>& Poles,
                                 const NCollection_Array1<double>* Weights,
                                 gp_Pnt&                           P,
                                 gp_Vec&                           V1,
                                 gp_Vec&                           V2);

  Standard_EXPORT static void D2(const double                        U,
                                 const NCollection_Array1<gp_Pnt2d>& Poles,
                                 const NCollection_Array1<double>*   Weights,
                                 gp_Pnt2d&                           P,
                                 gp_Vec2d&                           V1,
                                 gp_Vec2d&                           V2);

  Standard_EXPORT static void D3(const double                      U,
                                 const int                         Index,
                                 const int                         Degree,
                                 const bool                        Periodic,
                                 const NCollection_Array1<double>& Poles,
                                 const NCollection_Array1<double>* Weights,
                                 const NCollection_Array1<double>& Knots,
                                 const NCollection_Array1<int>*    Mults,
                                 double&                           P,
                                 double&                           V1,
                                 double&                           V2,
                                 double&                           V3);

  Standard_EXPORT static void D3(const double                      U,
                                 const int                         Index,
                                 const int                         Degree,
                                 const bool                        Periodic,
                                 const NCollection_Array1<gp_Pnt>& Poles,
                                 const NCollection_Array1<double>* Weights,
                                 const NCollection_Array1<double>& Knots,
                                 const NCollection_Array1<int>*    Mults,
                                 gp_Pnt&                           P,
                                 gp_Vec&                           V1,
                                 gp_Vec&                           V2,
                                 gp_Vec&                           V3);

  Standard_EXPORT static void D3(const double                        U,
                                 const int                           UIndex,
                                 const int                           Degree,
                                 const bool                          Periodic,
                                 const NCollection_Array1<gp_Pnt2d>& Poles,
                                 const NCollection_Array1<double>*   Weights,
                                 const NCollection_Array1<double>&   Knots,
                                 const NCollection_Array1<int>*      Mults,
                                 gp_Pnt2d&                           P,
                                 gp_Vec2d&                           V1,
                                 gp_Vec2d&                           V2,
                                 gp_Vec2d&                           V3);

  Standard_EXPORT static void D3(const double                      U,
                                 const NCollection_Array1<gp_Pnt>& Poles,
                                 const NCollection_Array1<double>* Weights,
                                 gp_Pnt&                           P,
                                 gp_Vec&                           V1,
                                 gp_Vec&                           V2,
                                 gp_Vec&                           V3);

  Standard_EXPORT static void D3(const double                        U,
                                 const NCollection_Array1<gp_Pnt2d>& Poles,
                                 const NCollection_Array1<double>*   Weights,
                                 gp_Pnt2d&                           P,
                                 gp_Vec2d&                           V1,
                                 gp_Vec2d&                           V2,
                                 gp_Vec2d&                           V3);

  Standard_EXPORT static void DN(const double                      U,
                                 const int                         N,
                                 const int                         Index,
                                 const int                         Degree,
                                 const bool                        Periodic,
                                 const NCollection_Array1<double>& Poles,
                                 const NCollection_Array1<double>* Weights,
                                 const NCollection_Array1<double>& Knots,
                                 const NCollection_Array1<int>*    Mults,
                                 double&                           VN);

  Standard_EXPORT static void DN(const double                      U,
                                 const int                         N,
                                 const int                         Index,
                                 const int                         Degree,
                                 const bool                        Periodic,
                                 const NCollection_Array1<gp_Pnt>& Poles,
                                 const NCollection_Array1<double>* Weights,
                                 const NCollection_Array1<double>& Knots,
                                 const NCollection_Array1<int>*    Mults,
                                 gp_Vec&                           VN);

  Standard_EXPORT static void DN(const double                        U,
                                 const int                           N,
                                 const int                           UIndex,
                                 const int                           Degree,
                                 const bool                          Periodic,
                                 const NCollection_Array1<gp_Pnt2d>& Poles,
                                 const NCollection_Array1<double>*   Weights,
                                 const NCollection_Array1<double>&   Knots,
                                 const NCollection_Array1<int>*      Mults,
                                 gp_Vec2d&                           V);

  Standard_EXPORT static void DN(const double                      U,
                                 const int                         N,
                                 const NCollection_Array1<gp_Pnt>& Poles,
                                 const NCollection_Array1<double>& Weights,
                                 gp_Pnt&                           P,
                                 gp_Vec&                           VN);

  //! The above functions compute values and
  //! derivatives in the following situations:
  //!
  //! * 3D, 2D and 1D
  //!
  //! * Rational or not Rational.
  //!
  //! * Knots and multiplicities or "flat knots" without
  //! multiplicities.
  //!
  //! * The <Index> is the localization of the
  //! parameter in the knot sequence. If <Index> is out
  //! of range the correct value will be searched.
  //!
  //! VERY IMPORTANT!!!
  //! USE BSplCLib::NoWeights() as Weights argument for non
  //! rational curves computations.
  Standard_EXPORT static void DN(const double                        U,
                                 const int                           N,
                                 const NCollection_Array1<gp_Pnt2d>& Poles,
                                 const NCollection_Array1<double>&   Weights,
                                 gp_Pnt2d&                           P,
                                 gp_Vec2d&                           VN);

  //! This evaluates the Bspline Basis at a
  //! given parameter Parameter up to the
  //! requested DerivativeOrder and store the
  //! result in the array BsplineBasis in the
  //! following fashion
  //! BSplineBasis(1,1)   =
  //! value of first non vanishing
  //! Bspline function which has Index FirstNonZeroBsplineIndex
  //! BsplineBasis(1,2)   =
  //! value of second non vanishing
  //! Bspline function which has Index
  //! FirstNonZeroBsplineIndex + 1
  //! BsplineBasis(1,n)   =
  //! value of second non vanishing non vanishing
  //! Bspline function which has Index
  //! FirstNonZeroBsplineIndex + n (n <= Order)
  //! BSplineBasis(2,1)   =
  //! value of derivative of first non vanishing
  //! Bspline function which has Index FirstNonZeroBsplineIndex
  //! BSplineBasis(N,1)   =
  //! value of Nth derivative of first non vanishing
  //! Bspline function which has Index FirstNonZeroBsplineIndex
  //! if N <= DerivativeOrder + 1
  Standard_EXPORT static int EvalBsplineBasis(const int                         DerivativeOrder,
                                              const int                         Order,
                                              const NCollection_Array1<double>& FlatKnots,
                                              const double                      Parameter,
                                              int&         FirstNonZeroBsplineIndex,
                                              math_Matrix& BsplineBasis,
                                              const bool   isPeriodic = false);

  //! This Builds a fully blown Matrix of
  //! (ni)
  //! Bi    (tj)
  //!
  //! with i and j within 1..Order + NumPoles
  //! The integer ni is the ith slot of the
  //! array OrderArray, tj is the jth slot of
  //! the array Parameters
  Standard_EXPORT static int BuildBSpMatrix(const NCollection_Array1<double>& Parameters,
                                            const NCollection_Array1<int>&    OrderArray,
                                            const NCollection_Array1<double>& FlatKnots,
                                            const int                         Degree,
                                            math_Matrix&                      Matrix,
                                            int&                              UpperBandWidth,
                                            int&                              LowerBandWidth);

  //! this factors the Banded Matrix in
  //! the LU form with a Banded storage of
  //! components of the L matrix
  //! WARNING : do not use if the Matrix is
  //! totally positive (It is the case for
  //! Bspline matrices build as above with
  //! parameters being the Schoenberg points
  Standard_EXPORT static int FactorBandedMatrix(math_Matrix& Matrix,
                                                const int    UpperBandWidth,
                                                const int    LowerBandWidth,
                                                int&         PivotIndexProblem);

  //! This solves the system Matrix.X = B
  //! with when Matrix is factored in LU form
  //! The Array is an seen as an
  //! Array[1..N][1..ArrayDimension] with N =
  //! the rank of the matrix Matrix. The
  //! result is stored in Array when each
  //! coordinate is solved that is B is the
  //! array whose values are
  //! B[i] = Array[i][p] for each p in 1..ArrayDimension
  Standard_EXPORT static int SolveBandedSystem(const math_Matrix& Matrix,
                                               const int          UpperBandWidth,
                                               const int          LowerBandWidth,
                                               const int          ArrayDimension,
                                               double&            Array);

  //! This solves the system Matrix.X = B
  //! with when Matrix is factored in LU form
  //! The Array has the length of
  //! the rank of the matrix Matrix. The
  //! result is stored in Array when each
  //! coordinate is solved that is B is the
  //! array whose values are
  //! B[i] = Array[i][p] for each p in 1..ArrayDimension
  Standard_EXPORT static int SolveBandedSystem(const math_Matrix&            Matrix,
                                               const int                     UpperBandWidth,
                                               const int                     LowerBandWidth,
                                               NCollection_Array1<gp_Pnt2d>& Array);

  //! This solves the system Matrix.X = B
  //! with when Matrix is factored in LU form
  //! The Array has the length of
  //! the rank of the matrix Matrix. The
  //! result is stored in Array when each
  //! coordinate is solved that is B is the
  //! array whose values are
  //! B[i] = Array[i][p] for each p in 1..ArrayDimension
  Standard_EXPORT static int SolveBandedSystem(const math_Matrix&          Matrix,
                                               const int                   UpperBandWidth,
                                               const int                   LowerBandWidth,
                                               NCollection_Array1<gp_Pnt>& Array);

  Standard_EXPORT static int SolveBandedSystem(const math_Matrix& Matrix,
                                               const int          UpperBandWidth,
                                               const int          LowerBandWidth,
                                               const bool         HomogenousFlag,
                                               const int          ArrayDimension,
                                               double&            Array,
                                               double&            Weights);

  //! This solves the system Matrix.X = B
  //! with when Matrix is factored in LU form
  //! The Array is an seen as an
  //! Array[1..N][1..ArrayDimension] with N =
  //! the rank of the matrix Matrix. The
  //! result is stored in Array when each
  //! coordinate is solved that is B is the
  //! array whose values are B[i] = Array[i][p]
  //! for each p in 1..ArrayDimension.
  //! If HomogeneousFlag == 0
  //! the Poles are multiplied by the
  //! Weights upon Entry and once
  //! interpolation is carried over the
  //! result of the poles are divided by the
  //! result of the interpolation of the
  //! weights. Otherwise if HomogenousFlag == 1
  //! the Poles and Weights are treated homogeneously
  //! that is that those are interpolated as they
  //! are and result is returned without division
  //! by the interpolated weights.
  Standard_EXPORT static int SolveBandedSystem(const math_Matrix&            Matrix,
                                               const int                     UpperBandWidth,
                                               const int                     LowerBandWidth,
                                               const bool                    HomogenousFlag,
                                               NCollection_Array1<gp_Pnt2d>& Array,
                                               NCollection_Array1<double>&   Weights);

  //! This solves the system Matrix.X = B
  //! with when Matrix is factored in LU form
  //! The Array is an seen as an
  //! Array[1..N][1..ArrayDimension] with N =
  //! the rank of the matrix Matrix. The
  //! result is stored in Array when each
  //! coordinate is solved that is B is the
  //! array whose values are B[i] = Array[i][p]
  //! for each p in 1..ArrayDimension
  //! If HomogeneousFlag == 0
  //! the Poles are multiplied by the
  //! Weights upon Entry and once
  //! interpolation is carried over the
  //! result of the poles are divided by the
  //! result of the interpolation of the
  //! weights. Otherwise if HomogenousFlag == 1
  //! the Poles and Weights are treated homogeneously
  //! that is that those are interpolated as they
  //! are and result is returned without division
  //! by the interpolated weights.
  Standard_EXPORT static int SolveBandedSystem(const math_Matrix&          Matrix,
                                               const int                   UpperBandWidth,
                                               const int                   LowerBandWidth,
                                               const bool                  HomogeneousFlag,
                                               NCollection_Array1<gp_Pnt>& Array,
                                               NCollection_Array1<double>& Weights);

  //! Merges two knot vector by setting the starting and
  //! ending values to StartValue and EndValue
  Standard_EXPORT static void MergeBSplineKnots(const double                      Tolerance,
                                                const double                      StartValue,
                                                const double                      EndValue,
                                                const int                         Degree1,
                                                const NCollection_Array1<double>& Knots1,
                                                const NCollection_Array1<int>&    Mults1,
                                                const int                         Degree2,
                                                const NCollection_Array1<double>& Knots2,
                                                const NCollection_Array1<int>&    Mults2,
                                                int&                              NumPoles,
                                                occ::handle<NCollection_HArray1<double>>& NewKnots,
                                                occ::handle<NCollection_HArray1<int>>&    NewMults);

  //! This function will compose a given Vectorial BSpline F(t)
  //! defined by its BSplineDegree and BSplineFlatKnotsl,
  //! its Poles array which are coded as an array of Real
  //! of the form [1..NumPoles][1..PolesDimension] with a
  //! function a(t) which is assumed to satisfy the
  //! following:
  //!
  //! 1. F(a(t)) is a polynomial BSpline
  //! that can be expressed exactly as a BSpline of degree
  //! NewDegree on the knots FlatKnots
  //!
  //! 2. a(t) defines a differentiable
  //! isomorphism between the range of FlatKnots to the range
  //! of BSplineFlatKnots which is the
  //! same as the range of F(t)
  //!
  //! Warning: it is
  //! the caller's responsibility to insure that conditions
  //! 1. and 2. above are satisfied: no check whatsoever
  //! is made in this method
  //!
  //! theStatus will return 0 if OK else it will return the pivot index
  //! of the matrix that was inverted to compute the multiplied
  //! BSpline: the method used is interpolation at Schoenenberg
  //! points of F(a(t))
  Standard_EXPORT static void FunctionReparameterise(
    const BSplCLib_EvaluatorFunction& Function,
    const int                         BSplineDegree,
    const NCollection_Array1<double>& BSplineFlatKnots,
    const int                         PolesDimension,
    double&                           Poles,
    const NCollection_Array1<double>& FlatKnots,
    const int                         NewDegree,
    double&                           NewPoles,
    int&                              theStatus);

  //! This function will compose a given Vectorial BSpline F(t)
  //! defined by its BSplineDegree and BSplineFlatKnotsl,
  //! its Poles array which are coded as an array of Real
  //! of the form [1..NumPoles][1..PolesDimension] with a
  //! function a(t) which is assumed to satisfy the
  //! following:
  //!
  //! 1. F(a(t)) is a polynomial BSpline
  //! that can be expressed exactly as a BSpline of degree
  //! NewDegree on the knots FlatKnots
  //!
  //! 2. a(t) defines a differentiable
  //! isomorphism between the range of FlatKnots to the range
  //! of BSplineFlatKnots which is the
  //! same as the range of F(t)
  //!
  //! Warning: it is
  //! the caller's responsibility to insure that conditions
  //! 1. and 2. above are satisfied: no check whatsoever
  //! is made in this method
  //!
  //! theStatus will return 0 if OK else it will return the pivot index
  //! of the matrix that was inverted to compute the multiplied
  //! BSpline: the method used is interpolation at Schoenenberg
  //! points of F(a(t))
  Standard_EXPORT static void FunctionReparameterise(
    const BSplCLib_EvaluatorFunction& Function,
    const int                         BSplineDegree,
    const NCollection_Array1<double>& BSplineFlatKnots,
    const NCollection_Array1<double>& Poles,
    const NCollection_Array1<double>& FlatKnots,
    const int                         NewDegree,
    NCollection_Array1<double>&       NewPoles,
    int&                              theStatus);

  //! this will compose a given Vectorial BSpline F(t)
  //! defined by its BSplineDegree and BSplineFlatKnotsl,
  //! its Poles array which are coded as an array of Real
  //! of the form [1..NumPoles][1..PolesDimension] with a
  //! function a(t) which is assumed to satisfy the
  //! following: 1. F(a(t)) is a polynomial BSpline
  //! that can be expressed exactly as a BSpline of degree
  //! NewDegree on the knots FlatKnots
  //! 2. a(t) defines a differentiable
  //! isomorphism between the range of FlatKnots to the range
  //! of BSplineFlatKnots which is the
  //! same as the range of F(t)
  //! Warning: it is
  //! the caller's responsibility to insure that conditions
  //! 1. and 2. above are satisfied: no check whatsoever
  //! is made in this method
  //! theStatus will return 0 if OK else it will return the pivot index
  //! of the matrix that was inverted to compute the multiplied
  //! BSpline: the method used is interpolation at Schoenenberg
  //! points of F(a(t))
  Standard_EXPORT static void FunctionReparameterise(
    const BSplCLib_EvaluatorFunction& Function,
    const int                         BSplineDegree,
    const NCollection_Array1<double>& BSplineFlatKnots,
    const NCollection_Array1<gp_Pnt>& Poles,
    const NCollection_Array1<double>& FlatKnots,
    const int                         NewDegree,
    NCollection_Array1<gp_Pnt>&       NewPoles,
    int&                              theStatus);

  //! this will compose a given Vectorial BSpline F(t)
  //! defined by its BSplineDegree and BSplineFlatKnotsl,
  //! its Poles array which are coded as an array of Real
  //! of the form [1..NumPoles][1..PolesDimension] with a
  //! function a(t) which is assumed to satisfy the
  //! following: 1. F(a(t)) is a polynomial BSpline
  //! that can be expressed exactly as a BSpline of degree
  //! NewDegree on the knots FlatKnots
  //! 2. a(t) defines a differentiable
  //! isomorphism between the range of FlatKnots to the range
  //! of BSplineFlatKnots which is the
  //! same as the range of F(t)
  //! Warning: it is
  //! the caller's responsibility to insure that conditions
  //! 1. and 2. above are satisfied: no check whatsoever
  //! is made in this method
  //! theStatus will return 0 if OK else it will return the pivot index
  //! of the matrix that was inverted to compute the multiplied
  //! BSpline: the method used is interpolation at Schoenenberg
  //! points of F(a(t))
  Standard_EXPORT static void FunctionReparameterise(
    const BSplCLib_EvaluatorFunction&   Function,
    const int                           BSplineDegree,
    const NCollection_Array1<double>&   BSplineFlatKnots,
    const NCollection_Array1<gp_Pnt2d>& Poles,
    const NCollection_Array1<double>&   FlatKnots,
    const int                           NewDegree,
    NCollection_Array1<gp_Pnt2d>&       NewPoles,
    int&                                theStatus);

  //! this will multiply a given Vectorial BSpline F(t)
  //! defined by its BSplineDegree and BSplineFlatKnotsl,
  //! its Poles array which are coded as an array of Real
  //! of the form [1..NumPoles][1..PolesDimension] by a
  //! function a(t) which is assumed to satisfy the
  //! following: 1. a(t) * F(t) is a polynomial BSpline
  //! that can be expressed exactly as a BSpline of degree
  //! NewDegree on the knots FlatKnots 2. the range of a(t)
  //! is the same as the range of F(t)
  //! Warning: it is
  //! the caller's responsibility to insure that conditions
  //! 1. and 2. above are satisfied: no check whatsoever
  //! is made in this method
  //! theStatus will return 0 if OK else it will return the pivot index
  //! of the matrix that was inverted to compute the multiplied
  //! BSpline: the method used is interpolation at Schoenenberg
  //! points of a(t)*F(t)
  Standard_EXPORT static void FunctionMultiply(const BSplCLib_EvaluatorFunction& Function,
                                               const int                         BSplineDegree,
                                               const NCollection_Array1<double>& BSplineFlatKnots,
                                               const int                         PolesDimension,
                                               double&                           Poles,
                                               const NCollection_Array1<double>& FlatKnots,
                                               const int                         NewDegree,
                                               double&                           NewPoles,
                                               int&                              theStatus);

  //! this will multiply a given Vectorial BSpline F(t)
  //! defined by its BSplineDegree and BSplineFlatKnotsl,
  //! its Poles array which are coded as an array of Real
  //! of the form [1..NumPoles][1..PolesDimension] by a
  //! function a(t) which is assumed to satisfy the
  //! following: 1. a(t) * F(t) is a polynomial BSpline
  //! that can be expressed exactly as a BSpline of degree
  //! NewDegree on the knots FlatKnots 2. the range of a(t)
  //! is the same as the range of F(t)
  //! Warning: it is
  //! the caller's responsibility to insure that conditions
  //! 1. and 2. above are satisfied: no check whatsoever
  //! is made in this method
  //! theStatus will return 0 if OK else it will return the pivot index
  //! of the matrix that was inverted to compute the multiplied
  //! BSpline: the method used is interpolation at Schoenenberg
  //! points of a(t)*F(t)
  Standard_EXPORT static void FunctionMultiply(const BSplCLib_EvaluatorFunction& Function,
                                               const int                         BSplineDegree,
                                               const NCollection_Array1<double>& BSplineFlatKnots,
                                               const NCollection_Array1<double>& Poles,
                                               const NCollection_Array1<double>& FlatKnots,
                                               const int                         NewDegree,
                                               NCollection_Array1<double>&       NewPoles,
                                               int&                              theStatus);

  //! this will multiply a given Vectorial BSpline F(t)
  //! defined by its BSplineDegree and BSplineFlatKnotsl,
  //! its Poles array which are coded as an array of Real
  //! of the form [1..NumPoles][1..PolesDimension] by a
  //! function a(t) which is assumed to satisfy the
  //! following: 1. a(t) * F(t) is a polynomial BSpline
  //! that can be expressed exactly as a BSpline of degree
  //! NewDegree on the knots FlatKnots 2. the range of a(t)
  //! is the same as the range of F(t)
  //! Warning: it is
  //! the caller's responsibility to insure that conditions
  //! 1. and 2. above are satisfied: no check whatsoever
  //! is made in this method
  //! theStatus will return 0 if OK else it will return the pivot index
  //! of the matrix that was inverted to compute the multiplied
  //! BSpline: the method used is interpolation at Schoenenberg
  //! points of a(t)*F(t)
  Standard_EXPORT static void FunctionMultiply(const BSplCLib_EvaluatorFunction&   Function,
                                               const int                           BSplineDegree,
                                               const NCollection_Array1<double>&   BSplineFlatKnots,
                                               const NCollection_Array1<gp_Pnt2d>& Poles,
                                               const NCollection_Array1<double>&   FlatKnots,
                                               const int                           NewDegree,
                                               NCollection_Array1<gp_Pnt2d>&       NewPoles,
                                               int&                                theStatus);

  //! this will multiply a given Vectorial BSpline F(t)
  //! defined by its BSplineDegree and BSplineFlatKnotsl,
  //! its Poles array which are coded as an array of Real
  //! of the form [1..NumPoles][1..PolesDimension] by a
  //! function a(t) which is assumed to satisfy the
  //! following: 1. a(t) * F(t) is a polynomial BSpline
  //! that can be expressed exactly as a BSpline of degree
  //! NewDegree on the knots FlatKnots 2. the range of a(t)
  //! is the same as the range of F(t)
  //! Warning: it is
  //! the caller's responsibility to insure that conditions
  //! 1. and 2. above are satisfied: no check whatsoever
  //! is made in this method
  //! theStatus will return 0 if OK else it will return the pivot index
  //! of the matrix that was inverted to compute the multiplied
  //! BSpline: the method used is interpolation at Schoenenberg
  //! points of a(t)*F(t)
  Standard_EXPORT static void FunctionMultiply(const BSplCLib_EvaluatorFunction& Function,
                                               const int                         BSplineDegree,
                                               const NCollection_Array1<double>& BSplineFlatKnots,
                                               const NCollection_Array1<gp_Pnt>& Poles,
                                               const NCollection_Array1<double>& FlatKnots,
                                               const int                         NewDegree,
                                               NCollection_Array1<gp_Pnt>&       NewPoles,
                                               int&                              theStatus);

  //! Perform the De Boor algorithm to evaluate a point at
  //! parameter <U>, with <Degree> and <Dimension>.
  //!
  //! Poles is an array of Reals of size
  //!
  //! <Dimension> * <Degree>+1
  //!
  //! Containing the poles. At the end <Poles> contains
  //! the current point. Poles Contain all the poles of
  //! the BsplineCurve, Knots also Contains all the knots
  //! of the BsplineCurve. ExtrapMode has two slots [0] =
  //! Degree used to extrapolate before the first knot [1]
  //! = Degre used to extrapolate after the last knot has
  //! to be between 1 and Degree
  Standard_EXPORT static void Eval(const double                      U,
                                   const bool                        PeriodicFlag,
                                   const int                         DerivativeRequest,
                                   int&                              ExtrapMode,
                                   const int                         Degree,
                                   const NCollection_Array1<double>& FlatKnots,
                                   const int                         ArrayDimension,
                                   double&                           Poles,
                                   double&                           Result);

  //! Perform the De Boor algorithm to evaluate a point at
  //! parameter <U>, with <Degree> and <Dimension>.
  //! Evaluates by multiplying the Poles by the Weights and
  //! gives the homogeneous result in PolesResult that is
  //! the results of the evaluation of the numerator once it
  //! has been multiplied by the weights and in
  //! WeightsResult one has the result of the evaluation of
  //! the denominator
  //!
  //! Warning: <PolesResult> and <WeightsResult> must be
  //! dimensioned properly.
  Standard_EXPORT static void Eval(const double                      U,
                                   const bool                        PeriodicFlag,
                                   const int                         DerivativeRequest,
                                   int&                              ExtrapMode,
                                   const int                         Degree,
                                   const NCollection_Array1<double>& FlatKnots,
                                   const int                         ArrayDimension,
                                   double&                           Poles,
                                   double&                           Weights,
                                   double&                           PolesResult,
                                   double&                           WeightsResult);

  //! Perform the evaluation of the Bspline Basis
  //! and then multiplies by the weights
  //! this just evaluates the current point
  Standard_EXPORT static void Eval(const double                      U,
                                   const bool                        PeriodicFlag,
                                   const bool                        HomogeneousFlag,
                                   int&                              ExtrapMode,
                                   const int                         Degree,
                                   const NCollection_Array1<double>& FlatKnots,
                                   const NCollection_Array1<gp_Pnt>& Poles,
                                   const NCollection_Array1<double>& Weights,
                                   gp_Pnt&                           Point,
                                   double&                           Weight);

  //! Perform the evaluation of the Bspline Basis
  //! and then multiplies by the weights
  //! this just evaluates the current point
  Standard_EXPORT static void Eval(const double                        U,
                                   const bool                          PeriodicFlag,
                                   const bool                          HomogeneousFlag,
                                   int&                                ExtrapMode,
                                   const int                           Degree,
                                   const NCollection_Array1<double>&   FlatKnots,
                                   const NCollection_Array1<gp_Pnt2d>& Poles,
                                   const NCollection_Array1<double>&   Weights,
                                   gp_Pnt2d&                           Point,
                                   double&                             Weight);

  //! Extend a BSpline nD using the tangency map
  //! <C1Coefficient> is the coefficient of reparametrisation
  //! <Continuity> must be equal to 1, 2 or 3.
  //! <Degree> must be greater or equal than <Continuity> + 1.
  //!
  //! Warning: <KnotsResult> and <PolesResult> must be dimensioned
  //! properly.
  Standard_EXPORT static void TangExtendToConstraint(
    const NCollection_Array1<double>& FlatKnots,
    const double                      C1Coefficient,
    const int                         NumPoles,
    double&                           Poles,
    const int                         Dimension,
    const int                         Degree,
    const NCollection_Array1<double>& ConstraintPoint,
    const int                         Continuity,
    const bool                        After,
    int&                              NbPolesResult,
    int&                              NbKnotsRsult,
    double&                           KnotsResult,
    double&                           PolesResult);

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
  Standard_EXPORT static void CacheD0(const double                      U,
                                      const int                         Degree,
                                      const double                      CacheParameter,
                                      const double                      SpanLenght,
                                      const NCollection_Array1<gp_Pnt>& Poles,
                                      const NCollection_Array1<double>* Weights,
                                      gp_Pnt&                           Point);

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
  Standard_EXPORT static void CacheD0(const double                        U,
                                      const int                           Degree,
                                      const double                        CacheParameter,
                                      const double                        SpanLenght,
                                      const NCollection_Array1<gp_Pnt2d>& Poles,
                                      const NCollection_Array1<double>*   Weights,
                                      gp_Pnt2d&                           Point);

  //! Calls CacheD0 for Bezier Curves Arrays computed with
  //! the method PolesCoefficients.
  //! Warning: To be used for Beziercurves ONLY!!!
  static void CoefsD0(const double                      U,
                      const NCollection_Array1<gp_Pnt>& Poles,
                      const NCollection_Array1<double>* Weights,
                      gp_Pnt&                           Point);

  //! Calls CacheD0 for Bezier Curves Arrays computed with
  //! the method PolesCoefficients.
  //! Warning: To be used for Beziercurves ONLY!!!
  static void CoefsD0(const double                        U,
                      const NCollection_Array1<gp_Pnt2d>& Poles,
                      const NCollection_Array1<double>*   Weights,
                      gp_Pnt2d&                           Point);

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
  Standard_EXPORT static void CacheD1(const double                      U,
                                      const int                         Degree,
                                      const double                      CacheParameter,
                                      const double                      SpanLenght,
                                      const NCollection_Array1<gp_Pnt>& Poles,
                                      const NCollection_Array1<double>* Weights,
                                      gp_Pnt&                           Point,
                                      gp_Vec&                           Vec);

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
  Standard_EXPORT static void CacheD1(const double                        U,
                                      const int                           Degree,
                                      const double                        CacheParameter,
                                      const double                        SpanLenght,
                                      const NCollection_Array1<gp_Pnt2d>& Poles,
                                      const NCollection_Array1<double>*   Weights,
                                      gp_Pnt2d&                           Point,
                                      gp_Vec2d&                           Vec);

  //! Calls CacheD1 for Bezier Curves Arrays computed with
  //! the method PolesCoefficients.
  //! Warning: To be used for Beziercurves ONLY!!!
  static void CoefsD1(const double                      U,
                      const NCollection_Array1<gp_Pnt>& Poles,
                      const NCollection_Array1<double>* Weights,
                      gp_Pnt&                           Point,
                      gp_Vec&                           Vec);

  //! Calls CacheD1 for Bezier Curves Arrays computed with
  //! the method PolesCoefficients.
  //! Warning: To be used for Beziercurves ONLY!!!
  static void CoefsD1(const double                        U,
                      const NCollection_Array1<gp_Pnt2d>& Poles,
                      const NCollection_Array1<double>*   Weights,
                      gp_Pnt2d&                           Point,
                      gp_Vec2d&                           Vec);

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
  Standard_EXPORT static void CacheD2(const double                      U,
                                      const int                         Degree,
                                      const double                      CacheParameter,
                                      const double                      SpanLenght,
                                      const NCollection_Array1<gp_Pnt>& Poles,
                                      const NCollection_Array1<double>* Weights,
                                      gp_Pnt&                           Point,
                                      gp_Vec&                           Vec1,
                                      gp_Vec&                           Vec2);

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
  Standard_EXPORT static void CacheD2(const double                        U,
                                      const int                           Degree,
                                      const double                        CacheParameter,
                                      const double                        SpanLenght,
                                      const NCollection_Array1<gp_Pnt2d>& Poles,
                                      const NCollection_Array1<double>*   Weights,
                                      gp_Pnt2d&                           Point,
                                      gp_Vec2d&                           Vec1,
                                      gp_Vec2d&                           Vec2);

  //! Calls CacheD1 for Bezier Curves Arrays computed with
  //! the method PolesCoefficients.
  //! Warning: To be used for Beziercurves ONLY!!!
  static void CoefsD2(const double                      U,
                      const NCollection_Array1<gp_Pnt>& Poles,
                      const NCollection_Array1<double>* Weights,
                      gp_Pnt&                           Point,
                      gp_Vec&                           Vec1,
                      gp_Vec&                           Vec2);

  //! Calls CacheD1 for Bezier Curves Arrays computed with
  //! the method PolesCoefficients.
  //! Warning: To be used for Beziercurves ONLY!!!
  static void CoefsD2(const double                        U,
                      const NCollection_Array1<gp_Pnt2d>& Poles,
                      const NCollection_Array1<double>*   Weights,
                      gp_Pnt2d&                           Point,
                      gp_Vec2d&                           Vec1,
                      gp_Vec2d&                           Vec2);

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
  Standard_EXPORT static void CacheD3(const double                      U,
                                      const int                         Degree,
                                      const double                      CacheParameter,
                                      const double                      SpanLenght,
                                      const NCollection_Array1<gp_Pnt>& Poles,
                                      const NCollection_Array1<double>* Weights,
                                      gp_Pnt&                           Point,
                                      gp_Vec&                           Vec1,
                                      gp_Vec&                           Vec2,
                                      gp_Vec&                           Vec3);

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
  Standard_EXPORT static void CacheD3(const double                        U,
                                      const int                           Degree,
                                      const double                        CacheParameter,
                                      const double                        SpanLenght,
                                      const NCollection_Array1<gp_Pnt2d>& Poles,
                                      const NCollection_Array1<double>*   Weights,
                                      gp_Pnt2d&                           Point,
                                      gp_Vec2d&                           Vec1,
                                      gp_Vec2d&                           Vec2,
                                      gp_Vec2d&                           Vec3);

  //! Calls CacheD1 for Bezier Curves Arrays computed with
  //! the method PolesCoefficients.
  //! Warning: To be used for Beziercurves ONLY!!!
  static void CoefsD3(const double                      U,
                      const NCollection_Array1<gp_Pnt>& Poles,
                      const NCollection_Array1<double>* Weights,
                      gp_Pnt&                           Point,
                      gp_Vec&                           Vec1,
                      gp_Vec&                           Vec2,
                      gp_Vec&                           Vec3);

  //! Calls CacheD1 for Bezier Curves Arrays computed with
  //! the method PolesCoefficients.
  //! Warning: To be used for Beziercurves ONLY!!!
  static void CoefsD3(const double                        U,
                      const NCollection_Array1<gp_Pnt2d>& Poles,
                      const NCollection_Array1<double>*   Weights,
                      gp_Pnt2d&                           Point,
                      gp_Vec2d&                           Vec1,
                      gp_Vec2d&                           Vec2,
                      gp_Vec2d&                           Vec3);

  //! Perform the evaluation of the Taylor expansion
  //! of the Bspline normalized between 0 and 1.
  //! If rational computes the homogeneous Taylor expansion
  //! for the numerator and stores it in CachePoles
  Standard_EXPORT static void BuildCache(const double                      U,
                                         const double                      InverseOfSpanDomain,
                                         const bool                        PeriodicFlag,
                                         const int                         Degree,
                                         const NCollection_Array1<double>& FlatKnots,
                                         const NCollection_Array1<gp_Pnt>& Poles,
                                         const NCollection_Array1<double>* Weights,
                                         NCollection_Array1<gp_Pnt>&       CachePoles,
                                         NCollection_Array1<double>*       CacheWeights);

  //! Perform the evaluation of the Taylor expansion
  //! of the Bspline normalized between 0 and 1.
  //! If rational computes the homogeneous Taylor expansion
  //! for the numerator and stores it in CachePoles
  Standard_EXPORT static void BuildCache(const double                        U,
                                         const double                        InverseOfSpanDomain,
                                         const bool                          PeriodicFlag,
                                         const int                           Degree,
                                         const NCollection_Array1<double>&   FlatKnots,
                                         const NCollection_Array1<gp_Pnt2d>& Poles,
                                         const NCollection_Array1<double>*   Weights,
                                         NCollection_Array1<gp_Pnt2d>&       CachePoles,
                                         NCollection_Array1<double>*         CacheWeights);

  //! Perform the evaluation of the Taylor expansion
  //! of the Bspline normalized between 0 and 1.
  //! Structure of result optimized for BSplCLib_Cache.
  Standard_EXPORT static void BuildCache(const double                      theParameter,
                                         const double                      theSpanDomain,
                                         const bool                        thePeriodicFlag,
                                         const int                         theDegree,
                                         const int                         theSpanIndex,
                                         const NCollection_Array1<double>& theFlatKnots,
                                         const NCollection_Array1<gp_Pnt>& thePoles,
                                         const NCollection_Array1<double>* theWeights,
                                         NCollection_Array2<double>&       theCacheArray);

  //! Perform the evaluation of the Taylor expansion
  //! of the Bspline normalized between 0 and 1.
  //! Structure of result optimized for BSplCLib_Cache.
  Standard_EXPORT static void BuildCache(const double                        theParameter,
                                         const double                        theSpanDomain,
                                         const bool                          thePeriodicFlag,
                                         const int                           theDegree,
                                         const int                           theSpanIndex,
                                         const NCollection_Array1<double>&   theFlatKnots,
                                         const NCollection_Array1<gp_Pnt2d>& thePoles,
                                         const NCollection_Array1<double>*   theWeights,
                                         NCollection_Array2<double>&         theCacheArray);

  static void PolesCoefficients(const NCollection_Array1<gp_Pnt2d>& Poles,
                                NCollection_Array1<gp_Pnt2d>&       CachePoles);

  Standard_EXPORT static void PolesCoefficients(const NCollection_Array1<gp_Pnt2d>& Poles,
                                                const NCollection_Array1<double>*   Weights,
                                                NCollection_Array1<gp_Pnt2d>&       CachePoles,
                                                NCollection_Array1<double>*         CacheWeights);

  static void PolesCoefficients(const NCollection_Array1<gp_Pnt>& Poles,
                                NCollection_Array1<gp_Pnt>&       CachePoles);

  //! Encapsulation of BuildCache to perform the
  //! evaluation of the Taylor expansion for beziercurves
  //! at parameter 0.
  //! Warning: To be used for Beziercurves ONLY!!!
  Standard_EXPORT static void PolesCoefficients(const NCollection_Array1<gp_Pnt>& Poles,
                                                const NCollection_Array1<double>* Weights,
                                                NCollection_Array1<gp_Pnt>&       CachePoles,
                                                NCollection_Array1<double>*       CacheWeights);

  //! Returns pointer to statically allocated array representing
  //! flat knots for bezier curve of the specified degree.
  //! Raises OutOfRange if Degree > MaxDegree()
  Standard_EXPORT static const double& FlatBezierKnots(const int Degree);

  //! builds the Schoenberg points from the flat knot
  //! used to interpolate a BSpline since the
  //! BSpline matrix is invertible.
  Standard_EXPORT static void BuildSchoenbergPoints(const int                         Degree,
                                                    const NCollection_Array1<double>& FlatKnots,
                                                    NCollection_Array1<double>&       Parameters);

  //! Performs the interpolation of the data given in
  //! the Poles array according to the requests in
  //! ContactOrderArray that is: if
  //! ContactOrderArray(i) has value d it means that
  //! Poles(i) contains the dth derivative of the
  //! function to be interpolated. The length L of the
  //! following arrays must be the same:
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
  Standard_EXPORT static void Interpolate(const int                         Degree,
                                          const NCollection_Array1<double>& FlatKnots,
                                          const NCollection_Array1<double>& Parameters,
                                          const NCollection_Array1<int>&    ContactOrderArray,
                                          NCollection_Array1<gp_Pnt>&       Poles,
                                          int&                              InversionProblem);

  //! Performs the interpolation of the data given in
  //! the Poles array according to the requests in
  //! ContactOrderArray that is: if
  //! ContactOrderArray(i) has value d it means that
  //! Poles(i) contains the dth derivative of the
  //! function to be interpolated. The length L of the
  //! following arrays must be the same:
  //! Parameters, ContactOrderArray, Poles,
  //! The length of FlatKnots is Degree + L + 1
  //! Warning:
  //! the method used to do that interpolation is
  //! gauss elimination WITHOUT pivoting. Thus if the
  //! diagonal is not dominant there is no guarantee
  //! that the algorithm will work. Nevertheless for
  //! Cubic interpolation at knots or interpolation at
  //! Scheonberg points the method will work.
  //! The InversionProblem will report 0 if there was no
  //! problem else it will give the index of the faulty
  //! pivot
  Standard_EXPORT static void Interpolate(const int                         Degree,
                                          const NCollection_Array1<double>& FlatKnots,
                                          const NCollection_Array1<double>& Parameters,
                                          const NCollection_Array1<int>&    ContactOrderArray,
                                          NCollection_Array1<gp_Pnt2d>&     Poles,
                                          int&                              InversionProblem);

  //! Performs the interpolation of the data given in
  //! the Poles array according to the requests in
  //! ContactOrderArray that is: if
  //! ContactOrderArray(i) has value d it means that
  //! Poles(i) contains the dth derivative of the
  //! function to be interpolated. The length L of the
  //! following arrays must be the same:
  //! Parameters, ContactOrderArray, Poles,
  //! The length of FlatKnots is Degree + L + 1
  //! Warning:
  //! the method used to do that interpolation is
  //! gauss elimination WITHOUT pivoting. Thus if the
  //! diagonal is not dominant there is no guarantee
  //! that the algorithm will work. Nevertheless for
  //! Cubic interpolation at knots or interpolation at
  //! Scheonberg points the method will work.
  //! The InversionProblem will report 0 if there was no
  //! problem else it will give the index of the faulty
  //! pivot
  Standard_EXPORT static void Interpolate(const int                         Degree,
                                          const NCollection_Array1<double>& FlatKnots,
                                          const NCollection_Array1<double>& Parameters,
                                          const NCollection_Array1<int>&    ContactOrderArray,
                                          NCollection_Array1<gp_Pnt>&       Poles,
                                          NCollection_Array1<double>&       Weights,
                                          int&                              InversionProblem);

  //! Performs the interpolation of the data given in
  //! the Poles array according to the requests in
  //! ContactOrderArray that is: if
  //! ContactOrderArray(i) has value d it means that
  //! Poles(i) contains the dth derivative of the
  //! function to be interpolated. The length L of the
  //! following arrays must be the same:
  //! Parameters, ContactOrderArray, Poles,
  //! The length of FlatKnots is Degree + L + 1
  //! Warning:
  //! the method used to do that interpolation is
  //! gauss elimination WITHOUT pivoting. Thus if the
  //! diagonal is not dominant there is no guarantee
  //! that the algorithm will work. Nevertheless for
  //! Cubic interpolation at knots or interpolation at
  //! Scheonberg points the method will work.
  //! The InversionProblem will report 0 if there was
  //! no problem else it will give the i
  Standard_EXPORT static void Interpolate(const int                         Degree,
                                          const NCollection_Array1<double>& FlatKnots,
                                          const NCollection_Array1<double>& Parameters,
                                          const NCollection_Array1<int>&    ContactOrderArray,
                                          NCollection_Array1<gp_Pnt2d>&     Poles,
                                          NCollection_Array1<double>&       Weights,
                                          int&                              InversionProblem);

  //! Performs the interpolation of the data given in
  //! the Poles array according to the requests in
  //! ContactOrderArray that is: if
  //! ContactOrderArray(i) has value d it means that
  //! Poles(i) contains the dth derivative of the
  //! function to be interpolated. The length L of the
  //! following arrays must be the same:
  //! Parameters, ContactOrderArray
  //! The length of FlatKnots is Degree + L + 1
  //! The PolesArray is an seen as an
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
  Standard_EXPORT static void Interpolate(const int                         Degree,
                                          const NCollection_Array1<double>& FlatKnots,
                                          const NCollection_Array1<double>& Parameters,
                                          const NCollection_Array1<int>&    ContactOrderArray,
                                          const int                         ArrayDimension,
                                          double&                           Poles,
                                          int&                              InversionProblem);

  Standard_EXPORT static void Interpolate(const int                         Degree,
                                          const NCollection_Array1<double>& FlatKnots,
                                          const NCollection_Array1<double>& Parameters,
                                          const NCollection_Array1<int>&    ContactOrderArray,
                                          const int                         ArrayDimension,
                                          double&                           Poles,
                                          double&                           Weights,
                                          int&                              InversionProblem);

  //! Find the new poles which allows an old point (with a
  //! given <u> as parameter) to reach a new position
  //! Index1 and Index2 indicate the range of poles we can move
  //! (1, NbPoles-1) or (2, NbPoles) -> no constraint for one side
  //! don't enter (1,NbPoles) -> error: rigid move
  //! (2, NbPoles-1) -> the ends are enforced
  //! (3, NbPoles-2) -> the ends and the tangency are enforced
  //! if Problem in BSplineBasis calculation, no change for the curve
  //! and FirstIndex, LastIndex = 0
  Standard_EXPORT static void MovePoint(const double                        U,
                                        const gp_Vec2d&                     Displ,
                                        const int                           Index1,
                                        const int                           Index2,
                                        const int                           Degree,
                                        const NCollection_Array1<gp_Pnt2d>& Poles,
                                        const NCollection_Array1<double>*   Weights,
                                        const NCollection_Array1<double>&   FlatKnots,
                                        int&                                FirstIndex,
                                        int&                                LastIndex,
                                        NCollection_Array1<gp_Pnt2d>&       NewPoles);

  //! Find the new poles which allows an old point (with a
  //! given <u> as parameter) to reach a new position
  //! Index1 and Index2 indicate the range of poles we can move
  //! (1, NbPoles-1) or (2, NbPoles) -> no constraint for one side
  //! don't enter (1,NbPoles) -> error: rigid move
  //! (2, NbPoles-1) -> the ends are enforced
  //! (3, NbPoles-2) -> the ends and the tangency are enforced
  //! if Problem in BSplineBasis calculation, no change for the curve
  //! and FirstIndex, LastIndex = 0
  Standard_EXPORT static void MovePoint(const double                      U,
                                        const gp_Vec&                     Displ,
                                        const int                         Index1,
                                        const int                         Index2,
                                        const int                         Degree,
                                        const NCollection_Array1<gp_Pnt>& Poles,
                                        const NCollection_Array1<double>* Weights,
                                        const NCollection_Array1<double>& FlatKnots,
                                        int&                              FirstIndex,
                                        int&                              LastIndex,
                                        NCollection_Array1<gp_Pnt>&       NewPoles);

  //! This is the dimension free version of the utility
  //! U is the parameter must be within the first FlatKnots and the
  //! last FlatKnots Delta is the amount the curve has to be moved
  //! DeltaDerivative is the amount the derivative has to be moved.
  //! Delta and DeltaDerivative must be array of dimension
  //! ArrayDimension Degree is the degree of the BSpline and the
  //! FlatKnots are the knots of the BSpline Starting Condition if =
  //! -1 means the starting point of the curve can move
  //! = 0 means the
  //! starting point of the curve cannot move but tangent starting
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
  Standard_EXPORT static void MovePointAndTangent(const double U,
                                                  const int    ArrayDimension,
                                                  double&      Delta,
                                                  double&      DeltaDerivative,
                                                  const double Tolerance,
                                                  const int    Degree,
                                                  const int    StartingCondition,
                                                  const int    EndingCondition,
                                                  double&      Poles,
                                                  const NCollection_Array1<double>* Weights,
                                                  const NCollection_Array1<double>& FlatKnots,
                                                  double&                           NewPoles,
                                                  int&                              ErrorStatus);

  //! This is the dimension free version of the utility
  //! U is the parameter must be within the first FlatKnots and the
  //! last FlatKnots Delta is the amount the curve has to be moved
  //! DeltaDerivative is the amount the derivative has to be moved.
  //! Delta and DeltaDerivative must be array of dimension
  //! ArrayDimension Degree is the degree of the BSpline and the
  //! FlatKnots are the knots of the BSpline Starting Condition if =
  //! -1 means the starting point of the curve can move
  //! = 0 means the
  //! starting point of the curve cannot move but tangent starting
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
  Standard_EXPORT static void MovePointAndTangent(const double  U,
                                                  const gp_Vec& Delta,
                                                  const gp_Vec& DeltaDerivative,
                                                  const double  Tolerance,
                                                  const int     Degree,
                                                  const int     StartingCondition,
                                                  const int     EndingCondition,
                                                  const NCollection_Array1<gp_Pnt>& Poles,
                                                  const NCollection_Array1<double>* Weights,
                                                  const NCollection_Array1<double>& FlatKnots,
                                                  NCollection_Array1<gp_Pnt>&       NewPoles,
                                                  int&                              ErrorStatus);

  //! This is the dimension free version of the utility
  //! U is the parameter must be within the first FlatKnots and the
  //! last FlatKnots Delta is the amount the curve has to be moved
  //! DeltaDerivative is the amount the derivative has to be moved.
  //! Delta and DeltaDerivative must be array of dimension
  //! ArrayDimension Degree is the degree of the BSpline and the
  //! FlatKnots are the knots of the BSpline Starting Condition if =
  //! -1 means the starting point of the curve can move
  //! = 0 means the
  //! starting point of the curve cannot move but tangent starting
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
  Standard_EXPORT static void MovePointAndTangent(const double    U,
                                                  const gp_Vec2d& Delta,
                                                  const gp_Vec2d& DeltaDerivative,
                                                  const double    Tolerance,
                                                  const int       Degree,
                                                  const int       StartingCondition,
                                                  const int       EndingCondition,
                                                  const NCollection_Array1<gp_Pnt2d>& Poles,
                                                  const NCollection_Array1<double>*   Weights,
                                                  const NCollection_Array1<double>&   FlatKnots,
                                                  NCollection_Array1<gp_Pnt2d>&       NewPoles,
                                                  int&                                ErrorStatus);

  //! given a tolerance in 3D space returns a
  //! tolerance in U parameter space such that
  //! all u1 and u0 in the domain of the curve f(u)
  //! | u1 - u0 | < UTolerance and
  //! we have |f (u1) - f (u0)| < Tolerance3D
  Standard_EXPORT static void Resolution(double&                           PolesArray,
                                         const int                         ArrayDimension,
                                         const int                         NumPoles,
                                         const NCollection_Array1<double>* Weights,
                                         const NCollection_Array1<double>& FlatKnots,
                                         const int                         Degree,
                                         const double                      Tolerance3D,
                                         double&                           UTolerance);

  //! given a tolerance in 3D space returns a
  //! tolerance in U parameter space such that
  //! all u1 and u0 in the domain of the curve f(u)
  //! | u1 - u0 | < UTolerance and
  //! we have |f (u1) - f (u0)| < Tolerance3D
  Standard_EXPORT static void Resolution(const NCollection_Array1<gp_Pnt>& Poles,
                                         const NCollection_Array1<double>* Weights,
                                         const int                         NumPoles,
                                         const NCollection_Array1<double>& FlatKnots,
                                         const int                         Degree,
                                         const double                      Tolerance3D,
                                         double&                           UTolerance);

  //! given a tolerance in 3D space returns a
  //! tolerance in U parameter space such that
  //! all u1 and u0 in the domain of the curve f(u)
  //! | u1 - u0 | < UTolerance and
  //! we have |f (u1) - f (u0)| < Tolerance3D
  Standard_EXPORT static void Resolution(const NCollection_Array1<gp_Pnt2d>& Poles,
                                         const NCollection_Array1<double>*   Weights,
                                         const int                           NumPoles,
                                         const NCollection_Array1<double>&   FlatKnots,
                                         const int                           Degree,
                                         const double                        Tolerance3D,
                                         double&                             UTolerance);

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
  Standard_EXPORT static int Intervals(const NCollection_Array1<double>& theKnots,
                                       const NCollection_Array1<int>&    theMults,
                                       int                               theDegree,
                                       bool                              isPeriodic,
                                       int                               theContinuity,
                                       double                            theFirst,
                                       double                            theLast,
                                       double                            theTolerance,
                                       NCollection_Array1<double>*       theIntervals);

private:
  Standard_EXPORT static void LocateParameter(const NCollection_Array1<double>& Knots,
                                              const double                      U,
                                              const bool                        Periodic,
                                              const int                         K1,
                                              const int                         K2,
                                              int&                              Index,
                                              double&                           NewU,
                                              const double                      Uf,
                                              const double                      Ue);
};

#include <BSplCLib.lxx>

#endif // _BSplCLib_HeaderFile
