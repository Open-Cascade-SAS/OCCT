// Created on: 1993-06-22
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

#ifndef _StepToGeom_MakeBoundedSurface_HeaderFile
#define _StepToGeom_MakeBoundedSurface_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Boolean.hxx>
class StepGeom_BoundedSurface;
class Geom_BoundedSurface;


//! This class implements the mapping between classes
//! BoundedSurface from
//! StepGeom which describes a BoundedSurface from
//! prostep and the class BoundedSurface from Geom.
//! As BoundedSurface is an abstract BoundedSurface this class
//! is an access to the sub-class required.
class StepToGeom_MakeBoundedSurface 
{
public:

  DEFINE_STANDARD_ALLOC

  
  Standard_EXPORT static Standard_Boolean Convert (const Handle(StepGeom_BoundedSurface)& SS, Handle(Geom_BoundedSurface)& CS);




protected:





private:





};







#endif // _StepToGeom_MakeBoundedSurface_HeaderFile
