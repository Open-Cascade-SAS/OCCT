// Created on: 2004-11-23
// Created by: Pavel TELKOV
// Copyright (c) 2004-2014 OPEN CASCADE SAS
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

// The original implementation Copyright: (C) RINA S.p.A

#ifndef TObj_ObjectIterator_HeaderFile
#define TObj_ObjectIterator_HeaderFile

#include <TObj_Common.hxx>

class TObj_Object;

/**
* This class provides an iterator by objects in a partition.
* (implements TObj_ObjectIterator interface)
*/

class TObj_ObjectIterator : public Standard_Transient
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
  DEFINE_STANDARD_RTTIEXT(TObj_ObjectIterator,Standard_Transient)
};

//! Define handle class for TObj_ObjectIterator
DEFINE_STANDARD_HANDLE(TObj_ObjectIterator,Standard_Transient)

#endif

#ifdef _MSC_VER
#pragma once
#endif
