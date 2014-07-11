// Created on: 2013-08-27
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

#ifndef _OpenGl_SceneGeometry_Header
#define _OpenGl_SceneGeometry_Header

#include <BVH_Geometry.hxx>
#include <BVH_Triangulation.hxx>
#include <NCollection_StdAllocator.hxx>

class  OpenGl_Element;
struct OpenGl_ElementNode;
class  OpenGl_Group;
class  OpenGl_Structure;
class  OpenGl_PrimitiveArray;

namespace OpenGl_Raytrace
{
  //! Checks to see if the group contains ray-trace geometry.
  Standard_Boolean IsRaytracedGroup (const OpenGl_Group* theGroup);

  //! Checks to see if the element contains ray-trace geometry.
  Standard_Boolean IsRaytracedElement (const OpenGl_ElementNode* theNode);

  //! Checks to see if the element contains ray-trace geometry.
  Standard_Boolean IsRaytracedElement (const OpenGl_Element* theElement);

  //! Checks to see if the structure contains ray-trace geometry.
  Standard_Boolean IsRaytracedStructure (const OpenGl_Structure* theStructure);
}

//! Stores properties of surface material.
class OpenGl_RaytraceMaterial
{
public:

  //! Ambient reflection coefficient.
  BVH_Vec4f Ambient;

  //! Diffuse reflection coefficient.
  BVH_Vec4f Diffuse;

  //! Glossy reflection coefficient.
  BVH_Vec4f Specular;

  //! Material emission.
  BVH_Vec4f Emission;

  //! Specular reflection coefficient.
  BVH_Vec4f Reflection;

  //! Specular refraction coefficient.
  BVH_Vec4f Refraction;

  //! Material transparency.
  BVH_Vec4f Transparency;

public:

  //! Creates new default material.
  OpenGl_RaytraceMaterial();

  //! Creates new material with specified properties.
  OpenGl_RaytraceMaterial (const BVH_Vec4f& theAmbient,
                           const BVH_Vec4f& theDiffuse,
                           const BVH_Vec4f& theSpecular);

  //! Creates new material with specified properties.
  OpenGl_RaytraceMaterial (const BVH_Vec4f& theAmbient,
                           const BVH_Vec4f& theDiffuse,
                           const BVH_Vec4f& theSpecular,
                           const BVH_Vec4f& theEmission,
                           const BVH_Vec4f& theTranspar);

  //! Creates new material with specified properties.
  OpenGl_RaytraceMaterial (const BVH_Vec4f& theAmbient,
                           const BVH_Vec4f& theDiffuse,
                           const BVH_Vec4f& theSpecular,
                           const BVH_Vec4f& theEmission,
                           const BVH_Vec4f& theTranspar,
                           const BVH_Vec4f& theReflection,
                           const BVH_Vec4f& theRefraction);

  //! Returns packed (serialized) representation of material.
  const Standard_ShortReal* Packed()
  {
    return reinterpret_cast<Standard_ShortReal*> (this);
  }
};

//! Stores properties of OpenGL light source.
class OpenGl_RaytraceLight
{
public:

  //! Diffuse intensity (in terms of OpenGL).
  BVH_Vec4f Diffuse;

  //! Position of light source (in terms of OpenGL).
  BVH_Vec4f Position;

public:

  //! Creates new light source.
  OpenGl_RaytraceLight (const BVH_Vec4f& theDiffuse,
                        const BVH_Vec4f& thePosition);

  //! Returns packed (serialized) representation of light source.
  const Standard_ShortReal* Packed()
  {
    return reinterpret_cast<Standard_ShortReal*> (this);
  }
};

//! Triangulation of single OpenGL primitive array.
class OpenGl_TriangleSet : public BVH_Triangulation<Standard_ShortReal, 4>
{
public:

  //! Value of invalid material index to return in case of errors.
  static const Standard_Integer INVALID_MATERIAL = -1;

public:

  //! Creates new OpenGL element triangulation.
  OpenGl_TriangleSet (const Standard_Size theArrayID)
  : BVH_Triangulation<Standard_ShortReal, 4>(),
    myArrayID (theArrayID)
   {
     //
   }

  //! Releases resources of OpenGL element triangulation.
  ~OpenGl_TriangleSet()
  {
    //
  }

  //! Returns Id of associated primitive array.
  const Standard_Size AssociatedPArrayID() const
  {
    return myArrayID;
  }

  //! Returns material index of triangle set.
  Standard_Integer MaterialIndex() const
  {
    if (Elements.size() == 0)
      return INVALID_MATERIAL;

    return Elements.front().w();
  }

  //! Sets material index for entire triangle set.
  void SetMaterialIndex (Standard_Integer aMatID)
  {
    for (Standard_Size anIdx = 0; anIdx < Elements.size(); ++anIdx)
      Elements[anIdx].w() = aMatID;
  }

  //! Returns AABB of primitive set.
  BVH_BoxNt Box() const
  {
    const BVH_Transform<Standard_ShortReal, 4>* aTransform = 
      dynamic_cast<const BVH_Transform<Standard_ShortReal, 4>* > (Properties().operator->());
 
    BVH_BoxNt aBox = BVH_PrimitiveSet<Standard_ShortReal, 4>::Box(); 
 
    if (aTransform)
    {
      return aTransform->Apply (aBox);
    }
 
    return aBox;
  }

public:

  BVH_Array4f Normals; //!< Array of vertex normals.
 
private:

  Standard_Size myArrayID; //!< Id of associated primitive array.

};

//! Stores geometry of ray-tracing scene.
class OpenGl_RaytraceGeometry : public BVH_Geometry<Standard_ShortReal, 4>
{
public:

  //! Value of invalid offset to return in case of errors.
  static const Standard_Integer INVALID_OFFSET = -1;

public:

  //! Array of properties of light sources.
  std::vector<OpenGl_RaytraceLight,
    NCollection_StdAllocator<OpenGl_RaytraceLight> > Sources;

  //! Array of 'front' material properties.
  std::vector<OpenGl_RaytraceMaterial,
    NCollection_StdAllocator<OpenGl_RaytraceMaterial> > Materials;

  //! Global ambient from all light sources.
  BVH_Vec4f Ambient;

public:

  //! Creates uninitialized ray-tracing geometry.
  OpenGl_RaytraceGeometry()
  : BVH_Geometry<Standard_ShortReal, 4>(),
    myHighLevelTreeDepth (0),
    myBottomLevelTreeDepth (0)
  {
    //
  }

  //! Releases resources of ray-tracing geometry.
  ~OpenGl_RaytraceGeometry()
  {
    //
  }

  //! Clears ray-tracing geometry.
  void Clear();

  //! Clears only ray-tracing materials.
  void ClearMaterials()
  {
    std::vector<OpenGl_RaytraceMaterial,
      NCollection_StdAllocator<OpenGl_RaytraceMaterial> > anEmptyMaterials;
 
    Materials.swap (anEmptyMaterials);
  }

public:

  //! Performs post-processing of high-level scene BVH.
  Standard_Boolean ProcessAcceleration();

  //! Returns offset of bottom-level BVH for given leaf node.
  //! If the node index is not valid the function returns -1.
  //! @note Can be used after processing acceleration structure.
  Standard_Integer AccelerationOffset (Standard_Integer theNodeIdx);

  //! Returns offset of triangulation vertices for given leaf node.
  //! If the node index is not valid the function returns -1.
  //! @note Can be used after processing acceleration structure.
  Standard_Integer VerticesOffset (Standard_Integer theNodeIdx);

  //! Returns offset of triangulation elements for given leaf node.
  //! If the node index is not valid the function returns -1.
  //! @note Can be used after processing acceleration structure.
  Standard_Integer ElementsOffset (Standard_Integer theNodeIdx);

  //! Returns triangulation data for given leaf node.
  //! If the node index is not valid the function returns NULL.
  //! @note Can be used after processing acceleration structure.
  OpenGl_TriangleSet* TriangleSet (Standard_Integer theNodeIdx);

  //! Returns depth of high-level scene BVH from last build.
  Standard_Integer HighLevelTreeDepth() const
  {
    return myHighLevelTreeDepth;
  }

  //! Returns maximum depth of bottom-level scene BVHs from last build.
  Standard_Integer BottomLevelTreeDepth() const
  {
    return myBottomLevelTreeDepth;
  }

protected:

  Standard_Integer myHighLevelTreeDepth;   //!< Depth of high-level scene BVH from last build
  Standard_Integer myBottomLevelTreeDepth; //!< Maximum depth of bottom-level scene BVHs from last build

};

#endif
