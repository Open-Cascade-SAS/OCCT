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

#include <BVH_PrimitiveSet.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_IndexedMap.hxx>

#include <OpenGl_Structure.hxx>

//! Defines index map of OpenGL structures.
typedef NCollection_IndexedMap<const OpenGl_Structure*> OpenGl_IndexedMapOfStructure;

//! Defines array of indexed maps of OpenGL structures.
typedef NCollection_Array1<OpenGl_IndexedMapOfStructure> OpenGl_ArrayOfIndexedMapOfStructure;

//! Set of OpenGl_Structures for building BVH tree.
class OpenGl_BVHClipPrimitiveSet : public BVH_PrimitiveSet<Standard_ShortReal, 4>
{
protected:

  using BVH_PrimitiveSet<Standard_ShortReal, 4>::Box;

public:

  //! Creates an empty primitive set for BVH clipping.
  OpenGl_BVHClipPrimitiveSet();

  //! Returns total number of structures.
  virtual Standard_Integer Size() const;

  //! Returns AABB of the structure.
  virtual Graphic3d_BndBox4f Box (const Standard_Integer theIdx) const;

  //! Calculates center of the AABB along given axis.
  virtual Standard_ShortReal Center (const Standard_Integer theIdx,
                                     const Standard_Integer theAxis) const;

  //! Swaps structures with the given indices.
  virtual void Swap (const Standard_Integer theIdx1,
                     const Standard_Integer theIdx2);

  //! Replaces the set by the given array taking into account
  //! if each structure is cullable or not.
  void Assign (const OpenGl_ArrayOfIndexedMapOfStructure& theStructs);

  //! Adds structure to the set.
  void Add (const OpenGl_Structure* theStruct);

  //! Removes the given structure from the set.
  void Remove (const OpenGl_Structure* theStruct);

  //! Cleans the whole primitive set.
  void Clear();

  //! Returns the structure corresponding to the given ID.
  const OpenGl_Structure* GetStructureById (Standard_Integer theId);

private:

  NCollection_IndexedMap<const OpenGl_Structure*> myStructs;    //!< Indexed map of structures.

};

#endif // _OpenGl_BVHClipPrimitiveSet_HeaderFile
