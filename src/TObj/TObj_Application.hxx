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

#ifndef TObj_Application_HeaderFile
#define TObj_Application_HeaderFile

#include <TDocStd_Application.hxx>
#include <TObj_Common.hxx>
#include <Message_Gravity.hxx>
#include <Message_Messenger.hxx>

class Handle(TObj_Application);
class TColStd_SequenceOfExtendedString;

//!
//! This is a base class for OCAF based TObj models
//!  with declared virtual methods
//!

class TObj_Application : public TDocStd_Application
{
public:
  //! Returns static instance of the application
  Standard_EXPORT static Handle(TObj_Application) GetInstance();
 
  //! Returns reference to associated messenger handle
  Standard_EXPORT Handle(Message_Messenger) &Messenger() { return myMessenger; }
    
 public:
  /**
  *    Load/Save support
  */

  //! Saving the OCAF document to a file
  virtual Standard_EXPORT Standard_Boolean SaveDocument
                         (const Handle(TDocStd_Document)& theSourceDoc,
                          const char*                     theTargetFile);

  //! Loading the OCAF document from a file
  virtual Standard_EXPORT Standard_Boolean LoadDocument
                         (const char*                     theSourceFile,
                          Handle(TDocStd_Document)&       theTargetDoc);

  //! Create the OCAF document from scratch
  virtual Standard_EXPORT Standard_Boolean CreateNewDocument
                         (Handle(TDocStd_Document)&         theDoc,
                          const TCollection_ExtendedString& theFormat);

  //! Signal error during Load or Save
  //! Default imiplementation is empty
  virtual Standard_EXPORT void ErrorMessage
                        (const TCollection_ExtendedString &theMsg,
                         const Message_Gravity theLevel);

  //! Signal error during Load or Save
  //! Default imiplementation invoke previous declaration with 0
  virtual Standard_EXPORT void ErrorMessage
                        (const TCollection_ExtendedString &theMsg)
  { ErrorMessage( theMsg, Message_Alarm ); }


  //! Sets the verbose flag, meaning that load/save models should show
  //! CPU and elapsed times
  void SetVerbose (const Standard_Boolean isVerbose)
  { myIsVerbose = isVerbose; }

  //! Returns the verbose flag
  Standard_Boolean IsVerbose () const
  { return myIsVerbose; }

 public:
  /** 
  * Redefined OCAF methods
  */

  //! Return name of resource (i.e. "TObj")
  virtual Standard_EXPORT Standard_CString ResourcesName();

  //! Return format (i.e "TObj")
  virtual Standard_EXPORT void Formats(TColStd_SequenceOfExtendedString& theFormats);

  //! Defines message driver for CDM layer
  virtual Standard_EXPORT Handle(CDM_MessageDriver) MessageDriver()
  { return myMessageDriver; }

 protected:
  /**
  * Constructor
  */

  //! Constructor is protected. Use method GetInstance() method to obtain 
  //! the static instance of the object (or derive your own application)
  Standard_EXPORT TObj_Application();

 private:
  /**
  * Fields
  */

  Standard_Boolean          myIsError;       //!< error flag
  Standard_Boolean          myIsVerbose;     //!< verbose flag
  Handle(Message_Messenger) myMessenger;     //!< messenger
  Handle(CDM_MessageDriver) myMessageDriver; //!< message driver

 public:
  //! CASCADE RTTI
  DEFINE_STANDARD_RTTI(TObj_Application)
};


//! Define handle class 
DEFINE_STANDARD_HANDLE(TObj_Application,TDocStd_Application)

#endif

#ifdef _MSC_VER
#pragma once
#endif
