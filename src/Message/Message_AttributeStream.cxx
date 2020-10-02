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

#include <Message_AttributeStream.hxx>
#include <Standard_Dump.hxx>

IMPLEMENT_STANDARD_RTTIEXT(Message_AttributeStream, Message_Attribute)

//=======================================================================
//function : Constructor
//purpose  :
//=======================================================================
Message_AttributeStream::Message_AttributeStream (const Standard_SStream& theStream,
                                                  const TCollection_AsciiString& theName)
: Message_Attribute(theName)
{
  SetStream (theStream);
}

//=======================================================================
//function : SetStream
//purpose  :
//=======================================================================
void Message_AttributeStream::SetStream (const Standard_SStream& theStream)
{
  myStream.str ("");
  myStream << theStream.str().c_str();
}

//=======================================================================
//function : DumpJson
//purpose  :
//=======================================================================
void Message_AttributeStream::DumpJson (Standard_OStream& theOStream,
                                        Standard_Integer theDepth) const
{
  OCCT_DUMP_TRANSIENT_CLASS_BEGIN (theOStream)
  OCCT_DUMP_BASE_CLASS (theOStream, theDepth, Message_Attribute)

  TCollection_AsciiString aStream = Standard_Dump::Text (myStream);
  OCCT_DUMP_FIELD_VALUE_STRING (theOStream, aStream)
}
