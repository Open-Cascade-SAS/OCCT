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

#ifndef _BinObjMgt_Position_HeaderFile
#define _BinObjMgt_Position_HeaderFile

#include <Standard_Type.hxx>

class BinObjMgt_Position;
DEFINE_STANDARD_HANDLE (BinObjMgt_Position, Standard_Transient)

//! Stores and manipulates position in the stream.
class BinObjMgt_Position : public Standard_Transient
{
public:

  DEFINE_STANDARD_ALLOC

  //! Creates position using the current stream position.
  Standard_EXPORT BinObjMgt_Position (Standard_OStream& theStream);

  //! Stores the difference between the current position and the stored one.
  Standard_EXPORT void StoreSize (Standard_OStream& theStream);
  //! Writes stored size at the stored position. Changes the current stream position.
//! If theDummy is true, is writes to the current position zero size.
  Standard_EXPORT void WriteSize (Standard_OStream& theStream, const Standard_Boolean theDummy = Standard_False);

  DEFINE_STANDARD_RTTIEXT (BinObjMgt_Position, Standard_Transient)

private:
  std::streampos myPosition;
  uint64_t mySize;
};

#endif // _BinObjMgt_Position_HeaderFile
