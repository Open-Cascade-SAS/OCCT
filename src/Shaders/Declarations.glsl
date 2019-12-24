
//! @file Declarations.glsl includes definition of common uniform variables in OCCT GLSL programs
//! @def THE_MAX_LIGHTS
//! Specifies the length of array of lights, which is 8 by default. Defined by Shader Manager.
// #define THE_MAX_LIGHTS 8

//! @def THE_MAX_CLIP_PLANES
//! Specifies the length of array of clipping planes, which is 8 by default. Defined by Shader Manager.
// #define THE_MAX_CLIP_PLANES 8

//! @def THE_NB_FRAG_OUTPUTS
//! Specifies the length of array of Fragment Shader outputs, which is 1 by default. Defined by Shader Manager.
// #define THE_NB_FRAG_OUTPUTS 1

// compatibility macros
#if (__VERSION__ >= 130)
  #define THE_ATTRIBUTE  in
  #define THE_SHADER_IN  in
  #define THE_SHADER_OUT out
  #define THE_OUT        out
  #define occTexture1D   texture
  #define occTexture2D   texture
  #define occTexture3D   texture
  #define occTextureCube texture
  #define occTextureCubeLod textureLod
#else
  #define THE_ATTRIBUTE  attribute
  #define THE_SHADER_IN  varying
  #define THE_SHADER_OUT varying
  #define THE_OUT
  #define occTexture1D   texture1D
  #define occTexture2D   texture2D
  #define occTexture3D   texture3D
  #define occTextureCube textureCube
  #define occTextureCubeLod textureCubeLod
#endif

#ifdef GL_ES
#if (__VERSION__ >= 300)
  #define THE_PREC_ENUM highp // lowp should be enough for enums but triggers driver bugs
#else
  #define THE_PREC_ENUM lowp
#endif
#else
  #define THE_PREC_ENUM
#endif

// Vertex attributes
#ifdef VERTEX_SHADER
  THE_ATTRIBUTE vec4 occVertex;
  THE_ATTRIBUTE vec3 occNormal;
  THE_ATTRIBUTE vec4 occTexCoord;
  THE_ATTRIBUTE vec4 occVertColor;
#elif defined(FRAGMENT_SHADER)
  #if (__VERSION__ >= 130)
    #ifdef OCC_ENABLE_draw_buffers
      out vec4 occFragColorArray[THE_NB_FRAG_OUTPUTS];
      #define occFragColorArrayAlias occFragColorArray
      #define occFragColor0 occFragColorArray[0]
    #else
      out vec4 occFragColor0;
    #endif
  #else
    #ifdef OCC_ENABLE_draw_buffers
      #define occFragColorArrayAlias gl_FragData
      #define occFragColor0 gl_FragData[0]
    #else
      #define occFragColor0 gl_FragColor
    #endif
  #endif

  #if (THE_NB_FRAG_OUTPUTS >= 2)
    #define occFragColor1 occFragColorArrayAlias[1]
  #else
    vec4 occFragColor1;
  #endif
  #if (THE_NB_FRAG_OUTPUTS >= 3)
    #define occFragColor2 occFragColorArrayAlias[2]
  #else
    vec4 occFragColor2;
  #endif
  #if (THE_NB_FRAG_OUTPUTS >= 4)
    #define occFragColor3 occFragColorArrayAlias[3]
  #else
    vec4 occFragColor3;
  #endif

  // Built-in outputs notation
  #define occFragColor    occFragColor0
  #define occFragCoverage occFragColor1

  //! Define the main Fragment Shader output - color value.
  void occSetFragColor (in vec4 theColor);
#endif

// Pi number definitions
#define PI       3.141592654
#define PI_2     6.283185307
#define PI_DIV_2 1.570796327
#define PI_DIV_3 1.047197551
#define PI_DIV_4 0.785398163
#define INV_PI   0.318309886
#define INV_PI_2 0.159154943

// Matrix state
uniform mat4 occWorldViewMatrix;  //!< World-view  matrix
uniform mat4 occProjectionMatrix; //!< Projection  matrix
uniform mat4 occModelWorldMatrix; //!< Model-world matrix

uniform mat4 occWorldViewMatrixInverse;    //!< Inverse of the world-view  matrix
uniform mat4 occProjectionMatrixInverse;   //!< Inverse of the projection  matrix
uniform mat4 occModelWorldMatrixInverse;   //!< Inverse of the model-world matrix

uniform mat4 occWorldViewMatrixTranspose;  //!< Transpose of the world-view  matrix
uniform mat4 occProjectionMatrixTranspose; //!< Transpose of the projection  matrix
uniform mat4 occModelWorldMatrixTranspose; //!< Transpose of the model-world matrix

uniform mat4 occWorldViewMatrixInverseTranspose;  //!< Transpose of the inverse of the world-view  matrix
uniform mat4 occProjectionMatrixInverseTranspose; //!< Transpose of the inverse of the projection  matrix
uniform mat4 occModelWorldMatrixInverseTranspose; //!< Transpose of the inverse of the model-world matrix

#if defined(THE_IS_PBR)
uniform sampler2D   occEnvLUT;             //!< Environment Lookup Table
uniform sampler2D   occDiffIBLMapSHCoeffs; //!< Packed diffuse (irradiance) IBL map's spherical harmonics coefficients
uniform samplerCube occSpecIBLMap;         //!< Specular IBL map
uniform int         occNbSpecIBLLevels;    //!< Number of mipmap levels used in occSpecIBLMap to store different roughness values maps

vec3 occDiffIBLMap (in vec3 theNormal); //!< Unpacks spherical harmonics coefficients to diffuse IBL map's values
#endif

// light type enumeration (same as Graphic3d_TypeOfLightSource)
const int OccLightType_Direct = 1; //!< directional     light source
const int OccLightType_Point  = 2; //!< isotropic point light source
const int OccLightType_Spot   = 3; //!< spot            light source

// Light sources
uniform               vec4 occLightAmbient;      //!< Cumulative ambient color
#if defined(THE_MAX_LIGHTS) && (THE_MAX_LIGHTS > 0)
uniform THE_PREC_ENUM int  occLightSourcesCount; //!< Total number of light sources

//! Type of light source, int (see OccLightType enum).
#define occLight_Type(theId)              occLightSourcesTypes[theId]

//! Specular intensity (equals to diffuse), vec3.
#define occLight_Specular(theId)          occLightSources[theId * 4 + 0].rgb

//! Intensity of light source (>= 0), float.
#define occLight_Intensity(theId)         occLightSources[theId * 4 + 0].a

//! Is light a headlight, bool?
#define occLight_IsHeadlight(theId)      (occLightSources[theId * 4 + 1].w > 0.5)

//! Position of specified light source, vec3.
#define occLight_Position(theId)          occLightSources[theId * 4 + 1].xyz

//! Direction of specified spot light source, vec3.
#define occLight_SpotDirection(theId)     occLightSources[theId * 4 + 2].xyz

//! Range on which point light source (positional or spot) can affect (>= 0), float.
#define occLight_Range(theId)             occLightSources[theId * 4 + 2].w

//! Maximum spread angle of the spot light (in radians), float.
#define occLight_SpotCutOff(theId)        occLightSources[theId * 4 + 3].z

//! Attenuation of the spot light intensity (from 0 to 1), float.
#define occLight_SpotExponent(theId)      occLightSources[theId * 4 + 3].w

#if !defined(THE_IS_PBR)
//! Diffuse intensity (equals to Specular), vec3.
#define occLight_Diffuse(theId)           occLightSources[theId * 4 + 0].rgb

//! Const attenuation factor of positional light source, float.
#define occLight_ConstAttenuation(theId)  occLightSources[theId * 4 + 3].x

//! Linear attenuation factor of positional light source, float.
#define occLight_LinearAttenuation(theId) occLightSources[theId * 4 + 3].y
#endif
#endif

#if defined(THE_IS_PBR)
//! Converts roughness value from range [0, 1] to real value for calculations
float occRoughness (in float theNormalizedRoughness);

// Front/back material properties accessors
vec4  occPBRMaterial_Color(in bool theIsFront);    //!< Base color of PBR material
float occPBRMaterial_Metallic(in bool theIsFront); //!< Metallic coefficient
float occPBRMaterial_NormalizedRoughness(in bool theIsFront); //!< Normalized roughness coefficient
vec3  occPBRMaterial_Emission(in bool theIsFront); //!< Light intensity emitted by material
float occPBRMaterial_IOR(in bool theIsFront);      //!< Index of refraction
#else
// Front material properties accessors
vec4  occFrontMaterial_Emission(void);           //!< Emission color
vec4  occFrontMaterial_Ambient(void);            //!< Ambient  reflection
vec4  occFrontMaterial_Diffuse(void);            //!< Diffuse  reflection
vec4  occFrontMaterial_Specular(void);           //!< Specular reflection
float occFrontMaterial_Shininess(void);          //!< Specular exponent
float occFrontMaterial_Transparency(void);       //!< Transparency coefficient

// Back material properties accessors
vec4  occBackMaterial_Emission(void);            //!< Emission color
vec4  occBackMaterial_Ambient(void);             //!< Ambient  reflection
vec4  occBackMaterial_Diffuse(void);             //!< Diffuse  reflection
vec4  occBackMaterial_Specular(void);            //!< Specular reflection
float occBackMaterial_Shininess(void);           //!< Specular exponent
float occBackMaterial_Transparency(void);        //!< Transparency coefficient
#endif

#ifdef THE_HAS_DEFAULT_SAMPLER
#define occActiveSampler    occSampler0  //!< alias for backward compatibility
#define occSamplerBaseColor occSampler0  //!< alias to a base color texture
uniform sampler2D           occSampler0; //!< current active sampler;
#endif                                   //!  occSampler1, occSampler2,... should be defined in GLSL program body for multitexturing

#if defined(THE_HAS_TEXTURE_COLOR)
#define occTextureColor(theMatColor, theTexCoord) (theMatColor * occTexture2D(occSamplerBaseColor, theTexCoord))
#else
#define occTextureColor(theMatColor, theTexCoord) theMatColor
#endif

#if defined(THE_HAS_TEXTURE_OCCLUSION) && defined(FRAGMENT_SHADER)
uniform sampler2D occSamplerOcclusion;   //!< R occlusion texture sampler
#define occTextureOcclusion(theColor, theTexCoord) theColor *= occTexture2D(occSamplerOcclusion, theTexCoord).r;
#else
#define occTextureOcclusion(theColor, theTexCoord)
#endif

#if defined(THE_HAS_TEXTURE_EMISSIVE) && defined(FRAGMENT_SHADER)
uniform sampler2D occSamplerEmissive;    //!< RGB emissive texture sampler
#define occTextureEmissive(theMatEmis, theTexCoord) (theMatEmis * occTexture2D(occSamplerEmissive, theTexCoord).rgb)
#else
#define occTextureEmissive(theMatEmis, theTexCoord) theMatEmis
#endif

#if defined(THE_HAS_TEXTURE_NORMAL) && defined(FRAGMENT_SHADER)
uniform sampler2D occSamplerNormal;      //!< XYZ normal texture sampler with W==0 indicating no texture
#define occTextureNormal(theTexCoord) occTexture2D(occSamplerNormal, theTexCoord)
#else
#define occTextureNormal(theTexCoord) vec4(0.0) // no normal map
#endif

#if defined(THE_HAS_TEXTURE_METALROUGHNESS) && defined(FRAGMENT_SHADER)
uniform sampler2D occSamplerMetallicRoughness; //!< BG metallic-roughness texture sampler
#define occTextureRoughness(theRoug, theTexCoord) (theRoug * occTexture2D(occSamplerMetallicRoughness, theTexCoord).g)
#define occTextureMetallic(theMet,   theTexCoord) (theMet  * occTexture2D(occSamplerMetallicRoughness, theTexCoord).b)
#else
#define occTextureRoughness(theRoug, theTexCoord) theRoug
#define occTextureMetallic(theMet,   theTexCoord) theMet
#endif

uniform               vec4      occColor;              //!< color value (in case of disabled lighting)
uniform THE_PREC_ENUM int       occDistinguishingMode; //!< Are front and back faces distinguished?
uniform THE_PREC_ENUM int       occTextureEnable;      //!< Is texture enabled?
uniform               vec4      occTexTrsf2d[2];       //!< 2D texture transformation parameters
uniform               float     occPointSize;          //!< point size

//! Parameters of blended order-independent transparency rendering algorithm
uniform               int       occOitOutput;      //!< Enable bit for writing output color buffers for OIT (occFragColor, occFragCoverage)
uniform               float     occOitDepthFactor; //!< Influence of the depth component to the coverage of the accumulated fragment
uniform               float     occAlphaCutoff;    //!< alpha test cutoff value

//! Parameters of clipping planes
#if defined(THE_MAX_CLIP_PLANES) && (THE_MAX_CLIP_PLANES > 0)
uniform               vec4 occClipPlaneEquations[THE_MAX_CLIP_PLANES];
uniform THE_PREC_ENUM int  occClipPlaneChains[THE_MAX_CLIP_PLANES]; //! Indicating the number of planes in the Chain
uniform THE_PREC_ENUM int  occClipPlaneCount;   //!< Total number of clip planes
#endif
//! @endfile Declarations.glsl
