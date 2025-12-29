// Created on: 2015-08-06
// Created by: Ilya Novikov
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

#ifndef _XCAFDoc_Dimension_HeaderFile
#define _XCAFDoc_Dimension_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <TDataStd_GenericEmpty.hxx>
class Standard_GUID;
class TDF_Label;
class XCAFDimTolObjects_DimensionObject;

// resolve name collisions with WinAPI headers
#ifdef GetObject
  #undef GetObject
#endif

//! Attribute that identifies a dimension in the GD&T table.
//! Its parent label is used as a container to store data provided
//! by XCAFDimTolObjects_DimensionObject.
class XCAFDoc_Dimension : public TDataStd_GenericEmpty
{

public:
  Standard_EXPORT XCAFDoc_Dimension();

  Standard_EXPORT static const Standard_GUID& GetID();

  Standard_EXPORT static occ::handle<XCAFDoc_Dimension> Set(const TDF_Label& theLabel);

  Standard_EXPORT const Standard_GUID& ID() const override;

  //! Updates parent's label and its sub-labels with data taken from theDimensionObject.
  //! Old data associated with the label will be lost.
  Standard_EXPORT void SetObject(
    const occ::handle<XCAFDimTolObjects_DimensionObject>& theDimensionObject);

  //! Returns dimension object data taken from the parent's label and its sub-labels.
  Standard_EXPORT occ::handle<XCAFDimTolObjects_DimensionObject> GetObject() const;

  //! Dumps the content of me into the stream
  Standard_EXPORT void DumpJson(Standard_OStream& theOStream, int theDepth = -1) const override;

  DEFINE_DERIVED_ATTRIBUTE(XCAFDoc_Dimension, TDataStd_GenericEmpty)
};

#endif
