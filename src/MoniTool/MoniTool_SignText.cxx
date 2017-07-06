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


#include <MoniTool_SignText.hxx>
#include <Standard_Transient.hxx>
#include <Standard_Type.hxx>
#include <TCollection_AsciiString.hxx>

IMPLEMENT_STANDARD_RTTIEXT(MoniTool_SignText,Standard_Transient)

TCollection_AsciiString  MoniTool_SignText::TextAlone
  (const Handle(Standard_Transient)& ent) const
{
  Handle(Standard_Transient) nulctx;  // no context
  TCollection_AsciiString atext = Text (ent,nulctx);
  if (atext.Length() == 0) {
    if (ent.IsNull()) atext.AssignCat ("(NULL)");
    else              atext.AssignCat (ent->DynamicType()->Name());
  }
  return atext;
}
