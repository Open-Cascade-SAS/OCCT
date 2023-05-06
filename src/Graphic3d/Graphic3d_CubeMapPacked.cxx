// Author: Ilya Khramov
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

#include <Graphic3d_CubeMapPacked.hxx>

#include <Image_AlienPixMap.hxx>
#include <Image_DDSParser.hxx>

IMPLEMENT_STANDARD_RTTIEXT(Graphic3d_CubeMapPacked, Graphic3d_CubeMap)

// =======================================================================
// function : Graphic3d_CubeMapPacked
// purpose  :
// =======================================================================
Graphic3d_CubeMapPacked::Graphic3d_CubeMapPacked (const TCollection_AsciiString&        theFilePath,
                                                  const Graphic3d_ValidatedCubeMapOrder& theOrder)
  :
  Graphic3d_CubeMap (theFilePath),
  myOrder           (theOrder),
  myTileNumberX     (1)
{}

// =======================================================================
// function : Graphic3d_CubeMapPacked
// purpose  :
// =======================================================================
Graphic3d_CubeMapPacked::Graphic3d_CubeMapPacked (const Handle(Image_PixMap)&           theImage,
                                                  const Graphic3d_ValidatedCubeMapOrder& theOrder)
  :
  Graphic3d_CubeMap (Handle(Image_PixMap)()),
  myOrder           (theOrder),
  myTileNumberX     (1)
{
  if (checkImage (theImage, myTileNumberX))
  {
    myPixMap = theImage;
  }
}

// =======================================================================
// function : CompressedValue
// purpose  :
// =======================================================================
Handle(Image_CompressedPixMap) Graphic3d_CubeMapPacked::CompressedValue (const Handle(Image_SupportedFormats)& theSupported)
{
  if (myTileNumberX == 0
  || !myPixMap.IsNull())
  {
    return Handle(Image_CompressedPixMap)();
  }

  TCollection_AsciiString aFilePath;
  myPath.SystemName (aFilePath);
  if (!aFilePath.IsEmpty())
  {
    const unsigned int aTileIndex = myOrder[myCurrentSide];
    Handle(Image_CompressedPixMap) anImage = Image_DDSParser::Load (theSupported, aFilePath, (Standard_Integer )aTileIndex);
    if (!anImage.IsNull()
      && anImage->NbFaces() == 6
      && anImage->SizeX() == anImage->SizeY())
    {
      myIsTopDown = anImage->IsTopDown();
      return anImage;
    }
  }
  return Handle(Image_CompressedPixMap)();
}

// =======================================================================
// function : Value
// purpose  :
// =======================================================================
Handle(Image_PixMap) Graphic3d_CubeMapPacked::Value (const Handle(Image_SupportedFormats)& theSupported)
{
  if (myTileNumberX != 0)
  {
    if (myPixMap.IsNull())
    {
      TCollection_AsciiString aFilePath;
      myPath.SystemName (aFilePath);
      if (!aFilePath.IsEmpty())
      {
        tryLoadImage (theSupported, aFilePath);
      }
    }

    if (!myPixMap.IsNull())
    {
      Handle(Image_PixMap) aWrapper = new Image_PixMap();

      Standard_Size aTileSize = myPixMap->SizeX() / myTileNumberX;

      myIsTopDown = myPixMap->IsTopDown();

      Graphic3d_CubeMapOrder anOrder = myOrder;

      if (!myIsTopDown)
      {
        myPixMap->SetTopDown (true);
        anOrder.Swap (Graphic3d_CMS_POS_Y, Graphic3d_CMS_NEG_Y);
      }

      unsigned int aTileIndexX = anOrder[myCurrentSide] % myTileNumberX;
      unsigned int aTileIndexY = anOrder[myCurrentSide] / myTileNumberX;

      aTileIndexY = myIsTopDown ? aTileIndexY : (6 / myTileNumberX - 1 - aTileIndexY);

      if (aWrapper->InitWrapper (myPixMap->Format(),
                                 myPixMap->ChangeRawValue(aTileIndexY * aTileSize, aTileIndexX * aTileSize),
                                 aTileSize,
                                 aTileSize,
                                 myPixMap->SizeRowBytes()))
      {
        myPixMap->SetTopDown (myIsTopDown);
        return aWrapper;
      }
      else
      {
        myPixMap->SetTopDown(myIsTopDown);
      }
    }
  }

  return Handle(Image_PixMap)();
}

// =======================================================================
// function : checkOrder
// purpose  :
// =======================================================================
Standard_Boolean Graphic3d_CubeMapPacked::checkOrder (const NCollection_Array1<unsigned int>& theOrder)
{
  Standard_Boolean anOrderIsValid = Standard_True;

  if (theOrder.Size() != 6)
  {
    anOrderIsValid = Standard_False;
  }
  else
  {
    for (unsigned int i = 0; i < 6 && anOrderIsValid; ++i)
    {
      if (theOrder[i] > 5)
      {
        anOrderIsValid = Standard_False;
        break;
      }

      for (unsigned int j = i + 1; j < 6; ++j)
      {
        if (theOrder[i] == theOrder[j])
        {
          anOrderIsValid =  Standard_False;
          break;
        }
      }
    }
  }

  if (!anOrderIsValid)
  {
    throw Standard_Failure ("Ivalid order format in tiles of Graphic3d_CubeMapPacked");
  }

  return anOrderIsValid;
}

// =======================================================================
// function : checkImage
// purpose  :
// =======================================================================
Standard_Boolean Graphic3d_CubeMapPacked::checkImage (const Handle(Image_PixMap)& theImage,
                                                      unsigned int&               theTileNumberX)
{
  Standard_Size aSizeX = theImage->SizeX();
  Standard_Size aSizeY = theImage->SizeY();

  if ((aSizeY % aSizeX == 0) && (aSizeY / aSizeX == 6))
  {
    theTileNumberX = 1;
  }
  else if ((aSizeX % aSizeY == 0) && (aSizeX / aSizeY == 6))
  {
    theTileNumberX = 6;
  }
  else if ((aSizeX % 2 == 0) && (aSizeY % 3 == 0) && (aSizeX / 2 == aSizeY / 3))
  {
    theTileNumberX = 2;
  }
  else if ((aSizeX % 3 == 0) && (aSizeY % 2 == 0) && (aSizeX / 3 == aSizeY / 2))
  {
    theTileNumberX = 3;
  }
  else
  {
    return Standard_False;
  }

  return Standard_True;
}

// =======================================================================
// function : tryLoadImage
// purpose  :
// =======================================================================
void Graphic3d_CubeMapPacked::tryLoadImage (const Handle(Image_SupportedFormats)& theSupported,
                                            const TCollection_AsciiString& theFilePath)
{
  Handle(Image_AlienPixMap) anImage = new Image_AlienPixMap;
  if (anImage->Load (theFilePath))
  {
    if (checkImage (anImage, myTileNumberX))
    {
      convertToCompatible (theSupported, anImage);
      myPixMap = anImage;
    }
  }
}
