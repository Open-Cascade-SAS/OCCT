// Created on: 2007-04-17
// Created by: Michael Sazonov
// Copyright (c) 2007-2012 OPEN CASCADE SAS
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

#ifndef TObj_CheckModel_HeaderFile
#define TObj_CheckModel_HeaderFile

#include <TObj_Common.hxx>
#include <TObj_Model.hxx>
#include <Message_Algorithm.hxx>

/**
 * This class provides consistency check of the TObj model.
 * It collects all inconsistencies in the status bits and prepaires
 * messages to be sent using SendStatusMessages (SendMessages) method.
 * It supports also the fix mode, in which some inconsistencies are
 * corrected.
 */

class TObj_CheckModel: public Message_Algorithm
{
public:
  //! Initialize checker by model
  TObj_CheckModel(const Handle(TObj_Model)& theModel)
    : myModel (theModel),
      myToFix (Standard_False)
  {}

  //! Sets flag allowing fixing inconsistencies
  void SetToFix(const Standard_Boolean theToFix)
  { myToFix = theToFix; }

  //! Returns true if it is allowed to fix inconsistencies
  Standard_Boolean IsToFix() const
  { return myToFix; }

  //! Returns the checked model
  const Handle(TObj_Model)& GetModel() const
  { return myModel; }

  //! Empty virtual destructor
  virtual ~TObj_CheckModel () {}

  //! Performs all checks. Descendants should call parent method before
  //! doing own checks.
  //! This implementation checks OCAF references and back references
  //! between objects of the model.
  //! Returns true if no inconsistencies found.
  virtual Standard_EXPORT Standard_Boolean Perform();

private:
  //! Check References (and back references in model).
  //! This method just tries to find object to that this reference is indicate and
  //! test if that object is not null or not deleted. Also it test if that object has back 
  //! reference to correct object (object that has forward reference).
  Standard_EXPORT Standard_Boolean checkReferences();

private:
  Handle(TObj_Model) myModel;
  Standard_Boolean       myToFix;

 public:
  //! Declaration of CASCADE RTTI
  DEFINE_STANDARD_RTTI (TObj_CheckModel)
};

//! Definition of HANDLE object using Standard_DefineHandle.hxx
DEFINE_STANDARD_HANDLE (TObj_CheckModel,Message_Algorithm)

#endif

#ifdef _MSC_VER
#pragma once
#endif
