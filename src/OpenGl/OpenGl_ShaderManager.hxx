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
#include <Graphic3d_TypeOfShadingModel.hxx>

#include <NCollection_DataMap.hxx>
#include <NCollection_Sequence.hxx>

#include <OpenGl_SetOfShaderPrograms.hxx>
#include <OpenGl_ShaderStates.hxx>
#include <OpenGl_AspectFace.hxx>
#include <OpenGl_AspectLine.hxx>
#include <OpenGl_AspectText.hxx>
#include <OpenGl_AspectMarker.hxx>
#include <OpenGl_MaterialState.hxx>
#include <OpenGl_Texture.hxx>

class OpenGl_View;

//! List of shader programs.
typedef NCollection_Sequence<Handle(OpenGl_ShaderProgram)> OpenGl_ShaderProgramList;

//! This class is responsible for managing shader programs.
class OpenGl_ShaderManager : public Standard_Transient
{
  DEFINE_STANDARD_RTTIEXT(OpenGl_ShaderManager, Standard_Transient)
  friend class OpenGl_ShaderProgram;
public:

  //! Creates new empty shader manager.
  Standard_EXPORT OpenGl_ShaderManager (OpenGl_Context* theContext);

  //! Releases resources of shader manager.
  Standard_EXPORT virtual ~OpenGl_ShaderManager();

  //! Release all resources.
  Standard_EXPORT void clear();

  //! Return local camera transformation.
  const gp_XYZ& LocalOrigin() const { return myLocalOrigin; }

  //! Setup local camera transformation for compensating float precision issues.
  void SetLocalOrigin (const gp_XYZ& theOrigin)
  {
    myLocalOrigin    = theOrigin;
    myHasLocalOrigin = !theOrigin.IsEqual (gp_XYZ(0.0, 0.0, 0.0), gp::Resolution());
  }

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
  Standard_Boolean BindFaceProgram (const Handle(OpenGl_TextureSet)& theTextures,
                                    const Standard_Boolean           theToLightOn,
                                    const Standard_Boolean              theHasVertColor,
                                    const Standard_Boolean              theEnableEnvMap,
                                    const Handle(OpenGl_ShaderProgram)& theCustomProgram)
  {
    if (!theCustomProgram.IsNull()
     || myContext->caps->ffpEnable)
    {
      return bindProgramWithState (theCustomProgram);
    }

    const Standard_Integer        aBits    = getProgramBits (theTextures, theHasVertColor, theEnableEnvMap);
    Handle(OpenGl_ShaderProgram)& aProgram = getStdProgram (theToLightOn, aBits);
    return bindProgramWithState (aProgram);
  }

  //! Bind program for line rendering
  Standard_Boolean BindLineProgram (const Handle(OpenGl_TextureSet)&    theTextures,
                                    const Standard_Boolean              theStipple,
                                    const Standard_Boolean              theToLightOn,
                                    const Standard_Boolean              theHasVertColor,
                                    const Handle(OpenGl_ShaderProgram)& theCustomProgram)
  {
    if (!theCustomProgram.IsNull()
     || myContext->caps->ffpEnable)
    {
      return bindProgramWithState (theCustomProgram);
    }

    Standard_Integer aBits = getProgramBits (theTextures, theHasVertColor);
    if (theStipple)
    {
      aBits |= OpenGl_PO_StippleLine;
    }

    Handle(OpenGl_ShaderProgram)& aProgram = getStdProgram (theToLightOn, aBits);
    return bindProgramWithState (aProgram);
  }

  //! Bind program for point rendering
  Standard_Boolean BindMarkerProgram (const Handle(OpenGl_TextureSet)&    theTextures,
                                      const Standard_Boolean              theToLightOn,
                                      const Standard_Boolean              theHasVertColor,
                                      const Handle(OpenGl_ShaderProgram)& theCustomProgram)
  {
    if (!theCustomProgram.IsNull()
     || myContext->caps->ffpEnable)
    {
      return bindProgramWithState (theCustomProgram);
    }

    const Standard_Integer        aBits    = getProgramBits (theTextures, theHasVertColor) | OpenGl_PO_Point;
    Handle(OpenGl_ShaderProgram)& aProgram = getStdProgram (theToLightOn, aBits);
    return bindProgramWithState (aProgram);
  }

  //! Bind program for rendering alpha-textured font.
  Standard_Boolean BindFontProgram (const Handle(OpenGl_ShaderProgram)& theCustomProgram)
  {
    if (!theCustomProgram.IsNull()
     || myContext->caps->ffpEnable)
    {
      return bindProgramWithState (theCustomProgram);
    }

    if (myFontProgram.IsNull())
    {
      prepareStdProgramFont();
    }

    return bindProgramWithState (myFontProgram);
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

  //! Bind program for blended order-independent transparency buffers compositing.
  Standard_Boolean BindOitCompositingProgram (const Standard_Boolean theIsMSAAEnabled)
  {
    const Standard_Integer aProgramIdx = theIsMSAAEnabled ? 1 : 0;
    if (myOitCompositingProgram[aProgramIdx].IsNull())
    {
      prepareStdProgramOitCompositing (theIsMSAAEnabled);
    }

    const Handle(OpenGl_ShaderProgram)& aProgram = myOitCompositingProgram [aProgramIdx];
    return !aProgram.IsNull() && myContext->BindProgram (aProgram);
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
  const OpenGl_LightSourceState& LightSourceState() const { return myLightSourceState; }

  //! Updates state of OCCT light sources.
  Standard_EXPORT void UpdateLightSourceStateTo (const OpenGl_ListOfLight* theLights);

  //! Invalidate state of OCCT light sources.
  Standard_EXPORT void UpdateLightSourceState();

  //! Pushes current state of OCCT light sources to specified program.
  Standard_EXPORT void PushLightSourceState (const Handle(OpenGl_ShaderProgram)& theProgram) const;

public:

  //! Returns current state of OCCT projection transform.
  const OpenGl_ProjectionState& ProjectionState() const { return myProjectionState; }

  //! Updates state of OCCT projection transform.
  Standard_EXPORT void UpdateProjectionStateTo (const OpenGl_Mat4& theProjectionMatrix);

  //! Pushes current state of OCCT projection transform to specified program.
  Standard_EXPORT void PushProjectionState (const Handle(OpenGl_ShaderProgram)& theProgram) const;

public:

  //! Returns current state of OCCT model-world transform.
  const OpenGl_ModelWorldState& ModelWorldState() const { return myModelWorldState; }

  //! Updates state of OCCT model-world transform.
  Standard_EXPORT void UpdateModelWorldStateTo (const OpenGl_Mat4& theModelWorldMatrix);

  //! Pushes current state of OCCT model-world transform to specified program.
  Standard_EXPORT void PushModelWorldState (const Handle(OpenGl_ShaderProgram)& theProgram) const;

public:

  //! Returns current state of OCCT world-view transform.
  const OpenGl_WorldViewState& WorldViewState() const { return myWorldViewState; }

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

  //! Returns current state of material.
  const OpenGl_MaterialState& MaterialState() const { return myMaterialState; }

  //! Updates state of material.
  void UpdateMaterialStateTo (const OpenGl_Material& theFrontMat,
                              const OpenGl_Material& theBackMat,
                              const bool theToDistinguish,
                              const bool theToMapTexture)
  {
    myMaterialState.Set (theFrontMat, theBackMat, theToDistinguish, theToMapTexture);
    myMaterialState.Update();
  }

  //! Updates state of material.
  void UpdateMaterialState()
  {
    myMaterialState.Update();
  }

  //! Pushes current state of material to specified program.
  void PushMaterialState (const Handle(OpenGl_ShaderProgram)& theProgram) const;

public:

  //! Set the state of OIT rendering pass.
  //! @param theToEnableOitWrite [in] flag indicating whether the special output should be written for OIT algorithm.
  //! @param theDepthFactor [in] the scalar factor of depth influence to the fragment's coverage.
  void SetOitState (const bool theToEnableOitWrite, const float theDepthFactor)
  {
    myOitState.Set (theToEnableOitWrite, theDepthFactor);
    myOitState.Update();
  }

  //! Pushes state of OIT uniforms to the specified program.
  Standard_EXPORT void PushOitState (const Handle(OpenGl_ShaderProgram)& theProgram) const;

public:

  //! Pushes current state of OCCT graphics parameters to specified program.
  Standard_EXPORT void PushState (const Handle(OpenGl_ShaderProgram)& theProgram) const;

public:

  //! Overwrites context
  void SetContext (OpenGl_Context* theCtx)
  {
    myContext = theCtx;
  }

  //! Returns true when provided context is the same as used one by shader manager.
  bool IsSameContext (OpenGl_Context* theCtx) const
  {
    return myContext == theCtx;
  }

  //! Sets shading model.
  Standard_EXPORT void SetShadingModel (const Graphic3d_TypeOfShadingModel theModel);

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
  Standard_Integer getProgramBits (const Handle(OpenGl_TextureSet)& theTextures,
                                   const Standard_Boolean theHasVertColor,
                                   const Standard_Boolean theEnableEnvMap = Standard_False)

  {
    Standard_Integer aBits = 0;

    const Standard_Integer aNbPlanes = myContext->Clipping().NbClippingOrCappingOn();
    if (aNbPlanes > 0)
    {
      aBits |= OpenGl_PO_ClipPlanesN;
      if (aNbPlanes == 1)
      {
        aBits |= OpenGl_PO_ClipPlanes1;
      }
      else if (aNbPlanes == 2)
      {
        aBits |= OpenGl_PO_ClipPlanes2;
      }
    }

    if (theEnableEnvMap)
    {
      // Environment map overwrites material texture
      aBits |= OpenGl_PO_TextureEnv;
    }
    else if (!theTextures.IsNull()
          && !theTextures->IsEmpty()
          && !theTextures->First().IsNull())
    {
      aBits |= theTextures->First()->IsAlpha() ? OpenGl_PO_TextureA : OpenGl_PO_TextureRGB;
    }
    if (theHasVertColor)
    {
      aBits |= OpenGl_PO_VertColor;
    }

    if (myOitState.ToEnableWrite())
    {
      aBits |= OpenGl_PO_WriteOit;
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

  //! Prepare standard GLSL program for accessing point sprite alpha.
  Standard_EXPORT TCollection_AsciiString pointSpriteAlphaSrc (const Standard_Integer theBits);

  //! Prepare standard GLSL program for computing point sprite shading.
  Standard_EXPORT TCollection_AsciiString pointSpriteShadingSrc (const TCollection_AsciiString theBaseColorSrc, const Standard_Integer theBits);

  //! Prepare standard GLSL program for textured font.
  Standard_EXPORT Standard_Boolean prepareStdProgramFont();

  //! Prepare standard GLSL program for FBO blit operation.
  Standard_EXPORT Standard_Boolean prepareStdProgramFboBlit();

  //! Prepare standard GLSL programs for OIT compositing operation.
  Standard_EXPORT Standard_Boolean prepareStdProgramOitCompositing (const Standard_Boolean theMsaa);

  //! Prepare standard GLSL program without lighting.
  Standard_EXPORT Standard_Boolean prepareStdProgramFlat (Handle(OpenGl_ShaderProgram)& theProgram,
                                                          const Standard_Integer        theBits);

  //! Prepare standard GLSL program with lighting.
  Standard_Boolean prepareStdProgramLight (Handle(OpenGl_ShaderProgram)& theProgram,
                                           const Standard_Integer        theBits)
  {
    return myShadingModel == Graphic3d_TOSM_FRAGMENT
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
  Standard_EXPORT Standard_Boolean bindProgramWithState (const Handle(OpenGl_ShaderProgram)& theProgram);

  //! Set pointer myLightPrograms to active lighting programs set from myMapOfLightPrograms
  Standard_EXPORT void switchLightPrograms();

  //! Prepare standard GLSL program for stereoscopic image.
  Standard_EXPORT Standard_Boolean prepareStdProgramStereo (Handle(OpenGl_ShaderProgram)& theProgram,
                                                            const Graphic3d_StereoMode    theStereoMode);

protected:

  //! Packed properties of light source
  struct OpenGl_ShaderLightParameters
  {
    OpenGl_Vec4 Color;
    OpenGl_Vec4 Position;
    OpenGl_Vec4 Direction;
    OpenGl_Vec4 Parameters;

    //! Returns packed (serialized) representation of light source properties
    const OpenGl_Vec4* Packed() const { return reinterpret_cast<const OpenGl_Vec4*> (this); }
    static Standard_Integer NbOfVec4() { return 4; }
  };

  //! Packed light source type information
  struct OpenGl_ShaderLightType
  {
    Standard_Integer Type;
    Standard_Integer IsHeadlight;

    //! Returns packed (serialized) representation of light source type
    const OpenGl_Vec2i* Packed() const { return reinterpret_cast<const OpenGl_Vec2i*> (this); }
    static Standard_Integer NbOfVec2i() { return 1; }
  };

  //! Fake OpenGL program for tracking FFP state in the way consistent to programmable pipeline.
  class OpenGl_ShaderProgramFFP : public OpenGl_ShaderProgram
  {
    DEFINE_STANDARD_RTTI_INLINE(OpenGl_ShaderProgramFFP, OpenGl_ShaderProgram)
    friend class OpenGl_ShaderManager;
  protected:
    OpenGl_ShaderProgramFFP() {}
  };

protected:

  Handle(OpenGl_ShaderProgramFFP)    myFfpProgram;

  Graphic3d_TypeOfShadingModel       myShadingModel;       //!< lighting shading model
  OpenGl_ShaderProgramList           myProgramList;        //!< The list of shader programs
  Handle(OpenGl_SetOfShaderPrograms) myLightPrograms;      //!< pointer to active lighting programs matrix
  OpenGl_SetOfShaderPrograms         myFlatPrograms;       //!< programs matrix without  lighting
  Handle(OpenGl_ShaderProgram)       myFontProgram;        //!< standard program for textured text
  Handle(OpenGl_ShaderProgram)       myBlitProgram;        //!< standard program for FBO blit emulation
  Handle(OpenGl_ShaderProgram)       myOitCompositingProgram[2]; //!< standard program for OIT compositing (default and MSAA).
  OpenGl_MapOfShaderPrograms         myMapOfLightPrograms; //!< map of lighting programs depending on shading model and lights configuration

  Handle(OpenGl_ShaderProgram)       myStereoPrograms[Graphic3d_StereoMode_NB]; //!< standard stereo programs

  OpenGl_Context*                    myContext;            //!< OpenGL context

protected:

  OpenGl_ProjectionState             myProjectionState;    //!< State of OCCT projection  transformation
  OpenGl_ModelWorldState             myModelWorldState;    //!< State of OCCT model-world transformation
  OpenGl_WorldViewState              myWorldViewState;     //!< State of OCCT world-view  transformation
  OpenGl_ClippingState               myClippingState;      //!< State of OCCT clipping planes
  OpenGl_LightSourceState            myLightSourceState;   //!< State of OCCT light sources
  OpenGl_MaterialState               myMaterialState;      //!< State of Front and Back materials
  OpenGl_OitState                    myOitState;           //!< State of OIT uniforms

  gp_XYZ                             myLocalOrigin;        //!< local camera transformation
  Standard_Boolean                   myHasLocalOrigin;     //!< flag indicating that local camera transformation has been set

  mutable OpenGl_ShaderLightType       myLightTypeArray  [OpenGLMaxLights];
  mutable OpenGl_ShaderLightParameters myLightParamsArray[OpenGLMaxLights];

private:

  const OpenGl_View*                 myLastView;           //!< Pointer to the last view shader manager used with

};

DEFINE_STANDARD_HANDLE(OpenGl_ShaderManager, Standard_Transient)

#endif // _OpenGl_ShaderManager_HeaderFile
