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
  : NCollection_Buffer (Handle(NCollection_BaseAllocator)()),
    myTopRowPtr  (NULL),
    SizeBPP      (0),
    SizeX        (0),
    SizeY        (0),
    SizeZ        (0),
    SizeRowBytes (0),
    SizeSliceBytes (0),
    TopToDown    (Standard_Size(-1))
  {
    //
  }

  //! Initializer.
  bool Init (const Handle(NCollection_BaseAllocator)& theAlloc,
             const Standard_Size                      theSizeBPP,
             const Standard_Size                      theSizeX,
             const Standard_Size                      theSizeY,
             const Standard_Size                      theSizeRowBytes,
             Standard_Byte*                           theDataPtr)
  {
    return Init (theAlloc, theSizeBPP, NCollection_Vec3<Standard_Size> (theSizeX, theSizeY, 1), theSizeRowBytes, theDataPtr);
  }

  //! Initializer.
  bool Init (const Handle(NCollection_BaseAllocator)& theAlloc,
             const Standard_Size                      theSizeBPP,
             const NCollection_Vec3<Standard_Size>&   theSizeXYZ,
             const Standard_Size                      theSizeRowBytes,
             Standard_Byte*                           theDataPtr)
  {
    SetAllocator (theAlloc); // will free old data as well

    myData       = theDataPtr;
    myTopRowPtr  = NULL;
    SizeBPP      = theSizeBPP;
    SizeX        = theSizeXYZ.x();
    SizeY        = theSizeXYZ.y();
    SizeZ        = theSizeXYZ.z();
    SizeRowBytes = theSizeRowBytes != 0 ? theSizeRowBytes : (SizeX * theSizeBPP);
    SizeSliceBytes = SizeRowBytes * SizeY;
    mySize       = SizeSliceBytes * SizeZ;
    if (myData == NULL)
    {
      Allocate (mySize);
    }
    SetTopDown (TopToDown == 1);
    return !IsEmpty();
  }

  //! Reset all values to zeros.
  void ZeroData()
  {
    if (myData != NULL)
    {
      memset (myData, 0, mySize);
    }
  }

  //! Return data pointer to requested row (first column).
  const Standard_Byte* Row (const Standard_Size theRow) const
  {
    return myTopRowPtr + ptrdiff_t(SizeRowBytes * theRow * TopToDown);
  }

  //! Return data pointer to requested row (first column).
  Standard_Byte* ChangeRow (const Standard_Size theRow)
  {
    return myTopRowPtr + ptrdiff_t(SizeRowBytes * theRow * TopToDown);
  }

  //! Return data pointer to requested position.
  const Standard_Byte* Value (const Standard_Size theRow,
                              const Standard_Size theCol) const
  {
    return myTopRowPtr + ptrdiff_t(SizeRowBytes * theRow * TopToDown) + SizeBPP * theCol;
  }

  //! Return data pointer to requested position.
  Standard_Byte* ChangeValue (Standard_Size theRow,
                              Standard_Size theCol)
  {
    return myTopRowPtr + ptrdiff_t(SizeRowBytes * theRow * TopToDown) + SizeBPP * theCol;
  }

  //! Return data pointer to requested position.
  const Standard_Byte* ValueXY (Standard_Size theX,
                                Standard_Size theY) const
  {
    return myTopRowPtr + ptrdiff_t(SizeRowBytes * theY * TopToDown) + SizeBPP * theX;
  }

  //! Return data pointer to requested position.
  Standard_Byte* ChangeValueXY (Standard_Size theX,
                                Standard_Size theY)
  {
    return myTopRowPtr + ptrdiff_t(SizeRowBytes * theY * TopToDown) + SizeBPP * theX;
  }

public:

  //! Return data pointer to requested 2D slice.
  const Standard_Byte* Slice (Standard_Size theSlice) const
  {
    return myData + ptrdiff_t(SizeSliceBytes * theSlice);
  }

  //! Return data pointer to requested 2D slice.
  Standard_Byte* ChangeSlice (Standard_Size theSlice)
  {
    return myData + ptrdiff_t(SizeSliceBytes * theSlice);
  }

  //! Return data pointer to requested row (first column).
  const Standard_Byte* SliceRow (Standard_Size theSlice,
                                 Standard_Size theRow) const
  {
    return myTopRowPtr + ptrdiff_t(SizeRowBytes * theRow * TopToDown) + ptrdiff_t(SizeSliceBytes * theSlice);
  }

  //! Return data pointer to requested row (first column).
  Standard_Byte* ChangeSliceRow (Standard_Size theSlice,
                                 Standard_Size theRow)
  {
    return myTopRowPtr + ptrdiff_t(SizeRowBytes * theRow * TopToDown) + ptrdiff_t(SizeSliceBytes * theSlice);
  }

  //! Return data pointer to requested position.
  const Standard_Byte* ValueXYZ (Standard_Size theX,
                                 Standard_Size theY,
                                 Standard_Size theZ) const
  {
    return myTopRowPtr + ptrdiff_t(SizeRowBytes * theY * TopToDown) + SizeBPP * theX + ptrdiff_t(SizeSliceBytes * theZ);
  }

  //! Return data pointer to requested position.
  Standard_Byte* ChangeValueXYZ (Standard_Size theX,
                                 Standard_Size theY,
                                 Standard_Size theZ)
  {
    return myTopRowPtr + ptrdiff_t(SizeRowBytes * theY * TopToDown) + SizeBPP * theX + ptrdiff_t(SizeSliceBytes * theZ);
  }

  //! Compute the maximal row alignment for current row size.
  //! @return maximal row alignment in bytes (up to 16 bytes).
  Standard_Size MaxRowAligmentBytes() const
  {
    Standard_Size anAlignment = 2;
    for (; anAlignment <= 16; anAlignment <<= 1)
    {
      if ((SizeRowBytes % anAlignment) != 0 || (Standard_Size(myData) % anAlignment) != 0)
      {
        return (anAlignment >> 1);
      }
    }
    return anAlignment;
  }

  //! Setup scanlines order in memory - top-down or bottom-up.
  //! Drawers should explicitly specify this value if current state IsTopDown() was ignored!
  //! @param theIsTopDown top-down flag
  void SetTopDown (const bool theIsTopDown)
  {
    TopToDown   = (theIsTopDown ? 1 : Standard_Size(-1));
    myTopRowPtr = ((TopToDown == 1 || myData == NULL)
                ? myData : (myData + SizeRowBytes * (SizeY - 1)));
  }

protected:

  Standard_Byte* myTopRowPtr;  //!< pointer to the topmost row (depending on scanlines order in memory)

public:

  Standard_Size  SizeBPP;        //!< bytes per pixel
  Standard_Size  SizeX;          //!< width  in pixels
  Standard_Size  SizeY;          //!< height in pixels
  Standard_Size  SizeZ;          //!< depth  in pixels
  Standard_Size  SizeRowBytes;   //!< number of bytes per line (in most cases equal to 3 * sizeX)
  Standard_Size  SizeSliceBytes; //!< number of bytes per 2D slice
  Standard_Size  TopToDown;      //!< image scanlines direction in memory from Top to the Down

public:

  DEFINE_STANDARD_RTTIEXT(Image_PixMapData, NCollection_Buffer)

};

DEFINE_STANDARD_HANDLE(Image_PixMapData, NCollection_Buffer)

#endif // _Image_PixMapData_H__
