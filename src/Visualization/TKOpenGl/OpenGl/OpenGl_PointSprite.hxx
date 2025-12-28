// Created by: Kirill GAVRILOV
// Copyright (c) 2013-2014 OPEN CASCADE SAS
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

#ifndef OpenGl_PointSprite_HeaderFile
#define OpenGl_PointSprite_HeaderFile

#include <OpenGl_Texture.hxx>

//! Point sprite resource. On modern hardware it will be texture with extra parameters.
//! On ancient hardware sprites will be drawn using bitmaps.
class OpenGl_PointSprite : public OpenGl_Texture
{
  DEFINE_STANDARD_RTTIEXT(OpenGl_PointSprite, OpenGl_Texture)
public:
  //! Create uninitialized resource.
  Standard_EXPORT OpenGl_PointSprite(const TCollection_AsciiString& theResourceId);

  //! Destroy object.
  Standard_EXPORT virtual ~OpenGl_PointSprite();

  //! Destroy object - will release GPU memory if any.
  Standard_EXPORT virtual void Release(OpenGl_Context* theCtx) override;

  //! Returns TRUE for point sprite texture.
  virtual bool IsPointSprite() const override { return true; }

  //! @return true if current object was initialized
  virtual bool IsValid() const override
  {
    return myBitmapList != 0 || myTextureId != NO_TEXTURE;
  }

  //! @return true if this is display list bitmap
  inline bool IsDisplayList() const { return myBitmapList != 0; }

  //! Draw sprite using glBitmap.
  //! Please call glRasterPos3fv() before to setup sprite position.
  Standard_EXPORT void DrawBitmap(const occ::handle<OpenGl_Context>& theCtx) const;

  //! Initialize point sprite as display list
  Standard_EXPORT void SetDisplayList(const occ::handle<OpenGl_Context>& theCtx,
                                      const GLuint                  theBitmapList);

protected:
  // clang-format off
  GLuint myBitmapList; //!< if of display list to draw sprite using glBitmap (for backward compatibility)
  // clang-format on
};

#endif // _OpenGl_PointSprite_H__
