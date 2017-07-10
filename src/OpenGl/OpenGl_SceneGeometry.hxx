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
#include <BVH_BinnedBuilder.hxx>
#include <NCollection_StdAllocator.hxx>
#include <OpenGl_TextureBufferArb.hxx>
#include <OpenGl_Texture.hxx>
#include <OpenGl_Sampler.hxx>

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

  //! Texture transformation matrix.
  BVH_Mat4f TextureTransform;

  //! Physically-based material properties (used in path tracing engine).
  struct Physical
  {
    //! Weight of coat specular/glossy BRDF.
    BVH_Vec4f Kc;

    //! Weight of base diffuse BRDF.
    BVH_Vec4f Kd;

    //! Weight of base specular/glossy BRDF.
    BVH_Vec4f Ks;

    //! Weight of base specular/glossy BTDF.
    BVH_Vec4f Kt;

    //! Radiance emitted by the surface.
    BVH_Vec4f Le;

    //! Fresnel coefficients of coat layer.
    BVH_Vec4f FresnelCoat;

    //! Fresnel coefficients of base layer.
    BVH_Vec4f FresnelBase;

    //! Absorption color/intensity.
    BVH_Vec4f Absorption;

  } BSDF;

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
  BVH_Vec4f Emission;

  //! Position of light source (in terms of OpenGL).
  BVH_Vec4f Position;

public:

  //! Creates new light source.
  OpenGl_RaytraceLight() { }

  //! Creates new light source.
  OpenGl_RaytraceLight (const BVH_Vec4f& theEmission,
                        const BVH_Vec4f& thePosition);

  //! Returns packed (serialized) representation of light source.
  const Standard_ShortReal* Packed()
  {
    return reinterpret_cast<Standard_ShortReal*> (this);
  }
};

//! Shared pointer to quad BVH (QBVH) tree.
typedef opencascade::handle<BVH_Tree<Standard_ShortReal, 3, BVH_QuadTree> > QuadBvhHandle;
typedef BVH_Triangulation<Standard_ShortReal, 3> OpenGl_BVHTriangulation3f;

//! Triangulation of single OpenGL primitive array.
class OpenGl_TriangleSet : public OpenGl_BVHTriangulation3f
{
  DEFINE_STANDARD_RTTIEXT(OpenGl_TriangleSet, OpenGl_BVHTriangulation3f)
public:

  //! Value of invalid material index to return in case of errors.
  static const Standard_Integer INVALID_MATERIAL = -1;

public:

  //! Creates new OpenGL element triangulation.
  OpenGl_TriangleSet (const Standard_Size theArrayID,
                      const opencascade::handle<BVH_Builder<Standard_ShortReal, 3> >& theBuilder);

  //! Returns ID of associated primitive array.
  Standard_Size AssociatedPArrayID() const
  {
    return myArrayID;
  }

  //! Returns material index of triangle set.
  Standard_Integer MaterialIndex() const
  {
    if (Elements.size() == 0)
    {
      return INVALID_MATERIAL;
    }

    return Elements.front().w();
  }

  //! Sets material index for entire triangle set.
  void SetMaterialIndex (Standard_Integer theMatID)
  {
    for (Standard_Size anIdx = 0; anIdx < Elements.size(); ++anIdx)
    {
      Elements[anIdx].w() = theMatID;
    }
  }

  //! Returns AABB of primitive set.
  virtual BVH_BoxNt Box() const Standard_OVERRIDE;

  //! Returns AABB of the given object.
  using BVH_Triangulation<Standard_ShortReal, 3>::Box;

  //! Returns centroid position along the given axis.
  virtual Standard_ShortReal Center (const Standard_Integer theIndex, const Standard_Integer theAxis) const Standard_OVERRIDE;

  //! Returns quad BVH (QBVH) tree produced from binary BVH.
  const QuadBvhHandle& QuadBVH();

public:

  BVH_Array3f Normals; //!< Array of vertex normals.
  BVH_Array2f TexCrds; //!< Array of texture coords.

private:

  Standard_Size myArrayID; //!< ID of associated primitive array.

  QuadBvhHandle myQuadBVH; //!< QBVH produced from binary BVH tree.

};

//! Stores geometry of ray-tracing scene.
class OpenGl_RaytraceGeometry : public BVH_Geometry<Standard_ShortReal, 3>
{
public:

  //! Value of invalid offset to return in case of errors.
  static const Standard_Integer INVALID_OFFSET = -1;

  //! Maximum number of textures used in ray-tracing shaders.
  //! This is not restriction of the solution implemented, but
  //! rather the reasonable limit of the number of textures in
  //! various applications (can be increased if needed).
  static const Standard_Integer MAX_TEX_NUMBER = 32;

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
  : BVH_Geometry<Standard_ShortReal, 3>(),
    myTopLevelTreeDepth (0),
    myBotLevelTreeDepth (0)
  {
    //
  }

  //! Releases resources of ray-tracing geometry.
  ~OpenGl_RaytraceGeometry()
  {
    //
  }

  //! Clears only ray-tracing materials.
  void ClearMaterials()
  {
    std::vector<OpenGl_RaytraceMaterial,
      NCollection_StdAllocator<OpenGl_RaytraceMaterial> > anEmptyMaterials;

    Materials.swap (anEmptyMaterials);

    myTextures.Clear();
  }

  //! Clears ray-tracing geometry.
  virtual void Clear() Standard_OVERRIDE;

public: //! @name methods related to acceleration structure

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

  //! Returns quad BVH (QBVH) tree produced from binary BVH.
  const QuadBvhHandle& QuadBVH();

public: //! @name methods related to texture management

  //! Checks if scene contains textured objects.
  Standard_Boolean HasTextures() const
  {
    return !myTextures.IsEmpty();
  }

  //! Adds new OpenGL texture to the scene and returns its index.
  Standard_Integer AddTexture (const Handle(OpenGl_Texture)& theTexture);

  //! Updates unique 64-bit texture handles to use in shaders.
  Standard_Boolean UpdateTextureHandles (const Handle(OpenGl_Context)& theContext);

  //! Makes the OpenGL texture handles resident (must be called before using).
  Standard_Boolean AcquireTextures (const Handle(OpenGl_Context)& theContext);

  //! Makes the OpenGL texture handles non-resident (must be called after using).
  Standard_Boolean ReleaseTextures (const Handle(OpenGl_Context)& theContext) const;

  //! Returns array of texture handles.
  const std::vector<GLuint64>& TextureHandles() const
  {
    return myTextureHandles;
  }

  //! Releases OpenGL resources.
  void ReleaseResources (const Handle(OpenGl_Context)& )
  {
    //
  }

public: //! @name auxiliary methods

  //! Returns depth of top-level scene BVH from last build.
  Standard_Integer TopLevelTreeDepth() const
  {
    return myTopLevelTreeDepth;
  }

  //! Returns maximum depth of bottom-level scene BVHs from last build.
  Standard_Integer BotLevelTreeDepth() const
  {
    return myBotLevelTreeDepth;
  }

protected:

  NCollection_Vector<Handle(OpenGl_Texture)> myTextures;           //!< Array of texture maps shared between rendered objects
  std::vector<GLuint64>                      myTextureHandles;     //!< Array of unique 64-bit texture handles obtained from OpenGL
  Standard_Integer                           myTopLevelTreeDepth;  //!< Depth of high-level scene BVH from last build
  Standard_Integer                           myBotLevelTreeDepth;  //!< Maximum depth of bottom-level scene BVHs from last build

  QuadBvhHandle myQuadBVH; //!< QBVH produced from binary BVH tree.

};

#endif
