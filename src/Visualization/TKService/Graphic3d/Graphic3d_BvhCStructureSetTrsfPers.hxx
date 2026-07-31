// Created on: 2015-06-30
// Created by: Anton POLETAEV
// Copyright (c) 2015 OPEN CASCADE SAS
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

#ifndef _Graphic3d_BvhCStructureSetTrsfPers_HeaderFile
#define _Graphic3d_BvhCStructureSetTrsfPers_HeaderFile

#include <Graphic3d_BndBox3d.hxx>
#include <NCollection_Mat4.hxx>
#include <Standard_TypeDef.hxx>
#include <Graphic3d_WorldViewProjState.hxx>
#include <NCollection_Shared.hxx>
#include <NCollection_IndexedMap.hxx>
#include <BVH_Builder3d.hxx>

class Graphic3d_Camera;
class Graphic3d_CStructure;

//! Set of transformation persistent OpenGl_Structure for building BVH tree.
//! Provides built-in mechanism to invalidate tree when world view projection state changes.
//! Due to frequent invalidation of BVH tree the choice of BVH tree builder is made
//! in favor of BVH linear builder (quick rebuild).
class Graphic3d_BvhCStructureSetTrsfPers : public BVH_Set<double, 3>
{
private:
  typedef NCollection_Shared<Graphic3d_BndBox3d> HBndBox3d;

public:
  //! Creates an empty primitive set for BVH clipping.
  Standard_EXPORT Graphic3d_BvhCStructureSetTrsfPers(const occ::handle<BVH_Builder3d>& theBuilder);

  //! Returns total number of structures.
  Standard_EXPORT int Size() const override;

  //! Returns AABB of the structure.
  Standard_EXPORT Graphic3d_BndBox3d Box(const int theIdx) const override;

  //! Calculates center of the AABB along given axis.
  Standard_EXPORT double Center(const int theIdx, const int theAxis) const override;

  //! Swaps structures with the given indices.
  Standard_EXPORT void Swap(const int theIdx1, const int theIdx2) override;

  //! Adds structure to the set.
  //! @return true if structure added, otherwise returns false (structure already in the set).
  Standard_EXPORT bool Add(const Graphic3d_CStructure* theStruct);

  //! Removes the given structure from the set.
  //! @return true if structure removed, otherwise returns false (structure is not in the set).
  Standard_EXPORT bool Remove(const Graphic3d_CStructure* theStruct);

  //! Cleans the whole primitive set.
  Standard_EXPORT void Clear();

  //! Returns the structure corresponding to the given ID.
  Standard_EXPORT const Graphic3d_CStructure* GetStructureById(int theId);

  //! Access directly a collection of structures.
  const NCollection_IndexedMap<const Graphic3d_CStructure*>& Structures() const
  {
    return myStructs;
  }

  //! Marks object state as outdated (needs BVH rebuilding).
  void MarkDirty() { myIsDirty = true; }

  //! Returns BVH tree for the given world view projection (builds it if necessary).
  Standard_EXPORT const opencascade::handle<BVH_Tree<double, 3>>& BVH(
    const occ::handle<Graphic3d_Camera>& theCamera,
    const NCollection_Mat4<double>&      theProjectionMatrix,
    const NCollection_Mat4<double>&      theWorldViewMatrix,
    const int                            theViewportWidth,
    const int                            theViewportHeight,
    const Graphic3d_WorldViewProjState&  theWVPState);

  //! Returns builder for bottom-level BVH.
  const occ::handle<BVH_Builder3d>& Builder() const { return myBuilder; }

  //! Assigns builder for bottom-level BVH.
  void SetBuilder(const occ::handle<BVH_Builder3d>& theBuilder) { myBuilder = theBuilder; }

private:
  //! Marks internal object state as outdated.
  bool myIsDirty;

  //! Constructed bottom-level BVH.
  opencascade::handle<BVH_Tree<double, 3>> myBVH;

  //! Builder for bottom-level BVH.
  occ::handle<BVH_Builder3d> myBuilder;

  //! Indexed map of structures.
  NCollection_IndexedMap<const Graphic3d_CStructure*> myStructs;

  //! Cached set of bounding boxes precomputed for transformation persistent selectable objects.
  //! Cache exists only during computation of BVH Tree. Bounding boxes are world view projection
  //! dependent and should by synchronized.
  NCollection_IndexedMap<occ::handle<HBndBox3d>> myStructBoxes;

  //! State of world view projection used for generation of transformation persistence bounding
  //! boxes.
  Graphic3d_WorldViewProjState myStructBoxesState;
};

#endif // _Graphic3d_BvhCStructureSetTrsfPers_HeaderFile
