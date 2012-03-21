// Created on: 2009-01-30
// Created by: Andrey BETENEV (abv)
// Copyright (c) 2009-2012 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.


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
class NCollection_Handle : public Handle_Standard_Transient
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

  public:
    T* myPtr; //!< Pointer to the object
  };
  
  //! Constructor of handle from pointer on newly allocated object.
  //! Note that additional argument is used to avoid ambiguity with
  //! public constructor from pointer when Handle is intilialized by 0.
  NCollection_Handle (Ptr* thePtr, int) 
    : Handle_Standard_Transient (thePtr) {}
  
 public:
  
  //! Default constructor; creates null handle
  NCollection_Handle () {}
  
  //! Constructor of handle from pointer on newly allocated object
  NCollection_Handle (T* theObject) 
    : Handle_Standard_Transient (theObject ? new Ptr (theObject) : 0) {}
  
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
