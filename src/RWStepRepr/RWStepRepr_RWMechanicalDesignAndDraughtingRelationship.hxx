// Copyright (c) 2024 OPEN CASCADE SAS
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

#ifndef _RWStepRepr_RWMechanicalDesignAndDraughtingRelationship_HeaderFile
#define _RWStepRepr_RWMechanicalDesignAndDraughtingRelationship_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Integer.hxx>
class StepData_StepReaderData;
class Interface_Check;
class StepRepr_MechanicalDesignAndDraughtingRelationship;
class StepData_StepWriter;
class Interface_EntityIterator;


//! Read & Write Module for MechanicalDesignAndDraughtingRelationship
class RWStepRepr_RWMechanicalDesignAndDraughtingRelationship
{
public:

  DEFINE_STANDARD_ALLOC

  Standard_EXPORT RWStepRepr_RWMechanicalDesignAndDraughtingRelationship();
  
  Standard_EXPORT void ReadStep(const Handle(StepData_StepReaderData)& theData,
                                const Standard_Integer theNum,
                                Handle(Interface_Check)& theAch,
                                const Handle(StepRepr_MechanicalDesignAndDraughtingRelationship)& theEnt) const;
  
  Standard_EXPORT void WriteStep(StepData_StepWriter& theSW,
                                 const Handle(StepRepr_MechanicalDesignAndDraughtingRelationship)& theEnt) const;
  
  Standard_EXPORT void Share(const Handle(StepRepr_MechanicalDesignAndDraughtingRelationship)& theEnt,
                             Interface_EntityIterator& theIter) const;

};
#endif // _RWStepRepr_RWMechanicalDesignAndDraughtingRelationship_HeaderFile
