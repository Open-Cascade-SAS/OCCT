// Created on: 2018-06-10
// Created by: Natalia Ermolaeva
// Copyright (c) 2018-2020 OPEN CASCADE SAS
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

#ifndef _TopoDS_AlertAttribute_HeaderFile
#define _TopoDS_AlertAttribute_HeaderFile

#include <Message_AttributeStream.hxx>
#include <Message.hxx>
#include <Message_Messenger.hxx>
#include <Message_Report.hxx>

#include <TopoDS_Shape.hxx>

class Message_Messenger;

//! Alert attribute object storing TopoDS shape in its field
class TopoDS_AlertAttribute : public Message_AttributeStream
{
  DEFINE_STANDARD_RTTIEXT(TopoDS_AlertAttribute, Message_AttributeStream)
public:
  //! Constructor with shape argument
  Standard_EXPORT TopoDS_AlertAttribute (const TopoDS_Shape& theShape,
                                         const TCollection_AsciiString& theName = TCollection_AsciiString());

  //! Returns contained shape
  const TopoDS_Shape& GetShape() const { return myShape; }

public:

  //! Push shape information into messenger
  Standard_EXPORT static void Send (const Handle(Message_Messenger)& theMessenger,
                                    const TopoDS_Shape& theShape);

  //! Dumps the content of me into the stream
  Standard_EXPORT void DumpJson (Standard_OStream& theOStream,
                                 Standard_Integer theDepth = -1) const Standard_OVERRIDE;

private:
  TopoDS_Shape myShape;
};

inline const Handle(Message_Messenger)& operator<< (const Handle(Message_Messenger)& theMessenger,
                                                    const TopoDS_Shape& theShape)
{
  TopoDS_AlertAttribute::Send (theMessenger, theShape);
  return theMessenger;
}

#endif // _TopoDS_AlertAttribute_HeaderFile
