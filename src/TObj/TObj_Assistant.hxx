// Created on: 2004-11-22
// Created by: Pavel TELKOV
// Copyright (c) 2004-2012 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.

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
