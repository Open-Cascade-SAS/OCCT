// Created on: 2001-01-06
// Created by: OCC Team
// Copyright (c) 2001-2014 OPEN CASCADE SAS
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

#include <Message_PrinterOStream.hxx>

#include <OSD_OpenFile.hxx>
#include <TCollection_AsciiString.hxx>
#include <TCollection_ExtendedString.hxx>

IMPLEMENT_STANDARD_RTTIEXT(Message_PrinterOStream,Message_Printer)

#if !defined(_MSC_VER)
  #include <strings.h>
#endif

//=======================================================================
//function : Constructor
//purpose  : Empty constructor, defaulting to cerr
//=======================================================================
Message_PrinterOStream::Message_PrinterOStream (const Message_Gravity theTraceLevel)
: myStream  (&std::cout),
  myIsFile  (Standard_False),
  myToColorize (Standard_True)
{
  myTraceLevel = theTraceLevel;
}

//=======================================================================
//function : Constructor
//purpose  : Opening a file as an std::ostream
//           for specific file names standard streams are created
//=======================================================================
Message_PrinterOStream::Message_PrinterOStream (const Standard_CString theFileName,
                                                const Standard_Boolean theToAppend,
                                                const Message_Gravity  theTraceLevel)
: myStream (&std::cout),
  myIsFile (Standard_False),
  myToColorize (Standard_True)
{
  myTraceLevel = theTraceLevel;
  if (strcasecmp(theFileName, "cerr") == 0)
  {
    myStream = &std::cerr;
    return;
  }
  else if (strcasecmp(theFileName, "cout") == 0)
  {
    myStream = &std::cout;
    return;
  }

  TCollection_AsciiString aFileName (theFileName);
#ifdef _WIN32
  aFileName.ChangeAll ('/', '\\');
#endif

  std::ofstream* aFile = new std::ofstream();
  OSD_OpenStream (*aFile, aFileName.ToCString(), (theToAppend ? (std::ios_base::app | std::ios_base::out) : std::ios_base::out));
  if (aFile->is_open())
  {
    myStream = (Standard_OStream* )aFile;
    myIsFile = Standard_True;
    myToColorize = Standard_False;
  }
  else
  {
    delete aFile;
    myStream = &std::cout;
#ifdef OCCT_DEBUG
    std::cerr << "Error opening " << theFileName << std::endl << std::flush;
#endif
  }
}

//=======================================================================
//function : Close
//purpose  : 
//=======================================================================

void Message_PrinterOStream::Close ()
{
  if ( ! myStream ) return;
  Standard_OStream* ostr = (Standard_OStream*)myStream;
  myStream = 0;

  ostr->flush();
  if ( myIsFile )
  {
    std::ofstream* ofile = (std::ofstream* )ostr;
    ofile->close();
    delete ofile;
    myIsFile = Standard_False;
  }
}

//=======================================================================
//function : send
//purpose  :
//=======================================================================
void Message_PrinterOStream::send (const TCollection_AsciiString& theString,
                                   const Message_Gravity theGravity) const
{
  if (theGravity < myTraceLevel
   || myStream == NULL)
  {
    return;
  }

  Message_ConsoleColor aColor = Message_ConsoleColor_Default;
  bool toIntense = false;
  if (myToColorize && !myIsFile)
  {
    switch(theGravity)
    {
      case Message_Trace:
        aColor = Message_ConsoleColor_Yellow;
        break;
      case Message_Info:
        aColor = Message_ConsoleColor_Green;
        toIntense = true;
        break;
      case Message_Warning:
        aColor = Message_ConsoleColor_Yellow;
        toIntense = true;
        break;
      case Message_Alarm:
        aColor = Message_ConsoleColor_Red;
        toIntense = true;
        break;
      case Message_Fail:
        aColor = Message_ConsoleColor_Red;
        toIntense = true;
        break;
    }
  }

  Standard_OStream* aStream = (Standard_OStream*)myStream;
  if (toIntense || aColor != Message_ConsoleColor_Default)
  {
    SetConsoleTextColor (aStream, aColor, toIntense);
    *aStream << theString;
    SetConsoleTextColor (aStream, Message_ConsoleColor_Default, false);
  }
  else
  {
    *aStream << theString;
  }
  (*aStream) << std::endl;
}

//=======================================================================
//function : SetConsoleTextColor
//purpose  :
//=======================================================================
void Message_PrinterOStream::SetConsoleTextColor (Standard_OStream* theOStream,
                                                  Message_ConsoleColor theTextColor,
                                                  bool theIsIntenseText)
{
#ifdef _WIN32
  // there is no difference between STD_OUTPUT_HANDLE/STD_ERROR_HANDLE for std::cout/std::cerr
  (void )theOStream;
  if (HANDLE anStdOut = GetStdHandle (STD_OUTPUT_HANDLE))
  {
    WORD aFlags = 0;
    if (theIsIntenseText)
    {
      aFlags |= FOREGROUND_INTENSITY;
    }
    switch (theTextColor)
    {
      case Message_ConsoleColor_Default:
      case Message_ConsoleColor_White:
        aFlags |= FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE;
        break;
      case Message_ConsoleColor_Black:
        break;
      case Message_ConsoleColor_Red:
        aFlags |= FOREGROUND_RED;
        break;
      case Message_ConsoleColor_Green:
        aFlags |= FOREGROUND_GREEN;
        break;
      case Message_ConsoleColor_Blue:
        aFlags |= FOREGROUND_BLUE;
        break;
      case Message_ConsoleColor_Yellow:
        aFlags |= FOREGROUND_RED | FOREGROUND_GREEN;
        break;
      case Message_ConsoleColor_Cyan:
        aFlags |= FOREGROUND_GREEN | FOREGROUND_BLUE;
        break;
      case Message_ConsoleColor_Magenta:
        aFlags |= FOREGROUND_RED | FOREGROUND_BLUE;
        break;
    }
    SetConsoleTextAttribute (anStdOut, aFlags);
  }
#elif defined(__EMSCRIPTEN__)
  // Terminal capabilities are undefined on this platform.
  // std::cout could be redirected to HTML page, into terminal or somewhere else.
  (void )theOStream;
  (void )theTextColor;
  (void )theIsIntenseText;
#else
  if (theOStream == NULL)
  {
    return;
  }

  const char* aCode = "\e[0m";
  switch (theTextColor)
  {
    case Message_ConsoleColor_Default:
      aCode = theIsIntenseText ? "\e[0;1m" : "\e[0m";
      break;
    case Message_ConsoleColor_Black:
      aCode = theIsIntenseText ? "\e[30;1m" : "\e[30m";
      break;
    case Message_ConsoleColor_Red:
      aCode = theIsIntenseText ? "\e[31;1m" : "\e[31m";
      break;
    case Message_ConsoleColor_Green:
      aCode = theIsIntenseText ? "\e[32;1m" : "\e[32m";
      break;
    case Message_ConsoleColor_Yellow:
      aCode = theIsIntenseText ? "\e[33;1m" : "\e[33m";
      break;
    case Message_ConsoleColor_Blue:
      aCode = theIsIntenseText ? "\e[34;1m" : "\e[34m";
      break;
    case Message_ConsoleColor_Magenta:
      aCode = theIsIntenseText ? "\e[35;1m" : "\e[35m";
      break;
    case Message_ConsoleColor_Cyan:
      aCode = theIsIntenseText ? "\e[36;1m" : "\e[36m";
      break;
    case Message_ConsoleColor_White:
      aCode = theIsIntenseText ? "\e[37;1m" : "\e[37m";
      break;
  }
  *theOStream << aCode;
#endif
}
