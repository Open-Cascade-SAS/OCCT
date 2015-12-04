// Created on: 2004-11-23
// Created by: Pavel TELKOV
// Copyright (c) 2004-2014 OPEN CASCADE SAS
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

// The original implementation Copyright: (C) RINA S.p.A

#include <TObj_SequenceIterator.hxx>

#include <TObj_Object.hxx>


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
