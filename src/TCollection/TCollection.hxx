// Created on: 1992-10-13
// Created by: Ramin BARRETO
// Copyright (c) 1992-1999 Matra Datavision
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

#ifndef _TCollection_HeaderFile
#define _TCollection_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Integer.hxx>
class TCollection_AsciiString;
class TCollection_ExtendedString;
class TCollection_HAsciiString;
class TCollection_HExtendedString;
class TCollection_Array1;
class TCollection_HArray1;
class TCollection_Array2;
class TCollection_HArray2;
class TCollection_List;
class TCollection_ListNode;
class TCollection_ListIterator;
class TCollection_BaseSequence;
class TCollection_SeqNode;
class TCollection_Sequence;
class TCollection_SequenceNode;
class TCollection_HSequence;
class TCollection_MapHasher;
class TCollection_BasicMap;
class TCollection_MapNode;
class TCollection_BasicMapIterator;
class TCollection_Map;
class TCollection_MapIterator;
class TCollection_StdMapNode;
class TCollection_DataMap;
class TCollection_DataMapIterator;
class TCollection_DataMapNode;
class TCollection_DoubleMap;
class TCollection_DoubleMapIterator;
class TCollection_DoubleMapNode;
class TCollection_IndexedMap;
class TCollection_IndexedMapNode;
class TCollection_IndexedDataMap;
class TCollection_IndexedDataMapNode;


//! The package <TCollection> provides the services for the
//! transient basic data structures.
class TCollection 
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! Returns a  prime number greater than  <I> suitable
  //! to dimension a Map.  When  <I> becomes great there
  //! is  a  limit on  the  result (today  the  limit is
  //! around 1 000 000). This is not a limit of the number of
  //! items but a limit in the number  of buckets.  i.e.
  //! there will be more collisions  in  the map.
  Standard_EXPORT static Standard_Integer NextPrimeForMap (const Standard_Integer I);




protected:





private:




friend class TCollection_AsciiString;
friend class TCollection_ExtendedString;
friend class TCollection_HAsciiString;
friend class TCollection_HExtendedString;
friend class TCollection_Array1;
friend class TCollection_HArray1;
friend class TCollection_Array2;
friend class TCollection_HArray2;
friend class TCollection_List;
friend class TCollection_ListNode;
friend class TCollection_ListIterator;
friend class TCollection_BaseSequence;
friend class TCollection_SeqNode;
friend class TCollection_Sequence;
friend class TCollection_SequenceNode;
friend class TCollection_HSequence;
friend class TCollection_MapHasher;
friend class TCollection_BasicMap;
friend class TCollection_MapNode;
friend class TCollection_BasicMapIterator;
friend class TCollection_Map;
friend class TCollection_MapIterator;
friend class TCollection_StdMapNode;
friend class TCollection_DataMap;
friend class TCollection_DataMapIterator;
friend class TCollection_DataMapNode;
friend class TCollection_DoubleMap;
friend class TCollection_DoubleMapIterator;
friend class TCollection_DoubleMapNode;
friend class TCollection_IndexedMap;
friend class TCollection_IndexedMapNode;
friend class TCollection_IndexedDataMap;
friend class TCollection_IndexedDataMapNode;

};







#endif // _TCollection_HeaderFile
