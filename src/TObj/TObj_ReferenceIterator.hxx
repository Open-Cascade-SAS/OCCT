// File:        TObj_ReferenceIterator.hxx
// Created:     Tue Nov 23 12:18:32 2004
// Author:      Pavel TELKOV
// Copyright:   Open CASCADE  2007
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
