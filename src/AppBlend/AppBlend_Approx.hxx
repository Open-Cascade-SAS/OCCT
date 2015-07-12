// Created on: 1996-08-27
// Created by: Philippe MANGIN
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

#ifndef _AppBlend_Approx_HeaderFile
#define _AppBlend_Approx_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Boolean.hxx>
#include <Standard_Integer.hxx>
#include <TColgp_Array2OfPnt.hxx>
#include <TColStd_Array2OfReal.hxx>
#include <TColStd_Array1OfReal.hxx>
#include <TColStd_Array1OfInteger.hxx>
#include <TColgp_Array1OfPnt2d.hxx>
#include <Standard_Real.hxx>
class StdFail_NotDone;
class Standard_DomainError;
class Standard_OutOfRange;


//! Bspline approximation of a surface.
class AppBlend_Approx 
{
public:

  DEFINE_STANDARD_ALLOC

  
  Standard_EXPORT virtual Standard_Boolean IsDone() const = 0;
  
  Standard_EXPORT virtual void SurfShape (Standard_Integer& UDegree, Standard_Integer& VDegree, Standard_Integer& NbUPoles, Standard_Integer& NbVPoles, Standard_Integer& NbUKnots, Standard_Integer& NbVKnots) const = 0;
  
  Standard_EXPORT virtual void Surface (TColgp_Array2OfPnt& TPoles, TColStd_Array2OfReal& TWeights, TColStd_Array1OfReal& TUKnots, TColStd_Array1OfReal& TVKnots, TColStd_Array1OfInteger& TUMults, TColStd_Array1OfInteger& TVMults) const = 0;
  
  Standard_EXPORT virtual Standard_Integer UDegree() const = 0;
  
  Standard_EXPORT virtual Standard_Integer VDegree() const = 0;
  
  Standard_EXPORT virtual const TColgp_Array2OfPnt& SurfPoles() const = 0;
  
  Standard_EXPORT virtual const TColStd_Array2OfReal& SurfWeights() const = 0;
  
  Standard_EXPORT virtual const TColStd_Array1OfReal& SurfUKnots() const = 0;
  
  Standard_EXPORT virtual const TColStd_Array1OfReal& SurfVKnots() const = 0;
  
  Standard_EXPORT virtual const TColStd_Array1OfInteger& SurfUMults() const = 0;
  
  Standard_EXPORT virtual const TColStd_Array1OfInteger& SurfVMults() const = 0;
  
  Standard_EXPORT virtual Standard_Integer NbCurves2d() const = 0;
  
  Standard_EXPORT virtual void Curves2dShape (Standard_Integer& Degree, Standard_Integer& NbPoles, Standard_Integer& NbKnots) const = 0;
  
  Standard_EXPORT virtual void Curve2d (const Standard_Integer Index, TColgp_Array1OfPnt2d& TPoles, TColStd_Array1OfReal& TKnots, TColStd_Array1OfInteger& TMults) const = 0;
  
  Standard_EXPORT virtual Standard_Integer Curves2dDegree() const = 0;
  
  Standard_EXPORT virtual const TColgp_Array1OfPnt2d& Curve2dPoles (const Standard_Integer Index) const = 0;
  
  Standard_EXPORT virtual const TColStd_Array1OfReal& Curves2dKnots() const = 0;
  
  Standard_EXPORT virtual const TColStd_Array1OfInteger& Curves2dMults() const = 0;
  
  Standard_EXPORT virtual void TolReached (Standard_Real& Tol3d, Standard_Real& Tol2d) const = 0;
  
  Standard_EXPORT virtual Standard_Real TolCurveOnSurf (const Standard_Integer Index) const = 0;
  Standard_EXPORT virtual ~AppBlend_Approx();




protected:





private:





};







#endif // _AppBlend_Approx_HeaderFile
