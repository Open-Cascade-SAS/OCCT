// File:        TObj_CheckModel.hxx
// Created:     17.04.2007
// Author:      Michael Sazonov
// Copyright:   Open CASCADE  2007
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
