// Created on: 2012-07-18
// Created by: Kirill GAVRILOV
// Copyright (c) 2012-2014 OPEN CASCADE SAS
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

#ifndef Image_PixMapData_HeaderFile
#define Image_PixMapData_HeaderFile

#include <Image_Color.hxx>
#include <NCollection_Buffer.hxx>
#include <NCollection_Vec3.hxx>

//! Structure to manage image buffer.
class Image_PixMapData : public NCollection_Buffer
{
public:
  //! Empty constructor.
  Image_PixMapData()
      : NCollection_Buffer(occ::handle<NCollection_BaseAllocator>()),
        myTopRowPtr(NULL),
        SizeBPP(0),
        SizeX(0),
        SizeY(0),
        SizeZ(0),
        SizeRowBytes(0),
        SizeSliceBytes(0),
        TopToDown(size_t(-1))
  {
    //
  }

  //! Initializer.
  bool Init(const occ::handle<NCollection_BaseAllocator>& theAlloc,
            const size_t                                  theSizeBPP,
            const size_t                                  theSizeX,
            const size_t                                  theSizeY,
            const size_t                                  theSizeRowBytes,
            uint8_t*                                      theDataPtr)
  {
    return Init(theAlloc,
                theSizeBPP,
                NCollection_Vec3<size_t>(theSizeX, theSizeY, 1),
                theSizeRowBytes,
                theDataPtr);
  }

  //! Initializer.
  bool Init(const occ::handle<NCollection_BaseAllocator>& theAlloc,
            const size_t                                  theSizeBPP,
            const NCollection_Vec3<size_t>&               theSizeXYZ,
            const size_t                                  theSizeRowBytes,
            uint8_t*                                      theDataPtr)
  {
    SetAllocator(theAlloc); // will free old data as well

    myData         = theDataPtr;
    myTopRowPtr    = NULL;
    SizeBPP        = theSizeBPP;
    SizeX          = theSizeXYZ.x();
    SizeY          = theSizeXYZ.y();
    SizeZ          = theSizeXYZ.z();
    SizeRowBytes   = theSizeRowBytes != 0 ? theSizeRowBytes : (SizeX * theSizeBPP);
    SizeSliceBytes = SizeRowBytes * SizeY;
    mySize         = SizeSliceBytes * SizeZ;
    if (myData == NULL)
    {
      Allocate(mySize);
    }
    SetTopDown(TopToDown == 1);
    return !IsEmpty();
  }

  //! Reset all values to zeros.
  void ZeroData()
  {
    if (myData != NULL)
    {
      memset(myData, 0, mySize);
    }
  }

  //! Return data pointer to requested row (first column).
  const uint8_t* Row(const size_t theRow) const
  {
    return myTopRowPtr + ptrdiff_t(SizeRowBytes * theRow * TopToDown);
  }

  //! Return data pointer to requested row (first column).
  uint8_t* ChangeRow(const size_t theRow)
  {
    return myTopRowPtr + ptrdiff_t(SizeRowBytes * theRow * TopToDown);
  }

  //! Return data pointer to requested position.
  const uint8_t* Value(const size_t theRow, const size_t theCol) const
  {
    return myTopRowPtr + ptrdiff_t(SizeRowBytes * theRow * TopToDown) + SizeBPP * theCol;
  }

  //! Return data pointer to requested position.
  uint8_t* ChangeValue(size_t theRow, size_t theCol)
  {
    return myTopRowPtr + ptrdiff_t(SizeRowBytes * theRow * TopToDown) + SizeBPP * theCol;
  }

  //! Return data pointer to requested position.
  const uint8_t* ValueXY(size_t theX, size_t theY) const
  {
    return myTopRowPtr + ptrdiff_t(SizeRowBytes * theY * TopToDown) + SizeBPP * theX;
  }

  //! Return data pointer to requested position.
  uint8_t* ChangeValueXY(size_t theX, size_t theY)
  {
    return myTopRowPtr + ptrdiff_t(SizeRowBytes * theY * TopToDown) + SizeBPP * theX;
  }

public:
  //! Return data pointer to requested 2D slice.
  const uint8_t* Slice(size_t theSlice) const
  {
    return myData + ptrdiff_t(SizeSliceBytes * theSlice);
  }

  //! Return data pointer to requested 2D slice.
  uint8_t* ChangeSlice(size_t theSlice) { return myData + ptrdiff_t(SizeSliceBytes * theSlice); }

  //! Return data pointer to requested row (first column).
  const uint8_t* SliceRow(size_t theSlice, size_t theRow) const
  {
    return myTopRowPtr + ptrdiff_t(SizeRowBytes * theRow * TopToDown)
           + ptrdiff_t(SizeSliceBytes * theSlice);
  }

  //! Return data pointer to requested row (first column).
  uint8_t* ChangeSliceRow(size_t theSlice, size_t theRow)
  {
    return myTopRowPtr + ptrdiff_t(SizeRowBytes * theRow * TopToDown)
           + ptrdiff_t(SizeSliceBytes * theSlice);
  }

  //! Return data pointer to requested position.
  const uint8_t* ValueXYZ(size_t theX, size_t theY, size_t theZ) const
  {
    return myTopRowPtr + ptrdiff_t(SizeRowBytes * theY * TopToDown) + SizeBPP * theX
           + ptrdiff_t(SizeSliceBytes * theZ);
  }

  //! Return data pointer to requested position.
  uint8_t* ChangeValueXYZ(size_t theX, size_t theY, size_t theZ)
  {
    return myTopRowPtr + ptrdiff_t(SizeRowBytes * theY * TopToDown) + SizeBPP * theX
           + ptrdiff_t(SizeSliceBytes * theZ);
  }

  //! Compute the maximal row alignment for current row size.
  //! @return maximal row alignment in bytes (up to 16 bytes).
  size_t MaxRowAligmentBytes() const
  {
    size_t anAlignment = 2;
    for (; anAlignment <= 16; anAlignment <<= 1)
    {
      if ((SizeRowBytes % anAlignment) != 0 || (size_t(myData) % anAlignment) != 0)
      {
        return (anAlignment >> 1);
      }
    }
    return anAlignment;
  }

  //! Setup scanlines order in memory - top-down or bottom-up.
  //! Drawers should explicitly specify this value if current state IsTopDown() was ignored!
  //! @param theIsTopDown top-down flag
  void SetTopDown(const bool theIsTopDown)
  {
    TopToDown = (theIsTopDown ? 1 : size_t(-1));
    myTopRowPtr =
      ((TopToDown == 1 || myData == NULL) ? myData : (myData + SizeRowBytes * (SizeY - 1)));
  }

protected:
  // clang-format off
  uint8_t* myTopRowPtr;  //!< pointer to the topmost row (depending on scanlines order in memory)
  // clang-format on

public:
  size_t SizeBPP;        //!< bytes per pixel
  size_t SizeX;          //!< width  in pixels
  size_t SizeY;          //!< height in pixels
  size_t SizeZ;          //!< depth  in pixels
  size_t SizeRowBytes;   //!< number of bytes per line (in most cases equal to 3 * sizeX)
  size_t SizeSliceBytes; //!< number of bytes per 2D slice
  size_t TopToDown;      //!< image scanlines direction in memory from Top to the Down

public:
  DEFINE_STANDARD_RTTIEXT(Image_PixMapData, NCollection_Buffer)
};

#endif // _Image_PixMapData_H__
