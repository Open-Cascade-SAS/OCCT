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

#include <Graphic3d_ShaderManager.hxx>

#include <Graphic3d_LightSet.hxx>
#include <Graphic3d_ShaderProgram.hxx>
#include <Graphic3d_TextureSetBits.hxx>
#include <Message.hxx>

#include "../Shaders/Shaders_LightShadow_glsl.pxx"
#include "../Shaders/Shaders_PBRDistribution_glsl.pxx"
#include "../Shaders/Shaders_PBRDirectionalLight_glsl.pxx"
#include "../Shaders/Shaders_PBRGeometry_glsl.pxx"
#include "../Shaders/Shaders_PBRFresnel_glsl.pxx"
#include "../Shaders/Shaders_PBRCookTorrance_glsl.pxx"
#include "../Shaders/Shaders_PBRIllumination_glsl.pxx"
#include "../Shaders/Shaders_PBRPointLight_glsl.pxx"
#include "../Shaders/Shaders_PBRSpotLight_glsl.pxx"
#include "../Shaders/Shaders_PBREnvBaking_fs.pxx"
#include "../Shaders/Shaders_PBREnvBaking_vs.pxx"
#include "../Shaders/Shaders_PhongDirectionalLight_glsl.pxx"
#include "../Shaders/Shaders_PhongPointLight_glsl.pxx"
#include "../Shaders/Shaders_PhongSpotLight_glsl.pxx"
#include "../Shaders/Shaders_PointLightAttenuation_glsl.pxx"
#include "../Shaders/Shaders_SkydomBackground_fs.pxx"
#include "../Shaders/Shaders_TangentSpaceNormal_glsl.pxx"

IMPLEMENT_STANDARD_RTTIEXT(Graphic3d_ShaderManager, Standard_Transient)

namespace
{
  //! Number specifying maximum number of light sources to prepare a GLSL program with unrolled loop.
  const Standard_Integer THE_NB_UNROLLED_LIGHTS_MAX = 32;

  //! Compute the size of array storing holding light sources definition.
  static Standard_Integer roundUpMaxLightSources (Standard_Integer theNbLights)
  {
    Standard_Integer aMaxLimit = THE_NB_UNROLLED_LIGHTS_MAX;
    for (; aMaxLimit < theNbLights; aMaxLimit *= 2) {}
    return aMaxLimit;
  }

#define EOL "\n"

//! Compute TexCoord value in Vertex Shader
const char THE_VARY_TexCoord_Trsf[] =
  EOL"  float aRotSin = occTextureTrsf_RotationSin();"
  EOL"  float aRotCos = occTextureTrsf_RotationCos();"
  EOL"  vec2  aTex2   = vec2 (occTexCoord.x * aRotCos - occTexCoord.y * aRotSin,"
  EOL"                        occTexCoord.x * aRotSin + occTexCoord.y * aRotCos);"
  EOL"  aTex2 = (aTex2 + occTextureTrsf_Translation()) * occTextureTrsf_Scale();"
  EOL"  TexCoord = vec4(aTex2, occTexCoord.zw);";

//! Auxiliary function to flip gl_PointCoord vertically
#define THE_VEC2_glPointCoord "vec2 (gl_PointCoord.x, 1.0 - gl_PointCoord.y)"

//! Auxiliary function to transform normal from model to view coordinate system.
const char THE_FUNC_transformNormal_view[] =
  EOL"vec3 transformNormal (in vec3 theNormal)"
  EOL"{"
  EOL"  vec4 aResult = occWorldViewMatrixInverseTranspose"
  EOL"               * occModelWorldMatrixInverseTranspose"
  EOL"               * vec4 (theNormal, 0.0);"
  EOL"  return normalize (aResult.xyz);"
  EOL"}";

//! The same function as THE_FUNC_transformNormal but is used in PBR pipeline.
//! The normals are expected to be in world coordinate system in PBR pipeline.
const char THE_FUNC_transformNormal_world[] =
  EOL"vec3 transformNormal (in vec3 theNormal)"
  EOL"{"
  EOL"  vec4 aResult = occModelWorldMatrixInverseTranspose"
  EOL"               * vec4 (theNormal, 0.0);"
  EOL"  return normalize (aResult.xyz);"
  EOL"}";

//! Global shader variable for color definition with lighting enabled.
const char THE_FUNC_lightDef[] =
  EOL"vec3 Ambient;"   //!< Ambient  contribution of light sources
  EOL"vec3 Diffuse;"   //!< Diffuse  contribution of light sources
  EOL"vec3 Specular;"; //!< Specular contribution of light sources

//! Global shader variable for color definition with lighting enabled.
const char THE_FUNC_PBR_lightDef[] =
  EOL"vec3  DirectLighting;" //!< Accumulator of direct lighting from light sources
  EOL"vec4  BaseColor;"      //!< Base color (albedo) of material for PBR
  EOL"float Metallic;"       //!< Metallic coefficient of material
  EOL"float NormalizedRoughness;" //!< Normalized roughness coefficient of material
  EOL"float Roughness;"      //!< Roughness coefficient of material
  EOL"vec3  Emission;"       //!< Light intensity emitted by material
  EOL"float IOR;";           //!< Material's index of refraction

//! The same as Shaders_PhongDirectionalLight_glsl but for the light with zero index
//! (avoids limitations on some mobile devices).
const char THE_FUNC_directionalLightFirst[] =
  EOL"void directionalLightFirst (in vec3 theNormal,"
  EOL"                            in vec3 theView,"
  EOL"                            in bool theIsFront,"
  EOL"                            in float theShadow)"
  EOL"{"
  EOL"  vec3 aLight = occLight_Position (0);"
  EOL
  EOL"  vec3 aHalf = normalize (aLight + theView);"
  EOL
  EOL"  vec3  aFaceSideNormal = theIsFront ? theNormal : -theNormal;"
  EOL"  float aNdotL = max (0.0, dot (aFaceSideNormal, aLight));"
  EOL"  float aNdotH = max (0.0, dot (aFaceSideNormal, aHalf ));"
  EOL
  EOL"  float aSpecl = 0.0;"
  EOL"  if (aNdotL > 0.0)"
  EOL"  {"
  EOL"    aSpecl = pow (aNdotH, occMaterial_Shininess(theIsFront));"
  EOL"  }"
  EOL
  EOL"  Diffuse  += occLight_Diffuse(0)  * aNdotL * theShadow;"
  EOL"  Specular += occLight_Specular(0) * aSpecl * theShadow;"
  EOL"}";

//! Returns the real cubemap fetching direction considering sides orientation, memory layout and vertical flip.
const char THE_FUNC_cubemap_vector_transform[] =
  EOL"vec3 cubemapVectorTransform (in vec3 theVector,"
  EOL"                             in int  theYCoeff,"
  EOL"                             in int  theZCoeff)"
  EOL"{"
  EOL"  theVector = theVector.yzx;"
  EOL"  theVector.y *= float(theYCoeff);"
  EOL"  theVector.z *= float(theZCoeff);"
  EOL"  return theVector;"
  EOL"}";

//! Process clipping planes in Fragment Shader.
//! Should be added at the beginning of the main() function.
const char THE_FRAG_CLIP_PLANES_N[] =
  EOL"  for (int aPlaneIter = 0; aPlaneIter < occClipPlaneCount; ++aPlaneIter)"
  EOL"  {"
  EOL"    vec4 aClipEquation = occClipPlaneEquations[aPlaneIter];"
  EOL"    if (dot (aClipEquation.xyz, PositionWorld.xyz / PositionWorld.w) + aClipEquation.w < 0.0)"
  EOL"    {"
  EOL"      discard;"
  EOL"    }"
  EOL"  }";

//! Process chains of clipping planes in Fragment Shader.
const char THE_FRAG_CLIP_CHAINS_N[] =
EOL"  for (int aPlaneIter = 0; aPlaneIter < occClipPlaneCount;)"
EOL"  {"
EOL"    vec4 aClipEquation = occClipPlaneEquations[aPlaneIter];"
EOL"    if (dot (aClipEquation.xyz, PositionWorld.xyz / PositionWorld.w) + aClipEquation.w < 0.0)"
EOL"    {"
EOL"      if (occClipPlaneChains[aPlaneIter] == 1)"
EOL"      {"
EOL"        discard;"
EOL"      }"
EOL"      aPlaneIter += 1;"
EOL"    }"
EOL"    else"
EOL"    {"
EOL"      aPlaneIter += occClipPlaneChains[aPlaneIter];"
EOL"    }"
EOL"  }";

//! Process 1 clipping plane in Fragment Shader.
const char THE_FRAG_CLIP_PLANES_1[] =
  EOL"  vec4 aClipEquation0 = occClipPlaneEquations[0];"
  EOL"  if (dot (aClipEquation0.xyz, PositionWorld.xyz / PositionWorld.w) + aClipEquation0.w < 0.0)"
  EOL"  {"
  EOL"    discard;"
  EOL"  }";

//! Process 2 clipping planes in Fragment Shader.
const char THE_FRAG_CLIP_PLANES_2[] =
  EOL"  vec4 aClipEquation0 = occClipPlaneEquations[0];"
  EOL"  vec4 aClipEquation1 = occClipPlaneEquations[1];"
  EOL"  if (dot (aClipEquation0.xyz, PositionWorld.xyz / PositionWorld.w) + aClipEquation0.w < 0.0"
  EOL"   || dot (aClipEquation1.xyz, PositionWorld.xyz / PositionWorld.w) + aClipEquation1.w < 0.0)"
  EOL"  {"
  EOL"    discard;"
  EOL"  }";

//! Process a chain of 2 clipping planes in Fragment Shader (3/4 section).
const char THE_FRAG_CLIP_CHAINS_2[] =
EOL"  vec4 aClipEquation0 = occClipPlaneEquations[0];"
EOL"  vec4 aClipEquation1 = occClipPlaneEquations[1];"
EOL"  if (dot (aClipEquation0.xyz, PositionWorld.xyz / PositionWorld.w) + aClipEquation0.w < 0.0"
EOL"   && dot (aClipEquation1.xyz, PositionWorld.xyz / PositionWorld.w) + aClipEquation1.w < 0.0)"
EOL"  {"
EOL"    discard;"
EOL"  }";

//! Modify color for Wireframe presentation.
const char THE_FRAG_WIREFRAME_COLOR[] =
EOL"vec4 getFinalColor(void)"
EOL"{"
EOL"  float aDistance = min (min (EdgeDistance[0], EdgeDistance[1]), EdgeDistance[2]);"
EOL"  bool isHollow = occWireframeColor.a < 0.0;"
EOL"  float aMixVal = smoothstep (occLineWidth - occLineFeather * 0.5, occLineWidth + occLineFeather * 0.5, aDistance);"
EOL"  vec4 aMixColor = isHollow"
EOL"                 ? vec4 (getColor().rgb, 1.0 - aMixVal)"          // edges only (of interior color)
EOL"                 : mix (occWireframeColor, getColor(), aMixVal);" // interior + edges
EOL"  return aMixColor;"
EOL"}";

//! Compute gl_Position vertex shader output.
const char THE_VERT_gl_Position[] =
EOL"  gl_Position = occProjectionMatrix * occWorldViewMatrix * occModelWorldMatrix * occVertex;";

//! Displace gl_Position alongside vertex normal for outline rendering.
//! This code adds silhouette only for smooth surfaces of closed primitive, and produces visual artifacts on sharp edges.
const char THE_VERT_gl_Position_OUTLINE[] =
EOL"  float anOutlineDisp = occOrthoScale > 0.0 ? occOrthoScale : gl_Position.w;"
EOL"  vec4  anOutlinePos  = occVertex + vec4 (occNormal * (occSilhouetteThickness * anOutlineDisp), 0.0);"
EOL"  gl_Position = occProjectionMatrix * occWorldViewMatrix * occModelWorldMatrix * anOutlinePos;";

}

// =======================================================================
// function : genLightKey
// purpose  :
// =======================================================================
TCollection_AsciiString Graphic3d_ShaderManager::genLightKey (const Handle(Graphic3d_LightSet)& theLights,
                                                              const bool theHasShadowMap) const
{
  if (theLights->NbEnabled() <= THE_NB_UNROLLED_LIGHTS_MAX)
  {
    return theHasShadowMap
         ? TCollection_AsciiString ("ls_") + theLights->KeyEnabledLong()
         : TCollection_AsciiString ("l_")  + theLights->KeyEnabledLong();
  }

  const Standard_Integer aMaxLimit = roundUpMaxLightSources (theLights->NbEnabled());
  return TCollection_AsciiString ("l_") + theLights->KeyEnabledShort() + aMaxLimit;
}

// =======================================================================
// function : Graphic3d_ShaderManager
// purpose  :
// =======================================================================
Graphic3d_ShaderManager::Graphic3d_ShaderManager (Aspect_GraphicsLibrary theGapi)
: myGapi (theGapi),
  // desktop defines a dedicated API for point size, with gl_PointSize added later to GLSL
  myHasFlatShading (true),
  myToReverseDFdxSign (false),
  mySetPointSize (myGapi == Aspect_GraphicsLibrary_OpenGLES),
  myUseRedAlpha (false),
  myToEmulateDepthClamp (true),
  mySRgbState (true)
{
  memset (myGlslExtensions, 0, sizeof(myGlslExtensions));
}

// =======================================================================
// function : ~Graphic3d_ShaderManager
// purpose  :
// =======================================================================
Graphic3d_ShaderManager::~Graphic3d_ShaderManager()
{
  //
}

// =======================================================================
// function : hasGlslBitwiseOps
// purpose  :
// =======================================================================
bool Graphic3d_ShaderManager::hasGlslBitwiseOps() const
{
  switch (myGapi)
  {
    case Aspect_GraphicsLibrary_OpenGL:
    {
      return IsGapiGreaterEqual (3, 0)
          || myGlslExtensions[Graphic3d_GlslExtension_GL_EXT_gpu_shader4];
    }
    case Aspect_GraphicsLibrary_OpenGLES:
    {
      return IsGapiGreaterEqual (3, 0);
    }
  }
  return false;
}

// =======================================================================
// function : defaultGlslVersion
// purpose  :
// =======================================================================
int Graphic3d_ShaderManager::defaultGlslVersion (const Handle(Graphic3d_ShaderProgram)& theProgram,
                                                 const TCollection_AsciiString& theName,
                                                 int theBits,
                                                 bool theUsesDerivates) const
{
  int aBits = theBits;
  const bool toUseDerivates = theUsesDerivates
                          || (theBits & Graphic3d_ShaderFlags_StippleLine) != 0
                          || (theBits & Graphic3d_ShaderFlags_HasTextures) == Graphic3d_ShaderFlags_TextureNormal;
  switch (myGapi)
  {
    case Aspect_GraphicsLibrary_OpenGL:
    {
      if (IsGapiGreaterEqual (3, 2))
      {
        theProgram->SetHeader ("#version 150");
      }
      else
      {
        // TangentSpaceNormal() function uses mat2x3 type
        const bool toUseMat2x3 = (theBits & Graphic3d_ShaderFlags_HasTextures) == Graphic3d_ShaderFlags_TextureNormal;
        // gl_PointCoord has been added since GLSL 1.2
        const bool toUsePointCoord = (theBits & Graphic3d_ShaderFlags_PointSprite) != 0;
        if (toUseMat2x3 || toUsePointCoord)
        {
          if (IsGapiGreaterEqual (2, 1))
          {
            theProgram->SetHeader ("#version 120");
          }
        }
        if ((theBits & Graphic3d_ShaderFlags_StippleLine) != 0
         || theProgram->IsPBR())
        {
          if (IsGapiGreaterEqual (3, 0))
          {
            theProgram->SetHeader ("#version 130");
          }
          else if (myGlslExtensions[Graphic3d_GlslExtension_GL_EXT_gpu_shader4])
          {
            // GL_EXT_gpu_shader4 defines GLSL type "unsigned int", while core GLSL specs define type "uint"
            theProgram->SetHeader ("#extension GL_EXT_gpu_shader4 : enable\n"
                                   "#define uint unsigned int");
          }
        }
      }
      (void )toUseDerivates;
      break;
    }
    case Aspect_GraphicsLibrary_OpenGLES:
    {
    #if defined(__EMSCRIPTEN__)
      if (IsGapiGreaterEqual (3, 0))
      {
        // consider this is browser responsibility to provide working WebGL 2.0 implementation
        // and black-list broken drivers (there is no OpenGL ES greater than 3.0)
        theProgram->SetHeader ("#version 300 es");
      }
    #endif
      // prefer "100 es" on OpenGL ES 3.0- devices (save the features unavailable before "300 es")
      // and    "300 es" on OpenGL ES 3.1+ devices
      if (IsGapiGreaterEqual (3, 1))
      {
        if ((theBits & Graphic3d_ShaderFlags_NeedsGeomShader) != 0)
        {
          theProgram->SetHeader (IsGapiGreaterEqual (3, 2) ? "#version 320 es" : "#version 310 es");
        }
        else
        {
          theProgram->SetHeader ("#version 300 es");
        }
      }
      else
      {
        TCollection_AsciiString aGles2Extensions;
        if (theProgram->IsPBR())
        {
          if (IsGapiGreaterEqual (3, 0))
          {
            theProgram->SetHeader ("#version 300 es");
          }
          else if (myGlslExtensions[Graphic3d_GlslExtension_GL_EXT_shader_texture_lod])
          {
            aGles2Extensions += "#extension GL_EXT_shader_texture_lod : enable\n"
                                "#define textureCubeLod textureCubeLodEXT\n";
          }
        }
        if ((theBits & Graphic3d_ShaderFlags_WriteOit) != 0
         || (theBits & Graphic3d_ShaderFlags_OitDepthPeeling) != 0
         || (theBits & Graphic3d_ShaderFlags_StippleLine) != 0)
        {
          if (IsGapiGreaterEqual (3, 0))
          {
            theProgram->SetHeader ("#version 300 es");
          }
          else
          {
            aBits = aBits & ~Graphic3d_ShaderFlags_WriteOit;
            aBits = aBits & ~Graphic3d_ShaderFlags_OitDepthPeeling;
            if (!myGlslExtensions[Graphic3d_GlslExtension_GL_OES_standard_derivatives])
            {
              aBits = aBits & ~Graphic3d_ShaderFlags_StippleLine;
            }
          }
        }
        if (toUseDerivates)
        {
          if (IsGapiGreaterEqual (3, 0))
          {
            theProgram->SetHeader ("#version 300 es");
          }
          else if (myGlslExtensions[Graphic3d_GlslExtension_GL_OES_standard_derivatives])
          {
            aGles2Extensions += "#extension GL_OES_standard_derivatives : enable\n";
          }
        }

        if (!aGles2Extensions.IsEmpty())
        {
          theProgram->SetHeader (aGles2Extensions);
        }
      }
      break;
    }
  }

  // should fit Graphic3d_ShaderFlags_NB
  char aBitsStr[64];
  Sprintf (aBitsStr, "%04x", aBits);
  theProgram->SetId (TCollection_AsciiString ("occt_") + theName + aBitsStr);
  return aBits;
}

// =======================================================================
// function : defaultOitGlslVersion
// purpose  :
// =======================================================================
void Graphic3d_ShaderManager::defaultOitGlslVersion (const Handle(Graphic3d_ShaderProgram)& theProgram,
                                                     const TCollection_AsciiString& theName,
                                                     bool theMsaa) const
{
  switch (myGapi)
  {
    case Aspect_GraphicsLibrary_OpenGL:
    {
      if (theMsaa)
      {
        if (IsGapiGreaterEqual (4, 0))
        {
          theProgram->SetHeader ("#version 400");
        }
      }
      else
      {
        if (IsGapiGreaterEqual (3, 2))
        {
          theProgram->SetHeader ("#version 150");
        }
      }
      break;
    }
    case Aspect_GraphicsLibrary_OpenGLES:
    {
      if (theMsaa)
      {
        if (IsGapiGreaterEqual (3, 2))
        {
          theProgram->SetHeader ("#version 320 es");
        }
        else if (IsGapiGreaterEqual (3, 0))
        {
          theProgram->SetHeader ("#version 300 es"); // with GL_OES_sample_variables extension
        }
      }
      else
      {
        if (IsGapiGreaterEqual (3, 0))
        {
          theProgram->SetHeader ("#version 300 es");
        }
      }
      break;
    }
  }
  theProgram->SetId (TCollection_AsciiString ("occt_") + theName + (theMsaa ? "_msaa" : ""));
}

// =======================================================================
// function : getStdProgramFont
// purpose  :
// =======================================================================
Handle(Graphic3d_ShaderProgram) Graphic3d_ShaderManager::getStdProgramFont() const
{
  Graphic3d_ShaderObject::ShaderVariableList aUniforms, aStageInOuts;
  aUniforms   .Append (Graphic3d_ShaderObject::ShaderVariable ("sampler2D occSamplerBaseColor", Graphic3d_TOS_FRAGMENT));
  aStageInOuts.Append (Graphic3d_ShaderObject::ShaderVariable ("vec2 TexCoord", Graphic3d_TOS_VERTEX | Graphic3d_TOS_FRAGMENT));

  TCollection_AsciiString aSrcVert = TCollection_AsciiString()
    + EOL"void main()"
      EOL"{"
      EOL"  TexCoord = occTexCoord.st;"
    + THE_VERT_gl_Position
    + EOL"}";

  TCollection_AsciiString
    aSrcGetAlpha = myUseRedAlpha
                 ? EOL"float getAlpha(void) { return occTexture2D(occSamplerBaseColor, TexCoord.st).r; }"
                 : EOL"float getAlpha(void) { return occTexture2D(occSamplerBaseColor, TexCoord.st).a; }";

  TCollection_AsciiString aSrcFrag =
       aSrcGetAlpha
     + EOL"void main()"
       EOL"{"
       EOL"  vec4 aColor = occColor;"
       EOL"  aColor.a *= getAlpha();"
       EOL"  if (aColor.a <= 0.285) discard;"
       EOL"  occSetFragColor (aColor);"
       EOL"}";

  Handle(Graphic3d_ShaderProgram) aProgramSrc = new Graphic3d_ShaderProgram();
  defaultGlslVersion (aProgramSrc, "font", 0);
  aProgramSrc->SetDefaultSampler (false);
  aProgramSrc->SetNbLightsMax (0);
  aProgramSrc->SetNbShadowMaps (0);
  aProgramSrc->SetNbClipPlanesMax (0);
  aProgramSrc->AttachShader (Graphic3d_ShaderObject::CreateFromSource (aSrcVert, Graphic3d_TOS_VERTEX,   aUniforms, aStageInOuts));
  aProgramSrc->AttachShader (Graphic3d_ShaderObject::CreateFromSource (aSrcFrag, Graphic3d_TOS_FRAGMENT, aUniforms, aStageInOuts));
  return aProgramSrc;
}

// =======================================================================
// function : getStdProgramFboBlit
// purpose  :
// =======================================================================
Handle(Graphic3d_ShaderProgram) Graphic3d_ShaderManager::getStdProgramFboBlit (Standard_Integer theNbSamples,
                                                                               Standard_Boolean theIsFallback_sRGB) const
{
  Graphic3d_ShaderObject::ShaderVariableList aUniforms, aStageInOuts;
  aStageInOuts.Append (Graphic3d_ShaderObject::ShaderVariable ("vec2 TexCoord", Graphic3d_TOS_VERTEX | Graphic3d_TOS_FRAGMENT));

  TCollection_AsciiString aSrcVert =
      EOL"void main()"
      EOL"{"
      EOL"  TexCoord    = occVertex.zw;"
      EOL"  gl_Position = vec4(occVertex.x, occVertex.y, 0.0, 1.0);"
      EOL"}";

  TCollection_AsciiString aSrcFrag;
  if (theNbSamples > 1)
  {
    if (myGapi == Aspect_GraphicsLibrary_OpenGLES)
    {
      aUniforms.Append (Graphic3d_ShaderObject::ShaderVariable ("highp sampler2DMS uColorSampler", Graphic3d_TOS_FRAGMENT));
      aUniforms.Append (Graphic3d_ShaderObject::ShaderVariable ("highp sampler2DMS uDepthSampler", Graphic3d_TOS_FRAGMENT));
    }
    else
    {
      aUniforms.Append (Graphic3d_ShaderObject::ShaderVariable ("sampler2DMS uColorSampler", Graphic3d_TOS_FRAGMENT));
      aUniforms.Append (Graphic3d_ShaderObject::ShaderVariable ("sampler2DMS uDepthSampler", Graphic3d_TOS_FRAGMENT));
    }

    aSrcFrag = TCollection_AsciiString()
    + EOL"#define THE_NUM_SAMPLES " + theNbSamples
    + (theIsFallback_sRGB ? EOL"#define THE_SHIFT_sRGB" : "")
    + EOL"void main()"
      EOL"{"
      EOL"  ivec2 aSize  = textureSize (uColorSampler);"
      EOL"  ivec2 anUV   = ivec2 (vec2 (aSize) * TexCoord);"
      EOL"  gl_FragDepth = texelFetch (uDepthSampler, anUV, THE_NUM_SAMPLES / 2 - 1).r;"
      EOL
      EOL"  vec4 aColor = vec4 (0.0);"
      EOL"  for (int aSample = 0; aSample < THE_NUM_SAMPLES; ++aSample)"
      EOL"  {"
      EOL"    vec4 aVal = texelFetch (uColorSampler, anUV, aSample);"
      EOL"    aColor += aVal;"
      EOL"  }"
      EOL"  aColor /= float(THE_NUM_SAMPLES);"
      EOL"#ifdef THE_SHIFT_sRGB"
      EOL"  aColor.rgb = pow (aColor.rgb, vec3 (1.0 / 2.2));"
      EOL"#endif"
      EOL"  occSetFragColor (aColor);"
      EOL"}";
  }
  else
  {
    aUniforms.Append (Graphic3d_ShaderObject::ShaderVariable ("sampler2D uColorSampler", Graphic3d_TOS_FRAGMENT));
    aUniforms.Append (Graphic3d_ShaderObject::ShaderVariable ("sampler2D uDepthSampler", Graphic3d_TOS_FRAGMENT));
    aSrcFrag = TCollection_AsciiString()
    + (theIsFallback_sRGB ? EOL"#define THE_SHIFT_sRGB" : "")
    + EOL"void main()"
      EOL"{"
      EOL"  gl_FragDepth = occTexture2D (uDepthSampler, TexCoord).r;"
      EOL"  vec4  aColor = occTexture2D (uColorSampler, TexCoord);"
      EOL"#ifdef THE_SHIFT_sRGB"
      EOL"  aColor.rgb = pow (aColor.rgb, vec3 (1.0 / 2.2));"
      EOL"#endif"
      EOL"  occSetFragColor (aColor);"
      EOL"}";
  }

  Handle(Graphic3d_ShaderProgram) aProgramSrc = new Graphic3d_ShaderProgram();
  switch (myGapi)
  {
    case Aspect_GraphicsLibrary_OpenGL:
    {
      if (IsGapiGreaterEqual (3, 2))
      {
        aProgramSrc->SetHeader ("#version 150");
      }
      break;
    }
    case Aspect_GraphicsLibrary_OpenGLES:
    {
      if (IsGapiGreaterEqual (3, 1))
      {
        // required for MSAA sampler
        aProgramSrc->SetHeader ("#version 310 es");
      }
      else if (IsGapiGreaterEqual (3, 0))
      {
        aProgramSrc->SetHeader ("#version 300 es");
      }
      else if (myGlslExtensions[Graphic3d_GlslExtension_GL_EXT_frag_depth])
      {
        aProgramSrc->SetHeader ("#extension GL_EXT_frag_depth : enable"
                                EOL"#define gl_FragDepth gl_FragDepthEXT");
      }
      else
      {
        // there is no way to draw into depth buffer
        aSrcFrag =
          EOL"void main()"
          EOL"{"
          EOL"  occSetFragColor (occTexture2D (uColorSampler, TexCoord));"
          EOL"}";
      }
      break;
    }
  }

  TCollection_AsciiString anId = "occt_blit";
  if (theNbSamples > 1)
  {
    anId += TCollection_AsciiString ("_msaa") + theNbSamples;
  }
  if (theIsFallback_sRGB)
  {
    anId += "_gamma";
  }
  aProgramSrc->SetId (anId);
  aProgramSrc->SetDefaultSampler (false);
  aProgramSrc->SetNbLightsMax (0);
  aProgramSrc->SetNbShadowMaps (0);
  aProgramSrc->SetNbClipPlanesMax (0);
  aProgramSrc->AttachShader (Graphic3d_ShaderObject::CreateFromSource (aSrcVert, Graphic3d_TOS_VERTEX,   aUniforms, aStageInOuts));
  aProgramSrc->AttachShader (Graphic3d_ShaderObject::CreateFromSource (aSrcFrag, Graphic3d_TOS_FRAGMENT, aUniforms, aStageInOuts));
  return aProgramSrc;
}

// =======================================================================
// function : getStdProgramOitCompositing
// purpose  :
// =======================================================================
Handle(Graphic3d_ShaderProgram) Graphic3d_ShaderManager::getStdProgramOitCompositing (const Standard_Boolean theMsaa) const
{
  Handle(Graphic3d_ShaderProgram) aProgramSrc = new Graphic3d_ShaderProgram();
  TCollection_AsciiString aSrcVert, aSrcFrag;

  Graphic3d_ShaderObject::ShaderVariableList aUniforms, aStageInOuts;
  aStageInOuts.Append (Graphic3d_ShaderObject::ShaderVariable ("vec2 TexCoord", Graphic3d_TOS_VERTEX | Graphic3d_TOS_FRAGMENT));

  aSrcVert =
    EOL"void main()"
    EOL"{"
    EOL"  TexCoord    = occVertex.zw;"
    EOL"  gl_Position = vec4 (occVertex.x, occVertex.y, 0.0, 1.0);"
    EOL"}";

  if (!theMsaa)
  {
    aUniforms.Append (Graphic3d_ShaderObject::ShaderVariable ("sampler2D uAccumTexture",  Graphic3d_TOS_FRAGMENT));
    aUniforms.Append (Graphic3d_ShaderObject::ShaderVariable ("sampler2D uWeightTexture", Graphic3d_TOS_FRAGMENT));
    aSrcFrag =
      EOL"void main()"
      EOL"{"
      EOL"  vec4 aAccum   = occTexture2D (uAccumTexture,  TexCoord);"
      EOL"  float aWeight = occTexture2D (uWeightTexture, TexCoord).r;"
      EOL"  occSetFragColor (vec4 (aAccum.rgb / max (aWeight, 0.00001), aAccum.a));"
      EOL"}";
  }
  else
  {
    aUniforms.Append (Graphic3d_ShaderObject::ShaderVariable ("sampler2DMS uAccumTexture",  Graphic3d_TOS_FRAGMENT));
    aUniforms.Append (Graphic3d_ShaderObject::ShaderVariable ("sampler2DMS uWeightTexture", Graphic3d_TOS_FRAGMENT));
    aSrcFrag =
      EOL"void main()"
      EOL"{"
      EOL"  ivec2 aTexel  = ivec2 (vec2 (textureSize (uAccumTexture)) * TexCoord);"
      EOL"  vec4 aAccum   = texelFetch (uAccumTexture,  aTexel, gl_SampleID);"
      EOL"  float aWeight = texelFetch (uWeightTexture, aTexel, gl_SampleID).r;"
      EOL"  occSetFragColor (vec4 (aAccum.rgb / max (aWeight, 0.00001), aAccum.a));"
      EOL"}";
  }
  defaultOitGlslVersion (aProgramSrc, "weight_oit", theMsaa);

  aProgramSrc->SetDefaultSampler (false);
  aProgramSrc->SetNbLightsMax (0);
  aProgramSrc->SetNbShadowMaps (0);
  aProgramSrc->SetNbClipPlanesMax (0);
  aProgramSrc->AttachShader (Graphic3d_ShaderObject::CreateFromSource (aSrcVert, Graphic3d_TOS_VERTEX,   aUniforms, aStageInOuts));
  aProgramSrc->AttachShader (Graphic3d_ShaderObject::CreateFromSource (aSrcFrag, Graphic3d_TOS_FRAGMENT, aUniforms, aStageInOuts));
  return aProgramSrc;
}

// =======================================================================
// function : getStdProgramOitDepthPeelingBlend
// purpose  :
// =======================================================================
Handle(Graphic3d_ShaderProgram) Graphic3d_ShaderManager::getStdProgramOitDepthPeelingBlend (Standard_Boolean theMsaa) const
{
  Handle(Graphic3d_ShaderProgram) aProgramSrc = new Graphic3d_ShaderProgram();
  TCollection_AsciiString aSrcVert, aSrcFrag;

  Graphic3d_ShaderObject::ShaderVariableList aUniforms, aStageInOuts;
  aSrcVert =
    EOL"void main()"
    EOL"{"
    EOL"  gl_Position = vec4 (occVertex.x, occVertex.y, 0.0, 1.0);"
    EOL"}";

  aUniforms.Append (Graphic3d_ShaderObject::ShaderVariable (theMsaa
                                                       ? "sampler2DMS uDepthPeelingBackColor"
                                                       :   "sampler2D uDepthPeelingBackColor", Graphic3d_TOS_FRAGMENT));
  aSrcFrag = TCollection_AsciiString()
  + EOL"void main()"
    EOL"{"
    EOL"  #define THE_SAMPLE_ID " + (theMsaa ? "gl_SampleID" : "0")
  + EOL"  occFragColor = texelFetch (uDepthPeelingBackColor, ivec2 (gl_FragCoord.xy), THE_SAMPLE_ID);"
    EOL"  if (occFragColor.a == 0.0) { discard; }"
    EOL"}";

  defaultOitGlslVersion (aProgramSrc, "oit_peeling_blend", theMsaa);
  aProgramSrc->SetDefaultSampler (false);
  aProgramSrc->SetNbLightsMax (0);
  aProgramSrc->SetNbClipPlanesMax (0);
  aProgramSrc->AttachShader (Graphic3d_ShaderObject::CreateFromSource (aSrcVert, Graphic3d_TOS_VERTEX,   aUniforms, aStageInOuts));
  aProgramSrc->AttachShader (Graphic3d_ShaderObject::CreateFromSource (aSrcFrag, Graphic3d_TOS_FRAGMENT, aUniforms, aStageInOuts));
  return aProgramSrc;
}

// =======================================================================
// function : getStdProgramOitDepthPeelingFlush
// purpose  :
// =======================================================================
Handle(Graphic3d_ShaderProgram) Graphic3d_ShaderManager::getStdProgramOitDepthPeelingFlush (Standard_Boolean theMsaa) const
{
  Handle(Graphic3d_ShaderProgram) aProgramSrc = new Graphic3d_ShaderProgram();
  TCollection_AsciiString aSrcVert, aSrcFrag;

  Graphic3d_ShaderObject::ShaderVariableList aUniforms, aStageInOuts;
  aSrcVert =
    EOL"void main()"
    EOL"{"
    EOL"  gl_Position = vec4 (occVertex.x, occVertex.y, 0.0, 1.0);"
    EOL"}";

  aUniforms.Append (Graphic3d_ShaderObject::ShaderVariable (theMsaa
                                                       ? "sampler2DMS uDepthPeelingFrontColor"
                                                       :   "sampler2D uDepthPeelingFrontColor", Graphic3d_TOS_FRAGMENT));
  aUniforms.Append (Graphic3d_ShaderObject::ShaderVariable (theMsaa
                                                       ? "sampler2DMS uDepthPeelingBackColor"
                                                       :   "sampler2D uDepthPeelingBackColor", Graphic3d_TOS_FRAGMENT));
  aSrcFrag = TCollection_AsciiString()
  + EOL"void main()"
    EOL"{"
    EOL"  #define THE_SAMPLE_ID " + (theMsaa ? "gl_SampleID" : "0")
  + EOL"  ivec2 aFragCoord  = ivec2 (gl_FragCoord.xy);"
    EOL"  vec4  aFrontColor = texelFetch (uDepthPeelingFrontColor, aFragCoord, THE_SAMPLE_ID);"
    EOL"  vec4  aBackColor  = texelFetch (uDepthPeelingBackColor,  aFragCoord, THE_SAMPLE_ID);"
    EOL"  float anAlphaMult = 1.0 - aFrontColor.a;"
    EOL"  occFragColor = vec4 (aFrontColor.rgb + anAlphaMult * aBackColor.rgb, aFrontColor.a + aBackColor.a);"
    EOL"}";

  defaultOitGlslVersion (aProgramSrc, "oit_peeling_flush", theMsaa);
  aProgramSrc->SetDefaultSampler (false);
  aProgramSrc->SetNbLightsMax (0);
  aProgramSrc->SetNbClipPlanesMax (0);
  aProgramSrc->AttachShader (Graphic3d_ShaderObject::CreateFromSource (aSrcVert, Graphic3d_TOS_VERTEX,   aUniforms, aStageInOuts));
  aProgramSrc->AttachShader (Graphic3d_ShaderObject::CreateFromSource (aSrcFrag, Graphic3d_TOS_FRAGMENT, aUniforms, aStageInOuts));
  return aProgramSrc;
}

// =======================================================================
// function : pointSpriteAlphaSrc
// purpose  :
// =======================================================================
TCollection_AsciiString Graphic3d_ShaderManager::pointSpriteAlphaSrc (Standard_Integer theBits) const
{
  const bool isAlpha = (theBits & Graphic3d_ShaderFlags_PointSpriteA) == Graphic3d_ShaderFlags_PointSpriteA;
  return isAlpha && myUseRedAlpha
       ? EOL"float getAlpha(void) { return occTexture2D(occSamplerPointSprite, " THE_VEC2_glPointCoord ").r; }"
       : EOL"float getAlpha(void) { return occTexture2D(occSamplerPointSprite, " THE_VEC2_glPointCoord ").a; }";
}

// =======================================================================
// function : pointSpriteShadingSrc
// purpose  :
// =======================================================================
TCollection_AsciiString Graphic3d_ShaderManager::pointSpriteShadingSrc (const TCollection_AsciiString& theBaseColorSrc,
                                                                        Standard_Integer theBits) const
{
  TCollection_AsciiString aSrcFragGetColor;
  if ((theBits & Graphic3d_ShaderFlags_PointSpriteA) == Graphic3d_ShaderFlags_PointSpriteA)
  {
    aSrcFragGetColor = pointSpriteAlphaSrc (theBits) +
      EOL"vec4 getColor(void)"
      EOL"{"
      EOL"  vec4 aColor = " + theBaseColorSrc + ";"
      EOL"  aColor.a = getAlpha();"
      EOL"  if (aColor.a <= 0.1) discard;"
      EOL"  return aColor;"
      EOL"}";
  }
  else if ((theBits & Graphic3d_ShaderFlags_PointSprite) == Graphic3d_ShaderFlags_PointSprite)
  {
    aSrcFragGetColor = TCollection_AsciiString() +
      EOL"vec4 getColor(void)"
      EOL"{"
      EOL"  vec4 aColor = " + theBaseColorSrc + ";"
      EOL"  aColor = occTexture2D(occSamplerPointSprite, " THE_VEC2_glPointCoord ") * aColor;"
      EOL"  if (aColor.a <= 0.1) discard;"
      EOL"  return aColor;"
      EOL"}";
  }

  return aSrcFragGetColor;
}

//! Prepare GLSL source for geometry shader according to parameters.
static TCollection_AsciiString prepareGeomMainSrc (Graphic3d_ShaderObject::ShaderVariableList& theUnifoms,
                                                   Graphic3d_ShaderObject::ShaderVariableList& theStageInOuts,
                                                   Standard_Integer theBits)
{
  if ((theBits & Graphic3d_ShaderFlags_NeedsGeomShader) == 0)
  {
    return TCollection_AsciiString();
  }

  TCollection_AsciiString aSrcMainGeom =
    EOL"void main()"
    EOL"{";

  if ((theBits & Graphic3d_ShaderFlags_MeshEdges) != 0)
  {
    theUnifoms.Append    (Graphic3d_ShaderObject::ShaderVariable ("vec4 occViewport",       Graphic3d_TOS_GEOMETRY));
    theUnifoms.Append    (Graphic3d_ShaderObject::ShaderVariable ("bool occIsQuadMode",     Graphic3d_TOS_GEOMETRY));
    theUnifoms.Append    (Graphic3d_ShaderObject::ShaderVariable ("float occLineWidth",     Graphic3d_TOS_GEOMETRY));
    theUnifoms.Append    (Graphic3d_ShaderObject::ShaderVariable ("float occLineWidth",     Graphic3d_TOS_FRAGMENT));
    theUnifoms.Append    (Graphic3d_ShaderObject::ShaderVariable ("float occLineFeather",   Graphic3d_TOS_FRAGMENT));
    theUnifoms.Append    (Graphic3d_ShaderObject::ShaderVariable ("vec4 occWireframeColor", Graphic3d_TOS_FRAGMENT));
    theStageInOuts.Append(Graphic3d_ShaderObject::ShaderVariable ("vec3 EdgeDistance",      Graphic3d_TOS_GEOMETRY | Graphic3d_TOS_FRAGMENT));

    aSrcMainGeom = TCollection_AsciiString()
    + EOL"vec3 ViewPortTransform (vec4 theVec)"
      EOL"{"
      EOL"  vec3 aWinCoord = theVec.xyz / theVec.w;"
      EOL"  aWinCoord    = aWinCoord * 0.5 + 0.5;"
      EOL"  aWinCoord.xy = aWinCoord.xy * occViewport.zw + occViewport.xy;"
      EOL"  return aWinCoord;"
      EOL"}"
    + aSrcMainGeom
    + EOL"  vec3 aSideA = ViewPortTransform (gl_in[2].gl_Position) - ViewPortTransform (gl_in[1].gl_Position);"
      EOL"  vec3 aSideB = ViewPortTransform (gl_in[2].gl_Position) - ViewPortTransform (gl_in[0].gl_Position);"
      EOL"  vec3 aSideC = ViewPortTransform (gl_in[1].gl_Position) - ViewPortTransform (gl_in[0].gl_Position);"
      EOL"  float aQuadArea = abs (aSideB.x * aSideC.y - aSideB.y * aSideC.x);"
      EOL"  vec3 aLenABC    = vec3 (length (aSideA), length (aSideB), length (aSideC));"
      EOL"  vec3 aHeightABC = vec3 (aQuadArea) / aLenABC;"
      EOL"  aHeightABC = max (aHeightABC, vec3 (10.0 * occLineWidth));" // avoid shrunk presentation disappearing at distance
      EOL"  float aQuadModeHeightC = occIsQuadMode ? occLineWidth + 1.0 : 0.0;";
  }

  for (Standard_Integer aVertIter = 0; aVertIter < 3; ++aVertIter)
  {
    const TCollection_AsciiString aVertIndex (aVertIter);
    // pass variables from Vertex shader to Fragment shader through Geometry shader
    for (Graphic3d_ShaderObject::ShaderVariableList::Iterator aVarListIter (theStageInOuts); aVarListIter.More(); aVarListIter.Next())
    {
      if (aVarListIter.Value().Stages == (Graphic3d_TOS_VERTEX | Graphic3d_TOS_FRAGMENT))
      {
        const TCollection_AsciiString aVarName = aVarListIter.Value().Name.Token (" ", 2);
        if (aVarName.Value (aVarName.Length()) == ']')
        {
          // copy the whole array
          const TCollection_AsciiString aVarName2 = aVarName.Token ("[", 1);
          aSrcMainGeom += TCollection_AsciiString()
            + EOL"  geomOut." + aVarName2 + " = geomIn[" + aVertIndex + "]." + aVarName2 + ";";
        }
        else
        {
          aSrcMainGeom += TCollection_AsciiString()
           + EOL"  geomOut." + aVarName + " = geomIn[" + aVertIndex + "]." + aVarName + ";";
         }
      }
    }

    if ((theBits & Graphic3d_ShaderFlags_MeshEdges) != 0)
    {
      switch (aVertIter)
      {
        case 0: aSrcMainGeom += EOL"  EdgeDistance = vec3 (aHeightABC[0], 0.0, aQuadModeHeightC);"; break;
        case 1: aSrcMainGeom += EOL"  EdgeDistance = vec3 (0.0, aHeightABC[1], aQuadModeHeightC);"; break;
        case 2: aSrcMainGeom += EOL"  EdgeDistance = vec3 (0.0, 0.0, aHeightABC[2]);"; break;
      }
    }
    aSrcMainGeom += TCollection_AsciiString()
     + EOL"  gl_Position = gl_in[" + aVertIndex + "].gl_Position;"
       EOL"  EmitVertex();";
  }
  aSrcMainGeom +=
    EOL"  EndPrimitive();"
    EOL"}";

  return aSrcMainGeom;
}

// =======================================================================
// function : getStdProgramUnlit
// purpose  :
// =======================================================================
Handle(Graphic3d_ShaderProgram) Graphic3d_ShaderManager::getStdProgramUnlit (Standard_Integer theBits,
                                                                             Standard_Boolean theIsOutline) const
{
  Handle(Graphic3d_ShaderProgram) aProgramSrc = new Graphic3d_ShaderProgram();
  TCollection_AsciiString aSrcVert, aSrcVertExtraMain, aSrcVertExtraFunc, aSrcGetAlpha, aSrcVertEndMain;
  TCollection_AsciiString aSrcFrag, aSrcFragExtraMain;
  TCollection_AsciiString aSrcFragGetColor     = EOL"vec4 getColor(void) { return occColor; }";
  TCollection_AsciiString aSrcFragMainGetColor = EOL"  occSetFragColor (getFinalColor());";
  Graphic3d_ShaderObject::ShaderVariableList aUniforms, aStageInOuts;

  if ((theBits & Graphic3d_ShaderFlags_IsPoint) != 0)
  {
    if (mySetPointSize)
    {
      aSrcVertExtraMain += EOL"  gl_PointSize = occPointSize;";
    }

    if ((theBits & Graphic3d_ShaderFlags_PointSprite) != 0)
    {
      aUniforms.Append (Graphic3d_ShaderObject::ShaderVariable ("sampler2D occSamplerPointSprite", Graphic3d_TOS_FRAGMENT));
      if ((theBits & Graphic3d_ShaderFlags_PointSpriteA) != Graphic3d_ShaderFlags_PointSpriteA)
      {
        aSrcFragGetColor =
          EOL"vec4 getColor(void) { return occTexture2D(occSamplerPointSprite, " THE_VEC2_glPointCoord "); }";
      }
      else if ((theBits & Graphic3d_ShaderFlags_TextureRGB) != 0
            && (theBits & Graphic3d_ShaderFlags_VertColor) == 0)
      {
        aProgramSrc->SetTextureSetBits (Graphic3d_TextureSetBits_BaseColor);
        aUniforms   .Append (Graphic3d_ShaderObject::ShaderVariable ("sampler2D occSamplerBaseColor", Graphic3d_TOS_VERTEX));
        aStageInOuts.Append (Graphic3d_ShaderObject::ShaderVariable ("vec4 VertColor", Graphic3d_TOS_VERTEX | Graphic3d_TOS_FRAGMENT));
        aSrcVertExtraMain +=
          EOL"  VertColor = occTexture2D (occSamplerBaseColor, occTexCoord.xy);";
        aSrcFragGetColor =
          EOL"vec4 getColor(void) { return VertColor; }";
      }

      aSrcGetAlpha = pointSpriteAlphaSrc (theBits);
      aSrcFragMainGetColor =
        EOL"  vec4 aColor = getColor();"
        EOL"  aColor.a = getAlpha();"
        EOL"  if (aColor.a <= 0.1) discard;"
        EOL"  occSetFragColor (aColor);";
    }
    else
    {
      if ((theBits & Graphic3d_ShaderFlags_TextureRGB) != 0
       && (theBits & Graphic3d_ShaderFlags_VertColor) == 0)
      {
        aProgramSrc->SetTextureSetBits (Graphic3d_TextureSetBits_BaseColor);
        aUniforms   .Append (Graphic3d_ShaderObject::ShaderVariable ("sampler2D occSamplerBaseColor", Graphic3d_TOS_VERTEX));
        aStageInOuts.Append (Graphic3d_ShaderObject::ShaderVariable ("vec4 VertColor", Graphic3d_TOS_VERTEX | Graphic3d_TOS_FRAGMENT));
        aSrcVertExtraMain +=
          EOL"  VertColor = occTexture2D (occSamplerBaseColor, occTexCoord.xy);";
        aSrcFragGetColor =
          EOL"vec4 getColor(void) { return VertColor; }";
      }

      aSrcFragMainGetColor =
        EOL"  vec4 aColor = getColor();"
        EOL"  if (aColor.a <= 0.1) discard;"
        EOL"  occSetFragColor (aColor);";
    }
  }
  else
  {
    if ((theBits & Graphic3d_ShaderFlags_HasTextures) != 0)
    {
      aUniforms   .Append (Graphic3d_ShaderObject::ShaderVariable ("sampler2D occSamplerBaseColor", Graphic3d_TOS_FRAGMENT));
      aStageInOuts.Append (Graphic3d_ShaderObject::ShaderVariable ("vec4 TexCoord", Graphic3d_TOS_VERTEX | Graphic3d_TOS_FRAGMENT));

      if ((theBits & Graphic3d_ShaderFlags_HasTextures) == Graphic3d_ShaderFlags_TextureEnv)
      {
        aSrcVertExtraFunc = THE_FUNC_transformNormal_view;

        aSrcVertExtraMain +=
          EOL"  vec4 aPosition = occWorldViewMatrix * occModelWorldMatrix * occVertex;"
          EOL"  vec3 aNormal   = transformNormal (occNormal);"
          EOL"  vec3 aReflect  = reflect (normalize (aPosition.xyz), aNormal);"
          EOL"  aReflect.z += 1.0;"
          EOL"  TexCoord = vec4(aReflect.xy * inversesqrt (dot (aReflect, aReflect)) * 0.5 + vec2 (0.5), 0.0, 1.0);";

        aSrcFragGetColor =
          EOL"vec4 getColor(void) { return occTexture2D (occSamplerBaseColor, TexCoord.st); }";
      }
      else
      {
        aProgramSrc->SetTextureSetBits (Graphic3d_TextureSetBits_BaseColor);
        aSrcVertExtraMain += THE_VARY_TexCoord_Trsf;

        aSrcFragGetColor =
          EOL"vec4 getColor(void) { return occTexture2D(occSamplerBaseColor, TexCoord.st / TexCoord.w); }";
      }
    }
  }
  if ((theBits & Graphic3d_ShaderFlags_VertColor) != 0)
  {
    aStageInOuts.Append (Graphic3d_ShaderObject::ShaderVariable ("vec4 VertColor", Graphic3d_TOS_VERTEX | Graphic3d_TOS_FRAGMENT));
    aSrcVertExtraMain += EOL"  VertColor = occVertColor;";
    aSrcFragGetColor  =  EOL"vec4 getColor(void) { return VertColor; }";
  }

  int aNbClipPlanes = 0;
  if ((theBits & Graphic3d_ShaderFlags_ClipPlanesN) != 0)
  {
    aStageInOuts.Append (Graphic3d_ShaderObject::ShaderVariable ("vec4 PositionWorld", Graphic3d_TOS_VERTEX | Graphic3d_TOS_FRAGMENT));
    aSrcVertExtraMain +=
      EOL"  PositionWorld = occModelWorldMatrix * occVertex;";

    if ((theBits & Graphic3d_ShaderFlags_ClipPlanesN) == Graphic3d_ShaderFlags_ClipPlanesN)
    {
      aNbClipPlanes = Graphic3d_ShaderProgram::THE_MAX_CLIP_PLANES_DEFAULT;
      aSrcFragExtraMain += (theBits & Graphic3d_ShaderFlags_ClipChains) != 0
                         ? THE_FRAG_CLIP_CHAINS_N
                         : THE_FRAG_CLIP_PLANES_N;
    }
    else if ((theBits & Graphic3d_ShaderFlags_ClipPlanes1) != 0)
    {
      aNbClipPlanes = 1;
      aSrcFragExtraMain += THE_FRAG_CLIP_PLANES_1;
    }
    else if ((theBits & Graphic3d_ShaderFlags_ClipPlanes2) != 0)
    {
      aNbClipPlanes = 2;
      aSrcFragExtraMain += (theBits & Graphic3d_ShaderFlags_ClipChains) != 0
                         ? THE_FRAG_CLIP_CHAINS_2
                         : THE_FRAG_CLIP_PLANES_2;
    }
  }
  if ((theBits & Graphic3d_ShaderFlags_OitDepthPeeling) != 0)
  {
    aProgramSrc->SetNbFragmentOutputs (3);
    aProgramSrc->SetOitOutput (Graphic3d_RTM_DEPTH_PEELING_OIT);
  }
  else if ((theBits & Graphic3d_ShaderFlags_WriteOit) != 0)
  {
    aProgramSrc->SetNbFragmentOutputs (2);
    aProgramSrc->SetOitOutput (Graphic3d_RTM_BLEND_OIT);
  }

  if (theIsOutline)
  {
    aUniforms.Append (Graphic3d_ShaderObject::ShaderVariable ("float occOrthoScale",          Graphic3d_TOS_VERTEX));
    aUniforms.Append (Graphic3d_ShaderObject::ShaderVariable ("float occSilhouetteThickness", Graphic3d_TOS_VERTEX));
    aSrcVertEndMain = THE_VERT_gl_Position_OUTLINE;
  }
  else if ((theBits & Graphic3d_ShaderFlags_StippleLine) != 0)
  {
    const Standard_Integer aBits = defaultGlslVersion (aProgramSrc, "unlit", theBits);
    if ((aBits & Graphic3d_ShaderFlags_StippleLine) != 0)
    {
      if (hasGlslBitwiseOps())
      {
        aUniforms.Append (Graphic3d_ShaderObject::ShaderVariable ("int   occStipplePattern", Graphic3d_TOS_FRAGMENT));
      }
      else
      {
        aUniforms.Append (Graphic3d_ShaderObject::ShaderVariable ("bool  occStipplePattern[16]", Graphic3d_TOS_FRAGMENT));
      }
      aUniforms.Append (Graphic3d_ShaderObject::ShaderVariable ("float occStippleFactor",  Graphic3d_TOS_FRAGMENT));
      aUniforms.Append (Graphic3d_ShaderObject::ShaderVariable ("vec4 occViewport", Graphic3d_TOS_VERTEX));
      aStageInOuts.Append (Graphic3d_ShaderObject::ShaderVariable ("vec2 ScreenSpaceCoord", Graphic3d_TOS_VERTEX | Graphic3d_TOS_FRAGMENT));
      aSrcVertEndMain =
        EOL"  vec2 aPosition   = gl_Position.xy / gl_Position.w;"
        EOL"  aPosition        = aPosition * 0.5 + 0.5;"
        EOL"  ScreenSpaceCoord = aPosition.xy * occViewport.zw + occViewport.xy;";
      aSrcFragMainGetColor = TCollection_AsciiString()
      + EOL"  vec2 anAxis = vec2 (0.0, 1.0);"
        EOL"  if (abs (dFdx (ScreenSpaceCoord.x)) - abs (dFdy (ScreenSpaceCoord.y)) > 0.001)"
        EOL"  {"
        EOL"    anAxis = vec2 (1.0, 0.0);"
        EOL"  }"
        EOL"  float aRotatePoint = dot (gl_FragCoord.xy, anAxis);"
      + (hasGlslBitwiseOps()
       ? EOL"  uint aBit = uint (floor (aRotatePoint / occStippleFactor + 0.5)) & 15U;"
         EOL"  if ((uint (occStipplePattern) & (1U << aBit)) == 0U) discard;"
       : EOL"  int aBit = int (mod (floor (aRotatePoint / occStippleFactor + 0.5), 16.0));"
         EOL"  if (!occStipplePattern[aBit]) discard;")
      + EOL"  vec4 aColor = getFinalColor();"
        EOL"  if (aColor.a <= 0.1) discard;"
        EOL"  occSetFragColor (aColor);";
    }
    else
    {
      Message::SendWarning ("Warning: stipple lines in GLSL will be ignored");
    }
  }

  aSrcVert =
      aSrcVertExtraFunc
    + EOL"void main()"
      EOL"{"
    + aSrcVertExtraMain
    + THE_VERT_gl_Position
    + aSrcVertEndMain
    + EOL"}";

  TCollection_AsciiString aSrcGeom = prepareGeomMainSrc (aUniforms, aStageInOuts, theBits);
  aSrcFragGetColor += (theBits & Graphic3d_ShaderFlags_MeshEdges) != 0
    ? THE_FRAG_WIREFRAME_COLOR
    : EOL"#define getFinalColor getColor";

  aSrcFrag =
      aSrcFragGetColor
    + aSrcGetAlpha
    + EOL"void main()"
      EOL"{"
      EOL"  if (occFragEarlyReturn()) { return; }"
    + aSrcFragExtraMain
    + aSrcFragMainGetColor
    + EOL"}";

  defaultGlslVersion (aProgramSrc, theIsOutline ? "outline" : "unlit", theBits);
  aProgramSrc->SetDefaultSampler (false);
  aProgramSrc->SetNbLightsMax (0);
  aProgramSrc->SetNbShadowMaps (0);
  aProgramSrc->SetNbClipPlanesMax (aNbClipPlanes);
  aProgramSrc->SetAlphaTest ((theBits & Graphic3d_ShaderFlags_AlphaTest) != 0);
  const Standard_Integer aNbGeomInputVerts = !aSrcGeom.IsEmpty() ? 3 : 0;
  aProgramSrc->AttachShader (Graphic3d_ShaderObject::CreateFromSource (aSrcVert, Graphic3d_TOS_VERTEX,   aUniforms, aStageInOuts, "", "", aNbGeomInputVerts));
  aProgramSrc->AttachShader (Graphic3d_ShaderObject::CreateFromSource (aSrcGeom, Graphic3d_TOS_GEOMETRY, aUniforms, aStageInOuts, "geomIn", "geomOut", aNbGeomInputVerts));
  aProgramSrc->AttachShader (Graphic3d_ShaderObject::CreateFromSource (aSrcFrag, Graphic3d_TOS_FRAGMENT, aUniforms, aStageInOuts, "", "", aNbGeomInputVerts));
  return aProgramSrc;
}

// =======================================================================
// function : stdComputeLighting
// purpose  :
// =======================================================================
TCollection_AsciiString Graphic3d_ShaderManager::stdComputeLighting (Standard_Integer& theNbLights,
                                                                     const Handle(Graphic3d_LightSet)& theLights,
                                                                     Standard_Boolean  theHasVertColor,
                                                                     Standard_Boolean  theIsPBR,
                                                                     Standard_Boolean  theHasTexColor,
                                                                     Standard_Integer  theNbShadowMaps) const
{
  TCollection_AsciiString aLightsFunc, aLightsLoop;
  theNbLights = 0;
  if (!theLights.IsNull())
  {
    theNbLights = theLights->NbEnabled();
    if (theNbLights <= THE_NB_UNROLLED_LIGHTS_MAX)
    {
      Standard_Integer anIndex = 0;
      for (Graphic3d_LightSet::Iterator aLightIter (theLights, Graphic3d_LightSet::IterationFilter_ExcludeDisabledAndAmbient);
           aLightIter.More(); aLightIter.Next())
      {
        switch (aLightIter.Value()->Type())
        {
          case Graphic3d_TypeOfLightSource_Ambient:
          {
            break; // skip ambient
          }
          case Graphic3d_TypeOfLightSource_Directional:
          {
            if (theNbShadowMaps > 0
             && aLightIter.Value()->ToCastShadows())
            {
              aLightsLoop = aLightsLoop +
                EOL"    occDirectionalLight (" + anIndex + ", theNormal, theView, theIsFront,"
                EOL"                         occLightShadow (occShadowMapSamplers[" + anIndex + "], " + anIndex + ", theNormal));";
            }
            else
            {
              aLightsLoop = aLightsLoop + EOL"    occDirectionalLight (" + anIndex + ", theNormal, theView, theIsFront, 1.0);";
            }
            ++anIndex;
            break;
          }
          case Graphic3d_TypeOfLightSource_Positional:
          {
            aLightsLoop = aLightsLoop + EOL"    occPointLight (" + anIndex + ", theNormal, theView, aPoint, theIsFront);";
            ++anIndex;
            break;
          }
          case Graphic3d_TypeOfLightSource_Spot:
          {
            if (theNbShadowMaps > 0
             && aLightIter.Value()->ToCastShadows())
            {
              aLightsLoop = aLightsLoop +
                EOL"    occSpotLight (" + anIndex + ", theNormal, theView, aPoint, theIsFront,"
                EOL"                  occLightShadow (occShadowMapSamplers[" + anIndex + "], " + anIndex + ", theNormal));";
            }
            else
            {
              aLightsLoop = aLightsLoop + EOL"    occSpotLight (" + anIndex + ", theNormal, theView, aPoint, theIsFront, 1.0);";
            }
            ++anIndex;
            break;
          }
        }
      }
    }
    else
    {
      theNbLights = roundUpMaxLightSources (theNbLights);
      bool isFirstInLoop = true;
      aLightsLoop = aLightsLoop +
        EOL"    for (int anIndex = 0; anIndex < occLightSourcesCount; ++anIndex)"
        EOL"    {"
        EOL"      int aType = occLight_Type (anIndex);";
      if (theLights->NbEnabledLightsOfType (Graphic3d_TypeOfLightSource_Directional) > 0)
      {
        isFirstInLoop = false;
        aLightsLoop +=
          EOL"      if (aType == OccLightType_Direct)"
          EOL"      {"
          EOL"        occDirectionalLight (anIndex, theNormal, theView, theIsFront, 1.0);"
          EOL"      }";
      }
      if (theLights->NbEnabledLightsOfType (Graphic3d_TypeOfLightSource_Positional) > 0)
      {
        if (!isFirstInLoop)
        {
          aLightsLoop += EOL"      else ";
        }
        isFirstInLoop = false;
        aLightsLoop +=
          EOL"      if (aType == OccLightType_Point)"
          EOL"      {"
          EOL"        occPointLight (anIndex, theNormal, theView, aPoint, theIsFront);"
          EOL"      }";
      }
      if (theLights->NbEnabledLightsOfType (Graphic3d_TypeOfLightSource_Spot) > 0)
      {
        if (!isFirstInLoop)
        {
          aLightsLoop += EOL"      else ";
        }
        isFirstInLoop = false;
        aLightsLoop +=
          EOL"      if (aType == OccLightType_Spot)"
          EOL"      {"
          EOL"        occSpotLight (anIndex, theNormal, theView, aPoint, theIsFront, 1.0);"
          EOL"      }";
      }
      aLightsLoop += EOL"    }";
    }

    if (theIsPBR)
    {
      aLightsFunc += Shaders_PBRDistribution_glsl;
      aLightsFunc += Shaders_PBRGeometry_glsl;
      aLightsFunc += Shaders_PBRFresnel_glsl;
      aLightsFunc += Shaders_PBRCookTorrance_glsl;
      aLightsFunc += Shaders_PBRIllumination_glsl;
    }

    bool isShadowShaderAdded = false;
    if (theLights->NbEnabledLightsOfType (Graphic3d_TypeOfLightSource_Directional) == 1
     && theNbLights == 1
     && !theIsPBR
     && theNbShadowMaps == 0)
    {
      // use the version with hard-coded first index
      aLightsLoop = EOL"    directionalLightFirst(theNormal, theView, theIsFront, 1.0);";
      aLightsFunc += THE_FUNC_directionalLightFirst;
    }
    else if (theLights->NbEnabledLightsOfType (Graphic3d_TypeOfLightSource_Directional) > 0)
    {
      if (theNbShadowMaps > 0 && !isShadowShaderAdded)
      {
        aLightsFunc += Shaders_LightShadow_glsl;
        isShadowShaderAdded = true;
      }
      aLightsFunc += theIsPBR ? Shaders_PBRDirectionalLight_glsl : Shaders_PhongDirectionalLight_glsl;
    }
    if (theLights->NbEnabledLightsOfType (Graphic3d_TypeOfLightSource_Positional) > 0)
    {
      aLightsFunc += theIsPBR ? Shaders_PBRPointLight_glsl : Shaders_PhongPointLight_glsl;
    }
    if (theLights->NbEnabledLightsOfType (Graphic3d_TypeOfLightSource_Spot) > 0)
    {
      if (theNbShadowMaps > 0 && !isShadowShaderAdded)
      {
        aLightsFunc += Shaders_LightShadow_glsl;
      }
      aLightsFunc += theIsPBR ? Shaders_PBRSpotLight_glsl : Shaders_PhongSpotLight_glsl;
    }
  }

  if (!theIsPBR)
  {
    return TCollection_AsciiString()
    + THE_FUNC_lightDef
    + Shaders_PointLightAttenuation_glsl
    + aLightsFunc
    + EOL
      EOL"vec4 computeLighting (in vec3 theNormal,"
      EOL"                      in vec3 theView,"
      EOL"                      in vec4 thePoint,"
      EOL"                      in bool theIsFront)"
      EOL"{"
      EOL"  Ambient  = occLightAmbient.rgb;"
      EOL"  Diffuse  = vec3 (0.0);"
      EOL"  Specular = vec3 (0.0);"
      EOL"  vec3 aPoint = thePoint.xyz / thePoint.w;"
    + aLightsLoop
    + EOL"  vec3 aMatAmbient  = occMaterial_Ambient(theIsFront);"
      EOL"  vec4 aMatDiffuse  = occMaterial_Diffuse(theIsFront);"
      EOL"  vec3 aMatSpecular = occMaterial_Specular(theIsFront);"
      EOL"  vec4 aColor = vec4(Ambient * aMatAmbient + Diffuse * aMatDiffuse.rgb + Specular * aMatSpecular, aMatDiffuse.a);"
    + (theHasVertColor ?
      EOL"  aColor *= getVertColor();" : "")
    + (theHasTexColor ?
      EOL"#if defined(THE_HAS_TEXTURE_COLOR) && defined(FRAGMENT_SHADER)"
      EOL"  aColor *= occTexture2D(occSamplerBaseColor, TexCoord.st / TexCoord.w);"
      EOL"#endif" : "")
    + EOL"  occMaterialOcclusion(aColor.rgb, TexCoord.st / TexCoord.w);"
      EOL"  vec3 aMatEmission = occMaterialEmission(theIsFront, TexCoord.st / TexCoord.w);"
      EOL"  aColor.rgb += aMatEmission.rgb;"
      EOL"  return aColor;"
      EOL"}";
  }
  else
  {
    return TCollection_AsciiString()
    + THE_FUNC_PBR_lightDef
    + Shaders_PointLightAttenuation_glsl
    + aLightsFunc
    + EOL
      EOL"vec4 computeLighting (in vec3 theNormal,"
      EOL"                      in vec3 theView,"
      EOL"                      in vec4 thePoint,"
      EOL"                      in bool theIsFront)"
      EOL"{"
      EOL"  DirectLighting = vec3(0.0);"
      EOL"  BaseColor           = occMaterialBaseColor(theIsFront, TexCoord.st / TexCoord.w)" + (theHasVertColor ? " * getVertColor()" : "") + ";"
    + EOL"  Emission            = occMaterialEmission(theIsFront, TexCoord.st / TexCoord.w);"
      EOL"  Metallic            = occMaterialMetallic(theIsFront, TexCoord.st / TexCoord.w);"
      EOL"  NormalizedRoughness = occMaterialRoughness(theIsFront, TexCoord.st / TexCoord.w);"
      EOL"  Roughness = occRoughness (NormalizedRoughness);"
      EOL"  IOR       = occPBRMaterial_IOR (theIsFront);"
      EOL"  vec3 aPoint = thePoint.xyz / thePoint.w;"
    + aLightsLoop
    + EOL"  vec3 aColor = DirectLighting;"
      EOL"  vec3 anIndirectLightingSpec = occPBRFresnel (BaseColor.rgb, Metallic, IOR);"
      EOL"  vec2 aCoeff = occTexture2D (occEnvLUT, vec2(abs(dot(theView, theNormal)), NormalizedRoughness)).xy;"
      EOL"  anIndirectLightingSpec *= aCoeff.x;"
      EOL"  anIndirectLightingSpec += aCoeff.y;"
      EOL"  anIndirectLightingSpec *= occTextureCubeLod (occSpecIBLMap, -reflect (theView, theNormal), NormalizedRoughness * float (occNbSpecIBLLevels - 1)).rgb;"
      EOL"  vec3 aRefractionCoeff = 1.0 - occPBRFresnel (BaseColor.rgb, Metallic, NormalizedRoughness, IOR, abs(dot(theView, theNormal)));"
      EOL"  aRefractionCoeff *= (1.0 - Metallic);"
      EOL"  vec3 anIndirectLightingDiff = aRefractionCoeff * BaseColor.rgb * BaseColor.a;"
      EOL"  anIndirectLightingDiff *= occDiffIBLMap (theNormal).rgb;"
      EOL"  aColor += occLightAmbient.rgb * (anIndirectLightingDiff + anIndirectLightingSpec);"
      EOL"  aColor += Emission;"
      EOL"  occMaterialOcclusion(aColor, TexCoord.st / TexCoord.w);"
      EOL"  return vec4 (aColor, mix(1.0, BaseColor.a, aRefractionCoeff.x));"
      EOL"}";
  }
}

// =======================================================================
// function : getStdProgramGouraud
// purpose  :
// =======================================================================
Handle(Graphic3d_ShaderProgram) Graphic3d_ShaderManager::getStdProgramGouraud (const Handle(Graphic3d_LightSet)& theLights,
                                                                               Standard_Integer theBits) const
{
  Handle(Graphic3d_ShaderProgram) aProgramSrc = new Graphic3d_ShaderProgram();
  TCollection_AsciiString aSrcVert, aSrcVertColor, aSrcVertExtraMain;
  TCollection_AsciiString aSrcFrag, aSrcFragExtraMain;
  TCollection_AsciiString aSrcFragGetColor = EOL"vec4 getColor(void) { return gl_FrontFacing ? FrontColor : BackColor; }";
  Graphic3d_ShaderObject::ShaderVariableList aUniforms, aStageInOuts;
  bool toUseTexColor = false;
  if ((theBits & Graphic3d_ShaderFlags_IsPoint) != 0)
  {
    if (mySetPointSize)
    {
      aSrcVertExtraMain += EOL"  gl_PointSize = occPointSize;";
    }

    if ((theBits & Graphic3d_ShaderFlags_PointSprite) != 0)
    {
      aUniforms.Append (Graphic3d_ShaderObject::ShaderVariable ("sampler2D occSamplerPointSprite", Graphic3d_TOS_FRAGMENT));
      aSrcFragGetColor = pointSpriteShadingSrc ("gl_FrontFacing ? FrontColor : BackColor", theBits);
    }

    if ((theBits & Graphic3d_ShaderFlags_TextureRGB) != 0
     && (theBits & Graphic3d_ShaderFlags_VertColor) == 0)
    {
      aProgramSrc->SetTextureSetBits (Graphic3d_TextureSetBits_BaseColor);
      aUniforms.Append (Graphic3d_ShaderObject::ShaderVariable ("sampler2D occSamplerBaseColor", Graphic3d_TOS_VERTEX));
      aSrcVertColor = EOL"vec4 getVertColor(void) { return occTexture2D (occSamplerBaseColor, occTexCoord.xy); }";
    }
  }
  else
  {
    if ((theBits & Graphic3d_ShaderFlags_TextureRGB) != 0)
    {
      toUseTexColor = true;
      aProgramSrc->SetTextureSetBits (Graphic3d_TextureSetBits_BaseColor);
      aUniforms   .Append (Graphic3d_ShaderObject::ShaderVariable ("sampler2D occSamplerBaseColor", Graphic3d_TOS_FRAGMENT));
      aStageInOuts.Append (Graphic3d_ShaderObject::ShaderVariable ("vec4 TexCoord", Graphic3d_TOS_VERTEX | Graphic3d_TOS_FRAGMENT));
      aSrcVertExtraMain += THE_VARY_TexCoord_Trsf;

      aSrcFragGetColor =
        EOL"vec4 getColor(void)"
        EOL"{"
        EOL"  vec4 aColor = gl_FrontFacing ? FrontColor : BackColor;"
        EOL"  return occTexture2D(occSamplerBaseColor, TexCoord.st / TexCoord.w) * aColor;"
        EOL"}";
    }
  }

  if ((theBits & Graphic3d_ShaderFlags_VertColor) != 0)
  {
    aSrcVertColor = EOL"vec4 getVertColor(void) { return occVertColor; }";
  }

  int aNbClipPlanes = 0;
  if ((theBits & Graphic3d_ShaderFlags_ClipPlanesN) != 0)
  {
    aStageInOuts.Append (Graphic3d_ShaderObject::ShaderVariable ("vec4 PositionWorld", Graphic3d_TOS_VERTEX | Graphic3d_TOS_FRAGMENT));
    aSrcVertExtraMain +=
      EOL"  PositionWorld = aPositionWorld;";

    if ((theBits & Graphic3d_ShaderFlags_ClipPlanesN) == Graphic3d_ShaderFlags_ClipPlanesN)
    {
      aNbClipPlanes = Graphic3d_ShaderProgram::THE_MAX_CLIP_PLANES_DEFAULT;
      aSrcFragExtraMain += (theBits & Graphic3d_ShaderFlags_ClipChains) != 0
                         ? THE_FRAG_CLIP_CHAINS_N
                         : THE_FRAG_CLIP_PLANES_N;
    }
    else if ((theBits & Graphic3d_ShaderFlags_ClipPlanes1) != 0)
    {
      aNbClipPlanes = 1;
      aSrcFragExtraMain += THE_FRAG_CLIP_PLANES_1;
    }
    else if ((theBits & Graphic3d_ShaderFlags_ClipPlanes2) != 0)
    {
      aNbClipPlanes = 2;
      aSrcFragExtraMain += (theBits & Graphic3d_ShaderFlags_ClipChains) != 0
                          ? THE_FRAG_CLIP_CHAINS_2
                          : THE_FRAG_CLIP_PLANES_2;
    }
  }
  if ((theBits & Graphic3d_ShaderFlags_OitDepthPeeling) != 0)
  {
    aProgramSrc->SetNbFragmentOutputs (3);
    aProgramSrc->SetOitOutput (Graphic3d_RTM_DEPTH_PEELING_OIT);
  }
  else if ((theBits & Graphic3d_ShaderFlags_WriteOit) != 0)
  {
    aProgramSrc->SetNbFragmentOutputs (2);
    aProgramSrc->SetOitOutput (Graphic3d_RTM_BLEND_OIT);
  }

  aStageInOuts.Append (Graphic3d_ShaderObject::ShaderVariable ("vec4 FrontColor", Graphic3d_TOS_VERTEX | Graphic3d_TOS_FRAGMENT));
  aStageInOuts.Append (Graphic3d_ShaderObject::ShaderVariable ("vec4 BackColor",  Graphic3d_TOS_VERTEX | Graphic3d_TOS_FRAGMENT));

  Standard_Integer aNbLights = 0;
  const TCollection_AsciiString aLights = stdComputeLighting (aNbLights, theLights, !aSrcVertColor.IsEmpty(), false, toUseTexColor, 0);
  aSrcVert = TCollection_AsciiString()
    + THE_FUNC_transformNormal_world
    + EOL
    + aSrcVertColor
    + aLights
    + EOL"void main()"
      EOL"{"
      EOL"  vec4 aPositionWorld = occModelWorldMatrix * occVertex;"
      EOL"  vec3 aNormal        = transformNormal (occNormal);"
      EOL"  vec3 aView;"
      EOL"  if (occProjectionMatrix[3][3] == 1.0)"
      EOL"  {"
      EOL"    aView = (occWorldViewMatrixInverse * vec4(0.0, 0.0, 1.0, 0.0)).xyz;"
      EOL"  }"
      EOL"  else"
      EOL"  {"
      EOL"    vec3 anEye = (occWorldViewMatrixInverse * vec4(0.0, 0.0, 0.0, 1.0)).xyz;"
      EOL"    aView = normalize (anEye - aPositionWorld.xyz);"
      EOL"  }"
      EOL"  FrontColor  = computeLighting (aNormal, aView, aPositionWorld, true);"
      EOL"  BackColor   = computeLighting (aNormal, aView, aPositionWorld, false);"
    + aSrcVertExtraMain
    + THE_VERT_gl_Position
    + EOL"}";

  TCollection_AsciiString aSrcGeom = prepareGeomMainSrc (aUniforms, aStageInOuts, theBits);
  aSrcFragGetColor += (theBits & Graphic3d_ShaderFlags_MeshEdges) != 0
    ? THE_FRAG_WIREFRAME_COLOR
    : EOL"#define getFinalColor getColor";

  aSrcFrag = TCollection_AsciiString()
    + aSrcFragGetColor
    + EOL"void main()"
      EOL"{"
      EOL"  if (occFragEarlyReturn()) { return; }"
    + aSrcFragExtraMain
    + EOL"  occSetFragColor (getFinalColor());"
    + EOL"}";

  const TCollection_AsciiString aProgId = TCollection_AsciiString ("gouraud-") + genLightKey (theLights, false) + "-";
  defaultGlslVersion (aProgramSrc, aProgId, theBits);
  aProgramSrc->SetDefaultSampler (false);
  aProgramSrc->SetNbLightsMax (aNbLights);
  aProgramSrc->SetNbShadowMaps (0);
  aProgramSrc->SetNbClipPlanesMax (aNbClipPlanes);
  aProgramSrc->SetAlphaTest ((theBits & Graphic3d_ShaderFlags_AlphaTest) != 0);
  const Standard_Integer aNbGeomInputVerts = !aSrcGeom.IsEmpty() ? 3 : 0;
  aProgramSrc->AttachShader (Graphic3d_ShaderObject::CreateFromSource (aSrcVert, Graphic3d_TOS_VERTEX,   aUniforms, aStageInOuts, "", "", aNbGeomInputVerts));
  aProgramSrc->AttachShader (Graphic3d_ShaderObject::CreateFromSource (aSrcGeom, Graphic3d_TOS_GEOMETRY, aUniforms, aStageInOuts, "geomIn", "geomOut", aNbGeomInputVerts));
  aProgramSrc->AttachShader (Graphic3d_ShaderObject::CreateFromSource (aSrcFrag, Graphic3d_TOS_FRAGMENT, aUniforms, aStageInOuts, "", "", aNbGeomInputVerts));
  return aProgramSrc;
}

// =======================================================================
// function : getStdProgramPhong
// purpose  :
// =======================================================================
Handle(Graphic3d_ShaderProgram) Graphic3d_ShaderManager::getStdProgramPhong (const Handle(Graphic3d_LightSet)& theLights,
                                                                             const Standard_Integer theBits,
                                                                             const Standard_Boolean theIsFlatNormal,
                                                                             const Standard_Boolean theIsPBR,
                                                                             const Standard_Integer theNbShadowMaps) const
{
  TCollection_AsciiString aPhongCompLight = TCollection_AsciiString() +
    "computeLighting (normalize (Normal), normalize (View), PositionWorld, gl_FrontFacing)";
  const bool isFlatNormal = theIsFlatNormal && myHasFlatShading;
  const char* aDFdxSignReversion = myToReverseDFdxSign ? "-" : "";
  bool toUseTexColor = false;
  if (isFlatNormal != theIsFlatNormal)
  {
    Message::SendWarning ("Warning: flat shading requires OpenGL ES 3.0+ or GL_OES_standard_derivatives extension");
  }
  else if (isFlatNormal && myToReverseDFdxSign)
  {
    Message::SendWarning ("Warning: applied workaround for GLSL flat shading normal computation using dFdx/dFdy on Adreno");
  }

  Handle(Graphic3d_ShaderProgram) aProgramSrc = new Graphic3d_ShaderProgram();
  aProgramSrc->SetPBR (theIsPBR); // should be set before defaultGlslVersion()

  TCollection_AsciiString aSrcVert, aSrcVertExtraFunc, aSrcVertExtraMain;
  TCollection_AsciiString aSrcFrag, aSrcFragGetVertColor, aSrcFragExtraMain;
  TCollection_AsciiString aSrcFragGetColor = TCollection_AsciiString() + EOL"vec4 getColor(void) { return " + aPhongCompLight +  "; }";
  Graphic3d_ShaderObject::ShaderVariableList aUniforms, aStageInOuts;
  if ((theBits & Graphic3d_ShaderFlags_IsPoint) != 0)
  {
    if (mySetPointSize)
    {
      aSrcVertExtraMain += EOL"  gl_PointSize = occPointSize;";
    }

    if ((theBits & Graphic3d_ShaderFlags_PointSprite) != 0)
    {
      aUniforms.Append (Graphic3d_ShaderObject::ShaderVariable ("sampler2D occSamplerPointSprite", Graphic3d_TOS_FRAGMENT));
      aSrcFragGetColor = pointSpriteShadingSrc (aPhongCompLight, theBits);
    }

    if ((theBits & Graphic3d_ShaderFlags_TextureRGB) != 0
     && (theBits & Graphic3d_ShaderFlags_VertColor) == 0)
    {
      aProgramSrc->SetTextureSetBits (Graphic3d_TextureSetBits_BaseColor);
      aUniforms   .Append (Graphic3d_ShaderObject::ShaderVariable ("sampler2D occSamplerBaseColor", Graphic3d_TOS_VERTEX));
      aStageInOuts.Append (Graphic3d_ShaderObject::ShaderVariable ("vec4 VertColor", Graphic3d_TOS_VERTEX | Graphic3d_TOS_FRAGMENT));

      aSrcVertExtraMain   += EOL"  VertColor = occTexture2D (occSamplerBaseColor, occTexCoord.xy);";
      aSrcFragGetVertColor = EOL"vec4 getVertColor(void) { return VertColor; }";
    }
  }
  else
  {
    if ((theBits & Graphic3d_ShaderFlags_TextureRGB) != 0)
    {
      toUseTexColor = true;
      aUniforms   .Append (Graphic3d_ShaderObject::ShaderVariable ("sampler2D occSamplerBaseColor", Graphic3d_TOS_FRAGMENT));
      aStageInOuts.Append (Graphic3d_ShaderObject::ShaderVariable ("vec4 TexCoord", Graphic3d_TOS_VERTEX | Graphic3d_TOS_FRAGMENT));
      aSrcVertExtraMain += THE_VARY_TexCoord_Trsf;

      Standard_Integer aTextureBits = Graphic3d_TextureSetBits_BaseColor | Graphic3d_TextureSetBits_Occlusion | Graphic3d_TextureSetBits_Emissive;
      if (theIsPBR)
      {
        aTextureBits |= Graphic3d_TextureSetBits_MetallicRoughness;
      }
      if ((theBits & Graphic3d_ShaderFlags_HasTextures) == Graphic3d_ShaderFlags_TextureNormal
       && !isFlatNormal)
      {
        if (myHasFlatShading)
        {
          aTextureBits |= Graphic3d_TextureSetBits_Normal;
        }
        else
        {
          Message::SendWarning ("Warning: ignoring Normal Map texture in GLSL due to hardware capabilities");
        }
      }
      aProgramSrc->SetTextureSetBits (aTextureBits);
    }
  }

  if ((theBits & Graphic3d_ShaderFlags_VertColor) != 0)
  {
    aStageInOuts.Append (Graphic3d_ShaderObject::ShaderVariable ("vec4 VertColor", Graphic3d_TOS_VERTEX | Graphic3d_TOS_FRAGMENT));
    aSrcVertExtraMain   += EOL"  VertColor = occVertColor;";
    aSrcFragGetVertColor = EOL"vec4 getVertColor(void) { return VertColor; }";
  }

  int aNbClipPlanes = 0;
  if ((theBits & Graphic3d_ShaderFlags_ClipPlanesN) != 0)
  {
    if ((theBits & Graphic3d_ShaderFlags_ClipPlanesN) == Graphic3d_ShaderFlags_ClipPlanesN)
    {
      aNbClipPlanes = Graphic3d_ShaderProgram::THE_MAX_CLIP_PLANES_DEFAULT;
      aSrcFragExtraMain += (theBits & Graphic3d_ShaderFlags_ClipChains) != 0
                         ? THE_FRAG_CLIP_CHAINS_N
                         : THE_FRAG_CLIP_PLANES_N;
    }
    else if ((theBits & Graphic3d_ShaderFlags_ClipPlanes1) != 0)
    {
      aNbClipPlanes = 1;
      aSrcFragExtraMain += THE_FRAG_CLIP_PLANES_1;
    }
    else if ((theBits & Graphic3d_ShaderFlags_ClipPlanes2) != 0)
    {
      aNbClipPlanes = 2;
      aSrcFragExtraMain += (theBits & Graphic3d_ShaderFlags_ClipChains) != 0
                         ? THE_FRAG_CLIP_CHAINS_2
                         : THE_FRAG_CLIP_PLANES_2;
    }
  }
  if ((theBits & Graphic3d_ShaderFlags_OitDepthPeeling) != 0)
  {
    aProgramSrc->SetNbFragmentOutputs (3);
    aProgramSrc->SetOitOutput (Graphic3d_RTM_DEPTH_PEELING_OIT);
  }
  else if ((theBits & Graphic3d_ShaderFlags_WriteOit) != 0)
  {
    aProgramSrc->SetNbFragmentOutputs (2);
    aProgramSrc->SetOitOutput (Graphic3d_RTM_BLEND_OIT);
  }

  if (isFlatNormal)
  {
    aSrcFragExtraMain += TCollection_AsciiString()
      + EOL"  Normal = " + aDFdxSignReversion + "normalize (cross (dFdx (PositionWorld.xyz / PositionWorld.w), dFdy (PositionWorld.xyz / PositionWorld.w)));"
        EOL"  if (!gl_FrontFacing) { Normal = -Normal; }";
  }
  else
  {
    aStageInOuts.Append(Graphic3d_ShaderObject::ShaderVariable("vec3 vNormal", Graphic3d_TOS_VERTEX | Graphic3d_TOS_FRAGMENT));
    aSrcVertExtraFunc += THE_FUNC_transformNormal_world;
    aSrcVertExtraMain += EOL"  vNormal = transformNormal (occNormal);";
    aSrcFragExtraMain += EOL"  Normal = vNormal;";

    if ((theBits & Graphic3d_ShaderFlags_IsPoint) == 0
     && (theBits & Graphic3d_ShaderFlags_HasTextures) == Graphic3d_ShaderFlags_TextureNormal
     && myHasFlatShading)
    {
      aSrcFrag += Shaders_TangentSpaceNormal_glsl;
      // apply normal map texture
      aSrcFragExtraMain +=
        EOL"#if defined(THE_HAS_TEXTURE_NORMAL)"
        EOL"  vec2 aTexCoord = TexCoord.st / TexCoord.w;"
        EOL"  vec4 aMapNormalValue = occTextureNormal(aTexCoord);"
        EOL"  if (aMapNormalValue.w > 0.5)"
        EOL"  {"
        EOL"    mat2 aDeltaUVMatrix = mat2 (dFdx(aTexCoord), dFdy(aTexCoord));"
        EOL"    mat2x3 aDeltaVectorMatrix = mat2x3 (dFdx (PositionWorld.xyz), dFdy (PositionWorld.xyz));"
        EOL"    Normal = TangentSpaceNormal (aDeltaUVMatrix, aDeltaVectorMatrix, aMapNormalValue.xyz, Normal, !gl_FrontFacing);"
        EOL"  }"
        EOL"#endif";
    }
  }

  aStageInOuts.Append (Graphic3d_ShaderObject::ShaderVariable ("vec4 PositionWorld", Graphic3d_TOS_VERTEX | Graphic3d_TOS_FRAGMENT));
  aStageInOuts.Append (Graphic3d_ShaderObject::ShaderVariable ("vec3 View",          Graphic3d_TOS_VERTEX | Graphic3d_TOS_FRAGMENT));
  if (theNbShadowMaps > 0)
  {
    aUniforms.Append (Graphic3d_ShaderObject::ShaderVariable ("mat4      occShadowMapMatrices[THE_NB_SHADOWMAPS]", Graphic3d_TOS_VERTEX));
    aUniforms.Append (Graphic3d_ShaderObject::ShaderVariable ("sampler2D occShadowMapSamplers[THE_NB_SHADOWMAPS]", Graphic3d_TOS_FRAGMENT));
    aUniforms.Append (Graphic3d_ShaderObject::ShaderVariable ("vec2      occShadowMapSizeBias",                    Graphic3d_TOS_FRAGMENT));

    aStageInOuts.Append (Graphic3d_ShaderObject::ShaderVariable ("vec4 PosLightSpace[THE_NB_SHADOWMAPS]", Graphic3d_TOS_VERTEX | Graphic3d_TOS_FRAGMENT));
    aSrcVertExtraMain +=
      EOL"  for (int aShadowIter = 0; aShadowIter < THE_NB_SHADOWMAPS; ++aShadowIter)"
      EOL"  {"
      EOL"    PosLightSpace[aShadowIter] = occShadowMapMatrices[aShadowIter] * PositionWorld;"
      EOL"  }";
  }

  aSrcVert = TCollection_AsciiString()
    + aSrcVertExtraFunc
    + EOL"void main()"
      EOL"{"
      EOL"  PositionWorld = occModelWorldMatrix * occVertex;"
      EOL"  if (occProjectionMatrix[3][3] == 1.0)"
      EOL"  {"
      EOL"    View = (occWorldViewMatrixInverse * vec4(0.0, 0.0, 1.0, 0.0)).xyz;"
      EOL"  }"
      EOL"  else"
      EOL"  {"
      EOL"    vec3 anEye = (occWorldViewMatrixInverse * vec4(0.0, 0.0, 0.0, 1.0)).xyz;"
      EOL"    View = normalize (anEye - PositionWorld.xyz);"
      EOL"  }"
    + aSrcVertExtraMain
    + THE_VERT_gl_Position
    + EOL"}";

  TCollection_AsciiString aSrcGeom = prepareGeomMainSrc (aUniforms, aStageInOuts, theBits);
  aSrcFragGetColor += (theBits & Graphic3d_ShaderFlags_MeshEdges) != 0
    ? THE_FRAG_WIREFRAME_COLOR
    : EOL"#define getFinalColor getColor";

  Standard_Integer aNbLights = 0;
  const TCollection_AsciiString aLights = stdComputeLighting (aNbLights, theLights, !aSrcFragGetVertColor.IsEmpty(),
                                                              theIsPBR, toUseTexColor, theNbShadowMaps);
  aSrcFrag += TCollection_AsciiString()
    + EOL
    + aSrcFragGetVertColor
    + EOL"vec3  Normal;"
    + aLights
    + aSrcFragGetColor
    + EOL
      EOL"void main()"
      EOL"{"
      EOL"  if (occFragEarlyReturn()) { return; }"
    + aSrcFragExtraMain
    + EOL"  occSetFragColor (getFinalColor());"
    + EOL"}";

  const TCollection_AsciiString aProgId = TCollection_AsciiString (theIsFlatNormal ? "flat-" : "phong-") + (theIsPBR ? "pbr-" : "")
                                        + genLightKey (theLights, theNbShadowMaps > 0) + "-";
  defaultGlslVersion (aProgramSrc, aProgId, theBits, isFlatNormal);
  aProgramSrc->SetDefaultSampler (false);
  aProgramSrc->SetNbLightsMax (aNbLights);
  aProgramSrc->SetNbShadowMaps (theNbShadowMaps);
  aProgramSrc->SetNbClipPlanesMax (aNbClipPlanes);
  aProgramSrc->SetAlphaTest ((theBits & Graphic3d_ShaderFlags_AlphaTest) != 0);

  const Standard_Integer aNbGeomInputVerts = !aSrcGeom.IsEmpty() ? 3 : 0;
  aProgramSrc->AttachShader (Graphic3d_ShaderObject::CreateFromSource (aSrcVert, Graphic3d_TOS_VERTEX,   aUniforms, aStageInOuts, "", "", aNbGeomInputVerts));
  aProgramSrc->AttachShader (Graphic3d_ShaderObject::CreateFromSource (aSrcGeom, Graphic3d_TOS_GEOMETRY, aUniforms, aStageInOuts, "geomIn", "geomOut", aNbGeomInputVerts));
  aProgramSrc->AttachShader (Graphic3d_ShaderObject::CreateFromSource (aSrcFrag, Graphic3d_TOS_FRAGMENT, aUniforms, aStageInOuts, "", "", aNbGeomInputVerts));
  return aProgramSrc;
}

// =======================================================================
// function : getStdProgramStereo
// purpose  :
// =======================================================================
Handle(Graphic3d_ShaderProgram) Graphic3d_ShaderManager::getStdProgramStereo (Graphic3d_StereoMode theStereoMode) const
{
  Handle(Graphic3d_ShaderProgram) aProgramSrc = new Graphic3d_ShaderProgram();
  Graphic3d_ShaderObject::ShaderVariableList aUniforms, aStageInOuts;

  aStageInOuts.Append (Graphic3d_ShaderObject::ShaderVariable ("vec2 TexCoord", Graphic3d_TOS_VERTEX | Graphic3d_TOS_FRAGMENT));
  TCollection_AsciiString aSrcVert =
      EOL"void main()"
      EOL"{"
      EOL"  TexCoord    = occVertex.zw;"
      EOL"  gl_Position = vec4(occVertex.x, occVertex.y, 0.0, 1.0);"
      EOL"}";

  TCollection_AsciiString aSrcFrag;
  aUniforms.Append (Graphic3d_ShaderObject::ShaderVariable ("sampler2D uLeftSampler",  Graphic3d_TOS_FRAGMENT));
  aUniforms.Append (Graphic3d_ShaderObject::ShaderVariable ("sampler2D uRightSampler", Graphic3d_TOS_FRAGMENT));
  const char* aName = "stereo";
  switch (theStereoMode)
  {
    case Graphic3d_StereoMode_Anaglyph:
    {
      aName = "anaglyph";
      aUniforms.Append (Graphic3d_ShaderObject::ShaderVariable ("mat4 uMultL", Graphic3d_TOS_FRAGMENT));
      aUniforms.Append (Graphic3d_ShaderObject::ShaderVariable ("mat4 uMultR", Graphic3d_TOS_FRAGMENT));
      const TCollection_AsciiString aNormalize = mySRgbState
                                               ? EOL"#define sRgb2linear(theColor) theColor"
                                                 EOL"#define linear2sRgb(theColor) theColor"
                                               : EOL"#define sRgb2linear(theColor) pow(theColor, vec4(2.2, 2.2, 2.2, 1.0))"
                                                 EOL"#define linear2sRgb(theColor) pow(theColor, 1.0 / vec4(2.2, 2.2, 2.2, 1.0))";
      aSrcFrag = aNormalize
      + EOL"void main()"
        EOL"{"
        EOL"  vec4 aColorL = occTexture2D (uLeftSampler,  TexCoord);"
        EOL"  vec4 aColorR = occTexture2D (uRightSampler, TexCoord);"
        EOL"  aColorL = sRgb2linear (aColorL);"
        EOL"  aColorR = sRgb2linear (aColorR);"
        EOL"  vec4 aColor = uMultR * aColorR + uMultL * aColorL;"
        EOL"  occSetFragColor (linear2sRgb (aColor));"
        EOL"}";
      break;
    }
    case Graphic3d_StereoMode_RowInterlaced:
    {
      aName = "row-interlaced";
      aUniforms.Append (Graphic3d_ShaderObject::ShaderVariable ("vec2 uTexOffset", Graphic3d_TOS_FRAGMENT));
      aSrcFrag =
          EOL"void main()"
          EOL"{"
          EOL"  vec2 aTexCoordL = TexCoord - uTexOffset;"
          EOL"  vec2 aTexCoordR = TexCoord + uTexOffset;"
          EOL"  vec4 aColorL = occTexture2D (uLeftSampler,  aTexCoordL);"
          EOL"  vec4 aColorR = occTexture2D (uRightSampler, aTexCoordR);"
          EOL"  if (int (mod (gl_FragCoord.y - 1023.5, 2.0)) != 1)"
          EOL"  {"
          EOL"    occSetFragColor (aColorL);"
          EOL"  }"
          EOL"  else"
          EOL"  {"
          EOL"    occSetFragColor (aColorR);"
          EOL"  }"
          EOL"}";
      break;
    }
    case Graphic3d_StereoMode_ColumnInterlaced:
    {
      aName = "column-interlaced";
      aUniforms.Append (Graphic3d_ShaderObject::ShaderVariable ("vec2 uTexOffset", Graphic3d_TOS_FRAGMENT));
      aSrcFrag =
          EOL"void main()"
          EOL"{"
          EOL"  vec2 aTexCoordL = TexCoord - uTexOffset;"
          EOL"  vec2 aTexCoordR = TexCoord + uTexOffset;"
          EOL"  vec4 aColorL = occTexture2D (uLeftSampler,  aTexCoordL);"
          EOL"  vec4 aColorR = occTexture2D (uRightSampler, aTexCoordR);"
          EOL"  if (int (mod (gl_FragCoord.x - 1023.5, 2.0)) == 1)"
          EOL"  {"
          EOL"    occSetFragColor (aColorL);"
          EOL"  }"
          EOL"  else"
          EOL"  {"
          EOL"    occSetFragColor (aColorR);"
          EOL"  }"
          EOL"}";
      break;
    }
    case Graphic3d_StereoMode_ChessBoard:
    {
      aName = "chessboard";
      aUniforms.Append (Graphic3d_ShaderObject::ShaderVariable ("vec2 uTexOffset", Graphic3d_TOS_FRAGMENT));
      aSrcFrag =
          EOL"void main()"
          EOL"{"
          EOL"  vec2 aTexCoordL = TexCoord - uTexOffset;"
          EOL"  vec2 aTexCoordR = TexCoord + uTexOffset;"
          EOL"  vec4 aColorL = occTexture2D (uLeftSampler,  aTexCoordL);"
          EOL"  vec4 aColorR = occTexture2D (uRightSampler, aTexCoordR);"
          EOL"  bool isEvenX = int(mod(floor(gl_FragCoord.x - 1023.5), 2.0)) != 1;"
          EOL"  bool isEvenY = int(mod(floor(gl_FragCoord.y - 1023.5), 2.0)) == 1;"
          EOL"  if ((isEvenX && isEvenY) || (!isEvenX && !isEvenY))"
          EOL"  {"
          EOL"    occSetFragColor (aColorL);"
          EOL"  }"
          EOL"  else"
          EOL"  {"
          EOL"    occSetFragColor (aColorR);"
          EOL"  }"
          EOL"}";
      break;
    }
    case Graphic3d_StereoMode_SideBySide:
    {
      aName = "sidebyside";
      aSrcFrag =
          EOL"void main()"
          EOL"{"
          EOL"  vec2 aTexCoord = vec2 (TexCoord.x * 2.0, TexCoord.y);"
          EOL"  if (TexCoord.x > 0.5)"
          EOL"  {"
          EOL"    aTexCoord.x -= 1.0;"
          EOL"  }"
          EOL"  vec4 aColorL = occTexture2D (uLeftSampler,  aTexCoord);"
          EOL"  vec4 aColorR = occTexture2D (uRightSampler, aTexCoord);"
          EOL"  if (TexCoord.x <= 0.5)"
          EOL"  {"
          EOL"    occSetFragColor (aColorL);"
          EOL"  }"
          EOL"  else"
          EOL"  {"
          EOL"    occSetFragColor (aColorR);"
          EOL"  }"
          EOL"}";
      break;
    }
    case Graphic3d_StereoMode_OverUnder:
    {
      aName = "overunder";
      aSrcFrag =
          EOL"void main()"
          EOL"{"
          EOL"  vec2 aTexCoord = vec2 (TexCoord.x, TexCoord.y * 2.0);"
          EOL"  if (TexCoord.y > 0.5)"
          EOL"  {"
          EOL"    aTexCoord.y -= 1.0;"
          EOL"  }"
          EOL"  vec4 aColorL = occTexture2D (uLeftSampler,  aTexCoord);"
          EOL"  vec4 aColorR = occTexture2D (uRightSampler, aTexCoord);"
          EOL"  if (TexCoord.y <= 0.5)"
          EOL"  {"
          EOL"    occSetFragColor (aColorL);"
          EOL"  }"
          EOL"  else"
          EOL"  {"
          EOL"    occSetFragColor (aColorR);"
          EOL"  }"
          EOL"}";
      break;
    }
    case Graphic3d_StereoMode_QuadBuffer:
    case Graphic3d_StereoMode_SoftPageFlip:
    case Graphic3d_StereoMode_OpenVR:
    default:
    {
      aSrcFrag =
          EOL"void main()"
          EOL"{"
          EOL"  vec4 aColorL = occTexture2D (uLeftSampler,  TexCoord);"
          EOL"  vec4 aColorR = occTexture2D (uRightSampler, TexCoord);"
          EOL"  aColorL.b = 0.0;"
          EOL"  aColorL.g = 0.0;"
          EOL"  aColorR.r = 0.0;"
          EOL"  occSetFragColor (aColorL + aColorR);"
          EOL"}";
      break;
    }
  }

  defaultGlslVersion (aProgramSrc, aName, 0);
  aProgramSrc->SetDefaultSampler (false);
  aProgramSrc->SetNbLightsMax (0);
  aProgramSrc->SetNbShadowMaps (0);
  aProgramSrc->SetNbClipPlanesMax (0);
  aProgramSrc->AttachShader (Graphic3d_ShaderObject::CreateFromSource (aSrcVert, Graphic3d_TOS_VERTEX,   aUniforms, aStageInOuts));
  aProgramSrc->AttachShader (Graphic3d_ShaderObject::CreateFromSource (aSrcFrag, Graphic3d_TOS_FRAGMENT, aUniforms, aStageInOuts));
  return aProgramSrc;
}

// =======================================================================
// function : getStdProgramBoundBox
// purpose  :
// =======================================================================
Handle(Graphic3d_ShaderProgram) Graphic3d_ShaderManager::getStdProgramBoundBox() const
{
  Handle(Graphic3d_ShaderProgram) aProgramSrc = new Graphic3d_ShaderProgram();

  Graphic3d_ShaderObject::ShaderVariableList aUniforms, aStageInOuts;
  aUniforms.Append (Graphic3d_ShaderObject::ShaderVariable ("vec3 occBBoxCenter", Graphic3d_TOS_VERTEX));
  aUniforms.Append (Graphic3d_ShaderObject::ShaderVariable ("vec3 occBBoxSize",   Graphic3d_TOS_VERTEX));

  TCollection_AsciiString aSrcVert =
    EOL"void main()"
    EOL"{"
    EOL"  vec4 aCenter = vec4(occVertex.xyz * occBBoxSize + occBBoxCenter, 1.0);"
    EOL"  vec4 aPos    = vec4(occVertex.xyz * occBBoxSize + occBBoxCenter, 1.0);"
    EOL"  gl_Position = occProjectionMatrix * occWorldViewMatrix * occModelWorldMatrix * aPos;"
    EOL"}";

  TCollection_AsciiString aSrcFrag =
    EOL"void main()"
    EOL"{"
    EOL"  occSetFragColor (occColor);"
    EOL"}";

  defaultGlslVersion (aProgramSrc, "bndbox", 0);
  aProgramSrc->SetDefaultSampler (false);
  aProgramSrc->SetNbLightsMax (0);
  aProgramSrc->SetNbShadowMaps (0);
  aProgramSrc->SetNbClipPlanesMax (0);
  aProgramSrc->AttachShader (Graphic3d_ShaderObject::CreateFromSource (aSrcVert, Graphic3d_TOS_VERTEX,   aUniforms, aStageInOuts));
  aProgramSrc->AttachShader (Graphic3d_ShaderObject::CreateFromSource (aSrcFrag, Graphic3d_TOS_FRAGMENT, aUniforms, aStageInOuts));
  return aProgramSrc;
}

// =======================================================================
// function : getPBREnvBakingProgram
// purpose  :
// =======================================================================
Handle(Graphic3d_ShaderProgram) Graphic3d_ShaderManager::getPBREnvBakingProgram (Standard_Integer theIndex) const
{
  Standard_ASSERT_RAISE (theIndex >= 0 && theIndex <= 2,"");
  Handle(Graphic3d_ShaderProgram) aProgramSrc = new Graphic3d_ShaderProgram();
  aProgramSrc->SetPBR (true); // should be set before defaultGlslVersion()

  Graphic3d_ShaderObject::ShaderVariableList aUniforms, aStageInOuts;

  TCollection_AsciiString aSrcVert = TCollection_AsciiString()
  + THE_FUNC_cubemap_vector_transform
  + Shaders_PBREnvBaking_vs;

  TCollection_AsciiString aSrcFrag = TCollection_AsciiString()
  + THE_FUNC_cubemap_vector_transform
  + Shaders_PBRDistribution_glsl
  + ((theIndex == 0 || theIndex == 2) ? "\n#define THE_TO_BAKE_DIFFUSE\n" : "\n#define THE_TO_BAKE_SPECULAR\n")
  + (theIndex == 2 ? "\n#define THE_TO_PACK_FLOAT\n" : "")
  + Shaders_PBREnvBaking_fs;

  // constant array definition requires OpenGL 2.1+ or OpenGL ES 3.0+
  switch (myGapi)
  {
    case Aspect_GraphicsLibrary_OpenGL:
    {
      aProgramSrc->SetHeader ("#version 120");
      break;
    }
    case Aspect_GraphicsLibrary_OpenGLES:
    {
      if (IsGapiGreaterEqual (3, 0))
      {
        aProgramSrc->SetHeader ("#version 300 es");
      }
      else if (myGlslExtensions[Graphic3d_GlslExtension_GL_EXT_shader_texture_lod])
      {
        aProgramSrc->SetHeader ("#extension GL_EXT_shader_texture_lod : enable\n"
                                "#define textureCubeLod textureCubeLodEXT");
      }
      else
      {
        Message::SendWarning ("Warning: incomplete PBR lighting implementation due to missing OpenGL ES 3.0 or GL_EXT_shader_texture_lod support.");
      }
      break;
    }
  }

  static const char* THE_BAKE_NAMES[3] = { "pbr_env_baking_diffuse", "pbr_env_baking_specular", "pbr_env_baking_difffallback" };
  defaultGlslVersion (aProgramSrc, THE_BAKE_NAMES[theIndex], 0);
  aProgramSrc->SetDefaultSampler (false);
  aProgramSrc->SetNbLightsMax (0);
  aProgramSrc->SetNbShadowMaps (0);
  aProgramSrc->SetNbClipPlanesMax (0);
  aProgramSrc->AttachShader (Graphic3d_ShaderObject::CreateFromSource (aSrcVert, Graphic3d_TOS_VERTEX,   aUniforms, aStageInOuts));
  aProgramSrc->AttachShader (Graphic3d_ShaderObject::CreateFromSource (aSrcFrag, Graphic3d_TOS_FRAGMENT, aUniforms, aStageInOuts));
  return aProgramSrc;
}

// =======================================================================
// function : getBgCubeMapProgram
// purpose  :
// =======================================================================
Handle(Graphic3d_ShaderProgram) Graphic3d_ShaderManager::getBgCubeMapProgram() const
{
  Handle(Graphic3d_ShaderProgram) aProgSrc = new Graphic3d_ShaderProgram();

  Graphic3d_ShaderObject::ShaderVariableList aUniforms, aStageInOuts;
  aStageInOuts.Append (Graphic3d_ShaderObject::ShaderVariable("vec3 ViewDirection", Graphic3d_TOS_VERTEX | Graphic3d_TOS_FRAGMENT));
  aUniforms.Append (Graphic3d_ShaderObject::ShaderVariable ("samplerCube occSampler0", Graphic3d_TOS_FRAGMENT));
  aUniforms.Append (Graphic3d_ShaderObject::ShaderVariable ("int uYCoeff", Graphic3d_TOS_VERTEX));
  aUniforms.Append (Graphic3d_ShaderObject::ShaderVariable ("int uZCoeff", Graphic3d_TOS_VERTEX));

  TCollection_AsciiString aSrcVert = TCollection_AsciiString()
  + THE_FUNC_cubemap_vector_transform
  + EOL"void main()"
    EOL"{"
    EOL"  ViewDirection = cubemapVectorTransform (occVertex.xyz, uYCoeff, uZCoeff);"
    EOL"  vec4 aPos = occProjectionMatrix * occWorldViewMatrix * vec4(occVertex.xyz, 1.0);"
    // setting Z to W ensures that final Z will be 1.0 after perspective division, (w/w=1))
    // which allows rendering skybox after everything else with depth test enabled (GL_LEQUAL)
    EOL"  gl_Position = aPos.xyww;"
    EOL"}";

  TCollection_AsciiString aDepthClamp;
  if (myToEmulateDepthClamp)
  {
    // workaround Z clamping issues on some GPUs
    aDepthClamp = EOL"  gl_FragDepth = clamp (gl_FragDepth, 0.0, 1.0);";
    if (myGapi == Aspect_GraphicsLibrary_OpenGLES)
    {
      if (IsGapiGreaterEqual (3, 0))
      {
        aProgSrc->SetHeader ("#version 300 es");
      }
      else if (myGlslExtensions[Graphic3d_GlslExtension_GL_EXT_frag_depth])
      {
        aProgSrc->SetHeader ("#extension GL_EXT_frag_depth : enable"
                          EOL"#define gl_FragDepth gl_FragDepthEXT");
      }
      else
      {
        aDepthClamp.Clear();
      }
    }
  }

  TCollection_AsciiString aSrcFrag = TCollection_AsciiString()
  + EOL"#define occEnvCubemap occSampler0"
    EOL"void main()"
    EOL"{"
    EOL"  occSetFragColor (vec4(occTextureCube (occEnvCubemap, ViewDirection).rgb, 1.0));"
  + aDepthClamp
  + EOL"}";

  defaultGlslVersion (aProgSrc, "background_cubemap", 0);
  aProgSrc->SetDefaultSampler (false);
  aProgSrc->SetNbLightsMax (0);
  aProgSrc->SetNbShadowMaps (0);
  aProgSrc->SetNbClipPlanesMax (0);
  aProgSrc->AttachShader (Graphic3d_ShaderObject::CreateFromSource (aSrcVert, Graphic3d_TOS_VERTEX,   aUniforms, aStageInOuts));
  aProgSrc->AttachShader (Graphic3d_ShaderObject::CreateFromSource (aSrcFrag, Graphic3d_TOS_FRAGMENT, aUniforms, aStageInOuts));
  return aProgSrc;
}

// =======================================================================
// function : getBgSkydomeProgram
// purpose  :
// =======================================================================
Handle(Graphic3d_ShaderProgram) Graphic3d_ShaderManager::getBgSkydomeProgram() const
{
  Handle(Graphic3d_ShaderProgram) aProgSrc = new Graphic3d_ShaderProgram();

  Graphic3d_ShaderObject::ShaderVariableList aUniforms, aStageInOuts;
  aStageInOuts.Append (Graphic3d_ShaderObject::ShaderVariable ("vec2 TexCoord", Graphic3d_TOS_VERTEX | Graphic3d_TOS_FRAGMENT));

  TCollection_AsciiString aSrcVert = TCollection_AsciiString()
  + EOL"void main()"
    EOL"{"
    EOL"  gl_Position = vec4 (occVertex.xy, 0.0, 1.0);"
    EOL"  TexCoord    = 0.5 * gl_Position.xy + vec2 (0.5);"
    EOL"}";

  TCollection_AsciiString aSrcFrag = Shaders_SkydomBackground_fs;

  if (myGapi == Aspect_GraphicsLibrary_OpenGL)
  {
    aProgSrc->SetHeader ("#version 130");
  }
  else if (myGapi == Aspect_GraphicsLibrary_OpenGLES)
  {
    if (IsGapiGreaterEqual (3, 0))
    {
      aProgSrc->SetHeader ("#version 300 es");
    }
  }
  aProgSrc->AttachShader (Graphic3d_ShaderObject::CreateFromSource (aSrcVert, Graphic3d_TOS_VERTEX, aUniforms, aStageInOuts));
  aProgSrc->AttachShader (Graphic3d_ShaderObject::CreateFromSource (aSrcFrag, Graphic3d_TOS_FRAGMENT, aUniforms, aStageInOuts));

  return aProgSrc;
}

// =======================================================================
// function : getColoredQuadProgram
// purpose  :
// =======================================================================
Handle(Graphic3d_ShaderProgram) Graphic3d_ShaderManager::getColoredQuadProgram() const
{
  Handle(Graphic3d_ShaderProgram) aProgSrc = new Graphic3d_ShaderProgram();

  Graphic3d_ShaderObject::ShaderVariableList aUniforms, aStageInOuts;
  aStageInOuts.Append (Graphic3d_ShaderObject::ShaderVariable ("vec2 TexCoord", Graphic3d_TOS_VERTEX | Graphic3d_TOS_FRAGMENT));
  aUniforms.Append (Graphic3d_ShaderObject::ShaderVariable ("vec3 uColor1", Graphic3d_TOS_FRAGMENT));
  aUniforms.Append (Graphic3d_ShaderObject::ShaderVariable ("vec3 uColor2", Graphic3d_TOS_FRAGMENT));

  TCollection_AsciiString aSrcVert = TCollection_AsciiString()
  + EOL"void main()"
    EOL"{"
    EOL"  TexCoord    = occTexCoord.st;"
    EOL"  gl_Position = occProjectionMatrix * occWorldViewMatrix * occModelWorldMatrix * occVertex;"
    EOL"}";

  TCollection_AsciiString aSrcFrag = TCollection_AsciiString()
  + EOL"void main()"
    EOL"{"
    EOL"  vec3 c1 = mix (uColor1, uColor2, TexCoord.x);"
    EOL"  occSetFragColor (vec4 (mix (uColor2, c1, TexCoord.y), 1.0));"
    EOL"}";

  defaultGlslVersion (aProgSrc, "colored_quad", 0);
  aProgSrc->AttachShader (Graphic3d_ShaderObject::CreateFromSource (aSrcVert, Graphic3d_TOS_VERTEX,   aUniforms, aStageInOuts));
  aProgSrc->AttachShader (Graphic3d_ShaderObject::CreateFromSource (aSrcFrag, Graphic3d_TOS_FRAGMENT, aUniforms, aStageInOuts));

  return aProgSrc;
}
