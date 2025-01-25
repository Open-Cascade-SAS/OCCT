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

#include <XCAFDoc_NotesTool.hxx>

#include <TColStd_HArray1OfByte.hxx>
#include <TDF_Label.hxx>
#include <TDF_ChildIDIterator.hxx>
#include <TDF_LabelSequence.hxx>
#include <TDF_Tool.hxx>
#include <XCAFDoc.hxx>
#include <XCAFDoc_GraphNode.hxx>
#include <XCAFDoc_NoteBalloon.hxx>
#include <XCAFDoc_NoteComment.hxx>
#include <XCAFDoc_NoteBinData.hxx>
#include <XCAFDoc_AssemblyItemRef.hxx>

namespace
{

XCAFDoc_AssemblyItemId labeledItem(const TDF_Label& theLabel)
{
  TCollection_AsciiString anEntry;
  TDF_Tool::Entry(theLabel, anEntry);
  return XCAFDoc_AssemblyItemId(anEntry);
}

} // namespace

IMPLEMENT_DERIVED_ATTRIBUTE(XCAFDoc_NotesTool, TDataStd_GenericEmpty)

enum NotesTool_RootLabels
{
  NotesTool_NotesRoot = 1,
  NotesTool_AnnotatedItemsRoot
};

// =======================================================================
// function : GetID
// purpose  :
// =======================================================================
const Standard_GUID& XCAFDoc_NotesTool::GetID()
{
  static Standard_GUID s_ID("8F8174B1-6125-47a0-B357-61BD2D89380C");
  return s_ID;
}

// =======================================================================
// function : Set
// purpose  :
// =======================================================================
Handle(XCAFDoc_NotesTool) XCAFDoc_NotesTool::Set(const TDF_Label& theLabel)
{
  Handle(XCAFDoc_NotesTool) aTool;
  if (!theLabel.IsNull() && !theLabel.FindAttribute(XCAFDoc_NotesTool::GetID(), aTool))
  {
    aTool = new XCAFDoc_NotesTool();
    theLabel.AddAttribute(aTool);
  }
  return aTool;
}

// =======================================================================
// function : XCAFDoc_NotesTool
// purpose  :
// =======================================================================
XCAFDoc_NotesTool::XCAFDoc_NotesTool() {}

// =======================================================================
// function : GetNotesLabel
// purpose  :
// =======================================================================
TDF_Label XCAFDoc_NotesTool::GetNotesLabel() const
{
  return Label().FindChild(NotesTool_NotesRoot);
}

// =======================================================================
// function : GetAnnotatedItemsLabel
// purpose  :
// =======================================================================
TDF_Label XCAFDoc_NotesTool::GetAnnotatedItemsLabel() const
{
  return Label().FindChild(NotesTool_AnnotatedItemsRoot);
}

// =======================================================================
// function : NbNotes
// purpose  :
// =======================================================================
Standard_Integer XCAFDoc_NotesTool::NbNotes() const
{
  Standard_Integer nbNotes = 0;
  for (TDF_ChildIterator anIter(GetNotesLabel()); anIter.More(); anIter.Next())
  {
    const TDF_Label aLabel = anIter.Value();
    if (!XCAFDoc_Note::Get(aLabel).IsNull())
      ++nbNotes;
  }
  return nbNotes;
}

// =======================================================================
// function : NbAnnotatedItems
// purpose  :
// =======================================================================
Standard_Integer XCAFDoc_NotesTool::NbAnnotatedItems() const
{
  Standard_Integer nbItems = 0;
  for (TDF_ChildIDIterator anIter(GetAnnotatedItemsLabel(), XCAFDoc_AssemblyItemRef::GetID());
       anIter.More();
       anIter.Next())
  {
    ++nbItems;
  }
  return nbItems;
}

// =======================================================================
// function : GetNotes
// purpose  :
// =======================================================================
void XCAFDoc_NotesTool::GetNotes(TDF_LabelSequence& theNoteLabels) const
{
  for (TDF_ChildIterator anIter(GetNotesLabel()); anIter.More(); anIter.Next())
  {
    const TDF_Label aLabel = anIter.Value();
    if (!XCAFDoc_Note::Get(aLabel).IsNull())
      theNoteLabels.Append(aLabel);
  }
}

// =======================================================================
// function : GetAnnotatedItems
// purpose  :
// =======================================================================
void XCAFDoc_NotesTool::GetAnnotatedItems(TDF_LabelSequence& theItemLabels) const
{
  for (TDF_ChildIDIterator anIter(GetAnnotatedItemsLabel(), XCAFDoc_AssemblyItemRef::GetID());
       anIter.More();
       anIter.Next())
  {
    theItemLabels.Append(anIter.Value()->Label());
  }
}

// =======================================================================
// function : IsAnnotatedItem
// purpose  :
// =======================================================================
Standard_Boolean XCAFDoc_NotesTool::IsAnnotatedItem(const XCAFDoc_AssemblyItemId& theItemId) const
{
  return !FindAnnotatedItem(theItemId).IsNull();
}

// =======================================================================
// function : IsAnnotatedItem
// purpose  :
// =======================================================================
Standard_Boolean XCAFDoc_NotesTool::IsAnnotatedItem(const TDF_Label& theItemLabel) const
{
  return IsAnnotatedItem(labeledItem(theItemLabel));
}

// =======================================================================
// function : FindAnnotatedItem
// purpose  :
// =======================================================================
TDF_Label XCAFDoc_NotesTool::FindAnnotatedItem(const XCAFDoc_AssemblyItemId& theItemId) const
{
  for (TDF_ChildIDIterator anIter(GetAnnotatedItemsLabel(), XCAFDoc_AssemblyItemRef::GetID());
       anIter.More();
       anIter.Next())
  {
    Handle(XCAFDoc_AssemblyItemRef) anItemRef =
      Handle(XCAFDoc_AssemblyItemRef)::DownCast(anIter.Value());
    if (!anItemRef.IsNull() && anItemRef->GetItem().IsEqual(theItemId) && !anItemRef->HasExtraRef())
      return anItemRef->Label();
  }
  return TDF_Label();
}

// =======================================================================
// function : FindAnnotatedItem
// purpose  :
// =======================================================================
TDF_Label XCAFDoc_NotesTool::FindAnnotatedItem(const TDF_Label& theItemLabel) const
{
  return FindAnnotatedItem(labeledItem(theItemLabel));
}

// =======================================================================
// function : FindAnnotatedItemAttr
// purpose  :
// =======================================================================
TDF_Label XCAFDoc_NotesTool::FindAnnotatedItemAttr(const XCAFDoc_AssemblyItemId& theItemId,
                                                   const Standard_GUID&          theGUID) const
{
  for (TDF_ChildIDIterator anIter(GetAnnotatedItemsLabel(), XCAFDoc_AssemblyItemRef::GetID());
       anIter.More();
       anIter.Next())
  {
    Handle(XCAFDoc_AssemblyItemRef) anItemRef =
      Handle(XCAFDoc_AssemblyItemRef)::DownCast(anIter.Value());
    if (!anItemRef.IsNull() && anItemRef->GetItem().IsEqual(theItemId) && anItemRef->HasExtraRef()
        && anItemRef->GetGUID() == theGUID)
      return anItemRef->Label();
  }
  return TDF_Label();
}

// =======================================================================
// function : FindAnnotatedItemAttr
// purpose  :
// =======================================================================
TDF_Label XCAFDoc_NotesTool::FindAnnotatedItemAttr(const TDF_Label&     theItemLabel,
                                                   const Standard_GUID& theGUID) const
{
  return FindAnnotatedItemAttr(labeledItem(theItemLabel), theGUID);
}

// =======================================================================
// function : FindAnnotatedItemSubshape
// purpose  :
// =======================================================================
TDF_Label XCAFDoc_NotesTool::FindAnnotatedItemSubshape(const XCAFDoc_AssemblyItemId& theItemId,
                                                       Standard_Integer theSubshapeIndex) const
{
  for (TDF_ChildIDIterator anIter(GetAnnotatedItemsLabel(), XCAFDoc_AssemblyItemRef::GetID());
       anIter.More();
       anIter.Next())
  {
    Handle(XCAFDoc_AssemblyItemRef) anItemRef =
      Handle(XCAFDoc_AssemblyItemRef)::DownCast(anIter.Value());
    if (!anItemRef.IsNull() && anItemRef->GetItem().IsEqual(theItemId) && anItemRef->HasExtraRef()
        && anItemRef->GetSubshapeIndex() == theSubshapeIndex)
      return anItemRef->Label();
  }
  return TDF_Label();
}

// =======================================================================
// function : FindAnnotatedItemSubshape
// purpose  :
// =======================================================================
TDF_Label XCAFDoc_NotesTool::FindAnnotatedItemSubshape(const TDF_Label& theItemLabel,
                                                       Standard_Integer theSubshapeIndex) const
{
  return FindAnnotatedItemSubshape(labeledItem(theItemLabel), theSubshapeIndex);
}

// =======================================================================
// function : CreateComment
// purpose  :
// =======================================================================
Handle(XCAFDoc_Note) XCAFDoc_NotesTool::CreateComment(
  const TCollection_ExtendedString& theUserName,
  const TCollection_ExtendedString& theTimeStamp,
  const TCollection_ExtendedString& theComment)
{
  TDF_Label     aNoteLabel;
  TDF_TagSource aTag;
  aNoteLabel = aTag.NewChild(GetNotesLabel());
  return XCAFDoc_NoteComment::Set(aNoteLabel, theUserName, theTimeStamp, theComment);
}

// =======================================================================
// function : CreateBalloon
// purpose  :
// =======================================================================
Handle(XCAFDoc_Note) XCAFDoc_NotesTool::CreateBalloon(
  const TCollection_ExtendedString& theUserName,
  const TCollection_ExtendedString& theTimeStamp,
  const TCollection_ExtendedString& theComment)
{
  TDF_Label     aNoteLabel;
  TDF_TagSource aTag;
  aNoteLabel = aTag.NewChild(GetNotesLabel());
  return XCAFDoc_NoteBalloon::Set(aNoteLabel, theUserName, theTimeStamp, theComment);
}

// =======================================================================
// function : CreateBinData
// purpose  :
// =======================================================================
Handle(XCAFDoc_Note) XCAFDoc_NotesTool::CreateBinData(
  const TCollection_ExtendedString& theUserName,
  const TCollection_ExtendedString& theTimeStamp,
  const TCollection_ExtendedString& theTitle,
  const TCollection_AsciiString&    theMIMEtype,
  OSD_File&                         theFile)
{
  TDF_Label     aNoteLabel;
  TDF_TagSource aTag;
  aNoteLabel = aTag.NewChild(GetNotesLabel());
  return XCAFDoc_NoteBinData::Set(aNoteLabel,
                                  theUserName,
                                  theTimeStamp,
                                  theTitle,
                                  theMIMEtype,
                                  theFile);
}

// =======================================================================
// function : CreateBinData
// purpose  :
// =======================================================================
Handle(XCAFDoc_Note) XCAFDoc_NotesTool::CreateBinData(
  const TCollection_ExtendedString&    theUserName,
  const TCollection_ExtendedString&    theTimeStamp,
  const TCollection_ExtendedString&    theTitle,
  const TCollection_AsciiString&       theMIMEtype,
  const Handle(TColStd_HArray1OfByte)& theData)
{
  TDF_Label     aNoteLabel;
  TDF_TagSource aTag;
  aNoteLabel = aTag.NewChild(GetNotesLabel());
  return XCAFDoc_NoteBinData::Set(aNoteLabel,
                                  theUserName,
                                  theTimeStamp,
                                  theTitle,
                                  theMIMEtype,
                                  theData);
}

// =======================================================================
// function : GetNotes
// purpose  :
// =======================================================================
Standard_Integer XCAFDoc_NotesTool::GetNotes(const XCAFDoc_AssemblyItemId& theItemId,
                                             TDF_LabelSequence&            theNoteLabels) const
{
  TDF_Label anAnnotatedItem = FindAnnotatedItem(theItemId);
  if (anAnnotatedItem.IsNull())
    return 0;

  Handle(XCAFDoc_GraphNode) aChild;
  if (!anAnnotatedItem.FindAttribute(XCAFDoc::NoteRefGUID(), aChild))
    return 0;

  Standard_Integer nbFathers = aChild->NbFathers();
  for (Standard_Integer iFather = 1; iFather <= nbFathers; ++iFather)
  {
    Handle(XCAFDoc_GraphNode) aFather = aChild->GetFather(iFather);
    theNoteLabels.Append(aFather->Label());
  }

  return theNoteLabels.Length();
}

// =======================================================================
// function : GetNotes
// purpose  :
// =======================================================================
Standard_Integer XCAFDoc_NotesTool::GetNotes(const TDF_Label&   theItemLabel,
                                             TDF_LabelSequence& theNoteLabels) const
{
  return GetNotes(labeledItem(theItemLabel), theNoteLabels);
}

// =======================================================================
// function : GetAttrNotes
// purpose  :
// =======================================================================
Standard_Integer XCAFDoc_NotesTool::GetAttrNotes(const XCAFDoc_AssemblyItemId& theItemId,
                                                 const Standard_GUID&          theGUID,
                                                 TDF_LabelSequence&            theNoteLabels) const
{
  TDF_Label anAnnotatedItem = FindAnnotatedItemAttr(theItemId, theGUID);
  if (anAnnotatedItem.IsNull())
    return 0;

  Handle(XCAFDoc_GraphNode) aChild;
  if (!anAnnotatedItem.FindAttribute(XCAFDoc::NoteRefGUID(), aChild))
    return 0;

  Standard_Integer nbFathers = aChild->NbFathers();
  for (Standard_Integer iFather = 1; iFather <= nbFathers; ++iFather)
  {
    Handle(XCAFDoc_GraphNode) aFather = aChild->GetFather(iFather);
    theNoteLabels.Append(aFather->Label());
  }

  return theNoteLabels.Length();
}

// =======================================================================
// function : GetAttrNotes
// purpose  :
// =======================================================================
Standard_Integer XCAFDoc_NotesTool::GetAttrNotes(const TDF_Label&     theItemLabel,
                                                 const Standard_GUID& theGUID,
                                                 TDF_LabelSequence&   theNoteLabels) const
{
  return GetAttrNotes(labeledItem(theItemLabel), theGUID, theNoteLabels);
}

// =======================================================================
// function : GetSubshapeNotes
// purpose  :
// =======================================================================
Standard_Integer XCAFDoc_NotesTool::GetSubshapeNotes(const XCAFDoc_AssemblyItemId& theItemId,
                                                     Standard_Integer              theSubshapeIndex,
                                                     TDF_LabelSequence& theNoteLabels) const
{
  TDF_Label anAnnotatedItem = FindAnnotatedItemSubshape(theItemId, theSubshapeIndex);
  if (anAnnotatedItem.IsNull())
    return 0;

  Handle(XCAFDoc_GraphNode) aChild;
  if (!anAnnotatedItem.FindAttribute(XCAFDoc::NoteRefGUID(), aChild))
    return 0;

  Standard_Integer nbFathers = aChild->NbFathers();
  for (Standard_Integer iFather = 1; iFather <= nbFathers; ++iFather)
  {
    Handle(XCAFDoc_GraphNode) aFather = aChild->GetFather(iFather);
    theNoteLabels.Append(aFather->Label());
  }

  return theNoteLabels.Length();
}

// =======================================================================
// function : AddNote
// purpose  :
// =======================================================================
Handle(XCAFDoc_AssemblyItemRef) XCAFDoc_NotesTool::AddNote(const TDF_Label& theNoteLabel,
                                                           const XCAFDoc_AssemblyItemId& theItemId)
{
  Handle(XCAFDoc_AssemblyItemRef) anItemRef;

  if (!XCAFDoc_Note::IsMine(theNoteLabel))
    return anItemRef;

  Handle(XCAFDoc_GraphNode) aChild;
  TDF_Label                 anAnnotatedItem = FindAnnotatedItem(theItemId);
  if (anAnnotatedItem.IsNull())
  {
    TDF_TagSource aTag;
    anAnnotatedItem = aTag.NewChild(GetAnnotatedItemsLabel());
    if (anAnnotatedItem.IsNull())
      return anItemRef;
  }

  if (!anAnnotatedItem.FindAttribute(XCAFDoc::NoteRefGUID(), aChild))
  {
    aChild = XCAFDoc_GraphNode::Set(anAnnotatedItem, XCAFDoc::NoteRefGUID());
    if (aChild.IsNull())
      return anItemRef;
  }

  if (!anAnnotatedItem.FindAttribute(XCAFDoc_AssemblyItemRef::GetID(), anItemRef))
  {
    anItemRef = XCAFDoc_AssemblyItemRef::Set(anAnnotatedItem, theItemId);
    if (anItemRef.IsNull())
      return anItemRef;
  }

  Handle(XCAFDoc_GraphNode) aFather;
  if (!theNoteLabel.FindAttribute(XCAFDoc::NoteRefGUID(), aFather))
  {
    aFather = XCAFDoc_GraphNode::Set(theNoteLabel, XCAFDoc::NoteRefGUID());
    if (aFather.IsNull())
      return anItemRef;
  }

  aChild->SetFather(aFather);
  aFather->SetChild(aChild);

  return anItemRef;
}

// =======================================================================
// function : AddNote
// purpose  :
// =======================================================================
Handle(XCAFDoc_AssemblyItemRef) XCAFDoc_NotesTool::AddNote(const TDF_Label& theNoteLabel,
                                                           const TDF_Label& theItemLabel)
{
  return AddNote(theNoteLabel, labeledItem(theItemLabel));
}

// =======================================================================
// function : AddNoteToAttr
// purpose  :
// =======================================================================
Handle(XCAFDoc_AssemblyItemRef) XCAFDoc_NotesTool::AddNoteToAttr(
  const TDF_Label&              theNoteLabel,
  const XCAFDoc_AssemblyItemId& theItemId,
  const Standard_GUID&          theGUID)
{
  Handle(XCAFDoc_AssemblyItemRef) anItemRef;

  if (!XCAFDoc_Note::IsMine(theNoteLabel))
    return anItemRef;

  Handle(XCAFDoc_GraphNode) aChild;
  TDF_Label                 anAnnotatedItem = FindAnnotatedItemAttr(theItemId, theGUID);
  if (anAnnotatedItem.IsNull())
  {
    TDF_TagSource aTag;
    anAnnotatedItem = aTag.NewChild(GetAnnotatedItemsLabel());
    if (anAnnotatedItem.IsNull())
      return anItemRef;
  }

  if (!anAnnotatedItem.FindAttribute(XCAFDoc::NoteRefGUID(), aChild))
  {
    aChild = XCAFDoc_GraphNode::Set(anAnnotatedItem, XCAFDoc::NoteRefGUID());
    if (aChild.IsNull())
      return anItemRef;
  }

  if (!anAnnotatedItem.FindAttribute(XCAFDoc_AssemblyItemRef::GetID(), anItemRef))
  {
    anItemRef = XCAFDoc_AssemblyItemRef::Set(anAnnotatedItem, theItemId);
    if (anItemRef.IsNull())
      return anItemRef;
  }

  Handle(XCAFDoc_GraphNode) aFather;
  if (!theNoteLabel.FindAttribute(XCAFDoc::NoteRefGUID(), aFather))
  {
    aFather = XCAFDoc_GraphNode::Set(theNoteLabel, XCAFDoc::NoteRefGUID());
    if (aFather.IsNull())
      return anItemRef;
  }

  aChild->SetFather(aFather);
  aFather->SetChild(aChild);

  anItemRef->SetGUID(theGUID);

  return anItemRef;
}

// =======================================================================
// function : AddNoteToAttr
// purpose  :
// =======================================================================
Handle(XCAFDoc_AssemblyItemRef) XCAFDoc_NotesTool::AddNoteToAttr(const TDF_Label&     theNoteLabel,
                                                                 const TDF_Label&     theItemLabel,
                                                                 const Standard_GUID& theGUID)
{
  return AddNoteToAttr(theNoteLabel, labeledItem(theItemLabel), theGUID);
}

// =======================================================================
// function : AddNoteToSubshape
// purpose  :
// =======================================================================
Handle(XCAFDoc_AssemblyItemRef) XCAFDoc_NotesTool::AddNoteToSubshape(
  const TDF_Label&              theNoteLabel,
  const XCAFDoc_AssemblyItemId& theItemId,
  Standard_Integer              theSubshapeIndex)
{
  Handle(XCAFDoc_AssemblyItemRef) anItemRef;

  if (!XCAFDoc_Note::IsMine(theNoteLabel))
    return anItemRef;

  Handle(XCAFDoc_GraphNode) aChild;
  TDF_Label anAnnotatedItem = FindAnnotatedItemSubshape(theItemId, theSubshapeIndex);
  if (anAnnotatedItem.IsNull())
  {
    TDF_TagSource aTag;
    anAnnotatedItem = aTag.NewChild(GetAnnotatedItemsLabel());
    if (anAnnotatedItem.IsNull())
      return anItemRef;
  }

  if (!anAnnotatedItem.FindAttribute(XCAFDoc::NoteRefGUID(), aChild))
  {
    aChild = XCAFDoc_GraphNode::Set(anAnnotatedItem, XCAFDoc::NoteRefGUID());
    if (aChild.IsNull())
      return anItemRef;
  }

  if (!anAnnotatedItem.FindAttribute(XCAFDoc_AssemblyItemRef::GetID(), anItemRef))
  {
    anItemRef = XCAFDoc_AssemblyItemRef::Set(anAnnotatedItem, theItemId);
    if (anItemRef.IsNull())
      return anItemRef;
  }

  Handle(XCAFDoc_GraphNode) aFather;
  if (!theNoteLabel.FindAttribute(XCAFDoc::NoteRefGUID(), aFather))
  {
    aFather = XCAFDoc_GraphNode::Set(theNoteLabel, XCAFDoc::NoteRefGUID());
    if (aFather.IsNull())
      return anItemRef;
  }

  aChild->SetFather(aFather);
  aFather->SetChild(aChild);

  anItemRef->SetSubshapeIndex(theSubshapeIndex);

  return anItemRef;
}

// =======================================================================
// function : AddNoteToSubshape
// purpose  :
// =======================================================================
Handle(XCAFDoc_AssemblyItemRef) XCAFDoc_NotesTool::AddNoteToSubshape(
  const TDF_Label& theNoteLabel,
  const TDF_Label& theItemLabel,
  Standard_Integer theSubshapeIndex)
{
  return AddNoteToSubshape(theNoteLabel, labeledItem(theItemLabel), theSubshapeIndex);
}

// =======================================================================
// function : RemoveNote
// purpose  :
// =======================================================================
Standard_Boolean XCAFDoc_NotesTool::RemoveNote(const TDF_Label&              theNoteLabel,
                                               const XCAFDoc_AssemblyItemId& theItemId,
                                               Standard_Boolean              theDelIfOrphan)
{
  Handle(XCAFDoc_Note) aNote = XCAFDoc_Note::Get(theNoteLabel);

  if (aNote.IsNull())
    return Standard_False;

  Handle(XCAFDoc_GraphNode) aFather;
  if (!theNoteLabel.FindAttribute(XCAFDoc::NoteRefGUID(), aFather))
    return Standard_False;

  TDF_Label anAnnotatedItem = FindAnnotatedItem(theItemId);
  if (anAnnotatedItem.IsNull())
    return Standard_False;

  Handle(XCAFDoc_GraphNode) aChild;
  if (!anAnnotatedItem.FindAttribute(XCAFDoc::NoteRefGUID(), aChild))
    return Standard_False;

  aChild->UnSetFather(aFather);
  if (aChild->NbFathers() == 0)
    anAnnotatedItem.ForgetAllAttributes();

  if (theDelIfOrphan && aNote->IsOrphan())
    DeleteNote(theNoteLabel);

  return Standard_True;
}

// =======================================================================
// function : RemoveNote
// purpose  :
// =======================================================================
Standard_Boolean XCAFDoc_NotesTool::RemoveNote(const TDF_Label& theNoteLabel,
                                               const TDF_Label& theItemLabel,
                                               Standard_Boolean theDelIfOrphan)
{
  return RemoveNote(theNoteLabel, labeledItem(theItemLabel), theDelIfOrphan);
}

// =======================================================================
// function : RemoveSubshapeNote
// purpose  :
// =======================================================================
Standard_Boolean XCAFDoc_NotesTool::RemoveSubshapeNote(const TDF_Label&              theNoteLabel,
                                                       const XCAFDoc_AssemblyItemId& theItemId,
                                                       Standard_Integer theSubshapeIndex,
                                                       Standard_Boolean theDelIfOrphan)
{
  Handle(XCAFDoc_Note) aNote = XCAFDoc_Note::Get(theNoteLabel);

  if (aNote.IsNull())
    return Standard_False;

  Handle(XCAFDoc_GraphNode) aFather;
  if (!theNoteLabel.FindAttribute(XCAFDoc::NoteRefGUID(), aFather))
    return Standard_False;

  TDF_Label anAnnotatedItem = FindAnnotatedItemSubshape(theItemId, theSubshapeIndex);
  if (anAnnotatedItem.IsNull())
    return Standard_False;

  Handle(XCAFDoc_GraphNode) aChild;
  if (!anAnnotatedItem.FindAttribute(XCAFDoc::NoteRefGUID(), aChild))
    return Standard_False;

  aChild->UnSetFather(aFather);
  if (aChild->NbFathers() == 0)
    anAnnotatedItem.ForgetAllAttributes();

  if (theDelIfOrphan && aNote->IsOrphan())
    DeleteNote(theNoteLabel);

  return Standard_True;
}

// =======================================================================
// function : RemoveSubshapeNote
// purpose  :
// =======================================================================
Standard_Boolean XCAFDoc_NotesTool::RemoveSubshapeNote(const TDF_Label& theNoteLabel,
                                                       const TDF_Label& theItemLabel,
                                                       Standard_Integer theSubshapeIndex,
                                                       Standard_Boolean theDelIfOrphan)
{
  return RemoveSubshapeNote(theNoteLabel,
                            labeledItem(theItemLabel),
                            theSubshapeIndex,
                            theDelIfOrphan);
}

// =======================================================================
// function : RemoveAttrNote
// purpose  :
// =======================================================================
Standard_Boolean XCAFDoc_NotesTool::RemoveAttrNote(const TDF_Label&              theNoteLabel,
                                                   const XCAFDoc_AssemblyItemId& theItemId,
                                                   const Standard_GUID&          theGUID,
                                                   Standard_Boolean              theDelIfOrphan)
{
  Handle(XCAFDoc_Note) aNote = XCAFDoc_Note::Get(theNoteLabel);

  if (aNote.IsNull())
    return Standard_False;

  Handle(XCAFDoc_GraphNode) aFather;
  if (!theNoteLabel.FindAttribute(XCAFDoc::NoteRefGUID(), aFather))
    return Standard_False;

  TDF_Label anAnnotatedItem = FindAnnotatedItemAttr(theItemId, theGUID);
  if (anAnnotatedItem.IsNull())
    return Standard_False;

  Handle(XCAFDoc_GraphNode) aChild;
  if (!anAnnotatedItem.FindAttribute(XCAFDoc::NoteRefGUID(), aChild))
    return Standard_False;

  aChild->UnSetFather(aFather);
  if (aChild->NbFathers() == 0)
    anAnnotatedItem.ForgetAllAttributes();

  if (theDelIfOrphan && aNote->IsOrphan())
    DeleteNote(theNoteLabel);

  return Standard_True;
}

// =======================================================================
// function : RemoveAttrNote
// purpose  :
// =======================================================================
Standard_Boolean XCAFDoc_NotesTool::RemoveAttrNote(const TDF_Label&     theNoteLabel,
                                                   const TDF_Label&     theItemLabel,
                                                   const Standard_GUID& theGUID,
                                                   Standard_Boolean     theDelIfOrphan)
{
  return RemoveAttrNote(theNoteLabel, labeledItem(theItemLabel), theGUID, theDelIfOrphan);
}

// =======================================================================
// function : RemoveAllNotes
// purpose  :
// =======================================================================
Standard_Boolean XCAFDoc_NotesTool::RemoveAllNotes(const XCAFDoc_AssemblyItemId& theItemId,
                                                   Standard_Boolean              theDelIfOrphan)
{
  TDF_Label anAnnotatedItem = FindAnnotatedItem(theItemId);
  if (anAnnotatedItem.IsNull())
    return Standard_False;

  Handle(XCAFDoc_GraphNode) aChild;
  if (!anAnnotatedItem.FindAttribute(XCAFDoc::NoteRefGUID(), aChild))
    return Standard_False;

  while (aChild->NbFathers() > 0)
  {
    Handle(XCAFDoc_GraphNode) aFather = aChild->GetFather(1);
    Handle(XCAFDoc_Note)      aNote   = XCAFDoc_Note::Get(aFather->Label());
    if (!aNote.IsNull())
    {
      aFather->UnSetChild(aChild);
      if (theDelIfOrphan && aNote->IsOrphan())
        DeleteNote(aFather->Label());
    }
  }

  anAnnotatedItem.ForgetAllAttributes();

  return Standard_True;
}

// =======================================================================
// function : RemoveAllNotes
// purpose  :
// =======================================================================
Standard_Boolean XCAFDoc_NotesTool::RemoveAllNotes(const TDF_Label& theItemLabel,
                                                   Standard_Boolean theDelIfOrphan)
{
  return RemoveAllNotes(labeledItem(theItemLabel), theDelIfOrphan);
}

// =======================================================================
// function : RemoveAllSubshapeNotes
// purpose  :
// =======================================================================
Standard_Boolean XCAFDoc_NotesTool::RemoveAllSubshapeNotes(const XCAFDoc_AssemblyItemId& theItemId,
                                                           Standard_Integer theSubshapeIndex,
                                                           Standard_Boolean theDelIfOrphan)
{
  TDF_Label anAnnotatedItem = FindAnnotatedItemSubshape(theItemId, theSubshapeIndex);
  if (anAnnotatedItem.IsNull())
    return Standard_False;

  Handle(XCAFDoc_GraphNode) aChild;
  if (!anAnnotatedItem.FindAttribute(XCAFDoc::NoteRefGUID(), aChild))
    return Standard_False;

  while (aChild->NbFathers() > 0)
  {
    Handle(XCAFDoc_GraphNode) aFather = aChild->GetFather(1);
    Handle(XCAFDoc_Note)      aNote   = XCAFDoc_Note::Get(aFather->Label());
    if (!aNote.IsNull())
    {
      aFather->UnSetChild(aChild);
      if (theDelIfOrphan && aNote->IsOrphan())
        DeleteNote(aFather->Label());
    }
  }

  anAnnotatedItem.ForgetAllAttributes();

  return Standard_True;
}

// =======================================================================
// function : RemoveAllAttrNotes
// purpose  :
// =======================================================================
Standard_Boolean XCAFDoc_NotesTool::RemoveAllAttrNotes(const XCAFDoc_AssemblyItemId& theItemId,
                                                       const Standard_GUID&          theGUID,
                                                       Standard_Boolean              theDelIfOrphan)
{
  TDF_Label anAnnotatedItem = FindAnnotatedItemAttr(theItemId, theGUID);
  if (anAnnotatedItem.IsNull())
    return Standard_False;

  Handle(XCAFDoc_GraphNode) aChild;
  if (!anAnnotatedItem.FindAttribute(XCAFDoc::NoteRefGUID(), aChild))
    return Standard_False;

  while (aChild->NbFathers() > 0)
  {
    Handle(XCAFDoc_GraphNode) aFather = aChild->GetFather(1);
    Handle(XCAFDoc_Note)      aNote   = XCAFDoc_Note::Get(aFather->Label());
    if (!aNote.IsNull())
    {
      aFather->UnSetChild(aChild);
      if (theDelIfOrphan && aNote->IsOrphan())
        DeleteNote(aFather->Label());
    }
  }

  anAnnotatedItem.ForgetAllAttributes();

  return Standard_True;
}

// =======================================================================
// function : RemoveAllAttrNotes
// purpose  :
// =======================================================================
Standard_Boolean XCAFDoc_NotesTool::RemoveAllAttrNotes(const TDF_Label&     theItemLabel,
                                                       const Standard_GUID& theGUID,
                                                       Standard_Boolean     theDelIfOrphan)
{
  return RemoveAllAttrNotes(labeledItem(theItemLabel), theGUID, theDelIfOrphan);
}

// =======================================================================
// function : DeleteNote
// purpose  :
// =======================================================================
Standard_Boolean XCAFDoc_NotesTool::DeleteNote(const TDF_Label& theNoteLabel)
{
  Handle(XCAFDoc_Note) aNote = XCAFDoc_Note::Get(theNoteLabel);
  if (!aNote.IsNull())
  {
    Handle(XCAFDoc_GraphNode) aFather;
    if (theNoteLabel.FindAttribute(XCAFDoc::NoteRefGUID(), aFather) && !aFather.IsNull())
    {
      while (aFather->NbChildren() > 0)
      {
        Handle(XCAFDoc_GraphNode) aChild = aFather->GetChild(1);
        aFather->UnSetChild(aChild);
        if (aChild->NbFathers() == 0)
          aChild->Label().ForgetAllAttributes(Standard_True);
      }
    }
    theNoteLabel.ForgetAllAttributes(Standard_True);
    return Standard_True;
  }
  return Standard_False;
}

// =======================================================================
// function : DeleteNotes
// purpose  :
// =======================================================================
Standard_Integer XCAFDoc_NotesTool::DeleteNotes(TDF_LabelSequence& theNoteLabels)
{
  Standard_Integer nbNotes = 0;
  for (TDF_LabelSequence::Iterator anIter(theNoteLabels); anIter.More(); anIter.Next())
  {
    if (DeleteNote(anIter.Value()))
      ++nbNotes;
  }
  return nbNotes;
}

// =======================================================================
// function : DeleteAllNotes
// purpose  :
// =======================================================================
Standard_Integer XCAFDoc_NotesTool::DeleteAllNotes()
{
  Standard_Integer nbNotes = 0;
  for (TDF_ChildIterator anIter(GetNotesLabel()); anIter.More(); anIter.Next())
  {
    if (DeleteNote(anIter.Value()))
      ++nbNotes;
  }
  return nbNotes;
}

// =======================================================================
// function : NbOrphanNotes
// purpose  :
// =======================================================================
Standard_Integer XCAFDoc_NotesTool::NbOrphanNotes() const
{
  Standard_Integer nbNotes = 0;
  for (TDF_ChildIterator anIter(GetNotesLabel()); anIter.More(); anIter.Next())
  {
    const TDF_Label      aLabel = anIter.Value();
    Handle(XCAFDoc_Note) aNote  = XCAFDoc_Note::Get(aLabel);
    if (!aNote.IsNull() && aNote->IsOrphan())
      ++nbNotes;
  }
  return nbNotes;
}

// =======================================================================
// function : GetOrphanNotes
// purpose  :
// =======================================================================
void XCAFDoc_NotesTool::GetOrphanNotes(TDF_LabelSequence& theNoteLabels) const
{
  for (TDF_ChildIterator anIter(GetNotesLabel()); anIter.More(); anIter.Next())
  {
    const TDF_Label      aLabel = anIter.Value();
    Handle(XCAFDoc_Note) aNote  = XCAFDoc_Note::Get(aLabel);
    if (!aNote.IsNull() && aNote->IsOrphan())
      theNoteLabels.Append(aLabel);
  }
}

// =======================================================================
// function : DeleteOrphanNotes
// purpose  :
// =======================================================================
Standard_Integer XCAFDoc_NotesTool::DeleteOrphanNotes()
{
  Standard_Integer nbNotes = 0;
  for (TDF_ChildIterator anIter(GetNotesLabel()); anIter.More(); anIter.Next())
  {
    const TDF_Label      aLabel = anIter.Value();
    Handle(XCAFDoc_Note) aNote  = XCAFDoc_Note::Get(aLabel);
    if (!aNote.IsNull() && aNote->IsOrphan() && DeleteNote(aLabel))
      ++nbNotes;
  }
  return nbNotes;
}

// =======================================================================
// function : ID
// purpose  :
// =======================================================================
const Standard_GUID& XCAFDoc_NotesTool::ID() const
{
  return GetID();
}

// =======================================================================
// function : Dump
// purpose  :
// =======================================================================
Standard_OStream& XCAFDoc_NotesTool::Dump(Standard_OStream& theOS) const
{
  theOS << "Notes           : " << NbNotes() << "\n"
        << "Annotated items : " << NbAnnotatedItems() << "\n";
  return theOS;
}
