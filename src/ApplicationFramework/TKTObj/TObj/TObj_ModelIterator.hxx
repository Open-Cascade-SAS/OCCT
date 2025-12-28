// Created on: 2001-01-31
// Created by: Pavel TELKOV
// Copyright (c) 2001-2014 OPEN CASCADE SAS
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

#ifndef TObj_ModelIterator_HeaderFile
#define TObj_ModelIterator_HeaderFile

#include <TObj_Object.hxx>
#include <NCollection_Sequence.hxx>
#include <TObj_ObjectIterator.hxx>

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
  Standard_EXPORT TObj_ModelIterator(const occ::handle<TObj_Model>& theModel);

public:
  /**
   * Methods to iterate on objects.
   */

  //! Returns True if iteration is not finished and method Value()
  //! will give the object
  virtual Standard_EXPORT bool More() const override;

  //! Iterates to the next object
  virtual Standard_EXPORT void Next() override;

  //! Returns current object (or MainObj of Model if iteration has finished)
  virtual Standard_EXPORT occ::handle<TObj_Object> Value() const override;

protected:
  /**
   * private methods
   */

  //! Add iterator on children of indicated object recursively.
  virtual Standard_EXPORT void addIterator(const occ::handle<TObj_Object>& theObj);

protected:
  /**
   * Fields
   */
  occ::handle<TObj_Object> myObject; //!< Current object
  NCollection_Sequence<occ::handle<TObj_ObjectIterator>>
    myIterSeq; //!< Sequence of iterators in model

public:
  //! CASCADE RTTI
  DEFINE_STANDARD_RTTIEXT(TObj_ModelIterator, TObj_ObjectIterator)
};

//! Define handle class for TObj_ObjectIterator
#endif

#ifdef _MSC_VER
#pragma once
#endif
