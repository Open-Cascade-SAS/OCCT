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
#include <NCollection_Vec2.hxx>
#include <Standard_TypeDef.hxx>
#include <Standard_Transient.hxx>
#include <TCollection_AsciiString.hxx>

class Graphic3d_LightSet;
class Graphic3d_ShaderProgram;

//! GLSL syntax extensions.
enum Graphic3d_GlslExtension
{
  Graphic3d_GlslExtension_GL_OES_standard_derivatives, //!< OpenGL ES 2.0 extension
                                                       //!< GL_OES_standard_derivatives
  Graphic3d_GlslExtension_GL_EXT_shader_texture_lod,   //!< OpenGL ES 2.0 extension
                                                       //!< GL_EXT_shader_texture_lod
  Graphic3d_GlslExtension_GL_EXT_frag_depth,  //!< OpenGL ES 2.0 extension GL_EXT_frag_depth
  Graphic3d_GlslExtension_GL_EXT_gpu_shader4, //!< OpenGL 2.0 extension GL_EXT_gpu_shader4
};

enum
{
  Graphic3d_GlslExtension_NB = Graphic3d_GlslExtension_GL_EXT_gpu_shader4 + 1
};

//! This class is responsible for generation of shader programs.
class Graphic3d_ShaderManager : public Standard_Transient
{
  DEFINE_STANDARD_RTTIEXT(Graphic3d_ShaderManager, Standard_Transient)
public:
  //! Creates new empty shader manager.
  Standard_EXPORT Graphic3d_ShaderManager(Aspect_GraphicsLibrary theGapi);

  //! Releases resources of shader manager.
  Standard_EXPORT virtual ~Graphic3d_ShaderManager();

  //! @return true if detected GL version is greater or equal to requested one.
  bool IsGapiGreaterEqual(int theVerMajor, int theVerMinor) const
  {
    return (myGapiVersion[0] > theVerMajor)
           || (myGapiVersion[0] == theVerMajor && myGapiVersion[1] >= theVerMinor);
  }

  //! Return GAPI version major number.
  int GapiVersionMajor() const { return myGapiVersion[0]; }

  //! Return GAPI version minor number.
  int GapiVersionMinor() const { return myGapiVersion[1]; }

  //! Return GAPI version major number.
  void SetGapiVersion(int theVerMajor, int theVerMinor)
  {
    myGapiVersion.SetValues(theVerMajor, theVerMinor);
  }

  //! Return TRUE if RED channel should be used instead of ALPHA for single-channel textures
  //! (e.g. GAPI supports only GL_RED textures and not GL_ALPHA).
  bool UseRedAlpha() const { return myUseRedAlpha; }

  //! Set if RED channel should be used instead of ALPHA for single-channel textures.
  void SetUseRedAlpha(bool theUseRedAlpha) { myUseRedAlpha = theUseRedAlpha; }

  //! Return flag indicating flat shading usage; TRUE by default.
  bool HasFlatShading() const { return myHasFlatShading; }

  //! Return flag indicating flat shading should reverse normal flag; FALSE by default.
  bool ToReverseDFdxSign() const { return myToReverseDFdxSign; }

  //! Set flag indicating flat shading usage.
  void SetFlatShading(bool theToUse, bool theToReverseSign)
  {
    myHasFlatShading    = theToUse;
    myToReverseDFdxSign = theToReverseSign;
  }

  //! Return TRUE if depth clamping should be emulated by GLSL program; TRUE by default.
  bool ToEmulateDepthClamp() const { return myToEmulateDepthClamp; }

  //! Set if depth clamping should be emulated by GLSL program.
  void SetEmulateDepthClamp(bool theToEmulate) { myToEmulateDepthClamp = theToEmulate; }

  //! Return TRUE if specified extension is available.
  bool HasGlslExtension(Graphic3d_GlslExtension theExt) const { return myGlslExtensions[theExt]; }

  //! Set if specified extension is available or not.
  void EnableGlslExtension(Graphic3d_GlslExtension theExt, bool theToEnable = true)
  {
    myGlslExtensions[theExt] = theToEnable;
  }

protected:
  //! Generate map key for light sources configuration.
  //! @param[in] theLights  list of light sources
  //! @param[in] theHasShadowMap  flag indicating shadow maps usage
  Standard_EXPORT TCollection_AsciiString
    genLightKey(const occ::handle<Graphic3d_LightSet>& theLights, const bool theHasShadowMap) const;

  //! Prepare standard GLSL program for textured font.
  Standard_EXPORT occ::handle<Graphic3d_ShaderProgram> getStdProgramFont() const;

  //! Prepare standard GLSL program without lighting.
  //! @param[in] theBits       program bits
  //! @param[in] theIsOutline  draw silhouette
  Standard_EXPORT occ::handle<Graphic3d_ShaderProgram> getStdProgramUnlit(
    int  theBits,
    bool theIsOutline = false) const;

  //! Prepare standard GLSL program with per-vertex lighting.
  //! @param[in] theLights  list of light sources
  //! @param[in] theBits    program bits
  Standard_EXPORT occ::handle<Graphic3d_ShaderProgram> getStdProgramGouraud(
    const occ::handle<Graphic3d_LightSet>& theLights,
    int                                    theBits) const;

  //! Prepare standard GLSL program with per-pixel lighting.
  //! @param[in] theLights  list of light sources
  //! @param[in] theBits    program bits
  //! @param[in] theIsFlatNormal  when TRUE, the Vertex normals will be ignored and Face normal will
  //! be computed instead
  //! @param[in] theIsPBR   when TRUE, the PBR pipeline will be activated
  //! @param[in] theNbShadowMaps  number of shadow maps
  Standard_EXPORT occ::handle<Graphic3d_ShaderProgram> getStdProgramPhong(
    const occ::handle<Graphic3d_LightSet>& theLights,
    const int                              theBits,
    const bool                             theIsFlatNormal,
    const bool                             theIsPBR,
    const int                              theNbShadowMaps) const;

  //! Prepare standard GLSL program for bounding box.
  Standard_EXPORT occ::handle<Graphic3d_ShaderProgram> getStdProgramBoundBox() const;

  //! Generates shader program to render environment cubemap as background.
  Standard_EXPORT occ::handle<Graphic3d_ShaderProgram> getBgCubeMapProgram() const;

  //! Generates shader program to render skydome background.
  Standard_EXPORT occ::handle<Graphic3d_ShaderProgram> getBgSkydomeProgram() const;

  //! Generates shader program to render correctly colored quad.
  Standard_EXPORT occ::handle<Graphic3d_ShaderProgram> getColoredQuadProgram() const;

  //! Prepare GLSL source for IBL generation used in PBR pipeline.
  Standard_EXPORT occ::handle<Graphic3d_ShaderProgram> getPBREnvBakingProgram(int theIndex) const;

  //! Prepare standard GLSL program for FBO blit operation.
  Standard_EXPORT occ::handle<Graphic3d_ShaderProgram> getStdProgramFboBlit(
    int  theNbSamples,
    bool theIsFallback_sRGB) const;

  //! Prepare standard GLSL program for stereoscopic image.
  Standard_EXPORT occ::handle<Graphic3d_ShaderProgram> getStdProgramStereo(
    Graphic3d_StereoMode theStereoMode) const;

  //! Prepare standard GLSL programs for OIT compositing operation.
  Standard_EXPORT occ::handle<Graphic3d_ShaderProgram> getStdProgramOitCompositing(
    bool theMsaa) const;

  //! Prepare standard GLSL programs for OIT Depth Peeling blend operation.
  Standard_EXPORT occ::handle<Graphic3d_ShaderProgram> getStdProgramOitDepthPeelingBlend(
    bool theMsaa) const;

  //! Prepare standard GLSL programs for OIT Depth Peeling flush operation.
  Standard_EXPORT occ::handle<Graphic3d_ShaderProgram> getStdProgramOitDepthPeelingFlush(
    bool theMsaa) const;

protected:
  //! Return TRUE if bitwise operations can be used in GLSL program.
  Standard_EXPORT bool hasGlslBitwiseOps() const;

  //! Prepare GLSL version header.
  //! @param[in][out] theProgram   program to set version header
  //! @param[in] theName  program id suffix
  //! @param[in] theBits  program bits
  //! @param[in] theUsesDerivates  program uses standard derivatives functions or not
  //! @return filtered program bits with unsupported features disabled
  Standard_EXPORT int defaultGlslVersion(const occ::handle<Graphic3d_ShaderProgram>& theProgram,
                                         const TCollection_AsciiString&              theName,
                                         int                                         theBits,
                                         bool theUsesDerivates = false) const;

  //! Prepare GLSL version header for OIT composition programs.
  //! @param[in][out] theProgram   program to set version header
  //! @param[in] theName  program id suffix
  //! @param[in] theMsaa  multisampling flag
  Standard_EXPORT void defaultOitGlslVersion(const occ::handle<Graphic3d_ShaderProgram>& theProgram,
                                             const TCollection_AsciiString&              theName,
                                             bool theMsaa) const;

  //! Prepare standard GLSL program for accessing point sprite alpha.
  Standard_EXPORT TCollection_AsciiString pointSpriteAlphaSrc(int theBits) const;

  //! Prepare standard GLSL program for computing point sprite shading.
  Standard_EXPORT TCollection_AsciiString
    pointSpriteShadingSrc(const TCollection_AsciiString& theBaseColorSrc, int theBits) const;

  //! Define computeLighting GLSL function depending on current lights configuration
  //! @param[out] theNbLights      number of defined light sources
  //! @param[in] theLights         light sources list
  //! @param[in] theHasVertColor   flag to use getVertColor() instead of Ambient and Diffuse
  //! components of active material
  //! @param[in] theIsPBR          flag to activate PBR pipeline
  //! @param[in] theHasTexColor    flag to include color texturing
  //! @param[in] theNbShadowMaps   flag to include shadow map
  Standard_EXPORT TCollection_AsciiString
    stdComputeLighting(int&                                   theNbLights,
                       const occ::handle<Graphic3d_LightSet>& theLights,
                       bool                                   theHasVertColor,
                       bool                                   theIsPBR,
                       bool                                   theHasTexColor,
                       int                                    theNbShadowMaps) const;

protected:
  Aspect_GraphicsLibrary myGapi;        //!< GAPI name
  NCollection_Vec2<int>  myGapiVersion; //!< GAPI version major/minor number pair
  bool                   myGlslExtensions[Graphic3d_GlslExtension_NB];
  bool                   myHasFlatShading;    //!< flag indicating flat shading usage
  bool                   myToReverseDFdxSign; //!< flag to reverse flat shading normal (workaround)
  bool                   mySetPointSize;      //!< always set gl_PointSize variable
                                              // clang-format off
  bool myUseRedAlpha;         //!< use RED channel instead of ALPHA (e.g. GAPI supports only GL_RED textures and not GL_ALPHA)
                                              // clang-format on
  bool myToEmulateDepthClamp;                 //!< emulate depth clamping in GLSL program
  bool mySRgbState;                           //!< track sRGB state
};

#endif // _Graphic3d_ShaderManager_HeaderFile
