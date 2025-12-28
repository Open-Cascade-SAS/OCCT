// Copyright (c) 2021 OPEN CASCADE SAS
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

#ifndef _BinTools_IStream_HeaderFile
#define _BinTools_IStream_HeaderFile

#include <BinTools.hxx>
#include <BinTools_ObjectType.hxx>
#include <TopAbs_ShapeEnum.hxx>
#include <TopAbs_Orientation.hxx>
#include <gp_Pnt.hxx>

//! Substitution of IStream for shape reader for fast management of position in the file (get and
//! go) and operation on all reading types.
class BinTools_IStream
{
public:
  //! Creates IStream using the current stream IStream.
  Standard_EXPORT BinTools_IStream(Standard_IStream& theStream);

  //! Reads and returns the type.
  Standard_EXPORT BinTools_ObjectType ReadType();

  //! Returns the last read type.
  Standard_EXPORT const BinTools_ObjectType& LastType() { return myLastType; }

  //! Returns the shape type by the last retrieved type.
  Standard_EXPORT TopAbs_ShapeEnum ShapeType();
  //! Returns the shape orientation by the last retrieved type.
  Standard_EXPORT TopAbs_Orientation ShapeOrientation();

  //! Returns the current position in the stream.
  Standard_EXPORT uint64_t Position() { return myPosition; }

  //! Moves the current stream position to the given one.
  Standard_EXPORT void GoTo(const uint64_t& thePosition);

  //! Returns true if the last restored type is one of a reference
  Standard_EXPORT bool IsReference();
  //! Reads a reference IStream using the last restored type.
  Standard_EXPORT uint64_t ReadReference();

  //! Returns the original IStream.
  Standard_EXPORT Standard_IStream& Stream() { return *myStream; }

  //! Makes up to date the myPosition because myStream was used outside and position is changed.
  Standard_EXPORT void UpdatePosition() { myPosition = uint64_t(myStream->tellg()); }

  //! Returns false if stream reading is failed.
  Standard_EXPORT operator bool() const;

  //! Reads real value from the stream.
  Standard_EXPORT double ReadReal()
  {
    double aValue;
    *this >> aValue;
    return aValue;
  }

  Standard_EXPORT BinTools_IStream& operator>>(double& theValue);

  //! Reads integer value from the stream.
  Standard_EXPORT int ReadInteger()
  {
    int aValue;
    *this >> aValue;
    return aValue;
  }

  Standard_EXPORT BinTools_IStream& operator>>(int& theValue);

  //! Reads point coordinates value from the stream.
  Standard_EXPORT gp_Pnt ReadPnt()
  {
    gp_Pnt aValue;
    *this >> aValue;
    return aValue;
  }

  Standard_EXPORT BinTools_IStream& operator>>(gp_Pnt& theValue);

  //! Reads byte value from the stream.
  Standard_EXPORT uint8_t ReadByte()
  {
    uint8_t aValue;
    *this >> aValue;
    return aValue;
  }

  Standard_EXPORT BinTools_IStream& operator>>(uint8_t& theValue);

  //! Reads boolean value from the stream (stored as one byte).
  Standard_EXPORT bool ReadBool() { return ReadByte() != 0; }

  Standard_EXPORT BinTools_IStream& operator>>(bool& theValue)
  {
    theValue = ReadByte() != 0;
    return *this;
  }

  //! Reads short real value from the stream.
  Standard_EXPORT float ReadShortReal()
  {
    float aValue;
    *this >> aValue;
    return aValue;
  }

  Standard_EXPORT BinTools_IStream& operator>>(float& theValue);
  //! Reads transformation value from the stream.
  Standard_EXPORT BinTools_IStream& operator>>(gp_Trsf& theValue);
  //! Reads 3 boolean values from one byte
  Standard_EXPORT void ReadBools(bool& theBool1, bool& theBool2, bool& theBool3);
  //! Reads 7 boolean values from one byte
  Standard_EXPORT void ReadBools(bool& theBool1,
                                 bool& theBool2,
                                 bool& theBool3,
                                 bool& theBool4,
                                 bool& theBool5,
                                 bool& theBool6,
                                 bool& theBool7);

private:
  Standard_IStream*   myStream;   ///< pointer to the stream
  uint64_t            myPosition; ///< equivalent to tellg returned value for fast access
  BinTools_ObjectType myLastType; ///< last type that was read
};

#endif // _BinTools_IStream_HeaderFile
