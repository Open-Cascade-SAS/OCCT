// Created on: 1993-02-26
// Created by: Remi LEQUETTE
// Copyright (c) 1993-1999 Matra Datavision
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

#ifndef _TCollection_BasicMapIterator_HeaderFile
#define _TCollection_BasicMapIterator_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Integer.hxx>
#include <Standard_Address.hxx>
#include <Standard_Boolean.hxx>
class TCollection_BasicMap;


//! This  class  provides    basic   services  for the
//! iterators  on Maps. The  iterators  are  inherited
//! from this one.
//!
//! The  iterator   contains  an   array   of pointers
//! (buckets). Each bucket is a  pointer  on a node. A
//! node contains a pointer on the next node.
//!
//! This class  provides also basic  services for  the
//! implementation of Maps.
//! A map iterator provides a step by step exploration of all
//! entries of a map. After initialization of a concrete derived
//! iterator, use in a loop:
//! -   the function More to know if there is a current entry for
//! the iterator in the map,
//! -   then the functions which read data on an entry of the
//! map (these functions are provided by each type of map),
//! -   then the function Next to set the iterator to the next   entry of the map.
//! Warning
//! -   A map is a non-ordered data structure. The order in
//! which entries of a map are explored by the iterator
//! depends on its contents, and change when the map is edited.
//! -   It is not recommended to modify the contents of a map
//! during iteration: the result is unpredictable.
class TCollection_BasicMapIterator 
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! Resets the iterator to the first node.
  Standard_EXPORT void Reset();
  
  //! Returns true if there is a current entry for this iterator in the map.
  //! Use the function Next to set this iterator to the position of
  //! the next entry, if it exists.
    Standard_Boolean More() const;
  
  //! Sets this iterator to the position of the next entry of the map.
  //! Nothing is changed if there is no more entry to explore in
  //! the map: this iterator is always positioned on the last entry
  //! of the map but the function More returns false.
  Standard_EXPORT void Next();




protected:

  
  //! Creates an empty iterator.
  Standard_EXPORT TCollection_BasicMapIterator();
  
  //! Initialize on the first node in the buckets.
  Standard_EXPORT TCollection_BasicMapIterator(const TCollection_BasicMap& M);
  
  //! Initialize on the first node in the buckets.
  Standard_EXPORT void Initialize (const TCollection_BasicMap& M);


  Standard_Address myNode;


private:



  Standard_Integer myNbBuckets;
  Standard_Address myBuckets;
  Standard_Integer myBucket;


};


#include <TCollection_BasicMapIterator.lxx>





#endif // _TCollection_BasicMapIterator_HeaderFile
