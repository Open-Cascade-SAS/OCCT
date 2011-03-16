// File:        NCollection_Handle.hxx
// Created:     Fri Jan 30 15:15:17 2009
// Author:      Andrey BETENEV (abv)
// Copyright:   Open CASCADE 2009

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
