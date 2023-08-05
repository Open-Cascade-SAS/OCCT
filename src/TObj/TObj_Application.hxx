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

#ifndef TObj_Application_HeaderFile
#define TObj_Application_HeaderFile

#include <TDocStd_Application.hxx>
#include <Message_Gravity.hxx>
#include <Message_Messenger.hxx>

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
  Handle(Message_Messenger) &Messenger() { return myMessenger; }
    
 public:
  /**
  *    Load/Save support
  */

  //! Saving the OCAF document to a file
  Standard_EXPORT virtual Standard_Boolean SaveDocument
                        (const Handle(TDocStd_Document)&   theSourceDoc,
                         const TCollection_ExtendedString& theTargetFile);

  //! Saving the OCAF document to a stream
  Standard_EXPORT virtual Standard_Boolean SaveDocument
                        (const Handle(TDocStd_Document)&   theSourceDoc,
                         Standard_OStream&                 theOStream);

  //! Loading the OCAF document from a file
  Standard_EXPORT virtual Standard_Boolean LoadDocument
                         (const TCollection_ExtendedString& theSourceFile,
                          Handle(TDocStd_Document)&         theTargetDoc);

  //! Loading the OCAF document from a stream
  Standard_EXPORT virtual Standard_Boolean LoadDocument
                        (Standard_IStream&                 theIStream,
                         Handle(TDocStd_Document)&         theTargetDoc);

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
  virtual void ErrorMessage
                        (const TCollection_ExtendedString &theMsg)
  { ErrorMessage( theMsg, Message_Alarm ); }


  //! Sets the verbose flag, meaning that load/save models should show
  //! CPU and elapsed times
  void SetVerbose (const Standard_Boolean isVerbose)
  { myIsVerbose = isVerbose; }

  //! Returns the verbose flag
  Standard_Boolean IsVerbose () const
  { return myIsVerbose; }

  //! Dumps the content of me into the stream
  Standard_EXPORT void DumpJson (Standard_OStream& theOStream, Standard_Integer theDepth = -1) const;

 public:
  /** 
  * Redefined OCAF methods
  */

  //! Return name of resource (i.e. "TObj")
  virtual Standard_EXPORT Standard_CString ResourcesName() Standard_OVERRIDE;

 protected:
  /**
  * Constructor
  */

  //! Constructor is protected. Use method GetInstance() method to obtain 
  //! the static instance of the object (or derive your own application)
  Standard_EXPORT TObj_Application();

  //! Sets an error occurred on storage of a document.
  void SetError (const PCDM_StoreStatus theStatus, const TCollection_ExtendedString& theInfo);

  //! Sets an error occurred on reading of a document.
  void SetError (const PCDM_ReaderStatus theStatus, const TCollection_ExtendedString& theInfo);

 private:
  /**
  * Fields
  */

  Standard_Boolean          myIsError;       //!< error flag
  Standard_Boolean          myIsVerbose;     //!< verbose flag
  Handle(Message_Messenger) myMessenger;     //!< messenger

 public:
  //! CASCADE RTTI
  DEFINE_STANDARD_RTTIEXT(TObj_Application,TDocStd_Application)
};


//! Define handle class 
DEFINE_STANDARD_HANDLE(TObj_Application,TDocStd_Application)

#endif
