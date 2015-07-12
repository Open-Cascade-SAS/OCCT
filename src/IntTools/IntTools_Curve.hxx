// Created on: 2000-11-23
// Created by: Michael KLOKOV
// Copyright (c) 2000-2014 OPEN CASCADE SAS
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

#ifndef _IntTools_Curve_HeaderFile
#define _IntTools_Curve_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Boolean.hxx>
#include <Standard_Real.hxx>
#include <GeomAbs_CurveType.hxx>
class Geom_Curve;
class Geom2d_Curve;
class gp_Pnt;


//! class is a container of
//! one 3d curve
//! two 2d curves
class IntTools_Curve 
{
public:

  DEFINE_STANDARD_ALLOC

  

  //! Empty constructor
  Standard_EXPORT IntTools_Curve();
  

  //! Initializes me by a 3d curve
  //! and two 2d curves
  Standard_EXPORT IntTools_Curve(const Handle(Geom_Curve)& Curve3d, const Handle(Geom2d_Curve)& FirstCurve2d, const Handle(Geom2d_Curve)& SecondCurve2d);
  

  //! Modifier
  Standard_EXPORT void SetCurves (const Handle(Geom_Curve)& Curve3d, const Handle(Geom2d_Curve)& FirstCurve2d, const Handle(Geom2d_Curve)& SecondCurve2d);
  

  //! Modifier
    void SetCurve (const Handle(Geom_Curve)& Curve3d);
  

  //! Modifier
    void SetFirstCurve2d (const Handle(Geom2d_Curve)& FirstCurve2d);
  

  //! Modifier
    void SetSecondCurve2d (const Handle(Geom2d_Curve)& SecondCurve2d);
  

  //! Selector
    const Handle(Geom_Curve)& Curve() const;
  

  //! Selector
    const Handle(Geom2d_Curve)& FirstCurve2d() const;
  

  //! Selector
    const Handle(Geom2d_Curve)& SecondCurve2d() const;
  

  //! Returns true if 3d curve is BoundedCurve from Geom
  Standard_EXPORT Standard_Boolean HasBounds() const;
  

  //! Returns boundary parameters
  //! and corresponded 3d point.
  //!
  //! Warning:
  //! If HasBounds returns false
  //! the returned parameters are equal
  //! to zero.
  Standard_EXPORT void Bounds (Standard_Real& aT1, Standard_Real& aT2, gp_Pnt& aP1, gp_Pnt& aP2) const;
  

  //! Computes 3d point corresponded to parameter aT1
  //! Returns true if given parameter aT1
  //! is inside the boundaries of the curve
  Standard_EXPORT Standard_Boolean D0 (Standard_Real& aT1, gp_Pnt& aP1) const;
  

  //! Returns the type of 3d curve
  Standard_EXPORT GeomAbs_CurveType Type() const;




protected:





private:



  Handle(Geom_Curve) my3dCurve;
  Handle(Geom2d_Curve) my2dCurve1;
  Handle(Geom2d_Curve) my2dCurve2;


};


#include <IntTools_Curve.lxx>





#endif // _IntTools_Curve_HeaderFile
