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

#include <BOPCol_BaseAllocator.hxx>
#include <BOPDS_PDS.hxx>
#include <BOPDS_ListOfPassKeyBoolean.hxx>
#include <BOPDS_ListIteratorOfListOfPassKeyBoolean.hxx>
#include <BOPCol_PListOfInteger.hxx>
#include <BOPCol_ListOfInteger.hxx>
#include <Standard_Boolean.hxx>
#include <Standard_Integer.hxx>
class BOPDS_DS;



//! The class BOPDS_SubIterator is
//! 1.to compute intersections between two sub-sets of
//! BRep sub-shapes
//! of arguments of an operation (see the class BOPDS_DS)
//! in terms of theirs bounding boxes
//! 2.provides interface to iterare the pairs of
//! intersected sub-shapes of given type
class BOPDS_SubIterator 
{
public:

  DEFINE_STANDARD_ALLOC

  

  //! Empty contructor
  Standard_EXPORT BOPDS_SubIterator();
Standard_EXPORT virtual ~BOPDS_SubIterator();
  

  //! Contructor
  //! theAllocator - the allocator to manage the memory
  Standard_EXPORT BOPDS_SubIterator(const BOPCol_BaseAllocator& theAllocator);
  

  //! Modifier
  //! Sets the data structure <pDS> to process
  Standard_EXPORT void SetDS (const BOPDS_PDS& pDS);
  

  //! Selector
  //! Returns the data structure
  Standard_EXPORT const BOPDS_DS& DS() const;
  

  //! Modifier
  //! Sets the first set of indices  <theLI> to process
  Standard_EXPORT void SetSubSet1 (const BOPCol_ListOfInteger& theLI);
  

  //! Selector
  //! Returns the first set of indices to process
  Standard_EXPORT const BOPCol_ListOfInteger& SubSet1() const;
  

  //! Modifier
  //! Sets the second set of indices  <theLI> to process
  Standard_EXPORT void SetSubSet2 (const BOPCol_ListOfInteger& theLI);
  

  //! Selector
  //! Returns the second set of indices to process
  Standard_EXPORT const BOPCol_ListOfInteger& SubSet2() const;
  

  //! Initializes the  iterator
  Standard_EXPORT void Initialize();
  

  //! Returns  true if still there are pairs
  //! of intersected shapes
  Standard_EXPORT Standard_Boolean More() const;
  

  //! Moves iterations ahead
  Standard_EXPORT void Next();
  

  //! Returns indices (DS) of intersected shapes
  //! theIndex1 - the index of the first shape
  //! theIndex2 - the index of the second shape
  Standard_EXPORT void Value (Standard_Integer& theIndex1, Standard_Integer& theIndex2) const;
  

  //! Perform the intersection algorithm and prepare
  //! the results to be used
  Standard_EXPORT virtual void Prepare();




protected:

  
  Standard_EXPORT virtual void Intersect();


  BOPCol_BaseAllocator myAllocator;
  BOPDS_PDS myDS;
  BOPDS_ListOfPassKeyBoolean myList;
  BOPDS_ListIteratorOfListOfPassKeyBoolean myIterator;
  BOPCol_PListOfInteger mySubSet1;
  BOPCol_PListOfInteger mySubSet2;


private:





};







#endif // _BOPDS_SubIterator_HeaderFile
