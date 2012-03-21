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

#ifndef TObj_ObjectIterator_HeaderFile
#define TObj_ObjectIterator_HeaderFile

#include <TObj_Common.hxx>

class Handle(TObj_Object);

/**
* This class provides an iterator by objects in a partition.
* (implements TObj_ObjectIterator interface)
*/

class TObj_ObjectIterator : public MMgt_TShared
{
 public: 
  /*
  * Methods to iterate on a objects
  */
  
  //! Returns True if iteration is not finished and method Current()
  //! will give the object.
  //! Default implementation returns False
  virtual Standard_EXPORT Standard_Boolean More () const;
  
  //! Iterates to the next object
  //! Default implementation does nothing
  virtual Standard_EXPORT void Next ();
  
  //! Returns current object (or null if iteration has finished)
  //! Default implementation returns null handle
  virtual Standard_EXPORT Handle(TObj_Object) Value () const;

 public:
   //! CASCADE RTTI
  DEFINE_STANDARD_RTTI(TObj_ObjectIterator)
};

//! Define handle class for TObj_ObjectIterator
DEFINE_STANDARD_HANDLE(TObj_ObjectIterator,MMgt_TShared)

#endif

#ifdef _MSC_VER
#pragma once
#endif
