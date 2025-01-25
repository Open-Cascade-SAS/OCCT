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
#include <OSD_Environment.hxx>

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

OpenGl_VariableSetterSelector OpenGl_ShaderProgram::mySetterSelector =
  OpenGl_VariableSetterSelector();

// Declare OCCT-specific OpenGL/GLSL shader variables
Standard_CString OpenGl_ShaderProgram::PredefinedKeywords[] = {
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
  "occClipPlaneChains",    // OpenGl_OCC_CLIP_PLANE_CHAINS
  "occClipPlaneCount",     // OpenGl_OCC_CLIP_PLANE_COUNT

  "occLightSourcesCount", // OpenGl_OCC_LIGHT_SOURCE_COUNT
  "occLightSourcesTypes", // OpenGl_OCC_LIGHT_SOURCE_TYPES
  "occLightSources",      // OpenGl_OCC_LIGHT_SOURCE_PARAMS
  "occLightAmbient",      // OpenGl_OCC_LIGHT_AMBIENT
  "occShadowMapSizeBias", // OpenGl_OCC_LIGHT_SHADOWMAP_SIZE_BIAS
  "occShadowMapSamplers", // OpenGl_OCC_LIGHT_SHADOWMAP_SAMPLERS,
  "occShadowMapMatrices", // OpenGl_OCC_LIGHT_SHADOWMAP_MATRICES,

  "occTextureEnable",      // OpenGl_OCCT_TEXTURE_ENABLE
  "occDistinguishingMode", // OpenGl_OCCT_DISTINGUISH_MODE
  "occPbrMaterial",        // OpenGl_OCCT_PBR_MATERIAL
  "occCommonMaterial",     // OpenGl_OCCT_COMMON_MATERIAL
  "occAlphaCutoff",        // OpenGl_OCCT_ALPHA_CUTOFF
  "occColor",              // OpenGl_OCCT_COLOR

  "occOitOutput",      // OpenGl_OCCT_OIT_OUTPUT
  "occOitDepthFactor", // OpenGl_OCCT_OIT_DEPTH_FACTOR

  "occTexTrsf2d", // OpenGl_OCCT_TEXTURE_TRSF2D
  "occPointSize", // OpenGl_OCCT_POINT_SIZE

  "occViewport",       // OpenGl_OCCT_VIEWPORT
  "occLineWidth",      // OpenGl_OCCT_LINE_WIDTH
  "occLineFeather",    // OpenGl_OCCT_LINE_FEATHER
  "occStipplePattern", // OpenGl_OCCT_LINE_STIPPLE_PATTERN
  "occStippleFactor",  // OpenGl_OCCT_LINE_STIPPLE_FACTOR
  "occWireframeColor", // OpenGl_OCCT_WIREFRAME_COLOR
  "occIsQuadMode",     // OpenGl_OCCT_QUAD_MODE_STATE

  "occOrthoScale",          // OpenGl_OCCT_ORTHO_SCALE
  "occSilhouetteThickness", // OpenGl_OCCT_SILHOUETTE_THICKNESS

  "occNbSpecIBLLevels" // OpenGl_OCCT_NB_SPEC_IBL_LEVELS
};

namespace
{
//! Convert Graphic3d_TypeOfShaderObject enumeration into OpenGL enumeration.
static GLenum shaderTypeToGl(Graphic3d_TypeOfShaderObject theType)
{
  switch (theType)
  {
    case Graphic3d_TOS_VERTEX:
      return GL_VERTEX_SHADER;
    case Graphic3d_TOS_FRAGMENT:
      return GL_FRAGMENT_SHADER;
    case Graphic3d_TOS_GEOMETRY:
      return GL_GEOMETRY_SHADER;
    case Graphic3d_TOS_TESS_CONTROL:
      return GL_TESS_CONTROL_SHADER;
    case Graphic3d_TOS_TESS_EVALUATION:
      return GL_TESS_EVALUATION_SHADER;
    case Graphic3d_TOS_COMPUTE:
      return GL_COMPUTE_SHADER;
  }
  return 0;
}
} // namespace

// =======================================================================
// function : OpenGl_VariableSetterSelector
// purpose  : Creates new variable setter selector
// =======================================================================
OpenGl_VariableSetterSelector::OpenGl_VariableSetterSelector()
{
  // Note: Add new variable setters here
  mySetterList = OpenGl_HashMapInitializer::CreateListOf<size_t, OpenGl_SetterInterface*>(
    Graphic3d_UniformValueTypeID<int>::ID,
    new OpenGl_VariableSetter<int>())(Graphic3d_UniformValueTypeID<float>::ID,
                                      new OpenGl_VariableSetter<float>())(
    Graphic3d_UniformValueTypeID<OpenGl_Vec2>::ID,
    new OpenGl_VariableSetter<OpenGl_Vec2>())(Graphic3d_UniformValueTypeID<OpenGl_Vec3>::ID,
                                              new OpenGl_VariableSetter<OpenGl_Vec3>())(
    Graphic3d_UniformValueTypeID<OpenGl_Vec4>::ID,
    new OpenGl_VariableSetter<OpenGl_Vec4>())(Graphic3d_UniformValueTypeID<OpenGl_Vec2i>::ID,
                                              new OpenGl_VariableSetter<OpenGl_Vec2i>())(
    Graphic3d_UniformValueTypeID<OpenGl_Vec3i>::ID,
    new OpenGl_VariableSetter<OpenGl_Vec3i>())(Graphic3d_UniformValueTypeID<OpenGl_Vec4i>::ID,
                                               new OpenGl_VariableSetter<OpenGl_Vec4i>());
}

// =======================================================================
// function : ~OpenGl_VariableSetterSelector
// purpose  : Releases memory resources of variable setter selector
// =======================================================================
OpenGl_VariableSetterSelector::~OpenGl_VariableSetterSelector()
{
  for (OpenGl_SetterList::Iterator anIt(mySetterList); anIt.More(); anIt.Next())
  {
    delete anIt.Value();
  }

  mySetterList.Clear();
}

// =======================================================================
// function : Set
// purpose  : Sets generic variable to specified shader program
// =======================================================================
void OpenGl_VariableSetterSelector::Set(const Handle(OpenGl_Context)&           theCtx,
                                        const Handle(Graphic3d_ShaderVariable)& theVariable,
                                        OpenGl_ShaderProgram*                   theProgram) const
{
  Standard_ASSERT_RETURN(mySetterList.IsBound(theVariable->Value()->TypeID()),
                         "The type of user-defined uniform variable is not supported...", );

  mySetterList.Find(theVariable->Value()->TypeID())->Set(theCtx, theVariable, theProgram);
}

// =======================================================================
// function : OpenGl_ShaderProgram
// purpose  : Creates uninitialized shader program
// =======================================================================
OpenGl_ShaderProgram::OpenGl_ShaderProgram(const Handle(Graphic3d_ShaderProgram)& theProxy,
                                           const TCollection_AsciiString&         theId)
    : OpenGl_NamedResource(!theProxy.IsNull() ? theProxy->GetId() : theId),
      myProgramID(NO_PROGRAM),
      myProxy(theProxy),
      myShareCount(1),
      myNbLightsMax(0),
      myNbShadowMaps(0),
      myNbClipPlanesMax(0),
      myNbFragOutputs(1),
      myTextureSetBits(Graphic3d_TextureSetBits_NONE),
      myOitOutput(Graphic3d_RTM_BLEND_UNORDERED),
      myHasAlphaTest(false),
      myHasTessShader(false)
{
  memset(myCurrentState, 0, sizeof(myCurrentState));
}

// =======================================================================
// function : Initialize
// purpose  : Initializes program object with the list of shader objects
// =======================================================================
Standard_Boolean OpenGl_ShaderProgram::Initialize(const Handle(OpenGl_Context)&     theCtx,
                                                  const Graphic3d_ShaderObjectList& theShaders)
{
  myHasTessShader = false;
  if (theCtx.IsNull() || !Create(theCtx))
  {
    return Standard_False;
  }

  TCollection_AsciiString aHeaderVer =
    !myProxy.IsNull() ? myProxy->Header() : TCollection_AsciiString();
  int aShaderMask = 0;
  for (Graphic3d_ShaderObjectList::Iterator anIter(theShaders); anIter.More(); anIter.Next())
  {
    aShaderMask |= anIter.Value()->Type();
  }
  myHasTessShader =
    (aShaderMask & (Graphic3d_TOS_TESS_CONTROL | Graphic3d_TOS_TESS_EVALUATION)) != 0;
  myNbFragOutputs  = !myProxy.IsNull() ? myProxy->NbFragmentOutputs() : 1;
  myTextureSetBits = Graphic3d_TextureSetBits_NONE;
  myHasAlphaTest   = !myProxy.IsNull() && myProxy->HasAlphaTest();
  myOitOutput      = !myProxy.IsNull() ? myProxy->OitOutput() : Graphic3d_RTM_BLEND_UNORDERED;
  if (myOitOutput == Graphic3d_RTM_BLEND_OIT && myNbFragOutputs < 2)
  {
    myOitOutput = Graphic3d_RTM_BLEND_UNORDERED;
  }
  else if (myOitOutput == Graphic3d_RTM_DEPTH_PEELING_OIT && myNbFragOutputs < 3)
  {
    myOitOutput = Graphic3d_RTM_BLEND_UNORDERED;
  }

  // detect the minimum GLSL version required for defined Shader Objects
  if (theCtx->GraphicsLibrary() == Aspect_GraphicsLibrary_OpenGLES)
  {
    if (myHasTessShader)
    {
      if (!theCtx->IsGlGreaterEqual(3, 2))
      {
        theCtx->PushMessage(GL_DEBUG_SOURCE_APPLICATION,
                            GL_DEBUG_TYPE_ERROR,
                            0,
                            GL_DEBUG_SEVERITY_HIGH,
                            "Error! Tessellation shader requires OpenGL ES 3.2+");
        return false;
      }
      else if (aHeaderVer.IsEmpty())
      {
        aHeaderVer = "#version 320 es";
      }
    }
    else if ((aShaderMask & Graphic3d_TOS_GEOMETRY) != 0)
    {
      switch (theCtx->hasGeometryStage)
      {
        case OpenGl_FeatureNotAvailable: {
          theCtx->PushMessage(
            GL_DEBUG_SOURCE_APPLICATION,
            GL_DEBUG_TYPE_ERROR,
            0,
            GL_DEBUG_SEVERITY_HIGH,
            "Error! Geometry shader requires OpenGL ES 3.2+ or GL_EXT_geometry_shader");
          return false;
        }
        case OpenGl_FeatureInExtensions: {
          if (aHeaderVer.IsEmpty())
          {
            aHeaderVer = "#version 310 es";
          }
          break;
        }
        case OpenGl_FeatureInCore: {
          if (aHeaderVer.IsEmpty())
          {
            aHeaderVer = "#version 320 es";
          }
          break;
        }
      }
    }
    else if ((aShaderMask & Graphic3d_TOS_COMPUTE) != 0)
    {
      if (!theCtx->IsGlGreaterEqual(3, 1))
      {
        theCtx->PushMessage(GL_DEBUG_SOURCE_APPLICATION,
                            GL_DEBUG_TYPE_ERROR,
                            0,
                            GL_DEBUG_SEVERITY_HIGH,
                            "Error! Compute shaders require OpenGL ES 3.1+");
        return false;
      }
      else if (aHeaderVer.IsEmpty())
      {
        aHeaderVer = "#version 310 es";
      }
    }
  }
  else
  {
    if ((aShaderMask & Graphic3d_TOS_COMPUTE) != 0)
    {
      if (!theCtx->IsGlGreaterEqual(4, 3))
      {
        theCtx->PushMessage(GL_DEBUG_SOURCE_APPLICATION,
                            GL_DEBUG_TYPE_ERROR,
                            0,
                            GL_DEBUG_SEVERITY_HIGH,
                            "Error! Compute shaders require OpenGL 4.3+");
        return 0;
      }
      else if (aHeaderVer.IsEmpty())
      {
        aHeaderVer = "#version 430";
      }
    }
    else if (myHasTessShader)
    {
      if (!theCtx->IsGlGreaterEqual(4, 0))
      {
        theCtx->PushMessage(GL_DEBUG_SOURCE_APPLICATION,
                            GL_DEBUG_TYPE_ERROR,
                            0,
                            GL_DEBUG_SEVERITY_HIGH,
                            "Error! Tessellation shaders require OpenGL 4.0+");
        return 0;
      }
      else if (aHeaderVer.IsEmpty())
      {
        aHeaderVer = "#version 400";
      }
    }
    else if ((aShaderMask & Graphic3d_TOS_GEOMETRY) != 0)
    {
      if (!theCtx->IsGlGreaterEqual(3, 2))
      {
        theCtx->PushMessage(GL_DEBUG_SOURCE_APPLICATION,
                            GL_DEBUG_TYPE_ERROR,
                            0,
                            GL_DEBUG_SEVERITY_HIGH,
                            "Error! Geometry shaders require OpenGL 3.2+");
        return 0;
      }
      else if (aHeaderVer.IsEmpty())
      {
        aHeaderVer = "#version 150";
      }
    }
  }

  for (Graphic3d_ShaderObjectList::Iterator anIter(theShaders); anIter.More(); anIter.Next())
  {
    if (!anIter.Value()->IsDone())
    {
      const TCollection_ExtendedString aMsg = "Error! Failed to get shader source";
      theCtx->PushMessage(GL_DEBUG_SOURCE_APPLICATION,
                          GL_DEBUG_TYPE_ERROR,
                          0,
                          GL_DEBUG_SEVERITY_HIGH,
                          aMsg);
      return Standard_False;
    }

    const GLenum aShaderType = shaderTypeToGl(anIter.Value()->Type());
    if (aShaderType == 0)
    {
      return Standard_False;
    }

    Handle(OpenGl_ShaderObject) aShader = new OpenGl_ShaderObject(aShaderType);
    if (!aShader->Create(theCtx))
    {
      aShader->Release(theCtx.operator->());
      return Standard_False;
    }

    TCollection_AsciiString anExtensions = "// Enable extensions used in OCCT GLSL programs\n";
    if (myNbFragOutputs > 1)
    {
      if (theCtx->hasDrawBuffers)
      {
        anExtensions += "#define OCC_ENABLE_draw_buffers\n";
        switch (myOitOutput)
        {
          case Graphic3d_RTM_BLEND_UNORDERED:
            break;
          case Graphic3d_RTM_BLEND_OIT:
            anExtensions += "#define OCC_WRITE_WEIGHT_OIT_COVERAGE\n";
            break;
          case Graphic3d_RTM_DEPTH_PEELING_OIT:
            anExtensions += "#define OCC_DEPTH_PEEL_OIT\n";
            break;
        }
      }
      else
      {
        theCtx->PushMessage(
          GL_DEBUG_SOURCE_APPLICATION,
          GL_DEBUG_TYPE_ERROR,
          0,
          GL_DEBUG_SEVERITY_HIGH,
          "Error! Multiple draw buffers required by the program, but aren't supported by OpenGL");
        return Standard_False;
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
    }
    if (myHasAlphaTest)
    {
      anExtensions += "#define OCC_ALPHA_TEST\n";
    }

    if (theCtx->hasSampleVariables == OpenGl_FeatureInExtensions)
    {
      if (theCtx->GraphicsLibrary() == Aspect_GraphicsLibrary_OpenGLES
          && theCtx->oesSampleVariables)
      {
        anExtensions += "#extension GL_OES_sample_variables : enable\n";
      }
      else if (theCtx->GraphicsLibrary() == Aspect_GraphicsLibrary_OpenGL
               && theCtx->arbSampleShading)
      {
        anExtensions += "#extension GL_ARB_sample_shading : enable\n";
      }
    }

    if (theCtx->GraphicsLibrary() == Aspect_GraphicsLibrary_OpenGLES
        && theCtx->hasGeometryStage == OpenGl_FeatureInExtensions)
    {
      anExtensions += "#extension GL_EXT_geometry_shader : enable\n"
                      "#extension GL_EXT_shader_io_blocks : enable\n";
    }

    TCollection_AsciiString aPrecisionHeader;
    if (anIter.Value()->Type() == Graphic3d_TOS_FRAGMENT
        && theCtx->GraphicsLibrary() == Aspect_GraphicsLibrary_OpenGLES)
    {
      aPrecisionHeader = theCtx->hasHighp ? "precision highp float;\n"
                                            "precision highp int;\n"
                                          : "precision mediump float;\n"
                                            "precision mediump int;\n";
    }

    TCollection_AsciiString aHeaderType;
    switch (anIter.Value()->Type())
    {
      case Graphic3d_TOS_COMPUTE: {
        aHeaderType = "#define COMPUTE_SHADER\n";
        break;
      }
      case Graphic3d_TOS_VERTEX: {
        aHeaderType = "#define VERTEX_SHADER\n";
        break;
      }
      case Graphic3d_TOS_TESS_CONTROL: {
        aHeaderType = "#define TESS_CONTROL_SHADER\n";
        break;
      }
      case Graphic3d_TOS_TESS_EVALUATION: {
        aHeaderType = "#define TESS_EVALUATION_SHADER\n";
        break;
      }
      case Graphic3d_TOS_GEOMETRY: {
        aHeaderType = "#define GEOMETRY_SHADER\n";
        break;
      }
      case Graphic3d_TOS_FRAGMENT: {
        aHeaderType = "#define FRAGMENT_SHADER\n";
        break;
      }
    }

    TCollection_AsciiString aHeaderConstants;
    myNbLightsMax     = !myProxy.IsNull() ? myProxy->NbLightsMax() : 0;
    myNbShadowMaps    = !myProxy.IsNull() ? myProxy->NbShadowMaps() : 0;
    myNbClipPlanesMax = !myProxy.IsNull() ? myProxy->NbClipPlanesMax() : 0;
    aHeaderConstants += TCollection_AsciiString("#define THE_MAX_LIGHTS ") + myNbLightsMax + "\n";
    aHeaderConstants +=
      TCollection_AsciiString("#define THE_MAX_CLIP_PLANES ") + myNbClipPlanesMax + "\n";
    aHeaderConstants +=
      TCollection_AsciiString("#define THE_NB_FRAG_OUTPUTS ") + myNbFragOutputs + "\n";
    if (myNbShadowMaps > 0)
    {
      aHeaderConstants +=
        TCollection_AsciiString("#define THE_NB_SHADOWMAPS ") + myNbShadowMaps + "\n";
    }
    if (theCtx->caps->useZeroToOneDepth && theCtx->arbClipControl)
    {
      aHeaderConstants += "#define THE_ZERO_TO_ONE_DEPTH\n";
    }
    if (!myProxy.IsNull() && myProxy->HasDefaultSampler())
    {
      aHeaderConstants += "#define THE_HAS_DEFAULT_SAMPLER\n";
    }
    if (!myProxy.IsNull())
    {
      if (myProxy->IsPBR())
      {
        aHeaderConstants += "#define THE_IS_PBR\n";
      }
      if ((myProxy->TextureSetBits() & Graphic3d_TextureSetBits_BaseColor) != 0)
      {
        aHeaderConstants += "#define THE_HAS_TEXTURE_COLOR\n";
      }
      if ((myProxy->TextureSetBits() & Graphic3d_TextureSetBits_Emissive) != 0)
      {
        aHeaderConstants += "#define THE_HAS_TEXTURE_EMISSIVE\n";
      }
      if ((myProxy->TextureSetBits() & Graphic3d_TextureSetBits_Normal) != 0)
      {
        aHeaderConstants += "#define THE_HAS_TEXTURE_NORMAL\n";
      }
      if ((myProxy->TextureSetBits() & Graphic3d_TextureSetBits_Occlusion) != 0)
      {
        aHeaderConstants += "#define THE_HAS_TEXTURE_OCCLUSION\n";
      }
      if ((myProxy->TextureSetBits() & Graphic3d_TextureSetBits_MetallicRoughness) != 0)
      {
        aHeaderConstants += "#define THE_HAS_TEXTURE_METALROUGHNESS\n";
      }
    }

    const TCollection_AsciiString aSource =
      aHeaderVer // #version   - header defining GLSL version, should be first
      + (!aHeaderVer.IsEmpty() ? "\n" : "")
      + anExtensions     // #extension - list of enabled extensions,   should be second
      + aPrecisionHeader // precision  - default precision qualifiers, should be before any code
      + aHeaderType      // auxiliary macros defining a shader stage (type)
      + aHeaderConstants
      + Shaders_Declarations_glsl // common declarations (global constants and Vertex Shader inputs)
      + Shaders_DeclarationsImpl_glsl
      // clang-format off
                                          + anIter.Value()->Source();      // the source code itself (defining main() function)
    // clang-format on
    if (!aShader->LoadAndCompile(theCtx, myResourceId, aSource))
    {
      aShader->Release(theCtx.operator->());
      return Standard_False;
    }

    if (theCtx->caps->glslDumpLevel)
    {
      TCollection_AsciiString anOutputSource = aSource;
      if (theCtx->caps->glslDumpLevel == OpenGl_ShaderProgramDumpLevel_Short)
      {
        anOutputSource = aHeaderVer + (!aHeaderVer.IsEmpty() ? "\n" : "") + anExtensions
                         + aPrecisionHeader + aHeaderType + aHeaderConstants
                         + anIter.Value()->Source();
      }
      aShader->DumpSourceCode(theCtx, myResourceId, anOutputSource);
    }

    if (!AttachShader(theCtx, aShader))
    {
      aShader->Release(theCtx.operator->());
      return Standard_False;
    }
  }

  // bind locations for pre-defined Vertex Attributes
  SetAttributeName(theCtx, Graphic3d_TOA_POS, "occVertex");
  SetAttributeName(theCtx, Graphic3d_TOA_NORM, "occNormal");
  SetAttributeName(theCtx, Graphic3d_TOA_UV, "occTexCoord");
  SetAttributeName(theCtx, Graphic3d_TOA_COLOR, "occVertColor");

  // bind custom Vertex Attributes
  if (!myProxy.IsNull())
  {
    for (Graphic3d_ShaderAttributeList::Iterator anAttribIter(myProxy->VertexAttributes());
         anAttribIter.More();
         anAttribIter.Next())
    {
      SetAttributeName(theCtx,
                       anAttribIter.Value()->Location(),
                       anAttribIter.Value()->Name().ToCString());
    }
  }

  if (!Link(theCtx))
  {
    return Standard_False;
  }

  // set uniform defaults
  const Handle(OpenGl_ShaderProgram)& anOldProgram = theCtx->ActiveProgram();
  theCtx->core20fwd->glUseProgram(myProgramID);
  if (const OpenGl_ShaderUniformLocation aLocTexEnable =
        GetStateLocation(OpenGl_OCCT_TEXTURE_ENABLE))
  {
    SetUniform(theCtx, aLocTexEnable, 0); // Off
  }
  if (const OpenGl_ShaderUniformLocation aLocSampler =
        GetUniformLocation(theCtx, "occActiveSampler"))
  {
    SetUniform(theCtx, aLocSampler, GLint(Graphic3d_TextureUnit_0));
  }
  if (const OpenGl_ShaderUniformLocation aLocSampler =
        GetUniformLocation(theCtx, "occSamplerBaseColor"))
  {
    myTextureSetBits |= Graphic3d_TextureSetBits_BaseColor;
    SetUniform(theCtx, aLocSampler, GLint(Graphic3d_TextureUnit_BaseColor));
  }
  if (const OpenGl_ShaderUniformLocation aLocSampler =
        GetUniformLocation(theCtx, "occSamplerPointSprite"))
  {
    // Graphic3d_TextureUnit_PointSprite
    // myTextureSetBits |= Graphic3d_TextureSetBits_PointSprite;
    SetUniform(theCtx, aLocSampler, GLint(theCtx->SpriteTextureUnit()));
  }
  if (const OpenGl_ShaderUniformLocation aLocSampler =
        GetUniformLocation(theCtx, "occSamplerMetallicRoughness"))
  {
    myTextureSetBits |= Graphic3d_TextureSetBits_MetallicRoughness;
    SetUniform(theCtx, aLocSampler, GLint(Graphic3d_TextureUnit_MetallicRoughness));
  }
  if (const OpenGl_ShaderUniformLocation aLocSampler =
        GetUniformLocation(theCtx, "occSamplerEmissive"))
  {
    myTextureSetBits |= Graphic3d_TextureSetBits_Emissive;
    SetUniform(theCtx, aLocSampler, GLint(Graphic3d_TextureUnit_Emissive));
  }
  if (const OpenGl_ShaderUniformLocation aLocSampler =
        GetUniformLocation(theCtx, "occSamplerOcclusion"))
  {
    myTextureSetBits |= Graphic3d_TextureSetBits_Occlusion;
    SetUniform(theCtx, aLocSampler, GLint(Graphic3d_TextureUnit_Occlusion));
  }
  if (const OpenGl_ShaderUniformLocation aLocSampler =
        GetUniformLocation(theCtx, "occSamplerNormal"))
  {
    myTextureSetBits |= Graphic3d_TextureSetBits_Normal;
    SetUniform(theCtx, aLocSampler, GLint(Graphic3d_TextureUnit_Normal));
  }
  if (const OpenGl_ShaderUniformLocation aLocSampler =
        GetUniformLocation(theCtx, "occDiffIBLMapSHCoeffs"))
  {
    SetUniform(theCtx, aLocSampler, GLint(theCtx->PBRDiffIBLMapSHTexUnit()));
  }
  if (const OpenGl_ShaderUniformLocation aLocSampler = GetUniformLocation(theCtx, "occSpecIBLMap"))
  {
    SetUniform(theCtx, aLocSampler, GLint(theCtx->PBRSpecIBLMapTexUnit()));
  }
  if (const OpenGl_ShaderUniformLocation aLocSampler = GetUniformLocation(theCtx, "occEnvLUT"))
  {
    SetUniform(theCtx, aLocSampler, GLint(theCtx->PBREnvLUTTexUnit()));
  }
  if (const OpenGl_ShaderUniformLocation aLocSampler =
        GetUniformLocation(theCtx, "occShadowMapSamplers"))
  {
    std::vector<GLint> aShadowSamplers(myNbShadowMaps);
    const GLint        aSamplFrom = GLint(theCtx->ShadowMapTexUnit()) - myNbShadowMaps + 1;
    for (Standard_Integer aSamplerIter = 0; aSamplerIter < myNbShadowMaps; ++aSamplerIter)
    {
      aShadowSamplers[aSamplerIter] = aSamplFrom + aSamplerIter;
    }
    SetUniform(theCtx, aLocSampler, myNbShadowMaps, &aShadowSamplers.front());
  }

  if (const OpenGl_ShaderUniformLocation aLocSampler =
        GetUniformLocation(theCtx, "occDepthPeelingDepth"))
  {
    SetUniform(theCtx, aLocSampler, GLint(theCtx->DepthPeelingDepthTexUnit()));
  }
  if (const OpenGl_ShaderUniformLocation aLocSampler =
        GetUniformLocation(theCtx, "occDepthPeelingFrontColor"))
  {
    SetUniform(theCtx, aLocSampler, GLint(theCtx->DepthPeelingFrontColorTexUnit()));
  }

  const TCollection_AsciiString aSamplerNamePrefix("occSampler");
  const Standard_Integer        aNbUnitsMax =
    Max(theCtx->MaxCombinedTextureUnits(), Graphic3d_TextureUnit_NB);
  for (GLint aUnitIter = 0; aUnitIter < aNbUnitsMax; ++aUnitIter)
  {
    const TCollection_AsciiString aName = aSamplerNamePrefix + aUnitIter;
    if (const OpenGl_ShaderUniformLocation aLocSampler =
          GetUniformLocation(theCtx, aName.ToCString()))
    {
      SetUniform(theCtx, aLocSampler, aUnitIter);
    }
  }

  theCtx->core20fwd->glUseProgram(!anOldProgram.IsNull() ? anOldProgram->ProgramId()
                                                         : OpenGl_ShaderProgram::NO_PROGRAM);
  return Standard_True;
}

// =======================================================================
// function : ~OpenGl_ShaderProgram
// purpose  : Releases resources of shader program
// =======================================================================
OpenGl_ShaderProgram::~OpenGl_ShaderProgram()
{
  Release(NULL);
}

// =======================================================================
// function : AttachShader
// purpose  : Attaches shader object to the program object
// =======================================================================
Standard_Boolean OpenGl_ShaderProgram::AttachShader(const Handle(OpenGl_Context)&      theCtx,
                                                    const Handle(OpenGl_ShaderObject)& theShader)
{
  if (myProgramID == NO_PROGRAM || theShader.IsNull())
  {
    return Standard_False;
  }

  for (OpenGl_ShaderList::Iterator anIter(myShaderObjects); anIter.More(); anIter.Next())
  {
    if (theShader == anIter.Value())
    {
      return Standard_False;
    }
  }

  myShaderObjects.Append(theShader);
  theCtx->core20fwd->glAttachShader(myProgramID, theShader->myShaderID);
  return Standard_True;
}

// =======================================================================
// function : DetachShader
// purpose  : Detaches shader object to the program object
// =======================================================================
Standard_Boolean OpenGl_ShaderProgram::DetachShader(const Handle(OpenGl_Context)&      theCtx,
                                                    const Handle(OpenGl_ShaderObject)& theShader)
{
  if (myProgramID == NO_PROGRAM || theShader.IsNull())
  {
    return Standard_False;
  }

  OpenGl_ShaderList::Iterator anIter(myShaderObjects);
  while (anIter.More())
  {
    if (theShader == anIter.Value())
    {
      myShaderObjects.Remove(anIter);
      break;
    }

    anIter.Next();
  }

  if (!anIter.More())
  {
    return Standard_False;
  }

  theCtx->core20fwd->glDetachShader(myProgramID, theShader->myShaderID);
  return Standard_True;
}

// =======================================================================
// function : Link
// purpose  : Links the program object
// =======================================================================
Standard_Boolean OpenGl_ShaderProgram::link(const Handle(OpenGl_Context)& theCtx)
{
  if (myProgramID == NO_PROGRAM)
  {
    return Standard_False;
  }

  GLint aStatus = GL_FALSE;
  theCtx->core20fwd->glLinkProgram(myProgramID);
  theCtx->core20fwd->glGetProgramiv(myProgramID, GL_LINK_STATUS, &aStatus);
  if (aStatus == GL_FALSE)
  {
    return Standard_False;
  }

  memset(myCurrentState, 0, sizeof(myCurrentState));
  for (GLint aVar = 0; aVar < OpenGl_OCCT_NUMBER_OF_STATE_VARIABLES; ++aVar)
  {
    myStateLocations[aVar] = GetUniformLocation(theCtx, PredefinedKeywords[aVar]);
  }
  return Standard_True;
}

// =======================================================================
// function : Link
// purpose  :
// =======================================================================
Standard_Boolean OpenGl_ShaderProgram::Link(const Handle(OpenGl_Context)& theCtx, bool theIsVerbose)
{
  if (!theIsVerbose)
  {
    return link(theCtx);
  }

  if (!link(theCtx))
  {
    TCollection_AsciiString aLog;
    FetchInfoLog(theCtx, aLog);
    if (aLog.IsEmpty())
    {
      aLog = "Linker log is empty.";
    }
    theCtx->PushMessage(GL_DEBUG_SOURCE_APPLICATION,
                        GL_DEBUG_TYPE_ERROR,
                        0,
                        GL_DEBUG_SEVERITY_HIGH,
                        TCollection_AsciiString("Failed to link program object [") + myResourceId
                          + "]! Linker log:\n" + aLog);
    return false;
  }
  else if (theCtx->caps->glslWarnings)
  {
    TCollection_AsciiString aLog;
    FetchInfoLog(theCtx, aLog);
    if (!aLog.IsEmpty() && !aLog.IsEqual("No errors.\n"))
    {
      theCtx->PushMessage(GL_DEBUG_SOURCE_APPLICATION,
                          GL_DEBUG_TYPE_PORTABILITY,
                          0,
                          GL_DEBUG_SEVERITY_LOW,
                          TCollection_AsciiString("GLSL linker log [") + myResourceId + "]:\n"
                            + aLog);
    }
  }
  return true;
}

// =======================================================================
// function : FetchInfoLog
// purpose  : Fetches information log of the last link operation
// =======================================================================
Standard_Boolean OpenGl_ShaderProgram::FetchInfoLog(const Handle(OpenGl_Context)& theCtx,
                                                    TCollection_AsciiString&      theOutput)
{
  if (myProgramID == NO_PROGRAM)
  {
    return Standard_False;
  }

  GLint aLength = 0;
  theCtx->core20fwd->glGetProgramiv(myProgramID, GL_INFO_LOG_LENGTH, &aLength);
  if (aLength > 0)
  {
    GLchar* aLog = (GLchar*)alloca(aLength);
    memset(aLog, 0, aLength);
    theCtx->core20fwd->glGetProgramInfoLog(myProgramID, aLength, NULL, aLog);
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

  for (Graphic3d_ShaderVariableList::Iterator anIter(myProxy->Variables()); anIter.More();
       anIter.Next())
  {
    mySetterSelector.Set(theCtx, anIter.Value(), this);
  }

  myProxy->ClearVariables();
  return Standard_True;
}

// =======================================================================
// function : GetUniformLocation
// purpose  : Returns location (index) of the specific uniform variable
// =======================================================================
OpenGl_ShaderUniformLocation OpenGl_ShaderProgram::GetUniformLocation(
  const Handle(OpenGl_Context)& theCtx,
  const GLchar*                 theName) const
{
  return OpenGl_ShaderUniformLocation(
    myProgramID != NO_PROGRAM ? theCtx->core20fwd->glGetUniformLocation(myProgramID, theName)
                              : INVALID_LOCATION);
}

// =======================================================================
// function : GetAttributeLocation
// purpose  : Returns location (index) of the generic vertex attribute
// =======================================================================
GLint OpenGl_ShaderProgram::GetAttributeLocation(const Handle(OpenGl_Context)& theCtx,
                                                 const GLchar*                 theName) const
{
  return myProgramID != NO_PROGRAM ? theCtx->core20fwd->glGetAttribLocation(myProgramID, theName)
                                   : INVALID_LOCATION;
}

// =======================================================================
// function : GetUniform
// purpose  : Returns the value of the integer uniform variable
// =======================================================================
Standard_Boolean OpenGl_ShaderProgram::GetUniform(const Handle(OpenGl_Context)& theCtx,
                                                  GLint                         theLocation,
                                                  OpenGl_Vec4i&                 theValue) const
{
  if (myProgramID == NO_PROGRAM || theLocation == INVALID_LOCATION)
  {
    return Standard_False;
  }

  theCtx->core20fwd->glGetUniformiv(myProgramID, theLocation, theValue);
  return Standard_True;
}

// =======================================================================
// function : GetUniform
// purpose  : Returns the value of the floating-point uniform variable
// =======================================================================
Standard_Boolean OpenGl_ShaderProgram::GetUniform(const Handle(OpenGl_Context)& theCtx,
                                                  GLint                         theLocation,
                                                  OpenGl_Vec4&                  theValue) const
{
  if (myProgramID == NO_PROGRAM || theLocation == INVALID_LOCATION)
  {
    return Standard_False;
  }

  theCtx->core20fwd->glGetUniformfv(myProgramID, theLocation, theValue);
  return Standard_True;
}

// =======================================================================
// function : GetAttribute
// purpose  : Returns the integer vertex attribute
// =======================================================================
Standard_Boolean OpenGl_ShaderProgram::GetAttribute(const Handle(OpenGl_Context)& theCtx,
                                                    GLint                         theIndex,
                                                    OpenGl_Vec4i&                 theValue) const
{
  if (myProgramID == NO_PROGRAM || theIndex == INVALID_LOCATION)
  {
    return Standard_False;
  }

  theCtx->core20fwd->glGetVertexAttribiv(theIndex, GL_CURRENT_VERTEX_ATTRIB, theValue);
  return Standard_True;
}

// =======================================================================
// function : GetAttribute
// purpose  : Returns the floating-point vertex attribute
// =======================================================================
Standard_Boolean OpenGl_ShaderProgram::GetAttribute(const Handle(OpenGl_Context)& theCtx,
                                                    GLint                         theIndex,
                                                    OpenGl_Vec4&                  theValue) const
{
  if (myProgramID == NO_PROGRAM || theIndex == INVALID_LOCATION)
  {
    return Standard_False;
  }

  theCtx->core20fwd->glGetVertexAttribfv(theIndex, GL_CURRENT_VERTEX_ATTRIB, theValue);
  return Standard_True;
}

// =======================================================================
// function : SetAttributeName
// purpose  :
// =======================================================================
Standard_Boolean OpenGl_ShaderProgram::SetAttributeName(const Handle(OpenGl_Context)& theCtx,
                                                        GLint                         theIndex,
                                                        const GLchar*                 theName)
{
  theCtx->core20fwd->glBindAttribLocation(myProgramID, theIndex, theName);
  return Standard_True;
}

// =======================================================================
// function : SetAttribute
// purpose  :
// =======================================================================
Standard_Boolean OpenGl_ShaderProgram::SetAttribute(const Handle(OpenGl_Context)& theCtx,
                                                    GLint                         theIndex,
                                                    GLfloat                       theValue)
{
  if (myProgramID == NO_PROGRAM || theIndex == INVALID_LOCATION)
  {
    return Standard_False;
  }

  theCtx->core20fwd->glVertexAttrib1f(theIndex, theValue);
  return Standard_True;
}

// =======================================================================
// function : SetAttribute
// purpose  :
// =======================================================================
Standard_Boolean OpenGl_ShaderProgram::SetAttribute(const Handle(OpenGl_Context)& theCtx,
                                                    GLint                         theIndex,
                                                    const OpenGl_Vec2&            theValue)
{
  if (myProgramID == NO_PROGRAM || theIndex == INVALID_LOCATION)
  {
    return Standard_False;
  }

  theCtx->core20fwd->glVertexAttrib2fv(theIndex, theValue);
  return Standard_True;
}

// =======================================================================
// function : SetAttribute
// purpose  :
// =======================================================================
Standard_Boolean OpenGl_ShaderProgram::SetAttribute(const Handle(OpenGl_Context)& theCtx,
                                                    GLint                         theIndex,
                                                    const OpenGl_Vec3&            theValue)
{
  if (myProgramID == NO_PROGRAM || theIndex == INVALID_LOCATION)
  {
    return Standard_False;
  }

  theCtx->core20fwd->glVertexAttrib3fv(theIndex, theValue);
  return Standard_True;
}

// =======================================================================
// function : SetAttribute
// purpose  :
// =======================================================================
Standard_Boolean OpenGl_ShaderProgram::SetAttribute(const Handle(OpenGl_Context)& theCtx,
                                                    GLint                         theIndex,
                                                    const OpenGl_Vec4&            theValue)
{
  if (myProgramID == NO_PROGRAM || theIndex == INVALID_LOCATION)
  {
    return Standard_False;
  }

  theCtx->core20fwd->glVertexAttrib4fv(theIndex, theValue);
  return Standard_True;
}

// =======================================================================
// function : SetUniform
// purpose  : Specifies the value of the integer uniform variable
// =======================================================================
Standard_Boolean OpenGl_ShaderProgram::SetUniform(const Handle(OpenGl_Context)& theCtx,
                                                  GLint                         theLocation,
                                                  GLint                         theValue)
{
  if (myProgramID == NO_PROGRAM || theLocation == INVALID_LOCATION)
  {
    return Standard_False;
  }

  theCtx->core20fwd->glUniform1i(theLocation, theValue);
  return Standard_True;
}

// =======================================================================
// function : SetUniform
// purpose  :
// =======================================================================
Standard_Boolean OpenGl_ShaderProgram::SetUniform(const Handle(OpenGl_Context)& theCtx,
                                                  GLint                         theLocation,
                                                  const OpenGl_Vec2u&           theValue)
{
  if (myProgramID == NO_PROGRAM || theLocation == INVALID_LOCATION)
  {
    return false;
  }

  if (theCtx->GraphicsLibrary() == Aspect_GraphicsLibrary_OpenGLES)
  {
    if (theCtx->core30 != NULL)
    {
      theCtx->core30->glUniform2uiv(theLocation, 1, theValue.GetData());
      return true;
    }
  }
  else
  {
    if (theCtx->core32 != NULL)
    {
      theCtx->core32->glUniform2uiv(theLocation, 1, theValue.GetData());
      return true;
    }
  }

  return false;
}

// =======================================================================
// function : SetUniform
// purpose  :
// =======================================================================
Standard_Boolean OpenGl_ShaderProgram::SetUniform(const Handle(OpenGl_Context)& theCtx,
                                                  const GLchar*                 theName,
                                                  const GLsizei                 theCount,
                                                  const OpenGl_Vec2u*           theValue)
{
  return SetUniform(theCtx, GetUniformLocation(theCtx, theName), theCount, theValue);
}

// =======================================================================
// function : SetUniform
// purpose  :
// =======================================================================
Standard_Boolean OpenGl_ShaderProgram::SetUniform(const Handle(OpenGl_Context)& theCtx,
                                                  GLint                         theLocation,
                                                  const GLsizei                 theCount,
                                                  const OpenGl_Vec2u*           theValue)
{
  if (myProgramID == NO_PROGRAM || theLocation == INVALID_LOCATION)
  {
    return false;
  }

  if (theCtx->GraphicsLibrary() == Aspect_GraphicsLibrary_OpenGLES)
  {
    if (theCtx->core30 != NULL)
    {
      theCtx->core30->glUniform2uiv(theLocation, theCount, theValue->GetData());
      return true;
    }
  }
  else
  {
    if (theCtx->core32 != NULL)
    {
      theCtx->core32->glUniform2uiv(theLocation, theCount, theValue->GetData());
      return true;
    }
  }
  return false;
}

// =======================================================================
// function : SetUniform
// purpose  : Specifies the value of the floating-point uniform variable
// =======================================================================
Standard_Boolean OpenGl_ShaderProgram::SetUniform(const Handle(OpenGl_Context)& theCtx,
                                                  GLint                         theLocation,
                                                  GLfloat                       theValue)
{
  if (myProgramID == NO_PROGRAM || theLocation == INVALID_LOCATION)
  {
    return Standard_False;
  }

  theCtx->core20fwd->glUniform1f(theLocation, theValue);
  return Standard_True;
}

// =======================================================================
// function : SetUniform
// purpose  : Specifies the value of the integer uniform 2D vector
// =======================================================================
Standard_Boolean OpenGl_ShaderProgram::SetUniform(const Handle(OpenGl_Context)& theCtx,
                                                  GLint                         theLocation,
                                                  const OpenGl_Vec2i&           theValue)
{
  if (myProgramID == NO_PROGRAM || theLocation == INVALID_LOCATION)
  {
    return Standard_False;
  }

  theCtx->core20fwd->glUniform2iv(theLocation, 1, theValue);
  return Standard_True;
}

// =======================================================================
// function : SetUniform
// purpose  : Specifies the value of the integer uniform 3D vector
// =======================================================================
Standard_Boolean OpenGl_ShaderProgram::SetUniform(const Handle(OpenGl_Context)& theCtx,
                                                  GLint                         theLocation,
                                                  const OpenGl_Vec3i&           theValue)
{
  if (myProgramID == NO_PROGRAM || theLocation == INVALID_LOCATION)
  {
    return Standard_False;
  }

  theCtx->core20fwd->glUniform3iv(theLocation, 1, theValue);
  return Standard_True;
}

// =======================================================================
// function : SetUniform
// purpose  : Specifies the value of the integer uniform 4D vector
// =======================================================================
Standard_Boolean OpenGl_ShaderProgram::SetUniform(const Handle(OpenGl_Context)& theCtx,
                                                  GLint                         theLocation,
                                                  const OpenGl_Vec4i&           theValue)
{
  if (myProgramID == NO_PROGRAM || theLocation == INVALID_LOCATION)
  {
    return Standard_False;
  }

  theCtx->core20fwd->glUniform4iv(theLocation, 1, theValue);
  return Standard_True;
}

// =======================================================================
// function : SetUniform
// purpose  : Specifies the value of the floating-point uniform 2D vector
// =======================================================================
Standard_Boolean OpenGl_ShaderProgram::SetUniform(const Handle(OpenGl_Context)& theCtx,
                                                  GLint                         theLocation,
                                                  const OpenGl_Vec2&            theValue)
{
  if (myProgramID == NO_PROGRAM || theLocation == INVALID_LOCATION)
  {
    return Standard_False;
  }

  theCtx->core20fwd->glUniform2fv(theLocation, 1, theValue);
  return Standard_True;
}

// =======================================================================
// function : SetUniform
// purpose  : Specifies the value of the floating-point uniform 3D vector
// =======================================================================
Standard_Boolean OpenGl_ShaderProgram::SetUniform(const Handle(OpenGl_Context)& theCtx,
                                                  GLint                         theLocation,
                                                  const OpenGl_Vec3&            theValue)
{
  if (myProgramID == NO_PROGRAM || theLocation == INVALID_LOCATION)
  {
    return Standard_False;
  }

  theCtx->core20fwd->glUniform3fv(theLocation, 1, theValue);
  return Standard_True;
}

// =======================================================================
// function : SetUniform
// purpose  : Specifies the value of the floating-point uniform 4D vector
// =======================================================================
Standard_Boolean OpenGl_ShaderProgram::SetUniform(const Handle(OpenGl_Context)& theCtx,
                                                  GLint                         theLocation,
                                                  const OpenGl_Vec4&            theValue)
{
  if (myProgramID == NO_PROGRAM || theLocation == INVALID_LOCATION)
  {
    return Standard_False;
  }

  theCtx->core20fwd->glUniform4fv(theLocation, 1, theValue);
  return Standard_True;
}

// =======================================================================
// function : SetUniform
// purpose  :
// =======================================================================
Standard_Boolean OpenGl_ShaderProgram::SetUniform(const Handle(OpenGl_Context)&  theCtx,
                                                  GLint                          theLocation,
                                                  GLuint                         theCount,
                                                  const NCollection_Mat3<float>* theData)
{
  if (myProgramID == NO_PROGRAM || theLocation == INVALID_LOCATION)
  {
    return Standard_False;
  }

  theCtx->core20fwd->glUniformMatrix3fv(theLocation, theCount, GL_FALSE, theData->GetData());
  return Standard_True;
}

// =======================================================================
// function : SetUniform
// purpose  : Specifies the value of the floating-point uniform 4x4 matrix
// =======================================================================
Standard_Boolean OpenGl_ShaderProgram::SetUniform(const Handle(OpenGl_Context)& theCtx,
                                                  GLint                         theLocation,
                                                  const OpenGl_Mat4&            theValue,
                                                  GLboolean                     theTranspose)
{
  if (myProgramID == NO_PROGRAM || theLocation == INVALID_LOCATION)
  {
    return Standard_False;
  }

  theCtx->core20fwd->glUniformMatrix4fv(theLocation,
                                        1,
                                        GL_FALSE,
                                        theTranspose ? theValue.Transposed().GetData()
                                                     : theValue.GetData());
  return Standard_True;
}

// =======================================================================
// function : SetUniform
// purpose  :
// =======================================================================
Standard_Boolean OpenGl_ShaderProgram::SetUniform(const Handle(OpenGl_Context)& theCtx,
                                                  GLint                         theLocation,
                                                  GLuint                        theCount,
                                                  const OpenGl_Mat4*            theData)
{
  if (myProgramID == NO_PROGRAM || theLocation == INVALID_LOCATION)
  {
    return Standard_False;
  }

  theCtx->core20fwd->glUniformMatrix4fv(theLocation, theCount, GL_FALSE, theData->GetData());
  return Standard_True;
}

// =======================================================================
// function : SetUniform
// purpose  : Specifies the value of the float uniform array
// =======================================================================
Standard_Boolean OpenGl_ShaderProgram::SetUniform(const Handle(OpenGl_Context)& theCtx,
                                                  GLint                         theLocation,
                                                  GLuint                        theCount,
                                                  const Standard_ShortReal*     theData)
{
  if (myProgramID == NO_PROGRAM || theLocation == INVALID_LOCATION)
  {
    return Standard_False;
  }

  theCtx->core20fwd->glUniform1fv(theLocation, theCount, theData);
  return Standard_True;
}

// =======================================================================
// function : SetUniform
// purpose  : Specifies the value of the float2 uniform array
// =======================================================================
Standard_Boolean OpenGl_ShaderProgram::SetUniform(const Handle(OpenGl_Context)& theCtx,
                                                  GLint                         theLocation,
                                                  GLuint                        theCount,
                                                  const OpenGl_Vec2*            theData)
{
  if (myProgramID == NO_PROGRAM || theLocation == INVALID_LOCATION)
  {
    return Standard_False;
  }

  theCtx->core20fwd->glUniform2fv(theLocation, theCount, theData[0].GetData());
  return Standard_True;
}

// =======================================================================
// function : SetUniform
// purpose  : Specifies the value of the float3 uniform array
// =======================================================================
Standard_Boolean OpenGl_ShaderProgram::SetUniform(const Handle(OpenGl_Context)& theCtx,
                                                  GLint                         theLocation,
                                                  GLuint                        theCount,
                                                  const OpenGl_Vec3*            theData)
{
  if (myProgramID == NO_PROGRAM || theLocation == INVALID_LOCATION)
  {
    return Standard_False;
  }

  theCtx->core20fwd->glUniform3fv(theLocation, theCount, theData[0].GetData());
  return Standard_True;
}

// =======================================================================
// function : SetUniform
// purpose  : Specifies the value of the float4 uniform array
// =======================================================================
Standard_Boolean OpenGl_ShaderProgram::SetUniform(const Handle(OpenGl_Context)& theCtx,
                                                  GLint                         theLocation,
                                                  GLuint                        theCount,
                                                  const OpenGl_Vec4*            theData)
{
  if (myProgramID == NO_PROGRAM || theLocation == INVALID_LOCATION)
  {
    return Standard_False;
  }

  theCtx->core20fwd->glUniform4fv(theLocation, theCount, theData[0].GetData());
  return Standard_True;
}

// =======================================================================
// function : SetUniform
// purpose  : Specifies the value of the integer uniform array
// =======================================================================
Standard_Boolean OpenGl_ShaderProgram::SetUniform(const Handle(OpenGl_Context)& theCtx,
                                                  GLint                         theLocation,
                                                  GLuint                        theCount,
                                                  const Standard_Integer*       theData)
{
  if (myProgramID == NO_PROGRAM || theLocation == INVALID_LOCATION)
  {
    return Standard_False;
  }

  theCtx->core20fwd->glUniform1iv(theLocation, theCount, theData);
  return Standard_True;
}

// =======================================================================
// function : SetUniform
// purpose  : Specifies the value of the int2 uniform array
// =======================================================================
Standard_Boolean OpenGl_ShaderProgram::SetUniform(const Handle(OpenGl_Context)& theCtx,
                                                  GLint                         theLocation,
                                                  GLuint                        theCount,
                                                  const OpenGl_Vec2i*           theData)
{
  if (myProgramID == NO_PROGRAM || theLocation == INVALID_LOCATION)
  {
    return Standard_False;
  }

  theCtx->core20fwd->glUniform2iv(theLocation, theCount, theData[0].GetData());
  return Standard_True;
}

// =======================================================================
// function : SetUniform
// purpose  : Specifies the value of the int3 uniform array
// =======================================================================
Standard_Boolean OpenGl_ShaderProgram::SetUniform(const Handle(OpenGl_Context)& theCtx,
                                                  GLint                         theLocation,
                                                  GLuint                        theCount,
                                                  const OpenGl_Vec3i*           theData)
{
  if (myProgramID == NO_PROGRAM || theLocation == INVALID_LOCATION)
  {
    return Standard_False;
  }

  theCtx->core20fwd->glUniform3iv(theLocation, theCount, theData[0].GetData());
  return Standard_True;
}

// =======================================================================
// function : SetUniform
// purpose  : Specifies the value of the int4 uniform array
// =======================================================================
Standard_Boolean OpenGl_ShaderProgram::SetUniform(const Handle(OpenGl_Context)& theCtx,
                                                  GLint                         theLocation,
                                                  GLuint                        theCount,
                                                  const OpenGl_Vec4i*           theData)
{
  if (myProgramID == NO_PROGRAM || theLocation == INVALID_LOCATION)
  {
    return Standard_False;
  }

  theCtx->core20fwd->glUniform4iv(theLocation, theCount, theData[0].GetData());
  return Standard_True;
}

// =======================================================================
// function : SetSampler
// purpose  : Specifies the value of the sampler uniform variable
// =======================================================================
Standard_Boolean OpenGl_ShaderProgram::SetSampler(const Handle(OpenGl_Context)& theCtx,
                                                  GLint                         theLocation,
                                                  const Graphic3d_TextureUnit   theTextureUnit)
{
  if (myProgramID == NO_PROGRAM || theLocation == INVALID_LOCATION)
  {
    return Standard_False;
  }

  theCtx->core20fwd->glUniform1i(theLocation, theTextureUnit);
  return Standard_True;
}

// =======================================================================
// function : Create
// purpose  : Creates new empty shader program of specified type
// =======================================================================
Standard_Boolean OpenGl_ShaderProgram::Create(const Handle(OpenGl_Context)& theCtx)
{
  if (myProgramID == NO_PROGRAM && theCtx->core20fwd != NULL)
  {
    myProgramID = theCtx->core20fwd->glCreateProgram();
  }

  return myProgramID != NO_PROGRAM;
}

// =======================================================================
// function : Release
// purpose  : Destroys shader program
// =======================================================================
void OpenGl_ShaderProgram::Release(OpenGl_Context* theCtx)
{
  if (myProgramID == NO_PROGRAM)
  {
    return;
  }

  Standard_ASSERT_RETURN(
    theCtx != NULL,
    "OpenGl_ShaderProgram destroyed without GL context! Possible GPU memory leakage...", );

  for (OpenGl_ShaderList::Iterator anIter(myShaderObjects); anIter.More(); anIter.Next())
  {
    if (!anIter.Value().IsNull())
    {
      anIter.ChangeValue()->Release(theCtx);
      anIter.ChangeValue().Nullify();
    }
  }

  if (theCtx->core20fwd != NULL && theCtx->IsValid())
  {
    theCtx->core20fwd->glDeleteProgram(myProgramID);
  }

  myProgramID = NO_PROGRAM;
}

// =======================================================================
// function : UpdateDebugDump
// purpose  :
// =======================================================================
Standard_Boolean OpenGl_ShaderProgram::UpdateDebugDump(const Handle(OpenGl_Context)&  theCtx,
                                                       const TCollection_AsciiString& theFolder,
                                                       Standard_Boolean               theToBeautify,
                                                       Standard_Boolean               theToReset)
{
  if (myProgramID == NO_PROGRAM)
  {
    return Standard_False;
  }

  TCollection_AsciiString aFolder = theFolder;
  if (aFolder.IsEmpty())
  {
    OSD_Environment aShaderVar("CSF_ShadersDirectoryDump");
    aFolder = aShaderVar.Value();
    if (aFolder.IsEmpty())
    {
      aFolder = ".";
    }
  }

  bool hasUpdates = false;
  for (OpenGl_ShaderList::Iterator anIter(myShaderObjects); anIter.More(); anIter.Next())
  {
    if (!anIter.Value().IsNull())
    {
      // desktop OpenGL (but not OpenGL ES) allows multiple shaders of the same stage to be
      // attached, but here we expect only single source per stage
      hasUpdates = anIter.ChangeValue()->updateDebugDump(theCtx,
                                                         myResourceId,
                                                         aFolder,
                                                         theToBeautify,
                                                         theToReset)
                   || hasUpdates;
    }
  }
  if (hasUpdates)
  {
    return Link(theCtx);
  }
  return Standard_False;
}
