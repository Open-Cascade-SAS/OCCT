// Created by: DAUTRY Philippe
// Copyright (c) 1997-1999 Matra Datavision
// Copyright (c) 1999-2014 OPEN CASCADE SAS
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

//      	--------------
// Version:	0.0
// Version	Date		Purpose
//		0.0	Sep 15 1997	Creation

#include <TDocStd_XLink.hxx>

#include <Standard_Type.hxx>
#include <TCollection_AsciiString.hxx>
#include <TDF_Attribute.hxx>
#include <TDF_AttributeDelta.hxx>
#include <TDF_DeltaOnAddition.hxx>
#include <TDF_DeltaOnRemoval.hxx>
#include <TDF_Label.hxx>
#include <TDF_Reference.hxx>
#include <TDF_RelocationTable.hxx>
#include <TDF_Tool.hxx>
#include <TDocStd.hxx>
#include <TDocStd_Document.hxx>
#include <TDocStd_XLinkRoot.hxx>

IMPLEMENT_STANDARD_RTTIEXT(TDocStd_XLink, TDF_Attribute)

//=================================================================================================

TDocStd_XLink::TDocStd_XLink()
    : myNext(nullptr)
{
}

//=================================================================================================

occ::handle<TDocStd_XLink> TDocStd_XLink::Set(const TDF_Label& atLabel)
{
  occ::handle<TDocStd_XLink> xRef;
  if (!atLabel.FindAttribute(TDocStd_XLink::GetID(), xRef))
  {
    xRef = new TDocStd_XLink;
    atLabel.AddAttribute(xRef);
  }
  return xRef;
}

//=================================================================================================

occ::handle<TDF_Reference> TDocStd_XLink::Update()
{
  TDF_Label                     reflabel;
  occ::handle<TDocStd_Document> refdoc;
  int                           IEntry = myDocEntry.IntegerValue();
  occ::handle<TDocStd_Document> mydoc  = TDocStd_Document::Get(Label()); // mon document
  refdoc                               = occ::down_cast<TDocStd_Document>(mydoc->Document(IEntry));
  TDF_Tool::Label(refdoc->GetData(), myLabelEntry, reflabel);
  // return TXLink::Import(reflabel,Label());
  return TDF_Reference::Set(Label(), reflabel);
}

//=================================================================================================

const Standard_GUID& TDocStd_XLink::ID() const
{
  return GetID();
}

//=================================================================================================

const Standard_GUID& TDocStd_XLink::GetID()
{
  static Standard_GUID myID("5d587400-5690-11d1-8940-080009dc3333");
  return myID;
}

//=================================================================================================

void TDocStd_XLink::DocumentEntry(const TCollection_AsciiString& aDocEntry)
{
  Backup();
  myDocEntry = aDocEntry;
}

//=================================================================================================

const TCollection_AsciiString& TDocStd_XLink::DocumentEntry() const
{
  return myDocEntry;
}

//=================================================================================================

void TDocStd_XLink::LabelEntry(const TDF_Label& aLabel)
{
  Backup();
  TDF_Tool::Entry(aLabel, myLabelEntry);
}

//=================================================================================================

void TDocStd_XLink::LabelEntry(const TCollection_AsciiString& aLabEntry)
{
  Backup();
  myLabelEntry = aLabEntry;
}

//=================================================================================================

const TCollection_AsciiString& TDocStd_XLink::LabelEntry() const
{
  return myLabelEntry;
}

//=================================================================================================

void TDocStd_XLink::AfterAddition()
{
  TDocStd_XLinkRoot::Insert(this);
  Label().Imported(true);
}

//=================================================================================================

void TDocStd_XLink::BeforeRemoval()
{
  if (!IsBackuped())
  {
    TDocStd_XLinkRoot::Remove(this);
    Label().Imported(false);
  }
}

//=======================================================================
// function : BeforeUndo
// purpose  : Before application of a TDF_Delta.
//=======================================================================

bool TDocStd_XLink::BeforeUndo(const occ::handle<TDF_AttributeDelta>& anAttDelta,
                               const bool /*forceIt*/)
{
  if (anAttDelta->IsKind(STANDARD_TYPE(TDF_DeltaOnAddition)))
  {
    anAttDelta->Attribute()->BeforeRemoval();
  }
  return true;
}

//=======================================================================
// function : AfterUndo
// purpose  : After application of a TDF_Delta.
//=======================================================================

bool TDocStd_XLink::AfterUndo(const occ::handle<TDF_AttributeDelta>& anAttDelta,
                              const bool /*forceIt*/)
{
  if (anAttDelta->IsKind(STANDARD_TYPE(TDF_DeltaOnRemoval)))
  {
    anAttDelta->Attribute()->AfterAddition();
  }
  return true;
}

//=================================================================================================

occ::handle<TDF_Attribute> TDocStd_XLink::BackupCopy() const
{
  occ::handle<TDocStd_XLink> xRef = new TDocStd_XLink();
  xRef->DocumentEntry(myDocEntry);
  xRef->LabelEntry(myLabelEntry);
  return xRef;
}

//=================================================================================================

void TDocStd_XLink::Restore(const occ::handle<TDF_Attribute>& anAttribute)
{
  occ::handle<TDocStd_XLink> xRef(occ::down_cast<TDocStd_XLink>(anAttribute));
  if (!xRef.IsNull())
  {
    myDocEntry   = xRef->DocumentEntry();
    myLabelEntry = xRef->LabelEntry();
  }
}

//=================================================================================================

occ::handle<TDF_Attribute> TDocStd_XLink::NewEmpty() const
{
  return new TDocStd_XLink();
}

//=================================================================================================

void TDocStd_XLink::Paste(const occ::handle<TDF_Attribute>& intoAttribute,
                          const occ::handle<TDF_RelocationTable>& /*aRelocationTable*/) const
{
  occ::handle<TDocStd_XLink> xRef(occ::down_cast<TDocStd_XLink>(intoAttribute));
  if (!xRef.IsNull())
  {
    xRef->DocumentEntry(myDocEntry);
    xRef->LabelEntry(myLabelEntry);
  }
}

//=================================================================================================

Standard_OStream& TDocStd_XLink::Dump(Standard_OStream& anOS) const
{
  return anOS;
}
