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

#include <InterfaceGraphic_telem.hxx>
#include <Aspect_InteriorStyle.hxx>
#include <Aspect_PolygonOffsetMode.hxx>
#include <TCollection_AsciiString.hxx>

#include <OpenGl_AspectLine.hxx>
#include <OpenGl_Element.hxx>

#include <Graphic3d_AspectFillArea3d.hxx>
#include <Graphic3d_CAspectFillArea.hxx>
#include <Graphic3d_ShaderProgram.hxx>
#include <Graphic3d_TextureMap.hxx>
#include <Graphic3d_BSDF.hxx>

class OpenGl_Texture;

#define OPENGL_AMBIENT_MASK  (1<<0)
#define OPENGL_DIFFUSE_MASK  (1<<1)
#define OPENGL_SPECULAR_MASK (1<<2)
#define OPENGL_EMISSIVE_MASK (1<<3)

static const TEL_POFFSET_PARAM THE_DEFAULT_POFFSET = { Aspect_POM_Fill, 1.0F, 0.0F };

struct OPENGL_SURF_PROP
{
  Standard_ShortReal amb;
  Standard_ShortReal diff;
  Standard_ShortReal spec;
  Standard_ShortReal emsv;

  Standard_ShortReal trans;
  Standard_ShortReal shine;
  Standard_ShortReal index;

  Standard_ShortReal env_reflexion;
  Standard_Integer   isphysic;

  unsigned int color_mask;

  TEL_COLOUR speccol;
  TEL_COLOUR difcol;
  TEL_COLOUR ambcol;
  TEL_COLOUR emscol;
  TEL_COLOUR matcol;

  Graphic3d_BSDF BSDF;

  DEFINE_STANDARD_ALLOC
};

class OpenGl_AspectFace : public OpenGl_Element
{

public:

  Standard_EXPORT OpenGl_AspectFace();

  //! Copy parameters
  Standard_EXPORT void SetAspect (const CALL_DEF_CONTEXTFILLAREA& theAspect);
  Standard_EXPORT void SetAspect (const Handle(Graphic3d_AspectFillArea3d)& theAspect);

  //! Set edge aspect.
  void SetAspectEdge (const OpenGl_AspectLine* theAspectEdge)
  {
    myAspectEdge = *theAspectEdge;
  }

  //! @return edge aspect.
  const OpenGl_AspectLine* AspectEdge() const 
  {
    return &myAspectEdge;
  }

  //! @return interior style
  const Aspect_InteriorStyle InteriorStyle() const
  {
    return myInteriorStyle;
  }

  Aspect_InteriorStyle& ChangeInteriorStyle()
  {
    return myInteriorStyle;
  }

  //! @return edge on flag.
  int Edge() const
  {
    return myEdge;
  }

  //! @return edge on flag.
  int& ChangeEdge()
  {
    return myEdge;
  }

  //! @return hatch type.
  int Hatch() const
  {
    return myHatch;
  }

  //! @return hatch type variable.
  int& ChangeHatch()
  {
    return myHatch;
  }

  //! @return distinguishing mode.
  int DistinguishingMode() const
  {
    return myDistinguishingMode;
  }

  //! @return distinguishing mode.
  int& ChangeDistinguishingMode()
  {
    return myDistinguishingMode;
  }

  //! @return culling mode.
  int CullingMode() const
  {
    return myCullingMode;
  }

  //! @return culling mode.
  int& ChangeCullingMode()
  {
    return myCullingMode;
  }

  //! @return front material properties.
  const OPENGL_SURF_PROP& IntFront() const
  {
    return myIntFront;
  }

  //! @return front material properties.
  OPENGL_SURF_PROP& ChangeIntFront()
  {
    return myIntFront;
  }

  //! @return back material properties.
  const OPENGL_SURF_PROP& IntBack() const
  {
    return myIntBack;
  }

  //! @return back material properties.
  OPENGL_SURF_PROP& ChangeIntBack()
  {
    return myIntBack;
  }

  //! @return polygon offset parameters.
  const TEL_POFFSET_PARAM& PolygonOffset() const
  {
    return myPolygonOffset;
  }

  //! @return polygon offset parameters.
  TEL_POFFSET_PARAM& ChangePolygonOffset()
  {
    return myPolygonOffset;
  }

  //! @return texture mapping flag.
  bool DoTextureMap() const
  {
    return myDoTextureMap;
  }

  //! @return texture mapping flag.
  bool& ChangeDoTextureMap()
  {
    return myDoTextureMap;
  }

  //! @return texture mapping parameters.
  const Handle(Graphic3d_TextureParams)& TextureParams() const
  {
    return myTexture->GetParams();
  }

  //! @return texture map.
  const Handle(OpenGl_Texture)& TextureRes (const Handle(OpenGl_Context)& theCtx) const
  {
    if (!myResources.IsTextureReady())
    {
      myResources.BuildTexture (theCtx, myTexture);
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
      myResources.BuildShader (theCtx, myShaderProgram);
      myResources.SetShaderReady();
    }

    return myResources.ShaderProgram;
  }

  Standard_EXPORT virtual void Render  (const Handle(OpenGl_Workspace)& theWorkspace) const;
  Standard_EXPORT virtual void Release (OpenGl_Context* theContext);

protected:

  Standard_EXPORT void convertMaterial (const CALL_DEF_MATERIAL& theMat,
                                        OPENGL_SURF_PROP&        theSurf);

protected: //! @name ordinary aspect properties

  Aspect_InteriorStyle            myInteriorStyle;
  int                             myEdge;
  int                             myHatch;
  int                             myDistinguishingMode;
  int                             myCullingMode;
  OPENGL_SURF_PROP                myIntFront;
  OPENGL_SURF_PROP                myIntBack;
  TEL_POFFSET_PARAM               myPolygonOffset;
  bool                            myDoTextureMap;
  Handle(Graphic3d_TextureMap)    myTexture;
  Handle(Graphic3d_ShaderProgram) myShaderProgram;

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

protected:

  OpenGl_AspectLine               myAspectEdge;

public:

  DEFINE_STANDARD_ALLOC

};

#endif //_OpenGl_AspectFace_Header
