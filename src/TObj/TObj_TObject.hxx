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
