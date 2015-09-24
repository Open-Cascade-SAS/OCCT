// Created on: 2015-07-07
// Created by: Irina KRYLOVA
// Copyright (c) 2015 OPEN CASCADE SAS
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

#ifndef _RWStepAP242_RWItemIdentifiedRepresentationUsage_HeaderFile
#define _RWStepAP242_RWItemIdentifiedRepresentationUsage_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Integer.hxx>

class StepData_StepReaderData;
class Interface_Check;
class StepAP242_ItemIdentifiedRepresentationUsage;
class StepData_StepWriter;
class Interface_EntityIterator;


//! Read & Write Module for ItemIdentifiedRepresentationUsage
class RWStepAP242_RWItemIdentifiedRepresentationUsage 
{
public:

  DEFINE_STANDARD_ALLOC

  
  Standard_EXPORT RWStepAP242_RWItemIdentifiedRepresentationUsage();
  
  Standard_EXPORT   void ReadStep (const Handle(StepData_StepReaderData)& data, const Standard_Integer num, Handle(Interface_Check)& ach, const Handle(StepAP242_ItemIdentifiedRepresentationUsage)& ent)  const;
  
  Standard_EXPORT   void WriteStep (StepData_StepWriter& SW, const Handle(StepAP242_ItemIdentifiedRepresentationUsage)& ent)  const;
  
  Standard_EXPORT   void Share (const Handle(StepAP242_ItemIdentifiedRepresentationUsage)& ent, Interface_EntityIterator& iter)  const;
};
#endif // _RWStepAP242_RWItemIdentifiedRepresentationUsage_HeaderFile
