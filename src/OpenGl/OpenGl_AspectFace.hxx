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
#include <OpenGl_TextureSet.hxx>
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

  //! Returne textures map.
  const Handle(OpenGl_TextureSet)& TextureSet (const Handle(OpenGl_Context)& theCtx) const
  {
    if (!myResources.IsTextureReady())
    {
      myResources.BuildTextures (theCtx, myAspect->TextureSet());
      myResources.SetTextureReady();
    }
    return myResources.TextureSet();
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
    //! Empty constructor.
    Resources()
    : myIsTextureReady (Standard_False),
      myIsShaderReady  (Standard_False) {}

    //! Return TRUE if texture resource is up-to-date.
    Standard_Boolean IsTextureReady() const { return myIsTextureReady; }

    //! Return TRUE if shader resource is up-to-date.
    Standard_Boolean IsShaderReady () const { return myIsShaderReady;  }

    //! Set texture resource up-to-date state.
    void SetTextureReady() { myIsTextureReady = Standard_True; }

    //! Set shader resource up-to-date state.
    void SetShaderReady () { myIsShaderReady  = Standard_True; }

    //! Reset shader resource up-to-date state.
    void ResetShaderReadiness () { myIsShaderReady  = Standard_False; }

    //! Return textures array.
    const Handle(OpenGl_TextureSet)& TextureSet() const { return myTextures; }

    //! Update texture resource up-to-date state.
    Standard_EXPORT void UpdateTexturesRediness (const Handle(Graphic3d_TextureSet)& theTextures);

    //! Build texture resource.
    Standard_EXPORT void BuildTextures (const Handle(OpenGl_Context)& theCtx,
                                        const Handle(Graphic3d_TextureSet)& theTextures);

    //! Build shader resource.
    Standard_EXPORT void BuildShader  (const Handle(OpenGl_Context)&          theCtx,
                                       const Handle(Graphic3d_ShaderProgram)& theShader);

    //! Release texture resource.
    Standard_EXPORT void ReleaseTextures (OpenGl_Context* theCtx);

    Handle(OpenGl_ShaderProgram) ShaderProgram;
    TCollection_AsciiString      ShaderProgramId;

  private:

    Handle(OpenGl_TextureSet) myTextures;
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
