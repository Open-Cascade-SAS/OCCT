#define _OCC_MAX_LIGHTS_ 8

#define _OCC_MAX_CLIP_PLANES_ 8


//! OCCT ambient light source.
const int occAmbientLight = 0;

//! OCCT directional light source.
const int occDirectLight = 1;

//! OCCT isotropic point light source.
const int occPointLight = 2;

//! OCCT spot light source.
const int occSpotLight = 3;


//! Parameters of OCCT light source.
struct occLightSource
{
  //! Type of light source.
  int Type;
  
  //! Is light a headlight?
  int Head;
  
  //! Ambient intensity.
  vec3 Ambient;
  
  //! Diffuse intensity.
  vec3 Diffuse;
  
  //! Specular intensity.
  vec3 Specular;
  
  //! Position of light source.
  vec3 Position;
  
  //! Direction of the spot light.
  vec3 SpotDirection;
    
  //! Maximum spread angle of the spot light (in radians).
  float SpotCutoff;
  
  //! Attenuation of the spot light intensity (from 0 to 1).
  float SpotExponent;

  //! Const attenuation factor of positional light source.
  float ConstAttenuation;
  
  //! Linear attenuation factor of positional light source.
  float LinearAttenuation;
};

//! Parameters of OCCT material.
struct occMaterialParams
{
  //! Emission color.
  vec4 Emission;
  
  //! Ambient reflection.
  vec4 Ambient;
  
  //! Diffuse reflection.
  vec4 Diffuse;
  
  //! Specular reflection.
  vec4 Specular;
  
  //! Specular exponent.
  float Shininess;
  
  //! Transparency coefficient.
  float Transparency;
};

//! OCCT view-space clipping plane.
const int occEquationCoordsView = 0;

//! OCCT world-space clipping plane.
const int occEquationCoordsWorld = 1;

//! Parameters of OCCT clipping plane.
struct occClipPlane
{
  //! Plane equation.
  vec4 Equation;

  //! Equation space.
  int Space;
};

#ifdef VERTEX_SHADER

/////////////////////////////////////////////////////////////////////
// OCCT vertex attributes

// Note: At the moment, we just 'rename' the default OpenGL
// vertex attributes from compatibility profile. In the next
// release old functionality will be removed from shader API.

//! Vertex color.
#define occColor gl_Color

//! Normal coordinates.
#define occNormal gl_Normal

//! Vertex coordinates.
#define occVertex gl_Vertex

//! Texture coordinates.
#define occTexCoord gl_MultiTexCoord0

#endif

/////////////////////////////////////////////////////////////////////
// OCCT matrix state

//! World-view matrix.
uniform mat4 occWorldViewMatrix;

//! Projection matrix.
uniform mat4 occProjectionMatrix;

//! Model-world matrix.
uniform mat4 occModelWorldMatrix;

//-------------------------------------------------------

//! Inverse of the world-view matrix.
uniform mat4 occWorldViewMatrixInverse;

//! Inverse of the projection matrix.
uniform mat4 occProjectionMatrixInverse;

//! Inverse of the model-world matrix.
uniform mat4 occModelWorldMatrixInverse;

//-------------------------------------------------------

//! Transpose of the world-view matrix.
uniform mat4 occWorldViewMatrixTranspose;

//! Transpose of the projection matrix.
uniform mat4 occProjectionMatrixTranspose;

//! Transpose of the model-world matrix.
uniform mat4 occModelWorldMatrixTranspose;

//-------------------------------------------------------

//! Transpose of the inverse of the world-view matrix.
uniform mat4 occWorldViewMatrixInverseTranspose;

//! Transpose of the inverse of the projection matrix.
uniform mat4 occProjectionMatrixInverseTranspose;

//! Transpose of the inverse of the model-world matrix.
uniform mat4 occModelWorldMatrixInverseTranspose;

/////////////////////////////////////////////////////////////////////
// OCCT light source state

//! Array of OCCT light sources.
uniform occLightSource occLightSources[_OCC_MAX_LIGHTS_];

//! Total number of OCCT light sources.
uniform int occLightSourcesCount;

/////////////////////////////////////////////////////////////////////
// OCCT material state

//! Parameters of OCCT back material.
uniform occMaterialParams occBackMaterial;

//! Parameters of OCCT front material.
uniform occMaterialParams occFrontMaterial;

//! Are front and back faces distinguished?
uniform int occDistinguishingMode;

//! Is texture enabled?
uniform int occTextureEnable;

//! Current active sampler.
uniform sampler2D occActiveSampler;

/////////////////////////////////////////////////////////////////////
// OCCT clipping planes state

uniform occClipPlane occClipPlanes[_OCC_MAX_CLIP_PLANES_];
