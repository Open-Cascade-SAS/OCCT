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

#ifndef _Message_AttributeObject_HeaderFile
#define _Message_AttributeObject_HeaderFile

#include <Message_Attribute.hxx>
#include <NCollection_DefineAlloc.hxx>

//! Alert object storing a transient object
class Message_AttributeObject : public Message_Attribute
{
  DEFINE_STANDARD_RTTIEXT(Message_AttributeObject, Message_Attribute)
public:
  //! Constructor with string argument
  Standard_EXPORT Message_AttributeObject(
    const Handle(Standard_Transient)& theObject,
    const TCollection_AsciiString&    theName = TCollection_AsciiString());

  //! Returns object
  //! @return the object instance
  const Handle(Standard_Transient)& Object() const { return myObject; }

  //! Sets the object
  //! @param theObject an instance
  void SetObject(const Handle(Standard_Transient)& theObject) { myObject = theObject; }

  //! Dumps the content of me into the stream
  virtual Standard_EXPORT void DumpJson(Standard_OStream& theOStream,
                                        Standard_Integer  theDepth = -1) const Standard_OVERRIDE;

private:
  Handle(Standard_Transient) myObject; //!< alert object
};

#endif // _Message_AttributeObject_HeaderFile
