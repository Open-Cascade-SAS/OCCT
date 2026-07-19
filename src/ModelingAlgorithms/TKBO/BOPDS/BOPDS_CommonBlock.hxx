// Created by: Peter KURNEV
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

#ifndef _BOPDS_CommonBlock_HeaderFile
#define _BOPDS_CommonBlock_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Standard_Handle.hxx>
#include <NCollection_BaseAllocator.hxx>
#include <Standard_Integer.hxx>
#include <Standard_Transient.hxx>
#include <NCollection_List.hxx>
class BOPDS_PaveBlock;

//! The class BOPDS_CommonBlock is to store the information
//! about pave blocks that have geometrical coincidence
//! (in terms of a tolerance) with:
//! a) other pave block(s);
//! b) face(s).
//! First pave block in the common block (real pave block)
//! is always a pave block with the minimal index of the original edge.
class BOPDS_CommonBlock : public Standard_Transient
{

public:
  //! Empty constructor
  Standard_EXPORT BOPDS_CommonBlock();

  //! Constructor
  //! @param theAllocator the allocator to manage the memory
  Standard_EXPORT BOPDS_CommonBlock(const occ::handle<NCollection_BaseAllocator>& theAllocator);

  //! Modifier
  //! Adds the pave block <aPB> to the list of pave blocks
  //! of the common block
  Standard_EXPORT void AddPaveBlock(const occ::handle<BOPDS_PaveBlock>& aPB);

  //! Modifier
  //! Sets the list of pave blocks for the common block
  Standard_EXPORT void SetPaveBlocks(const NCollection_List<occ::handle<BOPDS_PaveBlock>>& aLPB);

  //! Modifier
  //! Adds the index of the face <aF>
  //! to the list of indices of faces
  //! of the common block
  Standard_EXPORT void AddFace(const int aF);

  //! Modifier
  //! Sets the list of indices of faces <aLF>
  //! of the common block
  Standard_EXPORT void SetFaces(const NCollection_List<int>& aLF);

  //! Modifier
  //! Appends the list of indices of faces <aLF>
  //! to the list of indices of faces
  //! of the common block (the input list is emptied)
  Standard_EXPORT void AppendFaces(NCollection_List<int>& aLF);

  //! Selector
  //! Returns the list of pave blocks
  //! of the common block
  Standard_EXPORT const NCollection_List<occ::handle<BOPDS_PaveBlock>>& PaveBlocks() const;

  //! Selector
  //! Returns the list of indices of faces
  //! of the common block
  Standard_EXPORT const NCollection_List<int>& Faces() const;

  //! Selector
  //! Returns the first pave block
  //! of the common block
  Standard_EXPORT const occ::handle<BOPDS_PaveBlock>& PaveBlock1() const;

  //! Selector
  //! Returns the pave block that belongs
  //! to the edge with index <theIx>
  Standard_EXPORT occ::handle<BOPDS_PaveBlock>& PaveBlockOnEdge(const int theIndex);

  //! Query
  //! Returns true if the common block contains
  //! a pave block that belongs
  //! to the face with index <theIx>
  Standard_EXPORT bool IsPaveBlockOnFace(const int theIndex) const;

  //! Query
  //! Returns true if the common block contains
  //! a pave block that belongs
  //! to the edge with index <theIx>
  Standard_EXPORT bool IsPaveBlockOnEdge(const int theIndex) const;

  //! Query
  //! Returns true if the common block contains
  //! a pave block that is equal to <thePB>
  Standard_EXPORT bool Contains(const occ::handle<BOPDS_PaveBlock>& thePB) const;

  //! Query
  //! Returns true if the common block contains
  //! the face with index equal to <theF>
  Standard_EXPORT bool Contains(const int theF) const;

  //! Modifier
  //! Assign the index <theEdge> as the edge index
  //! to all pave blocks of the common block
  Standard_EXPORT void SetEdge(const int theEdge);

  //! Selector
  //! Returns the index of the edge
  //! of all pave blocks of the common block
  Standard_EXPORT int Edge() const;

  Standard_EXPORT void Dump() const;

  //! Moves the pave blocks in the list to make the given
  //! pave block to be the first.
  //! It will be representative for the whole group.
  Standard_EXPORT void SetRealPaveBlock(const occ::handle<BOPDS_PaveBlock>& thePB);

  //! Sets the tolerance for the common block
  void SetTolerance(const double theTol) { myTolerance = theTol; }

  //! Return the tolerance of common block
  double Tolerance() const { return myTolerance; }

  DEFINE_STANDARD_RTTIEXT(BOPDS_CommonBlock, Standard_Transient)

protected:
  NCollection_List<occ::handle<BOPDS_PaveBlock>> myPaveBlocks; //!< Pave blocks of the common block
  NCollection_List<int> myFaces;     //!< Faces on which the pave blocks are lying
  double                myTolerance; //!< Tolerance of the common block
};

#endif // _BOPDS_CommonBlock_HeaderFile
