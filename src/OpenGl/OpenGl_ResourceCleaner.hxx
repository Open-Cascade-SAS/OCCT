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


#ifndef _OPENGL_RESOURCECLEANER_H
#define _OPENGL_RESOURCECLEANER_H

#include <OpenGl_GlCore11.hxx>
#include <NCollection_Queue.hxx>
#include <NCollection_List.hxx>
#include <NCollection_Map.hxx>
#include <NCollection_DataMap.hxx>
#include <OpenGl_Resource.hxx>

class OpenGl_Resource;
class Handle(OpenGl_Resource);
class Handle(OpenGl_Context);

typedef NCollection_Queue<Handle_OpenGl_Resource> QueueOfResources;
typedef NCollection_DataMap<GLCONTEXT, QueueOfResources> DataMapOfContextsResources;
typedef NCollection_Map<GLCONTEXT> MapOfContexts;

//! OpenGl_ResourceCleaner should be used to clean OpenGl memory resources;
//! The reason is that the resources might be shared between the contexts and
//! should be cleaned up only while suitable context is active;
class OpenGl_ResourceCleaner
{

public:
	
  //! Constructor
  OpenGl_ResourceCleaner();

  //! Destructor
  virtual ~OpenGl_ResourceCleaner() { }

  //! Append OpenGl context to the OpenGl_ResourceCleaner
  //! control list.
  void AppendContext(GLCONTEXT theContext, Standard_Boolean isShared);	
  
  //! Tell the OpenGl_ResourceCleaner to clean up the OpenGl memory resource
  //! which has been created by the specified OpenGl context;
  //! The context should be in the OpenGl_ResourceCleaner control list.
  Standard_EXPORT Standard_Boolean AddResource(GLCONTEXT theContext, const Handle(OpenGl_Resource)& theResource);

  //! Cancel clean procedure for all the resources added to the OpenGl_ResourceCleaner.
  void Clear();

  //! Cancel clean procedure for all the resources of the specific OpenGl context 
  //! which were added to the OpenGl_ResourceCleaner.
  Standard_Boolean Clear(GLCONTEXT theContext);

  //! Cancel clean procedure for all of the shared resources.
  void ClearShared();

  //! Clear the unused resources for active OpenGl context;
  //! You should add the cleaner resources by AddResources method;
  //! It is suggested to call this method right before the OpenGl
  //! new frame drawing procedure starts.
  void Cleanup (const Handle(OpenGl_Context)& theGlContext);

  //! Remove the OpenGl context from the OpenGl_ResourceCleaner control list.
  void RemoveContext(GLCONTEXT theContext);

  //! Get any of shared contexts from the OpenGl_ResourceCleaner list 
  //! to share resources with a new one
  GLCONTEXT GetSharedContext() const;

  //! Get the global instance of OpenGl_ResourceCleaner
  Standard_EXPORT static OpenGl_ResourceCleaner* GetInstance();

private:

  DataMapOfContextsResources myInstanceQueue;  // map for queues of non-shared context's resources
  QueueOfResources mySharedQueue;              // queue of shared context's resources
  MapOfContexts mySharedContexts;              // the control list of shared contexts

};

#endif
