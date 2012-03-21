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

#ifndef TObj_SequenceIterator_HeaderFile
#define TObj_SequenceIterator_HeaderFile

#include <TObj_Container.hxx>
#include <TObj_ObjectIterator.hxx>

/**
* This class is an iterator on sequence
*/

class TObj_SequenceIterator : public TObj_ObjectIterator
{

 protected:
  /**
  * Constructor
  */

  //! Creates an Empty Iterator
  Standard_EXPORT TObj_SequenceIterator();

 public:
  /**
  * Constructor
  */

  //! Creates an iterator an initialize it by sequence of objects.
  Standard_EXPORT TObj_SequenceIterator
                        (const Handle(TObj_HSequenceOfObject)& theObjects,
                         const Handle(Standard_Type)&              theType = NULL);
  
 public:
  /**
  * Redefined methods
  */
  
  //! Returns True if there is a current Item in the iteration.
  virtual Standard_EXPORT Standard_Boolean More () const;

  //! Move to the next Item
  virtual Standard_EXPORT void Next ();

  //! Returns the current item
  virtual Standard_EXPORT Handle(TObj_Object) Value () const;

 protected:
  /**
  * Fields
  */
  Standard_Integer                   myIndex; //!< current index of object in sequence
  Handle(Standard_Type)              myType;  //!< type of object
  Handle(TObj_HSequenceOfObject) myObjects;   //!< seqence of objects

 public:
   //! CASCADE RTTI
  DEFINE_STANDARD_RTTI(TObj_SequenceIterator)
};

//! Define handle class for TObj_SequenceIterator
DEFINE_STANDARD_HANDLE(TObj_SequenceIterator,TObj_ObjectIterator)

#endif

#ifdef _MSC_VER
#pragma once
#endif
