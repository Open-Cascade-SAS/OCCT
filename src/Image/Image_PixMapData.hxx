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
    SizeRowBytes (0),
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
    SetAllocator (theAlloc); // will free old data as well

    myData       = theDataPtr;
    myTopRowPtr  = NULL;
    SizeBPP      = theSizeBPP;
    SizeX        = theSizeX;
    SizeY        = theSizeY;
    SizeRowBytes = theSizeRowBytes != 0 ? theSizeRowBytes : (theSizeX * theSizeBPP);
    mySize       = SizeRowBytes * SizeY;
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

  //! @return data pointer to requested row (first column).
  inline const Standard_Byte* Row (const Standard_Size theRow) const
  {
    return myTopRowPtr + ptrdiff_t(SizeRowBytes * theRow * TopToDown);
  }

  //! @return data pointer to requested row (first column).
  inline Standard_Byte* ChangeRow (const Standard_Size theRow)
  {
    return myTopRowPtr + ptrdiff_t(SizeRowBytes * theRow * TopToDown);
  }

  //! @return data pointer to requested position.
  inline const Standard_Byte* Value (const Standard_Size theRow,
                                     const Standard_Size theCol) const
  {
    return myTopRowPtr + ptrdiff_t(SizeRowBytes * theRow * TopToDown) + SizeBPP * theCol;
  }

  //! @return data pointer to requested position.
  inline Standard_Byte* ChangeValue (const Standard_Size theRow,
                                     const Standard_Size theCol)
  {
    return myTopRowPtr + ptrdiff_t(SizeRowBytes * theRow * TopToDown) + SizeBPP * theCol;
  }

  //! Compute the maximal row alignment for current row size.
  //! @return maximal row alignment in bytes (up to 16 bytes).
  inline Standard_Size MaxRowAligmentBytes() const
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
  inline void SetTopDown (const bool theIsTopDown)
  {
    TopToDown   = (theIsTopDown ? 1 : Standard_Size(-1));
    myTopRowPtr = ((TopToDown == 1 || myData == NULL)
                ? myData : (myData + SizeRowBytes * (SizeY - 1)));
  }

protected:

  Standard_Byte* myTopRowPtr;  //!< pointer to the topmost row (depending on scanlines order in memory)

public:

  Standard_Size  SizeBPP;      //!< bytes per pixel
  Standard_Size  SizeX;        //!< width  in pixels
  Standard_Size  SizeY;        //!< height in pixels
  Standard_Size  SizeRowBytes; //!< number of bytes per line (in most cases equal to 3 * sizeX)
  Standard_Size  TopToDown;    //!< image scanlines direction in memory from Top to the Down


public:

  DEFINE_STANDARD_RTTI_INLINE(Image_PixMapData,NCollection_Buffer) // Type definition

};

DEFINE_STANDARD_HANDLE(Image_PixMapData, NCollection_Buffer)

#endif // _Image_PixMapData_H__
