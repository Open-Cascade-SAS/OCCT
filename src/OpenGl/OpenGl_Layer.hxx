// Created on: 2011-11-02
// Created by: Sergey ZERCHANINOV
// Copyright (c) 2011-2015 OPEN CASCADE SAS
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

#ifndef _OpenGl_Layer_Header
#define _OpenGl_Layer_Header

#include <NCollection_Sequence.hxx>
#include <NCollection_Array1.hxx>

#include <InterfaceGraphic_telem.hxx>

#include <Handle_OpenGl_Workspace.hxx>
#include <OpenGl_BVHClipPrimitiveSet.hxx>
#include <OpenGl_BVHTreeSelector.hxx>

#include <Graphic3d_ZLayerSettings.hxx>
#include <OpenGl_GlCore11.hxx>

class Handle(OpenGl_Workspace);

struct OpenGl_GlobalLayerSettings
{
  GLint DepthFunc;
  GLboolean DepthMask;
};

//! Presentations list sorted within priorities.
class OpenGl_Layer
{
public:

  //! Initializes associated priority list and layer properties
  OpenGl_Layer (const Standard_Integer theNbPriorities = 11);

  //! Destructor.
  virtual ~OpenGl_Layer();

  //! Returns settings of the layer object.
  const Graphic3d_ZLayerSettings& LayerSettings() const { return myLayerSettings; };

  //! Sets settings of the layer object.
  void SetLayerSettings (const Graphic3d_ZLayerSettings& theSettings)
  {
    myLayerSettings = theSettings;
  }

  //! Returns true if theSetting is enabled for the layer.
  const Standard_Boolean IsSettingEnabled (const Graphic3d_ZLayerSetting theSetting) const
  {
    return myLayerSettings.IsSettingEnabled (theSetting);
  }

  void Add (const OpenGl_Structure* theStruct,
            const Standard_Integer  thePriority,
            Standard_Boolean        isForChangePriority = Standard_False);

  //! Remove structure and returns its priority, if the structure is not found, method returns negative value
  bool Remove (const OpenGl_Structure* theStruct,
               Standard_Integer&       thePriority,
               Standard_Boolean        isForChangePriority = Standard_False);

  //! @return the number of structures
  Standard_Integer NbStructures() const { return myNbStructures; }

  //! Returns the number of available priority levels
  Standard_Integer NbPriorities() const { return myArray.Length(); }

  //! Append layer of acceptable type (with similar number of priorities or less).
  //! Returns Standard_False if the list can not be accepted.
  Standard_Boolean Append (const OpenGl_Layer& theOther);

  //! Returns array of OpenGL structures.
  const OpenGl_ArrayOfIndexedMapOfStructure& ArrayOfStructures() const { return myArray; }

  //! Marks BVH tree for given priority list as dirty and
  //! marks primitive set for rebuild.
  void InvalidateBVHData();

  // Render all structures.
  void Render (const Handle(OpenGl_Workspace)&   theWorkspace,
               const OpenGl_GlobalLayerSettings& theDefaultSettings) const;

protected:

  //! Traverses through BVH tree to determine which structures are in view volume.
  void traverse (OpenGl_BVHTreeSelector& theSelector) const;

  //! Iterates through the hierarchical list of existing structures and renders them all.
  void renderAll (const Handle(OpenGl_Workspace)& theWorkspace) const;

  //! Iterates through the hierarchical list of existing structures and renders only overlapping ones.
  void renderTraverse (const Handle(OpenGl_Workspace)& theWorkspace) const;

private:

  OpenGl_ArrayOfIndexedMapOfStructure myArray;
  Standard_Integer                    myNbStructures;
  Graphic3d_ZLayerSettings            myLayerSettings;             //!< Layer setting flags
  mutable OpenGl_BVHClipPrimitiveSet  myBVHPrimitives;             //!< Set of OpenGl_Structures for building BVH tree
  mutable Standard_Boolean            myBVHIsLeftChildQueuedFirst; //!< Is needed for implementation of stochastic order of BVH traverse
  mutable Standard_Boolean            myIsBVHPrimitivesNeedsReset; //!< Defines if the primitive set for BVH is outdated

public:

  DEFINE_STANDARD_ALLOC

};

#endif //_OpenGl_Layer_Header
