// Created on: 1991-08-26
// Created by: JCV
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

#ifndef _BSplSLib_HeaderFile
#define _BSplSLib_HeaderFile

#include <BSplSLib_EvaluatorFunction.hxx>
#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <gp_Pnt.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_Array2.hxx>
#include <Standard_Integer.hxx>

class gp_Pnt;
class gp_Vec;

//! BSplSLib B-spline surface Library
//! This package provides an implementation of geometric
//! functions for rational and non rational, periodic and non
//! periodic B-spline surface computation.
//!
//! this package uses the multi-dimensions splines methods
//! provided in the package BSplCLib.
//!
//! In this package the B-spline surface is defined with:
//! . its control points :  Array2OfPnt     Poles
//! . its weights        :  Array2OfReal    Weights
//! . its knots and their multiplicity in the two parametric
//! direction U and V: Array1OfReal UKnots, VKnots and
//! Array1OfInteger UMults, VMults.
//! . the degree of the normalized Spline functions:
//! UDegree, VDegree
//!
//! . the Booleans URational, VRational to know if the weights
//! are constant in the U or V direction.
//!
//! . the Booleans UPeriodic, VRational to know if the surface
//! is periodic in the U or V direction.
//!
//! Warnings: The bounds of UKnots and UMults should be the
//! same, the bounds of VKnots and VMults should be the same,
//! the bounds of Poles and Weights should be the same.
//!
//! The Control points representation is:
//! Poles(Uorigin,Vorigin) ...................Poles(Uorigin,Vend)
//! .                                     .
//! .                                     .
//! Poles(Uend, Vorigin) .....................Poles(Uend, Vend)
//!
//! For the double array the row indice corresponds to the
//! parametric U direction and the columns indice corresponds
//! to the parametric V direction.
//!
//! Note: weight and multiplicity arrays can be passed by pointer for
//! some functions so that NULL pointer is valid.
//! That means no weights/no multiplicities passed.
//!
//! KeyWords :
//! B-spline surface, Functions, Library
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
class BSplSLib
{
public:
  DEFINE_STANDARD_ALLOC

  //! this is a one dimensional function
  //! typedef  void (*EvaluatorFunction)  (
  //! int     // Derivative Request
  //! double    *   // StartEnd[2][2]
  //! //  [0] = U
  //! //  [1] = V
  //! //        [0] = start
  //! //        [1] = end
  //! double        // UParameter
  //! double        // VParamerer
  //! double    &   // Result
  //! int &) ;// Error Code
  //! serves to multiply a given vectorial BSpline by a function
  //! Computes the derivatives of a ratio of two-variables
  //! functions x(u,v) / w(u,v) at orders
  //! <N,M>, x(u,v) is a vector in dimension <3>.
  //!
  //! <Ders> is an array containing the values of the
  //! input derivatives from 0 to std::min(<N>,<UDeg>), 0 to
  //! std::min(<M>,<VDeg>). For orders higher than
  //! <UDeg,VDeg> the input derivatives are assumed to
  //! be 0.
  //!
  //! The <Ders> is a 2d array and the dimension of the
  //! lines is always (<VDeg>+1) * (<3>+1), even
  //! if <N> is smaller than <Udeg> (the derivatives
  //! higher than <N> are not used).
  //!
  //! Content of <Ders>:
  //!
  //! x(i,j)[k] means: the composant k of x derivated
  //! (i) times in u and (j) times in v.
  //!
  //! ... First line ...
  //!
  //! x[1],x[2],...,x[3],w
  //! x(0,1)[1],...,x(0,1)[3],w(1,0)
  //! ...
  //! x(0,VDeg)[1],...,x(0,VDeg)[3],w(0,VDeg)
  //!
  //! ... Then second line ...
  //!
  //! x(1,0)[1],...,x(1,0)[3],w(1,0)
  //! x(1,1)[1],...,x(1,1)[3],w(1,1)
  //! ...
  //! x(1,VDeg)[1],...,x(1,VDeg)[3],w(1,VDeg)
  //!
  //! ...
  //!
  //! ... Last line ...
  //!
  //! x(UDeg,0)[1],...,x(UDeg,0)[3],w(UDeg,0)
  //! x(UDeg,1)[1],...,x(UDeg,1)[3],w(UDeg,1)
  //! ...
  //! x(Udeg,VDeg)[1],...,x(UDeg,VDeg)[3],w(Udeg,VDeg)
  //!
  //! If <All> is false, only the derivative at order
  //! <N,M> is computed. <RDers> is an array of length
  //! 3 which will contain the result :
  //!
  //! x(1)/w , x(2)/w ,  ... derivated <N> <M> times
  //!
  //! If <All> is true multiples derivatives are
  //! computed. All the derivatives (i,j) with 0 <= i+j
  //! <= std::max(N,M) are computed. <RDers> is an array of
  //! length 3 * (<N>+1) * (<M>+1) which will contains:
  //!
  //! x(1)/w , x(2)/w ,  ...
  //! x(1)/w , x(2)/w ,  ... derivated <0,1> times
  //! x(1)/w , x(2)/w ,  ... derivated <0,2> times
  //! ...
  //! x(1)/w , x(2)/w ,  ... derivated <0,N> times
  //!
  //! x(1)/w , x(2)/w ,  ... derivated <1,0> times
  //! x(1)/w , x(2)/w ,  ... derivated <1,1> times
  //! ...
  //! x(1)/w , x(2)/w ,  ... derivated <1,N> times
  //!
  //! x(1)/w , x(2)/w ,  ... derivated <N,0> times
  //! ....
  //! Warning: <RDers> must be dimensioned properly.
  Standard_EXPORT static void RationalDerivative(const int  UDeg,
                                                 const int  VDeg,
                                                 const int  N,
                                                 const int  M,
                                                 double&    Ders,
                                                 double&    RDers,
                                                 const bool All = true);

  Standard_EXPORT static void D0(const double                      U,
                                 const double                      V,
                                 const int                         UIndex,
                                 const int                         VIndex,
                                 const NCollection_Array2<gp_Pnt>& Poles,
                                 const NCollection_Array2<double>* Weights,
                                 const NCollection_Array1<double>& UKnots,
                                 const NCollection_Array1<double>& VKnots,
                                 const NCollection_Array1<int>*    UMults,
                                 const NCollection_Array1<int>*    VMults,
                                 const int                         UDegree,
                                 const int                         VDegree,
                                 const bool                        URat,
                                 const bool                        VRat,
                                 const bool                        UPer,
                                 const bool                        VPer,
                                 gp_Pnt&                           P);

  Standard_EXPORT static void D1(const double                      U,
                                 const double                      V,
                                 const int                         UIndex,
                                 const int                         VIndex,
                                 const NCollection_Array2<gp_Pnt>& Poles,
                                 const NCollection_Array2<double>* Weights,
                                 const NCollection_Array1<double>& UKnots,
                                 const NCollection_Array1<double>& VKnots,
                                 const NCollection_Array1<int>*    UMults,
                                 const NCollection_Array1<int>*    VMults,
                                 const int                         Degree,
                                 const int                         VDegree,
                                 const bool                        URat,
                                 const bool                        VRat,
                                 const bool                        UPer,
                                 const bool                        VPer,
                                 gp_Pnt&                           P,
                                 gp_Vec&                           Vu,
                                 gp_Vec&                           Vv);

  Standard_EXPORT static void D2(const double                      U,
                                 const double                      V,
                                 const int                         UIndex,
                                 const int                         VIndex,
                                 const NCollection_Array2<gp_Pnt>& Poles,
                                 const NCollection_Array2<double>* Weights,
                                 const NCollection_Array1<double>& UKnots,
                                 const NCollection_Array1<double>& VKnots,
                                 const NCollection_Array1<int>*    UMults,
                                 const NCollection_Array1<int>*    VMults,
                                 const int                         UDegree,
                                 const int                         VDegree,
                                 const bool                        URat,
                                 const bool                        VRat,
                                 const bool                        UPer,
                                 const bool                        VPer,
                                 gp_Pnt&                           P,
                                 gp_Vec&                           Vu,
                                 gp_Vec&                           Vv,
                                 gp_Vec&                           Vuu,
                                 gp_Vec&                           Vvv,
                                 gp_Vec&                           Vuv);

  Standard_EXPORT static void D3(const double                      U,
                                 const double                      V,
                                 const int                         UIndex,
                                 const int                         VIndex,
                                 const NCollection_Array2<gp_Pnt>& Poles,
                                 const NCollection_Array2<double>* Weights,
                                 const NCollection_Array1<double>& UKnots,
                                 const NCollection_Array1<double>& VKnots,
                                 const NCollection_Array1<int>*    UMults,
                                 const NCollection_Array1<int>*    VMults,
                                 const int                         UDegree,
                                 const int                         VDegree,
                                 const bool                        URat,
                                 const bool                        VRat,
                                 const bool                        UPer,
                                 const bool                        VPer,
                                 gp_Pnt&                           P,
                                 gp_Vec&                           Vu,
                                 gp_Vec&                           Vv,
                                 gp_Vec&                           Vuu,
                                 gp_Vec&                           Vvv,
                                 gp_Vec&                           Vuv,
                                 gp_Vec&                           Vuuu,
                                 gp_Vec&                           Vvvv,
                                 gp_Vec&                           Vuuv,
                                 gp_Vec&                           Vuvv);

  Standard_EXPORT static void DN(const double                      U,
                                 const double                      V,
                                 const int                         Nu,
                                 const int                         Nv,
                                 const int                         UIndex,
                                 const int                         VIndex,
                                 const NCollection_Array2<gp_Pnt>& Poles,
                                 const NCollection_Array2<double>* Weights,
                                 const NCollection_Array1<double>& UKnots,
                                 const NCollection_Array1<double>& VKnots,
                                 const NCollection_Array1<int>*    UMults,
                                 const NCollection_Array1<int>*    VMults,
                                 const int                         UDegree,
                                 const int                         VDegree,
                                 const bool                        URat,
                                 const bool                        VRat,
                                 const bool                        UPer,
                                 const bool                        VPer,
                                 gp_Vec&                           Vn);

  //! Computes the poles and weights of an isoparametric
  //! curve at parameter <Param> (UIso if <IsU> is True,
  //! VIso else).
  Standard_EXPORT static void Iso(const double                      Param,
                                  const bool                        IsU,
                                  const NCollection_Array2<gp_Pnt>& Poles,
                                  const NCollection_Array2<double>* Weights,
                                  const NCollection_Array1<double>& Knots,
                                  const NCollection_Array1<int>*    Mults,
                                  const int                         Degree,
                                  const bool                        Periodic,
                                  NCollection_Array1<gp_Pnt>&       CPoles,
                                  NCollection_Array1<double>*       CWeights);

  //! Reverses the array of poles. Last is the Index of
  //! the new first Row( Col) of Poles.
  //! On a non periodic surface Last is
  //! Poles.Upper().
  //! On a periodic curve last is
  //! (number of flat knots - degree - 1)
  //! or
  //! (sum of multiplicities(but for the last) + degree
  //! - 1)
  Standard_EXPORT static void Reverse(NCollection_Array2<gp_Pnt>& Poles,
                                      const int                   Last,
                                      const bool                  UDirection);

  //! Makes an homogeneous evaluation of Poles and Weights
  //! any and returns in P the Numerator value and
  //! in W the Denominator value if Weights are present
  //! otherwise returns 1.0e0
  Standard_EXPORT static void HomogeneousD0(const double                      U,
                                            const double                      V,
                                            const int                         UIndex,
                                            const int                         VIndex,
                                            const NCollection_Array2<gp_Pnt>& Poles,
                                            const NCollection_Array2<double>* Weights,
                                            const NCollection_Array1<double>& UKnots,
                                            const NCollection_Array1<double>& VKnots,
                                            const NCollection_Array1<int>*    UMults,
                                            const NCollection_Array1<int>*    VMults,
                                            const int                         UDegree,
                                            const int                         VDegree,
                                            const bool                        URat,
                                            const bool                        VRat,
                                            const bool                        UPer,
                                            const bool                        VPer,
                                            double&                           W,
                                            gp_Pnt&                           P);

  //! Makes an homogeneous evaluation of Poles and Weights
  //! any and returns in P the Numerator value and
  //! in W the Denominator value if Weights are present
  //! otherwise returns 1.0e0
  Standard_EXPORT static void HomogeneousD1(const double                      U,
                                            const double                      V,
                                            const int                         UIndex,
                                            const int                         VIndex,
                                            const NCollection_Array2<gp_Pnt>& Poles,
                                            const NCollection_Array2<double>* Weights,
                                            const NCollection_Array1<double>& UKnots,
                                            const NCollection_Array1<double>& VKnots,
                                            const NCollection_Array1<int>*    UMults,
                                            const NCollection_Array1<int>*    VMults,
                                            const int                         UDegree,
                                            const int                         VDegree,
                                            const bool                        URat,
                                            const bool                        VRat,
                                            const bool                        UPer,
                                            const bool                        VPer,
                                            gp_Pnt&                           N,
                                            gp_Vec&                           Nu,
                                            gp_Vec&                           Nv,
                                            double&                           D,
                                            double&                           Du,
                                            double&                           Dv);

  //! Reverses the array of weights.
  Standard_EXPORT static void Reverse(NCollection_Array2<double>& Weights,
                                      const int                   Last,
                                      const bool                  UDirection);

  //! Returns False if all the weights of the array <Weights>
  //! in the area [I1,I2] * [J1,J2] are identic.
  //! Epsilon is used for comparing weights.
  //! If Epsilon is 0. the Epsilon of the first weight is used.
  Standard_EXPORT static bool IsRational(const NCollection_Array2<double>& Weights,
                                         const int                         I1,
                                         const int                         I2,
                                         const int                         J1,
                                         const int                         J2,
                                         const double                      Epsilon = 0.0);

  //! Copy in FP the coordinates of the poles.
  Standard_EXPORT static void SetPoles(const NCollection_Array2<gp_Pnt>& Poles,
                                       NCollection_Array1<double>&       FP,
                                       const bool                        UDirection);

  //! Copy in FP the coordinates of the poles.
  Standard_EXPORT static void SetPoles(const NCollection_Array2<gp_Pnt>& Poles,
                                       const NCollection_Array2<double>& Weights,
                                       NCollection_Array1<double>&       FP,
                                       const bool                        UDirection);

  //! Get from FP the coordinates of the poles.
  Standard_EXPORT static void GetPoles(const NCollection_Array1<double>& FP,
                                       NCollection_Array2<gp_Pnt>&       Poles,
                                       const bool                        UDirection);

  //! Get from FP the coordinates of the poles.
  Standard_EXPORT static void GetPoles(const NCollection_Array1<double>& FP,
                                       NCollection_Array2<gp_Pnt>&       Poles,
                                       NCollection_Array2<double>&       Weights,
                                       const bool                        UDirection);

  //! Find the new poles which allows an old point (with a
  //! given u,v as parameters) to reach a new position
  //! UIndex1,UIndex2 indicate the range of poles we can
  //! move for U
  //! (1, UNbPoles-1) or (2, UNbPoles) -> no constraint
  //! for one side in U
  //! (2, UNbPoles-1) -> the ends are enforced for U
  //! don't enter (1,NbPoles) and (1,VNbPoles)
  //! -> error: rigid move
  //! if problem in BSplineBasis calculation, no change
  //! for the curve and
  //! UFirstIndex, VLastIndex = 0
  //! VFirstIndex, VLastIndex = 0
  Standard_EXPORT static void MovePoint(const double                      U,
                                        const double                      V,
                                        const gp_Vec&                     Displ,
                                        const int                         UIndex1,
                                        const int                         UIndex2,
                                        const int                         VIndex1,
                                        const int                         VIndex2,
                                        const int                         UDegree,
                                        const int                         VDegree,
                                        const bool                        Rational,
                                        const NCollection_Array2<gp_Pnt>& Poles,
                                        const NCollection_Array2<double>& Weights,
                                        const NCollection_Array1<double>& UFlatKnots,
                                        const NCollection_Array1<double>& VFlatKnots,
                                        int&                              UFirstIndex,
                                        int&                              ULastIndex,
                                        int&                              VFirstIndex,
                                        int&                              VLastIndex,
                                        NCollection_Array2<gp_Pnt>&       NewPoles);

  Standard_EXPORT static void InsertKnots(const bool                        UDirection,
                                          const int                         Degree,
                                          const bool                        Periodic,
                                          const NCollection_Array2<gp_Pnt>& Poles,
                                          const NCollection_Array2<double>* Weights,
                                          const NCollection_Array1<double>& Knots,
                                          const NCollection_Array1<int>&    Mults,
                                          const NCollection_Array1<double>& AddKnots,
                                          const NCollection_Array1<int>*    AddMults,
                                          NCollection_Array2<gp_Pnt>&       NewPoles,
                                          NCollection_Array2<double>*       NewWeights,
                                          NCollection_Array1<double>&       NewKnots,
                                          NCollection_Array1<int>&          NewMults,
                                          const double                      Epsilon,
                                          const bool                        Add = true);

  Standard_EXPORT static bool RemoveKnot(const bool                        UDirection,
                                         const int                         Index,
                                         const int                         Mult,
                                         const int                         Degree,
                                         const bool                        Periodic,
                                         const NCollection_Array2<gp_Pnt>& Poles,
                                         const NCollection_Array2<double>* Weights,
                                         const NCollection_Array1<double>& Knots,
                                         const NCollection_Array1<int>&    Mults,
                                         NCollection_Array2<gp_Pnt>&       NewPoles,
                                         NCollection_Array2<double>*       NewWeights,
                                         NCollection_Array1<double>&       NewKnots,
                                         NCollection_Array1<int>&          NewMults,
                                         const double                      Tolerance);

  Standard_EXPORT static void IncreaseDegree(const bool                        UDirection,
                                             const int                         Degree,
                                             const int                         NewDegree,
                                             const bool                        Periodic,
                                             const NCollection_Array2<gp_Pnt>& Poles,
                                             const NCollection_Array2<double>* Weights,
                                             const NCollection_Array1<double>& Knots,
                                             const NCollection_Array1<int>&    Mults,
                                             NCollection_Array2<gp_Pnt>&       NewPoles,
                                             NCollection_Array2<double>*       NewWeights,
                                             NCollection_Array1<double>&       NewKnots,
                                             NCollection_Array1<int>&          NewMults);

  Standard_EXPORT static void Unperiodize(const bool                        UDirection,
                                          const int                         Degree,
                                          const NCollection_Array1<int>&    Mults,
                                          const NCollection_Array1<double>& Knots,
                                          const NCollection_Array2<gp_Pnt>& Poles,
                                          const NCollection_Array2<double>* Weights,
                                          NCollection_Array1<int>&          NewMults,
                                          NCollection_Array1<double>&       NewKnots,
                                          NCollection_Array2<gp_Pnt>&       NewPoles,
                                          NCollection_Array2<double>*       NewWeights);

  //! Used as argument for a non rational curve.
  static NCollection_Array2<double>* NoWeights();

  //! Perform the evaluation of the Taylor expansion
  //! of the Bspline normalized between 0 and 1.
  //! If rational computes the homogeneous Taylor expansion
  //! for the numerator and stores it in CachePoles
  Standard_EXPORT static void BuildCache(const double                      U,
                                         const double                      V,
                                         const double                      USpanDomain,
                                         const double                      VSpanDomain,
                                         const bool                        UPeriodicFlag,
                                         const bool                        VPeriodicFlag,
                                         const int                         UDegree,
                                         const int                         VDegree,
                                         const int                         UIndex,
                                         const int                         VIndex,
                                         const NCollection_Array1<double>& UFlatKnots,
                                         const NCollection_Array1<double>& VFlatKnots,
                                         const NCollection_Array2<gp_Pnt>& Poles,
                                         const NCollection_Array2<double>* Weights,
                                         NCollection_Array2<gp_Pnt>&       CachePoles,
                                         NCollection_Array2<double>*       CacheWeights);

  //! Perform the evaluation of the Taylor expansion
  //! of the Bspline normalized between 0 and 1.
  //! Structure of result optimized for BSplSLib_Cache.
  Standard_EXPORT static void BuildCache(const double                      theU,
                                         const double                      theV,
                                         const double                      theUSpanDomain,
                                         const double                      theVSpanDomain,
                                         const bool                        theUPeriodic,
                                         const bool                        theVPeriodic,
                                         const int                         theUDegree,
                                         const int                         theVDegree,
                                         const int                         theUIndex,
                                         const int                         theVIndex,
                                         const NCollection_Array1<double>& theUFlatKnots,
                                         const NCollection_Array1<double>& theVFlatKnots,
                                         const NCollection_Array2<gp_Pnt>& thePoles,
                                         const NCollection_Array2<double>* theWeights,
                                         NCollection_Array2<double>&       theCacheArray);

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
                                      const double                      V,
                                      const int                         UDegree,
                                      const int                         VDegree,
                                      const double                      UCacheParameter,
                                      const double                      VCacheParameter,
                                      const double                      USpanLenght,
                                      const double                      VSpanLength,
                                      const NCollection_Array2<gp_Pnt>& Poles,
                                      const NCollection_Array2<double>* Weights,
                                      gp_Pnt&                           Point);

  //! Calls CacheD0 for Bezier Surfaces Arrays computed with
  //! the method PolesCoefficients.
  //! Warning: To be used for BezierSurfaces ONLY!!!
  static void CoefsD0(const double                      U,
                      const double                      V,
                      const NCollection_Array2<gp_Pnt>& Poles,
                      const NCollection_Array2<double>* Weights,
                      gp_Pnt&                           Point);

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
                                      const double                      V,
                                      const int                         UDegree,
                                      const int                         VDegree,
                                      const double                      UCacheParameter,
                                      const double                      VCacheParameter,
                                      const double                      USpanLenght,
                                      const double                      VSpanLength,
                                      const NCollection_Array2<gp_Pnt>& Poles,
                                      const NCollection_Array2<double>* Weights,
                                      gp_Pnt&                           Point,
                                      gp_Vec&                           VecU,
                                      gp_Vec&                           VecV);

  //! Calls CacheD0 for Bezier Surfaces Arrays computed with
  //! the method PolesCoefficients.
  //! Warning: To be used for BezierSurfaces ONLY!!!
  static void CoefsD1(const double                      U,
                      const double                      V,
                      const NCollection_Array2<gp_Pnt>& Poles,
                      const NCollection_Array2<double>* Weights,
                      gp_Pnt&                           Point,
                      gp_Vec&                           VecU,
                      gp_Vec&                           VecV);

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
                                      const double                      V,
                                      const int                         UDegree,
                                      const int                         VDegree,
                                      const double                      UCacheParameter,
                                      const double                      VCacheParameter,
                                      const double                      USpanLenght,
                                      const double                      VSpanLength,
                                      const NCollection_Array2<gp_Pnt>& Poles,
                                      const NCollection_Array2<double>* Weights,
                                      gp_Pnt&                           Point,
                                      gp_Vec&                           VecU,
                                      gp_Vec&                           VecV,
                                      gp_Vec&                           VecUU,
                                      gp_Vec&                           VecUV,
                                      gp_Vec&                           VecVV);

  //! Calls CacheD0 for Bezier Surfaces Arrays computed with
  //! the method PolesCoefficients.
  //! Warning: To be used for BezierSurfaces ONLY!!!
  static void CoefsD2(const double                      U,
                      const double                      V,
                      const NCollection_Array2<gp_Pnt>& Poles,
                      const NCollection_Array2<double>* Weights,
                      gp_Pnt&                           Point,
                      gp_Vec&                           VecU,
                      gp_Vec&                           VecV,
                      gp_Vec&                           VecUU,
                      gp_Vec&                           VecUV,
                      gp_Vec&                           VecVV);

  //! Warning! To be used for BezierSurfaces ONLY!!!
  static void PolesCoefficients(const NCollection_Array2<gp_Pnt>& Poles,
                                NCollection_Array2<gp_Pnt>&       CachePoles);

  //! Encapsulation of BuildCache to perform the
  //! evaluation of the Taylor expansion for beziersurfaces
  //! at parameters 0.,0.;
  //! Warning: To be used for BezierSurfaces ONLY!!!
  Standard_EXPORT static void PolesCoefficients(const NCollection_Array2<gp_Pnt>& Poles,
                                                const NCollection_Array2<double>* Weights,
                                                NCollection_Array2<gp_Pnt>&       CachePoles,
                                                NCollection_Array2<double>*       CacheWeights);

  //! Given a tolerance in 3D space returns two
  //! tolerances, one in U one in V such that for
  //! all (u1,v1) and (u0,v0) in the domain of
  //! the surface f(u,v) we have :
  //! | u1 - u0 | < UTolerance and
  //! | v1 - v0 | < VTolerance
  //! we have |f (u1,v1) - f (u0,v0)| < Tolerance3D
  Standard_EXPORT static void Resolution(const NCollection_Array2<gp_Pnt>& Poles,
                                         const NCollection_Array2<double>* Weights,
                                         const NCollection_Array1<double>& UKnots,
                                         const NCollection_Array1<double>& VKnots,
                                         const NCollection_Array1<int>&    UMults,
                                         const NCollection_Array1<int>&    VMults,
                                         const int                         UDegree,
                                         const int                         VDegree,
                                         const bool                        URat,
                                         const bool                        VRat,
                                         const bool                        UPer,
                                         const bool                        VPer,
                                         const double                      Tolerance3D,
                                         double&                           UTolerance,
                                         double&                           VTolerance);

  //! Performs the interpolation of the data points given in
  //! the Poles array in the form
  //! [1,...,RL][1,...,RC][1...PolesDimension]. The
  //! ColLength CL and the Length of UParameters must be the
  //! same. The length of VFlatKnots is VDegree + CL + 1.
  //!
  //! The RowLength RL and the Length of VParameters must be
  //! the same. The length of VFlatKnots is Degree + RL + 1.
  //!
  //! Warning: the method used to do that interpolation
  //! is gauss elimination WITHOUT pivoting. Thus if the
  //! diagonal is not dominant there is no guarantee that
  //! the algorithm will work. Nevertheless for Cubic
  //! interpolation at knots or interpolation at Scheonberg
  //! points the method will work. The InversionProblem
  //! will report 0 if there was no problem else it will
  //! give the index of the faulty pivot
  Standard_EXPORT static void Interpolate(const int                         UDegree,
                                          const int                         VDegree,
                                          const NCollection_Array1<double>& UFlatKnots,
                                          const NCollection_Array1<double>& VFlatKnots,
                                          const NCollection_Array1<double>& UParameters,
                                          const NCollection_Array1<double>& VParameters,
                                          NCollection_Array2<gp_Pnt>&       Poles,
                                          NCollection_Array2<double>&       Weights,
                                          int&                              InversionProblem);

  //! Performs the interpolation of the data points given in
  //! the Poles array.
  //! The ColLength CL and the Length of UParameters must be
  //! the same. The length of VFlatKnots is VDegree + CL + 1.
  //!
  //! The RowLength RL and the Length of VParameters must be
  //! the same. The length of VFlatKnots is Degree + RL + 1.
  //!
  //! Warning: the method used to do that interpolation
  //! is gauss elimination WITHOUT pivoting. Thus if the
  //! diagonal is not dominant there is no guarantee that
  //! the algorithm will work. Nevertheless for Cubic
  //! interpolation at knots or interpolation at Scheonberg
  //! points the method will work. The InversionProblem
  //! will report 0 if there was no problem else it will
  //! give the index of the faulty pivot
  Standard_EXPORT static void Interpolate(const int                         UDegree,
                                          const int                         VDegree,
                                          const NCollection_Array1<double>& UFlatKnots,
                                          const NCollection_Array1<double>& VFlatKnots,
                                          const NCollection_Array1<double>& UParameters,
                                          const NCollection_Array1<double>& VParameters,
                                          NCollection_Array2<gp_Pnt>&       Poles,
                                          int&                              InversionProblem);

  //! this will multiply a given BSpline numerator N(u,v)
  //! and denominator D(u,v) defined by its
  //! U/VBSplineDegree and U/VBSplineKnots, and
  //! U/VMults. Its Poles and Weights are arrays which are
  //! coded as array2 of the form
  //! [1..UNumPoles][1..VNumPoles] by a function a(u,v)
  //! which is assumed to satisfy the following:
  //! 1. a(u,v) * N(u,v) and a(u,v) * D(u,v) is a polynomial
  //! BSpline that can be expressed exactly as a BSpline of
  //! degree U/VNewDegree on the knots U/VFlatKnots
  //! 2. the range of a(u,v) is the same as the range of
  //! N(u,v) or D(u,v)
  //! Warning: it is the caller's responsibility to
  //! insure that conditions 1. and 2. above are satisfied
  //! no check whatsoever is made in this method
  //! theStatus will return 0 if OK else it will return the
  //! pivot index of the matrix that was inverted to
  //! compute the multiplied BSpline : the method used
  //! is interpolation at Schoenenberg points of
  //! a(u,v)* N(u,v) and a(u,v) * D(u,v)
  //! theStatus will return 0 if OK else it will return the pivot index
  //! of the matrix that was inverted to compute the multiplied
  //! BSpline: the method used is interpolation at Schoenenberg
  //! points of a(u,v)*F(u,v)
  //! --
  Standard_EXPORT static void FunctionMultiply(const BSplSLib_EvaluatorFunction& Function,
                                               const int                         UBSplineDegree,
                                               const int                         VBSplineDegree,
                                               const NCollection_Array1<double>& UBSplineKnots,
                                               const NCollection_Array1<double>& VBSplineKnots,
                                               const NCollection_Array1<int>*    UMults,
                                               const NCollection_Array1<int>*    VMults,
                                               const NCollection_Array2<gp_Pnt>& Poles,
                                               const NCollection_Array2<double>* Weights,
                                               const NCollection_Array1<double>& UFlatKnots,
                                               const NCollection_Array1<double>& VFlatKnots,
                                               const int                         UNewDegree,
                                               const int                         VNewDegree,
                                               NCollection_Array2<gp_Pnt>&       NewNumerator,
                                               NCollection_Array2<double>&       NewDenominator,
                                               int&                              theStatus);

  //! Returns an NCollection_Array2<double> filled with 1.0 values.
  //! If theNbUPoles * theNbVPoles <= BSplCLib::MaxUnitWeightsSize(),
  //! references a pre-allocated global array (zero allocation).
  //! Otherwise, allocates a new array and fills with 1.0.
  //! @warning The returned array may reference global static memory — do NOT modify elements.
  //! @param[in] theNbUPoles number of poles in U direction
  //! @param[in] theNbVPoles number of poles in V direction
  //! @return array of unit weights with bounds [1, theNbUPoles] x [1, theNbVPoles]
  Standard_EXPORT static NCollection_Array2<double> UnitWeights(const int theNbUPoles,
                                                                const int theNbVPoles);
};

#include <BSplSLib.lxx>

#endif // _BSplSLib_HeaderFile
