// Created on: 2004-11-23
// Created by: Pavel TELKOV
// Copyright (c) 2004-2012 OPEN CASCADE SAS
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

// The original implementation Copyright: (C) RINA S.p.A

#ifndef TObj_Persistence_HeaderFile
#define TObj_Persistence_HeaderFile

#include <TObj_Container.hxx>

class TDF_Label;
class Handle(TObj_Object);

/** This class is intended to be a root of tools (one per class)
*   to manage persistence of objects inherited from TObj_Object
*   It provides a mechanism to recover correctly typed
*   objects (subtypes of TObj_Object) out of their persistent names
*
*   This is a special kind of object, it automatically registers itself
*   in a global map when created, and the only thing it does is to
*   create a new object of the type that it manages, by request
*/

class TObj_Persistence
{
public:
  /**
  * Public methods, to be called externally
  */

  //! Creates and returns a new object of the registered type
  //! If the type is not registered, returns Null handle
  static Standard_EXPORT Handle(TObj_Object) CreateNewObject
                                (const Standard_CString theType,
                                 const TDF_Label& theLabel);

  //! Dumps names of all the types registered for persistence to the
  //! specified stream
  static Standard_EXPORT void DumpTypes (Standard_OStream& theOs);

protected:
  /**
  * Protected methods, to be used or defined by descendants
  */

  //! The constructor registers the object
  Standard_EXPORT TObj_Persistence (const Standard_CString theType);

  //! The destructor unregisters the object
  virtual Standard_EXPORT ~TObj_Persistence ();

  //! The method must be redefined in the derived class and return
  //! new object of the proper type
  virtual Standard_EXPORT Handle(TObj_Object) New
                                (const TDF_Label& theLabel) const = 0;

  //! Dictionary storing all the registered types. It is implemented as static
  //! variable inside member function in order to ensure initialization
  //!  at first call
  static Standard_EXPORT TObj_DataMapOfStringPointer& getMapOfTypes();

 private:
  Standard_CString myType;  //!< Name of managed type (recorded for unregistering)
};

//! Declare subclass and methods of the class inherited from TObj_Object
//! necessary for implementation of persistence
//! This declaration should be put inside class declaration, under 'protected' modifier
#ifdef SOLARIS
//! Workaround on SUN to avoid stupid warnings
#define _TOBJOCAF_PERSISTENCE_ACCESS_ public:
#else
#define _TOBJOCAF_PERSISTENCE_ACCESS_
#endif
#define DECLARE_TOBJOCAF_PERSISTENCE(name,ancestor)                                      \
  name (const TObj_Persistence *p,                                                   \
        const TDF_Label& aLabel) : ancestor(p,aLabel)                                    \
  { initFields(); } /* give the object a chance to initialize its fields */              \
                                                                                         \
    /* Creates an object of a proper type */                                             \
    /* First argument is used just to avoid possible conflict with other constructors */ \
  _TOBJOCAF_PERSISTENCE_ACCESS_                                                          \
  class Persistence_ : public TObj_Persistence {                                     \
    /* Friend private class of name, is a tool providing persistence */                  \
  public:                                                                                \
    Persistence_ () : TObj_Persistence(#name) {} /* register the tool */             \
    virtual Handle(TObj_Object) New (const TDF_Label& aLabel) const;                 \
      /* Creates an object of a proper type */                                           \
  };                                                                                     \
  friend class Persistence_;                                                             \
  static Persistence_ myPersistence_; /* Static field implementing persistsnce tool */

//! Implement mechanism for registration the type for persistence
//! This should not be used for abstract classes (while DECLARE should)
#define IMPLEMENT_TOBJOCAF_PERSISTENCE(name)                                             \
  name::Persistence_ name::myPersistence_;                                               \
  Handle(TObj_Object) name::Persistence_::New (const TDF_Label& aLabel) const {      \
    return new name((const TObj_Persistence*)0, aLabel);                             \
  }

#endif

#ifdef _MSC_VER
#pragma once
#endif
