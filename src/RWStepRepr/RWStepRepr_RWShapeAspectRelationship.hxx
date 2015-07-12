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

#ifndef _RWStepRepr_RWShapeAspectRelationship_HeaderFile
#define _RWStepRepr_RWShapeAspectRelationship_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Integer.hxx>
class StepData_StepReaderData;
class Interface_Check;
class StepRepr_ShapeAspectRelationship;
class StepData_StepWriter;
class Interface_EntityIterator;


//! Read & Write tool for ShapeAspectRelationship
class RWStepRepr_RWShapeAspectRelationship 
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! Empty constructor
  Standard_EXPORT RWStepRepr_RWShapeAspectRelationship();
  
  //! Reads ShapeAspectRelationship
  Standard_EXPORT void ReadStep (const Handle(StepData_StepReaderData)& data, const Standard_Integer num, Handle(Interface_Check)& ach, const Handle(StepRepr_ShapeAspectRelationship)& ent) const;
  
  //! Writes ShapeAspectRelationship
  Standard_EXPORT void WriteStep (StepData_StepWriter& SW, const Handle(StepRepr_ShapeAspectRelationship)& ent) const;
  
  //! Fills data for graph (shared items)
  Standard_EXPORT void Share (const Handle(StepRepr_ShapeAspectRelationship)& ent, Interface_EntityIterator& iter) const;




protected:





private:





};







#endif // _RWStepRepr_RWShapeAspectRelationship_HeaderFile
