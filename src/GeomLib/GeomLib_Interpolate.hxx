// Created on: 1996-08-30
// Created by: Xavier BENVENISTE
// Copyright (c) 1996-1999 Matra Datavision
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

#ifndef _GeomLib_Interpolate_HeaderFile
#define _GeomLib_Interpolate_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <GeomLib_InterpolationErrors.hxx>
#include <Standard_Integer.hxx>
#include <TColgp_Array1OfPnt.hxx>
#include <TColStd_Array1OfReal.hxx>
class Geom_BSplineCurve;


//! this class is used to construct a BSpline curve by
//! interpolation  of points  at given parameters  The
//! continuity   of the curve   is degree -  1 and the
//! method used when boundary  condition are not given
//! is to use odd degrees  and null the derivatives on
//! both sides from degree -1 down to (degree+1) / 2
//! When even degree is given the returned curve is of
//! degree - 1 so that the degree of the curve is odd
class GeomLib_Interpolate 
{
public:

  DEFINE_STANDARD_ALLOC

  
  Standard_EXPORT GeomLib_Interpolate(const Standard_Integer Degree, const Standard_Integer NumPoints, const TColgp_Array1OfPnt& Points, const TColStd_Array1OfReal& Parameters);
  

  //! returns if everything went OK
    Standard_Boolean IsDone() const;
  
  //! returns the error type if any
    GeomLib_InterpolationErrors Error() const;
  
  //! returns the interpolated curve of the requested degree
  Standard_EXPORT Handle(Geom_BSplineCurve) Curve() const;




protected:





private:



  Handle(Geom_BSplineCurve) myCurve;
  Standard_Boolean myIsDone;
  GeomLib_InterpolationErrors myError;


};


#include <GeomLib_Interpolate.lxx>





#endif // _GeomLib_Interpolate_HeaderFile
