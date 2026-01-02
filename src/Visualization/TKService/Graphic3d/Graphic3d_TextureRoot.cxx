// Created on: 1997-07-28
// Created by: Pierre CHALAMET
// Copyright (c) 1997-1999 Matra Datavision
// Copyright (c) 1999-2014 OPEN CASCADE SAS
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

#include <Graphic3d_TextureRoot.hxx>

#include <Graphic3d_GraphicDriver.hxx>
#include <Graphic3d_TextureParams.hxx>
#include <Image_AlienPixMap.hxx>
#include <Image_DDSParser.hxx>
#include <Image_SupportedFormats.hxx>
#include <OSD_Directory.hxx>
#include <OSD_Environment.hxx>
#include <OSD_File.hxx>
#include <OSD_OpenFile.hxx>

#include <atomic>

IMPLEMENT_STANDARD_RTTIEXT(Graphic3d_TextureRoot, Standard_Transient)

namespace
{
static std::atomic<int> THE_TEXTURE_COUNTER(0);
}

//=================================================================================================

TCollection_AsciiString Graphic3d_TextureRoot::TexturesFolder()
{
  static bool                    IsDefined = false;
  static TCollection_AsciiString VarName;
  if (!IsDefined)
  {
    IsDefined = true;
    OSD_Environment aTexDirEnv("CSF_MDTVTexturesDirectory");
    VarName = aTexDirEnv.Value();
    if (VarName.IsEmpty())
    {
      OSD_Environment aCasRootEnv("CASROOT");
      VarName = aCasRootEnv.Value();
      if (!VarName.IsEmpty())
      {
        VarName += "/src/Textures";
      }
    }

    if (VarName.IsEmpty())
    {
#ifdef OCCT_DEBUG
      std::cerr
        << "Both environment variables CSF_MDTVTexturesDirectory and CASROOT are undefined!\n"
        << "At least one should be defined to use standard Textures.\n";
#endif
      throw Standard_Failure("CSF_MDTVTexturesDirectory and CASROOT are undefined");
    }

    const OSD_Path                aDirPath(VarName);
    OSD_Directory                 aDir(aDirPath);
    const TCollection_AsciiString aTexture = VarName + "/2d_MatraDatavision.rgb";
    OSD_File                      aTextureFile(aTexture);
    if (!aDir.Exists() || !aTextureFile.Exists())
    {
#ifdef OCCT_DEBUG
      std::cerr << " CSF_MDTVTexturesDirectory or CASROOT not correctly set\n";
      std::cerr << " not all files are found in : " << VarName.ToCString() << std::endl;
#endif
      throw Standard_Failure("CSF_MDTVTexturesDirectory or CASROOT not correctly set");
    }
  }
  return VarName;
}

//=================================================================================================

Graphic3d_TextureRoot::Graphic3d_TextureRoot(const TCollection_AsciiString& theFileName,
                                             const Graphic3d_TypeOfTexture  theType)
    : myParams(new Graphic3d_TextureParams()),
      myPath(theFileName),
      myRevision(0),
      myType(theType == Graphic3d_TOT_2D_MIPMAP ? Graphic3d_TypeOfTexture_2D : theType),
      myIsColorMap(true),
      myIsTopDown(true),
      myHasMipmaps(theType == Graphic3d_TOT_2D_MIPMAP)
{
  generateId();
}

//=================================================================================================

Graphic3d_TextureRoot::Graphic3d_TextureRoot(const occ::handle<Image_PixMap>& thePixMap,
                                             const Graphic3d_TypeOfTexture    theType)
    : myParams(new Graphic3d_TextureParams()),
      myPixMap(thePixMap),
      myRevision(0),
      myType(theType == Graphic3d_TOT_2D_MIPMAP ? Graphic3d_TypeOfTexture_2D : theType),
      myIsColorMap(true),
      myIsTopDown(true),
      myHasMipmaps(theType == Graphic3d_TOT_2D_MIPMAP)
{
  generateId();
}

//=================================================================================================

Graphic3d_TextureRoot::~Graphic3d_TextureRoot()
= default;

//=================================================================================================

void Graphic3d_TextureRoot::generateId()
{
  myTexId = TCollection_AsciiString("Graphic3d_TextureRoot_")
            + TCollection_AsciiString(++THE_TEXTURE_COUNTER);
}

//=================================================================================================

occ::handle<Image_CompressedPixMap> Graphic3d_TextureRoot::GetCompressedImage(
  const occ::handle<Image_SupportedFormats>& theSupported)
{
  if (!myPixMap.IsNull())
  {
    return occ::handle<Image_CompressedPixMap>();
  }

  // Case 2: texture source is specified as path
  TCollection_AsciiString aFilePath;
  myPath.SystemName(aFilePath);
  if (aFilePath.IsEmpty())
  {
    return occ::handle<Image_CompressedPixMap>();
  }

  TCollection_AsciiString aFilePathLower = aFilePath;
  aFilePathLower.LowerCase();
  if (!aFilePathLower.EndsWith(".dds"))
  {
    // do not waste time on file system access in case of wrong file extension
    return occ::handle<Image_CompressedPixMap>();
  }

  if (occ::handle<Image_CompressedPixMap> anImage =
        Image_DDSParser::Load(theSupported, aFilePath, 0))
  {
    myIsTopDown = anImage->IsTopDown();
    return anImage;
  }
  return occ::handle<Image_CompressedPixMap>();
}

//=================================================================================================

occ::handle<Image_PixMap> Graphic3d_TextureRoot::GetImage(
  const occ::handle<Image_SupportedFormats>& theSupported)
{
  if (occ::handle<Image_PixMap> anOldImage = GetImage())
  {
    myIsTopDown = anOldImage->IsTopDown();
    return anOldImage; // compatibility with old API
  }

  // Case 1: texture source is specified as pixmap
  if (!myPixMap.IsNull())
  {
    myIsTopDown = myPixMap->IsTopDown();
    return myPixMap;
  }

  // Case 2: texture source is specified as path
  TCollection_AsciiString aFilePath;
  myPath.SystemName(aFilePath);
  if (aFilePath.IsEmpty())
  {
    return occ::handle<Image_PixMap>();
  }

  occ::handle<Image_AlienPixMap> anImage = new Image_AlienPixMap();
  if (anImage->Load(aFilePath))
  {
    myIsTopDown = anImage->IsTopDown();
    convertToCompatible(theSupported, anImage);
    return anImage;
  }

  return occ::handle<Image_PixMap>();
}

//=================================================================================================

void Graphic3d_TextureRoot::convertToCompatible(
  const occ::handle<Image_SupportedFormats>& theSupported,
  const occ::handle<Image_PixMap>&           theImage)
{
  if (theImage.IsNull() || theSupported.IsNull() || theSupported->IsSupported(theImage->Format()))
  {
    return;
  }

  switch (theImage->Format())
  {
    // BGR formats are unsupported in OpenGL ES, only RGB
    case Image_Format_BGR:
      Image_PixMap::SwapRgbaBgra(*theImage);
      theImage->SetFormat(Image_Format_RGB);
      break;
    case Image_Format_BGRA:
    case Image_Format_BGR32:
      Image_PixMap::SwapRgbaBgra(*theImage);
      theImage->SetFormat(theImage->Format() == Image_Format_BGR32 ? Image_Format_RGB32
                                                                   : Image_Format_RGBA);
      break;
    default:
      break;
  }
}

//=================================================================================================

bool Graphic3d_TextureRoot::IsDone() const
{
  // Case 1: texture source is specified as pixmap
  if (!myPixMap.IsNull())
  {
    return !myPixMap->IsEmpty();
  }

  // Case 2: texture source is specified as path
  OSD_File aTextureFile(myPath);
  return aTextureFile.Exists();
}
