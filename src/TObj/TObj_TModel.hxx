// Created on: 2004-11-23
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

#ifndef TObj_TModel_HeaderFile
#define TObj_TModel_HeaderFile

#include <TDF_Attribute.hxx>
#include <Handle_TObj_Model.hxx>

class Handle(TDF_RelocationTable);
class Handle(TObj_TModel);

/** 
* Attribute to store OCAF-based models in OCAF tree
* The persistency mechanism of the TObj_TModel allowes to save
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
  Standard_EXPORT const Standard_GUID& ID() const;
  
 public:
  //! Methods for setting and obtaining the Model object
  
  //! Sets the the Model object
  Standard_EXPORT void Set(const Handle(TObj_Model)& theModel);
  
  //! Returns the Model object
  Standard_EXPORT Handle(TObj_Model) Model() const;
  
 public:
  //! Redefined OCAF abstract methods
    
  //! Returns an new empty TObj_TModel attribute. It is used by the
  //! copy algorithm.
  Standard_EXPORT Handle(TDF_Attribute) NewEmpty() const;
  
  //! Restores the backuped contents from <theWith> into this one. It is used 
  //! when aborting a transaction.
  Standard_EXPORT void Restore (const Handle(TDF_Attribute)& theWith);
  
  //! This method is used when copying an attribute from a source structure
  //! into a target structure.
  Standard_EXPORT void Paste (const Handle(TDF_Attribute)& theInto,
                              const Handle(TDF_RelocationTable)& theRT) const;
  
 private:
  //! Fields
  Handle(TObj_Model) myModel; //!< The Model object stored by the attribute
  
 public:
  //! CASCADE RTTI
  DEFINE_STANDARD_RTTI(TObj_TModel)
};

//! Define handle class for TObj_TModel
DEFINE_STANDARD_HANDLE(TObj_TModel,TDF_Attribute)

#endif

#ifdef _MSC_VER
#pragma once
#endif
