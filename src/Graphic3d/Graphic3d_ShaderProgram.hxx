// Created on: 2013-09-20
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

#ifndef _Graphic3d_ShaderProgram_HeaderFile
#define _Graphic3d_ShaderProgram_HeaderFile

#include <Graphic3d_RenderTransparentMethod.hxx>
#include <Graphic3d_ShaderAttribute.hxx>
#include <Graphic3d_ShaderObject.hxx>
#include <Graphic3d_ShaderVariable.hxx>
#include <Graphic3d_TextureParams.hxx>
#include <Graphic3d_TextureSetBits.hxx>
#include <NCollection_Sequence.hxx>

//! List of shader objects.
typedef NCollection_Sequence<Handle(Graphic3d_ShaderObject)> Graphic3d_ShaderObjectList;

//! List of custom uniform shader variables.
typedef NCollection_Sequence<Handle(Graphic3d_ShaderVariable)> Graphic3d_ShaderVariableList;

//! List of custom vertex shader attributes
typedef NCollection_Sequence<Handle(Graphic3d_ShaderAttribute)> Graphic3d_ShaderAttributeList;

//! This class is responsible for managing shader programs.
class Graphic3d_ShaderProgram : public Standard_Transient
{
  DEFINE_STANDARD_RTTIEXT(Graphic3d_ShaderProgram, Standard_Transient)
public:

  //! Default value of THE_MAX_LIGHTS macros within GLSL program (see Declarations.glsl).
  static const Standard_Integer THE_MAX_LIGHTS_DEFAULT = 8;

  //! Default value of THE_MAX_CLIP_PLANES macros within GLSL program (see Declarations.glsl).
  static const Standard_Integer THE_MAX_CLIP_PLANES_DEFAULT = 8;

  //! Default value of THE_NB_FRAG_OUTPUTS macros within GLSL program (see Declarations.glsl).
  static const Standard_Integer THE_NB_FRAG_OUTPUTS = 1;

public:

  //! Creates new empty program object.
  Standard_EXPORT Graphic3d_ShaderProgram();

  //! Releases resources of program object.
  Standard_EXPORT virtual ~Graphic3d_ShaderProgram();

  //! Checks if the program object is valid or not.
  Standard_EXPORT virtual Standard_Boolean IsDone() const;

  //! Returns unique ID used to manage resource in graphic driver.
  const TCollection_AsciiString& GetId() const { return myID; }

  //! Sets unique ID used to manage resource in graphic driver.
  //! WARNING! Graphic3d_ShaderProgram constructor generates a unique id for proper resource management;
  //! however if application overrides it, it is responsibility of application to avoid name collisions.
  void SetId (const TCollection_AsciiString& theId) { myID = theId; }

  //! Returns GLSL header (version code and extensions).
  const TCollection_AsciiString& Header() const { return myHeader; }

  //! Setup GLSL header containing language version code and used extensions.
  //! Will be prepended to the very beginning of the source code.
  //! Example:
  //! @code
  //!   #version 300 es
  //!   #extension GL_ARB_bindless_texture : require
  //! @endcode
  void SetHeader (const TCollection_AsciiString& theHeader) { myHeader = theHeader; }

  //! Append line to GLSL header.
  void AppendToHeader (const TCollection_AsciiString& theHeaderLine)
  {
    if (!myHeader.IsEmpty())
    {
      myHeader += "\n";
    }
    myHeader += theHeaderLine;
  }

  //! Return the length of array of light sources (THE_MAX_LIGHTS),
  //! to be used for initialization occLightSources.
  //! Default value is THE_MAX_LIGHTS_DEFAULT.
  Standard_Integer NbLightsMax() const { return myNbLightsMax; }

  //! Specify the length of array of light sources (THE_MAX_LIGHTS).
  void SetNbLightsMax (Standard_Integer theNbLights) { myNbLightsMax = theNbLights; }

  //! Return the length of array of shadow maps (THE_NB_SHADOWMAPS); 0 by default.
  Standard_Integer NbShadowMaps() const { return myNbShadowMaps; }

  //! Specify the length of array of shadow maps (THE_NB_SHADOWMAPS).
  void SetNbShadowMaps (Standard_Integer theNbMaps) { myNbShadowMaps = theNbMaps; }

  //! Return the length of array of clipping planes (THE_MAX_CLIP_PLANES),
  //! to be used for initialization occClipPlaneEquations.
  //! Default value is THE_MAX_CLIP_PLANES_DEFAULT.
  Standard_Integer NbClipPlanesMax() const { return myNbClipPlanesMax; }

  //! Specify the length of array of clipping planes (THE_MAX_CLIP_PLANES).
  void SetNbClipPlanesMax (Standard_Integer theNbPlanes) { myNbClipPlanesMax = theNbPlanes; }

  //! Attaches shader object to the program object.
  Standard_EXPORT Standard_Boolean AttachShader (const Handle(Graphic3d_ShaderObject)& theShader);

  //! Detaches shader object from the program object.
  Standard_EXPORT Standard_Boolean DetachShader (const Handle(Graphic3d_ShaderObject)& theShader);

  //! Returns list of attached shader objects.
  const Graphic3d_ShaderObjectList& ShaderObjects() const { return myShaderObjects; }

  //! The list of currently pushed but not applied custom uniform variables.
  //! This list is automatically cleared after applying to GLSL program.
  const Graphic3d_ShaderVariableList& Variables() const { return myVariables; }

  //! Return the list of custom vertex attributes.
  const Graphic3d_ShaderAttributeList& VertexAttributes() const { return myAttributes; }

  //! Assign the list of custom vertex attributes.
  //! Should be done before GLSL program initialization.
  Standard_EXPORT void SetVertexAttributes (const Graphic3d_ShaderAttributeList& theAttributes);

  //! Returns the number (1+) of Fragment Shader outputs to be written to
  //! (more than 1 can be in case of multiple draw buffers); 1 by default.
  Standard_Integer NbFragmentOutputs() const { return myNbFragOutputs; }

  //! Sets the number of Fragment Shader outputs to be written to.
  //! Should be done before GLSL program initialization.
  void SetNbFragmentOutputs (const Standard_Integer theNbOutputs) { myNbFragOutputs = theNbOutputs; }

  //! Return true if Fragment Shader should perform alpha test; FALSE by default.
  Standard_Boolean HasAlphaTest() const { return myHasAlphaTest; }

  //! Set if Fragment Shader should perform alpha test.
  //! Note that this flag is designed for usage with - custom shader program may discard fragment regardless this flag.
  void SetAlphaTest (Standard_Boolean theAlphaTest) { myHasAlphaTest = theAlphaTest; }

  //! Return TRUE if standard program header should define default texture sampler occSampler0; TRUE by default for compatibility.
  Standard_Boolean HasDefaultSampler() const { return myHasDefSampler; }

  //! Set if standard program header should define default texture sampler occSampler0.
  void SetDefaultSampler (Standard_Boolean theHasDefSampler) { myHasDefSampler = theHasDefSampler; }

  //! Return if Fragment Shader color should output to OIT buffers; OFF by default.
  Graphic3d_RenderTransparentMethod OitOutput() const { return myOitOutput; }

  //! Set if Fragment Shader color should output to OIT buffers.
  //! Note that weighted OIT also requires at least 2 Fragment Outputs (color + coverage),
  //! and Depth Peeling requires at least 3 Fragment Outputs (depth + front color + back color),
  void SetOitOutput (Graphic3d_RenderTransparentMethod theOutput) { myOitOutput = theOutput; }

  //! Return TRUE if standard program header should define functions and variables used in PBR pipeline.
  //! FALSE by default.
  Standard_Boolean IsPBR() const { return myIsPBR; }

  //! Sets whether standard program header should define functions and variables used in PBR pipeline.
  void SetPBR (Standard_Boolean theIsPBR) { myIsPBR = theIsPBR; }

  //! Return texture units declared within the program, @sa Graphic3d_TextureSetBits.
  Standard_Integer TextureSetBits() const { return myTextureSetBits; }

  //! Set texture units declared within the program.
  void SetTextureSetBits (Standard_Integer theBits) { myTextureSetBits = theBits; }

  //! Pushes custom uniform variable to the program.
  //! The list of pushed variables is automatically cleared after applying to GLSL program.
  //! Thus after program recreation even unchanged uniforms should be pushed anew.
  template<class T>
  Standard_Boolean PushVariable (const TCollection_AsciiString& theName,
                                 const T&                       theValue);

  //! Removes all custom uniform variables from the program.
  Standard_EXPORT void ClearVariables();

  //! Pushes float uniform.
  Standard_Boolean PushVariableFloat (const TCollection_AsciiString& theName, const float theValue)            { return PushVariable (theName, theValue); }

  //! Pushes vec2 uniform.
  Standard_Boolean PushVariableVec2  (const TCollection_AsciiString& theName, const Graphic3d_Vec2& theValue)  { return PushVariable (theName, theValue); }

  //! Pushes vec3 uniform.
  Standard_Boolean PushVariableVec3  (const TCollection_AsciiString& theName, const Graphic3d_Vec3& theValue)  { return PushVariable (theName, theValue); }

  //! Pushes vec4 uniform.
  Standard_Boolean PushVariableVec4  (const TCollection_AsciiString& theName, const Graphic3d_Vec4& theValue)  { return PushVariable (theName, theValue); }

  //! Pushes int uniform.
  Standard_Boolean PushVariableInt   (const TCollection_AsciiString& theName, const int theValue)              { return PushVariable (theName, theValue); }

  //! Pushes vec2i uniform.
  Standard_Boolean PushVariableVec2i (const TCollection_AsciiString& theName, const Graphic3d_Vec2i& theValue) { return PushVariable (theName, theValue); }

  //! Pushes vec3i uniform.
  Standard_Boolean PushVariableVec3i (const TCollection_AsciiString& theName, const Graphic3d_Vec3i& theValue) { return PushVariable (theName, theValue); }

  //! Pushes vec4i uniform.
  Standard_Boolean PushVariableVec4i (const TCollection_AsciiString& theName, const Graphic3d_Vec4i& theValue) { return PushVariable (theName, theValue); }

public:

  //! The path to GLSL programs determined from CSF_ShadersDirectory or CASROOT environment variables.
  //! @return the root folder with default GLSL programs.
  Standard_EXPORT static const TCollection_AsciiString& ShadersFolder();

private:

  TCollection_AsciiString       myID;            //!< the unique identifier of program object
  Graphic3d_ShaderObjectList    myShaderObjects; //!< the list of attached shader objects
  Graphic3d_ShaderVariableList  myVariables;     //!< the list of custom uniform variables
  Graphic3d_ShaderAttributeList myAttributes;    //!< the list of custom vertex attributes
  TCollection_AsciiString       myHeader;        //!< GLSL header with version code and used extensions
  Standard_Integer              myNbLightsMax;   //!< length of array of light sources (THE_MAX_LIGHTS)
  Standard_Integer              myNbShadowMaps;  //!< length of array of shadow maps (THE_NB_SHADOWMAPS)
  Standard_Integer              myNbClipPlanesMax; //!< length of array of clipping planes (THE_MAX_CLIP_PLANES)
  Standard_Integer              myNbFragOutputs; //!< length of array of Fragment Shader outputs (THE_NB_FRAG_OUTPUTS)
  Standard_Integer              myTextureSetBits;//!< texture units declared within the program, @sa Graphic3d_TextureSetBits
  Graphic3d_RenderTransparentMethod myOitOutput; //!< flag indicating that Fragment Shader includes OIT outputs
  Standard_Boolean              myHasDefSampler; //!< flag indicating that program defines default texture sampler occSampler0
  Standard_Boolean              myHasAlphaTest;       //!< flag indicating that Fragment Shader performs alpha test
  Standard_Boolean              myIsPBR;         //!< flag indicating that program defines functions and variables used in PBR pipeline

};

DEFINE_STANDARD_HANDLE (Graphic3d_ShaderProgram, Standard_Transient)

// =======================================================================
// function : PushVariable
// purpose  : Pushes custom uniform variable to the program
// =======================================================================
template<class T> inline
Standard_Boolean Graphic3d_ShaderProgram::PushVariable (const TCollection_AsciiString& theName,
                                                        const T& theValue)
{
  Handle(Graphic3d_ShaderVariable) aVariable = Graphic3d_ShaderVariable::Create (theName, theValue);
  if (aVariable.IsNull() || !aVariable->IsDone())
  {
    return Standard_False;
  }

  myVariables.Append (aVariable);
  return Standard_True;
}

#endif
