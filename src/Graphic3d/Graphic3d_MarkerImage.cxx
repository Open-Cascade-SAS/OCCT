// Created on: 2013-06-25
// Created by: Dmitry BOBYLEV
// Copyright (c) 2013 OPEN CASCADE SAS
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

#include <Graphic3d_MarkerImage.hxx>

#include <Image_PixMap.hxx>
#include <Standard_Atomic.hxx>
#include <TColStd_HArray1OfByte.hxx>

namespace
{
  static volatile Standard_Integer THE_MARKER_IMAGE_COUNTER = 0;
};

IMPLEMENT_STANDARD_HANDLE (Graphic3d_MarkerImage, Standard_Transient)
IMPLEMENT_STANDARD_RTTIEXT(Graphic3d_MarkerImage, Standard_Transient)

// =======================================================================
// function : Graphic3d_MarkerImage
// purpose  :
// =======================================================================
Graphic3d_MarkerImage::Graphic3d_MarkerImage (const Handle(Image_PixMap)& theImage)
: myBitMap (NULL),
  myImage  (theImage),
  myImageAlpha (NULL),
  myMargin (1),
  myWidth  ((Standard_Integer )theImage->Width()),
  myHeight ((Standard_Integer )theImage->Height())
{
  myImageId = TCollection_AsciiString ("Graphic3d_MarkerImage_")
            + TCollection_AsciiString (Standard_Atomic_Increment (&THE_MARKER_IMAGE_COUNTER));

  myImageAlphaId = TCollection_AsciiString ("Graphic3d_MarkerImageAlpha_")
                 + TCollection_AsciiString (THE_MARKER_IMAGE_COUNTER);
}

// =======================================================================
// function : Graphic3d_MarkerImage
// purpose  :
// =======================================================================
Graphic3d_MarkerImage::Graphic3d_MarkerImage (const Handle(TColStd_HArray1OfByte)& theBitMap,
                                              const Standard_Integer& theWidth,
                                              const Standard_Integer& theHeight)
: myBitMap (theBitMap),
  myImage  (NULL),
  myImageAlpha (NULL),
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
// function : GetBitMapArray
// purpose  :
// =======================================================================
Handle(TColStd_HArray1OfByte) Graphic3d_MarkerImage::GetBitMapArray (const Standard_Real& theAlphaValue) const
{
  if (!myBitMap.IsNull())
  {
    return myBitMap;
  }

  Handle(TColStd_HArray1OfByte) aBitMap;
  if (myImage.IsNull())
  {
    return aBitMap;
  }

  const Standard_Integer aNumOfBytesInRow = (Standard_Integer )(myImage->Width() / 8) + (myImage->Width() % 8 ? 1 : 0);
  const Standard_Integer aNumOfBytes      = (Standard_Integer )(aNumOfBytesInRow * myImage->Height());
  const Standard_Integer aHeight = (Standard_Integer )myImage->Height();
  const Standard_Integer aWidth  = (Standard_Integer )myImage->Width();
  aBitMap = new TColStd_HArray1OfByte (0, aNumOfBytes - 1);
  aBitMap->Init (0);
  for (Standard_Integer aRow = 0; aRow < aHeight; aRow++)
  {
    for (Standard_Integer aColumn = 0; aColumn < aWidth; aColumn++)
    {
      Quantity_Parameter anAlphaValue;
      Quantity_Color aColor = myImage->PixelColor (aColumn, aRow, anAlphaValue);
      Standard_Boolean aBitOn = Standard_False;

      if (myImage->Format() == Image_PixMap::ImgGray)
      {
        aBitOn = aColor.Red() > theAlphaValue;
      }
      else
      {
        aBitOn = anAlphaValue > theAlphaValue;
      }

      aBitMap->ChangeValue (aNumOfBytesInRow * aRow + aColumn / 8) += aBitOn ? (0x80 >> (aColumn % 8)) : 0;
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
  if (!myImage.IsNull())
  {
    return myImage;
  }

  if (myBitMap.IsNull())
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
  myImage->InitZero (Image_PixMap::ImgGray, aSize + myMargin * 2, aSize + myMargin * 2);
  for (Standard_Integer aRowIter = 0; aRowIter < myHeight; aRowIter++)
  {
    Standard_Byte* anImageRow = myImage->ChangeRow (aRowIter + aRowOffset);
    for (Standard_Integer aColumnIter = 0; aColumnIter < myWidth; aColumnIter++)
    {
      Standard_Boolean aBitOn = myBitMap->Value (aLowerIndex + aNumOfBytesInRow * aRowIter + aColumnIter / 8) & (0x80 >> (aColumnIter % 8));
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
    if (myImage->Format() == Image_PixMap::ImgGray)
    {
      myImageAlpha = myImage;
    }
    else
    {
      myImageAlpha = new Image_PixMap();
      myImageAlpha->InitZero (Image_PixMap::ImgGray, myImage->Width(), myImage->Height());
      myImageAlpha->SetTopDown (Standard_False);
      Quantity_Parameter anAlpha;
      for (Standard_Size aRowIter = 0; aRowIter < myImage->Height(); aRowIter++)
      {
        Standard_Byte* anImageRow = myImageAlpha->ChangeRow (aRowIter);
        for (Standard_Size aColumnIter = 0; aColumnIter < myImage->Width(); aColumnIter++)
        {
          myImage->PixelColor ((Standard_Integer)aColumnIter, (Standard_Integer)aRowIter, anAlpha);
          anImageRow[aColumnIter] = Standard_Byte (255.0 * anAlpha);
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
