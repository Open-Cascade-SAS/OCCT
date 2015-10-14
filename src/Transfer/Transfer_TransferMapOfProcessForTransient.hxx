// Created on: 1992-02-03
// Created by: Christian CAILLET
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

#ifndef _Transfer_TransferMapOfProcessForTransient_HeaderFile
#define _Transfer_TransferMapOfProcessForTransient_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <TCollection_BasicMap.hxx>
#include <TColStd_MapTransientHasher.hxx>
#include <Standard_Integer.hxx>
#include <Standard_Boolean.hxx>
#include <Standard_Address.hxx>
class Standard_DomainError;
class Standard_OutOfRange;
class Standard_NoSuchObject;
class Standard_Transient;
class Transfer_Binder;
class Transfer_IndexedDataMapNodeOfTransferMapOfProcessForTransient;



class Transfer_TransferMapOfProcessForTransient  : public TCollection_BasicMap
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! Constructs an IndexedDataMap with NbBuckets
  //! (defaulted to 1) buckets.
  //! Note that the map will be automatically redimensioned
  //! during its use if the number of entries becomes too large.
  //! Use:
  //! -   the function Add to add an entry (key, item, index) in the map,
  //! -   operator() to read an item from an index, or to
  //! assign a new value to this item,
  //! -   the function FindFromKey or ChangeFromKey to
  //! read an item from a key, or to assign a new value to this item,
  //! -   the function RemoveLast to remove the last entry from the map,
  //! -   and other available edition and querying functions.
  Standard_EXPORT Transfer_TransferMapOfProcessForTransient(const Standard_Integer NbBuckets = 1);
  
  //! Replace the content of this map by  the content of
  //! the map <Other>.
  Standard_EXPORT Transfer_TransferMapOfProcessForTransient& Assign (const Transfer_TransferMapOfProcessForTransient& Other);
Transfer_TransferMapOfProcessForTransient& operator = (const Transfer_TransferMapOfProcessForTransient& Other)
{
  return Assign(Other);
}
  
  //! Changes the  number    of  buckets of  <me>  to be
  //! <NbBuckets>. The entries (key + item + index) already
  //! stored in this map are maintained.
  Standard_EXPORT void ReSize (const Standard_Integer NbBuckets);
  
  //! Removes all keys in the map.
  Standard_EXPORT void Clear();
~Transfer_TransferMapOfProcessForTransient()
{
  Clear();
}
  
  //! Adds  the Key  <K> to the  Map <me>.   Returns the
  //! index of  the  Key.  The key is new  in the map if
  //! Extent  has  been  incremented.  The Item  <I>  is
  //! stored with the key. If the key was already in the
  //! map the previous item is not replaced by <I>.
  Standard_EXPORT Standard_Integer Add (const Handle(Standard_Transient)& K, const Handle(Transfer_Binder)& I);
  
  //! Substitutes the Key at  index <I>  with  <K>.  <I>
  //! must be a valid index, <K> must  be a new key. <T>
  //! becomes the Item stored with <K>.
  //! Trigger: Raises OutOfRange if I < 1 or I > Extent.
  //! Raises DomainError if Contains(K).
  Standard_EXPORT void Substitute (const Standard_Integer I, const Handle(Standard_Transient)& K, const Handle(Transfer_Binder)& T);
  
  //! Removes  the last key entered  in the map, i.e the
  //! key of index Extent().
  //! I must be a valid index and K must be a new key.
  //! Exceptions
  //! -   Standard_OutOfRange if the index I is less than 1 or
  //! greater than the number of entries in this map.
  //! -   Standard_DomainError if the key K is already in this map.
  Standard_EXPORT void RemoveLast();
  
  //! Returns True if the key <K>  is stored  in the map <me>.
  Standard_EXPORT Standard_Boolean Contains (const Handle(Standard_Transient)& K) const;
  
  //! Returns the Key of index <I>.
  //! Exceptions
  //! Standard_OutOfRange if I is less than 1 or greater than
  //! the number of entries in this map.
  Standard_EXPORT const Handle(Standard_Transient)& FindKey (const Standard_Integer I) const;
  
  //! Returns the Item of index <I>.
  Standard_EXPORT const Handle(Transfer_Binder)& FindFromIndex (const Standard_Integer I) const;
const Handle(Transfer_Binder)& operator () (const Standard_Integer I) const
{
  return FindFromIndex(I);
}
  
  //! Returns  the  Item of index <I>.  The Item  can be
  //! modified with the syntax aMap(Index) = newItem.
  Standard_EXPORT Handle(Transfer_Binder)& ChangeFromIndex (const Standard_Integer I);
Handle(Transfer_Binder)& operator () (const Standard_Integer I)
{
  return ChangeFromIndex(I);
}
  
  //! Returns the index of the key <K>.
  //! Returns 0 if K is not in the map.
  Standard_EXPORT Standard_Integer FindIndex (const Handle(Standard_Transient)& K) const;
  
  //! Returns the Item of the key <K>
  //! Trigger: Raises NoSuchObject if K is not in the map.
  Standard_EXPORT const Handle(Transfer_Binder)& FindFromKey (const Handle(Standard_Transient)& K) const;
  
  //! Returns the Item of the key <K>
  //! Trigger: Raises NoSuchObject if K is not in the map.
  Standard_EXPORT Handle(Transfer_Binder)& ChangeFromKey (const Handle(Standard_Transient)& K);
  
  //! Returns the address of Item of the key <K>
  //! or NULL if K is not in the map.
  Standard_EXPORT Standard_Address FindFromKey1 (const Handle(Standard_Transient)& K) const;
  
  //! Returns the address of Item of the key <K>
  //! or NULL if K is not in the map.
  Standard_EXPORT Standard_Address ChangeFromKey1 (const Handle(Standard_Transient)& K);




protected:





private:

  
  //! As  copying  Map  is an expensive  operation it is
  //! incorrect  to  do  it implicitly. This constructor
  //! will raise an  error if the Map  is not  empty. To copy the
  //! content of a  Map use the  Assign  method (operator =).
  Standard_EXPORT Transfer_TransferMapOfProcessForTransient(const Transfer_TransferMapOfProcessForTransient& Other);




};







#endif // _Transfer_TransferMapOfProcessForTransient_HeaderFile
