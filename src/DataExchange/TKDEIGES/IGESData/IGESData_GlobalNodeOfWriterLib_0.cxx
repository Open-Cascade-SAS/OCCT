// Created on: 1992-04-06
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

#include <IGESData_GlobalNodeOfWriterLib.hxx>

#include <Standard_Type.hxx>

#include <IGESData_ReadWriteModule.hxx>
#include <IGESData_Protocol.hxx>
#include <IGESData_IGESEntity.hxx>
#include <IGESData_WriterLib.hxx>
#include <IGESData_NodeOfWriterLib.hxx>

IGESData_GlobalNodeOfWriterLib::IGESData_GlobalNodeOfWriterLib() {}

void IGESData_GlobalNodeOfWriterLib::Add(const Handle(IGESData_ReadWriteModule)& amodule,
                                         const Handle(IGESData_Protocol)&        aprotocol)
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
      thenext = new IGESData_GlobalNodeOfWriterLib;
      thenext->Add(amodule, aprotocol);
    }
  }
  else
    thenext->Add(amodule, aprotocol);
}

const Handle(IGESData_ReadWriteModule)& IGESData_GlobalNodeOfWriterLib::Module() const
{
  return themod;
}

const Handle(IGESData_Protocol)& IGESData_GlobalNodeOfWriterLib::Protocol() const
{
  return theprot;
}

const Handle(IGESData_GlobalNodeOfWriterLib)& IGESData_GlobalNodeOfWriterLib::Next() const
{
  return thenext;
}
