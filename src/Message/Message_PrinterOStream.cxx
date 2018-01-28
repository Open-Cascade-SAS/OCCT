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
  myUseUtf8 (Standard_False)
{
  myTraceLevel = theTraceLevel;
}

//=======================================================================
//function : Constructor
//purpose  : Opening a file as an ostream
//           for specific file names standard streams are created
//=======================================================================
Message_PrinterOStream::Message_PrinterOStream (const Standard_CString theFileName,
                                                const Standard_Boolean theToAppend,
                                                const Message_Gravity  theTraceLevel)
: myStream (&std::cout),
  myIsFile (Standard_False)
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
//function : Send
//purpose  : 
//=======================================================================

void Message_PrinterOStream::Send (const Standard_CString theString,
				   const Message_Gravity theGravity,
				   const Standard_Boolean putEndl) const
{
  if ( theGravity < myTraceLevel || ! myStream ) return;
  Standard_OStream* ostr = (Standard_OStream*)myStream;
  (*ostr) << theString;
  if ( putEndl ) (*ostr) << endl;
}

//=======================================================================
//function : Send
//purpose  : 
//=======================================================================

void Message_PrinterOStream::Send (const TCollection_AsciiString &theString,
				   const Message_Gravity theGravity,
				   const Standard_Boolean putEndl) const
{
  Send ( theString.ToCString(), theGravity, putEndl );
}

//=======================================================================
//function : Send
//purpose  : 
//=======================================================================

void Message_PrinterOStream::Send (const TCollection_ExtendedString &theString,
				   const Message_Gravity theGravity,
				   const Standard_Boolean putEndl) const
{
  TCollection_AsciiString aStr (theString, myUseUtf8 ? Standard_Character(0) : '?');
  Send (aStr.ToCString(), theGravity, putEndl);
}
