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

#include <BOPDS_IndexedMapOfPaveBlock.hxx>
#include <NCollection_BaseAllocator.hxx>
#include <Standard_Integer.hxx>
#include <TColStd_MapOfInteger.hxx>

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
  BOPDS_FaceInfo(const Handle(NCollection_BaseAllocator)& theAllocator);

  //! Clears the contents
  void Clear();

  //! Modifier
  //! Sets the index of the face <theI>
  void SetIndex(const Standard_Integer theI);

  //! Selector
  //! Returns the index of the face
  //!
  //! In
  Standard_Integer Index() const;

  //! Selector
  //! Returns the pave blocks of the face
  //! that  have state In
  const BOPDS_IndexedMapOfPaveBlock& PaveBlocksIn() const;

  //! Selector/Modifier
  //! Returns the pave blocks
  //! of the face
  //! that  have state In
  BOPDS_IndexedMapOfPaveBlock& ChangePaveBlocksIn();

  //! Selector
  //! Returns the list of indices for vertices
  //! of the face
  //! that have state In
  const TColStd_MapOfInteger& VerticesIn() const;

  //! Selector/Modifier
  //! Returns the list of indices for vertices
  //! of the face
  //! that have state In
  //!
  //! On
  TColStd_MapOfInteger& ChangeVerticesIn();

  //! Selector
  //! Returns the pave blocks of the face
  //! that  have state On
  const BOPDS_IndexedMapOfPaveBlock& PaveBlocksOn() const;

  //! Selector/Modifier
  //! Returns the pave blocks
  //! of the face
  //! that  have state On
  BOPDS_IndexedMapOfPaveBlock& ChangePaveBlocksOn();

  //! Selector
  //! Returns the list of indices for vertices
  //! of the face
  //! that have state On
  const TColStd_MapOfInteger& VerticesOn() const;

  //! Selector/Modifier
  //! Returns the list of indices for vertices
  //! of the face
  //! that have state On
  //!
  //! Sections
  TColStd_MapOfInteger& ChangeVerticesOn();

  //! Selector
  //! Returns the pave blocks of the face
  //! that are  pave blocks of section edges
  const BOPDS_IndexedMapOfPaveBlock& PaveBlocksSc() const;

  BOPDS_IndexedMapOfPaveBlock& ChangePaveBlocksSc();

  //! Selector
  //! Returns the list of indices for section  vertices
  //! of the face
  const TColStd_MapOfInteger& VerticesSc() const;

  //! Selector/Modifier
  //! Returns the list of indices for section  vertices
  //! of the face
  //!
  //! Others
  TColStd_MapOfInteger& ChangeVerticesSc();

protected:
  Handle(NCollection_BaseAllocator) myAllocator;
  Standard_Integer                  myIndex;
  BOPDS_IndexedMapOfPaveBlock       myPaveBlocksIn;
  TColStd_MapOfInteger              myVerticesIn;
  BOPDS_IndexedMapOfPaveBlock       myPaveBlocksOn;
  TColStd_MapOfInteger              myVerticesOn;
  BOPDS_IndexedMapOfPaveBlock       myPaveBlocksSc;
  TColStd_MapOfInteger              myVerticesSc;

private:
};

#include <BOPDS_FaceInfo.lxx>

#endif // _BOPDS_FaceInfo_HeaderFile
