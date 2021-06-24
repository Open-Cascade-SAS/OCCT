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

#ifndef _BinObjMgt_Persistent_HeaderFile
#define _BinObjMgt_Persistent_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <TColStd_SequenceOfAddress.hxx>
#include <Standard_Integer.hxx>
#include <Standard_Boolean.hxx>
#include <Standard_Character.hxx>
#include <Standard_Byte.hxx>
#include <Standard_ExtCharacter.hxx>
#include <Standard_Real.hxx>
#include <Standard_ShortReal.hxx>
#include <Standard_CString.hxx>
#include <BinObjMgt_PChar.hxx>
#include <BinObjMgt_PByte.hxx>
#include <BinObjMgt_PExtChar.hxx>
#include <BinObjMgt_PInteger.hxx>
#include <BinObjMgt_PReal.hxx>
#include <BinObjMgt_PShortReal.hxx>
#include <BinObjMgt_Position.hxx>
#include <Standard_OStream.hxx>
#include <Standard_IStream.hxx>
#include <Standard_Address.hxx>
class TCollection_AsciiString;
class TCollection_ExtendedString;
class TDF_Label;
class Standard_GUID;
class TDF_Data;


//! Binary persistent representation of an object.
//! Really it is used as a buffer for read/write an object.
//!
//! It takes care of Little/Big endian by inversing bytes
//! in objects of standard types (see FSD_FileHeader.hxx
//! for the default value of DO_INVERSE).
class BinObjMgt_Persistent 
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! Empty constructor
  Standard_EXPORT BinObjMgt_Persistent();
  
  Standard_EXPORT BinObjMgt_Persistent& PutCharacter (const Standard_Character theValue);
BinObjMgt_Persistent& operator << (const Standard_Character theValue)
{
  return PutCharacter(theValue);
}
  
  Standard_EXPORT BinObjMgt_Persistent& PutByte (const Standard_Byte theValue);
BinObjMgt_Persistent& operator << (const Standard_Byte theValue)
{
  return PutByte(theValue);
}
  
  Standard_EXPORT BinObjMgt_Persistent& PutExtCharacter (const Standard_ExtCharacter theValue);
BinObjMgt_Persistent& operator << (const Standard_ExtCharacter theValue)
{
  return PutExtCharacter(theValue);
}
  
  Standard_EXPORT BinObjMgt_Persistent& PutInteger (const Standard_Integer theValue);
BinObjMgt_Persistent& operator << (const Standard_Integer theValue)
{
  return PutInteger(theValue);
}
  
    BinObjMgt_Persistent& PutBoolean (const Standard_Boolean theValue);
  BinObjMgt_Persistent& operator << (const Standard_Boolean theValue)
{
  return PutBoolean(theValue);
}
  
  Standard_EXPORT BinObjMgt_Persistent& PutReal (const Standard_Real theValue);
BinObjMgt_Persistent& operator << (const Standard_Real theValue)
{
  return PutReal(theValue);
}
  
  Standard_EXPORT BinObjMgt_Persistent& PutShortReal (const Standard_ShortReal theValue);
BinObjMgt_Persistent& operator << (const Standard_ShortReal theValue)
{
  return PutShortReal(theValue);
}
  
  //! Offset in output buffer is not aligned
  Standard_EXPORT BinObjMgt_Persistent& PutCString (const Standard_CString theValue);
BinObjMgt_Persistent& operator << (const Standard_CString theValue)
{
  return PutCString(theValue);
}
  
  //! Offset in output buffer is word-aligned
  Standard_EXPORT BinObjMgt_Persistent& PutAsciiString (const TCollection_AsciiString& theValue);
BinObjMgt_Persistent& operator << (const TCollection_AsciiString& theValue)
{
  return PutAsciiString(theValue);
}
  
  //! Offset in output buffer is word-aligned
  Standard_EXPORT BinObjMgt_Persistent& PutExtendedString (const TCollection_ExtendedString& theValue);
BinObjMgt_Persistent& operator << (const TCollection_ExtendedString& theValue)
{
  return PutExtendedString(theValue);
}
  
  Standard_EXPORT BinObjMgt_Persistent& PutLabel (const TDF_Label& theValue);
BinObjMgt_Persistent& operator << (const TDF_Label& theValue)
{
  return PutLabel(theValue);
}
  
  Standard_EXPORT BinObjMgt_Persistent& PutGUID (const Standard_GUID& theValue);
BinObjMgt_Persistent& operator << (const Standard_GUID& theValue)
{
  return PutGUID(theValue);
}
  
  //! Put C array of char, theLength is the number of elements
  Standard_EXPORT BinObjMgt_Persistent& PutCharArray (const BinObjMgt_PChar theArray, const Standard_Integer theLength);
  
  //! Put C array of unsigned chars, theLength is the number of elements
  Standard_EXPORT BinObjMgt_Persistent& PutByteArray (const BinObjMgt_PByte theArray, const Standard_Integer theLength);
  
  //! Put C array of ExtCharacter, theLength is the number of elements
  Standard_EXPORT BinObjMgt_Persistent& PutExtCharArray (const BinObjMgt_PExtChar theArray, const Standard_Integer theLength);
  
  //! Put C array of int, theLength is the number of elements
  Standard_EXPORT BinObjMgt_Persistent& PutIntArray (const BinObjMgt_PInteger theArray, const Standard_Integer theLength);
  
  //! Put C array of double, theLength is the number of elements
  Standard_EXPORT BinObjMgt_Persistent& PutRealArray (const BinObjMgt_PReal theArray, const Standard_Integer theLength);
  
  //! Put C array of float, theLength is the number of elements
  Standard_EXPORT BinObjMgt_Persistent& PutShortRealArray (const BinObjMgt_PShortReal theArray, const Standard_Integer theLength);
  
  Standard_EXPORT const BinObjMgt_Persistent& GetCharacter (Standard_Character& theValue) const;
const BinObjMgt_Persistent& operator >> (Standard_Character& theValue) const
{
  return GetCharacter(theValue);
}
  
  Standard_EXPORT const BinObjMgt_Persistent& GetByte (Standard_Byte& theValue) const;
const BinObjMgt_Persistent& operator >> (Standard_Byte& theValue) const
{
  return GetByte(theValue);
}
  
  Standard_EXPORT const BinObjMgt_Persistent& GetExtCharacter (Standard_ExtCharacter& theValue) const;
const BinObjMgt_Persistent& operator >> (Standard_ExtCharacter& theValue) const
{
  return GetExtCharacter(theValue);
}
  
  Standard_EXPORT const BinObjMgt_Persistent& GetInteger (Standard_Integer& theValue) const;
const BinObjMgt_Persistent& operator >> (Standard_Integer& theValue) const
{
  return GetInteger(theValue);
}
  
    const BinObjMgt_Persistent& GetBoolean (Standard_Boolean& theValue) const;
  const BinObjMgt_Persistent& operator >> (Standard_Boolean& theValue) const
{
  return GetBoolean(theValue);
}
  
  Standard_EXPORT const BinObjMgt_Persistent& GetReal (Standard_Real& theValue) const;
const BinObjMgt_Persistent& operator >> (Standard_Real& theValue) const
{
  return GetReal(theValue);
}
  
  Standard_EXPORT const BinObjMgt_Persistent& GetShortReal (Standard_ShortReal& theValue) const;
const BinObjMgt_Persistent& operator >> (Standard_ShortReal& theValue) const
{
  return GetShortReal(theValue);
}
  
  Standard_EXPORT const BinObjMgt_Persistent& GetAsciiString (TCollection_AsciiString& theValue) const;
const BinObjMgt_Persistent& operator >> (TCollection_AsciiString& theValue) const
{
  return GetAsciiString(theValue);
}
  
  Standard_EXPORT const BinObjMgt_Persistent& GetExtendedString (TCollection_ExtendedString& theValue) const;
const BinObjMgt_Persistent& operator >> (TCollection_ExtendedString& theValue) const
{
  return GetExtendedString(theValue);
}
  
  Standard_EXPORT const BinObjMgt_Persistent& GetLabel (const Handle(TDF_Data)& theDS, TDF_Label& theValue) const;
  
  Standard_EXPORT const BinObjMgt_Persistent& GetGUID (Standard_GUID& theValue) const;
const BinObjMgt_Persistent& operator >> (Standard_GUID& theValue) const
{
  return GetGUID(theValue);
}
  
  //! Get C array of char, theLength is the number of elements;
  //! theArray must point to a
  //! space enough to place theLength elements
  Standard_EXPORT const BinObjMgt_Persistent& GetCharArray (const BinObjMgt_PChar theArray, const Standard_Integer theLength) const;
  
  //! Get C array of unsigned chars, theLength is the number of elements;
  //! theArray must point to a
  //! space enough to place theLength elements
  Standard_EXPORT const BinObjMgt_Persistent& GetByteArray (const BinObjMgt_PByte theArray, const Standard_Integer theLength) const;
  
  //! Get C array of ExtCharacter, theLength is the number of elements;
  //! theArray must point to a
  //! space enough to place theLength elements
  Standard_EXPORT const BinObjMgt_Persistent& GetExtCharArray (const BinObjMgt_PExtChar theArray, const Standard_Integer theLength) const;
  
  //! Get C array of int, theLength is the number of elements;
  //! theArray must point to a
  //! space enough to place theLength elements
  Standard_EXPORT const BinObjMgt_Persistent& GetIntArray (const BinObjMgt_PInteger theArray, const Standard_Integer theLength) const;
  
  //! Get C array of double, theLength is the number of elements;
  //! theArray must point to a
  //! space enough to place theLength elements
  Standard_EXPORT const BinObjMgt_Persistent& GetRealArray (const BinObjMgt_PReal theArray, const Standard_Integer theLength) const;
  
  //! Get C array of float, theLength is the number of elements;
  //! theArray must point to a
  //! space enough to place theLength elements
  Standard_EXPORT const BinObjMgt_Persistent& GetShortRealArray (const BinObjMgt_PShortReal theArray, const Standard_Integer theLength) const;
  
  //! Tells the current position for get/put
    Standard_Integer Position() const;
  
  //! Sets the current position for get/put.
  //! Resets an error state depending on the validity of thePos.
  //! Returns the new state (value of IsOK())
    Standard_Boolean SetPosition (const Standard_Integer thePos) const;
  
  //! Truncates the buffer by current position,
  //! i.e. updates mySize
    void Truncate();
  
  //! Indicates an error after Get methods or SetPosition
    Standard_Boolean IsError() const;
  Standard_Boolean operator !() const
  {
    return IsError();
  }
  
  //! Indicates a good state after Get methods or SetPosition
    Standard_Boolean IsOK() const;
  operator Standard_Boolean () const { return IsOK(); }
  
  //! Initializes me to reuse again
  Standard_EXPORT void Init();
  
  //! Sets the Id of the object
    void SetId (const Standard_Integer theId);
  
  //! Sets the Id of the type of the object
    void SetTypeId (const Standard_Integer theId);
  
  //! Returns the Id of the object
    Standard_Integer Id() const;
  
  //! Returns the Id of the type of the object
    Standard_Integer TypeId() const;
  
  //! Returns the length of data
    Standard_Integer Length() const;
  
  //! Stores <me> to the stream.
  //! inline Standard_OStream& operator<< (Standard_OStream&,
  //! BinObjMgt_Persistent&) is also available.
  //! If theDirectStream is true, after this data the direct stream data is stored.
  Standard_EXPORT Standard_OStream& Write (Standard_OStream& theOS, const Standard_Boolean theDirectStream = Standard_False);
  
  //! Retrieves <me> from the stream.
  //! inline Standard_IStream& operator>> (Standard_IStream&,
  //! BinObjMgt_Persistent&) is also available
  Standard_EXPORT Standard_IStream& Read (Standard_IStream& theIS);
  
  //! Frees the allocated memory;
  //! This object can be reused after call to Init
  Standard_EXPORT void Destroy();
  ~BinObjMgt_Persistent()
  {
    Destroy();
  }

  //! Sets the stream for direct writing
  Standard_EXPORT void SetOStream (Standard_OStream& theStream) { myOStream = &theStream; }
  //! Sets the stream for direct reading
  Standard_EXPORT void SetIStream (Standard_IStream& theStream) { myIStream = &theStream; }
  //! Gets the stream for and enables direct writing
  Standard_EXPORT Standard_OStream* GetOStream();
  //! Gets the stream for and enables direct reading
  Standard_EXPORT Standard_IStream* GetIStream();
  //! Returns true if after this record a direct writing to the stream is performed.
  Standard_EXPORT Standard_Boolean IsDirect() { return myDirectWritingIsEnabled; }
  //! Returns the start position of the direct writing in the stream
  Standard_EXPORT Handle(BinObjMgt_Position) StreamStart() { return myStreamStart; }

private:

  
  //! Aligns myOffset to the given size;
  //! enters the next piece if the end of the current one is reached;
  //! toClear==true means to fill unused space by 0
    void alignOffset (const Standard_Integer theSize, const Standard_Boolean toClear = Standard_False) const;
  
  //! Prepares the room for theSize bytes;
  //! returns the number of pieces except for the current one
  //! are to be occupied
    Standard_Integer prepareForPut (const Standard_Integer theSize);
  
  //! Allocates theNbPieces more pieces
  Standard_EXPORT void incrementData (const Standard_Integer theNbPieces);
  
  //! Checks if there is no more data of the given size starting
  //! from the current position in myData
    Standard_Boolean noMoreData (const Standard_Integer theSize) const;
  
  //! Puts theLength bytes from theArray
  Standard_EXPORT void putArray (const Standard_Address theArray, const Standard_Integer theSize);
  
  //! Gets theLength bytes into theArray
  Standard_EXPORT void getArray (const Standard_Address theArray, const Standard_Integer theSize) const;
  
  //! Inverses bytes in the data addressed by the given values
  Standard_EXPORT void inverseExtCharData (const Standard_Integer theIndex, const Standard_Integer theOffset, const Standard_Integer theSize);
  
  //! Inverses bytes in the data addressed by the given values
  Standard_EXPORT void inverseIntData (const Standard_Integer theIndex, const Standard_Integer theOffset, const Standard_Integer theSize);
  
  //! Inverses bytes in the data addressed by the given values
  Standard_EXPORT void inverseRealData (const Standard_Integer theIndex, const Standard_Integer theOffset, const Standard_Integer theSize);
  
  //! Inverses bytes in the data addressed by the given values
  Standard_EXPORT void inverseShortRealData (const Standard_Integer theIndex, const Standard_Integer theOffset, const Standard_Integer theSize);


  TColStd_SequenceOfAddress myData;
  Standard_Integer myIndex;
  Standard_Integer myOffset;
  Standard_Integer mySize;
  Standard_Boolean myIsError;
  Standard_OStream* myOStream; ///< stream to write in case direct writing is enabled
  Standard_IStream* myIStream; ///< stream to write in case direct reading is enabled
  Standard_Boolean myDirectWritingIsEnabled;
  Handle(BinObjMgt_Position) myStreamStart; ///< position where the direct writing to the script is started
};


#include <BinObjMgt_Persistent.lxx>





#endif // _BinObjMgt_Persistent_HeaderFile
