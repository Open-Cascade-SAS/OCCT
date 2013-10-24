// Created by: Kirill GAVRILOV
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

#include <OpenGl_PointSprite.hxx>

#include <Graphic3d_TextureParams.hxx>
#include <OpenGl_Context.hxx>
#include <Standard_Assert.hxx>
#include <Image_PixMap.hxx>

IMPLEMENT_STANDARD_HANDLE (OpenGl_PointSprite, OpenGl_Texture)
IMPLEMENT_STANDARD_RTTIEXT(OpenGl_PointSprite, OpenGl_Texture)

// =======================================================================
// function : OpenGl_PointSprite
// purpose  :
// =======================================================================
OpenGl_PointSprite::OpenGl_PointSprite()
: OpenGl_Texture (NULL),
  myBitmapList (0)
{
  //myParams->SetFilter (Graphic3d_TOTF_NEAREST);
  myParams->SetGenMode (Graphic3d_TOTM_SPRITE,
                        Graphic3d_Vec4 (0.0f, 0.0f, 0.0f, 0.0f),
                        Graphic3d_Vec4 (0.0f, 0.0f, 0.0f, 0.0f));
}

// =======================================================================
// function : ~OpenGl_PointSprite
// purpose  :
// =======================================================================
OpenGl_PointSprite::~OpenGl_PointSprite()
{
  Release (NULL);
}

// =======================================================================
// function : Release
// purpose  :
// =======================================================================
void OpenGl_PointSprite::Release (const OpenGl_Context* theGlCtx)
{
  if (myBitmapList != 0)
  {
    if (theGlCtx->IsValid())
    {
      glDeleteLists (myBitmapList, 1);
    }
    myBitmapList = 0;
  }

  OpenGl_Texture::Release (theGlCtx);
}

// =======================================================================
// function : SetDisplayList
// purpose  :
// =======================================================================
void OpenGl_PointSprite::SetDisplayList (const Handle(OpenGl_Context)& theCtx,
                                         const GLuint                  theBitmapList)
{
  Release (theCtx.operator->());
  myBitmapList = theBitmapList;
}


// =======================================================================
// function : DrawBitmap
// purpose  :
// =======================================================================
void OpenGl_PointSprite::DrawBitmap (const Handle(OpenGl_Context)& ) const
{
  if (myBitmapList != 0)
  {
    glCallList (myBitmapList);
  }
}
