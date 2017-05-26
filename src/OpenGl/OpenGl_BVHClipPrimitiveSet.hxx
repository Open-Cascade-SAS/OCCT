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

#ifndef _OpenGl_BVHClipPrimitiveSet_HeaderFile
#define _OpenGl_BVHClipPrimitiveSet_HeaderFile

#include <BVH_PrimitiveSet3d.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_IndexedMap.hxx>

#include <OpenGl_Structure.hxx>

//! Set of OpenGl_Structures for building BVH tree.
class OpenGl_BVHClipPrimitiveSet : public BVH_PrimitiveSet3d
{
  DEFINE_STANDARD_RTTIEXT(OpenGl_BVHClipPrimitiveSet, BVH_PrimitiveSet3d)
protected:

  using BVH_PrimitiveSet3d::Box;

public:

  //! Creates an empty primitive set for BVH clipping.
  OpenGl_BVHClipPrimitiveSet();

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

private:

  NCollection_IndexedMap<const OpenGl_Structure*> myStructs;    //!< Indexed map of structures.

};

#endif // _OpenGl_BVHClipPrimitiveSet_HeaderFile
