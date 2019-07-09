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

#include <Graphic3d_CubeMapSeparate.hxx>
#include <Image_AlienPixMap.hxx>
#include <Message.hxx>
#include <Message_Messenger.hxx>
#include <OSD_File.hxx>

IMPLEMENT_STANDARD_RTTIEXT(Graphic3d_CubeMapSeparate, Graphic3d_CubeMap)

// =======================================================================
// function : Graphic3d_CubeMapSeparate
// purpose  :
// =======================================================================
Graphic3d_CubeMapSeparate::Graphic3d_CubeMapSeparate (const NCollection_Array1<TCollection_AsciiString>& thePaths)
{
  if (thePaths.Size() == 6)
  {
    for (unsigned int i = 0; i < 6; ++i)
    {
      myPaths[i] = thePaths[i];
    }
  }
  else
  {
    throw Standard_Failure("Invalid number of paths to load Graphic3d_CubeMapSeparate");
  }
}

// =======================================================================
// function : Graphic3d_CubeMapSeparate
// purpose  :
// =======================================================================
Graphic3d_CubeMapSeparate::Graphic3d_CubeMapSeparate (const NCollection_Array1<Handle(Image_PixMap)>& theImages)
{
  if (theImages.Size() == 6)
  {
    if (theImages[0].IsNull())
    {
      return;
    }

    if (theImages[0]->SizeX() != theImages[0]->SizeY())
    {
      return;
    }

    myImages[0] = theImages[0];
    myIsTopDown = myImages[0]->IsTopDown();

    for (unsigned int i = 1; i < 6; ++i)
    {
      if (!theImages[i].IsNull())
      {
        if (theImages[i]->SizeX() == myImages[0]->SizeX()
         && theImages[i]->SizeY() == myImages[0]->SizeY()
         && theImages[i]->Format() == myImages[0]->Format()
         && theImages[i]->IsTopDown() == myImages[0]->IsTopDown())
        {
          myImages[i] = theImages[i];
          continue;
        }
      }
      resetImages();
      return;
    }
  }
  else
  {
    throw Standard_Failure("Invalid number of images in Graphic3d_CubeMapSeparate initialization");
  }
}

// =======================================================================
// function : Value
// purpose  :
// =======================================================================
Handle(Image_PixMap) Graphic3d_CubeMapSeparate::Value()
{
  Graphic3d_CubeMapOrder anOrder = Graphic3d_CubeMapOrder::Default();
  if (!myIsTopDown)
  {
    anOrder.Swap(Graphic3d_CMS_POS_Y, Graphic3d_CMS_NEG_Y);
  }

  if (!myImages[anOrder[myCurrentSide]].IsNull())
  {
    return myImages[anOrder[myCurrentSide]];
  }
  else
  {
    TCollection_AsciiString aFilePath;
    myPaths[anOrder[myCurrentSide]].SystemName(aFilePath);
    if (!aFilePath.IsEmpty())
    {
      Handle(Image_AlienPixMap) anImage = new Image_AlienPixMap;
      if (anImage->Load(aFilePath))
      {
        if (anImage->SizeX() == anImage->SizeY())
        {
          if (myCurrentSide == 0)
          {
            mySize =   anImage->SizeX();
            myFormat = anImage->Format();
            myIsTopDown = anImage->IsTopDown();
            return anImage;
          }
          else
          {
            if (anImage->Format() == myFormat
             && anImage->SizeX() == mySize
             && anImage->IsTopDown() == myIsTopDown)
            {
              return anImage;
            }
            else
            {
              Message::DefaultMessenger()->Send(TCollection_AsciiString() +
                "'" + aFilePath + "' inconsistent image format or dimension in Graphic3d_CubeMapSeparate");
            }
          }
        }
      }
      else
      {
        Message::DefaultMessenger()->Send(TCollection_AsciiString() +
          "Unable to load '" + aFilePath + "' image of Graphic3d_CubeMapSeparate");
      }
    }
    else
    {
      Message::DefaultMessenger()->Send(TCollection_AsciiString() +
        "[" + myCurrentSide + "] path of Graphic3d_CubeMapSeparate is invalid");
    }
  }

  return Handle(Image_PixMap)();
}

// =======================================================================
// function : IsDone
// purpose  :
// =======================================================================
Standard_Boolean Graphic3d_CubeMapSeparate::IsDone() const
{
  if (!myImages[0].IsNull())
  {
    return Standard_True;
  }

  for (unsigned int i = 0; i < 6; ++i)
  {
    OSD_File aCubeMapFile(myPaths[i]);
    if (!aCubeMapFile.Exists())
    {
      return Standard_False;
    }
  }

  return Standard_True;
}

// =======================================================================
// function : resetImages
// purpose  :
// =======================================================================
void Graphic3d_CubeMapSeparate::resetImages()
{
  for (unsigned int i = 0; i < 6; ++i)
  { 
    myImages[i].Nullify();
  }
}
