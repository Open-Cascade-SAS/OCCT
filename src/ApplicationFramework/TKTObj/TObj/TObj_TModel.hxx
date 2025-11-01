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

#ifndef TObj_TModel_HeaderFile
#define TObj_TModel_HeaderFile

#include <TDF_Attribute.hxx>

class TObj_Model;

/**
 * Attribute to store OCAF-based models in OCAF tree
 * The persistency mechanism of the TObj_TModel allows to save
 * and restore various types of models without recompilation of the schema
 */

class TObj_TModel : public TDF_Attribute
{
public:
  /**
   * Standard methods of attribute
   */

  //! Empty constructor
  Standard_EXPORT TObj_TModel();

  //! This method is used in implementation of ID()
  static Standard_EXPORT const Standard_GUID& GetID();

  //! Returns the ID of TObj_TModel attribute.
  Standard_EXPORT const Standard_GUID& ID() const Standard_OVERRIDE;

public:
  //! Methods for setting and obtaining the Model object

  //! Sets the Model object
  Standard_EXPORT void Set(const Handle(TObj_Model)& theModel);

  //! Returns the Model object
  Standard_EXPORT Handle(TObj_Model) Model() const;

public:
  //! Redefined OCAF abstract methods

  //! Returns an new empty TObj_TModel attribute. It is used by the
  //! copy algorithm.
  Standard_EXPORT Handle(TDF_Attribute) NewEmpty() const Standard_OVERRIDE;

  //! Restores the backuped contents from <theWith> into this one. It is used
  //! when aborting a transaction.
  Standard_EXPORT void Restore(const Handle(TDF_Attribute)& theWith) Standard_OVERRIDE;

  //! This method is used when copying an attribute from a source structure
  //! into a target structure.
  Standard_EXPORT void Paste(const Handle(TDF_Attribute)&       theInto,
                             const Handle(TDF_RelocationTable)& theRT) const Standard_OVERRIDE;

private:
  //! Fields
  Handle(TObj_Model) myModel; //!< The Model object stored by the attribute

public:
  //! CASCADE RTTI
  DEFINE_STANDARD_RTTIEXT(TObj_TModel, TDF_Attribute)
};

//! Define handle class for TObj_TModel
DEFINE_STANDARD_HANDLE(TObj_TModel, TDF_Attribute)

#endif

#ifdef _MSC_VER
#pragma once
#endif
