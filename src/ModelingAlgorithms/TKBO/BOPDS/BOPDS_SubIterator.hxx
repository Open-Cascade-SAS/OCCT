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

#ifndef _BOPDS_SubIterator_HeaderFile
#define _BOPDS_SubIterator_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <BOPDS_PDS.hxx>
#include <NCollection_Vector.hxx>
#include <BOPDS_Pair.hxx>
#include <NCollection_BaseAllocator.hxx>
#include <Standard_Integer.hxx>
#include <NCollection_List.hxx>
#include <Standard_Integer.hxx>

//! The class BOPDS_SubIterator is used to compute intersections between
//! bounding boxes of two sub-sets of BRep sub-shapes of arguments
//! of an operation (see the class BOPDS_DS).
//! The class provides interface to iterate the pairs of intersected sub-shapes.

class BOPDS_SubIterator
{
public:
  DEFINE_STANDARD_ALLOC

  //! Empty constructor
  Standard_EXPORT BOPDS_SubIterator();
  Standard_EXPORT virtual ~BOPDS_SubIterator();

  //! Constructor
  //! theAllocator - the allocator to manage the memory
  Standard_EXPORT BOPDS_SubIterator(const occ::handle<NCollection_BaseAllocator>& theAllocator);

  //! Sets the data structure <pDS> to process.
  //! It is used to access the shapes and their bounding boxes.
  void SetDS(const BOPDS_PDS& pDS) { myDS = pDS; }

  //! Returns the data structure
  const BOPDS_DS& DS() const { return *myDS; }

  //! Sets the first set of indices <theLI> to process
  void SetSubSet1(const NCollection_List<int>& theLI)
  {
    mySubSet1 = (NCollection_List<int>*)&theLI;
  }

  //! Returns the first set of indices to process
  const NCollection_List<int>& SubSet1() const { return *mySubSet1; }

  //! Sets the second set of indices <theLI> to process
  void SetSubSet2(const NCollection_List<int>& theLI)
  {
    mySubSet2 = (NCollection_List<int>*)&theLI;
  }

  //! Returns the second set of indices to process
  const NCollection_List<int>& SubSet2() const { return *mySubSet2; }

  //! Initializes the iterator
  Standard_EXPORT void Initialize();

  //! Returns true if there are more pairs of intersected shapes
  bool More() const { return myIterator.More(); }

  //! Moves iterations ahead
  void Next() { myIterator.Next(); }

  //! Returns indices (DS) of intersected shapes
  //! theIndex1 - the index of the first shape
  //! theIndex2 - the index of the second shape
  Standard_EXPORT void Value(int& theIndex1, int& theIndex2) const;

  //! Perform the intersection algorithm and prepare
  //! the results to be used
  Standard_EXPORT virtual void Prepare();

  //! Returns the number of interfering pairs
  int ExpectedLength() const { return myList.Length(); }

protected:
  //! Performs intersection of bounding boxes
  Standard_EXPORT virtual void Intersect();

  occ::handle<NCollection_BaseAllocator> myAllocator;
  BOPDS_PDS                         myDS;
  NCollection_Vector<BOPDS_Pair>                myList;
  NCollection_Vector<BOPDS_Pair>::Iterator      myIterator;
  NCollection_List<int>*            mySubSet1;
  NCollection_List<int>*            mySubSet2;

};

#endif // _BOPDS_SubIterator_HeaderFile
