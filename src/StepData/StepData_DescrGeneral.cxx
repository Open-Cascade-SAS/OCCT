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
#include <Interface_ShareTool.hxx>
#include <Standard_Transient.hxx>
#include <Standard_Type.hxx>
#include <StepData_DescrGeneral.hxx>
#include <StepData_Described.hxx>
#include <StepData_EDescr.hxx>
#include <StepData_Protocol.hxx>

IMPLEMENT_STANDARD_RTTIEXT(StepData_DescrGeneral,StepData_GeneralModule)

StepData_DescrGeneral::StepData_DescrGeneral
  (const Handle(StepData_Protocol)& proto)
    :  theproto (proto)    {  }


    void  StepData_DescrGeneral::FillSharedCase
  (const Standard_Integer, const Handle(Standard_Transient)& ent,
   Interface_EntityIterator& iter) const
{
  Handle(StepData_Described) ds = Handle(StepData_Described)::DownCast(ent);
  if (!ds.IsNull()) ds->Shared (iter);
}


void StepData_DescrGeneral::CheckCase(const Standard_Integer ,
                                      const Handle(Standard_Transient)&,
                                      const Interface_ShareTool&,
                                      Handle(Interface_Check)&) const
{
}    // pour l instant


void  StepData_DescrGeneral::CopyCase(const Standard_Integer,
                                      const Handle(Standard_Transient)&,
                                      const Handle(Standard_Transient)&, 
                                      Interface_CopyTool&) const 
{
}    // pour l instant

Standard_Boolean  StepData_DescrGeneral::NewVoid
  (const Standard_Integer CN, Handle(Standard_Transient)& ent) const
{
  ent = theproto->Descr(CN)->NewEntity();
  return (!ent.IsNull());
}
