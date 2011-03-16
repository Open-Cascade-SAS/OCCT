// File:        TObj_OcafObjectIterator.hxx
// Created:     Tue Nov  23 12:17:17 2004
// Author:      Pavel TELKOV
// Copyright:   Open CASCADE  2007
// The original implementation Copyright: (C) RINA S.p.A

#ifndef TObj_OcafObjectIterator_HeaderFile
#define TObj_OcafObjectIterator_HeaderFile

#include <TObj_LabelIterator.hxx>

/**
* This class provides an iterator by objects in a partition.
* (implements TObj_ObjectIterator interface)
*/

class TObj_OcafObjectIterator : public TObj_LabelIterator
{
 public:
  /**
  * Constructor
  */

  //! Creates the iterator on objects in the sub labels of theLabel
  //! theType narrows a variety of iterated objects
  Standard_EXPORT TObj_OcafObjectIterator
                         (const TDF_Label&             theLabel,
                          const Handle(Standard_Type)& theType = NULL,
                          const Standard_Boolean       theRecursive = Standard_False);

 protected:
  /*
  * Internal methods
  */

  //! Shift iterator to the next object
  virtual Standard_EXPORT void MakeStep();

 protected:
  /**
  * fields
  */

  Handle(Standard_Type) myType; //!< type of objects to iterate on
  
 public:
   //! CASCADE RTTI
  DEFINE_STANDARD_RTTI(TObj_OcafObjectIterator)
};

//! Define handle class for TObj_OcafObjectIterator
DEFINE_STANDARD_HANDLE(TObj_OcafObjectIterator,TObj_LabelIterator)

#endif

#ifdef _MSC_VER
#pragma once
#endif
