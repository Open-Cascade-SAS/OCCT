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

#include <Interface_Check.hxx>
#include <Interface_CopyTool.hxx>
#include <Interface_EntityIterator.hxx>
#include <Interface_GeneralLib.hxx>
#include <Interface_Macros.hxx>
#include <Interface_ShareTool.hxx>
#include <Interface_UndefinedContent.hxx>
#include <Standard_Transient.hxx>
#include <Standard_Type.hxx>
#include <StepData.hxx>
#include <StepData_DefaultGeneral.hxx>
#include <StepData_Protocol.hxx>
#include <StepData_UndefinedEntity.hxx>

IMPLEMENT_STANDARD_RTTIEXT(StepData_DefaultGeneral, StepData_GeneralModule)

//  StepData DefaultGeneral recognizes ONLY ONE TYPE: UndefinedEntity
StepData_DefaultGeneral::StepData_DefaultGeneral()
{
  // Register this module globally with the StepData protocol
  Interface_GeneralLib::SetGlobal(this, StepData::Protocol());
}

void StepData_DefaultGeneral::FillSharedCase(const Standard_Integer            casenum,
                                             const Handle(Standard_Transient)& ent,
                                             Interface_EntityIterator&         iter) const
{
  // Fill iterator with shared entities from UndefinedEntity parameters
  if (casenum != 1)
    return; // Only handles case 1 (UndefinedEntity)
  DeclareAndCast(StepData_UndefinedEntity, undf, ent);
  Handle(Interface_UndefinedContent) cont = undf->UndefinedContent();
  Standard_Integer                   nb   = cont->NbParams();
  // Iterate through all parameters looking for entity references
  for (Standard_Integer i = 1; i <= nb; i++)
  {
    Interface_ParamType ptype = cont->ParamType(i);
    if (ptype == Interface_ParamSub)
    {
      // Handle sub-entity parameters recursively
      DeclareAndCast(StepData_UndefinedEntity, subent, cont->ParamEntity(i));
      FillSharedCase(casenum, cont->ParamEntity(i), iter);
    }
    else if (ptype == Interface_ParamIdent)
    {
      // Handle entity identifier parameters
      iter.GetOneItem(cont->ParamEntity(i));
    }
  }
}

void StepData_DefaultGeneral::CheckCase(const Standard_Integer,
                                        const Handle(Standard_Transient)&,
                                        const Interface_ShareTool&,
                                        Handle(Interface_Check)&) const
{
} //  No validation check performed on an UndefinedEntity

Standard_Boolean StepData_DefaultGeneral::NewVoid(const Standard_Integer      CN,
                                                  Handle(Standard_Transient)& ent) const
{
  // Create a new empty entity instance (only UndefinedEntity supported)
  if (CN != 1)
    return Standard_False; // Only case 1 supported
  ent = new StepData_UndefinedEntity;
  return Standard_True;
}

void StepData_DefaultGeneral::CopyCase(const Standard_Integer            casenum,
                                       const Handle(Standard_Transient)& entfrom,
                                       const Handle(Standard_Transient)& entto,
                                       Interface_CopyTool&               TC) const
{
  // Copy content from source UndefinedEntity to target UndefinedEntity
  if (casenum != 1)
    return; // Only handles case 1 (UndefinedEntity)
  DeclareAndCast(StepData_UndefinedEntity, undfrom, entfrom);
  DeclareAndCast(StepData_UndefinedEntity, undto, entto);
  undto->GetFromAnother(undfrom, TC); //  We could optimize this operation
}
