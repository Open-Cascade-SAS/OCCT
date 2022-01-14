// Copyright (c) 2011-2019 OPEN CASCADE SAS
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

#ifndef _Graphic3d_Layer_HeaderFile
#define _Graphic3d_Layer_HeaderFile

#include <Graphic3d_BvhCStructureSet.hxx>
#include <Graphic3d_BvhCStructureSetTrsfPers.hxx>
#include <Graphic3d_DisplayPriority.hxx>
#include <Graphic3d_ZLayerId.hxx>
#include <Graphic3d_ZLayerSettings.hxx>
#include <Graphic3d_RenderingParams.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_IndexedMap.hxx>

#include <array>

//! Defines index map of structures.
typedef NCollection_IndexedMap<const Graphic3d_CStructure*> Graphic3d_IndexedMapOfStructure;

//! Defines array of indexed maps of structures.
typedef std::array<Graphic3d_IndexedMapOfStructure, Graphic3d_DisplayPriority_NB> Graphic3d_ArrayOfIndexedMapOfStructure;

class Graphic3d_CullingTool;

//! Presentations list sorted within priorities.
class Graphic3d_Layer : public Standard_Transient
{
  DEFINE_STANDARD_RTTIEXT(Graphic3d_Layer, Standard_Transient)
public:

  //! Initializes associated priority list and layer properties
  Standard_EXPORT Graphic3d_Layer (Graphic3d_ZLayerId theId,
                                   const Handle(Select3D_BVHBuilder3d)& theBuilder);

  //! Destructor.
  Standard_EXPORT virtual ~Graphic3d_Layer();

  //! Return layer id.
  Graphic3d_ZLayerId LayerId() const { return myLayerId; }

  //! Returns BVH tree builder for frustum culling.
  const Handle(Select3D_BVHBuilder3d)& FrustumCullingBVHBuilder() const { return myBVHPrimitivesTrsfPers.Builder(); }

  //! Assigns BVH tree builder for frustum culling.
  void SetFrustumCullingBVHBuilder (const Handle(Select3D_BVHBuilder3d)& theBuilder) { myBVHPrimitivesTrsfPers.SetBuilder (theBuilder); }

  //! Return true if layer was marked with immediate flag.
  Standard_Boolean IsImmediate() const  { return myLayerSettings.IsImmediate(); }

  //! Returns settings of the layer object.
  const Graphic3d_ZLayerSettings& LayerSettings() const { return myLayerSettings; };

  //! Sets settings of the layer object.
  Standard_EXPORT void SetLayerSettings (const Graphic3d_ZLayerSettings& theSettings);

  Standard_EXPORT void Add (const Graphic3d_CStructure* theStruct,
                            Graphic3d_DisplayPriority thePriority,
                            Standard_Boolean isForChangePriority = Standard_False);

  //! Remove structure and returns its priority, if the structure is not found, method returns negative value
  Standard_EXPORT bool Remove (const Graphic3d_CStructure* theStruct,
                               Graphic3d_DisplayPriority& thePriority,
                               Standard_Boolean isForChangePriority = Standard_False);

  //! @return the number of structures
  Standard_Integer NbStructures() const { return myNbStructures; }

  //! Number of NOT culled structures in the layer.
  Standard_Integer NbStructuresNotCulled() const { return myNbStructuresNotCulled; }

  //! Returns the number of available priority levels
  Standard_Integer NbPriorities() const { return Graphic3d_DisplayPriority_NB; }

  //! Append layer of acceptable type (with similar number of priorities or less).
  //! Returns Standard_False if the list can not be accepted.
  Standard_EXPORT Standard_Boolean Append (const Graphic3d_Layer& theOther);

  //! Returns array of structures.
  const Graphic3d_ArrayOfIndexedMapOfStructure& ArrayOfStructures() const { return myArray; }

  //! Returns structures for specified priority.
  const Graphic3d_IndexedMapOfStructure& Structures (Graphic3d_DisplayPriority thePriority) const { return myArray[thePriority]; }

  //! Marks BVH tree for given priority list as dirty and
  //! marks primitive set for rebuild.
  Standard_EXPORT void InvalidateBVHData();

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
  Standard_EXPORT Bnd_Box BoundingBox (Standard_Integer theViewId,
                                       const Handle(Graphic3d_Camera)& theCamera,
                                       Standard_Integer theWindowWidth,
                                       Standard_Integer theWindowHeight,
                                       Standard_Boolean theToIncludeAuxiliary) const;

  //! Returns zoom-scale factor.
  Standard_EXPORT Standard_Real considerZoomPersistenceObjects (Standard_Integer theViewId,
                                                                const Handle(Graphic3d_Camera)& theCamera,
                                                                Standard_Integer theWindowWidth,
                                                                Standard_Integer theWindowHeight) const;

  //! Update culling state - should be called before rendering.
  //! Traverses through BVH tree to determine which structures are in view volume.
  Standard_EXPORT void UpdateCulling (Standard_Integer theViewId,
                                      const Graphic3d_CullingTool& theSelector,
                                      const Graphic3d_RenderingParams::FrustumCulling theFrustumCullingState);

  //! Returns TRUE if layer is empty or has been discarded entirely by culling test.
  bool IsCulled() const { return myNbStructuresNotCulled == 0; }

  //! Returns number of transform persistence objects.
  Standard_Integer NbOfTransformPersistenceObjects() const
  {
    return myBVHPrimitivesTrsfPers.Size();
  }

public:

  //! Returns set of Graphic3d_CStructures structures for building BVH tree.
  const Graphic3d_BvhCStructureSet& CullableStructuresBVH() const { return myBVHPrimitives; }

  //! Returns set of transform persistent Graphic3d_CStructures for building BVH tree.
  const Graphic3d_BvhCStructureSetTrsfPers& CullableTrsfPersStructuresBVH() const { return myBVHPrimitivesTrsfPers; }

  //! Returns indexed map of always rendered structures.
  const NCollection_IndexedMap<const Graphic3d_CStructure*>& NonCullableStructures() const { return myAlwaysRenderedMap; }

  //! Dumps the content of me into the stream
  Standard_EXPORT void DumpJson (Standard_OStream& theOStream, Standard_Integer theDepth = -1) const;

protected:

  //! Updates BVH trees if their state has been invalidated.
  Standard_EXPORT void updateBVH() const;

private:

  //! Array of Graphic3d_CStructures by priority rendered in layer.
  Graphic3d_ArrayOfIndexedMapOfStructure myArray;

  //! Overall number of structures rendered in the layer.
  Standard_Integer myNbStructures;

  //! Number of NOT culled structures in the layer.
  Standard_Integer myNbStructuresNotCulled;

  //! Layer setting flags.
  Graphic3d_ZLayerSettings myLayerSettings;

  //! Layer id.
  Graphic3d_ZLayerId myLayerId;

  //! Set of Graphic3d_CStructures structures for building BVH tree.
  mutable Graphic3d_BvhCStructureSet myBVHPrimitives;

  //! Set of transform persistent Graphic3d_CStructures for building BVH tree.
  mutable Graphic3d_BvhCStructureSetTrsfPers myBVHPrimitivesTrsfPers;

  //! Indexed map of always rendered structures.
  mutable NCollection_IndexedMap<const Graphic3d_CStructure*> myAlwaysRenderedMap;

  //! Is needed for implementation of stochastic order of BVH traverse.
  Standard_Boolean myBVHIsLeftChildQueuedFirst;

  //! Defines if the primitive set for BVH is outdated.
  mutable Standard_Boolean myIsBVHPrimitivesNeedsReset;

  //! Defines if the cached bounding box is outdated.
  mutable bool myIsBoundingBoxNeedsReset[2];

  //! Cached layer bounding box.
  mutable Bnd_Box myBoundingBox[2];

};

#endif // _Graphic3d_Layer_HeaderFile
