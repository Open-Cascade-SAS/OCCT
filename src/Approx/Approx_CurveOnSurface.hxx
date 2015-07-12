// Created on: 1997-09-30
// Created by: Roman BORISOV
// Copyright (c) 1997-1999 Matra Datavision
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

#ifndef _Approx_CurveOnSurface_HeaderFile
#define _Approx_CurveOnSurface_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Boolean.hxx>
#include <Standard_Real.hxx>
#include <GeomAbs_Shape.hxx>
#include <Standard_Integer.hxx>
class Geom2d_BSplineCurve;
class Geom_BSplineCurve;
class Standard_OutOfRange;
class Standard_ConstructionError;
class Adaptor2d_HCurve2d;
class Adaptor3d_HSurface;


//! Approximation of   curve on surface
class Approx_CurveOnSurface 
{
public:

  DEFINE_STANDARD_ALLOC

  
  Standard_EXPORT Approx_CurveOnSurface(const Handle(Adaptor2d_HCurve2d)& C2D, const Handle(Adaptor3d_HSurface)& Surf, const Standard_Real First, const Standard_Real Last, const Standard_Real Tol, const GeomAbs_Shape Continuity, const Standard_Integer MaxDegree, const Standard_Integer MaxSegments, const Standard_Boolean Only3d = Standard_False, const Standard_Boolean Only2d = Standard_False);
  
  Standard_EXPORT Standard_Boolean IsDone() const;
  
  Standard_EXPORT Standard_Boolean HasResult() const;
  
  Standard_EXPORT Handle(Geom_BSplineCurve) Curve3d() const;
  
  Standard_EXPORT Standard_Real MaxError3d() const;
  
  Standard_EXPORT Handle(Geom2d_BSplineCurve) Curve2d() const;
  
  Standard_EXPORT Standard_Real MaxError2dU() const;
  
  //! returns the maximum errors relativly to the  U component or the V component of the
  //! 2d Curve
  Standard_EXPORT Standard_Real MaxError2dV() const;




protected:





private:



  Handle(Geom2d_BSplineCurve) myCurve2d;
  Handle(Geom_BSplineCurve) myCurve3d;
  Standard_Boolean myIsDone;
  Standard_Boolean myHasResult;
  Standard_Real myError3d;
  Standard_Real myError2dU;
  Standard_Real myError2dV;


};







#endif // _Approx_CurveOnSurface_HeaderFile
