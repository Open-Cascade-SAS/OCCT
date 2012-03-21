// Created on: 2004-11-23
// Created by: Pavel TELKOV
// Copyright (c) 2004-2012 OPEN CASCADE SAS
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

// The original implementation Copyright: (C) RINA S.p.A

#include <TObj_SequenceIterator.hxx>

#include <TObj_Object.hxx>

IMPLEMENT_STANDARD_HANDLE(TObj_SequenceIterator,TObj_ObjectIterator)
IMPLEMENT_STANDARD_RTTIEXT(TObj_SequenceIterator,TObj_ObjectIterator)

//=======================================================================
//function : TObj_SequenceIterator
//purpose  :
//=======================================================================

TObj_SequenceIterator::TObj_SequenceIterator() :
  myIndex( 1 )
{
}

//=======================================================================
//function : TObj_SequenceIterator
//purpose  :
//=======================================================================

TObj_SequenceIterator::TObj_SequenceIterator
  (const Handle(TObj_HSequenceOfObject)& theObjects,
   const Handle(Standard_Type)&              theType)
{
  myIndex = 1;
  myType = theType;
  myObjects = theObjects;
}

//=======================================================================
//function : More
//purpose  :
//=======================================================================

Standard_Boolean TObj_SequenceIterator::More() const
{
  const Standard_Boolean isMore = (!myObjects.IsNull() &&
                                   (myIndex <= myObjects->Length() && myIndex > 0) &&
                                   !myObjects->Value(myIndex).IsNull());

  // check type
  if (isMore && !myType.IsNull() && !myObjects->Value(myIndex)->IsKind( myType ))
  {
    TObj_SequenceIterator* me = (TObj_SequenceIterator*) this;
    me->Next();
    return More();
  }

  return isMore;
}

//=======================================================================
//function : Next
//purpose  :
//=======================================================================

void TObj_SequenceIterator::Next()
{ myIndex++; }

//=======================================================================
//function : Value
//purpose  :
//=======================================================================

Handle(TObj_Object) TObj_SequenceIterator::Value() const
{
  return myObjects->Value(myIndex);
}
