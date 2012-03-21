// Created on: 2001-01-31
// Created by: Pavel TELKOV
// Copyright (c) 2001-2012 OPEN CASCADE SAS
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

#ifndef TObj_ModelIterator_HeaderFile
#define TObj_ModelIterator_HeaderFile

#include <TObj_Object.hxx>
#include <TObj_ObjectIterator.hxx>
#include <TObj_SequenceOfIterator.hxx>

class Handle(TObj_Model);

/**
* This class provides an iterator by all objects in the model.
*/

class TObj_ModelIterator : public TObj_ObjectIterator
{
 public:
  /**
  * Constructor
  */
  //! Creates Iterator and initialize it by Model`s label
  Standard_EXPORT TObj_ModelIterator(const Handle(TObj_Model)& theModel);
  
 public:
  /**
  * Methods to iterate on objects.
  */
  
  //! Returns True if iteration is not finished and method Value()
  //! will give the object
  virtual Standard_EXPORT Standard_Boolean More() const;
  
  //! Iterates to the next object
  virtual Standard_EXPORT void Next ();
  
  //! Returns current object (or MainObj of Model if iteration has finished)
  virtual Standard_EXPORT Handle(TObj_Object) Value () const;
  
 protected:
  /**
   * private methods
   */
  
  //! Add iterator on children of indicated object recursively.
  virtual Standard_EXPORT void addIterator(const Handle(TObj_Object)& theObj);
  
 protected:
  /**
  * Fields
  */
  Handle(TObj_Object)  myObject;     //!< Current object
  TObj_SequenceOfIterator myIterSeq; //!< Sequence of iterators in model
    
 public:
  //! CASCADE RTTI
  DEFINE_STANDARD_RTTI(TObj_ModelIterator)
};

//! Define handle class for TObj_ObjectIterator
DEFINE_STANDARD_HANDLE(TObj_ModelIterator,TObj_ObjectIterator)

#endif

#ifdef _MSC_VER
#pragma once
#endif
