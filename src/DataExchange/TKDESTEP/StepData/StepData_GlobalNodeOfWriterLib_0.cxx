// Created on: 1992-02-11
// Created by: Christian CAILLET
// Copyright (c) 1992-1999 Matra Datavision
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

#include <StepData_GlobalNodeOfWriterLib.hxx>

#include <Standard_Type.hxx>

#include <StepData_ReadWriteModule.hxx>
#include <StepData_Protocol.hxx>
#include <Standard_Transient.hxx>
#include <StepData_WriterLib.hxx>
#include <StepData_NodeOfWriterLib.hxx>

StepData_GlobalNodeOfWriterLib::StepData_GlobalNodeOfWriterLib() {}

void StepData_GlobalNodeOfWriterLib::Add(const Handle(StepData_ReadWriteModule)& amodule,
                                         const Handle(StepData_Protocol)&        aprotocol)
{
  if (themod == amodule)
    return;
  if (theprot == aprotocol)
    themod = amodule;
  else if (thenext.IsNull())
  {
    if (themod.IsNull())
    {
      themod  = amodule;
      theprot = aprotocol;
    }
    else
    {
      thenext = new StepData_GlobalNodeOfWriterLib;
      thenext->Add(amodule, aprotocol);
    }
  }
  else
    thenext->Add(amodule, aprotocol);
}

const Handle(StepData_ReadWriteModule)& StepData_GlobalNodeOfWriterLib::Module() const
{
  return themod;
}

const Handle(StepData_Protocol)& StepData_GlobalNodeOfWriterLib::Protocol() const
{
  return theprot;
}

const Handle(StepData_GlobalNodeOfWriterLib)& StepData_GlobalNodeOfWriterLib::Next() const
{
  return thenext;
}
