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

#ifndef _Message_Attribute_HeaderFile
#define _Message_Attribute_HeaderFile

#include <Standard_Transient.hxx>
#include <NCollection_DefineAlloc.hxx>
#include <TCollection_AsciiString.hxx>

DEFINE_STANDARD_HANDLE(Message_Attribute, Standard_Transient)

//! Additional information of extended alert attribute
//! To provide other custom attribute container, it might be redefined.
class Message_Attribute : public Standard_Transient
{
  DEFINE_STANDARD_RTTIEXT(Message_Attribute, Standard_Transient)
public:
  //! Empty constructor
  Standard_EXPORT Message_Attribute (const TCollection_AsciiString& theName = TCollection_AsciiString());

  //! Return a C string to be used as a key for generating text user messages describing this alert.
  //! The messages are generated with help of Message_Msg class, in Message_Report::Dump().
  //! Base implementation returns dynamic type name of the instance.
  Standard_EXPORT virtual Standard_CString GetMessageKey() const;

  //! Returns custom name of alert if it is set
  //! @return alert name
  const TCollection_AsciiString& GetName() const { return myName; }

  //! Sets the custom name of alert
  //! @param theName a name for the alert
  void SetName (const TCollection_AsciiString& theName) { myName = theName; }

  //! Dumps the content of me into the stream
  virtual Standard_EXPORT void DumpJson (Standard_OStream& theOStream, Standard_Integer theDepth = -1) const;

private:
  TCollection_AsciiString myName; //!< alert name, if defined is used in GetMessageKey

};

#endif // _Message_Attribute_HeaderFile
