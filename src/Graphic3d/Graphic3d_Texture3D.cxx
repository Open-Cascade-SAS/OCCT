// Copyright (c) 2022 OPEN CASCADE SAS
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

#include <Graphic3d_Texture3D.hxx>

#include <Graphic3d_TextureParams.hxx>
#include <Image_AlienPixMap.hxx>
#include <Message.hxx>

IMPLEMENT_STANDARD_RTTIEXT(Graphic3d_Texture3D, Graphic3d_TextureMap)

// =======================================================================
// function : Graphic3d_Texture3D
// purpose  :
// =======================================================================
Graphic3d_Texture3D::Graphic3d_Texture3D(const TCollection_AsciiString& theFileName)
    : Graphic3d_TextureMap(theFileName, Graphic3d_TypeOfTexture_3D)
{
  myParams->SetFilter(Graphic3d_TOTF_TRILINEAR);
}

// =======================================================================
// function : Graphic3d_Texture3D
// purpose  :
// =======================================================================
Graphic3d_Texture3D::Graphic3d_Texture3D(const Handle(Image_PixMap)& thePixMap)
    : Graphic3d_TextureMap(thePixMap, Graphic3d_TypeOfTexture_3D)
{
  myParams->SetFilter(Graphic3d_TOTF_TRILINEAR);
}

// =======================================================================
// function : Graphic3d_Texture3D
// purpose  :
// =======================================================================
Graphic3d_Texture3D::Graphic3d_Texture3D(
  const NCollection_Array1<TCollection_AsciiString>& theFiles)
    : Graphic3d_TextureMap("", Graphic3d_TypeOfTexture_3D)
{
  myParams->SetFilter(Graphic3d_TOTF_TRILINEAR);
  myPaths.Resize(theFiles.Lower(), theFiles.Upper(), false);
  myPaths.Assign(theFiles);
}

// =======================================================================
// function : ~Graphic3d_Texture3D
// purpose  :
// =======================================================================
Graphic3d_Texture3D::~Graphic3d_Texture3D()
{
  //
}

// =======================================================================
// function : SetImage
// purpose  :
// =======================================================================
void Graphic3d_Texture3D::SetImage(const Handle(Image_PixMap)& thePixMap)
{
  myPixMap = thePixMap;
  myPath   = OSD_Path();

  NCollection_Array1<TCollection_AsciiString> anArr;
  myPaths.Move(anArr);
}

// =======================================================================
// function : GetImage
// purpose  :
// =======================================================================
Handle(Image_PixMap) Graphic3d_Texture3D::GetImage(
  const Handle(Image_SupportedFormats)& theSupported)
{
  if (myPaths.IsEmpty() || !myPixMap.IsNull())
  {
    return base_type::GetImage(theSupported);
  }

  Handle(Image_PixMap)   anImage3D;
  const Standard_Integer aNbSlices = myPaths.Length();
  for (Standard_Integer aSlice = 0; aSlice < aNbSlices; ++aSlice)
  {
    const TCollection_AsciiString& aSlicePath = myPaths[myPaths.Lower() + aSlice];
    Handle(Image_AlienPixMap)      anImage    = new Image_AlienPixMap();
    if (!anImage->Load(aSlicePath))
    {
      Message::SendFail() << "Graphic3d_Texture3D::GetImage() failed to load slice " << aSlice
                          << " from '" << aSlicePath << "'";
      return Handle(Image_PixMap)();
    }

    convertToCompatible(theSupported, anImage);
    if (anImage3D.IsNull())
    {
      myIsTopDown = anImage->IsTopDown();
      anImage3D   = new Image_PixMap();
      anImage3D->SetTopDown(myIsTopDown);
      if (!anImage3D->InitTrash3D(
            anImage->Format(),
            NCollection_Vec3<Standard_Size>(anImage->SizeX(), anImage->SizeY(), aNbSlices),
            anImage->SizeRowBytes()))
      {
        Message::SendFail() << "Graphic3d_Texture3D::GetImage() failed to allocate 3D image "
                            << (int)anImage->SizeX() << "x" << (int)anImage->SizeY() << "x"
                            << aNbSlices;
        return Handle(Image_PixMap)();
      }
    }
    if (anImage->Format() != anImage3D->Format() || anImage->SizeX() != anImage3D->SizeX()
        || anImage->SizeY() != anImage3D->SizeY()
        || anImage->SizeRowBytes() != anImage3D->SizeRowBytes())
    {
      Message::SendFail() << "Graphic3d_Texture3D::GetImage() slice " << aSlice << " from '"
                          << aSlicePath << "' have different dimensions";
      return Handle(Image_PixMap)();
    }

    memcpy(anImage3D->ChangeSlice(aSlice), anImage->Data(), anImage->SizeBytes());
  }

  return anImage3D;
}
