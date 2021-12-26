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

#include <BinTools_OStream.hxx>

#if DO_INVERSE
#include <FSD_BinaryFile.hxx>
#endif

//=======================================================================
//function : BinTools_OStream
//purpose  : 
//=======================================================================
BinTools_OStream::BinTools_OStream (Standard_OStream& theStream)
  : myStream (&theStream), myPosition (theStream.tellp())
{}

//=======================================================================
//function : WriteReference
//purpose  : 
//=======================================================================
void BinTools_OStream::WriteReference (const uint64_t& thePosition)
{
  uint64_t aDelta = myPosition - thePosition;
  if (aDelta <= 0xFF)
  {
    *myStream << (Standard_Byte)BinTools_ObjectType_Reference8;
    *myStream << (Standard_Byte)aDelta;
    myPosition += sizeof (Standard_Byte) * 2;
  }
  else if (aDelta <= 0xFFFF)
  {
    *myStream << (Standard_Byte)BinTools_ObjectType_Reference16;
    uint16_t aDelta16 = uint16_t (aDelta);
#if DO_INVERSE
    aDelta16 = (0 | ((aDelta16 & 0x00FF) << 8)
      | ((aDelta16 & 0xFF00) >> 8));
#endif
    myStream->write ((char*)&aDelta16, sizeof (uint16_t));
    myPosition += sizeof (Standard_Byte)  + sizeof (uint16_t);
  }
  else if (aDelta <= 0xFFFFFFFF)
  {
    *myStream << (Standard_Byte)BinTools_ObjectType_Reference32;
    uint32_t aDelta32 = uint32_t (aDelta);
#if DO_INVERSE
    aDelta32 = (0 | ((aDelta32 & 0x000000ff) << 24)
      | ((aDelta32 & 0x0000ff00) << 8)
      | ((aDelta32 & 0x00ff0000) >> 8)
      | ((aDelta32 >> 24) & 0x000000ff) );
#endif
    myStream->write ((char*)&aDelta32, sizeof (uint32_t));
    myPosition += sizeof (Standard_Byte)  + sizeof (uint32_t);
  }
  else
  {
    *myStream << (Standard_Byte)BinTools_ObjectType_Reference64;
#if DO_INVERSE
    aDelta = FSD_BinaryFile::InverseUint64 (aDelta);
#endif
    myStream->write ((char*)&aDelta, sizeof (uint64_t));
    myPosition += sizeof (Standard_Byte)  + sizeof (uint64_t);
  }
}

//=======================================================================
//function : WriteShape
//purpose  : 
//=======================================================================
void BinTools_OStream::WriteShape (const TopAbs_ShapeEnum& theType, const TopAbs_Orientation& theOrientation)
{
  Standard_Byte aType = Standard_Byte (BinTools_ObjectType_EndShape) + 1 +   // taking into account that orientation <= 3
    (Standard_Byte (theType) << 2) + Standard_Byte (theOrientation); // and type <= 8
  myStream->put ((Standard_Byte)aType);
  myPosition += sizeof (Standard_Byte);
}

//=======================================================================
//function : operator <<
//purpose  : 
//=======================================================================
BinTools_OStream& BinTools_OStream::operator << (const BinTools_ObjectType& theType)
{
  myStream->put ((Standard_Byte)theType);
  myPosition += sizeof (Standard_Byte);
  return *this;
}

//=======================================================================
//function : operator <<
//purpose  : 
//=======================================================================
BinTools_OStream& BinTools_OStream::operator << (const Standard_Byte& theValue)
{
  myStream->put (theValue);
  myPosition += sizeof (Standard_Byte);
  return *this;
}

//=======================================================================
//function : operator <<
//purpose  : 
//=======================================================================
BinTools_OStream& BinTools_OStream::operator << (const Standard_Real& theValue)
{
#if DO_INVERSE
  const Standard_Real aRValue = FSD_BinaryFile::InverseReal (theValue);
  myStream->write ((char*)&aRValue, sizeof (Standard_Real));
#else
  myStream->write ((char*)&theValue, sizeof (Standard_Real));
#endif
  myPosition += sizeof (Standard_Real);
  return *this;
}

//=======================================================================
//function : operator <<
//purpose  : 
//=======================================================================
BinTools_OStream& BinTools_OStream::operator << (const Standard_Boolean& theValue)
{
  myStream->put ((Standard_Byte)(theValue ? 1 : 0));
  myPosition += sizeof (Standard_Byte);
  return *this;
}

//=======================================================================
//function : operator <<
//purpose  : 
//=======================================================================
BinTools_OStream& BinTools_OStream::operator << (const Standard_Integer& theValue)
{
#if DO_INVERSE
  const Standard_Integer aRValue = FSD_BinaryFile::InverseInt (theValue);
  myStream->write ((char*)&aRValue, sizeof (Standard_Integer));
#else
  myStream->write ((char*)&theValue, sizeof (Standard_Integer));
#endif
  myPosition += sizeof (Standard_Integer);
  return *this;
}

//=======================================================================
//function : operator <<
//purpose  : 
//=======================================================================
BinTools_OStream& BinTools_OStream::operator << (const Standard_ExtCharacter& theValue)
{
#if DO_INVERSE
  const Standard_ExtCharacter aRValue = FSD_BinaryFile::InverseExtChar (theValue);
  myStream->write ((char*)&aRValue, sizeof (Standard_ExtCharacter));
#else
  myStream->write ((char*)&theValue, sizeof (Standard_ExtCharacter));
#endif
  myPosition += sizeof (Standard_ExtCharacter);
  return *this;
}

//=======================================================================
//function : operator <<
//purpose  : 
//=======================================================================
BinTools_OStream& BinTools_OStream::operator << (const gp_Pnt& theValue)
{
#if DO_INVERSE
  myRealBuf[0] = FSD_BinaryFile::InverseReal (theValue.X());
  myRealBuf[1] = FSD_BinaryFile::InverseReal (theValue.Y());
  myRealBuf[2] = FSD_BinaryFile::InverseReal (theValue.Z());
#else
  myRealBuf[0] = theValue.X();
  myRealBuf[1] = theValue.Y();
  myRealBuf[2] = theValue.Z();
#endif
  myStream->write ((char*)myRealBuf, sizeof (Standard_Real) * 3);
  myPosition += sizeof (Standard_Real) * 3;
  return *this;
}

//=======================================================================
//function : operator <<
//purpose  : 
//=======================================================================
BinTools_OStream& BinTools_OStream::operator << (const gp_Dir& theValue)
{
#if DO_INVERSE
  myRealBuf[0] = FSD_BinaryFile::InverseReal (theValue.X());
  myRealBuf[1] = FSD_BinaryFile::InverseReal (theValue.Y());
  myRealBuf[2] = FSD_BinaryFile::InverseReal (theValue.Z());
#else
  myRealBuf[0] = theValue.X();
  myRealBuf[1] = theValue.Y();
  myRealBuf[2] = theValue.Z();
#endif
  myStream->write ((char*)myRealBuf, sizeof (Standard_Real) * 3);
  myPosition += sizeof (Standard_Real) * 3;
  return *this;
}

//=======================================================================
//function : operator <<
//purpose  : 
//=======================================================================
BinTools_OStream& BinTools_OStream::operator << (const gp_Pnt2d& theValue)
{
#if DO_INVERSE
  myRealBuf[0] = FSD_BinaryFile::InverseReal (theValue.X());
  myRealBuf[1] = FSD_BinaryFile::InverseReal (theValue.Y());
#else
  myRealBuf[0] = theValue.X();
  myRealBuf[1] = theValue.Y();
#endif
  myStream->write ((char*)myRealBuf, sizeof (Standard_Real) * 2);
  myPosition += sizeof (Standard_Real) * 2;
  return *this;
}

//=======================================================================
//function : operator <<
//purpose  : 
//=======================================================================
BinTools_OStream& BinTools_OStream::operator << (const gp_Dir2d& theValue)
{
#if DO_INVERSE
  myRealBuf[0] = FSD_BinaryFile::InverseReal (theValue.X());
  myRealBuf[1] = FSD_BinaryFile::InverseReal (theValue.Y());
#else
  myRealBuf[0] = theValue.X();
  myRealBuf[1] = theValue.Y();
#endif
  myStream->write ((char*)myRealBuf, sizeof (Standard_Real) * 2);
  myPosition += sizeof (Standard_Real) * 2;
  return *this;
}

//=======================================================================
//function : operator <<
//purpose  : 
//=======================================================================
BinTools_OStream& BinTools_OStream::operator << (const gp_Trsf& theValue)
{
  gp_XYZ aTr = theValue.TranslationPart();
  gp_Mat aMat = theValue.VectorialPart();
#if DO_INVERSE
  myRealBuf[0] = FSD_BinaryFile::InverseReal (aMat (1, 1));
  myRealBuf[1] = FSD_BinaryFile::InverseReal (aMat (1, 2));
  myRealBuf[2] = FSD_BinaryFile::InverseReal (aMat (1, 3));
  myRealBuf[3] = FSD_BinaryFile::InverseReal (aTr.Coord (1));
  myRealBuf[4] = FSD_BinaryFile::InverseReal (aMat (2, 1));
  myRealBuf[5] = FSD_BinaryFile::InverseReal (aMat (2, 2));
  myRealBuf[6] = FSD_BinaryFile::InverseReal (aMat (2, 3));
  myRealBuf[7] = FSD_BinaryFile::InverseReal (aTr.Coord (2));
  myRealBuf[8] = FSD_BinaryFile::InverseReal (aMat (3, 1));
  myRealBuf[9] = FSD_BinaryFile::InverseReal (aMat (3, 2));
  myRealBuf[10] = FSD_BinaryFile::InverseReal (aMat (3, 3));
  myRealBuf[11] = FSD_BinaryFile::InverseReal (aTr.Coord (3));
#else
  myRealBuf[0] = aMat (1, 1);
  myRealBuf[1] = aMat (1, 2);
  myRealBuf[2] = aMat (1, 3);
  myRealBuf[3] = aTr.Coord (1);
  myRealBuf[4] = aMat (2, 1);
  myRealBuf[5] = aMat (2, 2);
  myRealBuf[6] = aMat (2, 3);
  myRealBuf[7] = aTr.Coord (2);
  myRealBuf[8] = aMat (3, 1);
  myRealBuf[9] = aMat (3, 2);
  myRealBuf[10] = aMat (3, 3);
  myRealBuf[11] = aTr.Coord (3);
#endif
  myStream->write ((char*)myRealBuf, sizeof (Standard_Real) * 12);
  myPosition += sizeof (Standard_Real) * 12;
  return *this;
}

//=======================================================================
//function : operator <<
//purpose  : 
//=======================================================================
BinTools_OStream& BinTools_OStream::operator << (const Poly_Triangle& theValue)
{
  theValue.Value(1);
#if DO_INVERSE
  myIntBuf[0] = FSD_BinaryFile::InverseInt (theValue.Value(1));
  myIntBuf[1] = FSD_BinaryFile::InverseInt (theValue.Value(2));
  myIntBuf[2] = FSD_BinaryFile::InverseInt (theValue.Value(3));
#else
  myIntBuf[0] = theValue.Value(1);
  myIntBuf[1] = theValue.Value(2);
  myIntBuf[2] = theValue.Value(3);
#endif
  myStream->write ((char*)myIntBuf, sizeof (Standard_Integer) * 3);
  myPosition += sizeof (Standard_Integer) * 3;
  return *this;
}

//=======================================================================
//function : operator <<
//purpose  : 
//=======================================================================
BinTools_OStream& BinTools_OStream::operator << (const gp_Vec3f& theValue)
{
#if DO_INVERSE
  myFloatBuf[0] = FSD_BinaryFile::InverseShortReal (theValue.x());
  myFloatBuf[1] = FSD_BinaryFile::InverseShortReal (theValue.y());
  myFloatBuf[2] = FSD_BinaryFile::InverseShortReal (theValue.z());
#else
  myFloatBuf[0] = theValue.x();
  myFloatBuf[1] = theValue.y();
  myFloatBuf[2] = theValue.z();
#endif
  myStream->write ((char*)myFloatBuf, sizeof (float) * 3);
  myPosition += sizeof (float) * 3;
  return *this;
}

//=======================================================================
//function : PutBools
//purpose  : 
//=======================================================================
void BinTools_OStream::PutBools (
  const Standard_Boolean theValue1, const Standard_Boolean theValue2, const Standard_Boolean theValue3)
{
  Standard_Byte aValue = (theValue1 ? 1 : 0) | (theValue2 ? 2 : 0) | (theValue3 ? 4 : 0);
  myStream->write((char*)&aValue, sizeof(Standard_Byte));
  myPosition += sizeof(Standard_Byte);
}

//=======================================================================
//function : PutBools
//purpose  : 
//=======================================================================
void BinTools_OStream::PutBools (
  const Standard_Boolean theValue1, const Standard_Boolean theValue2, const Standard_Boolean theValue3,
  const Standard_Boolean theValue4, const Standard_Boolean theValue5, const Standard_Boolean theValue6,
  const Standard_Boolean theValue7)
{
  Standard_Byte aValue = (theValue1 ? 1 : 0) | (theValue2 ? 2 : 0) | (theValue3 ? 4 : 0) | (theValue4 ? 8 : 0) |
    (theValue5 ? 16 : 0) | (theValue6 ? 32 : 0) | (theValue7 ? 64 : 0);
  myStream->write((char*)&aValue, sizeof(Standard_Byte));
  myPosition += sizeof(Standard_Byte);
}
