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

#ifndef _Message_PrinterOStream_HeaderFile
#define _Message_PrinterOStream_HeaderFile

#include <Message_ConsoleColor.hxx>
#include <Message_Printer.hxx>
#include <Standard_OStream.hxx>

class Message_PrinterOStream;
DEFINE_STANDARD_HANDLE(Message_PrinterOStream, Message_Printer)

//! Implementation of a message printer associated with an std::ostream
//! The std::ostream may be either externally defined one (e.g. std::cout),
//! or file stream maintained internally (depending on constructor).
class Message_PrinterOStream : public Message_Printer
{
  DEFINE_STANDARD_RTTIEXT(Message_PrinterOStream, Message_Printer)
public:

  //! Setup console text color.
  //!
  //! On Windows, this would affect active terminal color output.
  //! On other systems, this would put special terminal codes;
  //! the terminal should support these codes or them will appear in text otherwise.
  //! The same will happen when stream is redirected into text file.
  //!
  //! Beware that within multi-threaded environment inducing console colors
  //! might lead to colored text mixture due to concurrency.
  Standard_EXPORT static void SetConsoleTextColor (Standard_OStream* theOStream,
                                                   Message_ConsoleColor theTextColor,
                                                   bool theIsIntenseText = false);

public:
  
  //! Empty constructor, defaulting to cout
  Standard_EXPORT Message_PrinterOStream(const Message_Gravity theTraceLevel = Message_Info);
  
  //! Create printer for output to a specified file.
  //! The option theDoAppend specifies whether file should be
  //! appended or rewritten.
  //! For specific file names (cout, cerr) standard streams are used
  Standard_EXPORT Message_PrinterOStream(const Standard_CString theFileName, const Standard_Boolean theDoAppend, const Message_Gravity theTraceLevel = Message_Info);
  
  //! Flushes the output stream and destroys it if it has been
  //! specified externally with option doFree (or if it is internal
  //! file stream)
  Standard_EXPORT void Close();
~Message_PrinterOStream()
{
  Close();
}

  //! Returns reference to the output stream
  Standard_OStream& GetStream() const { return *(Standard_OStream*)myStream; }

  //! Returns TRUE if text output into console should be colorized depending on message gravity; TRUE by default.
  Standard_Boolean ToColorize() const { return myToColorize; }

  //! Set if text output into console should be colorized depending on message gravity.
  void SetToColorize (Standard_Boolean theToColorize) { myToColorize = theToColorize; }

protected:

  //! Puts a message to the current stream
  //! if its gravity is equal or greater
  //! to the trace level set by SetTraceLevel()
  Standard_EXPORT virtual void send (const TCollection_AsciiString& theString, const Message_Gravity theGravity) const Standard_OVERRIDE;

private:

  Standard_Address myStream;
  Standard_Boolean myIsFile;
  Standard_Boolean myToColorize;

};

#endif // _Message_PrinterOStream_HeaderFile
