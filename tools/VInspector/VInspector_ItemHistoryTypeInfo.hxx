// Created on: 2017-06-16
// Created by: Natalia ERMOLAEVA
// Copyright (c) 2017 OPEN CASCADE SAS
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

#ifndef VInspector_ItemHistoryTypeInfo_H
#define VInspector_ItemHistoryTypeInfo_H

#include <inspector/VInspector_CallBackMode.hxx>

//! \class VInspector_ItemHistoryTypeInfo
//! History info for a type item. It contains container of information for the element
struct VInspector_ItemHistoryTypeInfo
{

  //! Constructor
  VInspector_ItemHistoryTypeInfo() : myMode(VInspector_CallBackMode_None) {}

  //! Destructor
  VInspector_ItemHistoryTypeInfo (const VInspector_CallBackMode& theMode, const QList<QVariant>& theInfo)
    : myMode(theMode) { AddElement(theInfo); }

  //! Append new element
  //! \param theInfo an information element
  void AddElement (const QList<QVariant>& theInfo) { myElements.append(theInfo); }

  VInspector_CallBackMode myMode; //!< history element mode
  QList<QList<QVariant>> myElements; //!< container of elements information
};

#endif 
