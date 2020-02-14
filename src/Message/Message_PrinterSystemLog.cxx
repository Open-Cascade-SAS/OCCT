// Copyright (c) 2019 OPEN CASCADE SAS
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

#ifdef _WIN32
  #include <windows.h>
#endif

#include <Message_PrinterSystemLog.hxx>

#include <TCollection_ExtendedString.hxx>

#if defined(OCCT_UWP)
  //
#elif defined(_WIN32)
  //! Convert message gravity into EventLog enumeration.
  static WORD getEventLogPriority (const Message_Gravity theGravity)
  {
    switch (theGravity)
    {
      case Message_Alarm:
      case Message_Fail:
        return EVENTLOG_ERROR_TYPE;
      case Message_Warning:
        return EVENTLOG_WARNING_TYPE;
      case Message_Info:
      case Message_Trace:
        return EVENTLOG_INFORMATION_TYPE;
    }
    return EVENTLOG_INFORMATION_TYPE;
  }
#elif defined(__ANDROID__)
  #include <android/log.h>

  //! Convert message gravity into Android log enumeration.
  static android_LogPriority getAndroidLogPriority (const Message_Gravity theGravity)
  {
    switch (theGravity)
    {
      case Message_Trace:   return ANDROID_LOG_DEBUG;
      case Message_Info:    return ANDROID_LOG_INFO;
      case Message_Warning: return ANDROID_LOG_WARN;
      case Message_Alarm:   return ANDROID_LOG_ERROR;
      case Message_Fail:    return ANDROID_LOG_ERROR;
    }
    return ANDROID_LOG_DEBUG;
  }
#elif defined(__EMSCRIPTEN__)
  #include <emscripten/emscripten.h>

  // actual version of Emscripten does not define these yet
  #ifndef EM_LOG_INFO
    #define EM_LOG_INFO 0
  #endif
  #ifndef EM_LOG_DEBUG
    #define EM_LOG_DEBUG 0
  #endif

  //! Convert message gravity into emscripten_log() flags.
  static int getEmscriptenPriority (const Message_Gravity theGravity)
  {
    switch (theGravity)
    {
      case Message_Trace:   return EM_LOG_CONSOLE | EM_LOG_DEBUG;
      case Message_Info:    return EM_LOG_CONSOLE | EM_LOG_INFO;
      case Message_Warning: return EM_LOG_CONSOLE | EM_LOG_WARN;
      case Message_Alarm:   return EM_LOG_CONSOLE | EM_LOG_ERROR;
      case Message_Fail:    return EM_LOG_CONSOLE | EM_LOG_ERROR;
    }
    return EM_LOG_CONSOLE;
  }

  //! Print message to console.debug().
  EM_JS(void, debugMsgToConsole, (const char* theStr), {
    console.debug(UTF8ToString(theStr));
  });
#else
  #include <syslog.h>

  //! Convert message gravity into syslog() enumeration.
  static int getSysLogPriority (const Message_Gravity theGravity)
  {
    switch (theGravity)
    {
      case Message_Trace:   return LOG_DEBUG;
      case Message_Info:    return LOG_INFO;
      case Message_Warning: return LOG_WARNING;
      case Message_Alarm:   return LOG_ERR;
      case Message_Fail:    return LOG_ERR;
    }
    return LOG_DEBUG;
  }
#endif

IMPLEMENT_STANDARD_RTTIEXT(Message_PrinterSystemLog, Message_Printer)

//=======================================================================
//function : Constructor
//purpose  :
//=======================================================================
Message_PrinterSystemLog::Message_PrinterSystemLog (const TCollection_AsciiString& theEventSourceName,
                                                    const Message_Gravity theTraceLevel)
: myEventSourceName (theEventSourceName)
{
  myTraceLevel = theTraceLevel;
#if defined(OCCT_UWP)
  myEventSource = NULL;
#elif defined(_WIN32)
  const TCollection_ExtendedString aWideSrcName (theEventSourceName);
  myEventSource = (Standard_Address )RegisterEventSourceW (NULL, aWideSrcName.ToWideString());
#elif defined(__ANDROID__)
  //
#elif defined(__EMSCRIPTEN__)
  //
#else
  openlog (myEventSourceName.ToCString(), LOG_PID | LOG_NDELAY, LOG_USER);
#endif
}

//=======================================================================
//function : ~Message_PrinterSystemLog
//purpose  :
//=======================================================================
Message_PrinterSystemLog::~Message_PrinterSystemLog()
{
#if defined(_WIN32)
  if (myEventSource != NULL)
  {
  #if !defined(OCCT_UWP)
    DeregisterEventSource ((HANDLE )myEventSource);
  #endif
  }
#elif defined(__ANDROID__)
  //
#elif defined(__EMSCRIPTEN__)
  //
#else
  closelog();
#endif
}

//=======================================================================
//function : Send
//purpose  :
//=======================================================================
void Message_PrinterSystemLog::Send (const Standard_CString theString,
                                     const Message_Gravity theGravity,
                                     const Standard_Boolean ) const
{
  if (theGravity < myTraceLevel)
  {
    return;
  }

#if defined(_WIN32)
  Send (TCollection_ExtendedString (theString), theGravity, true);
#elif defined(__ANDROID__)
  __android_log_write (getAndroidLogPriority (theGravity), myEventSourceName.ToCString(), theString);
#elif defined(__EMSCRIPTEN__)
  if (theGravity == Message_Trace)
  {
    debugMsgToConsole (theString);
  }
  else
  {
    emscripten_log (getEmscriptenPriority (theGravity), "%s", theString);
  }
#else
  syslog (getSysLogPriority (theGravity), "%s", theString);
#endif
}

//=======================================================================
//function : Send
//purpose  :
//=======================================================================
void Message_PrinterSystemLog::Send (const TCollection_AsciiString& theString,
                                     const Message_Gravity theGravity,
                                     const Standard_Boolean ) const
{
  if (theGravity < myTraceLevel)
  {
    return;
  }

#if defined(_WIN32)
  Send (TCollection_ExtendedString (theString), theGravity, true);
#elif defined(__ANDROID__)
  __android_log_write (getAndroidLogPriority (theGravity), myEventSourceName.ToCString(), theString.ToCString());
#elif defined(__EMSCRIPTEN__)
  if (theGravity == Message_Trace)
  {
    debugMsgToConsole (theString.ToCString());
  }
  else
  {
    emscripten_log (getEmscriptenPriority (theGravity), "%s", theString.ToCString());
  }
#else
  syslog (getSysLogPriority (theGravity), "%s", theString.ToCString());
#endif
}

//=======================================================================
//function : Send
//purpose  :
//=======================================================================
void Message_PrinterSystemLog::Send (const TCollection_ExtendedString& theString,
                                     const Message_Gravity theGravity,
                                     const Standard_Boolean ) const
{
  if (theGravity < myTraceLevel)
  {
    return;
  }

#if defined(_WIN32)
  if (myEventSource != NULL)
  {
  #if !defined(OCCT_UWP)
    const WORD aLogType = getEventLogPriority (theGravity);
    const wchar_t* aMessage[1] = { theString.ToWideString() };
    ReportEventW ((HANDLE )myEventSource, aLogType, 0, 0, NULL,
                  1, 0, aMessage, NULL);
  #else
    (void )theString;
  #endif
  }
#else
  Send (TCollection_AsciiString (theString), theGravity, true);
#endif
}
