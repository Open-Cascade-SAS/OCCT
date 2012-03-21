// Created by: DAUTRY Philippe
// Copyright (c) 1997-1999 Matra Datavision
// Copyright (c) 1999-2012 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.

//      	--------------

// Version:	0.0
//Version	Date		Purpose
//		0.0	Sep 15 1997	Creation



#include <TDocStd_XLink.ixx>

#include <Standard_NotImplemented.hxx>
#include <TDF_DeltaOnAddition.hxx>
#include <TDF_DeltaOnRemoval.hxx>
#include <TDF_Tool.hxx>
#include <TDF_Reference.hxx>
#include <TDocStd.hxx>
#include <TDocStd_Document.hxx>
#include <TDocStd_XLinkRoot.hxx>


//=======================================================================
//function : TDocStd_XLink
//purpose  : 
//=======================================================================

TDocStd_XLink::TDocStd_XLink()
: myNext(NULL)
{}


//=======================================================================
//function : Set
//purpose  : Class method.
//=======================================================================

Handle(TDocStd_XLink) TDocStd_XLink::Set (const TDF_Label& atLabel) 
{
  Handle(TDocStd_XLink) xRef;
  if (!atLabel.FindAttribute(TDocStd_XLink::GetID(),xRef)) {
    xRef = new TDocStd_XLink;  
    atLabel.AddAttribute(xRef);
  }
  return xRef;
}

//=======================================================================
//function : Update
//purpose  : 
//=======================================================================

Handle(TDF_Reference) TDocStd_XLink::Update ()
{
  TDF_Label reflabel;  
  Handle(TDocStd_Document) refdoc;
  Standard_Integer IEntry = myDocEntry.IntegerValue();
  Handle(TDocStd_Document) mydoc = TDocStd_Document::Get(Label()); //mon document
  refdoc = Handle(TDocStd_Document)::DownCast(mydoc->Document(IEntry));
  TDF_Tool::Label(refdoc->GetData(),myLabelEntry,reflabel);
  // return TXLink::Import(reflabel,Label());
  return TDF_Reference::Set(Label(),reflabel);
}


//=======================================================================
//function : ID
//purpose  : 
//=======================================================================

const Standard_GUID& TDocStd_XLink::ID() const
{ return GetID(); }


//=======================================================================
//function : GetID
//purpose  : 
//======================================================================

const Standard_GUID& TDocStd_XLink::GetID() 
{
  static Standard_GUID myID("5d587400-5690-11d1-8940-080009dc3333");
  return myID;
}


//=======================================================================
//function : DocumentEntry
//purpose  : 
//=======================================================================

void TDocStd_XLink::DocumentEntry
(const TCollection_AsciiString& aDocEntry) 
{ Backup(); myDocEntry = aDocEntry; }


//=======================================================================
//function : DocumentEntry
//purpose  : 
//=======================================================================

const TCollection_AsciiString& TDocStd_XLink::DocumentEntry() const
{ return myDocEntry; }


//=======================================================================
//function : LabelEntry
//purpose  : 
//=======================================================================

void TDocStd_XLink::LabelEntry
(const TDF_Label& aLabel) 
{
  Backup();
  TDF_Tool::Entry(aLabel,myLabelEntry);
}


//=======================================================================
//function : LabelEntry
//purpose  : 
//=======================================================================

void TDocStd_XLink::LabelEntry
(const TCollection_AsciiString& aLabEntry) 
{ Backup(); myLabelEntry = aLabEntry; }


//=======================================================================
//function : LabelEntry
//purpose  : 
//=======================================================================

const TCollection_AsciiString& TDocStd_XLink::LabelEntry() const
{ return myLabelEntry; }


//=======================================================================
//function : AfterAddition
//purpose  : 
//=======================================================================

void TDocStd_XLink::AfterAddition() 
{
  TDocStd_XLinkRoot::Insert(this);
  Label().Imported(Standard_True);
}


//=======================================================================
//function : BeforeRemoval
//purpose  : 
//=======================================================================

void TDocStd_XLink::BeforeRemoval() 
{
  if (!IsBackuped()) {
    TDocStd_XLinkRoot::Remove(this);
    Label().Imported(Standard_False);
  }
}


//=======================================================================
//function : BeforeUndo
//purpose  : Before application of a TDF_Delta.
//=======================================================================

Standard_Boolean TDocStd_XLink::BeforeUndo
(const Handle(TDF_AttributeDelta)& anAttDelta,
 const Standard_Boolean forceIt)
{
  if (anAttDelta->IsKind(STANDARD_TYPE(TDF_DeltaOnAddition))) {
    anAttDelta->Attribute()->BeforeRemoval();
  }
  return Standard_True;
}


//=======================================================================
//function : AfterUndo
//purpose  : After application of a TDF_Delta.
//=======================================================================

Standard_Boolean TDocStd_XLink::AfterUndo
(const Handle(TDF_AttributeDelta)& anAttDelta,
 const Standard_Boolean forceIt)
{
  if (anAttDelta->IsKind(STANDARD_TYPE(TDF_DeltaOnRemoval))) {
    anAttDelta->Attribute()->AfterAddition();
  }
  return Standard_True;
}


//=======================================================================
//function : BackupCopy
//purpose  : 
//=======================================================================

Handle(TDF_Attribute) TDocStd_XLink::BackupCopy() const
{
  Handle(TDocStd_XLink) xRef = new TDocStd_XLink();
  xRef->DocumentEntry(myDocEntry);
  xRef->LabelEntry(myLabelEntry);
  return xRef;
}


//=======================================================================
//function : Restore
//purpose  : 
//=======================================================================

void TDocStd_XLink::Restore(const Handle(TDF_Attribute)& anAttribute) 
{
  const Handle(TDocStd_XLink)& xRef =
    Handle(TDocStd_XLink)::DownCast(anAttribute);
  if (!xRef.IsNull()) {
    myDocEntry = xRef->DocumentEntry();
    myLabelEntry = xRef->LabelEntry();
  }
}


//=======================================================================
//function : NewEmpty
//purpose  : 
//=======================================================================

Handle(TDF_Attribute) TDocStd_XLink::NewEmpty() const
{ return new TDocStd_XLink(); }


//=======================================================================
//function : Paste
//purpose  : 
//=======================================================================

void TDocStd_XLink::Paste
(const Handle(TDF_Attribute)& intoAttribute,
 const Handle(TDF_RelocationTable)& aRelocationTable) const
{
  const Handle(TDocStd_XLink)& xRef =
    Handle(TDocStd_XLink)::DownCast(intoAttribute);
  if (!xRef.IsNull()) {
    xRef->DocumentEntry(myDocEntry);
    xRef->LabelEntry(myLabelEntry);
  }
}


//=======================================================================
//function : Dump
//purpose  : 
//=======================================================================

Standard_OStream& TDocStd_XLink::Dump(Standard_OStream& anOS) const
{
  return anOS;
}

