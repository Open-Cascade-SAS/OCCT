// Created on: 2021-01-15
// Copyright (c) 2021 OPEN CASCADE SAS
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

#ifndef _OpenGl_DepthPeeling_HeaderFile
#define _OpenGl_DepthPeeling_HeaderFile

#include <OpenGl_FrameBuffer.hxx>
#include <OpenGl_NamedResource.hxx>

//! Class provides FBOs for dual depth peeling.
class OpenGl_DepthPeeling : public OpenGl_NamedResource
{
  DEFINE_STANDARD_RTTIEXT(OpenGl_DepthPeeling, OpenGl_NamedResource)
public:

  //! Constructor.
  Standard_EXPORT OpenGl_DepthPeeling();

  //! Destructor.
  Standard_EXPORT virtual ~OpenGl_DepthPeeling();

  //! Release OpenGL resources
  Standard_EXPORT virtual void Release (OpenGl_Context* theGlCtx) Standard_OVERRIDE;

  //! Returns estimated GPU memory usage for holding data without considering overheads and allocation alignment rules.
  Standard_EXPORT virtual Standard_Size EstimatedDataSize() const Standard_OVERRIDE;

  //! Attach a texture image.
  //! Resets the active FBO to 0.
  Standard_EXPORT void AttachDepthTexture (const Handle(OpenGl_Context)& theCtx,
                                           const Handle(OpenGl_Texture)& theDepthStencilTexture);

  //! Detach a texture image.
  //! Resets the active FBO to 0.
  Standard_EXPORT void DetachDepthTexture (const Handle(OpenGl_Context)& theCtx);

  //! Returns additional buffers for ping-pong
  const Handle(OpenGl_FrameBuffer)* DepthPeelFbosOit() const { return myDepthPeelFbosOit; }

  //! Returns additional buffers for ping-pong
  const Handle(OpenGl_FrameBuffer)* FrontBackColorFbosOit() const { return myFrontBackColorFbosOit; }

  //! Returns additional FBO for depth peeling
  const Handle(OpenGl_FrameBuffer)& BlendBackFboOit() const { return myBlendBackFboOit; }

private:

  Handle(OpenGl_FrameBuffer) myDepthPeelFbosOit[2];      //!< depth + front color + back color
  Handle(OpenGl_FrameBuffer) myFrontBackColorFbosOit[2]; //!< front color + back color
  Handle(OpenGl_FrameBuffer) myBlendBackFboOit;

};

#endif // _OpenGl_DepthPeeling_HeaderFile
