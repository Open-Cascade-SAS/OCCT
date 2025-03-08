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

#include <BinTools_IStream.hxx>
#include <Storage_StreamTypeMismatchError.hxx>

//=================================================================================================

BinTools_IStream::BinTools_IStream(Standard_IStream& theStream)
    : myStream(&theStream),
      myPosition(theStream.tellg()),
      myLastType(BinTools_ObjectType_Unknown)
{
}

//=================================================================================================

BinTools_ObjectType BinTools_IStream::ReadType()
{
  myLastType = BinTools_ObjectType(myStream->get());
  myPosition++;
  return myLastType;
}

//=================================================================================================

Standard_Boolean BinTools_IStream::IsReference()
{
  return myLastType == BinTools_ObjectType_Reference8
         || myLastType == BinTools_ObjectType_Reference16
         || myLastType == BinTools_ObjectType_Reference32
         || myLastType == BinTools_ObjectType_Reference64;
}

//=================================================================================================

uint64_t BinTools_IStream::ReadReference()
{
  uint64_t aDelta      = 0;
  uint64_t aCurrentPos = uint64_t(myStream->tellg());
  switch (myLastType)
  {
    case BinTools_ObjectType_Reference8:
      aDelta = uint64_t(myStream->get());
      myPosition++;
      break;
    case BinTools_ObjectType_Reference16: {
      uint16_t aDelta16 = 0;
      myStream->read((char*)&aDelta16, sizeof(uint16_t));
      myPosition += 2;
#if DO_INVERSE
      aDelta16 = (0 | ((aDelta16 & 0x00FF) << 8) | ((aDelta16 & 0xFF00) >> 8));
#endif
      aDelta = uint64_t(aDelta16);
      break;
    }
    case BinTools_ObjectType_Reference32: {
      uint32_t aDelta32 = 0;
      myStream->read((char*)&aDelta32, sizeof(uint32_t));
      myPosition += 4;
#if DO_INVERSE
      aDelta32 = (0 | ((aDelta32 & 0x000000ff) << 24) | ((aDelta32 & 0x0000ff00) << 8)
                  | ((aDelta32 & 0x00ff0000) >> 8) | ((aDelta32 >> 24) & 0x000000ff));
#endif
      aDelta = uint64_t(aDelta32);
      break;
    }
    case BinTools_ObjectType_Reference64:
      myStream->read((char*)&aDelta, sizeof(uint64_t));
      myPosition += 8;
#if DO_INVERSE
      aDelta = InverseUint64(aDelta);
#endif
      break;
    default:
      break;
  }
  if (aDelta == 0)
  {
    Standard_SStream aMsg;
    aMsg << "BinTools_IStream::ReadReference: invalid reference " << (char)myLastType << std::endl;
    throw Standard_Failure(aMsg.str().c_str());
  }
  return aCurrentPos - aDelta - 1; // add a type-byte
}

//=================================================================================================

void BinTools_IStream::GoTo(const uint64_t& thePosition)
{
  myStream->seekg(std::streampos(thePosition));
  myPosition = thePosition;
}

//=================================================================================================

TopAbs_ShapeEnum BinTools_IStream::ShapeType()
{
  return TopAbs_ShapeEnum(
    (Standard_Byte(myLastType) - Standard_Byte(BinTools_ObjectType_EndShape) - 1) >> 2);
}

//=================================================================================================

TopAbs_Orientation BinTools_IStream::ShapeOrientation()
{
  return TopAbs_Orientation(
    (Standard_Byte(myLastType) - Standard_Byte(BinTools_ObjectType_EndShape) - 1) & 3);
}

//=================================================================================================

BinTools_IStream::operator bool() const
{
  return *myStream ? Standard_True : Standard_False;
}

//=======================================================================
// function : operator <<
// purpose  :
//=======================================================================
BinTools_IStream& BinTools_IStream::operator>>(Standard_Real& theValue)
{
  if (!myStream->read((char*)&theValue, sizeof(Standard_Real)))
    throw Storage_StreamTypeMismatchError();
  myPosition += sizeof(Standard_Real);
#if DO_INVERSE
  theValue = InverseReal(theValue);
#endif
  return *this;
}

//=======================================================================
// function : operator <<
// purpose  :
//=======================================================================
BinTools_IStream& BinTools_IStream::operator>>(Standard_Integer& theValue)
{
  if (!myStream->read((char*)&theValue, sizeof(Standard_Integer)))
    throw Storage_StreamTypeMismatchError();
  myPosition += sizeof(Standard_Integer);
#if DO_INVERSE
  theValue = InverseInt(theValue);
#endif
  return *this;
}

//=======================================================================
// function : operator <<
// purpose  :
//=======================================================================
BinTools_IStream& BinTools_IStream::operator>>(gp_Pnt& theValue)
{
  Standard_Real aValue;
  for (int aCoord = 1; aCoord <= 3; aCoord++)
  {
    if (!myStream->read((char*)&aValue, sizeof(Standard_Real)))
      throw Storage_StreamTypeMismatchError();
#if DO_INVERSE
    aValue = InverseReal(aValue);
#endif
    theValue.SetCoord(aCoord, aValue);
  }
  myPosition += 3 * sizeof(Standard_Real);
  return *this;
}

//=======================================================================
// function : operator <<
// purpose  :
//=======================================================================
BinTools_IStream& BinTools_IStream::operator>>(Standard_Byte& theValue)
{
  myStream->read((char*)&theValue, sizeof(Standard_Byte));
  myPosition += sizeof(Standard_Byte);
  return *this;
}

//=======================================================================
// function : operator <<
// purpose  :
//=======================================================================
BinTools_IStream& BinTools_IStream::operator>>(Standard_ShortReal& theValue)
{
  myStream->read((char*)&theValue, sizeof(Standard_ShortReal));
  myPosition += sizeof(Standard_ShortReal);
  return *this;
}

//=======================================================================
// function : operator <<
// purpose  :
//=======================================================================
BinTools_IStream& BinTools_IStream::operator>>(gp_Trsf& theValue)
{
  Standard_Real aV1[3], aV2[3], aV3[3], aV[3];
  *this >> aV1[0] >> aV1[1] >> aV1[2] >> aV[0];
  *this >> aV2[0] >> aV2[1] >> aV2[2] >> aV[1];
  *this >> aV3[0] >> aV3[1] >> aV3[2] >> aV[2];
  theValue.SetValues(aV1[0],
                     aV1[1],
                     aV1[2],
                     aV[0],
                     aV2[0],
                     aV2[1],
                     aV2[2],
                     aV[1],
                     aV3[0],
                     aV3[1],
                     aV3[2],
                     aV[2]);
  return *this;
}

//=================================================================================================

void BinTools_IStream::ReadBools(Standard_Boolean& theBool1,
                                 Standard_Boolean& theBool2,
                                 Standard_Boolean& theBool3)
{
  Standard_Byte aByte = ReadByte();
  theBool1            = (aByte & 1) == 1;
  theBool2            = (aByte & 2) == 2;
  theBool3            = (aByte & 4) == 4;
}

//=================================================================================================

void BinTools_IStream::ReadBools(Standard_Boolean& theBool1,
                                 Standard_Boolean& theBool2,
                                 Standard_Boolean& theBool3,
                                 Standard_Boolean& theBool4,
                                 Standard_Boolean& theBool5,
                                 Standard_Boolean& theBool6,
                                 Standard_Boolean& theBool7)
{
  Standard_Byte aByte = ReadByte();
  theBool1            = (aByte & 1) == 1;
  theBool2            = (aByte & 2) == 2;
  theBool3            = (aByte & 4) == 4;
  theBool4            = (aByte & 8) == 8;
  theBool5            = (aByte & 16) == 16;
  theBool6            = (aByte & 32) == 32;
  theBool7            = (aByte & 64) == 64;
}
