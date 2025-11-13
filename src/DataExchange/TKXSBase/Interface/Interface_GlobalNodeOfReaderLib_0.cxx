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

#include <Interface_GlobalNodeOfReaderLib.hxx>

#include <Standard_Type.hxx>

#include <Interface_ReaderModule.hxx>
#include <Interface_Protocol.hxx>
#include <Standard_Transient.hxx>
#include <Interface_ReaderLib.hxx>
#include <Interface_NodeOfReaderLib.hxx>

Interface_GlobalNodeOfReaderLib::Interface_GlobalNodeOfReaderLib() {}

void Interface_GlobalNodeOfReaderLib::Add(const Handle(Interface_ReaderModule)& amodule,
                                          const Handle(Interface_Protocol)&     aprotocol)
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
      thenext = new Interface_GlobalNodeOfReaderLib;
      thenext->Add(amodule, aprotocol);
    }
  }
  else
    thenext->Add(amodule, aprotocol);
}

const Handle(Interface_ReaderModule)& Interface_GlobalNodeOfReaderLib::Module() const
{
  return themod;
}

const Handle(Interface_Protocol)& Interface_GlobalNodeOfReaderLib::Protocol() const
{
  return theprot;
}

const Handle(Interface_GlobalNodeOfReaderLib)& Interface_GlobalNodeOfReaderLib::Next() const
{
  return thenext;
}
