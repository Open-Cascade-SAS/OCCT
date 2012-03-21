// Created on: 2007-09-03
// Created by: Alexander GRIGORIEV
// Copyright (c) 2007-2012 OPEN CASCADE SAS
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



#include <NIS_ObjectsIterator.hxx>
#include <NIS_InteractiveContext.hxx>

//=======================================================================
//function : NIS_ObjectsIterator()
//purpose  : Constructor
//=======================================================================

void NIS_ObjectsIterator::Initialize
                        (const Handle(NIS_InteractiveContext)& theCtx)
{
  if (theCtx.IsNull())
    myIter = NCollection_SparseArray <Handle_NIS_InteractiveObject>::Iterator();
  else
    for (myIter.Init (theCtx->myObjects); myIter.More(); myIter.Next())
      if (myIter.Value().IsNull() == Standard_False)
        break;
}

//=======================================================================
//function : Next
//purpose  : 
//=======================================================================

void NIS_ObjectsIterator::Next ()
{
  for (myIter.Next(); myIter.More(); myIter.Next())
    if (myIter.Value().IsNull() == Standard_False)
      break;
}

//=======================================================================
//function : Value
//purpose  : 
//=======================================================================

const Handle(NIS_InteractiveObject)& NIS_ObjectsIterator::Value() const
{
  if (More())
    return myIter.Value();
  static const Handle(NIS_InteractiveObject) aNullObj;
  return aNullObj;
}
