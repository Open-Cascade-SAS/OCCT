// Created on: 2009-01-30
// Created by: Andrey BETENEV (abv)
// Copyright (c) 2009-2014 OPEN CASCADE SAS
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

#ifndef NCollection_Handle_HeaderFile
#define NCollection_Handle_HeaderFile

#include <MMgt_TShared.hxx>

//! Standard type function allowing to check that contained object is Handle
Standard_EXPORT const Handle(Standard_Type)& STANDARD_TYPE(NCollection_Handle);
  
//! Purpose: This template class is used to define Handle adaptor
//! for allocated dynamically objects of arbitrary type.
//!
//! The advantage is that this handle will automatically destroy 
//! the object when last referred Handle is destroyed (i.e. it is a 
//! typical smart pointer), and that it can be handled as 
//! Handle(Standard_Transient) in OCCT components.
//!
//! Use it as follows:
//!
//! NCollection_Handle<T> aPtr = new T (...);
//!
//! aPtr->Method(...);
//!
//! Handle(Standard_Transient) aBase = aPtr;
//! if ( aBase->IsKind(STANDARD_TYPE(NCollection_Handle)) )
//! {
//!   NCollection_Handle<T> aPtr2 = NCollection_Handle<T>::DownCast (aBase);
//!   if ( ! aPtr2.IsNull() )
//!     aPtr2->Method2();
//! }

template <class T>
class NCollection_Handle : public Handle(Standard_Transient)
{
 private:

  //! Internal adaptor class wrapping actual type
  //! and enhancing it by reference counter inherited from
  //! Standard_Transient
  class Ptr : public Standard_Transient
  {
  public:

    //! Constructor: stores pointer to the object
    Ptr (T* theObj) { myPtr = theObj; }

    //! Destructor deletes the object
    ~Ptr () { if ( myPtr ) delete myPtr; myPtr = 0; }

    //! Implementation of DynamicType() method
    const Handle(Standard_Type)& DynamicType() const 
      { return STANDARD_TYPE(NCollection_Handle); }

  protected:

    //! Copy constructor
    Ptr(const Ptr&);

    //! Assignment operator
    Ptr& operator=(const Ptr&);

  public:
    T* myPtr; //!< Pointer to the object
  };
  
  //! Constructor of handle from pointer on newly allocated object.
  //! Note that additional argument is used to avoid ambiguity with
  //! public constructor from pointer when Handle is intilialized by 0.
  NCollection_Handle (Ptr* thePtr, int) 
    : Handle(Standard_Transient) (thePtr) {}
  
 public:

  typedef T element_type;

  //! Default constructor; creates null handle
  NCollection_Handle () {}
  
  //! Constructor of handle from pointer on newly allocated object
  NCollection_Handle (T* theObject) 
    : Handle(Standard_Transient) (theObject ? new Ptr (theObject) : 0) {}
  
  //! Cast handle to contained type
  T* operator -> () { return ((Ptr*)ControlAccess())->myPtr; }
  
  //! Cast handle to contained type
  const T* operator -> () const { return ((Ptr*)ControlAccess())->myPtr; }
  
  //! Cast handle to contained type
  T& operator * () { return *((Ptr*)ControlAccess())->myPtr; }
  
  //! Cast handle to contained type
  const T& operator * () const { return *((Ptr*)ControlAccess())->myPtr; }
  
  //! Downcast arbitrary Handle to the argument type if contained
  //! object is Handle for this type; returns null otherwise
  static NCollection_Handle<T> DownCast (const Handle(Standard_Transient)& theOther) 
  {
    return NCollection_Handle<T> (theOther.IsNull() ? 0 : dynamic_cast<Ptr*> (theOther.operator->()), 0);
  }
};

#endif
