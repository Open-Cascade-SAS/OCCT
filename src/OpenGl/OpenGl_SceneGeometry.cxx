// Created on: 2013-08-27
// Created by: Denis BOGOLEPOV
// Copyright (c) 2013 OPEN CASCADE SAS
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

#include <OSD_Timer.hxx>
#include <OSD_Parallel.hxx>
#include <Standard_Assert.hxx>
#include <OpenGl_ArbTexBindless.hxx>
#include <OpenGl_PrimitiveArray.hxx>
#include <OpenGl_SceneGeometry.hxx>
#include <OpenGl_Structure.hxx>
#include <Graphic3d_GraphicDriver.hxx>

//! Use this macro to output BVH profiling info
// #define RAY_TRACE_PRINT_INFO

namespace
{
  //! Useful constant for null floating-point 4D vector.
  static const BVH_Vec4f ZERO_VEC_4F;
}

IMPLEMENT_STANDARD_RTTIEXT(OpenGl_TriangleSet, OpenGl_BVHTriangulation3f)

// =======================================================================
// function : OpenGl_RaytraceMaterial
// purpose  : Creates new default material
// =======================================================================
OpenGl_RaytraceMaterial::OpenGl_RaytraceMaterial()
: Ambient      (ZERO_VEC_4F),
  Diffuse      (ZERO_VEC_4F),
  Specular     (ZERO_VEC_4F),
  Emission     (ZERO_VEC_4F),
  Reflection   (ZERO_VEC_4F),
  Refraction   (ZERO_VEC_4F),
  Transparency (ZERO_VEC_4F)
{ }

// =======================================================================
// function : OpenGl_RaytraceMaterial
// purpose  : Creates new material with specified properties
// =======================================================================
OpenGl_RaytraceMaterial::OpenGl_RaytraceMaterial (const BVH_Vec4f& theAmbient,
                                                  const BVH_Vec4f& theDiffuse,
                                                  const BVH_Vec4f& theSpecular)
: Ambient      (theAmbient),
  Diffuse      (theDiffuse),
  Specular     (theSpecular),
  Emission     (ZERO_VEC_4F),
  Reflection   (ZERO_VEC_4F),
  Refraction   (ZERO_VEC_4F),
  Transparency (ZERO_VEC_4F)
{
  //
}

// =======================================================================
// function : OpenGl_RaytraceMaterial
// purpose  : Creates new material with specified properties
// =======================================================================
OpenGl_RaytraceMaterial::OpenGl_RaytraceMaterial (const BVH_Vec4f& theAmbient,
                                                  const BVH_Vec4f& theDiffuse,
                                                  const BVH_Vec4f& theSpecular,
                                                  const BVH_Vec4f& theEmission,
                                                  const BVH_Vec4f& theTranspar)
: Ambient      (theAmbient),
  Diffuse      (theDiffuse),
  Specular     (theSpecular),
  Emission     (theEmission),
  Reflection   (ZERO_VEC_4F),
  Refraction   (ZERO_VEC_4F),
  Transparency (theTranspar)
{
  //
}

// =======================================================================
// function : OpenGl_RaytraceMaterial
// purpose  : Creates new material with specified properties
// =======================================================================
OpenGl_RaytraceMaterial::OpenGl_RaytraceMaterial (const BVH_Vec4f& theAmbient,
                                                  const BVH_Vec4f& theDiffuse,
                                                  const BVH_Vec4f& theSpecular,
                                                  const BVH_Vec4f& theEmission,
                                                  const BVH_Vec4f& theTranspar,
                                                  const BVH_Vec4f& theReflection,
                                                  const BVH_Vec4f& theRefraction)
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
OpenGl_RaytraceLight::OpenGl_RaytraceLight (const BVH_Vec4f& theEmission,
                                            const BVH_Vec4f& thePosition)
: Emission (theEmission),
  Position (thePosition)
{
  //
}

// =======================================================================
// function : QuadBVH
// purpose  : Returns quad BVH (QBVH) tree produced from binary BVH
// =======================================================================
const QuadBvhHandle& OpenGl_TriangleSet::QuadBVH()
{
  if (!myIsDirty)
  {
    Standard_ASSERT_RAISE (!myQuadBVH.IsNull(), "Error! BVH was not collapsed into QBVH");
  }
  else
  {
    myQuadBVH = BVH()->CollapseToQuadTree(); // build binary BVH and collapse it

    myBVH->Clear(); // erase binary BVH
  }

  return myQuadBVH;
}

// =======================================================================
// function : Center
// purpose  : Returns centroid position along the given axis
// =======================================================================
Standard_ShortReal OpenGl_TriangleSet::Center (
  const Standard_Integer theIndex, const Standard_Integer theAxis) const
{
  // Note: Experiments show that the use of the box centroid (instead
  // of the triangle centroid) increases render performance up to 12%

  const BVH_Vec4i& aTriangle = Elements[theIndex];

  const Standard_ShortReal aVertex0 =
    BVH::VecComp<Standard_ShortReal, 3>::Get (Vertices[aTriangle.x()], theAxis);
  const Standard_ShortReal aVertex1 =
    BVH::VecComp<Standard_ShortReal, 3>::Get (Vertices[aTriangle.y()], theAxis);
  const Standard_ShortReal aVertex2 =
    BVH::VecComp<Standard_ShortReal, 3>::Get (Vertices[aTriangle.z()], theAxis);

  return (Min (Min (aVertex0, aVertex1), aVertex2) +
          Max (Max (aVertex0, aVertex1), aVertex2)) * 0.5f;
}

// =======================================================================
// function : Box
// purpose  : Returns AABB of primitive set
// =======================================================================
OpenGl_TriangleSet::BVH_BoxNt OpenGl_TriangleSet::Box() const
{
  BVH_BoxNt aBox = BVH_PrimitiveSet<Standard_ShortReal, 3>::Box();
  const BVH_Transform<Standard_ShortReal, 4>* aTransform = dynamic_cast<const BVH_Transform<Standard_ShortReal, 4>* > (Properties().get());
  if (aTransform == NULL)
  {
    return aBox;
  }

  BVH_BoxNt aTransformedBox;
  for (Standard_Integer aX = 0; aX <= 1; ++aX)
  {
    for (Standard_Integer aY = 0; aY <= 1; ++aY)
    {
      for (Standard_Integer aZ = 0; aZ <= 1; ++aZ)
      {
        BVH_Vec4f aCorner = aTransform->Transform() * BVH_Vec4f (
          aX == 0 ? aBox.CornerMin().x() : aBox.CornerMax().x(),
          aY == 0 ? aBox.CornerMin().y() : aBox.CornerMax().y(),
          aZ == 0 ? aBox.CornerMin().z() : aBox.CornerMax().z(),
          1.f);

        aTransformedBox.Add (aCorner.xyz());
      }
    }
  }
  return aTransformedBox;
}

// =======================================================================
// function : OpenGl_TriangleSet
// purpose  : Creates new OpenGL element triangulation
// =======================================================================
OpenGl_TriangleSet::OpenGl_TriangleSet (const Standard_Size theArrayID,
                                        const opencascade::handle<BVH_Builder<Standard_ShortReal, 3> >& theBuilder)
: BVH_Triangulation<Standard_ShortReal, 3> (theBuilder),
  myArrayID (theArrayID)
{
  //
}

// =======================================================================
// function : Clear
// purpose  : Clears ray-tracing geometry
// =======================================================================
void OpenGl_RaytraceGeometry::Clear()
{
  BVH_Geometry<Standard_ShortReal, 3>::BVH_Geometry::Clear();

  std::vector<OpenGl_RaytraceLight,
    NCollection_StdAllocator<OpenGl_RaytraceLight> > anEmptySources;

  Sources.swap (anEmptySources);

  std::vector<OpenGl_RaytraceMaterial,
    NCollection_StdAllocator<OpenGl_RaytraceMaterial> > anEmptyMaterials;

  Materials.swap (anEmptyMaterials);
}

struct OpenGL_BVHParallelBuilder
{
  BVH_ObjectSet<Standard_ShortReal, 3>* Set;

  OpenGL_BVHParallelBuilder (BVH_ObjectSet<Standard_ShortReal, 3>* theSet)
  : Set (theSet)
  {
    //
  }

  void operator() (const Standard_Integer theObjectIdx) const
  {
    OpenGl_TriangleSet* aTriangleSet = dynamic_cast<OpenGl_TriangleSet*> (
      Set->Objects().ChangeValue (static_cast<Standard_Integer> (theObjectIdx)).operator->());

    if (aTriangleSet != NULL)
      aTriangleSet->QuadBVH();
  }
};

// =======================================================================
// function : ProcessAcceleration
// purpose  : Performs post-processing of high-level BVH
// =======================================================================
Standard_Boolean OpenGl_RaytraceGeometry::ProcessAcceleration()
{
#ifdef RAY_TRACE_PRINT_INFO
  OSD_Timer aTimer;
#endif

  MarkDirty(); // force BVH rebuilding

#ifdef RAY_TRACE_PRINT_INFO
  aTimer.Reset();
  aTimer.Start();
#endif

  OSD_Parallel::For (0, Size(), OpenGL_BVHParallelBuilder (this));

  myBotLevelTreeDepth = 1;

  for (Standard_Integer anObjectIdx = 0; anObjectIdx < Size(); ++anObjectIdx)
  {
    OpenGl_TriangleSet* aTriangleSet = dynamic_cast<OpenGl_TriangleSet*> (
      myObjects.ChangeValue (anObjectIdx).operator->());

    Standard_ASSERT_RETURN (aTriangleSet != NULL,
      "Error! Failed to get triangulation of OpenGL element", Standard_False);

    Standard_ASSERT_RETURN (!aTriangleSet->QuadBVH().IsNull(),
      "Error! Failed to update bottom-level BVH of OpenGL element", Standard_False);

    QuadBvhHandle aBVH = aTriangleSet->QuadBVH();

    // correct data array of bottom-level BVH to set special flag for outer
    // nodes in order to distinguish them from outer nodes of top-level BVH
    for (Standard_Integer aNodeIdx = 0; aNodeIdx < aBVH->Length(); ++aNodeIdx)
    {
      if (aBVH->IsOuter (aNodeIdx))
      {
        aBVH->NodeInfoBuffer()[aNodeIdx].x() = -1;
      }
    }

    myBotLevelTreeDepth = Max (myBotLevelTreeDepth, aTriangleSet->QuadBVH()->Depth());
  }

#ifdef RAY_TRACE_PRINT_INFO
  aTimer.Stop();

  std::cout << "Updating bottom-level BVHs (sec): " <<
    aTimer.ElapsedTime() << std::endl;
#endif

#ifdef RAY_TRACE_PRINT_INFO
  aTimer.Reset();
  aTimer.Start();
#endif

  QuadBvhHandle aBVH = QuadBVH();

  Standard_ASSERT_RETURN (!aBVH.IsNull(),
    "Error! Failed to update high-level BVH of ray-tracing scene", Standard_False);

  myTopLevelTreeDepth = aBVH->Depth();

#ifdef RAY_TRACE_PRINT_INFO
  aTimer.Stop();

  std::cout << "Updating high-level BVH (sec): " <<
    aTimer.ElapsedTime() << std::endl;
#endif

  Standard_Integer aVerticesOffset = 0;
  Standard_Integer aElementsOffset = 0;
  Standard_Integer aBvhNodesOffset = QuadBVH()->Length();

  for (Standard_Integer aNodeIdx = 0; aNodeIdx < aBVH->Length(); ++aNodeIdx)
  {
    if (aBVH->IsOuter (aNodeIdx))
    {
      Standard_ASSERT_RETURN (aBVH->BegPrimitive (aNodeIdx) == aBVH->EndPrimitive (aNodeIdx),
        "Error! Invalid leaf node in high-level BVH (contains several objects)", Standard_False);

      const Standard_Integer anObjectIdx = aBVH->BegPrimitive (aNodeIdx);

      Standard_ASSERT_RETURN (anObjectIdx < myObjects.Size(),
        "Error! Invalid leaf node in high-level BVH (contains out-of-range object)", Standard_False);

      OpenGl_TriangleSet* aTriangleSet = dynamic_cast<OpenGl_TriangleSet*> (myObjects (anObjectIdx).get());

      // Note: We overwrite node info record to store parameters
      // of bottom-level BVH and triangulation of OpenGL element

      aBVH->NodeInfoBuffer()[aNodeIdx] = BVH_Vec4i (anObjectIdx + 1, // to keep leaf flag
                                                    aBvhNodesOffset,
                                                    aVerticesOffset,
                                                    aElementsOffset);

      aVerticesOffset += static_cast<Standard_Integer> (aTriangleSet->Vertices.size());
      aElementsOffset += static_cast<Standard_Integer> (aTriangleSet->Elements.size());

      aBvhNodesOffset += aTriangleSet->QuadBVH()->Length();
    }
  }

  return Standard_True;
}

// =======================================================================
// function : QuadBVH
// purpose  : Returns quad BVH (QBVH) tree produced from binary BVH
// =======================================================================
const QuadBvhHandle& OpenGl_RaytraceGeometry::QuadBVH()
{
  if (!myIsDirty)
  {
    Standard_ASSERT_RAISE (!myQuadBVH.IsNull(), "Error! BVH was not collapsed into QBVH");
  }
  else
  {
    myQuadBVH = BVH()->CollapseToQuadTree(); // build binary BVH and collapse it

    myBVH->Clear(); // erase binary BVH
  }

  return myQuadBVH;
}

// =======================================================================
// function : AccelerationOffset
// purpose  : Returns offset of bottom-level BVH for given leaf node
// =======================================================================
Standard_Integer OpenGl_RaytraceGeometry::AccelerationOffset (Standard_Integer theNodeIdx)
{
  const QuadBvhHandle& aBVH = QuadBVH();

  if (theNodeIdx >= aBVH->Length() || !aBVH->IsOuter (theNodeIdx))
    return INVALID_OFFSET;

  return aBVH->NodeInfoBuffer().at (theNodeIdx).y();
}

// =======================================================================
// function : VerticesOffset
// purpose  : Returns offset of triangulation vertices for given leaf node
// =======================================================================
Standard_Integer OpenGl_RaytraceGeometry::VerticesOffset (Standard_Integer theNodeIdx)
{
  const QuadBvhHandle& aBVH = QuadBVH();

  if (theNodeIdx >= aBVH->Length() || !aBVH->IsOuter (theNodeIdx))
    return INVALID_OFFSET;

  return aBVH->NodeInfoBuffer().at (theNodeIdx).z();
}

// =======================================================================
// function : ElementsOffset
// purpose  : Returns offset of triangulation elements for given leaf node
// =======================================================================
Standard_Integer OpenGl_RaytraceGeometry::ElementsOffset (Standard_Integer theNodeIdx)
{
  const QuadBvhHandle& aBVH = QuadBVH();

  if (theNodeIdx >= aBVH->Length() || !aBVH->IsOuter (theNodeIdx))
    return INVALID_OFFSET;

  return aBVH->NodeInfoBuffer().at (theNodeIdx).w();
}

// =======================================================================
// function : TriangleSet
// purpose  : Returns triangulation data for given leaf node
// =======================================================================
OpenGl_TriangleSet* OpenGl_RaytraceGeometry::TriangleSet (Standard_Integer theNodeIdx)
{
  const QuadBvhHandle& aBVH = QuadBVH();

  if (theNodeIdx >= aBVH->Length() || !aBVH->IsOuter (theNodeIdx))
    return NULL;

  if (aBVH->NodeInfoBuffer().at (theNodeIdx).x() > myObjects.Size())
    return NULL;

  return dynamic_cast<OpenGl_TriangleSet*> (
    myObjects (aBVH->NodeInfoBuffer().at (theNodeIdx).x() - 1).get());
}

// =======================================================================
// function : AcquireTextures
// purpose  : Makes the OpenGL texture handles resident
// =======================================================================
Standard_Boolean OpenGl_RaytraceGeometry::AcquireTextures (const Handle(OpenGl_Context)& theContext)
{
  if (theContext->arbTexBindless == NULL)
  {
    return Standard_True;
  }

#if !defined(GL_ES_VERSION_2_0)
  Standard_Integer aTexIter = 0;
  for (NCollection_Vector<Handle(OpenGl_Texture)>::Iterator aTexSrcIter (myTextures); aTexSrcIter.More(); aTexSrcIter.Next(), ++aTexIter)
  {
    GLuint64& aHandle = myTextureHandles[aTexIter];
    const Handle(OpenGl_Texture)& aTexture = aTexSrcIter.Value();
    if (!aTexture->Sampler()->IsValid()
     || !aTexture->Sampler()->IsImmutable())
    {
      // need to recreate texture sampler handle
      aHandle = GLuint64(-1); // specs do not define value for invalid handle, set -1 to initialize something
      if (!aTexture->InitSamplerObject (theContext))
      {
        continue;
      }

      aTexture->Sampler()->SetImmutable();
      aHandle = theContext->arbTexBindless->glGetTextureSamplerHandleARB (aTexture->TextureId(), aTexture->Sampler()->SamplerID());
      const GLenum anErr = glGetError();
      if (anErr != GL_NO_ERROR)
      {
        theContext->PushMessage (GL_DEBUG_SOURCE_APPLICATION, GL_DEBUG_TYPE_ERROR, 0, GL_DEBUG_SEVERITY_HIGH,
                                 TCollection_AsciiString ("Error: Failed to get 64-bit handle of OpenGL texture #") + int(anErr));
        myTextureHandles.clear();
        return Standard_False;
      }
    }

    theContext->arbTexBindless->glMakeTextureHandleResidentARB (aHandle);
    const GLenum anErr = glGetError();
    if (anErr != GL_NO_ERROR)
    {
      theContext->PushMessage (GL_DEBUG_SOURCE_APPLICATION, GL_DEBUG_TYPE_ERROR, 0, GL_DEBUG_SEVERITY_HIGH,
                               TCollection_AsciiString ("Error: Failed to make OpenGL texture resident #") + int(anErr));
      return Standard_False;
    }
  }
#endif

  return Standard_True;
}

// =======================================================================
// function : ReleaseTextures
// purpose  : Makes the OpenGL texture handles non-resident
// =======================================================================
Standard_Boolean OpenGl_RaytraceGeometry::ReleaseTextures (const Handle(OpenGl_Context)& theContext) const
{
  if (theContext->arbTexBindless == NULL)
  {
    return Standard_True;
  }

#if !defined(GL_ES_VERSION_2_0)
  for (size_t aTexIter = 0; aTexIter < myTextureHandles.size(); ++aTexIter)
  {
    theContext->arbTexBindless->glMakeTextureHandleNonResidentARB (myTextureHandles[aTexIter]);
    const GLenum anErr = glGetError();
    if (anErr != GL_NO_ERROR)
    {
      theContext->PushMessage (GL_DEBUG_SOURCE_APPLICATION, GL_DEBUG_TYPE_ERROR, 0, GL_DEBUG_SEVERITY_HIGH,
                               TCollection_AsciiString("Error: Failed to make OpenGL texture non-resident #") + int(anErr));
      return Standard_False;
    }
  }
#endif

  return Standard_True;
}

// =======================================================================
// function : AddTexture
// purpose  : Adds new OpenGL texture to the scene and returns its index
// =======================================================================
Standard_Integer OpenGl_RaytraceGeometry::AddTexture (const Handle(OpenGl_Texture)& theTexture)
{
  if (theTexture->TextureId() == OpenGl_Texture::NO_TEXTURE)
  {
    return -1;
  }

  NCollection_Vector<Handle (OpenGl_Texture)>::iterator anIter =
    std::find (myTextures.begin(), myTextures.end(), theTexture);

  if (anIter == myTextures.end())
  {
    if (myTextures.Size() >= MAX_TEX_NUMBER)
    {
      return -1;
    }

    myTextures.Append (theTexture);
  }

  return static_cast<Standard_Integer> (anIter - myTextures.begin());
}

// =======================================================================
// function : UpdateTextureHandles
// purpose  : Updates unique 64-bit texture handles to use in shaders
// =======================================================================
Standard_Boolean OpenGl_RaytraceGeometry::UpdateTextureHandles (const Handle(OpenGl_Context)& theContext)
{
  if (theContext->arbTexBindless == NULL)
  {
    return Standard_False;
  }

  myTextureHandles.clear();
  myTextureHandles.resize (myTextures.Size());

#if !defined(GL_ES_VERSION_2_0)
  Standard_Integer aTexIter = 0;
  for (NCollection_Vector<Handle(OpenGl_Texture)>::Iterator aTexSrcIter (myTextures); aTexSrcIter.More(); aTexSrcIter.Next(), ++aTexIter)
  {
    GLuint64& aHandle = myTextureHandles[aTexIter];
    aHandle = GLuint64(-1); // specs do not define value for invalid handle, set -1 to initialize something

    const Handle(OpenGl_Texture)& aTexture = aTexSrcIter.Value();
    if (!aTexture->Sampler()->IsValid()
     && !aTexture->InitSamplerObject (theContext))
    {
      continue;
    }

    aTexture->Sampler()->SetImmutable();
    aHandle = theContext->arbTexBindless->glGetTextureSamplerHandleARB (aTexture->TextureId(), aTexture->Sampler()->SamplerID());
    const GLenum anErr = glGetError();
    if (anErr != GL_NO_ERROR)
    {
      theContext->PushMessage (GL_DEBUG_SOURCE_APPLICATION, GL_DEBUG_TYPE_ERROR, 0, GL_DEBUG_SEVERITY_HIGH,
                               TCollection_AsciiString ("Error: Failed to get 64-bit handle of OpenGL texture #") + int(anErr));
      myTextureHandles.clear();
      return Standard_False;
    }
  }
#endif

  return Standard_True;
}

namespace OpenGl_Raytrace
{
  // =======================================================================
  // function : IsRaytracedElement
  // purpose  : Checks to see if the element contains ray-trace geometry
  // =======================================================================
  Standard_Boolean IsRaytracedElement (const OpenGl_ElementNode* theNode)
  {
    OpenGl_PrimitiveArray* anArray = dynamic_cast<OpenGl_PrimitiveArray*> (theNode->elem);
    return anArray != NULL
        && anArray->DrawMode() >= GL_TRIANGLES;
  }

  // =======================================================================
  // function : IsRaytracedElement
  // purpose  : Checks to see if the element contains ray-trace geometry
  // =======================================================================
  Standard_Boolean IsRaytracedElement (const OpenGl_Element* theElement)
  {
    const OpenGl_PrimitiveArray* anArray = dynamic_cast<const OpenGl_PrimitiveArray*> (theElement);
    return anArray != NULL
        && anArray->DrawMode() >= GL_TRIANGLES;
  }

  // =======================================================================
  // function : IsRaytracedGroup
  // purpose  : Checks to see if the group contains ray-trace geometry
  // =======================================================================
  Standard_Boolean IsRaytracedGroup (const OpenGl_Group* theGroup)
  {
    for (const OpenGl_ElementNode* aNode = theGroup->FirstNode(); aNode != NULL; aNode = aNode->next)
    {
      if (IsRaytracedElement (aNode))
      {
        return Standard_True;
      }
    }
    return Standard_False;
  }
}
