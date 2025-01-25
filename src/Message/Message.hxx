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

#include <Message_Messenger.hxx>
#include <Message_Gravity.hxx>
#include <Message_MetricType.hxx>
#include <NCollection_Vector.hxx>
#include <OSD_MemInfo.hxx>

#include <TCollection_AsciiString.hxx>

class Message_Report;

//! Defines
//! - tools to work with messages
//! - basic tools intended for progress indication
class Message
{
public:
  DEFINE_STANDARD_ALLOC

  //! Defines default messenger for OCCT applications.
  //! This is global static instance of the messenger.
  //! By default, it contains single printer directed to std::cout.
  //! It can be customized according to the application needs.
  //!
  //! The following syntax can be used to print messages:
  //! @code
  //!   Message::DefaultMessenger()->Send ("My Warning", Message_Warning);
  //!   Message::SendWarning ("My Warning"); // short-cut for Message_Warning
  //!   Message::SendWarning() << "My Warning with " << theCounter << " arguments";
  //!   Message::SendFail ("My Failure"); // short-cut for Message_Fail
  //! @endcode
  Standard_EXPORT static const Handle(Message_Messenger)& DefaultMessenger();

public:
  //!@name Short-cuts to DefaultMessenger

  static Message_Messenger::StreamBuffer Send(Message_Gravity theGravity)
  {
    return DefaultMessenger()->Send(theGravity);
  }

  static void Send(const TCollection_AsciiString& theMessage, Message_Gravity theGravity)
  {
    DefaultMessenger()->Send(theMessage, theGravity);
  }

  static Message_Messenger::StreamBuffer SendFail() { return DefaultMessenger()->SendFail(); }

  static Message_Messenger::StreamBuffer SendAlarm() { return DefaultMessenger()->SendAlarm(); }

  static Message_Messenger::StreamBuffer SendWarning() { return DefaultMessenger()->SendWarning(); }

  static Message_Messenger::StreamBuffer SendInfo() { return DefaultMessenger()->SendInfo(); }

  static Message_Messenger::StreamBuffer SendTrace() { return DefaultMessenger()->SendTrace(); }

  static void SendFail(const TCollection_AsciiString& theMessage)
  {
    return DefaultMessenger()->SendFail(theMessage);
  }

  static void SendAlarm(const TCollection_AsciiString& theMessage)
  {
    return DefaultMessenger()->SendAlarm(theMessage);
  }

  static void SendWarning(const TCollection_AsciiString& theMessage)
  {
    return DefaultMessenger()->SendWarning(theMessage);
  }

  static void SendInfo(const TCollection_AsciiString& theMessage)
  {
    return DefaultMessenger()->SendInfo(theMessage);
  }

  static void SendTrace(const TCollection_AsciiString& theMessage)
  {
    return DefaultMessenger()->SendTrace(theMessage);
  }

public:
  //! Returns the string filled with values of hours, minutes and seconds.
  //! Example:
  //! 1. (5, 12, 26.3345) returns "05h:12m:26.33s",
  //! 2. (0,  6, 34.496 ) returns "06m:34.50s",
  //! 3. (0,  0,  4.5   ) returns "4.50s"
  Standard_EXPORT static TCollection_AsciiString FillTime(const Standard_Integer Hour,
                                                          const Standard_Integer Minute,
                                                          const Standard_Real    Second);

public:
  //! returns the only one instance of Report
  //! When theToCreate is true - automatically creates message report when not exist.
  Standard_EXPORT static const Handle(Message_Report)& DefaultReport(
    const Standard_Boolean theToCreate = Standard_False);

  //! Determines the metric from the given string identifier.
  //! @param theString string identifier
  //! @param theType detected type of metric
  //! @return TRUE if string identifier is known
  Standard_EXPORT static Standard_Boolean MetricFromString(const Standard_CString theString,
                                                           Message_MetricType&    theType);

  //! Returns the string name for a given metric type.
  //! @param theType metric type
  //! @return string identifier from the list of Message_MetricType
  Standard_EXPORT static Standard_CString MetricToString(const Message_MetricType theType);

  //! Returns the metric type from the given string identifier.
  //! @param theString string identifier
  //! @return metric type or Message_MetricType_None if string identifier is invalid
  static Message_MetricType MetricFromString(const Standard_CString theString)
  {
    Message_MetricType aMetric = Message_MetricType_None;
    MetricFromString(theString, aMetric);
    return aMetric;
  }

  //! Converts message metric to OSD memory info type.
  //! @param[in] theMetric  message metric
  //! @param[out] theMemInfo  filled memory info type
  //! @return true if converted
  static Standard_EXPORT Standard_Boolean ToOSDMetric(const Message_MetricType theMetric,
                                                      OSD_MemInfo::Counter&    theMemInfo);

  //! Converts OSD memory info type to message metric.
  //! @param theMemInfo [int] memory info type
  //! @param[out] theMetric  filled message metric
  //! @return true if converted
  static Standard_EXPORT Standard_Boolean ToMessageMetric(const OSD_MemInfo::Counter theMemInfo,
                                                          Message_MetricType&        theMetric);
};

#endif // _Message_HeaderFile
