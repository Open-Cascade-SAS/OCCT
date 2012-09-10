// Created on: 2012-07-18
// Created by: Kirill GAVRILOV
// Copyright (c) 2012 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.

#ifndef _Image_PixMapData_H__
#define _Image_PixMapData_H__

#include <Image_Color.hxx>

//! POD template structure to access image buffer
template<typename ColorType_t>
struct Image_PixMapData
{

  //! @return data pointer for low-level operations (copying entire buffer, parsing with extra tools etc.).
  inline const ColorType_t* Data() const
  {
    return (const ColorType_t* )myDataPtr;
  }

  //! @return data pointer for low-level operations (copying entire buffer, parsing with extra tools etc.).
  inline ColorType_t* ChangeData()
  {
    return (ColorType_t* )myDataPtr;
  }

  //! @return data pointer to requested row (first column).
  inline const ColorType_t* Row (const Standard_Size theRow) const
  {
    return (ColorType_t* )(myTopRowPtr + mySizeRowBytes * theRow * myTopToDown);
  }

  //! @return data pointer to requested row (first column).
  inline ColorType_t* ChangeRow (const Standard_Size theRow)
  {
    return (ColorType_t* )(myTopRowPtr + mySizeRowBytes * theRow * myTopToDown);
  }

  //! @return data pointer to requested position.
  inline const ColorType_t& Value (const Standard_Size theRow,
                                   const Standard_Size theCol) const
  {
    return *(const ColorType_t* )(myTopRowPtr + mySizeRowBytes * theRow * myTopToDown + mySizeBPP * theCol);
  }

  //! @return data pointer to requested position.
  inline ColorType_t& ChangeValue (const Standard_Size theRow,
                                   const Standard_Size theCol)
  {
    return *(ColorType_t* )(myTopRowPtr + mySizeRowBytes * theRow * myTopToDown + mySizeBPP * theCol);
  }

  //! Compute the maximal row alignment for current row size.
  //! @return maximal row alignment in bytes (up to 16 bytes).
  inline Standard_Size MaxRowAligmentBytes() const
  {
    Standard_Size anAlignment = 2;
    for (; anAlignment <= 16; anAlignment <<= 1)
    {
      if ((mySizeRowBytes % anAlignment) != 0 || (Standard_Size(myDataPtr) % anAlignment) != 0)
      {
        return (anAlignment >> 1);
      }
    }
    return anAlignment;
  }

  //! @return bytes allocated for the whole image plane.
  inline Standard_Size SizeBytes() const
  {
    return mySizeRowBytes * mySizeY;
  }

  //! @return image width in pixels
  inline Standard_Size SizeX() const
  {
    return mySizeX;
  }

  //! @return image height in pixels
  inline Standard_Size SizeY() const
  {
    return mySizeY;
  }

public:

  Standard_Byte* myDataPtr;      //!< pointer to the data
  Standard_Byte* myTopRowPtr;    //!< pointer to the topmost row (depending on scanlines order in memory)
  Standard_Size  mySizeBPP;      //!< bytes per pixel
  Standard_Size  mySizeX;        //!< width  in pixels
  Standard_Size  mySizeY;        //!< height in pixels
  Standard_Size  mySizeRowBytes; //!< number of bytes per line (in most cases equal to 3 * sizeX)
  Standard_Size  myTopToDown;    //!< image scanlines direction in memory from Top to the Down

};

#endif // _Image_PixMapData_H__
