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

#ifndef TObj_TReference_HeaderFile
#define TObj_TReference_HeaderFile

#include <TObj_Common.hxx>
#include <TDF_Attribute.hxx>
#include <TDF_Label.hxx>

class Handle(TObj_TReference);
class Handle(TObj_Object);
class Standard_GUID;
class Handle(TDF_RelocationTable);

/**
* Attribute for storing references to the objects which implement
* TObj_Object interface in the OCAF tree.
* Its persistency mechanism provides transparent method for storing
* cross-model references.
* Each reference, when created, registers itself in the referred object,
* to support back references
*/

class TObj_TReference : public TDF_Attribute
{
 public:
  //! Standard methods of OCAF attribute

  //! Empty constructor
  Standard_EXPORT TObj_TReference();

  //! This method is used in implementation of ID()
  static Standard_EXPORT const Standard_GUID& GetID();

  //! Returns the ID of TObj_TReference attribute.
  Standard_EXPORT const Standard_GUID& ID() const;

 public:
  //! Method for create TObj_TReference object

  //! Creates reference on TDF_Label <theLabel> to the object <theObject> and
  //! creates backreference from the object <theObject> to <theMaster> one.
  static Standard_EXPORT Handle(TObj_TReference) Set
                         (const TDF_Label&               theLabel,
                          const Handle(TObj_Object)& theObject,
                          const Handle(TObj_Object)& theMaster);

 public:
  //! Methods for setting and obtaining referenced object

  //! Sets the reference to the theObject
  Standard_EXPORT void Set(const Handle(TObj_Object)& theObject,
                           const TDF_Label&               theMasterLabel);

  //! Sets the reference to the theObject at indicated Label.
  //! It is method for persistent only. Don`t use anywhere else.
  Standard_EXPORT void Set(const TDF_Label& theLabel,
                           const TDF_Label& theMasterLabel);

  //! Returns the referenced theObject
  Standard_EXPORT Handle(TObj_Object) Get() const;

  //! Returns the Label of master object.
  Standard_EXPORT TDF_Label GetMasterLabel() const {return myMasterLabel;}

  //! Returns the referred label.
  Standard_EXPORT TDF_Label GetLabel() const {return myLabel;}

 public:
  //! Redefined OCAF abstract methods

  //! Returns an new empty TObj_TReference attribute. It is used by the
  //! copy algorithm.
  Standard_EXPORT Handle(TDF_Attribute) NewEmpty() const;

  //! Restores the backuped contents from <theWith> into this one. It is used
  //! when aborting a transaction.
  Standard_EXPORT void Restore(const Handle(TDF_Attribute)& theWith);

  //! This method is used when copying an attribute from a source structure
  //! into a target structure.
  Standard_EXPORT void Paste(const Handle(TDF_Attribute)&       theInto,
                             const Handle(TDF_RelocationTable)& theRT) const;

  //! Remove back references of it reference if it is in other document.
  virtual Standard_EXPORT void BeforeForget();

  //! It is necessary for tranzaction mechanism (Undo/Redo).
  virtual Standard_EXPORT Standard_Boolean BeforeUndo
                   (const Handle(TDF_AttributeDelta)& theDelta,
                    const Standard_Boolean            isForced = Standard_False);

  //! It is necessary for tranzaction mechanism (Undo/Redo).
  virtual Standard_EXPORT Standard_Boolean AfterUndo
                   (const Handle(TDF_AttributeDelta)& theDelta,
                    const Standard_Boolean            isForced = Standard_False);

  //! Check if back reference exists for reference.
  virtual Standard_EXPORT void AfterResume();

  //! Called after retrieval reference from file.
  virtual Standard_EXPORT Standard_Boolean AfterRetrieval
                         (const Standard_Boolean forceIt = Standard_False);

 private:
  //! Fields
  TDF_Label myLabel;          //!< Label that indicate referenced object
  TDF_Label myMasterLabel;    //!< Label of object that have this reference.

 public:
  //! CASCADE RTTI
  DEFINE_STANDARD_RTTI(TObj_TReference)
};

//! Define handle class for TObj_TReference
DEFINE_STANDARD_HANDLE(TObj_TReference,TDF_Attribute)

#endif

#ifdef _MSC_VER
#pragma once
#endif
