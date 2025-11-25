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

void BSplCLib::Reverse(TColgp_Array1OfPnt2d& Poles, const Standard_Integer L)
{
  BSplCLib_Reverse(Poles, L);
}

//==================================================================================================

Standard_Boolean BSplCLib::RemoveKnot(const Standard_Integer         Index,
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
                                      const Standard_Real            Tolerance)
{
  return BSplCLib_RemoveKnot<gp_Pnt2d, gp_Vec2d, TColgp_Array1OfPnt2d, 2>(Index,
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

void BSplCLib::InsertKnots(const Standard_Integer         Degree,
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
                           const Standard_Boolean         Add)
{
  BSplCLib_InsertKnots<gp_Pnt2d, gp_Vec2d, TColgp_Array1OfPnt2d, 2>(Degree,
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

void BSplCLib::InsertKnot(const Standard_Integer,
                          const Standard_Real            U,
                          const Standard_Integer         UMult,
                          const Standard_Integer         Degree,
                          const Standard_Boolean         Periodic,
                          const TColgp_Array1OfPnt2d&    Poles,
                          const TColStd_Array1OfReal*    Weights,
                          const TColStd_Array1OfReal&    Knots,
                          const TColStd_Array1OfInteger& Mults,
                          TColgp_Array1OfPnt2d&          NewPoles,
                          TColStd_Array1OfReal*          NewWeights)
{
  BSplCLib_InsertKnot<gp_Pnt2d, gp_Vec2d, TColgp_Array1OfPnt2d, 2>(0,
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

void BSplCLib::RaiseMultiplicity(const Standard_Integer         KnotIndex,
                                 const Standard_Integer         Mult,
                                 const Standard_Integer         Degree,
                                 const Standard_Boolean         Periodic,
                                 const TColgp_Array1OfPnt2d&    Poles,
                                 const TColStd_Array1OfReal*    Weights,
                                 const TColStd_Array1OfReal&    Knots,
                                 const TColStd_Array1OfInteger& Mults,
                                 TColgp_Array1OfPnt2d&          NewPoles,
                                 TColStd_Array1OfReal*          NewWeights)
{
  BSplCLib_RaiseMultiplicity<gp_Pnt2d, gp_Vec2d, TColgp_Array1OfPnt2d, 2>(KnotIndex,
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

void BSplCLib::IncreaseDegree(const Standard_Integer         Degree,
                              const Standard_Integer         NewDegree,
                              const Standard_Boolean         Periodic,
                              const TColgp_Array1OfPnt2d&    Poles,
                              const TColStd_Array1OfReal*    Weights,
                              const TColStd_Array1OfReal&    Knots,
                              const TColStd_Array1OfInteger& Mults,
                              TColgp_Array1OfPnt2d&          NewPoles,
                              TColStd_Array1OfReal*          NewWeights,
                              TColStd_Array1OfReal&          NewKnots,
                              TColStd_Array1OfInteger&       NewMults)
{
  BSplCLib_IncreaseDegree<gp_Pnt2d, gp_Vec2d, TColgp_Array1OfPnt2d, 2>(Degree,
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

void BSplCLib::Unperiodize(const Standard_Integer         Degree,
                           const TColStd_Array1OfInteger& Mults,
                           const TColStd_Array1OfReal&    Knots,
                           const TColgp_Array1OfPnt2d&    Poles,
                           const TColStd_Array1OfReal*    Weights,
                           TColStd_Array1OfInteger&       NewMults,
                           TColStd_Array1OfReal&          NewKnots,
                           TColgp_Array1OfPnt2d&          NewPoles,
                           TColStd_Array1OfReal*          NewWeights)
{
  BSplCLib_Unperiodize<gp_Pnt2d, gp_Vec2d, TColgp_Array1OfPnt2d, 2>(Degree,
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

void BSplCLib::Trimming(const Standard_Integer         Degree,
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
                        TColStd_Array1OfReal*          NewWeights)
{
  BSplCLib_Trimming<gp_Pnt2d, gp_Vec2d, TColgp_Array1OfPnt2d, 2>(Degree,
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

void BSplCLib::BuildEval(const Standard_Integer      Degree,
                         const Standard_Integer      Index,
                         const TColgp_Array1OfPnt2d& Poles,
                         const TColStd_Array1OfReal* Weights,
                         Standard_Real&              LP)
{
  BSplCLib_BuildEval<gp_Pnt2d, gp_Vec2d, TColgp_Array1OfPnt2d, 2>(Degree,
                                                                  Index,
                                                                  Poles,
                                                                  Weights,
                                                                  LP);
}

//==================================================================================================

void BSplCLib::D0(const Standard_Real            U,
                  const Standard_Integer         Index,
                  const Standard_Integer         Degree,
                  const Standard_Boolean         Periodic,
                  const TColgp_Array1OfPnt2d&    Poles,
                  const TColStd_Array1OfReal*    Weights,
                  const TColStd_Array1OfReal&    Knots,
                  const TColStd_Array1OfInteger* Mults,
                  gp_Pnt2d&                      P)
{
  BSplCLib_D0<gp_Pnt2d, gp_Vec2d, TColgp_Array1OfPnt2d, 2>(U,
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

void BSplCLib::D1(const Standard_Real            U,
                  const Standard_Integer         Index,
                  const Standard_Integer         Degree,
                  const Standard_Boolean         Periodic,
                  const TColgp_Array1OfPnt2d&    Poles,
                  const TColStd_Array1OfReal*    Weights,
                  const TColStd_Array1OfReal&    Knots,
                  const TColStd_Array1OfInteger* Mults,
                  gp_Pnt2d&                      P,
                  gp_Vec2d&                      V)
{
  BSplCLib_D1<gp_Pnt2d, gp_Vec2d, TColgp_Array1OfPnt2d, 2>(U,
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

void BSplCLib::D2(const Standard_Real            U,
                  const Standard_Integer         Index,
                  const Standard_Integer         Degree,
                  const Standard_Boolean         Periodic,
                  const TColgp_Array1OfPnt2d&    Poles,
                  const TColStd_Array1OfReal*    Weights,
                  const TColStd_Array1OfReal&    Knots,
                  const TColStd_Array1OfInteger* Mults,
                  gp_Pnt2d&                      P,
                  gp_Vec2d&                      V1,
                  gp_Vec2d&                      V2)
{
  BSplCLib_D2<gp_Pnt2d, gp_Vec2d, TColgp_Array1OfPnt2d, 2>(U,
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

void BSplCLib::D3(const Standard_Real            U,
                  const Standard_Integer         Index,
                  const Standard_Integer         Degree,
                  const Standard_Boolean         Periodic,
                  const TColgp_Array1OfPnt2d&    Poles,
                  const TColStd_Array1OfReal*    Weights,
                  const TColStd_Array1OfReal&    Knots,
                  const TColStd_Array1OfInteger* Mults,
                  gp_Pnt2d&                      P,
                  gp_Vec2d&                      V1,
                  gp_Vec2d&                      V2,
                  gp_Vec2d&                      V3)
{
  BSplCLib_D3<gp_Pnt2d, gp_Vec2d, TColgp_Array1OfPnt2d, 2>(U,
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

void BSplCLib::DN(const Standard_Real            U,
                  const Standard_Integer         N,
                  const Standard_Integer         Index,
                  const Standard_Integer         Degree,
                  const Standard_Boolean         Periodic,
                  const TColgp_Array1OfPnt2d&    Poles,
                  const TColStd_Array1OfReal*    Weights,
                  const TColStd_Array1OfReal&    Knots,
                  const TColStd_Array1OfInteger* Mults,
                  gp_Vec2d&                      VN)
{
  BSplCLib_DN<gp_Pnt2d, gp_Vec2d, TColgp_Array1OfPnt2d, 2>(U,
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

Standard_Integer BSplCLib::SolveBandedSystem(const math_Matrix&     Matrix,
                                             const Standard_Integer UpperBandWidth,
                                             const Standard_Integer LowerBandWidth,
                                             TColgp_Array1OfPnt2d&  PolesArray)
{
  return BSplCLib_SolveBandedSystem<gp_Pnt2d, gp_Vec2d, TColgp_Array1OfPnt2d, 2>(Matrix,
                                                                                 UpperBandWidth,
                                                                                 LowerBandWidth,
                                                                                 PolesArray);
}

//==================================================================================================

Standard_Integer BSplCLib::SolveBandedSystem(const math_Matrix&     Matrix,
                                             const Standard_Integer UpperBandWidth,
                                             const Standard_Integer LowerBandWidth,
                                             const Standard_Boolean HomogeneousFlag,
                                             TColgp_Array1OfPnt2d&  PolesArray,
                                             TColStd_Array1OfReal&  WeightsArray)
{
  return BSplCLib_SolveBandedSystem<gp_Pnt2d, gp_Vec2d, TColgp_Array1OfPnt2d, 2>(Matrix,
                                                                                 UpperBandWidth,
                                                                                 LowerBandWidth,
                                                                                 HomogeneousFlag,
                                                                                 PolesArray,
                                                                                 WeightsArray);
}

//==================================================================================================

void BSplCLib::Eval(const Standard_Real         Parameter,
                    const Standard_Boolean      PeriodicFlag,
                    const Standard_Boolean      HomogeneousFlag,
                    Standard_Integer&           ExtrapMode,
                    const Standard_Integer      Degree,
                    const TColStd_Array1OfReal& FlatKnots,
                    const TColgp_Array1OfPnt2d& PolesArray,
                    const TColStd_Array1OfReal& WeightsArray,
                    gp_Pnt2d&                   aPoint,
                    Standard_Real&              aWeight)
{
  BSplCLib_Eval<gp_Pnt2d, gp_Vec2d, TColgp_Array1OfPnt2d, 2>(Parameter,
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

void BSplCLib::CacheD0(const Standard_Real         Parameter,
                       const Standard_Integer      Degree,
                       const Standard_Real         CacheParameter,
                       const Standard_Real         SpanLenght,
                       const TColgp_Array1OfPnt2d& PolesArray,
                       const TColStd_Array1OfReal* WeightsArray,
                       gp_Pnt2d&                   aPoint)
{
  BSplCLib_CacheD0<gp_Pnt2d, gp_Vec2d, TColgp_Array1OfPnt2d, 2>(Parameter,
                                                                Degree,
                                                                CacheParameter,
                                                                SpanLenght,
                                                                PolesArray,
                                                                WeightsArray,
                                                                aPoint);
}

//==================================================================================================

void BSplCLib::CacheD1(const Standard_Real         Parameter,
                       const Standard_Integer      Degree,
                       const Standard_Real         CacheParameter,
                       const Standard_Real         SpanLenght,
                       const TColgp_Array1OfPnt2d& PolesArray,
                       const TColStd_Array1OfReal* WeightsArray,
                       gp_Pnt2d&                   aPoint,
                       gp_Vec2d&                   aVector)
{
  BSplCLib_CacheD1<gp_Pnt2d, gp_Vec2d, TColgp_Array1OfPnt2d, 2>(Parameter,
                                                                Degree,
                                                                CacheParameter,
                                                                SpanLenght,
                                                                PolesArray,
                                                                WeightsArray,
                                                                aPoint,
                                                                aVector);
}

//==================================================================================================

void BSplCLib::CacheD2(const Standard_Real         Parameter,
                       const Standard_Integer      Degree,
                       const Standard_Real         CacheParameter,
                       const Standard_Real         SpanLenght,
                       const TColgp_Array1OfPnt2d& PolesArray,
                       const TColStd_Array1OfReal* WeightsArray,
                       gp_Pnt2d&                   aPoint,
                       gp_Vec2d&                   aVector1,
                       gp_Vec2d&                   aVector2)
{
  BSplCLib_CacheD2<gp_Pnt2d, gp_Vec2d, TColgp_Array1OfPnt2d, 2>(Parameter,
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

void BSplCLib::CacheD3(const Standard_Real         Parameter,
                       const Standard_Integer      Degree,
                       const Standard_Real         CacheParameter,
                       const Standard_Real         SpanLenght,
                       const TColgp_Array1OfPnt2d& PolesArray,
                       const TColStd_Array1OfReal* WeightsArray,
                       gp_Pnt2d&                   aPoint,
                       gp_Vec2d&                   aVector1,
                       gp_Vec2d&                   aVector2,
                       gp_Vec2d&                   aVector3)
{
  BSplCLib_CacheD3<gp_Pnt2d, gp_Vec2d, TColgp_Array1OfPnt2d, 2>(Parameter,
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

void BSplCLib::BuildCache(const Standard_Real         U,
                          const Standard_Real         SpanDomain,
                          const Standard_Boolean      Periodic,
                          const Standard_Integer      Degree,
                          const TColStd_Array1OfReal& FlatKnots,
                          const TColgp_Array1OfPnt2d& Poles,
                          const TColStd_Array1OfReal* Weights,
                          TColgp_Array1OfPnt2d&       CachePoles,
                          TColStd_Array1OfReal*       CacheWeights)
{
  BSplCLib_BuildCache<gp_Pnt2d, gp_Vec2d, TColgp_Array1OfPnt2d, 2>(U,
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

void BSplCLib::BuildCache(const Standard_Real         theParameter,
                          const Standard_Real         theSpanDomain,
                          const Standard_Boolean      thePeriodicFlag,
                          const Standard_Integer      theDegree,
                          const Standard_Integer      theSpanIndex,
                          const TColStd_Array1OfReal& theFlatKnots,
                          const TColgp_Array1OfPnt2d& thePoles,
                          const TColStd_Array1OfReal* theWeights,
                          TColStd_Array2OfReal&       theCacheArray)
{
  BSplCLib_BuildCache<gp_Pnt2d, gp_Vec2d, TColgp_Array1OfPnt2d, 2>(theParameter,
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

void BSplCLib::Interpolate(const Standard_Integer         Degree,
                           const TColStd_Array1OfReal&    FlatKnots,
                           const TColStd_Array1OfReal&    Parameters,
                           const TColStd_Array1OfInteger& ContactOrderArray,
                           TColgp_Array1OfPnt2d&          Poles,
                           Standard_Integer&              InversionProblem)
{
  BSplCLib_Interpolate<gp_Pnt2d, gp_Vec2d, TColgp_Array1OfPnt2d, 2>(Degree,
                                                                    FlatKnots,
                                                                    Parameters,
                                                                    ContactOrderArray,
                                                                    Poles,
                                                                    InversionProblem);
}

//==================================================================================================

void BSplCLib::Interpolate(const Standard_Integer         Degree,
                           const TColStd_Array1OfReal&    FlatKnots,
                           const TColStd_Array1OfReal&    Parameters,
                           const TColStd_Array1OfInteger& ContactOrderArray,
                           TColgp_Array1OfPnt2d&          Poles,
                           TColStd_Array1OfReal&          Weights,
                           Standard_Integer&              InversionProblem)
{
  BSplCLib_Interpolate<gp_Pnt2d, gp_Vec2d, TColgp_Array1OfPnt2d, 2>(Degree,
                                                                    FlatKnots,
                                                                    Parameters,
                                                                    ContactOrderArray,
                                                                    Poles,
                                                                    Weights,
                                                                    InversionProblem);
}

//==================================================================================================

void BSplCLib::MovePoint(const Standard_Real         U,
                         const gp_Vec2d&             Displ,
                         const Standard_Integer      Index1,
                         const Standard_Integer      Index2,
                         const Standard_Integer      Degree,
                         const TColgp_Array1OfPnt2d& Poles,
                         const TColStd_Array1OfReal* Weights,
                         const TColStd_Array1OfReal& FlatKnots,
                         Standard_Integer&           FirstIndex,
                         Standard_Integer&           LastIndex,
                         TColgp_Array1OfPnt2d&       NewPoles)
{
  BSplCLib_MovePoint<gp_Pnt2d, gp_Vec2d, TColgp_Array1OfPnt2d, 2>(U,
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

void BSplCLib::MovePointAndTangent(const Standard_Real         U,
                                   const gp_Vec2d&             Delta,
                                   const gp_Vec2d&             DeltaDerivatives,
                                   const Standard_Real         Tolerance,
                                   const Standard_Integer      Degree,
                                   const Standard_Integer      StartingCondition,
                                   const Standard_Integer      EndingCondition,
                                   const TColgp_Array1OfPnt2d& Poles,
                                   const TColStd_Array1OfReal* Weights,
                                   const TColStd_Array1OfReal& FlatKnots,
                                   TColgp_Array1OfPnt2d&       NewPoles,
                                   Standard_Integer&           ErrorStatus)
{
  BSplCLib_MovePointAndTangent<gp_Pnt2d, gp_Vec2d, TColgp_Array1OfPnt2d, 2>(U,
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

void BSplCLib::Resolution(const TColgp_Array1OfPnt2d& Poles,
                          const TColStd_Array1OfReal* Weights,
                          const Standard_Integer      NumPoles,
                          const TColStd_Array1OfReal& FlatKnots,
                          const Standard_Integer      Degree,
                          const Standard_Real         Tolerance3D,
                          Standard_Real&              UTolerance)
{
  BSplCLib_Resolution<gp_Pnt2d, gp_Vec2d, TColgp_Array1OfPnt2d, 2>(Poles,
                                                                   Weights,
                                                                   NumPoles,
                                                                   FlatKnots,
                                                                   Degree,
                                                                   Tolerance3D,
                                                                   UTolerance);
}

//==================================================================================================

void BSplCLib::FunctionMultiply(const BSplCLib_EvaluatorFunction& FunctionPtr,
                                const Standard_Integer            BSplineDegree,
                                const TColStd_Array1OfReal&       BSplineFlatKnots,
                                const TColgp_Array1OfPnt2d&       Poles,
                                const TColStd_Array1OfReal&       FlatKnots,
                                const Standard_Integer            NewDegree,
                                TColgp_Array1OfPnt2d&             NewPoles,
                                Standard_Integer&                 theStatus)
{
  BSplCLib_FunctionMultiply<gp_Pnt2d, gp_Vec2d, TColgp_Array1OfPnt2d, 2>(FunctionPtr,
                                                                         BSplineDegree,
                                                                         BSplineFlatKnots,
                                                                         Poles,
                                                                         FlatKnots,
                                                                         NewDegree,
                                                                         NewPoles,
                                                                         theStatus);
}

//==================================================================================================

void BSplCLib::FunctionReparameterise(const BSplCLib_EvaluatorFunction& FunctionPtr,
                                      const Standard_Integer            BSplineDegree,
                                      const TColStd_Array1OfReal&       BSplineFlatKnots,
                                      const TColgp_Array1OfPnt2d&       Poles,
                                      const TColStd_Array1OfReal&       FlatKnots,
                                      const Standard_Integer            NewDegree,
                                      TColgp_Array1OfPnt2d&             NewPoles,
                                      Standard_Integer&                 theStatus)
{
  BSplCLib_FunctionReparameterise<gp_Pnt2d, gp_Vec2d, TColgp_Array1OfPnt2d, 2>(FunctionPtr,
                                                                               BSplineDegree,
                                                                               BSplineFlatKnots,
                                                                               Poles,
                                                                               FlatKnots,
                                                                               NewDegree,
                                                                               NewPoles,
                                                                               theStatus);
}
