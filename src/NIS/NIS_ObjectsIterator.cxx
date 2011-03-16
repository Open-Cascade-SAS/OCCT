// File:      NIS_ObjectsIterator.cxx
// Created:   03.09.07 00:06
// Author:    Alexander GRIGORIEV
// Copyright: Open Cascade 2007


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
    myIter = NCollection_Vector <Handle_NIS_InteractiveObject>::Iterator();
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
