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

#ifndef _GeomToStep_MakeToroidalSurface_HeaderFile
#define _GeomToStep_MakeToroidalSurface_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <GeomToStep_Root.hxx>

class StepData_Factors;
class StepGeom_ToroidalSurface;
class Geom_ToroidalSurface;


//! This class implements the mapping between class
//! ToroidalSurface from Geom and the class
//! ToroidalSurface from StepGeom which describes a
//! toroidal_surface from Prostep
class GeomToStep_MakeToroidalSurface  : public GeomToStep_Root
{
public:

  DEFINE_STANDARD_ALLOC

  
  Standard_EXPORT GeomToStep_MakeToroidalSurface(const Handle(Geom_ToroidalSurface)& TorSurf,
                                                 const StepData_Factors& theLocalFactors);
  
  Standard_EXPORT const Handle(StepGeom_ToroidalSurface)& Value() const;




protected:





private:



  Handle(StepGeom_ToroidalSurface) theToroidalSurface;


};







#endif // _GeomToStep_MakeToroidalSurface_HeaderFile
