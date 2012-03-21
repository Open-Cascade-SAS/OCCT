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

//      	------------------

// Version:	0.0
//Version	Date		Purpose
//		0.0	Sep 15 1997	Creation



#include <TDocStd_XLinkRoot.ixx>

#include <TDocStd_XLink.hxx>


//=======================================================================
//             CLASS METHODS
//=======================================================================

//=======================================================================
//function : GetID
//purpose  : 
//=======================================================================

const Standard_GUID& TDocStd_XLinkRoot::GetID() 
{
  static Standard_GUID myID("5d587401-5690-11d1-8940-080009dc3333");
  return myID;
}


//=======================================================================
//function : Set
//purpose  : CLASS method.
//=======================================================================

Handle(TDocStd_XLinkRoot) TDocStd_XLinkRoot::Set
(const Handle(TDF_Data)& aDF) 
{
  Handle(TDocStd_XLinkRoot) xRefRoot;
  if (!aDF->Root().FindAttribute (TDocStd_XLinkRoot::GetID(),xRefRoot)) {
    xRefRoot = new TDocStd_XLinkRoot;
    aDF->Root().AddAttribute(xRefRoot);
  }
  return xRefRoot;
}


//=======================================================================
//function : Insert
//purpose  : 
//=======================================================================

void TDocStd_XLinkRoot::Insert(const TDocStd_XLinkPtr& anXLinkPtr)
{
  Handle(TDocStd_XLinkRoot) xRefRoot =
    TDocStd_XLinkRoot::Set(anXLinkPtr->Label().Data());
  // Insertion at beginning because the order is not significant.
  anXLinkPtr->Next(xRefRoot->First());
  xRefRoot->First(anXLinkPtr);
}


//=======================================================================
//function : Remove
//purpose  : 
//=======================================================================

void TDocStd_XLinkRoot::Remove(const TDocStd_XLinkPtr& anXLinkPtr)
{
  Handle(TDocStd_XLinkRoot) xRefRoot;
  if (anXLinkPtr->Label().Root().FindAttribute(TDocStd_XLinkRoot::GetID(),
					      xRefRoot)) {
    TDocStd_XLink* previous = xRefRoot->First();
    if (previous == anXLinkPtr) {
      xRefRoot->First(anXLinkPtr->Next());
      previous = NULL; anXLinkPtr->Next(previous);
    }
    else {
      while (previous != NULL && previous->Next() != anXLinkPtr)
	previous = previous->Next();
      if (previous != NULL) {
	previous->Next(anXLinkPtr->Next());
	previous = NULL; anXLinkPtr->Next(previous);
      }
    }
  }
}


//=======================================================================
//             INSTANCE METHODS
//=======================================================================

//=======================================================================
//function : TDocStd_XLinkRoot
//purpose  : 
//=======================================================================

TDocStd_XLinkRoot::TDocStd_XLinkRoot()
: myFirst(NULL)
{}


//=======================================================================
//function : ID
//purpose  : 
//=======================================================================

const Standard_GUID& TDocStd_XLinkRoot::ID() const
{ return GetID(); }


//=======================================================================
//function : BackupCopy
//purpose  : 
//=======================================================================

Handle(TDF_Attribute) TDocStd_XLinkRoot::BackupCopy() const
{ return new TDocStd_XLinkRoot; } // Does nothing.


//=======================================================================
//function : Restore
//purpose  : 
//=======================================================================

void TDocStd_XLinkRoot::Restore(const Handle(TDF_Attribute)& anAttribute) 
{} // Does nothing.


//=======================================================================
//function : NewEmpty
//purpose  : 
//=======================================================================

Handle(TDF_Attribute) TDocStd_XLinkRoot::NewEmpty() const
{ return new TDocStd_XLinkRoot; }


//=======================================================================
//function : Paste
//purpose  : 
//=======================================================================

void TDocStd_XLinkRoot::Paste
(const Handle(TDF_Attribute)& intoAttribute,
 const Handle(TDF_RelocationTable)& aRelocationTable) const
{} // Does nothing.


//=======================================================================
//function : Dump
//purpose  : 
//=======================================================================

Standard_OStream& TDocStd_XLinkRoot::Dump(Standard_OStream& anOS) const
{ return anOS; }

