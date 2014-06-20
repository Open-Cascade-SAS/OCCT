// Created on: 2011-11-02
// Created by: Sergey ZERCHANINOV
// Copyright (c) 2011-2014 OPEN CASCADE SAS
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

#include <OpenGl_PriorityList.hxx>

#include <OpenGl_BVHTreeSelector.hxx>
#include <OpenGl_Structure.hxx>
#include <OpenGl_View.hxx>

// =======================================================================
// function : OpenGl_PriorityList
// purpose  :
// =======================================================================
OpenGl_PriorityList::OpenGl_PriorityList (const Standard_Integer theNbPriorities)
: myArray (0, theNbPriorities - 1),
  myNbStructures (0),
  myBVHIsLeftChildQueuedFirst (Standard_True),
  myIsBVHPrimitivesNeedsReset (Standard_False)
{
  //
}

// =======================================================================
// function : ~OpenGl_PriorityList
// purpose  :
// =======================================================================
OpenGl_PriorityList::~OpenGl_PriorityList()
{
  //
}

// =======================================================================
// function : Add
// purpose  :
// =======================================================================
void OpenGl_PriorityList::Add (const OpenGl_Structure* theStructure,
                               const Standard_Integer  thePriority,
                               Standard_Boolean isForChangePriority)
{
  const Standard_Integer anIndex = Min (Max (thePriority, 0), myArray.Length() - 1);

  myArray (anIndex).Append (theStructure);
  if (theStructure->IsAlwaysRendered())
  {
    theStructure->MarkAsNotCulled();
  }
  else if (!isForChangePriority)
  {
    myBVHPrimitives.Add (theStructure);
  }
  ++myNbStructures;
}

// =======================================================================
// function : Remove
// purpose  :
// =======================================================================
Standard_Integer OpenGl_PriorityList::Remove (const OpenGl_Structure* theStructure,
                                              Standard_Boolean isForChangePriority)
{
  const Standard_Integer aNbPriorities = myArray.Length();
  OpenGl_SequenceOfStructure::Iterator aStructIter;
  for (Standard_Integer aPriorityIter = 0; aPriorityIter < aNbPriorities; ++aPriorityIter)
  {
    OpenGl_SequenceOfStructure& aSeq = myArray (aPriorityIter);
    for (aStructIter.Init (aSeq); aStructIter.More(); aStructIter.Next())
    {
      if (aStructIter.Value() == theStructure)
      {
        aSeq.Remove (aStructIter);
        if (!theStructure->IsAlwaysRendered()
            && !isForChangePriority)
        {
          myBVHPrimitives.Remove (theStructure);
        }
        --myNbStructures;
        return aPriorityIter;
      }
    }
  }

  return -1;
}

// =======================================================================
// function : InvalidateBVHData
// purpose  :
// =======================================================================
void OpenGl_PriorityList::InvalidateBVHData()
{
  myIsBVHPrimitivesNeedsReset = Standard_True;
}

// =======================================================================
// function : Render
// purpose  :
// =======================================================================
void OpenGl_PriorityList::Render (const Handle(OpenGl_Workspace)& theWorkspace) const
{
  theWorkspace->IsCullingEnabled() ? renderTraverse (theWorkspace) : renderAll (theWorkspace);
}

// =======================================================================
// function : renderAll
// purpose  :
// =======================================================================
void OpenGl_PriorityList::renderAll (const Handle(OpenGl_Workspace)& theWorkspace) const
{
  const Standard_Integer aNbPriorities = myArray.Length();
  OpenGl_SequenceOfStructure::Iterator aStructIter;
  for (Standard_Integer aPriorityIter = 0; aPriorityIter < aNbPriorities; ++aPriorityIter)
  {
    for (aStructIter.Init (myArray (aPriorityIter)); aStructIter.More(); aStructIter.Next())
    {
      aStructIter.Value()->Render (theWorkspace);
    }
  }
}

// =======================================================================
// function : renderTraverse
// purpose  :
// =======================================================================
void OpenGl_PriorityList::renderTraverse (const Handle(OpenGl_Workspace)& theWorkspace) const
{
  if (myIsBVHPrimitivesNeedsReset)
  {
    myBVHPrimitives.Assign (myArray);
    myIsBVHPrimitivesNeedsReset = Standard_False;
  }

  OpenGl_BVHTreeSelector& aSelector = theWorkspace->ActiveView()->BVHTreeSelector();
  traverse (aSelector);

  const Standard_Integer aNbPriorities = myArray.Length();
  OpenGl_SequenceOfStructure::Iterator aStructIter;
  for (Standard_Integer aPriorityIter = 0; aPriorityIter < aNbPriorities; ++aPriorityIter)
  {
    for (aStructIter.Init (myArray (aPriorityIter)); aStructIter.More(); aStructIter.Next())
    {
      if (!aStructIter.Value()->IsCulled())
      {
        aStructIter.Value()->Render (theWorkspace);
        aStructIter.Value()->ResetCullingStatus();
      }
    }
  }
}

// =======================================================================
// function : traverse
// purpose  :
// =======================================================================
void OpenGl_PriorityList::traverse (OpenGl_BVHTreeSelector& theSelector) const
{
  // handle a case when all objects are infinite
  if (myBVHPrimitives.Size() == 0)
    return;

  const NCollection_Handle<BVH_Tree<Standard_ShortReal, 4> >& aBVHTree = myBVHPrimitives.BVH();

  Standard_Integer aNode = 0; // a root node
  theSelector.CacheClipPtsProjections();
  if (!theSelector.Intersect (aBVHTree->MinPoint (0),
                              aBVHTree->MaxPoint (0)))
  {
    return;
  }

  Standard_Integer aStack[32];
  Standard_Integer aHead = -1;
  for (;;)
  {
    if (!aBVHTree->IsOuter (aNode))
    {
      const Standard_Integer aLeftChildIdx  = aBVHTree->LeftChild  (aNode);
      const Standard_Integer aRightChildIdx = aBVHTree->RightChild (aNode);
      const Standard_Boolean isLeftChildIn  = theSelector.Intersect (aBVHTree->MinPoint (aLeftChildIdx),
                                                                     aBVHTree->MaxPoint (aLeftChildIdx));
      const Standard_Boolean isRightChildIn = theSelector.Intersect (aBVHTree->MinPoint (aRightChildIdx),
                                                                     aBVHTree->MaxPoint (aRightChildIdx));
      if (isLeftChildIn
       && isRightChildIn)
      {
        aNode = myBVHIsLeftChildQueuedFirst ? aLeftChildIdx : aRightChildIdx;
        ++aHead;
        aStack[aHead] = myBVHIsLeftChildQueuedFirst ? aRightChildIdx : aLeftChildIdx;
        myBVHIsLeftChildQueuedFirst = !myBVHIsLeftChildQueuedFirst;
      }
      else if (isLeftChildIn
            || isRightChildIn)
      {
        aNode = isLeftChildIn ? aLeftChildIdx : aRightChildIdx;
      }
      else
      {
        if (aHead < 0)
        {
          return;
        }

        aNode = aStack[aHead];
        --aHead;
      }
    }
    else
    {
      if (theSelector.Intersect (aBVHTree->MinPoint (aNode),
                                 aBVHTree->MaxPoint (aNode)))
      {
        Standard_Integer aIdx = aBVHTree->BegPrimitive (aNode);
        myBVHPrimitives.GetStructureById (aIdx)->MarkAsNotCulled();
        if (aHead < 0)
        {
          return;
        }

        aNode = aStack[aHead];
        --aHead;
      }
    }
  }
}

// =======================================================================
// function : Append
// purpose  :
// =======================================================================
Standard_Boolean OpenGl_PriorityList::Append (const OpenGl_PriorityList& theOther)
{
  // the source priority list shouldn't have more priorities
  const Standard_Integer aNbPriorities = theOther.NbPriorities ();
  if (aNbPriorities > NbPriorities())
  {
    return Standard_False;
  }

  // add all structures to destination priority list
  OpenGl_SequenceOfStructure::Iterator aStructIter;
  for (Standard_Integer aPriorityIter = 0; aPriorityIter < aNbPriorities; ++aPriorityIter)
  {
    const OpenGl_SequenceOfStructure& aSeq = theOther.myArray (aPriorityIter);
    for (aStructIter.Init (aSeq); aStructIter.More(); aStructIter.Next())
    {
      Add (aStructIter.Value(), aPriorityIter);
    }
  }

  return Standard_True;
}

// =======================================================================
// function : NbPriorities
// purpose  :
// =======================================================================
Standard_Integer OpenGl_PriorityList::NbPriorities() const
{
  return myArray.Length();
}
