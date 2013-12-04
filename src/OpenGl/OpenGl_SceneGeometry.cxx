// Created on: 2013-08-27
// Created by: Denis BOGOLEPOV
// Copyright (c) 2013 OPEN CASCADE SAS
//
// This file is part of Open CASCADE Technology software library.
//
// This library is free software; you can redistribute it and / or modify it
// under the terms of the GNU Lesser General Public version 2.1 as published
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

#include <limits>

#include <OpenGl_SceneGeometry.hxx>

namespace
{

  //! Number of node bins per axis.
  static const int THE_NUMBER_OF_BINS = 32;

  //! Max number of triangles per leaf node.
  static const int THE_MAX_LEAF_TRIANGLES = 4;

  //! Useful constant for null integer 4D vector.
  static const OpenGl_RTVec4i THE_ZERO_VEC_4I;

  //! Useful constant for null floating-point 4D vector.
  static const OpenGl_RTVec4f THE_ZERO_VEC_4F;

};

// =======================================================================
// function : OpenGl_RaytraceMaterial
// purpose  : Creates new default material
// =======================================================================
OpenGl_RaytraceMaterial::OpenGl_RaytraceMaterial()
: Ambient      (THE_ZERO_VEC_4F),
  Diffuse      (THE_ZERO_VEC_4F),
  Specular     (THE_ZERO_VEC_4F),
  Emission     (THE_ZERO_VEC_4F),
  Reflection   (THE_ZERO_VEC_4F),
  Refraction   (THE_ZERO_VEC_4F),
  Transparency (THE_ZERO_VEC_4F)
{ }

// =======================================================================
// function : OpenGl_RaytraceMaterial
// purpose  : Creates new material with specified properties
// =======================================================================
OpenGl_RaytraceMaterial::OpenGl_RaytraceMaterial (const OpenGl_RTVec4f& theAmbient,
                                                  const OpenGl_RTVec4f& theDiffuse,
                                                  const OpenGl_RTVec4f& theSpecular)
: Ambient      (theAmbient),
  Diffuse      (theDiffuse),
  Specular     (theSpecular),
  Emission     (THE_ZERO_VEC_4F),
  Reflection   (THE_ZERO_VEC_4F),
  Refraction   (THE_ZERO_VEC_4F),
  Transparency (THE_ZERO_VEC_4F)
{
  //
}

// =======================================================================
// function : OpenGl_RaytraceMaterial
// purpose  : Creates new material with specified properties
// =======================================================================
OpenGl_RaytraceMaterial::OpenGl_RaytraceMaterial (const OpenGl_RTVec4f& theAmbient,
                                                  const OpenGl_RTVec4f& theDiffuse,
                                                  const OpenGl_RTVec4f& theSpecular,
                                                  const OpenGl_RTVec4f& theEmission,
                                                  const OpenGl_RTVec4f& theTranspar)
: Ambient      (theAmbient),
  Diffuse      (theDiffuse),
  Specular     (theSpecular),
  Emission     (theEmission),
  Reflection   (THE_ZERO_VEC_4F),
  Refraction   (THE_ZERO_VEC_4F),
  Transparency (theTranspar)
{
  //
}

// =======================================================================
// function : OpenGl_RaytraceMaterial
// purpose  : Creates new material with specified properties
// =======================================================================
OpenGl_RaytraceMaterial::OpenGl_RaytraceMaterial (const OpenGl_RTVec4f& theAmbient,
                                                  const OpenGl_RTVec4f& theDiffuse,
                                                  const OpenGl_RTVec4f& theSpecular,
                                                  const OpenGl_RTVec4f& theEmission,
                                                  const OpenGl_RTVec4f& theTranspar,
                                                  const OpenGl_RTVec4f& theReflection,
                                                  const OpenGl_RTVec4f& theRefraction)
: Ambient      (theAmbient),
  Diffuse      (theDiffuse),
  Specular     (theSpecular),
  Emission     (theEmission),
  Reflection   (theReflection),
  Refraction   (theRefraction),
  Transparency (theTranspar)
{
  //
}

// =======================================================================
// function : OpenGl_LightSource
// purpose  : Creates new light source
// =======================================================================
OpenGl_RaytraceLight::OpenGl_RaytraceLight (const OpenGl_RTVec4f& theAmbient)
: Ambient (theAmbient)
{
  //
}

// =======================================================================
// function : OpenGl_LightSource
// purpose  : Creates new light source
// =======================================================================
OpenGl_RaytraceLight::OpenGl_RaytraceLight (const OpenGl_RTVec4f& theDiffuse,
                                            const OpenGl_RTVec4f& thePosition)
: Diffuse (theDiffuse),
  Position (thePosition)
{
  //
}

// =======================================================================
// function : Center
// purpose  : Returns centroid of specified triangle
// =======================================================================
OpenGl_RTVec4f OpenGl_RaytraceScene::Center (const int theTriangle) const
{
  const OpenGl_RTVec4i& anIndex = Triangles [theTriangle];

  return ( Vertices[anIndex.x()] +
           Vertices[anIndex.y()] +
           Vertices[anIndex.z()] ) * ( 1.f / 3.f );
}

// =======================================================================
// function : CenterAxis
// purpose  : Returns centroid of specified triangle
// =======================================================================
float OpenGl_RaytraceScene::CenterAxis (const int theTriangle,
                                        const int theAxis) const
{
  const OpenGl_RTVec4i& anIndex = Triangles [theTriangle];

  return ( Vertices[anIndex.x()][theAxis] +
           Vertices[anIndex.y()][theAxis] +
           Vertices[anIndex.z()][theAxis] ) * ( 1.f / 3.f );
}

// =======================================================================
// function : Box
// purpose  : Returns AABB of specified triangle
// =======================================================================
OpenGl_AABB OpenGl_RaytraceScene::Box (const int theTriangle) const
{
  const OpenGl_RTVec4i& anIndex = Triangles[theTriangle];

  const OpenGl_RTVec4f& pA = Vertices[anIndex.x()];
  const OpenGl_RTVec4f& pB = Vertices[anIndex.y()];
  const OpenGl_RTVec4f& pC = Vertices[anIndex.z()];

  OpenGl_AABB aBox (pA);

  aBox.Add (pB);
  aBox.Add (pC);

  return aBox;
}

// =======================================================================
// function : Clear
// purpose  : Clears all scene geometry data
// =======================================================================
void OpenGl_RaytraceScene::Clear()
{
  AABB.Clear();

  OpenGl_RTArray4f anEmptyNormals;
  Normals.swap (anEmptyNormals);

  OpenGl_RTArray4f anEmptyVertices;
  Vertices.swap (anEmptyVertices);

  OpenGl_RTArray4i anEmptyTriangles;
  Triangles.swap (anEmptyTriangles);

  std::vector<OpenGl_RaytraceMaterial,
              NCollection_StdAllocator<OpenGl_RaytraceMaterial> > anEmptyMaterials;

  Materials.swap (anEmptyMaterials);
}

// =======================================================================
// function : OpenGl_Node
// purpose  : Creates new empty BVH node
// =======================================================================
OpenGl_BVHNode::OpenGl_BVHNode()
: myMinPoint (THE_ZERO_VEC_4F),
  myMaxPoint (THE_ZERO_VEC_4F),
  myDataRcrd (THE_ZERO_VEC_4I)
{
  //
}

// =======================================================================
// function : OpenGl_Node
// purpose  : Creates new BVH node with specified data
// =======================================================================
OpenGl_BVHNode::OpenGl_BVHNode (const OpenGl_RTVec4f& theMinPoint,
                                const OpenGl_RTVec4f& theMaxPoint,
                                const OpenGl_RTVec4i& theDataRcrd)
: myMinPoint (theMinPoint),
  myMaxPoint (theMaxPoint),
  myDataRcrd (theDataRcrd)
{
  //
}

// =======================================================================
// function : OpenGl_Node
// purpose  : Creates new leaf BVH node with specified data
// =======================================================================
OpenGl_BVHNode::OpenGl_BVHNode (const OpenGl_AABB& theAABB,
                                const int          theBegTriangle,
                                const int          theEndTriangle)
: myMinPoint (theAABB.CornerMin()),
  myMaxPoint (theAABB.CornerMax()),
  myDataRcrd (1,
              theBegTriangle,
              theEndTriangle,
              0)
{
  //
}

// =======================================================================
// function : OpenGl_Node
// purpose  : Creates new leaf BVH node with specified data
// =======================================================================
OpenGl_BVHNode::OpenGl_BVHNode (const OpenGl_RTVec4f& theMinPoint,
                                const OpenGl_RTVec4f& theMaxPoint,
                                const int             theBegTriangle,
                                const int             theEndTriangle)
: myMinPoint (theMinPoint),
  myMaxPoint (theMaxPoint),
  myDataRcrd (1,
              theBegTriangle,
              theEndTriangle,
              0)
{
  //
}

// =======================================================================
// function : CleanUp
// purpose  : Removes all tree nodes
// =======================================================================
void OpenGl_BVH::CleanUp()
{
  OpenGl_RTArray4f anEmptyMinPointBuffer;
  myMinPointBuffer.swap (anEmptyMinPointBuffer);

  OpenGl_RTArray4f anEmptyMaxPointBuffer;
  myMaxPointBuffer.swap (anEmptyMaxPointBuffer);

  OpenGl_RTArray4i anEmptyDataRcrdBuffer;
  myDataRcrdBuffer.swap (anEmptyDataRcrdBuffer);
}

// =======================================================================
// function : Node
// purpose  : Returns node with specified index
// =======================================================================
OpenGl_BVHNode OpenGl_BVH::Node (const int theIndex) const
{
  return OpenGl_BVHNode (myMinPointBuffer[theIndex],
                         myMaxPointBuffer[theIndex],
                         myDataRcrdBuffer[theIndex]);
}

// =======================================================================
// function : SetNode
// purpose  : Replaces node with specified index
// =======================================================================
void OpenGl_BVH::SetNode (const int             theIndex,
                          const OpenGl_BVHNode& theNode)
{
  if (theIndex < static_cast<int> (myMinPointBuffer.size()))
  {
    myMinPointBuffer[theIndex] = theNode.myMinPoint;
    myMaxPointBuffer[theIndex] = theNode.myMaxPoint;
    myDataRcrdBuffer[theIndex] = theNode.myDataRcrd;
  }
}

// =======================================================================
// function : PushNode
// purpose  : Adds new node to the tree
// =======================================================================
int OpenGl_BVH::PushNode (const OpenGl_BVHNode& theNode)
{
  myMinPointBuffer.push_back (theNode.myMinPoint);
  myMaxPointBuffer.push_back (theNode.myMaxPoint);
  myDataRcrdBuffer.push_back (theNode.myDataRcrd);
  return static_cast<int> (myDataRcrdBuffer.size() - 1);
}

// =======================================================================
// function : OpenGl_NodeBuildTask
// purpose  : Creates new node building task
// =======================================================================
OpenGl_BVHNodeTask::OpenGl_BVHNodeTask()
: NodeToBuild (0),
  BegTriangle (0),
  EndTriangle (0)
{
  //
}

// =======================================================================
// function : OpenGl_NodeBuildTask
// purpose  : Creates new node building task
// =======================================================================
OpenGl_BVHNodeTask::OpenGl_BVHNodeTask (const int theNodeToBuild,
                                        const int theBegTriangle,
                                        const int theEndTriangle)
: NodeToBuild (theNodeToBuild),
  BegTriangle (theBegTriangle),
  EndTriangle (theEndTriangle)
{
  //
}

// =======================================================================
// function : OpenGl_BinnedBVHBuilder
// purpose  : Creates new binned BVH builder
// =======================================================================
OpenGl_BinnedBVHBuilder::OpenGl_BinnedBVHBuilder()
: myMaxDepth (30)
{
  //
}

// =======================================================================
// function : ~OpenGl_BinnedBVHBuilder
// purpose  : Releases binned BVH builder
// =======================================================================
OpenGl_BinnedBVHBuilder::~OpenGl_BinnedBVHBuilder()
{
  //
}

#define BVH_DEBUG_OUTPUT_

#if defined( BVH_DEBUG_OUTPUT )
  #include <iostream>
#endif

// =======================================================================
// function : ReinterpretIntAsFloat
// purpose  : Reinterprets bits of integer value as floating-point value
// =======================================================================
inline float ReinterpretIntAsFloat (int theValue)
{
  return *reinterpret_cast< float* > (&theValue);
}

// =======================================================================
// function : Build
// purpose  : Builds BVH tree using binned SAH algorithm
// =======================================================================
void OpenGl_BinnedBVHBuilder::Build (OpenGl_RaytraceScene& theGeometry,
                                     const float           theEpsilon)
{
  CleanUp();

#ifdef BVH_DEBUG_OUTPUT
  std::cout << "Start building BVH..." << std::endl;

  std::cout << "Triangles: " << theGeometry.Triangles.size() << std::endl;
#endif

  if (theGeometry.Triangles.size() == 0)
    return;

  // Create root node with all scene triangles
  OpenGl_AABB anAABB = theGeometry.AABB;
  anAABB.CornerMin() = OpenGl_RTVec4f (anAABB.CornerMin().x() - theEpsilon,
                                       anAABB.CornerMin().y() - theEpsilon,
                                       anAABB.CornerMin().z() - theEpsilon,
                                       1.0f);
  anAABB.CornerMax() = OpenGl_RTVec4f (anAABB.CornerMax().x() + theEpsilon,
                                       anAABB.CornerMax().y() + theEpsilon,
                                       anAABB.CornerMax().z() + theEpsilon,
                                       1.0f);
  myTree.PushNode (OpenGl_BVHNode (anAABB, 0, static_cast<int> (theGeometry.Triangles.size() - 1)));

#ifdef BVH_DEBUG_OUTPUT
  std::cout << "Push root node: [" << 0 << ", " <<
                      theGeometry.Triangles.size() - 1 << "]" << std::endl;
#endif

  // Setup splitting task for the root node
  myNodeTasksQueue.push_back (OpenGl_BVHNodeTask (0, 0, static_cast<int> (theGeometry.Triangles.size() - 1)));

  // Building nodes while tasks queue is not empty
  for (int aTaskId = 0; aTaskId < static_cast<int> (myNodeTasksQueue.size()); ++aTaskId)
  {
    BuildNode (theGeometry, aTaskId);
  }

  // Write support data to optimize traverse
  for (int aNode = 0; aNode < static_cast<int> (myTree.DataRcrdBuffer().size()); ++aNode)
  {
    OpenGl_RTVec4i aData = myTree.DataRcrdBuffer()[aNode];
    myTree.MinPointBuffer()[aNode].w() = ReinterpretIntAsFloat (aData[0] ? aData[1] : -aData[1]);
    myTree.MaxPointBuffer()[aNode].w() = ReinterpretIntAsFloat (aData[0] ? aData[2] : -aData[2]);
  }
}

// =======================================================================
// function : CleanUp
// purpose  : Clears previously built tree
// =======================================================================
void OpenGl_BinnedBVHBuilder::CleanUp()
{
  myTree.CleanUp();
  myNodeTasksQueue.clear();
}

// =======================================================================
// function : SetMaxDepth
// purpose  : Sets maximum tree depth
// =======================================================================
void OpenGl_BinnedBVHBuilder::SetMaxDepth (const int theMaxDepth)
{
  if (theMaxDepth > 1 && theMaxDepth < 30)
  {
    myMaxDepth = theMaxDepth - 1;
  }
}

//! Minimum node size to split.
static const float THE_NODE_MIN_SIZE = 1e-4f;

// =======================================================================
// function : BuildNode
// purpose  : Builds node using task info
// =======================================================================
void OpenGl_BinnedBVHBuilder::BuildNode (OpenGl_RaytraceScene& theGeometry,
                                         const int             theTask)
{
  OpenGl_BVHNodeTask aTask = myNodeTasksQueue[theTask];
  OpenGl_BVHNode     aNode = myTree.Node (aTask.NodeToBuild);

#ifdef BVH_DEBUG_OUTPUT
  std::cout << "Build node " << aTask.NodeToBuild << ": [" <<
                  aTask.BegTriangle << ", " << aTask.EndTriangle << "]" << std::endl;
#endif

  OpenGl_AABB anAABB (aNode.MinPoint(), aNode.MaxPoint());
  const OpenGl_RTVec4f aNodeSize = anAABB.Size();
  const float aNodeArea = anAABB.Area();

  // Parameters for storing best split
  float aMinSplitCost = std::numeric_limits<float>::max();

  int aMinSplitAxis     = -1;
  int aMinSplitIndex    =  0;
  int aMinSplitLftCount =  0;
  int aMinSplitRghCount =  0;

  OpenGl_AABB aMinSplitLftAABB;
  OpenGl_AABB aMinSplitRghAABB;

  // Find best split
  for (int anAxis = 0; anAxis < 3; ++anAxis)
  {
    if (aNodeSize[anAxis] <= THE_NODE_MIN_SIZE)
      continue;

    OpenGl_BinVector aBins (THE_NUMBER_OF_BINS);
    GetSubVolumes (theGeometry, aNode, aBins, anAxis);

    // Choose the best split (with minimum SAH cost)
    for (int aSplit = 1; aSplit < THE_NUMBER_OF_BINS; ++aSplit)
    {
      int aLftCount = 0;
      int aRghCount = 0;
      OpenGl_AABB aLftAABB;
      OpenGl_AABB aRghAABB;
      for (int anIndex = 0; anIndex < aSplit; ++anIndex)
      {
        aLftCount += aBins[anIndex].Count;
        aLftAABB.Combine (aBins[anIndex].Volume);
      }

      for (int anIndex = aSplit; anIndex < THE_NUMBER_OF_BINS; ++anIndex)
      {
        aRghCount += aBins[anIndex].Count;
        aRghAABB.Combine (aBins[anIndex].Volume);
      }

      // Simple SAH evaluation
      float aCost = ( aLftAABB.Area() / aNodeArea ) * aLftCount +
                    ( aRghAABB.Area() / aNodeArea ) * aRghCount;

#ifdef BVH_DEBUG_OUTPUT
      std::cout << "\t\tBin " << aSplit << ", Cost = " << aCost << std::endl;
#endif

      if (aCost <= aMinSplitCost)
      {
        aMinSplitCost     = aCost;
        aMinSplitAxis     = anAxis;
        aMinSplitIndex    = aSplit;
        aMinSplitLftAABB  = aLftAABB;
        aMinSplitRghAABB  = aRghAABB;
        aMinSplitLftCount = aLftCount;
        aMinSplitRghCount = aRghCount;
      }
    }
  }

  if (aMinSplitAxis == -1)
  {
    // make outer (leaf) node
    myTree.DataRcrdBuffer()[aTask.NodeToBuild].x() = 1;
    return;
  }

#ifdef BVH_DEBUG_OUTPUT
  switch (aMinSplitAxis)
  {
  case 0:
    std::cout << "\tSplit axis: X = " << aMinSplitIndex << std::endl;
    break;
  case 1:
    std::cout << "\tSplit axis: Y = " << aMinSplitIndex << std::endl;
    break;
  case 2:
    std::cout << "\tSplit axis: Z = " << aMinSplitIndex << std::endl;
    break;
  }
#endif

  int aMiddle = SplitTriangles (theGeometry, aTask.BegTriangle, aTask.EndTriangle,
                                aNode, aMinSplitIndex - 1, aMinSplitAxis);

#ifdef BVH_DEBUG_OUTPUT
  std::cout << "\tLeft child: [" << aTask.BegTriangle << ", "
                      << aMiddle - 1 << "]" << std::endl;

  std::cout << "\tRight child: [" << aMiddle << ", "
                      << aTask.EndTriangle << "]" << std::endl;
#endif

#define BVH_SIDE_LFT 1
#define BVH_SIDE_RGH 2

  // Setting up tasks for child nodes
  for (int aSide = BVH_SIDE_LFT; aSide <= BVH_SIDE_RGH; ++aSide)
  {
    OpenGl_RTVec4f aMinPoint = (aSide == BVH_SIDE_LFT)
                             ? aMinSplitLftAABB.CornerMin()
                             : aMinSplitRghAABB.CornerMin();
    OpenGl_RTVec4f aMaxPoint = (aSide == BVH_SIDE_LFT)
                             ? aMinSplitLftAABB.CornerMax()
                             : aMinSplitRghAABB.CornerMax();

    int aBegTriangle = (aSide == BVH_SIDE_LFT)
                     ? aTask.BegTriangle
                     : aMiddle;
    int aEndTriangle = (aSide == BVH_SIDE_LFT)
                     ? aMiddle - 1
                     : aTask.EndTriangle;

    OpenGl_BVHNode aChild (aMinPoint, aMaxPoint, aBegTriangle, aEndTriangle);
    aChild.SetLevel (aNode.Level() + 1);

    // Check to see if child node must be split
    const int aNbTriangles = (aSide == BVH_SIDE_LFT)
                           ? aMinSplitLftCount
                           : aMinSplitRghCount;

    const int isChildALeaf = (aNbTriangles <= THE_MAX_LEAF_TRIANGLES) || (aNode.Level() >= myMaxDepth);
    if (isChildALeaf)
      aChild.SetOuter();
    else
      aChild.SetInner();

    const int aChildIndex = myTree.PushNode (aChild);

    // Modify parent node
    myTree.DataRcrdBuffer()[aTask.NodeToBuild].x() = 0; // inner node flag
    if (aSide == BVH_SIDE_LFT)
      myTree.DataRcrdBuffer()[aTask.NodeToBuild].y() = aChildIndex; // left child
    else
      myTree.DataRcrdBuffer()[aTask.NodeToBuild].z() = aChildIndex; // right child

    // Make new building task
    if (!isChildALeaf)
      myNodeTasksQueue.push_back (OpenGl_BVHNodeTask (aChildIndex, aBegTriangle, aEndTriangle));
  }
}

// =======================================================================
// function : SplitTriangles
// purpose  : Splits node triangles into two intervals for child nodes
// =======================================================================
int OpenGl_BinnedBVHBuilder::SplitTriangles (OpenGl_RaytraceScene& theGeometry,
                                             const int             theBegTriangle,
                                             const int             theEndTriangle,
                                             OpenGl_BVHNode&       theNode,
                                             int                   theBin,
                                             const int             theAxis)
{
  int aLftIndex (theBegTriangle);
  int aRghIndex (theEndTriangle);

  const float aMin = theNode.MinPoint()[theAxis];
  const float aMax = theNode.MaxPoint()[theAxis];

  const float aStep = (aMax - aMin) / THE_NUMBER_OF_BINS;

  do
  {
    while ((int )floorf ((theGeometry.CenterAxis (aLftIndex, theAxis) - aMin) / aStep) <= theBin
              && aLftIndex < theEndTriangle)
    {
      ++aLftIndex;
    }
    while ((int )floorf ((theGeometry.CenterAxis (aRghIndex, theAxis) - aMin) / aStep) >  theBin
              && aRghIndex > theBegTriangle)
    {
      --aRghIndex;
    }

    if (aLftIndex <= aRghIndex)
    {
      if (aLftIndex != aRghIndex)
      {
        OpenGl_RTVec4i aLftTrg = theGeometry.Triangles[aLftIndex];
        OpenGl_RTVec4i aRghTrg = theGeometry.Triangles[aRghIndex];
        theGeometry.Triangles[aLftIndex] = aRghTrg;
        theGeometry.Triangles[aRghIndex] = aLftTrg;
      }

      aLftIndex++; aRghIndex--;
    }
  } while (aLftIndex <= aRghIndex);

  return aLftIndex;
}

// =======================================================================
// function : GetSubVolumes
// purpose  : Arranges node triangles into bins
// =======================================================================
void OpenGl_BinnedBVHBuilder::GetSubVolumes (OpenGl_RaytraceScene& theGeometry,
                                             const OpenGl_BVHNode& theNode,
                                             OpenGl_BinVector&     theBins,
                                             const int             theAxis)
{
  const float aMin = theNode.MinPoint()[theAxis];
  const float aMax = theNode.MaxPoint()[theAxis];

  const float aStep = (aMax - aMin) / THE_NUMBER_OF_BINS;

  for (int aTri = theNode.BegTriangle(); aTri <= theNode.EndTriangle(); ++aTri)
  {
    float aCenter = theGeometry.CenterAxis (aTri, theAxis);
    int aBinIndex = (int )floorf ((aCenter - aMin) * ( 1.0f / aStep));
    if (aBinIndex < 0)
    {
      aBinIndex = 0;
    }
    else if (aBinIndex >= THE_NUMBER_OF_BINS)
    {
      aBinIndex = THE_NUMBER_OF_BINS - 1;
    }

    theBins[aBinIndex].Count++;
    theBins[aBinIndex].Volume.Combine (theGeometry.Box (aTri));
  }
}

namespace OpenGl_Raytrace
{
  // =======================================================================
  // function : IsRaytracedElement
  // purpose  : Checks to see if the element contains ray-trace geometry
  // =======================================================================
  Standard_Boolean IsRaytracedElement (const OpenGl_ElementNode* theNode)
  {
    if (TelParray == theNode->type)
    {
      OpenGl_PrimitiveArray* anArray = dynamic_cast< OpenGl_PrimitiveArray* > (theNode->elem);
      return anArray->PArray()->type >= TelPolygonsArrayType;
    }
    return Standard_False;
  }

  // =======================================================================
  // function : IsRaytracedGroup
  // purpose  : Checks to see if the group contains ray-trace geometry
  // =======================================================================
  Standard_Boolean IsRaytracedGroup (const OpenGl_Group *theGroup)
  {
    const OpenGl_ElementNode* aNode;
    for (aNode = theGroup->FirstNode(); aNode != NULL; aNode = aNode->next)
    {
      if (IsRaytracedElement (aNode))
      {
        return Standard_True;
      }
    }
    return Standard_False;
  }

  // =======================================================================
  // function : IsRaytracedStructure
  // purpose  : Checks to see if the structure contains ray-trace geometry
  // =======================================================================
  Standard_Boolean IsRaytracedStructure (const OpenGl_Structure *theStructure)
  {
    for (OpenGl_ListOfGroup::Iterator anItg (theStructure->Groups());
         anItg.More(); anItg.Next())
    {
      if (anItg.Value()->IsRaytracable())
        return Standard_True;
    }
    for (OpenGl_ListOfStructure::Iterator anIts (theStructure->ConnectedStructures());
         anIts.More(); anIts.Next())
    {
      if (IsRaytracedStructure (anIts.Value()))
        return Standard_True;
    }
    return Standard_False;
  }
}

#endif
