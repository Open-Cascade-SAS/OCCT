// Created on: 2013-09-19
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

#ifndef OpenGl_ShaderProgram_HeaderFile
#define OpenGl_ShaderProgram_HeaderFile

#include <NCollection_DataMap.hxx>
#include <NCollection_Sequence.hxx>
#include <TCollection_AsciiString.hxx>

#include <Graphic3d_ShaderProgram.hxx>
#include <Graphic3d_TextureSetBits.hxx>

#include <OpenGl_Vec.hxx>
#include <OpenGl_NamedResource.hxx>
#include <OpenGl_ShaderObject.hxx>

class OpenGl_ShaderProgram;
DEFINE_STANDARD_HANDLE(OpenGl_ShaderProgram, OpenGl_NamedResource)

//! The enumeration of OCCT-specific OpenGL/GLSL variables.
enum OpenGl_StateVariable
{
  // OpenGL matrix state
  OpenGl_OCC_MODEL_WORLD_MATRIX,
  OpenGl_OCC_WORLD_VIEW_MATRIX,
  OpenGl_OCC_PROJECTION_MATRIX,
  OpenGl_OCC_MODEL_WORLD_MATRIX_INVERSE,
  OpenGl_OCC_WORLD_VIEW_MATRIX_INVERSE,
  OpenGl_OCC_PROJECTION_MATRIX_INVERSE,
  OpenGl_OCC_MODEL_WORLD_MATRIX_TRANSPOSE,
  OpenGl_OCC_WORLD_VIEW_MATRIX_TRANSPOSE,
  OpenGl_OCC_PROJECTION_MATRIX_TRANSPOSE,
  OpenGl_OCC_MODEL_WORLD_MATRIX_INVERSE_TRANSPOSE,
  OpenGl_OCC_WORLD_VIEW_MATRIX_INVERSE_TRANSPOSE,
  OpenGl_OCC_PROJECTION_MATRIX_INVERSE_TRANSPOSE,

  // OpenGL clip planes state
  OpenGl_OCC_CLIP_PLANE_EQUATIONS,
  OpenGl_OCC_CLIP_PLANE_CHAINS,
  OpenGl_OCC_CLIP_PLANE_COUNT,

  // OpenGL light state
  OpenGl_OCC_LIGHT_SOURCE_COUNT,
  OpenGl_OCC_LIGHT_SOURCE_TYPES,
  OpenGl_OCC_LIGHT_SOURCE_PARAMS,
  OpenGl_OCC_LIGHT_AMBIENT,
  OpenGl_OCC_LIGHT_SHADOWMAP_SIZE_BIAS,// occShadowMapSizeBias
  OpenGl_OCC_LIGHT_SHADOWMAP_SAMPLERS, // occShadowMapSamplers
  OpenGl_OCC_LIGHT_SHADOWMAP_MATRICES, // occShadowMapMatrices

  // Material state
  OpenGl_OCCT_TEXTURE_ENABLE,
  OpenGl_OCCT_DISTINGUISH_MODE,
  OpenGl_OCCT_PBR_MATERIAL,
  OpenGl_OCCT_COMMON_MATERIAL,
  OpenGl_OCCT_ALPHA_CUTOFF,
  OpenGl_OCCT_COLOR,

  // Weighted, Blended Order-Independent Transparency rendering state
  OpenGl_OCCT_OIT_OUTPUT,
  OpenGl_OCCT_OIT_DEPTH_FACTOR,

  // Context-dependent state
  OpenGl_OCCT_TEXTURE_TRSF2D,
  OpenGl_OCCT_POINT_SIZE,

  // Wireframe state
  OpenGl_OCCT_VIEWPORT,
  OpenGl_OCCT_LINE_WIDTH,
  OpenGl_OCCT_LINE_FEATHER,
  OpenGl_OCCT_LINE_STIPPLE_PATTERN, // occStipplePattern
  OpenGl_OCCT_LINE_STIPPLE_FACTOR,  // occStippleFactor
  OpenGl_OCCT_WIREFRAME_COLOR,
  OpenGl_OCCT_QUAD_MODE_STATE,

  // Parameters of outline (silhouette) shader
  OpenGl_OCCT_ORTHO_SCALE,
  OpenGl_OCCT_SILHOUETTE_THICKNESS,

  // PBR state
  OpenGl_OCCT_NB_SPEC_IBL_LEVELS,

  // DON'T MODIFY THIS ITEM (insert new items before it)
  OpenGl_OCCT_NUMBER_OF_STATE_VARIABLES
};

//! Interface for generic setter of user-defined uniform variables.
struct OpenGl_SetterInterface
{
  //! Sets user-defined uniform variable to specified program.
  virtual void Set (const Handle(OpenGl_Context)&           theCtx,
                    const Handle(Graphic3d_ShaderVariable)& theVariable,
                    OpenGl_ShaderProgram*                   theProgram) = 0;

  //! Destructor
  virtual ~OpenGl_SetterInterface() {}
};

//! List of OpenGL shader objects.
typedef NCollection_Sequence<Handle(OpenGl_ShaderObject)>    OpenGl_ShaderList;

//! List of shader variable setters.
typedef NCollection_DataMap<size_t, OpenGl_SetterInterface*> OpenGl_SetterList;

//! Support tool for setting user-defined uniform variables.
class OpenGl_VariableSetterSelector
{
public:

  //! Creates new setter selector.
  OpenGl_VariableSetterSelector();

  //! Releases memory resources of setter selector.
  ~OpenGl_VariableSetterSelector();

  //! Sets user-defined uniform variable to specified program.
  void Set (const Handle(OpenGl_Context)&           theCtx,
            const Handle(Graphic3d_ShaderVariable)& theVariable,
            OpenGl_ShaderProgram*                   theProgram) const;

private:

  //! List of variable setters.
  OpenGl_SetterList mySetterList;
};

//! Defines types of uniform state variables.
enum OpenGl_UniformStateType
{
  OpenGl_LIGHT_SOURCES_STATE,
  OpenGl_CLIP_PLANES_STATE,
  OpenGl_MODEL_WORLD_STATE,
  OpenGl_WORLD_VIEW_STATE,
  OpenGl_PROJECTION_STATE,
  OpenGl_MATERIAL_STATE,
  OpenGl_SURF_DETAIL_STATE,
  OpenGL_OIT_STATE,
  OpenGl_UniformStateType_NB
};

//! Simple class represents GLSL program variable location.
class OpenGl_ShaderUniformLocation
{
public:
  //! Invalid location of uniform/attribute variable.
  static const GLint INVALID_LOCATION = -1;
public:

  //! Construct an invalid location.
  OpenGl_ShaderUniformLocation() : myLocation (INVALID_LOCATION) {}

  //! Constructor with initialization.
  explicit OpenGl_ShaderUniformLocation (GLint theLocation) : myLocation (theLocation) {}

  //! Note you may safely put invalid location in functions like glUniform* - the data passed in will be silently ignored.
  //! @return true if location is not equal to -1.
  bool IsValid() const { return myLocation != INVALID_LOCATION; }

  //! Return TRUE for non-invalid location.
  operator bool() const { return myLocation != INVALID_LOCATION; }

  //! Convert operators help silently put object to GL functions like glUniform*.
  operator GLint() const { return myLocation; }

private:
  GLint myLocation;
};

//! Wrapper for OpenGL program object.
class OpenGl_ShaderProgram : public OpenGl_NamedResource
{
  friend class OpenGl_View;
  friend class OpenGl_ShaderManager;
  DEFINE_STANDARD_RTTIEXT(OpenGl_ShaderProgram, OpenGl_NamedResource)
public:

  //! Non-valid shader name.
  static const GLuint NO_PROGRAM = 0;

  //! Invalid location of uniform/attribute variable.
  static const GLint INVALID_LOCATION = -1;

  //! List of pre-defined OCCT state uniform variables.
  static Standard_CString PredefinedKeywords[OpenGl_OCCT_NUMBER_OF_STATE_VARIABLES];

  //! Wrapper for compiling shader object with verbose printing on error.
  Standard_EXPORT static bool compileShaderVerbose (const Handle(OpenGl_Context)& theCtx,
                                                    const Handle(OpenGl_ShaderObject)& theShader,
                                                    const TCollection_AsciiString& theSource,
                                                    bool theToPrintSource = true);

  //! Creates uninitialized shader program.
  //!
  //! WARNING! This constructor is not intended to be called anywhere but from OpenGl_ShaderManager::Create().
  //! Manager has been designed to synchronize camera position, lights definition and other aspects of the program implicitly,
  //! as well as sharing same program across rendering groups.
  //!
  //! Program created outside the manager will be left detached from these routines,
  //! and them should be performed manually by caller.
  //!
  //! This constructor has been made public to provide more flexibility to re-use OCCT OpenGL classes without OCCT Viewer itself.
  //! If this is not the case - create the program using shared OpenGl_ShaderManager instance instead.
  Standard_EXPORT OpenGl_ShaderProgram (const Handle(Graphic3d_ShaderProgram)& theProxy = NULL,
                                        const TCollection_AsciiString& theId = "");

protected:

  static OpenGl_VariableSetterSelector mySetterSelector;

public:

  //! Releases resources of shader program.
  Standard_EXPORT virtual ~OpenGl_ShaderProgram();

  //! Creates new empty shader program of specified type.
  Standard_EXPORT Standard_Boolean Create (const Handle(OpenGl_Context)& theCtx);

  //! Destroys shader program.
  Standard_EXPORT virtual void Release (OpenGl_Context* theCtx) Standard_OVERRIDE;

  //! Returns estimated GPU memory usage - cannot be easily estimated.
  virtual Standard_Size EstimatedDataSize() const Standard_OVERRIDE { return 0; }

  //! Attaches shader object to the program object.
  Standard_EXPORT Standard_Boolean AttachShader (const Handle(OpenGl_Context)&      theCtx,
                                                 const Handle(OpenGl_ShaderObject)& theShader);

  //! Detaches shader object to the program object.
  Standard_EXPORT Standard_Boolean DetachShader (const Handle(OpenGl_Context)&      theCtx,
                                                 const Handle(OpenGl_ShaderObject)& theShader);

  //! Initializes program object with the list of shader objects.
  Standard_EXPORT Standard_Boolean Initialize (const Handle(OpenGl_Context)&     theCtx,
                                               const Graphic3d_ShaderObjectList& theShaders);

  //! Links the program object.
  //! @param theCtx bound OpenGL context
  //! @param theIsVerbose flag to print log on error
  Standard_EXPORT Standard_Boolean Link (const Handle(OpenGl_Context)& theCtx,
                                         bool theIsVerbose = true);

  //! Fetches information log of the last link operation.
  Standard_EXPORT Standard_Boolean FetchInfoLog (const Handle(OpenGl_Context)& theCtx,
                                                 TCollection_AsciiString&      theLog);

  //! Fetches uniform variables from proxy shader program.
  Standard_EXPORT Standard_Boolean ApplyVariables (const Handle(OpenGl_Context)& theCtx);
  
  //! @return proxy shader program.
  const Handle(Graphic3d_ShaderProgram)& Proxy() const { return myProxy; }

  //! @return true if current object was initialized
  inline bool IsValid() const
  {
    return myProgramID != NO_PROGRAM;
  }

  //! @return program ID
  inline GLuint ProgramId() const
  {
    return myProgramID;
  }

public:

  //! Return TRUE if program defines tessellation stage.
  Standard_Boolean HasTessellationStage() const { return myHasTessShader; }

  //! Return the length of array of light sources (THE_MAX_LIGHTS),
  //! to be used for initialization occLightSources (OpenGl_OCC_LIGHT_SOURCE_PARAMS).
  Standard_Integer NbLightsMax() const { return myNbLightsMax; }

  //! Return the length of array of shadow maps (THE_NB_SHADOWMAPS); 0 by default.
  Standard_Integer NbShadowMaps() const { return myNbShadowMaps; }

  //! Return the length of array of clipping planes (THE_MAX_CLIP_PLANES),
  //! to be used for initialization occClipPlaneEquations (OpenGl_OCC_CLIP_PLANE_EQUATIONS) and occClipPlaneChains (OpenGl_OCC_CLIP_PLANE_CHAINS).
  Standard_Integer NbClipPlanesMax() const { return myNbClipPlanesMax; }

  //! Return the length of array of Fragment Shader outputs (THE_NB_FRAG_OUTPUTS),
  //! to be used for initialization occFragColorArray/occFragColorN.
  Standard_Integer NbFragmentOutputs() const { return myNbFragOutputs; }

  //! Return true if Fragment Shader should perform alpha test; FALSE by default.
  Standard_Boolean HasAlphaTest() const { return myHasAlphaTest; }

  //! Return if Fragment Shader color should output the OIT values; OFF by default.
  Graphic3d_RenderTransparentMethod OitOutput() const { return myOitOutput; }

  //! Return texture units declared within the program, @sa Graphic3d_TextureSetBits.
  Standard_Integer TextureSetBits() const { return myTextureSetBits; }

private:

  //! Returns index of last modification of variables of specified state type.
  Standard_Size ActiveState (const OpenGl_UniformStateType theType) const
  {
    return theType < OpenGl_UniformStateType_NB
         ? myCurrentState[theType]
         : 0;
  }

  //! Updates index of last modification of variables of specified state type.
  void UpdateState (const OpenGl_UniformStateType theType,
                    const Standard_Size           theIndex)
  {
    if (theType < OpenGl_UniformStateType_NB)
    {
      myCurrentState[theType] = theIndex;
    }
  }

public:

  //! Returns location of the specific uniform variable.
  Standard_EXPORT OpenGl_ShaderUniformLocation GetUniformLocation (const Handle(OpenGl_Context)& theCtx,
                                                                   const GLchar*                 theName) const;

  //! Returns index of the generic vertex attribute by variable name.
  Standard_EXPORT GLint GetAttributeLocation (const Handle(OpenGl_Context)& theCtx,
                                              const GLchar*                 theName) const;

  //! Returns location of the OCCT state uniform variable.
  const OpenGl_ShaderUniformLocation& GetStateLocation (OpenGl_StateVariable theVariable) const { return myStateLocations[theVariable]; }

public:

  //! Returns the value of the uniform variable from given name.
  template<typename ValueType>
  bool GetUniform (const Handle(OpenGl_Context)& theCtx,
                   const GLchar* theName,
                   ValueType& theValue) const
  {
    return GetUniform (theCtx, GetUniformLocation (theCtx, theName), theValue);
  }

  //! Returns the value of the integer uniform variable.
  //! Wrapper for glGetUniformiv()
  Standard_EXPORT Standard_Boolean GetUniform (const Handle(OpenGl_Context)& theCtx,
                                               GLint                         theLocation,
                                               OpenGl_Vec4i&                 theValue) const;

  //! Returns the value of the float uniform variable.
  //! Wrapper for glGetUniformfv()
  Standard_EXPORT Standard_Boolean GetUniform (const Handle(OpenGl_Context)& theCtx,
                                               GLint                         theLocation,
                                               OpenGl_Vec4&                  theValue) const;

public:

  //! Returns the vertex attribute from given name.
  template<typename ValueType>
  bool GetAttribute (const Handle(OpenGl_Context)& theCtx,
                     const GLchar* theName,
                     ValueType& theValue) const
  {
    return GetAttribute (theCtx, GetAttributeLocation (theCtx, theName), theValue);
  }

  //! Returns the integer vertex attribute.
  //! Wrapper for glGetVertexAttribiv()
  Standard_EXPORT Standard_Boolean GetAttribute (const Handle(OpenGl_Context)& theCtx,
                                                 GLint                         theIndex,
                                                 OpenGl_Vec4i&                 theValue) const;

  //! Returns the float vertex attribute.
  //! Wrapper for glGetVertexAttribfv()
  Standard_EXPORT Standard_Boolean GetAttribute (const Handle(OpenGl_Context)& theCtx,
                                                 GLint                         theIndex,
                                                 OpenGl_Vec4&                  theValue) const;

public:

  //! Wrapper for glBindAttribLocation()
  Standard_EXPORT Standard_Boolean SetAttributeName (const Handle(OpenGl_Context)& theCtx,
                                                     GLint                         theIndex,
                                                     const GLchar*                 theName);

  //! Wrapper for glVertexAttrib*() for attribute with the given name.
  template<typename ValueType>
  bool SetAttribute (const Handle(OpenGl_Context)& theCtx,
                     const GLchar* theName,
                     const ValueType& theValue)
  {
    return SetAttribute (theCtx, GetAttributeLocation (theCtx, theName), theValue);
  }

  //! Wrapper for glVertexAttrib1f()
  Standard_EXPORT Standard_Boolean SetAttribute (const Handle(OpenGl_Context)& theCtx,
                                                 GLint                         theIndex,
                                                 GLfloat                       theValue);

  //! Wrapper for glVertexAttrib2fv()
  Standard_EXPORT Standard_Boolean SetAttribute (const Handle(OpenGl_Context)& theCtx,
                                                 GLint                         theIndex,
                                                 const OpenGl_Vec2&            theValue);

  //! Wrapper for glVertexAttrib3fv()
  Standard_EXPORT Standard_Boolean SetAttribute (const Handle(OpenGl_Context)& theCtx,
                                                 GLint                         theIndex,
                                                 const OpenGl_Vec3&            theValue);

  //! Wrapper for glVertexAttrib4fv()
  Standard_EXPORT Standard_Boolean SetAttribute (const Handle(OpenGl_Context)& theCtx,
                                                 GLint                         theIndex,
                                                 const OpenGl_Vec4&            theValue);

public:

  //! Specifies the value of the uniform variable with given name.
  template<typename ValueType>
  bool SetUniform (const Handle(OpenGl_Context)& theCtx,
                   const GLchar* theName,
                   const ValueType& theValue)
  {
    return SetUniform (theCtx, GetUniformLocation (theCtx, theName), theValue);
  }

  //! Specifies the value of the integer uniform variable.
  //! Wrapper for glUniform1i()
  Standard_EXPORT Standard_Boolean SetUniform (const Handle(OpenGl_Context)& theCtx,
                                               GLint                         theLocation,
                                               GLint                         theValue);

  //! Specifies the value of the integer uniform 2D vector.
  //! Wrapper for glUniform2iv()
  Standard_EXPORT Standard_Boolean SetUniform (const Handle(OpenGl_Context)& theCtx,
                                               GLint                         theLocation,
                                               const OpenGl_Vec2i&           theValue);

  //! Specifies the value of the integer uniform 3D vector.
  //! Wrapper for glUniform3iv()
  Standard_EXPORT Standard_Boolean SetUniform (const Handle(OpenGl_Context)& theCtx,
                                               GLint                         theLocation,
                                               const OpenGl_Vec3i&           theValue);

  //! Specifies the value of the integer uniform 4D vector.
  //! Wrapper for glUniform4iv()
  Standard_EXPORT Standard_Boolean SetUniform (const Handle(OpenGl_Context)& theCtx,
                                               GLint                         theLocation,
                                               const OpenGl_Vec4i&           theValue);

public:

  //! Specifies the value of the unsigned integer uniform 2D vector (uvec2).
  //! Wrapper for glUniform2uiv()
  Standard_EXPORT Standard_Boolean SetUniform (const Handle(OpenGl_Context)& theCtx,
                                               GLint                         theLocation,
                                               const OpenGl_Vec2u&           theValue);

  //! Specifies the value of the uvec2 uniform array
  //! Wrapper for glUniform2uiv()
  Standard_EXPORT Standard_Boolean SetUniform (const Handle(OpenGl_Context)& theCtx,
                                               const GLchar*                 theName,
                                               const GLsizei                 theCount,
                                               const OpenGl_Vec2u*           theValue);

  //! Specifies the value of the uvec2 uniform array
  //! Wrapper for glUniform2uiv()
  Standard_EXPORT Standard_Boolean SetUniform (const Handle(OpenGl_Context)& theCtx,
                                               GLint                         theLocation,
                                               const GLsizei                 theCount,
                                               const OpenGl_Vec2u*           theValue);

public:

  //! Specifies the value of the float uniform variable.
  //! Wrapper for glUniform1f()
  Standard_EXPORT Standard_Boolean SetUniform (const Handle(OpenGl_Context)& theCtx,
                                               GLint                         theLocation,
                                               GLfloat                       theValue);

  //! Specifies the value of the float uniform 2D vector.
  //! Wrapper for glUniform2fv()
  Standard_EXPORT Standard_Boolean SetUniform (const Handle(OpenGl_Context)& theCtx,
                                               GLint                         theLocation,
                                               const OpenGl_Vec2&            theValue);

  //! Specifies the value of the float uniform 3D vector.
  //! Wrapper for glUniform3fv()
  Standard_EXPORT Standard_Boolean SetUniform (const Handle(OpenGl_Context)& theCtx,
                                               GLint                         theLocation,
                                               const OpenGl_Vec3&            theValue);

  //! Specifies the value of the float uniform 4D vector.
  //! Wrapper for glUniform4fv()
  Standard_EXPORT Standard_Boolean SetUniform (const Handle(OpenGl_Context)& theCtx,
                                               GLint                         theLocation,
                                               const OpenGl_Vec4&            theValue);

public:

  //! Specifies the value of the array of float uniform 3x3 matrices.
  //! Wrapper over glUniformMatrix3fv().
  Standard_EXPORT Standard_Boolean SetUniform (const Handle(OpenGl_Context)&  theCtx,
                                               GLint                          theLocation,
                                               GLuint                         theCount,
                                               const NCollection_Mat3<float>* theData);

  //! Specifies the value of the float uniform 4x4 matrix.
  //! Wrapper for glUniformMatrix4fv()
  bool SetUniform (const Handle(OpenGl_Context)& theCtx,
                   const GLchar*                 theName,
                   const OpenGl_Mat4&            theValue,
                   GLboolean                     theTranspose = GL_FALSE)
  {
    return SetUniform (theCtx, GetUniformLocation (theCtx, theName), theValue, theTranspose);
  }

  //! Specifies the value of the float uniform 4x4 matrix.
  //! Wrapper for glUniformMatrix4fv()
  Standard_EXPORT Standard_Boolean SetUniform (const Handle(OpenGl_Context)& theCtx,
                                               GLint                         theLocation,
                                               const OpenGl_Mat4&            theValue,
                                               GLboolean                     theTranspose = GL_FALSE);

  //! Specifies the value of the array of float uniform 4x4 matrices.
  //! Wrapper over glUniformMatrix4fv().
  Standard_EXPORT Standard_Boolean SetUniform (const Handle(OpenGl_Context)& theCtx,
                                               GLint                         theLocation,
                                               GLuint                        theCount,
                                               const OpenGl_Mat4*            theData);

  //! Specifies the value of the float uniform array
  //! Wrapper over glUniform1fv()
  Standard_EXPORT Standard_Boolean SetUniform (const Handle(OpenGl_Context)& theCtx,
                                               GLint                         theLocation,
                                               GLuint                        theCount,
                                               const Standard_ShortReal*     theData);

  //! Specifies the value of the float2 uniform array
  //! Wrapper over glUniform2fv()
  Standard_EXPORT Standard_Boolean SetUniform (const Handle(OpenGl_Context)& theCtx,
                                               GLint                         theLocation,
                                               GLuint                        theCount,
                                               const OpenGl_Vec2*            theData);

  //! Specifies the value of the float3 uniform array
  //! Wrapper over glUniform3fv()
  Standard_EXPORT Standard_Boolean SetUniform (const Handle(OpenGl_Context)& theCtx,
                                               GLint                         theLocation,
                                               GLuint                        theCount,
                                               const OpenGl_Vec3*            theData);

  //! Specifies the value of the float4 uniform array
  //! Wrapper over glUniform4fv()
  Standard_EXPORT Standard_Boolean SetUniform (const Handle(OpenGl_Context)& theCtx,
                                               GLint                         theLocation,
                                               GLuint                        theCount,
                                               const OpenGl_Vec4*            theData);

  //! Specifies the value of the integer uniform array
  //! Wrapper over glUniform1iv()
  Standard_EXPORT Standard_Boolean SetUniform (const Handle(OpenGl_Context)& theCtx,
                                               GLint                         theLocation,
                                               GLuint                        theCount,
                                               const Standard_Integer*       theData);

  //! Specifies the value of the int2 uniform array
  //! Wrapper over glUniform2iv()
  Standard_EXPORT Standard_Boolean SetUniform (const Handle(OpenGl_Context)& theCtx,
                                               GLint                         theLocation,
                                               GLuint                        theCount,
                                               const OpenGl_Vec2i*           theData);

  //! Specifies the value of the int3 uniform array
  //! Wrapper over glUniform3iv()
  Standard_EXPORT Standard_Boolean SetUniform (const Handle(OpenGl_Context)& theCtx,
                                               GLint                         theLocation,
                                               GLuint                        theCount,
                                               const OpenGl_Vec3i*           theData);

  //! Specifies the value of the int4 uniform array
  //! Wrapper over glUniform4iv()
  Standard_EXPORT Standard_Boolean SetUniform (const Handle(OpenGl_Context)& theCtx,
                                               GLint                         theLocation,
                                               GLuint                        theCount,
                                               const OpenGl_Vec4i*           theData);

public:

  //! Specifies the value of the sampler uniform variable.
  bool SetSampler (const Handle(OpenGl_Context)& theCtx,
                   const GLchar*                 theName,
                   const Graphic3d_TextureUnit   theTextureUnit)
  {
    return SetSampler (theCtx, GetUniformLocation (theCtx, theName), theTextureUnit);
  }

  //! Specifies the value of the sampler uniform variable.
  Standard_EXPORT Standard_Boolean SetSampler (const Handle(OpenGl_Context)& theCtx,
                                               GLint                         theLocation,
                                               const Graphic3d_TextureUnit   theTextureUnit);

public:

  //! Update the shader program from external files (per shader stage) in the following way:
  //! 1) If external file does not exist, then it will be created (current source code will be dumped, no recompilation) and FALSE will be returned.
  //! 2) If external file exists and it has the same timestamp as   myDumpDate, nothing will be done and FALSE will be returned.
  //! 3) If external file exists and it has    newer timestamp than myDumpDate, shader  will be recompiled and relinked and TRUE will be returned.
  //! @param theCtx OpenGL context bound to this working thread
  //! @param theFolder folder to store files; when unspecified, $CSF_ShadersDirectoryDump or current folder will be used instead
  //! @param theToBeautify flag improving formatting (add extra newlines)
  //! @param theToReset when TRUE, existing dumps will be overridden
  Standard_EXPORT Standard_Boolean UpdateDebugDump (const Handle(OpenGl_Context)& theCtx,
                                                    const TCollection_AsciiString& theFolder = "",
                                                    Standard_Boolean theToBeautify = Standard_False,
                                                    Standard_Boolean theToReset = Standard_False);

protected:

  //! Increments counter of users.
  //! Used by OpenGl_ShaderManager.
  //! @return true when resource has been restored from delayed release queue
  bool Share()
  {
    return ++myShareCount == 1;
  }

  //! Decrements counter of users.
  //! Used by OpenGl_ShaderManager.
  //! @return true when there are no more users of this program has been left
  bool UnShare()
  {
    return --myShareCount == 0;
  }

  //! Links the program object.
  Standard_EXPORT Standard_Boolean link (const Handle(OpenGl_Context)& theCtx);

protected:

  GLuint                          myProgramID;     //!< Handle of OpenGL shader program
  OpenGl_ShaderList               myShaderObjects; //!< List of attached shader objects
  Handle(Graphic3d_ShaderProgram) myProxy;         //!< Proxy shader program (from application layer)
  Standard_Integer                myShareCount;    //!< program users count, initialized with 1 (already shared by one user)
  Standard_Integer                myNbLightsMax;   //!< length of array of light sources (THE_MAX_LIGHTS)
  Standard_Integer                myNbShadowMaps;  //!< length of array of shadow maps (THE_NB_SHADOWMAPS)
  Standard_Integer                myNbClipPlanesMax; //!< length of array of clipping planes (THE_MAX_CLIP_PLANES)
  Standard_Integer                myNbFragOutputs; //!< length of array of Fragment Shader outputs (THE_NB_FRAG_OUTPUTS)
  Standard_Integer                myTextureSetBits;//!< texture units declared within the program, @sa Graphic3d_TextureSetBits
  Graphic3d_RenderTransparentMethod myOitOutput;   //!< flag indicating that Fragment Shader includes OIT outputs
  Standard_Boolean                myHasAlphaTest;  //!< flag indicating that Fragment Shader should perform alpha-test
  Standard_Boolean                myHasTessShader; //!< flag indicating that program defines tessellation stage

protected:

  Standard_Size myCurrentState[OpenGl_UniformStateType_NB]; //!< defines last modification for variables of each state type

  //! Stores locations of OCCT state uniform variables.
  OpenGl_ShaderUniformLocation myStateLocations[OpenGl_OCCT_NUMBER_OF_STATE_VARIABLES];

};

template<class T>
struct OpenGl_VariableSetter : public OpenGl_SetterInterface
{
  virtual void Set (const Handle(OpenGl_Context)&           theCtx,
                    const Handle(Graphic3d_ShaderVariable)& theVariable,
                    OpenGl_ShaderProgram*                   theProgram)
  {
    theProgram->SetUniform (theCtx,
                            theVariable->Name().ToCString(),
                            theVariable->Value()->As<T>());
  }
};

namespace OpenGl_HashMapInitializer
{
  template<class K, class V>
  struct MapListOfType
  {
    NCollection_DataMap<K, V> myDictionary;

    MapListOfType (K theKey, V theValue)
    {
      myDictionary.Bind (theKey, theValue);
    }

    MapListOfType& operator() (K theKey, V theValue)
    {
      myDictionary.Bind (theKey, theValue);
      return *this;
    }

    operator const NCollection_DataMap<K, V>& () const
    {
      return myDictionary;
    }
  };

  template<class K, class V>
  MapListOfType<K, V> CreateListOf (K theKey, V theValue)
  {
    return MapListOfType<K, V> (theKey, theValue);
  }
}

#endif // _OpenGl_ShaderProgram_Header
