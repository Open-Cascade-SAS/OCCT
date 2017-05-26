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

#include <OpenGl_BVHClipPrimitiveSet.hxx>
#include <OpenGl_BVHClipPrimitiveTrsfPersSet.hxx>
#include <OpenGl_BVHTreeSelector.hxx>

#include <Graphic3d_ZLayerSettings.hxx>
#include <Graphic3d_Camera.hxx>
#include <OpenGl_GlCore11.hxx>


struct OpenGl_GlobalLayerSettings
{
  GLint DepthFunc;
  GLboolean DepthMask;
};

//! Defines index map of OpenGL structures.
typedef NCollection_IndexedMap<const OpenGl_Structure*> OpenGl_IndexedMapOfStructure;

//! Defines array of indexed maps of OpenGL structures.
typedef NCollection_Array1<OpenGl_IndexedMapOfStructure> OpenGl_ArrayOfIndexedMapOfStructure;

//! Presentations list sorted within priorities.
class OpenGl_Layer : public Standard_Transient
{
  DEFINE_STANDARD_RTTIEXT(OpenGl_Layer, Standard_Transient)
public:

  //! Initializes associated priority list and layer properties
  OpenGl_Layer (const Standard_Integer theNbPriorities,
                const Handle(Select3D_BVHBuilder3d)& theBuilder);

  //! Destructor.
  virtual ~OpenGl_Layer();

  //! Returns BVH tree builder for frustom culling.
  const Handle(Select3D_BVHBuilder3d)& FrustumCullingBVHBuilder() const { return myBVHPrimitivesTrsfPers.Builder(); }

  //! Assigns BVH tree builder for frustom culling.
  void SetFrustumCullingBVHBuilder (const Handle(Select3D_BVHBuilder3d)& theBuilder) { myBVHPrimitivesTrsfPers.SetBuilder (theBuilder); }

  //! Return true if layer was marked with immediate flag.
  Standard_Boolean IsImmediate() const  { return myLayerSettings.IsImmediate(); }

  //! Returns settings of the layer object.
  const Graphic3d_ZLayerSettings& LayerSettings() const { return myLayerSettings; };

  //! Sets settings of the layer object.
  void SetLayerSettings (const Graphic3d_ZLayerSettings& theSettings);

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
  void InvalidateBVHData() const;

  //! Marks cached bounding box as obsolete.
  void InvalidateBoundingBox() const
  {
    myIsBoundingBoxNeedsReset[0] = myIsBoundingBoxNeedsReset[1] = true;
  }

  //! Returns layer bounding box.
  //! @param theViewId             view index to consider View Affinity in structure
  //! @param theCamera             camera definition
  //! @param theWindowWidth        viewport width  (for applying transformation-persistence)
  //! @param theWindowHeight       viewport height (for applying transformation-persistence)
  //! @param theToIncludeAuxiliary consider also auxiliary presentations (with infinite flag or with trihedron transformation persistence)
  //! @return computed bounding box
  Bnd_Box BoundingBox (const Standard_Integer          theViewId,
                       const Handle(Graphic3d_Camera)& theCamera,
                       const Standard_Integer          theWindowWidth,
                       const Standard_Integer          theWindowHeight,
                       const Standard_Boolean          theToIncludeAuxiliary) const;

  //! Returns zoom-scale factor.
  Standard_Real considerZoomPersistenceObjects (const Standard_Integer          theViewId,
                                                const Handle(Graphic3d_Camera)& theCamera,
                                                const Standard_Integer          theWindowWidth,
                                                const Standard_Integer          theWindowHeight) const;

  // Render all structures.
  void Render (const Handle(OpenGl_Workspace)&   theWorkspace,
               const OpenGl_GlobalLayerSettings& theDefaultSettings) const;

  //! Returns number of transform persistence objects.
  Standard_Integer NbOfTransformPersistenceObjects() const
  {
    return myBVHPrimitivesTrsfPers.Size();
  }

protected:

  //! Updates BVH trees if their state has been invalidated.
  void updateBVH() const;

  //! Traverses through BVH tree to determine which structures are in view volume.
  void traverse (OpenGl_BVHTreeSelector& theSelector) const;

  //! Iterates through the hierarchical list of existing structures and renders them all.
  void renderAll (const Handle(OpenGl_Workspace)& theWorkspace) const;

  //! Iterates through the hierarchical list of existing structures and renders only overlapping ones.
  void renderTraverse (const Handle(OpenGl_Workspace)& theWorkspace) const;

private:

  //! Array of OpenGl_Structures by priority rendered in layer.
  OpenGl_ArrayOfIndexedMapOfStructure myArray;

  //! Overall number of structures rendered in the layer.
  Standard_Integer myNbStructures;

  //! Layer setting flags.
  Graphic3d_ZLayerSettings myLayerSettings;

  //! Set of OpenGl_Structures structures for building BVH tree.
  mutable OpenGl_BVHClipPrimitiveSet myBVHPrimitives;

  //! Set of transform persistent OpenGl_Structures for building BVH tree.
  mutable OpenGl_BVHClipPrimitiveTrsfPersSet myBVHPrimitivesTrsfPers;

  //! Indexed map of always rendered structures.
  mutable NCollection_IndexedMap<const OpenGl_Structure*> myAlwaysRenderedMap;

  //! Is needed for implementation of stochastic order of BVH traverse.
  mutable Standard_Boolean myBVHIsLeftChildQueuedFirst;

  //! Defines if the primitive set for BVH is outdated.
  mutable Standard_Boolean myIsBVHPrimitivesNeedsReset;

  //! Defines if the cached bounding box is outdated.
  mutable bool myIsBoundingBoxNeedsReset[2];

  //! Cached layer bounding box.
  mutable Bnd_Box myBoundingBox[2];

public:

  DEFINE_STANDARD_ALLOC

};

#endif //_OpenGl_Layer_Header
