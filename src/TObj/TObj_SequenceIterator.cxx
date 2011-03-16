// File:        TObj_SequenceIterator.cxx
// Created:     Tue Nov 23 12:39:22 2004
// Author:      Pavel TELKOV
// Copyright:   Open CASCADE  2007
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
