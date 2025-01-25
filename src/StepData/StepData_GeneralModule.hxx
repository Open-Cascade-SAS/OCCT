// Created on: 1993-02-02
// Created by: Christian CAILLET
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

#ifndef _StepData_GeneralModule_HeaderFile
#define _StepData_GeneralModule_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Interface_GeneralModule.hxx>
#include <Standard_Integer.hxx>
class Standard_Transient;
class Interface_EntityIterator;
class Interface_ShareTool;
class Interface_Check;
class Interface_CopyTool;

class StepData_GeneralModule;
DEFINE_STANDARD_HANDLE(StepData_GeneralModule, Interface_GeneralModule)

//! Specific features for General Services adapted to STEP
class StepData_GeneralModule : public Interface_GeneralModule
{

public:
  //! Specific filling of the list of Entities shared by an Entity
  //! <ent>. Can use the internal utility method Share, below
  Standard_EXPORT virtual void FillSharedCase(const Standard_Integer            casenum,
                                              const Handle(Standard_Transient)& ent,
                                              Interface_EntityIterator&         iter) const
    Standard_OVERRIDE = 0;

  //! Specific Checking of an Entity <ent>
  Standard_EXPORT virtual void CheckCase(const Standard_Integer            casenum,
                                         const Handle(Standard_Transient)& ent,
                                         const Interface_ShareTool&        shares,
                                         Handle(Interface_Check)& ach) const Standard_OVERRIDE = 0;

  //! Specific Copy ("Deep") from <entfrom> to <entto> (same type)
  //! by using a TransferControl which provides its working Map.
  //! Use method Transferred from TransferControl to work
  //! Specific Copying of Implied References
  //! A Default is provided which does nothing (must current case !)
  //! Already copied references (by CopyFrom) must remain unchanged
  //! Use method Search from TransferControl to work
  Standard_EXPORT virtual void CopyCase(const Standard_Integer            casenum,
                                        const Handle(Standard_Transient)& entfrom,
                                        const Handle(Standard_Transient)& entto,
                                        Interface_CopyTool& TC) const Standard_OVERRIDE = 0;

  DEFINE_STANDARD_RTTIEXT(StepData_GeneralModule, Interface_GeneralModule)

protected:
private:
};

#endif // _StepData_GeneralModule_HeaderFile
