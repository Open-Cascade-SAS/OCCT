// Created on: 1993-06-14
// Created by: Martine LANGLOIS
// Copyright (c) 1993-1999 Matra Datavision
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

#ifndef _GeomToStep_MakeSphericalSurface_HeaderFile
#define _GeomToStep_MakeSphericalSurface_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <GeomToStep_Root.hxx>

class StepGeom_SphericalSurface;
class Geom_SphericalSurface;

//! This class implements the mapping between class
//! SphericalSurface from Geom and the class
//! SphericalSurface from StepGeom which describes a
//! spherical_surface from Prostep
class GeomToStep_MakeSphericalSurface : public GeomToStep_Root
{
public:
  DEFINE_STANDARD_ALLOC

  Standard_EXPORT GeomToStep_MakeSphericalSurface(
    const Handle(Geom_SphericalSurface)& CSurf,
    const StepData_Factors&              theLocalFactors = StepData_Factors());

  Standard_EXPORT const Handle(StepGeom_SphericalSurface)& Value() const;

protected:
private:
  Handle(StepGeom_SphericalSurface) theSphericalSurface;
};

#endif // _GeomToStep_MakeSphericalSurface_HeaderFile
