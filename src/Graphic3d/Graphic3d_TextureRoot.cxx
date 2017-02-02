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
#include <OSD_Directory.hxx>
#include <OSD_Environment.hxx>
#include <OSD_File.hxx>
#include <OSD_Protection.hxx>
#include <Standard_Atomic.hxx>

IMPLEMENT_STANDARD_RTTIEXT(Graphic3d_TextureRoot, Standard_Transient)

namespace
{
  static volatile Standard_Integer THE_TEXTURE_COUNTER = 0;
}

// =======================================================================
// function : TexturesFolder
// purpose  :
// =======================================================================
TCollection_AsciiString Graphic3d_TextureRoot::TexturesFolder()
{
  static Standard_Boolean IsDefined = Standard_False;
  static TCollection_AsciiString VarName;
  if (!IsDefined)
  {
    IsDefined = Standard_True;
    OSD_Environment aTexDirEnv ("CSF_MDTVTexturesDirectory");
    VarName = aTexDirEnv.Value();
    if (VarName.IsEmpty())
    {
      OSD_Environment aCasRootEnv ("CASROOT");
      VarName = aCasRootEnv.Value();
      if (!VarName.IsEmpty())
      {
        VarName += "/src/Textures";
      }
    }

    if (VarName.IsEmpty())
    {
#ifdef OCCT_DEBUG
      std::cerr << "Both environment variables CSF_MDTVTexturesDirectory and CASROOT are undefined!\n"
                << "At least one should be defined to use standard Textures.\n";
#endif
      throw Standard_Failure("CSF_MDTVTexturesDirectory and CASROOT are undefined");
    }

    const OSD_Path aDirPath (VarName);
    OSD_Directory aDir (aDirPath);
    const TCollection_AsciiString aTexture = VarName + "/2d_MatraDatavision.rgb";
    OSD_File aTextureFile (aTexture);
    if (!aDir.Exists() || !aTextureFile.Exists())
    {
#ifdef OCCT_DEBUG
      std::cerr << " CSF_MDTVTexturesDirectory or CASROOT not correctly setted\n";
      std::cerr << " not all files are found in : "<< VarName.ToCString() << std::endl;
#endif
      throw Standard_Failure("CSF_MDTVTexturesDirectory or CASROOT not correctly setted");
    }
  }
  return VarName;
}

// =======================================================================
// function : Graphic3d_TextureRoot
// purpose  :
// =======================================================================
Graphic3d_TextureRoot::Graphic3d_TextureRoot (const TCollection_AsciiString& theFileName,
                                              const Graphic3d_TypeOfTexture  theType)
: myParams   (new Graphic3d_TextureParams()),
  myPath     (theFileName),
  myRevision (0),
  myType     (theType)
{
  generateId();
}

// =======================================================================
// function : Graphic3d_TextureRoot
// purpose  :
// =======================================================================
Graphic3d_TextureRoot::Graphic3d_TextureRoot (const Handle(Image_PixMap)&   thePixMap,
                                              const Graphic3d_TypeOfTexture theType)
: myParams   (new Graphic3d_TextureParams()),
  myPixMap   (thePixMap),
  myRevision (0),
  myType     (theType)
{
  generateId();
}

// =======================================================================
// function : ~Graphic3d_TextureRoot
// purpose  :
// =======================================================================
Graphic3d_TextureRoot::~Graphic3d_TextureRoot()
{
  //
}

// =======================================================================
// function : generateId
// purpose  :
// =======================================================================
void Graphic3d_TextureRoot::generateId()
{
  myTexId = TCollection_AsciiString ("Graphic3d_TextureRoot_")
          + TCollection_AsciiString (Standard_Atomic_Increment (&THE_TEXTURE_COUNTER));
}

// =======================================================================
// function : GetImage
// purpose  :
// =======================================================================
Handle(Image_PixMap) Graphic3d_TextureRoot::GetImage() const
{
  // Case 1: texture source is specified as pixmap
  if (!myPixMap.IsNull())
  {
    return myPixMap;
  }

  // Case 2: texture source is specified as path
  TCollection_AsciiString aFilePath;
  myPath.SystemName (aFilePath);
  if (aFilePath.IsEmpty())
  {
    return Handle(Image_PixMap)();
  }

  Handle(Image_AlienPixMap) anImage = new Image_AlienPixMap();
  if (!anImage->Load (aFilePath))
  {
    return Handle(Image_PixMap)();
  }

  return anImage;
}

// =======================================================================
// function : IsDone
// purpose  :
// =======================================================================
Standard_Boolean Graphic3d_TextureRoot::IsDone() const
{
  // Case 1: texture source is specified as pixmap
  if (!myPixMap.IsNull())
  {
    return !myPixMap->IsEmpty();
  }

  // Case 2: texture source is specified as path
  OSD_File aTextureFile (myPath);
  return aTextureFile.Exists();
}
