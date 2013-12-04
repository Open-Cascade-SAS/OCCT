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

#ifndef _OpenGl_SceneGeometry_Header
#define _OpenGl_SceneGeometry_Header

#ifdef HAVE_OPENCL

#include <OpenGl_AABB.hxx>
#include <OpenGl_Structure.hxx>
#include <OpenGl_PrimitiveArray.hxx>

namespace OpenGl_Raytrace
{
  //! Checks to see if the group contains ray-trace geometry.
  Standard_Boolean IsRaytracedGroup (const OpenGl_Group* theGroup);

  //! Checks to see if the element contains ray-trace geometry.
  Standard_Boolean IsRaytracedElement (const OpenGl_ElementNode* theNode);

  //! Checks to see if the structure contains ray-trace geometry.
  Standard_Boolean IsRaytracedStructure (const OpenGl_Structure* theStructure);
}

//! Stores properties of surface material.
class OpenGl_RaytraceMaterial
{
public:

  //! Ambient reflection coefficient.
  OpenGl_RTVec4f Ambient;

  //! Diffuse reflection coefficient.
  OpenGl_RTVec4f Diffuse;

  //! Glossy reflection coefficient.
  OpenGl_RTVec4f Specular;

  //! Material emission.
  OpenGl_RTVec4f Emission;

  //! Specular reflection coefficient.
  OpenGl_RTVec4f Reflection;

  //! Specular refraction coefficient.
  OpenGl_RTVec4f Refraction;

  //! Material transparency.
  OpenGl_RTVec4f Transparency;

public:

  //! Creates new default material.
  OpenGl_RaytraceMaterial();

  //! Creates new material with specified properties.
  OpenGl_RaytraceMaterial (const OpenGl_RTVec4f& theAmbient,
                           const OpenGl_RTVec4f& theDiffuse,
                           const OpenGl_RTVec4f& theSpecular);

  //! Creates new material with specified properties.
  OpenGl_RaytraceMaterial (const OpenGl_RTVec4f& theAmbient,
                           const OpenGl_RTVec4f& theDiffuse,
                           const OpenGl_RTVec4f& theSpecular,
                           const OpenGl_RTVec4f& theEmission,
                           const OpenGl_RTVec4f& theTranspar);

  //! Creates new material with specified properties.
  OpenGl_RaytraceMaterial (const OpenGl_RTVec4f& theAmbient,
                           const OpenGl_RTVec4f& theDiffuse,
                           const OpenGl_RTVec4f& theSpecular,
                           const OpenGl_RTVec4f& theEmission,
                           const OpenGl_RTVec4f& theTranspar,
                           const OpenGl_RTVec4f& theReflection,
                           const OpenGl_RTVec4f& theRefraction);

  //! Returns packed (serialized) representation of material.
  const float* Packed() { return reinterpret_cast<float*> (this); }
};

//! Stores properties of OpenGL light source.
class OpenGl_RaytraceLight
{
public:

  //! 'Ambient' intensity.
  OpenGl_RTVec4f Ambient;

  //! 'Diffuse' intensity.
  OpenGl_RTVec4f Diffuse;

  //! Position of light source (in terms of OpenGL).
  OpenGl_RTVec4f Position;


public:

  //! Creates new light source.
  OpenGl_RaytraceLight (const OpenGl_RTVec4f& theAmbient);

  //! Creates new light source.
  OpenGl_RaytraceLight (const OpenGl_RTVec4f& theDiffuse,
                        const OpenGl_RTVec4f& thePosition);

  //! Returns packed (serialized) representation of light source.
  const float* Packed() { return reinterpret_cast<float*> (this); }
};

//! Stores scene geometry data.
struct OpenGl_RaytraceScene
{
  //! AABB of 3D scene.
  OpenGl_AABB AABB;

  //! Array of vertex normals.
  OpenGl_RTArray4f Normals;

  //! Array of vertex coordinates.
  OpenGl_RTArray4f Vertices;

  //! Array of scene triangles.
  OpenGl_RTArray4i Triangles;

  //! Array of 'front' material properties.
  std::vector<OpenGl_RaytraceMaterial,
              NCollection_StdAllocator<OpenGl_RaytraceMaterial> > Materials;

  //! Array of properties of light sources.
  std::vector<OpenGl_RaytraceLight,
              NCollection_StdAllocator<OpenGl_RaytraceLight> > LightSources;

  //! Clears all scene geometry and material data.
  void Clear();

  //! Returns AABB of specified triangle.
  OpenGl_AABB Box (const int theTriangle) const;

  //! Returns centroid of specified triangle.
  OpenGl_RTVec4f Center (const int theTriangle) const;

  //! Returns centroid coordinate for specified axis.
  float CenterAxis (const int theTriangle, const int theAxis) const;
};

//! Stores parameters of BVH tree node.
class OpenGl_BVHNode
{
  friend class OpenGl_BVH;

public:

  //! Creates new empty BVH node.
  OpenGl_BVHNode();

  //! Creates new BVH node with specified data.
  OpenGl_BVHNode (const OpenGl_RTVec4f& theMinPoint,
                  const OpenGl_RTVec4f& theMaxPoint,
                  const OpenGl_RTVec4i& theDataRcrd);

  //! Creates new leaf BVH node with specified data.
  OpenGl_BVHNode (const OpenGl_RTVec4f& theMinPoint,
                  const OpenGl_RTVec4f& theMaxPoint,
                  const int theBegTriangle,
                  const int theEndTriangle);

  //! Creates new leaf BVH node with specified data.
  OpenGl_BVHNode (const OpenGl_AABB& theAABB,
                  const int theBegTriangle,
                  const int theEndTriangle);

  //! Returns minimum point of node's AABB.
  OpenGl_RTVec4f& MinPoint() { return myMinPoint; }
  //! Returns maximum point of node's AABB.
  OpenGl_RTVec4f& MaxPoint() { return myMaxPoint; }

  //! Returns minimum point of node's AABB.
  const OpenGl_RTVec4f& MinPoint() const { return myMinPoint; }
  //! Returns maximum point of node's AABB.
  const OpenGl_RTVec4f& MaxPoint() const { return myMaxPoint; }

  //! Returns index of left child of inner node.
  int LeftChild() const { return myDataRcrd.y(); }
  //! Sets index of left child of inner node.
  void SetLeftChild (int theChild) { myDataRcrd.y() = theChild; }

  //! Returns index of right child of inner node.
  int RightChild() const { return myDataRcrd.z(); }
  //! Sets index of right child of inner node.
  void SetRightChild (int theChild) { myDataRcrd.z() = theChild; }

  //! Returns index of begin triangle of leaf node.
  int BegTriangle() const { return myDataRcrd.y(); }
  //! Sets index of begin triangle of leaf node.
  void SetBegTriangle (int theIndex) { myDataRcrd.y() = theIndex; }

  //! Returns index of end triangle of leaf node.
  int EndTriangle() const { return myDataRcrd.z(); }
  //! Sets index of end triangle of leaf node.
  void SetEndTriangle (int theIndex) { myDataRcrd.z() = theIndex; }

  //! Returns level of the node in BVH tree.
  int Level() const { return myDataRcrd.w(); }
  //! Sets level of the node in BVH tree.
  void SetLevel (int theLevel) { myDataRcrd.w() = theLevel; }

  //! Is node a leaf (outer)?
  bool IsOuter() const { return myDataRcrd.x() == 1; }

  //! Sets node type to 'outer'.
  void SetOuter() { myDataRcrd.x() = 1; }
  //! Sets node type to 'inner'.
  void SetInner() { myDataRcrd.x() = 0; }

private:

  //! Minimum point of node's bounding box.
  OpenGl_RTVec4f myMinPoint;
  //! Maximum point of node's bounding box.
  OpenGl_RTVec4f myMaxPoint;

  //! Data vector (stores data fields of the node).
  OpenGl_RTVec4i myDataRcrd;
};

//! Stores parameters of BVH tree.
class OpenGl_BVH
{
public:

  //! Removes all tree nodes.
  void CleanUp();

  //! Adds new node to the tree.
  int PushNode (const OpenGl_BVHNode& theNode);

  //! Returns node with specified index.
  OpenGl_BVHNode Node (const int theIndex) const;

  //! Replaces node with specified index by the new one.
  void SetNode (const int theIndex, const OpenGl_BVHNode& theNode);

  //! Returns array of node min points.
  OpenGl_RTArray4f& MinPointBuffer() { return myMinPointBuffer; }
  //! Returns array of node max points.
  OpenGl_RTArray4f& MaxPointBuffer() { return myMaxPointBuffer; }
  //! Returns array of node data records.
  OpenGl_RTArray4i& DataRcrdBuffer() { return myDataRcrdBuffer; }

private:

  //! Array of min points of BVH nodes.
  OpenGl_RTArray4f myMinPointBuffer;
  //! Array of max points of BVH nodes.
  OpenGl_RTArray4f myMaxPointBuffer;
  //! Array of data vectors of BVH nodes.
  OpenGl_RTArray4i myDataRcrdBuffer;
};

//! Stores parameters of single node bin (slice of AABB).
struct OpenGl_BVHBin
{
  //! Creates new node bin.
  OpenGl_BVHBin(): Count (0) { }

  //! Number of primitives in the bin.
  int Count;

  //! AABB of the bin.
  OpenGl_AABB Volume;
};

//! Node building task.
struct OpenGl_BVHNodeTask
{
  //! Creates new node building task.
  OpenGl_BVHNodeTask();

  //! Creates new node building task.
  OpenGl_BVHNodeTask (const int theNodeToBuild,
                      const int theBegTriangle,
                      const int theEndTriangle);

  //! Index of building tree node.
  int NodeToBuild;
  //! Index of start node triangle.
  int BegTriangle;
  //! Index of final node triangle.
  int EndTriangle;
};

//! The array of bins of BVH tree node.
typedef std::vector<OpenGl_BVHBin,
                    NCollection_StdAllocator<OpenGl_BVHBin> > OpenGl_BinVector;

//! Binned SAH-based BVH builder.
class OpenGl_BinnedBVHBuilder
{
public:

  //! Creates new binned BVH builder.
  OpenGl_BinnedBVHBuilder();

  //! Releases binned BVH builder.
  ~OpenGl_BinnedBVHBuilder();

  //! Builds BVH tree using binned SAH algorithm.
  void Build (OpenGl_RaytraceScene& theGeometry, const float theEpsilon = 1e-3f);

  //! Sets maximum tree depth.
  void SetMaxDepth (const int theMaxDepth);

  //! Clears previously constructed BVH tree.
  void CleanUp();

  //! Return constructed BVH tree.
  OpenGl_BVH& Tree() { return myTree; }

private:

  //! Builds node using task info.
  void BuildNode (OpenGl_RaytraceScene& theGeometry, const int theTask);

  //! Arranges node triangles into bins.
  void GetSubVolumes (OpenGl_RaytraceScene& theGeometry, const OpenGl_BVHNode& theNode,
                                                OpenGl_BinVector& theBins, const int theAxis);

  //! Splits node triangles into two intervals for child nodes.
  int SplitTriangles (OpenGl_RaytraceScene& theGeometry, const int theFirst, const int theLast,
                                                  OpenGl_BVHNode& theNode, int theBin, const int theAxis);

private:

  //! Queue of node building tasks.
  std::vector<OpenGl_BVHNodeTask> myNodeTasksQueue;

  //! Builded BVH tree.
  OpenGl_BVH myTree;

  //! Maximum depth of BVH tree.
  int myMaxDepth;
};

#endif
#endif
