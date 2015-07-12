// Created on: 1995-06-02
// Created by: Xavier BENVENISTE
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

#ifndef _Approx_SameParameter_HeaderFile
#define _Approx_SameParameter_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Boolean.hxx>
#include <Standard_Real.hxx>
class Geom2d_BSplineCurve;
class Adaptor2d_HCurve2d;
class Adaptor3d_HCurve;
class Adaptor3d_HSurface;
class Standard_OutOfRange;
class Standard_ConstructionError;
class Geom_Curve;
class Geom2d_Curve;
class Geom_Surface;


//! Approximation of a  PCurve  on a surface to  make  its
//! parameter be the same that the parameter of a given 3d
//! reference curve.
class Approx_SameParameter 
{
public:

  DEFINE_STANDARD_ALLOC

  

  //! Warning: the C3D and C2D must have the same parametric domain.
  Standard_EXPORT Approx_SameParameter(const Handle(Geom_Curve)& C3D, const Handle(Geom2d_Curve)& C2D, const Handle(Geom_Surface)& S, const Standard_Real Tol);
  
  Standard_EXPORT Approx_SameParameter(const Handle(Adaptor3d_HCurve)& C3D, const Handle(Geom2d_Curve)& C2D, const Handle(Adaptor3d_HSurface)& S, const Standard_Real Tol);
  

  //! Warning: the C3D and C2D must have the same parametric domain.
  Standard_EXPORT Approx_SameParameter(const Handle(Adaptor3d_HCurve)& C3D, const Handle(Adaptor2d_HCurve2d)& C2D, const Handle(Adaptor3d_HSurface)& S, const Standard_Real Tol);
  
    Standard_Boolean IsDone() const;
  
    Standard_Real TolReached() const;
  
  //! Tells whether the original data  had already the  same
  //! parameter up to  the tolerance :  in that case nothing
  //! is done.
    Standard_Boolean IsSameParameter() const;
  
  //! Returns the 2D  curve that has  the same parameter  as
  //! the  3D curve once evaluated on  the surface up to the
  //! specified tolerance
    Handle(Geom2d_BSplineCurve) Curve2d() const;




protected:





private:

  
  //! Compute the Pcurve (internal use only).
  Standard_EXPORT void Build (const Standard_Real Tol);


  Standard_Boolean mySameParameter;
  Standard_Boolean myDone;
  Standard_Real myTolReached;
  Handle(Geom2d_BSplineCurve) myCurve2d;
  Handle(Adaptor2d_HCurve2d) myHCurve2d;
  Handle(Adaptor3d_HCurve) myC3d;
  Handle(Adaptor3d_HSurface) mySurf;


};


#include <Approx_SameParameter.lxx>





#endif // _Approx_SameParameter_HeaderFile
