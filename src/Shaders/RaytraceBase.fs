#ifdef ADAPTIVE_SAMPLING
  #extension GL_ARB_shader_image_load_store : require
  #extension GL_NV_shader_atomic_float : require
#endif

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

//! Model-view-projection matrix.
uniform mat4 uViewMat;

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

//! Total number of light sources.
uniform int uLightCount;
//! Intensity of global ambient light.
uniform vec4 uGlobalAmbient;

//! Enables/disables hard shadows.
uniform int uShadowsEnabled;
//! Enables/disables specular reflections.
uniform int uReflectEnabled;
//! Enables/disables spherical environment map.
uniform int uSphereMapEnabled;
//! Enables/disables environment map background.
uniform int uSphereMapForBack;

//! Radius of bounding sphere of the scene.
uniform float uSceneRadius;
//! Scene epsilon to prevent self-intersections.
uniform float uSceneEpsilon;

#ifdef USE_TEXTURES
  //! Unique 64-bit handles of OpenGL textures.
  uniform uvec2 uTextureSamplers[MAX_TEX_NUMBER];
#endif

#ifdef ADAPTIVE_SAMPLING
  //! OpenGL image used for accumulating rendering result.
  volatile restrict layout(size1x32) uniform image2D  uRenderImage;

  //! OpenGL image storing offsets of sampled pixels blocks.
  coherent restrict layout(size2x32) uniform iimage2D uOffsetImage;
#endif

//! Top color of gradient background.
uniform vec4 uBackColorTop = vec4 (0.0);
//! Bottom color of gradient background.
uniform vec4 uBackColorBot = vec4 (0.0);

//! Aperture radius of camera used for depth-of-field
uniform float uApertureRadius = 0.f;

//! Focal distance of camera used for depth-of field
uniform float uFocalPlaneDist = 10.f;

//! Camera position used for projective mode
uniform vec3 uEyeOrig;

//! Camera view direction used for projective mode
uniform vec3 uEyeView;

//! Camera's screen vertical direction used for projective mode
uniform vec3 uEyeVert;

//! Camera's screen horizontal direction used for projective mode
uniform vec3 uEyeSide;

//! Camera's screen size used for projective mode
uniform vec2 uEyeSize;

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

#define M_PI   3.141592653f
#define M_2_PI 6.283185307f
#define M_PI_2 1.570796327f

#define LUMA vec3 (0.2126f, 0.7152f, 0.0722f)

// =======================================================================
// function : MatrixRowMultiplyDir
// purpose  : Multiplies a vector by matrix
// =======================================================================
vec3 MatrixRowMultiplyDir (in vec3 v,
                           in vec4 m0,
                           in vec4 m1,
                           in vec4 m2)
{
  return vec3 (dot (m0.xyz, v),
               dot (m1.xyz, v),
               dot (m2.xyz, v));
}

//! 32-bit state of random number generator.
uint RandState;

// =======================================================================
// function : SeedRand
// purpose  : Applies hash function by Thomas Wang to randomize seeds
//            (see http://www.burtleburtle.net/bob/hash/integer.html)
// =======================================================================
void SeedRand (in int theSeed, in int theSizeX, in int theRadius)
{
  RandState = uint (int (gl_FragCoord.y) / theRadius * theSizeX + int (gl_FragCoord.x) / theRadius + theSeed);

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
// purpose  : Generates a random float in 0 <= x < 1 range
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
  return vec3 (m0.x * v.x + m1.x * v.y + m2.x * v.z + m3.x,
               m0.y * v.x + m1.y * v.y + m2.y * v.z + m3.y,
               m0.z * v.x + m1.z * v.y + m2.z * v.z + m3.z);
}

// =======================================================================
// function : MatrixColMultiplyDir
// purpose  : Multiplies a vector by matrix
// =======================================================================
vec3 MatrixColMultiplyDir (in vec3 v,
                           in vec4 m0,
                           in vec4 m1,
                           in vec4 m2)
{
  return vec3 (m0.x * v.x + m1.x * v.y + m2.x * v.z,
               m0.y * v.x + m1.y * v.y + m2.y * v.z,
               m0.z * v.x + m1.z * v.y + m2.z * v.z);
}

//=======================================================================
// function : InverseDirection
// purpose  : Returns safely inverted direction of the given one
//=======================================================================
vec3 InverseDirection (in vec3 theInput)
{
  vec3 anInverse = 1.f / max (abs (theInput), SMALL);

  return mix (-anInverse, anInverse, step (ZERO, theInput));
}

//=======================================================================
// function : BackgroundColor
// purpose  : Returns color of gradient background
//=======================================================================
vec4 BackgroundColor()
{
#ifdef ADAPTIVE_SAMPLING

  ivec2 aFragCoord = ivec2 (gl_FragCoord.xy);

  ivec2 aTileXY = imageLoad (uOffsetImage, ivec2 (aFragCoord.x / BLOCK_SIZE,
                                                  aFragCoord.y / BLOCK_SIZE)).xy;

  aTileXY.y += aFragCoord.y % min (uWinSizeY - aTileXY.y, BLOCK_SIZE);

  return mix (uBackColorBot, uBackColorTop, float (aTileXY.y) / uWinSizeY);

#else

  return mix (uBackColorBot, uBackColorTop, vPixel.y);

#endif
}

/////////////////////////////////////////////////////////////////////////////////////////
// Functions for compute ray-object intersection

//=======================================================================
// function : sampleUniformDisk
// purpose  :
//=======================================================================
vec2 sampleUniformDisk ()
{
  vec2 aPoint;

  float aKsi1 = 2.f * RandFloat () - 1.f;
  float aKsi2 = 2.f * RandFloat () - 1.f;

  if (aKsi1 > -aKsi2)
  {
    if (aKsi1 > aKsi2)
      aPoint = vec2 (aKsi1, (M_PI / 4.f) * (0.f + aKsi2 / aKsi1));
    else
      aPoint = vec2 (aKsi2, (M_PI / 4.f) * (2.f - aKsi1 / aKsi2));
  }
  else
  {
    if (aKsi1 < aKsi2)
      aPoint = vec2 (-aKsi1, (M_PI / 4.f) * (4.f + aKsi2 / aKsi1));
    else
      aPoint = vec2 (-aKsi2, (M_PI / 4.f) * (6.f - aKsi1 / aKsi2));
  }

  return vec2 (sin (aPoint.y), cos (aPoint.y)) * aPoint.x;
}

// =======================================================================
// function : GenerateRay
// purpose  :
// =======================================================================
SRay GenerateRay (in vec2 thePixel)
{
#ifndef DEPTH_OF_FIELD

  vec3 aP0 = mix (uOriginLB, uOriginRB, thePixel.x);
  vec3 aP1 = mix (uOriginLT, uOriginRT, thePixel.x);

  vec3 aD0 = mix (uDirectLB, uDirectRB, thePixel.x);
  vec3 aD1 = mix (uDirectLT, uDirectRT, thePixel.x);

  vec3 aDirection = normalize (mix (aD0, aD1, thePixel.y));

  return SRay (mix (aP0, aP1, thePixel.y), aDirection);

#else

  vec2 aPixel = uEyeSize * (thePixel - vec2 (0.5f)) * 2.f;

  vec2 aAperturePnt = sampleUniformDisk () * uApertureRadius;

  vec3 aLocalDir = normalize (vec3 (
    aPixel * uFocalPlaneDist - aAperturePnt, uFocalPlaneDist));

  vec3 aOrigin = uEyeOrig +
                 uEyeSide * aAperturePnt.x +
                 uEyeVert * aAperturePnt.y;

  vec3 aDirect = uEyeView * aLocalDir.z +
                 uEyeSide * aLocalDir.x +
                 uEyeVert * aLocalDir.y;

  return SRay (aOrigin, aDirect);

#endif
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
void IntersectTriangle (in SRay theRay,
                        in vec3 thePnt0,
                        in vec3 thePnt1,
                        in vec3 thePnt2,
                        out vec3 theUVT,
                        out vec3 theNorm)
{
  vec3 aToTrg = thePnt0 - theRay.Origin;

  vec3 aEdge0 = thePnt1 - thePnt0;
  vec3 aEdge1 = thePnt0 - thePnt2;

  theNorm = cross (aEdge1, aEdge0);

  vec3 theVect = cross (theRay.Direct, aToTrg);

  theUVT = vec3 (dot (theNorm, aToTrg),
                 dot (theVect, aEdge1),
                 dot (theVect, aEdge0)) * (1.f / dot (theNorm, theRay.Direct));

  theUVT.x = any (lessThan (theUVT, ZERO)) || (theUVT.y + theUVT.z) > 1.f ? MAXFLOAT : theUVT.x;
}

#define EMPTY_ROOT ivec4(0)

//! Utility structure containing information about
//! currently traversing sub-tree of scene's BVH.
struct SSubTree
{
  //! Transformed ray.
  SRay  TrsfRay;

  //! Inversed ray direction.
  vec3  Inverse;

  //! Parameters of sub-root node.
  ivec4 SubData;
};

#define MATERIAL_AMBN(index) (19 * index + 0)
#define MATERIAL_DIFF(index) (19 * index + 1)
#define MATERIAL_SPEC(index) (19 * index + 2)
#define MATERIAL_EMIS(index) (19 * index + 3)
#define MATERIAL_REFL(index) (19 * index + 4)
#define MATERIAL_REFR(index) (19 * index + 5)
#define MATERIAL_TRAN(index) (19 * index + 6)
#define MATERIAL_TRS1(index) (19 * index + 7)
#define MATERIAL_TRS2(index) (19 * index + 8)
#define MATERIAL_TRS3(index) (19 * index + 9)

#define TRS_OFFSET(treelet) treelet.SubData.x
#define BVH_OFFSET(treelet) treelet.SubData.y
#define VRT_OFFSET(treelet) treelet.SubData.z
#define TRG_OFFSET(treelet) treelet.SubData.w

//! Identifies the absence of intersection.
#define INALID_HIT ivec4 (-1)

//! Global stack shared between traversal functions.
int Stack[STACK_SIZE];

// =======================================================================
// function : pop
// purpose  :
// =======================================================================
int pop (inout int theHead)
{
  int aData = Stack[theHead];

  int aMask = aData >> 26;
  int aNode = aMask & 0x3;

  aMask >>= 2;

  if ((aMask & 0x3) == aNode)
  {
    --theHead;
  }
  else
  {
    aMask |= (aMask << 2) & 0x30;

    Stack[theHead] = (aData & 0x03FFFFFF) | (aMask << 26);
  }

  return (aData & 0x03FFFFFF) + aNode;
}

// =======================================================================
// function : SceneNearestHit
// purpose  : Finds intersection with nearest scene triangle
// =======================================================================
ivec4 SceneNearestHit (in SRay theRay, in vec3 theInverse, inout SIntersect theHit, out int theTrsfId)
{
  ivec4 aTriIndex = INALID_HIT;

  int aNode =  0; // node to traverse
  int aHead = -1; // pointer of stack
  int aStop = -1; // BVH level switch

  SSubTree aSubTree = SSubTree (theRay, theInverse, EMPTY_ROOT);

  for (bool toContinue = true; toContinue; /* none */)
  {
    ivec4 aData = texelFetch (uSceneNodeInfoTexture, aNode);

    if (aData.x == 0) // if inner node
    {
      aData.y += BVH_OFFSET (aSubTree);

      vec4 aHitTimes = vec4 (MAXFLOAT,
                             MAXFLOAT,
                             MAXFLOAT,
                             MAXFLOAT);

      vec3 aRayOriginInverse = -aSubTree.TrsfRay.Origin * aSubTree.Inverse;

      vec3 aNodeMin0 = texelFetch (uSceneMinPointTexture, aData.y +                0).xyz * aSubTree.Inverse + aRayOriginInverse;
      vec3 aNodeMin1 = texelFetch (uSceneMinPointTexture, aData.y +                1).xyz * aSubTree.Inverse + aRayOriginInverse;
      vec3 aNodeMin2 = texelFetch (uSceneMinPointTexture, aData.y + min (2, aData.z)).xyz * aSubTree.Inverse + aRayOriginInverse;
      vec3 aNodeMin3 = texelFetch (uSceneMinPointTexture, aData.y + min (3, aData.z)).xyz * aSubTree.Inverse + aRayOriginInverse;
      vec3 aNodeMax0 = texelFetch (uSceneMaxPointTexture, aData.y +                0).xyz * aSubTree.Inverse + aRayOriginInverse;
      vec3 aNodeMax1 = texelFetch (uSceneMaxPointTexture, aData.y +                1).xyz * aSubTree.Inverse + aRayOriginInverse;
      vec3 aNodeMax2 = texelFetch (uSceneMaxPointTexture, aData.y + min (2, aData.z)).xyz * aSubTree.Inverse + aRayOriginInverse;
      vec3 aNodeMax3 = texelFetch (uSceneMaxPointTexture, aData.y + min (3, aData.z)).xyz * aSubTree.Inverse + aRayOriginInverse;

      vec3 aTimeMax = max (aNodeMin0, aNodeMax0);
      vec3 aTimeMin = min (aNodeMin0, aNodeMax0);

      float aTimeLeave = min (aTimeMax.x, min (aTimeMax.y, aTimeMax.z));
      float aTimeEnter = max (aTimeMin.x, max (aTimeMin.y, aTimeMin.z));

      aHitTimes.x = (aTimeEnter <= aTimeLeave && aTimeEnter <= theHit.Time && aTimeLeave >= 0.f) ? aTimeEnter : MAXFLOAT;

      aTimeMax = max (aNodeMin1, aNodeMax1);
      aTimeMin = min (aNodeMin1, aNodeMax1);

      aTimeLeave = min (aTimeMax.x, min (aTimeMax.y, aTimeMax.z));
      aTimeEnter = max (aTimeMin.x, max (aTimeMin.y, aTimeMin.z));

      aHitTimes.y = (aTimeEnter <= aTimeLeave && aTimeEnter <= theHit.Time && aTimeLeave >= 0.f) ? aTimeEnter : MAXFLOAT;

      aTimeMax = max (aNodeMin2, aNodeMax2);
      aTimeMin = min (aNodeMin2, aNodeMax2);

      aTimeLeave = min (aTimeMax.x, min (aTimeMax.y, aTimeMax.z));
      aTimeEnter = max (aTimeMin.x, max (aTimeMin.y, aTimeMin.z));

      aHitTimes.z = (aTimeEnter <= aTimeLeave && aTimeEnter <= theHit.Time && aTimeLeave >= 0.f && aData.z > 1) ? aTimeEnter : MAXFLOAT;

      aTimeMax = max (aNodeMin3, aNodeMax3);
      aTimeMin = min (aNodeMin3, aNodeMax3);

      aTimeLeave = min (aTimeMax.x, min (aTimeMax.y, aTimeMax.z));
      aTimeEnter = max (aTimeMin.x, max (aTimeMin.y, aTimeMin.z));

      aHitTimes.w = (aTimeEnter <= aTimeLeave && aTimeEnter <= theHit.Time && aTimeLeave >= 0.f && aData.z > 2) ? aTimeEnter : MAXFLOAT;

      ivec4 aChildren = ivec4 (0, 1, 2, 3);

      aChildren.xy = aHitTimes.y < aHitTimes.x ? aChildren.yx : aChildren.xy;
      aHitTimes.xy = aHitTimes.y < aHitTimes.x ? aHitTimes.yx : aHitTimes.xy;
      aChildren.zw = aHitTimes.w < aHitTimes.z ? aChildren.wz : aChildren.zw;
      aHitTimes.zw = aHitTimes.w < aHitTimes.z ? aHitTimes.wz : aHitTimes.zw;
      aChildren.xz = aHitTimes.z < aHitTimes.x ? aChildren.zx : aChildren.xz;
      aHitTimes.xz = aHitTimes.z < aHitTimes.x ? aHitTimes.zx : aHitTimes.xz;
      aChildren.yw = aHitTimes.w < aHitTimes.y ? aChildren.wy : aChildren.yw;
      aHitTimes.yw = aHitTimes.w < aHitTimes.y ? aHitTimes.wy : aHitTimes.yw;
      aChildren.yz = aHitTimes.z < aHitTimes.y ? aChildren.zy : aChildren.yz;
      aHitTimes.yz = aHitTimes.z < aHitTimes.y ? aHitTimes.zy : aHitTimes.yz;

      if (aHitTimes.x != MAXFLOAT)
      {
        int aHitMask = (aHitTimes.w != MAXFLOAT ? aChildren.w : aChildren.z) << 2
                     | (aHitTimes.z != MAXFLOAT ? aChildren.z : aChildren.y);

        if (aHitTimes.y != MAXFLOAT)
          Stack[++aHead] = aData.y | (aHitMask << 2 | aChildren.y) << 26;

        aNode = aData.y + aChildren.x;
      }
      else
      {
        toContinue = (aHead >= 0);

        if (aHead == aStop) // go to top-level BVH
        {
          aStop = -1; aSubTree = SSubTree (theRay, theInverse, EMPTY_ROOT);
        }

        if (aHead >= 0)
          aNode = pop (aHead);
      }
    }
    else if (aData.x < 0) // leaf node (contains triangles)
    {
      vec3 aNormal;
      vec3 aTimeUV;

      for (int anIdx = aData.y; anIdx <= aData.z; ++anIdx)
      {
        ivec4 aTriangle = texelFetch (uGeometryTriangTexture, anIdx + TRG_OFFSET (aSubTree));

        vec3 aPoint0 = texelFetch (uGeometryVertexTexture, aTriangle.x += VRT_OFFSET (aSubTree)).xyz;
        vec3 aPoint1 = texelFetch (uGeometryVertexTexture, aTriangle.y += VRT_OFFSET (aSubTree)).xyz;
        vec3 aPoint2 = texelFetch (uGeometryVertexTexture, aTriangle.z += VRT_OFFSET (aSubTree)).xyz;

        IntersectTriangle (aSubTree.TrsfRay, aPoint0, aPoint1, aPoint2, aTimeUV, aNormal);

        if (aTimeUV.x < theHit.Time)
        {
          aTriIndex = aTriangle;

          theTrsfId = TRS_OFFSET (aSubTree);

          theHit = SIntersect (aTimeUV.x, aTimeUV.yz, aNormal);
        }
      }

      toContinue = (aHead >= 0);

      if (aHead == aStop) // go to top-level BVH
      {
        aStop = -1; aSubTree = SSubTree (theRay, theInverse, EMPTY_ROOT);
      }

      if (aHead >= 0)
        aNode = pop (aHead);
    }
    else if (aData.x > 0) // switch node
    {
      aSubTree.SubData = ivec4 (4 * aData.x - 4, aData.yzw); // store BVH sub-root

      vec4 aInvTransf0 = texelFetch (uSceneTransformTexture, TRS_OFFSET (aSubTree) + 0);
      vec4 aInvTransf1 = texelFetch (uSceneTransformTexture, TRS_OFFSET (aSubTree) + 1);
      vec4 aInvTransf2 = texelFetch (uSceneTransformTexture, TRS_OFFSET (aSubTree) + 2);
      vec4 aInvTransf3 = texelFetch (uSceneTransformTexture, TRS_OFFSET (aSubTree) + 3);

      aSubTree.TrsfRay.Direct = MatrixColMultiplyDir (theRay.Direct,
                                                      aInvTransf0,
                                                      aInvTransf1,
                                                      aInvTransf2);

      aSubTree.Inverse = mix (-UNIT, UNIT, step (ZERO, aSubTree.TrsfRay.Direct)) /
        max (abs (aSubTree.TrsfRay.Direct), SMALL);

      aSubTree.TrsfRay.Origin = MatrixColMultiplyPnt (theRay.Origin,
                                                      aInvTransf0,
                                                      aInvTransf1,
                                                      aInvTransf2,
                                                      aInvTransf3);

      aNode = BVH_OFFSET (aSubTree); // go to sub-root node

      aStop = aHead; // store current stack pointer
    }
  }

  return aTriIndex;
}

// =======================================================================
// function : SceneAnyHit
// purpose  : Finds intersection with any scene triangle
// =======================================================================
float SceneAnyHit (in SRay theRay, in vec3 theInverse, in float theDistance)
{
  float aFactor = 1.f;

  int aNode =  0; // node to traverse
  int aHead = -1; // pointer of stack
  int aStop = -1; // BVH level switch

  SSubTree aSubTree = SSubTree (theRay, theInverse, EMPTY_ROOT);

  for (bool toContinue = true; toContinue; /* none */)
  {
    ivec4 aData = texelFetch (uSceneNodeInfoTexture, aNode);

    if (aData.x == 0) // if inner node
    {
      aData.y += BVH_OFFSET (aSubTree);

      vec4 aHitTimes = vec4 (MAXFLOAT,
                             MAXFLOAT,
                             MAXFLOAT,
                             MAXFLOAT);

      vec3 aRayOriginInverse = -aSubTree.TrsfRay.Origin * aSubTree.Inverse;

      vec3 aNodeMin0 = texelFetch (uSceneMinPointTexture, aData.y +                0).xyz * aSubTree.Inverse + aRayOriginInverse;
      vec3 aNodeMin1 = texelFetch (uSceneMinPointTexture, aData.y +                1).xyz * aSubTree.Inverse + aRayOriginInverse;
      vec3 aNodeMin2 = texelFetch (uSceneMinPointTexture, aData.y + min (2, aData.z)).xyz * aSubTree.Inverse + aRayOriginInverse;
      vec3 aNodeMin3 = texelFetch (uSceneMinPointTexture, aData.y + min (3, aData.z)).xyz * aSubTree.Inverse + aRayOriginInverse;
      vec3 aNodeMax0 = texelFetch (uSceneMaxPointTexture, aData.y +                0).xyz * aSubTree.Inverse + aRayOriginInverse;
      vec3 aNodeMax1 = texelFetch (uSceneMaxPointTexture, aData.y +                1).xyz * aSubTree.Inverse + aRayOriginInverse;
      vec3 aNodeMax2 = texelFetch (uSceneMaxPointTexture, aData.y + min (2, aData.z)).xyz * aSubTree.Inverse + aRayOriginInverse;
      vec3 aNodeMax3 = texelFetch (uSceneMaxPointTexture, aData.y + min (3, aData.z)).xyz * aSubTree.Inverse + aRayOriginInverse;

      vec3 aTimeMax = max (aNodeMin0, aNodeMax0);
      vec3 aTimeMin = min (aNodeMin0, aNodeMax0);

      float aTimeLeave = min (aTimeMax.x, min (aTimeMax.y, aTimeMax.z));
      float aTimeEnter = max (aTimeMin.x, max (aTimeMin.y, aTimeMin.z));

      aHitTimes.x = (aTimeEnter <= aTimeLeave && aTimeEnter <= theDistance && aTimeLeave >= 0.f) ? aTimeEnter : MAXFLOAT;

      aTimeMax = max (aNodeMin1, aNodeMax1);
      aTimeMin = min (aNodeMin1, aNodeMax1);

      aTimeLeave = min (aTimeMax.x, min (aTimeMax.y, aTimeMax.z));
      aTimeEnter = max (aTimeMin.x, max (aTimeMin.y, aTimeMin.z));

      aHitTimes.y = (aTimeEnter <= aTimeLeave && aTimeEnter <= theDistance && aTimeLeave >= 0.f) ? aTimeEnter : MAXFLOAT;

      aTimeMax = max (aNodeMin2, aNodeMax2);
      aTimeMin = min (aNodeMin2, aNodeMax2);

      aTimeLeave = min (aTimeMax.x, min (aTimeMax.y, aTimeMax.z));
      aTimeEnter = max (aTimeMin.x, max (aTimeMin.y, aTimeMin.z));

      aHitTimes.z = (aTimeEnter <= aTimeLeave && aTimeEnter <= theDistance && aTimeLeave >= 0.f && aData.z > 1) ? aTimeEnter : MAXFLOAT;

      aTimeMax = max (aNodeMin3, aNodeMax3);
      aTimeMin = min (aNodeMin3, aNodeMax3);

      aTimeLeave = min (aTimeMax.x, min (aTimeMax.y, aTimeMax.z));
      aTimeEnter = max (aTimeMin.x, max (aTimeMin.y, aTimeMin.z));

      aHitTimes.w = (aTimeEnter <= aTimeLeave && aTimeEnter <= theDistance && aTimeLeave >= 0.f && aData.z > 2) ? aTimeEnter : MAXFLOAT;

      ivec4 aChildren = ivec4 (0, 1, 2, 3);

      aChildren.xy = aHitTimes.y < aHitTimes.x ? aChildren.yx : aChildren.xy;
      aHitTimes.xy = aHitTimes.y < aHitTimes.x ? aHitTimes.yx : aHitTimes.xy;
      aChildren.zw = aHitTimes.w < aHitTimes.z ? aChildren.wz : aChildren.zw;
      aHitTimes.zw = aHitTimes.w < aHitTimes.z ? aHitTimes.wz : aHitTimes.zw;
      aChildren.xz = aHitTimes.z < aHitTimes.x ? aChildren.zx : aChildren.xz;
      aHitTimes.xz = aHitTimes.z < aHitTimes.x ? aHitTimes.zx : aHitTimes.xz;
      aChildren.yw = aHitTimes.w < aHitTimes.y ? aChildren.wy : aChildren.yw;
      aHitTimes.yw = aHitTimes.w < aHitTimes.y ? aHitTimes.wy : aHitTimes.yw;
      aChildren.yz = aHitTimes.z < aHitTimes.y ? aChildren.zy : aChildren.yz;
      aHitTimes.yz = aHitTimes.z < aHitTimes.y ? aHitTimes.zy : aHitTimes.yz;

      if (aHitTimes.x != MAXFLOAT)
      {
        int aHitMask = (aHitTimes.w != MAXFLOAT ? aChildren.w : aChildren.z) << 2
                     | (aHitTimes.z != MAXFLOAT ? aChildren.z : aChildren.y);

        if (aHitTimes.y != MAXFLOAT)
          Stack[++aHead] = aData.y | (aHitMask << 2 | aChildren.y) << 26;

        aNode = aData.y + aChildren.x;
      }
      else
      {
        toContinue = (aHead >= 0);

        if (aHead == aStop) // go to top-level BVH
        {
          aStop = -1; aSubTree = SSubTree (theRay, theInverse, EMPTY_ROOT);
        }

        if (aHead >= 0)
          aNode = pop (aHead);
      }
    }
    else if (aData.x < 0) // leaf node
    {
      vec3 aNormal;
      vec3 aTimeUV;

      for (int anIdx = aData.y; anIdx <= aData.z; ++anIdx)
      {
        ivec4 aTriangle = texelFetch (uGeometryTriangTexture, anIdx + TRG_OFFSET (aSubTree));

        vec3 aPoint0 = texelFetch (uGeometryVertexTexture, aTriangle.x += VRT_OFFSET (aSubTree)).xyz;
        vec3 aPoint1 = texelFetch (uGeometryVertexTexture, aTriangle.y += VRT_OFFSET (aSubTree)).xyz;
        vec3 aPoint2 = texelFetch (uGeometryVertexTexture, aTriangle.z += VRT_OFFSET (aSubTree)).xyz;

        IntersectTriangle (aSubTree.TrsfRay, aPoint0, aPoint1, aPoint2, aTimeUV, aNormal);

#ifdef TRANSPARENT_SHADOWS
        if (aTimeUV.x < theDistance)
        {
          aFactor *= 1.f - texelFetch (uRaytraceMaterialTexture, MATERIAL_TRAN (aTriangle.w)).x;
        }
#else
        if (aTimeUV.x < theDistance)
        {
          aFactor = 0.f;
        }
#endif
      }

      toContinue = (aHead >= 0) && (aFactor > 0.1f);

      if (aHead == aStop) // go to top-level BVH
      {
        aStop = -1; aSubTree = SSubTree (theRay, theInverse, EMPTY_ROOT);
      }

      if (aHead >= 0)
        aNode = pop (aHead);
    }
    else if (aData.x > 0) // switch node
    {
      aSubTree.SubData = ivec4 (4 * aData.x - 4, aData.yzw); // store BVH sub-root

      vec4 aInvTransf0 = texelFetch (uSceneTransformTexture, TRS_OFFSET (aSubTree) + 0);
      vec4 aInvTransf1 = texelFetch (uSceneTransformTexture, TRS_OFFSET (aSubTree) + 1);
      vec4 aInvTransf2 = texelFetch (uSceneTransformTexture, TRS_OFFSET (aSubTree) + 2);
      vec4 aInvTransf3 = texelFetch (uSceneTransformTexture, TRS_OFFSET (aSubTree) + 3);

      aSubTree.TrsfRay.Direct = MatrixColMultiplyDir (theRay.Direct,
                                                      aInvTransf0,
                                                      aInvTransf1,
                                                      aInvTransf2);

      aSubTree.TrsfRay.Origin = MatrixColMultiplyPnt (theRay.Origin,
                                                      aInvTransf0,
                                                      aInvTransf1,
                                                      aInvTransf2,
                                                      aInvTransf3);

      aSubTree.Inverse = mix (-UNIT, UNIT, step (ZERO, aSubTree.TrsfRay.Direct)) / max (abs (aSubTree.TrsfRay.Direct), SMALL);

      aNode = BVH_OFFSET (aSubTree); // go to sub-root node

      aStop = aHead; // store current stack pointer
    }
  }

  return aFactor;
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

#define POLYGON_OFFSET_UNIT 0.f
#define POLYGON_OFFSET_FACTOR 1.f
#define POLYGON_OFFSET_SCALE 0.006f

// =======================================================================
// function : PolygonOffset
// purpose  : Computes OpenGL polygon offset
// =======================================================================
float PolygonOffset (in vec3 theNormal, in vec3 thePoint)
{
  vec4 aProjectedNorm = vec4 (theNormal, -dot (theNormal, thePoint)) * uUnviewMat;

  float aPolygonOffset = POLYGON_OFFSET_UNIT;

  if (aProjectedNorm.z * aProjectedNorm.z > 1e-20f)
  {
    aProjectedNorm.xy *= 1.f / aProjectedNorm.z;

    aPolygonOffset += POLYGON_OFFSET_FACTOR * max (abs (aProjectedNorm.x),
                                                   abs (aProjectedNorm.y));
  }

  return aPolygonOffset;
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
// function : FetchEnvironment
// purpose  :
// =======================================================================
vec4 FetchEnvironment (in vec2 theTexCoord)
{
  return uSphereMapEnabled == 0 ?
    vec4 (0.f, 0.f, 0.f, 1.f) : textureLod (uEnvironmentMapTexture, theTexCoord, 0.f);
}

// =======================================================================
// function : Refract
// purpose  : Computes refraction ray (also handles TIR)
// =======================================================================
#ifndef PATH_TRACING
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
#endif

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
#ifndef PATH_TRACING
vec4 Radiance (in SRay theRay, in vec3 theInverse)
{
  vec3 aResult = vec3 (0.0f);
  vec4 aWeight = vec4 (1.0f);

  int aTrsfId;

  float aRaytraceDepth = MAXFLOAT;

  for (int aDepth = 0; aDepth < NB_BOUNCES; ++aDepth)
  {
    SIntersect aHit = SIntersect (MAXFLOAT, vec2 (ZERO), ZERO);

    ivec4 aTriIndex = SceneNearestHit (theRay, theInverse, aHit, aTrsfId);

    if (aTriIndex.x == -1)
    {
      vec4 aColor = vec4 (0.0);

      if (bool(uSphereMapForBack) || aWeight.w == 0.0f /* reflection */)
      {
        float aTime = IntersectSphere (theRay, uSceneRadius);

        aColor = FetchEnvironment (Latlong (
          theRay.Direct * aTime + theRay.Origin, uSceneRadius));
      }
      else
      {
        aColor = BackgroundColor();
      }

      aResult += aWeight.xyz * aColor.xyz; aWeight.w *= aColor.w;

      break; // terminate path
    }

    vec3 aInvTransf0 = texelFetch (uSceneTransformTexture, aTrsfId + 0).xyz;
    vec3 aInvTransf1 = texelFetch (uSceneTransformTexture, aTrsfId + 1).xyz;
    vec3 aInvTransf2 = texelFetch (uSceneTransformTexture, aTrsfId + 2).xyz;

    aHit.Normal = normalize (vec3 (dot (aInvTransf0, aHit.Normal),
                                   dot (aInvTransf1, aHit.Normal),
                                   dot (aInvTransf2, aHit.Normal)));

    theRay.Origin += theRay.Direct * aHit.Time; // intersection point

    // Evaluate depth on first hit
    if (aDepth == 0)
    {
      vec4 aNDCPoint = uViewMat * vec4 (theRay.Origin, 1.f);

      float aPolygonOffset = PolygonOffset (aHit.Normal, theRay.Origin);
      aRaytraceDepth = (aNDCPoint.z / aNDCPoint.w + aPolygonOffset * POLYGON_OFFSET_SCALE) * 0.5f + 0.5f;
    }

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

      vec4 aTexColor = textureLod (
        sampler2D (uTextureSamplers[int(aDiffuse.w)]), aTexCoord.st, 0.f);

      aDiffuse.rgb *= aTexColor.rgb;
      aAmbient.rgb *= aTexColor.rgb;

      // keep refractive index untouched (Z component)
      aOpacity.xy = vec2 (aTexColor.w * aOpacity.x, 1.0f - aTexColor.w * aOpacity.x);
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

        if (bool(uShadowsEnabled))
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
          vec3 aIntensity = min (UNIT, vec3 (texelFetch (
            uRaytraceLightSrcTexture, LIGHT_PWR (aLightIdx))));

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
    }
    else
    {
      aWeight *= bool(uReflectEnabled) ?
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
    }

    theRay.Origin += theRay.Direct * uSceneEpsilon;
  }

  gl_FragDepth = aRaytraceDepth;

  return vec4 (aResult.x,
               aResult.y,
               aResult.z,
               aWeight.w);
}
#endif
