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

#ifndef _RWStepBasic_RWGeneralPropertyRelationship_HeaderFile
#define _RWStepBasic_RWGeneralPropertyRelationship_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Integer.hxx>
class StepData_StepReaderData;
class Interface_Check;
class StepBasic_GeneralProperty;
class StepData_StepWriter;
class Interface_EntityIterator;

class StepBasic_GeneralPropertyRelationship;

//! Read & Write tool for GeneralPropertyRelationship
class RWStepBasic_RWGeneralPropertyRelationship
{
public:

  DEFINE_STANDARD_ALLOC

  //! Empty constructor
  Standard_EXPORT RWStepBasic_RWGeneralPropertyRelationship();

  //! Reads GeneralPropertyAssociation
  Standard_EXPORT void ReadStep (const Handle(StepData_StepReaderData)& theData,
                                 const Standard_Integer theNum,
                                 Handle(Interface_Check)& theAch,
                                 const Handle(StepBasic_GeneralPropertyRelationship)& theEnt) const;

  //! Writes GeneralPropertyAssociation
  Standard_EXPORT void WriteStep (StepData_StepWriter& theSW,
                                  const Handle(StepBasic_GeneralPropertyRelationship)& theEnt) const;

  //! Fills data for graph (shared items)
  Standard_EXPORT void Share (const Handle(StepBasic_GeneralPropertyRelationship)& theEnt,
                              Interface_EntityIterator& theIter) const;

protected:

private:

};

#endif // _RWStepBasic_RWGeneralPropertyRelationship_HeaderFile
