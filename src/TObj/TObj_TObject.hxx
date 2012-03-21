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

#ifndef TObj_TObject_HeaderFile
#define TObj_TObject_HeaderFile

#include <TObj_Common.hxx>
#include <TObj_Object.hxx>

#include <TDF_Attribute.hxx>

class Handle(TObj_TObject);
class Standard_GUID;
class TDF_Label;
class Handle(TDF_RelocationTable);

/** 
* Purpose: OCAF Attribute to storing objects (interfaces) of OCAF-based 
* modelers in the OCAF tree.
* The persistency mechanism of the TObj_TObject allowes to save
* and restore objects of various subtypes without recompilation of the schema
*/

class TObj_TObject : public TDF_Attribute
{
 public:
  //! Standard methods of OCAF attribute

  //! Empty constructor
  Standard_EXPORT TObj_TObject();
  
  //! This method is used in implementation of ID()
  static Standard_EXPORT const Standard_GUID& GetID();
  
  //! Returns the ID of TObj_TObject attribute.
  Standard_EXPORT const Standard_GUID& ID() const;

 public:
  //! Method for create TObj_TObject object
  
  //! Creates TObj_TObject attribute on given label
  static Standard_EXPORT Handle(TObj_TObject) Set(const TDF_Label& theLabel, 
                                                      const Handle(TObj_Object)& theElem);
  
 public:
  //! Methods for setting and obtaining TObj_Object
  
  //! Sets the TObj_Object object
  Standard_EXPORT void Set(const Handle(TObj_Object)& theElem);
  
  //! Returns the TObj_Object object
  Standard_EXPORT Handle(TObj_Object) Get() const;
  
 public:
  //! Redefined OCAF abstract methods 
    
  //! Returns an new empty TObj_TObject attribute. It is used by the
  //! copy algorithm.
  Standard_EXPORT Handle(TDF_Attribute) NewEmpty() const;
  
  //! Restores the backuped contents from <theWith> into this one. It is used 
  //! when aborting a transaction.
  Standard_EXPORT void Restore(const Handle(TDF_Attribute)& theWith);
  
  //! This method is used when copying an attribute from a source structure
  //! into a target structure.
  Standard_EXPORT void Paste(const Handle(TDF_Attribute)& theInto,
                             const Handle(TDF_RelocationTable)& theRT) const;
  
  //! Tell TObj_Object to die,
  //! i.e. (myElem->IsAlive() == false) after that
  Standard_EXPORT void BeforeForget();
  
  //! Tell TObj_Object to rise from the dead,
  //! i.e. (myElem->IsAlive() == true) after that
  Standard_EXPORT Standard_Boolean AfterUndo
                              (const Handle(TDF_AttributeDelta)& anAttDelta,
                               const Standard_Boolean forceIt);
  
 private:
  //! Fields
  Handle(TObj_Object) myElem; //!< The object interface stored by the attribute
  
 public:
  //! CASCADE RTTI
  DEFINE_STANDARD_RTTI(TObj_TObject)
};

//! Define handle class for TObj_TObject
DEFINE_STANDARD_HANDLE(TObj_TObject,TDF_Attribute)

#endif

#ifdef _MSC_VER
#pragma once
#endif
