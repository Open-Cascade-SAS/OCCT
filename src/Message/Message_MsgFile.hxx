// Created on: 2001-04-26
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

#ifndef _Message_MsgFile_HeaderFile
#define _Message_MsgFile_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Boolean.hxx>
#include <Standard_CString.hxx>
class TCollection_AsciiString;
class TCollection_ExtendedString;

//! A tool providing facility to load definitions of message strings from
//! resource file(s).
//!
//! The message file is an ASCII file which defines a set of messages.
//! Each message is identified by its keyword (string).
//!
//! All lines in the file starting with the exclamation sign
//! (perhaps preceeding by spaces and/or tabs) are ignored as comments.
//!
//! Each line in the file starting with the dot character "."
//! (perhaps preceeding by spaces and/or tabs) defines the keyword.
//! The keyword is a string starting from the next symbol after dot
//! and ending at the symbol preceeding ending newline character "\n".
//!
//! All the lines in the file after the keyword and before next
//! keyword (and which are not comments) define the message for that
//! keyword. If the message consists of several lines, the message
//! string will contain newline symbols "\n" between parts (but not
//! at the end).
//!
//! The experimental support of Unicode message files is provided.
//! These are distinguished by two bytes FF.FE or FE.FF at the beginning.
//!
//! The loaded messages are stored in static data map; all methods of that
//! class are also static.
class Message_MsgFile 
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! Load message file <theFileName> from directory <theDirName>
  //! or its sub-directory
  Standard_EXPORT static Standard_Boolean Load (const Standard_CString theDirName, const Standard_CString theFileName);
  
  //! Load the messages from the given file, additive to any previously
  //! loaded messages. Messages with same keywords, if already present,
  //! are replaced with the new ones.
  Standard_EXPORT static Standard_Boolean LoadFile (const Standard_CString theFName);
  
  //! Loads the messages from the file with name (without extension) given by environment variable.
  //! Extension of the file name is given separately. If its not defined, it is taken:
  //! - by default from environment CSF_LANGUAGE,
  //! - if not defined either, as "us".
  //! @name theEnvName  environment variable name
  //! @name theFileName file name without language suffix
  //! @name theLangExt  language file name extension
  //! @return TRUE on success
  Standard_EXPORT static Standard_Boolean LoadFromEnv (const Standard_CString theEnvName,
                                                       const Standard_CString theFileName,
                                                       const Standard_CString theLangExt = "");

  //! Loads the messages from the given text buffer.
  //! @param theContent string containing the messages
  //! @param theLength  length of the buffer;
  //!                   when -1 specified - theContent will be considered as NULL-terminated string
  Standard_EXPORT static Standard_Boolean LoadFromString (const Standard_CString theContent,
                                                          const Standard_Integer theLength = -1);

  //! Adds new message to the map. Parameter <key> gives
  //! the key of the message, <text> defines the message itself.
  //! If there already was defined the message identified by the
  //! same keyword, it is replaced with the new one.
  Standard_EXPORT static Standard_Boolean AddMsg (const TCollection_AsciiString& key, const TCollection_ExtendedString& text);
  
  //! Returns True if message with specified keyword is registered
  Standard_EXPORT static Standard_Boolean HasMsg (const TCollection_AsciiString& key);
  
  Standard_EXPORT static const TCollection_ExtendedString& Msg (const Standard_CString key);
  
  //! Gives the text for the message identified by the keyword <key>
  //! If there are no messages with such keyword defined,
  //! the error message is returned.
  //! In that case reference to static string is returned, it can
  //! be chenged with next call(s) to Msg().
  //! Note: The error message is constructed like 'Unknown message: <key>', and can
  //! itself be customized by defining message with key Message_Msg_BadKeyword.
  Standard_EXPORT static const TCollection_ExtendedString& Msg (const TCollection_AsciiString& key);




protected:





private:





};







#endif // _Message_MsgFile_HeaderFile
