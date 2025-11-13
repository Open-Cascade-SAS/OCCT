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

#include <IGESData_GlobalNodeOfSpecificLib.hxx>

#include <Standard_Type.hxx>

#include <IGESData_SpecificModule.hxx>
#include <IGESData_Protocol.hxx>
#include <IGESData_IGESEntity.hxx>
#include <IGESData_SpecificLib.hxx>
#include <IGESData_NodeOfSpecificLib.hxx>

IGESData_GlobalNodeOfSpecificLib::IGESData_GlobalNodeOfSpecificLib() {}

void IGESData_GlobalNodeOfSpecificLib::Add(const Handle(IGESData_SpecificModule)& amodule,
                                           const Handle(IGESData_Protocol)&       aprotocol)
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
      thenext = new IGESData_GlobalNodeOfSpecificLib;
      thenext->Add(amodule, aprotocol);
    }
  }
  else
    thenext->Add(amodule, aprotocol);
}

const Handle(IGESData_SpecificModule)& IGESData_GlobalNodeOfSpecificLib::Module() const
{
  return themod;
}

const Handle(IGESData_Protocol)& IGESData_GlobalNodeOfSpecificLib::Protocol() const
{
  return theprot;
}

const Handle(IGESData_GlobalNodeOfSpecificLib)& IGESData_GlobalNodeOfSpecificLib::Next() const
{
  return thenext;
}
