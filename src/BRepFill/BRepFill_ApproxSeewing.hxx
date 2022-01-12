// Created on: 1995-09-21
// Created by: Bruno DUMORTIER
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

#ifndef _BRepFill_ApproxSeewing_HeaderFile
#define _BRepFill_ApproxSeewing_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <BRepFill_MultiLine.hxx>
class Geom_Curve;
class Geom2d_Curve;


//! Evaluate the 3dCurve and the PCurves described in a MultiLine from BRepFill.
//! The parametrization of those curves is not imposed by the Bissectrice.
//! The parametrization is given approximately by the abscissa of the curve3d.
class BRepFill_ApproxSeewing
{
public:

  DEFINE_STANDARD_ALLOC

  Standard_EXPORT BRepFill_ApproxSeewing();
  
  Standard_EXPORT BRepFill_ApproxSeewing(const BRepFill_MultiLine& ML);
  
  Standard_EXPORT void Perform (const BRepFill_MultiLine& ML);
  
  Standard_EXPORT Standard_Boolean IsDone() const;
  
  //! returns the approximation of the 3d Curve
  Standard_EXPORT const Handle(Geom_Curve)& Curve() const;
  
  //! returns the  approximation  of the  PCurve  on the
  //! first face of the MultiLine
  Standard_EXPORT const Handle(Geom2d_Curve)& CurveOnF1() const;
  
  //! returns the  approximation  of the  PCurve  on the
  //! first face of the MultiLine
  Standard_EXPORT const Handle(Geom2d_Curve)& CurveOnF2() const;

private:

  BRepFill_MultiLine myML;
  Standard_Boolean myIsDone;
  Handle(Geom_Curve) myCurve;
  Handle(Geom2d_Curve) myPCurve1;
  Handle(Geom2d_Curve) myPCurve2;

};

#endif // _BRepFill_ApproxSeewing_HeaderFile
