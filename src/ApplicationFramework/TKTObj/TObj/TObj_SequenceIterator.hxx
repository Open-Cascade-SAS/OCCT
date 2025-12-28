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

#ifndef TObj_SequenceIterator_HeaderFile
#define TObj_SequenceIterator_HeaderFile

#include <NCollection_DataMap.hxx>
#include <TCollection_AsciiString.hxx>
#include <TCollection_HExtendedString.hxx>
#include <TDF_Label.hxx>
#include <TObj_SequenceOfObject.hxx>
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
  Standard_EXPORT TObj_SequenceIterator(
    const occ::handle<NCollection_HSequence<occ::handle<TObj_Object>>>& theObjects,
    const occ::handle<Standard_Type>&                                   theType = nullptr);

public:
  /**
   * Redefined methods
   */

  //! Returns True if there is a current Item in the iteration.
  Standard_EXPORT bool More() const override;

  //! Move to the next Item
  Standard_EXPORT void Next() override;

  //! Returns the current item
  Standard_EXPORT occ::handle<TObj_Object> Value() const override;

protected:
  /**
   * Fields
   */
  int                        myIndex; //!< current index of object in sequence
  occ::handle<Standard_Type> myType;  //!< type of object
  occ::handle<NCollection_HSequence<occ::handle<TObj_Object>>> myObjects; //!< sequence of objects

public:
  //! CASCADE RTTI
  DEFINE_STANDARD_RTTIEXT(TObj_SequenceIterator, TObj_ObjectIterator)
};

//! Define handle class for TObj_SequenceIterator
#endif

#ifdef _MSC_VER
#pragma once
#endif
