// Created on: 2011-07-13
// Created by: Sergey ZERCHANINOV
// Copyright (c) 2011-2014 OPEN CASCADE SAS
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

#ifndef _OpenGl_AspectFace_Header
#define _OpenGl_AspectFace_Header

#include <OpenGl_AspectLine.hxx>
#include <Graphic3d_AspectFillArea3d.hxx>
#include <Graphic3d_ShaderProgram.hxx>
#include <Graphic3d_TextureMap.hxx>
#include <Graphic3d_BSDF.hxx>

class OpenGl_Texture;

//! The element holding Graphic3d_AspectFillArea3d.
class OpenGl_AspectFace : public OpenGl_Element
{

public:

  //! Empty constructor.
  Standard_EXPORT OpenGl_AspectFace();

  //! Create and assign parameters.
  Standard_EXPORT OpenGl_AspectFace (const Handle(Graphic3d_AspectFillArea3d)& theAspect);

  //! Return aspect.
  const Handle(Graphic3d_AspectFillArea3d)& Aspect() const { return myAspect; }

  //! Assign parameters.
  Standard_EXPORT void SetAspect (const Handle(Graphic3d_AspectFillArea3d)& theAspect);

  //! Set edge aspect.
  void SetAspectEdge (const OpenGl_AspectLine* theAspectEdge) { myAspectEdge = *theAspectEdge; }

  //! @return edge aspect.
  const OpenGl_AspectLine* AspectEdge() const  { return &myAspectEdge; }

  //! Returns true if lighting should be disabled.
  bool IsNoLighting() const { return myIsNoLighting; }

  //! Set if lighting should be disabled or not.
  void SetNoLighting (bool theValue) { myIsNoLighting = theValue; }

  //! @return texture mapping parameters.
  const Handle(Graphic3d_TextureParams)& TextureParams() const
  {
    return myAspect->TextureMap()->GetParams();
  }

  //! @return texture map.
  const Handle(OpenGl_Texture)& TextureRes (const Handle(OpenGl_Context)& theCtx) const
  {
    if (!myResources.IsTextureReady())
    {
      myResources.BuildTexture (theCtx, myAspect->TextureMap());
      myResources.SetTextureReady();
    }

    return myResources.Texture;
  }

  //! Init and return OpenGl shader program resource.
  //! @return shader program resource.
  const Handle(OpenGl_ShaderProgram)& ShaderProgramRes (const Handle(OpenGl_Context)& theCtx) const
  {
    if (!myResources.IsShaderReady())
    {
      myResources.BuildShader (theCtx, myAspect->ShaderProgram());
      myResources.SetShaderReady();
    }

    return myResources.ShaderProgram;
  }

  Standard_EXPORT virtual void Render  (const Handle(OpenGl_Workspace)& theWorkspace) const;
  Standard_EXPORT virtual void Release (OpenGl_Context* theContext);

protected:

  //! OpenGl resources
  mutable struct Resources
  {
  public:
    Resources()
      : myIsTextureReady (Standard_False),
        myIsShaderReady  (Standard_False) {}

    Standard_Boolean IsTextureReady() const { return myIsTextureReady; }
    Standard_Boolean IsShaderReady () const { return myIsShaderReady;  }
    void SetTextureReady() { myIsTextureReady = Standard_True; }
    void SetShaderReady () { myIsShaderReady  = Standard_True; }
    void ResetTextureReadiness() { myIsTextureReady = Standard_False; }
    void ResetShaderReadiness () { myIsShaderReady  = Standard_False; }

    Standard_EXPORT void BuildTexture (const Handle(OpenGl_Context)&          theCtx,
                                       const Handle(Graphic3d_TextureMap)&    theTexture);
    Standard_EXPORT void BuildShader  (const Handle(OpenGl_Context)&          theCtx,
                                       const Handle(Graphic3d_ShaderProgram)& theShader);

    Handle(OpenGl_Texture)       Texture;
    TCollection_AsciiString      TextureId;
    Handle(OpenGl_ShaderProgram) ShaderProgram;
    TCollection_AsciiString      ShaderProgramId;

  private:

    Standard_Boolean myIsTextureReady;
    Standard_Boolean myIsShaderReady;

  } myResources;

  Handle(Graphic3d_AspectFillArea3d) myAspect;
  OpenGl_AspectLine                  myAspectEdge;
  bool                               myIsNoLighting;

public:

  DEFINE_STANDARD_ALLOC

};

#endif //_OpenGl_AspectFace_Header
