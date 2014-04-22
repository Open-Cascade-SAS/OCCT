//! Normalized pixel coordinates.
in vec2 vPixel;

//! Origin of viewing ray in left-top corner.
uniform vec3 uOriginLT;
//! Origin of viewing ray in left-bottom corner.
uniform vec3 uOriginLB;
//! Origin of viewing ray in right-top corner.
uniform vec3 uOriginRT;
//! Origin of viewing ray in right-bottom corner.
uniform vec3 uOriginRB;

//! Direction of viewing ray in left-top corner.
uniform vec3 uDirectLT;
//! Direction of viewing ray in left-bottom corner.
uniform vec3 uDirectLB;
//! Direction of viewing ray in right-top corner.
uniform vec3 uDirectRT;
//! Direction of viewing ray in right-bottom corner.
uniform vec3 uDirectRB;

//! Texture buffer of data records of high-level BVH nodes.
uniform isamplerBuffer uSceneNodeInfoTexture;
//! Texture buffer of minimum points of high-level BVH nodes.
uniform samplerBuffer uSceneMinPointTexture;
//! Texture buffer of maximum points of high-level BVH nodes.
uniform samplerBuffer uSceneMaxPointTexture;
//! Texture buffer of transformations of high-level BVH nodes.
uniform samplerBuffer uSceneTransformTexture;

//! Texture buffer of data records of bottom-level BVH nodes.
uniform isamplerBuffer uObjectNodeInfoTexture;
//! Texture buffer of minimum points of bottom-level BVH nodes.
uniform samplerBuffer uObjectMinPointTexture;
//! Texture buffer of maximum points of bottom-level BVH nodes.
uniform samplerBuffer uObjectMaxPointTexture;

//! Texture buffer of vertex coords.
uniform samplerBuffer uGeometryVertexTexture;
//! Texture buffer of vertex normals.
uniform samplerBuffer uGeometryNormalTexture;
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


// =======================================================================
// function : MatrixRowMultiply
// purpose  : Multiplies a vector by matrix
// =======================================================================
vec3 MatrixRowMultiply (in vec4 v,
                        in vec4 m0,
                        in vec4 m1,
                        in vec4 m2,
                        in vec4 m3)
{
  return vec3 (dot (m0, v),
               dot (m1, v),
               dot (m2, v));
}


// =======================================================================
// function : MatrixColMultiply
// purpose  : Multiplies a vector by matrix
// =======================================================================
vec3 MatrixColMultiply (in vec4 v,
                        in vec4 m0,
                        in vec4 m1,
                        in vec4 m2,
                        in vec4 m3)
{
  return vec3 (m0[0] * v.x + m1[0] * v.y + m2[0] * v.z + m3[0] * v.w,
               m0[1] * v.x + m1[1] * v.y + m2[1] * v.z + m3[1] * v.w,
               m0[2] * v.x + m1[2] * v.y + m2[2] * v.z + m3[2] * v.w);
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
  
  return SRay (mix (aP0, aP1, thePixel.y),
               mix (aD0, aD1, thePixel.y));
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
  int aHead = theSentinel; // stack pointer
  int aNode = 0;           // node to visit

  ivec4 aTriIndex = INALID_HIT;

  float aTimeOut;
  float aTimeLft;
  float aTimeRgh;

  while (true)
  {
    ivec3 aData = texelFetch (uObjectNodeInfoTexture, aNode + theBVHOffset).xyz;

    if (aData.x == 0) // if inner node
    {
      vec3 aNodeMinLft = texelFetch (uObjectMinPointTexture, aData.y + theBVHOffset).xyz;
      vec3 aNodeMaxLft = texelFetch (uObjectMaxPointTexture, aData.y + theBVHOffset).xyz;
      vec3 aNodeMinRgh = texelFetch (uObjectMinPointTexture, aData.z + theBVHOffset).xyz;
      vec3 aNodeMaxRgh = texelFetch (uObjectMaxPointTexture, aData.z + theBVHOffset).xyz;

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
          if (aHead == theSentinel)
            return aTriIndex;
            
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
                                         
        if (aTime < theHit.Time)
        {
          aTriIndex = aTriangle;
          
          theHit = SIntersect (aTime, aParams, aNormal);
        }
      }
      
      if (aHead == theSentinel)
        return aTriIndex;

      aNode = Stack[aHead--];
    }
  }

  return aTriIndex;
}

// =======================================================================
// function : ObjectAnyHit
// purpose  : Finds intersection with any object triangle
// =======================================================================
float ObjectAnyHit (in int theBVHOffset, in int theVrtOffset, in int theTrgOffset,
  in SRay theRay, in vec3 theInverse, in float theDistance, in int theSentinel)
{
  int aHead = theSentinel; // stack pointer
  int aNode = 0;           // node to visit

  float aTimeOut;
  float aTimeLft;
  float aTimeRgh;

  while (true)
  {
    ivec4 aData = texelFetch (uObjectNodeInfoTexture, aNode + theBVHOffset);

    if (aData.x == 0) // if inner node
    {
      vec3 aNodeMinLft = texelFetch (uObjectMinPointTexture, aData.y + theBVHOffset).xyz;
      vec3 aNodeMaxLft = texelFetch (uObjectMaxPointTexture, aData.y + theBVHOffset).xyz;
      vec3 aNodeMinRgh = texelFetch (uObjectMinPointTexture, aData.z + theBVHOffset).xyz;
      vec3 aNodeMaxRgh = texelFetch (uObjectMaxPointTexture, aData.z + theBVHOffset).xyz;

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
          if (aHead == theSentinel)
            return 1.0f;

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
                                         
        if (aTime < theDistance)
          return 0.0f;
      }
      
      if (aHead == theSentinel)
        return 1.0f;

      aNode = Stack[aHead--];
    }
  }

  return 1.0f;
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
  
  float aTimeOut;
  float aTimeLft;
  float aTimeRgh;

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

        SRay aNewRay;

        aNewRay.Origin = MatrixColMultiply (vec4 (theRay.Origin, 1.0f), 
          aInvTransf0, aInvTransf1, aInvTransf2, aInvTransf3);

        aNewRay.Direct = MatrixColMultiply (vec4 (theRay.Direct, 0.0f), 
          aInvTransf0, aInvTransf1, aInvTransf2, aInvTransf3);

        vec3 aNewInverse = 1.0f / max (abs (aNewRay.Direct), SMALL);
        
        aNewInverse.x = aNewRay.Direct.x < 0.0f ? -aNewInverse.x : aNewInverse.x;
        aNewInverse.y = aNewRay.Direct.y < 0.0f ? -aNewInverse.y : aNewInverse.y;
        aNewInverse.z = aNewRay.Direct.z < 0.0f ? -aNewInverse.z : aNewInverse.z;

        ivec4 aTriIndex = ObjectNearestHit (
          aData.y, aData.z, aData.w, aNewRay, aNewInverse, theHit, aHead);

        if (aTriIndex.x != -1)
        {
          aHitObject = ivec4 (aTriIndex.x + aData.z,  // vertex 0
                              aTriIndex.y + aData.z,  // vertex 1
                              aTriIndex.z + aData.z,  // vertex 2
                              aTriIndex.w);           // material

          theObjectId = anObjectId;
        }
      }
      
      if (aHead < 0)
        return aHitObject;
            
      aNode = Stack[aHead--];
    }
    else // if inner node
    {
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
  
  float aTimeOut;
  float aTimeLft;
  float aTimeRgh;

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

      SRay aNewRay;

      aNewRay.Origin = MatrixColMultiply (vec4 (theRay.Origin, 1.0f), 
        aInvTransf0, aInvTransf1, aInvTransf2, aInvTransf3);

      aNewRay.Direct = MatrixColMultiply (vec4 (theRay.Direct, 0.0f), 
        aInvTransf0, aInvTransf1, aInvTransf2, aInvTransf3);

      vec3 aNewInverse = 1.0f / max (abs (aNewRay.Direct), SMALL);
      
      aNewInverse.x = aNewRay.Direct.x < 0.0f ? -aNewInverse.x : aNewInverse.x;
      aNewInverse.y = aNewRay.Direct.y < 0.0f ? -aNewInverse.y : aNewInverse.y;
      aNewInverse.z = aNewRay.Direct.z < 0.0f ? -aNewInverse.z : aNewInverse.z;

      bool isShadow = 0.0f == ObjectAnyHit (
        aData.y, aData.z, aData.w, aNewRay, aNewInverse, theDistance, aHead);
        
      if (aHead < 0 || isShadow)
        return isShadow ? 0.0f : 1.0f;
            
      aNode = Stack[aHead--];
    }
    else // if inner node
    {
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
          if (aHead < 0)
            return 1.0f;

          aNode = Stack[aHead--];
        }
      }
    }
  }
  
  return 1.0f;
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
// function : Refract
// purpose  :
// =======================================================================
vec3 Refract (in vec3 theInput,
              in vec3 theNormal,
              in float theRefractIndex,
              in float theInvRefractIndex)
{
  float aNdotI  = dot (theInput, theNormal);
  
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

#define THRESHOLD vec3 (0.1f, 0.1f, 0.1f)

#define MATERIAL_AMBN(index) (7 * index + 0)
#define MATERIAL_DIFF(index) (7 * index + 1)
#define MATERIAL_SPEC(index) (7 * index + 2)
#define MATERIAL_EMIS(index) (7 * index + 3)
#define MATERIAL_REFL(index) (7 * index + 4)
#define MATERIAL_REFR(index) (7 * index + 5)
#define MATERIAL_TRAN(index) (7 * index + 6)

#define LIGHT_POS(index) (2 * index + 1)
#define LIGHT_PWR(index) (2 * index + 0)

// =======================================================================
// function : Radiance
// purpose  : Computes color of specified ray
// =======================================================================
vec4 Radiance (in SRay theRay, in vec3 theInverse)
{
  vec3 aResult = vec3 (0.0f);
  vec4 aWeight = vec4 (1.0f);

  int anObjectId;
  
  for (int aDepth = 0; aDepth < 5; ++aDepth)
  {
    SIntersect aHit = SIntersect (MAXFLOAT, vec2 (ZERO), ZERO);
    
    ivec4 aTriIndex = SceneNearestHit (theRay, theInverse, aHit, anObjectId);

    if (aTriIndex.x == -1)
    {
      if (aWeight.w != 0.0f)
      {
        return vec4 (aResult.x,
                     aResult.y,
                     aResult.z,
                     aWeight.w);
      }

      if (bool(uEnvironmentEnable))
      {
        float aTime = IntersectSphere (theRay, uSceneRadius);
        
        aResult.xyz += aWeight.xyz * textureLod (uEnvironmentMapTexture,
          Latlong (theRay.Direct * aTime + theRay.Origin, uSceneRadius), 0.0f).xyz;
      }
      
      return vec4 (aResult.x,
                   aResult.y,
                   aResult.z,
                   aWeight.w);
    }
    
    vec3 aPoint = theRay.Direct * aHit.Time + theRay.Origin;
    
    vec3 aAmbient  = texelFetch (
      uRaytraceMaterialTexture, MATERIAL_AMBN (aTriIndex.w)).rgb;
    vec3 aDiffuse  = texelFetch (
      uRaytraceMaterialTexture, MATERIAL_DIFF (aTriIndex.w)).rgb;
    vec4 aSpecular = texelFetch (
      uRaytraceMaterialTexture, MATERIAL_SPEC (aTriIndex.w));
    vec4 aOpacity  = texelFetch (
      uRaytraceMaterialTexture, MATERIAL_TRAN (aTriIndex.w));
      
    vec3 aNormal = SmoothNormal (aHit.UV, aTriIndex);

    vec4 aInvTransf0 = texelFetch (uSceneTransformTexture, anObjectId * 4 + 0);
    vec4 aInvTransf1 = texelFetch (uSceneTransformTexture, anObjectId * 4 + 1);
    vec4 aInvTransf2 = texelFetch (uSceneTransformTexture, anObjectId * 4 + 2);
    vec4 aInvTransf3 = texelFetch (uSceneTransformTexture, anObjectId * 4 + 3);

    aNormal = MatrixRowMultiply (vec4 (aNormal, 0.0f), aInvTransf0, aInvTransf1, aInvTransf2, aInvTransf3);
    aNormal = normalize (aNormal);
    
    aHit.Normal = normalize (aHit.Normal);
    
    for (int aLightIdx = 0; aLightIdx < uLightCount; ++aLightIdx)
    {
      vec4 aLight = texelFetch (
        uRaytraceLightSrcTexture, LIGHT_POS (aLightIdx));
      
      float aDistance = MAXFLOAT;
      
      if (aLight.w != 0.0f) // point light source
      {
        aDistance = length (aLight.xyz -= aPoint);
        
        aLight.xyz *= 1.0f / aDistance;
      }

      SRay aShadow = SRay (aPoint + aLight.xyz * uSceneEpsilon, aLight.xyz);
      
      aShadow.Origin += aHit.Normal * uSceneEpsilon *
        (dot (aHit.Normal, aLight.xyz) >= 0.0f ? 1.0f : -1.0f);
      
      float aVisibility = 1.0f;
     
      if (bool(uShadowsEnable))
      {
        vec3 aInverse = 1.0f / max (abs (aLight.xyz), SMALL);
        
        aInverse.x = aLight.x < 0.0f ? -aInverse.x : aInverse.x;
        aInverse.y = aLight.y < 0.0f ? -aInverse.y : aInverse.y;
        aInverse.z = aLight.z < 0.0f ? -aInverse.z : aInverse.z;
        
        aVisibility = SceneAnyHit (aShadow, aInverse, aDistance);
      }
      
      if (aVisibility > 0.0f)
      {
        vec3 aIntensity = vec3 (texelFetch (
          uRaytraceLightSrcTexture, LIGHT_PWR (aLightIdx)));
 
        float aLdotN = dot (aShadow.Direct, aNormal);
        
        if (aOpacity.y > 0.0f)    // force two-sided lighting
          aLdotN = abs (aLdotN); // for transparent surfaces
          
        if (aLdotN > 0.0f)
        {
          float aRdotV = dot (reflect (aShadow.Direct, aNormal), theRay.Direct);
          
          aResult.xyz += aWeight.xyz * aOpacity.x * aIntensity *
            (aDiffuse * aLdotN + aSpecular.xyz * pow (max (0.0f, aRdotV), aSpecular.w));
        }
      }
    }
    
    aResult.xyz += aWeight.xyz * uGlobalAmbient.xyz *
      aAmbient * aOpacity.x * max (abs (dot (aNormal, theRay.Direct)), 0.5f);
    
    if (aOpacity.x != 1.0f)
    {
      aWeight *= aOpacity.y;
      
      theRay.Direct = Refract (theRay.Direct, aNormal, aOpacity.z, aOpacity.w);

      theInverse = 1.0f / max (abs (theRay.Direct), SMALL);
      
      theInverse.x = theRay.Direct.x < 0.0f ? -theInverse.x : theInverse.x;
      theInverse.y = theRay.Direct.y < 0.0f ? -theInverse.y : theInverse.y;
      theInverse.z = theRay.Direct.z < 0.0f ? -theInverse.z : theInverse.z;
      
      aPoint += aHit.Normal * (dot (aHit.Normal, theRay.Direct) >= 0.0f ? uSceneEpsilon : -uSceneEpsilon);
    }
    else
    {
      aWeight *= bool(uReflectionsEnable) ?
        texelFetch (uRaytraceMaterialTexture, MATERIAL_REFL (aTriIndex.w)) : vec4 (0.0f);
      
      theRay.Direct = reflect (theRay.Direct, aNormal);
      
      if (dot (theRay.Direct, aHit.Normal) < 0.0f)
      {
        theRay.Direct = reflect (theRay.Direct, aHit.Normal);      
      }

      theInverse = 1.0f / max (abs (theRay.Direct), SMALL);
      
      theInverse.x = theRay.Direct.x < 0.0f ? -theInverse.x : theInverse.x;
      theInverse.y = theRay.Direct.y < 0.0f ? -theInverse.y : theInverse.y;
      theInverse.z = theRay.Direct.z < 0.0f ? -theInverse.z : theInverse.z;
      
      aPoint += aHit.Normal * (dot (aHit.Normal, theRay.Direct) >= 0.0f ? uSceneEpsilon : -uSceneEpsilon);
    }
    
    if (all (lessThanEqual (aWeight.xyz, THRESHOLD)))
    {
      return vec4 (aResult.x,
                   aResult.y,
                   aResult.z,
                   aWeight.w);
    }
    
    theRay.Origin = theRay.Direct * uSceneEpsilon + aPoint;
  }

  return vec4 (aResult.x,
               aResult.y,
               aResult.z,
               aWeight.w);
}
