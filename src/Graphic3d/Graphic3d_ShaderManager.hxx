// Copyright (c) 2013-2021 OPEN CASCADE SAS
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

#ifndef _Graphic3d_ShaderManager_HeaderFile
#define _Graphic3d_ShaderManager_HeaderFile

#include <Aspect_GraphicsLibrary.hxx>
#include <Graphic3d_ShaderFlags.hxx>
#include <Graphic3d_StereoMode.hxx>
#include <Graphic3d_Vec2.hxx>
#include <Standard_Transient.hxx>
#include <TCollection_AsciiString.hxx>

class Graphic3d_LightSet;
class Graphic3d_ShaderProgram;

//! GLSL syntax extensions.
enum Graphic3d_GlslExtension
{
  Graphic3d_GlslExtension_GL_OES_standard_derivatives, //!< OpenGL ES 2.0 extension GL_OES_standard_derivatives
  Graphic3d_GlslExtension_GL_EXT_shader_texture_lod,   //!< OpenGL ES 2.0 extension GL_EXT_shader_texture_lod
  Graphic3d_GlslExtension_GL_EXT_frag_depth,           //!< OpenGL ES 2.0 extension GL_EXT_frag_depth
  Graphic3d_GlslExtension_GL_EXT_gpu_shader4,          //!< OpenGL 2.0 extension GL_EXT_gpu_shader4
};
enum { Graphic3d_GlslExtension_NB = Graphic3d_GlslExtension_GL_EXT_gpu_shader4 + 1 };

//! This class is responsible for generation of shader programs.
class Graphic3d_ShaderManager : public Standard_Transient
{
  DEFINE_STANDARD_RTTIEXT(Graphic3d_ShaderManager, Standard_Transient)
public:

  //! Creates new empty shader manager.
  Standard_EXPORT Graphic3d_ShaderManager (Aspect_GraphicsLibrary theGapi);

  //! Releases resources of shader manager.
  Standard_EXPORT virtual ~Graphic3d_ShaderManager();

  //! @return true if detected GL version is greater or equal to requested one.
  bool IsGapiGreaterEqual (Standard_Integer theVerMajor,
                           Standard_Integer theVerMinor) const
  {
    return (myGapiVersion[0] >  theVerMajor)
        || (myGapiVersion[0] == theVerMajor && myGapiVersion[1] >= theVerMinor);
  }

  //! Return GAPI version major number.
  Standard_Integer GapiVersionMajor() const { return myGapiVersion[0]; }

  //! Return GAPI version minor number.
  Standard_Integer GapiVersionMinor() const { return myGapiVersion[1]; }

  //! Return GAPI version major number.
  void SetGapiVersion (Standard_Integer theVerMajor,
                       Standard_Integer theVerMinor)
  {
    myGapiVersion.SetValues (theVerMajor, theVerMinor);
  }

  //! Return TRUE if RED channel should be used instead of ALPHA for single-channel textures
  //! (e.g. GAPI supports only GL_RED textures and not GL_ALPHA).
  bool UseRedAlpha() const { return myUseRedAlpha; }

  //! Set if RED channel should be used instead of ALPHA for single-channel textures.
  void SetUseRedAlpha (bool theUseRedAlpha) { myUseRedAlpha = theUseRedAlpha; }

  //! Return flag indicating flat shading usage; TRUE by default.
  bool HasFlatShading() const { return myHasFlatShading; }

  //! Return flag indicating flat shading should reverse normal flag; FALSE by default.
  bool ToReverseDFdxSign() const { return myToReverseDFdxSign; }

  //! Set flag indicating flat shading usage.
  void SetFlatShading (bool theToUse,
                       bool theToReverseSign)
  {
    myHasFlatShading = theToUse;
    myToReverseDFdxSign = theToReverseSign;
  }

  //! Return TRUE if depth clamping should be emulated by GLSL program; TRUE by default.
  bool ToEmulateDepthClamp() const { return myToEmulateDepthClamp; }

  //! Set if depth clamping should be emulated by GLSL program.
  void SetEmulateDepthClamp (bool theToEmulate) { myToEmulateDepthClamp = theToEmulate; }

  //! Return TRUE if specified extension is available.
  bool HasGlslExtension (Graphic3d_GlslExtension theExt) const { return myGlslExtensions[theExt]; }

  //! Set if specified extension is available or not.
  void EnableGlslExtension (Graphic3d_GlslExtension theExt,
                            bool theToEnable = true) { myGlslExtensions[theExt] = theToEnable; }

protected:

  //! Generate map key for light sources configuration.
  //! @param theLights [in] list of light sources
  //! @param theHasShadowMap [in] flag indicating shadow maps usage
  Standard_EXPORT TCollection_AsciiString genLightKey (const Handle(Graphic3d_LightSet)& theLights,
                                                       const bool theHasShadowMap) const;

  //! Prepare standard GLSL program for textured font.
  Standard_EXPORT Handle(Graphic3d_ShaderProgram) getStdProgramFont() const;

  //! Prepare standard GLSL program without lighting.
  //! @param theBits      [in] program bits
  //! @param theIsOutline [in] draw silhouette
  Standard_EXPORT Handle(Graphic3d_ShaderProgram) getStdProgramUnlit (Standard_Integer theBits,
                                                                      Standard_Boolean theIsOutline = false) const;

  //! Prepare standard GLSL program with per-vertex lighting.
  //! @param theLights [in] list of light sources
  //! @param theBits   [in] program bits
  Standard_EXPORT Handle(Graphic3d_ShaderProgram) getStdProgramGouraud (const Handle(Graphic3d_LightSet)& theLights,
                                                                        Standard_Integer theBits) const;

  //! Prepare standard GLSL program with per-pixel lighting.
  //! @param theLights [in] list of light sources
  //! @param theBits   [in] program bits
  //! @param theIsFlatNormal [in] when TRUE, the Vertex normals will be ignored and Face normal will be computed instead
  //! @param theIsPBR  [in] when TRUE, the PBR pipeline will be activated
  //! @param theNbShadowMaps [in] number of shadow maps
  Standard_EXPORT Handle(Graphic3d_ShaderProgram) getStdProgramPhong (const Handle(Graphic3d_LightSet)& theLights,
                                                                      const Standard_Integer theBits,
                                                                      const Standard_Boolean theIsFlatNormal,
                                                                      const Standard_Boolean theIsPBR,
                                                                      const Standard_Integer theNbShadowMaps) const;

  //! Prepare standard GLSL program for bounding box.
  Standard_EXPORT Handle(Graphic3d_ShaderProgram) getStdProgramBoundBox() const;

  //! Generates shader program to render environment cubemap as background.
  Standard_EXPORT Handle(Graphic3d_ShaderProgram) getBgCubeMapProgram() const;

  //! Generates shader program to render skydome background.
  Standard_EXPORT Handle(Graphic3d_ShaderProgram) getBgSkydomeProgram() const;

  //! Generates shader program to render correctly colored quad.
  Standard_EXPORT Handle(Graphic3d_ShaderProgram) getColoredQuadProgram() const;

  //! Prepare GLSL source for IBL generation used in PBR pipeline.
  Standard_EXPORT Handle(Graphic3d_ShaderProgram) getPBREnvBakingProgram (Standard_Integer theIndex) const;

  //! Prepare standard GLSL program for FBO blit operation.
  Standard_EXPORT Handle(Graphic3d_ShaderProgram) getStdProgramFboBlit (Standard_Integer theNbSamples,
                                                                        Standard_Boolean theIsFallback_sRGB) const;

  //! Prepare standard GLSL program for stereoscopic image.
  Standard_EXPORT Handle(Graphic3d_ShaderProgram) getStdProgramStereo (Graphic3d_StereoMode theStereoMode) const;

  //! Prepare standard GLSL programs for OIT compositing operation.
  Standard_EXPORT Handle(Graphic3d_ShaderProgram) getStdProgramOitCompositing (Standard_Boolean theMsaa) const;

  //! Prepare standard GLSL programs for OIT Depth Peeling blend operation.
  Standard_EXPORT Handle(Graphic3d_ShaderProgram) getStdProgramOitDepthPeelingBlend (Standard_Boolean theMsaa) const;

  //! Prepare standard GLSL programs for OIT Depth Peeling flush operation.
  Standard_EXPORT Handle(Graphic3d_ShaderProgram) getStdProgramOitDepthPeelingFlush (Standard_Boolean theMsaa) const;

protected:

  //! Return TRUE if bitwise operations can be used in GLSL program.
  Standard_EXPORT bool hasGlslBitwiseOps() const;

  //! Prepare GLSL version header.
  //! @param theProgram [in] [out] program to set version header
  //! @param theName [in] program id suffix
  //! @param theBits [in] program bits
  //! @param theUsesDerivates [in] program uses standard derivatives functions or not
  //! @return filtered program bits with unsupported features disabled
  Standard_EXPORT Standard_Integer defaultGlslVersion (const Handle(Graphic3d_ShaderProgram)& theProgram,
                                                       const TCollection_AsciiString& theName,
                                                       Standard_Integer theBits,
                                                       bool theUsesDerivates = false) const;

  //! Prepare GLSL version header for OIT composition programs.
  //! @param theProgram [in] [out] program to set version header
  //! @param theName [in] program id suffix
  //! @param theMsaa [in] multisampling flag
  Standard_EXPORT void defaultOitGlslVersion (const Handle(Graphic3d_ShaderProgram)& theProgram,
                                              const TCollection_AsciiString& theName,
                                              bool theMsaa) const;

  //! Prepare standard GLSL program for accessing point sprite alpha.
  Standard_EXPORT TCollection_AsciiString pointSpriteAlphaSrc (Standard_Integer theBits) const;

  //! Prepare standard GLSL program for computing point sprite shading.
  Standard_EXPORT TCollection_AsciiString pointSpriteShadingSrc (const TCollection_AsciiString& theBaseColorSrc,
                                                                 Standard_Integer theBits) const;

  //! Define computeLighting GLSL function depending on current lights configuration
  //! @param theNbLights     [out] number of defined light sources
  //! @param theLights       [in]  light sources list
  //! @param theHasVertColor [in]  flag to use getVertColor() instead of Ambient and Diffuse components of active material
  //! @param theIsPBR        [in]  flag to activate PBR pipeline
  //! @param theHasTexColor  [in]  flag to include color texturing
  //! @param theNbShadowMaps [in]  flag to include shadow map
  Standard_EXPORT TCollection_AsciiString stdComputeLighting (Standard_Integer& theNbLights,
                                                              const Handle(Graphic3d_LightSet)& theLights,
                                                              Standard_Boolean  theHasVertColor,
                                                              Standard_Boolean  theIsPBR,
                                                              Standard_Boolean  theHasTexColor,
                                                              Standard_Integer  theNbShadowMaps) const;

protected:

  Aspect_GraphicsLibrary myGapi;          //!< GAPI name
  Graphic3d_Vec2i  myGapiVersion;         //!< GAPI version major/minor number pair
  Standard_Boolean myGlslExtensions[Graphic3d_GlslExtension_NB];
  Standard_Boolean myHasFlatShading;      //!< flag indicating flat shading usage
  Standard_Boolean myToReverseDFdxSign;   //!< flag to reverse flat shading normal (workaround)
  Standard_Boolean mySetPointSize;        //!< always set gl_PointSize variable
  Standard_Boolean myUseRedAlpha;         //!< use RED channel instead of ALPHA (e.g. GAPI supports only GL_RED textures and not GL_ALPHA)
  Standard_Boolean myToEmulateDepthClamp; //!< emulate depth clamping in GLSL program
  Standard_Boolean mySRgbState;           //!< track sRGB state

};

#endif // _Graphic3d_ShaderManager_HeaderFile
