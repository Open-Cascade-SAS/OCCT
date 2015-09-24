// Created on: 2015-08-11
// Created by: Irina KRYLOVA
// Copyright (c) 2015 OPEN CASCADE SAS
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

#ifndef _RWStepDimTol_RWGeoTolAndGeoTolWthDatRefAndUneqDisGeoTol_HeaderFile
#define _RWStepDimTol_RWGeoTolAndGeoTolWthDatRefAndUneqDisGeoTol_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Integer.hxx>
class StepData_StepReaderData;
class Interface_Check;
class StepDimTol_GeoTolAndGeoTolWthDatRefAndUneqDisGeoTol;
class StepData_StepWriter;
class Interface_EntityIterator;


//! Read & Write Module for GeoTolAndGeoTolWthDatRefAndUneqDisGeoTol
class RWStepDimTol_RWGeoTolAndGeoTolWthDatRefAndUneqDisGeoTol
{
public:

  DEFINE_STANDARD_ALLOC

  
  Standard_EXPORT RWStepDimTol_RWGeoTolAndGeoTolWthDatRefAndUneqDisGeoTol();
  
  Standard_EXPORT void ReadStep (const Handle(StepData_StepReaderData)& data, const Standard_Integer num, Handle(Interface_Check)& ach, const Handle(StepDimTol_GeoTolAndGeoTolWthDatRefAndUneqDisGeoTol)& ent) const;
  
  Standard_EXPORT void WriteStep (StepData_StepWriter& SW, const Handle(StepDimTol_GeoTolAndGeoTolWthDatRefAndUneqDisGeoTol)& ent) const;
  
  Standard_EXPORT void Share (const Handle(StepDimTol_GeoTolAndGeoTolWthDatRefAndUneqDisGeoTol)& ent, Interface_EntityIterator& iter) const;
};
#endif // _RWStepDimTol_RWGeoTolAndGeoTolWthDatRefAndUneqDisGeoTol_HeaderFile
