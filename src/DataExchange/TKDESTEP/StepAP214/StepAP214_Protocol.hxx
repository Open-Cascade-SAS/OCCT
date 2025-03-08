// Created on: 1995-12-01
// Created by: EXPRESS->CDL V0.2 Translator
// Copyright (c) 1995-1999 Matra Datavision
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

#ifndef _StepAP214_Protocol_HeaderFile
#define _StepAP214_Protocol_HeaderFile

#include <Standard.hxx>

#include <StepData_Protocol.hxx>
#include <Standard_Integer.hxx>
#include <Standard_Type.hxx>
#include <Standard_CString.hxx>
class Interface_Protocol;

class StepAP214_Protocol;
DEFINE_STANDARD_HANDLE(StepAP214_Protocol, StepData_Protocol)

//! Protocol for StepAP214 Entities
//! It requires StepAP214 as a Resource
class StepAP214_Protocol : public StepData_Protocol
{

public:
  Standard_EXPORT StepAP214_Protocol();

  //! Returns a Case Number for each of the StepAP214 Entities
  Standard_EXPORT virtual Standard_Integer TypeNumber(const Handle(Standard_Type)& atype) const
    Standard_OVERRIDE;

  Standard_EXPORT virtual Standard_CString SchemaName(
    const Handle(Interface_InterfaceModel)& theModel) const Standard_OVERRIDE;

  //! Returns count of Protocol used as Resources (level one)
  Standard_EXPORT virtual Standard_Integer NbResources() const Standard_OVERRIDE;

  //! Returns a Resource, given its rank (between 1 and NbResources)
  Standard_EXPORT virtual Handle(Interface_Protocol) Resource(const Standard_Integer num) const
    Standard_OVERRIDE;

  DEFINE_STANDARD_RTTIEXT(StepAP214_Protocol, StepData_Protocol)

protected:
private:
};

#endif // _StepAP214_Protocol_HeaderFile
