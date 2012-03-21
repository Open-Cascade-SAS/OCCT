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
