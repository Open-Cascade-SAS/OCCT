// Created on: 2017-02-10
// Created by: Sergey NIKONOV
// Copyright (c) 2000-2017 OPEN CASCADE SAS
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

#include <Standard_GUID.hxx>
#include <TDF_AttributeIterator.hxx>
#include <TDF_Label.hxx>
#include <XCAFDoc.hxx>
#include <XCAFDoc_GraphNode.hxx>
#include <XCAFDoc_Note.hxx>

IMPLEMENT_STANDARD_RTTIEXT(XCAFDoc_Note, TDF_Attribute)

Standard_Boolean 
XCAFDoc_Note::IsMine(const TDF_Label& theLabel)
{
  return !Get(theLabel).IsNull();
}

XCAFDoc_Note::XCAFDoc_Note()
{
}

Handle(XCAFDoc_Note) 
XCAFDoc_Note::Get(const TDF_Label& theLabel)
{
  Handle(XCAFDoc_Note) aNote;
  for (TDF_AttributeIterator anIt(theLabel); anIt.More(); anIt.Next())
  {
    aNote = Handle(XCAFDoc_Note)::DownCast(anIt.Value());
    if (!aNote.IsNull())
      break;
  }
  return aNote;
}

void 
XCAFDoc_Note::Set(const TCollection_ExtendedString& theUserName,
                  const TCollection_ExtendedString& theTimeStamp)
{
  Backup();

  myUserName = theUserName;
  myTimeStamp = theTimeStamp;
}

const TCollection_ExtendedString& 
XCAFDoc_Note::UserName() const
{
  return myUserName;
}

const TCollection_ExtendedString& 
XCAFDoc_Note::TimeStamp() const
{
  return myTimeStamp;
}

Standard_Boolean 
XCAFDoc_Note::IsOrphan() const
{
  Handle(XCAFDoc_GraphNode) aFather;
  return !Label().FindAttribute(XCAFDoc::NoteRefGUID(), aFather) ||
         (aFather->NbChildren() == 0);
}

void 
XCAFDoc_Note::Restore(const Handle(TDF_Attribute)& theAttr)
{
  myUserName = Handle(XCAFDoc_Note)::DownCast(theAttr)->myUserName;
  myTimeStamp = Handle(XCAFDoc_Note)::DownCast(theAttr)->myTimeStamp;
}

void 
XCAFDoc_Note::Paste(const Handle(TDF_Attribute)&       theAttrInto,
                    const Handle(TDF_RelocationTable)& /*theRT*/) const
{
  Handle(XCAFDoc_Note)::DownCast(theAttrInto)->Set(myUserName, myTimeStamp);
}

Standard_OStream& 
XCAFDoc_Note::Dump(Standard_OStream& theOS) const
{
  TDF_Attribute::Dump(theOS);
  theOS 
    << "Note : " 
    << (myUserName.IsEmpty() ? myUserName : "<anonymous>")
    << " on "
    << (myTimeStamp.IsEmpty() ? myTimeStamp : "<unknown>")
    ;
  return theOS;
}
