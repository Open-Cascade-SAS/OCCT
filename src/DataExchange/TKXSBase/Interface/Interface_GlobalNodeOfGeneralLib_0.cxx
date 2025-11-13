// Created on: 1992-02-03
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

#include <Interface_GlobalNodeOfGeneralLib.hxx>

#include <Standard_Type.hxx>

#include <Interface_GeneralModule.hxx>
#include <Interface_Protocol.hxx>
#include <Standard_Transient.hxx>
#include <Interface_GeneralLib.hxx>
#include <Interface_NodeOfGeneralLib.hxx>

Interface_GlobalNodeOfGeneralLib::Interface_GlobalNodeOfGeneralLib() {}

void Interface_GlobalNodeOfGeneralLib::Add(const Handle(Interface_GeneralModule)& amodule,
                                           const Handle(Interface_Protocol)&      aprotocol)
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
      thenext = new Interface_GlobalNodeOfGeneralLib;
      thenext->Add(amodule, aprotocol);
    }
  }
  else
    thenext->Add(amodule, aprotocol);
}

const Handle(Interface_GeneralModule)& Interface_GlobalNodeOfGeneralLib::Module() const
{
  return themod;
}

const Handle(Interface_Protocol)& Interface_GlobalNodeOfGeneralLib::Protocol() const
{
  return theprot;
}

const Handle(Interface_GlobalNodeOfGeneralLib)& Interface_GlobalNodeOfGeneralLib::Next() const
{
  return thenext;
}
