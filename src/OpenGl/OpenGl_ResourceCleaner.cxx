// Created on: 2011-03-18
// Created by: Anton POLETAEV
// Copyright (c) 2011-2012 OPEN CASCADE SAS
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


#include <OpenGl_ResourceCleaner.hxx>
#include <OpenGl_ResourceVBO.hxx>

//=======================================================================
//function : OpenGl_ResourceCleaner
//purpose  : Constructor
//=======================================================================

OpenGl_ResourceCleaner::OpenGl_ResourceCleaner() 
{
  mySharedContexts.Clear();
  mySharedQueue.Clear();
  myInstanceQueue.Clear();
}

//=======================================================================
//function : AppendContext
//purpose  : Append given OpenGl context to the OpenGl_ResourceCleaner
//           control list
//=======================================================================

void OpenGl_ResourceCleaner::AppendContext(GLCONTEXT theContext, Standard_Boolean isShared)
{  
  
  // appending shared context
  if (isShared) 
  {
    mySharedContexts.Add(theContext);
  }
  else
  {
    // if context is already in the list
    if (myInstanceQueue.IsBound(theContext)) 
    {
      QueueOfResources *aQueue = &myInstanceQueue.ChangeFind(theContext);
      aQueue->Clear();
    }
    else 
    {
      // context is not in the list - create empty queue for it and add queue to the list
      QueueOfResources aQueue;
      aQueue.Clear();
      myInstanceQueue.Bind(theContext, aQueue);
    }
  }

}

//=======================================================================
//function : AddResource
//purpose  : Tell the OpenGl_ResourceCleaner to clean up the OpenGl 
//           memory resource
//=======================================================================

Standard_Boolean OpenGl_ResourceCleaner::AddResource(GLCONTEXT theContext, const Handle(OpenGl_Resource)& theResource) 
{
  // if context found in the shared list
  if (mySharedContexts.Contains(theContext)) 
  {
    mySharedQueue.Push(theResource);
    return Standard_True;
  }
  // if not, then it might be found in the non-shared list
  else if (myInstanceQueue.IsBound(theContext)) 
  {
    QueueOfResources * aQueue = &myInstanceQueue.ChangeFind(theContext);
    aQueue->Push(theResource);
    return Standard_True;
  }
  
  // context is not under OpenGl_ResourceCleaner control, do not tell to clean the resource
  return Standard_False;
}

//=======================================================================
//function : Clear
//purpose  : Cancel clean procedure for all the resources added to the 
//           OpenGl_ResourceCleaner 
//=======================================================================

void OpenGl_ResourceCleaner::Clear() 
{
  mySharedQueue.Clear();
  DataMapOfContextsResources::Iterator anIter(myInstanceQueue);

  // remove the resources from the list
  for (anIter.Reset(); anIter.More(); anIter.Next()) 
  {
    QueueOfResources * aQueue = &anIter.ChangeValue();
    aQueue->Clear();
  }

}

//=======================================================================
//function : Clear
//purpose  : Cancel clean procedure for all the resources of the specific
//           OpenGl context which were added to the OpenGl_ResourceCleaner
//=======================================================================

Standard_Boolean OpenGl_ResourceCleaner::Clear(GLCONTEXT theContext) 
{
  // check if the context in the the control list
  if (myInstanceQueue.IsBound(theContext)) 
  {
    QueueOfResources * aQueue = &myInstanceQueue.ChangeFind(theContext);
    aQueue->Clear();
    return Standard_True;
  }

  return Standard_False;
}

//=======================================================================
//function : ClearShared
//purpose  : Cancel clean procedure for all of the shared resources
//=======================================================================

void OpenGl_ResourceCleaner::ClearShared()
{
  mySharedQueue.Clear();
}

//=======================================================================
//function : Cleanup
//purpose  : Clear the unused resources for active OpenGl context
//=======================================================================

void OpenGl_ResourceCleaner::Cleanup (const Handle(OpenGl_Context)& theGlContext)
{
  GLCONTEXT aContext = GET_GL_CONTEXT();
  if (aContext == NULL)
    return;

  // if the context is found in shared list
  if (mySharedContexts.Contains (aContext)) 
  {
    while (mySharedQueue.Size() > 0) 
    {
      mySharedQueue.Front()->Clean (theGlContext); // delete resource memory
      mySharedQueue.Pop();
    }
  }
  // if the context is found in non-shared list
  else if (myInstanceQueue.IsBound (aContext)) 
  {
    QueueOfResources* aQueue = &myInstanceQueue.ChangeFind (aContext);
    while (aQueue->Size() > 0) 
    {
      aQueue->Front()->Clean (theGlContext); // delete resource memory
      aQueue->Pop();
    }
  }
}

//=======================================================================
//function : RemoveContext
//purpose  : Remove the OpenGl context from the OpenGl_ResourceCleaner list
//=======================================================================

void OpenGl_ResourceCleaner::RemoveContext(GLCONTEXT theContext)
{
  // if context can be found in shared list try to remove it
  // if it wasn't in there, try to remove it from non-shared list
  if (!mySharedContexts.Remove(theContext))
    myInstanceQueue.UnBind(theContext);  

  // if no contexts in shared list, then there is no need to clean
  // the resources on redraw
  if (mySharedContexts.Size() == 0)
    mySharedQueue.Clear();

}

//=======================================================================
//function : GetSharedContext
//purpose  : Get any of shared contexts from the OpenGl_ResourceCleaner list
//=======================================================================

GLCONTEXT OpenGl_ResourceCleaner::GetSharedContext() const
{
  if(mySharedContexts.Size() > 0) 
  {
    MapOfContexts::Iterator anIter(mySharedContexts);
    anIter.Reset();
    return anIter.Value();
  }

  return 0;
}

//=======================================================================
//function : GetInstance
//purpose  : Get the global instance of OpenGl_ResourceCleaner
//=======================================================================

OpenGl_ResourceCleaner* OpenGl_ResourceCleaner::GetInstance()
{
  // the static instance entity
  static OpenGl_ResourceCleaner* anInstance = NULL;

  if (anInstance == NULL)
    anInstance = new OpenGl_ResourceCleaner;

  return anInstance;
}
