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

#include <NCollection_Array1.hxx>
#include <NCollection_HArray1.hxx>
#include <TDF_Label.hxx>
#include <TDF_ChildIDIterator.hxx>
#include <NCollection_Sequence.hxx>
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

//=================================================================================================

const Standard_GUID& XCAFDoc_NotesTool::GetID()
{
  static Standard_GUID s_ID("8F8174B1-6125-47a0-B357-61BD2D89380C");
  return s_ID;
}

//=================================================================================================

occ::handle<XCAFDoc_NotesTool> XCAFDoc_NotesTool::Set(const TDF_Label& theLabel)
{
  occ::handle<XCAFDoc_NotesTool> aTool;
  if (!theLabel.IsNull() && !theLabel.FindAttribute(XCAFDoc_NotesTool::GetID(), aTool))
  {
    aTool = new XCAFDoc_NotesTool();
    theLabel.AddAttribute(aTool);
  }
  return aTool;
}

//=================================================================================================

XCAFDoc_NotesTool::XCAFDoc_NotesTool() {}

//=================================================================================================

TDF_Label XCAFDoc_NotesTool::GetNotesLabel() const
{
  return Label().FindChild(NotesTool_NotesRoot);
}

//=================================================================================================

TDF_Label XCAFDoc_NotesTool::GetAnnotatedItemsLabel() const
{
  return Label().FindChild(NotesTool_AnnotatedItemsRoot);
}

//=================================================================================================

int XCAFDoc_NotesTool::NbNotes() const
{
  int nbNotes = 0;
  for (TDF_ChildIterator anIter(GetNotesLabel()); anIter.More(); anIter.Next())
  {
    const TDF_Label aLabel = anIter.Value();
    if (!XCAFDoc_Note::Get(aLabel).IsNull())
      ++nbNotes;
  }
  return nbNotes;
}

//=================================================================================================

int XCAFDoc_NotesTool::NbAnnotatedItems() const
{
  int nbItems = 0;
  for (TDF_ChildIDIterator anIter(GetAnnotatedItemsLabel(), XCAFDoc_AssemblyItemRef::GetID());
       anIter.More();
       anIter.Next())
  {
    ++nbItems;
  }
  return nbItems;
}

//=================================================================================================

void XCAFDoc_NotesTool::GetNotes(NCollection_Sequence<TDF_Label>& theNoteLabels) const
{
  for (TDF_ChildIterator anIter(GetNotesLabel()); anIter.More(); anIter.Next())
  {
    const TDF_Label aLabel = anIter.Value();
    if (!XCAFDoc_Note::Get(aLabel).IsNull())
      theNoteLabels.Append(aLabel);
  }
}

//=================================================================================================

void XCAFDoc_NotesTool::GetAnnotatedItems(NCollection_Sequence<TDF_Label>& theItemLabels) const
{
  for (TDF_ChildIDIterator anIter(GetAnnotatedItemsLabel(), XCAFDoc_AssemblyItemRef::GetID());
       anIter.More();
       anIter.Next())
  {
    theItemLabels.Append(anIter.Value()->Label());
  }
}

//=================================================================================================

bool XCAFDoc_NotesTool::IsAnnotatedItem(const XCAFDoc_AssemblyItemId& theItemId) const
{
  return !FindAnnotatedItem(theItemId).IsNull();
}

//=================================================================================================

bool XCAFDoc_NotesTool::IsAnnotatedItem(const TDF_Label& theItemLabel) const
{
  return IsAnnotatedItem(labeledItem(theItemLabel));
}

//=================================================================================================

TDF_Label XCAFDoc_NotesTool::FindAnnotatedItem(const XCAFDoc_AssemblyItemId& theItemId) const
{
  for (TDF_ChildIDIterator anIter(GetAnnotatedItemsLabel(), XCAFDoc_AssemblyItemRef::GetID());
       anIter.More();
       anIter.Next())
  {
    occ::handle<XCAFDoc_AssemblyItemRef> anItemRef =
      occ::down_cast<XCAFDoc_AssemblyItemRef>(anIter.Value());
    if (!anItemRef.IsNull() && anItemRef->GetItem().IsEqual(theItemId) && !anItemRef->HasExtraRef())
      return anItemRef->Label();
  }
  return TDF_Label();
}

//=================================================================================================

TDF_Label XCAFDoc_NotesTool::FindAnnotatedItem(const TDF_Label& theItemLabel) const
{
  return FindAnnotatedItem(labeledItem(theItemLabel));
}

//=================================================================================================

TDF_Label XCAFDoc_NotesTool::FindAnnotatedItemAttr(const XCAFDoc_AssemblyItemId& theItemId,
                                                   const Standard_GUID&          theGUID) const
{
  for (TDF_ChildIDIterator anIter(GetAnnotatedItemsLabel(), XCAFDoc_AssemblyItemRef::GetID());
       anIter.More();
       anIter.Next())
  {
    occ::handle<XCAFDoc_AssemblyItemRef> anItemRef =
      occ::down_cast<XCAFDoc_AssemblyItemRef>(anIter.Value());
    if (!anItemRef.IsNull() && anItemRef->GetItem().IsEqual(theItemId) && anItemRef->HasExtraRef()
        && anItemRef->GetGUID() == theGUID)
      return anItemRef->Label();
  }
  return TDF_Label();
}

//=================================================================================================

TDF_Label XCAFDoc_NotesTool::FindAnnotatedItemAttr(const TDF_Label&     theItemLabel,
                                                   const Standard_GUID& theGUID) const
{
  return FindAnnotatedItemAttr(labeledItem(theItemLabel), theGUID);
}

//=================================================================================================

TDF_Label XCAFDoc_NotesTool::FindAnnotatedItemSubshape(const XCAFDoc_AssemblyItemId& theItemId,
                                                       int theSubshapeIndex) const
{
  for (TDF_ChildIDIterator anIter(GetAnnotatedItemsLabel(), XCAFDoc_AssemblyItemRef::GetID());
       anIter.More();
       anIter.Next())
  {
    occ::handle<XCAFDoc_AssemblyItemRef> anItemRef =
      occ::down_cast<XCAFDoc_AssemblyItemRef>(anIter.Value());
    if (!anItemRef.IsNull() && anItemRef->GetItem().IsEqual(theItemId) && anItemRef->HasExtraRef()
        && anItemRef->GetSubshapeIndex() == theSubshapeIndex)
      return anItemRef->Label();
  }
  return TDF_Label();
}

//=================================================================================================

TDF_Label XCAFDoc_NotesTool::FindAnnotatedItemSubshape(const TDF_Label& theItemLabel,
                                                       int              theSubshapeIndex) const
{
  return FindAnnotatedItemSubshape(labeledItem(theItemLabel), theSubshapeIndex);
}

//=================================================================================================

occ::handle<XCAFDoc_Note> XCAFDoc_NotesTool::CreateComment(
  const TCollection_ExtendedString& theUserName,
  const TCollection_ExtendedString& theTimeStamp,
  const TCollection_ExtendedString& theComment)
{
  TDF_Label     aNoteLabel;
  TDF_TagSource aTag;
  aNoteLabel = aTag.NewChild(GetNotesLabel());
  return XCAFDoc_NoteComment::Set(aNoteLabel, theUserName, theTimeStamp, theComment);
}

//=================================================================================================

occ::handle<XCAFDoc_Note> XCAFDoc_NotesTool::CreateBalloon(
  const TCollection_ExtendedString& theUserName,
  const TCollection_ExtendedString& theTimeStamp,
  const TCollection_ExtendedString& theComment)
{
  TDF_Label     aNoteLabel;
  TDF_TagSource aTag;
  aNoteLabel = aTag.NewChild(GetNotesLabel());
  return XCAFDoc_NoteBalloon::Set(aNoteLabel, theUserName, theTimeStamp, theComment);
}

//=================================================================================================

occ::handle<XCAFDoc_Note> XCAFDoc_NotesTool::CreateBinData(
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

//=================================================================================================

occ::handle<XCAFDoc_Note> XCAFDoc_NotesTool::CreateBinData(
  const TCollection_ExtendedString&                theUserName,
  const TCollection_ExtendedString&                theTimeStamp,
  const TCollection_ExtendedString&                theTitle,
  const TCollection_AsciiString&                   theMIMEtype,
  const occ::handle<NCollection_HArray1<uint8_t>>& theData)
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

//=================================================================================================

int XCAFDoc_NotesTool::GetNotes(const XCAFDoc_AssemblyItemId&    theItemId,
                                NCollection_Sequence<TDF_Label>& theNoteLabels) const
{
  TDF_Label anAnnotatedItem = FindAnnotatedItem(theItemId);
  if (anAnnotatedItem.IsNull())
    return 0;

  occ::handle<XCAFDoc_GraphNode> aChild;
  if (!anAnnotatedItem.FindAttribute(XCAFDoc::NoteRefGUID(), aChild))
    return 0;

  int nbFathers = aChild->NbFathers();
  for (int iFather = 1; iFather <= nbFathers; ++iFather)
  {
    occ::handle<XCAFDoc_GraphNode> aFather = aChild->GetFather(iFather);
    theNoteLabels.Append(aFather->Label());
  }

  return theNoteLabels.Length();
}

//=================================================================================================

int XCAFDoc_NotesTool::GetNotes(const TDF_Label&                 theItemLabel,
                                NCollection_Sequence<TDF_Label>& theNoteLabels) const
{
  return GetNotes(labeledItem(theItemLabel), theNoteLabels);
}

//=================================================================================================

int XCAFDoc_NotesTool::GetAttrNotes(const XCAFDoc_AssemblyItemId&    theItemId,
                                    const Standard_GUID&             theGUID,
                                    NCollection_Sequence<TDF_Label>& theNoteLabels) const
{
  TDF_Label anAnnotatedItem = FindAnnotatedItemAttr(theItemId, theGUID);
  if (anAnnotatedItem.IsNull())
    return 0;

  occ::handle<XCAFDoc_GraphNode> aChild;
  if (!anAnnotatedItem.FindAttribute(XCAFDoc::NoteRefGUID(), aChild))
    return 0;

  int nbFathers = aChild->NbFathers();
  for (int iFather = 1; iFather <= nbFathers; ++iFather)
  {
    occ::handle<XCAFDoc_GraphNode> aFather = aChild->GetFather(iFather);
    theNoteLabels.Append(aFather->Label());
  }

  return theNoteLabels.Length();
}

//=================================================================================================

int XCAFDoc_NotesTool::GetAttrNotes(const TDF_Label&                 theItemLabel,
                                    const Standard_GUID&             theGUID,
                                    NCollection_Sequence<TDF_Label>& theNoteLabels) const
{
  return GetAttrNotes(labeledItem(theItemLabel), theGUID, theNoteLabels);
}

//=================================================================================================

int XCAFDoc_NotesTool::GetSubshapeNotes(const XCAFDoc_AssemblyItemId&    theItemId,
                                        int                              theSubshapeIndex,
                                        NCollection_Sequence<TDF_Label>& theNoteLabels) const
{
  TDF_Label anAnnotatedItem = FindAnnotatedItemSubshape(theItemId, theSubshapeIndex);
  if (anAnnotatedItem.IsNull())
    return 0;

  occ::handle<XCAFDoc_GraphNode> aChild;
  if (!anAnnotatedItem.FindAttribute(XCAFDoc::NoteRefGUID(), aChild))
    return 0;

  int nbFathers = aChild->NbFathers();
  for (int iFather = 1; iFather <= nbFathers; ++iFather)
  {
    occ::handle<XCAFDoc_GraphNode> aFather = aChild->GetFather(iFather);
    theNoteLabels.Append(aFather->Label());
  }

  return theNoteLabels.Length();
}

//=================================================================================================

occ::handle<XCAFDoc_AssemblyItemRef> XCAFDoc_NotesTool::AddNote(
  const TDF_Label&              theNoteLabel,
  const XCAFDoc_AssemblyItemId& theItemId)
{
  occ::handle<XCAFDoc_AssemblyItemRef> anItemRef;

  if (!XCAFDoc_Note::IsMine(theNoteLabel))
    return anItemRef;

  occ::handle<XCAFDoc_GraphNode> aChild;
  TDF_Label                      anAnnotatedItem = FindAnnotatedItem(theItemId);
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

  occ::handle<XCAFDoc_GraphNode> aFather;
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

//=================================================================================================

occ::handle<XCAFDoc_AssemblyItemRef> XCAFDoc_NotesTool::AddNote(const TDF_Label& theNoteLabel,
                                                                const TDF_Label& theItemLabel)
{
  return AddNote(theNoteLabel, labeledItem(theItemLabel));
}

//=================================================================================================

occ::handle<XCAFDoc_AssemblyItemRef> XCAFDoc_NotesTool::AddNoteToAttr(
  const TDF_Label&              theNoteLabel,
  const XCAFDoc_AssemblyItemId& theItemId,
  const Standard_GUID&          theGUID)
{
  occ::handle<XCAFDoc_AssemblyItemRef> anItemRef;

  if (!XCAFDoc_Note::IsMine(theNoteLabel))
    return anItemRef;

  occ::handle<XCAFDoc_GraphNode> aChild;
  TDF_Label                      anAnnotatedItem = FindAnnotatedItemAttr(theItemId, theGUID);
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

  occ::handle<XCAFDoc_GraphNode> aFather;
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

//=================================================================================================

occ::handle<XCAFDoc_AssemblyItemRef> XCAFDoc_NotesTool::AddNoteToAttr(const TDF_Label& theNoteLabel,
                                                                      const TDF_Label& theItemLabel,
                                                                      const Standard_GUID& theGUID)
{
  return AddNoteToAttr(theNoteLabel, labeledItem(theItemLabel), theGUID);
}

//=================================================================================================

occ::handle<XCAFDoc_AssemblyItemRef> XCAFDoc_NotesTool::AddNoteToSubshape(
  const TDF_Label&              theNoteLabel,
  const XCAFDoc_AssemblyItemId& theItemId,
  int                           theSubshapeIndex)
{
  occ::handle<XCAFDoc_AssemblyItemRef> anItemRef;

  if (!XCAFDoc_Note::IsMine(theNoteLabel))
    return anItemRef;

  occ::handle<XCAFDoc_GraphNode> aChild;
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

  occ::handle<XCAFDoc_GraphNode> aFather;
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

//=================================================================================================

occ::handle<XCAFDoc_AssemblyItemRef> XCAFDoc_NotesTool::AddNoteToSubshape(
  const TDF_Label& theNoteLabel,
  const TDF_Label& theItemLabel,
  int              theSubshapeIndex)
{
  return AddNoteToSubshape(theNoteLabel, labeledItem(theItemLabel), theSubshapeIndex);
}

//=================================================================================================

bool XCAFDoc_NotesTool::RemoveNote(const TDF_Label&              theNoteLabel,
                                   const XCAFDoc_AssemblyItemId& theItemId,
                                   bool                          theDelIfOrphan)
{
  occ::handle<XCAFDoc_Note> aNote = XCAFDoc_Note::Get(theNoteLabel);

  if (aNote.IsNull())
    return false;

  occ::handle<XCAFDoc_GraphNode> aFather;
  if (!theNoteLabel.FindAttribute(XCAFDoc::NoteRefGUID(), aFather))
    return false;

  TDF_Label anAnnotatedItem = FindAnnotatedItem(theItemId);
  if (anAnnotatedItem.IsNull())
    return false;

  occ::handle<XCAFDoc_GraphNode> aChild;
  if (!anAnnotatedItem.FindAttribute(XCAFDoc::NoteRefGUID(), aChild))
    return false;

  aChild->UnSetFather(aFather);
  if (aChild->NbFathers() == 0)
    anAnnotatedItem.ForgetAllAttributes();

  if (theDelIfOrphan && aNote->IsOrphan())
    DeleteNote(theNoteLabel);

  return true;
}

//=================================================================================================

bool XCAFDoc_NotesTool::RemoveNote(const TDF_Label& theNoteLabel,
                                   const TDF_Label& theItemLabel,
                                   bool             theDelIfOrphan)
{
  return RemoveNote(theNoteLabel, labeledItem(theItemLabel), theDelIfOrphan);
}

//=================================================================================================

bool XCAFDoc_NotesTool::RemoveSubshapeNote(const TDF_Label&              theNoteLabel,
                                           const XCAFDoc_AssemblyItemId& theItemId,
                                           int                           theSubshapeIndex,
                                           bool                          theDelIfOrphan)
{
  occ::handle<XCAFDoc_Note> aNote = XCAFDoc_Note::Get(theNoteLabel);

  if (aNote.IsNull())
    return false;

  occ::handle<XCAFDoc_GraphNode> aFather;
  if (!theNoteLabel.FindAttribute(XCAFDoc::NoteRefGUID(), aFather))
    return false;

  TDF_Label anAnnotatedItem = FindAnnotatedItemSubshape(theItemId, theSubshapeIndex);
  if (anAnnotatedItem.IsNull())
    return false;

  occ::handle<XCAFDoc_GraphNode> aChild;
  if (!anAnnotatedItem.FindAttribute(XCAFDoc::NoteRefGUID(), aChild))
    return false;

  aChild->UnSetFather(aFather);
  if (aChild->NbFathers() == 0)
    anAnnotatedItem.ForgetAllAttributes();

  if (theDelIfOrphan && aNote->IsOrphan())
    DeleteNote(theNoteLabel);

  return true;
}

//=================================================================================================

bool XCAFDoc_NotesTool::RemoveSubshapeNote(const TDF_Label& theNoteLabel,
                                           const TDF_Label& theItemLabel,
                                           int              theSubshapeIndex,
                                           bool             theDelIfOrphan)
{
  return RemoveSubshapeNote(theNoteLabel,
                            labeledItem(theItemLabel),
                            theSubshapeIndex,
                            theDelIfOrphan);
}

//=================================================================================================

bool XCAFDoc_NotesTool::RemoveAttrNote(const TDF_Label&              theNoteLabel,
                                       const XCAFDoc_AssemblyItemId& theItemId,
                                       const Standard_GUID&          theGUID,
                                       bool                          theDelIfOrphan)
{
  occ::handle<XCAFDoc_Note> aNote = XCAFDoc_Note::Get(theNoteLabel);

  if (aNote.IsNull())
    return false;

  occ::handle<XCAFDoc_GraphNode> aFather;
  if (!theNoteLabel.FindAttribute(XCAFDoc::NoteRefGUID(), aFather))
    return false;

  TDF_Label anAnnotatedItem = FindAnnotatedItemAttr(theItemId, theGUID);
  if (anAnnotatedItem.IsNull())
    return false;

  occ::handle<XCAFDoc_GraphNode> aChild;
  if (!anAnnotatedItem.FindAttribute(XCAFDoc::NoteRefGUID(), aChild))
    return false;

  aChild->UnSetFather(aFather);
  if (aChild->NbFathers() == 0)
    anAnnotatedItem.ForgetAllAttributes();

  if (theDelIfOrphan && aNote->IsOrphan())
    DeleteNote(theNoteLabel);

  return true;
}

//=================================================================================================

bool XCAFDoc_NotesTool::RemoveAttrNote(const TDF_Label&     theNoteLabel,
                                       const TDF_Label&     theItemLabel,
                                       const Standard_GUID& theGUID,
                                       bool                 theDelIfOrphan)
{
  return RemoveAttrNote(theNoteLabel, labeledItem(theItemLabel), theGUID, theDelIfOrphan);
}

//=================================================================================================

bool XCAFDoc_NotesTool::RemoveAllNotes(const XCAFDoc_AssemblyItemId& theItemId, bool theDelIfOrphan)
{
  TDF_Label anAnnotatedItem = FindAnnotatedItem(theItemId);
  if (anAnnotatedItem.IsNull())
    return false;

  occ::handle<XCAFDoc_GraphNode> aChild;
  if (!anAnnotatedItem.FindAttribute(XCAFDoc::NoteRefGUID(), aChild))
    return false;

  while (aChild->NbFathers() > 0)
  {
    occ::handle<XCAFDoc_GraphNode> aFather = aChild->GetFather(1);
    occ::handle<XCAFDoc_Note>      aNote   = XCAFDoc_Note::Get(aFather->Label());
    if (!aNote.IsNull())
    {
      aFather->UnSetChild(aChild);
      if (theDelIfOrphan && aNote->IsOrphan())
        DeleteNote(aFather->Label());
    }
  }

  anAnnotatedItem.ForgetAllAttributes();

  return true;
}

//=================================================================================================

bool XCAFDoc_NotesTool::RemoveAllNotes(const TDF_Label& theItemLabel, bool theDelIfOrphan)
{
  return RemoveAllNotes(labeledItem(theItemLabel), theDelIfOrphan);
}

//=================================================================================================

bool XCAFDoc_NotesTool::RemoveAllSubshapeNotes(const XCAFDoc_AssemblyItemId& theItemId,
                                               int                           theSubshapeIndex,
                                               bool                          theDelIfOrphan)
{
  TDF_Label anAnnotatedItem = FindAnnotatedItemSubshape(theItemId, theSubshapeIndex);
  if (anAnnotatedItem.IsNull())
    return false;

  occ::handle<XCAFDoc_GraphNode> aChild;
  if (!anAnnotatedItem.FindAttribute(XCAFDoc::NoteRefGUID(), aChild))
    return false;

  while (aChild->NbFathers() > 0)
  {
    occ::handle<XCAFDoc_GraphNode> aFather = aChild->GetFather(1);
    occ::handle<XCAFDoc_Note>      aNote   = XCAFDoc_Note::Get(aFather->Label());
    if (!aNote.IsNull())
    {
      aFather->UnSetChild(aChild);
      if (theDelIfOrphan && aNote->IsOrphan())
        DeleteNote(aFather->Label());
    }
  }

  anAnnotatedItem.ForgetAllAttributes();

  return true;
}

//=================================================================================================

bool XCAFDoc_NotesTool::RemoveAllAttrNotes(const XCAFDoc_AssemblyItemId& theItemId,
                                           const Standard_GUID&          theGUID,
                                           bool                          theDelIfOrphan)
{
  TDF_Label anAnnotatedItem = FindAnnotatedItemAttr(theItemId, theGUID);
  if (anAnnotatedItem.IsNull())
    return false;

  occ::handle<XCAFDoc_GraphNode> aChild;
  if (!anAnnotatedItem.FindAttribute(XCAFDoc::NoteRefGUID(), aChild))
    return false;

  while (aChild->NbFathers() > 0)
  {
    occ::handle<XCAFDoc_GraphNode> aFather = aChild->GetFather(1);
    occ::handle<XCAFDoc_Note>      aNote   = XCAFDoc_Note::Get(aFather->Label());
    if (!aNote.IsNull())
    {
      aFather->UnSetChild(aChild);
      if (theDelIfOrphan && aNote->IsOrphan())
        DeleteNote(aFather->Label());
    }
  }

  anAnnotatedItem.ForgetAllAttributes();

  return true;
}

//=================================================================================================

bool XCAFDoc_NotesTool::RemoveAllAttrNotes(const TDF_Label&     theItemLabel,
                                           const Standard_GUID& theGUID,
                                           bool                 theDelIfOrphan)
{
  return RemoveAllAttrNotes(labeledItem(theItemLabel), theGUID, theDelIfOrphan);
}

//=================================================================================================

bool XCAFDoc_NotesTool::DeleteNote(const TDF_Label& theNoteLabel)
{
  occ::handle<XCAFDoc_Note> aNote = XCAFDoc_Note::Get(theNoteLabel);
  if (!aNote.IsNull())
  {
    occ::handle<XCAFDoc_GraphNode> aFather;
    if (theNoteLabel.FindAttribute(XCAFDoc::NoteRefGUID(), aFather) && !aFather.IsNull())
    {
      while (aFather->NbChildren() > 0)
      {
        occ::handle<XCAFDoc_GraphNode> aChild = aFather->GetChild(1);
        aFather->UnSetChild(aChild);
        if (aChild->NbFathers() == 0)
          aChild->Label().ForgetAllAttributes(true);
      }
    }
    theNoteLabel.ForgetAllAttributes(true);
    return true;
  }
  return false;
}

//=================================================================================================

int XCAFDoc_NotesTool::DeleteNotes(NCollection_Sequence<TDF_Label>& theNoteLabels)
{
  int nbNotes = 0;
  for (NCollection_Sequence<TDF_Label>::Iterator anIter(theNoteLabels); anIter.More();
       anIter.Next())
  {
    if (DeleteNote(anIter.Value()))
      ++nbNotes;
  }
  return nbNotes;
}

//=================================================================================================

int XCAFDoc_NotesTool::DeleteAllNotes()
{
  int nbNotes = 0;
  for (TDF_ChildIterator anIter(GetNotesLabel()); anIter.More(); anIter.Next())
  {
    if (DeleteNote(anIter.Value()))
      ++nbNotes;
  }
  return nbNotes;
}

//=================================================================================================

int XCAFDoc_NotesTool::NbOrphanNotes() const
{
  int nbNotes = 0;
  for (TDF_ChildIterator anIter(GetNotesLabel()); anIter.More(); anIter.Next())
  {
    const TDF_Label           aLabel = anIter.Value();
    occ::handle<XCAFDoc_Note> aNote  = XCAFDoc_Note::Get(aLabel);
    if (!aNote.IsNull() && aNote->IsOrphan())
      ++nbNotes;
  }
  return nbNotes;
}

//=================================================================================================

void XCAFDoc_NotesTool::GetOrphanNotes(NCollection_Sequence<TDF_Label>& theNoteLabels) const
{
  for (TDF_ChildIterator anIter(GetNotesLabel()); anIter.More(); anIter.Next())
  {
    const TDF_Label           aLabel = anIter.Value();
    occ::handle<XCAFDoc_Note> aNote  = XCAFDoc_Note::Get(aLabel);
    if (!aNote.IsNull() && aNote->IsOrphan())
      theNoteLabels.Append(aLabel);
  }
}

//=================================================================================================

int XCAFDoc_NotesTool::DeleteOrphanNotes()
{
  int nbNotes = 0;
  for (TDF_ChildIterator anIter(GetNotesLabel()); anIter.More(); anIter.Next())
  {
    const TDF_Label           aLabel = anIter.Value();
    occ::handle<XCAFDoc_Note> aNote  = XCAFDoc_Note::Get(aLabel);
    if (!aNote.IsNull() && aNote->IsOrphan() && DeleteNote(aLabel))
      ++nbNotes;
  }
  return nbNotes;
}

//=================================================================================================

const Standard_GUID& XCAFDoc_NotesTool::ID() const
{
  return GetID();
}

//=================================================================================================

Standard_OStream& XCAFDoc_NotesTool::Dump(Standard_OStream& theOS) const
{
  theOS << "Notes           : " << NbNotes() << "\n"
        << "Annotated items : " << NbAnnotatedItems() << "\n";
  return theOS;
}
