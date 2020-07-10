// Created on: 2004-05-18
// Created by: Sergey ZARITCHNY
// Copyright (c) 2004-2014 OPEN CASCADE SAS
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


#include <BinTools.hxx>
#include <BinTools_ShapeSet.hxx>
#include <FSD_FileHeader.hxx>
#include <OSD_OpenFile.hxx>
#include <Storage_StreamTypeMismatchError.hxx>

//=======================================================================
//function : PutBool
//purpose  : 
//=======================================================================
Standard_OStream& BinTools::PutBool(Standard_OStream& OS, const Standard_Boolean aValue)
{
  OS.put((Standard_Byte)(aValue ? 1 : 0));
  return OS;
}


//=======================================================================
//function : PutInteger
//purpose  : 
//=======================================================================

Standard_OStream& BinTools::PutInteger(Standard_OStream& OS, const Standard_Integer aValue)
{
  Standard_Integer anIntValue = aValue;
#if DO_INVERSE
      anIntValue = InverseInt (aValue);
#endif
  OS.write((char*)&anIntValue, sizeof(Standard_Integer));  
  return OS;
}


//=======================================================================
//function : PutReal
//purpose  :
//=======================================================================
Standard_OStream& BinTools::PutReal (Standard_OStream& theOS,
                                     const Standard_Real& theValue)
{
#if DO_INVERSE
  const Standard_Real aRValue = InverseReal (theValue);
  theOS.write((char*)&aRValue, sizeof(Standard_Real));
#else
  theOS.write((char*)&theValue, sizeof(Standard_Real));
#endif
  return theOS;
}

//=======================================================================
//function : PutShortReal
//purpose  :
//=======================================================================
Standard_OStream& BinTools::PutShortReal (Standard_OStream& theOS,
                                          const Standard_ShortReal& theValue)
{
#if DO_INVERSE
  const Standard_ShortReal aValue = InverseShortReal (theValue);
  theOS.write ((char*)&aValue, sizeof(Standard_ShortReal));
#else
  theOS.write ((char*)&theValue, sizeof(Standard_ShortReal));
#endif
  return theOS;
}

//=======================================================================
//function : PutExtChar
//purpose  : 
//=======================================================================

Standard_OStream& BinTools::PutExtChar(Standard_OStream& OS, const Standard_ExtCharacter aValue)
{
  Standard_ExtCharacter aSValue = aValue;
#if DO_INVERSE
      aSValue = InverseExtChar (aValue);
#endif
  OS.write((char*)&aSValue, sizeof(Standard_ExtCharacter));  
  return OS;
}

//=======================================================================
//function : GetReal
//purpose  :
//=======================================================================
Standard_IStream& BinTools::GetReal (Standard_IStream& theIS,
                                     Standard_Real& theValue)
{
  if (!theIS.read ((char*)&theValue, sizeof(Standard_Real)))
  {
    throw Storage_StreamTypeMismatchError();
  }
#if DO_INVERSE
  theValue = InverseReal (theValue);
#endif
  return theIS;
}

//=======================================================================
//function : GetShortReal
//purpose  :
//=======================================================================
Standard_IStream& BinTools::GetShortReal (Standard_IStream& theIS,
                                          Standard_ShortReal& theValue)
{
  if (!theIS.read ((char*)&theValue, sizeof(Standard_ShortReal)))
  {
    throw Storage_StreamTypeMismatchError();
  }
#if DO_INVERSE
  theValue = InverseShortReal (theValue);
#endif
  return theIS;
}

//=======================================================================
//function : GetInteger
//purpose  : 
//=======================================================================

Standard_IStream& BinTools::GetInteger(Standard_IStream& IS, Standard_Integer& aValue)
{
  if(!IS.read ((char*)&aValue, sizeof(Standard_Integer)))
    throw Storage_StreamTypeMismatchError();
#if DO_INVERSE
  aValue = InverseInt (aValue);
#endif
  return IS;
}

//=======================================================================
//function : GetExtChar
//purpose  : 
//=======================================================================

Standard_IStream& BinTools::GetExtChar(Standard_IStream& IS, Standard_ExtCharacter& theValue)
{
  if(!IS.read ((char*)&theValue, sizeof(Standard_ExtCharacter)))
    throw Storage_StreamTypeMismatchError();
#if DO_INVERSE
  theValue = InverseExtChar (theValue);
#endif
  return IS;
}

//=======================================================================
//function : GetBool
//purpose  : 
//=======================================================================

Standard_IStream& BinTools::GetBool(Standard_IStream& IS, Standard_Boolean& aValue)
{
  aValue = (IS.get() != 0);
  return IS;
}

//=======================================================================
//function : Write
//purpose  : 
//=======================================================================

void BinTools::Write (const TopoDS_Shape& theShape, Standard_OStream& theStream,
                      const Message_ProgressRange& theRange)
{
  BinTools_ShapeSet aShapeSet(Standard_True);
  aShapeSet.SetFormatNb (3);
  aShapeSet.Add (theShape);
  aShapeSet.Write (theStream, theRange);
  aShapeSet.Write (theShape, theStream);
}

//=======================================================================
//function : Read
//purpose  : 
//=======================================================================

void BinTools::Read (TopoDS_Shape& theShape, Standard_IStream& theStream,
                     const Message_ProgressRange& theRange)
{
  BinTools_ShapeSet aShapeSet(Standard_True);
  aShapeSet.Read (theStream, theRange);
  aShapeSet.Read (theShape, theStream, aShapeSet.NbShapes());
}

//=======================================================================
//function : Write
//purpose  : 
//=======================================================================

Standard_Boolean BinTools::Write (const TopoDS_Shape& theShape, const Standard_CString theFile,
                                  const Message_ProgressRange& theRange)
{
  std::ofstream aStream;
  aStream.precision (15);
  OSD_OpenStream (aStream, theFile, std::ios::out | std::ios::binary);
  if (!aStream.good())
    return Standard_False;

  Write (theShape, aStream, theRange);
  aStream.close();
  return aStream.good();
}

//=======================================================================
//function : Read
//purpose  : 
//=======================================================================

Standard_Boolean BinTools::Read (TopoDS_Shape& theShape, const Standard_CString theFile,
                                 const Message_ProgressRange& theRange)
{
  std::filebuf aBuf;
  OSD_OpenStream (aBuf, theFile, std::ios::in | std::ios::binary);
  if (!aBuf.is_open())
    return Standard_False;

  Standard_IStream aStream (&aBuf);
  Read (theShape, aStream, theRange);
  return aStream.good();
}
