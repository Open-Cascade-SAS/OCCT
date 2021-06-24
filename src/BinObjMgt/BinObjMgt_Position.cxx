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

#include <BinObjMgt_Position.hxx>

IMPLEMENT_STANDARD_RTTIEXT (BinObjMgt_Position, Standard_Transient)

//=======================================================================
//function : BinObjMgt_Position
//purpose  : 
//=======================================================================
BinObjMgt_Position::BinObjMgt_Position (Standard_OStream& theStream) :
  myPosition (theStream.tellp()), mySize(0)
{}

//=======================================================================
//function : StoreSize
//purpose  : 
//=======================================================================
void BinObjMgt_Position::StoreSize (Standard_OStream& theStream)
{
  mySize = uint64_t (theStream.tellp() - myPosition);
}

//=======================================================================
//function : WriteSize
//purpose  : 
//=======================================================================
void BinObjMgt_Position::WriteSize (Standard_OStream& theStream, const Standard_Boolean theDummy)
{
  if (!theDummy && theStream.tellp() != myPosition)
    theStream.seekp (myPosition);
  uint64_t aSize = theDummy ? 0 : mySize;
#if DO_INVERSE
  aSize = FSD_BinaryFile::InverseUint64 (aSize);
#endif
  theStream.write ((char*)&aSize, sizeof (uint64_t));
}
