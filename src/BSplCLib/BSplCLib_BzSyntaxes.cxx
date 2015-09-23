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
#include <TColStd_Array1OfInteger.hxx>
#include <TColStd_Array1OfReal.hxx>

#define No_Standard_RangeError
#define No_Standard_OutOfRange

//=======================================================================
//struct : BSplCLib_BezierArrays 
//purpose: Auxiliary structure providing standard definitions of bspline 
//         knots for bezier (using stack allocation)
//=======================================================================

class BSplCLib_BezierArrays 
{
 public:
  BSplCLib_BezierArrays (Standard_Integer Degree) 
  : knots (aKnots[0], 1, 2), mults (aMults[0], 1, 2) 
  {
    aKnots[0] = 0.;
    aKnots[1] = 1.;
    aMults[0] = aMults[1] = Degree + 1;
  }

 private:
  Standard_Real aKnots[2];
  Standard_Integer aMults[2];
  
 public:
  TColStd_Array1OfReal    knots;
  TColStd_Array1OfInteger mults;
};
  
//=======================================================================
//function : IncreaseDegree
//purpose  : 
//=======================================================================

void BSplCLib::IncreaseDegree(const Standard_Integer      NewDegree, 
			      const TColgp_Array1OfPnt&   Poles, 
			      const TColStd_Array1OfReal* Weights, 
			      TColgp_Array1OfPnt&         NewPoles, 
			      TColStd_Array1OfReal*       NewWeights)
{
  Standard_Integer deg = Poles.Length() - 1;
  BSplCLib_BezierArrays bzarr(deg);
  BSplCLib::IncreaseDegree(deg, NewDegree, 0,
			   Poles, Weights, bzarr.knots, bzarr.mults,
			   NewPoles, NewWeights, bzarr.knots, bzarr.mults);
}

//=======================================================================
//function : IncreaseDegree
//purpose  : 
//=======================================================================

void BSplCLib::IncreaseDegree(const Standard_Integer      NewDegree, 
			      const TColgp_Array1OfPnt2d& Poles, 
			      const TColStd_Array1OfReal* Weights, 
			      TColgp_Array1OfPnt2d&       NewPoles, 
			      TColStd_Array1OfReal*       NewWeights)
{
  Standard_Integer deg = Poles.Length() - 1;
  BSplCLib_BezierArrays bzarr(deg);
  BSplCLib::IncreaseDegree(deg, NewDegree, 0,
			   Poles, Weights, bzarr.knots, bzarr.mults,
			   NewPoles, NewWeights, bzarr.knots, bzarr.mults);
}

//=======================================================================
//function : PolesCoefficients
//purpose  : 
//=======================================================================

void BSplCLib::PolesCoefficients(const TColgp_Array1OfPnt&   Poles, 
				 const TColStd_Array1OfReal* Weights, 
				 TColgp_Array1OfPnt&         CachePoles, 
				 TColStd_Array1OfReal*       CacheWeights)
{
  Standard_Integer deg  = Poles.Length() - 1;
  TColStd_Array1OfReal bidflatknots (FlatBezierKnots(deg), 1, 2*(deg+1));
  BSplCLib::BuildCache(0.,1.,0,deg,bidflatknots,
		       Poles, Weights, CachePoles,CacheWeights);
}

//=======================================================================
//function : PolesCoefficients
//purpose  : 
//=======================================================================

void BSplCLib::PolesCoefficients(const TColgp_Array1OfPnt2d& Poles, 
				 const TColStd_Array1OfReal* Weights, 
				 TColgp_Array1OfPnt2d&       CachePoles, 
				 TColStd_Array1OfReal*       CacheWeights)
{
  Standard_Integer deg  = Poles.Length() - 1;
  TColStd_Array1OfReal bidflatknots (FlatBezierKnots(deg), 1, 2*(deg+1));
  BSplCLib::BuildCache(0.,1.,0,deg,bidflatknots,
		       Poles, Weights, CachePoles,CacheWeights);
}

//=======================================================================
//function : D0
//purpose  : 
//=======================================================================

void BSplCLib::D0(const Standard_Real         U,
		  const TColgp_Array1OfPnt&   Poles, 
		  const TColStd_Array1OfReal* Weights,
		  gp_Pnt&                     P)
{
  Standard_Integer deg = Poles.Length() - 1;
  BSplCLib_BezierArrays bzarr(deg);
  BSplCLib::D0(U, 1, deg, 0, Poles, Weights, bzarr.knots, &bzarr.mults, P);
}

//=======================================================================
//function : D0
//purpose  : 
//=======================================================================

void BSplCLib::D0(const Standard_Real         U,
		  const TColgp_Array1OfPnt2d& Poles, 
		  const TColStd_Array1OfReal* Weights,
		  gp_Pnt2d&                   P)
{
  Standard_Integer deg = Poles.Length() - 1;
  BSplCLib_BezierArrays bzarr(deg);
  BSplCLib::D0(U, 1, deg, 0, Poles, Weights, bzarr.knots, &bzarr.mults, P);
}

//=======================================================================
//function : D1
//purpose  : 
//=======================================================================

void BSplCLib::D1(const Standard_Real         U,
		  const TColgp_Array1OfPnt&   Poles, 
		  const TColStd_Array1OfReal* Weights,
		  gp_Pnt&                     P,
		  gp_Vec&                     V)
{
  Standard_Integer deg = Poles.Length() - 1;
  BSplCLib_BezierArrays bzarr(deg);
  BSplCLib::D1(U, 1, deg, 0, Poles, Weights, bzarr.knots, &bzarr.mults, P, V);
}

//=======================================================================
//function : D1
//purpose  : 
//=======================================================================

void BSplCLib::D1(const Standard_Real         U,
		  const TColgp_Array1OfPnt2d& Poles, 
		  const TColStd_Array1OfReal* Weights,
		  gp_Pnt2d&                   P,
		  gp_Vec2d&                   V)
{
  Standard_Integer deg = Poles.Length() - 1;
  BSplCLib_BezierArrays bzarr(deg);
  BSplCLib::D1(U, 1, deg, 0, Poles, Weights, bzarr.knots, &bzarr.mults, P, V);
}


//=======================================================================
//function : D2
//purpose  : 
//=======================================================================

void BSplCLib::D2(const Standard_Real         U,
		  const TColgp_Array1OfPnt&   Poles, 
		  const TColStd_Array1OfReal* Weights,
		  gp_Pnt&                     P,
		  gp_Vec&                     V1,
		  gp_Vec&                     V2)
{
  Standard_Integer deg = Poles.Length() - 1;
  BSplCLib_BezierArrays bzarr(deg);
  BSplCLib::D2(U, 1, deg, 0, Poles, Weights, bzarr.knots, &bzarr.mults, P, V1, V2);
}

//=======================================================================
//function : D2
//purpose  : 
//=======================================================================

void BSplCLib::D2(const Standard_Real         U,
		  const TColgp_Array1OfPnt2d& Poles, 
		  const TColStd_Array1OfReal* Weights,
		  gp_Pnt2d&                   P,
		  gp_Vec2d&                   V1,
		  gp_Vec2d&                   V2)
{
  Standard_Integer deg = Poles.Length() - 1;
  BSplCLib_BezierArrays bzarr(deg);
  BSplCLib::D2(U, 1, deg, 0, Poles, Weights, bzarr.knots, &bzarr.mults, P, V1, V2);
}


//=======================================================================
//function : D3
//purpose  : 
//=======================================================================

void BSplCLib::D3(const Standard_Real         U,
		  const TColgp_Array1OfPnt&   Poles, 
		  const TColStd_Array1OfReal* Weights,
		  gp_Pnt&                     P,
		  gp_Vec&                     V1,
		  gp_Vec&                     V2,
		  gp_Vec&                     V3)
{
  Standard_Integer deg = Poles.Length() - 1;
  BSplCLib_BezierArrays bzarr(deg);
  BSplCLib::D3(U, 1, deg, 0, Poles, Weights, bzarr.knots, &bzarr.mults, 
	       P, V1, V2, V3);
}

//=======================================================================
//function : D3
//purpose  : 
//=======================================================================

void BSplCLib::D3(const Standard_Real         U,
		  const TColgp_Array1OfPnt2d& Poles, 
		  const TColStd_Array1OfReal* Weights,
		  gp_Pnt2d&                   P,
		  gp_Vec2d&                   V1,
		  gp_Vec2d&                   V2,
		  gp_Vec2d&                   V3)
{
  Standard_Integer deg = Poles.Length() - 1;
  BSplCLib_BezierArrays bzarr(deg);
  BSplCLib::D3(U, 1, deg, 0, Poles, Weights, bzarr.knots, &bzarr.mults, 
	       P, V1, V2, V3);
}

