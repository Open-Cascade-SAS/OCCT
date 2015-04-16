#ifdef USE_TEXTURES
  #extension GL_ARB_bindless_texture : require
#endif

//! Normalized pixel coordinates.
in vec2 vPixel;

//! Sub-pixel offset in X direction for FSAA.
uniform float uOffsetX = 0.f;
//! Sub-pixel offset in Y direction for FSAA.
uniform float uOffsetY = 0.f;

//! Origin of viewing ray in left-top corner.
uniform vec3 uOriginLT;
//! Origin of viewing ray in left-bottom corner.
uniform vec3 uOriginLB;
//! Origin of viewing ray in right-top corner.
uniform vec3 uOriginRT;
//! Origin of viewing ray in right-bottom corner.
uniform vec3 uOriginRB;

//! Width of the rendering window.
uniform int uWinSizeX;
//! Height of the rendering window.
uniform int uWinSizeY;

//! Direction of viewing ray in left-top corner.
uniform vec3 uDirectLT;
//! Direction of viewing ray in left-bottom corner.
uniform vec3 uDirectLB;
//! Direction of viewing ray in right-top corner.
uniform vec3 uDirectRT;
//! Direction of viewing ray in right-bottom corner.
uniform vec3 uDirectRB;

//! Inverse model-view-projection matrix.
uniform mat4 uUnviewMat;

//! Texture buffer of data records of bottom-level BVH nodes.
uniform isamplerBuffer uSceneNodeInfoTexture;
//! Texture buffer of minimum points of bottom-level BVH nodes.
uniform samplerBuffer uSceneMinPointTexture;
//! Texture buffer of maximum points of bottom-level BVH nodes.
uniform samplerBuffer uSceneMaxPointTexture;
//! Texture buffer of transformations of high-level BVH nodes.
uniform samplerBuffer uSceneTransformTexture;

//! Texture buffer of vertex coords.
uniform samplerBuffer uGeometryVertexTexture;
//! Texture buffer of vertex normals.
uniform samplerBuffer uGeometryNormalTexture;
#ifdef USE_TEXTURES
  //! Texture buffer of per-vertex UV-coordinates.
  uniform samplerBuffer uGeometryTexCrdTexture;
#endif
//! Texture buffer of triangle indices.
uniform isamplerBuffer uGeometryTriangTexture;

//! Texture buffer of material properties.
uniform samplerBuffer uRaytraceMaterialTexture;
//! Texture buffer of light source properties.
uniform samplerBuffer uRaytraceLightSrcTexture;
//! Environment map texture.
uniform sampler2D uEnvironmentMapTexture;

//! Input pre-raytracing image rendered by OpenGL.
uniform sampler2D uOpenGlColorTexture;
//! Input pre-raytracing depth image rendered by OpenGL.
uniform sampler2D uOpenGlDepthTexture;

//! Total number of light sources.
uniform int uLightCount;
//! Intensity of global ambient light.
uniform vec4 uGlobalAmbient;

//! Enables/disables environment map.
uniform int uEnvironmentEnable;
//! Enables/disables computation of shadows.
uniform int uShadowsEnable;
//! Enables/disables computation of reflections.
uniform int uReflectionsEnable;

//! Radius of bounding sphere of the scene.
uniform float uSceneRadius;
//! Scene epsilon to prevent self-intersections.
uniform float uSceneEpsilon;

#ifdef USE_TEXTURES
  //! Unique 64-bit handles of OpenGL textures.
  uniform sampler2D uTextureSamplers[MAX_TEX_NUMBER];
#endif

/////////////////////////////////////////////////////////////////////////////////////////
// Specific data types
  
//! Stores ray parameters.
struct SRay
{
  vec3 Origin;
  
  vec3 Direct;
};

//! Stores intersection parameters.
struct SIntersect
{
  float Time;
  
  vec2 UV;
  
  vec3 Normal;
};

/////////////////////////////////////////////////////////////////////////////////////////
// Some useful constants

#define MAXFLOAT 1e15f

#define SMALL vec3 (exp2 (-80.0f))

#define ZERO vec3 (0.0f, 0.0f, 0.0f)
#define UNIT vec3 (1.0f, 1.0f, 1.0f)

#define AXIS_X vec3 (1.0f, 0.0f, 0.0f)
#define AXIS_Y vec3 (0.0f, 1.0f, 0.0f)
#define AXIS_Z vec3 (0.0f, 0.0f, 1.0f)

//! 32-bit state of random number generator.
uint RandState;

// =======================================================================
// function : SeedRand
// purpose  : Applies hash function by Thomas Wang to randomize seeds
//            (see http://www.burtleburtle.net/bob/hash/integer.html)
// =======================================================================
void SeedRand (in int theSeed)
{
  RandState = uint (int (gl_FragCoord.y) * uWinSizeX + int (gl_FragCoord.x) + theSeed);

  RandState = (RandState + 0x479ab41du) + (RandState <<  8);
  RandState = (RandState ^ 0xe4aa10ceu) ^ (RandState >>  5);
  RandState = (RandState + 0x9942f0a6u) - (RandState << 14);
  RandState = (RandState ^ 0x5aedd67du) ^ (RandState >>  3);
  RandState = (RandState + 0x17bea992u) + (RandState <<  7);
}

// =======================================================================
// function : RandInt
// purpose  : Generates integer using Xorshift algorithm by G. Marsaglia
// =======================================================================
uint RandInt()
{
  RandState ^= (RandState << 13);
  RandState ^= (RandState >> 17);
  RandState ^= (RandState <<  5);

  return RandState;
}

// =======================================================================
// function : RandFloat
// purpose  : Generates a random float in [0, 1) range
// =======================================================================
float RandFloat()
{
  return float (RandInt()) * (1.f / 4294967296.f);
}

// =======================================================================
// function : MatrixColMultiplyPnt
// purpose  : Multiplies a vector by matrix
// =======================================================================
vec3 MatrixColMultiplyPnt (in vec3 v,
                           in vec4 m0,
                           in vec4 m1,
                           in vec4 m2,
                           in vec4 m3)
{
  return vec3 (m0[0] * v.x + m1[0] * v.y + m2[0] * v.z + m3[0],
               m0[1] * v.x + m1[1] * v.y + m2[1] * v.z + m3[1],
               m0[2] * v.x + m1[2] * v.y + m2[2] * v.z + m3[2]);
}

// =======================================================================
// function : MatrixColMultiplyDir
// purpose  : Multiplies a vector by matrix
// =======================================================================
vec3 MatrixColMultiplyDir (in vec3 v,
                           in vec4 m0,
                           in vec4 m1,
                           in vec4 m2,
                           in vec4 m3)
{
  return vec3 (m0[0] * v.x + m1[0] * v.y + m2[0] * v.z,
               m0[1] * v.x + m1[1] * v.y + m2[1] * v.z,
               m0[2] * v.x + m1[2] * v.y + m2[2] * v.z);
}

/////////////////////////////////////////////////////////////////////////////////////////
// Functions for compute ray-object intersection

// =======================================================================
// function : GenerateRay
// purpose  :
// =======================================================================
SRay GenerateRay (in vec2 thePixel)
{
  vec3 aP0 = mix (uOriginLB, uOriginRB, thePixel.x);
  vec3 aP1 = mix (uOriginLT, uOriginRT, thePixel.x);

  vec3 aD0 = mix (uDirectLB, uDirectRB, thePixel.x);
  vec3 aD1 = mix (uDirectLT, uDirectRT, thePixel.x);

  vec3 aDirection = normalize (mix (aD0, aD1, thePixel.y));

  return SRay (mix (aP0, aP1, thePixel.y), aDirection);
}

// =======================================================================
// function : ComputeOpenGlDepth
// purpose  :
// =======================================================================
float ComputeOpenGlDepth (in SRay theRay)
{
  // a depth in range [0,1]
  float anOpenGlDepth = texelFetch (uOpenGlDepthTexture, ivec2 (gl_FragCoord.xy), 0).r;
  // pixel point in NDC-space [-1,1]
  vec4 aPoint = vec4 (2.0f * vPixel.x - 1.0f,
                      2.0f * vPixel.y - 1.0f,
                      2.0f * anOpenGlDepth - 1.0f,
                      1.0f);
  vec4 aFinal = uUnviewMat * aPoint;
  aFinal.xyz *= 1.f / aFinal.w;

  return (anOpenGlDepth < 1.f) ? length (aFinal.xyz - theRay.Origin) : MAXFLOAT;
}

// =======================================================================
// function : ComputeOpenGlColor
// purpose  :
// =======================================================================
vec4 ComputeOpenGlColor (in SRay theRay)
{
  vec4 anOpenGlColor = texelFetch (uOpenGlColorTexture, ivec2 (gl_FragCoord.xy), 0);
  // During blending with factors GL_SRC_ALPHA and GL_ONE_MINUS_SRC_ALPHA (for text and markers)
  // the alpha channel (written in the color buffer) was squared.
  anOpenGlColor.a = 1.f - sqrt (anOpenGlColor.a);

  return anOpenGlColor;
}

// =======================================================================
// function : IntersectSphere
// purpose  : Computes ray-sphere intersection
// =======================================================================
float IntersectSphere (in SRay theRay, in float theRadius)
{
  float aDdotD = dot (theRay.Direct, theRay.Direct);
  float aDdotO = dot (theRay.Direct, theRay.Origin);
  float aOdotO = dot (theRay.Origin, theRay.Origin);

  float aD = aDdotO * aDdotO - aDdotD * (aOdotO - theRadius * theRadius);

  if (aD > 0.0f)
  {
    float aTime = (sqrt (aD) - aDdotO) * (1.0f / aDdotD);
    
    return aTime > 0.0f ? aTime : MAXFLOAT;
  }

  return MAXFLOAT;
}

// =======================================================================
// function : IntersectTriangle
// purpose  : Computes ray-triangle intersection (branchless version)
// =======================================================================
float IntersectTriangle (in SRay theRay,
                         in vec3 thePnt0,
                         in vec3 thePnt1,
                         in vec3 thePnt2,
                         out vec2 theUV,
                         out vec3 theNorm)
{
  vec3 aEdge0 = thePnt1 - thePnt0;
  vec3 aEdge1 = thePnt0 - thePnt2;

  theNorm = cross (aEdge1, aEdge0);

  vec3 aEdge2 = (1.0f / dot (theNorm, theRay.Direct)) * (thePnt0 - theRay.Origin);

  float aTime = dot (theNorm, aEdge2);

  vec3 theVec = cross (theRay.Direct, aEdge2);

  theUV.x = dot (theVec, aEdge1);
  theUV.y = dot (theVec, aEdge0);

  return bool (int(aTime >= 0.0f) &
               int(theUV.x >= 0.0f) &
               int(theUV.y >= 0.0f) &
               int(theUV.x + theUV.y <= 1.0f)) ? aTime : MAXFLOAT;
}

//! Identifies the absence of intersection.
#define INALID_HIT ivec4 (-1)

//! Global stack shared between traversal functions.
int Stack[STACK_SIZE];

// =======================================================================
// function : ObjectNearestHit
// purpose  : Finds intersection with nearest object triangle
// =======================================================================
ivec4 ObjectNearestHit (in int theBVHOffset, in int theVrtOffset, in int theTrgOffset,
  in SRay theRay, in vec3 theInverse, inout SIntersect theHit, in int theSentinel)
{
  int aHead = theSentinel;  // stack pointer
  int aNode = theBVHOffset; // node to visit

  ivec4 aTriIndex = INALID_HIT;

  bool toContinue = true;

  while (toContinue)
  {
    ivec3 aData = texelFetch (uSceneNodeInfoTexture, aNode).xyz;

    if (aData.x == 0) // if inner node
    {
      float aTimeOut;
      float aTimeLft;
      float aTimeRgh;

      aData.y += theBVHOffset;
      aData.z += theBVHOffset;

      vec3 aNodeMinLft = texelFetch (uSceneMinPointTexture, aData.y).xyz;
      vec3 aNodeMinRgh = texelFetch (uSceneMinPointTexture, aData.z).xyz;
      vec3 aNodeMaxLft = texelFetch (uSceneMaxPointTexture, aData.y).xyz;
      vec3 aNodeMaxRgh = texelFetch (uSceneMaxPointTexture, aData.z).xyz;

      vec3 aTime0 = (aNodeMinLft - theRay.Origin) * theInverse;
      vec3 aTime1 = (aNodeMaxLft - theRay.Origin) * theInverse;

      vec3 aTimeMax = max (aTime0, aTime1);
      vec3 aTimeMin = min (aTime0, aTime1);

      aTime0 = (aNodeMinRgh - theRay.Origin) * theInverse;
      aTime1 = (aNodeMaxRgh - theRay.Origin) * theInverse;

      aTimeOut = min (aTimeMax.x, min (aTimeMax.y, aTimeMax.z));
      aTimeLft = max (aTimeMin.x, max (aTimeMin.y, aTimeMin.z));

      int aHitLft = int(aTimeLft <= aTimeOut) & int(aTimeOut >= 0.0f) & int(aTimeLft <= theHit.Time);

      aTimeMax = max (aTime0, aTime1);
      aTimeMin = min (aTime0, aTime1);

      aTimeOut = min (aTimeMax.x, min (aTimeMax.y, aTimeMax.z));
      aTimeRgh = max (aTimeMin.x, max (aTimeMin.y, aTimeMin.z));

      int aHitRgh = int(aTimeRgh <= aTimeOut) & int(aTimeOut >= 0.0f) & int(aTimeRgh <= theHit.Time);

      if (bool(aHitLft & aHitRgh))
      {
        aNode = (aTimeLft < aTimeRgh) ? aData.y : aData.z;

        Stack[++aHead] = (aTimeLft < aTimeRgh) ? aData.z : aData.y;
      }
      else
      {
        if (bool(aHitLft | aHitRgh))
        {
          aNode = bool(aHitLft) ? aData.y : aData.z;
        }
        else
        {
          toContinue = (aHead != theSentinel);

          if (toContinue)
            aNode = Stack[aHead--];
        }
      }
    }
    else // if leaf node
    {
      vec3 aNormal;
      vec2 aParams;

      for (int anIdx = aData.y; anIdx <= aData.z; ++anIdx)
      {
        ivec4 aTriangle = texelFetch (uGeometryTriangTexture, anIdx + theTrgOffset);

        vec3 aPoint0 = texelFetch (uGeometryVertexTexture, aTriangle.x += theVrtOffset).xyz;
        vec3 aPoint1 = texelFetch (uGeometryVertexTexture, aTriangle.y += theVrtOffset).xyz;
        vec3 aPoint2 = texelFetch (uGeometryVertexTexture, aTriangle.z += theVrtOffset).xyz;

        float aTime = IntersectTriangle (theRay,
                                         aPoint0,
                                         aPoint1,
                                         aPoint2,
                                         aParams,
                                         aNormal);

        if (aTime < theHit.Time)
        {
          aTriIndex = aTriangle;

          theHit = SIntersect (aTime, aParams, aNormal);
        }
      }

      toContinue = (aHead != theSentinel);

      if (toContinue)
        aNode = Stack[aHead--];
    }
  }

  return aTriIndex;
}

#define MATERIAL_AMBN(index) (11 * index + 0)
#define MATERIAL_DIFF(index) (11 * index + 1)
#define MATERIAL_SPEC(index) (11 * index + 2)
#define MATERIAL_EMIS(index) (11 * index + 3)
#define MATERIAL_REFL(index) (11 * index + 4)
#define MATERIAL_REFR(index) (11 * index + 5)
#define MATERIAL_TRAN(index) (11 * index + 6)
#define MATERIAL_TRS1(index) (11 * index + 7)
#define MATERIAL_TRS2(index) (11 * index + 8)
#define MATERIAL_TRS3(index) (11 * index + 9)

// =======================================================================
// function : ObjectAnyHit
// purpose  : Finds intersection with any object triangle
// =======================================================================
float ObjectAnyHit (in int theBVHOffset, in int theVrtOffset, in int theTrgOffset,
  in SRay theRay, in vec3 theInverse, in float theDistance, in int theSentinel)
{
  int aHead = theSentinel;  // stack pointer
  int aNode = theBVHOffset; // node to visit

#ifdef TRANSPARENT_SHADOWS
  float aFactor = 1.0f;
#endif

  while (true)
  {
    ivec4 aData = texelFetch (uSceneNodeInfoTexture, aNode);

    if (aData.x == 0) // if inner node
    {
      float aTimeOut;
      float aTimeLft;
      float aTimeRgh;

      aData.y += theBVHOffset;
      aData.z += theBVHOffset;

      vec3 aNodeMinLft = texelFetch (uSceneMinPointTexture, aData.y).xyz;
      vec3 aNodeMaxLft = texelFetch (uSceneMaxPointTexture, aData.y).xyz;
      vec3 aNodeMinRgh = texelFetch (uSceneMinPointTexture, aData.z).xyz;
      vec3 aNodeMaxRgh = texelFetch (uSceneMaxPointTexture, aData.z).xyz;

      vec3 aTime0 = (aNodeMinLft - theRay.Origin) * theInverse;
      vec3 aTime1 = (aNodeMaxLft - theRay.Origin) * theInverse;

      vec3 aTimeMax = max (aTime0, aTime1);
      vec3 aTimeMin = min (aTime0, aTime1);

      aTime0 = (aNodeMinRgh - theRay.Origin) * theInverse;
      aTime1 = (aNodeMaxRgh - theRay.Origin) * theInverse;

      aTimeOut = min (aTimeMax.x, min (aTimeMax.y, aTimeMax.z));
      aTimeLft = max (aTimeMin.x, max (aTimeMin.y, aTimeMin.z));

      int aHitLft = int(aTimeLft <= aTimeOut) & int(aTimeOut >= 0.0f) & int(aTimeLft <= theDistance);

      aTimeMax = max (aTime0, aTime1);
      aTimeMin = min (aTime0, aTime1);

      aTimeOut = min (aTimeMax.x, min (aTimeMax.y, aTimeMax.z));
      aTimeRgh = max (aTimeMin.x, max (aTimeMin.y, aTimeMin.z));

      int aHitRgh = int(aTimeRgh <= aTimeOut) & int(aTimeOut >= 0.0f) & int(aTimeRgh <= theDistance);

      if (bool(aHitLft & aHitRgh))
      {
        aNode = (aTimeLft < aTimeRgh) ? aData.y : aData.z;

        Stack[++aHead] = (aTimeLft < aTimeRgh) ? aData.z : aData.y;
      }
      else
      {
        if (bool(aHitLft | aHitRgh))
        {
          aNode = bool(aHitLft) ? aData.y : aData.z;
        }
        else
        {
#ifdef TRANSPARENT_SHADOWS
          if (aHead == theSentinel)
            return aFactor;
#else
          if (aHead == theSentinel)
            return 1.0f;
#endif

          aNode = Stack[aHead--];
        }
      }
    }
    else // if leaf node
    {
      vec3 aNormal;
      vec2 aParams;

      for (int anIdx = aData.y; anIdx <= aData.z; ++anIdx)
      {
        ivec4 aTriangle = texelFetch (uGeometryTriangTexture, anIdx + theTrgOffset);

        vec3 aPoint0 = texelFetch (uGeometryVertexTexture, aTriangle.x + theVrtOffset).xyz;
        vec3 aPoint1 = texelFetch (uGeometryVertexTexture, aTriangle.y + theVrtOffset).xyz;
        vec3 aPoint2 = texelFetch (uGeometryVertexTexture, aTriangle.z + theVrtOffset).xyz;

        float aTime = IntersectTriangle (theRay,
                                         aPoint0,
                                         aPoint1,
                                         aPoint2,
                                         aParams,
                                         aNormal);

#ifdef TRANSPARENT_SHADOWS
        if (aTime < theDistance)
        {
          aFactor *= 1.0f - texelFetch (uRaytraceMaterialTexture, MATERIAL_TRAN (aTriangle.w)).x;
        }
#else
        if (aTime < theDistance)
          return 0.0f;
#endif
      }

#ifdef TRANSPARENT_SHADOWS
      if (aHead == theSentinel || aFactor < 0.1f)
        return aFactor;
#else
      if (aHead == theSentinel)
        return 1.0f;
#endif

      aNode = Stack[aHead--];
    }
  }

#ifdef TRANSPARENT_SHADOWS
  return aFactor;
#else
  return 1.0f;
#endif
}

// =======================================================================
// function : SceneNearestHit
// purpose  : Finds intersection with nearest scene triangle
// =======================================================================
ivec4 SceneNearestHit (in SRay theRay, in vec3 theInverse, inout SIntersect theHit, out int theObjectId)
{
  int aHead = -1; // stack pointer
  int aNode =  0; // node to visit

  ivec4 aHitObject = INALID_HIT;

  while (true)
  {
    ivec4 aData = texelFetch (uSceneNodeInfoTexture, aNode);

    if (aData.x != 0) // if leaf node
    {
      vec3 aNodeMin = texelFetch (uSceneMinPointTexture, aNode).xyz;
      vec3 aNodeMax = texelFetch (uSceneMaxPointTexture, aNode).xyz;

      vec3 aTime0 = (aNodeMin - theRay.Origin) * theInverse;
      vec3 aTime1 = (aNodeMax - theRay.Origin) * theInverse;

      vec3 aTimes = min (aTime0, aTime1);

      if (max (aTimes.x, max (aTimes.y, aTimes.z)) < theHit.Time)
      {
        // fetch object transformation
        int anObjectId = aData.x - 1;

        vec4 aInvTransf0 = texelFetch (uSceneTransformTexture, anObjectId * 4 + 0);
        vec4 aInvTransf1 = texelFetch (uSceneTransformTexture, anObjectId * 4 + 1);
        vec4 aInvTransf2 = texelFetch (uSceneTransformTexture, anObjectId * 4 + 2);
        vec4 aInvTransf3 = texelFetch (uSceneTransformTexture, anObjectId * 4 + 3);

        SRay aTrsfRay = SRay (
          MatrixColMultiplyPnt (theRay.Origin, aInvTransf0, aInvTransf1, aInvTransf2, aInvTransf3),
          MatrixColMultiplyDir (theRay.Direct, aInvTransf0, aInvTransf1, aInvTransf2, aInvTransf3));

        vec3 aTrsfInverse = 1.0f / max (abs (aTrsfRay.Direct), SMALL);

        aTrsfInverse = mix (-aTrsfInverse, aTrsfInverse, step (ZERO, aTrsfRay.Direct));

        ivec4 aTriIndex = ObjectNearestHit (
          aData.y, aData.z, aData.w, aTrsfRay, aTrsfInverse, theHit, aHead);

        if (aTriIndex.x != -1)
        {
          aHitObject = ivec4 (aTriIndex.x,  // vertex 0
                              aTriIndex.y,  // vertex 1
                              aTriIndex.z,  // vertex 2
                              aTriIndex.w); // material

          theObjectId = anObjectId;
        }
      }

      if (aHead < 0)
        return aHitObject;

      aNode = Stack[aHead--];
    }
    else // if inner node
    {
      float aTimeOut;
      float aTimeLft;
      float aTimeRgh;

      vec3 aNodeMinLft = texelFetch (uSceneMinPointTexture, aData.y).xyz;
      vec3 aNodeMaxLft = texelFetch (uSceneMaxPointTexture, aData.y).xyz;
      vec3 aNodeMinRgh = texelFetch (uSceneMinPointTexture, aData.z).xyz;
      vec3 aNodeMaxRgh = texelFetch (uSceneMaxPointTexture, aData.z).xyz;

      vec3 aTime0 = (aNodeMinLft - theRay.Origin) * theInverse;
      vec3 aTime1 = (aNodeMaxLft - theRay.Origin) * theInverse;

      vec3 aTimeMax = max (aTime0, aTime1);
      vec3 aTimeMin = min (aTime0, aTime1);

      aTimeOut = min (aTimeMax.x, min (aTimeMax.y, aTimeMax.z));
      aTimeLft = max (aTimeMin.x, max (aTimeMin.y, aTimeMin.z));

      int aHitLft = int(aTimeLft <= aTimeOut) & int(aTimeOut >= 0.0f) & int(aTimeLft <= theHit.Time);

      aTime0 = (aNodeMinRgh - theRay.Origin) * theInverse;
      aTime1 = (aNodeMaxRgh - theRay.Origin) * theInverse;

      aTimeMax = max (aTime0, aTime1);
      aTimeMin = min (aTime0, aTime1);

      aTimeOut = min (aTimeMax.x, min (aTimeMax.y, aTimeMax.z));
      aTimeRgh = max (aTimeMin.x, max (aTimeMin.y, aTimeMin.z));

      int aHitRgh = int(aTimeRgh <= aTimeOut) & int(aTimeOut >= 0.0f) & int(aTimeRgh <= theHit.Time);

      if (bool(aHitLft & aHitRgh))
      {
        aNode = (aTimeLft < aTimeRgh) ? aData.y : aData.z;

        Stack[++aHead] = (aTimeLft < aTimeRgh) ? aData.z : aData.y;
      }
      else
      {
        if (bool(aHitLft | aHitRgh))
        {
          aNode = bool(aHitLft) ? aData.y : aData.z;
        }
        else
        {
          if (aHead < 0)
            return aHitObject;

          aNode = Stack[aHead--];
        }
      }
    }
  }

  return aHitObject;
}

// =======================================================================
// function : SceneAnyHit
// purpose  : Finds intersection with any scene triangle
// =======================================================================
float SceneAnyHit (in SRay theRay, in vec3 theInverse, in float theDistance)
{
  int aHead = -1; // stack pointer
  int aNode =  0; // node to visit

#ifdef TRANSPARENT_SHADOWS
  float aFactor = 1.0f;
#endif

  while (true)
  {
    ivec4 aData = texelFetch (uSceneNodeInfoTexture, aNode);

    if (aData.x != 0) // if leaf node
    {
      // fetch object transformation
      int anObjectId = aData.x - 1;

      vec4 aInvTransf0 = texelFetch (uSceneTransformTexture, anObjectId * 4 + 0);
      vec4 aInvTransf1 = texelFetch (uSceneTransformTexture, anObjectId * 4 + 1);
      vec4 aInvTransf2 = texelFetch (uSceneTransformTexture, anObjectId * 4 + 2);
      vec4 aInvTransf3 = texelFetch (uSceneTransformTexture, anObjectId * 4 + 3);

      SRay aTrsfRay = SRay (
        MatrixColMultiplyPnt (theRay.Origin, aInvTransf0, aInvTransf1, aInvTransf2, aInvTransf3),
        MatrixColMultiplyDir (theRay.Direct, aInvTransf0, aInvTransf1, aInvTransf2, aInvTransf3));

      vec3 aTrsfInverse = 1.0f / max (abs (aTrsfRay.Direct), SMALL);

      aTrsfInverse = mix (-aTrsfInverse, aTrsfInverse, step (ZERO, aTrsfRay.Direct));

#ifdef TRANSPARENT_SHADOWS
      aFactor *= ObjectAnyHit (
        aData.y, aData.z, aData.w, aTrsfRay, aTrsfInverse, theDistance, aHead);

      if (aHead < 0 || aFactor < 0.1f)
        return aFactor;
#else
      bool isShadow = 0.0f == ObjectAnyHit (
        aData.y, aData.z, aData.w, aTrsfRay, aTrsfInverse, theDistance, aHead);

      if (aHead < 0 || isShadow)
        return isShadow ? 0.0f : 1.0f;
#endif

      aNode = Stack[aHead--];
    }
    else // if inner node
    {
      float aTimeOut;
      float aTimeLft;
      float aTimeRgh;

      vec3 aNodeMinLft = texelFetch (uSceneMinPointTexture, aData.y).xyz;
      vec3 aNodeMaxLft = texelFetch (uSceneMaxPointTexture, aData.y).xyz;
      vec3 aNodeMinRgh = texelFetch (uSceneMinPointTexture, aData.z).xyz;
      vec3 aNodeMaxRgh = texelFetch (uSceneMaxPointTexture, aData.z).xyz;
      
      vec3 aTime0 = (aNodeMinLft - theRay.Origin) * theInverse;
      vec3 aTime1 = (aNodeMaxLft - theRay.Origin) * theInverse;

      vec3 aTimeMax = max (aTime0, aTime1);
      vec3 aTimeMin = min (aTime0, aTime1);

      aTimeOut = min (aTimeMax.x, min (aTimeMax.y, aTimeMax.z));
      aTimeLft = max (aTimeMin.x, max (aTimeMin.y, aTimeMin.z));

      int aHitLft = int(aTimeLft <= aTimeOut) & int(aTimeOut >= 0.0f) & int(aTimeLft <= theDistance);
      
      aTime0 = (aNodeMinRgh - theRay.Origin) * theInverse;
      aTime1 = (aNodeMaxRgh - theRay.Origin) * theInverse;

      aTimeMax = max (aTime0, aTime1);
      aTimeMin = min (aTime0, aTime1);

      aTimeOut = min (aTimeMax.x, min (aTimeMax.y, aTimeMax.z));
      aTimeRgh = max (aTimeMin.x, max (aTimeMin.y, aTimeMin.z));
      
      int aHitRgh = int(aTimeRgh <= aTimeOut) & int(aTimeOut >= 0.0f) & int(aTimeRgh <= theDistance);

      if (bool(aHitLft & aHitRgh))
      {
        aNode = (aTimeLft < aTimeRgh) ? aData.y : aData.z;

        Stack[++aHead] = (aTimeLft < aTimeRgh) ? aData.z : aData.y;
      }
      else
      {
        if (bool(aHitLft | aHitRgh))
        {
          aNode = bool(aHitLft) ? aData.y : aData.z;
        }
        else
        {
#ifdef TRANSPARENT_SHADOWS
          if (aHead < 0)
            return aFactor;
#else
          if (aHead < 0)
            return 1.0f;
#endif

          aNode = Stack[aHead--];
        }
      }
    }
  }

#ifdef TRANSPARENT_SHADOWS
  return aFactor;
#else
  return 1.0f;
#endif
}

#define PI 3.1415926f

// =======================================================================
// function : Latlong
// purpose  : Converts world direction to environment texture coordinates
// =======================================================================
vec2 Latlong (in vec3 thePoint, in float theRadius)
{
  float aPsi = acos (-thePoint.z / theRadius);

  float aPhi = atan (thePoint.y, thePoint.x) + PI;

  return vec2 (aPhi * 0.1591549f,
               aPsi * 0.3183098f);
}

// =======================================================================
// function : SmoothNormal
// purpose  : Interpolates normal across the triangle
// =======================================================================
vec3 SmoothNormal (in vec2 theUV, in ivec4 theTriangle)
{
  vec3 aNormal0 = texelFetch (uGeometryNormalTexture, theTriangle.x).xyz;
  vec3 aNormal1 = texelFetch (uGeometryNormalTexture, theTriangle.y).xyz;
  vec3 aNormal2 = texelFetch (uGeometryNormalTexture, theTriangle.z).xyz;

  return normalize (aNormal1 * theUV.x +
                    aNormal2 * theUV.y +
                    aNormal0 * (1.0f - theUV.x - theUV.y));
}

// =======================================================================
// function : SmoothUV
// purpose  : Interpolates UV coordinates across the triangle
// =======================================================================
#ifdef USE_TEXTURES
vec2 SmoothUV (in vec2 theUV, in ivec4 theTriangle)
{
  vec2 aTexCrd0 = texelFetch (uGeometryTexCrdTexture, theTriangle.x).st;
  vec2 aTexCrd1 = texelFetch (uGeometryTexCrdTexture, theTriangle.y).st;
  vec2 aTexCrd2 = texelFetch (uGeometryTexCrdTexture, theTriangle.z).st;

  return aTexCrd1 * theUV.x +
         aTexCrd2 * theUV.y +
         aTexCrd0 * (1.0f - theUV.x - theUV.y);
}
#endif

// =======================================================================
// function : Refract
// purpose  : Computes refraction ray (also handles TIR)
// =======================================================================
vec3 Refract (in vec3 theInput,
              in vec3 theNormal,
              in float theRefractIndex,
              in float theInvRefractIndex)
{
  float aNdotI = dot (theInput, theNormal);

  float anIndex = aNdotI < 0.0f
                ? theInvRefractIndex
                : theRefractIndex;

  float aSquare = anIndex * anIndex * (1.0f - aNdotI * aNdotI);

  if (aSquare > 1.0f)
  {
    return reflect (theInput, theNormal);
  }

  float aNdotT = sqrt (1.0f - aSquare);

  return normalize (anIndex * theInput -
    (anIndex * aNdotI + (aNdotI < 0.0f ? aNdotT : -aNdotT)) * theNormal);
}

#define MIN_SLOPE 0.0001f
#define EPS_SCALE 8.0000f

#define THRESHOLD vec3 (0.1f)

#define INVALID_BOUNCES 1000

#define LIGHT_POS(index) (2 * index + 1)
#define LIGHT_PWR(index) (2 * index + 0)

// =======================================================================
// function : Radiance
// purpose  : Computes color along the given ray
// =======================================================================
vec4 Radiance (in SRay theRay, in vec3 theInverse)
{
  vec3 aResult = vec3 (0.0f);
  vec4 aWeight = vec4 (1.0f);

  int anObjectId;

  float anOpenGlDepth = ComputeOpenGlDepth (theRay);

  for (int aDepth = 0; aDepth < NB_BOUNCES; ++aDepth)
  {
    SIntersect aHit = SIntersect (MAXFLOAT, vec2 (ZERO), ZERO);

    ivec4 aTriIndex = SceneNearestHit (theRay, theInverse, aHit, anObjectId);

    if (aTriIndex.x == -1)
    {
      vec4 aColor = vec4 (0.0f);

      if (aWeight.w != 0.0f)
      {
        aColor = anOpenGlDepth != MAXFLOAT ?
          ComputeOpenGlColor (theRay) : vec4 (0.0f, 0.0f, 0.0f, 1.0f);
      }
      else if (bool(uEnvironmentEnable))
      {
        float aTime = IntersectSphere (theRay, uSceneRadius);

        aColor = textureLod (uEnvironmentMapTexture, Latlong (
          theRay.Direct * aTime + theRay.Origin, uSceneRadius), 0.0f);
      }

      return vec4 (aResult.xyz + aWeight.xyz * aColor.xyz, aWeight.w * aColor.w);
    }

    vec3 aInvTransf0 = texelFetch (uSceneTransformTexture, anObjectId * 4 + 0).xyz;
    vec3 aInvTransf1 = texelFetch (uSceneTransformTexture, anObjectId * 4 + 1).xyz;
    vec3 aInvTransf2 = texelFetch (uSceneTransformTexture, anObjectId * 4 + 2).xyz;

    aHit.Normal = normalize (vec3 (dot (aInvTransf0, aHit.Normal),
                                   dot (aInvTransf1, aHit.Normal),
                                   dot (aInvTransf2, aHit.Normal)));

    // For polygons that are parallel to the screen plane, the depth slope
    // is equal to 1, resulting in small polygon offset. For polygons that
    // that are at a large angle to the screen, the depth slope tends to 1,
    // resulting in a larger polygon offset
    float aPolygonOffset = uSceneEpsilon * EPS_SCALE /
      max (abs (dot (theRay.Direct, aHit.Normal)), MIN_SLOPE);

    if (anOpenGlDepth < aHit.Time + aPolygonOffset)
    {
      vec4 aGlColor = ComputeOpenGlColor (theRay);

      aResult += aWeight.xyz * aGlColor.xyz;
      aWeight *= aGlColor.w;
    }

    theRay.Origin += theRay.Direct * aHit.Time; // intersection point

    vec3 aNormal = SmoothNormal (aHit.UV, aTriIndex);

    aNormal = normalize (vec3 (dot (aInvTransf0, aNormal),
                               dot (aInvTransf1, aNormal),
                               dot (aInvTransf2, aNormal)));

    vec3 aAmbient  = texelFetch (
      uRaytraceMaterialTexture, MATERIAL_AMBN (aTriIndex.w)).rgb;
    vec4 aDiffuse  = texelFetch (
      uRaytraceMaterialTexture, MATERIAL_DIFF (aTriIndex.w));
    vec4 aSpecular = texelFetch (
      uRaytraceMaterialTexture, MATERIAL_SPEC (aTriIndex.w));
    vec4 aOpacity  = texelFetch (
      uRaytraceMaterialTexture, MATERIAL_TRAN (aTriIndex.w));

#ifdef USE_TEXTURES
    if (aDiffuse.w >= 0.f)
    {
      vec4 aTexCoord = vec4 (SmoothUV (aHit.UV, aTriIndex), 0.f, 1.f);

      vec4 aTrsfRow1 = texelFetch (
        uRaytraceMaterialTexture, MATERIAL_TRS1 (aTriIndex.w));
      vec4 aTrsfRow2 = texelFetch (
        uRaytraceMaterialTexture, MATERIAL_TRS2 (aTriIndex.w));

      aTexCoord.st = vec2 (dot (aTrsfRow1, aTexCoord),
                           dot (aTrsfRow2, aTexCoord));

      vec3 aTexColor = textureLod (
        uTextureSamplers[int(aDiffuse.w)], aTexCoord.st, 0.f).rgb;

      aDiffuse.rgb *= aTexColor;
      aAmbient.rgb *= aTexColor;
    }
#endif

    vec3 aEmission = texelFetch (
      uRaytraceMaterialTexture, MATERIAL_EMIS (aTriIndex.w)).rgb;

    float aGeomFactor = dot (aNormal, theRay.Direct);

    aResult.xyz += aWeight.xyz * aOpacity.x * (
      uGlobalAmbient.xyz * aAmbient * max (abs (aGeomFactor), 0.5f) + aEmission);

    vec3 aSidedNormal = mix (aNormal, -aNormal, step (0.0f, aGeomFactor));

    for (int aLightIdx = 0; aLightIdx < uLightCount; ++aLightIdx)
    {
      vec4 aLight = texelFetch (
        uRaytraceLightSrcTexture, LIGHT_POS (aLightIdx));

      float aDistance = MAXFLOAT;

      if (aLight.w != 0.0f) // point light source
      {
        aDistance = length (aLight.xyz -= theRay.Origin);

        aLight.xyz *= 1.0f / aDistance;
      }

      float aLdotN = dot (aLight.xyz, aSidedNormal);

      if (aLdotN > 0.0f) // first check if light source is important
      {
        float aVisibility = 1.0f;

        if (bool(uShadowsEnable))
        {
          SRay aShadow = SRay (theRay.Origin, aLight.xyz);

          aShadow.Origin += uSceneEpsilon * (aLight.xyz +
            mix (-aHit.Normal, aHit.Normal, step (0.0f, dot (aHit.Normal, aLight.xyz))));

          vec3 aInverse = 1.0f / max (abs (aLight.xyz), SMALL);

          aVisibility = SceneAnyHit (
            aShadow, mix (-aInverse, aInverse, step (ZERO, aLight.xyz)), aDistance);
        }

        if (aVisibility > 0.0f)
        {
          vec3 aIntensity = vec3 (texelFetch (
            uRaytraceLightSrcTexture, LIGHT_PWR (aLightIdx)));

          float aRdotV = dot (reflect (aLight.xyz, aSidedNormal), theRay.Direct);

          aResult.xyz += aWeight.xyz * (aOpacity.x * aVisibility) * aIntensity *
            (aDiffuse.xyz * aLdotN + aSpecular.xyz * pow (max (0.f, aRdotV), aSpecular.w));
        }
      }
    }

    if (aOpacity.x != 1.0f)
    {
      aWeight *= aOpacity.y;

      if (aOpacity.z != 1.0f)
      {
        theRay.Direct = Refract (theRay.Direct, aNormal, aOpacity.z, aOpacity.w);
      }
      else
      {
        anOpenGlDepth -= aHit.Time + uSceneEpsilon;
      }
    }
    else
    {
      aWeight *= bool(uReflectionsEnable) ?
        texelFetch (uRaytraceMaterialTexture, MATERIAL_REFL (aTriIndex.w)) : vec4 (0.0f);

      vec3 aReflect = reflect (theRay.Direct, aNormal);

      if (dot (aReflect, aHit.Normal) * dot (theRay.Direct, aHit.Normal) > 0.0f)
      {
        aReflect = reflect (theRay.Direct, aHit.Normal);
      }

      theRay.Direct = aReflect;
    }

    if (all (lessThanEqual (aWeight.xyz, THRESHOLD)))
    {
      aDepth = INVALID_BOUNCES;
    }
    else if (aOpacity.x == 1.0f || aOpacity.z != 1.0f) // if no simple transparency
    {
      theRay.Origin += aHit.Normal * mix (
        -uSceneEpsilon, uSceneEpsilon, step (0.0f, dot (aHit.Normal, theRay.Direct)));

      theInverse = 1.0f / max (abs (theRay.Direct), SMALL);

      theInverse = mix (-theInverse, theInverse, step (ZERO, theRay.Direct));

      anOpenGlDepth = MAXFLOAT; // disable combining image with OpenGL output
    }

    theRay.Origin += theRay.Direct * uSceneEpsilon;
  }

  return vec4 (aResult.x,
               aResult.y,
               aResult.z,
               aWeight.w);
}
