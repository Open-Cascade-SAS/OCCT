// Created on: 1999-07-29
// Created by: Roman LYGIN
// Copyright (c) 1999 Matra Datavision
// Copyright (c) 1999-2014 OPEN CASCADE SAS
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

#ifndef _Message_HeaderFile
#define _Message_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Integer.hxx>
#include <Standard_Real.hxx>
class Message_Messenger;
class TCollection_AsciiString;
class Message_Msg;
class Message_MsgFile;
class Message_Messenger;
class Message_Algorithm;
class Message_Printer;
class Message_PrinterOStream;
class Message_ProgressIndicator;
class Message_ProgressScale;
class Message_ProgressSentry;


//! Defines
//! - tools to work with messages
//! - basic tools intended for progress indication
class Message 
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! Defines default messenger for OCCT applications.
  //! This is global static instance of the messenger.
  //! By default, it contains single printer directed to cout.
  //! It can be customized according to the application needs.
  Standard_EXPORT static const Handle(Message_Messenger)& DefaultMessenger();
  
  //! Returns the string filled with values of hours, minutes and seconds.
  //! Example:
  //! 1. (5, 12, 26.3345) returns "05h:12m:26.33s",
  //! 2. (0,  6, 34.496 ) returns "06m:34.50s",
  //! 3. (0,  0,  4.5   ) returns "4.50s"
  Standard_EXPORT static TCollection_AsciiString FillTime (const Standard_Integer Hour, const Standard_Integer Minute, const Standard_Real Second);




protected:





private:




friend class Message_Msg;
friend class Message_MsgFile;
friend class Message_Messenger;
friend class Message_Algorithm;
friend class Message_Printer;
friend class Message_PrinterOStream;
friend class Message_ProgressIndicator;
friend class Message_ProgressScale;
friend class Message_ProgressSentry;

};







#endif // _Message_HeaderFile
