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
