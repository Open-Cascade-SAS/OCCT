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

#ifndef _OpenGl_PriorityList_Header
#define _OpenGl_PriorityList_Header

#include <NCollection_Sequence.hxx>
#include <NCollection_Array1.hxx>

#include <InterfaceGraphic_telem.hxx>

#include <Handle_OpenGl_Workspace.hxx>
#include <OpenGl_BVHClipPrimitiveSet.hxx>
#include <OpenGl_BVHTreeSelector.hxx>

class OpenGl_Structure;

typedef NCollection_Sequence<const OpenGl_Structure*> OpenGl_SequenceOfStructure;
typedef NCollection_Array1<OpenGl_SequenceOfStructure> OpenGl_ArrayOfStructure;

class OpenGl_PriorityList
{
public:

  // Empty constructor.
  OpenGl_PriorityList (const Standard_Integer theNbPriorities = 11);

  //! Destructor.
  virtual ~OpenGl_PriorityList();

  void Add (const OpenGl_Structure* theStructure,
            const Standard_Integer  thePriority,
            Standard_Boolean isForChangePriority = Standard_False);

  //! Remove structure and returns its priority, if the structure is not found, method returns negative value
  Standard_Integer Remove (const OpenGl_Structure* theStructure,
                           Standard_Boolean isForChangePriority = Standard_False);

  //! @return the number of structures
  Standard_Integer NbStructures() const { return myNbStructures; }

  // Render all structures.
  void Render (const Handle(OpenGl_Workspace)& theWorkspace) const;

  //! Returns the number of available priority levels
  Standard_Integer NbPriorities() const;

  //! Append priority list of acceptable type (with similar number of priorities
  //! or less). Returns Standard_False if the list can not be accepted.
  Standard_Boolean Append (const OpenGl_PriorityList& theOther);

  //! Returns array of OpenGL structures.
  const OpenGl_ArrayOfStructure& ArrayOfStructures() const { return myArray; }

  //! Marks BVH tree for given priority list as dirty and
  //! marks primitive set for rebuild.
  void InvalidateBVHData();

protected:

  //! Traverses through BVH tree to determine which structures are in view volume.
  void traverse (OpenGl_BVHTreeSelector& theSelector) const;

  //! Iterates through the hierarchical list of existing structures and renders them all.
  void renderAll (const Handle(OpenGl_Workspace)& theWorkspace) const;

  //! Iterates through the hierarchical list of existing structures and renders only overlapping ones.
  void renderTraverse (const Handle(OpenGl_Workspace)& theWorkspace) const;

protected:

  OpenGl_ArrayOfStructure            myArray;
  Standard_Integer                   myNbStructures;
  mutable OpenGl_BVHClipPrimitiveSet myBVHPrimitives;             //<! Set of OpenGl_Structures for building BVH tree
  mutable Standard_Boolean           myBVHIsLeftChildQueuedFirst; //<! Is needed for implementation of stochastic order of BVH traverse
  mutable Standard_Boolean           myIsBVHPrimitivesNeedsReset; //<! Defines if the primitive set for BVH is outdated

public:

  DEFINE_STANDARD_ALLOC

};

#endif // _OpenGl_PriorityList_Header
