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

#ifndef _OpenGl_TextureBuffer_H__
#define _OpenGl_TextureBuffer_H__

#include <Graphic3d_TextureUnit.hxx>
#include <OpenGl_Buffer.hxx>

//! Texture Buffer Object.
//! This is a special 1D texture that VBO-style initialized.
//! The main differences from general 1D texture:
//!  - no interpolation between field;
//!  - greater sizes;
//!  - special sampler object in GLSL shader to access data by index.
//!
//! Notice that though TBO is inherited from VBO this is to unify design
//! user shouldn't cast it to base class and all really useful methods
//! are declared in this class.
class OpenGl_TextureBuffer : public OpenGl_Buffer
{
  DEFINE_STANDARD_RTTIEXT(OpenGl_TextureBuffer, OpenGl_Buffer)
public:
  //! Helpful constants
  static const unsigned int NO_TEXTURE = 0;

public:
  //! Create uninitialized TBO.
  Standard_EXPORT OpenGl_TextureBuffer();

  //! Destroy object, will throw exception if GPU memory not released with Release() before.
  Standard_EXPORT virtual ~OpenGl_TextureBuffer();

  //! Override VBO target
  Standard_EXPORT virtual unsigned int GetTarget() const Standard_OVERRIDE;

  //! Returns true if TBO is valid.
  //! Notice that no any real GL call is performed!
  bool IsValid() const { return OpenGl_Buffer::IsValid() && myTextureId != NO_TEXTURE; }

  //! Destroy object - will release GPU memory if any.
  Standard_EXPORT virtual void Release(OpenGl_Context* theGlCtx) Standard_OVERRIDE;

  //! Creates VBO and Texture names (ids) if not yet generated.
  //! Data should be initialized by another method.
  Standard_EXPORT bool Create(const Handle(OpenGl_Context)& theGlCtx) Standard_OVERRIDE;

  //! Perform TBO initialization with specified data.
  //! Existing data will be deleted.
  Standard_EXPORT bool Init(const Handle(OpenGl_Context)& theGlCtx,
                            const unsigned int            theComponentsNb,
                            const Standard_Integer        theElemsNb,
                            const float*                  theData);

  //! Perform TBO initialization with specified data.
  //! Existing data will be deleted.
  Standard_EXPORT bool Init(const Handle(OpenGl_Context)& theGlCtx,
                            const unsigned int            theComponentsNb,
                            const Standard_Integer        theElemsNb,
                            const unsigned int*           theData);

  //! Perform TBO initialization with specified data.
  //! Existing data will be deleted.
  Standard_EXPORT bool Init(const Handle(OpenGl_Context)& theGlCtx,
                            const unsigned int            theComponentsNb,
                            const Standard_Integer        theElemsNb,
                            const unsigned short*         theData);

  //! Perform TBO initialization with specified data.
  //! Existing data will be deleted.
  Standard_EXPORT bool Init(const Handle(OpenGl_Context)& theGlCtx,
                            const unsigned int            theComponentsNb,
                            const Standard_Integer        theElemsNb,
                            const Standard_Byte*          theData);

  //! Bind TBO to specified Texture Unit.
  Standard_EXPORT void BindTexture(const Handle(OpenGl_Context)& theGlCtx,
                                   const Graphic3d_TextureUnit   theTextureUnit) const;

  //! Unbind TBO.
  Standard_EXPORT void UnbindTexture(const Handle(OpenGl_Context)& theGlCtx,
                                     const Graphic3d_TextureUnit   theTextureUnit) const;

  //! Returns name of TBO.
  unsigned int TextureId() const { return myTextureId; }

  //! Returns internal texture format.
  unsigned int TextureFormat() const { return myTexFormat; }

protected:
  unsigned int myTextureId; //!< texture id
  unsigned int myTexFormat; //!< internal texture format
};

DEFINE_STANDARD_HANDLE(OpenGl_TextureBuffer, OpenGl_Buffer)

#endif // _OpenGl_TextureBuffer_H__
