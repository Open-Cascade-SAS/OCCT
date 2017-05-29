// Created on: 2017-06-26
// Created by: Andrey Betenev
// Copyright (c) 2017 OPEN CASCADE SAS
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

#ifndef _Message_Alert_HeaderFile
#define _Message_Alert_HeaderFile

#include <Standard_Type.hxx>

class Message_Alert;
DEFINE_STANDARD_HANDLE(Message_Alert, MMgt_TShared)

//! Base class of the hierarchy of classes describing various situations
//! occurring during execution of some algorithm or procedure.
//!
//! Alert should provide unique text identifier that can be used to distinguish 
//! particular type of alerts, e.g. to get text message string describing it. 
//! See method GetMessageKey(); by default, dynamic type name is used.
//!
//! Alert can contain some data. To avoid duplication of data, new alert
//! can be merged with another one of the same type. Method SupportsMerge() 
//! should return true if merge is supported; method Merge() should do the
//! merge if possible and return true in that case and false otherwise.
//! 
class Message_Alert : public Standard_Transient
{
public:

  //! Return a C string to be used as a key for generating text user 
  //! messages describing this alert.
  //! The messages are generated with help of Message_Msg class, in
  //! Message_Report::Dump().
  //! Base implementation returns dynamic type name of the instance.
  virtual Standard_EXPORT Standard_CString GetMessageKey () const;
  
  //! Return true if this type of alert can be merged with other
  //! of the same type to avoid duplication.
  //! Basis implementation returns true.
  virtual Standard_EXPORT Standard_Boolean SupportsMerge () const;
  
  //! If possible, merge data contained in this alert to theTarget.
  //! @return True if merged.
  //! Base implementation always returns true.
  virtual Standard_EXPORT Standard_Boolean Merge (const Handle(Message_Alert)& theTarget);
  
  // OCCT RTTI
  DEFINE_STANDARD_RTTIEXT(Message_Alert,Standard_Transient)
};

//! Macro allowing to define simple alert (without data) in single line of code
#define DEFINE_SIMPLE_ALERT(Alert) class Alert : public Message_Alert { DEFINE_STANDARD_RTTI_INLINE(Alert,Message_Alert) };

#endif // _Message_Alert_HeaderFile
