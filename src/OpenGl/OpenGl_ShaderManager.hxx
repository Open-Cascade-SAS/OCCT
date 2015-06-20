// Created on: 2013-09-26
// Created by: Denis BOGOLEPOV
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

#ifndef _OpenGl_ShaderManager_HeaderFile
#define _OpenGl_ShaderManager_HeaderFile

#include <Graphic3d_ShaderProgram.hxx>
#include <Graphic3d_StereoMode.hxx>

#include <NCollection_DataMap.hxx>
#include <NCollection_Sequence.hxx>

#include <Handle_OpenGl_ShaderManager.hxx>
#include <OpenGl_SetOfShaderPrograms.hxx>
#include <OpenGl_ShaderStates.hxx>
#include <OpenGl_AspectFace.hxx>
#include <OpenGl_AspectLine.hxx>
#include <OpenGl_AspectText.hxx>
#include <OpenGl_AspectMarker.hxx>
#include <OpenGl_Texture.hxx>
#include <Visual3d_TypeOfModel.hxx>

class OpenGl_View;

//! List of shader programs.
typedef NCollection_Sequence<Handle(OpenGl_ShaderProgram)> OpenGl_ShaderProgramList;

//! Map to declare per-program states of OCCT materials.
typedef NCollection_DataMap<Handle(OpenGl_ShaderProgram), OpenGl_MaterialState> OpenGl_MaterialStates;

//! This class is responsible for managing shader programs.
class OpenGl_ShaderManager : public Standard_Transient
{
  friend class OpenGl_ShaderProgram;

public:

  //! Creates new empty shader manager.
  Standard_EXPORT OpenGl_ShaderManager (OpenGl_Context* theContext);

  //! Releases resources of shader manager.
  Standard_EXPORT virtual ~OpenGl_ShaderManager();

  //! Release all resources.
  Standard_EXPORT void clear();

  //! Creates new shader program or re-use shared instance.
  //! @param theProxy    [IN]  program definition
  //! @param theShareKey [OUT] sharing key
  //! @param theProgram  [OUT] OpenGL program
  //! @return true on success
  Standard_EXPORT Standard_Boolean Create (const Handle(Graphic3d_ShaderProgram)& theProxy,
                                           TCollection_AsciiString&               theShareKey,
                                           Handle(OpenGl_ShaderProgram)&          theProgram);

  //! Unregisters specified shader program.
  Standard_EXPORT void Unregister (TCollection_AsciiString&      theShareKey,
                                   Handle(OpenGl_ShaderProgram)& theProgram);

  //! Returns list of registered shader programs.
  Standard_EXPORT const OpenGl_ShaderProgramList& ShaderPrograms() const;

  //! Returns true if no program objects are registered in the manager.
  Standard_EXPORT Standard_Boolean IsEmpty() const;

  //! Bind program for filled primitives rendering
  Standard_Boolean BindProgram (const OpenGl_AspectFace*            theAspect,
                                const Handle(OpenGl_Texture)&       theTexture,
                                const Standard_Boolean              theToLightOn,
                                const Standard_Boolean              theHasVertColor,
                                const Handle(OpenGl_ShaderProgram)& theCustomProgram)
  {
    if (!theCustomProgram.IsNull()
     || myContext->caps->ffpEnable)
    {
      return bindProgramWithState (theCustomProgram, theAspect);
    }

    const Standard_Integer        aBits    = getProgramBits (theTexture, theHasVertColor, Standard_True);
    Handle(OpenGl_ShaderProgram)& aProgram = getStdProgram (theToLightOn, aBits);
    return bindProgramWithState (aProgram, theAspect);
  }

  //! Bind program for line rendering
  Standard_Boolean BindProgram (const OpenGl_AspectLine*            theAspect,
                                const Handle(OpenGl_Texture)&       theTexture,
                                const Standard_Boolean              theToLightOn,
                                const Standard_Boolean              theHasVertColor,
                                const Handle(OpenGl_ShaderProgram)& theCustomProgram)
  {
    if (!theCustomProgram.IsNull()
     || myContext->caps->ffpEnable)
    {
      return bindProgramWithState (theCustomProgram, theAspect);
    }

    const Standard_Integer        aBits    = getProgramBits (theTexture, theHasVertColor);
    Handle(OpenGl_ShaderProgram)& aProgram = getStdProgram (theToLightOn, aBits);
    return bindProgramWithState (aProgram, theAspect);
  }

  //! Bind program for point rendering
  Standard_Boolean BindProgram (const OpenGl_AspectMarker*          theAspect,
                                const Handle(OpenGl_Texture)&       theTexture,
                                const Standard_Boolean              theToLightOn,
                                const Standard_Boolean              theHasVertColor,
                                const Handle(OpenGl_ShaderProgram)& theCustomProgram)
  {
    if (!theCustomProgram.IsNull()
     || myContext->caps->ffpEnable)
    {
      return bindProgramWithState (theCustomProgram, theAspect);
    }

    const Standard_Integer        aBits    = getProgramBits (theTexture, theHasVertColor) | OpenGl_PO_Point;
    Handle(OpenGl_ShaderProgram)& aProgram = getStdProgram (theToLightOn, aBits);
    return bindProgramWithState (aProgram, theAspect);
  }

  //! Bind program for rendering alpha-textured font.
  Standard_Boolean BindProgram (const OpenGl_AspectText*            theAspect,
                                const Handle(OpenGl_ShaderProgram)& theCustomProgram)
  {
    if (!theCustomProgram.IsNull()
     || myContext->caps->ffpEnable)
    {
      return bindProgramWithState (theCustomProgram, theAspect);
    }

    if (myFontProgram.IsNull())
    {
      prepareStdProgramFont();
    }
    return bindProgramWithState (myFontProgram, theAspect);
  }

  //! Bind program for FBO blit operation.
  Standard_Boolean BindFboBlitProgram()
  {
    if (myBlitProgram.IsNull())
    {
      prepareStdProgramFboBlit();
    }
    return !myBlitProgram.IsNull()
         && myContext->BindProgram (myBlitProgram);
  }

  //! Bind program for rendering stereoscopic image.
  Standard_Boolean BindStereoProgram (const Graphic3d_StereoMode theStereoMode)
  {
    if (theStereoMode < 0 || theStereoMode >= Graphic3d_StereoMode_NB)
    {
      return Standard_False;
    }

    if (myStereoPrograms[theStereoMode].IsNull())
    {
      prepareStdProgramStereo (myStereoPrograms[theStereoMode], theStereoMode);
    }
    const Handle(OpenGl_ShaderProgram)& aProgram = myStereoPrograms[theStereoMode];
    return !aProgram.IsNull()
         && myContext->BindProgram (aProgram);
  }

public:

  //! Returns current state of OCCT light sources.
  Standard_EXPORT const OpenGl_LightSourceState& LightSourceState() const;

  //! Updates state of OCCT light sources.
  Standard_EXPORT void UpdateLightSourceStateTo (const OpenGl_ListOfLight* theLights);

  //! Pushes current state of OCCT light sources to specified program.
  Standard_EXPORT void PushLightSourceState (const Handle(OpenGl_ShaderProgram)& theProgram) const;

public:

  //! Returns current state of OCCT projection transform.
  Standard_EXPORT const OpenGl_ProjectionState& ProjectionState() const;

  //! Updates state of OCCT projection transform.
  Standard_EXPORT void UpdateProjectionStateTo (const OpenGl_Mat4& theProjectionMatrix);

  //! Pushes current state of OCCT projection transform to specified program.
  Standard_EXPORT void PushProjectionState (const Handle(OpenGl_ShaderProgram)& theProgram) const;

public:

  //! Returns current state of OCCT model-world transform.
  Standard_EXPORT const OpenGl_ModelWorldState& ModelWorldState() const;

  //! Updates state of OCCT model-world transform.
  Standard_EXPORT void UpdateModelWorldStateTo (const OpenGl_Mat4& theModelWorldMatrix);

  //! Pushes current state of OCCT model-world transform to specified program.
  Standard_EXPORT void PushModelWorldState (const Handle(OpenGl_ShaderProgram)& theProgram) const;

public:

  //! Returns current state of OCCT world-view transform.
  Standard_EXPORT const OpenGl_WorldViewState& WorldViewState() const;

  //! Updates state of OCCT world-view transform.
  Standard_EXPORT void UpdateWorldViewStateTo (const OpenGl_Mat4& theWorldViewMatrix);

  //! Pushes current state of OCCT world-view transform to specified program.
  Standard_EXPORT void PushWorldViewState (const Handle(OpenGl_ShaderProgram)& theProgram) const;

public:

  //! Updates state of OCCT clipping planes.
  Standard_EXPORT void UpdateClippingState();

  //! Reverts state of OCCT clipping planes.
  Standard_EXPORT void RevertClippingState();

  //! Pushes current state of OCCT clipping planes to specified program.
  Standard_EXPORT void PushClippingState (const Handle(OpenGl_ShaderProgram)& theProgram) const;

public:

  //! Resets state of OCCT material for all programs.
  Standard_EXPORT void ResetMaterialStates();

  //! Updates state of OCCT material for specified program.
  Standard_EXPORT void UpdateMaterialStateTo (const Handle(OpenGl_ShaderProgram)& theProgram,
                                              const OpenGl_Element*               theAspect);

  //! Pushes current state of OCCT material to specified program.
  Standard_EXPORT void PushMaterialState (const Handle(OpenGl_ShaderProgram)& theProgram) const;

  //! Returns current state of OCCT material for specified program.
  Standard_EXPORT const OpenGl_MaterialState* MaterialState (const Handle(OpenGl_ShaderProgram)& theProgram) const;

public:

  //! Returns current state of OCCT surface detail.
  Standard_EXPORT const OpenGl_SurfaceDetailState& SurfaceDetailState() const;

  //! Updates state of OCCT surface detail.
  Standard_EXPORT void UpdateSurfaceDetailStateTo (const Visual3d_TypeOfSurfaceDetail theDetail);

public:

  //! Pushes current state of OCCT graphics parameters to specified program.
  Standard_EXPORT void PushState (const Handle(OpenGl_ShaderProgram)& theProgram) const;

public:

  //! Overwrites context
  void SetContext (OpenGl_Context* theCtx)
  {
    myContext = theCtx;
  }

  //! Sets shading model.
  Standard_EXPORT void SetShadingModel(const Visual3d_TypeOfModel theModel);

  //! Sets last view manger used with.
  //! Helps to handle matrix states in multi-view configurations.
  void SetLastView (const OpenGl_View* theLastView)
  {
    myLastView = theLastView;
  }

  //! Returns true when provided view is the same as cached one.
  bool IsSameView (const OpenGl_View* theView) const
  {
    return myLastView == theView;
  }

protected:

  //! Define program bits.
  Standard_Integer getProgramBits (const Handle(OpenGl_Texture)& theTexture,
                                   const Standard_Boolean        theHasVertColor,
                                   const Standard_Boolean        theEnableEnvMap = Standard_False)

  {
    Standard_Integer aBits = 0;
    if (myContext->Clipping().IsClippingOrCappingOn())
    {
      aBits |= OpenGl_PO_ClipPlanes;
    }
    if (theEnableEnvMap && mySurfaceDetailState.Detail() == Visual3d_TOD_ENVIRONMENT)
    {
      // Environment map overwrites material texture
      aBits |= OpenGl_PO_TextureEnv;
    }
    else if (!theTexture.IsNull())
    {
      aBits |= theTexture->IsAlpha() ? OpenGl_PO_TextureA : OpenGl_PO_TextureRGB;
    }
    if (theHasVertColor)
    {
      aBits |= OpenGl_PO_VertColor;
    }
    return aBits;
  }

  //! Prepare standard GLSL program.
  Handle(OpenGl_ShaderProgram)& getStdProgram (const Standard_Boolean theToLightOn,
                                               const Standard_Integer theBits)
  {
    // If environment map is enabled lighting calculations are
    // not needed (in accordance with default OCCT behaviour)
    if (theToLightOn && (theBits & OpenGl_PO_TextureEnv) == 0)
    {
      Handle(OpenGl_ShaderProgram)& aProgram = myLightPrograms->ChangeValue (theBits);
      if (aProgram.IsNull())
      {
        prepareStdProgramLight (aProgram, theBits);
      }
      return aProgram;
    }

    Handle(OpenGl_ShaderProgram)& aProgram = myFlatPrograms.ChangeValue (theBits);
    if (aProgram.IsNull())
    {
      prepareStdProgramFlat (aProgram, theBits);
    }
    return aProgram;
  }

  //! Prepare standard GLSL program for textured font.
  Standard_EXPORT Standard_Boolean prepareStdProgramFont();

  //! Prepare standard GLSL program for FBO blit operation.
  Standard_EXPORT Standard_Boolean prepareStdProgramFboBlit();

  //! Prepare standard GLSL program without lighting.
  Standard_EXPORT Standard_Boolean prepareStdProgramFlat (Handle(OpenGl_ShaderProgram)& theProgram,
                                                          const Standard_Integer        theBits);

  //! Prepare standard GLSL program with lighting.
  Standard_Boolean prepareStdProgramLight (Handle(OpenGl_ShaderProgram)& theProgram,
                                           const Standard_Integer        theBits)
  {
    return myShadingModel == Visual3d_TOM_FRAGMENT
         ? prepareStdProgramPhong   (theProgram, theBits)
         : prepareStdProgramGouraud (theProgram, theBits);
  }

  //! Prepare standard GLSL program with per-vertex lighting.
  Standard_EXPORT Standard_Boolean prepareStdProgramGouraud (Handle(OpenGl_ShaderProgram)& theProgram,
                                                             const Standard_Integer        theBits);

  //! Prepare standard GLSL program with per-pixel lighting.
  Standard_EXPORT Standard_Boolean prepareStdProgramPhong (Handle(OpenGl_ShaderProgram)& theProgram,
                                                           const Standard_Integer        theBits);

  //! Define computeLighting GLSL function depending on current lights configuration
  //! @param theHasVertColor flag to use getVertColor() instead of Ambient and Diffuse components of active material
  Standard_EXPORT TCollection_AsciiString stdComputeLighting (const Standard_Boolean theHasVertColor);

  //! Bind specified program to current context and apply state.
  Standard_EXPORT Standard_Boolean bindProgramWithState (const Handle(OpenGl_ShaderProgram)& theProgram,
                                                         const OpenGl_Element*               theAspect);

  //! Set pointer myLightPrograms to active lighting programs set from myMapOfLightPrograms
  Standard_EXPORT void switchLightPrograms();

  //! Prepare standard GLSL program for stereoscopic image.
  Standard_EXPORT Standard_Boolean prepareStdProgramStereo (Handle(OpenGl_ShaderProgram)& theProgram,
                                                            const Graphic3d_StereoMode    theStereoMode);

protected:

  Visual3d_TypeOfModel               myShadingModel;       //!< lighting shading model
  OpenGl_ShaderProgramList           myProgramList;        //!< The list of shader programs
  Handle(OpenGl_SetOfShaderPrograms) myLightPrograms;      //!< pointer to active lighting programs matrix
  OpenGl_SetOfShaderPrograms         myFlatPrograms;       //!< programs matrix without  lighting
  Handle(OpenGl_ShaderProgram)       myFontProgram;        //!< standard program for textured text
  Handle(OpenGl_ShaderProgram)       myBlitProgram;        //!< standard program for FBO blit emulation
  OpenGl_MapOfShaderPrograms         myMapOfLightPrograms; //!< map of lighting programs depending on shading model and lights configuration

  Handle(OpenGl_ShaderProgram)       myStereoPrograms[Graphic3d_StereoMode_NB]; //!< standard stereo programs

  OpenGl_Context*                    myContext;            //!< OpenGL context

protected:

  OpenGl_MaterialStates              myMaterialStates;     //!< Per-program state of OCCT material
  OpenGl_ProjectionState             myProjectionState;    //!< State of OCCT projection  transformation
  OpenGl_ModelWorldState             myModelWorldState;    //!< State of OCCT model-world transformation
  OpenGl_WorldViewState              myWorldViewState;     //!< State of OCCT world-view  transformation
  OpenGl_ClippingState               myClippingState;      //!< State of OCCT clipping planes
  OpenGl_LightSourceState            myLightSourceState;   //!< State of OCCT light sources
  OpenGl_SurfaceDetailState          mySurfaceDetailState; //!< State of OCCT surface detail

private:

  const OpenGl_View*                 myLastView;           //!< Pointer to the last view shader manager used with

public:

  DEFINE_STANDARD_RTTI (OpenGl_ShaderManager)

};

#endif // _OpenGl_ShaderManager_HeaderFile
