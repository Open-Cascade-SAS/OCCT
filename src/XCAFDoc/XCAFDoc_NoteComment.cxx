// Copyright (c) 2017-2018 OPEN CASCADE SAS
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

#include <XCAFDoc_NoteComment.hxx>

#include <Standard_GUID.hxx>
#include <TDF_Label.hxx>

IMPLEMENT_STANDARD_RTTIEXT(XCAFDoc_NoteComment, XCAFDoc_Note)

// =======================================================================
// function : GetID
// purpose  :
// =======================================================================
const Standard_GUID&
XCAFDoc_NoteComment::GetID()
{
  static Standard_GUID s_ID("FDEA4C52-0F54-484c-B590-579E18F7B5D4");
  return s_ID;
}

// =======================================================================
// function : Get
// purpose  :
// =======================================================================
Handle(XCAFDoc_NoteComment)
XCAFDoc_NoteComment::Get(const TDF_Label& theLabel)
{
  Handle(XCAFDoc_NoteComment) aThis;
  theLabel.FindAttribute(XCAFDoc_NoteComment::GetID(), aThis);
  return aThis;
}

// =======================================================================
// function : Set
// purpose  :
// =======================================================================
Handle(XCAFDoc_NoteComment)
XCAFDoc_NoteComment::Set(const TDF_Label&                  theLabel,
                         const TCollection_ExtendedString& theUserName,
                         const TCollection_ExtendedString& theTimeStamp,
                         const TCollection_ExtendedString& theComment)
{
  Handle(XCAFDoc_NoteComment) aNoteComment;
  if (!theLabel.IsNull() && !theLabel.FindAttribute(XCAFDoc_NoteComment::GetID(), aNoteComment))
  {
    aNoteComment = new XCAFDoc_NoteComment();
    aNoteComment->XCAFDoc_Note::Set(theUserName, theTimeStamp);
    aNoteComment->Set(theComment);
    theLabel.AddAttribute(aNoteComment);
  }
  return aNoteComment;
}

// =======================================================================
// function : XCAFDoc_NoteComment
// purpose  :
// =======================================================================
XCAFDoc_NoteComment::XCAFDoc_NoteComment()
{
}

// =======================================================================
// function : Set
// purpose  :
// =======================================================================
void
XCAFDoc_NoteComment::Set(const TCollection_ExtendedString& theComment)
{
  Backup();

  myComment = theComment;
}

// =======================================================================
// function : ID
// purpose  :
// =======================================================================
const Standard_GUID&
XCAFDoc_NoteComment::ID() const
{
  return GetID();
}

// =======================================================================
// function : NewEmpty
// purpose  :
// =======================================================================
Handle(TDF_Attribute)
XCAFDoc_NoteComment::NewEmpty() const
{
  return new XCAFDoc_NoteComment();
}

// =======================================================================
// function : Restore
// purpose  :
// =======================================================================
void
XCAFDoc_NoteComment::Restore(const Handle(TDF_Attribute)& theAttr)
{
  XCAFDoc_Note::Restore(theAttr);

  Handle(XCAFDoc_NoteComment) aMine = Handle(XCAFDoc_NoteComment)::DownCast(theAttr);
  if (!aMine.IsNull())
    myComment = aMine->myComment;
}

// =======================================================================
// function : Paste
// purpose  :
// =======================================================================
void
XCAFDoc_NoteComment::Paste(const Handle(TDF_Attribute)&       theAttrInto,
                           const Handle(TDF_RelocationTable)& theRT) const
{
  XCAFDoc_Note::Paste(theAttrInto, theRT);

  Handle(XCAFDoc_NoteComment) aMine = Handle(XCAFDoc_NoteComment)::DownCast(theAttrInto);
  if (!aMine.IsNull())
    aMine->Set(myComment);
}

// =======================================================================
// function : Dump
// purpose  :
// =======================================================================
Standard_OStream&
XCAFDoc_NoteComment::Dump(Standard_OStream& theOS) const
{
  XCAFDoc_Note::Dump(theOS);
  theOS << "\n"
    << "Comment : " << (!myComment.IsEmpty() ? myComment : "<empty>")
    ;
  return theOS;
}
