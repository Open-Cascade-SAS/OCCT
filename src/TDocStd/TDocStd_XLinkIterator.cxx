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

//      	----------------------

// Version:	0.0
//Version	Date		Purpose
//		0.0	Sep 15 1997	Creation



#include <TDocStd_XLinkIterator.ixx>
#include <TDF_Data.hxx>
#include <TDocStd_XLink.hxx>
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
  if (myValue == NULL) Standard_NoMoreObject::Raise("");
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

