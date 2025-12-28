// Created on: 2013-06-25
// Created by: Dmitry BOBYLEV
// Copyright (c) 2013-2014 OPEN CASCADE SAS
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

#include <Graphic3d_MarkerImage.hxx>

#include <Image_PixMap.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_HArray1.hxx>

#include "Graphic3d_MarkerImage.pxx"

#include <atomic>

IMPLEMENT_STANDARD_RTTIEXT(Graphic3d_MarkerImage, Standard_Transient)

namespace
{
static std::atomic<int> THE_MARKER_IMAGE_COUNTER(0);

//! Names of built-in markers
static const char* THE_MARKER_NAMES[Aspect_TOM_USERDEFINED] = {
  ".",     // Aspect_TOM_POINT
  "+",     // Aspect_TOM_PLUS
  "*",     // Aspect_TOM_STAR
  "x",     // Aspect_TOM_X
  "o",     // Aspect_TOM_O
  "o.",    // Aspect_TOM_O_POINT
  "o+",    // Aspect_TOM_O_PLUS
  "o*",    // Aspect_TOM_O_STAR
  "ox",    // Aspect_TOM_O_X
  "ring1", // Aspect_TOM_RING1
  "ring2", // Aspect_TOM_RING2
  "ring3", // Aspect_TOM_RING3
  "ball"   // Aspect_TOM_BALL
};

//! Returns a parameters for the marker of the specified type and scale.
static void getMarkerBitMapParam(const Aspect_TypeOfMarker theMarkerType,
                                 const float  theScale,
                                 int&         theWidth,
                                 int&         theHeight,
                                 int&         theOffset,
                                 int&         theNumOfBytes)
{
  const int aType =
    int(theMarkerType > Aspect_TOM_O ? Aspect_TOM_O : theMarkerType);
  const double anIndex = (double)(TEL_NO_OF_SIZES - 1)
                                * (theScale - (double)TEL_PM_START_SIZE)
                                / (double)(TEL_PM_END_SIZE - TEL_PM_START_SIZE);
  int anId = (int)(anIndex + 0.5);
  if (anId < 0)
  {
    anId = 0;
  }
  else if (anId >= TEL_NO_OF_SIZES)
  {
    anId = TEL_NO_OF_SIZES - 1;
  }

  theWidth                                = (int)arrPMFontInfo[aType][anId].width;
  theHeight                               = (int)arrPMFontInfo[aType][anId].height;
  theOffset                               = arrPMFontInfo[aType][anId].offset;
  const int aNumOfBytesInRow = theWidth / 8 + (theWidth % 8 ? 1 : 0);
  theNumOfBytes                           = theHeight * aNumOfBytesInRow;
}

//! Merge two image pixmap into one. Used for creating image for following markers:
//! Aspect_TOM_O_POINT, Aspect_TOM_O_PLUS, Aspect_TOM_O_STAR, Aspect_TOM_O_X, Aspect_TOM_RING1,
//! Aspect_TOM_RING2, Aspect_TOM_RING3
static occ::handle<Image_PixMap> mergeImages(const occ::handle<Image_PixMap>& theImage1,
                                        const occ::handle<Image_PixMap>& theImage2)
{
  if (theImage1.IsNull() && theImage2.IsNull())
  {
    return occ::handle<Image_PixMap>();
  }

  occ::handle<Image_PixMap> aResultImage = new Image_PixMap();

  int aWidth1 = 0, aHeight1 = 0;
  if (!theImage1.IsNull())
  {
    aWidth1  = (int)theImage1->Width();
    aHeight1 = (int)theImage1->Height();
  }

  int aWidth2 = 0, aHeight2 = 0;
  if (!theImage2.IsNull())
  {
    aWidth2  = (int)theImage2->Width();
    aHeight2 = (int)theImage2->Height();
  }

  const int aMaxWidth  = std::max(aWidth1, aWidth2);
  const int aMaxHeight = std::max(aHeight1, aHeight2);
  const int aSize      = std::max(aMaxWidth, aMaxHeight);
  aResultImage->InitZero(Image_Format_Alpha, aSize, aSize);

  if (!theImage1.IsNull())
  {
    const int aXOffset1  = std::abs(aWidth1 - aMaxWidth) / 2;
    const int anYOffset1 = std::abs(aHeight1 - aMaxHeight) / 2;
    for (int anY = 0; anY < aHeight1; anY++)
    {
      uint8_t* anImageLine      = theImage1->ChangeRow(anY);
      uint8_t* aResultImageLine = aResultImage->ChangeRow(anYOffset1 + anY);
      for (int aX = 0; aX < aWidth1; aX++)
      {
        aResultImageLine[aXOffset1 + aX] |= anImageLine[aX];
      }
    }
  }

  if (!theImage2.IsNull())
  {
    const int aXOffset2  = std::abs(aWidth2 - aMaxWidth) / 2;
    const int anYOffset2 = std::abs(aHeight2 - aMaxHeight) / 2;
    for (int anY = 0; anY < aHeight2; anY++)
    {
      uint8_t* anImageLine      = theImage2->ChangeRow(anY);
      uint8_t* aResultImageLine = aResultImage->ChangeRow(anYOffset2 + anY);
      for (int aX = 0; aX < aWidth2; aX++)
      {
        aResultImageLine[aXOffset2 + aX] |= anImageLine[aX];
      }
    }
  }

  return aResultImage;
}

//! Draw inner point as filled rectangle
static occ::handle<NCollection_HArray1<uint8_t>> fillPointBitmap(const int theSize)
{
  const int        aNbBytes = (theSize / 8 + (theSize % 8 ? 1 : 0)) * theSize;
  occ::handle<NCollection_HArray1<uint8_t>> aBitMap  = new NCollection_HArray1<uint8_t>(0, aNbBytes - 1);
  for (int anIter = 0; anIter < aBitMap->Length(); ++anIter)
  {
    aBitMap->SetValue(anIter, 255);
  }
  return aBitMap;
}

//! Returns a marker image for the marker of the specified type and scale.
static occ::handle<Graphic3d_MarkerImage> getTextureImage(const Aspect_TypeOfMarker theMarkerType,
                                                     const float  theScale)
{
  int aWidth = 0, aHeight = 0, anOffset = 0, aNbBytes = 0;
  getMarkerBitMapParam(theMarkerType, theScale, aWidth, aHeight, anOffset, aNbBytes);

  occ::handle<NCollection_HArray1<uint8_t>> aBitMap = new NCollection_HArray1<uint8_t>(0, aNbBytes - 1);
  for (int anIter = 0; anIter < aNbBytes; ++anIter)
  {
    aBitMap->ChangeValue(anIter) = Graphic3d_MarkerImage_myMarkerRaster[anOffset + anIter];
  }

  occ::handle<Graphic3d_MarkerImage> aTexture = new Graphic3d_MarkerImage(aBitMap, aWidth, aHeight);
  return aTexture;
}
} // namespace

//=================================================================================================

Graphic3d_MarkerImage::Graphic3d_MarkerImage(const occ::handle<Image_PixMap>& theImage,
                                             const occ::handle<Image_PixMap>& theImageAlpha)
    : myImage(theImage),
      myImageAlpha(theImageAlpha),
      myMargin(1),
      myWidth((int)theImage->Width()),
      myHeight((int)theImage->Height())
{
  myImageId = TCollection_AsciiString("Graphic3d_MarkerImage_")
              + TCollection_AsciiString(++THE_MARKER_IMAGE_COUNTER);

  myImageAlphaId = TCollection_AsciiString("Graphic3d_MarkerImageAlpha_")
                   + TCollection_AsciiString(THE_MARKER_IMAGE_COUNTER);

  if (!theImageAlpha.IsNull())
  {
    if (theImageAlpha->Format() != Image_Format_Alpha
        && theImageAlpha->Format() != Image_Format_Gray)
    {
      throw Standard_ProgramError("Graphic3d_MarkerImage, wrong color format of alpha image");
    }
    if (theImageAlpha->SizeX() != theImage->SizeX() || theImageAlpha->SizeY() != theImage->SizeY())
    {
      throw Standard_ProgramError("Graphic3d_MarkerImage, wrong dimensions of alpha image");
    }
  }
}

//=================================================================================================

Graphic3d_MarkerImage::Graphic3d_MarkerImage(const TCollection_AsciiString& theId,
                                             const TCollection_AsciiString& theAlphaId,
                                             const occ::handle<Image_PixMap>&    theImage,
                                             const occ::handle<Image_PixMap>&    theImageAlpha)
    : myImageId(theId),
      myImageAlphaId(theAlphaId),
      myImage(theImage),
      myImageAlpha(theImageAlpha),
      myMargin(1),
      myWidth((int)theImage->Width()),
      myHeight((int)theImage->Height())
{
  if (!theImageAlpha.IsNull())
  {
    if (theImageAlpha->Format() != Image_Format_Alpha
        && theImageAlpha->Format() != Image_Format_Gray)
    {
      throw Standard_ProgramError("Graphic3d_MarkerImage, wrong color format of alpha image");
    }
    if (theImageAlpha->SizeX() != theImage->SizeX() || theImageAlpha->SizeY() != theImage->SizeY())
    {
      throw Standard_ProgramError("Graphic3d_MarkerImage, wrong dimensions of alpha image");
    }
  }
}

//=================================================================================================

Graphic3d_MarkerImage::Graphic3d_MarkerImage(const occ::handle<NCollection_HArray1<uint8_t>>& theBitMap,
                                             const int               theWidth,
                                             const int               theHeight)
    : myBitMap(theBitMap),
      myMargin(1),
      myWidth(theWidth),
      myHeight(theHeight)
{
  myImageId = TCollection_AsciiString("Graphic3d_MarkerImage_")
              + TCollection_AsciiString(++THE_MARKER_IMAGE_COUNTER);

  myImageAlphaId = TCollection_AsciiString("Graphic3d_MarkerImageAlpha_")
                   + TCollection_AsciiString(THE_MARKER_IMAGE_COUNTER);
}

//=================================================================================================

bool Graphic3d_MarkerImage::IsColoredImage() const
{
  return !myImage.IsNull() && myImage->Format() != Image_Format_Alpha
         && myImage->Format() != Image_Format_Gray;
}

//=================================================================================================

occ::handle<NCollection_HArray1<uint8_t>> Graphic3d_MarkerImage::GetBitMapArray(
  const double    theAlphaValue,
  const bool theIsTopDown) const
{
  if (!myBitMap.IsNull() || myImage.IsNull())
  {
    return myBitMap;
  }

  const int aNumOfBytesInRow =
    (int)(myImage->Width() / 8) + (myImage->Width() % 8 ? 1 : 0);
  const int aNumOfBytes    = (int)(aNumOfBytesInRow * myImage->Height());
  const int aHeight        = (int)myImage->Height();
  const int aWidth         = (int)myImage->Width();
  occ::handle<NCollection_HArray1<uint8_t>> aBitMap = new NCollection_HArray1<uint8_t>(0, aNumOfBytes - 1);
  aBitMap->Init(0);
  for (int aRow = 0; aRow < aHeight; aRow++)
  {
    const int aResRow = !theIsTopDown ? (aHeight - aRow - 1) : aRow;
    for (int aColumn = 0; aColumn < aWidth; aColumn++)
    {
      const Quantity_ColorRGBA aColor = myImage->PixelColor(aColumn, aRow);
      bool         aBitOn = false;
      if (myImage->Format() == Image_Format_Gray)
      {
        aBitOn = aColor.GetRGB().Red() > theAlphaValue;
      }
      else // if (myImage->Format() == Image_Format_Alpha)
      {
        aBitOn = aColor.Alpha() > theAlphaValue;
      }

      int anIndex = aNumOfBytesInRow * aResRow + aColumn / 8;
      aBitMap->SetValue(
        anIndex,
        (uint8_t)(aBitMap->Value(anIndex) + (aBitOn ? (0x80 >> (aColumn % 8)) : 0)));
    }
  }

  return aBitMap;
}

//=================================================================================================

const occ::handle<Image_PixMap>& Graphic3d_MarkerImage::GetImage()
{
  if (!myImage.IsNull() || myBitMap.IsNull())
  {
    return myImage;
  }

  // Converting a byte array to bitmap image. Row and column offsets are used
  // to store bitmap in a square image, so the image will not be stretched
  // when rendering with point sprites.
  const int aNumOfBytesInRow = myWidth / 8 + (myWidth % 8 ? 1 : 0);
  const int aSize            = std::max(myWidth, myHeight);
  const int aRowOffset       = (aSize - myHeight) / 2 + myMargin;
  const int aColumnOffset    = (aSize - myWidth) / 2 + myMargin;
  const int aLowerIndex      = myBitMap->Lower();

  myImage = new Image_PixMap();
  myImage->InitZero(Image_Format_Alpha, aSize + myMargin * 2, aSize + myMargin * 2);
  for (int aRowIter = 0; aRowIter < myHeight; aRowIter++)
  {
    uint8_t* anImageRow = myImage->ChangeRow(aRowIter + aRowOffset);
    for (int aColumnIter = 0; aColumnIter < myWidth; aColumnIter++)
    {
      bool aBitOn =
        (myBitMap->Value(aLowerIndex + aNumOfBytesInRow * aRowIter + aColumnIter / 8)
         & (0x80 >> (aColumnIter % 8)))
        != 0;
      anImageRow[aColumnIter + aColumnOffset] = aBitOn ? 255 : 0;
    }
  }

  return myImage;
}

//=================================================================================================

const occ::handle<Image_PixMap>& Graphic3d_MarkerImage::GetImageAlpha()
{
  if (!myImageAlpha.IsNull())
  {
    return myImageAlpha;
  }

  if (!myImage.IsNull())
  {
    if (myImage->Format() == Image_Format_Gray || myImage->Format() == Image_Format_Alpha)
    {
      myImageAlpha = myImage;
    }
    else
    {
      myImageAlpha = new Image_PixMap();
      myImageAlpha->InitZero(Image_Format_Alpha, myImage->Width(), myImage->Height());
      myImageAlpha->SetTopDown(false);
      for (size_t aRowIter = 0; aRowIter < myImage->Height(); aRowIter++)
      {
        uint8_t* anImageRow = myImageAlpha->ChangeRow(aRowIter);
        for (size_t aColumnIter = 0; aColumnIter < myImage->Width(); aColumnIter++)
        {
          const Quantity_ColorRGBA aColor =
            myImage->PixelColor((int)aColumnIter, (int)aRowIter);
          anImageRow[aColumnIter] = static_cast<uint8_t>(255.0 * aColor.Alpha());
        }
      }
    }
  }

  return myImageAlpha;
}

//=================================================================================================

const TCollection_AsciiString& Graphic3d_MarkerImage::GetImageId() const
{
  return myImageId;
}

//=================================================================================================

const TCollection_AsciiString& Graphic3d_MarkerImage::GetImageAlphaId() const
{
  return myImageAlphaId;
}

//=================================================================================================

void Graphic3d_MarkerImage::GetTextureSize(int& theWidth,
                                           int& theHeight) const
{
  theWidth  = myWidth;
  theHeight = myHeight;
}

//=================================================================================================

occ::handle<Graphic3d_MarkerImage> Graphic3d_MarkerImage::StandardMarker(
  const Aspect_TypeOfMarker theMarkerType,
  const float  theScale,
  const NCollection_Vec4<float>&     theColor)
{
  if (theMarkerType == Aspect_TOM_USERDEFINED || theMarkerType == Aspect_TOM_EMPTY)
  {
    return occ::handle<Graphic3d_MarkerImage>();
  }

  // predefined markers are defined with 0.5 step
  const int  aScaleInt = int(theScale * 10.0f + 0.5f);
  TCollection_AsciiString aKey      = TCollection_AsciiString("Graphic3d_MarkerImage_")
                                 + THE_MARKER_NAMES[theMarkerType] + "_" + aScaleInt;
  TCollection_AsciiString aKeyA = TCollection_AsciiString("Graphic3d_MarkerImageAlpha_")
                                  + THE_MARKER_NAMES[theMarkerType] + "_" + aScaleInt;
  if (theMarkerType == Aspect_TOM_BALL)
  {
    unsigned int aColor[3] = {(unsigned int)(255.0f * theColor.r()),
                              (unsigned int)(255.0f * theColor.g()),
                              (unsigned int)(255.0f * theColor.b())};
    char         aBytes[8];
    Sprintf(aBytes, "%02X%02X%02X", aColor[0], aColor[1], aColor[2]);
    aKey += aBytes;
  }

  switch (theMarkerType)
  {
    case Aspect_TOM_O_POINT:
    case Aspect_TOM_O_PLUS:
    case Aspect_TOM_O_STAR:
    case Aspect_TOM_O_X: {
      // For this type of markers we merge two base bitmaps into one
      // For example Aspect_TOM_O_PLUS = Aspect_TOM_O + Aspect_TOM_PLUS
      occ::handle<Graphic3d_MarkerImage> aMarkerImage1 = getTextureImage(Aspect_TOM_O, theScale);
      occ::handle<Graphic3d_MarkerImage> aMarkerImage2;
      if (theMarkerType == Aspect_TOM_O_POINT)
      {
        // draw inner point as filled rectangle
        const int aSize = theScale > 7 ? 7 : (int)(theScale + 0.5F);
        occ::handle<NCollection_HArray1<uint8_t>> aBitMap = fillPointBitmap(aSize);
        aMarkerImage2                         = new Graphic3d_MarkerImage(aBitMap, aSize, aSize);
      }
      else
      {
        aMarkerImage2 =
          getTextureImage(Aspect_TypeOfMarker(theMarkerType - Aspect_TOM_O_POINT), theScale);
      }
      occ::handle<Image_PixMap> anImage =
        mergeImages(aMarkerImage1->GetImage(), aMarkerImage2->GetImage());
      occ::handle<Graphic3d_MarkerImage> aNewMarkerImage =
        new Graphic3d_MarkerImage(aKey, aKey, anImage);
      return aNewMarkerImage;
    }
    case Aspect_TOM_RING1:
    case Aspect_TOM_RING2:
    case Aspect_TOM_RING3: {
      const float aDelta = 0.1f;
      float       aScale = theScale;
      float       aLimit = 0.0f;
      if (theMarkerType == Aspect_TOM_RING1)
      {
        aLimit = aScale * 0.2f;
      }
      else if (theMarkerType == Aspect_TOM_RING2)
      {
        aLimit = aScale * 0.5f;
      }
      else if (theMarkerType == Aspect_TOM_RING3)
      {
        aLimit = aScale * 0.8f;
      }

      occ::handle<Image_PixMap> anImage;
      for (; aScale > aLimit && aScale >= 1.0f; aScale -= aDelta)
      {
        anImage = mergeImages(anImage, getTextureImage(Aspect_TOM_O, aScale)->GetImage());
      }
      occ::handle<Graphic3d_MarkerImage> aNewMarkerImage =
        new Graphic3d_MarkerImage(aKey, aKey, anImage);
      return aNewMarkerImage;
    }
    case Aspect_TOM_BALL: {
      int   aWidth = 0, aHeight = 0, anOffset = 0, aNbBytes = 0;
      float aScale = theScale;
      getMarkerBitMapParam(Aspect_TOM_O, aScale, aWidth, aHeight, anOffset, aNbBytes);

      NCollection_Vec4<double> aColor(theColor);

      const int aSize    = std::max(aWidth + 2, aHeight + 2); // includes extra margin
      occ::handle<Image_PixMap>   anImage  = new Image_PixMap();
      occ::handle<Image_PixMap>   anImageA = new Image_PixMap();
      anImage->InitZero(Image_Format_RGBA, aSize, aSize);
      anImageA->InitZero(Image_Format_Alpha, aSize, aSize);

      // we draw a set of circles
      Image_ColorRGBA aColor32;
      aColor32.a() = 255;
      double            aHLS[3];
      const float aDelta = 0.1f;
      while (aScale >= 1.0f)
      {
        Quantity_Color::RgbHls(aColor.r(), aColor.g(), aColor.b(), aHLS[0], aHLS[1], aHLS[2]);
        aHLS[2] *= 0.95; // 5% saturation change
        Quantity_Color::HlsRgb(aHLS[0], aHLS[1], aHLS[2], aColor.r(), aColor.g(), aColor.b());
        aColor32.r() = static_cast<uint8_t>(255.0 * aColor.r());
        aColor32.g() = static_cast<uint8_t>(255.0 * aColor.g());
        aColor32.b() = static_cast<uint8_t>(255.0 * aColor.b());

        const occ::handle<Graphic3d_MarkerImage> aMarker = getTextureImage(Aspect_TOM_O, aScale);
        const occ::handle<Image_PixMap>&         aCircle = aMarker->GetImage();

        const size_t aDiffX = (anImage->SizeX() - aCircle->SizeX()) / 2;
        const size_t aDiffY = (anImage->SizeY() - aCircle->SizeY()) / 2;
        for (size_t aRow = 0; aRow < aCircle->SizeY(); ++aRow)
        {
          const uint8_t* aRowData = aCircle->Row(aRow);
          for (size_t aCol = 0; aCol < aCircle->SizeX(); ++aCol)
          {
            if (aRowData[aCol] != 0)
            {
              anImage->ChangeValue<Image_ColorRGBA>(aDiffX + aRow, aDiffY + aCol) = aColor32;
              anImageA->ChangeValue<uint8_t>(aDiffX + aRow, aDiffY + aCol)  = 255;
            }
          }
        }
        aScale -= aDelta;
      }
      occ::handle<Graphic3d_MarkerImage> aNewMarkerImage =
        new Graphic3d_MarkerImage(aKey, aKeyA, anImage, anImageA);
      return aNewMarkerImage;
    }
    default: {
      occ::handle<Graphic3d_MarkerImage> aNewMarkerImage = getTextureImage(theMarkerType, theScale);
      aNewMarkerImage->myImageId                    = aKey;
      aNewMarkerImage->myImageAlphaId               = aKey;
      return aNewMarkerImage;
    }
  }
}
