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


#include <IFSelect_BasicDumper.hxx>
#include <IFSelect_SessionDumper.hxx>
#include <IFSelect_SessionFile.hxx>
#include <Standard_Transient.hxx>
#include <Standard_Type.hxx>
#include <TCollection_AsciiString.hxx>

IMPLEMENT_STANDARD_RTTIEXT(IFSelect_SessionDumper,Standard_Transient)

static Handle(IFSelect_SessionDumper)  thefirst;
static int cefait = 0;
// On commence la serie avec celui-la


    IFSelect_SessionDumper::IFSelect_SessionDumper ()
{
  if (!cefait)
    { cefait = 1;  Handle(IFSelect_BasicDumper) bid = new IFSelect_BasicDumper; }
  else thenext  = thefirst;
  thefirst = this;    // as Handle
}

    Handle(IFSelect_SessionDumper)  IFSelect_SessionDumper::First ()
      {  return thefirst;  }

    Handle(IFSelect_SessionDumper)  IFSelect_SessionDumper::Next () const
      {  return thenext;   }
