// Created on: 1996-01-25
// Created by: Frederic MAUPAS
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

#ifndef _GeomToStep_MakeRectangularTrimmedSurface_HeaderFile
#define _GeomToStep_MakeRectangularTrimmedSurface_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <GeomToStep_Root.hxx>

class StepData_Factors;
class StepGeom_RectangularTrimmedSurface;
class Geom_RectangularTrimmedSurface;


//! This class implements the mapping between class
//! RectangularTrimmedSurface from Geom and the class
//! RectangularTrimmedSurface from
//! StepGeom which describes a
//! rectangular_trimmed_surface from ISO-IS 10303-42
class GeomToStep_MakeRectangularTrimmedSurface  : public GeomToStep_Root
{
public:

  DEFINE_STANDARD_ALLOC

  
  Standard_EXPORT GeomToStep_MakeRectangularTrimmedSurface(const Handle(Geom_RectangularTrimmedSurface)& RTSurf,
                                                           const StepData_Factors& theLocalFactors);
  
  Standard_EXPORT const Handle(StepGeom_RectangularTrimmedSurface)& Value() const;




protected:





private:



  Handle(StepGeom_RectangularTrimmedSurface) theRectangularTrimmedSurface;


};







#endif // _GeomToStep_MakeRectangularTrimmedSurface_HeaderFile
