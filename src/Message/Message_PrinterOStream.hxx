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

#include <Message_Printer.hxx>
#include <Standard_Address.hxx>
#include <Standard_OStream.hxx>

class Message_PrinterOStream;
DEFINE_STANDARD_HANDLE(Message_PrinterOStream, Message_Printer)

//! Implementation of a message printer associated with an ostream
//! The ostream may be either externally defined one (e.g. cout),
//! or file stream maintained internally (depending on constructor).
class Message_PrinterOStream : public Message_Printer
{
  DEFINE_STANDARD_RTTIEXT(Message_PrinterOStream, Message_Printer)
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

  //! Returns option to convert non-Ascii symbols to UTF8 encoding
  Standard_Boolean GetUseUtf8() const { return myUseUtf8; }
  
  //! Sets option to convert non-Ascii symbols to UTF8 encoding
  void SetUseUtf8 (const Standard_Boolean useUtf8) { myUseUtf8 = useUtf8; }

  //! Returns reference to the output stream
  Standard_OStream& GetStream() const { return *(Standard_OStream*)myStream; }
  
  //! Puts a message to the current stream
  //! if its gravity is equal or greater
  //! to the trace level set by SetTraceLevel()
  Standard_EXPORT virtual void Send (const Standard_CString theString, const Message_Gravity theGravity, const Standard_Boolean putEndl = Standard_True) const Standard_OVERRIDE;
  
  //! Puts a message to the current stream
  //! if its gravity is equal or greater
  //! to the trace level set by SetTraceLevel()
  Standard_EXPORT virtual void Send (const TCollection_AsciiString& theString, const Message_Gravity theGravity, const Standard_Boolean putEndl = Standard_True) const Standard_OVERRIDE;
  
  //! Puts a message to the current stream
  //! if its gravity is equal or greater
  //! to the trace level set by SetTraceLevel()
  //! Non-Ascii symbols are converted to UTF-8 if UseUtf8
  //! option is set, else replaced by symbols '?'
  Standard_EXPORT virtual void Send (const TCollection_ExtendedString& theString, const Message_Gravity theGravity, const Standard_Boolean putEndl = Standard_True) const Standard_OVERRIDE;

private:

  Standard_Address myStream;
  Standard_Boolean myIsFile;
  Standard_Boolean myUseUtf8;

};

#endif // _Message_PrinterOStream_HeaderFile
