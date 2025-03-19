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
#include <OSD_FileSystem.hxx>
#include <Storage_StreamTypeMismatchError.hxx>

//=================================================================================================

Standard_OStream& BinTools::PutBool(Standard_OStream& OS, const Standard_Boolean aValue)
{
  OS.put((Standard_Byte)(aValue ? 1 : 0));
  return OS;
}

//=================================================================================================

Standard_OStream& BinTools::PutInteger(Standard_OStream& OS, const Standard_Integer aValue)
{
  Standard_Integer anIntValue = aValue;
#ifdef DO_INVERSE
  anIntValue = InverseInt(aValue);
#endif
  OS.write((char*)&anIntValue, sizeof(Standard_Integer));
  return OS;
}

//=================================================================================================

Standard_OStream& BinTools::PutReal(Standard_OStream& theOS, const Standard_Real& theValue)
{
#ifdef DO_INVERSE
  const Standard_Real aRValue = InverseReal(theValue);
  theOS.write((char*)&aRValue, sizeof(Standard_Real));
#else
  theOS.write((char*)&theValue, sizeof(Standard_Real));
#endif
  return theOS;
}

//=================================================================================================

Standard_OStream& BinTools::PutShortReal(Standard_OStream&         theOS,
                                         const Standard_ShortReal& theValue)
{
#ifdef DO_INVERSE
  const Standard_ShortReal aValue = InverseShortReal(theValue);
  theOS.write((char*)&aValue, sizeof(Standard_ShortReal));
#else
  theOS.write((char*)&theValue, sizeof(Standard_ShortReal));
#endif
  return theOS;
}

//=================================================================================================

Standard_OStream& BinTools::PutExtChar(Standard_OStream& OS, const Standard_ExtCharacter aValue)
{
  Standard_ExtCharacter aSValue = aValue;
#ifdef DO_INVERSE
  aSValue = InverseExtChar(aValue);
#endif
  OS.write((char*)&aSValue, sizeof(Standard_ExtCharacter));
  return OS;
}

//=================================================================================================

Standard_IStream& BinTools::GetReal(Standard_IStream& theIS, Standard_Real& theValue)
{
  if (!theIS.read((char*)&theValue, sizeof(Standard_Real)))
  {
    throw Storage_StreamTypeMismatchError();
  }
#ifdef DO_INVERSE
  theValue = InverseReal(theValue);
#endif
  return theIS;
}

//=================================================================================================

Standard_IStream& BinTools::GetShortReal(Standard_IStream& theIS, Standard_ShortReal& theValue)
{
  if (!theIS.read((char*)&theValue, sizeof(Standard_ShortReal)))
  {
    throw Storage_StreamTypeMismatchError();
  }
#ifdef DO_INVERSE
  theValue = InverseShortReal(theValue);
#endif
  return theIS;
}

//=================================================================================================

Standard_IStream& BinTools::GetInteger(Standard_IStream& IS, Standard_Integer& aValue)
{
  if (!IS.read((char*)&aValue, sizeof(Standard_Integer)))
    throw Storage_StreamTypeMismatchError();
#ifdef DO_INVERSE
  aValue = InverseInt(aValue);
#endif
  return IS;
}

//=================================================================================================

Standard_IStream& BinTools::GetExtChar(Standard_IStream& IS, Standard_ExtCharacter& theValue)
{
  if (!IS.read((char*)&theValue, sizeof(Standard_ExtCharacter)))
    throw Storage_StreamTypeMismatchError();
#ifdef DO_INVERSE
  theValue = InverseExtChar(theValue);
#endif
  return IS;
}

//=================================================================================================

Standard_IStream& BinTools::GetBool(Standard_IStream& IS, Standard_Boolean& aValue)
{
  aValue = (IS.get() != 0);
  return IS;
}

//=================================================================================================

void BinTools::Write(const TopoDS_Shape&          theShape,
                     Standard_OStream&            theStream,
                     const Standard_Boolean       theWithTriangles,
                     const Standard_Boolean       theWithNormals,
                     const BinTools_FormatVersion theVersion,
                     const Message_ProgressRange& theRange)
{
  BinTools_ShapeSet aShapeSet;
  aShapeSet.SetWithTriangles(theWithTriangles);
  aShapeSet.SetWithNormals(theWithNormals);
  aShapeSet.SetFormatNb(theVersion);
  aShapeSet.Add(theShape);
  aShapeSet.Write(theStream, theRange);
  aShapeSet.Write(theShape, theStream);
}

//=================================================================================================

void BinTools::Read(TopoDS_Shape&                theShape,
                    Standard_IStream&            theStream,
                    const Message_ProgressRange& theRange)
{
  BinTools_ShapeSet aShapeSet;
  aShapeSet.SetWithTriangles(Standard_True);
  aShapeSet.Read(theStream, theRange);
  aShapeSet.ReadSubs(theShape, theStream, aShapeSet.NbShapes());
}

//=================================================================================================

Standard_Boolean BinTools::Write(const TopoDS_Shape&          theShape,
                                 const Standard_CString       theFile,
                                 const Standard_Boolean       theWithTriangles,
                                 const Standard_Boolean       theWithNormals,
                                 const BinTools_FormatVersion theVersion,
                                 const Message_ProgressRange& theRange)
{
  const Handle(OSD_FileSystem)& aFileSystem = OSD_FileSystem::DefaultFileSystem();
  std::shared_ptr<std::ostream> aStream =
    aFileSystem->OpenOStream(theFile, std::ios::out | std::ios::binary);
  aStream->precision(15);
  if (aStream.get() == NULL || !aStream->good())
    return Standard_False;

  Write(theShape, *aStream, theWithTriangles, theWithNormals, theVersion, theRange);
  aStream->flush();
  return aStream->good();
}

//=================================================================================================

Standard_Boolean BinTools::Read(TopoDS_Shape&                theShape,
                                const Standard_CString       theFile,
                                const Message_ProgressRange& theRange)
{
  const Handle(OSD_FileSystem)& aFileSystem = OSD_FileSystem::DefaultFileSystem();
  std::shared_ptr<std::istream> aStream =
    aFileSystem->OpenIStream(theFile, std::ios::in | std::ios::binary);
  if (aStream.get() == NULL)
  {
    return Standard_False;
  }

  Read(theShape, *aStream, theRange);
  return aStream->good();
}
