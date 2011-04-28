// File:      OpenGl_ResourceCleaner.hxx
// Created:   18.03.11 9:30:00
// Author:    Anton POLETAEV

#ifndef _OPENGL_RESOURCECLEANER_H
#define _OPENGL_RESOURCECLEANER_H

#include <OpenGl_tgl_all.hxx>
#include <NCollection_Queue.hxx>
#include <NCollection_List.hxx>
#include <NCollection_Map.hxx>
#include <NCollection_DataMap.hxx>
#include <OpenGl_Resource.hxx>

class OpenGl_Resource;
class Handle_OpenGl_Resource;

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
  Standard_Boolean AddResource(GLCONTEXT theContext, Handle_OpenGl_Resource theResource);

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
  void Cleanup();

  //! Remove the OpenGl context from the OpenGl_ResourceCleaner control list.
  void RemoveContext(GLCONTEXT theContext);

  //! Get any of shared contexts from the OpenGl_ResourceCleaner list 
  //! to share resources with a new one
  GLCONTEXT GetSharedContext() const;

  //! Get the global instance of OpenGl_ResourceCleaner
  static OpenGl_ResourceCleaner* GetInstance();

private:

  DataMapOfContextsResources myInstanceQueue;  // map for queues of non-shared context's resources
  QueueOfResources mySharedQueue;              // queue of shared context's resources
  MapOfContexts mySharedContexts;              // the control list of shared contexts

};

#endif
