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

#include <BSplCLib.hxx>
#include <gp_Pnt2d.hxx>
#include <gp_Vec2d.hxx>
#include <Standard_NotImplemented.hxx>

// BSpline Curve in 2d space
// **************************

// Include the template implementation header
#include <BSplCLib_CurveComputation.pxx>

// Explicit template instantiations for 2D types
// This replaces the old macro-based approach with modern C++ templates

//==================================================================================================

void BSplCLib::Reverse(NCollection_Array1<gp_Pnt2d>& Poles, const int L)
{
  BSplCLib_Reverse(Poles, L);
}

//==================================================================================================

bool BSplCLib::RemoveKnot(const int                           Index,
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
                          const double                        Tolerance)
{
  return BSplCLib_RemoveKnot<gp_Pnt2d, gp_Vec2d, NCollection_Array1<gp_Pnt2d>, 2>(Index,
                                                                                  Mult,
                                                                                  Degree,
                                                                                  Periodic,
                                                                                  Poles,
                                                                                  Weights,
                                                                                  Knots,
                                                                                  Mults,
                                                                                  NewPoles,
                                                                                  NewWeights,
                                                                                  NewKnots,
                                                                                  NewMults,
                                                                                  Tolerance);
}

//==================================================================================================

void BSplCLib::InsertKnots(const int                           Degree,
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
                           const bool                          Add)
{
  BSplCLib_InsertKnots<gp_Pnt2d, gp_Vec2d, NCollection_Array1<gp_Pnt2d>, 2>(Degree,
                                                                            Periodic,
                                                                            Poles,
                                                                            Weights,
                                                                            Knots,
                                                                            Mults,
                                                                            AddKnots,
                                                                            AddMults,
                                                                            NewPoles,
                                                                            NewWeights,
                                                                            NewKnots,
                                                                            NewMults,
                                                                            Epsilon,
                                                                            Add);
}

//==================================================================================================

void BSplCLib::InsertKnot(const int,
                          const double                        U,
                          const int                           UMult,
                          const int                           Degree,
                          const bool                          Periodic,
                          const NCollection_Array1<gp_Pnt2d>& Poles,
                          const NCollection_Array1<double>*   Weights,
                          const NCollection_Array1<double>&   Knots,
                          const NCollection_Array1<int>&      Mults,
                          NCollection_Array1<gp_Pnt2d>&       NewPoles,
                          NCollection_Array1<double>*         NewWeights)
{
  BSplCLib_InsertKnot<gp_Pnt2d, gp_Vec2d, NCollection_Array1<gp_Pnt2d>, 2>(0,
                                                                           U,
                                                                           UMult,
                                                                           Degree,
                                                                           Periodic,
                                                                           Poles,
                                                                           Weights,
                                                                           Knots,
                                                                           Mults,
                                                                           NewPoles,
                                                                           NewWeights);
}

//==================================================================================================

void BSplCLib::RaiseMultiplicity(const int                           KnotIndex,
                                 const int                           Mult,
                                 const int                           Degree,
                                 const bool                          Periodic,
                                 const NCollection_Array1<gp_Pnt2d>& Poles,
                                 const NCollection_Array1<double>*   Weights,
                                 const NCollection_Array1<double>&   Knots,
                                 const NCollection_Array1<int>&      Mults,
                                 NCollection_Array1<gp_Pnt2d>&       NewPoles,
                                 NCollection_Array1<double>*         NewWeights)
{
  BSplCLib_RaiseMultiplicity<gp_Pnt2d, gp_Vec2d, NCollection_Array1<gp_Pnt2d>, 2>(KnotIndex,
                                                                                  Mult,
                                                                                  Degree,
                                                                                  Periodic,
                                                                                  Poles,
                                                                                  Weights,
                                                                                  Knots,
                                                                                  Mults,
                                                                                  NewPoles,
                                                                                  NewWeights);
}

//==================================================================================================

void BSplCLib::IncreaseDegree(const int                           Degree,
                              const int                           NewDegree,
                              const bool                          Periodic,
                              const NCollection_Array1<gp_Pnt2d>& Poles,
                              const NCollection_Array1<double>*   Weights,
                              const NCollection_Array1<double>&   Knots,
                              const NCollection_Array1<int>&      Mults,
                              NCollection_Array1<gp_Pnt2d>&       NewPoles,
                              NCollection_Array1<double>*         NewWeights,
                              NCollection_Array1<double>&         NewKnots,
                              NCollection_Array1<int>&            NewMults)
{
  BSplCLib_IncreaseDegree<gp_Pnt2d, gp_Vec2d, NCollection_Array1<gp_Pnt2d>, 2>(Degree,
                                                                               NewDegree,
                                                                               Periodic,
                                                                               Poles,
                                                                               Weights,
                                                                               Knots,
                                                                               Mults,
                                                                               NewPoles,
                                                                               NewWeights,
                                                                               NewKnots,
                                                                               NewMults);
}

//==================================================================================================

void BSplCLib::Unperiodize(const int                           Degree,
                           const NCollection_Array1<int>&      Mults,
                           const NCollection_Array1<double>&   Knots,
                           const NCollection_Array1<gp_Pnt2d>& Poles,
                           const NCollection_Array1<double>*   Weights,
                           NCollection_Array1<int>&            NewMults,
                           NCollection_Array1<double>&         NewKnots,
                           NCollection_Array1<gp_Pnt2d>&       NewPoles,
                           NCollection_Array1<double>*         NewWeights)
{
  BSplCLib_Unperiodize<gp_Pnt2d, gp_Vec2d, NCollection_Array1<gp_Pnt2d>, 2>(Degree,
                                                                            Mults,
                                                                            Knots,
                                                                            Poles,
                                                                            Weights,
                                                                            NewMults,
                                                                            NewKnots,
                                                                            NewPoles,
                                                                            NewWeights);
}

//==================================================================================================

void BSplCLib::Trimming(const int                           Degree,
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
                        NCollection_Array1<double>*         NewWeights)
{
  BSplCLib_Trimming<gp_Pnt2d, gp_Vec2d, NCollection_Array1<gp_Pnt2d>, 2>(Degree,
                                                                         Periodic,
                                                                         Knots,
                                                                         Mults,
                                                                         Poles,
                                                                         Weights,
                                                                         U1,
                                                                         U2,
                                                                         NewKnots,
                                                                         NewMults,
                                                                         NewPoles,
                                                                         NewWeights);
}

//==================================================================================================

void BSplCLib::BuildEval(const int                           Degree,
                         const int                           Index,
                         const NCollection_Array1<gp_Pnt2d>& Poles,
                         const NCollection_Array1<double>*   Weights,
                         double&                             LP)
{
  BSplCLib_BuildEval<gp_Pnt2d, gp_Vec2d, NCollection_Array1<gp_Pnt2d>, 2>(Degree,
                                                                          Index,
                                                                          Poles,
                                                                          Weights,
                                                                          LP);
}

//==================================================================================================

void BSplCLib::D0(const double                        U,
                  const int                           Index,
                  const int                           Degree,
                  const bool                          Periodic,
                  const NCollection_Array1<gp_Pnt2d>& Poles,
                  const NCollection_Array1<double>*   Weights,
                  const NCollection_Array1<double>&   Knots,
                  const NCollection_Array1<int>*      Mults,
                  gp_Pnt2d&                           P)
{
  BSplCLib_D0<gp_Pnt2d, gp_Vec2d, NCollection_Array1<gp_Pnt2d>, 2>(U,
                                                                   Index,
                                                                   Degree,
                                                                   Periodic,
                                                                   Poles,
                                                                   Weights,
                                                                   Knots,
                                                                   Mults,
                                                                   P);
}

//==================================================================================================

void BSplCLib::D1(const double                        U,
                  const int                           Index,
                  const int                           Degree,
                  const bool                          Periodic,
                  const NCollection_Array1<gp_Pnt2d>& Poles,
                  const NCollection_Array1<double>*   Weights,
                  const NCollection_Array1<double>&   Knots,
                  const NCollection_Array1<int>*      Mults,
                  gp_Pnt2d&                           P,
                  gp_Vec2d&                           V)
{
  BSplCLib_D1<gp_Pnt2d, gp_Vec2d, NCollection_Array1<gp_Pnt2d>, 2>(U,
                                                                   Index,
                                                                   Degree,
                                                                   Periodic,
                                                                   Poles,
                                                                   Weights,
                                                                   Knots,
                                                                   Mults,
                                                                   P,
                                                                   V);
}

//==================================================================================================

void BSplCLib::D2(const double                        U,
                  const int                           Index,
                  const int                           Degree,
                  const bool                          Periodic,
                  const NCollection_Array1<gp_Pnt2d>& Poles,
                  const NCollection_Array1<double>*   Weights,
                  const NCollection_Array1<double>&   Knots,
                  const NCollection_Array1<int>*      Mults,
                  gp_Pnt2d&                           P,
                  gp_Vec2d&                           V1,
                  gp_Vec2d&                           V2)
{
  BSplCLib_D2<gp_Pnt2d, gp_Vec2d, NCollection_Array1<gp_Pnt2d>, 2>(U,
                                                                   Index,
                                                                   Degree,
                                                                   Periodic,
                                                                   Poles,
                                                                   Weights,
                                                                   Knots,
                                                                   Mults,
                                                                   P,
                                                                   V1,
                                                                   V2);
}

//==================================================================================================

void BSplCLib::D3(const double                        U,
                  const int                           Index,
                  const int                           Degree,
                  const bool                          Periodic,
                  const NCollection_Array1<gp_Pnt2d>& Poles,
                  const NCollection_Array1<double>*   Weights,
                  const NCollection_Array1<double>&   Knots,
                  const NCollection_Array1<int>*      Mults,
                  gp_Pnt2d&                           P,
                  gp_Vec2d&                           V1,
                  gp_Vec2d&                           V2,
                  gp_Vec2d&                           V3)
{
  BSplCLib_D3<gp_Pnt2d, gp_Vec2d, NCollection_Array1<gp_Pnt2d>, 2>(U,
                                                                   Index,
                                                                   Degree,
                                                                   Periodic,
                                                                   Poles,
                                                                   Weights,
                                                                   Knots,
                                                                   Mults,
                                                                   P,
                                                                   V1,
                                                                   V2,
                                                                   V3);
}

//==================================================================================================

void BSplCLib::DN(const double                        U,
                  const int                           N,
                  const int                           Index,
                  const int                           Degree,
                  const bool                          Periodic,
                  const NCollection_Array1<gp_Pnt2d>& Poles,
                  const NCollection_Array1<double>*   Weights,
                  const NCollection_Array1<double>&   Knots,
                  const NCollection_Array1<int>*      Mults,
                  gp_Vec2d&                           VN)
{
  BSplCLib_DN<gp_Pnt2d, gp_Vec2d, NCollection_Array1<gp_Pnt2d>, 2>(U,
                                                                   N,
                                                                   Index,
                                                                   Degree,
                                                                   Periodic,
                                                                   Poles,
                                                                   Weights,
                                                                   Knots,
                                                                   Mults,
                                                                   VN);
}

//==================================================================================================

int BSplCLib::SolveBandedSystem(const math_Matrix&            Matrix,
                                const int                     UpperBandWidth,
                                const int                     LowerBandWidth,
                                NCollection_Array1<gp_Pnt2d>& PolesArray)
{
  return BSplCLib_SolveBandedSystem<gp_Pnt2d, gp_Vec2d, NCollection_Array1<gp_Pnt2d>, 2>(
    Matrix,
    UpperBandWidth,
    LowerBandWidth,
    PolesArray);
}

//==================================================================================================

int BSplCLib::SolveBandedSystem(const math_Matrix&            Matrix,
                                const int                     UpperBandWidth,
                                const int                     LowerBandWidth,
                                const bool                    HomogeneousFlag,
                                NCollection_Array1<gp_Pnt2d>& PolesArray,
                                NCollection_Array1<double>&   WeightsArray)
{
  return BSplCLib_SolveBandedSystem<gp_Pnt2d, gp_Vec2d, NCollection_Array1<gp_Pnt2d>, 2>(
    Matrix,
    UpperBandWidth,
    LowerBandWidth,
    HomogeneousFlag,
    PolesArray,
    WeightsArray);
}

//==================================================================================================

void BSplCLib::Eval(const double                        Parameter,
                    const bool                          PeriodicFlag,
                    const bool                          HomogeneousFlag,
                    int&                                ExtrapMode,
                    const int                           Degree,
                    const NCollection_Array1<double>&   FlatKnots,
                    const NCollection_Array1<gp_Pnt2d>& PolesArray,
                    const NCollection_Array1<double>&   WeightsArray,
                    gp_Pnt2d&                           aPoint,
                    double&                             aWeight)
{
  BSplCLib_Eval<gp_Pnt2d, gp_Vec2d, NCollection_Array1<gp_Pnt2d>, 2>(Parameter,
                                                                     PeriodicFlag,
                                                                     HomogeneousFlag,
                                                                     ExtrapMode,
                                                                     Degree,
                                                                     FlatKnots,
                                                                     PolesArray,
                                                                     WeightsArray,
                                                                     aPoint,
                                                                     aWeight);
}

//==================================================================================================

void BSplCLib::CacheD0(const double                        Parameter,
                       const int                           Degree,
                       const double                        CacheParameter,
                       const double                        SpanLenght,
                       const NCollection_Array1<gp_Pnt2d>& PolesArray,
                       const NCollection_Array1<double>*   WeightsArray,
                       gp_Pnt2d&                           aPoint)
{
  BSplCLib_CacheD0<gp_Pnt2d, gp_Vec2d, NCollection_Array1<gp_Pnt2d>, 2>(Parameter,
                                                                        Degree,
                                                                        CacheParameter,
                                                                        SpanLenght,
                                                                        PolesArray,
                                                                        WeightsArray,
                                                                        aPoint);
}

//==================================================================================================

void BSplCLib::CacheD1(const double                        Parameter,
                       const int                           Degree,
                       const double                        CacheParameter,
                       const double                        SpanLenght,
                       const NCollection_Array1<gp_Pnt2d>& PolesArray,
                       const NCollection_Array1<double>*   WeightsArray,
                       gp_Pnt2d&                           aPoint,
                       gp_Vec2d&                           aVector)
{
  BSplCLib_CacheD1<gp_Pnt2d, gp_Vec2d, NCollection_Array1<gp_Pnt2d>, 2>(Parameter,
                                                                        Degree,
                                                                        CacheParameter,
                                                                        SpanLenght,
                                                                        PolesArray,
                                                                        WeightsArray,
                                                                        aPoint,
                                                                        aVector);
}

//==================================================================================================

void BSplCLib::CacheD2(const double                        Parameter,
                       const int                           Degree,
                       const double                        CacheParameter,
                       const double                        SpanLenght,
                       const NCollection_Array1<gp_Pnt2d>& PolesArray,
                       const NCollection_Array1<double>*   WeightsArray,
                       gp_Pnt2d&                           aPoint,
                       gp_Vec2d&                           aVector1,
                       gp_Vec2d&                           aVector2)
{
  BSplCLib_CacheD2<gp_Pnt2d, gp_Vec2d, NCollection_Array1<gp_Pnt2d>, 2>(Parameter,
                                                                        Degree,
                                                                        CacheParameter,
                                                                        SpanLenght,
                                                                        PolesArray,
                                                                        WeightsArray,
                                                                        aPoint,
                                                                        aVector1,
                                                                        aVector2);
}

//==================================================================================================

void BSplCLib::CacheD3(const double                        Parameter,
                       const int                           Degree,
                       const double                        CacheParameter,
                       const double                        SpanLenght,
                       const NCollection_Array1<gp_Pnt2d>& PolesArray,
                       const NCollection_Array1<double>*   WeightsArray,
                       gp_Pnt2d&                           aPoint,
                       gp_Vec2d&                           aVector1,
                       gp_Vec2d&                           aVector2,
                       gp_Vec2d&                           aVector3)
{
  BSplCLib_CacheD3<gp_Pnt2d, gp_Vec2d, NCollection_Array1<gp_Pnt2d>, 2>(Parameter,
                                                                        Degree,
                                                                        CacheParameter,
                                                                        SpanLenght,
                                                                        PolesArray,
                                                                        WeightsArray,
                                                                        aPoint,
                                                                        aVector1,
                                                                        aVector2,
                                                                        aVector3);
}

//==================================================================================================

void BSplCLib::BuildCache(const double                        U,
                          const double                        SpanDomain,
                          const bool                          Periodic,
                          const int                           Degree,
                          const NCollection_Array1<double>&   FlatKnots,
                          const NCollection_Array1<gp_Pnt2d>& Poles,
                          const NCollection_Array1<double>*   Weights,
                          NCollection_Array1<gp_Pnt2d>&       CachePoles,
                          NCollection_Array1<double>*         CacheWeights)
{
  BSplCLib_BuildCache<gp_Pnt2d, gp_Vec2d, NCollection_Array1<gp_Pnt2d>, 2>(U,
                                                                           SpanDomain,
                                                                           Periodic,
                                                                           Degree,
                                                                           FlatKnots,
                                                                           Poles,
                                                                           Weights,
                                                                           CachePoles,
                                                                           CacheWeights);
}

//==================================================================================================

void BSplCLib::BuildCache(const double                        theParameter,
                          const double                        theSpanDomain,
                          const bool                          thePeriodicFlag,
                          const int                           theDegree,
                          const int                           theSpanIndex,
                          const NCollection_Array1<double>&   theFlatKnots,
                          const NCollection_Array1<gp_Pnt2d>& thePoles,
                          const NCollection_Array1<double>*   theWeights,
                          NCollection_Array2<double>&         theCacheArray)
{
  BSplCLib_BuildCache<gp_Pnt2d, gp_Vec2d, NCollection_Array1<gp_Pnt2d>, 2>(theParameter,
                                                                           theSpanDomain,
                                                                           thePeriodicFlag,
                                                                           theDegree,
                                                                           theSpanIndex,
                                                                           theFlatKnots,
                                                                           thePoles,
                                                                           theWeights,
                                                                           theCacheArray);
}

//==================================================================================================

void BSplCLib::Interpolate(const int                         Degree,
                           const NCollection_Array1<double>& FlatKnots,
                           const NCollection_Array1<double>& Parameters,
                           const NCollection_Array1<int>&    ContactOrderArray,
                           NCollection_Array1<gp_Pnt2d>&     Poles,
                           int&                              InversionProblem)
{
  BSplCLib_Interpolate<gp_Pnt2d, gp_Vec2d, NCollection_Array1<gp_Pnt2d>, 2>(Degree,
                                                                            FlatKnots,
                                                                            Parameters,
                                                                            ContactOrderArray,
                                                                            Poles,
                                                                            InversionProblem);
}

//==================================================================================================

void BSplCLib::Interpolate(const int                         Degree,
                           const NCollection_Array1<double>& FlatKnots,
                           const NCollection_Array1<double>& Parameters,
                           const NCollection_Array1<int>&    ContactOrderArray,
                           NCollection_Array1<gp_Pnt2d>&     Poles,
                           NCollection_Array1<double>&       Weights,
                           int&                              InversionProblem)
{
  BSplCLib_Interpolate<gp_Pnt2d, gp_Vec2d, NCollection_Array1<gp_Pnt2d>, 2>(Degree,
                                                                            FlatKnots,
                                                                            Parameters,
                                                                            ContactOrderArray,
                                                                            Poles,
                                                                            Weights,
                                                                            InversionProblem);
}

//==================================================================================================

void BSplCLib::MovePoint(const double                        U,
                         const gp_Vec2d&                     Displ,
                         const int                           Index1,
                         const int                           Index2,
                         const int                           Degree,
                         const NCollection_Array1<gp_Pnt2d>& Poles,
                         const NCollection_Array1<double>*   Weights,
                         const NCollection_Array1<double>&   FlatKnots,
                         int&                                FirstIndex,
                         int&                                LastIndex,
                         NCollection_Array1<gp_Pnt2d>&       NewPoles)
{
  BSplCLib_MovePoint<gp_Pnt2d, gp_Vec2d, NCollection_Array1<gp_Pnt2d>, 2>(U,
                                                                          Displ,
                                                                          Index1,
                                                                          Index2,
                                                                          Degree,
                                                                          Poles,
                                                                          Weights,
                                                                          FlatKnots,
                                                                          FirstIndex,
                                                                          LastIndex,
                                                                          NewPoles);
}

//==================================================================================================

void BSplCLib::MovePointAndTangent(const double                        U,
                                   const gp_Vec2d&                     Delta,
                                   const gp_Vec2d&                     DeltaDerivatives,
                                   const double                        Tolerance,
                                   const int                           Degree,
                                   const int                           StartingCondition,
                                   const int                           EndingCondition,
                                   const NCollection_Array1<gp_Pnt2d>& Poles,
                                   const NCollection_Array1<double>*   Weights,
                                   const NCollection_Array1<double>&   FlatKnots,
                                   NCollection_Array1<gp_Pnt2d>&       NewPoles,
                                   int&                                ErrorStatus)
{
  BSplCLib_MovePointAndTangent<gp_Pnt2d, gp_Vec2d, NCollection_Array1<gp_Pnt2d>, 2>(
    U,
    Delta,
    DeltaDerivatives,
    Tolerance,
    Degree,
    StartingCondition,
    EndingCondition,
    Poles,
    Weights,
    FlatKnots,
    NewPoles,
    ErrorStatus);
}

//==================================================================================================

void BSplCLib::Resolution(const NCollection_Array1<gp_Pnt2d>& Poles,
                          const NCollection_Array1<double>*   Weights,
                          const int                           NumPoles,
                          const NCollection_Array1<double>&   FlatKnots,
                          const int                           Degree,
                          const double                        Tolerance3D,
                          double&                             UTolerance)
{
  BSplCLib_Resolution<gp_Pnt2d, gp_Vec2d, NCollection_Array1<gp_Pnt2d>, 2>(Poles,
                                                                           Weights,
                                                                           NumPoles,
                                                                           FlatKnots,
                                                                           Degree,
                                                                           Tolerance3D,
                                                                           UTolerance);
}

//==================================================================================================

void BSplCLib::FunctionMultiply(const BSplCLib_EvaluatorFunction&   FunctionPtr,
                                const int                           BSplineDegree,
                                const NCollection_Array1<double>&   BSplineFlatKnots,
                                const NCollection_Array1<gp_Pnt2d>& Poles,
                                const NCollection_Array1<double>&   FlatKnots,
                                const int                           NewDegree,
                                NCollection_Array1<gp_Pnt2d>&       NewPoles,
                                int&                                theStatus)
{
  BSplCLib_FunctionMultiply<gp_Pnt2d, gp_Vec2d, NCollection_Array1<gp_Pnt2d>, 2>(FunctionPtr,
                                                                                 BSplineDegree,
                                                                                 BSplineFlatKnots,
                                                                                 Poles,
                                                                                 FlatKnots,
                                                                                 NewDegree,
                                                                                 NewPoles,
                                                                                 theStatus);
}

//==================================================================================================

void BSplCLib::FunctionReparameterise(const BSplCLib_EvaluatorFunction&   FunctionPtr,
                                      const int                           BSplineDegree,
                                      const NCollection_Array1<double>&   BSplineFlatKnots,
                                      const NCollection_Array1<gp_Pnt2d>& Poles,
                                      const NCollection_Array1<double>&   FlatKnots,
                                      const int                           NewDegree,
                                      NCollection_Array1<gp_Pnt2d>&       NewPoles,
                                      int&                                theStatus)
{
  BSplCLib_FunctionReparameterise<gp_Pnt2d, gp_Vec2d, NCollection_Array1<gp_Pnt2d>, 2>(
    FunctionPtr,
    BSplineDegree,
    BSplineFlatKnots,
    Poles,
    FlatKnots,
    NewDegree,
    NewPoles,
    theStatus);
}
