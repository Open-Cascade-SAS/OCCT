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
#include <Graphic3d_GraphicDevice.hxx>
#include <Graphic3d_GraphicDriver.hxx>

#include <Image_AlienPixMap.hxx>

#include <OSD_Protection.hxx>
#include <OSD_File.hxx>
#include <stdio.h>

// =======================================================================
// function : Graphic3d_TextureRoot
// purpose  :
// =======================================================================
Graphic3d_TextureRoot::Graphic3d_TextureRoot (const Handle(Graphic3d_StructureManager)& theSM,
                                              const Standard_CString                    thePath,
                                              const Standard_CString                    theFileName,
                                              const Graphic3d_TypeOfTexture             theType)
: myGraphicDriver (Handle(Graphic3d_GraphicDriver)::DownCast (theSM->GraphicDevice()->GraphicDriver())),
  myTexId (-1),
  myPath (theFileName),
  myType (theType),
  myTexUpperBounds (new TColStd_HArray1OfReal (1, 2)) // currently always allocating an array for two texture bounds...
{
  if (thePath != NULL && (strlen (thePath) > 0))
  {
    myPath.SetTrek (TCollection_AsciiString (thePath));
  }

  if (theFileName == NULL || (strlen (theFileName) <= 0))
  {
    return;
  }

  TCollection_AsciiString aFilePath;
  myPath.SystemName (aFilePath);
  Image_AlienPixMap anImage;
  if (!anImage.Load (aFilePath))
  {
    return;
  }

  myTexId = myGraphicDriver->CreateTexture (myType, anImage, theFileName, myTexUpperBounds);
  Update();
}

// =======================================================================
// function : Destroy
// purpose  :
// =======================================================================
void Graphic3d_TextureRoot::Destroy() const
{
  if (IsValid())
  {
    myGraphicDriver->DestroyTexture (myTexId);
  }
}

// =======================================================================
// function : TextureId
// purpose  :
// =======================================================================
Standard_Integer Graphic3d_TextureRoot::TextureId() const
{
  return myTexId;
}

// =======================================================================
// function : Update
// purpose  :
// =======================================================================
void Graphic3d_TextureRoot::Update() const
{
  if (IsValid())
  {
    myGraphicDriver->ModifyTexture (myTexId, MyCInitTexture);
  }
}

// =======================================================================
// function : IsValid
// purpose  :
// =======================================================================
Standard_Boolean Graphic3d_TextureRoot::IsValid() const
{
  return myTexId >= 0;
}

// =======================================================================
// function : IsDone
// purpose  :
// =======================================================================
Standard_Boolean Graphic3d_TextureRoot::IsDone() const
{
  return myTexId >= 0;
}

// =======================================================================
// function : LoadTexture
// purpose  :
// =======================================================================
Standard_Boolean Graphic3d_TextureRoot::LoadTexture (const Image_PixMap& theImage)
{
  if (myTexId >= 0)
  {
    myGraphicDriver->DestroyTexture (myTexId);
  }

  myTexId = myGraphicDriver->CreateTexture (myType, theImage, "", myTexUpperBounds);
  Update();
  return IsValid();
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

// =======================================================================
// function : GetTexUpperBounds
// purpose  :
// =======================================================================
Handle(TColStd_HArray1OfReal) Graphic3d_TextureRoot::GetTexUpperBounds() const
{
  return myTexUpperBounds;
}
