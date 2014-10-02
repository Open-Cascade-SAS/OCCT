// Created on: 2001-04-27
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

#include <Message_Msg.hxx>
#include <Message_MsgFile.hxx>
#include <TCollection_AsciiString.hxx>
#include <stdio.h>

typedef enum
{
  Msg_IntegerType,
  Msg_RealType,
  Msg_StringType,
  Msg_IndefiniteType
} FormatType;

//=======================================================================
//function : Message_Msg()
//purpose  : Constructor
//=======================================================================

Message_Msg::Message_Msg ()
{}


//=======================================================================
//function : Message_Msg()
//purpose  : Constructor
//=======================================================================

Message_Msg::Message_Msg (const Message_Msg& theMsg)
{
  myMessageBody = theMsg.myMessageBody;
  myOriginal = theMsg.myOriginal;
  for ( Standard_Integer i = 1, n = theMsg.mySeqOfFormats.Length(); i <=n; i++ )
    mySeqOfFormats.Append ( theMsg.mySeqOfFormats.Value(i) );
}

//=======================================================================
//function : Message_Msg()
//purpose  : Constructor
//=======================================================================

Message_Msg::Message_Msg (const Standard_CString theMsgCode)
{
  TCollection_AsciiString aKey((char*)theMsgCode);
  Set ( Message_MsgFile::Msg(aKey) );
}

//=======================================================================
//function : Message_Msg()
//purpose  : Constructor
//=======================================================================

Message_Msg::Message_Msg (const TCollection_ExtendedString& theMsgCode)
{
  Set ( Message_MsgFile::Msg(theMsgCode) );
}

//=======================================================================
//function : Set
//purpose  : 
//=======================================================================

void Message_Msg::Set (const Standard_CString theMsg)
{
  TCollection_AsciiString aMsg((char*)theMsg);
  Set ( aMsg );
}

//=======================================================================
//function : Set
//purpose  : 
//=======================================================================

void Message_Msg::Set (const TCollection_ExtendedString& theMsg)
{
  myMessageBody = theMsg;

  const Standard_ExtString anExtString = myMessageBody.ToExtString();
        Standard_Integer   anMsgLength = myMessageBody.Length();
  for (Standard_Integer i = 0; i < anMsgLength; i++)
  {
    //  Search for '%' character starting a format specification
    if (ToCharacter (anExtString[i]) == '%')
    {
      Standard_Integer   aStart = i++;
      Standard_Character aChar = ToCharacter (anExtString[i]);
      //        Check for format '%%'
      if (aChar == '%')
      {
        myMessageBody.Remove (i+1);
        if (i >= --anMsgLength) break;
        aChar = ToCharacter (anExtString[i]);
      }
      //        Skip flags, field width and precision
      while (i < anMsgLength)
      {
        if (aChar == '-' || aChar == '+' || aChar == ' ' ||
            aChar == '#' || (aChar >= '0' && aChar <= '9') || aChar == '.')
          i++;
        else break;
        aChar = ToCharacter (anExtString[i]);
      }
      if (i >= anMsgLength) break;

      FormatType aFormatType;
      if (aChar == 'h' || aChar == 'l') aChar = ToCharacter (anExtString[++i]);
      switch (aChar)                    // detect the type of format spec
      {
      case 'd':
      case 'i':
      case 'o':
      case 'u':
      case 'x':
      case 'X':
        aFormatType = Msg_IntegerType;
        break;
      case 'f':
      case 'e':
      case 'E':
      case 'g':
      case 'G':
        aFormatType = Msg_RealType;
        break;
      case 's':
        aFormatType = Msg_StringType;
        break;
      default:
        aFormatType = Msg_IndefiniteType;
        continue;
      }
      mySeqOfFormats.Append (Standard_Integer(aFormatType));  // type
      mySeqOfFormats.Append (aStart);                         // beginning pos
      mySeqOfFormats.Append (i + 1 - aStart);                 // length
    }
  }
  myOriginal = myMessageBody;
}

//=======================================================================
//function : Arg (Standard_CString)
//purpose  : 
//=======================================================================

Message_Msg& Message_Msg::Arg (const Standard_CString theString)
{
  // get location and format
  TCollection_AsciiString aFormat;
  Standard_Integer aFirst = getFormat ( Msg_StringType, aFormat );
  if ( !aFirst )
    return *this;

  // print string according to format
  char * sStringBuffer = new char [Max ((Standard_Integer)strlen(theString)+1, 1024)];
  Sprintf (sStringBuffer, aFormat.ToCString(), theString);
  TCollection_ExtendedString aStr ( sStringBuffer, Standard_True );
  delete [] sStringBuffer;
  sStringBuffer = 0;

  // replace the format placeholder by the actual string
  replaceText ( aFirst, aFormat.Length(), aStr );
  
  return *this;
}

//=======================================================================
//function : Arg (TCollection_ExtendedString)
//purpose  : 
//remark   : This type of string is inserted without conversion (i.e. like %s)
//=======================================================================

Message_Msg& Message_Msg::Arg (const TCollection_ExtendedString& theString)
{
  // get location and format
  TCollection_AsciiString aFormat;
  Standard_Integer aFirst = getFormat ( Msg_StringType, aFormat );
  if ( !aFirst )
    return *this;
  
  // replace the format placeholder by the actual string
  replaceText ( aFirst, aFormat.Length(), theString );
  
  return *this;
}

//=======================================================================
//function : Arg (Standard_Integer)
//purpose  : 
//=======================================================================

Message_Msg& Message_Msg::Arg (const Standard_Integer theValue)
{
  // get location and format
  TCollection_AsciiString aFormat;
  Standard_Integer aFirst = getFormat ( Msg_IntegerType, aFormat );
  if ( !aFirst )
    return *this;

  // print string according to format
  char sStringBuffer [64];
  Sprintf (sStringBuffer, aFormat.ToCString(), theValue);
  TCollection_ExtendedString aStr ( sStringBuffer );

  // replace the format placeholder by the actual string
  replaceText ( aFirst, aFormat.Length(), aStr );
  
  return *this;
}

//=======================================================================
//function : Arg (Standard_Real)
//purpose  : 
//=======================================================================

Message_Msg& Message_Msg::Arg (const Standard_Real theValue)
{
  // get location and format
  TCollection_AsciiString aFormat;
  Standard_Integer aFirst = getFormat ( Msg_RealType, aFormat );
  if ( !aFirst )
    return *this;

  // print string according to format
  char sStringBuffer [64];
  Sprintf (sStringBuffer, aFormat.ToCString(), theValue);
  TCollection_ExtendedString aStr ( sStringBuffer );

  // replace the format placeholder by the actual string
  replaceText ( aFirst, aFormat.Length(), aStr );
  
  return *this;
}

//=======================================================================
//function : Get
//purpose  : used when the message is dispatched in Message_Messenger
//=======================================================================

const TCollection_ExtendedString& Message_Msg::Get ()
{
  // remove all non-initialised format specifications
  Standard_Integer i, anIncrement = 0;
  static const TCollection_ExtendedString anUnknown ("UNKNOWN");
  for (i = 1; i < mySeqOfFormats.Length(); i += 3)
  {
    TCollection_ExtendedString aRightPart =
      myMessageBody.Split(mySeqOfFormats(i+1) + anIncrement);
    aRightPart.Remove(1, mySeqOfFormats(i+2));
    myMessageBody += anUnknown;
    myMessageBody += aRightPart;
    anIncrement += (anUnknown.Length() - mySeqOfFormats(i+2));
  }
  return myMessageBody;
}

//=======================================================================
//function : getFormat
//purpose  : Find placeholder in the string where to put next value of 
//           specified type, return its starting position in the string 
//           and relevant format string (located at that position).
//           The information on returned placeholder is deleted immediately,
//           so it will not be found further
//           If failed (no placeholder with relevant type found), returns 0
//=======================================================================

Standard_Integer Message_Msg::getFormat (const Standard_Integer theType,
                                         TCollection_AsciiString &theFormat)
{
  for (Standard_Integer i = 1; i <= mySeqOfFormats.Length(); i += 3)
    if (mySeqOfFormats(i) == theType)
    {
      // Extract format
      Standard_Integer aFirst = mySeqOfFormats(i+1);
      Standard_Integer aLen = mySeqOfFormats(i+2);
      theFormat = TCollection_AsciiString ( aLen, ' ' );
      for ( Standard_Integer j=1; j <= aLen; j++ )
        if ( IsAnAscii ( myMessageBody.Value ( aFirst + j ) ) )
          theFormat.SetValue ( j, (Standard_Character)myMessageBody.Value ( aFirst + j ) );
      // delete information on this placeholder
      mySeqOfFormats.Remove (i, i+2);
      // return start position
      return aFirst + 1;
    }
  return 0;
}

//=======================================================================
//function : replaceText
//purpose  : Replace format text in myMessageBody (theNb chars from theFirst)
//           by string theStr
//=======================================================================

void Message_Msg::replaceText (const Standard_Integer theFirst,
                               const Standard_Integer theNb,
                               const TCollection_ExtendedString &theStr)
{
  myMessageBody.Remove ( theFirst, theNb );
  myMessageBody.Insert ( theFirst, theStr );
  
  // update information on remaining format placeholders
  Standard_Integer anIncrement = theStr.Length() - theNb;
  if ( ! anIncrement ) return;
  for ( Standard_Integer i = 1; i <= mySeqOfFormats.Length(); i += 3 )
    if ( mySeqOfFormats(i+1) > theFirst )
      mySeqOfFormats(i+1) += anIncrement;
}
