// Created on: 1997-07-28
// Created by: Pierre CHALAMET
// Copyright (c) 1997-1999 Matra Datavision
// Copyright (c) 1999-2012 OPEN CASCADE SAS
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

#include <Graphic3d_TextureRoot.ixx>
#include <Graphic3d_GraphicDriver.hxx>

#include <Image_AlienPixMap.hxx>

#include <OSD_Directory.hxx>
#include <OSD_Environment.hxx>
#include <OSD_Path.hxx>
#include <OSD_Protection.hxx>
#include <OSD_File.hxx>
#include <Standard_Atomic.hxx>

namespace
{
  static volatile Standard_Integer THE_TEXTURE_COUNTER = 0;
};

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
      std::cerr << " CSF_MDTVTexturesDirectory and CASROOT not setted\n";
      std::cerr << " one of these variable are mandatory to use this functionality\n";
      Standard_Failure::Raise ("CSF_MDTVTexturesDirectory and CASROOT not setted");
      return VarName;
    }

    const OSD_Path aDirPath (VarName);
    OSD_Directory aDir (aDirPath);
    const TCollection_AsciiString aTexture = VarName + "/2d_MatraDatavision.rgb";
    OSD_File aTextureFile (aTexture);
    if (!aDir.Exists() || !aTextureFile.Exists())
    {
      std::cerr << " CSF_MDTVTexturesDirectory or CASROOT not correctly setted\n";
      std::cerr << " not all files are found in : "<< VarName.ToCString() << std::endl;
      Standard_Failure::Raise ("CSF_MDTVTexturesDirectory or CASROOT not correctly setted");
      return VarName;
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
: myParams (new Graphic3d_TextureParams()),
  myPath   (theFileName),
  myType   (theType)
{
  myTexId = TCollection_AsciiString ("Graphic3d_TextureRoot_") //DynamicType()->Name()
          + TCollection_AsciiString (Standard_Atomic_Increment (&THE_TEXTURE_COUNTER));
}

// =======================================================================
// function : Destroy
// purpose  :
// =======================================================================
void Graphic3d_TextureRoot::Destroy() const
{
  //
}

// =======================================================================
// function : GetId
// purpose  :
// =======================================================================
TCollection_AsciiString Graphic3d_TextureRoot::GetId() const
{
  return myTexId;
}

// =======================================================================
// function : GetParams
// purpose  :
// =======================================================================
const Handle(Graphic3d_TextureParams)& Graphic3d_TextureRoot::GetParams() const
{
  return myParams;
}

// =======================================================================
// function : GetImage
// purpose  :
// =======================================================================
Handle(Image_PixMap) Graphic3d_TextureRoot::GetImage() const
{
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
  OSD_File aTextureFile (myPath);
  return aTextureFile.Exists();
}

// =======================================================================
// function : Path
// purpose  :
// =======================================================================
const OSD_Path& Graphic3d_TextureRoot::Path() const
{
  return myPath;
}

// =======================================================================
// function : Type
// purpose  :
// =======================================================================
Graphic3d_TypeOfTexture Graphic3d_TextureRoot::Type() const
{
  return myType;
}
