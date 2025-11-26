// Created on: 1995-09-08
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

#include <BSplCLib.hxx>

#include <gp_Pnt.hxx>
#include <gp_Pnt2d.hxx>
#include <gp_Vec.hxx>
#include <gp_Vec2d.hxx>
#include <TColgp_Array1OfPnt.hxx>
#include <TColgp_Array1OfPnt2d.hxx>

#include "BSplCLib_CurveComputation.pxx"

#define No_Standard_RangeError
#define No_Standard_OutOfRange

//=================================================================================================

void BSplCLib::IncreaseDegree(const Standard_Integer      NewDegree,
                              const TColgp_Array1OfPnt&   Poles,
                              const TColStd_Array1OfReal* Weights,
                              TColgp_Array1OfPnt&         NewPoles,
                              TColStd_Array1OfReal*       NewWeights)
{
  BSplCLib_IncreaseDegree_Bezier<gp_Pnt, gp_Vec, TColgp_Array1OfPnt, 3>(NewDegree,
                                                                        Poles,
                                                                        Weights,
                                                                        NewPoles,
                                                                        NewWeights);
}

//=================================================================================================

void BSplCLib::IncreaseDegree(const Standard_Integer      NewDegree,
                              const TColgp_Array1OfPnt2d& Poles,
                              const TColStd_Array1OfReal* Weights,
                              TColgp_Array1OfPnt2d&       NewPoles,
                              TColStd_Array1OfReal*       NewWeights)
{
  BSplCLib_IncreaseDegree_Bezier<gp_Pnt2d, gp_Vec2d, TColgp_Array1OfPnt2d, 2>(NewDegree,
                                                                              Poles,
                                                                              Weights,
                                                                              NewPoles,
                                                                              NewWeights);
}

//=================================================================================================

void BSplCLib::PolesCoefficients(const TColgp_Array1OfPnt&   Poles,
                                 const TColStd_Array1OfReal* Weights,
                                 TColgp_Array1OfPnt&         CachePoles,
                                 TColStd_Array1OfReal*       CacheWeights)
{
  BSplCLib_PolesCoefficients_Bezier<gp_Pnt, gp_Vec, TColgp_Array1OfPnt, 3>(Poles,
                                                                           Weights,
                                                                           CachePoles,
                                                                           CacheWeights);
}

//=================================================================================================

void BSplCLib::PolesCoefficients(const TColgp_Array1OfPnt2d& Poles,
                                 const TColStd_Array1OfReal* Weights,
                                 TColgp_Array1OfPnt2d&       CachePoles,
                                 TColStd_Array1OfReal*       CacheWeights)
{
  BSplCLib_PolesCoefficients_Bezier<gp_Pnt2d, gp_Vec2d, TColgp_Array1OfPnt2d, 2>(Poles,
                                                                                 Weights,
                                                                                 CachePoles,
                                                                                 CacheWeights);
}

//=================================================================================================

void BSplCLib::D0(const Standard_Real         U,
                  const TColgp_Array1OfPnt&   Poles,
                  const TColStd_Array1OfReal* Weights,
                  gp_Pnt&                     P)
{
  const int              aDegree = Poles.Length() - 1;
  BSplCLib_KnotArrays<2> aBezierKnots(aDegree);
  BSplCLib::D0(U, 1, aDegree, 0, Poles, Weights, aBezierKnots.Knot, &aBezierKnots.Mult, P);
}

//=================================================================================================

void BSplCLib::D0(const Standard_Real         U,
                  const TColgp_Array1OfPnt2d& Poles,
                  const TColStd_Array1OfReal* Weights,
                  gp_Pnt2d&                   P)
{
  const int              aDegree = Poles.Length() - 1;
  BSplCLib_KnotArrays<2> aBezierKnots(aDegree);
  BSplCLib::D0(U, 1, aDegree, 0, Poles, Weights, aBezierKnots.Knot, &aBezierKnots.Mult, P);
}

//=================================================================================================

void BSplCLib::D1(const Standard_Real         U,
                  const TColgp_Array1OfPnt&   Poles,
                  const TColStd_Array1OfReal* Weights,
                  gp_Pnt&                     P,
                  gp_Vec&                     V)
{
  const int              aDegree = Poles.Length() - 1;
  BSplCLib_KnotArrays<2> aBezierKnots(aDegree);
  BSplCLib::D1(U, 1, aDegree, 0, Poles, Weights, aBezierKnots.Knot, &aBezierKnots.Mult, P, V);
}

//=================================================================================================

void BSplCLib::D1(const Standard_Real         U,
                  const TColgp_Array1OfPnt2d& Poles,
                  const TColStd_Array1OfReal* Weights,
                  gp_Pnt2d&                   P,
                  gp_Vec2d&                   V)
{
  const int              aDegree = Poles.Length() - 1;
  BSplCLib_KnotArrays<2> aBezierKnots(aDegree);
  BSplCLib::D1(U, 1, aDegree, 0, Poles, Weights, aBezierKnots.Knot, &aBezierKnots.Mult, P, V);
}

//=================================================================================================

void BSplCLib::D2(const Standard_Real         U,
                  const TColgp_Array1OfPnt&   Poles,
                  const TColStd_Array1OfReal* Weights,
                  gp_Pnt&                     P,
                  gp_Vec&                     V1,
                  gp_Vec&                     V2)
{
  const int              aDegree = Poles.Length() - 1;
  BSplCLib_KnotArrays<2> aBezierKnots(aDegree);
  BSplCLib::D2(U, 1, aDegree, 0, Poles, Weights, aBezierKnots.Knot, &aBezierKnots.Mult, P, V1, V2);
}

//=================================================================================================

void BSplCLib::D2(const Standard_Real         U,
                  const TColgp_Array1OfPnt2d& Poles,
                  const TColStd_Array1OfReal* Weights,
                  gp_Pnt2d&                   P,
                  gp_Vec2d&                   V1,
                  gp_Vec2d&                   V2)
{
  const int              aDegree = Poles.Length() - 1;
  BSplCLib_KnotArrays<2> aBezierKnots(aDegree);
  BSplCLib::D2(U, 1, aDegree, 0, Poles, Weights, aBezierKnots.Knot, &aBezierKnots.Mult, P, V1, V2);
}

//=================================================================================================

void BSplCLib::D3(const Standard_Real         U,
                  const TColgp_Array1OfPnt&   Poles,
                  const TColStd_Array1OfReal* Weights,
                  gp_Pnt&                     P,
                  gp_Vec&                     V1,
                  gp_Vec&                     V2,
                  gp_Vec&                     V3)
{
  const int              aDegree = Poles.Length() - 1;
  BSplCLib_KnotArrays<2> aBezierKnots(aDegree);
  BSplCLib::D3(U,
               1,
               aDegree,
               0,
               Poles,
               Weights,
               aBezierKnots.Knot,
               &aBezierKnots.Mult,
               P,
               V1,
               V2,
               V3);
}

//=================================================================================================

void BSplCLib::D3(const Standard_Real         U,
                  const TColgp_Array1OfPnt2d& Poles,
                  const TColStd_Array1OfReal* Weights,
                  gp_Pnt2d&                   P,
                  gp_Vec2d&                   V1,
                  gp_Vec2d&                   V2,
                  gp_Vec2d&                   V3)
{
  const int              aDegree = Poles.Length() - 1;
  BSplCLib_KnotArrays<2> aBezierKnots(aDegree);
  BSplCLib::D3(U,
               1,
               aDegree,
               0,
               Poles,
               Weights,
               aBezierKnots.Knot,
               &aBezierKnots.Mult,
               P,
               V1,
               V2,
               V3);
}
