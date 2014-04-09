// Created on: 2007-09-03
// Created by: Alexander GRIGORIEV
// Copyright (c) 2007-2014 OPEN CASCADE SAS
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
    myIter = NCollection_SparseArray <Handle(NIS_InteractiveObject)>::Iterator();
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
