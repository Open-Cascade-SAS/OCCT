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

#ifndef TObj_ReferenceIterator_HeaderFile
#define TObj_ReferenceIterator_HeaderFile

#include <TObj_LabelIterator.hxx>

/**
* This class provides an iterator by references of the object
* (implements TObj_ReferenceIterator interface)
*/

class TObj_ReferenceIterator : public TObj_LabelIterator
{
 public:
  /*
  * Constructor
  */
  
  //! Creates the iterator on references in partition
  //! theType narrows a variety of iterated objects
  Standard_EXPORT TObj_ReferenceIterator
                         (const TDF_Label&             theLabel,
                          const Handle(Standard_Type)& theType = NULL,
                          const Standard_Boolean       theRecursive = Standard_True);
  
 protected:
  /**
  * Internal methods
  */
  
  //! Shift iterator to the next object
  virtual Standard_EXPORT void MakeStep();

  Handle(Standard_Type) myType; //!< Type of objects to iterate on
  
 public:
  //! CASCADE RTTI
  DEFINE_STANDARD_RTTI(TObj_ReferenceIterator)
};

//! Define handle class for TObj_ReferenceIterator
DEFINE_STANDARD_HANDLE(TObj_ReferenceIterator,TObj_LabelIterator)
 
#endif

#ifdef _MSC_VER
#pragma once
#endif
