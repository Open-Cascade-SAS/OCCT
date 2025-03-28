// Copyright (c) 2020 OPEN CASCADE SAS
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

#include <Message_Attribute.hxx>

#include <Standard_Dump.hxx>

IMPLEMENT_STANDARD_RTTIEXT(Message_Attribute, Standard_Transient)

//=================================================================================================

Message_Attribute::Message_Attribute(const TCollection_AsciiString& theName)
    : myName(theName)
{
}

//=================================================================================================

Standard_CString Message_Attribute::GetMessageKey() const
{
  return !myName.IsEmpty() ? myName.ToCString() : "";
}

//=================================================================================================

void Message_Attribute::DumpJson(Standard_OStream& theOStream, Standard_Integer) const
{
  OCCT_DUMP_TRANSIENT_CLASS_BEGIN(theOStream)
  OCCT_DUMP_FIELD_VALUE_STRING(theOStream, myName)
}
