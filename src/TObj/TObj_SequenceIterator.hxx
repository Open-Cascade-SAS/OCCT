// File:        TObj_SequenceIterator.hxx
// Created:     Tue Nov 23 12:38:17 2004
// Author:      Pavel TELKOV
// Copyright:   Open CASCADE  2007
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
