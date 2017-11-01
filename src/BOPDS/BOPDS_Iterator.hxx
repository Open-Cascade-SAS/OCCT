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

#ifndef _BOPDS_Iterator_HeaderFile
#define _BOPDS_Iterator_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Integer.hxx>
#include <BOPDS_PDS.hxx>
#include <BOPDS_VectorOfPair.hxx>
#include <BOPDS_VectorOfVectorOfPair.hxx>
#include <NCollection_BaseAllocator.hxx>
#include <Precision.hxx>
#include <Standard_Boolean.hxx>
#include <TopAbs_ShapeEnum.hxx>
class BOPDS_DS;
class IntTools_Context;


//! The class BOPDS_Iterator is
//! 1.to compute intersections between BRep sub-shapes
//! of arguments of an operation (see the class BOPDS_DS)
//! in terms of theirs bounding boxes
//! 2.provides interface to iterare the pairs of
//! intersected sub-shapes of given type
class BOPDS_Iterator 
{
public:

  DEFINE_STANDARD_ALLOC

  //! Empty contructor
  Standard_EXPORT BOPDS_Iterator();
  Standard_EXPORT virtual ~BOPDS_Iterator();

  //! Contructor
  //! theAllocator - the allocator to manage the memory
  Standard_EXPORT BOPDS_Iterator(const Handle(NCollection_BaseAllocator)& theAllocator);

  //! Modifier
  //! Sets the data structure <pDS> to process
  Standard_EXPORT void SetDS (const BOPDS_PDS& pDS);

  //! Selector
  //! Returns the data structure
  Standard_EXPORT const BOPDS_DS& DS() const;

  //! Initializes the  iterator
  //! theType1 - the first type of shape
  //! theType2 - the second type of shape
  Standard_EXPORT void Initialize (const TopAbs_ShapeEnum theType1, const TopAbs_ShapeEnum theType2);

  //! Returns  true if still there are pairs
  //! of intersected shapes
  Standard_EXPORT Standard_Boolean More() const;

  //! Moves iterations ahead
  Standard_EXPORT void Next();

  //! Returns indices (DS) of intersected shapes
  //! theIndex1 - the index of the first shape
  //! theIndex2 - the index of the second shape
  Standard_EXPORT void Value (Standard_Integer& theIndex1,
                              Standard_Integer& theIndex2) const;

  //! Perform the intersection algorithm and prepare
  //! the results to be used
  Standard_EXPORT virtual void Prepare(const Handle(IntTools_Context)& theCtx = Handle(IntTools_Context)(),
                                       const Standard_Boolean theCheckOBB = Standard_False,
                                       const Standard_Real theFuzzyValue = Precision::Confusion());

  //! Returns the number of intersections founded
  Standard_EXPORT Standard_Integer ExpectedLength() const;

  //! Returns the block length
  Standard_EXPORT Standard_Integer BlockLength() const;

  //! Set the flag of parallel processing
  //! if <theFlag> is true  the parallel processing is switched on
  //! if <theFlag> is false the parallel processing is switched off
  Standard_EXPORT void SetRunParallel (const Standard_Boolean theFlag);

  //! Returns the flag of parallel processing
  Standard_EXPORT Standard_Boolean RunParallel() const;

protected:

  Standard_EXPORT virtual void Intersect(const Handle(IntTools_Context)& theCtx = Handle(IntTools_Context)(),
                                         const Standard_Boolean theCheckOBB = Standard_False,
                                         const Standard_Real theFuzzyValue = Precision::Confusion());

  Handle(NCollection_BaseAllocator) myAllocator;
  Standard_Integer myLength;
  BOPDS_PDS myDS;
  BOPDS_VectorOfVectorOfPair myLists;
  BOPDS_VectorOfPair::Iterator myIterator;
  Standard_Boolean myRunParallel;

private:

};

#endif // _BOPDS_Iterator_HeaderFile
