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
#include <Standard_Atomic.hxx>
#include <TColStd_HArray1OfByte.hxx>

#include "Graphic3d_MarkerImage.pxx"

IMPLEMENT_STANDARD_RTTIEXT(Graphic3d_MarkerImage,Standard_Transient)

namespace
{
  static volatile Standard_Integer THE_MARKER_IMAGE_COUNTER = 0;

  //! Names of built-in markers
  static const char* THE_MARKER_NAMES[Aspect_TOM_USERDEFINED] =
  {
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
  static void getMarkerBitMapParam (const Aspect_TypeOfMarker theMarkerType,
                                    const Standard_ShortReal theScale,
                                    Standard_Integer& theWidth,
                                    Standard_Integer& theHeight,
                                    Standard_Integer& theOffset,
                                    Standard_Integer& theNumOfBytes)
  {
    const Standard_Integer aType = Standard_Integer(theMarkerType > Aspect_TOM_O
                                                  ? Aspect_TOM_O
                                                  : theMarkerType);
    const Standard_Real anIndex = (Standard_Real)(TEL_NO_OF_SIZES - 1) * (theScale - (Standard_Real)TEL_PM_START_SIZE)
                                / (Standard_Real)(TEL_PM_END_SIZE - TEL_PM_START_SIZE);
    Standard_Integer anId = (Standard_Integer)(anIndex + 0.5);
    if (anId < 0)
    {
      anId = 0;
    }
    else if (anId >= TEL_NO_OF_SIZES)
    {
      anId = TEL_NO_OF_SIZES - 1;
    }

    theWidth  = (Standard_Integer)arrPMFontInfo[aType][anId].width;
    theHeight = (Standard_Integer)arrPMFontInfo[aType][anId].height;
    theOffset = arrPMFontInfo[aType][anId].offset;
    const Standard_Integer aNumOfBytesInRow = theWidth / 8 + (theWidth % 8 ? 1 : 0);
    theNumOfBytes = theHeight * aNumOfBytesInRow;
  }

  //! Merge two image pixmap into one. Used for creating image for following markers:
  //! Aspect_TOM_O_POINT, Aspect_TOM_O_PLUS, Aspect_TOM_O_STAR, Aspect_TOM_O_X, Aspect_TOM_RING1, Aspect_TOM_RING2, Aspect_TOM_RING3
  static Handle(Image_PixMap) mergeImages (const Handle(Image_PixMap)& theImage1,
                                           const Handle(Image_PixMap)& theImage2)
  {
    if (theImage1.IsNull() && theImage2.IsNull())
    {
      return Handle(Image_PixMap)();
    }

    Handle(Image_PixMap) aResultImage = new Image_PixMap();

    Standard_Integer aWidth1 = 0, aHeight1 = 0;
    if (!theImage1.IsNull())
    {
      aWidth1  = (Standard_Integer )theImage1->Width();
      aHeight1 = (Standard_Integer )theImage1->Height();
    }

    Standard_Integer aWidth2 = 0, aHeight2 = 0;
    if (!theImage2.IsNull())
    {
      aWidth2  = (Standard_Integer )theImage2->Width();
      aHeight2 = (Standard_Integer )theImage2->Height();
    }

    const Standard_Integer aMaxWidth  = Max (aWidth1,  aWidth2);
    const Standard_Integer aMaxHeight = Max (aHeight1, aHeight2);
    const Standard_Integer aSize = Max (aMaxWidth, aMaxHeight);
    aResultImage->InitZero (Image_Format_Alpha, aSize, aSize);

    if (!theImage1.IsNull())
    {
      const Standard_Integer aXOffset1  = Abs (aWidth1  - aMaxWidth)  / 2;
      const Standard_Integer anYOffset1 = Abs (aHeight1 - aMaxHeight) / 2;
      for (Standard_Integer anY = 0; anY < aHeight1; anY++)
      {
        Standard_Byte* anImageLine = theImage1->ChangeRow (anY);
        Standard_Byte* aResultImageLine = aResultImage->ChangeRow (anYOffset1 + anY);
        for (Standard_Integer aX = 0; aX < aWidth1; aX++)
        {
          aResultImageLine[aXOffset1 + aX] |= anImageLine[aX];
        }
      }
    }

    if (!theImage2.IsNull())
    {
      const Standard_Integer aXOffset2  = Abs (aWidth2  - aMaxWidth)  / 2;
      const Standard_Integer anYOffset2 = Abs (aHeight2 - aMaxHeight) / 2;
      for (Standard_Integer anY = 0; anY < aHeight2; anY++)
      {
        Standard_Byte* anImageLine = theImage2->ChangeRow (anY);
        Standard_Byte* aResultImageLine = aResultImage->ChangeRow (anYOffset2 + anY);
        for (Standard_Integer aX = 0; aX < aWidth2; aX++)
        {
          aResultImageLine[aXOffset2 + aX] |= anImageLine[aX];
        }
      }
    }

    return aResultImage;
  }

  //! Draw inner point as filled rectangle
  static Handle(TColStd_HArray1OfByte) fillPointBitmap (const Standard_Integer theSize)
  {
    const Standard_Integer        aNbBytes = (theSize / 8 + (theSize % 8 ? 1 : 0)) * theSize;
    Handle(TColStd_HArray1OfByte) aBitMap = new TColStd_HArray1OfByte (0, aNbBytes - 1);
    for (Standard_Integer anIter = 0; anIter < aBitMap->Length(); ++anIter)
    {
      aBitMap->SetValue (anIter, 255);
    }
    return aBitMap;
  }

  //! Returns a marker image for the marker of the specified type and scale.
  static Handle(Graphic3d_MarkerImage) getTextureImage (const Aspect_TypeOfMarker theMarkerType,
                                                        const Standard_ShortReal  theScale)
  {
    Standard_Integer aWidth = 0, aHeight = 0, anOffset = 0, aNbBytes = 0;
    getMarkerBitMapParam (theMarkerType, theScale, aWidth, aHeight, anOffset, aNbBytes);

    Handle(TColStd_HArray1OfByte) aBitMap = new TColStd_HArray1OfByte (0, aNbBytes - 1);
    for (Standard_Integer anIter = 0; anIter < aNbBytes; ++anIter)
    {
      aBitMap->ChangeValue (anIter) = Graphic3d_MarkerImage_myMarkerRaster[anOffset + anIter];
    }

    Handle(Graphic3d_MarkerImage) aTexture = new Graphic3d_MarkerImage (aBitMap, aWidth, aHeight);
    return aTexture;
  }
}

// =======================================================================
// function : Graphic3d_MarkerImage
// purpose  :
// =======================================================================
Graphic3d_MarkerImage::Graphic3d_MarkerImage (const Handle(Image_PixMap)& theImage,
                                              const Handle(Image_PixMap)& theImageAlpha)
: myImage  (theImage),
  myImageAlpha (theImageAlpha),
  myMargin (1),
  myWidth  ((Standard_Integer )theImage->Width()),
  myHeight ((Standard_Integer )theImage->Height())
{
  myImageId = TCollection_AsciiString ("Graphic3d_MarkerImage_")
            + TCollection_AsciiString (Standard_Atomic_Increment (&THE_MARKER_IMAGE_COUNTER));

  myImageAlphaId = TCollection_AsciiString ("Graphic3d_MarkerImageAlpha_")
                 + TCollection_AsciiString (THE_MARKER_IMAGE_COUNTER);

  if (!theImageAlpha.IsNull())
  {
    if (theImageAlpha->Format() != Image_Format_Alpha
     && theImageAlpha->Format() != Image_Format_Gray)
    {
      throw Standard_ProgramError ("Graphic3d_MarkerImage, wrong color format of alpha image");
    }
    if (theImageAlpha->SizeX() != theImage->SizeX()
     || theImageAlpha->SizeY() != theImage->SizeY())
    {
      throw Standard_ProgramError ("Graphic3d_MarkerImage, wrong dimensions of alpha image");
    }
  }
}

// =======================================================================
// function : Graphic3d_MarkerImage
// purpose  :
// =======================================================================
Graphic3d_MarkerImage::Graphic3d_MarkerImage (const TCollection_AsciiString& theId,
                                              const TCollection_AsciiString& theAlphaId,
                                              const Handle(Image_PixMap)& theImage,
                                              const Handle(Image_PixMap)& theImageAlpha)
: myImageId (theId),
  myImageAlphaId (theAlphaId),
  myImage  (theImage),
  myImageAlpha (theImageAlpha),
  myMargin (1),
  myWidth  ((Standard_Integer )theImage->Width()),
  myHeight ((Standard_Integer )theImage->Height())
{
  if (!theImageAlpha.IsNull())
  {
    if (theImageAlpha->Format() != Image_Format_Alpha
     && theImageAlpha->Format() != Image_Format_Gray)
    {
      throw Standard_ProgramError ("Graphic3d_MarkerImage, wrong color format of alpha image");
    }
    if (theImageAlpha->SizeX() != theImage->SizeX()
     || theImageAlpha->SizeY() != theImage->SizeY())
    {
      throw Standard_ProgramError ("Graphic3d_MarkerImage, wrong dimensions of alpha image");
    }
  }
}

// =======================================================================
// function : Graphic3d_MarkerImage
// purpose  :
// =======================================================================
Graphic3d_MarkerImage::Graphic3d_MarkerImage (const Handle(TColStd_HArray1OfByte)& theBitMap,
                                              const Standard_Integer theWidth,
                                              const Standard_Integer theHeight)
: myBitMap (theBitMap),
  myMargin (1),
  myWidth  (theWidth),
  myHeight (theHeight)
{
  myImageId = TCollection_AsciiString ("Graphic3d_MarkerImage_")
            + TCollection_AsciiString (Standard_Atomic_Increment (&THE_MARKER_IMAGE_COUNTER));

  myImageAlphaId = TCollection_AsciiString ("Graphic3d_MarkerImageAlpha_")
                 + TCollection_AsciiString (THE_MARKER_IMAGE_COUNTER);
}

// =======================================================================
// function : IsColoredImage
// purpose  :
// =======================================================================
bool Graphic3d_MarkerImage::IsColoredImage() const
{
  return !myImage.IsNull()
       && myImage->Format() != Image_Format_Alpha
       && myImage->Format() != Image_Format_Gray;
}

// =======================================================================
// function : GetBitMapArray
// purpose  :
// =======================================================================
Handle(TColStd_HArray1OfByte) Graphic3d_MarkerImage::GetBitMapArray (const Standard_Real theAlphaValue,
                                                                     const Standard_Boolean theIsTopDown) const
{
  if (!myBitMap.IsNull()
    || myImage.IsNull())
  {
    return myBitMap;
  }

  const Standard_Integer aNumOfBytesInRow = (Standard_Integer )(myImage->Width() / 8) + (myImage->Width() % 8 ? 1 : 0);
  const Standard_Integer aNumOfBytes      = (Standard_Integer )(aNumOfBytesInRow * myImage->Height());
  const Standard_Integer aHeight = (Standard_Integer )myImage->Height();
  const Standard_Integer aWidth  = (Standard_Integer )myImage->Width();
  Handle(TColStd_HArray1OfByte) aBitMap = new TColStd_HArray1OfByte (0, aNumOfBytes - 1);
  aBitMap->Init (0);
  for (Standard_Integer aRow = 0; aRow < aHeight; aRow++)
  {
    const Standard_Integer aResRow = !theIsTopDown ? (aHeight - aRow - 1) : aRow;
    for (Standard_Integer aColumn = 0; aColumn < aWidth; aColumn++)
    {
      const Quantity_ColorRGBA aColor = myImage->PixelColor (aColumn, aRow);
      Standard_Boolean aBitOn = Standard_False;
      if (myImage->Format() == Image_Format_Gray)
      {
        aBitOn = aColor.GetRGB().Red() > theAlphaValue;
      }
      else //if (myImage->Format() == Image_Format_Alpha)
      {
        aBitOn = aColor.Alpha() > theAlphaValue;
      }

      Standard_Integer anIndex = aNumOfBytesInRow * aResRow + aColumn / 8;
      aBitMap->SetValue (anIndex, (Standard_Byte)(aBitMap->Value (anIndex) + 
                                                  (aBitOn ? (0x80 >> (aColumn % 8)) : 0)));
    }
  }

  return aBitMap;
}

// =======================================================================
// function : GetImage
// purpose  :
// =======================================================================
const Handle(Image_PixMap)& Graphic3d_MarkerImage::GetImage()
{
  if (!myImage.IsNull()
    || myBitMap.IsNull())
  {
    return myImage;
  }

  // Converting a byte array to bitmap image. Row and column offsets are used
  // to store bitmap in a square image, so the image will not be stretched
  // when rendering with point sprites.
  const Standard_Integer aNumOfBytesInRow = myWidth / 8 + (myWidth % 8 ? 1 : 0);
  const Standard_Integer aSize            = Max (myWidth, myHeight);
  const Standard_Integer aRowOffset       = (aSize - myHeight) / 2 + myMargin;
  const Standard_Integer aColumnOffset    = (aSize - myWidth ) / 2 + myMargin;
  const Standard_Integer aLowerIndex      = myBitMap->Lower();

  myImage = new Image_PixMap();
  myImage->InitZero (Image_Format_Alpha, aSize + myMargin * 2, aSize + myMargin * 2);
  for (Standard_Integer aRowIter = 0; aRowIter < myHeight; aRowIter++)
  {
    Standard_Byte* anImageRow = myImage->ChangeRow (aRowIter + aRowOffset);
    for (Standard_Integer aColumnIter = 0; aColumnIter < myWidth; aColumnIter++)
    {
      Standard_Boolean aBitOn = (myBitMap->Value (aLowerIndex + aNumOfBytesInRow * aRowIter + aColumnIter / 8) & (0x80 >> (aColumnIter % 8))) != 0;
      anImageRow[aColumnIter + aColumnOffset] = aBitOn ? 255 : 0;
    }
  }

  return myImage;
}

// =======================================================================
// function : GetImageAlpha
// purpose  :
// =======================================================================
const Handle(Image_PixMap)& Graphic3d_MarkerImage::GetImageAlpha()
{
  if (!myImageAlpha.IsNull())
  {
    return myImageAlpha;
  }

  if (!myImage.IsNull())
  {
    if (myImage->Format() == Image_Format_Gray
     || myImage->Format() == Image_Format_Alpha)
    {
      myImageAlpha = myImage;
    }
    else
    {
      myImageAlpha = new Image_PixMap();
      myImageAlpha->InitZero (Image_Format_Alpha, myImage->Width(), myImage->Height());
      myImageAlpha->SetTopDown (Standard_False);
      for (Standard_Size aRowIter = 0; aRowIter < myImage->Height(); aRowIter++)
      {
        Standard_Byte* anImageRow = myImageAlpha->ChangeRow (aRowIter);
        for (Standard_Size aColumnIter = 0; aColumnIter < myImage->Width(); aColumnIter++)
        {
          const Quantity_ColorRGBA aColor = myImage->PixelColor ((Standard_Integer)aColumnIter, (Standard_Integer)aRowIter);
          anImageRow[aColumnIter] = Standard_Byte (255.0 * aColor.Alpha());
        }
      }
    }
  }

  return myImageAlpha;
}

// =======================================================================
// function : GetImageId
// purpose  :
// =======================================================================
const TCollection_AsciiString& Graphic3d_MarkerImage::GetImageId() const
{
  return myImageId;
}

// =======================================================================
// function : GetImageAlphaId
// purpose  :
// =======================================================================
const TCollection_AsciiString& Graphic3d_MarkerImage::GetImageAlphaId() const
{
  return myImageAlphaId;
}

// =======================================================================
// function : GetTextureSize
// purpose  :
// =======================================================================
void Graphic3d_MarkerImage::GetTextureSize (Standard_Integer& theWidth,
                                            Standard_Integer& theHeight) const
{
  theWidth  = myWidth;
  theHeight = myHeight;
}

// =======================================================================
// function : GetMarkerImage
// purpose  :
// =======================================================================
Handle(Graphic3d_MarkerImage) Graphic3d_MarkerImage::StandardMarker (const Aspect_TypeOfMarker theMarkerType,
                                                                     const Standard_ShortReal theScale,
                                                                     const Graphic3d_Vec4& theColor)
{
  if (theMarkerType == Aspect_TOM_USERDEFINED
   || theMarkerType == Aspect_TOM_EMPTY)
  {
    return Handle(Graphic3d_MarkerImage)();
  }

  // predefined markers are defined with 0.5 step
  const Standard_Integer aScaleInt = Standard_Integer(theScale * 10.0f + 0.5f);
  TCollection_AsciiString aKey  = TCollection_AsciiString ("Graphic3d_MarkerImage_")      + THE_MARKER_NAMES[theMarkerType] + "_" + aScaleInt;
  TCollection_AsciiString aKeyA = TCollection_AsciiString ("Graphic3d_MarkerImageAlpha_") + THE_MARKER_NAMES[theMarkerType] + "_" + aScaleInt;
  if (theMarkerType == Aspect_TOM_BALL)
  {
    unsigned int aColor[3] =
    {
      (unsigned int )(255.0f * theColor.r()),
      (unsigned int )(255.0f * theColor.g()),
      (unsigned int )(255.0f * theColor.b())
    };
    char aBytes[8];
    sprintf (aBytes, "%02X%02X%02X", aColor[0], aColor[1], aColor[2]);
    aKey += aBytes;
  }

  switch (theMarkerType)
  {
    case Aspect_TOM_O_POINT:
    case Aspect_TOM_O_PLUS:
    case Aspect_TOM_O_STAR:
    case Aspect_TOM_O_X:
    {
      // For this type of markers we merge two base bitmaps into one
      // For example Aspect_TOM_O_PLUS = Aspect_TOM_O + Aspect_TOM_PLUS
      Handle(Graphic3d_MarkerImage) aMarkerImage1 = getTextureImage (Aspect_TOM_O, theScale);
      Handle(Graphic3d_MarkerImage) aMarkerImage2;
      if (theMarkerType == Aspect_TOM_O_POINT)
      {
        // draw inner point as filled rectangle
        const Standard_Integer        aSize = theScale > 7 ? 7 : (Standard_Integer)(theScale + 0.5F);
        Handle(TColStd_HArray1OfByte) aBitMap = fillPointBitmap (aSize);
        aMarkerImage2 = new Graphic3d_MarkerImage (aBitMap, aSize, aSize);
      }
      else
      {
        aMarkerImage2 = getTextureImage (Aspect_TypeOfMarker(theMarkerType - Aspect_TOM_O_POINT), theScale);
      }
      Handle(Image_PixMap) anImage = mergeImages (aMarkerImage1->GetImage(), aMarkerImage2->GetImage());
      Handle(Graphic3d_MarkerImage) aNewMarkerImage = new Graphic3d_MarkerImage (aKey, aKey, anImage);
      return aNewMarkerImage;
    }
    case Aspect_TOM_RING1:
    case Aspect_TOM_RING2:
    case Aspect_TOM_RING3:
    {
      const Standard_ShortReal aDelta = 0.1f;
      Standard_ShortReal aScale = theScale;
      Standard_ShortReal aLimit = 0.0f;
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

      Handle(Image_PixMap) anImage;
      for (; aScale > aLimit && aScale >= 1.0f; aScale -= aDelta)
      {
        anImage = mergeImages (anImage, getTextureImage (Aspect_TOM_O, aScale)->GetImage());
      }
      Handle(Graphic3d_MarkerImage) aNewMarkerImage = new Graphic3d_MarkerImage (aKey, aKey, anImage);
      return aNewMarkerImage;
    }
    case Aspect_TOM_BALL:
    {
      Standard_Integer aWidth = 0, aHeight = 0, anOffset = 0, aNbBytes = 0;
      Standard_ShortReal aScale = theScale;
      getMarkerBitMapParam (Aspect_TOM_O, aScale, aWidth, aHeight, anOffset, aNbBytes);

      NCollection_Vec4<Standard_Real> aColor (theColor);

      const Standard_Integer aSize = Max (aWidth + 2, aHeight + 2); // includes extra margin
      Handle(Image_PixMap) anImage  = new Image_PixMap();
      Handle(Image_PixMap) anImageA = new Image_PixMap();
      anImage ->InitZero (Image_Format_RGBA,  aSize, aSize);
      anImageA->InitZero (Image_Format_Alpha, aSize, aSize);

      // we draw a set of circles
      Image_ColorRGBA aColor32;
      aColor32.a() = 255;
      Standard_Real aHLS[3];
      const Standard_ShortReal aDelta = 0.1f;
      while (aScale >= 1.0f)
      {
        Quantity_Color::RgbHls (aColor.r(), aColor.g(), aColor.b(), aHLS[0], aHLS[1], aHLS[2]);
        aHLS[2] *= 0.95; // 5% saturation change
        Quantity_Color::HlsRgb (aHLS[0], aHLS[1], aHLS[2], aColor.r(), aColor.g(), aColor.b());
        aColor32.r() = Standard_Byte (255.0 * aColor.r());
        aColor32.g() = Standard_Byte (255.0 * aColor.g());
        aColor32.b() = Standard_Byte (255.0 * aColor.b());

        const Handle(Graphic3d_MarkerImage) aMarker = getTextureImage (Aspect_TOM_O, aScale);
        const Handle(Image_PixMap)& aCircle = aMarker->GetImage();

        const Standard_Size aDiffX = (anImage->SizeX() - aCircle->SizeX()) / 2;
        const Standard_Size aDiffY = (anImage->SizeY() - aCircle->SizeY()) / 2;
        for (Standard_Size aRow = 0; aRow < aCircle->SizeY(); ++aRow)
        {
          const Standard_Byte* aRowData = aCircle->Row(aRow);
          for (Standard_Size aCol = 0; aCol < aCircle->SizeX(); ++aCol)
          {
            if (aRowData[aCol] != 0)
            {
              anImage->ChangeValue<Image_ColorRGBA>(aDiffX + aRow, aDiffY + aCol) = aColor32;
              anImageA->ChangeValue<Standard_Byte> (aDiffX + aRow, aDiffY + aCol) = 255;
            }
          }
        }
        aScale -= aDelta;
      }
      Handle(Graphic3d_MarkerImage) aNewMarkerImage = new Graphic3d_MarkerImage (aKey, aKeyA, anImage, anImageA);
      return aNewMarkerImage;
    }
    default:
    {
      Handle(Graphic3d_MarkerImage) aNewMarkerImage = getTextureImage (theMarkerType, theScale);
      aNewMarkerImage->myImageId = aKey;
      aNewMarkerImage->myImageAlphaId = aKey;
      return aNewMarkerImage;
    }
  }
}
