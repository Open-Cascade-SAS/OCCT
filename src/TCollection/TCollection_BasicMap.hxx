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

#ifndef _TCollection_BasicMap_HeaderFile
#define _TCollection_BasicMap_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Boolean.hxx>
#include <Standard_Integer.hxx>
#include <Standard_Address.hxx>
#include <Standard_OStream.hxx>
class TCollection_BasicMapIterator;


//! Root  class of  all the maps,  provides utilitites
//! for managing the buckets.
//! Maps are dynamically extended data structures where
//! data is quickly accessed with a key.
//! General properties of maps
//! -   Map items may be (complex) non-unitary data; they
//! may be difficult to manage with an array. Moreover, the
//! map allows a data structure to be indexed by complex   data.
//! -   The size of a map is dynamically extended. So a map
//! may be first dimensioned for a little number of items.
//! Maps avoid the use of large and quasi-empty arrays.
//! -   The access to a map item is much faster than the one
//! to a sequence, a list, a queue or a stack item.
//! -   The access time to a map item may be compared with
//! the one to an array item. First of all, it depends on the
//! size of the map. It also depends on the quality of a user
//! redefinable function (the hashing function) to find
//! quickly where the item is.
//! -   The exploration of a map may be of better performance
//! than the exploration of an array because the size of the
//! map is adapted to the number of inserted items.
//! These properties explain why maps are commonly used as
//! internal data structures for algorithms.
//! Definitions
//! -   A map is a data structure for which data is addressed   by keys.
//! -   Once inserted in the map, a map item is referenced as   an entry of the map.
//! -   Each entry of the map is addressed by a key. Two
//! different keys address two different entries of the map.
//! -   The position of an entry in the map is called a bucket.
//! -   A map is dimensioned by its number of buckets, i.e. the
//! maximum number of entries in the map. The
//! performance of a map is conditioned by the number of buckets.
//! -   The hashing function transforms a key into a bucket
//! index. The number of values that can be computed by
//! the hashing function is equal to the number of buckets of the map.
//! -   Both the hashing function and the equality test
//! between two keys are provided by a hasher object.
//! -   A map may be explored by a map iterator. This
//! exploration provides only inserted entries in the map
//! (i.e. non empty buckets).
//! Collections' generic maps
//! The Collections component provides numerous generic derived maps.
//! -   These maps include automatic management of the
//! number of buckets: they are automatically resized when
//! the number of keys exceeds the number of buckets. If
//! you have a fair idea of the number of items in your map,
//! you can save on automatic resizing by specifying a
//! number of buckets at the time of construction, or by using
//! a resizing function. This may be considered for crucial optimization issues.
//! -   Keys, items and hashers are parameters of these generic derived maps.
//! -   TCollection_MapHasher class describes the
//! functions required by any hasher which is to be used
//! with a map instantiated from the Collections component.
//! -   An iterator class is automatically instantiated at the
//! time of instantiation of a map provided by the
//! Collections component if this map is to be explored
//! with an iterator. Note that some provided generic maps
//! are not to be explored with an iterator but with indexes   (indexed maps).
class TCollection_BasicMap 
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! Returns the number of buckets in <me>.
    Standard_Integer NbBuckets() const;
  
  //! Returns the number of keys already stored in <me>.
    Standard_Integer Extent() const;
  
  //! Returns  True when the map  contains no keys.
  //! This is exactly Extent() == 0.
    Standard_Boolean IsEmpty() const;
  
  //! Prints  on <S> usefull  statistics  about  the map
  //! <me>.  It  can be used  to test the quality of the hashcoding.
  Standard_EXPORT void Statistics (Standard_OStream& S) const;


friend class TCollection_BasicMapIterator;


protected:

  
  //! Initialize the map.  Single is  True when the  map
  //! uses only one table of buckets.
  //!
  //! One table  : Map, DataMap
  //! Two tables : DoubleMap, IndexedMap, IndexedDataMap
  Standard_EXPORT TCollection_BasicMap(const Standard_Integer NbBuckets, const Standard_Boolean single);
  
  //! Tries to resize  the Map with  NbBuckets.  Returns
  //! True if  possible, NewBuckts is  the  new nuber of
  //! buckets.   data1 and data2  are the new tables  of
  //! buckets where the data must be copied.
  Standard_EXPORT Standard_Boolean BeginResize (const Standard_Integer NbBuckets, Standard_Integer& NewBuckets, Standard_Address& data1, Standard_Address& data2) const;
  
  //! If  BeginResize was  succesfull  after copying the
  //! data to  data1  and data2 this methods  update the
  //! tables and destroys the old ones.
  Standard_EXPORT void EndResize (const Standard_Integer NbBuckets, const Standard_Integer NewBuckets, const Standard_Address data1, const Standard_Address data2);
  
  //! Returns   True  if resizing   the   map should  be
  //! considered.
    Standard_Boolean Resizable() const;
  
  //! Decrement the  extent of the  map.
    void Increment();
  
  //! Decrement the  extent of the  map.
    void Decrement();
  
  //! Destroys the buckets.
  Standard_EXPORT void Destroy();


  Standard_Address myData1;
  Standard_Address myData2;


private:



  Standard_Boolean isDouble;
  Standard_Boolean mySaturated;
  Standard_Integer myNbBuckets;
  Standard_Integer mySize;


};


#include <TCollection_BasicMap.lxx>





#endif // _TCollection_BasicMap_HeaderFile
