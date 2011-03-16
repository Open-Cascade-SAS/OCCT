// File:        TObj_ObjectIterator.hxx
// Created:     Tue Nov  23 12:17:17 2004
// Author:      Pavel TELKOV
// Copyright:   Open CASCADE  2007
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
