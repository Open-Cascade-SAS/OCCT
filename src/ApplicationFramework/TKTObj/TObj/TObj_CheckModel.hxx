// Created on: 2007-04-17
// Created by: Michael Sazonov
// Copyright (c) 2007-2014 OPEN CASCADE SAS
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

#ifndef TObj_CheckModel_HeaderFile
#define TObj_CheckModel_HeaderFile

#include <TObj_Model.hxx>
#include <Message_Algorithm.hxx>

/**
 * This class provides consistency check of the TObj model.
 * It collects all inconsistencies in the status bits and prepares
 * messages to be sent using SendStatusMessages (SendMessages) method.
 * It supports also the fix mode, in which some inconsistencies are
 * corrected.
 */

class TObj_CheckModel : public Message_Algorithm
{
public:
  //! Initialize checker by model
  TObj_CheckModel(const Handle(TObj_Model)& theModel)
      : myModel(theModel),
        myToFix(Standard_False)
  {
  }

  //! Sets flag allowing fixing inconsistencies
  void SetToFix(const Standard_Boolean theToFix) { myToFix = theToFix; }

  //! Returns true if it is allowed to fix inconsistencies
  Standard_Boolean IsToFix() const { return myToFix; }

  //! Returns the checked model
  const Handle(TObj_Model)& GetModel() const { return myModel; }

  //! Empty virtual destructor
  virtual ~TObj_CheckModel() {}

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
  Standard_Boolean   myToFix;

public:
  //! Declaration of CASCADE RTTI
  DEFINE_STANDARD_RTTIEXT(TObj_CheckModel, Message_Algorithm)
};

//! Definition of HANDLE object using Standard_DefineHandle.hxx
DEFINE_STANDARD_HANDLE(TObj_CheckModel, Message_Algorithm)

#endif

#ifdef _MSC_VER
#pragma once
#endif
