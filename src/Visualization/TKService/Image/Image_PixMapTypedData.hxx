// Copyright (c) 2019 OPEN CASCADE SAS
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

#ifndef _Image_PixMapTypedData_Header
#define _Image_PixMapTypedData_Header

#include <Image_PixMapData.hxx>

//! Structure to manage image buffer with predefined pixel type.
template <typename PixelType_t>
class Image_PixMapTypedData : public Image_PixMapData
{
public:
  //! Empty constructor.
  Image_PixMapTypedData() {}

  //! Initializer.
  bool Init(const Handle(NCollection_BaseAllocator)& theAlloc,
            Standard_Size                            theSizeX,
            Standard_Size                            theSizeY,
            Standard_Size                            theSizeRowBytes = 0,
            Standard_Byte*                           theDataPtr      = 0)
  {
    const Standard_Size aSizeBPP = sizeof(PixelType_t);
    return Image_PixMapData::Init(theAlloc,
                                  aSizeBPP,
                                  theSizeX,
                                  theSizeY,
                                  theSizeRowBytes,
                                  theDataPtr);
  }

  //! Reset all values to specified one.
  void Init(const PixelType_t& theValue)
  {
    for (Standard_Size aRowIter = 0; aRowIter < SizeY; ++aRowIter)
    {
      for (Standard_Size aColIter = 0; aColIter < SizeX; ++aColIter)
      {
        ChangeValue(aRowIter, aColIter) = theValue;
      }
    }
  }

  //! @return data pointer to requested row (first column).
  const PixelType_t* Row(Standard_Size theRow) const
  {
    return (const PixelType_t*)Image_PixMapData::Row(theRow);
  }

  //! @return data pointer to requested row (first column).
  PixelType_t* ChangeRow(Standard_Size theRow)
  {
    return (PixelType_t*)Image_PixMapData::ChangeRow(theRow);
  }

  //! @return data pointer to requested position.
  const PixelType_t& Value(Standard_Size theRow, Standard_Size theCol) const
  {
    return *(const PixelType_t*)Image_PixMapData::Value(theRow, theCol);
  }

  //! @return data pointer to requested position.
  PixelType_t& ChangeValue(Standard_Size theRow, Standard_Size theCol)
  {
    return *(PixelType_t*)Image_PixMapData::ChangeValue(theRow, theCol);
  }
};

#endif // _Image_PixMapTypedData_Header
