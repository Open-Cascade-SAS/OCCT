// Created on: 1993-03-26
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

#ifndef _StepData_DefaultGeneral_HeaderFile
#define _StepData_DefaultGeneral_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <StepData_GeneralModule.hxx>
#include <Standard_Integer.hxx>
class Standard_Transient;
class Interface_EntityIterator;
class Interface_ShareTool;
class Interface_Check;
class Interface_CopyTool;

class StepData_DefaultGeneral;
DEFINE_STANDARD_HANDLE(StepData_DefaultGeneral, StepData_GeneralModule)

//! DefaultGeneral defines a GeneralModule which processes
//! Unknown Entity from StepData  only
class StepData_DefaultGeneral : public StepData_GeneralModule
{

public:
  //! Creates a Default General Module
  Standard_EXPORT StepData_DefaultGeneral();

  //! Specific filling of the list of Entities shared by an Entity
  //! <ent>, which is an UnknownEntity from StepData.
  Standard_EXPORT void FillSharedCase(const Standard_Integer            casenum,
                                      const Handle(Standard_Transient)& ent,
                                      Interface_EntityIterator& iter) const Standard_OVERRIDE;

  //! Specific Checking of an Entity <ent>
  Standard_EXPORT void CheckCase(const Standard_Integer            casenum,
                                 const Handle(Standard_Transient)& ent,
                                 const Interface_ShareTool&        shares,
                                 Handle(Interface_Check)&          ach) const Standard_OVERRIDE;

  //! Specific creation of a new void entity
  Standard_EXPORT Standard_Boolean
    NewVoid(const Standard_Integer CN, Handle(Standard_Transient)& entto) const Standard_OVERRIDE;

  //! Specific Copy ("Deep") from <entfrom> to <entto> (same type)
  //! by using a CopyTool which provides its working Map.
  //! Use method Transferred from TransferControl to work
  Standard_EXPORT void CopyCase(const Standard_Integer            casenum,
                                const Handle(Standard_Transient)& entfrom,
                                const Handle(Standard_Transient)& entto,
                                Interface_CopyTool&               TC) const Standard_OVERRIDE;

  DEFINE_STANDARD_RTTIEXT(StepData_DefaultGeneral, StepData_GeneralModule)

protected:
private:
};

#endif // _StepData_DefaultGeneral_HeaderFile
