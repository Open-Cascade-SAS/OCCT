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

#include <Graphic3d_ShaderManager.hxx>
#include <OpenGl_Aspects.hxx>
#include <OpenGl_Context.hxx>
#include <OpenGl_MaterialState.hxx>
#include <OpenGl_PBREnvironment.hxx>
#include <OpenGl_SetOfShaderPrograms.hxx>
#include <OpenGl_ShaderProgram.hxx>
#include <OpenGl_ShaderStates.hxx>
#include <OpenGl_Texture.hxx>
#include <OpenGl_TextureSet.hxx>

class OpenGl_VertexBuffer;

//! List of shader programs.

//! This class is responsible for managing shader programs.
class OpenGl_ShaderManager : public Graphic3d_ShaderManager
{
  DEFINE_STANDARD_RTTIEXT(OpenGl_ShaderManager, Graphic3d_ShaderManager)
  friend class OpenGl_ShaderProgram;

public:
  //! Creates new empty shader manager.
  Standard_EXPORT OpenGl_ShaderManager(OpenGl_Context* theContext);

  //! Releases resources of shader manager.
  Standard_EXPORT ~OpenGl_ShaderManager() override;

  //! Release all resources.
  Standard_EXPORT void clear();

  //! Fetch sRGB state from caps and invalidates programs, if necessary.
  Standard_EXPORT void UpdateSRgbState();

  //! Return local camera transformation.
  const gp_XYZ& LocalOrigin() const { return myLocalOrigin; }

  //! Setup local camera transformation for compensating float precision issues.
  void SetLocalOrigin(const gp_XYZ& theOrigin)
  {
    myLocalOrigin    = theOrigin;
    myHasLocalOrigin = !theOrigin.IsEqual(gp_XYZ(0.0, 0.0, 0.0), gp::Resolution());
  }

  //! Return clipping plane W equation value moved considering local camera transformation.
  double LocalClippingPlaneW(const Graphic3d_ClipPlane& thePlane) const
  {
    const NCollection_Vec4<double>& anEq = thePlane.GetEquation();
    if (myHasLocalOrigin)
    {
      const gp_XYZ aPos = thePlane.ToPlane().Position().Location().XYZ() - myLocalOrigin;
      return -(anEq.x() * aPos.X() + anEq.y() * aPos.Y() + anEq.z() * aPos.Z());
    }
    return anEq.w();
  }

  //! Creates new shader program or re-use shared instance.
  //! @param[in] theProxy      program definition
  //! @param[out] theShareKey  sharing key
  //! @param[out] theProgram   OpenGL program
  //! @return true on success
  Standard_EXPORT bool Create(const occ::handle<Graphic3d_ShaderProgram>& theProxy,
                              TCollection_AsciiString&                    theShareKey,
                              occ::handle<OpenGl_ShaderProgram>&          theProgram);

  //! Unregisters specified shader program.
  Standard_EXPORT void Unregister(TCollection_AsciiString&           theShareKey,
                                  occ::handle<OpenGl_ShaderProgram>& theProgram);

  //! Returns list of registered shader programs.
  const NCollection_Sequence<occ::handle<OpenGl_ShaderProgram>>& ShaderPrograms() const
  {
    return myProgramList;
  }

  //! Returns true if no program objects are registered in the manager.
  bool IsEmpty() const { return myProgramList.IsEmpty(); }

  //! Bind program for filled primitives rendering
  bool BindFaceProgram(const occ::handle<OpenGl_TextureSet>&    theTextures,
                       Graphic3d_TypeOfShadingModel             theShadingModel,
                       Graphic3d_AlphaMode                      theAlphaMode,
                       bool                                     theHasVertColor,
                       bool                                     theEnableEnvMap,
                       const occ::handle<OpenGl_ShaderProgram>& theCustomProgram)
  {
    return BindFaceProgram(theTextures,
                           theShadingModel,
                           theAlphaMode,
                           Aspect_IS_SOLID,
                           theHasVertColor,
                           theEnableEnvMap,
                           false,
                           theCustomProgram);
  }

  //! Bind program for filled primitives rendering
  bool BindFaceProgram(const occ::handle<OpenGl_TextureSet>&    theTextures,
                       Graphic3d_TypeOfShadingModel             theShadingModel,
                       Graphic3d_AlphaMode                      theAlphaMode,
                       Aspect_InteriorStyle                     theInteriorStyle,
                       bool                                     theHasVertColor,
                       bool                                     theEnableEnvMap,
                       bool                                     theEnableMeshEdges,
                       const occ::handle<OpenGl_ShaderProgram>& theCustomProgram)
  {
    const Graphic3d_TypeOfShadingModel aShadeModelOnFace =
      theShadingModel != Graphic3d_TypeOfShadingModel_Unlit
          && (theTextures.IsNull() || theTextures->IsModulate())
        ? theShadingModel
        : Graphic3d_TypeOfShadingModel_Unlit;
    if (!theCustomProgram.IsNull() || myContext->caps->ffpEnable)
    {
      return bindProgramWithState(theCustomProgram, aShadeModelOnFace);
    }

    const int                          aBits    = getProgramBits(theTextures,
                                     theAlphaMode,
                                     theInteriorStyle,
                                     theHasVertColor,
                                     theEnableEnvMap,
                                     theEnableMeshEdges);
    occ::handle<OpenGl_ShaderProgram>& aProgram = getStdProgram(aShadeModelOnFace, aBits);
    return bindProgramWithState(aProgram, aShadeModelOnFace);
  }

  //! Bind program for line rendering
  bool BindLineProgram(const occ::handle<OpenGl_TextureSet>&    theTextures,
                       const Aspect_TypeOfLine                  theLineType,
                       const Graphic3d_TypeOfShadingModel       theShadingModel,
                       const Graphic3d_AlphaMode                theAlphaMode,
                       const bool                               theHasVertColor,
                       const occ::handle<OpenGl_ShaderProgram>& theCustomProgram)
  {
    if (!theCustomProgram.IsNull() || myContext->caps->ffpEnable)
    {
      return bindProgramWithState(theCustomProgram, theShadingModel);
    }

    int aBits =
      getProgramBits(theTextures, theAlphaMode, Aspect_IS_SOLID, theHasVertColor, false, false);
    if (theLineType != Aspect_TOL_SOLID)
    {
      aBits |= Graphic3d_ShaderFlags_StippleLine;
    }

    occ::handle<OpenGl_ShaderProgram>& aProgram = getStdProgram(theShadingModel, aBits);
    return bindProgramWithState(aProgram, theShadingModel);
  }

  //! Bind program for point rendering
  Standard_EXPORT bool BindMarkerProgram(const occ::handle<OpenGl_TextureSet>&    theTextures,
                                         Graphic3d_TypeOfShadingModel             theShadingModel,
                                         Graphic3d_AlphaMode                      theAlphaMode,
                                         bool                                     theHasVertColor,
                                         const occ::handle<OpenGl_ShaderProgram>& theCustomProgram);

  //! Bind program for rendering alpha-textured font.
  bool BindFontProgram(const occ::handle<OpenGl_ShaderProgram>& theCustomProgram);

  //! Bind program for outline rendering
  bool BindOutlineProgram()
  {
    if (myContext->caps->ffpEnable)
    {
      return false;
    }

    const int aBits = getProgramBits(occ::handle<OpenGl_TextureSet>(),
                                     Graphic3d_AlphaMode_Opaque,
                                     Aspect_IS_SOLID,
                                     false,
                                     false,
                                     false);
    if (myOutlinePrograms.IsNull())
    {
      myOutlinePrograms = new OpenGl_SetOfPrograms();
    }
    occ::handle<OpenGl_ShaderProgram>& aProgram = myOutlinePrograms->ChangeValue(aBits);
    if (aProgram.IsNull())
    {
      prepareStdProgramUnlit(aProgram, aBits, true);
    }
    return bindProgramWithState(aProgram, Graphic3d_TypeOfShadingModel_Unlit);
  }

  //! Bind program for FBO blit operation.
  //! @param[in] theNbSamples        number of samples within source MSAA texture
  //! @param theIsFallback_sRGB[in]  flag indicating that destination buffer is not sRGB-ready
  Standard_EXPORT bool BindFboBlitProgram(int theNbSamples, bool theIsFallback_sRGB);

  //! Bind program for blended order-independent transparency buffers compositing.
  Standard_EXPORT bool BindOitCompositingProgram(bool theIsMSAAEnabled);

  //! Bind program for Depth Peeling order-independent transparency back color blending.
  Standard_EXPORT bool BindOitDepthPeelingBlendProgram(bool theIsMSAAEnabled);

  //! Bind program for Depth Peeling order-independent transparency flush.
  Standard_EXPORT bool BindOitDepthPeelingFlushProgram(bool theIsMSAAEnabled);

  //! Bind program for rendering stereoscopic image.
  Standard_EXPORT bool BindStereoProgram(Graphic3d_StereoMode theStereoMode);

  //! Bind program for rendering bounding box.
  bool BindBoundBoxProgram()
  {
    if (myBoundBoxProgram.IsNull())
    {
      prepareStdProgramBoundBox();
    }
    return bindProgramWithState(myBoundBoxProgram, Graphic3d_TypeOfShadingModel_Unlit);
  }

  //! Returns bounding box vertex buffer.
  const occ::handle<OpenGl_VertexBuffer>& BoundBoxVertBuffer() const
  {
    return myBoundBoxVertBuffer;
  }

  //! Bind program for IBL maps generation in PBR pipeline.
  bool BindPBREnvBakingProgram(int theIndex)
  {
    if (myPBREnvBakingProgram[theIndex].IsNull())
    {
      preparePBREnvBakingProgram(theIndex);
    }
    return myContext->BindProgram(myPBREnvBakingProgram[theIndex]);
  }

  //! Generates shader program to render environment cubemap as background.
  Standard_EXPORT const occ::handle<Graphic3d_ShaderProgram>& GetBgCubeMapProgram();

  //! Generates shader program to render skydome background.
  Standard_EXPORT const occ::handle<Graphic3d_ShaderProgram>& GetBgSkydomeProgram();

  //! Generates shader program to render correctly colored quad.
  Standard_EXPORT const occ::handle<Graphic3d_ShaderProgram>& GetColoredQuadProgram();

  //! Resets PBR shading models to corresponding non-PBR ones if PBR is not allowed.
  static Graphic3d_TypeOfShadingModel PBRShadingModelFallback(
    Graphic3d_TypeOfShadingModel theShadingModel,
    bool                         theIsPbrAllowed = false)
  {
    if (theIsPbrAllowed)
    {
      return theShadingModel;
    }

    switch (theShadingModel)
    {
      case Graphic3d_TypeOfShadingModel_Pbr:
        return Graphic3d_TypeOfShadingModel_Phong;
      case Graphic3d_TypeOfShadingModel_PbrFacet:
        return Graphic3d_TypeOfShadingModel_PhongFacet;
      default:
        return theShadingModel;
    }
  }

public:
  //! Returns current state of OCCT light sources.
  const OpenGl_LightSourceState& LightSourceState() const { return myLightSourceState; }

  //! Updates state of OCCT light sources.
  Standard_EXPORT void UpdateLightSourceStateTo(
    const occ::handle<Graphic3d_LightSet>&    theLights,
    int                                       theSpecIBLMapLevels,
    const occ::handle<OpenGl_ShadowMapArray>& theShadowMaps);

  //! Updates state of OCCT light sources to dynamically enable/disable shadowmap.
  //! @param[in] theToCast  flag to enable/disable shadowmap
  //! @return previous flag state
  bool SetCastShadows(const bool theToCast)
  {
    if (myLightSourceState.ShadowMaps().IsNull() || myLightSourceState.ToCastShadows() == theToCast)
    {
      return myLightSourceState.ToCastShadows();
    }

    myLightSourceState.SetCastShadows(theToCast);
    switchLightPrograms();
    return !theToCast;
  }

  //! Invalidate state of OCCT light sources.
  Standard_EXPORT void UpdateLightSourceState();

  //! Pushes current state of OCCT light sources to specified program (only on state change).
  //! Note that light sources definition depends also on WorldViewState.
  void PushLightSourceState(const occ::handle<OpenGl_ShaderProgram>& theProgram) const
  {
    if (myLightSourceState.Index() != theProgram->ActiveState(OpenGl_LIGHT_SOURCES_STATE)
        || myWorldViewState.Index() != theProgram->ActiveState(OpenGl_WORLD_VIEW_STATE))
    {
      pushLightSourceState(theProgram);
    }
  }

  //! Pushes current state of OCCT light sources to specified program.
  Standard_EXPORT void pushLightSourceState(
    const occ::handle<OpenGl_ShaderProgram>& theProgram) const;

public:
  //! Returns current state of OCCT projection transform.
  const OpenGl_ProjectionState& ProjectionState() const { return myProjectionState; }

  //! Updates state of OCCT projection transform.
  Standard_EXPORT void UpdateProjectionStateTo(const NCollection_Mat4<float>& theProjectionMatrix);

  //! Pushes current state of OCCT projection transform to specified program (only on state change).
  void PushProjectionState(const occ::handle<OpenGl_ShaderProgram>& theProgram) const
  {
    if (myProjectionState.Index() != theProgram->ActiveState(OpenGl_PROJECTION_STATE))
    {
      pushProjectionState(theProgram);
    }
  }

  //! Pushes current state of OCCT projection transform to specified program.
  Standard_EXPORT void pushProjectionState(
    const occ::handle<OpenGl_ShaderProgram>& theProgram) const;

public:
  //! Returns current state of OCCT model-world transform.
  const OpenGl_ModelWorldState& ModelWorldState() const { return myModelWorldState; }

  //! Updates state of OCCT model-world transform.
  Standard_EXPORT void UpdateModelWorldStateTo(const NCollection_Mat4<float>& theModelWorldMatrix);

  //! Pushes current state of OCCT model-world transform to specified program (only on state
  //! change).
  void PushModelWorldState(const occ::handle<OpenGl_ShaderProgram>& theProgram) const
  {
    if (myModelWorldState.Index() != theProgram->ActiveState(OpenGl_MODEL_WORLD_STATE))
    {
      pushModelWorldState(theProgram);
    }
  }

  //! Pushes current state of OCCT model-world transform to specified program.
  Standard_EXPORT void pushModelWorldState(
    const occ::handle<OpenGl_ShaderProgram>& theProgram) const;

public:
  //! Returns current state of OCCT world-view transform.
  const OpenGl_WorldViewState& WorldViewState() const { return myWorldViewState; }

  //! Updates state of OCCT world-view transform.
  Standard_EXPORT void UpdateWorldViewStateTo(const NCollection_Mat4<float>& theWorldViewMatrix);

  //! Pushes current state of OCCT world-view transform to specified program (only on state change).
  void PushWorldViewState(const occ::handle<OpenGl_ShaderProgram>& theProgram) const
  {
    if (myWorldViewState.Index() != theProgram->ActiveState(OpenGl_WORLD_VIEW_STATE))
    {
      pushWorldViewState(theProgram);
    }
  }

  //! Pushes current state of OCCT world-view transform to specified program.
  Standard_EXPORT void pushWorldViewState(
    const occ::handle<OpenGl_ShaderProgram>& theProgram) const;

public:
  //! Updates state of OCCT clipping planes.
  Standard_EXPORT void UpdateClippingState();

  //! Reverts state of OCCT clipping planes.
  Standard_EXPORT void RevertClippingState();

  //! Pushes current state of OCCT clipping planes to specified program (only on state change).
  void PushClippingState(const occ::handle<OpenGl_ShaderProgram>& theProgram) const
  {
    if (myClippingState.Index() != theProgram->ActiveState(OpenGl_CLIP_PLANES_STATE))
    {
      pushClippingState(theProgram);
    }
  }

  //! Pushes current state of OCCT clipping planes to specified program.
  Standard_EXPORT void pushClippingState(const occ::handle<OpenGl_ShaderProgram>& theProgram) const;

public:
  //! Returns current state of material.
  const OpenGl_MaterialState& MaterialState() const { return myMaterialState; }

  //! Updates state of material.
  void UpdateMaterialStateTo(const OpenGl_Material& theMat,
                             const float            theAlphaCutoff,
                             const bool             theToDistinguish,
                             const bool             theToMapTexture)
  {
    myMaterialState.Set(theMat, theAlphaCutoff, theToDistinguish, theToMapTexture);
    myMaterialState.Update();
  }

  //! Updates state of material.
  void UpdateMaterialState() { myMaterialState.Update(); }

  //! Pushes current state of material to specified program (only on state change).
  void PushMaterialState(const occ::handle<OpenGl_ShaderProgram>& theProgram) const
  {
    if (myMaterialState.Index() != theProgram->ActiveState(OpenGl_MATERIAL_STATE))
    {
      pushMaterialState(theProgram);
    }
  }

  //! Pushes current state of material to specified program.
  Standard_EXPORT void pushMaterialState(const occ::handle<OpenGl_ShaderProgram>& theProgram) const;

public:
  //! Setup interior style line edges variables.
  Standard_EXPORT void PushInteriorState(const occ::handle<OpenGl_ShaderProgram>& theProgram,
                                         const occ::handle<Graphic3d_Aspects>&    theAspect) const;

public:
  //! Returns state of OIT uniforms.
  const OpenGl_OitState& OitState() const { return myOitState; }

  //! Reset the state of OIT rendering pass (only on state change).
  void ResetOitState()
  {
    myOitState.Set(Graphic3d_RTM_BLEND_UNORDERED, 0.0f);
    myOitState.Update();
  }

  //! Set the state of OIT rendering pass (only on state change).
  //! @param[in] theMode  flag indicating whether the special output should be written for OIT
  //! algorithm
  void SetOitState(Graphic3d_RenderTransparentMethod theMode)
  {
    myOitState.Set(theMode, 0.0f);
    myOitState.Update();
  }

  //! Set the state of weighed OIT rendering pass (only on state change).
  //! @param[in] theDepthFactor  the scalar factor of depth influence to the fragment's coverage
  void SetWeighedOitState(float theDepthFactor)
  {
    myOitState.Set(Graphic3d_RTM_BLEND_OIT, theDepthFactor);
    myOitState.Update();
  }

  //! Pushes state of OIT uniforms to the specified program.
  void PushOitState(const occ::handle<OpenGl_ShaderProgram>& theProgram) const
  {
    if (theProgram->IsValid() && myOitState.Index() != theProgram->ActiveState(OpenGL_OIT_STATE))
    {
      pushOitState(theProgram);
    }
  }

  //! Pushes state of OIT uniforms to the specified program.
  Standard_EXPORT void pushOitState(const occ::handle<OpenGl_ShaderProgram>& theProgram) const;

public:
  //! Pushes current state of OCCT graphics parameters to specified program.
  Standard_EXPORT void PushState(
    const occ::handle<OpenGl_ShaderProgram>& theProgram,
    Graphic3d_TypeOfShadingModel theShadingModel = Graphic3d_TypeOfShadingModel_Unlit) const;

public:
  //! Overwrites context
  void SetContext(OpenGl_Context* theCtx) { myContext = theCtx; }

  //! Returns true when provided context is the same as used one by shader manager.
  bool IsSameContext(OpenGl_Context* theCtx) const { return myContext == theCtx; }

  //! Choose Shading Model for filled primitives.
  //! Fallbacks to FACET model if there are no normal attributes.
  //! Fallbacks to corresponding non-PBR models if PBR is unavailable.
  Graphic3d_TypeOfShadingModel ChooseFaceShadingModel(Graphic3d_TypeOfShadingModel theCustomModel,
                                                      bool theHasNodalNormals) const
  {
    if (!myContext->ColorMask())
    {
      return Graphic3d_TypeOfShadingModel_Unlit;
    }
    Graphic3d_TypeOfShadingModel aModel =
      theCustomModel != Graphic3d_TypeOfShadingModel_DEFAULT ? theCustomModel : myShadingModel;
    switch (aModel)
    {
      case Graphic3d_TypeOfShadingModel_DEFAULT:
      case Graphic3d_TypeOfShadingModel_Unlit:
      case Graphic3d_TypeOfShadingModel_PhongFacet:
        return aModel;
      case Graphic3d_TypeOfShadingModel_Gouraud:
      case Graphic3d_TypeOfShadingModel_Phong:
        return theHasNodalNormals ? aModel : Graphic3d_TypeOfShadingModel_PhongFacet;
      case Graphic3d_TypeOfShadingModel_Pbr:
        return PBRShadingModelFallback(theHasNodalNormals ? aModel
                                                          : Graphic3d_TypeOfShadingModel_PbrFacet,
                                       IsPbrAllowed());
      case Graphic3d_TypeOfShadingModel_PbrFacet:
        return PBRShadingModelFallback(aModel, IsPbrAllowed());
    }
    return Graphic3d_TypeOfShadingModel_Unlit;
  }

  //! Choose Shading Model for line primitives.
  //! Fallbacks to UNLIT model if there are no normal attributes.
  //! Fallbacks to corresponding non-PBR models if PBR is unavailable.
  Graphic3d_TypeOfShadingModel ChooseLineShadingModel(Graphic3d_TypeOfShadingModel theCustomModel,
                                                      bool theHasNodalNormals) const
  {
    if (!myContext->ColorMask())
    {
      return Graphic3d_TypeOfShadingModel_Unlit;
    }
    Graphic3d_TypeOfShadingModel aModel =
      theCustomModel != Graphic3d_TypeOfShadingModel_DEFAULT ? theCustomModel : myShadingModel;
    switch (aModel)
    {
      case Graphic3d_TypeOfShadingModel_DEFAULT:
      case Graphic3d_TypeOfShadingModel_Unlit:
      case Graphic3d_TypeOfShadingModel_PhongFacet:
        return Graphic3d_TypeOfShadingModel_Unlit;
      case Graphic3d_TypeOfShadingModel_Gouraud:
      case Graphic3d_TypeOfShadingModel_Phong:
        return theHasNodalNormals ? aModel : Graphic3d_TypeOfShadingModel_Unlit;
      case Graphic3d_TypeOfShadingModel_Pbr:
        return PBRShadingModelFallback(theHasNodalNormals ? aModel
                                                          : Graphic3d_TypeOfShadingModel_Unlit,
                                       IsPbrAllowed());
      case Graphic3d_TypeOfShadingModel_PbrFacet:
        return Graphic3d_TypeOfShadingModel_Unlit;
    }
    return Graphic3d_TypeOfShadingModel_Unlit;
  }

  //! Choose Shading Model for Marker primitives.
  Graphic3d_TypeOfShadingModel ChooseMarkerShadingModel(Graphic3d_TypeOfShadingModel theCustomModel,
                                                        bool theHasNodalNormals) const
  {
    return ChooseLineShadingModel(theCustomModel, theHasNodalNormals);
  }

  //! Returns default Shading Model.
  Graphic3d_TypeOfShadingModel ShadingModel() const { return myShadingModel; }

  //! Sets shading model.
  Standard_EXPORT void SetShadingModel(const Graphic3d_TypeOfShadingModel theModel);

protected:
  //! Define clipping planes program bits.
  int getClipPlaneBits() const
  {
    const int aNbPlanes = myContext->Clipping().NbClippingOrCappingOn();
    if (aNbPlanes <= 0)
    {
      return 0;
    }

    int aBits = 0;
    if (myContext->Clipping().HasClippingChains())
    {
      aBits |= Graphic3d_ShaderFlags_ClipChains;
    }

    if (aNbPlanes == 1)
    {
      aBits |= Graphic3d_ShaderFlags_ClipPlanes1;
    }
    else if (aNbPlanes == 2)
    {
      aBits |= Graphic3d_ShaderFlags_ClipPlanes2;
    }
    else
    {
      aBits |= Graphic3d_ShaderFlags_ClipPlanesN;
    }
    return aBits;
  }

  //! Define program bits.
  int getProgramBits(const occ::handle<OpenGl_TextureSet>& theTextures,
                     Graphic3d_AlphaMode                   theAlphaMode,
                     Aspect_InteriorStyle                  theInteriorStyle,
                     bool                                  theHasVertColor,
                     bool                                  theEnableEnvMap,
                     bool                                  theEnableMeshEdges) const
  {
    int aBits = 0;
    if (theAlphaMode == Graphic3d_AlphaMode_Mask || theAlphaMode == Graphic3d_AlphaMode_MaskBlend)
    {
      aBits |= Graphic3d_ShaderFlags_AlphaTest;
    }

    aBits |= getClipPlaneBits();
    if (theEnableMeshEdges && myContext->hasGeometryStage != OpenGl_FeatureNotAvailable)
    {
      aBits |= Graphic3d_ShaderFlags_MeshEdges;
      if (theInteriorStyle == Aspect_IS_HOLLOW)
      {
        aBits |= Graphic3d_ShaderFlags_AlphaTest;
      }
    }

    if (theEnableEnvMap)
    {
      // Environment map overwrites material texture
      aBits |= Graphic3d_ShaderFlags_TextureEnv;
    }
    else if (!theTextures.IsNull() && theTextures->HasNonPointSprite())
    {
      aBits |= Graphic3d_ShaderFlags_TextureRGB;
      if ((theTextures->TextureSetBits() & Graphic3d_TextureSetBits_Normal) != 0)
      {
        aBits |= Graphic3d_ShaderFlags_TextureNormal;
      }
    }
    if (theHasVertColor && theInteriorStyle != Aspect_IS_HIDDENLINE)
    {
      aBits |= Graphic3d_ShaderFlags_VertColor;
    }

    if (myOitState.ActiveMode() == Graphic3d_RTM_BLEND_OIT)
    {
      aBits |= Graphic3d_ShaderFlags_WriteOit;
    }
    else if (myOitState.ActiveMode() == Graphic3d_RTM_DEPTH_PEELING_OIT)
    {
      aBits |= Graphic3d_ShaderFlags_OitDepthPeeling;
    }
    return aBits;
  }

  //! Prepare standard GLSL program.
  occ::handle<OpenGl_ShaderProgram>& getStdProgram(Graphic3d_TypeOfShadingModel theShadingModel,
                                                   int                          theBits)
  {
    if (theShadingModel == Graphic3d_TypeOfShadingModel_Unlit
        || (theBits & Graphic3d_ShaderFlags_HasTextures) == Graphic3d_ShaderFlags_TextureEnv)
    {
      // If environment map is enabled lighting calculations are
      // not needed (in accordance with default OCCT behavior)
      occ::handle<OpenGl_ShaderProgram>& aProgram = myUnlitPrograms->ChangeValue(theBits);
      if (aProgram.IsNull())
      {
        prepareStdProgramUnlit(aProgram, theBits, false);
      }
      return aProgram;
    }

    occ::handle<OpenGl_ShaderProgram>& aProgram =
      myLightPrograms->ChangeValue(theShadingModel, theBits);
    if (aProgram.IsNull())
    {
      prepareStdProgramLight(aProgram, theShadingModel, theBits);
    }
    return aProgram;
  }

  //! Prepare standard GLSL program without lighting.
  Standard_EXPORT bool prepareStdProgramUnlit(occ::handle<OpenGl_ShaderProgram>& theProgram,
                                              int                                theBits,
                                              bool theIsOutline = false);

  //! Prepare standard GLSL program with lighting.
  bool prepareStdProgramLight(occ::handle<OpenGl_ShaderProgram>& theProgram,
                              Graphic3d_TypeOfShadingModel       theShadingModel,
                              int                                theBits)
  {
    switch (theShadingModel)
    {
      case Graphic3d_TypeOfShadingModel_Unlit:
        return prepareStdProgramUnlit(theProgram, theBits, false);
      case Graphic3d_TypeOfShadingModel_PhongFacet:
        return prepareStdProgramPhong(theProgram, theBits, true);
      case Graphic3d_TypeOfShadingModel_Gouraud:
        return prepareStdProgramGouraud(theProgram, theBits);
      case Graphic3d_TypeOfShadingModel_DEFAULT:
      case Graphic3d_TypeOfShadingModel_Phong:
        return prepareStdProgramPhong(theProgram, theBits, false);
      case Graphic3d_TypeOfShadingModel_Pbr:
        return prepareStdProgramPhong(theProgram, theBits, false, true);
      case Graphic3d_TypeOfShadingModel_PbrFacet:
        return prepareStdProgramPhong(theProgram, theBits, true, true);
    }
    return false;
  }

  //! Prepare standard GLSL program with per-vertex lighting.
  Standard_EXPORT bool prepareStdProgramGouraud(occ::handle<OpenGl_ShaderProgram>& theProgram,
                                                const int                          theBits);

  //! Prepare standard GLSL program with per-pixel lighting.
  //! @param theIsFlatNormal when TRUE, the Vertex normals will be ignored and Face normal will be
  //! computed instead
  //! @param theIsPBR when TRUE, the PBR pipeline will be activated
  Standard_EXPORT bool prepareStdProgramPhong(occ::handle<OpenGl_ShaderProgram>& theProgram,
                                              const int                          theBits,
                                              const bool theIsFlatNormal = false,
                                              const bool theIsPBR        = false);

  //! Bind specified program to current context and apply state.
  Standard_EXPORT bool bindProgramWithState(const occ::handle<OpenGl_ShaderProgram>& theProgram,
                                            Graphic3d_TypeOfShadingModel theShadingModel);

  //! Set pointer myLightPrograms to active lighting programs set from myMapOfLightPrograms
  Standard_EXPORT void switchLightPrograms();

  //! Prepare standard GLSL program for bounding box.
  Standard_EXPORT bool prepareStdProgramBoundBox();

  //! Prepare GLSL source for IBL generation used in PBR pipeline.
  Standard_EXPORT bool preparePBREnvBakingProgram(int theIndex);

  //! Checks whether one of PBR shading models is set as default model.
  bool IsPbrAllowed() const
  {
    return myShadingModel == Graphic3d_TypeOfShadingModel_Pbr
           || myShadingModel == Graphic3d_TypeOfShadingModel_PbrFacet;
  }

protected:
  //! Packed properties of light source
  struct OpenGl_ShaderLightParameters
  {
    NCollection_Vec4<float> Color;      //!< RGB color + Intensity (in .w)
    NCollection_Vec4<float> Position;   //!< XYZ Direction or Position + IsHeadlight (in .w)
    NCollection_Vec4<float> Direction;  //!< spot light XYZ direction + Range (in .w)
    NCollection_Vec4<float> Parameters; //!< same as Graphic3d_CLight::PackedParams()

    //! Returns packed (serialized) representation of light source properties
    const NCollection_Vec4<float>* Packed() const
    {
      return reinterpret_cast<const NCollection_Vec4<float>*>(this);
    }

    static int NbOfVec4() { return 4; }
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
  //! Append clipping plane definition to temporary buffers.
  void addClippingPlane(int&                            thePlaneId,
                        const Graphic3d_ClipPlane&      thePlane,
                        const NCollection_Vec4<double>& theEq,
                        const int                       theChainFwd) const
  {
    myClipChainArray.SetValue(thePlaneId, theChainFwd);
    NCollection_Vec4<float>& aPlaneEq = myClipPlaneArray.ChangeValue(thePlaneId);
    aPlaneEq.x()                      = float(theEq.x());
    aPlaneEq.y()                      = float(theEq.y());
    aPlaneEq.z()                      = float(theEq.z());
    aPlaneEq.w()                      = float(theEq.w());
    if (myHasLocalOrigin)
    {
      aPlaneEq.w() = float(LocalClippingPlaneW(thePlane));
    }
    ++thePlaneId;
  }

protected:
  occ::handle<OpenGl_ShaderProgramFFP> myFfpProgram;

  Graphic3d_TypeOfShadingModel myShadingModel; //!< lighting shading model
  NCollection_Sequence<occ::handle<OpenGl_ShaderProgram>>
    myProgramList; //!< The list of shader programs
                   // clang-format off
  occ::handle<OpenGl_SetOfShaderPrograms> myLightPrograms;      //!< pointer to active lighting programs matrix
  occ::handle<OpenGl_SetOfPrograms>       myUnlitPrograms;      //!< programs matrix without lighting
  occ::handle<OpenGl_SetOfPrograms>       myOutlinePrograms;    //!< programs matrix without lighting for outline presentation
  occ::handle<OpenGl_ShaderProgram>       myFontProgram;        //!< standard program for textured text
  NCollection_Array1<occ::handle<OpenGl_ShaderProgram>>
                                     myBlitPrograms[2];    //!< standard program for FBO blit emulation
  occ::handle<OpenGl_ShaderProgram>       myBoundBoxProgram;    //!< standard program for bounding box
  occ::handle<OpenGl_ShaderProgram>       myOitCompositingProgram[2]; //!< standard program for OIT compositing (default and MSAA).
  occ::handle<OpenGl_ShaderProgram>       myOitDepthPeelingBlendProgram[2]; //!< standard program for OIT Depth Peeling blend (default and MSAA)
  occ::handle<OpenGl_ShaderProgram>       myOitDepthPeelingFlushProgram[2]; //!< standard program for OIT Depth Peeling flush (default and MSAA)
  NCollection_DataMap<TCollection_AsciiString, occ::handle<OpenGl_SetOfShaderPrograms>>         myMapOfLightPrograms; //!< map of lighting programs depending on lights configuration

  occ::handle<OpenGl_ShaderProgram>       myPBREnvBakingProgram[3]; //!< programs for IBL maps generation used in PBR pipeline (0 for Diffuse; 1 for Specular; 2 for fallback)
  occ::handle<Graphic3d_ShaderProgram>    myBgCubeMapProgram;       //!< program for background cubemap rendering
  occ::handle<Graphic3d_ShaderProgram>    myBgSkydomeProgram;       //!< program for background cubemap rendering
  occ::handle<Graphic3d_ShaderProgram>    myColoredQuadProgram;     //!< program for correct quad rendering

  occ::handle<OpenGl_ShaderProgram>       myStereoPrograms[Graphic3d_StereoMode_NB]; //!< standard stereo programs

  occ::handle<OpenGl_VertexBuffer>        myBoundBoxVertBuffer; //!< bounding box vertex buffer

  mutable occ::handle<OpenGl_PBREnvironment> myPBREnvironment;  //!< manager of IBL maps used in PBR pipeline

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
  bool                   myHasLocalOrigin;     //!< flag indicating that local camera transformation has been set
                        // clang-format on

  mutable NCollection_Array1<int>                          myLightTypeArray;
  mutable NCollection_Array1<OpenGl_ShaderLightParameters> myLightParamsArray;
  mutable NCollection_Array1<NCollection_Mat4<float>>      myShadowMatArray;
  mutable NCollection_Array1<NCollection_Vec4<float>>      myClipPlaneArray;
  mutable NCollection_Array1<NCollection_Vec4<double>>     myClipPlaneArrayFfp;
  mutable NCollection_Array1<int>                          myClipChainArray;
};

#endif // _OpenGl_ShaderManager_HeaderFile
