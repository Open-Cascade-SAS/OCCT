// Created on: 2012-12-06
// Created by: Sergey KHROMOV
// Copyright (c) 2004-2014 OPEN CASCADE SAS
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

#ifndef _Extrema_POnSurfParams_HeaderFile
#define _Extrema_POnSurfParams_HeaderFile

#include <Extrema_ElementType.hxx>
#include <Extrema_POnSurf.hxx>
#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>

class gp_Pnt;

//! Data container for point on surface parameters. These parameters
//! are required to compute an initial approximation for extrema
//! computation.
class Extrema_POnSurfParams : public Extrema_POnSurf
{
public:
  DEFINE_STANDARD_ALLOC

  //! empty constructor
  Extrema_POnSurfParams()
      : mySqrDistance(0.),
        myElementType(Extrema_Node),
        myIndexU(0),
        myIndexV(0)
  {
  }

  //! Creation of a point on surface with parameter
  //! values on the surface and a Pnt from gp.
  Extrema_POnSurfParams(const double theU, const double theV, const gp_Pnt& thePnt)
      : Extrema_POnSurf(theU, theV, thePnt),
        mySqrDistance(0.),
        myElementType(Extrema_Node),
        myIndexU(0),
        myIndexV(0)
  {
  }

  //! Sets the square distance from this point to another one
  //! (e.g. to the point to be projected).
  void SetSqrDistance(const double theSqrDistance) { mySqrDistance = theSqrDistance; }

  //! Query the square distance from this point to another one.
  double GetSqrDistance() const { return mySqrDistance; }

  //! Sets the element type on which this point is situated.
  void SetElementType(const Extrema_ElementType theElementType) { myElementType = theElementType; }

  //! Query the element type on which this point is situated.
  Extrema_ElementType GetElementType() const { return myElementType; }

  //! Sets the U and V indices of an element that contains
  //! this point.
  void SetIndices(const int theIndexU, const int theIndexV)
  {
    myIndexU = theIndexU;
    myIndexV = theIndexV;
  }

  //! Query the U and V indices of an element that contains
  //! this point.
  void GetIndices(int& theIndexU, int& theIndexV) const
  {
    theIndexU = myIndexU;
    theIndexV = myIndexV;
  }

private:
  double              mySqrDistance;
  Extrema_ElementType myElementType;
  int                 myIndexU;
  int                 myIndexV;
};

#endif // _Extrema_POnSurfParams_HeaderFile
