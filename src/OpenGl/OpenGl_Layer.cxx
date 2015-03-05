// Created on: 2014-03-31
// Created by: Danila ULYANOV
// Copyright (c) 2014 OPEN CASCADE SAS
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

#include <OpenGl_Layer.hxx>

#include <OpenGl_BVHTreeSelector.hxx>
#include <OpenGl_Structure.hxx>
#include <OpenGl_View.hxx>
#include <OpenGl_Workspace.hxx>

// =======================================================================
// function : OpenGl_PriorityList
// purpose  :
// =======================================================================
OpenGl_Layer::OpenGl_Layer (const Standard_Integer theNbPriorities)
: myArray (0, theNbPriorities - 1),
  myNbStructures (0),
  myBVHIsLeftChildQueuedFirst (Standard_True),
  myIsBVHPrimitivesNeedsReset (Standard_False)
{
  //
}

// =======================================================================
// function : ~OpenGl_Layer
// purpose  :
// =======================================================================
OpenGl_Layer::~OpenGl_Layer()
{
  //
}

// =======================================================================
// function : Add
// purpose  :
// =======================================================================
void OpenGl_Layer::Add (const OpenGl_Structure* theStruct,
                        const Standard_Integer  thePriority,
                        Standard_Boolean        isForChangePriority)
{
  const Standard_Integer anIndex = Min (Max (thePriority, 0), myArray.Length() - 1);
  if (theStruct == NULL)
  {
    return;
  }

  myArray (anIndex).Append (theStruct);
  if (theStruct->IsAlwaysRendered())
  {
    theStruct->MarkAsNotCulled();
  }
  else if (!isForChangePriority)
  {
    myBVHPrimitives.Add (theStruct);
  }
  ++myNbStructures;
}

// =======================================================================
// function : Remove
// purpose  :
// =======================================================================
bool OpenGl_Layer::Remove (const OpenGl_Structure* theStruct,
                           Standard_Integer&       thePriority,
                           Standard_Boolean        isForChangePriority)
{
  if (theStruct == NULL)
  {
    thePriority = -1;
    return false;
  }

  const Standard_Integer aNbPriorities = myArray.Length();
  for (Standard_Integer aPriorityIter = 0; aPriorityIter < aNbPriorities; ++aPriorityIter)
  {
    OpenGl_SequenceOfStructure& aSeq = myArray (aPriorityIter);
    for (OpenGl_SequenceOfStructure::Iterator aStructIter (aSeq); aStructIter.More(); aStructIter.Next())
    {
      if (aStructIter.Value() == theStruct)
      {
        aSeq.Remove (aStructIter);
        if (!theStruct->IsAlwaysRendered()
         && !isForChangePriority)
        {
          myBVHPrimitives.Remove (theStruct);
        }
        --myNbStructures;
        thePriority = aPriorityIter;
        return true;
      }
    }
  }

  thePriority = -1;
  return false;
}

// =======================================================================
// function : InvalidateBVHData
// purpose  :
// =======================================================================
void OpenGl_Layer::InvalidateBVHData()
{
  myIsBVHPrimitivesNeedsReset = Standard_True;
}

// =======================================================================
// function : renderAll
// purpose  :
// =======================================================================
void OpenGl_Layer::renderAll (const Handle(OpenGl_Workspace)& theWorkspace) const
{
  const Standard_Integer aNbPriorities = myArray.Length();
  const Standard_Integer aViewId       = theWorkspace->ActiveViewId();
  for (Standard_Integer aPriorityIter = 0; aPriorityIter < aNbPriorities; ++aPriorityIter)
  {
    for (OpenGl_SequenceOfStructure::Iterator aStructIter (myArray (aPriorityIter)); aStructIter.More(); aStructIter.Next())
    {
      const OpenGl_Structure* aStruct = aStructIter.Value();
      if (!aStruct->IsVisible())
      {
        continue;
      }
      else if (!aStruct->ViewAffinity.IsNull()
            && !aStruct->ViewAffinity->IsVisible (aViewId))
      {
        continue;
      }

      aStruct->Render (theWorkspace);
    }
  }
}

// =======================================================================
// function : renderTraverse
// purpose  :
// =======================================================================
void OpenGl_Layer::renderTraverse (const Handle(OpenGl_Workspace)& theWorkspace) const
{
  if (myIsBVHPrimitivesNeedsReset)
  {
    myBVHPrimitives.Assign (myArray);
    myIsBVHPrimitivesNeedsReset = Standard_False;
  }

  OpenGl_BVHTreeSelector& aSelector = theWorkspace->ActiveView()->BVHTreeSelector();
  traverse (aSelector);

  const Standard_Integer aNbPriorities = myArray.Length();
  const Standard_Integer aViewId       = theWorkspace->ActiveViewId();
  for (Standard_Integer aPriorityIter = 0; aPriorityIter < aNbPriorities; ++aPriorityIter)
  {
    for (OpenGl_SequenceOfStructure::Iterator aStructIter (myArray (aPriorityIter)); aStructIter.More(); aStructIter.Next())
    {
      const OpenGl_Structure* aStruct = aStructIter.Value();
      if (!aStruct->IsVisible()
        || aStruct->IsCulled())
      {
        continue;
      }
      else if (!aStruct->ViewAffinity.IsNull()
            && !aStruct->ViewAffinity->IsVisible (aViewId))
      {
        continue;
      }

      aStruct->Render (theWorkspace);
      aStruct->ResetCullingStatus();
    }
  }
}

// =======================================================================
// function : traverse
// purpose  :
// =======================================================================
void OpenGl_Layer::traverse (OpenGl_BVHTreeSelector& theSelector) const
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
        aStack[++aHead] = myBVHIsLeftChildQueuedFirst ? aRightChildIdx : aLeftChildIdx;
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

        aNode = aStack[aHead--];
      }
    }
    else
    {
      Standard_Integer aIdx = aBVHTree->BegPrimitive (aNode);
      myBVHPrimitives.GetStructureById (aIdx)->MarkAsNotCulled();
      if (aHead < 0)
      {
        return;
      }

      aNode = aStack[aHead--];
    }
  }
}

// =======================================================================
// function : Append
// purpose  :
// =======================================================================
Standard_Boolean OpenGl_Layer::Append (const OpenGl_Layer& theOther)
{
  // the source priority list shouldn't have more priorities
  const Standard_Integer aNbPriorities = theOther.NbPriorities();
  if (aNbPriorities > NbPriorities())
  {
    return Standard_False;
  }

  // add all structures to destination priority list
  for (Standard_Integer aPriorityIter = 0; aPriorityIter < aNbPriorities; ++aPriorityIter)
  {
    for (OpenGl_SequenceOfStructure::Iterator aStructIter (theOther.myArray (aPriorityIter)); aStructIter.More(); aStructIter.Next())
    {
      Add (aStructIter.Value(), aPriorityIter);
    }
  }

  return Standard_True;
}

//=======================================================================
//function : Render
//purpose  :
//=======================================================================
void OpenGl_Layer::Render (const Handle(OpenGl_Workspace)&   theWorkspace,
                           const OpenGl_GlobalLayerSettings& theDefaultSettings) const
{
  TEL_POFFSET_PARAM anAppliedOffsetParams = theWorkspace->AppliedPolygonOffset();

  // separate depth buffers
  if (IsSettingEnabled (Graphic3d_ZLayerDepthClear))
  {
    glClear (GL_DEPTH_BUFFER_BIT);
  }
 
  // handle depth test
  if (IsSettingEnabled (Graphic3d_ZLayerDepthTest))
  {
    // assuming depth test is enabled by default
    glDepthFunc (theDefaultSettings.DepthFunc);
  }
  else
  {
    glDepthFunc (GL_ALWAYS);
  }
  
  // handle depth offset
  if (IsSettingEnabled (Graphic3d_ZLayerDepthOffset))
  {
    theWorkspace->SetPolygonOffset (Aspect_POM_Fill,
                                    myLayerSettings.DepthOffsetFactor,
                                    myLayerSettings.DepthOffsetUnits);
  }
  else
  {
    theWorkspace->SetPolygonOffset (anAppliedOffsetParams.mode,
                                    anAppliedOffsetParams.factor,
                                    anAppliedOffsetParams.units);
  }

  // handle depth write
  glDepthMask (IsSettingEnabled (Graphic3d_ZLayerDepthWrite) ? GL_TRUE : GL_FALSE);

  // render priority list
  theWorkspace->IsCullingEnabled() ? renderTraverse (theWorkspace) : renderAll (theWorkspace);

  // always restore polygon offset between layers rendering
  theWorkspace->SetPolygonOffset (anAppliedOffsetParams.mode,
                                  anAppliedOffsetParams.factor,
                                  anAppliedOffsetParams.units);
}
