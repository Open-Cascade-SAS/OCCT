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
#include <Message.hxx>
#include <Message_Messenger.hxx>
#include <TColStd_MapIteratorOfPackedMapOfInteger.hxx>

#include <cstdlib>

IMPLEMENT_STANDARD_RTTIEXT(Image_Diff,Standard_Transient)

namespace
{

  //! POD structure for packed RGB color value (3 bytes)
  struct Image_ColorXXX24
  {
    Standard_Byte v[3];
    typedef Standard_Byte ComponentType_t;         //!< Component type
    static Standard_Integer Length() { return 3; } //!< Returns the number of components
  };

  static Image_ColorXXX24 operator- (const Image_ColorXXX24& theA,
                                     const Image_ColorXXX24& theB)
  {
    return Image_ColorSub3 (theA, theB);
  }

  //! Dot squared for difference of two colors
  static Standard_Integer dotSquared (const Image_ColorXXX24& theColor)
  {
    // explicitly convert to integer
    const Standard_Integer r = theColor.v[0];
    const Standard_Integer g = theColor.v[1];
    const Standard_Integer b = theColor.v[2];
    return r * r + g * g + b * b;
  }

  //! Number of neighbor pixels.
  static const Standard_Size Image_Diff_NbOfNeighborPixels = 8;

  //! List of neighbor pixels (offsets).
  static const int Image_Diff_NEIGHBOR_PIXELS[Image_Diff_NbOfNeighborPixels][2] =
  {
    {-1, -1}, {0, -1}, {1, -1},
    {-1,  0},          {1,  0},
    {-1,  1}, {0,  1}, {1,  1}
  };

  //! @return true if pixel is black
  static bool isBlackPixel (const Image_PixMap& theData, Standard_Size theY, Standard_Size theX)
  {
    switch (theData.Format())
    {
      case Image_Format_Gray:
      case Image_Format_Alpha:
      {
        return theData.Value<unsigned char> (theY, theX) == 0;
      }
      case Image_Format_RGB:
      case Image_Format_BGR:
      case Image_Format_RGB32:
      case Image_Format_BGR32:
      case Image_Format_RGBA:
      case Image_Format_BGRA:
      {
        const Image_ColorXXX24& aColor = theData.Value<Image_ColorXXX24> (theY, theX);
        return aColor.v[0] == 0
            && aColor.v[1] == 0
            && aColor.v[2] == 0;
      }
      default:
      {
        const Quantity_ColorRGBA aPixelRgba = theData.PixelColor ((int)theY, (int)theX);
        const NCollection_Vec4<float>& aPixel = aPixelRgba;
        return aPixel.r() == 0.0f
            && aPixel.g() == 0.0f
            && aPixel.b() == 0.0f;
      }
    }
  }

}

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
    Message::DefaultMessenger()->Send ("Error: Images have different format or dimensions", Message_Fail);
    return Standard_False;
  }
  else if (theImageRef->SizeX() >= 0xFFFF
        || theImageRef->SizeY() >= 0xFFFF)
  {
    Message::DefaultMessenger()->Send ("Error: Images are too large", Message_Fail);
    return Standard_False;
  }

  myImageRef = theImageRef;
  myImageNew = theImageNew;
  if (theToBlackWhite)
  {
    Image_PixMap::ToBlackWhite (*myImageRef);
    Image_PixMap::ToBlackWhite (*myImageNew);
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
    Message::DefaultMessenger()->Send ("Error: Failed to load image(s) file(s)", Message_Fail);
    return Standard_False;
  }
  return Init (anImgRef, anImgNew, theToBlackWhite);
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

  // first check if images are exactly the same
  if (myImageNew->SizeBytes() == myImageRef->SizeBytes()
   && memcmp (myImageNew->Data(), myImageRef->Data(), myImageRef->SizeBytes()) == 0)
  {
    return 0;
  }

  switch (myImageRef->Format())
  {
    case Image_Format_Gray:
    case Image_Format_Alpha:
    {
      // Tolerance of comparison operation for color
      Standard_Integer aDiff = 255;
      const Standard_Real    aMaxDiffColor  = aDiff * aDiff;
      const Standard_Integer aDiffThreshold = Standard_Integer(aMaxDiffColor * myColorTolerance);
      for (Standard_Size aRow = 0; aRow < myImageRef->SizeY(); ++aRow)
      {
        for (Standard_Size aCol = 0; aCol < myImageRef->SizeX(); ++aCol)
        {
          aDiff = Standard_Integer(myImageNew->Value<unsigned char> (aRow, aCol)) - Standard_Integer(myImageRef->Value<unsigned char> (aRow, aCol));
          if (aDiff * aDiff > aDiffThreshold)
          {
            myDiffPixels.Append (PackXY ((uint16_t)aCol, (uint16_t)aRow));
            ++aNbDiffColors;
          }
        }
      }
      break;
    }
    case Image_Format_RGB:
    case Image_Format_BGR:
    case Image_Format_RGB32:
    case Image_Format_BGR32:
    case Image_Format_RGBA:
    case Image_Format_BGRA:
    {
      // Tolerance of comparison operation for color
      // Maximum difference between colors (white - black) = 100%
      Image_ColorXXX24 aDiff = {{255, 255, 255}};
      const Standard_Real    aMaxDiffColor  = dotSquared (aDiff);
      const Standard_Integer aDiffThreshold = Standard_Integer(aMaxDiffColor * myColorTolerance);

      // we don't care about RGB/BGR/RGBA/BGRA/RGB32/BGR32 differences
      // because we just compute summ of r g b components
      for (Standard_Size aRow = 0; aRow < myImageRef->SizeY(); ++aRow)
      {
        for (Standard_Size aCol = 0; aCol < myImageRef->SizeX(); ++aCol)
        {
          aDiff = myImageNew->Value<Image_ColorXXX24> (aRow, aCol) - myImageRef->Value<Image_ColorXXX24> (aRow, aCol);
          if (dotSquared (aDiff) > aDiffThreshold)
          {
            myDiffPixels.Append (PackXY ((uint16_t)aCol, (uint16_t)aRow));
            ++aNbDiffColors;
          }
        }
      }
      break;
    }
    default:
    {
      // Tolerance of comparison operation for color
      // Maximum difference between colors (white - black) = 100%
      NCollection_Vec3<float> aDiff (1.0f, 1.0f, 1.0f);
      const Standard_Real    aMaxDiffColor  = aDiff.SquareModulus();
      const Standard_Integer aDiffThreshold = Standard_Integer(aMaxDiffColor * myColorTolerance);
      for (Standard_Size aRow = 0; aRow < myImageRef->SizeY(); ++aRow)
      {
        for (Standard_Size aCol = 0; aCol < myImageRef->SizeX(); ++aCol)
        {
          const Quantity_ColorRGBA aPixel1Rgba = myImageRef->PixelColor (Standard_Integer(aCol), Standard_Integer(aRow));
          const Quantity_ColorRGBA aPixel2Rgba = myImageNew->PixelColor (Standard_Integer(aCol), Standard_Integer(aRow));
          const NCollection_Vec3<float>& aPixel1 = aPixel1Rgba.GetRGB();
          const NCollection_Vec3<float>& aPixel2 = aPixel2Rgba.GetRGB();
          aDiff = aPixel2 - aPixel1;
          if (aDiff.SquareModulus() > aDiffThreshold)
          {
            myDiffPixels.Append (PackXY ((uint16_t)aCol, (uint16_t)aRow));
            ++aNbDiffColors;
          }
        }
      }
      break;
    }
  }

  // take into account a border effect
  if (myIsBorderFilterOn && !myDiffPixels.IsEmpty())
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
   || theDiffImage.SizeY() != myImageRef->SizeY())
  {
    if (!theDiffImage.InitTrash (Image_Format_Gray, myImageRef->SizeX(), myImageRef->SizeY()))
    {
      return Standard_False;
    }
  }

  const Image_ColorXXX24   aWhite24 = {{255, 255, 255}};
  const Quantity_ColorRGBA aWhiteRgba (1.0f, 1.0f, 1.0f, 1.0f);

  // initialize black image for dump
  memset (theDiffImage.ChangeData(), 0, theDiffImage.SizeBytes());
  if (myGroupsOfDiffPixels.IsEmpty())
  {
    if (myIsBorderFilterOn)
    {
      return Standard_True;
    }

    switch (theDiffImage.Format())
    {
      case Image_Format_Gray:
      case Image_Format_Alpha:
      {
        for (NCollection_Vector<Standard_Integer>::Iterator aPixelIter (myDiffPixels); aPixelIter.More(); aPixelIter.Next())
        {
          theDiffImage.ChangeValue<unsigned char> (UnpackY(aPixelIter.Value()), UnpackX(aPixelIter.Value())) = 255;
        }
        break;
      }
      case Image_Format_RGB:
      case Image_Format_BGR:
      case Image_Format_RGB32:
      case Image_Format_BGR32:
      case Image_Format_RGBA:
      case Image_Format_BGRA:
      {
        for (NCollection_Vector<Standard_Integer>::Iterator aPixelIter (myDiffPixels); aPixelIter.More(); aPixelIter.Next())
        {
          theDiffImage.ChangeValue<Image_ColorXXX24> (UnpackY(aPixelIter.Value()), UnpackX(aPixelIter.Value())) = aWhite24;
        }
        break;
      }
      default:
      {
        for (NCollection_Vector<Standard_Integer>::Iterator aPixelIter (myDiffPixels); aPixelIter.More(); aPixelIter.Next())
        {
          theDiffImage.SetPixelColor (UnpackX(aPixelIter.Value()), UnpackY(aPixelIter.Value()), aWhiteRgba);
        }
        break;
      }
    }
    return Standard_True;
  }

  Standard_Integer aGroupId = 1;
  for (NCollection_List<Handle(TColStd_HPackedMapOfInteger)>::Iterator aGrIter (myGroupsOfDiffPixels); aGrIter.More(); aGrIter.Next(), ++aGroupId)
  {
    if (myLinearGroups.Contains (aGroupId))
    {
      continue; // skip linear groups
    }

    const Handle(TColStd_HPackedMapOfInteger)& aGroup = aGrIter.Value();
    switch (theDiffImage.Format())
    {
      case Image_Format_Gray:
      case Image_Format_Alpha:
      {
        for (TColStd_MapIteratorOfPackedMapOfInteger aPixelIter (aGroup->Map()); aPixelIter.More(); aPixelIter.Next())
        {
          Standard_Integer aDiffPixel (aPixelIter.Key());
          theDiffImage.ChangeValue<unsigned char> (UnpackY(aDiffPixel), UnpackX(aDiffPixel)) = 255;
        }
        break;
      }
      case Image_Format_RGB:
      case Image_Format_BGR:
      case Image_Format_RGB32:
      case Image_Format_BGR32:
      case Image_Format_RGBA:
      case Image_Format_BGRA:
      {
        for (TColStd_MapIteratorOfPackedMapOfInteger aPixelIter (aGroup->Map()); aPixelIter.More(); aPixelIter.Next())
        {
          Standard_Integer aDiffPixel (aPixelIter.Key());
          theDiffImage.ChangeValue<Image_ColorXXX24> (UnpackY(aDiffPixel), UnpackX(aDiffPixel)) = aWhite24;
        }
        break;
      }
      default:
      {
        for (TColStd_MapIteratorOfPackedMapOfInteger aPixelIter (aGroup->Map()); aPixelIter.More(); aPixelIter.Next())
        {
          Standard_Integer aDiffPixel (aPixelIter.Key());
          theDiffImage.SetPixelColor (UnpackX(aDiffPixel), UnpackY(aDiffPixel), aWhiteRgba);
        }
        break;
      }
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
  if (!aDiff.InitTrash (Image_Format_Gray, myImageRef->SizeX(), myImageRef->SizeY())
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
  const Standard_Integer aLen1 = !myDiffPixels.IsEmpty() ? (myDiffPixels.Length() - 1) : 0;
  for (Standard_Integer aPixelId1 = 0; aPixelId1 < aLen1; ++aPixelId1)
  {
    Standard_Integer aValue1 = myDiffPixels.Value (aPixelId1);

    // Check other pixels in the list looking for a neighbour of this one
    for (Standard_Integer aPixelId2 = aPixelId1 + 1; aPixelId2 < myDiffPixels.Length(); ++aPixelId2)
    {
      Standard_Integer aValue2 = myDiffPixels.Value (aPixelId2);
      if (Abs (Standard_Integer(UnpackX(aValue1)) - Standard_Integer(UnpackX(aValue2))) <= 1
       && Abs (Standard_Integer(UnpackY(aValue1)) - Standard_Integer(UnpackY(aValue2))) <= 1)
      {
        // A neighbour is found. Create a new group and add both pixels.
        if (myGroupsOfDiffPixels.IsEmpty())
        {
          Handle(TColStd_HPackedMapOfInteger) aGroup = new TColStd_HPackedMapOfInteger();
          aGroup->ChangeMap().Add (aValue1);
          aGroup->ChangeMap().Add (aValue2);
          myGroupsOfDiffPixels.Append (aGroup);
        }
        else
        {
          // Find a group the pixels belong to.
          Standard_Boolean isFound = Standard_False;
          for (NCollection_List<Handle(TColStd_HPackedMapOfInteger)>::Iterator aGrIter (myGroupsOfDiffPixels); aGrIter.More(); aGrIter.Next())
          {
            const Handle(TColStd_HPackedMapOfInteger)& aGroup = aGrIter.ChangeValue();
            if (aGroup->Map().Contains (aValue1))
            {
              aGroup->ChangeMap().Add (aValue2);
              isFound = Standard_True;
              break;
            }
          }

          if (!isFound)
          {
            // Create a new group
            Handle(TColStd_HPackedMapOfInteger) aGroup = new TColStd_HPackedMapOfInteger();
            aGroup->ChangeMap().Add (aValue1);
            aGroup->ChangeMap().Add (aValue2);
            myGroupsOfDiffPixels.Append (aGroup);
          }
        }
      }
    }
  }

  // filter linear groups which represent border of a solid shape
  Standard_Integer aGroupId = 1;
  for (NCollection_List<Handle(TColStd_HPackedMapOfInteger)>::Iterator aGrIter (myGroupsOfDiffPixels); aGrIter.More(); aGrIter.Next(), ++aGroupId)
  {
    Standard_Integer aNeighboursNb = 0;
    Standard_Boolean isLine = Standard_True;
    const Handle(TColStd_HPackedMapOfInteger)& aGroup = aGrIter.Value();
    if (aGroup->Map().IsEmpty())
    {
      continue;
    }

    Standard_Integer aDiffPixel = 0;
    for (TColStd_MapIteratorOfPackedMapOfInteger aPixelIter (aGroup->Map()); aPixelIter.More(); aPixelIter.Next())
    {
      aDiffPixel = aPixelIter.Key();
      aNeighboursNb = 0;

      // pixels of a line have only 1 or 2 neighbour pixels inside the same group
      // check all neighbour pixels on presence in the group
      for (Standard_Size aNgbrIter = 0; aNgbrIter < Image_Diff_NbOfNeighborPixels; ++aNgbrIter)
      {
        Standard_Integer anX = UnpackX(aDiffPixel) + Image_Diff_NEIGHBOR_PIXELS[aNgbrIter][0];
        Standard_Integer anY = UnpackY(aDiffPixel) + Image_Diff_NEIGHBOR_PIXELS[aNgbrIter][1];
        if (Standard_Size(anX) < myImageRef->SizeX()  // this unsigned math checks Standard_Size(-1) at-once
         && Standard_Size(anY) < myImageRef->SizeY()
         && aGroup->Map().Contains (PackXY((uint16_t)anX, (uint16_t)anY)))
        {
          ++aNeighboursNb;
        }
      }

      if (aNeighboursNb > 2)
      {
        isLine = Standard_False;
        break;
      }
    }

    if (isLine)
    {
      // Test a pixel of the linear group on belonging to a solid shape.
      // Consider neighbour pixels of the last pixel of the linear group in the 1st image.
      // If the pixel has greater than 1 not black neighbour pixel, it is a border of a shape.
      // Otherwise, it may be a topological edge, for example.
      aNeighboursNb = 0;
      for (Standard_Size aNgbrIter = 0; aNgbrIter < Image_Diff_NbOfNeighborPixels; ++aNgbrIter)
      {
        Standard_Integer anX = UnpackX(aDiffPixel) + Image_Diff_NEIGHBOR_PIXELS[aNgbrIter][0];
        Standard_Integer anY = UnpackY(aDiffPixel) + Image_Diff_NEIGHBOR_PIXELS[aNgbrIter][1];
        if (Standard_Size(anX) < myImageRef->SizeX()  // this unsigned math checks Standard_Size(-1) at-once
        &&  Standard_Size(anY) < myImageRef->SizeY()
        && !isBlackPixel (*myImageRef, Standard_Size(anY), Standard_Size(anX)))
        {
          ++aNeighboursNb;
        }
      }

      if (aNeighboursNb > 1)
      {
        myLinearGroups.Add (aGroupId);
      }
    }
  }

  // number of different groups of pixels (except linear groups)
  Standard_Integer aNbDiffColors = 0;
  aGroupId = 1;
  for (NCollection_List<Handle(TColStd_HPackedMapOfInteger)>::Iterator aGrIter (myGroupsOfDiffPixels); aGrIter.More(); aGrIter.Next(), ++aGroupId)
  {
    if (!myLinearGroups.Contains (aGroupId))
    {
      ++aNbDiffColors;
    }
  }

  return aNbDiffColors;
}

// =======================================================================
// function : releaseGroupsOfDiffPixels
// purpose  :
// =======================================================================
void Image_Diff::releaseGroupsOfDiffPixels()
{
  myGroupsOfDiffPixels.Clear();
  myLinearGroups.Clear();
}
