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

#ifndef _Message_AttributeStream_HeaderFile
#define _Message_AttributeStream_HeaderFile

#include <Message_Attribute.hxx>

#include <Standard_SStream.hxx>

//! Alert object storing stream value
class Message_AttributeStream : public Message_Attribute
{
  DEFINE_STANDARD_RTTIEXT(Message_AttributeStream, Message_Attribute)
public:

  //! Constructor with string argument
  Standard_EXPORT Message_AttributeStream (const Standard_SStream& theStream,
                                           const TCollection_AsciiString& theName = TCollection_AsciiString());

  //! Returns stream value
  const Standard_SStream& Stream() const { return myStream; }

  //! Sets stream value
  Standard_EXPORT void SetStream (const Standard_SStream& theStream);

  //! Dumps the content of me into the stream
  virtual Standard_EXPORT void DumpJson (Standard_OStream& theOStream,
                                         Standard_Integer theDepth = -1) const Standard_OVERRIDE;

private:
  Standard_SStream myStream; //!< container of values
};

#endif // _Message_AttributeStream_HeaderFile
