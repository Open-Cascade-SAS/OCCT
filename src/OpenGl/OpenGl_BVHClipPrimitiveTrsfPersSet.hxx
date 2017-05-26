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

#ifndef _OpenGl_BVHClipPrimitiveTrsfPersSet_HeaderFile
#define _OpenGl_BVHClipPrimitiveTrsfPersSet_HeaderFile

#include <BVH_Set.hxx>
#include <BVH_Tree.hxx>
#include <Graphic3d_BndBox4f.hxx>
#include <Graphic3d_WorldViewProjState.hxx>
#include <NCollection_Shared.hxx>
#include <NCollection_IndexedMap.hxx>
#include <OpenGl_Structure.hxx>
#include <OpenGl_Vec.hxx>
#include <Select3D_BVHBuilder3d.hxx>

//! Set of transformation persistent OpenGl_Structure for building BVH tree.
//! Provides built-in mechanism to invalidate tree when world view projection state changes.
//! Due to frequent invalidation of BVH tree the choice of BVH tree builder is made
//! in favor of BVH linear builder (quick rebuild).
class OpenGl_BVHClipPrimitiveTrsfPersSet : public BVH_Set<Standard_Real, 3>
{
private:

  typedef NCollection_Shared<Graphic3d_BndBox3d> HBndBox3d;

public:

  //! Creates an empty primitive set for BVH clipping.
  OpenGl_BVHClipPrimitiveTrsfPersSet (const Handle(Select3D_BVHBuilder3d)& theBuilder);

  //! Returns total number of structures.
  virtual Standard_Integer Size() const Standard_OVERRIDE;

  //! Returns AABB of the structure.
  virtual Graphic3d_BndBox3d Box (const Standard_Integer theIdx) const Standard_OVERRIDE;

  //! Calculates center of the AABB along given axis.
  virtual Standard_Real Center (const Standard_Integer theIdx,
                                const Standard_Integer theAxis) const Standard_OVERRIDE;

  //! Swaps structures with the given indices.
  virtual void Swap (const Standard_Integer theIdx1,
                     const Standard_Integer theIdx2) Standard_OVERRIDE;

  //! Adds structure to the set.
  //! @return true if structure added, otherwise returns false (structure already in the set).
  Standard_Boolean Add (const OpenGl_Structure* theStruct);

  //! Removes the given structure from the set.
  //! @return true if structure removed, otherwise returns false (structure is not in the set).
  Standard_Boolean Remove (const OpenGl_Structure* theStruct);

  //! Cleans the whole primitive set.
  void Clear();

  //! Returns the structure corresponding to the given ID.
  const OpenGl_Structure* GetStructureById (Standard_Integer theId);

  //! Marks object state as outdated (needs BVH rebuilding).
  void MarkDirty()
  {
    myIsDirty = Standard_True;
  }

  //! Returns BVH tree for the given world view projection (builds it if necessary).
  const opencascade::handle<BVH_Tree<Standard_Real, 3> >& BVH (const Handle(Graphic3d_Camera)& theCamera,
                                                               const OpenGl_Mat4d& theProjectionMatrix,
                                                               const OpenGl_Mat4d& theWorldViewMatrix,
                                                               const Standard_Integer theViewportWidth,
                                                               const Standard_Integer theViewportHeight,
                                                               const Graphic3d_WorldViewProjState& theWVPState);

  //! Returns builder for bottom-level BVH.
  const Handle(Select3D_BVHBuilder3d)& Builder() const { return myBuilder; }

  //! Assigns builder for bottom-level BVH.
  void SetBuilder (const Handle(Select3D_BVHBuilder3d)& theBuilder) { myBuilder = theBuilder; }

private:

  //! Marks internal object state as outdated.
  Standard_Boolean myIsDirty;

  //! Constructed bottom-level BVH.
  opencascade::handle<BVH_Tree<Standard_Real, 3> > myBVH;

  //! Builder for bottom-level BVH.
  Handle(Select3D_BVHBuilder3d) myBuilder;

  //! Indexed map of structures.
  NCollection_IndexedMap<const OpenGl_Structure*> myStructs;

  //! Cached set of bounding boxes precomputed for transformation persistent selectable objects.
  //! Cache exists only during computation of BVH Tree. Bounding boxes are world view projection
  //! dependent and should by synchronized.
  NCollection_IndexedMap<Handle(HBndBox3d)> myStructBoxes;

  //! State of world view projection used for generation of transformation persistence bounding boxes.
  Graphic3d_WorldViewProjState myStructBoxesState;
};

#endif // _OpenGl_BVHClipPrimitiveTrsfPersSet_HeaderFile
