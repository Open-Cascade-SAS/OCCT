// Created on: 1999-06-10
// Created by: Vladislav ROMASHKO
// Copyright (c) 1999-1999 Matra Datavision
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

#include <TDataStd_TreeNode.hxx>
#include <TDF_Label.hxx>
#include <TDF_DataSet.hxx>
#include <TDF_DeltaOnAddition.hxx>
#include <TDF_DeltaOnRemoval.hxx>
#include <TDF_RelocationTable.hxx>
#include <Standard_DomainError.hxx>

IMPLEMENT_STANDARD_RTTIEXT(TDataStd_TreeNode,TDF_Attribute)

//=======================================================================
//function : Find
//purpose  : 
//=======================================================================

Standard_Boolean TDataStd_TreeNode::Find(const TDF_Label& L,
					 Handle(TDataStd_TreeNode)& T)
{
  return L.FindAttribute(TDataStd_TreeNode::GetDefaultTreeID(), T);
}

//=======================================================================
//TreeNode : GetDefaultTreeID
//purpose  : Static method to get the default ID of a TreeNode
//=======================================================================

const Standard_GUID&  TDataStd_TreeNode::GetDefaultTreeID() 
{  
  static Standard_GUID TDataStd_TreeNodeID ("2a96b621-ec8b-11d0-bee7-080009dc3333");
  return TDataStd_TreeNodeID; 
}

//=======================================================================
//TreeNode : Set
//purpose  : Finds or creates a TreeNode attribute with default ID
//=======================================================================

Handle(TDataStd_TreeNode) TDataStd_TreeNode::Set(const TDF_Label& L)
{
  Handle(TDataStd_TreeNode) TN;
  if (!L.FindAttribute(TDataStd_TreeNode::GetDefaultTreeID(),TN)) {
    TN = new TDataStd_TreeNode();
    TN->SetTreeID(TDataStd_TreeNode::GetDefaultTreeID());
    L.AddAttribute(TN);
  }
  return TN;
}

//=======================================================================
//function : Set
//purpose  : Finds or creates a TreeNode  attribute with explicit ID
//         : a driver for it
//=======================================================================

Handle(TDataStd_TreeNode) TDataStd_TreeNode::Set (const TDF_Label& L, 
						  const Standard_GUID& explicitID)
{
  Handle(TDataStd_TreeNode) TN;
  if (!L.FindAttribute(explicitID,TN)) {
    TN = new TDataStd_TreeNode ();    
    TN->SetTreeID(explicitID);
    L.AddAttribute(TN);
  }
  return TN;
}

//=======================================================================
//TreeNode : ID
//purpose  : Returns GUID of the TreeNode
//=======================================================================

const Standard_GUID& TDataStd_TreeNode::ID() const
{
  return myTreeID; 
}

//=======================================================================
//TreeNode : TDataStd_TreeNode
//purpose  : Constructor
//=======================================================================

TDataStd_TreeNode::TDataStd_TreeNode () :
  myFather(NULL),  myPrevious(NULL), myNext(NULL), myFirst(NULL), myLast(NULL)
{}  


//=======================================================================
//function : Append
//purpose  : Add <TN> as last child of me
//=======================================================================

Standard_Boolean TDataStd_TreeNode::Append (const Handle(TDataStd_TreeNode)& TN)
{  
  if (!(TN->ID() == myTreeID) )
    throw Standard_DomainError("TDataStd_TreeNode::Append : uncompatible GUID");

  Handle(TDataStd_TreeNode) bid;
  TN->SetNext(bid); // Deconnects from next.

  // Find the last
  if (!HasFirst()) {
    SetFirst(TN);
    TN->SetPrevious(bid); // Deconnects from previous.
  }
  else 
  {
    Handle(TDataStd_TreeNode) L = Last();
    L->SetNext(TN);
    TN->SetPrevious(L);
  }
  // Set Father
  TN->SetFather(this);
  myLast=TN.operator->();
  return !TN.IsNull();
}

//=======================================================================
//function : Prepend
//purpose  : Add <TN> as first child of me
//=======================================================================

Standard_Boolean TDataStd_TreeNode::Prepend (const Handle(TDataStd_TreeNode)& TN)
{  
  if (!(TN->ID() == myTreeID) ) 
    throw Standard_DomainError("TDataStd_TreeNode::Prepend : uncompatible GUID");

  Handle(TDataStd_TreeNode) bid;
  TN->SetPrevious(bid);
  if (HasFirst()) {
    TN->SetNext(First());
    First()->SetPrevious(TN);
  }
  else {
    TN->SetNext(bid);
    SetLast(TN);
  }
  TN->SetFather(this);
  SetFirst(TN);
  return !TN.IsNull();
}

//=======================================================================
//function : InsertBefore
//purpose  : Inserts the TreeNode  <TN> before me
//=======================================================================

Standard_Boolean TDataStd_TreeNode::InsertBefore (const Handle(TDataStd_TreeNode)& TN)
{
  if (!(TN->ID() == myTreeID) ) 
    throw Standard_DomainError("TDataStd_TreeNode::InsertBefore : uncompatible GUID");

  TN->SetFather(Father());
  TN->SetPrevious(Previous());
  TN->SetNext(this);
  
  if (!HasPrevious())
    Father()->SetFirst(TN);
  else
    Previous()->SetNext(TN);
  
  SetPrevious(TN);
  return !TN.IsNull();
}

//=======================================================================
//function : InsertAfter
//purpose  : Inserts the TreeNode  <TN> after me
//=======================================================================

Standard_Boolean TDataStd_TreeNode::InsertAfter (const Handle(TDataStd_TreeNode)& TN)
{
  if (!(TN->ID() == myTreeID) ) 
    throw Standard_DomainError("TDataStd_TreeNode::InsertAfter : uncompatible GUID");
  
  if (HasFather() && !HasNext())
    Father()->SetLast(TN);

  TN->SetFather(Father());
  TN->SetPrevious(this);
  TN->SetNext(Next());
  
  if (HasNext()) 
    Next()->SetPrevious(TN);

  SetNext(TN);
  return !TN.IsNull();
}

//=======================================================================
//function : Remove
//purpose  : Removees the function from the function tree
//=======================================================================

Standard_Boolean TDataStd_TreeNode::Remove ()
{
  if (IsRoot()) return Standard_True;

  Handle(TDataStd_TreeNode) bid;
  if (!HasPrevious())
    Father()->SetFirst(Next());
  else
    Previous()->SetNext(Next());

  if (HasNext()) {
    if (HasPrevious()) Next()->SetPrevious(Previous());
    else Next()->SetPrevious(bid);
  }
  else {
    if (HasPrevious()) Previous()->SetNext(bid);
  }

  if (Father()->HasFirst()) {
    if (this == Father()->First().operator->()) {
      if (HasNext()) {
	    Father()->SetFirst(Next());
      }
      else Father()->SetFirst(bid);
    }
  }
  
  if(Father()->HasLast()) {
    Father()->SetLast(bid);
  }

  SetFather(bid);
  SetNext(bid);
  SetPrevious(bid);
  return Standard_True;
}


//=======================================================================
//function : Depth
//purpose  : 
//=======================================================================

Standard_Integer TDataStd_TreeNode::Depth () const
{  
  Standard_Integer depth = 0;

  TDataStd_TreeNode* O = (TDataStd_TreeNode*) this;
  while (O->myFather != NULL)
  {
    depth++;
    O = O->myFather;
  }

  return depth;
}

//=======================================================================
//function : NbChildren
//purpose  : Returns the number of child nodes.
//           If <allLevels> is true, the method counts children of all levels
//           (children of children ...)
//=======================================================================
Standard_Integer TDataStd_TreeNode::NbChildren(const Standard_Boolean allLevels) const
{  
    Standard_Integer nb = 0;
    TDataStd_TreeNode* C = myFirst;
    while (C != NULL)
    {
        if (allLevels && 
            C->myFirst != NULL)
        {
            nb += C->NbChildren(allLevels);
        }
        nb++;
        C = C->myNext;
    }
    return nb;
}
 
//=======================================================================
//function : SetTreeID
//purpose  : Finds or creates a TreeNode  attribute with explicit ID
//         : a driver for it
//=======================================================================

void TDataStd_TreeNode::SetTreeID (const Standard_GUID& explicitID)
{
  myTreeID = explicitID;
}


//=======================================================================
//function : IsAscendant
//purpose  : 
//=======================================================================

Standard_Boolean TDataStd_TreeNode::IsAscendant (const Handle(TDataStd_TreeNode)& ofTN) const
{
  return ofTN->IsDescendant(this);
}


//=======================================================================
//function : IsDescendant
//purpose  : 
//=======================================================================

Standard_Boolean TDataStd_TreeNode::IsDescendant (const Handle(TDataStd_TreeNode)& ofTN) const
{
  TDataStd_TreeNode* O = (TDataStd_TreeNode*) this;
  while (O->myFather != NULL)
  {
    if (O->myFather == ofTN)
      return Standard_True;
    O = O->myFather;
  }
  return Standard_False;
}

//=======================================================================
//function : IsFather
//purpose  : 
//=======================================================================

Standard_Boolean TDataStd_TreeNode::IsFather (const Handle(TDataStd_TreeNode)& ofTN) const
{
  return (ofTN->Father() == this);
}


//=======================================================================
//function : IsChild
//purpose  : 
//=======================================================================

Standard_Boolean TDataStd_TreeNode::IsChild (const Handle(TDataStd_TreeNode)& ofTN) const
{
  return (myFather == ofTN.operator->());
}


//=======================================================================
//TreeNode : Father
//purpose  : Returns the Father of the TreeNode
//=======================================================================

Handle(TDataStd_TreeNode) TDataStd_TreeNode::Father() const
{
  Handle(TDataStd_TreeNode) O = myFather;
  return O;
}

//=======================================================================
//TreeNode : IsRoot
//purpose  : Returns Standard_True if the TreeNode is not attached to a
//           TreeNode tree or hasn't an Father.
//=======================================================================

Standard_Boolean TDataStd_TreeNode::IsRoot() const
{
  if (myFather    == NULL && 
      myPrevious == NULL &&
      myNext     == NULL)
    return Standard_True;
  return Standard_False;
}

//=======================================================================
//TreeNode : Root
//purpose  : Returns the TreeNode which has no Father
//=======================================================================

Handle(TDataStd_TreeNode) TDataStd_TreeNode::Root() const
{
  TDataStd_TreeNode* O = (TDataStd_TreeNode*) this;
  while (O->myFather != NULL)
  {
    O = O->myFather;
  }
  return O;
}

//=======================================================================
//TreeNode : Next
//purpose  : Returns next (in the TreeNode tree) TreeNode
//=======================================================================

Handle(TDataStd_TreeNode) TDataStd_TreeNode::Next() const
{
  Handle(TDataStd_TreeNode) O = myNext;  
  return O;
}

//=======================================================================
//TreeNode : Previous
//purpose  : Returns previous (in the TreeNode tree) TreeNode
//=======================================================================

Handle(TDataStd_TreeNode) TDataStd_TreeNode::Previous() const
{
  Handle(TDataStd_TreeNode) O = myPrevious;
  return O;
}

//=======================================================================
//TreeNode : First 
//purpose  : Returns first child
//=======================================================================

Handle(TDataStd_TreeNode) TDataStd_TreeNode::First () const
{
  Handle(TDataStd_TreeNode) O = myFirst;
  return O;
}

//=======================================================================
//TreeNode : Last 
//purpose  : Returns last child
//           Optimize an Append operation if it is called in a circle
//=======================================================================

Handle(TDataStd_TreeNode) TDataStd_TreeNode::Last ()
{
  if(myLast && !myLast->IsChild(this))
    myLast=NULL;

  if(myLast == NULL) 
    return FindLast();
  
  return myLast;
}

//=======================================================================
//TreeNode : FindLast 
//purpose  : Returns last child
//           Optimizes an Append operation if it is called in a circle
//=======================================================================
Handle(TDataStd_TreeNode) TDataStd_TreeNode::FindLast ()
{
	if (myFirst == NULL)
		return myFirst;
	TDataStd_TreeNode* L = myFirst;
	while (L->myNext != NULL)
	{
		L = L->myNext;
	}
	return L;
}

//=======================================================================
//TreeNode : SetFather
//purpose  : Sets the TreeNode F as Father of me
//=======================================================================

void TDataStd_TreeNode::SetFather(const Handle(TDataStd_TreeNode)& F)
{
  Backup();
  if (F.IsNull()) myFather = NULL;
  else            myFather = F.operator->();
  myLast=NULL;
}

//=======================================================================
//TreeNode : SetNext
//purpose  : Sets the TreeNode F next to me 
//=======================================================================

void TDataStd_TreeNode::SetNext(const Handle(TDataStd_TreeNode)& F)
{
  Backup();
  if (F.IsNull()) myNext = NULL;
  else            myNext = F.operator->();
  myLast=NULL;
}


//=======================================================================
//TreeNode : SetPrevious
//purpose  : Sets the TreeNode F previous to me
//=======================================================================

void TDataStd_TreeNode::SetPrevious(const Handle(TDataStd_TreeNode)& F)
{
  Backup();
  if (F.IsNull()) myPrevious = NULL;
  else            myPrevious = F.operator->();
  myLast=NULL;
}

//=======================================================================
//TreeNode : SetFirst
//purpose  : Sets the TreeNode F as first in the TreeNode tree
//=======================================================================

void TDataStd_TreeNode::SetFirst(const Handle(TDataStd_TreeNode)& F)
{
  Backup();
  if (F.IsNull()) myFirst = NULL;
  else            myFirst = F.operator->();
  myLast=NULL;
}

//=======================================================================
//TreeNode : SetLast
//purpose  : Sets the TreeNode F as last in the TreeNode tree
//=======================================================================

void TDataStd_TreeNode::SetLast(const Handle(TDataStd_TreeNode)& F)
{
  Backup();
  if (F.IsNull()) myLast = NULL;
  else            myLast = F.operator->();
}

//=======================================================================
//TreeNode : AfterAddition
//purpose  : Connects the TreeNode to the tree.
//           Backuped attribute must stay disconnected
//=======================================================================

void TDataStd_TreeNode::AfterAddition() {
  if (!IsBackuped()) {
    if (myPrevious)
      myPrevious->SetNext(this);
    else if (myFather)
      myFather->SetFirst(this);
    if (myNext)
      myNext->SetPrevious(this);
  }
}

//=======================================================================
//TreeNode : BeforeForget
//purpose  : Disconnect the TreeNode from the tree.
//           Backuped attribute is normaly not concerned by such an operation
//=======================================================================

void TDataStd_TreeNode::BeforeForget() {
  if (!IsBackuped()) {
    Remove();
    while (HasFirst()) First()->Remove();
  }
}

//=======================================================================
//TreeNode : AfterResume
//purpose  : Connects the TreeNode to the tree
//=======================================================================

void TDataStd_TreeNode::AfterResume() {
  AfterAddition();
}

//=======================================================================
//TreeNode : BeforeUndo
//purpose  : Disconnect the TreeNode from the tree.
//=======================================================================

Standard_Boolean TDataStd_TreeNode::BeforeUndo(const Handle(TDF_AttributeDelta)& anAttDelta,
						const Standard_Boolean /*forceIt*/)
{
  if (anAttDelta->IsKind(STANDARD_TYPE(TDF_DeltaOnAddition))) BeforeForget(); // Disconnect.
  return Standard_True;
}

//=======================================================================
//TreeNode : AfterUndo
//purpose  : Connect the TreeNode from the tree.
//=======================================================================

Standard_Boolean TDataStd_TreeNode::AfterUndo(const Handle(TDF_AttributeDelta)& anAttDelta,
					      const Standard_Boolean /*forceIt*/)
{
  if (anAttDelta->IsKind(STANDARD_TYPE(TDF_DeltaOnRemoval))) AfterAddition(); // Reconnect.
  return Standard_True;
}

//=======================================================================
//TreeNode : Restore
//purpose  : 
//=======================================================================

void TDataStd_TreeNode::Restore(const Handle(TDF_Attribute)& other) {
  Handle(TDataStd_TreeNode) F =  Handle(TDataStd_TreeNode)::DownCast(other);
  myFather     = F->myFather;
  myPrevious   = F->myPrevious;
  myNext       = F->myNext;
  myFirst      = F->myFirst; 
  myTreeID     = F->myTreeID;
  myLast       = NULL;
}

//=======================================================================
//TreeNode : Paste
//purpose  : Method for Copy mechanism
//=======================================================================

void TDataStd_TreeNode::Paste(const Handle(TDF_Attribute)& into,
			      const Handle(TDF_RelocationTable)& RT) const
{
  Handle(TDataStd_TreeNode) intof = Handle(TDataStd_TreeNode)::DownCast(into);
  Handle(TDataStd_TreeNode) func;
  if (!RT->HasRelocation(myFather, func) && RT->AfterRelocate()) {
    func.Nullify();
  }
  intof->SetFather(func);
  if (!RT->HasRelocation(myNext, func) && RT->AfterRelocate()) {
    func.Nullify();
  }
  intof->SetNext(func);
  if (!RT->HasRelocation(myPrevious, func) && RT->AfterRelocate()) {
    func.Nullify();
  }
  intof->SetPrevious(func);
  if (!RT->HasRelocation(myFirst, func) && RT->AfterRelocate()) {
    func.Nullify();
  }

  intof->SetFirst(func);
  intof->SetTreeID(myTreeID);
}

//=======================================================================
//TreeNode : NewEmpty
//purpose  : Returns new empty TreeNode attribute
//=======================================================================

Handle(TDF_Attribute) TDataStd_TreeNode::NewEmpty() const
{
  Handle(TDataStd_TreeNode) T = new TDataStd_TreeNode();
  T->SetTreeID(myTreeID);
  return T;
}

//=======================================================================
//TreeNode : References
//purpose  : Collects the references
//=======================================================================

void TDataStd_TreeNode::References(const Handle(TDF_DataSet)& aDataSet) const
{
  TDataStd_TreeNode* fct = myFirst;
  while (fct != NULL) {
    aDataSet->AddAttribute(fct);
    fct = fct->myNext;
  }
}

//=======================================================================
//TreeNode : Dump
//purpose  : Dump of the TreeNode
//=======================================================================

Standard_OStream& TDataStd_TreeNode::Dump (Standard_OStream& anOS) const
{
  TDF_Attribute::Dump (anOS);
  if (myFather) {
    anOS<<"  Father=";
    if (!myFather->Label().IsNull()) myFather->Label().EntryDump(anOS);
  }
  if (myPrevious) {
    anOS<<"  Previous=";
    if (!myPrevious->Label().IsNull()) myPrevious->Label().EntryDump(anOS);
  }
  if (myNext) {
    anOS<<"  Next=";
    if (!myNext->Label().IsNull()) myNext->Label().EntryDump(anOS);
  }
  if (myFirst) {
    anOS<<"  First=";
    if (!myFirst->Label().IsNull()) myFirst->Label().EntryDump(anOS);
  }
  if (myLast) {
    anOS<<"  Last=";
    if (!myLast->Label().IsNull()) myLast->Label().EntryDump(anOS);
  }
  anOS<<endl;
  return anOS;
}
