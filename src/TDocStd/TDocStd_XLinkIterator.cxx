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

//      	----------------------
// Version:	0.0
//Version	Date		Purpose
//		0.0	Sep 15 1997	Creation

#include <Standard_NoMoreObject.hxx>
#include <TDF_Data.hxx>
#include <TDocStd_Document.hxx>
#include <TDocStd_XLink.hxx>
#include <TDocStd_XLinkIterator.hxx>
#include <TDocStd_XLinkRoot.hxx>

//=======================================================================
//function : TDocStd_XLinkIterator
//purpose  : 
//=======================================================================
TDocStd_XLinkIterator::TDocStd_XLinkIterator()
: myValue(NULL)
{}


//=======================================================================
//function : TDocStd_XLinkIterator
//purpose  : 
//=======================================================================

TDocStd_XLinkIterator::TDocStd_XLinkIterator
(const Handle(TDocStd_Document)& DOC)
: myValue(NULL)
{ Init(DOC); }


//=======================================================================
//function : Initialize
//purpose  : 
//=======================================================================

void TDocStd_XLinkIterator::Initialize(const Handle(TDocStd_Document)& DOC) 
{ myValue = NULL; Init(DOC); }


//=======================================================================
//function : Next
//purpose  : 
//=======================================================================

void TDocStd_XLinkIterator::Next() 
{
  if (myValue == NULL) throw Standard_NoMoreObject ("TDocStd_XLinkIterator::Next() - no more values available");
  else                 myValue = myValue->Next();
}

//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

void TDocStd_XLinkIterator::Init(const Handle(TDocStd_Document)& DOC) 
{
  Handle(TDocStd_XLinkRoot) xRefRoot;
  if (DOC->GetData()->Root().FindAttribute(TDocStd_XLinkRoot::GetID(),xRefRoot))
    myValue = xRefRoot->First();
}

