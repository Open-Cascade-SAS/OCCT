// File:        TObj_ModelIterator.cxx
// Created:     Tue Nov 23 11:58:14 2004
// Author:      Pavel TELKOV
// Copyright:   Open CASCADE  2007
// The original implementation Copyright: (C) RINA S.p.A

#include <TObj_ModelIterator.hxx>

#include <TObj_Model.hxx>

IMPLEMENT_STANDARD_HANDLE(TObj_ModelIterator,TObj_ObjectIterator)
IMPLEMENT_STANDARD_RTTIEXT(TObj_ModelIterator,TObj_ObjectIterator)

//=======================================================================
//function : Constructor
//purpose  : 
//=======================================================================

TObj_ModelIterator::TObj_ModelIterator(const Handle(TObj_Model)& theModel)
{
  myObject = theModel->GetRoot();
  if ( ! myObject.IsNull() )
    addIterator(myObject);
}

//=======================================================================
//function : addIterator
//purpose  : 
//=======================================================================

void TObj_ModelIterator::addIterator(const Handle(TObj_Object)& theObj)
{
  Handle(TObj_ObjectIterator) anIter = theObj->GetChildren();
  if (anIter.IsNull() )
    return; // object has no children.
  myIterSeq.Append(anIter);
}

//=======================================================================
//function : More
//purpose  : 
//=======================================================================

Standard_Boolean TObj_ModelIterator::More() const
{
  return ! myObject.IsNull();
}

//=======================================================================
//function : Value
//purpose  : 
//=======================================================================

Handle(TObj_Object) TObj_ModelIterator::Value() const
{
  return myObject;
}

//=======================================================================
//function : Next
//purpose  : 
//=======================================================================

void TObj_ModelIterator::Next()
{
  myObject.Nullify();
  while ( myIterSeq.Length() >0 ) 
  {
    if ( myIterSeq.Last()->More() ) 
    {
      myObject = myIterSeq.Last()->Value();
      myIterSeq.Last()->Next();
      addIterator ( myObject );
      return;
    }
    else myIterSeq.Remove(myIterSeq.Length());
  }
}
