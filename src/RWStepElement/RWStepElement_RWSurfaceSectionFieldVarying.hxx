// Created on: 2002-12-12
// Created by: data exchange team
// Copyright (c) 2002-2014 OPEN CASCADE SAS
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

#ifndef _RWStepElement_RWSurfaceSectionFieldVarying_HeaderFile
#define _RWStepElement_RWSurfaceSectionFieldVarying_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Integer.hxx>
class StepData_StepReaderData;
class Interface_Check;
class StepElement_SurfaceSectionFieldVarying;
class StepData_StepWriter;
class Interface_EntityIterator;


//! Read & Write tool for SurfaceSectionFieldVarying
class RWStepElement_RWSurfaceSectionFieldVarying 
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! Empty constructor
  Standard_EXPORT RWStepElement_RWSurfaceSectionFieldVarying();
  
  //! Reads SurfaceSectionFieldVarying
  Standard_EXPORT void ReadStep (const Handle(StepData_StepReaderData)& data, const Standard_Integer num, Handle(Interface_Check)& ach, const Handle(StepElement_SurfaceSectionFieldVarying)& ent) const;
  
  //! Writes SurfaceSectionFieldVarying
  Standard_EXPORT void WriteStep (StepData_StepWriter& SW, const Handle(StepElement_SurfaceSectionFieldVarying)& ent) const;
  
  //! Fills data for graph (shared items)
  Standard_EXPORT void Share (const Handle(StepElement_SurfaceSectionFieldVarying)& ent, Interface_EntityIterator& iter) const;




protected:





private:





};







#endif // _RWStepElement_RWSurfaceSectionFieldVarying_HeaderFile
