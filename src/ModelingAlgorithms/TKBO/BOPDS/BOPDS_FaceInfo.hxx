// Created by: Peter KURNEV
// Copyright (c) 2010-2014 OPEN CASCADE SAS
// Copyright (c) 2007-2010 CEA/DEN, EDF R&D, OPEN CASCADE
// Copyright (c) 2003-2007 OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN, CEDRAT,
//                         EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS
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

#ifndef _BOPDS_FaceInfo_HeaderFile
#define _BOPDS_FaceInfo_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <NCollection_IndexedMap.hxx>
#include <BOPDS_PaveBlock.hxx>
#include <NCollection_BaseAllocator.hxx>
#include <Standard_Integer.hxx>
#include <Standard_Integer.hxx>
#include <NCollection_Map.hxx>

//! The class BOPDS_FaceInfo is to store
//! handy information about state of face
class BOPDS_FaceInfo
{
public:
  DEFINE_STANDARD_ALLOC

  //! Empty constructor
  BOPDS_FaceInfo();

  virtual ~BOPDS_FaceInfo();

  //! Constructor
  //! @param theAllocator the allocator to manage the memory
  BOPDS_FaceInfo(const occ::handle<NCollection_BaseAllocator>& theAllocator);

  //! Clears the contents
  void Clear();

  //! Modifier
  //! Sets the index of the face <theI>
  void SetIndex(const int theI);

  //! Selector
  //! Returns the index of the face
  //!
  //! In
  int Index() const;

  //! Selector
  //! Returns the pave blocks of the face
  //! that have state In
  const NCollection_IndexedMap<occ::handle<BOPDS_PaveBlock>>& PaveBlocksIn() const;

  //! Selector/Modifier
  //! Returns the pave blocks
  //! of the face
  //! that have state In
  NCollection_IndexedMap<occ::handle<BOPDS_PaveBlock>>& ChangePaveBlocksIn();

  //! Selector
  //! Returns the list of indices for vertices
  //! of the face
  //! that have state In
  const NCollection_Map<int>& VerticesIn() const;

  //! Selector/Modifier
  //! Returns the list of indices for vertices
  //! of the face
  //! that have state In
  //!
  //! On
  NCollection_Map<int>& ChangeVerticesIn();

  //! Selector
  //! Returns the pave blocks of the face
  //! that have state On
  const NCollection_IndexedMap<occ::handle<BOPDS_PaveBlock>>& PaveBlocksOn() const;

  //! Selector/Modifier
  //! Returns the pave blocks
  //! of the face
  //! that have state On
  NCollection_IndexedMap<occ::handle<BOPDS_PaveBlock>>& ChangePaveBlocksOn();

  //! Selector
  //! Returns the list of indices for vertices
  //! of the face
  //! that have state On
  const NCollection_Map<int>& VerticesOn() const;

  //! Selector/Modifier
  //! Returns the list of indices for vertices
  //! of the face
  //! that have state On
  //!
  //! Sections
  NCollection_Map<int>& ChangeVerticesOn();

  //! Selector
  //! Returns the pave blocks of the face
  //! that are pave blocks of section edges
  const NCollection_IndexedMap<occ::handle<BOPDS_PaveBlock>>& PaveBlocksSc() const;

  NCollection_IndexedMap<occ::handle<BOPDS_PaveBlock>>& ChangePaveBlocksSc();

  //! Selector
  //! Returns the list of indices for section vertices
  //! of the face
  const NCollection_Map<int>& VerticesSc() const;

  //! Selector/Modifier
  //! Returns the list of indices for section vertices
  //! of the face
  //!
  //! Others
  NCollection_Map<int>& ChangeVerticesSc();

protected:
  occ::handle<NCollection_BaseAllocator> myAllocator;
  int                  myIndex;
  NCollection_IndexedMap<occ::handle<BOPDS_PaveBlock>>       myPaveBlocksIn;
  NCollection_Map<int>              myVerticesIn;
  NCollection_IndexedMap<occ::handle<BOPDS_PaveBlock>>       myPaveBlocksOn;
  NCollection_Map<int>              myVerticesOn;
  NCollection_IndexedMap<occ::handle<BOPDS_PaveBlock>>       myPaveBlocksSc;
  NCollection_Map<int>              myVerticesSc;

};

#include <BOPDS_FaceInfo.lxx>

#endif // _BOPDS_FaceInfo_HeaderFile
