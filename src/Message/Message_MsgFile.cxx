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

#include <Message_MsgFile.hxx>

#include <NCollection_Buffer.hxx>
#include <NCollection_DataMap.hxx>
#include <OSD_Environment.hxx>
#include <TCollection_AsciiString.hxx>
#include <TCollection_ExtendedString.hxx>
#include <Standard_Mutex.hxx>
#include <OSD_OpenFile.hxx>

#include <stdlib.h>
#include <stdio.h>

typedef NCollection_DataMap<TCollection_AsciiString,TCollection_ExtendedString> Message_DataMapOfExtendedString;

static Message_DataMapOfExtendedString& msgsDataMap ()
{
  static Message_DataMapOfExtendedString aDataMap;
  return aDataMap;
}

// mutex used to prevent concurrent access to message registry
static Standard_Mutex theMutex;

typedef enum
{
  MsgFile_WaitingKeyword,
  MsgFile_WaitingMessage,
  MsgFile_WaitingMoreMessage,
  MsgFile_Indefinite
} LoadingState;

//=======================================================================
//function : Message_MsgFile
//purpose  : Load file from given directories
//           theDirName may be represented as list: "/dirA/dirB /dirA/dirC"
//=======================================================================

Standard_Boolean Message_MsgFile::Load (const Standard_CString theDirName,
					const Standard_CString theFileName)
{
  if ( ! theDirName || ! theFileName ) return Standard_False;

  Standard_Boolean ret = Standard_True;
  TCollection_AsciiString aDirList (theDirName);
  //  Try to load from all consecutive directories in list
  for (int i = 1;; i++)
  {
    TCollection_AsciiString aFileName = aDirList.Token (" \t\n", i);
    if (aFileName.IsEmpty()) break;
#ifdef _WIN32
    aFileName += '\\';
#else
    aFileName += '/';
#endif
    aFileName += theFileName;
    if ( ! LoadFile (aFileName.ToCString()) ) 
      ret = Standard_False;
  }
  return ret;
}

//=======================================================================
//function : getString
//purpose  : Takes a TCollection_ExtendedString from Ascii or Unicode
//           Strings are left-trimmed; those beginning with '!' are omitted
//Called   : from loadFile()
//=======================================================================

template <class _Char> static inline Standard_Boolean
getString (_Char *&                     thePtr,
           TCollection_ExtendedString&  theString,
           Standard_Integer&            theLeftSpaces)
{
  _Char * anEndPtr = thePtr;
  _Char * aPtr;
  Standard_Integer aLeftSpaces;

  do 
  {
    //    Skip whitespaces in the beginning of the string
    aPtr = anEndPtr;
    aLeftSpaces = 0;
    for (;;)
    {
      _Char aChar = * aPtr;
      if      (aChar == ' ')  aLeftSpaces++;
      else if (aChar == '\t') aLeftSpaces += 8;
      else if (aChar == '\r' || * aPtr == '\n') aLeftSpaces = 0;
      else break;
      aPtr++;
    }

    //    Find the end of the string
    for (anEndPtr = aPtr; * anEndPtr; anEndPtr++)
      if (anEndPtr[0] == '\n')
      {
        if (anEndPtr[-1] == '\r') anEndPtr--;
        break;
      }

  } while (aPtr[0] == '!');

  //    form the result
  if (aPtr == anEndPtr) return Standard_False;
  thePtr = anEndPtr;
  if (*thePtr)
    *thePtr++ = '\0';
  theString = TCollection_ExtendedString (aPtr);
  theLeftSpaces = aLeftSpaces;
  return Standard_True;
}

//=======================================================================
//function : loadFile
//purpose  : Static function, fills the DataMap of Messages from Ascii or Unicode
//Called   : from LoadFile()
//=======================================================================

template <class _Char> static inline Standard_Boolean loadFile (_Char * theBuffer)
{
  TCollection_AsciiString               aKeyword;
  TCollection_ExtendedString            aMessage, aString;
  LoadingState                          aState = MsgFile_WaitingKeyword;
  _Char                                 * sCurrentString = theBuffer;
  Standard_Integer                      aLeftSpaces=0, aFirstLeftSpaces = 0;

  //    Take strings one-by-one; comments already screened
  while (::getString (sCurrentString, aString, aLeftSpaces))
  {
    Standard_Boolean isKeyword = (aString.Value(1) == '.');
    switch (aState)
    {
    case MsgFile_WaitingMoreMessage:
      if (isKeyword)
        Message_MsgFile::AddMsg (aKeyword, aMessage); // terminate the previous one
        //      Pass from here to 'case MsgFile_WaitingKeyword'
      else
      {
        //      Add another line to the message already in the buffer 'aMessage'
        aMessage += '\n';
        aLeftSpaces -= aFirstLeftSpaces;
        if (aLeftSpaces > 0) aMessage += TCollection_ExtendedString (aLeftSpaces, ' ');
        aMessage += aString;
        break;
      }
    case MsgFile_WaitingMessage:
      if (isKeyword == Standard_False)
      {
        aMessage         = aString;
        aFirstLeftSpaces = aLeftSpaces;         // remember the starting position
        aState = MsgFile_WaitingMoreMessage;
        break;
      }
      //      Pass from here to 'case MsgFile_WaitingKeyword'
    case MsgFile_WaitingKeyword:
      if (isKeyword)
      {
        // remove the first dot character and all subsequent spaces + right-trim
        aKeyword = TCollection_AsciiString (aString.Split(1));
        aKeyword.LeftAdjust();
        aKeyword.RightAdjust();
        aState = MsgFile_WaitingMessage;
      }
      break;
    default:
      break;
    }
  }
  //    Process the last string still remaining in the buffer
  if (aState == MsgFile_WaitingMoreMessage)
    Message_MsgFile::AddMsg (aKeyword, aMessage);
  return Standard_True;
}

//=======================================================================
//function : GetFileSize
//purpose  : 
//=======================================================================

static Standard_Integer GetFileSize (FILE *theFile)
{
  if ( !theFile ) return -1;

  // get real file size
  long nRealFileSize = 0;
  if ( fseek(theFile, 0, SEEK_END) != 0 ) return -1;
  nRealFileSize = ftell(theFile);
  if ( fseek(theFile, 0, SEEK_SET) != 0 ) return -1;

  return (Standard_Integer) nRealFileSize;
}

//=======================================================================
//function : LoadFile
//purpose  : Load the list of messages from a file
//=======================================================================

Standard_Boolean Message_MsgFile::LoadFile (const Standard_CString theFileName)
{
  if (theFileName == NULL || * theFileName == '\0') return Standard_False;

  //    Open the file
  FILE *anMsgFile = OSD_OpenFile(theFileName,"rb");
  if (!anMsgFile)
    return Standard_False;

  const Standard_Integer aFileSize = GetFileSize (anMsgFile);
  NCollection_Buffer aBuffer(NCollection_BaseAllocator::CommonBaseAllocator());
  if (aFileSize <= 0 || !aBuffer.Allocate(aFileSize + 2))
  {
    fclose (anMsgFile);
    return Standard_False;
  }

  char* anMsgBuffer = reinterpret_cast<char*>(aBuffer.ChangeData());
  const Standard_Integer nbRead =
    static_cast<Standard_Integer>( fread(anMsgBuffer, 1, aFileSize, anMsgFile) );

  fclose (anMsgFile);
  if (nbRead != aFileSize)
    return Standard_False;

  anMsgBuffer[aFileSize] = 0;
  anMsgBuffer[aFileSize + 1] = 0;

  // Read the messages in the file and append them to the global DataMap
  Standard_Boolean isLittleEndian = (anMsgBuffer[0] == '\xff' && anMsgBuffer[1] == '\xfe');
  Standard_Boolean isBigEndian    = (anMsgBuffer[0] == '\xfe' && anMsgBuffer[1] == '\xff');
  if ( isLittleEndian || isBigEndian )
  {
    Standard_ExtCharacter* aUnicodeBuffer =
      reinterpret_cast<Standard_ExtCharacter*>(&anMsgBuffer[2]);
    // Convert Unicode representation to order adopted on current platform
#if defined(__sparc) && defined(__sun)
    if ( isLittleEndian ) 
#else
    if ( isBigEndian ) 
#endif
    {
      // Reverse the bytes throughout the buffer
      const Standard_ExtCharacter* const anEnd =
        reinterpret_cast<const Standard_ExtCharacter* const>(&anMsgBuffer[aFileSize]);

      for (Standard_ExtCharacter* aPtr = aUnicodeBuffer; aPtr < anEnd; aPtr++)
      {
        unsigned short aWord = *aPtr;
        *aPtr = (aWord & 0x00ff) << 8 | (aWord & 0xff00) >> 8;
      }
    }
    return ::loadFile (aUnicodeBuffer);
  }
  else
    return ::loadFile (anMsgBuffer);
}

//=======================================================================
//function : LoadFromEnv
//purpose  :
//=======================================================================
Standard_Boolean Message_MsgFile::LoadFromEnv (const Standard_CString theEnvName,
                                               const Standard_CString theFileName,
                                               const Standard_CString theLangExt)
{
  TCollection_AsciiString aLangExt (theLangExt != NULL ? theLangExt : "");
  if (aLangExt.IsEmpty())
  {
    OSD_Environment aLangEnv ("CSF_LANGUAGE");
    aLangExt = aLangEnv.Value();
    if (aLangExt.IsEmpty())
    {
      aLangExt = "us";
    }
  }

  TCollection_AsciiString aFilePath (theFileName);
  if (theEnvName != NULL
   && theEnvName[0] != '\0')
  {
    OSD_Environment aNameEnv (theEnvName);
    TCollection_AsciiString aNameEnvStr = aNameEnv.Value();
    if (!aNameEnvStr.IsEmpty())
    {
      if (aNameEnvStr.Value (aNameEnvStr.Length()) != '/')
      {
        aFilePath.Insert (1, '/');
      }
      aFilePath.Insert (1, aNameEnvStr);
    }
  }

  if (aLangExt.Value (1) != '.')
  {
    aFilePath.AssignCat ('.');
  }
  aFilePath.AssignCat (aLangExt);

  return Message_MsgFile::LoadFile (aFilePath.ToCString());
}

//=======================================================================
//function : LoadFromString
//purpose  :
//=======================================================================
Standard_Boolean Message_MsgFile::LoadFromString (const Standard_CString theContent,
                                                  const Standard_Integer theLength)
{
  Standard_Integer aStringSize = theLength >= 0 ? theLength : (Standard_Integer )strlen (theContent);
  NCollection_Buffer aBuffer (NCollection_BaseAllocator::CommonBaseAllocator());
  if (aStringSize <= 0 || !aBuffer.Allocate (aStringSize + 2))
  {
    return Standard_False;
  }

  memcpy (aBuffer.ChangeData(), theContent, aStringSize);
  aBuffer.ChangeData()[aStringSize + 0] = '\0';
  aBuffer.ChangeData()[aStringSize + 1] = '\0';
  char* anMsgBuffer = reinterpret_cast<char*>(aBuffer.ChangeData());
  return ::loadFile (anMsgBuffer);
}

//=======================================================================
//function : AddMsg
//purpose  : Add one message to the global table. Fails if the same keyword
//           already exists in the table
//=======================================================================

Standard_Boolean Message_MsgFile::AddMsg (const TCollection_AsciiString& theKeyword,
					  const TCollection_ExtendedString&  theMessage)
{
  Message_DataMapOfExtendedString& aDataMap = ::msgsDataMap();

  Standard_Mutex::Sentry aSentry (theMutex);
  aDataMap.Bind (theKeyword, theMessage);
  return Standard_True;
}

//=======================================================================
//function : getMsg
//purpose  : retrieve the message previously defined for the given keyword
//=======================================================================

const TCollection_ExtendedString &Message_MsgFile::Msg (const Standard_CString theKeyword)
{
  TCollection_AsciiString aKey((char*)theKeyword);
  return Msg (aKey);
} 

//=======================================================================
//function : HasMsg
//purpose  : 
//=======================================================================

Standard_Boolean Message_MsgFile::HasMsg (const TCollection_AsciiString& theKeyword)
{
  Standard_Mutex::Sentry aSentry (theMutex);
  return ::msgsDataMap().IsBound (theKeyword);
}

//=======================================================================
//function : Msg
//purpose  : retrieve the message previously defined for the given keyword
//=======================================================================

const TCollection_ExtendedString &Message_MsgFile::Msg (const TCollection_AsciiString& theKeyword)
{
  // find message in the map
  Message_DataMapOfExtendedString& aDataMap = ::msgsDataMap();
  Standard_Mutex::Sentry aSentry (theMutex);

  // if message is not found, generate error message and add it to the map to minimize overhead
  // on consequent calls with the same key
  if (! aDataMap.IsBound(theKeyword))
  {
    // text of the error message can be itself defined in the map
    static const TCollection_AsciiString aPrefixCode("Message_Msg_BadKeyword");
    static const TCollection_ExtendedString aDefPrefix("Unknown message invoked with the keyword ");
    TCollection_AsciiString aErrorMessage = (aDataMap.IsBound(aPrefixCode) ? aDataMap(aPrefixCode) : aDefPrefix);
    aErrorMessage += theKeyword;
    aDataMap.Bind (theKeyword, aErrorMessage); // do not use AddMsg() here to avoid mutex deadlock
  }

  return aDataMap (theKeyword);
}
