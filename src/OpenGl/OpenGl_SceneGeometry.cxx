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

#ifdef HAVE_TBB
  // On Windows, function TryEnterCriticalSection has appeared in Windows NT
  // and is surrounded by #ifdef in MS VC++ 7.1 headers.
  // Thus to use it we need to define appropriate macro saying that we will
  // run on Windows NT 4.0 at least
  #if defined(_WIN32) && !defined(_WIN32_WINNT)
    #define _WIN32_WINNT 0x0501
  #endif

  #include <tbb/tbb.h>
#endif

#include <OpenGl_SceneGeometry.hxx>

#include <OpenGl_ArbTexBindless.hxx>
#include <OpenGl_PrimitiveArray.hxx>
#include <OpenGl_Structure.hxx>
#include <OSD_Timer.hxx>
#include <Standard_Assert.hxx>

//! Use this macro to output BVH profiling info
// #define RAY_TRACE_PRINT_INFO

namespace
{
  //! Useful constant for null floating-point 4D vector.
  static const BVH_Vec4f ZERO_VEC_4F;
}

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
OpenGl_RaytraceLight::OpenGl_RaytraceLight (const BVH_Vec4f& theDiffuse,
                                            const BVH_Vec4f& thePosition)
: Diffuse (theDiffuse),
  Position (thePosition)
{
  //
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
  const BVH_Transform<Standard_ShortReal, 4>* aTransform =
    dynamic_cast<const BVH_Transform<Standard_ShortReal, 4>* > (Properties().operator->());

  BVH_BoxNt aBox = BVH_PrimitiveSet<Standard_ShortReal, 3>::Box();

  if (aTransform != NULL)
  {
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

          aTransformedBox.Add (reinterpret_cast<BVH_Vec3f&> (aCorner));
        }
      }
    }

    return aTransformedBox;
  }

  return aBox;
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

#ifdef HAVE_TBB

struct OpenGL_BVHParallelBuilder
{
  BVH_ObjectSet<Standard_ShortReal, 3>* Set;

  OpenGL_BVHParallelBuilder (BVH_ObjectSet<Standard_ShortReal, 3>* theSet)
    : Set (theSet)
  {
    //
  }

  void operator() (const tbb::blocked_range<size_t>& theRange) const
  {
    for (size_t anObjectIdx = theRange.begin(); anObjectIdx != theRange.end(); ++anObjectIdx)
    {
      OpenGl_TriangleSet* aTriangleSet = dynamic_cast<OpenGl_TriangleSet*> (
        Set->Objects().ChangeValue (static_cast<Standard_Integer> (anObjectIdx)).operator->());

      if (aTriangleSet != NULL)
      {
        aTriangleSet->BVH();
      }
    }
  }
};

#endif

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

#ifdef HAVE_TBB
  // If Intel TBB is available, perform the preliminary
  // construction of bottom-level scene BVHs
  tbb::parallel_for (tbb::blocked_range<size_t> (0, Size()),
    OpenGL_BVHParallelBuilder (this));
#endif

  myBottomLevelTreeDepth = 0;

  for (Standard_Integer anObjectIdx = 0; anObjectIdx < Size(); ++anObjectIdx)
  {
    OpenGl_TriangleSet* aTriangleSet = dynamic_cast<OpenGl_TriangleSet*> (
      myObjects.ChangeValue (anObjectIdx).operator->());

    Standard_ASSERT_RETURN (aTriangleSet != NULL,
      "Error! Failed to get triangulation of OpenGL element", Standard_False);

    Standard_ASSERT_RETURN (!aTriangleSet->BVH().IsNull(),
      "Error! Failed to update bottom-level BVH of OpenGL element", Standard_False);

    myBottomLevelTreeDepth = Max (myBottomLevelTreeDepth, aTriangleSet->BVH()->Depth());
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

  NCollection_Handle<BVH_Tree<Standard_ShortReal, 3> > aBVH = BVH();

#ifdef RAY_TRACE_PRINT_INFO
  aTimer.Stop();

  std::cout << "Updating high-level BVH (sec): " <<
    aTimer.ElapsedTime() << std::endl;
#endif

  Standard_ASSERT_RETURN (!aBVH.IsNull(),
    "Error! Failed to update high-level BVH of ray-tracing scene", Standard_False);

  myHighLevelTreeDepth = aBVH->Depth();

  Standard_Integer aVerticesOffset = 0;
  Standard_Integer aElementsOffset = 0;
  Standard_Integer aBVHNodesOffset = 0;

  for (Standard_Integer aNodeIdx = 0; aNodeIdx < aBVH->Length(); ++aNodeIdx)
  {
    if (!aBVH->IsOuter (aNodeIdx))
      continue;

    Standard_ASSERT_RETURN (aBVH->BegPrimitive (aNodeIdx) == aBVH->EndPrimitive (aNodeIdx),
      "Error! Invalid leaf node in high-level BVH (contains several objects)", Standard_False);

    Standard_Integer anObjectIdx = aBVH->BegPrimitive (aNodeIdx);

    Standard_ASSERT_RETURN (anObjectIdx < myObjects.Size(),
      "Error! Invalid leaf node in high-level BVH (contains out-of-range object)", Standard_False);

    OpenGl_TriangleSet* aTriangleSet = dynamic_cast<OpenGl_TriangleSet*> (
      myObjects.ChangeValue (anObjectIdx).operator->());

    // Note: We overwrite node info record to store parameters
    // of bottom-level BVH and triangulation of OpenGL element

    aBVH->NodeInfoBuffer().at (aNodeIdx) = BVH_Vec4i (
      anObjectIdx + 1 /* to keep leaf flag */, aBVHNodesOffset, aVerticesOffset, aElementsOffset);

    aVerticesOffset += (int)aTriangleSet->Vertices.size();
    aElementsOffset += (int)aTriangleSet->Elements.size();
    aBVHNodesOffset += aTriangleSet->BVH()->Length();
  }

  return Standard_True;
}

// =======================================================================
// function : AccelerationOffset
// purpose  : Returns offset of bottom-level BVH for given leaf node
// =======================================================================
Standard_Integer OpenGl_RaytraceGeometry::AccelerationOffset (Standard_Integer theNodeIdx)
{
  const NCollection_Handle<BVH_Tree<Standard_ShortReal, 3> >& aBVH = BVH();

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
  const NCollection_Handle<BVH_Tree<Standard_ShortReal, 3> >& aBVH = BVH();

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
  const NCollection_Handle<BVH_Tree<Standard_ShortReal, 3> >& aBVH = BVH();

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
  const NCollection_Handle<BVH_Tree<Standard_ShortReal, 3> >& aBVH = BVH();

  if (theNodeIdx >= aBVH->Length() || !aBVH->IsOuter (theNodeIdx))
    return NULL;

  if (aBVH->NodeInfoBuffer().at (theNodeIdx).x() > myObjects.Size())
    return NULL;

  return dynamic_cast<OpenGl_TriangleSet*> (myObjects.ChangeValue (
    aBVH->NodeInfoBuffer().at (theNodeIdx).x() - 1).operator->());
}

// =======================================================================
// function : AcquireTextures
// purpose  : Makes the OpenGL texture handles resident
// =======================================================================
Standard_Boolean OpenGl_RaytraceGeometry::AcquireTextures (const Handle(OpenGl_Context)& theContext) const
{
  if (theContext->arbTexBindless == NULL)
  {
    return Standard_True;
  }

#if !defined(GL_ES_VERSION_2_0)
  for (Standard_Integer anIdx = 0; anIdx < myTextures.Size(); ++anIdx)
  {
    theContext->arbTexBindless->glMakeTextureHandleResidentARB (myTextureHandles[anIdx]);

    if (glGetError() != GL_NO_ERROR)
    {
#ifdef RAY_TRACE_PRINT_INFO
      std::cout << "Error: Failed to make OpenGL texture resident" << std::endl;
#endif
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
  for (Standard_Integer anIdx = 0; anIdx < myTextures.Size(); ++anIdx)
  {
    theContext->arbTexBindless->glMakeTextureHandleNonResidentARB (myTextureHandles[anIdx]);

    if (glGetError() != GL_NO_ERROR)
    {
#ifdef RAY_TRACE_PRINT_INFO
      std::cout << "Error: Failed to make OpenGL texture non-resident" << std::endl;
#endif
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

#if !defined(GL_ES_VERSION_2_0)
  for (Standard_Integer anIdx = 0; anIdx < myTextures.Size(); ++anIdx)
  {
    const GLuint64 aHandle = theContext->arbTexBindless->glGetTextureHandleARB (
      myTextures.Value (anIdx)->TextureId());

    if (glGetError() != GL_NO_ERROR)
    {
#ifdef RAY_TRACE_PRINT_INFO
      std::cout << "Error: Failed to get 64-bit handle of OpenGL texture" << std::endl;
#endif
      return Standard_False;
    }

    myTextureHandles.push_back (aHandle);
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
  Standard_Boolean IsRaytracedStructure (const OpenGl_Structure* theStructure)
  {
    for (OpenGl_Structure::GroupIterator aGroupIter (theStructure->DrawGroups());
         aGroupIter.More(); aGroupIter.Next())
    {
      if (aGroupIter.Value()->IsRaytracable())
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
