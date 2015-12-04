// Created on: 2012-07-10
// Created by: VRO
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

#include <Image_Diff.hxx>
#include <Image_AlienPixMap.hxx>

#include <TColStd_MapIteratorOfMapOfInteger.hxx>

#include <cstdlib>


IMPLEMENT_STANDARD_RTTIEXT(Image_Diff,Standard_Transient)

//! POD structure for packed RGB color value (3 bytes)
struct Image_ColorXXX24
{
  Standard_Byte v[3];
  typedef Standard_Byte ComponentType_t;         //!< Component type
  static Standard_Integer Length() { return 3; } //!< Returns the number of components
};

inline Image_ColorXXX24 operator- (const Image_ColorXXX24& theA,
                                   const Image_ColorXXX24& theB)
{
  return Image_ColorSub3 (theA, theB);
}

//! Dot squared for difference of two colors
inline Standard_Integer dotSquared (const Image_ColorXXX24& theColor)
{
  // explicitly convert to integer
  const Standard_Integer r = theColor.v[0];
  const Standard_Integer g = theColor.v[1];
  const Standard_Integer b = theColor.v[2];
  return r * r + g * g + b * b;
}

//! @return true if pixel is black
inline bool isBlack (const Image_ColorXXX24& theColor)
{
  return theColor.v[0] == 0
      && theColor.v[1] == 0
      && theColor.v[2] == 0;
}

//! Converts a pixel position (row, column) to one integer value
inline Standard_Size pixel2Int (const Standard_Size aRow,
                                const Standard_Size aCol)
{
  return aCol + (aRow << 15);
}

//! Converts an integer value to pixel coordinates (row, column)
inline void int2Pixel (const Standard_Size theValue,
                       Standard_Size&      theRow,
                       Standard_Size&      theCol)
{
  theRow = (theValue >> 15);
  theCol = theValue - (theRow << 15);
}

namespace
{

  inline ptrdiff_t getAbs (const ptrdiff_t theValue)
  {
    return theValue >= 0 ? theValue : -theValue;
  }

  static const Standard_Size NEIGHBOR_PIXELS_NB = 8;
  static struct
  {
    Standard_Integer row_inc;
    Standard_Integer col_inc;

    inline Standard_Size pixel2Int (const Standard_Size theRowCenter,
                                    const Standard_Size theColCenter) const
    {
      return ::pixel2Int (theRowCenter + Standard_Size(row_inc),
                          theColCenter + Standard_Size(col_inc));
    }

    inline bool isBlack (const Image_PixMap& theData,
                         const Standard_Size theRowCenter,
                         const Standard_Size theColCenter) const
    {
      return ::isBlack (theData.Value<Image_ColorXXX24> (theRowCenter + Standard_Size(row_inc),
                                                         theColCenter + Standard_Size(col_inc)));
    }

    inline bool isValid (const Image_PixMap& theData,
                         const Standard_Size theRowCenter,
                         const Standard_Size theColCenter) const
    {
      const Standard_Size aRow = theRowCenter + Standard_Size(row_inc);
      const Standard_Size aCol = theColCenter + Standard_Size(col_inc);
      return aRow < theData.SizeX()  // this unsigned math checks Standard_Size(-1) at-once
          && aCol < theData.SizeY();
    }
  }
  const NEIGHBOR_PIXELS[NEIGHBOR_PIXELS_NB] =
  {
    {-1, -1}, {-1,  0}, {-1,  1},
    { 0, -1},           { 0,  1},
    { 1, -1}, { 1,  0}, { 1,  1}
  };

  static bool isSupportedFormat (const Image_PixMap::ImgFormat theFormat)
  {
    return theFormat == Image_PixMap::ImgRGB
        || theFormat == Image_PixMap::ImgBGR
        || theFormat == Image_PixMap::ImgRGB32
        || theFormat == Image_PixMap::ImgBGR32
        || theFormat == Image_PixMap::ImgRGBA
        || theFormat == Image_PixMap::ImgBGRA;
  }
};

// =======================================================================
// function : Image_Diff
// purpose  :
// =======================================================================
Image_Diff::Image_Diff()
: myColorTolerance (0.0),
  myIsBorderFilterOn (Standard_False)
{
  //
}

// =======================================================================
// function : ~Image_Diff
// purpose  :
// =======================================================================
Image_Diff::~Image_Diff()
{
  releaseGroupsOfDiffPixels();
}

// =======================================================================
// function : Init
// purpose  :
// =======================================================================
Standard_Boolean Image_Diff::Init (const Handle(Image_PixMap)& theImageRef,
                                   const Handle(Image_PixMap)& theImageNew,
                                   const Standard_Boolean      theToBlackWhite)
{
  myImageRef.Nullify();
  myImageNew.Nullify();
  myDiffPixels.Clear();
  releaseGroupsOfDiffPixels();
  if (theImageRef.IsNull()   || theImageNew.IsNull()
   || theImageRef->IsEmpty() || theImageNew->IsEmpty()
   || theImageRef->SizeX()   != theImageNew->SizeX()
   || theImageRef->SizeY()   != theImageNew->SizeY()
   || theImageRef->Format()  != theImageNew->Format())
  {
#ifdef OCCT_DEBUG
    std::cerr << "Images has different format or dimensions\n";
#endif
    return Standard_False;
  }
  else if (!isSupportedFormat (theImageRef->Format()))
  {
#ifdef OCCT_DEBUG
    std::cerr << "Images has unsupported pixel format\n";
#endif
    return Standard_False;
  }
  else if (theImageRef->SizeX() >= 0xFFFF
        || theImageRef->SizeY() >= 0xFFFF)
  {
#ifdef OCCT_DEBUG
    std::cerr << "Image too large\n";
#endif
    return Standard_False;
  }

  myImageRef = theImageRef;
  myImageNew = theImageNew;

  if (theToBlackWhite)
  {
    // Convert the images to white/black
    const Image_ColorXXX24 aWhite = {{255, 255, 255}};
    for (Standard_Size aRow = 0; aRow < myImageRef->SizeY(); ++aRow)
    {
      for (Standard_Size aCol = 0; aCol < myImageRef->SizeX(); ++aCol)
      {
        Image_ColorXXX24& aPixel1 = myImageRef->ChangeValue<Image_ColorXXX24> (aRow, aCol);
        if (!isBlack (aPixel1))
        {
          aPixel1 = aWhite;
        }
        Image_ColorXXX24& aPixel2 = myImageNew->ChangeValue<Image_ColorXXX24> (aRow, aCol);
        if (!isBlack (aPixel2))
        {
          aPixel2 = aWhite;
        }
      }
    }
  }

  return Standard_True;
}


// =======================================================================
// function : Init
// purpose  :
// =======================================================================
Standard_Boolean Image_Diff::Init (const TCollection_AsciiString& theImgPathRef,
                                   const TCollection_AsciiString& theImgPathNew,
                                   const Standard_Boolean         theToBlackWhite)
{
  Handle(Image_AlienPixMap) anImgRef = new Image_AlienPixMap();
  Handle(Image_AlienPixMap) anImgNew = new Image_AlienPixMap();
  if (!anImgRef->Load (theImgPathRef)
   || !anImgNew->Load (theImgPathNew))
  {
#ifdef OCCT_DEBUG
    std::cerr << "Failed to load image(s) file(s)\n";
#endif
    return Standard_False;
  }
  return Init (anImgRef, anImgNew, theToBlackWhite);
}

// =======================================================================
// function : SetColorTolerance
// purpose  :
// =======================================================================
void Image_Diff::SetColorTolerance (const Standard_Real theTolerance)
{
  myColorTolerance = theTolerance;
}

// =======================================================================
// function : ColorTolerance
// purpose  :
// =======================================================================
Standard_Real Image_Diff::ColorTolerance() const
{
  return myColorTolerance;
}

// =======================================================================
// function : SetBorderFilterOn
// purpose  :
// =======================================================================
void Image_Diff::SetBorderFilterOn (const Standard_Boolean theToIgnore)
{
  myIsBorderFilterOn = theToIgnore;
}

// =======================================================================
// function : IsBorderFilterOn
// purpose  :
// =======================================================================
Standard_Boolean Image_Diff::IsBorderFilterOn() const
{
  return myIsBorderFilterOn;
}

// =======================================================================
// function : Compare
// purpose  :
// =======================================================================
Standard_Integer Image_Diff::Compare()
{
  // Number of different pixels (by color)
  Standard_Integer aNbDiffColors = 0;
  myDiffPixels.Clear();

  if (myImageRef.IsNull() || myImageNew.IsNull())
  {
    return -1;
  }

  // first check if images are exactly teh same
  if (! memcmp (myImageNew->Data(), myImageRef->Data(), myImageRef->SizeBytes()))
  {
    return 0;
  }

  // Tolerance of comparison operation for color
  // Maximum difference between colors (white - black) = 100%
  Image_ColorXXX24 aDiff = {{255, 255, 255}};
  const Standard_Integer aMaxDiffColor  = dotSquared (aDiff);
  const Standard_Integer aDiffThreshold = Standard_Integer(Standard_Real(aMaxDiffColor) * myColorTolerance);

  // we don't care about RGB/BGR/RGBA/BGRA/RGB32/BGR32 differences
  // because we just compute summ of r g b components

  // compare colors of each pixel
  for (Standard_Size aRow = 0; aRow < myImageRef->SizeY(); ++aRow)
  {
    for (Standard_Size aCol = 0; aCol < myImageRef->SizeX(); ++aCol)
    {
      aDiff = myImageNew->Value<Image_ColorXXX24> (aRow, aCol) - myImageRef->Value<Image_ColorXXX24> (aRow, aCol);
      if (dotSquared (aDiff) > aDiffThreshold)
      {
        const Standard_Size aValue = pixel2Int (aRow, aCol);
        myDiffPixels.Append (aValue);
        ++aNbDiffColors;
      }
    }
  }

  // take into account a border effect
  if (myIsBorderFilterOn && myDiffPixels.Length() > 0)
  {
    aNbDiffColors = ignoreBorderEffect();
  }

  return aNbDiffColors;
}

// =======================================================================
// function : SaveDiffImage
// purpose  :
// =======================================================================
Standard_Boolean Image_Diff::SaveDiffImage (Image_PixMap& theDiffImage) const
{
  if (myImageRef.IsNull() || myImageNew.IsNull())
  {
    return Standard_False;
  }

  if (theDiffImage.IsEmpty()
   || theDiffImage.SizeX() != myImageRef->SizeX()
   || theDiffImage.SizeY() != myImageRef->SizeY()
   || !isSupportedFormat (theDiffImage.Format()))
  {
    if (!theDiffImage.InitTrash (Image_PixMap::ImgRGB, myImageRef->SizeX(), myImageRef->SizeY()))
    {
      return Standard_False;
    }
  }

  Standard_Size aRow, aCol;
  const Image_ColorXXX24 aWhite = {{255, 255, 255}};

  // initialize black image for dump
  memset (theDiffImage.ChangeData(), 0, theDiffImage.SizeBytes());
  if (myGroupsOfDiffPixels.IsEmpty())
  {
    if (myIsBorderFilterOn)
    {
      return Standard_True;
    }

    for (Standard_Integer aPixelId = 0; aPixelId < myDiffPixels.Length(); ++aPixelId)
    {
      const Standard_Size aValue = myDiffPixels.Value (aPixelId);
      int2Pixel (aValue, aRow, aCol);
      theDiffImage.ChangeValue<Image_ColorXXX24> (aRow, aCol) = aWhite;
    }

    return Standard_True;
  }

  Standard_Integer aGroupId = 1;
  for (ListOfMapOfInteger::Iterator aGrIter (myGroupsOfDiffPixels); aGrIter.More(); aGrIter.Next(), ++aGroupId)
  {
    if (myLinearGroups.Contains (aGroupId))
    {
      continue; // skip linear groups
    }

    const TColStd_MapOfInteger* aGroup = aGrIter.Value();
    for (TColStd_MapIteratorOfMapOfInteger aPixelIter(*aGroup);
         aPixelIter.More(); aPixelIter.Next())
    {
      int2Pixel (aPixelIter.Key(), aRow, aCol);
      theDiffImage.ChangeValue<Image_ColorXXX24> (aRow, aCol) = aWhite;
    }
  }

  return Standard_True;
}

// =======================================================================
// function : SaveDiffImage
// purpose  :
// =======================================================================
Standard_Boolean Image_Diff::SaveDiffImage (const TCollection_AsciiString& theDiffPath) const
{
  if (myImageRef.IsNull() || myImageNew.IsNull() || theDiffPath.IsEmpty())
  {
    return Standard_False;
  }

  Image_AlienPixMap aDiff;
  if (!aDiff.InitTrash (Image_PixMap::ImgRGB, myImageRef->SizeX(), myImageRef->SizeY())
   || !SaveDiffImage (aDiff))
  {
    return Standard_False;
  }

  // save image
  return aDiff.Save (theDiffPath);
}

// =======================================================================
// function : ignoreBorderEffect
// purpose  :
// =======================================================================
Standard_Integer Image_Diff::ignoreBorderEffect()
{
  if (myImageRef.IsNull() || myImageNew.IsNull())
  {
    return 0;
  }

  // allocate groups of different pixels
  releaseGroupsOfDiffPixels();

  // Find a different area (a set of close to each other pixels which colors differ in both images).
  // It filters alone pixels with different color.
  Standard_Size aRow1 = 0, aCol1 = 0, aRow2, aCol2;
  Standard_Integer aLen1 = (myDiffPixels.Length() > 0) ? (myDiffPixels.Length() - 1) : 0;
  for (Standard_Integer aPixelId1 = 0; aPixelId1 < aLen1; ++aPixelId1)
  {
    const Standard_Size aValue1 = myDiffPixels.Value (aPixelId1);
    int2Pixel (aValue1, aRow1, aCol1);

    // Check other pixels in the list looking for a neighbour of this one
    for (Standard_Integer aPixelId2 = aPixelId1 + 1; aPixelId2 < myDiffPixels.Length(); ++aPixelId2)
    {
      const Standard_Size aValue2 = myDiffPixels.Value (aPixelId2);
      int2Pixel (aValue2, aRow2, aCol2);
      if (getAbs (ptrdiff_t (aCol1 - aCol2)) <= 1 &&
          getAbs (ptrdiff_t (aRow1 - aRow2)) <= 1)
      {
        // A neighbour is found. Create a new group and add both pixels.
        if (myGroupsOfDiffPixels.IsEmpty())
        {
          TColStd_MapOfInteger* aGroup = new TColStd_MapOfInteger();
          aGroup->Add ((Standard_Integer)aValue1);
          aGroup->Add ((Standard_Integer)aValue2);
          myGroupsOfDiffPixels.Append (aGroup);
        }
        else
        {
          // Find a group the pixels belong to.
          Standard_Boolean isFound = Standard_False;
          for (ListOfMapOfInteger::Iterator aGrIter (myGroupsOfDiffPixels); aGrIter.More(); aGrIter.Next())
          {
            TColStd_MapOfInteger*& aGroup = aGrIter.ChangeValue();
            if (aGroup->Contains ((Standard_Integer)aValue1))
            {
              aGroup->Add ((Standard_Integer)aValue2);
              isFound = Standard_True;
              break;
            }
          }

          if (!isFound)
          {
            // Create a new group
            TColStd_MapOfInteger* aGroup = new TColStd_MapOfInteger();
            aGroup->Add ((Standard_Integer)aValue1);
            aGroup->Add ((Standard_Integer)aValue2);
            myGroupsOfDiffPixels.Append (aGroup);
          }
        }
      }
    }
  }

  // filter linear groups which represent border of a solid shape
  Standard_Integer aGroupId = 1;
  for (ListOfMapOfInteger::Iterator aGrIter (myGroupsOfDiffPixels); aGrIter.More(); aGrIter.Next(), ++aGroupId)
  {
    Standard_Integer aNeighboursNb = 0;
    Standard_Boolean isLine = Standard_True;
    const TColStd_MapOfInteger* aGroup = aGrIter.Value();
    for (TColStd_MapIteratorOfMapOfInteger aPixelIter (*aGroup); aPixelIter.More(); aPixelIter.Next())
    {
      int2Pixel (aPixelIter.Key(), aRow1, aCol1);
      aNeighboursNb = 0;

      // pixels of a line have only 1 or 2 neighbour pixels inside the same group
      // check all neighbour pixels on presence in the group
      for (Standard_Size aNgbrIter = 0; aNgbrIter < NEIGHBOR_PIXELS_NB; ++aNgbrIter)
      {
        if (NEIGHBOR_PIXELS[aNgbrIter].isValid (*myImageRef, aRow1, aCol1)
         && aGroup->Contains ((Standard_Integer)NEIGHBOR_PIXELS[aNgbrIter].pixel2Int (aRow1, aCol1)))
        {
          ++aNeighboursNb;
        }
      }

      if (aNeighboursNb > 2)
      {
        isLine = Standard_False;
        break;
      }
    } // for pixels inside group...

    if (isLine)
    {
      // Test a pixel of the linear group on belonging to a solid shape.
      // Consider neighbour pixels of the last pixel of the linear group in the 1st image.
      // If the pixel has greater than 1 not black neighbour pixel, it is a border of a shape.
      // Otherwise, it may be a topological edge, for example.
      aNeighboursNb = 0;
      for (Standard_Size aNgbrIter = 0; aNgbrIter < NEIGHBOR_PIXELS_NB; ++aNgbrIter)
      {
        if ( NEIGHBOR_PIXELS[aNgbrIter].isValid (*myImageRef, aRow1, aCol1)
         && !NEIGHBOR_PIXELS[aNgbrIter].isBlack (*myImageRef, aRow1, aCol1))
        {
          ++aNeighboursNb;
        }
      }

      if (aNeighboursNb > 1)
      {
        myLinearGroups.Add (aGroupId);
      }
    }
  } // for groups...

  // number of different groups of pixels (except linear groups)
  Standard_Integer aNbDiffColors = 0;
  aGroupId = 1;
  for (ListOfMapOfInteger::Iterator aGrIter (myGroupsOfDiffPixels); aGrIter.More(); aGrIter.Next(), ++aGroupId)
  {
    if (!myLinearGroups.Contains (aGroupId))
      ++aNbDiffColors;
  }

  return aNbDiffColors;
}

// =======================================================================
// function : releaseGroupsOfDiffPixels
// purpose  :
// =======================================================================
void Image_Diff::releaseGroupsOfDiffPixels()
{
  for (ListOfMapOfInteger::Iterator aGrIter (myGroupsOfDiffPixels); aGrIter.More(); aGrIter.Next())
  {
    TColStd_MapOfInteger*& aGroup = aGrIter.ChangeValue();
    delete aGroup;
  }
  myGroupsOfDiffPixels.Clear();
  myLinearGroups.Clear();
}
