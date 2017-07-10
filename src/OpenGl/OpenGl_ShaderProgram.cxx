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

#include <OSD_File.hxx>
#include <OSD_Protection.hxx>

#include <Graphic3d_Buffer.hxx>
#include <Standard_Assert.hxx>
#include <Standard_Atomic.hxx>
#include <TCollection_ExtendedString.hxx>

#include <OpenGl_Context.hxx>
#include <OpenGl_ShaderProgram.hxx>
#include <OpenGl_ShaderManager.hxx>
#include <OpenGl_ArbTexBindless.hxx>

#include <OpenGl_GlCore32.hxx>

#include "../Shaders/Shaders_DeclarationsImpl_glsl.pxx"
#include "../Shaders/Shaders_Declarations_glsl.pxx"

#ifdef _WIN32
  #include <malloc.h> // for alloca()
#endif

IMPLEMENT_STANDARD_RTTIEXT(OpenGl_ShaderProgram, OpenGl_NamedResource)

OpenGl_VariableSetterSelector OpenGl_ShaderProgram::mySetterSelector = OpenGl_VariableSetterSelector();

// Declare OCCT-specific OpenGL/GLSL shader variables
Standard_CString OpenGl_ShaderProgram::PredefinedKeywords[] =
{
  "occModelWorldMatrix",                 // OpenGl_OCC_MODEL_WORLD_MATRIX
  "occWorldViewMatrix",                  // OpenGl_OCC_WORLD_VIEW_MATRIX
  "occProjectionMatrix",                 // OpenGl_OCC_PROJECTION_MATRIX
  "occModelWorldMatrixInverse",          // OpenGl_OCC_MODEL_WORLD_MATRIX_INVERSE
  "occWorldViewMatrixInverse",           // OpenGl_OCC_WORLD_VIEW_MATRIX_INVERSE
  "occProjectionMatrixInverse",          // OpenGl_OCC_PROJECTION_MATRIX_INVERSE
  "occModelWorldMatrixTranspose",        // OpenGl_OCC_MODEL_WORLD_MATRIX_TRANSPOSE
  "occWorldViewMatrixTranspose",         // OpenGl_OCC_WORLD_VIEW_MATRIX_TRANSPOSE
  "occProjectionMatrixTranspose",        // OpenGl_OCC_PROJECTION_MATRIX_TRANSPOSE
  "occModelWorldMatrixInverseTranspose", // OpenGl_OCC_MODEL_WORLD_MATRIX_INVERSE_TRANSPOSE
  "occWorldViewMatrixInverseTranspose",  // OpenGl_OCC_WORLD_VIEW_MATRIX_INVERSE_TRANSPOSE
  "occProjectionMatrixInverseTranspose", // OpenGl_OCC_PROJECTION_MATRIX_INVERSE_TRANSPOSE

  "occClipPlaneEquations", // OpenGl_OCC_CLIP_PLANE_EQUATIONS
  "occClipPlaneCount",     // OpenGl_OCC_CLIP_PLANE_COUNT

  "occLightSourcesCount",  // OpenGl_OCC_LIGHT_SOURCE_COUNT
  "occLightSourcesTypes",  // OpenGl_OCC_LIGHT_SOURCE_TYPES
  "occLightSources",       // OpenGl_OCC_LIGHT_SOURCE_PARAMS
  "occLightAmbient",       // OpenGl_OCC_LIGHT_AMBIENT

  "occTextureEnable",      // OpenGl_OCCT_TEXTURE_ENABLE
  "occDistinguishingMode", // OpenGl_OCCT_DISTINGUISH_MODE
  "occFrontMaterial",      // OpenGl_OCCT_FRONT_MATERIAL
  "occBackMaterial",       // OpenGl_OCCT_BACK_MATERIAL
  "occColor",              // OpenGl_OCCT_COLOR

  "occOitOutput",          // OpenGl_OCCT_OIT_OUTPUT
  "occOitDepthFactor",     // OpenGl_OCCT_OIT_DEPTH_FACTOR

  "occTexTrsf2d",          // OpenGl_OCCT_TEXTURE_TRSF2D
  "occPointSize"           // OpenGl_OCCT_POINT_SIZE
};

// =======================================================================
// function : OpenGl_VariableSetterSelector
// purpose  : Creates new variable setter selector
// =======================================================================
OpenGl_VariableSetterSelector::OpenGl_VariableSetterSelector()
{
  // Note: Add new variable setters here
  mySetterList = OpenGl_HashMapInitializer::CreateListOf<size_t, OpenGl_SetterInterface*>
    (Graphic3d_UniformValueTypeID<int>::ID,          new OpenGl_VariableSetter<int>())
    (Graphic3d_UniformValueTypeID<float>::ID,        new OpenGl_VariableSetter<float>())
    (Graphic3d_UniformValueTypeID<OpenGl_Vec2>::ID,  new OpenGl_VariableSetter<OpenGl_Vec2>())
    (Graphic3d_UniformValueTypeID<OpenGl_Vec3>::ID,  new OpenGl_VariableSetter<OpenGl_Vec3>())
    (Graphic3d_UniformValueTypeID<OpenGl_Vec4>::ID,  new OpenGl_VariableSetter<OpenGl_Vec4>())
    (Graphic3d_UniformValueTypeID<OpenGl_Vec2i>::ID, new OpenGl_VariableSetter<OpenGl_Vec2i>())
    (Graphic3d_UniformValueTypeID<OpenGl_Vec3i>::ID, new OpenGl_VariableSetter<OpenGl_Vec3i>())
    (Graphic3d_UniformValueTypeID<OpenGl_Vec4i>::ID, new OpenGl_VariableSetter<OpenGl_Vec4i>());
}

// =======================================================================
// function : ~OpenGl_VariableSetterSelector
// purpose  : Releases memory resources of variable setter selector
// =======================================================================
OpenGl_VariableSetterSelector::~OpenGl_VariableSetterSelector()
{
  for (OpenGl_SetterList::Iterator anIt (mySetterList); anIt.More(); anIt.Next())
  {
    delete anIt.Value();
  }

  mySetterList.Clear();
}

// =======================================================================
// function : Set
// purpose  : Sets generic variable to specified shader program
// =======================================================================
void OpenGl_VariableSetterSelector::Set (const Handle(OpenGl_Context)&           theCtx,
                                         const Handle(Graphic3d_ShaderVariable)& theVariable,
                                         OpenGl_ShaderProgram*                   theProgram) const
{
  Standard_ASSERT_RETURN (mySetterList.IsBound (theVariable->Value()->TypeID()),
    "The type of user-defined uniform variable is not supported...", );

  mySetterList.Find (theVariable->Value()->TypeID())->Set (theCtx, theVariable, theProgram);
}

// =======================================================================
// function : OpenGl_ShaderProgram
// purpose  : Creates uninitialized shader program
// =======================================================================
OpenGl_ShaderProgram::OpenGl_ShaderProgram (const Handle(Graphic3d_ShaderProgram)& theProxy)
: OpenGl_NamedResource (!theProxy.IsNull() ? theProxy->GetId() : ""),
  myProgramID (NO_PROGRAM),
  myProxy     (theProxy),
  myShareCount(1)
{
  memset (myCurrentState, 0, sizeof (myCurrentState));
  for (GLint aVar = 0; aVar < OpenGl_OCCT_NUMBER_OF_STATE_VARIABLES; ++aVar)
  {
    myStateLocations[aVar] = INVALID_LOCATION;
  }
}

// =======================================================================
// function : Initialize
// purpose  : Initializes program object with the list of shader objects
// =======================================================================
Standard_Boolean OpenGl_ShaderProgram::Initialize (const Handle(OpenGl_Context)&     theCtx,
                                                   const Graphic3d_ShaderObjectList& theShaders)
{
  if (theCtx.IsNull() || !Create (theCtx))
  {
    return Standard_False;
  }

  TCollection_AsciiString aHeader = !myProxy.IsNull() && !myProxy->Header().IsEmpty()
                                  ? (myProxy->Header() + "\n")
                                  : TCollection_AsciiString();

  TCollection_AsciiString aDeclarations = Shaders_Declarations_glsl;
  TCollection_AsciiString aDeclImpl = Shaders_DeclarationsImpl_glsl;

  aDeclarations += aDeclImpl;

  for (Graphic3d_ShaderObjectList::Iterator anIter (theShaders);
       anIter.More(); anIter.Next())
  {
    if (!anIter.Value()->IsDone())
    {
      const TCollection_ExtendedString aMsg = "Error! Failed to get shader source";
      theCtx->PushMessage (GL_DEBUG_SOURCE_APPLICATION,
                           GL_DEBUG_TYPE_ERROR,
                           0,
                           GL_DEBUG_SEVERITY_HIGH,
                           aMsg);
      return Standard_False;
    }

    Handle(OpenGl_ShaderObject) aShader;

    // Note: Add support of other shader types here
    switch (anIter.Value()->Type())
    {
      case Graphic3d_TOS_VERTEX:
        aShader = new OpenGl_ShaderObject (GL_VERTEX_SHADER);
        break;
      case Graphic3d_TOS_FRAGMENT:
        aShader = new OpenGl_ShaderObject (GL_FRAGMENT_SHADER);
        break;
    }

    // Is unsupported shader type?
    if (aShader.IsNull())
    {
      TCollection_ExtendedString aMsg = "Error! Unsupported shader type";
      theCtx->PushMessage (GL_DEBUG_SOURCE_APPLICATION,
                           GL_DEBUG_TYPE_ERROR,
                           0,
                           GL_DEBUG_SEVERITY_HIGH,
                           aMsg);
      return Standard_False;
    }

    if (!aShader->Create (theCtx))
    {
      aShader->Release (theCtx.operator->());
      return Standard_False;
    }

    TCollection_AsciiString aSource = aDeclarations + anIter.Value()->Source();
    TCollection_AsciiString anExtensions = "// Enable extensions used in OCCT GLSL programs\n";
    if (theCtx->hasDrawBuffers)
    {
      anExtensions += "#define OCC_ENABLE_draw_buffers\n";
    }
    if (theCtx->hasDrawBuffers == OpenGl_FeatureInExtensions)
    {
      if (theCtx->arbDrawBuffers)
      {
        anExtensions += "#extension GL_ARB_draw_buffers : enable\n";
      }
      else if (theCtx->extDrawBuffers)
      {
        anExtensions += "#extension GL_EXT_draw_buffers : enable\n";
      }
    }

    if (theCtx->hasSampleVariables == OpenGl_FeatureInExtensions)
    {
#if defined(GL_ES_VERSION_2_0)
      if (theCtx->oesSampleVariables)
      {
        anExtensions += "#extension GL_OES_sample_variables : enable\n";
      }
#else
      if (theCtx->arbSampleShading)
      {
        anExtensions += "#extension GL_ARB_sample_shading : enable\n";
      }
#endif
    }

    switch (anIter.Value()->Type())
    {
      case Graphic3d_TOS_VERTEX:
      {
        aSource = aHeader + TCollection_AsciiString ("#define VERTEX_SHADER\n") + anExtensions + aSource;
        break;
      }
      case Graphic3d_TOS_FRAGMENT:
      {
      #if defined(GL_ES_VERSION_2_0)
        TCollection_AsciiString aPrefix (theCtx->hasHighp
                                       ? "precision highp float;\n"
                                         "precision highp int;\n"
                                       : "precision mediump float;\n"
                                         "precision mediump int;\n");
        aSource = aHeader + anExtensions + aPrefix + aSource;
      #else
        aSource = aHeader + anExtensions + aSource;
      #endif
        break;
      }
    }

    if (!aShader->LoadSource (theCtx, aSource))
    {
      theCtx->PushMessage (GL_DEBUG_SOURCE_APPLICATION, GL_DEBUG_TYPE_ERROR, 0, GL_DEBUG_SEVERITY_HIGH, aSource);
      const TCollection_ExtendedString aMsg = "Error! Failed to set shader source";
      theCtx->PushMessage (GL_DEBUG_SOURCE_APPLICATION,
                           GL_DEBUG_TYPE_ERROR,
                           0,
                           GL_DEBUG_SEVERITY_HIGH,
                           aMsg);
      aShader->Release (theCtx.operator->());
      return Standard_False;
    }

    if (!aShader->Compile (theCtx))
    {
      theCtx->PushMessage (GL_DEBUG_SOURCE_APPLICATION, GL_DEBUG_TYPE_ERROR, 0, GL_DEBUG_SEVERITY_HIGH, aSource);
      TCollection_AsciiString aLog;
      aShader->FetchInfoLog (theCtx, aLog);
      if (aLog.IsEmpty())
      {
        aLog = "Compilation log is empty.";
      }
      theCtx->PushMessage (GL_DEBUG_SOURCE_APPLICATION,
                           GL_DEBUG_TYPE_ERROR,
                           0,
                           GL_DEBUG_SEVERITY_HIGH,
                           TCollection_ExtendedString ("Failed to compile shader object. Compilation log:\n") + aLog);
      aShader->Release (theCtx.operator->());
      return Standard_False;
    }
    else if (theCtx->caps->glslWarnings)
    {
      TCollection_AsciiString aLog;
      aShader->FetchInfoLog (theCtx, aLog);
      if (!aLog.IsEmpty()
       && !aLog.IsEqual ("No errors.\n"))
      {
        theCtx->PushMessage (GL_DEBUG_SOURCE_APPLICATION,
                             GL_DEBUG_TYPE_PORTABILITY,
                             0,
                             GL_DEBUG_SEVERITY_LOW,
                             TCollection_ExtendedString ("Shader compilation log:\n") + aLog);
      }
    }

    if (!AttachShader (theCtx, aShader))
    {
      aShader->Release (theCtx.operator->());
      return Standard_False;
    }
  }

  // bind locations for pre-defined Vertex Attributes
  SetAttributeName (theCtx, Graphic3d_TOA_POS,   "occVertex");
  SetAttributeName (theCtx, Graphic3d_TOA_NORM,  "occNormal");
  SetAttributeName (theCtx, Graphic3d_TOA_UV,    "occTexCoord");
  SetAttributeName (theCtx, Graphic3d_TOA_COLOR, "occVertColor");

  // bind custom Vertex Attributes
  if (!myProxy.IsNull())
  {
    for (Graphic3d_ShaderAttributeList::Iterator anAttribIter (myProxy->VertexAttributes());
         anAttribIter.More(); anAttribIter.Next())
    {
      SetAttributeName (theCtx, anAttribIter.Value()->Location(), anAttribIter.Value()->Name().ToCString());
    }
  }

  if (!Link (theCtx))
  {
    TCollection_AsciiString aLog;
    FetchInfoLog (theCtx, aLog);
    if (aLog.IsEmpty())
    {
      aLog = "Linker log is empty.";
    }
    theCtx->PushMessage (GL_DEBUG_SOURCE_APPLICATION,
                         GL_DEBUG_TYPE_ERROR,
                         0,
                         GL_DEBUG_SEVERITY_HIGH,
                         TCollection_ExtendedString ("Failed to link program object! Linker log:\n") + aLog);
    return Standard_False;
  }
  else if (theCtx->caps->glslWarnings)
  {
    TCollection_AsciiString aLog;
    FetchInfoLog (theCtx, aLog);
    if (!aLog.IsEmpty()
     && !aLog.IsEqual ("No errors.\n"))
    {
      theCtx->PushMessage (GL_DEBUG_SOURCE_APPLICATION,
                           GL_DEBUG_TYPE_PORTABILITY,
                           0,
                           GL_DEBUG_SEVERITY_LOW,
                           TCollection_ExtendedString ("GLSL linker log:\n") + aLog);
    }
  }

  // set uniform defaults
  const Handle(OpenGl_ShaderProgram)& anOldProgram = theCtx->ActiveProgram();
  theCtx->core20fwd->glUseProgram (myProgramID);
  {
    const GLint aLocTexEnable = GetStateLocation (OpenGl_OCCT_TEXTURE_ENABLE);
    if (aLocTexEnable != INVALID_LOCATION)
    {
      SetUniform (theCtx, aLocTexEnable, 0); // Off
    }
  }
  {
    const GLint aLocSampler = GetUniformLocation (theCtx, "occActiveSampler");
    if (aLocSampler != INVALID_LOCATION)
    {
      SetUniform (theCtx, aLocSampler, GLint(Graphic3d_TextureUnit_0));
    }
  }

  const TCollection_AsciiString aSamplerNamePrefix ("occSampler");
  const Standard_Integer aNbUnitsMax = Max (theCtx->MaxCombinedTextureUnits(), Graphic3d_TextureUnit_NB);
  for (GLint aUnitIter = 0; aUnitIter < aNbUnitsMax; ++aUnitIter)
  {
    const TCollection_AsciiString aName = aSamplerNamePrefix + aUnitIter;
    const GLint aLocSampler = GetUniformLocation (theCtx, aName.ToCString());
    if (aLocSampler != INVALID_LOCATION)
    {
      SetUniform (theCtx, aLocSampler, aUnitIter);
    }
  }

  theCtx->core20fwd->glUseProgram (!anOldProgram.IsNull() ? anOldProgram->ProgramId() : OpenGl_ShaderProgram::NO_PROGRAM);
  return Standard_True;
}

// =======================================================================
// function : ~OpenGl_ShaderProgram
// purpose  : Releases resources of shader program
// =======================================================================
OpenGl_ShaderProgram::~OpenGl_ShaderProgram()
{
  Release (NULL);
}

// =======================================================================
// function : AttachShader
// purpose  : Attaches shader object to the program object
// =======================================================================
Standard_Boolean OpenGl_ShaderProgram::AttachShader (const Handle(OpenGl_Context)&      theCtx,
                                                     const Handle(OpenGl_ShaderObject)& theShader)
{
  if (myProgramID == NO_PROGRAM || theShader.IsNull())
  {
    return Standard_False;
  }

  for (OpenGl_ShaderList::Iterator anIter (myShaderObjects); anIter.More(); anIter.Next())
  {
    if (theShader == anIter.Value())
    {
      return Standard_False;
    }
  }

  myShaderObjects.Append (theShader);
  theCtx->core20fwd->glAttachShader (myProgramID, theShader->myShaderID);
  return Standard_True;
}

// =======================================================================
// function : DetachShader
// purpose  : Detaches shader object to the program object
// =======================================================================
Standard_Boolean OpenGl_ShaderProgram::DetachShader (const Handle(OpenGl_Context)&      theCtx,
                                                     const Handle(OpenGl_ShaderObject)& theShader)
{
  if (myProgramID == NO_PROGRAM
   || theShader.IsNull())
  {
    return Standard_False;
  }

  OpenGl_ShaderList::Iterator anIter (myShaderObjects);
  while (anIter.More())
  {
    if (theShader == anIter.Value())
    {
      myShaderObjects.Remove (anIter);
      break;
    }

    anIter.Next();
  }

  if (!anIter.More())
  {
    return Standard_False;
  }

  theCtx->core20fwd->glDetachShader (myProgramID, theShader->myShaderID);
  return Standard_True;
}

// =======================================================================
// function : Link
// purpose  : Links the program object
// =======================================================================
Standard_Boolean OpenGl_ShaderProgram::Link (const Handle(OpenGl_Context)& theCtx)
{
  if (myProgramID == NO_PROGRAM)
  {
    return Standard_False;
  }

  GLint aStatus = GL_FALSE;
  theCtx->core20fwd->glLinkProgram (myProgramID);
  theCtx->core20fwd->glGetProgramiv (myProgramID, GL_LINK_STATUS, &aStatus);
  if (aStatus == GL_FALSE)
  {
    return Standard_False;
  }

  for (GLint aVar = 0; aVar < OpenGl_OCCT_NUMBER_OF_STATE_VARIABLES; ++aVar)
  {
    myStateLocations[aVar] = GetUniformLocation (theCtx, PredefinedKeywords[aVar]);
  }
  return Standard_True;
}

// =======================================================================
// function : FetchInfoLog
// purpose  : Fetches information log of the last link operation
// =======================================================================
Standard_Boolean OpenGl_ShaderProgram::FetchInfoLog (const Handle(OpenGl_Context)& theCtx,
                                                     TCollection_AsciiString&      theOutput)
{
  if (myProgramID == NO_PROGRAM)
  {
    return Standard_False;
  }

  GLint aLength = 0;
  theCtx->core20fwd->glGetProgramiv (myProgramID, GL_INFO_LOG_LENGTH, &aLength);
  if (aLength > 0)
  {
    GLchar* aLog = (GLchar*) alloca (aLength);
    memset (aLog, 0, aLength);
    theCtx->core20fwd->glGetProgramInfoLog (myProgramID, aLength, NULL, aLog);
    theOutput = aLog;
  }
  return Standard_True;
}

// =======================================================================
// function : ApplyVariables
// purpose  : Fetches uniform variables from proxy shader program
// =======================================================================
Standard_Boolean OpenGl_ShaderProgram::ApplyVariables(const Handle(OpenGl_Context)& theCtx)
{
  if (myProxy.IsNull() || myProxy->Variables().IsEmpty())
  {
    return Standard_False;
  }

  for (Graphic3d_ShaderVariableList::Iterator anIter (myProxy->Variables()); anIter.More(); anIter.Next())
  {
    mySetterSelector.Set (theCtx, anIter.Value(), this);
  }

  myProxy->ClearVariables();
  return Standard_True;
}

// =======================================================================
// function : GetUniformLocation
// purpose  : Returns location (index) of the specific uniform variable
// =======================================================================
GLint OpenGl_ShaderProgram::GetUniformLocation (const Handle(OpenGl_Context)& theCtx,
                                                const GLchar*                 theName) const
{
  return myProgramID != NO_PROGRAM
       ? theCtx->core20fwd->glGetUniformLocation (myProgramID, theName)
       : INVALID_LOCATION;
}

// =======================================================================
// function : GetAttributeLocation
// purpose  : Returns location (index) of the generic vertex attribute
// =======================================================================
GLint OpenGl_ShaderProgram::GetAttributeLocation (const Handle(OpenGl_Context)& theCtx,
                                                  const GLchar*                 theName) const
{
  return myProgramID != NO_PROGRAM
       ? theCtx->core20fwd->glGetAttribLocation (myProgramID, theName)
       : INVALID_LOCATION;
}

// =======================================================================
// function : GetStateLocation
// purpose  : Returns location of the OCCT state uniform variable
// =======================================================================
GLint OpenGl_ShaderProgram::GetStateLocation (const GLuint theVariable) const
{
  if (theVariable < OpenGl_OCCT_NUMBER_OF_STATE_VARIABLES)
  {
    return myStateLocations[theVariable];
  }
  return INVALID_LOCATION;
}

// =======================================================================
// function : GetUniform
// purpose  : Returns the value of the integer uniform variable
// =======================================================================
Standard_Boolean OpenGl_ShaderProgram::GetUniform (const Handle(OpenGl_Context)& theCtx,
                                                   const GLchar*                 theName,
                                                   OpenGl_Vec4i&                 theValue) const
{
  return GetUniform (theCtx, GetUniformLocation (theCtx, theName), theValue);
}

// =======================================================================
// function : GetUniform
// purpose  : Returns the value of the integer uniform variable
// =======================================================================
Standard_Boolean OpenGl_ShaderProgram::GetUniform (const Handle(OpenGl_Context)& theCtx,
                                                   GLint                         theLocation,
                                                   OpenGl_Vec4i&                 theValue) const
{
  if (myProgramID == NO_PROGRAM || theLocation == INVALID_LOCATION)
  {
    return Standard_False;
  }

  theCtx->core20fwd->glGetUniformiv (myProgramID, theLocation, theValue);
  return Standard_True;
}

// =======================================================================
// function : GetUniform
// purpose  : Returns the value of the floating-point uniform variable
// =======================================================================
Standard_Boolean OpenGl_ShaderProgram::GetUniform (const Handle(OpenGl_Context)& theCtx,
                                                   const GLchar*                 theName,
                                                   OpenGl_Vec4&                  theValue) const
{
  return GetUniform (theCtx, GetUniformLocation (theCtx, theName), theValue);
}

// =======================================================================
// function : GetUniform
// purpose  : Returns the value of the floating-point uniform variable
// =======================================================================
Standard_Boolean OpenGl_ShaderProgram::GetUniform (const Handle(OpenGl_Context)& theCtx,
                                                   GLint                         theLocation,
                                                   OpenGl_Vec4&                  theValue) const
{
  if (myProgramID == NO_PROGRAM || theLocation == INVALID_LOCATION)
  {
    return Standard_False;
  }

  theCtx->core20fwd->glGetUniformfv (myProgramID, theLocation, theValue);
  return Standard_True;
}

// =======================================================================
// function : GetAttribute
// purpose  : Returns the integer vertex attribute
// =======================================================================
Standard_Boolean OpenGl_ShaderProgram::GetAttribute (const Handle(OpenGl_Context)& theCtx,
                                                     const GLchar*                 theName,
                                                     OpenGl_Vec4i&                 theValue) const
{
  return GetAttribute (theCtx, GetAttributeLocation (theCtx, theName), theValue);
}

// =======================================================================
// function : GetAttribute
// purpose  : Returns the integer vertex attribute
// =======================================================================
Standard_Boolean OpenGl_ShaderProgram::GetAttribute (const Handle(OpenGl_Context)& theCtx,
                                                     GLint                         theIndex,
                                                     OpenGl_Vec4i&                 theValue) const
{
  if (myProgramID == NO_PROGRAM || theIndex == INVALID_LOCATION)
  {
    return Standard_False;
  }

  theCtx->core20fwd->glGetVertexAttribiv (theIndex, GL_CURRENT_VERTEX_ATTRIB, theValue);
  return Standard_True;
}

// =======================================================================
// function : GetAttribute
// purpose  : Returns the floating-point vertex attribute
// =======================================================================
Standard_Boolean OpenGl_ShaderProgram::GetAttribute (const Handle(OpenGl_Context)& theCtx,
                                                     const GLchar*                 theName,
                                                     OpenGl_Vec4&                  theValue) const
{
  return GetAttribute (theCtx, GetAttributeLocation (theCtx, theName), theValue);
}

// =======================================================================
// function : GetAttribute
// purpose  : Returns the floating-point vertex attribute
// =======================================================================
Standard_Boolean OpenGl_ShaderProgram::GetAttribute (const Handle(OpenGl_Context)& theCtx,
                                                     GLint                         theIndex,
                                                     OpenGl_Vec4&                  theValue) const
{
  if (myProgramID == NO_PROGRAM || theIndex == INVALID_LOCATION)
  {
    return Standard_False;
  }

  theCtx->core20fwd->glGetVertexAttribfv (theIndex, GL_CURRENT_VERTEX_ATTRIB, theValue);
  return Standard_True;
}

// =======================================================================
// function : SetAttributeName
// purpose  :
// =======================================================================
Standard_Boolean OpenGl_ShaderProgram::SetAttributeName (const Handle(OpenGl_Context)& theCtx,
                                                         GLint                         theIndex,
                                                         const GLchar*                 theName)
{
  theCtx->core20fwd->glBindAttribLocation (myProgramID, theIndex, theName);
  return Standard_True;
}

// =======================================================================
// function : SetAttribute
// purpose  :
// =======================================================================
Standard_Boolean OpenGl_ShaderProgram::SetAttribute (const Handle(OpenGl_Context)& theCtx,
                                                     const GLchar*                 theName,
                                                     GLfloat                       theValue)
{
  return SetAttribute (theCtx, GetAttributeLocation (theCtx, theName), theValue);
}

// =======================================================================
// function : SetAttribute
// purpose  :
// =======================================================================
Standard_Boolean OpenGl_ShaderProgram::SetAttribute (const Handle(OpenGl_Context)& theCtx,
                                                     GLint                         theIndex,
                                                     GLfloat                       theValue)
{
  if (myProgramID == NO_PROGRAM || theIndex == INVALID_LOCATION)
  {
    return Standard_False;
  }

  theCtx->core20fwd->glVertexAttrib1f (theIndex, theValue);
  return Standard_True;
}

// =======================================================================
// function : SetAttribute
// purpose  :
// =======================================================================
Standard_Boolean OpenGl_ShaderProgram::SetAttribute (const Handle(OpenGl_Context)& theCtx,
                                                     const GLchar*                 theName,
                                                     const OpenGl_Vec2&            theValue)
{
  return SetAttribute (theCtx, GetAttributeLocation (theCtx, theName), theValue);
}

// =======================================================================
// function : SetAttribute
// purpose  :
// =======================================================================
Standard_Boolean OpenGl_ShaderProgram::SetAttribute (const Handle(OpenGl_Context)& theCtx,
                                                     GLint                         theIndex,
                                                     const OpenGl_Vec2&            theValue)
{
  if (myProgramID == NO_PROGRAM || theIndex == INVALID_LOCATION)
  {
    return Standard_False;
  }

  theCtx->core20fwd->glVertexAttrib2fv (theIndex, theValue);
  return Standard_True;
}

// =======================================================================
// function : SetAttribute
// purpose  :
// =======================================================================
Standard_Boolean OpenGl_ShaderProgram::SetAttribute (const Handle(OpenGl_Context)& theCtx,
                                                     const GLchar*                 theName,
                                                     const OpenGl_Vec3&            theValue)
{
  return SetAttribute (theCtx, GetAttributeLocation (theCtx, theName), theValue);
}

// =======================================================================
// function : SetAttribute
// purpose  :
// =======================================================================
Standard_Boolean OpenGl_ShaderProgram::SetAttribute (const Handle(OpenGl_Context)& theCtx,
                                                     GLint                         theIndex,
                                                     const OpenGl_Vec3&            theValue)
{
  if (myProgramID == NO_PROGRAM || theIndex == INVALID_LOCATION)
  {
    return Standard_False;
  }

  theCtx->core20fwd->glVertexAttrib3fv (theIndex, theValue);
  return Standard_True;
}

// =======================================================================
// function : SetAttribute
// purpose  :
// =======================================================================
Standard_Boolean OpenGl_ShaderProgram::SetAttribute (const Handle(OpenGl_Context)& theCtx,
                                                     const GLchar*                 theName,
                                                     const OpenGl_Vec4&            theValue)
{
  return SetAttribute (theCtx, GetAttributeLocation (theCtx, theName), theValue);
}

// =======================================================================
// function : SetAttribute
// purpose  :
// =======================================================================
Standard_Boolean OpenGl_ShaderProgram::SetAttribute (const Handle(OpenGl_Context)& theCtx,
                                                     GLint                         theIndex,
                                                     const OpenGl_Vec4&            theValue)
{
  if (myProgramID == NO_PROGRAM || theIndex == INVALID_LOCATION)
  {
    return Standard_False;
  }

  theCtx->core20fwd->glVertexAttrib4fv (theIndex, theValue);
  return Standard_True;
}

// =======================================================================
// function : SetUniform
// purpose  : Specifies the value of the integer uniform variable
// =======================================================================
Standard_Boolean OpenGl_ShaderProgram::SetUniform (const Handle(OpenGl_Context)& theCtx,
                                                   const GLchar*                 theName,
                                                   GLint                         theValue)
{
  return SetUniform (theCtx, GetUniformLocation (theCtx, theName), theValue);
}

// =======================================================================
// function : SetUniform
// purpose  : Specifies the value of the integer uniform variable
// =======================================================================
Standard_Boolean OpenGl_ShaderProgram::SetUniform (const Handle(OpenGl_Context)& theCtx,
                                                   GLint                         theLocation,
                                                   GLint                         theValue)
{
  if (myProgramID == NO_PROGRAM || theLocation == INVALID_LOCATION)
  {
    return Standard_False;
  }

  theCtx->core20fwd->glUniform1i (theLocation, theValue);
  return Standard_True;
}

// =======================================================================
// function : SetUniform
// purpose  :
// =======================================================================
Standard_Boolean OpenGl_ShaderProgram::SetUniform (const Handle(OpenGl_Context)& theCtx,
                                                   const GLchar*                 theName,
                                                   const OpenGl_Vec2u&           theValue)
{
  return SetUniform (theCtx, GetUniformLocation (theCtx, theName), theValue);
}

// =======================================================================
// function : SetUniform
// purpose  :
// =======================================================================
Standard_Boolean OpenGl_ShaderProgram::SetUniform (const Handle(OpenGl_Context)& theCtx,
                                                   GLint                         theLocation,
                                                   const OpenGl_Vec2u&           theValue)
{
  if (theCtx->core32 == NULL || myProgramID == NO_PROGRAM || theLocation == INVALID_LOCATION)
  {
    return Standard_False;
  }

#if !defined(GL_ES_VERSION_2_0)
  theCtx->core32->glUniform2uiv (theLocation, 1, theValue.GetData());
  return Standard_True;
#else
  (void )theValue;
  return Standard_False;
#endif
}

// =======================================================================
// function : SetUniform
// purpose  :
// =======================================================================
Standard_Boolean OpenGl_ShaderProgram::SetUniform (const Handle(OpenGl_Context)& theCtx,
                                                   const GLchar*                 theName,
                                                   const GLsizei                 theCount,
                                                   const OpenGl_Vec2u*           theValue)
{
  return SetUniform (theCtx, GetUniformLocation (theCtx, theName), theCount, theValue);
}

// =======================================================================
// function : SetUniform
// purpose  :
// =======================================================================
Standard_Boolean OpenGl_ShaderProgram::SetUniform (const Handle(OpenGl_Context)& theCtx,
                                                   GLint                         theLocation,
                                                   const GLsizei                 theCount,
                                                   const OpenGl_Vec2u*           theValue)
{
  if (theCtx->core32 == NULL || myProgramID == NO_PROGRAM || theLocation == INVALID_LOCATION)
  {
    return Standard_False;
  }

#if !defined(GL_ES_VERSION_2_0)
  theCtx->core32->glUniform2uiv (theLocation, theCount, theValue->GetData());
  return Standard_True;
#else
  (void )theCount;
  (void )theValue;
  return Standard_False;
#endif
}

// =======================================================================
// function : SetUniform
// purpose  : Specifies the value of the floating-point uniform variable
// =======================================================================
Standard_Boolean OpenGl_ShaderProgram::SetUniform (const Handle(OpenGl_Context)& theCtx,
                                                   const GLchar*                 theName,
                                                   GLfloat                       theValue)
{
  return SetUniform (theCtx, GetUniformLocation (theCtx, theName), theValue);
}

// =======================================================================
// function : SetUniform
// purpose  : Specifies the value of the floating-point uniform variable
// =======================================================================
Standard_Boolean OpenGl_ShaderProgram::SetUniform (const Handle(OpenGl_Context)& theCtx,
                                                   GLint                         theLocation,
                                                   GLfloat                       theValue)
{
  if (myProgramID == NO_PROGRAM || theLocation == INVALID_LOCATION)
  {
    return Standard_False;
  }

  theCtx->core20fwd->glUniform1f (theLocation, theValue);
  return Standard_True;
}

// =======================================================================
// function : SetUniform
// purpose  : Specifies the value of the integer uniform 2D vector
// =======================================================================
Standard_Boolean OpenGl_ShaderProgram::SetUniform (const Handle(OpenGl_Context)& theCtx,
                                                   const GLchar*                 theName,
                                                   const OpenGl_Vec2i&           theValue)
{
  return SetUniform (theCtx, GetUniformLocation (theCtx, theName), theValue);
}

// =======================================================================
// function : SetUniform
// purpose  : Specifies the value of the integer uniform 2D vector
// =======================================================================
Standard_Boolean OpenGl_ShaderProgram::SetUniform (const Handle(OpenGl_Context)& theCtx,
                                                   GLint                         theLocation,
                                                   const OpenGl_Vec2i&           theValue)
{
  if (myProgramID == NO_PROGRAM || theLocation == INVALID_LOCATION)
  {
    return Standard_False;
  }

  theCtx->core20fwd->glUniform2iv (theLocation, 1, theValue);
  return Standard_True;
}

// =======================================================================
// function : SetUniform
// purpose  : Specifies the value of the integer uniform 3D vector
// =======================================================================
Standard_Boolean OpenGl_ShaderProgram::SetUniform (const Handle(OpenGl_Context)& theCtx,
                                                   const GLchar*                 theName,
                                                   const OpenGl_Vec3i&           theValue)
{
  return SetUniform (theCtx, GetUniformLocation (theCtx, theName), theValue);
}

// =======================================================================
// function : SetUniform
// purpose  : Specifies the value of the integer uniform 3D vector
// =======================================================================
Standard_Boolean OpenGl_ShaderProgram::SetUniform (const Handle(OpenGl_Context)& theCtx,
                                                   GLint                         theLocation,
                                                   const OpenGl_Vec3i&           theValue)
{
  if (myProgramID == NO_PROGRAM || theLocation == INVALID_LOCATION)
  {
    return Standard_False;
  }

  theCtx->core20fwd->glUniform3iv (theLocation, 1, theValue);
  return Standard_True;
}

// =======================================================================
// function : SetUniform
// purpose  : Specifies the value of the integer uniform 4D vector
// =======================================================================
Standard_Boolean OpenGl_ShaderProgram::SetUniform (const Handle(OpenGl_Context)& theCtx,
                                                   const GLchar*                 theName,
                                                   const OpenGl_Vec4i&           theValue)
{
  return SetUniform (theCtx, GetUniformLocation (theCtx, theName), theValue);
}

// =======================================================================
// function : SetUniform
// purpose  : Specifies the value of the integer uniform 4D vector
// =======================================================================
Standard_Boolean OpenGl_ShaderProgram::SetUniform (const Handle(OpenGl_Context)& theCtx,
                                                   GLint                         theLocation,
                                                   const OpenGl_Vec4i&           theValue)
{
  if (myProgramID == NO_PROGRAM || theLocation == INVALID_LOCATION)
  {
    return Standard_False;
  }

  theCtx->core20fwd->glUniform4iv (theLocation, 1, theValue);
  return Standard_True;
}

// =======================================================================
// function : SetUniform
// purpose  : Specifies the value of the floating-point uniform 2D vector
// =======================================================================
Standard_Boolean OpenGl_ShaderProgram::SetUniform (const Handle(OpenGl_Context)& theCtx,
                                                   const GLchar*                 theName,
                                                   const OpenGl_Vec2&            theValue)
{
  return SetUniform (theCtx, GetUniformLocation (theCtx, theName), theValue);
}

// =======================================================================
// function : SetUniform
// purpose  : Specifies the value of the floating-point uniform 2D vector
// =======================================================================
Standard_Boolean OpenGl_ShaderProgram::SetUniform (const Handle(OpenGl_Context)& theCtx,
                                                   GLint                         theLocation,
                                                   const OpenGl_Vec2&            theValue)
{
  if (myProgramID == NO_PROGRAM || theLocation == INVALID_LOCATION)
  {
    return Standard_False;
  }

  theCtx->core20fwd->glUniform2fv (theLocation, 1, theValue);
  return Standard_True;
}

// =======================================================================
// function : SetUniform
// purpose  : Specifies the value of the floating-point uniform 3D vector
// =======================================================================
Standard_Boolean OpenGl_ShaderProgram::SetUniform (const Handle(OpenGl_Context)& theCtx,
                                                   const GLchar*                 theName,
                                                   const OpenGl_Vec3&            theValue)
{
  return SetUniform (theCtx, GetUniformLocation (theCtx, theName), theValue);
}

// =======================================================================
// function : SetUniform
// purpose  : Specifies the value of the floating-point uniform 3D vector
// =======================================================================
Standard_Boolean OpenGl_ShaderProgram::SetUniform (const Handle(OpenGl_Context)& theCtx,
                                                   GLint                         theLocation,
                                                   const OpenGl_Vec3&            theValue)
{
  if (myProgramID == NO_PROGRAM || theLocation == INVALID_LOCATION)
  {
    return Standard_False;
  }

  theCtx->core20fwd->glUniform3fv (theLocation, 1, theValue);
  return Standard_True;
}

// =======================================================================
// function : SetUniform
// purpose  : Specifies the value of the floating-point uniform 4D vector
// =======================================================================
Standard_Boolean OpenGl_ShaderProgram::SetUniform (const Handle(OpenGl_Context)& theCtx,
                                                   const GLchar*                 theName,
                                                   const OpenGl_Vec4&            theValue)
{
  return SetUniform (theCtx, GetUniformLocation (theCtx, theName), theValue);
}

// =======================================================================
// function : SetUniform
// purpose  : Specifies the value of the floating-point uniform 4D vector
// =======================================================================
Standard_Boolean OpenGl_ShaderProgram::SetUniform (const Handle(OpenGl_Context)& theCtx,
                                                   GLint                         theLocation,
                                                   const OpenGl_Vec4&            theValue)
{
  if (myProgramID == NO_PROGRAM || theLocation == INVALID_LOCATION)
  {
    return Standard_False;
  }

  theCtx->core20fwd->glUniform4fv (theLocation, 1, theValue);
  return Standard_True;
}

// =======================================================================
// function : SetUniform
// purpose  : Specifies the value of the floating-point uniform 4x4 matrix
// =======================================================================
Standard_Boolean OpenGl_ShaderProgram::SetUniform (const Handle(OpenGl_Context)& theCtx,
                                                   const GLchar*                 theName,
                                                   const OpenGl_Mat4&            theValue,
                                                   GLboolean                     theTranspose)
{
  return SetUniform (theCtx, GetUniformLocation (theCtx, theName), theValue, theTranspose);
}

// =======================================================================
// function : SetUniform
// purpose  : Specifies the value of the floating-point uniform 4x4 matrix
// =======================================================================
Standard_Boolean OpenGl_ShaderProgram::SetUniform (const Handle(OpenGl_Context)& theCtx,
                                                   GLint                         theLocation,
                                                   const OpenGl_Mat4&            theValue,
                                                   GLboolean                     theTranspose)
{
  if (myProgramID == NO_PROGRAM || theLocation == INVALID_LOCATION)
  {
    return Standard_False;
  }

  theCtx->core20fwd->glUniformMatrix4fv (theLocation, 1, GL_FALSE, theTranspose ? theValue.Transposed() : theValue);
  return Standard_True;
}

// =======================================================================
// function : SetUniform
// purpose  : Specifies the value of the floating-point uniform 4x4 matrix
// =======================================================================
Standard_Boolean OpenGl_ShaderProgram::SetUniform (const Handle(OpenGl_Context)& theCtx,
                                                   const GLchar*                 theName,
                                                   const OpenGl_Matrix&          theValue,
                                                   GLboolean                     theTranspose)
{
  return SetUniform (theCtx, GetUniformLocation (theCtx, theName), theValue, theTranspose);
}

// =======================================================================
// function : SetUniform
// purpose  : Specifies the value of the floating-point uniform 4x4 matrix
// =======================================================================
Standard_Boolean OpenGl_ShaderProgram::SetUniform (const Handle(OpenGl_Context)& theCtx,
                                                   GLint                         theLocation,
                                                   const OpenGl_Matrix&          theValue,
                                                   GLboolean                     theTranspose)
{
  return SetUniform (theCtx, theLocation, OpenGl_Mat4::Map (*theValue.mat), theTranspose);
}

// =======================================================================
// function : SetUniform
// purpose  : Specifies the value of the float uniform array
// =======================================================================
Standard_Boolean OpenGl_ShaderProgram::SetUniform (const Handle(OpenGl_Context)& theCtx,
                                                   GLint                         theLocation,
                                                   GLuint                        theCount,
                                                   const Standard_ShortReal*     theData)
{
  if (myProgramID == NO_PROGRAM || theLocation == INVALID_LOCATION)
  {
    return Standard_False;
  }

  theCtx->core20fwd->glUniform1fv (theLocation, theCount, theData);
  return Standard_True;
}

// =======================================================================
// function : SetUniform
// purpose  : Specifies the value of the float2 uniform array
// =======================================================================
Standard_Boolean OpenGl_ShaderProgram::SetUniform (const Handle(OpenGl_Context)& theCtx,
                                                   GLint                         theLocation,
                                                   GLuint                        theCount,
                                                   const OpenGl_Vec2*            theData)
{
  if (myProgramID == NO_PROGRAM || theLocation == INVALID_LOCATION)
  {
    return Standard_False;
  }

  theCtx->core20fwd->glUniform2fv (theLocation, theCount, theData[0].GetData());
  return Standard_True;
}

// =======================================================================
// function : SetUniform
// purpose  : Specifies the value of the float3 uniform array
// =======================================================================
Standard_Boolean OpenGl_ShaderProgram::SetUniform (const Handle(OpenGl_Context)& theCtx,
                                                   GLint                         theLocation,
                                                   GLuint                        theCount,
                                                   const OpenGl_Vec3*            theData)
{
  if (myProgramID == NO_PROGRAM || theLocation == INVALID_LOCATION)
  {
    return Standard_False;
  }

  theCtx->core20fwd->glUniform3fv (theLocation, theCount, theData[0].GetData());
  return Standard_True;
}

// =======================================================================
// function : SetUniform
// purpose  : Specifies the value of the float4 uniform array
// =======================================================================
Standard_Boolean OpenGl_ShaderProgram::SetUniform (const Handle(OpenGl_Context)& theCtx,
                                                   GLint                         theLocation,
                                                   GLuint                        theCount,
                                                   const OpenGl_Vec4*            theData)
{
  if (myProgramID == NO_PROGRAM || theLocation == INVALID_LOCATION)
  {
    return Standard_False;
  }

  theCtx->core20fwd->glUniform4fv (theLocation, theCount, theData[0].GetData());
  return Standard_True;
}

// =======================================================================
// function : SetUniform
// purpose  : Specifies the value of the integer uniform array
// =======================================================================
Standard_Boolean OpenGl_ShaderProgram::SetUniform (const Handle(OpenGl_Context)& theCtx,
                                                   GLint                         theLocation,
                                                   GLuint                        theCount,
                                                   const Standard_Integer*       theData)
{
  if (myProgramID == NO_PROGRAM || theLocation == INVALID_LOCATION)
  {
    return Standard_False;
  }

  theCtx->core20fwd->glUniform1iv (theLocation, theCount, theData);
  return Standard_True;
}

// =======================================================================
// function : SetUniform
// purpose  : Specifies the value of the int2 uniform array
// =======================================================================
Standard_Boolean OpenGl_ShaderProgram::SetUniform (const Handle(OpenGl_Context)& theCtx,
                                                   GLint                         theLocation,
                                                   GLuint                        theCount,
                                                   const OpenGl_Vec2i*           theData)
{
  if (myProgramID == NO_PROGRAM || theLocation == INVALID_LOCATION)
  {
    return Standard_False;
  }

  theCtx->core20fwd->glUniform2iv (theLocation, theCount, theData[0].GetData());
  return Standard_True;
}

// =======================================================================
// function : SetUniform
// purpose  : Specifies the value of the int3 uniform array
// =======================================================================
Standard_Boolean OpenGl_ShaderProgram::SetUniform (const Handle(OpenGl_Context)& theCtx,
                                                   GLint                         theLocation,
                                                   GLuint                        theCount,
                                                   const OpenGl_Vec3i*           theData)
{
  if (myProgramID == NO_PROGRAM || theLocation == INVALID_LOCATION)
  {
    return Standard_False;
  }

  theCtx->core20fwd->glUniform3iv (theLocation, theCount, theData[0].GetData());
  return Standard_True;
}

// =======================================================================
// function : SetUniform
// purpose  : Specifies the value of the int4 uniform array
// =======================================================================
Standard_Boolean OpenGl_ShaderProgram::SetUniform (const Handle(OpenGl_Context)& theCtx,
                                                   GLint                         theLocation,
                                                   GLuint                        theCount,
                                                   const OpenGl_Vec4i*           theData)
{
  if (myProgramID == NO_PROGRAM || theLocation == INVALID_LOCATION)
  {
    return Standard_False;
  }

  theCtx->core20fwd->glUniform4iv (theLocation, theCount, theData[0].GetData());
  return Standard_True;
}

// =======================================================================
// function : SetSampler
// purpose  : Specifies the value of the sampler uniform variable
// =======================================================================
Standard_Boolean OpenGl_ShaderProgram::SetSampler (const Handle(OpenGl_Context)& theCtx,
                                                   const GLchar*                 theName,
                                                   const Graphic3d_TextureUnit   theTextureUnit)
{
  return SetSampler (theCtx, GetUniformLocation (theCtx, theName), theTextureUnit);
}

// =======================================================================
// function : SetSampler
// purpose  : Specifies the value of the sampler uniform variable
// =======================================================================
Standard_Boolean OpenGl_ShaderProgram::SetSampler (const Handle(OpenGl_Context)& theCtx,
                                                   GLint                         theLocation,
                                                   const Graphic3d_TextureUnit   theTextureUnit)
{
  if (myProgramID == NO_PROGRAM || theLocation == INVALID_LOCATION)
  {
    return Standard_False;
  }

  theCtx->core20fwd->glUniform1i (theLocation, theTextureUnit);
  return Standard_True;
}

// =======================================================================
// function : Create
// purpose  : Creates new empty shader program of specified type
// =======================================================================
Standard_Boolean OpenGl_ShaderProgram::Create (const Handle(OpenGl_Context)& theCtx)
{
  if (myProgramID == NO_PROGRAM
   && theCtx->core20fwd != NULL)
  {
    myProgramID = theCtx->core20fwd->glCreateProgram();
  }

  return myProgramID != NO_PROGRAM;
}

// =======================================================================
// function : Release
// purpose  : Destroys shader program
// =======================================================================
void OpenGl_ShaderProgram::Release (OpenGl_Context* theCtx)
{
  if (myProgramID == NO_PROGRAM)
  {
    return;
  }

  Standard_ASSERT_RETURN (theCtx != NULL,
    "OpenGl_ShaderProgram destroyed without GL context! Possible GPU memory leakage...",);

  for (OpenGl_ShaderList::Iterator anIter (myShaderObjects); anIter.More(); anIter.Next())
  {
    if (!anIter.Value().IsNull())
    {
      anIter.ChangeValue()->Release (theCtx);
      anIter.ChangeValue().Nullify();
    }
  }

  if (theCtx->core20fwd != NULL
   && theCtx->IsValid())
  {
    theCtx->core20fwd->glDeleteProgram (myProgramID);
  }

  myProgramID = NO_PROGRAM;
}
