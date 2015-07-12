// Created on: 2003-01-22
// Created by: data exchange team
// Copyright (c) 2003-2014 OPEN CASCADE SAS
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

#ifndef _RWStepFEA_RWFeaSurfaceSectionGeometricRelationship_HeaderFile
#define _RWStepFEA_RWFeaSurfaceSectionGeometricRelationship_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Integer.hxx>
class StepData_StepReaderData;
class Interface_Check;
class StepFEA_FeaSurfaceSectionGeometricRelationship;
class StepData_StepWriter;
class Interface_EntityIterator;


//! Read & Write tool for FeaSurfaceSectionGeometricRelationship
class RWStepFEA_RWFeaSurfaceSectionGeometricRelationship 
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! Empty constructor
  Standard_EXPORT RWStepFEA_RWFeaSurfaceSectionGeometricRelationship();
  
  //! Reads FeaSurfaceSectionGeometricRelationship
  Standard_EXPORT void ReadStep (const Handle(StepData_StepReaderData)& data, const Standard_Integer num, Handle(Interface_Check)& ach, const Handle(StepFEA_FeaSurfaceSectionGeometricRelationship)& ent) const;
  
  //! Writes FeaSurfaceSectionGeometricRelationship
  Standard_EXPORT void WriteStep (StepData_StepWriter& SW, const Handle(StepFEA_FeaSurfaceSectionGeometricRelationship)& ent) const;
  
  //! Fills data for graph (shared items)
  Standard_EXPORT void Share (const Handle(StepFEA_FeaSurfaceSectionGeometricRelationship)& ent, Interface_EntityIterator& iter) const;




protected:





private:





};







#endif // _RWStepFEA_RWFeaSurfaceSectionGeometricRelationship_HeaderFile
