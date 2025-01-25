// Created on: 1998-06-03
// Created by: data exchange team
// Copyright (c) 1998-1999 Matra Datavision
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

#ifndef _ShapeAnalysis_Geom_HeaderFile
#define _ShapeAnalysis_Geom_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <TColgp_Array1OfPnt.hxx>
#include <TColStd_HArray2OfReal.hxx>
class gp_Pln;
class gp_Trsf;

//! Analyzing tool aimed to work on primitive geometrical objects
class ShapeAnalysis_Geom
{
public:
  DEFINE_STANDARD_ALLOC

  //! Builds a plane out of a set of points in array
  //! Returns in <dmax> the maximal distance between the produced
  //! plane and given points
  Standard_EXPORT static Standard_Boolean NearestPlane(const TColgp_Array1OfPnt& Pnts,
                                                       gp_Pln&                   aPln,
                                                       Standard_Real&            Dmax);

  //! Builds transformation object out of matrix.
  //! Matrix must be 3 x 4.
  //! Unit is used as multiplier.
  Standard_EXPORT static Standard_Boolean PositionTrsf(const Handle(TColStd_HArray2OfReal)& coefs,
                                                       gp_Trsf&                             trsf,
                                                       const Standard_Real                  unit,
                                                       const Standard_Real                  prec);

protected:
private:
};

#endif // _ShapeAnalysis_Geom_HeaderFile
