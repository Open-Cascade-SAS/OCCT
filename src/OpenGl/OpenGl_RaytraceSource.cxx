// Created on: 2013-10-16
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

#ifdef HAVE_CONFIG_H
  #include <config.h>
#endif

#ifdef HAVE_OPENCL

#define EOL "\n"

extern const char THE_RAY_TRACE_OPENCL_SOURCE[] =

  /////////////////////////////////////////////////////////////////////////////////////////
  // Specific data types
  EOL
  //! Stores ray parameters.
  EOL"  typedef struct __SRay"
  EOL"  {"
  EOL"    float4 Origin;"
  EOL"    float4 Direct;"
  EOL"  }"
  EOL"  SRay;"
  EOL
  //! Stores parameters of intersection point.
  EOL"  typedef struct __SIntersect"
  EOL"  {"
  EOL"    float4 Normal;"
  EOL"    float Time;"
  EOL"    float U;"
  EOL"    float V;"
  EOL"  }"
  EOL"  SIntersect;"
  EOL
  EOL
  /////////////////////////////////////////////////////////////////////////////////////////
  // Some useful vector constants
  EOL
  EOL"  #define ZERO ( float4 )( 0.f, 0.f, 0.f, 0.f )"
  EOL"  #define UNIT ( float4 )( 1.f, 1.f, 1.f, 0.f )"
  EOL
  EOL"  #define AXIS_X ( float4 )( 1.f, 0.f, 0.f, 0.f )"
  EOL"  #define AXIS_Y ( float4 )( 0.f, 1.f, 0.f, 0.f )"
  EOL"  #define AXIS_Z ( float4 )( 0.f, 0.f, 1.f, 0.f )"
  EOL
  EOL
  /////////////////////////////////////////////////////////////////////////////////////////
  // Support functions
  EOL
  // =======================================================================
  // function : GenerateRay
  // purpose  : Generates primary ray for current work item
  // =======================================================================
  EOL"  void GenerateRay (SRay* theRay,"
  EOL"                    const float theX,"
  EOL"                    const float theY,"
  EOL"                    const int theSizeX,"
  EOL"                    const int theSizeY,"
  EOL"                    const float16 theOrigins,"
  EOL"                    const float16 theDirects)"
  EOL"  {"
  EOL"    float2 aPixel = (float2) (theX / (float)theSizeX,"
  EOL"                              theY / (float)theSizeY);"
  EOL
  EOL"    float4 aP0 = mix (theOrigins.lo.lo, theOrigins.lo.hi, aPixel.x);"
  EOL"    float4 aP1 = mix (theOrigins.hi.lo, theOrigins.hi.hi, aPixel.x);"
  EOL
  EOL"    theRay->Origin = mix (aP0, aP1, aPixel.y);"
  EOL
  EOL"    aP0 = mix (theDirects.lo.lo, theDirects.lo.hi, aPixel.x);"
  EOL"    aP1 = mix (theDirects.hi.lo, theDirects.hi.hi, aPixel.x);"
  EOL
  EOL"    theRay->Direct = mix (aP0, aP1, aPixel.y);"
  EOL"  }"
  EOL
  EOL
  /////////////////////////////////////////////////////////////////////////////////////////
  // Functions for compute ray-object intersection
  EOL
  EOL"  #define _OOEPS_ exp2( -80.0f )"
  EOL
  // =======================================================================
  // function : IntersectSphere
  // purpose  : Computes ray-sphere intersection
  // =======================================================================
  EOL"  bool IntersectSphere (const SRay* theRay, float theRadius, float* theTime)"
  EOL"  {"
  EOL"    float aDdotD = dot (theRay->Direct.xyz, theRay->Direct.xyz);"
  EOL"    float aDdotO = dot (theRay->Direct.xyz, theRay->Origin.xyz);"
  EOL"    float aOdotO = dot (theRay->Origin.xyz, theRay->Origin.xyz);"
  EOL
  EOL"    float aD = aDdotO * aDdotO - aDdotD * (aOdotO - theRadius * theRadius);"
  EOL
  EOL"    if (aD > 0.f)"
  EOL"    {"
  EOL"      *theTime = (-aDdotO + native_sqrt (aD)) * (1.f / aDdotD);"
  EOL
  EOL"      return *theTime > 0.f;"
  EOL"    }"
  EOL
  EOL"    return false;"
  EOL"  }"
  EOL
  // =======================================================================
  // function : IntersectBox
  // purpose  : Computes ray-box intersection (slab test)
  // =======================================================================
  EOL"  bool IntersectBox (const SRay* theRay,"
  EOL"                     float4 theMinPoint,"
  EOL"                     float4 theMaxPoint,"
  EOL"                     float* theTimeStart,"
  EOL"                     float* theTimeFinal)"
  EOL"  {"
  EOL"    const float4 aInvDirect = (float4)("
  EOL"                    1.f / (fabs (theRay->Direct.x) > _OOEPS_ ?"
  EOL"                           theRay->Direct.x : copysign (_OOEPS_, theRay->Direct.x)),"
  EOL"                    1.f / (fabs (theRay->Direct.y) > _OOEPS_ ?"
  EOL"                           theRay->Direct.y : copysign (_OOEPS_, theRay->Direct.y)),"
  EOL"                    1.f / (fabs (theRay->Direct.z) > _OOEPS_ ?"
  EOL"                           theRay->Direct.z : copysign (_OOEPS_, theRay->Direct.z)),"
  EOL"                    0.f);"
  EOL
  EOL"    const float4 aTime0 = (theMinPoint - theRay->Origin) * aInvDirect;"
  EOL"    const float4 aTime1 = (theMaxPoint - theRay->Origin) * aInvDirect;"
  EOL
  EOL"    const float4 aTimeMax = max (aTime0, aTime1);"
  EOL"    const float4 aTimeMin = min (aTime0, aTime1);"
  EOL
  EOL"    *theTimeFinal = min (aTimeMax.x, min (aTimeMax.y, aTimeMax.z));"
  EOL"    *theTimeStart = max (aTimeMin.x, max (aTimeMin.y, aTimeMin.z));"
  EOL
  EOL"    return (*theTimeStart <= *theTimeFinal) & (*theTimeFinal >= 0.f);"
  EOL"  }"
  EOL
  // =======================================================================
  // function : IntersectNodes
  // purpose  : Computes intersection of ray with two child nodes (boxes)
  // =======================================================================
  EOL"  void IntersectNodes (const SRay* theRay,"
  EOL"                       float4 theMinPoint0,"
  EOL"                       float4 theMaxPoint0,"
  EOL"                       float4 theMinPoint1,"
  EOL"                       float4 theMaxPoint1,"
  EOL"                       float* theTimeStart0,"
  EOL"                       float* theTimeStart1,"
  EOL"                       float theMaxTime)"
  EOL"  {"
  EOL"    const float4 aInvDirect = (float4)("
  EOL"                    1.f / (fabs (theRay->Direct.x) > _OOEPS_ ?"
  EOL"                           theRay->Direct.x : copysign (_OOEPS_, theRay->Direct.x)),"
  EOL"                    1.f / (fabs (theRay->Direct.y) > _OOEPS_ ?"
  EOL"                           theRay->Direct.y : copysign (_OOEPS_, theRay->Direct.y)),"
  EOL"                    1.f / (fabs (theRay->Direct.z) > _OOEPS_ ?"
  EOL"                           theRay->Direct.z : copysign (_OOEPS_, theRay->Direct.z)),"
  EOL"                    0.f);"
  EOL
  EOL"    float4 aTime0 = (theMinPoint0 - theRay->Origin) * aInvDirect;"
  EOL"    float4 aTime1 = (theMaxPoint0 - theRay->Origin) * aInvDirect;"
  EOL
  EOL"    float4 aTimeMax = max (aTime0, aTime1);"
  EOL"    float4 aTimeMin = min (aTime0, aTime1);"
  EOL
  EOL"    aTime0 = (theMinPoint1 - theRay->Origin) * aInvDirect;"
  EOL"    aTime1 = (theMaxPoint1 - theRay->Origin) * aInvDirect;"
  EOL
  EOL"    float aTimeFinal = min (aTimeMax.x, min (aTimeMax.y, aTimeMax.z));"
  EOL"    float aTimeStart = max (aTimeMin.x, max (aTimeMin.y, aTimeMin.z));"
  EOL
  EOL"    aTimeMax = max (aTime0, aTime1);"
  EOL"    aTimeMin = min (aTime0, aTime1);"
  EOL
  EOL"    *theTimeStart0 = (aTimeStart <= aTimeFinal) & (aTimeFinal >= 0.f) & (aTimeStart <= theMaxTime)"
  EOL"                   ? aTimeStart : -MAXFLOAT;"
  EOL
  EOL"    aTimeFinal = min (aTimeMax.x, min (aTimeMax.y, aTimeMax.z));"
  EOL"    aTimeStart = max (aTimeMin.x, max (aTimeMin.y, aTimeMin.z));"
  EOL
  EOL"    *theTimeStart1 = (aTimeStart <= aTimeFinal) & (aTimeFinal >= 0.f) & (aTimeStart <= theMaxTime)"
  EOL"                   ? aTimeStart : -MAXFLOAT;"
  EOL"  }"
  EOL
  // =======================================================================
  // function : IntersectTriangle
  // purpose  : Computes ray-triangle intersection (branchless version)
  // =======================================================================
  EOL"  bool IntersectTriangle (const SRay* theRay,"
  EOL"                          const float4 thePoint0,"
  EOL"                          const float4 thePoint1,"
  EOL"                          const float4 thePoint2,"
  EOL"                          float4* theNormal,"
  EOL"                          float* theTime,"
  EOL"                          float* theU,"
  EOL"                          float* theV)"
  EOL"  {"
  EOL"    const float4 aEdge0 = thePoint1 - thePoint0;"
  EOL"    const float4 aEdge1 = thePoint0 - thePoint2;"
  EOL
  EOL"    *theNormal = cross (aEdge1, aEdge0);"
  EOL
  EOL"    const float4 aEdge2 = (1.f / dot (*theNormal, theRay->Direct)) * (thePoint0 - theRay->Origin);"
  EOL
  EOL"    *theTime = dot (*theNormal, aEdge2);"
  EOL
  EOL"    const float4 theInc = cross (theRay->Direct, aEdge2);"
  EOL
  EOL"    *theU = dot (theInc, aEdge1);"
  EOL"    *theV = dot (theInc, aEdge0);"
  EOL
  EOL"    return (*theTime > 0) & (*theU >= 0.f) & (*theV >= 0.f) & (*theU + *theV <= 1.f);"
  EOL"  }"
  EOL
  /////////////////////////////////////////////////////////////////////////////////////////
  // Support shading functions
  EOL
  EOL"  const sampler_t EnvironmentSampler ="
  EOL"            CLK_NORMALIZED_COORDS_TRUE | CLK_ADDRESS_REPEAT | CLK_FILTER_LINEAR;"
  EOL
  // =======================================================================
  // function : SmoothNormal
  // purpose  : Interpolates normal across the triangle
  // =======================================================================
  EOL"  float4 SmoothNormal (__global float4* theNormals,"
  EOL"                       const SIntersect* theHit,"
  EOL"                       const int4 theIndices)"
  EOL"  {"
  EOL"    float4 aNormal0 = theNormals[theIndices.x],"
  EOL"           aNormal1 = theNormals[theIndices.y],"
  EOL"           aNormal2 = theNormals[theIndices.z];"
  EOL
  EOL"    return fast_normalize (aNormal1 * theHit->U +"
  EOL"                           aNormal2 * theHit->V +"
  EOL"                           aNormal0 * (1.f - theHit->U - theHit->V));"
  EOL"  }"
  EOL
  // =======================================================================
  // function : Shade
  // purpose  : Computes Phong-based illumination
  // =======================================================================
  EOL"  float4 Shade (__global float4* theMaterials,"
  EOL"                const float4 theLight,"
  EOL"                const float4 theView,"
  EOL"                const float4 theNormal,"
  EOL"                const float4 theIntens,"
  EOL"                const float theTranspr,"
  EOL"                const int theMatIndex)"
  EOL"  {"
  EOL"    float aLambert = dot (theNormal, theLight);"
  EOL
  EOL"    aLambert = theTranspr > 0.f ? fabs (aLambert) : aLambert;"
  EOL
  EOL"    if (aLambert > 0.f)"
  EOL"    {"
  EOL"      const float4 aMatDiff = theMaterials[7 * theMatIndex + 1];"
  EOL"      const float4 aMatSpec = theMaterials[7 * theMatIndex + 2];"
  EOL
  EOL"      const float4 aReflect = 2.f * dot (theLight, theNormal) * theNormal - theLight;"
  EOL
  EOL"      const float aSpecular = pow (max (dot (aReflect.xyz, theView.xyz), 0.f), aMatSpec.w);"
  EOL
  EOL"      return theIntens * (aMatDiff * aLambert + aMatSpec * aSpecular);"
  EOL"    }"
  EOL
  EOL"    return ZERO;"
  EOL"  }"
  EOL
  // =======================================================================
  // function : Lat-long
  // purpose  : Converts world direction to environment texture coordinates
  // =======================================================================
  EOL"  float2 Latlong (const float4 theDirect)"
  EOL"  {"
  EOL"    float aPsi = acos( -theDirect.y );"
  EOL"    float aPhi = atan2( theDirect.z, theDirect.x );"
  EOL
  EOL"    aPhi = (aPhi < 0) ? (aPhi + 2.f * M_PI_F) : aPhi;"
  EOL
  EOL"    return (float2) (aPhi / (2.f * M_PI_F), aPsi / M_PI_F);"
  EOL"  }"
  EOL
  /////////////////////////////////////////////////////////////////////////////////////////
  // Core ray tracing function
  EOL
  // =======================================================================
  // function : push
  // purpose  : Pushes BVH node index to local stack
  // =======================================================================
  EOL"  void push (uint* theStack, char* thePos, const uint theValue)"
  EOL"  {"
  EOL"    (*thePos)++;"
  EOL"    theStack[*thePos] = theValue;"
  EOL"  }"
  EOL
  // =======================================================================
  // function : pop
  // purpose  : Pops BVH node index from local stack
  // =======================================================================
  EOL"  void pop (uint* theStack, char* thePos, uint* theValue)"
  EOL"  {"
  EOL"    *theValue = theStack[*thePos];"
  EOL"    (*thePos)--;"
  EOL"  }"
  EOL
  // #define BVH_MINIMIZE_MEM_LOADS
  EOL
  // =======================================================================
  // function : Traverse
  // purpose  : Finds intersection with nearest triangle
  // =======================================================================
  EOL"  int4 Traverse (const SRay* theRay,"
  EOL"                 __global int4* theIndices,"
  EOL"                 __global float4* theVertices,"
  EOL"                 __global float4* theNodeMinPoints,"
  EOL"                 __global float4* theNodeMaxPoints,"
  EOL"                 __global int4* theNodeDataRecords,"
  EOL"                 SIntersect* theHit)"
  EOL"  {"
  EOL"    uint aStack [32];"
  EOL"    char aHead = -1;"
  EOL
  EOL"    uint aNode = 0;" // root node
  EOL
  EOL"    float aTimeMin1;"
  EOL"    float aTimeMin2;"
  EOL
  EOL"    float4 aNodeMinLft;"
  EOL"    float4 aNodeMaxLft;"
  EOL"    float4 aNodeMinRgh;"
  EOL"    float4 aNodeMaxRgh;"
  EOL
  EOL"  #ifdef BVH_MINIMIZE_MEM_LOADS"
  EOL"    aNodeMinLft = theNodeMinPoints[aNode];"
  EOL"    aNodeMaxLft = theNodeMaxPoints[aNode];"
  EOL"  #endif"
  EOL
  EOL"    int4 aTriangleIndex = (int4) (-1);"
  EOL
  EOL"    theHit->Time = MAXFLOAT;"
  EOL
  EOL"  #ifdef BVH_MINIMIZE_MEM_LOADS"
  EOL"    int3 aData = (int3) (1,"
  EOL"                         as_int (aNodeMinLft.w),"
  EOL"                         as_int (aNodeMaxLft.w));"
  EOL
  EOL"    aData = aData.y < 0 ? -aData : aData;"
  EOL"  #endif"
  EOL
  EOL"    while (true)"
  EOL"    {"
  EOL"  #ifndef BVH_MINIMIZE_MEM_LOADS"
  EOL"      int3 aData = theNodeDataRecords[aNode].xyz;"
  EOL"  #endif"
  EOL
  EOL"      if (aData.x != 1)" // if inner node
  EOL"      {"
  EOL"        aNodeMinLft = theNodeMinPoints[aData.y];"
  EOL"        aNodeMinRgh = theNodeMinPoints[aData.z];"
  EOL"        aNodeMaxLft = theNodeMaxPoints[aData.y];"
  EOL"        aNodeMaxRgh = theNodeMaxPoints[aData.z];"
  EOL
  EOL"        IntersectNodes (theRay,"
  EOL"                        aNodeMinLft,"
  EOL"                        aNodeMaxLft,"
  EOL"                        aNodeMinRgh,"
  EOL"                        aNodeMaxRgh,"
  EOL"                        &aTimeMin1,"
  EOL"                        &aTimeMin2,"
  EOL"                        theHit->Time);"
  EOL
  EOL"        const bool aHitLft = (aTimeMin1 != -MAXFLOAT);"
  EOL"        const bool aHitRgh = (aTimeMin2 != -MAXFLOAT);"
  EOL
  EOL"        if (aHitLft & aHitRgh)"
  EOL"        {"
  EOL"          aNode = (aTimeMin1 < aTimeMin2) ? aData.y : aData.z;"
  EOL
  EOL"          push (aStack, &aHead, (aTimeMin1 < aTimeMin2) ? aData.z : aData.y);"
  EOL
  EOL"  #ifdef BVH_MINIMIZE_MEM_LOADS"
  EOL"          aData = (int3) (1,"
  EOL"                          as_int (aTimeMin1 < aTimeMin2 ? aNodeMinLft.w : aNodeMinRgh.w),"
  EOL"                          as_int (aTimeMin1 < aTimeMin2 ? aNodeMaxLft.w : aNodeMaxRgh.w));"
  EOL
  EOL"          aData = aData.y < 0 ? -aData : aData;"
  EOL"  #endif"
  EOL"        }"
  EOL"        else"
  EOL"        {"
  EOL"          if (aHitLft | aHitRgh)"
  EOL"          {"
  EOL"            aNode = aHitLft ? aData.y : aData.z;"
  EOL
  EOL"  #ifdef BVH_MINIMIZE_MEM_LOADS"
  EOL"            aData = (int3) (1,"
  EOL"                            as_int (aHitLft ? aNodeMinLft.w : aNodeMinRgh.w),"
  EOL"                            as_int (aHitLft ? aNodeMaxLft.w : aNodeMaxRgh.w));"
  EOL
  EOL"            aData = aData.y < 0 ? -aData : aData;"
  EOL"  #endif"
  EOL"          }"
  EOL"          else"
  EOL"          {"
  EOL"            if (aHead < 0)"
  EOL"              return aTriangleIndex;"
  EOL
  EOL"            pop (aStack, &aHead, &aNode);"
  EOL
  EOL"  #ifdef BVH_MINIMIZE_MEM_LOADS"
  EOL"            aData = theNodeDataRecords[aNode].xyz;"
  EOL"  #endif"
  EOL"          }"
  EOL"        }"
  EOL"      }"
  EOL"      else " // if leaf node
  EOL"      {"
  EOL"        for (int nTri = aData.y; nTri <= aData.z; ++nTri)"
  EOL"        {"
  EOL"          int4 anIndex = theIndices[nTri];"
  EOL
  EOL"          const float4 aP0 = theVertices[anIndex.x];"
  EOL"          const float4 aP1 = theVertices[anIndex.y];"
  EOL"          const float4 aP2 = theVertices[anIndex.z];"
  EOL
  EOL"          float4 aNormal;"
  EOL
  EOL"          float aTime, aU, aV;"
  EOL
  EOL"          if (IntersectTriangle (theRay, aP0, aP1, aP2, &aNormal, &aTime, &aU, &aV) & (aTime < theHit->Time))"
  EOL"          {"
  EOL"            aTriangleIndex = anIndex;"
  EOL"            theHit->Normal = aNormal;"
  EOL"            theHit->Time = aTime;"
  EOL"            theHit->U = aU;"
  EOL"            theHit->V = aV;"
  EOL"          }"
  EOL"        }"
  EOL
  EOL"        if (aHead < 0)"
  EOL"          return aTriangleIndex;"
  EOL
  EOL"        pop (aStack, &aHead, &aNode);"
  EOL
  EOL"  #ifdef BVH_MINIMIZE_MEM_LOADS"
  EOL"        aData = theNodeDataRecords[aNode].xyz;"
  EOL"  #endif"
  EOL"      }"
  EOL"    }"
  EOL
  EOL"    return aTriangleIndex;"
  EOL"   }"
  EOL
  EOL"  #define TRANSPARENT_SHADOW_"
  EOL
  // =======================================================================
  // function : TraverseShadow
  // purpose  : Finds intersection with any triangle
  // =======================================================================
  EOL"  float TraverseShadow (const SRay* theRay,"
  EOL"                        __global int4* theIndices,"
  EOL"                        __global float4* theVertices,"
  EOL"                        __global float4* materials,"
  EOL"                        __global float4* theNodeMinPoints,"
  EOL"                        __global float4* theNodeMaxPoints,"
  EOL"                        __global int4* theNodeDataRecords,"
  EOL"                        float theDistance)"
  EOL"  {"
  EOL"    uint aStack [32];"
  EOL"    char aHead = -1;"
  EOL
  EOL"    uint aNode = 0;" // root node
  EOL
  EOL"    float aFactor = 1.f;" // light attenuation factor
  EOL
  EOL"    float aTimeMin1;"
  EOL"    float aTimeMin2;"
  EOL
  EOL"    while (true)"
  EOL"    {"
  EOL"      int3 aData = theNodeDataRecords[aNode].xyz;"
  EOL
  EOL"      if (aData.x != 1)" // if inner node
  EOL"      {"
  EOL"        IntersectNodes (theRay,"
  EOL"                        theNodeMinPoints[aData.y],"
  EOL"                        theNodeMaxPoints[aData.y],"
  EOL"                        theNodeMinPoints[aData.z],"
  EOL"                        theNodeMaxPoints[aData.z],"
  EOL"                        &aTimeMin1,"
  EOL"                        &aTimeMin2,"
  EOL"                        theDistance);"
  EOL
  EOL"        const bool aHitLft = (aTimeMin1 != -MAXFLOAT);"
  EOL"        const bool aHitRgh = (aTimeMin2 != -MAXFLOAT);"
  EOL
  EOL"        if (aHitLft & aHitRgh)"
  EOL"        {"
  EOL"          aNode = (aTimeMin1 < aTimeMin2) ? aData.y : aData.z;"
  EOL
  EOL"          push (aStack, &aHead, (aTimeMin1 < aTimeMin2) ? aData.z : aData.y);"
  EOL"        }"
  EOL"        else"
  EOL"        {"
  EOL"          if (aHitLft | aHitRgh)"
  EOL"          {"
  EOL"            aNode = aHitLft ? aData.y : aData.z;"
  EOL"          }"
  EOL"          else"
  EOL"          {"
  EOL"            if (aHead < 0)"
  EOL"              return aFactor;"
  EOL
  EOL"            pop (aStack, &aHead, &aNode);"
  EOL"          }"
  EOL"        }"
  EOL"      }"
  EOL"      else " // if leaf node
  EOL"      {"
  EOL"        for (int nTri = aData.y; nTri <= aData.z; ++nTri)"
  EOL"        {"
  EOL"          int4 anIndex = theIndices[nTri];"
  EOL
  EOL"          const float4 aP0 = theVertices[anIndex.x];"
  EOL"          const float4 aP1 = theVertices[anIndex.y];"
  EOL"          const float4 aP2 = theVertices[anIndex.z];"
  EOL
  EOL"          float4 aNormal;"
  EOL
  EOL"          float aTime, aU, aV;"
  EOL
  EOL"          if (IntersectTriangle (theRay, aP0, aP1, aP2, &aNormal, &aTime, &aU, &aV) & (aTime < theDistance))"
  EOL"          {"
  EOL"  #ifdef TRANSPARENT_SHADOW"
  EOL"            aFactor *= materials[7 * index.w + 6].x;"
  EOL
  EOL"            if (aFactor < 0.1f)"
  EOL"              return aFactor;"
  EOL"  #else"
  EOL"            return 0.f;"
  EOL"  #endif"
  EOL"          }"
  EOL"        }"
  EOL
  EOL"        if (aHead < 0)"
  EOL"          return aFactor;"
  EOL
  EOL"        pop (aStack, &aHead, &aNode);"
  EOL"      }"
  EOL"    }"
  EOL
  EOL"    return aFactor;"
  EOL"  }"
  EOL
  EOL"  #define _MAX_DEPTH_ 5"
  EOL
  EOL"  #define _MAT_SIZE_ 7"
  EOL
  EOL"  #define _LGH_SIZE_ 3"
  EOL
  // =======================================================================
  // function : Raytrace
  // purpose  : Computes color of specified ray
  // =======================================================================
  EOL"  float4 Raytrace (SRay* theRay,"
  EOL"                   __read_only image2d_t theEnvMap,"
  EOL"                   __global float4* theNodeMinPoints,"
  EOL"                   __global float4* theNodeMaxPoints,"
  EOL"                   __global int4* theNodeDataRecords,"
  EOL"                   __global float4* theLightSources,"
  EOL"                   __global float4* theMaterials,"
  EOL"                   __global float4* theVertices,"
  EOL"                   __global float4* theNormals,"
  EOL"                   __global int4* theIndices,"
  EOL"                   const int theLightCount,"
  EOL"                   const float theEpsilon,"
  EOL"                   const float theRadius,"
  EOL"                   const int isShadows,"
  EOL"                   const int isReflect)"
  EOL"  {"
  EOL"    float4 aResult = (float4) (0.f, 0.f, 0.f, 0.f);"
  EOL"    float4 aWeight = (float4) (1.f, 1.f, 1.f, 1.f);"
  EOL
  EOL"    SIntersect aHit;"
  EOL
  EOL"    for (int aDepth = 0; aDepth < _MAX_DEPTH_; ++aDepth)"
  EOL"    {"
  EOL"      int4 aTriangle = Traverse (theRay,"
  EOL"                                 theIndices,"
  EOL"                                 theVertices,"
  EOL"                                 theNodeMinPoints,"
  EOL"                                 theNodeMaxPoints,"
  EOL"                                 theNodeDataRecords,"
  EOL"                                 &aHit);"
  EOL
  EOL"      if (aTriangle.x < 0.f)"
  EOL"      {"
  EOL"        float aTime;"
  EOL
  EOL"        if (aWeight.w != 0.f || !IntersectSphere (theRay, theRadius, &aTime))"
  EOL"          break;"
  EOL
  EOL"        float2 aTexCoord = Latlong (fma (theRay->Direct, (float4) (aTime), theRay->Origin) * (1.f / theRadius));"
  EOL
  EOL"        aResult += aWeight * read_imagef (theEnvMap, EnvironmentSampler, aTexCoord);"
  EOL
  EOL"        return (float4) (aResult.x,"
  EOL"                         aResult.y,"
  EOL"                         aResult.z,"
  EOL"                         aWeight.w);"
  EOL"      }"
  EOL
  EOL"     " // Compute geometric normal
  EOL"      float4 aGeomNormal = aHit.Normal; aGeomNormal = fast_normalize (aGeomNormal);"
  EOL
  EOL"     " // Compute interpolated normal
  EOL"      float4 aNormal = SmoothNormal (theNormals, &aHit, aTriangle);"
  EOL
  EOL"     " // Compute intersection point
  EOL"      float4 aPoint = theRay->Direct * aHit.Time + theRay->Origin;"
  EOL
  EOL"      float4 aMaterAmb = theMaterials [_MAT_SIZE_ * aTriangle.w + 0];"
  EOL"      float4 aMaterTrn = theMaterials [_MAT_SIZE_ * aTriangle.w + 6];"
  EOL
  EOL"      for (int nLight = 0; nLight < theLightCount; ++nLight)"
  EOL"      {"
  EOL"        float4 aLightAmbient = theLightSources [_LGH_SIZE_ * nLight];"
  EOL
  EOL"        aResult += aWeight * aLightAmbient * aMaterAmb *"
  EOL"            (aMaterTrn.x * max (fabs (dot (theRay->Direct, aNormal)), 0.5f));"
  EOL
  EOL"        if (aLightAmbient.w < 0.f)" // 'ambient' light
  EOL"        {"
  EOL"          continue;" // 'ambient' light has no another luminances
  EOL"        }"
  EOL
  EOL"        float4 aLightPosition = theLightSources [_LGH_SIZE_ * nLight + 2];"
  EOL
  EOL"        SRay aShadow;"
  EOL"        aShadow.Direct = aLightPosition;"
  EOL
  EOL"        float aLightDistance = MAXFLOAT;"
  EOL"        if (aLightPosition.w != 0.f)"
  EOL"        {"
  EOL"          aLightDistance = length (aLightPosition - aPoint);"
  EOL"          aShadow.Direct = (aLightPosition - aPoint) * (1.f / aLightDistance);"
  EOL"        }"
  EOL
  EOL"        aShadow.Origin = aPoint + aShadow.Direct * theEpsilon +"
  EOL"                    aGeomNormal * copysign (theEpsilon, dot (aGeomNormal, aShadow.Direct));"
  EOL
  EOL"        float aFactor = 1.f;"
  EOL
  EOL"        if (isShadows)"
  EOL"        {"
  EOL"          aFactor = TraverseShadow (&aShadow,"
  EOL"                                    theIndices,"
  EOL"                                    theVertices,"
  EOL"                                    theMaterials,"
  EOL"                                    theNodeMinPoints,"
  EOL"                                    theNodeMaxPoints,"
  EOL"                                    theNodeDataRecords,"
  EOL"                                    aLightDistance);"
  EOL"        }"
  EOL
  EOL"        aResult += (aMaterTrn.x * aFactor) * aWeight * Shade (theMaterials,"
  EOL"                                                              aShadow.Direct,"
  EOL"                                                              -theRay->Direct,"
  EOL"                                                              aNormal,"
  EOL"                                                              UNIT,"
  EOL"                                                              aMaterTrn.y,"
  EOL"                                                              aTriangle.w);"
  EOL"      }"
  EOL
  EOL"      if (aMaterTrn.y > 0.f)"
  EOL"      {"
  EOL"        aWeight *= aMaterTrn.y;"
  EOL"      }"
  EOL"      else"
  EOL"      {"
  EOL"        float4 aMaterRef = theMaterials [_MAT_SIZE_ * aTriangle.w + 4];"
  EOL"        aWeight *= isReflect ? aMaterRef : ZERO;"
  EOL
  EOL"        theRay->Direct -= 2.f * dot (theRay->Direct, aNormal) * aNormal;"
  EOL
  EOL"        float aDdotN = dot (theRay->Direct, aGeomNormal);"
  EOL"        if (aDdotN < 0.f)"
  EOL"          theRay->Direct -= aDdotN * aGeomNormal;"
  EOL"      }"
  EOL
  EOL"      if (aWeight.x < 0.1f && aWeight.y < 0.1f && aWeight.z < 0.1f)"
  EOL"      {"
  EOL"        return (float4) (aResult.x,"
  EOL"                         aResult.y,"
  EOL"                         aResult.z,"
  EOL"                         aWeight.w);"
  EOL"      }"
  EOL
  EOL"      theRay->Origin = theRay->Direct * theEpsilon + aPoint;"
  EOL"    }"
  EOL
  EOL"    return (float4) (aResult.x,"
  EOL"                     aResult.y,"
  EOL"                     aResult.z,"
  EOL"                     aWeight.w);"
  EOL"  }"
  EOL
  EOL
  ///////////////////////////////////////////////////////////////////////////////
  // Ray tracing kernel functions
  EOL
  // =======================================================================
  // function : Main
  // purpose  : Computes pixel color using ray-tracing
  // =======================================================================
  EOL"  __kernel void Main (__write_only image2d_t theOutput,"
  EOL"                      __read_only  image2d_t theEnvMap,"
  EOL"                      __global float4* theNodeMinPoints,"
  EOL"                      __global float4* theNodeMaxPoints,"
  EOL"                      __global int4* theNodeDataRecords,"
  EOL"                      __global float4* theLightSources,"
  EOL"                      __global float4* theMaterials,"
  EOL"                      __global float4* theVertices,"
  EOL"                      __global float4* theNormals,"
  EOL"                      __global int4* theIndices,"
  EOL"                      const float16 theOrigins,"
  EOL"                      const float16 theDirects,"
  EOL"                      const int theLightCount,"
  EOL"                      const float theEpsilon,"
  EOL"                      const float theRadius,"
  EOL"                      const int isShadows,"
  EOL"                      const int isReflect,"
  EOL"                      const int theSizeX,"
  EOL"                      const int theSizeY)"
  EOL"  {"
  EOL"    const int aX = get_global_id (0);"
  EOL"    const int aY = get_global_id (1);"
  EOL
  EOL"    if (aX >= theSizeX || aY >= theSizeY)"
  EOL"      return;"
  EOL
  EOL"    private SRay aRay;"
  EOL
  EOL"    GenerateRay (&aRay,"
  EOL"                 aX,"
  EOL"                 aY,"
  EOL"                 theSizeX,"
  EOL"                 theSizeY,"
  EOL"                 theOrigins,"
  EOL"                 theDirects);"
  EOL
  EOL"    float4 aBoxMin = theNodeMinPoints[0] - (float4) (theEpsilon, theEpsilon, theEpsilon, 0.f);"
  EOL"    float4 aBoxMax = theNodeMaxPoints[0] + (float4) (theEpsilon, theEpsilon, theEpsilon, 0.f);"
  EOL
  EOL"    float aTimeStart;"
  EOL"    float aTimeFinal;"
  EOL
  EOL"    float4 aColor = (float4) (0.f, 0.f, 0.f, 1.f);"
  EOL
  EOL"    if (IntersectBox (&aRay, aBoxMin, aBoxMax, &aTimeStart, &aTimeFinal))"
  EOL"    {"
  EOL"      aRay.Origin = fma (aRay.Direct, (float4) (aTimeStart), aRay.Origin);"
  EOL
  EOL"      aColor = Raytrace (&aRay,"
  EOL"                         theEnvMap,"
  EOL"                         theNodeMinPoints,"
  EOL"                         theNodeMaxPoints,"
  EOL"                         theNodeDataRecords,"
  EOL"                         theLightSources,"
  EOL"                         theMaterials,"
  EOL"                         theVertices,"
  EOL"                         theNormals,"
  EOL"                         theIndices,"
  EOL"                         theLightCount,"
  EOL"                         theEpsilon,"
  EOL"                         theRadius,"
  EOL"                         isShadows,"
  EOL"                         isReflect);"
  EOL"    }"
  EOL
  EOL"    write_imagef (theOutput, (int2) (aX, aY), aColor);"
  EOL"  }"
  EOL
  EOL"  const sampler_t OutputSampler ="
  EOL"            CLK_NORMALIZED_COORDS_FALSE | CLK_ADDRESS_CLAMP | CLK_FILTER_NEAREST;"
  EOL
  EOL"  #define _LUM_DELTA_ 0.075f"
  EOL
  EOL"  #define AA_MAX 0.559017f"
  EOL"  #define AA_MIN 0.186339f"
  EOL
  // =======================================================================
  // function : MainAntialiased
  // purpose  : Performs adaptive sub-pixel rendering
  // =======================================================================
  EOL"  __kernel void MainAntialiased ( __read_only image2d_t theInput,"
  EOL"                                  __write_only image2d_t theOutput,"
  EOL"                                  __read_only  image2d_t theEnvMap,"
  EOL"                                  __global float4* theNodeMinPoints,"
  EOL"                                  __global float4* theNodeMaxPoints,"
  EOL"                                  __global int4* theNodeDataRecords,"
  EOL"                                  __global float4* theLightSources,"
  EOL"                                  __global float4* theMaterials,"
  EOL"                                  __global float4* theVertices,"
  EOL"                                  __global float4* theNormals,"
  EOL"                                  __global int4* theIndices,"
  EOL"                                  const float16 theOrigins,"
  EOL"                                  const float16 theDirects,"
  EOL"                                  const int theLightCount,"
  EOL"                                  const float theEpsilon,"
  EOL"                                  const float theRadius,"
  EOL"                                  const int isShadows,"
  EOL"                                  const int isReflect,"
  EOL"                                  const int theSizeX,"
  EOL"                                  const int theSizeY )"
  EOL"  {"
  EOL"    const int aX = get_global_id (0);"
  EOL"    const int aY = get_global_id (1);"
  EOL
  EOL"    if (aX >= theSizeX || aY >= theSizeY)"
  EOL"      return;"
  EOL
  EOL"    float4 aClr0 = read_imagef (theInput, OutputSampler, (float2) (aX + 0, aY + 0));"
  EOL"    float4 aClr1 = read_imagef (theInput, OutputSampler, (float2) (aX + 0, aY - 1));"
  EOL"    float4 aClr2 = read_imagef (theInput, OutputSampler, (float2) (aX + 0, aY + 1));"
  EOL
  EOL"    float4 aClr3 = read_imagef (theInput, OutputSampler, (float2) (aX + 1, aY + 0));"
  EOL"    float4 aClr4 = read_imagef (theInput, OutputSampler, (float2) (aX + 1, aY - 1));"
  EOL"    float4 aClr5 = read_imagef (theInput, OutputSampler, (float2) (aX + 1, aY + 1));"
  EOL
  EOL"    float4 aClr6 = read_imagef (theInput, OutputSampler, (float2) (aX - 1, aY + 0));"
  EOL"    float4 aClr7 = read_imagef (theInput, OutputSampler, (float2) (aX - 1, aY - 1));"
  EOL"    float4 aClr8 = read_imagef (theInput, OutputSampler, (float2) (aX - 1, aY + 1));"
  EOL
  EOL"    aClr1 = (aClr1.w == 1.f) ? -UNIT : aClr1;"
  EOL"    aClr2 = (aClr2.w == 1.f) ? -UNIT : aClr2;"
  EOL"    aClr3 = (aClr3.w == 1.f) ? -UNIT : aClr3;"
  EOL"    aClr4 = (aClr4.w == 1.f) ? -UNIT : aClr4;"
  EOL"    aClr5 = (aClr5.w == 1.f) ? -UNIT : aClr5;"
  EOL"    aClr6 = (aClr6.w == 1.f) ? -UNIT : aClr6;"
  EOL"    aClr7 = (aClr7.w == 1.f) ? -UNIT : aClr7;"
  EOL"    aClr8 = (aClr8.w == 1.f) ? -UNIT : aClr8;"
  EOL
  EOL"    float aLum = (aClr0.w == 1.f) ? -1.f : (0.2126f * aClr0.x + 0.7152f * aClr0.y + 0.0722f * aClr0.z);"
  EOL
  EOL
  EOL"    bool render = fabs (0.2126f * aClr1.x + 0.7152f * aClr1.y + 0.0722f * aClr1.z - aLum) > _LUM_DELTA_ ||"
  EOL"                  fabs (0.2126f * aClr2.x + 0.7152f * aClr2.y + 0.0722f * aClr2.z - aLum) > _LUM_DELTA_ ||"
  EOL"                  fabs (0.2126f * aClr3.x + 0.7152f * aClr3.y + 0.0722f * aClr3.z - aLum) > _LUM_DELTA_ ||"
  EOL"                  fabs (0.2126f * aClr4.x + 0.7152f * aClr4.y + 0.0722f * aClr4.z - aLum) > _LUM_DELTA_ ||"
  EOL"                  fabs (0.2126f * aClr5.x + 0.7152f * aClr5.y + 0.0722f * aClr5.z - aLum) > _LUM_DELTA_ ||"
  EOL"                  fabs (0.2126f * aClr6.x + 0.7152f * aClr6.y + 0.0722f * aClr6.z - aLum) > _LUM_DELTA_ ||"
  EOL"                  fabs (0.2126f * aClr7.x + 0.7152f * aClr7.y + 0.0722f * aClr7.z - aLum) > _LUM_DELTA_ ||"
  EOL"                  fabs (0.2126f * aClr8.x + 0.7152f * aClr8.y + 0.0722f * aClr8.z - aLum) > _LUM_DELTA_;"
  EOL
  EOL"    float4 aColor = aClr0;"
  EOL
  EOL"    private SRay aRay;"
  EOL
  EOL"    const float4 aBoxMin = theNodeMinPoints[0] - (float4) (theEpsilon, theEpsilon, theEpsilon, 0.f);"
  EOL"    const float4 aBoxMax = theNodeMaxPoints[0] + (float4) (theEpsilon, theEpsilon, theEpsilon, 0.f);"
  EOL
  EOL"    if (render)"
  EOL"    {"
  EOL"      for (int aSample = 0; aSample <= 3; ++aSample)"
  EOL"      {"
  EOL"          float fX = aX, fY = aY;"
  EOL
  EOL"          if (aSample == 0)"
  EOL"          {"
  EOL"            fX -= AA_MIN; fY -= AA_MAX;"
  EOL"          }"
  EOL"          else if (aSample == 1)"
  EOL"          {"
  EOL"            fX -= AA_MAX; fY += AA_MIN;"
  EOL"          }"
  EOL"          else if (aSample == 2)"
  EOL"          {"
  EOL"            fX += AA_MIN; fY += AA_MAX;"
  EOL"          }"
  EOL"          else"
  EOL"          {"
  EOL"            fX += AA_MAX; fY -= AA_MIN;"
  EOL"          }"
  EOL
  EOL"          GenerateRay (&aRay,"
  EOL"                       fX,"
  EOL"                       fY,"
  EOL"                       theSizeX,"
  EOL"                       theSizeY,"
  EOL"                       theOrigins,"
  EOL"                       theDirects);"
  EOL
  EOL"          float aTimeStart;"
  EOL"          float aTimeFinal;"
  EOL
  EOL"          if (IntersectBox (&aRay, aBoxMin, aBoxMax, &aTimeStart, &aTimeFinal))"
  EOL"          {"
  EOL"            aRay.Origin = fma (aRay.Direct, (float4) (aTimeStart), aRay.Origin);"
  EOL
  EOL"            aColor += Raytrace (&aRay,"
  EOL"                                theEnvMap,"
  EOL"                                theNodeMinPoints,"
  EOL"                                theNodeMaxPoints,"
  EOL"                                theNodeDataRecords,"
  EOL"                                theLightSources,"
  EOL"                                theMaterials,"
  EOL"                                theVertices,"
  EOL"                                theNormals,"
  EOL"                                theIndices,"
  EOL"                                theLightCount,"
  EOL"                                theEpsilon,"
  EOL"                                theRadius,"
  EOL"                                isShadows,"
  EOL"                                isReflect);"
  EOL"          }"
  EOL"          else"
  EOL"            aColor += (float4) (0.f, 0.f, 0.f, 1.f);"
  EOL"        }"
  EOL
  EOL"        aColor *= 1.f / 5.f;"
  EOL"    }"
  EOL
  EOL"    write_imagef (theOutput, (int2) (aX, aY), aColor);"
  EOL"  }";

#endif
