// Created on: 2004-11-22
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

#ifndef TObj_Assistant_HeaderFile
#define TObj_Assistant_HeaderFile

#include <TObj_Common.hxx>

class Handle(TObj_Model);
class TColStd_SequenceOfTransient;
class TColStd_SequenceOfAsciiString;
class TColStd_IndexedMapOfTransient;

//! This class provides interface to the static data
//! to be used during save or load models.
//!
//! Static data:
//! 1. DataMap of Modeller name - handle to model to be used in loading of references
//! 2. Indexed map of Standard_Type to be used during save or load of object type
//! 3. Handle to the current model - model that is loaded at the current moment

class TObj_Assistant
{
public:
  /**
  * Interface for DataMap of Modeller name
  */

  //! Finds model by name
  static Standard_EXPORT Handle(TObj_Model)
    FindModel     (const Standard_CString theName);

  //! Binds model to the map
  static Standard_EXPORT void
    BindModel     (const Handle(TObj_Model) theModel);

  //! Clears all records from the model map
  static Standard_EXPORT void
    ClearModelMap ();

public:
  /**
  * Interface for Map of Standard Types
  */

  //! Finds Standard_Type by index;
  //! returns NULL handle if not found
  static Standard_EXPORT Handle(Standard_Type)
    FindType      (const Standard_Integer theTypeIndex);

  //! Rinds index by Standard_Type;
  //! returns 0 if not found
  static Standard_EXPORT Standard_Integer
    FindTypeIndex (const Handle(Standard_Type)& theType);

  //! Binds Standard_Type to the map;
  //! returns index of bound type
  static Standard_EXPORT Standard_Integer
    BindType      (const Handle(Standard_Type)& theType);

  //! Clears map of types
  static Standard_EXPORT void
    ClearTypeMap  ();

public:
  /**
  * Interface to the current model
  */

  //! Sets current model
  static Standard_EXPORT void
    SetCurrentModel (const Handle(TObj_Model)& theModel);

  //! Returns current model
  static Standard_EXPORT Handle(TObj_Model)
    GetCurrentModel();

  //! Unsets current model
  static Standard_EXPORT void
    UnSetCurrentModel ();

public:

  //! Returns the version of application which wrote the currently read document.
  //! Returns 0 if it has not been set yet for the current document.
  static Standard_EXPORT Standard_Integer               GetAppVersion();

private:

  //! Method for taking fields for map of models
  static Standard_EXPORT TColStd_SequenceOfTransient&   getModels();

  //! Method for taking fields for map types
  static Standard_EXPORT TColStd_IndexedMapOfTransient& getTypes();

  //! Method for taking fields for the Current model
  static Standard_EXPORT Handle(TObj_Model)&            getCurrentModel();

  //! Returns application version
  static Standard_EXPORT Standard_Integer&              getVersion();
};

#endif

#ifdef _MSC_VER
#pragma once
#endif
