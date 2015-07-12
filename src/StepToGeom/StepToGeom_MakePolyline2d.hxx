// Created on: 1999-02-15
// Created by: Andrey BETENEV
// Copyright (c) 1999 Matra Datavision
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

#ifndef _StepToGeom_MakePolyline2d_HeaderFile
#define _StepToGeom_MakePolyline2d_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Boolean.hxx>
class StepGeom_Polyline;
class Geom2d_BSplineCurve;


//! Translates Polyline entity into Geom2d_BSpline
//! In case if Polyline has more than 2 points bspline will be C0
class StepToGeom_MakePolyline2d 
{
public:

  DEFINE_STANDARD_ALLOC

  
  Standard_EXPORT static Standard_Boolean Convert (const Handle(StepGeom_Polyline)& SPL, Handle(Geom2d_BSplineCurve)& CC);




protected:





private:





};







#endif // _StepToGeom_MakePolyline2d_HeaderFile
