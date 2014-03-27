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

#ifdef HAVE_CONFIG_H
  #include <config.h>
#endif

#ifdef HAVE_OPENCL

#include <OpenGl_Cl.hxx>

#if defined(_WIN32)

  #include <windows.h>
  #include <wingdi.h>

  #pragma comment (lib, "DelayImp.lib")
  #pragma comment (lib, "OpenCL.lib")

#elif defined(__APPLE__) && !defined(MACOSX_USE_GLX)
  #include <OpenGL/CGLCurrent.h>
#else
  #include <GL/glx.h>
#endif

#include <OpenGl_Context.hxx>
#include <OpenGl_Texture.hxx>
#include <OpenGl_View.hxx>
#include <OpenGl_Workspace.hxx>
#include <Standard_Assert.hxx>

using namespace OpenGl_Raytrace;

//! Use this macro to output ray-tracing debug info
//#define RAY_TRACE_PRINT_INFO

#ifdef RAY_TRACE_PRINT_INFO
  #include <OSD_Timer.hxx>
#endif

//! OpenCL source of ray-tracing kernels.
extern const char THE_RAY_TRACE_OPENCL_SOURCE[];

// =======================================================================
// function : MatVecMult
// purpose  : Multiples 4x4 matrix by 4D vector
// =======================================================================
template<typename T>
BVH_Vec4f MatVecMult (const T m[16], const BVH_Vec4f& v)
{
  return BVH_Vec4f (
    static_cast<float> (m[ 0] * v.x() + m[ 4] * v.y() +
                        m[ 8] * v.z() + m[12] * v.w()),
    static_cast<float> (m[ 1] * v.x() + m[ 5] * v.y() +
                        m[ 9] * v.z() + m[13] * v.w()),
    static_cast<float> (m[ 2] * v.x() + m[ 6] * v.y() +
                        m[10] * v.z() + m[14] * v.w()),
    static_cast<float> (m[ 3] * v.x() + m[ 7] * v.y() +
                        m[11] * v.z() + m[15] * v.w()));
}

// =======================================================================
// function : UpdateRaytraceEnvironmentMap
// purpose  : Updates environment map for ray-tracing
// =======================================================================
Standard_Boolean OpenGl_Workspace::UpdateRaytraceEnvironmentMap()
{
  if (myView.IsNull())
    return Standard_False;

  if (myViewModificationStatus == myView->ModificationState())
    return Standard_True;

  cl_int anError = CL_SUCCESS;

  if (myRaytraceEnvironment != NULL)
    clReleaseMemObject (myRaytraceEnvironment);

  Standard_Integer aSizeX = 1;
  Standard_Integer aSizeY = 1;

  if (!myView->TextureEnv().IsNull() && myView->SurfaceDetail() != Visual3d_TOD_NONE)
  {
    aSizeX = (myView->TextureEnv()->SizeX() <= 0) ? 1 : myView->TextureEnv()->SizeX();
    aSizeY = (myView->TextureEnv()->SizeY() <= 0) ? 1 : myView->TextureEnv()->SizeY();
  }

  cl_image_format anImageFormat;

  anImageFormat.image_channel_order = CL_RGBA;
  anImageFormat.image_channel_data_type = CL_FLOAT;

  myRaytraceEnvironment = clCreateImage2D (myComputeContext,
    CL_MEM_READ_ONLY, &anImageFormat, aSizeX, aSizeY, 0, NULL, &anError);

  cl_float* aPixelData = new cl_float[aSizeX * aSizeY * 4];

  // Note: texture format is not compatible with OpenCL image
  // (it's not possible to create image directly from texture)

  if (!myView->TextureEnv().IsNull() && myView->SurfaceDetail() != Visual3d_TOD_NONE)
  {
    myView->TextureEnv()->Bind (GetGlContext());

    glGetTexImage (GL_TEXTURE_2D,
                   0,
                   GL_RGBA,
                   GL_FLOAT,
                   aPixelData);

    myView->TextureEnv()->Unbind (GetGlContext());
  }
  else
  {
    for (Standard_Integer aPixel = 0; aPixel < aSizeX * aSizeY * 4; ++aPixel)
      aPixelData[aPixel] = 0.f;
  }

  size_t anImageOffset[] = { 0,
                             0,
                             0 };

  size_t anImageRegion[] = { aSizeX,
                             aSizeY,
                             1 };

  anError |= clEnqueueWriteImage (myComputeQueue, myRaytraceEnvironment,
    CL_TRUE, anImageOffset, anImageRegion, 0, 0, aPixelData, 0, NULL, NULL);

#ifdef RAY_TRACE_PRINT_INFO
  if (anError != CL_SUCCESS)
    std::cout << "Error! Failed to write environment map image!" << std::endl;
#endif

  delete[] aPixelData;

  myViewModificationStatus = myView->ModificationState();

  return (anError == CL_SUCCESS);
}

// =======================================================================
// function : UpdateRaytraceGeometry
// purpose  : Updates 3D scene geometry for ray tracing
// =======================================================================
Standard_Boolean OpenGl_Workspace::UpdateRaytraceGeometry (Standard_Boolean theCheck)
{
  if (myView.IsNull())
    return Standard_False;

  // Note: In 'check' mode the scene geometry is analyzed for modifications
  // This is light-weight procedure performed for each frame

  if (!theCheck)
  {
    myRaytraceGeometry.Clear();

    myIsRaytraceDataValid = Standard_False;
  }
  else
  {
    if (myLayersModificationStatus != myView->LayerList().ModificationState())
    {
      return UpdateRaytraceGeometry (Standard_False);
    }
  }

  Standard_ShortReal* aTransform (NULL);

  // The set of processed structures (reflected to ray-tracing)
  // This set is used to remove out-of-date records from the
  // hash map of structures
  std::set<const OpenGl_Structure*> anElements;

  const OpenGl_LayerList& aList = myView->LayerList();

  for (OpenGl_SequenceOfLayers::Iterator anLayerIt (aList.Layers()); anLayerIt.More(); anLayerIt.Next())
  {
    const OpenGl_PriorityList& aPriorityList = anLayerIt.Value();

    if (aPriorityList.NbStructures() == 0)
      continue;

    const OpenGl_ArrayOfStructure& aStructArray = aPriorityList.ArrayOfStructures();

    for (Standard_Integer anIndex = 0; anIndex < aStructArray.Length(); ++anIndex)
    {
      OpenGl_SequenceOfStructure::Iterator aStructIt;

      for (aStructIt.Init (aStructArray (anIndex)); aStructIt.More(); aStructIt.Next())
      {
        const OpenGl_Structure* aStructure = aStructIt.Value();

        if (theCheck)
        {
          if (CheckRaytraceStructure (aStructure))
          {
            return UpdateRaytraceGeometry (Standard_False);
          }
        }
        else
        {
          if (!aStructure->IsRaytracable())
            continue;

          if (aStructure->Transformation()->mat != NULL)
          {
            if (aTransform == NULL)
              aTransform = new Standard_ShortReal[16];

            for (Standard_Integer i = 0; i < 4; ++i)
              for (Standard_Integer j = 0; j < 4; ++j)
              {
                aTransform[j * 4 + i] = aStructure->Transformation()->mat[i][j];
              }
          }

          AddRaytraceStructure (aStructure, aTransform, anElements);
        }
      }
    }
  }

  if (!theCheck)
  {
    // Actualize the hash map of structures -- remove out-of-date records
    std::map<const OpenGl_Structure*, Standard_Size>::iterator anIter = myStructureStates.begin();

    while (anIter != myStructureStates.end())
    {
      if (anElements.find (anIter->first) == anElements.end())
      {
        myStructureStates.erase (anIter++);
      }
      else
      {
        ++anIter;
      }
    }

    // Actualize OpenGL layer list state
    myLayersModificationStatus = myView->LayerList().ModificationState();

    // Rebuild bottom-level and high-level BVHs
    myRaytraceGeometry.ProcessAcceleration();

    const Standard_ShortReal aMinRadius = Max (fabs (myRaytraceGeometry.Box().CornerMin().x()), Max (
      fabs (myRaytraceGeometry.Box().CornerMin().y()), fabs (myRaytraceGeometry.Box().CornerMin().z())));
    const Standard_ShortReal aMaxRadius = Max (fabs (myRaytraceGeometry.Box().CornerMax().x()), Max (
      fabs (myRaytraceGeometry.Box().CornerMax().y()), fabs (myRaytraceGeometry.Box().CornerMax().z())));

    myRaytraceSceneRadius = 2.f /* scale factor */ * Max (aMinRadius, aMaxRadius);

    myRaytraceSceneEpsilon = Max (1e-4f,
      myRaytraceGeometry.Box().Size().Length() * 1e-4f);

    return WriteRaytraceSceneToDevice();
  }

  delete [] aTransform;

  return Standard_True;
}

// =======================================================================
// function : CheckRaytraceStructure
// purpose  :  Checks to see if the structure is modified
// =======================================================================
Standard_Boolean OpenGl_Workspace::CheckRaytraceStructure (const OpenGl_Structure* theStructure)
{
  if (!theStructure->IsRaytracable())
  {
    // Checks to see if all ray-tracable elements were
    // removed from the structure
    if (theStructure->ModificationState() > 0)
    {
      theStructure->ResetModificationState();
      return Standard_True;
    }

    return Standard_False;
  }

  std::map<const OpenGl_Structure*, Standard_Size>::iterator aStructState = myStructureStates.find (theStructure);

  if (aStructState != myStructureStates.end())
    return aStructState->second != theStructure->ModificationState();

  return Standard_True;
}

// =======================================================================
// function : CreateMaterial
// purpose  : Creates ray-tracing material properties
// =======================================================================
void CreateMaterial (const OPENGL_SURF_PROP& theProp, OpenGl_RaytraceMaterial& theMaterial)
{
  const float* aSrcAmb = theProp.isphysic ? theProp.ambcol.rgb : theProp.matcol.rgb;
  theMaterial.Ambient = BVH_Vec4f (aSrcAmb[0] * theProp.amb,
                                   aSrcAmb[1] * theProp.amb,
                                   aSrcAmb[2] * theProp.amb,
                                   1.0f);

  const float* aSrcDif = theProp.isphysic ? theProp.difcol.rgb : theProp.matcol.rgb;
  theMaterial.Diffuse = BVH_Vec4f (aSrcDif[0] * theProp.diff,
                                   aSrcDif[1] * theProp.diff,
                                   aSrcDif[2] * theProp.diff,
                                   1.0f);

  const float aDefSpecCol[4] = {1.0f, 1.0f, 1.0f, 1.0f};
  const float* aSrcSpe = theProp.isphysic ? theProp.speccol.rgb : aDefSpecCol;
  theMaterial.Specular = BVH_Vec4f (aSrcSpe[0] * theProp.spec,
                                    aSrcSpe[1] * theProp.spec,
                                    aSrcSpe[2] * theProp.spec,
                                    theProp.shine);

  const float* aSrcEms = theProp.isphysic ? theProp.emscol.rgb : theProp.matcol.rgb;
  theMaterial.Emission = BVH_Vec4f (aSrcEms[0] * theProp.emsv,
                                    aSrcEms[1] * theProp.emsv,
                                    aSrcEms[2] * theProp.emsv,
                                    1.0f);

  // Note: Here we use sub-linear transparency function
  // to produce realistic-looking transparency effect
  theMaterial.Transparency = BVH_Vec4f (powf (theProp.trans, 0.75f),
                                        1.f - theProp.trans,
                                        1.f,
                                        1.f);

  const float aMaxRefl = Max (theMaterial.Diffuse.x() + theMaterial.Specular.x(),
                         Max (theMaterial.Diffuse.y() + theMaterial.Specular.y(),
                              theMaterial.Diffuse.z() + theMaterial.Specular.z()));

  const float aReflectionScale = 0.75f / aMaxRefl;

  theMaterial.Reflection = BVH_Vec4f (theProp.speccol.rgb[0] * theProp.spec,
                                      theProp.speccol.rgb[1] * theProp.spec,
                                      theProp.speccol.rgb[2] * theProp.spec,
                                      0.f) * aReflectionScale;
}

// =======================================================================
// function : AddRaytraceStructure
// purpose  : Adds OpenGL structure to ray-traced scene geometry
// =======================================================================
Standard_Boolean OpenGl_Workspace::AddRaytraceStructure (const OpenGl_Structure* theStructure,
  const Standard_ShortReal* theTransform, std::set<const OpenGl_Structure*>& theElements)
{
  theElements.insert (theStructure);

  if (!theStructure->IsVisible())
  {
    myStructureStates[theStructure] = theStructure->ModificationState();
    return Standard_True;
  }

  // Get structure material
  Standard_Integer aStructMatID = -1;

  if (theStructure->AspectFace() != NULL)
  {
    aStructMatID = static_cast<Standard_Integer> (myRaytraceGeometry.Materials.size());

    OpenGl_RaytraceMaterial aStructMaterial;
    CreateMaterial (theStructure->AspectFace()->IntFront(), aStructMaterial);

    myRaytraceGeometry.Materials.push_back (aStructMaterial);
  }

  for (OpenGl_Structure::GroupIterator aGroupIter (theStructure->DrawGroups()); aGroupIter.More(); aGroupIter.Next())
  {
    // Get group material
    Standard_Integer aGroupMatID = -1;
    if (aGroupIter.Value()->AspectFace() != NULL)
    {
      aGroupMatID = static_cast<Standard_Integer> (myRaytraceGeometry.Materials.size());

      OpenGl_RaytraceMaterial aGroupMaterial;
      CreateMaterial (aGroupIter.Value()->AspectFace()->IntFront(), aGroupMaterial);

      myRaytraceGeometry.Materials.push_back (aGroupMaterial);
    }

    Standard_Integer aMatID = aGroupMatID < 0 ? aStructMatID : aGroupMatID;

    if (aMatID < 0)
    {
      aMatID = static_cast<Standard_Integer> (myRaytraceGeometry.Materials.size());

      myRaytraceGeometry.Materials.push_back (OpenGl_RaytraceMaterial());
    }

    // Add OpenGL elements from group (extract primitives arrays and aspects)
    for (const OpenGl_ElementNode* aNode = aGroupIter.Value()->FirstNode(); aNode != NULL; aNode = aNode->next)
    {
      OpenGl_AspectFace* anAspect = dynamic_cast<OpenGl_AspectFace*> (aNode->elem);
      if (anAspect != NULL)
      {
        aMatID = static_cast<Standard_Integer> (myRaytraceGeometry.Materials.size());

        OpenGl_RaytraceMaterial aMaterial;
        CreateMaterial (anAspect->IntFront(), aMaterial);

        myRaytraceGeometry.Materials.push_back (aMaterial);
      }
      else
      {
        OpenGl_PrimitiveArray* aPrimArray = dynamic_cast<OpenGl_PrimitiveArray*> (aNode->elem);
        if (aPrimArray != NULL)
        {
          NCollection_Handle<BVH_Object<Standard_ShortReal, 4> > aSet =
            AddRaytracePrimitiveArray (aPrimArray->PArray(), aMatID, theTransform);

          if (!aSet.IsNull())
            myRaytraceGeometry.Objects().Append (aSet);
        }
      }
    }
  }

  Standard_ShortReal* aTransform (NULL);

  // Process all connected OpenGL structures
  for (OpenGl_ListOfStructure::Iterator anIts (theStructure->ConnectedStructures()); anIts.More(); anIts.Next())
  {
    if (anIts.Value()->Transformation()->mat != NULL)
    {
      Standard_ShortReal* aTransform = new Standard_ShortReal[16];

      for (Standard_Integer i = 0; i < 4; ++i)
        for (Standard_Integer j = 0; j < 4; ++j)
        {
          aTransform[j * 4 + i] =
            anIts.Value()->Transformation()->mat[i][j];
        }
    }

    if (anIts.Value()->IsRaytracable())
      AddRaytraceStructure (anIts.Value(), aTransform != NULL ? aTransform : theTransform, theElements);
  }

  delete[] aTransform;

  myStructureStates[theStructure] = theStructure->ModificationState();

  return Standard_True;
}

// =======================================================================
// function : AddRaytracePrimitiveArray
// purpose  : Adds OpenGL primitive array to ray-traced scene geometry
// =======================================================================
OpenGl_TriangleSet* OpenGl_Workspace::AddRaytracePrimitiveArray (
  const CALL_DEF_PARRAY* theArray, Standard_Integer theMatID, const Standard_ShortReal* theTransform)
{
  if (theArray->type != TelPolygonsArrayType &&
      theArray->type != TelTrianglesArrayType &&
      theArray->type != TelQuadranglesArrayType &&
      theArray->type != TelTriangleFansArrayType &&
      theArray->type != TelTriangleStripsArrayType &&
      theArray->type != TelQuadrangleStripsArrayType)
  {
    return NULL;
  }

  if (theArray->vertices == NULL)
    return NULL;

#ifdef RAY_TRACE_PRINT_INFO
  switch (theArray->type)
  {
    case TelPolygonsArrayType:
      std::cout << "\tAdding TelPolygonsArrayType" << std::endl; break;
    case TelTrianglesArrayType:
      std::cout << "\tAdding TelTrianglesArrayType" << std::endl; break;
    case TelQuadranglesArrayType:
      std::cout << "\tAdding TelQuadranglesArrayType" << std::endl; break;
    case TelTriangleFansArrayType:
      std::cout << "\tAdding TelTriangleFansArrayType" << std::endl; break;
    case TelTriangleStripsArrayType:
      std::cout << "\tAdding TelTriangleStripsArrayType" << std::endl; break;
    case TelQuadrangleStripsArrayType:
      std::cout << "\tAdding TelQuadrangleStripsArrayType" << std::endl; break;
  }
#endif

  OpenGl_TriangleSet* aSet = new OpenGl_TriangleSet;

  {
    aSet->Vertices.reserve (theArray->num_vertexs);

    for (Standard_Integer aVert = 0; aVert < theArray->num_vertexs; ++aVert)
    {
      BVH_Vec4f aVertex (theArray->vertices[aVert].xyz[0],
                         theArray->vertices[aVert].xyz[1],
                         theArray->vertices[aVert].xyz[2],
                         1.f);
      if (theTransform)
        aVertex = MatVecMult (theTransform, aVertex);

      aSet->Vertices.push_back (aVertex);
    }

    aSet->Normals.reserve (theArray->num_vertexs);

    for (Standard_Integer aNorm = 0; aNorm < theArray->num_vertexs; ++aNorm)
    {
      BVH_Vec4f aNormal;

      // Note: In case of absence of normals, the
      // renderer uses generated geometric normals

      if (theArray->vnormals != NULL)
      {
        aNormal = BVH_Vec4f (theArray->vnormals[aNorm].xyz[0],
                             theArray->vnormals[aNorm].xyz[1],
                             theArray->vnormals[aNorm].xyz[2],
                             0.f);

        if (theTransform)
          aNormal = MatVecMult (theTransform, aNormal);
      }

      aSet->Normals.push_back (aNormal);
    }

    if (theArray->num_bounds > 0)
    {
  #ifdef RAY_TRACE_PRINT_INFO
      std::cout << "\tNumber of bounds = " << theArray->num_bounds << std::endl;
  #endif

      Standard_Integer aBoundStart = 0;

      for (Standard_Integer aBound = 0; aBound < theArray->num_bounds; ++aBound)
      {
        const Standard_Integer aVertNum = theArray->bounds[aBound];

  #ifdef RAY_TRACE_PRINT_INFO
        std::cout << "\tAdding indices from bound " << aBound << ": " <<
                                      aBoundStart << " .. " << aVertNum << std::endl;
  #endif

        if (!AddRaytraceVertexIndices (aSet, theArray, aBoundStart, aVertNum, theMatID))
        {
          delete aSet;
          return NULL;
        }

        aBoundStart += aVertNum;
      }
    }
    else
    {
      const Standard_Integer aVertNum = theArray->num_edges > 0 ? theArray->num_edges : theArray->num_vertexs;

  #ifdef RAY_TRACE_PRINT_INFO
        std::cout << "\tAdding indices from array: " << aVertNum << std::endl;
  #endif

      if (!AddRaytraceVertexIndices (aSet, theArray, 0, aVertNum, theMatID))
      {
        delete aSet;
        return NULL;
      }
    }
  }

  if (aSet->Size() != 0)
    aSet->MarkDirty();

  return aSet;
}

// =======================================================================
// function : AddRaytraceVertexIndices
// purpose  : Adds vertex indices to ray-traced scene geometry
// =======================================================================
Standard_Boolean OpenGl_Workspace::AddRaytraceVertexIndices (OpenGl_TriangleSet* theSet,
  const CALL_DEF_PARRAY* theArray, Standard_Integer theOffset, Standard_Integer theCount, Standard_Integer theMatID)
{
  switch (theArray->type)
  {
    case TelTrianglesArrayType:
      return AddRaytraceTriangleArray (theSet, theArray, theOffset, theCount, theMatID);

    case TelQuadranglesArrayType:
      return AddRaytraceQuadrangleArray (theSet, theArray, theOffset, theCount, theMatID);

    case TelTriangleFansArrayType:
      return AddRaytraceTriangleFanArray (theSet, theArray, theOffset, theCount, theMatID);

    case TelTriangleStripsArrayType:
      return AddRaytraceTriangleStripArray (theSet, theArray, theOffset, theCount, theMatID);

    case TelQuadrangleStripsArrayType:
      return AddRaytraceQuadrangleStripArray (theSet, theArray, theOffset, theCount, theMatID);

    default:
      return AddRaytracePolygonArray (theSet, theArray, theOffset, theCount, theMatID);
  }
}

// =======================================================================
// function : AddRaytraceTriangleArray
// purpose  : Adds OpenGL triangle array to ray-traced scene geometry
// =======================================================================
Standard_Boolean OpenGl_Workspace::AddRaytraceTriangleArray (OpenGl_TriangleSet* theSet,
  const CALL_DEF_PARRAY* theArray, Standard_Integer theOffset, Standard_Integer theCount, Standard_Integer theMatID)
{
  if (theCount < 3)
    return Standard_True;

  theSet->Elements.reserve (theSet->Elements.size() + theCount / 3);

  if (theArray->num_edges > 0)
  {
    for (Standard_Integer aVert = theOffset; aVert < theOffset + theCount - 2; aVert += 3)
    {
      theSet->Elements.push_back (BVH_Vec4i (theArray->edges[aVert + 0],
                                             theArray->edges[aVert + 1],
                                             theArray->edges[aVert + 2],
                                             theMatID));
    }
  }
  else
  {
    for (Standard_Integer aVert = theOffset; aVert < theOffset + theCount - 2; aVert += 3)
    {
      theSet->Elements.push_back (BVH_Vec4i (aVert + 0,
                                             aVert + 1,
                                             aVert + 2,
                                             theMatID));
    }
  }

  return Standard_True;
}

// =======================================================================
// function : AddRaytraceTriangleFanArray
// purpose  : Adds OpenGL triangle fan array to ray-traced scene geometry
// =======================================================================
Standard_Boolean OpenGl_Workspace::AddRaytraceTriangleFanArray (OpenGl_TriangleSet* theSet,
  const CALL_DEF_PARRAY* theArray, Standard_Integer theOffset, Standard_Integer theCount, Standard_Integer theMatID)
{
  if (theCount < 3)
    return Standard_True;

  theSet->Elements.reserve (theSet->Elements.size() + theCount - 2);

  if (theArray->num_edges > 0)
  {
    for (Standard_Integer aVert = theOffset; aVert < theOffset + theCount - 2; ++aVert)
    {
      theSet->Elements.push_back (BVH_Vec4i (theArray->edges[theOffset],
                                             theArray->edges[aVert + 1],
                                             theArray->edges[aVert + 2],
                                             theMatID));
    }
  }
  else
  {
    for (Standard_Integer aVert = theOffset; aVert < theOffset + theCount - 2; ++aVert)
    {
      theSet->Elements.push_back (BVH_Vec4i (theOffset,
                                             aVert + 1,
                                             aVert + 2,
                                             theMatID));
    }
  }

  return Standard_True;
}

// =======================================================================
// function : AddRaytraceTriangleStripArray
// purpose  : Adds OpenGL triangle strip array to ray-traced scene geometry
// =======================================================================
Standard_Boolean OpenGl_Workspace::AddRaytraceTriangleStripArray (OpenGl_TriangleSet* theSet,
  const CALL_DEF_PARRAY* theArray, Standard_Integer theOffset, Standard_Integer theCount, Standard_Integer theMatID)
{
  if (theCount < 3)
    return Standard_True;

  theSet->Elements.reserve (theSet->Elements.size() + theCount - 2);

  if (theArray->num_edges > 0)
  {
    for (Standard_Integer aVert = theOffset, aCW = 0; aVert < theOffset + theCount - 2; ++aVert, aCW = (aCW + 1) % 2)
    {
      theSet->Elements.push_back (BVH_Vec4i (theArray->edges[aVert + aCW ? 1 : 0],
                                             theArray->edges[aVert + aCW ? 0 : 1],
                                             theArray->edges[aVert + 2],
                                             theMatID));
    }
  }
  else
  {
    for (Standard_Integer aVert = theOffset, aCW = 0; aVert < theOffset + theCount - 2; ++aVert, aCW = (aCW + 1) % 2)
    {
      theSet->Elements.push_back (BVH_Vec4i (aVert + aCW ? 1 : 0,
                                             aVert + aCW ? 0 : 1,
                                             aVert + 2,
                                             theMatID));
    }
  }

  return Standard_True;
}

// =======================================================================
// function : AddRaytraceQuadrangleArray
// purpose  : Adds OpenGL quad array to ray-traced scene geometry
// =======================================================================
Standard_Boolean OpenGl_Workspace::AddRaytraceQuadrangleArray (OpenGl_TriangleSet* theSet,
  const CALL_DEF_PARRAY* theArray, Standard_Integer theOffset, Standard_Integer theCount, Standard_Integer theMatID)
{
  if (theCount < 4)
    return Standard_True;

  theSet->Elements.reserve (theSet->Elements.size() + theCount / 2);

  if (theArray->num_edges > 0)
  {
    for (Standard_Integer aVert = theOffset; aVert < theOffset + theCount - 3; aVert += 4)
    {
      theSet->Elements.push_back (BVH_Vec4i (theArray->edges[aVert + 0],
                                             theArray->edges[aVert + 1],
                                             theArray->edges[aVert + 2],
                                             theMatID));

      theSet->Elements.push_back (BVH_Vec4i (theArray->edges[aVert + 0],
                                             theArray->edges[aVert + 2],
                                             theArray->edges[aVert + 3],
                                             theMatID));
    }
  }
  else
  {
    for (Standard_Integer aVert = theOffset; aVert < theOffset + theCount - 3; aVert += 4)
    {
      theSet->Elements.push_back (BVH_Vec4i (aVert + 0,
                                             aVert + 1,
                                             aVert + 2,
                                             theMatID));

      theSet->Elements.push_back (BVH_Vec4i (aVert + 0,
                                             aVert + 2,
                                             aVert + 3,
                                             theMatID));
    }
  }

  return Standard_True;
}

// =======================================================================
// function : AddRaytraceQuadrangleStripArray
// purpose  : Adds OpenGL quad strip array to ray-traced scene geometry
// =======================================================================
Standard_Boolean OpenGl_Workspace::AddRaytraceQuadrangleStripArray (OpenGl_TriangleSet* theSet,
  const CALL_DEF_PARRAY* theArray, Standard_Integer theOffset, Standard_Integer theCount, Standard_Integer theMatID)
{
  if (theCount < 4)
    return Standard_True;

  theSet->Elements.reserve (theSet->Elements.size() + 2 * theCount - 6);

  if (theArray->num_edges > 0)
  {
    for (Standard_Integer aVert = theOffset; aVert < theOffset + theCount - 3; aVert += 2)
    {
      theSet->Elements.push_back (BVH_Vec4i (theArray->edges[aVert + 0],
                                             theArray->edges[aVert + 1],
                                             theArray->edges[aVert + 2],
                                             theMatID));

      theSet->Elements.push_back (BVH_Vec4i (theArray->edges[aVert + 1],
                                             theArray->edges[aVert + 3],
                                             theArray->edges[aVert + 2],
                                             theMatID));
    }
  }
  else
  {
    for (Standard_Integer aVert = theOffset; aVert < theOffset + theCount - 3; aVert += 2)
    {
      theSet->Elements.push_back (BVH_Vec4i (aVert + 0,
                                             aVert + 1,
                                             aVert + 2,
                                             theMatID));

      theSet->Elements.push_back (BVH_Vec4i (aVert + 1,
                                             aVert + 3,
                                             aVert + 2,
                                             theMatID));
    }
  }

  return Standard_True;
}

// =======================================================================
// function : AddRaytracePolygonArray
// purpose  : Adds OpenGL polygon array to ray-traced scene geometry
// =======================================================================
Standard_Boolean OpenGl_Workspace::AddRaytracePolygonArray (OpenGl_TriangleSet* theSet,
  const CALL_DEF_PARRAY* theArray, Standard_Integer theOffset, Standard_Integer theCount, Standard_Integer theMatID)
{
  if (theCount < 3)
    return Standard_True;

  theSet->Elements.reserve (theSet->Elements.size() + theCount - 2);

  if (theArray->num_edges > 0)
  {
    for (Standard_Integer aVert = theOffset; aVert < theOffset + theCount - 2; ++aVert)
    {
      theSet->Elements.push_back (BVH_Vec4i (theArray->edges[theOffset],
                                             theArray->edges[aVert + 1],
                                             theArray->edges[aVert + 2],
                                             theMatID));
    }
  }
  else
  {
    for (Standard_Integer aVert = theOffset; aVert < theOffset + theCount - 2; ++aVert)
    {
      theSet->Elements.push_back (BVH_Vec4i (theOffset,
                                             aVert + 1,
                                             aVert + 2,
                                             theMatID));
    }
  }

  return Standard_True;
}

// =======================================================================
// function : UpdateRaytraceLightSources
// purpose  : Updates 3D scene light sources for ray-tracing
// =======================================================================
Standard_Boolean OpenGl_Workspace::UpdateRaytraceLightSources (const GLdouble theInvModelView[16])
{
  myRaytraceGeometry.Sources.clear();

  myRaytraceGeometry.GlobalAmbient = BVH_Vec4f (0.0f, 0.0f, 0.0f, 0.0f);

  for (OpenGl_ListOfLight::Iterator anItl (myView->LightList()); anItl.More(); anItl.Next())
  {
    const OpenGl_Light& aLight = anItl.Value();

    if (aLight.Type == Visual3d_TOLS_AMBIENT)
    {
      myRaytraceGeometry.GlobalAmbient += BVH_Vec4f (aLight.Color.r(),
                                                     aLight.Color.g(),
                                                     aLight.Color.b(),
                                                     0.0f);
      continue;
    }

    BVH_Vec4f aDiffuse  (aLight.Color.r(),
                         aLight.Color.g(),
                         aLight.Color.b(),
                         1.0f);

    BVH_Vec4f aPosition (-aLight.Direction.x(),
                         -aLight.Direction.y(),
                         -aLight.Direction.z(),
                         0.0f);

    if (aLight.Type != Visual3d_TOLS_DIRECTIONAL)
    {
      aPosition = BVH_Vec4f (aLight.Position.x(),
                             aLight.Position.y(),
                             aLight.Position.z(),
                             1.0f);
    }

    if (aLight.IsHeadlight)
      aPosition = MatVecMult (theInvModelView, aPosition);

    myRaytraceGeometry.Sources.push_back (OpenGl_RaytraceLight (aDiffuse, aPosition));
  }

  cl_int anError = CL_SUCCESS;

  if (myRaytraceLightSourceBuffer != NULL)
    clReleaseMemObject (myRaytraceLightSourceBuffer);

  Standard_Integer aLightBufferSize = myRaytraceGeometry.Sources.size() != 0 ?
    static_cast<Standard_Integer> (myRaytraceGeometry.Sources.size()) : 1;

  myRaytraceLightSourceBuffer = clCreateBuffer (myComputeContext, CL_MEM_READ_ONLY,
    aLightBufferSize * sizeof(OpenGl_RaytraceLight), NULL, &anError);

  if (myRaytraceGeometry.Sources.size() != 0)
  {
    const void* aDataPtr = myRaytraceGeometry.Sources.front().Packed();

    anError |= clEnqueueWriteBuffer (myComputeQueue, myRaytraceLightSourceBuffer, CL_TRUE, 0,
      aLightBufferSize * sizeof(OpenGl_RaytraceLight), aDataPtr, 0, NULL, NULL);
  }

#ifdef RAY_TRACE_PRINT_INFO
  if (anError != CL_SUCCESS)
  {
    std::cout << "Error! Failed to set light sources";

    return Standard_False;
  }
#endif

  return Standard_True;
}

// =======================================================================
// function : CheckOpenCL
// purpose  : Checks OpenCL dynamic library availability
// =======================================================================
Standard_Boolean CheckOpenCL()
{
#if defined ( _WIN32 )

  __try
  {
    cl_uint aNbPlatforms;
    clGetPlatformIDs (0, NULL, &aNbPlatforms);
  }
  __except (EXCEPTION_EXECUTE_HANDLER)
  {
    return Standard_False;
  }

#endif

  return Standard_True;
}

// =======================================================================
// function : InitOpenCL
// purpose  : Initializes OpenCL objects
// =======================================================================
Standard_Boolean OpenGl_Workspace::InitOpenCL()
{
  if (myComputeInitStatus != OpenGl_CLIS_NONE)
  {
    return myComputeInitStatus == OpenGl_CLIS_INIT;
  }

  if (!CheckOpenCL())
  {
    myComputeInitStatus = OpenGl_CLIS_FAIL; // fail to load OpenCL library
    myGlContext->PushMessage (GL_DEBUG_SOURCE_APPLICATION_ARB,
                              GL_DEBUG_TYPE_ERROR_ARB,
                              0,
                              GL_DEBUG_SEVERITY_HIGH_ARB,
                              "Failed to load OpenCL dynamic library!");
    return Standard_False;
  }

  // Obtain the list of platforms available
  cl_uint aNbPlatforms = 0;
  cl_int  anError = clGetPlatformIDs (0, NULL, &aNbPlatforms);
  cl_platform_id* aPlatforms = (cl_platform_id* )alloca (aNbPlatforms * sizeof(cl_platform_id));
  anError |= clGetPlatformIDs (aNbPlatforms, aPlatforms, NULL);
  if (anError != CL_SUCCESS
   || aNbPlatforms == 0)
  {
    myComputeInitStatus = OpenGl_CLIS_FAIL;
    myGlContext->PushMessage (GL_DEBUG_SOURCE_APPLICATION_ARB,
                              GL_DEBUG_TYPE_ERROR_ARB,
                              0,
                              GL_DEBUG_SEVERITY_HIGH_ARB,
                              "No any OpenCL platform installed!");
    return Standard_False;
  }

  // Note: We try to find NVIDIA or AMD platforms with GPU devices!
  cl_platform_id aPrefPlatform = NULL;
  for (cl_uint aPlatIter = 0; aPlatIter < aNbPlatforms; ++aPlatIter)
  {
    char aName[256];
    anError = clGetPlatformInfo (aPlatforms[aPlatIter], CL_PLATFORM_NAME,
                                 sizeof(aName), aName, NULL);
    if (anError != CL_SUCCESS)
    {
      continue;
    }

    if (strncmp (aName, "NVIDIA", strlen ("NVIDIA")) == 0)
    {
      aPrefPlatform = aPlatforms[aPlatIter];

      // Use optimizations for NVIDIA GPUs
      myIsAmdComputePlatform = Standard_False;
    }
    else if (strncmp (aName, "AMD", strlen ("AMD")) == 0)
    {
      aPrefPlatform = (aPrefPlatform == NULL)
                    ? aPlatforms[aPlatIter]
                    : aPrefPlatform;

      // Use optimizations for ATI/AMD platform
      myIsAmdComputePlatform = Standard_True;
    }
  }

  if (aPrefPlatform == NULL)
  {
    aPrefPlatform = aPlatforms[0];
  }

  // Obtain the list of devices available in the selected platform
  cl_uint aNbDevices = 0;
  anError = clGetDeviceIDs (aPrefPlatform, CL_DEVICE_TYPE_GPU,
                            0, NULL, &aNbDevices);

  cl_device_id* aDevices = (cl_device_id* )alloca (aNbDevices * sizeof(cl_device_id));
  anError |= clGetDeviceIDs (aPrefPlatform, CL_DEVICE_TYPE_GPU,
                             aNbDevices, aDevices, NULL);
  if (anError != CL_SUCCESS)
  {
    myComputeInitStatus = OpenGl_CLIS_FAIL;
    myGlContext->PushMessage (GL_DEBUG_SOURCE_APPLICATION_ARB,
                              GL_DEBUG_TYPE_ERROR_ARB,
                              0,
                              GL_DEBUG_SEVERITY_HIGH_ARB,
                              "Failed to get OpenCL GPU device!");
    return Standard_False;
  }

  // Note: Simply get first available GPU
  cl_device_id aDevice = aDevices[0];

  // detect old contexts
  char aVerClStr[256];
  clGetDeviceInfo (aDevice, CL_DEVICE_VERSION,
                   sizeof(aVerClStr), aVerClStr, NULL);
  aVerClStr[strlen ("OpenCL 1.0")] = '\0';
  const bool isVer10 = strncmp (aVerClStr, "OpenCL 1.0", strlen ("OpenCL 1.0")) == 0;

  // Create OpenCL context
  cl_context_properties aCtxProp[] =
  {
  #if defined(__APPLE__) && !defined(MACOSX_USE_GLX)
    CL_CONTEXT_PROPERTY_USE_CGL_SHAREGROUP_APPLE,
    (cl_context_properties )CGLGetShareGroup (CGLGetCurrentContext()),
  #elif defined(_WIN32)
    CL_CONTEXT_PLATFORM, (cl_context_properties )aPrefPlatform,
    CL_GL_CONTEXT_KHR,   (cl_context_properties )wglGetCurrentContext(),
    CL_WGL_HDC_KHR,      (cl_context_properties )wglGetCurrentDC(),
  #else
    CL_GL_CONTEXT_KHR,   (cl_context_properties )glXGetCurrentContext(),
    CL_GLX_DISPLAY_KHR,  (cl_context_properties )glXGetCurrentDisplay(),
    CL_CONTEXT_PLATFORM, (cl_context_properties )aPrefPlatform,
  #endif
    0
  };

  myComputeContext = clCreateContext (aCtxProp,
                                    #if defined(__APPLE__) && !defined(MACOSX_USE_GLX)
                                      0, NULL, // device will be taken from GL context
                                    #else
                                      1, &aDevice,
                                    #endif
                                      NULL, NULL, &anError);
  if (anError != CL_SUCCESS)
  {
    myComputeInitStatus = OpenGl_CLIS_FAIL;
    myGlContext->PushMessage (GL_DEBUG_SOURCE_APPLICATION_ARB,
                              GL_DEBUG_TYPE_ERROR_ARB,
                              0,
                              GL_DEBUG_SEVERITY_HIGH_ARB,
                              "Failed to initialize OpenCL context!");
    return Standard_False;
  }

  // Create OpenCL program
  const char* aSources[] =
  {
    isVer10 ? "#define M_PI_F ( float )( 3.14159265359f )\n" : "",
    THE_RAY_TRACE_OPENCL_SOURCE
  };
  myRaytraceProgram = clCreateProgramWithSource (myComputeContext, 2,
                                                 aSources, NULL, &anError);
  if (anError != CL_SUCCESS)
  {
    myComputeInitStatus = OpenGl_CLIS_FAIL;
    myGlContext->PushMessage (GL_DEBUG_SOURCE_APPLICATION_ARB,
                              GL_DEBUG_TYPE_ERROR_ARB,
                              0,
                              GL_DEBUG_SEVERITY_HIGH_ARB,
                              "Failed to create OpenCL ray-tracing program!");
    return Standard_False;
  }

  anError = clBuildProgram (myRaytraceProgram, 0,
                            NULL, NULL, NULL, NULL);
  {
    // Fetch build log
    size_t aLogLen = 0;
    cl_int aResult = clGetProgramBuildInfo (myRaytraceProgram, aDevice,
                                            CL_PROGRAM_BUILD_LOG, 0, NULL, &aLogLen);

    char* aBuildLog = (char* )alloca (aLogLen);
    aResult |= clGetProgramBuildInfo (myRaytraceProgram, aDevice,
                                      CL_PROGRAM_BUILD_LOG, aLogLen, aBuildLog, NULL);
    if (aResult == CL_SUCCESS)
    {
      if (anError != CL_SUCCESS)
      {
        myGlContext->PushMessage (GL_DEBUG_SOURCE_APPLICATION_ARB,
                                  GL_DEBUG_TYPE_ERROR_ARB,
                                  0,
                                  GL_DEBUG_SEVERITY_HIGH_ARB,
                                  aBuildLog);
      }
      else
      {
      #ifdef RAY_TRACE_PRINT_INFO
        std::cout << aBuildLog << std::endl;
      #endif
      }
    }
  }

  if (anError != CL_SUCCESS)
  {
    return Standard_False;
  }

  // Create OpenCL ray tracing kernels
  myRaytraceRenderKernel = clCreateKernel (myRaytraceProgram, "RaytraceRender", &anError);
  if (anError != CL_SUCCESS)
  {
    myComputeInitStatus = OpenGl_CLIS_FAIL;
    myGlContext->PushMessage (GL_DEBUG_SOURCE_APPLICATION_ARB,
                              GL_DEBUG_TYPE_ERROR_ARB,
                              0,
                              GL_DEBUG_SEVERITY_HIGH_ARB,
                              "Failed to create OpenCL ray-tracing kernel!");
    return Standard_False;
  }

  myRaytraceSmoothKernel = clCreateKernel (myRaytraceProgram, "RaytraceSmooth", &anError);
  if (anError != CL_SUCCESS)
  {
    myComputeInitStatus = OpenGl_CLIS_FAIL;
    myGlContext->PushMessage (GL_DEBUG_SOURCE_APPLICATION_ARB,
                              GL_DEBUG_TYPE_ERROR_ARB,
                              0,
                              GL_DEBUG_SEVERITY_HIGH_ARB,
                              "Failed to create OpenCL ray-tracing kernel!");
    return Standard_False;
  }

  // Create OpenCL command queue
  // Note: For profiling set CL_QUEUE_PROFILING_ENABLE
  cl_command_queue_properties aProps = CL_QUEUE_PROFILING_ENABLE;

  myComputeQueue = clCreateCommandQueue (myComputeContext, aDevice, aProps, &anError);
  if (anError != CL_SUCCESS)
  {
    myComputeInitStatus = OpenGl_CLIS_FAIL;
    myGlContext->PushMessage (GL_DEBUG_SOURCE_APPLICATION_ARB,
                              GL_DEBUG_TYPE_ERROR_ARB,
                              0,
                              GL_DEBUG_SEVERITY_HIGH_ARB,
                              "Failed to create OpenCL command queue!");

    return Standard_False;
  }

  myComputeInitStatus = OpenGl_CLIS_INIT; // initialized in normal way
  return Standard_True;
}

// =======================================================================
// function : GetOpenClDeviceInfo
// purpose  : Returns information about device used for computations
// =======================================================================
Standard_Boolean OpenGl_Workspace::GetOpenClDeviceInfo (NCollection_DataMap<TCollection_AsciiString,
                                                                            TCollection_AsciiString>& theInfo) const
{
  theInfo.Clear();
  if (myComputeContext == NULL)
  {
    return Standard_False;
  }

  size_t aDevicesSize = 0;
  cl_int anError = clGetContextInfo (myComputeContext, CL_CONTEXT_DEVICES, 0, NULL, &aDevicesSize);
  cl_device_id* aDevices = (cl_device_id* )alloca (aDevicesSize);
  anError |= clGetContextInfo (myComputeContext, CL_CONTEXT_DEVICES, aDevicesSize, aDevices, NULL);
  if (anError != CL_SUCCESS)
  {
    return Standard_False;
  }

  char aDeviceName[256];
  anError |= clGetDeviceInfo (aDevices[0], CL_DEVICE_NAME, sizeof(aDeviceName), aDeviceName, NULL);
  theInfo.Bind ("Name", aDeviceName);

  char aDeviceVendor[256];
  anError |= clGetDeviceInfo (aDevices[0], CL_DEVICE_VENDOR, sizeof(aDeviceVendor), aDeviceVendor, NULL);
  theInfo.Bind ("Vendor", aDeviceVendor);

  cl_device_type aDeviceType;
  anError |= clGetDeviceInfo (aDevices[0], CL_DEVICE_TYPE, sizeof(aDeviceType), &aDeviceType, NULL);
  theInfo.Bind ("Type", aDeviceType == CL_DEVICE_TYPE_GPU ? "GPU" : "CPU");
  return Standard_True;
}

// =======================================================================
// function : ReleaseOpenCL
// purpose  : Releases resources of OpenCL objects
// =======================================================================
void OpenGl_Workspace::ReleaseOpenCL()
{
  clReleaseKernel (myRaytraceRenderKernel);
  clReleaseKernel (myRaytraceSmoothKernel);

  clReleaseProgram (myRaytraceProgram);
  clReleaseCommandQueue (myComputeQueue);

  clReleaseMemObject (myRaytraceOutputImage);
  clReleaseMemObject (myRaytraceEnvironment);
  clReleaseMemObject (myRaytraceOutputImageAA);

  clReleaseMemObject (myRaytraceMaterialBuffer);
  clReleaseMemObject (myRaytraceLightSourceBuffer);

  clReleaseMemObject (mySceneNodeInfoBuffer);
  clReleaseMemObject (mySceneMinPointBuffer);
  clReleaseMemObject (mySceneMaxPointBuffer);

  clReleaseMemObject (myObjectNodeInfoBuffer);
  clReleaseMemObject (myObjectMinPointBuffer);
  clReleaseMemObject (myObjectMaxPointBuffer);

  clReleaseMemObject (myGeometryVertexBuffer);
  clReleaseMemObject (myGeometryNormalBuffer);
  clReleaseMemObject (myGeometryTriangBuffer);

  clReleaseContext (myComputeContext);

  if (!myGlContext.IsNull())
  {
    if (!myRaytraceOutputTexture.IsNull())
      myGlContext->DelayedRelease (myRaytraceOutputTexture);
    myRaytraceOutputTexture.Nullify();

    if (!myRaytraceOutputTextureAA.IsNull())
      myGlContext->DelayedRelease (myRaytraceOutputTextureAA);
    myRaytraceOutputTextureAA.Nullify();
  }
}

// =======================================================================
// function : ResizeRaytraceOutputBuffer
// purpose  : Resizes OpenCL output image
// =======================================================================
Standard_Boolean OpenGl_Workspace::ResizeRaytraceOutputBuffer (const cl_int theSizeX,
                                                               const cl_int theSizeY)
{
  if (myComputeContext == NULL)
  {
    return Standard_False;
  }

  if (!myRaytraceOutputTexture.IsNull())
  {
    Standard_Boolean toResize = myRaytraceOutputTexture->SizeX() != theSizeX ||
                                myRaytraceOutputTexture->SizeY() != theSizeY;

    if (!toResize)
      return Standard_True;

    if (!myGlContext.IsNull())
    {
      if (!myRaytraceOutputTexture.IsNull())
        myGlContext->DelayedRelease (myRaytraceOutputTexture);
      if (!myRaytraceOutputTextureAA.IsNull())
        myGlContext->DelayedRelease (myRaytraceOutputTextureAA);
    }
  }

  myRaytraceOutputTexture = new OpenGl_Texture();

  myRaytraceOutputTexture->Create (myGlContext);
  myRaytraceOutputTexture->InitRectangle (myGlContext,
    theSizeX, theSizeY, OpenGl_TextureFormat::Create<GLfloat, 4>());

  myRaytraceOutputTextureAA = new OpenGl_Texture();

  myRaytraceOutputTextureAA->Create (myGlContext);
  myRaytraceOutputTextureAA->InitRectangle (myGlContext,
    theSizeX, theSizeY, OpenGl_TextureFormat::Create<GLfloat, 4>());

  if (myRaytraceOutputImage != NULL)
    clReleaseMemObject (myRaytraceOutputImage);

  if (myRaytraceOutputImageAA != NULL)
    clReleaseMemObject (myRaytraceOutputImageAA);

  cl_int anError = CL_SUCCESS;

  myRaytraceOutputImage = clCreateFromGLTexture2D (myComputeContext,
    CL_MEM_READ_WRITE, GL_TEXTURE_RECTANGLE, 0, myRaytraceOutputTexture->TextureId(), &anError);

  if (anError != CL_SUCCESS)
  {
#ifdef RAY_TRACE_PRINT_INFO
    std::cout << "Error! Failed to create output image!" << std::endl;
#endif
    return Standard_False;
  }

  myRaytraceOutputImageAA = clCreateFromGLTexture2D (myComputeContext,
    CL_MEM_READ_WRITE, GL_TEXTURE_RECTANGLE, 0, myRaytraceOutputTextureAA->TextureId(), &anError);

  if (anError != CL_SUCCESS)
  {
#ifdef RAY_TRACE_PRINT_INFO
    std::cout << "Error! Failed to create anti-aliased output image!" << std::endl;
#endif
    return Standard_False;
  }

  return Standard_True;
}

// =======================================================================
// function : WriteRaytraceSceneToDevice
// purpose  : Writes scene geometry to OpenCL device
// =======================================================================
Standard_Boolean OpenGl_Workspace::WriteRaytraceSceneToDevice()
{
  if (myComputeContext == NULL)
    return Standard_False;

  cl_int anErrorRes = CL_SUCCESS;

  if (mySceneNodeInfoBuffer != NULL)
    anErrorRes |= clReleaseMemObject (mySceneNodeInfoBuffer);

  if (mySceneMinPointBuffer != NULL)
    anErrorRes |= clReleaseMemObject (mySceneMinPointBuffer);

  if (mySceneMaxPointBuffer != NULL)
    anErrorRes |= clReleaseMemObject (mySceneMaxPointBuffer);

  if (myObjectNodeInfoBuffer != NULL)
    anErrorRes |= clReleaseMemObject (myObjectNodeInfoBuffer);

  if (myObjectMinPointBuffer != NULL)
    anErrorRes |= clReleaseMemObject (myObjectMinPointBuffer);

  if (myObjectMaxPointBuffer != NULL)
    anErrorRes |= clReleaseMemObject (myObjectMaxPointBuffer);

  if (myGeometryVertexBuffer != NULL)
    anErrorRes |= clReleaseMemObject (myGeometryVertexBuffer);

  if (myGeometryNormalBuffer != NULL)
    anErrorRes |= clReleaseMemObject (myGeometryNormalBuffer);

  if (myGeometryTriangBuffer != NULL)
    anErrorRes |= clReleaseMemObject (myGeometryTriangBuffer);

  if (myRaytraceMaterialBuffer != NULL)
    anErrorRes |= clReleaseMemObject (myRaytraceMaterialBuffer);

  if (anErrorRes != CL_SUCCESS)
  {
#ifdef RAY_TRACE_PRINT_INFO
    std::cout << "Error! Failed to release OpenCL buffers" << std::endl;
#endif
    return Standard_False;
  }

  /////////////////////////////////////////////////////////////////////////////
  // Create material buffer

  const size_t aMaterialBufferSize =
    myRaytraceGeometry.Materials.size() != 0 ? myRaytraceGeometry.Materials.size() : 1;

  myRaytraceMaterialBuffer = clCreateBuffer (myComputeContext,
    CL_MEM_READ_ONLY, aMaterialBufferSize * sizeof(OpenGl_RaytraceMaterial), NULL, &anErrorRes);

  if (anErrorRes != CL_SUCCESS)
  {
#ifdef RAY_TRACE_PRINT_INFO
    std::cout << "Error! Failed to create OpenCL material buffer" << std::endl;
#endif
    return Standard_False;
  }

  /////////////////////////////////////////////////////////////////////////////
  // Create BVHs buffers

  cl_int anErrorTmp = CL_SUCCESS;

  const NCollection_Handle<BVH_Tree<Standard_ShortReal, 4> >& aBVH = myRaytraceGeometry.BVH();

  const size_t aSceneMinPointBufferSize =
    aBVH->MinPointBuffer().size() != 0 ? aBVH->MinPointBuffer().size() : 1;

  mySceneMinPointBuffer = clCreateBuffer (myComputeContext,
    CL_MEM_READ_ONLY, aSceneMinPointBufferSize * sizeof(cl_float4), NULL, &anErrorTmp);
  anErrorRes |= anErrorTmp;

  const size_t aSceneMaxPointBufferSize =
    aBVH->MaxPointBuffer().size() != 0 ? aBVH->MaxPointBuffer().size() : 1;

  mySceneMaxPointBuffer = clCreateBuffer (myComputeContext,
    CL_MEM_READ_ONLY, aSceneMaxPointBufferSize * sizeof(cl_float4), NULL, &anErrorTmp);
  anErrorRes |= anErrorTmp;

  const size_t aSceneNodeInfoBufferSize =
    aBVH->NodeInfoBuffer().size() != 0 ? aBVH->NodeInfoBuffer().size() : 1;

  mySceneNodeInfoBuffer = clCreateBuffer (myComputeContext,
    CL_MEM_READ_ONLY, aSceneNodeInfoBufferSize * sizeof(cl_int4), NULL, &anErrorTmp);
  anErrorRes |= anErrorTmp;

  if (anErrorRes != CL_SUCCESS)
  {
#ifdef RAY_TRACE_PRINT_INFO
    std::cout << "Error! Failed to create OpenCL buffers for high-level scene BVH" << std::endl;
#endif
    return Standard_False;
  }

  Standard_Integer aTotalVerticesNb = 0;
  Standard_Integer aTotalElementsNb = 0;
  Standard_Integer aTotalBVHNodesNb = 0;

  for (Standard_Integer anElemIndex = 0; anElemIndex < myRaytraceGeometry.Size(); ++anElemIndex)
  {
    OpenGl_TriangleSet* aTriangleSet = dynamic_cast<OpenGl_TriangleSet*> (
      myRaytraceGeometry.Objects().ChangeValue (anElemIndex).operator->());

    Standard_ASSERT_RETURN (aTriangleSet != NULL,
      "Error! Failed to get triangulation of OpenGL element", Standard_False);

    aTotalVerticesNb += (int)aTriangleSet->Vertices.size();
    aTotalElementsNb += (int)aTriangleSet->Elements.size();

    Standard_ASSERT_RETURN (!aTriangleSet->BVH().IsNull(),
      "Error! Failed to get bottom-level BVH of OpenGL element", Standard_False);

    aTotalBVHNodesNb += (int)aTriangleSet->BVH()->NodeInfoBuffer().size();
  }

  aTotalBVHNodesNb = aTotalBVHNodesNb > 0 ? aTotalBVHNodesNb : 1;

  myObjectNodeInfoBuffer = clCreateBuffer (myComputeContext,
    CL_MEM_READ_ONLY, aTotalBVHNodesNb * sizeof(cl_int4), NULL, &anErrorTmp);
  anErrorRes |= anErrorTmp;

  myObjectMinPointBuffer = clCreateBuffer (myComputeContext,
    CL_MEM_READ_ONLY, aTotalBVHNodesNb * sizeof(cl_float4), NULL, &anErrorTmp);
  anErrorRes |= anErrorTmp;

  myObjectMaxPointBuffer = clCreateBuffer (myComputeContext,
    CL_MEM_READ_ONLY, aTotalBVHNodesNb * sizeof(cl_float4), NULL, &anErrorTmp);
  anErrorRes |= anErrorTmp;

  if (anErrorRes != CL_SUCCESS)
  {
#ifdef RAY_TRACE_PRINT_INFO
    std::cout << "Error! Failed to create OpenCL buffers for bottom-level scene BVHs" << std::endl;
#endif
    return Standard_False;
  }

  /////////////////////////////////////////////////////////////////////////////
  // Create geometry buffers

  aTotalVerticesNb = aTotalVerticesNb > 0 ? aTotalVerticesNb : 1;

  myGeometryVertexBuffer = clCreateBuffer (myComputeContext,
    CL_MEM_READ_ONLY, aTotalVerticesNb * sizeof(cl_float4), NULL, &anErrorTmp);
  anErrorRes |= anErrorTmp;

  myGeometryNormalBuffer = clCreateBuffer (myComputeContext,
    CL_MEM_READ_ONLY, aTotalVerticesNb * sizeof(cl_float4), NULL, &anErrorTmp);
  anErrorRes |= anErrorTmp;

  aTotalElementsNb = aTotalElementsNb > 0 ? aTotalElementsNb : 1;

  myGeometryTriangBuffer = clCreateBuffer (myComputeContext,
    CL_MEM_READ_ONLY, aTotalElementsNb * sizeof(cl_int4), NULL, &anErrorTmp);
  anErrorRes |= anErrorTmp;

  if (anErrorRes != CL_SUCCESS)
  {
#ifdef RAY_TRACE_PRINT_INFO
    std::cout << "Error! Failed to create OpenCL geometry buffers" << std::endl;
#endif
    return Standard_False;
  }

  /////////////////////////////////////////////////////////////////////////////
  // Write BVH and geometry buffers

  if (aBVH->NodeInfoBuffer().size() != 0)
  {
    anErrorRes |= clEnqueueWriteBuffer (myComputeQueue, mySceneNodeInfoBuffer, CL_FALSE, 0,
      aSceneNodeInfoBufferSize * sizeof(cl_int4), &aBVH->NodeInfoBuffer().front(), 0, NULL, NULL);

    anErrorRes |= clEnqueueWriteBuffer (myComputeQueue, mySceneMinPointBuffer, CL_FALSE, 0,
      aSceneMinPointBufferSize * sizeof(cl_float4), &aBVH->MinPointBuffer().front(), 0, NULL, NULL);

    anErrorRes |= clEnqueueWriteBuffer (myComputeQueue, mySceneMaxPointBuffer, CL_FALSE, 0,
      aSceneMaxPointBufferSize * sizeof(cl_float4), &aBVH->MaxPointBuffer().front(), 0, NULL, NULL);

    anErrorRes |= clFinish (myComputeQueue);

    if (anErrorRes != CL_SUCCESS)
    {
#ifdef RAY_TRACE_PRINT_INFO
      std::cout << "Error! Failed to write OpenCL buffers for high-level scene BVH" << std::endl;
#endif
      return Standard_False;
    }

    for (Standard_Integer aNodeIdx = 0; aNodeIdx < aBVH->Length(); ++aNodeIdx)
    {
      if (!aBVH->IsOuter (aNodeIdx))
        continue;

      OpenGl_TriangleSet* aTriangleSet = myRaytraceGeometry.TriangleSet (aNodeIdx);

      Standard_ASSERT_RETURN (aTriangleSet != NULL,
        "Error! Failed to get triangulation of OpenGL element", Standard_False);

      const size_t aBVHBuffserSize =
        aTriangleSet->BVH()->NodeInfoBuffer().size() != 0 ? aTriangleSet->BVH()->NodeInfoBuffer().size() : 1;

      const Standard_Integer aBVHOffset = myRaytraceGeometry.AccelerationOffset (aNodeIdx);

      Standard_ASSERT_RETURN (aBVHOffset != OpenGl_RaytraceGeometry::INVALID_OFFSET,
        "Error! Failed to get offset for bottom-level BVH", Standard_False);

      anErrorRes |= clEnqueueWriteBuffer (myComputeQueue, myObjectNodeInfoBuffer, CL_FALSE, aBVHOffset * sizeof(cl_int4),
        aBVHBuffserSize * sizeof(cl_int4), &aTriangleSet->BVH()->NodeInfoBuffer().front(), 0, NULL, NULL);

      anErrorRes |= clEnqueueWriteBuffer (myComputeQueue, myObjectMinPointBuffer, CL_FALSE, aBVHOffset * sizeof(cl_float4),
        aBVHBuffserSize * sizeof(cl_float4), &aTriangleSet->BVH()->MinPointBuffer().front(), 0, NULL, NULL);

      anErrorRes |= clEnqueueWriteBuffer (myComputeQueue, myObjectMaxPointBuffer, CL_FALSE, aBVHOffset * sizeof(cl_float4),
        aBVHBuffserSize * sizeof(cl_float4), &aTriangleSet->BVH()->MaxPointBuffer().front(), 0, NULL, NULL);

      anErrorRes |= clFinish (myComputeQueue);

      if (anErrorRes != CL_SUCCESS)
      {
#ifdef RAY_TRACE_PRINT_INFO
        std::cout << "Error! Failed to write OpenCL buffers for bottom-level scene BVHs" << std::endl;
#endif
        return Standard_False;
      }

      const Standard_Integer aVerticesOffset = myRaytraceGeometry.VerticesOffset (aNodeIdx);

      Standard_ASSERT_RETURN (aVerticesOffset != OpenGl_RaytraceGeometry::INVALID_OFFSET,
        "Error! Failed to get offset for triangulation vertices of OpenGL element", Standard_False);

      anErrorRes |= clEnqueueWriteBuffer (myComputeQueue, myGeometryVertexBuffer, CL_FALSE, aVerticesOffset * sizeof(cl_float4),
        aTriangleSet->Vertices.size() * sizeof(cl_float4), &aTriangleSet->Vertices.front(), 0, NULL, NULL);

      anErrorRes |= clEnqueueWriteBuffer (myComputeQueue, myGeometryNormalBuffer, CL_FALSE, aVerticesOffset * sizeof(cl_float4),
        aTriangleSet->Normals.size() * sizeof(cl_float4), &aTriangleSet->Normals.front(), 0, NULL, NULL);

      const Standard_Integer anElementsOffset = myRaytraceGeometry.ElementsOffset (aNodeIdx);

      Standard_ASSERT_RETURN (anElementsOffset != OpenGl_RaytraceGeometry::INVALID_OFFSET,
        "Error! Failed to get offset for triangulation elements of OpenGL element", Standard_False);

      anErrorRes |= clEnqueueWriteBuffer (myComputeQueue, myGeometryTriangBuffer, CL_FALSE, anElementsOffset * sizeof(cl_int4),
        aTriangleSet->Elements.size() * sizeof(cl_int4), &aTriangleSet->Elements.front(), 0, NULL, NULL);

      anErrorRes |= clFinish (myComputeQueue);

      if (anErrorRes != CL_SUCCESS)
      {
#ifdef RAY_TRACE_PRINT_INFO
        std::cout << "Error! Failed to write OpenCL triangulation buffers for OpenGL element" << std::endl;
#endif
        return Standard_False;
      }
    }
  }

  /////////////////////////////////////////////////////////////////////////////
  // Write material buffer

  if (myRaytraceGeometry.Materials.size() != 0)
  {
    const void* aDataPtr = myRaytraceGeometry.Materials.front().Packed();

    anErrorRes |= clEnqueueWriteBuffer (myComputeQueue, myRaytraceMaterialBuffer,
      CL_FALSE, 0, aMaterialBufferSize * sizeof(OpenGl_RaytraceMaterial), aDataPtr, 0, NULL, NULL);

    if (anErrorRes != CL_SUCCESS)
    {
  #ifdef RAY_TRACE_PRINT_INFO
      std::cout << "Error! Failed to write OpenCL material buffer" << std::endl;
  #endif
      return Standard_False;
    }
  }

  anErrorRes |= clFinish (myComputeQueue);

  if (anErrorRes == CL_SUCCESS)
  {
    myIsRaytraceDataValid = myRaytraceGeometry.Objects().Size() != 0;
  }
#ifdef RAY_TRACE_PRINT_INFO
  else
  {
    std::cout << "Error! Failed to set scene data buffers" << std::endl;
  }
#endif

#ifdef RAY_TRACE_PRINT_INFO

  Standard_ShortReal aMemUsed = 0.f;

  for (Standard_Integer anElemIdx = 0; anElemIdx < myRaytraceGeometry.Size(); ++anElemIdx)
  {
    OpenGl_TriangleSet* aTriangleSet = dynamic_cast<OpenGl_TriangleSet*> (
      myRaytraceGeometry.Objects().ChangeValue (anElemIdx).operator->());

    aMemUsed += static_cast<Standard_ShortReal> (
      aTriangleSet->Vertices.size() * sizeof (BVH_Vec4f));
    aMemUsed += static_cast<Standard_ShortReal> (
      aTriangleSet->Normals.size() * sizeof (BVH_Vec4f));
    aMemUsed += static_cast<Standard_ShortReal> (
      aTriangleSet->Elements.size() * sizeof (BVH_Vec4i));

    aMemUsed += static_cast<Standard_ShortReal> (
      aTriangleSet->BVH()->NodeInfoBuffer().size() * sizeof (BVH_Vec4i));
    aMemUsed += static_cast<Standard_ShortReal> (
      aTriangleSet->BVH()->MinPointBuffer().size() * sizeof (BVH_Vec4f));
    aMemUsed += static_cast<Standard_ShortReal> (
      aTriangleSet->BVH()->MaxPointBuffer().size() * sizeof (BVH_Vec4f));
  }

  aMemUsed += static_cast<Standard_ShortReal> (
    myRaytraceGeometry.BVH()->NodeInfoBuffer().size() * sizeof (BVH_Vec4i));
  aMemUsed += static_cast<Standard_ShortReal> (
    myRaytraceGeometry.BVH()->MinPointBuffer().size() * sizeof (BVH_Vec4f));
  aMemUsed += static_cast<Standard_ShortReal> (
    myRaytraceGeometry.BVH()->MaxPointBuffer().size() * sizeof (BVH_Vec4f));

  std::cout << "GPU Memory Used (MB): ~" << aMemUsed / 1048576 << std::endl;

#endif

  return (CL_SUCCESS == anErrorRes);
}

// Use it to estimate the optimal size of OpenCL work group
// #define OPENCL_GROUP_SIZE_TEST

// =======================================================================
// function : RunRaytraceOpenCLKernels
// purpose  : Runs OpenCL ray-tracing kernels
// =======================================================================
Standard_Boolean OpenGl_Workspace::RunRaytraceOpenCLKernels (const Graphic3d_CView&   theCView,
                                                             const Standard_ShortReal theOrigins[16],
                                                             const Standard_ShortReal theDirects[16],
                                                             const Standard_Integer   theSizeX,
                                                             const Standard_Integer   theSizeY)
{
  if (myRaytraceRenderKernel == NULL || myComputeQueue == NULL)
    return Standard_False;

  ////////////////////////////////////////////////////////////////////////
  // Set kernel arguments

  cl_uint anIndex = 0;
  cl_int  anError = 0;

  cl_int aLightSourceBufferSize = (cl_int)myRaytraceGeometry.Sources.size();

  anError |= clSetKernelArg (
    myRaytraceRenderKernel, anIndex++, sizeof(cl_int), &theSizeX);
  anError |= clSetKernelArg (
    myRaytraceRenderKernel, anIndex++, sizeof(cl_int), &theSizeY);
  anError |= clSetKernelArg (
    myRaytraceRenderKernel, anIndex++, sizeof(cl_float16), theOrigins);
  anError |= clSetKernelArg (
    myRaytraceRenderKernel, anIndex++, sizeof(cl_float16), theDirects);
  anError |= clSetKernelArg (
    myRaytraceRenderKernel, anIndex++, sizeof(cl_mem), &myRaytraceEnvironment);
  anError |= clSetKernelArg (
    myRaytraceRenderKernel, anIndex++, sizeof(cl_mem), &myRaytraceOutputImage);
  anError |= clSetKernelArg (
    myRaytraceRenderKernel, anIndex++, sizeof(cl_mem), &mySceneNodeInfoBuffer);
  anError |= clSetKernelArg (
    myRaytraceRenderKernel, anIndex++, sizeof(cl_mem), &mySceneMinPointBuffer);
  anError |= clSetKernelArg (
    myRaytraceRenderKernel, anIndex++, sizeof(cl_mem), &mySceneMaxPointBuffer);
  anError |= clSetKernelArg (
    myRaytraceRenderKernel, anIndex++, sizeof(cl_mem), &myObjectNodeInfoBuffer);
  anError |= clSetKernelArg (
    myRaytraceRenderKernel, anIndex++, sizeof(cl_mem), &myObjectMinPointBuffer);
  anError |= clSetKernelArg (
    myRaytraceRenderKernel, anIndex++, sizeof(cl_mem), &myObjectMaxPointBuffer);
  anError |= clSetKernelArg (
    myRaytraceRenderKernel, anIndex++, sizeof(cl_mem), &myGeometryTriangBuffer);
  anError |= clSetKernelArg (
    myRaytraceRenderKernel, anIndex++, sizeof(cl_mem), &myGeometryVertexBuffer);
  anError |= clSetKernelArg (
    myRaytraceRenderKernel, anIndex++, sizeof(cl_mem), &myGeometryNormalBuffer);
  anError |= clSetKernelArg (
    myRaytraceRenderKernel, anIndex++, sizeof(cl_mem), &myRaytraceLightSourceBuffer);
  anError |= clSetKernelArg (
    myRaytraceRenderKernel, anIndex++, sizeof(cl_mem), &myRaytraceMaterialBuffer);
  anError |= clSetKernelArg (
    myRaytraceRenderKernel, anIndex++, sizeof(cl_float4), &myRaytraceGeometry.GlobalAmbient);
  anError |= clSetKernelArg (
    myRaytraceRenderKernel, anIndex++, sizeof(cl_int), &aLightSourceBufferSize);
  anError |= clSetKernelArg (
    myRaytraceRenderKernel, anIndex++, sizeof(cl_int), &theCView.IsShadowsEnabled);
  anError |= clSetKernelArg (
    myRaytraceRenderKernel, anIndex++, sizeof(cl_int), &theCView.IsReflectionsEnabled);
  anError |= clSetKernelArg (
    myRaytraceRenderKernel, anIndex++, sizeof(cl_float), &myRaytraceSceneEpsilon);
  anError |= clSetKernelArg (
    myRaytraceRenderKernel, anIndex++, sizeof(cl_float), &myRaytraceSceneRadius);

  if (anError != CL_SUCCESS)
  {
    const TCollection_ExtendedString aMessage = "Error! Failed to set arguments of ray-tracing kernel!";

    myGlContext->PushMessage (GL_DEBUG_SOURCE_APPLICATION_ARB,
      GL_DEBUG_TYPE_ERROR_ARB, 0, GL_DEBUG_SEVERITY_HIGH_ARB, aMessage);

    return Standard_False;
  }

  // Second-pass 'smoothing' kernel runs only if anti-aliasing is enabled
  if (theCView.IsAntialiasingEnabled)
  {
    anIndex = 0;

    anError |= clSetKernelArg (
      myRaytraceSmoothKernel, anIndex++, sizeof(cl_int), &theSizeX);
    anError |= clSetKernelArg (
      myRaytraceSmoothKernel, anIndex++, sizeof(cl_int), &theSizeY);
    anError |= clSetKernelArg (
      myRaytraceSmoothKernel, anIndex++, sizeof(cl_float16), theOrigins);
    anError |= clSetKernelArg (
      myRaytraceSmoothKernel, anIndex++, sizeof(cl_float16), theDirects);
    anError |= clSetKernelArg (
      myRaytraceSmoothKernel, anIndex++, sizeof(cl_mem), &myRaytraceOutputImage);
    anError |= clSetKernelArg (
      myRaytraceSmoothKernel, anIndex++, sizeof(cl_mem), &myRaytraceEnvironment);
    anError |= clSetKernelArg (
      myRaytraceSmoothKernel, anIndex++, sizeof(cl_mem), &myRaytraceOutputImageAA);
    anError |= clSetKernelArg (
      myRaytraceSmoothKernel, anIndex++, sizeof(cl_mem), &mySceneNodeInfoBuffer);
    anError |= clSetKernelArg (
      myRaytraceSmoothKernel, anIndex++, sizeof(cl_mem), &mySceneMinPointBuffer);
    anError |= clSetKernelArg (
      myRaytraceSmoothKernel, anIndex++, sizeof(cl_mem), &mySceneMaxPointBuffer);
    anError |= clSetKernelArg (
      myRaytraceSmoothKernel, anIndex++, sizeof(cl_mem), &myObjectNodeInfoBuffer);
    anError |= clSetKernelArg (
      myRaytraceSmoothKernel, anIndex++, sizeof(cl_mem), &myObjectMinPointBuffer);
    anError |= clSetKernelArg (
      myRaytraceSmoothKernel, anIndex++, sizeof(cl_mem), &myObjectMaxPointBuffer);
    anError |= clSetKernelArg (
      myRaytraceSmoothKernel, anIndex++, sizeof(cl_mem), &myGeometryTriangBuffer);
    anError |= clSetKernelArg (
      myRaytraceSmoothKernel, anIndex++, sizeof(cl_mem), &myGeometryVertexBuffer);
    anError |= clSetKernelArg (
      myRaytraceSmoothKernel, anIndex++, sizeof(cl_mem), &myGeometryNormalBuffer);
    anError |= clSetKernelArg (
      myRaytraceSmoothKernel, anIndex++, sizeof(cl_mem), &myRaytraceLightSourceBuffer);
    anError |= clSetKernelArg (
      myRaytraceSmoothKernel, anIndex++, sizeof(cl_mem), &myRaytraceMaterialBuffer);
    anError |= clSetKernelArg (
      myRaytraceSmoothKernel, anIndex++, sizeof(cl_float4), &myRaytraceGeometry.GlobalAmbient);
    anError |= clSetKernelArg (
      myRaytraceSmoothKernel, anIndex++, sizeof(cl_int), &aLightSourceBufferSize);
    anError |= clSetKernelArg (
      myRaytraceSmoothKernel, anIndex++, sizeof(cl_int), &theCView.IsShadowsEnabled);
    anError |= clSetKernelArg (
      myRaytraceSmoothKernel, anIndex++, sizeof(cl_int), &theCView.IsReflectionsEnabled);
    anError |= clSetKernelArg (
      myRaytraceSmoothKernel, anIndex++, sizeof(cl_float), &myRaytraceSceneEpsilon);
    anError |= clSetKernelArg (
      myRaytraceSmoothKernel, anIndex++, sizeof(cl_float), &myRaytraceSceneRadius);

    if (anError != CL_SUCCESS)
    {
      const TCollection_ExtendedString aMessage = "Error! Failed to set arguments of smoothing kernel!";

      myGlContext->PushMessage (GL_DEBUG_SOURCE_APPLICATION_ARB,
        GL_DEBUG_TYPE_ERROR_ARB, 0, GL_DEBUG_SEVERITY_HIGH_ARB, aMessage);

      return Standard_False;
    }
  }

  ////////////////////////////////////////////////////////////////////////
  // Set work size

  size_t aLocWorkSize[] = { myIsAmdComputePlatform ? 2 : 4, 32 };

#ifdef OPENCL_GROUP_SIZE_TEST
  for (size_t aLocX = 2; aLocX <= 32; aLocX <<= 1)
  for (size_t aLocY = 2; aLocY <= 32; aLocY <<= 1)
  {
    aLocWorkSize[0] = aLocX;
    aLocWorkSize[1] = aLocY;
#endif

    size_t aWorkSizeX = theSizeX;
    if (aWorkSizeX % aLocWorkSize[0] != 0)
      aWorkSizeX += aLocWorkSize[0] - aWorkSizeX % aLocWorkSize[0];

    size_t aWokrSizeY = theSizeY;
    if (aWokrSizeY % aLocWorkSize[1] != 0 )
      aWokrSizeY += aLocWorkSize[1] - aWokrSizeY % aLocWorkSize[1];

    size_t aTotWorkSize[] = { aWorkSizeX, aWokrSizeY };

    cl_event anEvent = NULL, anEventSmooth = NULL;

    anError = clEnqueueNDRangeKernel (myComputeQueue,
      myRaytraceRenderKernel, 2, NULL, aTotWorkSize, aLocWorkSize, 0, NULL, &anEvent);

    if (anError != CL_SUCCESS)
    {
      const TCollection_ExtendedString aMessage = "Error! Failed to execute the ray-tracing kernel!";

      myGlContext->PushMessage (GL_DEBUG_SOURCE_APPLICATION_ARB,
        GL_DEBUG_TYPE_ERROR_ARB, 0, GL_DEBUG_SEVERITY_HIGH_ARB, aMessage);

      return Standard_False;
    }

    clWaitForEvents (1, &anEvent);

    if (theCView.IsAntialiasingEnabled)
    {
      size_t aLocWorkSizeSmooth[] = { myIsAmdComputePlatform ? 8 : 4,
                                      myIsAmdComputePlatform ? 8 : 32 };

#ifdef OPENCL_GROUP_SIZE_TEST
      aLocWorkSizeSmooth[0] = aLocX;
      aLocWorkSizeSmooth[1] = aLocY;
#endif

      aWorkSizeX = theSizeX;
      if (aWorkSizeX % aLocWorkSizeSmooth[0] != 0)
        aWorkSizeX += aLocWorkSizeSmooth[0] - aWorkSizeX % aLocWorkSizeSmooth[0];

      size_t aWokrSizeY = theSizeY;
      if (aWokrSizeY % aLocWorkSizeSmooth[1] != 0 )
        aWokrSizeY += aLocWorkSizeSmooth[1] - aWokrSizeY % aLocWorkSizeSmooth[1];

      size_t aTotWorkSizeSmooth [] = { aWorkSizeX, aWokrSizeY };

      anError = clEnqueueNDRangeKernel (myComputeQueue, myRaytraceSmoothKernel,
        2, NULL, aTotWorkSizeSmooth, aLocWorkSizeSmooth, 0, NULL, &anEventSmooth);

      clWaitForEvents (1, &anEventSmooth);

      if (anError != CL_SUCCESS)
      {
        const TCollection_ExtendedString aMessage = "Error! Failed to execute the smoothing kernel";

        myGlContext->PushMessage (GL_DEBUG_SOURCE_APPLICATION_ARB,
          GL_DEBUG_TYPE_ERROR_ARB, 0, GL_DEBUG_SEVERITY_HIGH_ARB, aMessage);

        return Standard_False;
      }
    }

#if defined (RAY_TRACE_PRINT_INFO) || defined (OPENCL_GROUP_SIZE_TEST)

    static cl_ulong ttt1 = 10000000000;
    static cl_ulong ttt2 = 10000000000;

    cl_ulong aBegTime = 0;
    cl_ulong aEndTime = 0;

    clGetEventProfilingInfo (anEvent,
      CL_PROFILING_COMMAND_START, sizeof(aBegTime), &aBegTime, NULL);
    clGetEventProfilingInfo (anEvent,
      CL_PROFILING_COMMAND_END, sizeof(aEndTime), &aEndTime, NULL);

    ttt1 = aEndTime - aBegTime < ttt1 ? aEndTime - aBegTime : ttt1;

    std::cout << "\tRender time (ms): " << ttt1 / 1e6f << std::endl;

    if (theCView.IsAntialiasingEnabled)
    {
      clGetEventProfilingInfo (anEventSmooth,
        CL_PROFILING_COMMAND_START, sizeof(aBegTime), &aBegTime, NULL);
      clGetEventProfilingInfo (anEventSmooth,
        CL_PROFILING_COMMAND_END, sizeof(aEndTime), &aEndTime, NULL);

      ttt2 = aEndTime - aBegTime < ttt2 ? aEndTime - aBegTime : ttt2;

      std::cout << "\tSmooth time (ms): " << ttt2 / 1e6f << std::endl;
    }

#endif

    if (anEvent != NULL)
      clReleaseEvent (anEvent);

    if (anEventSmooth != NULL)
      clReleaseEvent (anEventSmooth);

#ifdef OPENCL_GROUP_SIZE_TEST
  }
#endif

  return Standard_True;
}

// =======================================================================
// function : ComputeInverseMatrix
// purpose  : Computes inversion of 4x4 floating-point matrix
// =======================================================================
template <typename T>
void ComputeInverseMatrix (const T m[16], T inv[16])
{
  inv[ 0] = m[ 5] * (m[10] * m[15] - m[11] * m[14]) -
            m[ 9] * (m[ 6] * m[15] - m[ 7] * m[14]) -
            m[13] * (m[ 7] * m[10] - m[ 6] * m[11]);

  inv[ 1] = m[ 1] * (m[11] * m[14] - m[10] * m[15]) -
            m[ 9] * (m[ 3] * m[14] - m[ 2] * m[15]) -
            m[13] * (m[ 2] * m[11] - m[ 3] * m[10]);

  inv[ 2] = m[ 1] * (m[ 6] * m[15] - m[ 7] * m[14]) -
            m[ 5] * (m[ 2] * m[15] - m[ 3] * m[14]) -
            m[13] * (m[ 3] * m[ 6] - m[ 2] * m[ 7]);

  inv[ 3] = m[ 1] * (m[ 7] * m[10] - m[ 6] * m[11]) -
            m[ 5] * (m[ 3] * m[10] - m[ 2] * m[11]) -
            m[ 9] * (m[ 2] * m[ 7] - m[ 3] * m[ 6]);

  inv[ 4] = m[ 4] * (m[11] * m[14] - m[10] * m[15]) -
            m[ 8] * (m[ 7] * m[14] - m[ 6] * m[15]) -
            m[12] * (m[ 6] * m[11] - m[ 7] * m[10]);

  inv[ 5] = m[ 0] * (m[10] * m[15] - m[11] * m[14]) -
            m[ 8] * (m[ 2] * m[15] - m[ 3] * m[14]) -
            m[12] * (m[ 3] * m[10] - m[ 2] * m[11]);

  inv[ 6] = m[ 0] * (m[ 7] * m[14] - m[ 6] * m[15]) -
            m[ 4] * (m[ 3] * m[14] - m[ 2] * m[15]) -
            m[12] * (m[ 2] * m[ 7] - m[ 3] * m[ 6]);

  inv[ 7] = m[ 0] * (m[ 6] * m[11] - m[ 7] * m[10]) -
            m[ 4] * (m[ 2] * m[11] - m[ 3] * m[10]) -
            m[ 8] * (m[ 3] * m[ 6] - m[ 2] * m[ 7]);

  inv[ 8] = m[ 4] * (m[ 9] * m[15] - m[11] * m[13]) -
            m[ 8] * (m[ 5] * m[15] - m[ 7] * m[13]) -
            m[12] * (m[ 7] * m[ 9] - m[ 5] * m[11]);

  inv[ 9] = m[ 0] * (m[11] * m[13] - m[ 9] * m[15]) -
            m[ 8] * (m[ 3] * m[13] - m[ 1] * m[15]) -
            m[12] * (m[ 1] * m[11] - m[ 3] * m[ 9]);

  inv[10] = m[ 0] * (m[ 5] * m[15] - m[ 7] * m[13]) -
            m[ 4] * (m[ 1] * m[15] - m[ 3] * m[13]) -
            m[12] * (m[ 3] * m[ 5] - m[ 1] * m[ 7]);

  inv[11] = m[ 0] * (m[ 7] * m[ 9] - m[ 5] * m[11]) -
            m[ 4] * (m[ 3] * m[ 9] - m[ 1] * m[11]) -
            m[ 8] * (m[ 1] * m[ 7] - m[ 3] * m[ 5]);

  inv[12] = m[ 4] * (m[10] * m[13] - m[ 9] * m[14]) -
            m[ 8] * (m[ 6] * m[13] - m[ 5] * m[14]) -
            m[12] * (m[ 5] * m[10] - m[ 6] * m[ 9]);

  inv[13] = m[ 0] * (m[ 9] * m[14] - m[10] * m[13]) -
            m[ 8] * (m[ 1] * m[14] - m[ 2] * m[13]) -
            m[12] * (m[ 2] * m[ 9] - m[ 1] * m[10]);

  inv[14] = m[ 0] * (m[ 6] * m[13] - m[ 5] * m[14]) -
            m[ 4] * (m[ 2] * m[13] - m[ 1] * m[14]) -
            m[12] * (m[ 1] * m[ 6] - m[ 2] * m[ 5]);

  inv[15] = m[ 0] * (m[ 5] * m[10] - m[ 6] * m[ 9]) -
            m[ 4] * (m[ 1] * m[10] - m[ 2] * m[ 9]) -
            m[ 8] * (m[ 2] * m[ 5] - m[ 1] * m[ 6]);

  T det = m[0] * inv[ 0] +
          m[1] * inv[ 4] +
          m[2] * inv[ 8] +
          m[3] * inv[12];

  if (det == T (0.0)) return;

  det = T (1.0) / det;

  for (Standard_Integer i = 0; i < 16; ++i)
    inv[i] *= det;
}

// =======================================================================
// function : GenerateCornerRays
// purpose  : Generates primary rays for corners of screen quad
// =======================================================================
void GenerateCornerRays (const GLdouble theInvModelProj[16],
                         cl_float       theOrigins[16],
                         cl_float       theDirects[16])
{
  Standard_Integer aOriginIndex = 0;
  Standard_Integer aDirectIndex = 0;

  for (Standard_Integer y = -1; y <= 1; y += 2)
  {
    for (Standard_Integer x = -1; x <= 1; x += 2)
    {
      BVH_Vec4f aOrigin (float(x),
                         float(y),
                         -1.f,
                         1.f);

      aOrigin = MatVecMult (theInvModelProj, aOrigin);
      aOrigin.x() = aOrigin.x() / aOrigin.w();
      aOrigin.y() = aOrigin.y() / aOrigin.w();
      aOrigin.z() = aOrigin.z() / aOrigin.w();
      aOrigin.w() = 1.f;

      BVH_Vec4f aDirect (float(x),
                         float(y),
                         1.f,
                         1.f);

      aDirect = MatVecMult (theInvModelProj, aDirect);
      aDirect.x() = aDirect.x() / aDirect.w();
      aDirect.y() = aDirect.y() / aDirect.w();
      aDirect.z() = aDirect.z() / aDirect.w();
      aDirect.w() = 1.f;

      aDirect = aDirect - aOrigin;

      GLdouble aInvLen = 1.f / sqrt (aDirect.x() * aDirect.x() +
                                     aDirect.y() * aDirect.y() +
                                     aDirect.z() * aDirect.z());

      theOrigins [aOriginIndex++] = static_cast<GLfloat> (aOrigin.x());
      theOrigins [aOriginIndex++] = static_cast<GLfloat> (aOrigin.y());
      theOrigins [aOriginIndex++] = static_cast<GLfloat> (aOrigin.z());
      theOrigins [aOriginIndex++] = 1.f;

      theDirects [aDirectIndex++] = static_cast<GLfloat> (aDirect.x() * aInvLen);
      theDirects [aDirectIndex++] = static_cast<GLfloat> (aDirect.y() * aInvLen);
      theDirects [aDirectIndex++] = static_cast<GLfloat> (aDirect.z() * aInvLen);
      theDirects [aDirectIndex++] = 0.f;
    }
  }
}

// =======================================================================
// function : Raytrace
// purpose  : Redraws the window using OpenCL ray tracing
// =======================================================================
Standard_Boolean OpenGl_Workspace::Raytrace (const Graphic3d_CView& theCView,
                                             const Standard_Integer theSizeX,
                                             const Standard_Integer theSizeY,
                                             const Tint             theToSwap)
{
  if (!InitOpenCL())
    return Standard_False;

  if (!ResizeRaytraceOutputBuffer (theSizeX, theSizeY))
    return Standard_False;

  if (!UpdateRaytraceEnvironmentMap())
    return Standard_False;

  if (!UpdateRaytraceGeometry (Standard_True))
    return Standard_False;

  // Get model-view and projection matrices
  TColStd_Array2OfReal theOrientation (0, 3, 0, 3);
  TColStd_Array2OfReal theViewMapping (0, 3, 0, 3);

  myView->GetMatrices (theOrientation, theViewMapping);

  GLdouble aOrientationMatrix[16];
  GLdouble aViewMappingMatrix[16];
  GLdouble aOrientationInvers[16];

  for (Standard_Integer j = 0; j < 4; ++j)
    for (Standard_Integer i = 0; i < 4; ++i)
    {
      aOrientationMatrix [4 * j + i] = theOrientation (i, j);
      aViewMappingMatrix [4 * j + i] = theViewMapping (i, j);
    }

  ComputeInverseMatrix (aOrientationMatrix, aOrientationInvers);

  if (!UpdateRaytraceLightSources (aOrientationInvers))
    return Standard_False;

  // Generate primary rays for corners of the screen quad
  glMatrixMode (GL_MODELVIEW);

  glLoadMatrixd (aViewMappingMatrix);
  glMultMatrixd (aOrientationMatrix);

  GLdouble aModelProject[16];
  GLdouble aInvModelProj[16];

  glGetDoublev (GL_MODELVIEW_MATRIX, aModelProject);

  ComputeInverseMatrix (aModelProject, aInvModelProj);

  GLfloat aOrigins[16];
  GLfloat aDirects[16];

  GenerateCornerRays (aInvModelProj,
                      aOrigins,
                      aDirects);

  // Compute ray-traced image using OpenCL kernel
  cl_mem anImages[] = { myRaytraceOutputImage, myRaytraceOutputImageAA };
  cl_int anError = clEnqueueAcquireGLObjects (myComputeQueue,
                                              2, anImages,
                                              0, NULL, NULL);
  clFinish (myComputeQueue);

  if (myIsRaytraceDataValid)
  {
    RunRaytraceOpenCLKernels (theCView,
                              aOrigins,
                              aDirects,
                              theSizeX,
                              theSizeY);
  }

  anError |= clEnqueueReleaseGLObjects (myComputeQueue,
                                        2, anImages,
                                        0, NULL, NULL);
  clFinish (myComputeQueue);

  // Draw background
  glPushAttrib (GL_ENABLE_BIT |
                GL_CURRENT_BIT |
                GL_COLOR_BUFFER_BIT |
                GL_DEPTH_BUFFER_BIT);

  glDisable (GL_DEPTH_TEST);

  if (NamedStatus & OPENGL_NS_WHITEBACK)
  {
    glClearColor (1.0f, 1.0f, 1.0f, 1.0f);
  }
  else
  {
    glClearColor (myBgColor.rgb[0],
                  myBgColor.rgb[1],
                  myBgColor.rgb[2],
                  1.0f);
  }

  glClear (GL_COLOR_BUFFER_BIT);

  Handle(OpenGl_Workspace) aWorkspace (this);
  myView->DrawBackground (aWorkspace);

  // Draw dummy quad to show result image
  glEnable (GL_COLOR_MATERIAL);
  glEnable (GL_BLEND);

  glDisable (GL_DEPTH_TEST);

  glBlendFunc (GL_ONE, GL_SRC_ALPHA);

  glEnable (GL_TEXTURE_RECTANGLE);

  glMatrixMode (GL_PROJECTION);
  glLoadIdentity();

  glMatrixMode (GL_MODELVIEW);
  glLoadIdentity();

  glColor3f (1.0f, 1.0f, 1.0f);

  if (!theCView.IsAntialiasingEnabled)
    myRaytraceOutputTexture->Bind (myGlContext);
  else
    myRaytraceOutputTextureAA->Bind (myGlContext);

  if (myIsRaytraceDataValid)
  {
    glBegin (GL_QUADS);
    {
      glTexCoord2i (       0,        0);   glVertex2f (-1.f, -1.f);
      glTexCoord2i (       0, theSizeY);   glVertex2f (-1.f,  1.f);
      glTexCoord2i (theSizeX, theSizeY);   glVertex2f ( 1.f,  1.f);
      glTexCoord2i (theSizeX,        0);   glVertex2f ( 1.f, -1.f);
    }
    glEnd();
  }

  glPopAttrib();

  // Swap the buffers
  if (theToSwap)
  {
    GetGlContext()->SwapBuffers();
    myBackBufferRestored = Standard_False;
  }
  else
    glFlush();

  return Standard_True;
}

#endif
