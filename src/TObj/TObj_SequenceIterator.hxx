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
  Standard_EXPORT TObj_SequenceIterator(const Handle(TObj_HSequenceOfObject)& theObjects,
                                        const Handle(Standard_Type)&          theType = NULL);

public:
  /**
   * Redefined methods
   */

  //! Returns True if there is a current Item in the iteration.
  virtual Standard_EXPORT Standard_Boolean More() const Standard_OVERRIDE;

  //! Move to the next Item
  virtual Standard_EXPORT void Next() Standard_OVERRIDE;

  //! Returns the current item
  virtual Standard_EXPORT Handle(TObj_Object) Value() const Standard_OVERRIDE;

protected:
  /**
   * Fields
   */
  Standard_Integer               myIndex;   //!< current index of object in sequence
  Handle(Standard_Type)          myType;    //!< type of object
  Handle(TObj_HSequenceOfObject) myObjects; //!< sequence of objects

public:
  //! CASCADE RTTI
  DEFINE_STANDARD_RTTIEXT(TObj_SequenceIterator, TObj_ObjectIterator)
};

//! Define handle class for TObj_SequenceIterator
DEFINE_STANDARD_HANDLE(TObj_SequenceIterator, TObj_ObjectIterator)

#endif

#ifdef _MSC_VER
#pragma once
#endif
