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

#include <TObj_ModelIterator.hxx>

#include <TObj_Model.hxx>


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
