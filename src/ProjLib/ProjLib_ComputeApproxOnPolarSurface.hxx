// Created on: 1994-10-07
// Created by: Bruno DUMORTIER
// Copyright (c) 1994-1999 Matra Datavision
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

#ifndef _ProjLib_ComputeApproxOnPolarSurface_HeaderFile
#define _ProjLib_ComputeApproxOnPolarSurface_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Boolean.hxx>
#include <Standard_Real.hxx>
class Geom2d_BSplineCurve;
class Geom2d_Curve;
class Adaptor3d_HCurve;
class Adaptor3d_HSurface;
class Adaptor2d_HCurve2d;


//! Approximate the  projection  of a  3d curve  on an
//! polar  surface  and  stores the result  in  Approx.
//! The result is a  2d curve.  The evaluation of  the
//! current  point of the  2d  curve is done with  the
//! evaluation of the extrema  P3d - Surface.
class ProjLib_ComputeApproxOnPolarSurface 
{
public:

  DEFINE_STANDARD_ALLOC

  
  Standard_EXPORT ProjLib_ComputeApproxOnPolarSurface();
  
  Standard_EXPORT ProjLib_ComputeApproxOnPolarSurface(const Handle(Adaptor3d_HCurve)& C, const Handle(Adaptor3d_HSurface)& S, const Standard_Real Tol = 1.0e-4);
  
  Standard_EXPORT ProjLib_ComputeApproxOnPolarSurface(const Handle(Adaptor2d_HCurve2d)& InitCurve2d, const Handle(Adaptor3d_HCurve)& C, const Handle(Adaptor3d_HSurface)& S, const Standard_Real Tol);
  
  Standard_EXPORT ProjLib_ComputeApproxOnPolarSurface(const Handle(Adaptor2d_HCurve2d)& InitCurve2d, const Handle(Adaptor2d_HCurve2d)& InitCurve2dBis, const Handle(Adaptor3d_HCurve)& C, const Handle(Adaptor3d_HSurface)& S, const Standard_Real Tol);
  
  Standard_EXPORT Handle(Geom2d_BSplineCurve) Perform (const Handle(Adaptor2d_HCurve2d)& InitCurve2d, const Handle(Adaptor3d_HCurve)& C, const Handle(Adaptor3d_HSurface)& S);
  
  Standard_EXPORT Handle(Adaptor2d_HCurve2d) BuildInitialCurve2d (const Handle(Adaptor3d_HCurve)& Curve, const Handle(Adaptor3d_HSurface)& S);
  
  Standard_EXPORT Handle(Geom2d_BSplineCurve) ProjectUsingInitialCurve2d (const Handle(Adaptor3d_HCurve)& Curve, const Handle(Adaptor3d_HSurface)& S, const Handle(Adaptor2d_HCurve2d)& InitCurve2d);
  
  Standard_EXPORT Handle(Geom2d_BSplineCurve) BSpline() const;
  
  Standard_EXPORT Handle(Geom2d_Curve) Curve2d() const;
  
  Standard_EXPORT Standard_Boolean IsDone() const;




protected:





private:



  Standard_Boolean myProjIsDone;
  Standard_Real myTolerance;
  Handle(Geom2d_BSplineCurve) myBSpline;
  Handle(Geom2d_Curve) my2ndCurve;


};







#endif // _ProjLib_ComputeApproxOnPolarSurface_HeaderFile
