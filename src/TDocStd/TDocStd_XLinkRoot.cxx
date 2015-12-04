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

//      	------------------
// Version:	0.0
//Version	Date		Purpose
//		0.0	Sep 15 1997	Creation

#include <Standard_GUID.hxx>
#include <Standard_Type.hxx>
#include <TDF_Attribute.hxx>
#include <TDF_Data.hxx>
#include <TDF_RelocationTable.hxx>
#include <TDocStd_XLink.hxx>
#include <TDocStd_XLinkIterator.hxx>
#include <TDocStd_XLinkRoot.hxx>

IMPLEMENT_STANDARD_RTTIEXT(TDocStd_XLinkRoot,TDF_Attribute)

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

void TDocStd_XLinkRoot::Restore(const Handle(TDF_Attribute)& /*anAttribute*/) 
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
(const Handle(TDF_Attribute)&,
 const Handle(TDF_RelocationTable)&) const
{} // Does nothing.


//=======================================================================
//function : Dump
//purpose  : 
//=======================================================================

Standard_OStream& TDocStd_XLinkRoot::Dump(Standard_OStream& anOS) const
{ return anOS; }

