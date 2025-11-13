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

#include <Interface_NodeOfReaderLib.hxx>

#include <Standard_Type.hxx>

#include <Interface_GlobalNodeOfReaderLib.hxx>
#include <Standard_Transient.hxx>
#include <Interface_ReaderModule.hxx>
#include <Interface_Protocol.hxx>
#include <Interface_ReaderLib.hxx>

Interface_NodeOfReaderLib::Interface_NodeOfReaderLib() {}

void Interface_NodeOfReaderLib::AddNode(const Handle(Interface_GlobalNodeOfReaderLib)& anode)
{
  if (thenode == anode)
    return;
  if (thenext.IsNull())
  {
    if (thenode.IsNull())
      thenode = anode;
    else
    {
      thenext = new Interface_NodeOfReaderLib;
      thenext->AddNode(anode);
    }
  }
  else
    thenext->AddNode(anode);
}

const Handle(Interface_ReaderModule)& Interface_NodeOfReaderLib::Module() const
{
  return thenode->Module();
}

const Handle(Interface_Protocol)& Interface_NodeOfReaderLib::Protocol() const
{
  return thenode->Protocol();
}

const Handle(Interface_NodeOfReaderLib)& Interface_NodeOfReaderLib::Next() const
{
  return thenext;
}
