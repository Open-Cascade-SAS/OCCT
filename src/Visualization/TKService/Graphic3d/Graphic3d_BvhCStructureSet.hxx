// Created on: 2013-12-25
// Created by: Varvara POSKONINA
// Copyright (c) 1999-2014 OPEN CASCADE SAS
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

#ifndef _Graphic3d_BvhCStructureSet_HeaderFile
#define _Graphic3d_BvhCStructureSet_HeaderFile

#include <BVH_PrimitiveSet3d.hxx>
#include <Graphic3d_BndBox3d.hxx>
#include <NCollection_IndexedMap.hxx>

class Graphic3d_CStructure;

//! Set of OpenGl_Structures for building BVH tree.
class Graphic3d_BvhCStructureSet : public BVH_PrimitiveSet3d
{
  DEFINE_STANDARD_RTTIEXT(Graphic3d_BvhCStructureSet, BVH_PrimitiveSet3d)
protected:
  using BVH_PrimitiveSet3d::Box;

public:
  //! Creates an empty primitive set for BVH clipping.
  Standard_EXPORT Graphic3d_BvhCStructureSet();

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

private:
  // clang-format off
  NCollection_IndexedMap<const Graphic3d_CStructure*> myStructs;    //!< Indexed map of structures.
  // clang-format on
};

#endif // _Graphic3d_BvhCStructureSet_HeaderFile
