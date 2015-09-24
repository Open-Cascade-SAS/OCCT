// Created on: 2002-10-30
// Created by: Michael SAZONOV
// Copyright (c) 2002-2014 OPEN CASCADE SAS
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


#include <BinObjMgt_Persistent.hxx>
#include <FSD_FileHeader.hxx>
#include <Standard_GUID.hxx>
#include <TCollection_AsciiString.hxx>
#include <TCollection_ExtendedString.hxx>
#include <TColStd_ListIteratorOfListOfInteger.hxx>
#include <TColStd_ListOfInteger.hxx>
#include <TDF_Data.hxx>
#include <TDF_Label.hxx>
#include <TDF_Tool.hxx>
#include <FSD_BinaryFile.hxx>

#define BP_INTSIZE         ((Standard_Integer)sizeof(Standard_Integer))
#define BP_EXTCHARSIZE     ((Standard_Integer)sizeof(Standard_ExtCharacter))
#define BP_REALSIZE        ((Standard_Integer)sizeof(Standard_Real))
#define BP_SHORTREALSIZE   ((Standard_Integer)sizeof(Standard_ShortReal))
#define BP_UUIDSIZE        ((Standard_Integer)sizeof(BinObjMgt_UUID))

// We define a GUID structure that is different from Standard_UUID because
// the latter contains a 'unsigned long' member that has variables size
// (4 or 8 bits) thus making the persistent files non-portable.
// This structure below ensures the portability.
typedef struct {
  unsigned int   Data1 ;        // 4-bytes long on all OS
  unsigned short Data2 ;        // 2-bytes long on all OS
  unsigned short Data3 ;        // 2-bytes long on all OS
  unsigned char  Data4[8] ;     // 8-bytes long on all OS
} BinObjMgt_UUID ;

//=======================================================================
//function : BinObjMgt_Persistent
//purpose  : Empty constructor
//=======================================================================

BinObjMgt_Persistent::BinObjMgt_Persistent ()
     : myIndex (1),
       myOffset(BP_HEADSIZE),
       mySize  (BP_HEADSIZE),
       myIsError (Standard_False)
{
  Init();
}

//=======================================================================
//function : Init
//purpose  : Initializes me to reuse again
//=======================================================================

void BinObjMgt_Persistent::Init ()
{
  if (myData.IsEmpty()) {
    Standard_Address aPiece = Standard::Allocate (BP_PIECESIZE);
    myData.Append (aPiece);
  }
  Standard_Integer *aData = (Standard_Integer*) myData(1);
  aData[0] = 0;         // Type Id
  aData[1] = 0;         // Object Id
  aData[2] = 0;         // Data length
  myIndex = 1;
  myOffset = BP_HEADSIZE;
  mySize = BP_HEADSIZE;
  myIsError = Standard_False;
}

//=======================================================================
//function : Write
//purpose  : Stores <me> to the stream.
//           inline Standard_OStream& operator<< (Standard_OStream&,
//           const BinObjMgt_Persistent&) is also available
//=======================================================================

Standard_OStream& BinObjMgt_Persistent::Write (Standard_OStream& theOS)
{
  Standard_Integer nbWritten = 0;
  Standard_Integer *aData = (Standard_Integer*) myData(1);
  // update data length
  aData[2] = mySize - BP_HEADSIZE;
#if DO_INVERSE
  aData[0] = InverseInt (aData[0]);
  aData[1] = InverseInt (aData[1]);
  aData[2] = InverseInt (aData[2]);
#endif
  for (Standard_Integer i=1;
       theOS && nbWritten < mySize && i <= myData.Length();
       i++) {
    Standard_Integer nbToWrite = Min (mySize - nbWritten, BP_PIECESIZE);
    theOS.write ((char*)myData(i), nbToWrite);
    nbWritten += nbToWrite;
  }
  myIndex = 1;
  myOffset = BP_HEADSIZE;
  mySize = BP_HEADSIZE;
  myIsError = Standard_False;
  return theOS;
}

//=======================================================================
//function : Read
//purpose  : Retrieves <me> from the stream.
//           inline Standard_IStream& operator>> (Standard_IStream&,
//           BinObjMgt_Persistent&) is also available
//=======================================================================

Standard_IStream& BinObjMgt_Persistent::Read (Standard_IStream& theIS)
{
  myIndex = 1;
  myOffset = BP_HEADSIZE;
  mySize = BP_HEADSIZE;
  myIsError = Standard_False;

  Standard_Integer *aData = (Standard_Integer*) myData(1);
  aData[0] = 0;         // Type Id
  aData[1] = 0;         // Object Id
  aData[2] = 0;         // Data length

  // read TypeId
  theIS.read ((char*) &aData[0], BP_INTSIZE);
#if DO_INVERSE
  aData[0] = InverseInt (aData[0]);
#endif
  if (theIS && aData[0] > 0) {
    // read Id and Length
    theIS.read ((char*)&aData[1], 2 * BP_INTSIZE);
#if DO_INVERSE
    aData[1] = InverseInt (aData[1]);
    aData[2] = InverseInt (aData[2]);
#endif
    if (theIS && aData[1] > 0 && aData[2] > 0) {
      mySize += aData[2];
      // read remaining data
      Standard_Integer nbRead = BP_HEADSIZE;
      for (Standard_Integer i=1;
           theIS && nbRead < mySize;
           i++) {
        if (i > myData.Length()) {
          // grow myData dynamically
          Standard_Address aPiece = Standard::Allocate (BP_PIECESIZE);
          myData.Append (aPiece);
        }
        Standard_Integer nbToRead = Min (mySize - nbRead, BP_PIECESIZE);
        char *ptr = (char*)myData(i);
        if (i == 1) {
          // 1st piece: reduce the number of bytes by header size
          ptr += BP_HEADSIZE;
          if (nbToRead == BP_PIECESIZE) nbToRead -= BP_HEADSIZE;
        }
        theIS.read (ptr, nbToRead);
        nbRead += nbToRead;
      }
    }
    else
      aData[2] = 0;
  }
  return theIS;
}

//=======================================================================
//function : Destroy
//purpose  : Frees the allocated memory
//=======================================================================

void BinObjMgt_Persistent::Destroy ()
{
  for (Standard_Integer i=1; i <= myData.Length(); i++) {
    Standard::Free (myData(i));
  }
  myData.Clear();
  myIndex = myOffset = mySize = 0;
}

//=======================================================================
//function : incrementData
//purpose  : Allocates theNbPieces more pieces
//=======================================================================

void BinObjMgt_Persistent::incrementData
  (const Standard_Integer theNbPieces)
{
  for (Standard_Integer i=1; i <= theNbPieces; i++) {
    Standard_Address aPiece = Standard::Allocate (BP_PIECESIZE);
    myData.Append (aPiece);
  }
}

//=======================================================================
//function : PutCharacter
//purpose  : 
//=======================================================================

BinObjMgt_Persistent& BinObjMgt_Persistent::PutCharacter
  (const Standard_Character theValue)
{
  alignOffset (1);
  prepareForPut (1);
  Standard_Character *aData = (Standard_Character*) myData(myIndex) + myOffset;
  *aData = theValue;
  myOffset++;
  return *this;
}

//=======================================================================
//function : PutByte
//purpose  : 
//=======================================================================

BinObjMgt_Persistent& BinObjMgt_Persistent::PutByte
  (const Standard_Byte theValue)
{
  alignOffset (1);
  prepareForPut (1);
  Standard_Byte *aData = (Standard_Byte*) myData(myIndex) + myOffset;
  *aData = theValue;
  myOffset++;
  return *this;
}

//=======================================================================
//function : PutExtCharacter
//purpose  : 
//=======================================================================

BinObjMgt_Persistent& BinObjMgt_Persistent::PutExtCharacter
  (const Standard_ExtCharacter theValue)
{
  alignOffset (BP_EXTCHARSIZE, Standard_True);
  prepareForPut (BP_EXTCHARSIZE);
  Standard_ExtCharacter *aData =
    (Standard_ExtCharacter*) ((char*)myData(myIndex) + myOffset);
#if DO_INVERSE
  *aData = InverseExtChar (theValue);
#else
  *aData = theValue;
#endif
  myOffset += BP_EXTCHARSIZE;
  return *this;
}

//=======================================================================
//function : PutInteger
//purpose  : 
//=======================================================================

BinObjMgt_Persistent& BinObjMgt_Persistent::PutInteger
  (const Standard_Integer theValue)
{
  alignOffset (BP_INTSIZE, Standard_True);
  prepareForPut (BP_INTSIZE);
  Standard_Integer *aData = (Standard_Integer*) ((char*)myData(myIndex) + myOffset);
#if DO_INVERSE
  *aData = InverseInt (theValue);
#else
  *aData = theValue;
#endif
  myOffset += BP_INTSIZE;
  return *this;
}

//=======================================================================
//function : PutReal
//purpose  : 
//=======================================================================

BinObjMgt_Persistent& BinObjMgt_Persistent::PutReal
  (const Standard_Real theValue)
{
  alignOffset (BP_INTSIZE, Standard_True);
  Standard_Integer nbPieces = prepareForPut (BP_REALSIZE);
  if (nbPieces > 0) {
    // the value intersects a piece boundary => go a long way
#if DO_INVERSE
    Standard_Integer aStartIndex = myIndex;
    Standard_Integer aStartOffset = myOffset;
#endif
    putArray ((void*) &theValue, BP_REALSIZE);
#if DO_INVERSE
    inverseRealData (aStartIndex, aStartOffset, BP_REALSIZE);
#endif
  }
  else {
    // the value fits in the current piece => put it quickly
    Standard_Real *aData = (Standard_Real*) ((char*)myData(myIndex) + myOffset);
#if DO_INVERSE
    *aData = InverseReal (theValue);
#else
    *aData = theValue;
#endif
    myOffset += BP_REALSIZE;
  }
  return *this;
}

//=======================================================================
//function : PutShortReal
//purpose  : 
//=======================================================================

BinObjMgt_Persistent& BinObjMgt_Persistent::PutShortReal
  (const Standard_ShortReal theValue)
{
  alignOffset (BP_INTSIZE, Standard_True);
  prepareForPut (BP_SHORTREALSIZE);
  Standard_ShortReal *aData = (Standard_ShortReal*) ((char*)myData(myIndex) + myOffset);
#if DO_INVERSE
  *aData = InverseShortReal (theValue);
#else
  *aData = theValue;
#endif
  myOffset += BP_SHORTREALSIZE;
  return *this;
}

//=======================================================================
//function : PutCString
//purpose  : Offset in output buffer is not aligned
//=======================================================================

BinObjMgt_Persistent& BinObjMgt_Persistent::PutCString
  (const Standard_CString theValue)
{
  alignOffset (1);
  Standard_Integer aSize = (Standard_Integer)(strlen (theValue) + 1);
  prepareForPut (aSize);
  putArray ((void* const)theValue, aSize);
  return *this;
}

//=======================================================================
//function : PutAsciiString
//purpose  : Offset in output buffer is word-aligned
//=======================================================================

BinObjMgt_Persistent& BinObjMgt_Persistent::PutAsciiString
  (const TCollection_AsciiString& theValue)
{
  alignOffset (BP_INTSIZE, Standard_True);
  Standard_Integer aSize = theValue.Length() + 1;
  prepareForPut (aSize);
  putArray ((void* const)theValue.ToCString(), aSize);
  return *this;
}

//=======================================================================
//function : PutExtendedString
//purpose  : Offset in output buffer is word-aligned
//=======================================================================

BinObjMgt_Persistent& BinObjMgt_Persistent::PutExtendedString
  (const TCollection_ExtendedString& theValue)
{
  alignOffset (BP_INTSIZE, Standard_True);
  Standard_Integer aSize = (theValue.Length() + 1) * BP_EXTCHARSIZE;
  prepareForPut (aSize);
#if DO_INVERSE
  Standard_Integer aStartIndex = myIndex;
  Standard_Integer aStartOffset = myOffset;
#endif
  putArray ((void* const)theValue.ToExtString(), aSize);
#if DO_INVERSE
  inverseExtCharData (aStartIndex, aStartOffset, aSize - BP_EXTCHARSIZE);
#endif
  return *this;
}

//=======================================================================
//function : PutLabel
//purpose  : 
//=======================================================================

BinObjMgt_Persistent& BinObjMgt_Persistent::PutLabel
  (const TDF_Label& theValue)
{
  alignOffset (BP_INTSIZE, Standard_True);
  Standard_Integer aLen = (theValue.IsNull() ? 0 : theValue.Depth()+1);
  prepareForPut ((aLen + 1) * BP_INTSIZE);
  Standard_Integer *aData = (Standard_Integer*) ((char*)myData(myIndex) + myOffset);
  // store nb of tags
#if DO_INVERSE
  *aData++ = InverseInt (aLen);
#else
  *aData++ = aLen;
#endif
  myOffset += BP_INTSIZE;
  if (!theValue.IsNull()) {
    TColStd_ListOfInteger aTagList;
    TDF_Tool::TagList (theValue, aTagList);
    TColStd_ListIteratorOfListOfInteger itTag(aTagList);
    for (; itTag.More(); itTag.Next()) {
      if (myOffset >= BP_PIECESIZE) {
        myOffset = 0;
        myIndex++;
        aData = (Standard_Integer*) ((char*)myData(myIndex) + myOffset);
      }
#if DO_INVERSE
      *aData++ = InverseInt (itTag.Value());
#else
      *aData++ = itTag.Value();
#endif
      myOffset += BP_INTSIZE;
    }
  }
  return *this;
}

//=======================================================================
//function : PutGUID
//purpose  : 
//=======================================================================

BinObjMgt_Persistent& BinObjMgt_Persistent::PutGUID
  (const Standard_GUID& theValue)
{
  alignOffset (BP_INTSIZE, Standard_True);
  prepareForPut (BP_UUIDSIZE);
  const Standard_UUID aStandardUUID = theValue.ToUUID();
  BinObjMgt_UUID anUUID;
  anUUID.Data1 = (unsigned int)   aStandardUUID.Data1;
  anUUID.Data2 = (unsigned short) aStandardUUID.Data2;
  anUUID.Data3 = (unsigned short) aStandardUUID.Data3;
  anUUID.Data4[0] = aStandardUUID.Data4[0];
  anUUID.Data4[1] = aStandardUUID.Data4[1];
  anUUID.Data4[2] = aStandardUUID.Data4[2];
  anUUID.Data4[3] = aStandardUUID.Data4[3];
  anUUID.Data4[4] = aStandardUUID.Data4[4];
  anUUID.Data4[5] = aStandardUUID.Data4[5];
  anUUID.Data4[6] = aStandardUUID.Data4[6];
  anUUID.Data4[7] = aStandardUUID.Data4[7];
#if DO_INVERSE
  anUUID.Data1 = (unsigned int)   InverseInt     (anUUID.Data1);
  anUUID.Data2 = (unsigned short) InverseExtChar (anUUID.Data2);
  anUUID.Data3 = (unsigned short) InverseExtChar (anUUID.Data3);
#endif
  putArray (&anUUID, BP_UUIDSIZE);
  return *this;
}

//=======================================================================
//function : PutCharArray
//purpose  : Put C array of char, theLength is the number of elements
//=======================================================================

BinObjMgt_Persistent& BinObjMgt_Persistent::PutCharArray
  (const BinObjMgt_PChar  theArray,
   const Standard_Integer theLength)
{
  alignOffset (1);
  prepareForPut (theLength);
  putArray (theArray, theLength);
  return *this;
}

//=======================================================================
//function : PutByteArray
//purpose  : Put C array of byte, theLength is the number of elements
//=======================================================================

BinObjMgt_Persistent& BinObjMgt_Persistent::PutByteArray
  (const BinObjMgt_PByte  theArray,
   const Standard_Integer theLength)
{
  alignOffset (1);
  prepareForPut (theLength);
  putArray (theArray, theLength);
  return *this;
}

//=======================================================================
//function : PutExtCharArray
//purpose  : Put C array of ExtCharacter, theLength is the number of elements
//=======================================================================

BinObjMgt_Persistent& BinObjMgt_Persistent::PutExtCharArray
  (const BinObjMgt_PExtChar theArray,
   const Standard_Integer   theLength)
{
  alignOffset (BP_EXTCHARSIZE, Standard_True);
  Standard_Integer aSize = theLength * BP_EXTCHARSIZE;
  prepareForPut (aSize);
#if DO_INVERSE
  Standard_Integer aStartIndex = myIndex;
  Standard_Integer aStartOffset = myOffset;
#endif
  putArray (theArray, aSize);
#if DO_INVERSE
  inverseExtCharData (aStartIndex, aStartOffset, aSize);
#endif
  return *this;
}

//=======================================================================
//function : PutIntArray
//purpose  : Put C array of int, theLength is the number of elements
//=======================================================================

BinObjMgt_Persistent& BinObjMgt_Persistent::PutIntArray
  (const BinObjMgt_PInteger theArray,
   const Standard_Integer   theLength)
{
  alignOffset (BP_INTSIZE, Standard_True);
  Standard_Integer aSize = theLength * BP_INTSIZE;
  prepareForPut (aSize);
#if DO_INVERSE
  Standard_Integer aStartIndex = myIndex;
  Standard_Integer aStartOffset = myOffset;
#endif
  putArray (theArray, aSize);
#if DO_INVERSE
  inverseIntData (aStartIndex, aStartOffset, aSize);
#endif
  return *this;
}

//=======================================================================
//function : PutRealArray
//purpose  : Put C array of double, theLength is the number of elements
//=======================================================================

BinObjMgt_Persistent& BinObjMgt_Persistent::PutRealArray
  (const BinObjMgt_PReal  theArray,
   const Standard_Integer theLength)
{
  alignOffset (BP_INTSIZE, Standard_True);
  Standard_Integer aSize = theLength * BP_REALSIZE;
  prepareForPut (aSize);
#if DO_INVERSE
  Standard_Integer aStartIndex = myIndex;
  Standard_Integer aStartOffset = myOffset;
#endif
  putArray (theArray, aSize);
#if DO_INVERSE
  inverseRealData (aStartIndex, aStartOffset, aSize);
#endif
  return *this;
}

//=======================================================================
//function : PutShortRealArray
//purpose  : Put C array of float, theLength is the number of elements
//=======================================================================

BinObjMgt_Persistent& BinObjMgt_Persistent::PutShortRealArray
  (const BinObjMgt_PShortReal theArray,
   const Standard_Integer     theLength)
{
  alignOffset (BP_INTSIZE, Standard_True);
  Standard_Integer aSize = theLength * BP_SHORTREALSIZE;
  prepareForPut (aSize);
#if DO_INVERSE
  Standard_Integer aStartIndex = myIndex;
  Standard_Integer aStartOffset = myOffset;
#endif
  putArray (theArray, aSize);
#if DO_INVERSE
  inverseShortRealData (aStartIndex, aStartOffset, aSize);
#endif
  return *this;
}

//=======================================================================
//function : GetCharacter
//purpose  : 
//=======================================================================

const BinObjMgt_Persistent& BinObjMgt_Persistent::GetCharacter
  (Standard_Character& theValue) const
{
  alignOffset (1);
  if (noMoreData (1)) return *this;
  Standard_Character *aData = (Standard_Character*) myData(myIndex) + myOffset;
  theValue = *aData;
  ((BinObjMgt_Persistent*)this)->myOffset++;
  return *this;
}

//=======================================================================
//function : GetByte
//purpose  : 
//=======================================================================

const BinObjMgt_Persistent& BinObjMgt_Persistent::GetByte
  (Standard_Byte& theValue) const
{
  alignOffset (1);
  if (noMoreData (1)) return *this;
  Standard_Byte *aData = (Standard_Byte*) myData(myIndex) + myOffset;
  theValue = *aData;
  ((BinObjMgt_Persistent*)this)->myOffset++;
  return *this;
}

//=======================================================================
//function : GetExtCharacter
//purpose  : 
//=======================================================================

const BinObjMgt_Persistent& BinObjMgt_Persistent::GetExtCharacter
  (Standard_ExtCharacter& theValue) const
{
  alignOffset (BP_EXTCHARSIZE);
  if (noMoreData (BP_EXTCHARSIZE)) return *this;
  Standard_ExtCharacter *aData =
    (Standard_ExtCharacter*) ((char*)myData(myIndex) + myOffset);
#if DO_INVERSE
  theValue = InverseExtChar (*aData);
#else
  theValue = *aData;
#endif
  ((BinObjMgt_Persistent*)this)->myOffset += BP_EXTCHARSIZE;
  return *this;
}

//=======================================================================
//function : GetInteger
//purpose  : 
//=======================================================================

const BinObjMgt_Persistent& BinObjMgt_Persistent::GetInteger
  (Standard_Integer& theValue) const
{
  alignOffset (BP_INTSIZE);
  if (noMoreData (BP_INTSIZE)) return *this;
  Standard_Integer *aData = (Standard_Integer*) ((char*)myData(myIndex) + myOffset);
#if DO_INVERSE
  theValue = InverseInt (*aData);
#else
  theValue = *aData;
#endif
  ((BinObjMgt_Persistent*)this)->myOffset += BP_INTSIZE;
  return *this;
}

//=======================================================================
//function : GetReal
//purpose  : 
//=======================================================================

const BinObjMgt_Persistent& BinObjMgt_Persistent::GetReal
  (Standard_Real& theValue) const
{
  alignOffset (BP_INTSIZE);
  if (noMoreData (BP_REALSIZE)) return *this;
  Standard_Integer nbPieces = (myOffset + BP_REALSIZE - 1) / BP_PIECESIZE;
  if (nbPieces > 0) {
    // the value intersects a piece boundary => go a long way
    getArray ((void*) &theValue, BP_REALSIZE);
  }
  else {
    // the value fits in the current piece => get it quickly
    Standard_Real *aData = (Standard_Real*) ((char*)myData(myIndex) + myOffset);
    theValue = *aData;
    ((BinObjMgt_Persistent*)this)->myOffset += BP_REALSIZE;
  }
#if DO_INVERSE
  theValue = InverseReal (theValue);
#endif
  return *this;
}

//=======================================================================
//function : GetShortReal
//purpose  : 
//=======================================================================

const BinObjMgt_Persistent& BinObjMgt_Persistent::GetShortReal
  (Standard_ShortReal& theValue) const
{
  alignOffset (BP_INTSIZE);
  if (noMoreData (BP_SHORTREALSIZE)) return *this;
  Standard_ShortReal *aData = (Standard_ShortReal*) ((char*)myData(myIndex) + myOffset);
#if DO_INVERSE
  theValue = InverseShortReal (*aData);
#else
  theValue = *aData;
#endif
  ((BinObjMgt_Persistent*)this)->myOffset += BP_SHORTREALSIZE;
  return *this;
}

//=======================================================================
//function : GetAsciiString
//purpose  : 
//=======================================================================

const BinObjMgt_Persistent& BinObjMgt_Persistent::GetAsciiString
  (TCollection_AsciiString& theValue) const
{
  alignOffset (BP_INTSIZE);
  Standard_Integer aStartIndex = myIndex;
  Standard_Integer aStartOffset = myOffset;
  BinObjMgt_Persistent* me = (BinObjMgt_Persistent*)this;
  char *aData = (char*) myData(myIndex) + myOffset;

  // count the string length
  while (!noMoreData (1) && *aData++) {
    me->myOffset++;
    if (myOffset >= BP_PIECESIZE) {
      me->myOffset = 0;
      aData = (char*) myData(++me->myIndex);
    }
  }
  if (IsError()) {
    me->myIndex = aStartIndex;
    me->myOffset = aStartOffset;
    return *this;
  }
  me->myOffset++;  // count the end null char

  if (myIndex == aStartIndex) {
    // all string is in one piece => simply copy
    theValue = aData - myOffset + aStartOffset;
  }
  else {
    // work through buffer string
    Standard_Integer aSize = (myIndex - aStartIndex) * BP_PIECESIZE +
      myOffset - aStartOffset;
    Standard_Address aString = Standard::Allocate (aSize);
    me->myIndex = aStartIndex;
    me->myOffset = aStartOffset;
    getArray (aString, aSize);
    theValue = (char*) aString;
    Standard::Free (aString);
  }

  return *this;
}

//=======================================================================
//function : GetExtendedString
//purpose  : 
//=======================================================================

const BinObjMgt_Persistent& BinObjMgt_Persistent::GetExtendedString
  (TCollection_ExtendedString& theValue) const
{
  alignOffset (BP_INTSIZE);
  Standard_Integer aStartIndex = myIndex;
  Standard_Integer aStartOffset = myOffset;
  BinObjMgt_Persistent* me = (BinObjMgt_Persistent*)this;
  Standard_ExtCharacter *aData =
    (Standard_ExtCharacter*) ((char*)myData(myIndex) + myOffset);

  // count the string length
  while (!noMoreData (1) && *aData++) {
    me->myOffset += BP_EXTCHARSIZE;
    if (myOffset >= BP_PIECESIZE) {
      me->myOffset = 0;
      aData = (Standard_ExtCharacter*) myData(++me->myIndex);
    }
  }
  if (IsError()) {
    me->myIndex = aStartIndex;
    me->myOffset = aStartOffset;
    return *this;
  }
  me->myOffset += BP_EXTCHARSIZE;  // count the end null char

  if (myIndex == aStartIndex) {
    // all string is in one piece => simply copy
    theValue = aData - (myOffset - aStartOffset) / BP_EXTCHARSIZE;
  }
  else {
    // work through buffer string
    Standard_Integer aSize = (myIndex - aStartIndex) * BP_PIECESIZE +
      (myOffset - aStartOffset);
    Standard_Address aString = Standard::Allocate (aSize);
    me->myIndex = aStartIndex;
    me->myOffset = aStartOffset;
    getArray (aString, aSize);
    theValue = (Standard_ExtCharacter*) aString;
    Standard::Free (aString);
  }
#if DO_INVERSE
  Standard_PExtCharacter aString = (Standard_PExtCharacter)theValue.ToExtString();
  for (Standard_Integer i=0; i < theValue.Length(); i++)
    aString[i] = InverseExtChar (aString[i]);
#endif

  return *this;
}

//=======================================================================
//function : GetLabel
//purpose  : 
//=======================================================================

const BinObjMgt_Persistent& BinObjMgt_Persistent::GetLabel
  (const Handle(TDF_Data)& theDS,
   TDF_Label& theValue) const
{
  theValue.Nullify();
  alignOffset (BP_INTSIZE);
  if (noMoreData (BP_INTSIZE)) return *this;
  BinObjMgt_Persistent* me = (BinObjMgt_Persistent*)this;
  // retrieve nb of tags
  Standard_Integer *aData = (Standard_Integer*) ((char*)myData(myIndex) + myOffset);
  Standard_Integer aLen = *aData++;
#if DO_INVERSE
  aLen = InverseInt (aLen);
#endif
  me->myOffset += BP_INTSIZE;
  if (noMoreData (aLen * BP_INTSIZE)) return *this;

  if (aLen > 0) {
    // retrieve tag list
    TColStd_ListOfInteger aTagList;
    while (aLen > 0) {
      if (myOffset >= BP_PIECESIZE) {
        me->myOffset = 0;
        me->myIndex++;
        aData = (Standard_Integer*) ((char*)myData(myIndex) + myOffset);
      }
#if DO_INVERSE
      aTagList.Append (InverseInt (*aData++));
#else
      aTagList.Append (*aData++);
#endif
      me->myOffset += BP_INTSIZE;
      aLen--;
    }
    // find label by entry
    TDF_Tool::Label (theDS, aTagList, theValue, Standard_True);
  }
  return *this;
}

//=======================================================================
//function : GetGUID
//purpose  : 
//=======================================================================

const BinObjMgt_Persistent& BinObjMgt_Persistent::GetGUID
  (Standard_GUID& theValue) const
{
  alignOffset (BP_INTSIZE);
  if (noMoreData (BP_UUIDSIZE))
    return *this;
  BinObjMgt_UUID anUUID;
  getArray (&anUUID, BP_UUIDSIZE);
#if DO_INVERSE
  anUUID.Data1 = (unsigned int)   InverseInt     (anUUID.Data1);
  anUUID.Data2 = (unsigned short) InverseExtChar (anUUID.Data2);
  anUUID.Data3 = (unsigned short) InverseExtChar (anUUID.Data3);
#endif
  theValue = Standard_GUID (anUUID.Data1, anUUID.Data2, anUUID.Data3,
                            ((anUUID.Data4[0] << 8) | (anUUID.Data4[1])),
                            anUUID.Data4[2], anUUID.Data4[3], anUUID.Data4[4],
                            anUUID.Data4[5], anUUID.Data4[6], anUUID.Data4[7]);
  return *this;
}

//=======================================================================
//function : GetCharArray
//purpose  : Get C array of char, theLength is the number of elements;
//           theArray must point to a 
//           space enough to place theLength elements
//=======================================================================

const BinObjMgt_Persistent& BinObjMgt_Persistent::GetCharArray
  (const BinObjMgt_PChar  theArray,
   const Standard_Integer theLength) const
{
  alignOffset (1);
  if (noMoreData (theLength)) return *this;
  getArray (theArray, theLength);
  return *this;
}

//=======================================================================
//function : GetByteArray
//purpose  : Get C array of unsigned chars, theLength is the number of elements;
//           theArray must point to a 
//           space enough to place theLength elements
//=======================================================================

const BinObjMgt_Persistent& BinObjMgt_Persistent::GetByteArray
  (const BinObjMgt_PByte  theArray,
   const Standard_Integer theLength) const
{
  alignOffset (1);
  if (noMoreData (theLength)) return *this;
  getArray (theArray, theLength);
  return *this;
}

//=======================================================================
//function : GetExtCharArray
//purpose  : Get C array of ExtCharacter, theLength is the number of elements;
//           theArray must point to a 
//           space enough to place theLength elements
//=======================================================================

const BinObjMgt_Persistent& BinObjMgt_Persistent::GetExtCharArray
  (const BinObjMgt_PExtChar theArray,
   const Standard_Integer   theLength) const
{
  alignOffset (BP_EXTCHARSIZE, Standard_True);
  Standard_Integer aSize = theLength * BP_EXTCHARSIZE;
  if (noMoreData (aSize)) return *this;
  getArray (theArray, aSize);
#if DO_INVERSE
  for (Standard_Integer i=0; i < theLength; i++)
    theArray[i] = InverseExtChar (theArray[i]);
#endif
  return *this;
}

//=======================================================================
//function : GetIntArray
//purpose  : Get C array of int, theLength is the number of elements;
//           theArray must point to a 
//           space enough to place theLength elements
//=======================================================================

const BinObjMgt_Persistent& BinObjMgt_Persistent::GetIntArray
  (const BinObjMgt_PInteger theArray,
   const Standard_Integer   theLength) const
{
  alignOffset (BP_INTSIZE, Standard_True);
  Standard_Integer aSize = theLength * BP_INTSIZE;
  if (noMoreData (aSize)) return *this;
  getArray (theArray, aSize);
#if DO_INVERSE
  for (Standard_Integer i=0; i < theLength; i++)
    theArray[i] = InverseInt (theArray[i]);
#endif
  return *this;
}

//=======================================================================
//function : GetRealArray
//purpose  : Get C array of double, theLength is the number of elements;
//           theArray must point to a 
//           space enough to place theLength elements
//=======================================================================

const BinObjMgt_Persistent& BinObjMgt_Persistent::GetRealArray
  (const BinObjMgt_PReal  theArray,
   const Standard_Integer theLength) const
{
  alignOffset (BP_INTSIZE, Standard_True);
  Standard_Integer aSize = theLength * BP_REALSIZE;
  if (noMoreData (aSize)) return *this;
  getArray (theArray, aSize);
#if DO_INVERSE
  for (Standard_Integer i=0; i < theLength; i++)
    theArray[i] = InverseReal (theArray[i]);
#endif
  return *this;
}

//=======================================================================
//function : GetShortRealArray
//purpose  : Get C array of float, theLength is the number of elements;
//           theArray must point to a 
//           space enough to place theLength elements
//=======================================================================

const BinObjMgt_Persistent& BinObjMgt_Persistent::GetShortRealArray
  (const BinObjMgt_PShortReal theArray,
   const Standard_Integer     theLength) const
{
  alignOffset (BP_INTSIZE, Standard_True);
  Standard_Integer aSize = theLength * BP_SHORTREALSIZE;
  if (noMoreData (aSize)) return *this;
  getArray (theArray, aSize);
#if DO_INVERSE
  for (Standard_Integer i=0; i < theLength; i++)
    theArray[i] = InverseShortReal (theArray[i]);
#endif
  return *this;
}

//=======================================================================
//function : putArray
//purpose  : Puts theSize bytes from theArray
//=======================================================================

void BinObjMgt_Persistent::putArray
  (const Standard_Address theArray,
   const Standard_Integer theSize)
{
  char *aPtr = (char*) theArray;
  Standard_Integer aLen = theSize;
  while (aLen > 0) {
    if (myOffset >= BP_PIECESIZE) {
      myIndex++;
      myOffset = 0;
    }
    Standard_Integer aLenInPiece = Min (aLen, BP_PIECESIZE - myOffset);
    char *aData = (char*) myData(myIndex) + myOffset;
    memcpy (aData, aPtr, aLenInPiece);
    aLen -= aLenInPiece;
    aPtr += aLenInPiece;
    myOffset += aLenInPiece;
  }
}

//=======================================================================
//function : getArray
//purpose  : Gets theLength bytes into theArray
//=======================================================================

void BinObjMgt_Persistent::getArray
  (const Standard_Address theArray,
   const Standard_Integer theSize) const
{
  char *aPtr = (char*) theArray;
  Standard_Integer aLen = theSize;
  BinObjMgt_Persistent *me = (BinObjMgt_Persistent*) this;
  while (aLen > 0) {
    if (myOffset >= BP_PIECESIZE) {
      me->myIndex++;
      me->myOffset = 0;
    }
    Standard_Integer aLenInPiece = Min (aLen, BP_PIECESIZE - myOffset);
    char *aData = (char*) myData(myIndex) + myOffset;
    memcpy (aPtr, aData, aLenInPiece);
    aLen -= aLenInPiece;
    aPtr += aLenInPiece;
    me->myOffset += aLenInPiece;
  }
}

//=======================================================================
//function : inverseExtCharData
//purpose  : Inverses bytes in the data addressed by the given values
//=======================================================================

void BinObjMgt_Persistent::inverseExtCharData
  (const Standard_Integer theIndex,
   const Standard_Integer theOffset,
   const Standard_Integer theSize)
{
  Standard_Integer anIndex = theIndex;
  Standard_Integer anOffset = theOffset;
  Standard_Integer aLen = theSize;
  while (aLen > 0) {
    Standard_Integer aLenInPiece = Min (aLen, BP_PIECESIZE - anOffset);
    Standard_ExtCharacter *aData = (Standard_ExtCharacter*)
      ( (char*) myData(anIndex) + anOffset);
    for (Standard_Integer i=0; i < aLenInPiece / BP_EXTCHARSIZE; i++)
      aData[i] = FSD_BinaryFile::InverseExtChar (aData[i]);
    aLen -= aLenInPiece;
    anOffset += aLenInPiece;
    if (anOffset >= BP_PIECESIZE) {
      anIndex++;
      anOffset = 0;
    }
  }
}

//=======================================================================
//function : inverseIntData
//purpose  : Inverses bytes in the data addressed by the given values
//=======================================================================

void BinObjMgt_Persistent::inverseIntData
  (const Standard_Integer theIndex,
   const Standard_Integer theOffset,
   const Standard_Integer theSize)
{
  Standard_Integer anIndex = theIndex;
  Standard_Integer anOffset = theOffset;
  Standard_Integer aLen = theSize;
  while (aLen > 0) {
    Standard_Integer aLenInPiece = Min (aLen, BP_PIECESIZE - anOffset);
    Standard_Integer *aData = (Standard_Integer*) ((char*)myData(anIndex) + anOffset);
    for (Standard_Integer i=0; i < aLenInPiece / BP_INTSIZE; i++)
      aData[i] = FSD_BinaryFile::InverseInt (aData[i]);
    aLen -= aLenInPiece;
    anOffset += aLenInPiece;
    if (anOffset >= BP_PIECESIZE) {
      anIndex++;
      anOffset = 0;
    }
  }
}

//=======================================================================
//function : inverseRealData
//purpose  : Inverses bytes in the data addressed by the given values
//=======================================================================

void BinObjMgt_Persistent::inverseRealData
  (const Standard_Integer theIndex,
   const Standard_Integer theOffset,
   const Standard_Integer theSize)
{
  Standard_Integer anIndex = theIndex;
  Standard_Integer anOffset = theOffset;
  Standard_Integer aLen = theSize;

  union {
        Standard_Real*    aRealData;
        Standard_Integer* aIntData;
      } aWrapUnion;

  void *aPrevPtr = 0;
  while (aLen > 0) {
    Standard_Integer aLenInPiece = Min (aLen, BP_PIECESIZE - anOffset);

    aWrapUnion.aRealData = (Standard_Real*) ((char*)myData(anIndex) + anOffset);
    
    if (aPrevPtr) {
      Standard_Integer aTmp;
      aTmp = FSD_BinaryFile::InverseInt (*(Standard_Integer*)aPrevPtr);
      *(Standard_Integer*)aPrevPtr = FSD_BinaryFile::InverseInt (*aWrapUnion.aIntData);
      *aWrapUnion.aIntData = aTmp;
      aWrapUnion.aIntData++;
      aPrevPtr = 0;
    }
    for (Standard_Integer i=0; i < aLenInPiece / BP_REALSIZE; i++)
      aWrapUnion.aRealData[i] = FSD_BinaryFile::InverseReal(aWrapUnion.aRealData[i]);
    if (aLenInPiece % BP_REALSIZE)
      aPrevPtr = &aWrapUnion.aRealData[aLenInPiece / BP_REALSIZE];
    aLen -= aLenInPiece;
    anOffset += aLenInPiece;
    if (anOffset >= BP_PIECESIZE) {
      anIndex++;
      anOffset = 0;
    }
  }
}

//=======================================================================
//function : inverseShortRealData
//purpose  : Inverses bytes in the data addressed by the given values
//=======================================================================

void BinObjMgt_Persistent::inverseShortRealData
  (const Standard_Integer theIndex,
   const Standard_Integer theOffset,
   const Standard_Integer theSize)
{
  Standard_Integer anIndex = theIndex;
  Standard_Integer anOffset = theOffset;
  Standard_Integer aLen = theSize;
  while (aLen > 0) {
    Standard_Integer aLenInPiece = Min (aLen, BP_PIECESIZE - anOffset);
    Standard_ShortReal *aData =
      (Standard_ShortReal *) ((char *)myData(anIndex) + anOffset);
    for (Standard_Integer i=0; i < aLenInPiece / BP_INTSIZE; i++)
      aData[i] = FSD_BinaryFile::InverseShortReal (aData[i]);
    aLen -= aLenInPiece;
    anOffset += aLenInPiece;
    if (anOffset >= BP_PIECESIZE) {
      anIndex++;
      anOffset = 0;
    }
  }
}
