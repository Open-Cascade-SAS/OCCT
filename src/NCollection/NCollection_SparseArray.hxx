// Created on: 2006-11-23
// Created by: Andrey BETENEV
// Copyright (c) 2006-2014 OPEN CASCADE SAS
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

#ifndef NCollection_SparseArray_HeaderFile
#define NCollection_SparseArray_HeaderFile

#include <NCollection_SparseArrayBase.hxx>

/**
* Dynamically resizable sparse array of objects
*
* This class is similar to NCollection_Vector: it works like virtually
* unlimited array of items accessible by index; however unlike simple
* Vector it distinguishes items that have been set from the ones that
* have not been set explicitly.
*
* This class can be also seen as equivalence of
* NCollection_DataMap<Standard_Integer,TheItemType>
* with the only one practical difference: it can be much less 
* memory-expensive if items are small (e.g. Integer or Handle).
* 
* The index starts from 0, i.e. should be non-negative. Memory is allocated
* when item is set by SetValue(). 
*
* Iterator returns only defined items; 
* the item can be tested for being defined by IsSet(), 
* and undefined by UnsetValue().
*
* The attempt to access the item that has not been set will result
* in OutOfRange exception in Debug mode; in Release mode this will either
* return null-filled object or cause access violation.
*/

template <class TheItemType> class NCollection_SparseArray 
                                 : public NCollection_SparseArrayBase
{
public:

  //! Constructor; accepts size of blocks 
  explicit NCollection_SparseArray (Standard_Size theIncrement)
    : NCollection_SparseArrayBase(sizeof(TheItemType),theIncrement)
  { 
  }

  //! Explicit assignment operator
  NCollection_SparseArray& Assign (const NCollection_SparseArray& theOther) 
  {
    this->assign (theOther);
    return *this;
  }

  //! Exchange the data of two arrays;
  //! can be used primarily to move contents of theOther into the new array
  //! in a fast way (without creation of duplicated data)
  void Exchange (NCollection_SparseArray& theOther) 
  {
    this->exchange (theOther);
  }

  //! Destructor
  virtual ~NCollection_SparseArray ()
  {
    Clear();
  }

public:
  //!@name Array-like interface (in addition to inherited methods)
  //!@{

  //! Direct const access to the item 
  const TheItemType& Value (const Standard_Size theIndex) const 
  {
    return *(const TheItemType*)this->getValue(theIndex);
  }

  //! Const access to the item - operator()
  const TheItemType& operator () (const Standard_Size theIndex) const
  { 
    return Value (theIndex); 
  }

  //! Modification access to the item
  TheItemType& ChangeValue (const Standard_Size theIndex) 
  {
    return *(TheItemType*)(this->getValue (theIndex));
  }

  //! Access to the item - operator()
  TheItemType& operator () (const Standard_Size theIndex)
  { 
    return ChangeValue (theIndex); 
  }
  
  //! Set a value at specified index method
  TheItemType& SetValue (const Standard_Size theIndex,
                         const TheItemType&     theValue) 
  {
    return *(TheItemType*)this->setValue(theIndex, (Standard_Address)&theValue);
  }

  //!@}
  
public:
  //!@name DataMap-like interface
  //!@{

  //! Returns number of items in the array
  Standard_Size Extent () const 
  {
    return Size();
  }

  //! Returns True if array is empty
  Standard_Boolean IsEmpty () const 
  {
    return Size() == 0;
  }

  //! Direct const access to the item 
  const TheItemType& Find (const Standard_Size theIndex) const 
  {
    return Value(theIndex);
  }

  //! Modification access to the item
  TheItemType& ChangeFind (const Standard_Size theIndex) 
  {
    return ChangeValue(theIndex);
  }

  //! Set a value as explicit method
  TheItemType& Bind (const Standard_Size theIndex,
		     const TheItemType&     theValue) 
  {
    return SetValue(theIndex, theValue);
  }
  
  //! Returns True if the item is defined
  Standard_Boolean IsBound (const Standard_Size theIndex) const
  {
    return this->HasValue(theIndex);
  }
  
  //! Remove the item from array
  Standard_Boolean UnBind (const Standard_Size theIndex) 
  {
    return this->UnsetValue(theIndex);
  }
  
  //!@}

public:
  // Iterator interface

  /**
   * Implementation of type-specific const Iterator class
   */
  class ConstIterator : public NCollection_SparseArrayBase::Iterator
  {
  public:

    //! Empty constructor - for later Init
    ConstIterator () {}

    //! Constructor with initialisation
    ConstIterator (const NCollection_SparseArray& theVector) :
      NCollection_SparseArrayBase::Iterator (&theVector) {}

    //! Initialisation
    void Init (const NCollection_SparseArray& theVector) 
    { 
      this->init (&theVector); 
    } 

    //! Constant value access
    const TheItemType& Value (void) const
    {
      return *(const TheItemType*)this->value(); 
    }

    //! Constant value access operator
    const TheItemType& operator () (void) const
    {
      return *(const TheItemType*)this->value(); 
    }

    //! Access current index with 'a-la map' interface
    Standard_Size Key (void) const { return Index(); }
  };

  /**
   * Implementation of type-specific non-const Iterator class
   */
  class Iterator : public ConstIterator
  {
  public:

    //! Empty constructor - for later Init
    Iterator () {}

    //! Constructor with initialisation
    Iterator (NCollection_SparseArray& theVector) :
      ConstIterator (theVector) {}

    //! Initialisation
    void Init (const NCollection_SparseArray& theVector) 
    { 
      this->init (&theVector); 
    } 

    //! Value access
    TheItemType& ChangeValue (void)
    {
      return *(TheItemType*)this->value(); 
    }

    //! Value access operator
    TheItemType& operator () (void)
    {
      return *(TheItemType*)this->value(); 
    }

    //! Const access operator - the same as in parent class
    const TheItemType& operator () (void) const
    {
      return *(const TheItemType*)this->value(); 
    }
  };

private:
  // Implementation of virtual methods providing type-specific behaviour

  //! Create new item at the specified address with default constructor
//  virtual void createItem (Standard_Address theAddress) 
//  {
//    new (theAddress) TheItemType;
//  }
  
  //! Create new item at the specified address with copy constructor
  //! from existing item
  virtual void createItem (Standard_Address theAddress, Standard_Address theOther)
  {
    new (theAddress) TheItemType(*(const TheItemType*)theOther);
  }
  
  //! Call destructor to the item at given address
  virtual void destroyItem (Standard_Address theAddress)
  {
    ((TheItemType*)theAddress)->TheItemType::~TheItemType();
  }

  //! Call assignment operator to the item
  virtual void copyItem (Standard_Address theAddress, Standard_Address theOther)
  {
    (*(TheItemType*)theAddress) = *(const TheItemType*)theOther;
  }

};

#endif

