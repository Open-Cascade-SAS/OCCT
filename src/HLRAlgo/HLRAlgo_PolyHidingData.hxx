// Created on: 1993-10-29
// Created by: Christophe MARION
// Copyright (c) 1993-1999 Matra Datavision
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

#ifndef _HLRAlgo_PolyHidingData_HeaderFile
#define _HLRAlgo_PolyHidingData_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Integer.hxx>
#include <Standard_Real.hxx>


//! Data structure of a set of Hiding Triangles.
class HLRAlgo_PolyHidingData
{
public:
  DEFINE_STANDARD_ALLOC

  struct TriangleIndices
  {
    Standard_Integer Index, Min, Max;
  };

  struct PlaneT
  {
    gp_XYZ Normal;
    Standard_Real D;
  };

  HLRAlgo_PolyHidingData()
  {
  }

  void Set (
    const Standard_Integer Index,
    const Standard_Integer Minim,
    const Standard_Integer Maxim,
    const Standard_Real A,
    const Standard_Real B,
    const Standard_Real C,
    const Standard_Real D)
  {
    myIndices.Index = Index;
    myIndices.Min = Minim;
    myIndices.Max = Maxim;
    myPlane.Normal = gp_XYZ(A, B, C);
    myPlane.D = D;
  }

  TriangleIndices& Indices()
  {
    return myIndices;
  }

  PlaneT& Plane()
  {
    return myPlane;
  }

private:
  TriangleIndices myIndices;
  PlaneT myPlane;
};

#endif // _HLRAlgo_PolyHidingData_HeaderFile
