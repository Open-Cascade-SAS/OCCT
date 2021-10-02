// Created on: 1996-11-25
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

#ifndef _BRepBlend_AppSurface_HeaderFile
#define _BRepBlend_AppSurface_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Approx_SweepApproximation.hxx>
#include <AppBlend_Approx.hxx>
#include <Standard_Real.hxx>
#include <GeomAbs_Shape.hxx>
#include <Standard_Integer.hxx>
#include <Standard_Boolean.hxx>
#include <TColgp_Array2OfPnt.hxx>
#include <TColStd_Array2OfReal.hxx>
#include <TColStd_Array1OfReal.hxx>
#include <TColStd_Array1OfInteger.hxx>
#include <TColgp_Array1OfPnt2d.hxx>
#include <Standard_OStream.hxx>
class Approx_SweepFunction;


//! Used to Approximate the blending surfaces.
class BRepBlend_AppSurface  : public AppBlend_Approx
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! Approximation     of   the   new  Surface  (and
  //! eventually the  2d    Curves   on the   support
  //! surfaces).
  //! Normally     the  2d    curve are
  //! approximated  with an  tolerance   given  by   the
  //! resolution on   support surfaces,  but  if this
  //! tolerance is too large Tol2d  is used.
  Standard_EXPORT BRepBlend_AppSurface(const Handle(Approx_SweepFunction)& Funct, const Standard_Real First, const Standard_Real Last, const Standard_Real Tol3d, const Standard_Real Tol2d, const Standard_Real TolAngular, const GeomAbs_Shape Continuity = GeomAbs_C0, const Standard_Integer Degmax = 11, const Standard_Integer Segmax = 50);
  
    Standard_Boolean IsDone() const;
  
  Standard_EXPORT void SurfShape (Standard_Integer& UDegree, Standard_Integer& VDegree, Standard_Integer& NbUPoles, Standard_Integer& NbVPoles, Standard_Integer& NbUKnots, Standard_Integer& NbVKnots) const;
  
  Standard_EXPORT void Surface (TColgp_Array2OfPnt& TPoles, TColStd_Array2OfReal& TWeights, TColStd_Array1OfReal& TUKnots, TColStd_Array1OfReal& TVKnots, TColStd_Array1OfInteger& TUMults, TColStd_Array1OfInteger& TVMults) const;
  
    Standard_Integer UDegree() const;
  
    Standard_Integer VDegree() const;
  
    const TColgp_Array2OfPnt& SurfPoles() const;
  
    const TColStd_Array2OfReal& SurfWeights() const;
  
    const TColStd_Array1OfReal& SurfUKnots() const;
  
    const TColStd_Array1OfReal& SurfVKnots() const;
  
    const TColStd_Array1OfInteger& SurfUMults() const;
  
    const TColStd_Array1OfInteger& SurfVMults() const;
  
  //! returns the maximum error in the surface approximation.
  Standard_EXPORT Standard_Real MaxErrorOnSurf() const;
  
    Standard_Integer NbCurves2d() const;
  
  Standard_EXPORT void Curves2dShape (Standard_Integer& Degree, Standard_Integer& NbPoles, Standard_Integer& NbKnots) const;
  
  Standard_EXPORT void Curve2d (const Standard_Integer Index, TColgp_Array1OfPnt2d& TPoles, TColStd_Array1OfReal& TKnots, TColStd_Array1OfInteger& TMults) const;
  
    Standard_Integer Curves2dDegree() const;
  
    const TColgp_Array1OfPnt2d& Curve2dPoles (const Standard_Integer Index) const;
  
    const TColStd_Array1OfReal& Curves2dKnots() const;
  
    const TColStd_Array1OfInteger& Curves2dMults() const;
  
  Standard_EXPORT void TolReached (Standard_Real& Tol3d, Standard_Real& Tol2d) const;
  
  //! returns the maximum error in the <Index> 2d curve approximation.
  Standard_EXPORT Standard_Real Max2dError (const Standard_Integer Index) const;
  
  Standard_EXPORT Standard_Real TolCurveOnSurf (const Standard_Integer Index) const;
  
  //! display information on approximation.
  Standard_EXPORT void Dump (Standard_OStream& o) const;




protected:





private:



  Approx_SweepApproximation approx;


};


#include <BRepBlend_AppSurface.lxx>





#endif // _BRepBlend_AppSurface_HeaderFile
