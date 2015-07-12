// Created on: 2000-04-18
// Created by: Andrey BETENEV
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

#ifndef _RWStepShape_RWDimensionalLocationWithPath_HeaderFile
#define _RWStepShape_RWDimensionalLocationWithPath_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Integer.hxx>
class StepData_StepReaderData;
class Interface_Check;
class StepShape_DimensionalLocationWithPath;
class StepData_StepWriter;
class Interface_EntityIterator;


//! Read & Write tool for DimensionalLocationWithPath
class RWStepShape_RWDimensionalLocationWithPath 
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! Empty constructor
  Standard_EXPORT RWStepShape_RWDimensionalLocationWithPath();
  
  //! Reads DimensionalLocationWithPath
  Standard_EXPORT void ReadStep (const Handle(StepData_StepReaderData)& data, const Standard_Integer num, Handle(Interface_Check)& ach, const Handle(StepShape_DimensionalLocationWithPath)& ent) const;
  
  //! Writes DimensionalLocationWithPath
  Standard_EXPORT void WriteStep (StepData_StepWriter& SW, const Handle(StepShape_DimensionalLocationWithPath)& ent) const;
  
  //! Fills data for graph (shared items)
  Standard_EXPORT void Share (const Handle(StepShape_DimensionalLocationWithPath)& ent, Interface_EntityIterator& iter) const;




protected:





private:





};







#endif // _RWStepShape_RWDimensionalLocationWithPath_HeaderFile
