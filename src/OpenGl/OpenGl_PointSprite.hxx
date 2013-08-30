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

#ifndef _OpenGl_PointSprite_H__
#define _OpenGl_PointSprite_H__

#include <OpenGl_Texture.hxx>
#include <Handle_OpenGl_PointSprite.hxx>

//! Point sprite resource. On modern hardware it will be texture with extra parameters.
//! On ancient hardware sprites will be drawn using bitmaps.
class OpenGl_PointSprite : public OpenGl_Texture
{

public:

  //! Create uninitialized resource.
  Standard_EXPORT OpenGl_PointSprite();

  //! Destroy object.
  Standard_EXPORT virtual ~OpenGl_PointSprite();

  //! Destroy object - will release GPU memory if any.
  Standard_EXPORT virtual void Release (const OpenGl_Context* theCtx);

  //! @return true if this is display list bitmap
  inline Standard_Boolean IsDisplayList() const
  {
    return myBitmapList != 0;
  }

  //! Draw sprite using glBitmap.
  //! Please call glRasterPos3fv() before to setup sprite position.
  Standard_EXPORT void DrawBitmap (const Handle(OpenGl_Context)& theCtx) const;

  //! Initialize point sprite as display list
  Standard_EXPORT void SetDisplayList (const Handle(OpenGl_Context)& theCtx,
                                       const GLuint                  theBitmapList);

protected:

  GLuint myBitmapList; //!< if of display list to draw sprite using glBitmap (for backward compatibility)

public:

  DEFINE_STANDARD_RTTI(OpenGl_PointSprite) // Type definition

};

#endif // _OpenGl_PointSprite_H__
