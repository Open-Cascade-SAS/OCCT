// File:      Message_MsgFile.cxx
// Created:   26.04.01 19:48:39
// Author:    OCC Team
// Copyright: Open CASCADE S.A. 2005

#include <Message_MsgFile.hxx>

#include <TCollection_AsciiString.hxx>
#include <TCollection_ExtendedString.hxx>
#include <NCollection_DefineBaseCollection.hxx>
#include <NCollection_DefineDataMap.hxx>
#include <OSD_Environment.hxx>
#include <stdlib.h>
#include <stdio.h>

DEFINE_BASECOLLECTION(Message_CollectionOfExtendedString, TCollection_ExtendedString)
DEFINE_DATAMAP(Message_DataMapOfExtendedString,
               Message_CollectionOfExtendedString,
               TCollection_AsciiString,
               TCollection_ExtendedString)

static Message_DataMapOfExtendedString& msgsDataMap ()
{
  static Message_DataMapOfExtendedString aDataMap;
  return aDataMap;
}

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
#ifdef WNT
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
    while (1)
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
  FILE *anMsgFile = fopen (theFileName, "rb");
  if (!anMsgFile) return Standard_False;

  //    Read the file into memory
  class Buffer
  {
    // self-destructing buffer
    char *myBuf;
  public:
    Buffer (Standard_Integer theSize) : myBuf(new char [theSize]) {}
    ~Buffer () { delete [] myBuf; }
    operator char* () const { return myBuf; }
    char& operator [] (Standard_Integer theInd) { return myBuf[theInd]; }
  };
  Standard_Integer aFileSize = GetFileSize (anMsgFile);
  if (aFileSize <= 0)
  {
    fclose (anMsgFile);
    return Standard_False;
  }
  Buffer anMsgBuffer (aFileSize + 2);
  Standard_Integer nbRead =
    (Standard_Integer) fread (anMsgBuffer, 1, aFileSize, anMsgFile);
  fclose (anMsgFile);
  if (nbRead != aFileSize)
    return Standard_False;
  anMsgBuffer[aFileSize] = 0;
  anMsgBuffer[aFileSize+1] = 0;

  // Read the messages in the file and append them to the global DataMap
  Standard_Boolean isLittleEndian = (anMsgBuffer[0] == '\xff' && anMsgBuffer[1] == '\xfe');
  Standard_Boolean isBigEndian    = (anMsgBuffer[0] == '\xfe' && anMsgBuffer[1] == '\xff');
  if ( isLittleEndian || isBigEndian )
  {
    Standard_ExtCharacter * aUnicodeBuffer =
      (Standard_ExtCharacter *) &anMsgBuffer[2];
    // Convert Unicode representation to order adopted on current platform
#if defined(__sparc) && defined(__sun)
    if ( isLittleEndian ) 
#else
    if ( isBigEndian ) 
#endif
    {
      // Reverse the bytes throughout the buffer
      for (Standard_ExtCharacter * aPtr = aUnicodeBuffer;
	   aPtr < (Standard_ExtCharacter *) &anMsgBuffer[aFileSize]; aPtr++)
      {
	unsigned short aWord = *aPtr;
	*aPtr = (aWord & 0x00ff) << 8 | (aWord & 0xff00) >> 8;
      }
    }
    return ::loadFile (aUnicodeBuffer);
  }
  else
    return ::loadFile ((char*) anMsgBuffer);
}

//=======================================================================
//function : LoadFromEnv
//purpose  : 
//=======================================================================
void  Message_MsgFile::LoadFromEnv
  (const Standard_CString envname,
   const Standard_CString filename,
   const Standard_CString ext)
{
  Standard_CString extname = ext;
  TCollection_AsciiString extstr;
  if (!extname || extname[0] == '\0') {
    OSD_Environment extenv("CSF_LANGUAGE");
    extstr  = extenv.Value();
    extname = extstr.ToCString();
  }
  if (!extname || extname[0] == '\0') extname = "us";

  TCollection_AsciiString filestr(filename);
  if (envname && envname[0] != '\0') {
    OSD_Environment envenv(envname);
    TCollection_AsciiString envstr  = envenv.Value();
    if (envstr.Length() > 0) {
      if (envstr.Value(envstr.Length()) != '/') filestr.Insert (1,'/');
      filestr.Insert (1,envstr.ToCString());
    }
  }
  if (extname[0] != '.') filestr.AssignCat ('.');
  filestr.AssignCat (extname);

  Message_MsgFile::LoadFile (filestr.ToCString());
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
//  if (aDataMap.IsBound (theKeyword))
//    return Standard_False;
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
//function : getMsg
//purpose  : retrieve the message previously defined for the given keyword
//=======================================================================

const TCollection_ExtendedString &Message_MsgFile::Msg (const TCollection_AsciiString& theKeyword)
{
  // find message in the map
  Message_DataMapOfExtendedString& aDataMap = ::msgsDataMap();
  if (aDataMap.IsBound (theKeyword))
    return aDataMap.Find (theKeyword);

  // if not found, generate error message
  static const TCollection_ExtendedString aDefPrefix ("Unknown message invoked with the keyword: ");
  static const TCollection_AsciiString aPrefixCode ("Message_Msg_BadKeyword");
  static TCollection_ExtendedString aFailureMessage;
  if (aDataMap.IsBound (aPrefixCode))
    aFailureMessage = aDataMap.Find (aPrefixCode) + " " + theKeyword;
  else aFailureMessage = aDefPrefix + theKeyword;
  return aFailureMessage;
}
