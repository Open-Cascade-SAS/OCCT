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
  //! Creates the iterator on TObj objects on the sub-labels of theLabel.
  //! @param theLabel start label for searching
  //! @param theType type of the found objects, or all types if Null
  //! @param theRecursive search children recursively, not only on sub-labels of theLabel
  //! @param theAllSubChildren do not stop at the first level of children, but search for
  //! sub-children too
  Standard_EXPORT TObj_OcafObjectIterator(
    const TDF_Label&             theLabel,
    const Handle(Standard_Type)& theType           = NULL,
    const Standard_Boolean       theRecursive      = Standard_False,
    const Standard_Boolean       theAllSubChildren = Standard_False);

protected:
  //! Shift iterator to the next object
  virtual Standard_EXPORT void MakeStep() Standard_OVERRIDE;

protected:
  Handle(Standard_Type) myType; //!< type of objects to iterate on
  // clang-format off
  Standard_Boolean myAllSubChildren; //!< to iterate all sub-children, do not stop on the first level
  // clang-format on

public:
  //! CASCADE RTTI
  DEFINE_STANDARD_RTTIEXT(TObj_OcafObjectIterator, TObj_LabelIterator)
};

//! Define handle class for TObj_OcafObjectIterator
DEFINE_STANDARD_HANDLE(TObj_OcafObjectIterator, TObj_LabelIterator)

#endif

#ifdef _MSC_VER
#pragma once
#endif
