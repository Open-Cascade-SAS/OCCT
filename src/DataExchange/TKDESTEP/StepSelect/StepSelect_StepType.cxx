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

#include <Interface_InterfaceError.hxx>
#include <Interface_InterfaceModel.hxx>
#include <Interface_Macros.hxx>
#include <Standard_Transient.hxx>
#include <Standard_Type.hxx>
#include <StepData_Protocol.hxx>
#include <StepData_ReadWriteModule.hxx>
#include <StepData_UndefinedEntity.hxx>
#include <StepSelect_StepType.hxx>
#include <TColStd_SequenceOfAsciiString.hxx>

IMPLEMENT_STANDARD_RTTIEXT(StepSelect_StepType, IFSelect_Signature)

StepSelect_StepType::StepSelect_StepType()
    : IFSelect_Signature("Step Type")
{
}

void StepSelect_StepType::SetProtocol(const Handle(Interface_Protocol)& proto)
{
  DeclareAndCast(StepData_Protocol, newproto, proto);
  if (newproto.IsNull())
    throw Interface_InterfaceError("StepSelect_StepType");
  theproto = newproto;
  thelib.Clear();
  thelib.AddProtocol(theproto);
  thename.Clear();
  thename.AssignCat("Step Type (Schema ");
  thename.AssignCat(theproto->SchemaName(nullptr));
  thename.AssignCat(")");
}

Standard_CString StepSelect_StepType::Value(const Handle(Standard_Transient)&       ent,
                                            const Handle(Interface_InterfaceModel)& model) const
{
  std::lock_guard<std::mutex> aLock(myMutex);

  Handle(StepData_ReadWriteModule) aModule;
  Standard_Integer                 aCN;
  if (!thelib.Select(ent, aModule, aCN))
  {
    // Build error message for unrecognized entity
    theLastValue = "..NOT FROM SCHEMA ";
    theLastValue += theproto->SchemaName(model);
    theLastValue += "..";
    return theLastValue.ToCString();
  }

  // Handle simple (non-complex) type - return direct reference from module
  if (!aModule->IsComplex(aCN))
    return aModule->StepType(aCN).data();

  // Handle complex type from module
  TColStd_SequenceOfAsciiString aList;
  if (aModule->ComplexType(aCN, aList))
  {
    Standard_Integer aNb = aList.Length();
    if (aNb == 0)
    {
      theLastValue = "(..COMPLEX TYPE..)";
    }
    else
    {
      theLastValue = "(";
      for (Standard_Integer i = 1; i <= aNb; i++)
      {
        if (i > 1)
          theLastValue += ",";
        theLastValue += aList.Value(i);
      }
      theLastValue += ")";
    }
    return theLastValue.ToCString();
  }

  // Fallback: check for undefined entity
  DeclareAndCast(StepData_UndefinedEntity, anUnd, ent);
  if (anUnd.IsNull())
  {
    theLastValue.Clear();
    return theLastValue.ToCString();
  }

  if (!anUnd->IsComplex())
    return anUnd->StepType(); // Direct return from entity's internal storage

  // Build complex type from undefined entity
  theLastValue             = "(";
  Standard_Boolean isFirst = Standard_True;
  while (!anUnd.IsNull())
  {
    if (!isFirst)
      theLastValue += ",";
    theLastValue += anUnd->StepType();
    anUnd   = anUnd->Next();
    isFirst = Standard_False;
  }
  theLastValue += ")";

  return theLastValue.ToCString();
}
