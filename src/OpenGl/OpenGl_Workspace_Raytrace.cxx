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
template< typename T >
OpenGl_RTVec4f MatVecMult (const T m[16], const OpenGl_RTVec4f& v)
{
  return OpenGl_RTVec4f (
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

  int aSizeX = 1;
  int aSizeY = 1;

  if (!myView->TextureEnv().IsNull() && myView->SurfaceDetail() != Visual3d_TOD_NONE)
  {
    aSizeX = (myView->TextureEnv()->SizeX() <= 0) ? 1 : myView->TextureEnv()->SizeX();
    aSizeY = (myView->TextureEnv()->SizeY() <= 0) ? 1 : myView->TextureEnv()->SizeY();
  }

  cl_image_format aImageFormat;

  aImageFormat.image_channel_order = CL_RGBA;
  aImageFormat.image_channel_data_type = CL_FLOAT;

  myRaytraceEnvironment = clCreateImage2D (myComputeContext, CL_MEM_READ_ONLY,
                                           &aImageFormat, aSizeX, aSizeY, 0,
                                           NULL, &anError);

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
    for (int aPixel = 0; aPixel < aSizeX * aSizeY * 4; ++aPixel)
      aPixelData[aPixel] = 0.f;
  }

  size_t anImageOffset[] = { 0,
                             0,
                             0 };

  size_t anImageRegion[] = { aSizeX,
                             aSizeY,
                             1 };

  anError |= clEnqueueWriteImage (myRaytraceQueue, myRaytraceEnvironment, CL_TRUE,
                                  anImageOffset, anImageRegion, 0, 0, aPixelData,
                                  0, NULL, NULL);
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
    myRaytraceSceneData.Clear();

    myIsRaytraceDataValid = Standard_False;
  }
  else
  {
    if (myLayersModificationStatus != myView->LayerList().ModificationState())
    {
      return UpdateRaytraceGeometry (Standard_False);
    }
  }

  float* aTransform (NULL);

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

    for (int anIndex = 0; anIndex < aStructArray.Length(); ++anIndex)
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
              aTransform = new float[16];

            for (int i = 0; i < 4; ++i)
              for (int j = 0; j < 4; ++j)
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


#ifdef RAY_TRACE_PRINT_INFO
    OSD_Timer aTimer;
    aTimer.Start();
#endif

    myBVHBuilder.Build (myRaytraceSceneData);

#ifdef RAY_TRACE_PRINT_INFO
    std::cout << " Build time: " << aTimer.ElapsedTime() << " for "
                        << myRaytraceSceneData.Triangles.size() / 1000 << "K triangles" << std::endl;
#endif

    const float aScaleFactor = 1.5f;

    myRaytraceSceneRadius = aScaleFactor *
      Max ( Max (fabsf (myRaytraceSceneData.AABB.CornerMin().x()),
            Max (fabsf (myRaytraceSceneData.AABB.CornerMin().y()),
                 fabsf (myRaytraceSceneData.AABB.CornerMin().z()))),
            Max (fabsf (myRaytraceSceneData.AABB.CornerMax().x()),
            Max (fabsf (myRaytraceSceneData.AABB.CornerMax().y()),
                 fabsf (myRaytraceSceneData.AABB.CornerMax().z()))) );

    myRaytraceSceneEpsilon = Max (1e-4f, myRaytraceSceneRadius * 1e-4f);

    return WriteRaytraceSceneToDevice();
  }

  delete [] aTransform;

  return Standard_True;
}

// =======================================================================
// function : CheckRaytraceStructure
// purpose  : Adds OpenGL structure to ray-traced scene geometry
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
void CreateMaterial (const OPENGL_SURF_PROP&  theProp,
                     OpenGl_RaytraceMaterial& theMaterial)
{
  const float* aSrcAmb = theProp.isphysic ? theProp.ambcol.rgb : theProp.matcol.rgb;
  theMaterial.Ambient = OpenGl_RTVec4f (aSrcAmb[0] * theProp.amb,
                                        aSrcAmb[1] * theProp.amb,
                                        aSrcAmb[2] * theProp.amb,
                                        1.0f);

  const float* aSrcDif = theProp.isphysic ? theProp.difcol.rgb : theProp.matcol.rgb;
  theMaterial.Diffuse = OpenGl_RTVec4f (aSrcDif[0] * theProp.diff,
                                        aSrcDif[1] * theProp.diff,
                                        aSrcDif[2] * theProp.diff,
                                        1.0f);

  const float aDefSpecCol[4] = {1.0f, 1.0f, 1.0f, 1.0f};
  const float* aSrcSpe = theProp.isphysic ? theProp.speccol.rgb : aDefSpecCol;
  theMaterial.Specular = OpenGl_RTVec4f (aSrcSpe[0] * theProp.spec,
                                         aSrcSpe[1] * theProp.spec,
                                         aSrcSpe[2] * theProp.spec,
                                         theProp.shine);

  const float* aSrcEms = theProp.isphysic ? theProp.emscol.rgb : theProp.matcol.rgb;
  theMaterial.Emission = OpenGl_RTVec4f (aSrcEms[0] * theProp.emsv,
                                         aSrcEms[1] * theProp.emsv,
                                         aSrcEms[2] * theProp.emsv,
                                         1.0f);

  // Note: Here we use sub-linear transparency function
  // to produce realistic-looking transparency effect
  theMaterial.Transparency = OpenGl_RTVec4f (powf (theProp.trans, 0.75f),
                                             1.f - theProp.trans,
                                             1.f,
                                             1.f);

  const float aMaxRefl = Max (theMaterial.Diffuse.x() + theMaterial.Specular.x(),
                         Max (theMaterial.Diffuse.y() + theMaterial.Specular.y(),
                              theMaterial.Diffuse.z() + theMaterial.Specular.z()));

  const float aReflectionScale = 0.75f / aMaxRefl;

  theMaterial.Reflection = OpenGl_RTVec4f (theProp.speccol.rgb[0] * theProp.spec,
                                           theProp.speccol.rgb[1] * theProp.spec,
                                           theProp.speccol.rgb[2] * theProp.spec,
                                           0.f) * aReflectionScale;
}

// =======================================================================
// function : AddRaytraceStructure
// purpose  : Adds OpenGL structure to ray-traced scene geometry
// =======================================================================
Standard_Boolean OpenGl_Workspace::AddRaytraceStructure (const OpenGl_Structure*            theStructure,
                                                         const float*                       theTransform,
                                                         std::set<const OpenGl_Structure*>& theElements)
{
#ifdef RAY_TRACE_PRINT_INFO
  std::cout << "Add Structure" << std::endl;
#endif

  theElements.insert (theStructure);

  if (!theStructure->IsVisible())
  {
    myStructureStates[theStructure] = theStructure->ModificationState();
    return Standard_True;
  }

  // Get structure material
  int aStructMatID = -1;

  if (theStructure->AspectFace() != NULL)
  {
    aStructMatID = static_cast<int> (myRaytraceSceneData.Materials.size());

    OpenGl_RaytraceMaterial aStructMaterial;
    CreateMaterial (theStructure->AspectFace()->IntFront(), aStructMaterial);

    myRaytraceSceneData.Materials.push_back (aStructMaterial);
  }

  OpenGl_ListOfGroup::Iterator anItg (theStructure->Groups());

  while (anItg.More())
  {
    // Get group material
    int aGroupMatID = -1;

    if (anItg.Value()->AspectFace() != NULL)
    {
      aGroupMatID = static_cast<int> (myRaytraceSceneData.Materials.size());

      OpenGl_RaytraceMaterial aGroupMaterial;
      CreateMaterial (anItg.Value()->AspectFace()->IntFront(), aGroupMaterial);

      myRaytraceSceneData.Materials.push_back (aGroupMaterial);
    }

    int aMatID = aGroupMatID < 0 ? aStructMatID : aGroupMatID;

    if (aStructMatID < 0 && aGroupMatID < 0)
    {
      aMatID = static_cast<int> (myRaytraceSceneData.Materials.size());

      myRaytraceSceneData.Materials.push_back (OpenGl_RaytraceMaterial());
    }

    // Add OpenGL elements from group (only arrays of primitives)
    for (const OpenGl_ElementNode* aNode = anItg.Value()->FirstNode(); aNode != NULL; aNode = aNode->next)
    {
      if (TelNil == aNode->type)
      {
        OpenGl_AspectFace* anAspect = dynamic_cast<OpenGl_AspectFace*> (aNode->elem);

        if (anAspect != NULL)
        {
          aMatID = static_cast<int> (myRaytraceSceneData.Materials.size());

          OpenGl_RaytraceMaterial aMaterial;
          CreateMaterial (anAspect->IntFront(), aMaterial);

          myRaytraceSceneData.Materials.push_back (aMaterial);
        }
      }
      else if (TelParray == aNode->type)
      {
        OpenGl_PrimitiveArray* aPrimArray = dynamic_cast<OpenGl_PrimitiveArray*> (aNode->elem);

        if (aPrimArray != NULL)
        {
          AddRaytracePrimitiveArray (aPrimArray->PArray(), aMatID, theTransform);
        }
      }
    }

    anItg.Next();
  }

  float* aTransform (NULL);

  // Process all connected OpenGL structures
  OpenGl_ListOfStructure::Iterator anIts (theStructure->ConnectedStructures());

  while (anIts.More())
  {
    if (anIts.Value()->Transformation()->mat != NULL)
    {
      float* aTransform = new float[16];

      for (int i = 0; i < 4; ++i)
        for (int j = 0; j < 4; ++j)
        {
          aTransform[j * 4 + i] =
            anIts.Value()->Transformation()->mat[i][j];
        }
    }

    if (anIts.Value()->IsRaytracable())
      AddRaytraceStructure (anIts.Value(), aTransform != NULL ? aTransform : theTransform, theElements);

    anIts.Next();
  }

  delete[] aTransform;

  myStructureStates[theStructure] = theStructure->ModificationState();

  return Standard_True;
}

// =======================================================================
// function : AddRaytracePrimitiveArray
// purpose  : Adds OpenGL primitive array to ray-traced scene geometry
// =======================================================================
Standard_Boolean OpenGl_Workspace::AddRaytracePrimitiveArray (const CALL_DEF_PARRAY* theArray,
                                                              int                    theMatID,
                                                              const float*           theTransform)
{
  if (theArray->type != TelPolygonsArrayType &&
      theArray->type != TelTrianglesArrayType &&
      theArray->type != TelQuadranglesArrayType &&
      theArray->type != TelTriangleFansArrayType &&
      theArray->type != TelTriangleStripsArrayType &&
      theArray->type != TelQuadrangleStripsArrayType)
  {
    return Standard_True;
  }

  if (theArray->vertices == NULL)
    return Standard_False;

#ifdef RAY_TRACE_PRINT_INFO
  switch (theArray->type)
  {
    case TelPolygonsArrayType:
      std::cout << "\tTelPolygonsArrayType" << std::endl; break;
    case TelTrianglesArrayType:
      std::cout << "\tTelTrianglesArrayType" << std::endl; break;
    case TelQuadranglesArrayType:
      std::cout << "\tTelQuadranglesArrayType" << std::endl; break;
    case TelTriangleFansArrayType:
      std::cout << "\tTelTriangleFansArrayType" << std::endl; break;
    case TelTriangleStripsArrayType:
      std::cout << "\tTelTriangleStripsArrayType" << std::endl; break;
    case TelQuadrangleStripsArrayType:
      std::cout << "\tTelQuadrangleStripsArrayType" << std::endl; break;
  }
#endif

  // Simple optimization to eliminate possible memory allocations
  // during processing of the primitive array vertices
  myRaytraceSceneData.Vertices.reserve (
    myRaytraceSceneData.Vertices.size() + theArray->num_vertexs);

  const int aFirstVert = static_cast<int> (myRaytraceSceneData.Vertices.size());

  for (int aVert = 0; aVert < theArray->num_vertexs; ++aVert)
  {
    OpenGl_RTVec4f aVertex (theArray->vertices[aVert].xyz[0],
                            theArray->vertices[aVert].xyz[1],
                            theArray->vertices[aVert].xyz[2],
                            1.f);

    if (theTransform)
      aVertex = MatVecMult (theTransform, aVertex);

    myRaytraceSceneData.Vertices.push_back (aVertex);

    myRaytraceSceneData.AABB.Add (aVertex);
  }

  myRaytraceSceneData.Normals.reserve (
    myRaytraceSceneData.Normals.size() + theArray->num_vertexs);

  for (int aNorm = 0; aNorm < theArray->num_vertexs; ++aNorm)
  {
    OpenGl_RTVec4f aNormal;

    // Note: In case of absence of normals, the visualizer
    // will use generated geometric normals

    if (theArray->vnormals != NULL)
    {
      aNormal = OpenGl_RTVec4f (theArray->vnormals[aNorm].xyz[0],
                                theArray->vnormals[aNorm].xyz[1],
                                theArray->vnormals[aNorm].xyz[2],
                                0.f);

      if (theTransform)
        aNormal = MatVecMult (theTransform, aNormal);
    }

    myRaytraceSceneData.Normals.push_back (aNormal);
  }

  if (theArray->num_bounds > 0)
  {
#ifdef RAY_TRACE_PRINT_INFO
    std::cout << "\tNumber of bounds = " << theArray->num_bounds << std::endl;
#endif

    int aVertOffset = 0;

    for (int aBound = 0; aBound < theArray->num_bounds; ++aBound)
    {
      const int aVertNum = theArray->bounds[aBound];

#ifdef RAY_TRACE_PRINT_INFO
      std::cout << "\tAdd indices from bound " << aBound << ": " <<
                                    aVertOffset << ", " << aVertNum << std::endl;
#endif

      if (!AddRaytraceVertexIndices (theArray, aFirstVert, aVertOffset, aVertNum, theMatID))
      {
        return Standard_False;
      }

      aVertOffset += aVertNum;
    }
  }
  else
  {
    const int aVertNum = theArray->num_edges > 0 ? theArray->num_edges : theArray->num_vertexs;

#ifdef RAY_TRACE_PRINT_INFO
      std::cout << "\tAdd indices: " << aVertNum << std::endl;
#endif

    return AddRaytraceVertexIndices (theArray, aFirstVert, 0, aVertNum, theMatID);
  }

  return Standard_True;
}

// =======================================================================
// function : AddRaytraceVertexIndices
// purpose  : Adds vertex indices to ray-traced scene geometry
// =======================================================================
Standard_Boolean OpenGl_Workspace::AddRaytraceVertexIndices (const CALL_DEF_PARRAY* theArray,
                                                             int                    theFirstVert,
                                                             int                    theVertOffset,
                                                             int                    theVertNum,
                                                             int                    theMatID)
{
  myRaytraceSceneData.Triangles.reserve (myRaytraceSceneData.Triangles.size() + theVertNum);
  switch (theArray->type)
  {
    case TelTrianglesArrayType:        return AddRaytraceTriangleArray        (theArray, theFirstVert, theVertOffset, theVertNum, theMatID);
    case TelQuadranglesArrayType:      return AddRaytraceQuadrangleArray      (theArray, theFirstVert, theVertOffset, theVertNum, theMatID);
    case TelTriangleFansArrayType:     return AddRaytraceTriangleFanArray     (theArray, theFirstVert, theVertOffset, theVertNum, theMatID);
    case TelTriangleStripsArrayType:   return AddRaytraceTriangleStripArray   (theArray, theFirstVert, theVertOffset, theVertNum, theMatID);
    case TelQuadrangleStripsArrayType: return AddRaytraceQuadrangleStripArray (theArray, theFirstVert, theVertOffset, theVertNum, theMatID);
    case TelPolygonsArrayType:         return AddRaytracePolygonArray         (theArray, theFirstVert, theVertOffset, theVertNum, theMatID);
    default:                           return Standard_False;
  }
}

// =======================================================================
// function : AddRaytraceTriangleArray
// purpose  : Adds OpenGL triangle array to ray-traced scene geometry
// =======================================================================
Standard_Boolean OpenGl_Workspace::AddRaytraceTriangleArray (const CALL_DEF_PARRAY* theArray,
                                                             int                    theFirstVert,
                                                             int                    theVertOffset,
                                                             int                    theVertNum,
                                                             int                    theMatID)
{
  if (theVertNum < 3)
    return Standard_True;

  if (theArray->num_edges > 0)
  {
    for (int aVert = theVertOffset; aVert < theVertOffset + theVertNum - 2; aVert += 3)
    {
      myRaytraceSceneData.Triangles.push_back (OpenGl_RTVec4i (theFirstVert + theArray->edges[aVert + 0],
                                                               theFirstVert + theArray->edges[aVert + 1],
                                                               theFirstVert + theArray->edges[aVert + 2],
                                                               theMatID));
    }
  }
  else
  {
    for (int aVert = theVertOffset; aVert < theVertOffset + theVertNum - 2; aVert += 3)
    {
      myRaytraceSceneData.Triangles.push_back (OpenGl_RTVec4i (theFirstVert + aVert + 0,
                                                               theFirstVert + aVert + 1,
                                                               theFirstVert + aVert + 2,
                                                               theMatID));
    }
  }

  return Standard_True;
}

// =======================================================================
// function : AddRaytraceTriangleFanArray
// purpose  : Adds OpenGL triangle fan array to ray-traced scene geometry
// =======================================================================
Standard_Boolean OpenGl_Workspace::AddRaytraceTriangleFanArray (const CALL_DEF_PARRAY* theArray,
                                                                int                     theFirstVert,
                                                                int                     theVertOffset,
                                                                int                     theVertNum,
                                                                int                     theMatID)
{
  if (theVertNum < 3)
    return Standard_True;

  if (theArray->num_edges > 0)
  {
    for (int aVert = theVertOffset; aVert < theVertOffset + theVertNum - 2; ++aVert)
    {
      myRaytraceSceneData.Triangles.push_back (OpenGl_RTVec4i (theFirstVert + theArray->edges[theVertOffset],
                                                               theFirstVert + theArray->edges[aVert + 1],
                                                               theFirstVert + theArray->edges[aVert + 2],
                                                               theMatID));
    }
  }
  else
  {
    for (int aVert = theVertOffset; aVert < theVertOffset + theVertNum - 2; ++aVert)
    {
      myRaytraceSceneData.Triangles.push_back (OpenGl_RTVec4i (theFirstVert + theVertOffset,
                                                               theFirstVert + aVert + 1,
                                                               theFirstVert + aVert + 2,
                                                               theMatID));
    }
  }

  return Standard_True;
}

// =======================================================================
// function : AddRaytraceTriangleStripArray
// purpose  : Adds OpenGL triangle strip array to ray-traced scene geometry
// =======================================================================
Standard_Boolean OpenGl_Workspace::AddRaytraceTriangleStripArray (const CALL_DEF_PARRAY* theArray,
                                                                  int                    theFirstVert,
                                                                  int                    theVertOffset,
                                                                  int                    theVertNum,
                                                                  int                    theMatID)
{
  if (theVertNum < 3)
    return Standard_True;

  if (theArray->num_edges > 0)
  {
    myRaytraceSceneData.Triangles.push_back (OpenGl_RTVec4i (
                                      theFirstVert + theArray->edges[theVertOffset + 0],
                                      theFirstVert + theArray->edges[theVertOffset + 1],
                                      theFirstVert + theArray->edges[theVertOffset + 2],
                                      theMatID));

    for (int aVert = theVertOffset + 1, aTriNum = 1; aVert < theVertOffset + theVertNum - 2; ++aVert, ++aTriNum)
    {
      myRaytraceSceneData.Triangles.push_back (OpenGl_RTVec4i (
                                      theFirstVert + theArray->edges[aVert + (aTriNum % 2) ? 1 : 0],
                                      theFirstVert + theArray->edges[aVert + (aTriNum % 2) ? 0 : 1],
                                      theFirstVert + theArray->edges[aVert + 2],
                                      theMatID));
    }
  }
  else
  {
    myRaytraceSceneData.Triangles.push_back (OpenGl_RTVec4i (theFirstVert + theVertOffset + 0,
                                                             theFirstVert + theVertOffset + 1,
                                                             theFirstVert + theVertOffset + 2,
                                                             theMatID));

    for (int aVert = theVertOffset + 1, aTriNum = 1; aVert < theVertOffset + theVertNum - 2; ++aVert, ++aTriNum)
    {
      myRaytraceSceneData.Triangles.push_back (OpenGl_RTVec4i (theFirstVert + aVert + ( aTriNum % 2 ) ? 1 : 0,
                                                               theFirstVert + aVert + ( aTriNum % 2 ) ? 0 : 1,
                                                               theFirstVert + aVert + 2,
                                                               theMatID));
    }
  }

  return Standard_True;
}

// =======================================================================
// function : AddRaytraceQuadrangleArray
// purpose  : Adds OpenGL quad array to ray-traced scene geometry
// =======================================================================
Standard_Boolean OpenGl_Workspace::AddRaytraceQuadrangleArray (const CALL_DEF_PARRAY* theArray,
                                                               int                    theFirstVert,
                                                               int                    theVertOffset,
                                                               int                    theVertNum,
                                                               int                    theMatID)
{
  if (theVertNum < 4)
    return Standard_True;

  if (theArray->num_edges > 0)
  {
    for (int aVert = theVertOffset; aVert < theVertOffset + theVertNum - 3; aVert += 4)
    {
      myRaytraceSceneData.Triangles.push_back (OpenGl_RTVec4i (theFirstVert + theArray->edges[aVert + 0],
                                                               theFirstVert + theArray->edges[aVert + 1],
                                                               theFirstVert + theArray->edges[aVert + 2],
                                                               theMatID));

      myRaytraceSceneData.Triangles.push_back (OpenGl_RTVec4i (theFirstVert + theArray->edges[aVert + 0],
                                                               theFirstVert + theArray->edges[aVert + 2],
                                                               theFirstVert + theArray->edges[aVert + 3],
                                                               theMatID));
    }
  }
  else
  {
    for (int aVert = theVertOffset; aVert < theVertOffset + theVertNum - 3; aVert += 4)
    {
      myRaytraceSceneData.Triangles.push_back (OpenGl_RTVec4i (theFirstVert + aVert + 0,
                                                               theFirstVert + aVert + 1,
                                                               theFirstVert + aVert + 2,
                                                               theMatID));

      myRaytraceSceneData.Triangles.push_back (OpenGl_RTVec4i (theFirstVert + aVert + 0,
                                                               theFirstVert + aVert + 2,
                                                               theFirstVert + aVert + 3,
                                                               theMatID));
    }
  }

  return Standard_True;
}

// =======================================================================
// function : AddRaytraceQuadrangleStripArray
// purpose  : Adds OpenGL quad strip array to ray-traced scene geometry
// =======================================================================
Standard_Boolean OpenGl_Workspace::AddRaytraceQuadrangleStripArray (const CALL_DEF_PARRAY* theArray,
                                                                    int                    theFirstVert,
                                                                    int                    theVertOffset,
                                                                    int                    theVertNum,
                                                                    int                    theMatID)
{
  if (theVertNum < 4)
    return Standard_True;

  if (theArray->num_edges > 0)
  {
    myRaytraceSceneData.Triangles.push_back (OpenGl_RTVec4i (
                                theFirstVert + theArray->edges[theVertOffset + 0],
                                theFirstVert + theArray->edges[theVertOffset + 1],
                                theFirstVert + theArray->edges[theVertOffset + 2],
                                theMatID));

    myRaytraceSceneData.Triangles.push_back (OpenGl_RTVec4i (
                                theFirstVert + theArray->edges[theVertOffset + 1],
                                theFirstVert + theArray->edges[theVertOffset + 3],
                                theFirstVert + theArray->edges[theVertOffset + 2],
                                theMatID));

    for (int aVert = theVertOffset + 2; aVert < theVertOffset + theVertNum - 3; aVert += 2)
    {
      myRaytraceSceneData.Triangles.push_back (OpenGl_RTVec4i (
                                  theFirstVert + theArray->edges[aVert + 0],
                                  theFirstVert + theArray->edges[aVert + 1],
                                  theFirstVert + theArray->edges[aVert + 2],
                                  theMatID));

      myRaytraceSceneData.Triangles.push_back (OpenGl_RTVec4i (
                                  theFirstVert + theArray->edges[aVert + 1],
                                  theFirstVert + theArray->edges[aVert + 3],
                                  theFirstVert + theArray->edges[aVert + 2],
                                  theMatID));
    }
  }
  else
  {
    myRaytraceSceneData.Triangles.push_back (OpenGl_RTVec4i (theFirstVert + 0,
                                                             theFirstVert + 1,
                                                             theFirstVert + 2,
                                                             theMatID));

    myRaytraceSceneData.Triangles.push_back (OpenGl_RTVec4i (theFirstVert + 1,
                                                             theFirstVert + 3,
                                                             theFirstVert + 2,
                                                             theMatID));

    for (int aVert = theVertOffset + 2; aVert < theVertOffset + theVertNum - 3; aVert += 2)
    {
      myRaytraceSceneData.Triangles.push_back (OpenGl_RTVec4i (theFirstVert + aVert + 0,
                                                               theFirstVert + aVert + 1,
                                                               theFirstVert + aVert + 2,
                                                               theMatID));

      myRaytraceSceneData.Triangles.push_back (OpenGl_RTVec4i (theFirstVert + aVert + 1,
                                                               theFirstVert + aVert + 3,
                                                               theFirstVert + aVert + 2,
                                                               theMatID));
    }
  }

  return Standard_True;
}

// =======================================================================
// function : AddRaytracePolygonArray
// purpose  : Adds OpenGL polygon array to ray-traced scene geometry
// =======================================================================
Standard_Boolean OpenGl_Workspace::AddRaytracePolygonArray (const CALL_DEF_PARRAY* theArray,
                                                            int                    theFirstVert,
                                                            int                    theVertOffset,
                                                            int                    theVertNum,
                                                            int                    theMatID)
{
  if (theArray->num_vertexs < 3)
    return Standard_True;

  if (theArray->edges != NULL)
  {
    for (int aVert = theVertOffset; aVert < theVertOffset + theVertNum - 2; ++aVert)
    {
      myRaytraceSceneData.Triangles.push_back (OpenGl_RTVec4i (theFirstVert + theArray->edges[theVertOffset],
                                                               theFirstVert + theArray->edges[aVert + 1],
                                                               theFirstVert + theArray->edges[aVert + 2],
                                                               theMatID));
    }
  }
  else
  {
    for (int aVert = theVertOffset; aVert < theVertOffset + theVertNum - 2; ++aVert)
    {
      myRaytraceSceneData.Triangles.push_back (OpenGl_RTVec4i (theFirstVert + theVertOffset,
                                                               theFirstVert + aVert + 1,
                                                               theFirstVert + aVert + 2,
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
  myRaytraceSceneData.LightSources.clear();

  OpenGl_RTVec4f anAmbient (0.0f, 0.0f, 0.0f, 0.0f);
  for (OpenGl_ListOfLight::Iterator anItl (myView->LightList());
       anItl.More(); anItl.Next())
  {
    const OpenGl_Light& aLight = anItl.Value();
    if (aLight.Type == Visual3d_TOLS_AMBIENT)
    {
      anAmbient += OpenGl_RTVec4f (aLight.Color.r(), aLight.Color.g(), aLight.Color.b(), 0.0f);
      continue;
    }

    OpenGl_RTVec4f aDiffuse  (aLight.Color.r(), aLight.Color.g(), aLight.Color.b(), 1.0f);
    OpenGl_RTVec4f aPosition (-aLight.Direction.x(), -aLight.Direction.y(), -aLight.Direction.z(), 0.0f);
    if (aLight.Type != Visual3d_TOLS_DIRECTIONAL)
    {
      aPosition = OpenGl_RTVec4f (aLight.Position.x(), aLight.Position.y(), aLight.Position.z(), 1.0f);
    }
    if (aLight.IsHeadlight)
    {
      aPosition = MatVecMult (theInvModelView, aPosition);
    }

    myRaytraceSceneData.LightSources.push_back (OpenGl_RaytraceLight (aDiffuse, aPosition));
  }

  if (myRaytraceSceneData.LightSources.size() > 0)
  {
    myRaytraceSceneData.LightSources.front().Ambient += anAmbient;
  }
  else
  {
    myRaytraceSceneData.LightSources.push_back (OpenGl_RaytraceLight (OpenGl_RTVec4f (anAmbient.rgb(), -1.0f)));
  }

  cl_int anError = CL_SUCCESS;

  if (myRaytraceLightSourceBuffer != NULL)
    clReleaseMemObject (myRaytraceLightSourceBuffer);

  const size_t myLightBufferSize = myRaytraceSceneData.LightSources.size() > 0
                                 ? myRaytraceSceneData.LightSources.size()
                                 : 1;

  myRaytraceLightSourceBuffer = clCreateBuffer (myComputeContext, CL_MEM_READ_ONLY,
                                                myLightBufferSize * sizeof(OpenGl_RaytraceLight),
                                                NULL, &anError);

  if (myRaytraceSceneData.LightSources.size() > 0)
  {
    const void* aDataPtr = myRaytraceSceneData.LightSources.front().Packed();
    anError |= clEnqueueWriteBuffer (myRaytraceQueue, myRaytraceLightSourceBuffer, CL_TRUE, 0,
                                     myLightBufferSize * sizeof(OpenGl_RaytraceLight), aDataPtr,
                                     0, NULL, NULL);
  }

#ifdef RAY_TRACE_PRINT_INFO
  if (anError != CL_SUCCESS)
  {
    std::cout << "Error! Failed to set light sources!";

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
  myRaytraceRenderKernel = clCreateKernel (myRaytraceProgram, "Main",            &anError);
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

  myRaytraceSmoothKernel = clCreateKernel (myRaytraceProgram, "MainAntialiased", &anError);
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

  myRaytraceQueue = clCreateCommandQueue (myComputeContext, aDevice, aProps, &anError);
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
  clReleaseCommandQueue (myRaytraceQueue);

  clReleaseMemObject (myRaytraceOutputImage);
  clReleaseMemObject (myRaytraceEnvironment);
  clReleaseMemObject (myRaytraceOutputImageSmooth);

  clReleaseMemObject (myRaytraceVertexBuffer);
  clReleaseMemObject (myRaytraceNormalBuffer);
  clReleaseMemObject (myRaytraceTriangleBuffer);

  clReleaseMemObject (myRaytraceMaterialBuffer);
  clReleaseMemObject (myRaytraceLightSourceBuffer);

  clReleaseMemObject (myRaytraceNodeMinPointBuffer);
  clReleaseMemObject (myRaytraceNodeMaxPointBuffer);
  clReleaseMemObject (myRaytraceNodeDataRcrdBuffer);

  clReleaseContext (myComputeContext);

  if (glIsTexture (*myRaytraceOutputTexture))
    glDeleteTextures (2, myRaytraceOutputTexture);
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

  bool toResize = true;
  GLint aSizeX = -1;
  GLint aSizeY = -1;
  if (*myRaytraceOutputTexture != 0)
  {
    if (!myGlContext->IsGlGreaterEqual (2, 1))
    {
      return Standard_False;
    }

    glBindTexture (GL_TEXTURE_RECTANGLE, *myRaytraceOutputTexture);

    glGetTexLevelParameteriv (GL_TEXTURE_RECTANGLE, 0, GL_TEXTURE_WIDTH,  &aSizeX);
    glGetTexLevelParameteriv (GL_TEXTURE_RECTANGLE, 0, GL_TEXTURE_HEIGHT, &aSizeY);

    toResize = (aSizeX != theSizeX) || (aSizeY != theSizeY);
    if (toResize)
    {
      glDeleteTextures (2, myRaytraceOutputTexture);
    }
  }
  if (!toResize)
  {
    return Standard_True;
  }

  glGenTextures (2, myRaytraceOutputTexture);
  for (int aTexIter = 0; aTexIter < 2; ++aTexIter)
  {
    glBindTexture (GL_TEXTURE_RECTANGLE, myRaytraceOutputTexture[aTexIter]);

    glTexParameteri (GL_TEXTURE_RECTANGLE, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri (GL_TEXTURE_RECTANGLE, GL_TEXTURE_WRAP_T, GL_CLAMP);
    glTexParameteri (GL_TEXTURE_RECTANGLE, GL_TEXTURE_WRAP_R, GL_CLAMP);

    glTexParameteri (GL_TEXTURE_RECTANGLE, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri (GL_TEXTURE_RECTANGLE, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glTexImage2D (GL_TEXTURE_RECTANGLE, 0, GL_RGBA32F,
                  theSizeX, theSizeY, 0,
                  GL_RGBA, GL_FLOAT, NULL);
  }

  cl_int anError = CL_SUCCESS;

  if (myRaytraceOutputImage != NULL)
  {
    clReleaseMemObject (myRaytraceOutputImage);
  }
  if (myRaytraceOutputImageSmooth != NULL)
  {
    clReleaseMemObject (myRaytraceOutputImageSmooth);
  }

  myRaytraceOutputImage = clCreateFromGLTexture2D (myComputeContext, CL_MEM_READ_WRITE,
                                                   GL_TEXTURE_RECTANGLE, 0,
                                                   myRaytraceOutputTexture[0], &anError);
  if (anError != CL_SUCCESS)
  {
#ifdef RAY_TRACE_PRINT_INFO
    std::cout << "Error! Failed to create output image!" << std::endl;
#endif
    return Standard_False;
  }

  myRaytraceOutputImageSmooth = clCreateFromGLTexture2D (myComputeContext, CL_MEM_READ_WRITE,
                                                         GL_TEXTURE_RECTANGLE, 0,
                                                         myRaytraceOutputTexture[1], &anError);
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
// purpose  : Writes scene geometry to OpenCl device
// =======================================================================
Standard_Boolean OpenGl_Workspace::WriteRaytraceSceneToDevice()
{
  if (myComputeContext == NULL)
    return Standard_False;

  cl_int anError = CL_SUCCESS;

  if (myRaytraceNormalBuffer != NULL)
    anError |= clReleaseMemObject (myRaytraceNormalBuffer);

  if (myRaytraceVertexBuffer != NULL)
    anError |= clReleaseMemObject (myRaytraceVertexBuffer);

  if (myRaytraceTriangleBuffer != NULL)
    anError |= clReleaseMemObject (myRaytraceTriangleBuffer);

  if (myRaytraceNodeMinPointBuffer != NULL)
    anError |= clReleaseMemObject (myRaytraceNodeMinPointBuffer);

  if (myRaytraceNodeMaxPointBuffer != NULL)
    anError |= clReleaseMemObject (myRaytraceNodeMaxPointBuffer);

  if (myRaytraceNodeDataRcrdBuffer != NULL)
    anError |= clReleaseMemObject (myRaytraceNodeDataRcrdBuffer);

  if (myRaytraceMaterialBuffer != NULL)
    anError |= clReleaseMemObject (myRaytraceMaterialBuffer);

  if (anError != CL_SUCCESS)
  {
#ifdef RAY_TRACE_PRINT_INFO
    std::cout << "Error! Failed to release OpenCL scene buffers!" << std::endl;
#endif
    return Standard_False;
  }

  // Create geometry buffers
  cl_int anErrorTemp = CL_SUCCESS;
  const size_t myVertexBufferSize = myRaytraceSceneData.Vertices.size() > 0
                                  ? myRaytraceSceneData.Vertices.size() : 1;

  myRaytraceVertexBuffer = clCreateBuffer (myComputeContext, CL_MEM_READ_ONLY,
                                           myVertexBufferSize * sizeof(cl_float4), NULL, &anErrorTemp);
  anError |= anErrorTemp;

  const size_t myNormalBufferSize = myRaytraceSceneData.Normals.size() > 0
                                  ? myRaytraceSceneData.Normals.size() : 1;
  myRaytraceNormalBuffer = clCreateBuffer (myComputeContext, CL_MEM_READ_ONLY,
                                           myNormalBufferSize * sizeof(cl_float4), NULL, &anErrorTemp);
  anError |= anErrorTemp;

  const size_t myTriangleBufferSize = myRaytraceSceneData.Triangles.size() > 0
                                    ? myRaytraceSceneData.Triangles.size() : 1;
  myRaytraceTriangleBuffer = clCreateBuffer (myComputeContext, CL_MEM_READ_ONLY,
                                             myTriangleBufferSize * sizeof(cl_int4), NULL, &anErrorTemp);
  anError |= anErrorTemp;
  if (anError != CL_SUCCESS)
  {
#ifdef RAY_TRACE_PRINT_INFO
    std::cout << "Error! Failed to create OpenCL geometry buffers!" << std::endl;
#endif
    return Standard_False;
  }

  // Create material buffer
  const size_t myMaterialBufferSize = myRaytraceSceneData.Materials.size() > 0
                                    ? myRaytraceSceneData.Materials.size() : 1;
  myRaytraceMaterialBuffer = clCreateBuffer (myComputeContext, CL_MEM_READ_ONLY,
                                             myMaterialBufferSize * sizeof(OpenGl_RaytraceMaterial), NULL,
                                             &anErrorTemp);
  if (anErrorTemp != CL_SUCCESS)
  {
#ifdef RAY_TRACE_PRINT_INFO
    std::cout << "Error! Failed to create OpenCL material buffer!" << std::endl;
#endif
    return Standard_False;
  }

  // Create BVH buffers
  OpenGl_BVH aTree = myBVHBuilder.Tree();
  const size_t myNodeMinPointBufferSize = aTree.MinPointBuffer().size() > 0
                                        ? aTree.MinPointBuffer().size() : 1;
  myRaytraceNodeMinPointBuffer = clCreateBuffer (myComputeContext, CL_MEM_READ_ONLY,
                                                 myNodeMinPointBufferSize * sizeof(cl_float4), NULL,
                                                 &anErrorTemp);
  anError |= anErrorTemp;

  const size_t myNodeMaxPointBufferSize = aTree.MaxPointBuffer().size() > 0
                                        ? aTree.MaxPointBuffer().size() : 1;
  myRaytraceNodeMaxPointBuffer = clCreateBuffer (myComputeContext, CL_MEM_READ_ONLY,
                                                 myNodeMaxPointBufferSize * sizeof(cl_float4), NULL,
                                                 &anError);
  anError |= anErrorTemp;

  const size_t myNodeDataRecordBufferSize = aTree.DataRcrdBuffer().size() > 0
                                          ? aTree.DataRcrdBuffer().size() : 1;
  myRaytraceNodeDataRcrdBuffer = clCreateBuffer (myComputeContext, CL_MEM_READ_ONLY,
                                                 myNodeDataRecordBufferSize * sizeof(cl_int4), NULL,
                                                 &anError);
  anError |= anErrorTemp;
  if (anError != CL_SUCCESS)
  {
#ifdef RAY_TRACE_PRINT_INFO
    std::cout << "Error! Failed to create OpenCL BVH buffers!" << std::endl;
#endif
    return Standard_False;
  }

  // Write scene geometry buffers
  if (myRaytraceSceneData.Triangles.size() > 0)
  {
    anError |= clEnqueueWriteBuffer (myRaytraceQueue, myRaytraceVertexBuffer, CL_FALSE,
                                     0, myRaytraceSceneData.Vertices.size() * sizeof(cl_float4),
                                     &myRaytraceSceneData.Vertices.front(),
                                     0, NULL, NULL);
    anError |= clEnqueueWriteBuffer (myRaytraceQueue, myRaytraceNormalBuffer, CL_FALSE,
                                     0, myRaytraceSceneData.Normals.size() * sizeof(cl_float4),
                                     &myRaytraceSceneData.Normals.front(),
                                     0, NULL, NULL);
    anError |= clEnqueueWriteBuffer (myRaytraceQueue, myRaytraceTriangleBuffer, CL_FALSE,
                                     0, myRaytraceSceneData.Triangles.size() * sizeof(cl_int4),
                                     &myRaytraceSceneData.Triangles.front(),
                                     0, NULL, NULL);
    if (anError != CL_SUCCESS)
    {
  #ifdef RAY_TRACE_PRINT_INFO
      std::cout << "Error! Failed to write OpenCL geometry buffers!" << std::endl;
  #endif
      return Standard_False;
    }
  }

  // Write BVH buffers
  if (aTree.DataRcrdBuffer().size() > 0)
  {
    anError |= clEnqueueWriteBuffer (myRaytraceQueue, myRaytraceNodeMinPointBuffer, CL_FALSE,
                                     0, aTree.MinPointBuffer().size() * sizeof(cl_float4),
                                     &aTree.MinPointBuffer().front(),
                                     0, NULL, NULL);
    anError |= clEnqueueWriteBuffer (myRaytraceQueue, myRaytraceNodeMaxPointBuffer, CL_FALSE,
                                     0, aTree.MaxPointBuffer().size() * sizeof(cl_float4),
                                     &aTree.MaxPointBuffer().front(),
                                     0, NULL, NULL);
    anError |= clEnqueueWriteBuffer (myRaytraceQueue, myRaytraceNodeDataRcrdBuffer, CL_FALSE,
                                     0, aTree.DataRcrdBuffer().size() * sizeof(cl_int4),
                                     &aTree.DataRcrdBuffer().front(),
                                     0, NULL, NULL);
    if (anError != CL_SUCCESS)
    {
  #ifdef RAY_TRACE_PRINT_INFO
      std::cout << "Error! Failed to write OpenCL BVH buffers!" << std::endl;
  #endif
      return Standard_False;
    }
  }

  // Write material buffers
  if (myRaytraceSceneData.Materials.size() > 0)
  {
    const size_t aSize    = myRaytraceSceneData.Materials.size();
    const void*  aDataPtr = myRaytraceSceneData.Materials.front().Packed();

    anError |= clEnqueueWriteBuffer (myRaytraceQueue, myRaytraceMaterialBuffer, CL_FALSE,
                                     0, aSize * sizeof(OpenGl_RaytraceMaterial), aDataPtr,
                                     0, NULL, NULL);
    if (anError != CL_SUCCESS)
    {
  #ifdef RAY_TRACE_PRINT_INFO
      std::cout << "Error! Failed to write OpenCL material buffer!" << std::endl;
  #endif
      return Standard_False;
    }
  }

  anError |= clFinish (myRaytraceQueue);
#ifdef RAY_TRACE_PRINT_INFO
  if (anError != CL_SUCCESS)
    std::cout << "Error! Failed to set scene data buffers!" << std::endl;
#endif

  if (anError == CL_SUCCESS)
    myIsRaytraceDataValid = myRaytraceSceneData.Triangles.size() > 0;

#ifdef RAY_TRACE_PRINT_INFO

  float aMemUsed = static_cast<float> (
    myRaytraceSceneData.Materials.size()) * sizeof (OpenGl_RaytraceMaterial);

  aMemUsed += static_cast<float> (
    myRaytraceSceneData.Triangles.size() * sizeof (OpenGl_RTVec4i) +
    myRaytraceSceneData.Vertices.size()  * sizeof (OpenGl_RTVec4f) +
    myRaytraceSceneData.Normals.size()   * sizeof (OpenGl_RTVec4f));

  aMemUsed += static_cast<float> (
    aTree.MinPointBuffer().size() * sizeof (OpenGl_RTVec4f) +
    aTree.MaxPointBuffer().size() * sizeof (OpenGl_RTVec4f) +
    aTree.DataRcrdBuffer().size() * sizeof (OpenGl_RTVec4i));

  std::cout << "GPU memory used (Mb): " << aMemUsed / 1e6f << std::endl;

#endif

  myRaytraceSceneData.Clear();

  myBVHBuilder.CleanUp();

  return (CL_SUCCESS == anError);
}

#define OPENCL_GROUP_SIZE_TEST_

// =======================================================================
// function : RunRaytraceOpenCLKernels
// purpose  : Runs OpenCL ray-tracing kernels
// =======================================================================
Standard_Boolean OpenGl_Workspace::RunRaytraceOpenCLKernels (const Graphic3d_CView& theCView,
                                                             const GLfloat          theOrigins[16],
                                                             const GLfloat          theDirects[16],
                                                             const int              theSizeX,
                                                             const int              theSizeY)
{
  if (myRaytraceRenderKernel == NULL || myRaytraceQueue == NULL)
    return Standard_False;

  ////////////////////////////////////////////////////////////
  // Set kernel arguments

  cl_uint anIndex = 0;
  cl_int  anError = 0;

  anError  = clSetKernelArg (myRaytraceRenderKernel, anIndex++,
                             sizeof(cl_mem), &myRaytraceOutputImage);
  anError |= clSetKernelArg (myRaytraceRenderKernel, anIndex++,
                             sizeof(cl_mem), &myRaytraceEnvironment);
  anError |= clSetKernelArg (myRaytraceRenderKernel, anIndex++,
                             sizeof(cl_mem), &myRaytraceNodeMinPointBuffer);
  anError |= clSetKernelArg (myRaytraceRenderKernel, anIndex++,
                             sizeof(cl_mem), &myRaytraceNodeMaxPointBuffer);
  anError |= clSetKernelArg (myRaytraceRenderKernel, anIndex++,
                             sizeof(cl_mem), &myRaytraceNodeDataRcrdBuffer);
  anError |= clSetKernelArg (myRaytraceRenderKernel, anIndex++,
                             sizeof(cl_mem), &myRaytraceLightSourceBuffer);
  anError |= clSetKernelArg (myRaytraceRenderKernel, anIndex++,
                             sizeof(cl_mem), &myRaytraceMaterialBuffer);
  anError |= clSetKernelArg (myRaytraceRenderKernel, anIndex++,
                             sizeof(cl_mem), &myRaytraceVertexBuffer);
  anError |= clSetKernelArg (myRaytraceRenderKernel, anIndex++,
                             sizeof(cl_mem), &myRaytraceNormalBuffer);
  anError |= clSetKernelArg (myRaytraceRenderKernel, anIndex++,
                             sizeof(cl_mem), &myRaytraceTriangleBuffer);

  anError |= clSetKernelArg (myRaytraceRenderKernel, anIndex++,
                             sizeof(cl_float16), theOrigins);
  anError |= clSetKernelArg (myRaytraceRenderKernel, anIndex++,
                             sizeof(cl_float16), theDirects);

  cl_int aLightCount =  static_cast<cl_int> (myRaytraceSceneData.LightSources.size());

  anError |= clSetKernelArg (myRaytraceRenderKernel, anIndex++,
                             sizeof(cl_int),   &aLightCount);
  anError |= clSetKernelArg (myRaytraceRenderKernel, anIndex++,
                             sizeof(cl_float), &myRaytraceSceneEpsilon);
  anError |= clSetKernelArg (myRaytraceRenderKernel, anIndex++,
                             sizeof(cl_float), &myRaytraceSceneRadius);
  anError |= clSetKernelArg (myRaytraceRenderKernel, anIndex++,
                             sizeof(cl_int),   &theCView.IsShadowsEnabled);
  anError |= clSetKernelArg (myRaytraceRenderKernel, anIndex++,
                             sizeof(cl_int),   &theCView.IsReflectionsEnabled);
  anError |= clSetKernelArg (myRaytraceRenderKernel, anIndex++,
                             sizeof(cl_int),   &theSizeX);
  anError |= clSetKernelArg (myRaytraceRenderKernel, anIndex++,
                             sizeof(cl_int),   &theSizeY);
  if (anError != CL_SUCCESS)
  {
    const TCollection_ExtendedString aMsg = "Error! Failed to set arguments of ray-tracing kernel!";
    myGlContext->PushMessage (GL_DEBUG_SOURCE_APPLICATION_ARB,
                              GL_DEBUG_TYPE_ERROR_ARB,
                              0,
                              GL_DEBUG_SEVERITY_HIGH_ARB,
                              aMsg);
    return Standard_False;
  }

  // Note: second-pass 'smoothing' kernel runs only if anti-aliasing is enabled
  if (theCView.IsAntialiasingEnabled)
  {
    anIndex = 0;
    anError  = clSetKernelArg (myRaytraceSmoothKernel, anIndex++,
                               sizeof(cl_mem), &myRaytraceOutputImage);
    anError |= clSetKernelArg (myRaytraceSmoothKernel, anIndex++,
                               sizeof(cl_mem), &myRaytraceOutputImageSmooth);
    anError |= clSetKernelArg (myRaytraceSmoothKernel, anIndex++,
                               sizeof(cl_mem), &myRaytraceEnvironment);
    anError |= clSetKernelArg (myRaytraceSmoothKernel, anIndex++,
                               sizeof(cl_mem), &myRaytraceNodeMinPointBuffer);
    anError |= clSetKernelArg (myRaytraceSmoothKernel, anIndex++,
                               sizeof(cl_mem), &myRaytraceNodeMaxPointBuffer);
    anError |= clSetKernelArg (myRaytraceSmoothKernel, anIndex++,
                               sizeof(cl_mem), &myRaytraceNodeDataRcrdBuffer);
    anError |= clSetKernelArg (myRaytraceSmoothKernel, anIndex++,
                               sizeof(cl_mem), &myRaytraceLightSourceBuffer);
    anError |= clSetKernelArg (myRaytraceSmoothKernel, anIndex++,
                               sizeof(cl_mem), &myRaytraceMaterialBuffer);
    anError |= clSetKernelArg (myRaytraceSmoothKernel, anIndex++,
                               sizeof(cl_mem), &myRaytraceVertexBuffer);
    anError |= clSetKernelArg (myRaytraceSmoothKernel, anIndex++,
                               sizeof(cl_mem), &myRaytraceNormalBuffer);
    anError |= clSetKernelArg (myRaytraceSmoothKernel, anIndex++,
                               sizeof(cl_mem), &myRaytraceTriangleBuffer);

    anError |= clSetKernelArg (myRaytraceSmoothKernel, anIndex++,
                               sizeof(cl_float16), theOrigins);
    anError |= clSetKernelArg (myRaytraceSmoothKernel, anIndex++,
                                sizeof(cl_float16), theDirects);

    anError |= clSetKernelArg (myRaytraceSmoothKernel, anIndex++,
                               sizeof(cl_int),   &aLightCount);
    anError |= clSetKernelArg (myRaytraceSmoothKernel, anIndex++,
                               sizeof(cl_float), &myRaytraceSceneEpsilon);
    anError |= clSetKernelArg (myRaytraceSmoothKernel, anIndex++,
                               sizeof(cl_float), &myRaytraceSceneRadius);
    anError |= clSetKernelArg (myRaytraceSmoothKernel, anIndex++,
                               sizeof(cl_int),   &theCView.IsShadowsEnabled);
    anError |= clSetKernelArg (myRaytraceSmoothKernel, anIndex++,
                               sizeof(cl_int),   &theCView.IsReflectionsEnabled);
    anError |= clSetKernelArg (myRaytraceSmoothKernel, anIndex++,
                               sizeof(cl_int),   &theSizeX);
    anError |= clSetKernelArg (myRaytraceSmoothKernel, anIndex++,
                               sizeof(cl_int),   &theSizeY);
    if (anError != CL_SUCCESS)
    {
      const TCollection_ExtendedString aMsg = "Error! Failed to set arguments of 'smoothing' kernel!";
      myGlContext->PushMessage (GL_DEBUG_SOURCE_APPLICATION_ARB,
                                GL_DEBUG_TYPE_ERROR_ARB,
                                0,
                                GL_DEBUG_SEVERITY_HIGH_ARB,
                                aMsg);
      return Standard_False;
    }
  }

  // Set work size
  size_t aLocSizeRender[] = { myIsAmdComputePlatform ? 2 : 4, 32 };

#ifdef OPENCL_GROUP_SIZE_TEST
  for (size_t aLocX = 2; aLocX <= 32; aLocX <<= 1 )
  for (size_t aLocY = 2; aLocY <= 32; aLocY <<= 1 )
#endif
  {
#ifdef OPENCL_GROUP_SIZE_TEST
    aLocSizeRender[0] = aLocX;
    aLocSizeRender[1] = aLocY;
#endif

    size_t aWorkSizeX = theSizeX;
    if (aWorkSizeX % aLocSizeRender[0] != 0)
      aWorkSizeX += aLocSizeRender[0] - aWorkSizeX % aLocSizeRender[0];

    size_t aWokrSizeY = theSizeY;
    if (aWokrSizeY % aLocSizeRender[1] != 0 )
      aWokrSizeY += aLocSizeRender[1] - aWokrSizeY % aLocSizeRender[1];

    size_t aGlbSizeRender[] = { aWorkSizeX, aWokrSizeY };

    // Run kernel
    cl_event anEvent (NULL), anEventSmooth (NULL);
    anError = clEnqueueNDRangeKernel (myRaytraceQueue, myRaytraceRenderKernel,
                                      2, NULL, aGlbSizeRender, aLocSizeRender,
                                      0, NULL, &anEvent);
    if (anError != CL_SUCCESS)
    {
      const TCollection_ExtendedString aMsg = "Error! Failed to execute the ray-tracing kernel!";
      myGlContext->PushMessage (GL_DEBUG_SOURCE_APPLICATION_ARB,
                                GL_DEBUG_TYPE_ERROR_ARB,
                                0,
                                GL_DEBUG_SEVERITY_HIGH_ARB,
                                aMsg);
      return Standard_False;
    }
    clWaitForEvents (1, &anEvent);

    if (theCView.IsAntialiasingEnabled)
    {
      size_t aLocSizeSmooth[] = { myIsAmdComputePlatform ? 8 : 4,
                                  myIsAmdComputePlatform ? 8 : 32 };

#ifdef OPENCL_GROUP_SIZE_TEST
      aLocSizeSmooth[0] = aLocX;
      aLocSizeSmooth[1] = aLocY;
#endif

      aWorkSizeX = theSizeX;
      if (aWorkSizeX % aLocSizeSmooth[0] != 0)
        aWorkSizeX += aLocSizeSmooth[0] - aWorkSizeX % aLocSizeSmooth[0];

      size_t aWokrSizeY = theSizeY;
      if (aWokrSizeY % aLocSizeSmooth[1] != 0 )
        aWokrSizeY += aLocSizeSmooth[1] - aWokrSizeY % aLocSizeSmooth[1];

      size_t aGlbSizeSmooth [] = { aWorkSizeX, aWokrSizeY };
      anError = clEnqueueNDRangeKernel (myRaytraceQueue, myRaytraceSmoothKernel,
                                        2, NULL, aGlbSizeSmooth, aLocSizeSmooth,
                                        0, NULL, &anEventSmooth);
      clWaitForEvents (1, &anEventSmooth);

      if (anError != CL_SUCCESS)
      {
        const TCollection_ExtendedString aMsg = "Error! Failed to execute the 'smoothing' kernel!";
        myGlContext->PushMessage (GL_DEBUG_SOURCE_APPLICATION_ARB,
                                  GL_DEBUG_TYPE_ERROR_ARB,
                                  0,
                                  GL_DEBUG_SEVERITY_HIGH_ARB,
                                  aMsg);
        return Standard_False;
      }
    }

    // Get the profiling data
#if defined (RAY_TRACE_PRINT_INFO) || defined(OPENCL_GROUP_SIZE_TEST)

    cl_ulong aTimeStart,
             aTimeFinal;

    clGetEventProfilingInfo (anEvent, CL_PROFILING_COMMAND_START,
                             sizeof(aTimeStart), &aTimeStart, NULL);
    clGetEventProfilingInfo (anEvent, CL_PROFILING_COMMAND_END,
                             sizeof(aTimeFinal), &aTimeFinal, NULL);
    std::cout << "\tRender time (ms): " << ( aTimeFinal - aTimeStart ) / 1e6f << std::endl;

    if (theCView.IsAntialiasingEnabled)
    {
      clGetEventProfilingInfo (anEventSmooth, CL_PROFILING_COMMAND_START,
                               sizeof(aTimeStart), &aTimeStart, NULL);
      clGetEventProfilingInfo (anEventSmooth, CL_PROFILING_COMMAND_END,
                               sizeof(aTimeFinal), &aTimeFinal, NULL);
      std::cout << "\tSmoothing time (ms): " << ( aTimeFinal - aTimeStart ) / 1e6f << std::endl;
    }
#endif

    if (anEvent != NULL)
      clReleaseEvent (anEvent);

    if (anEventSmooth != NULL)
      clReleaseEvent (anEventSmooth);
  }

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

  for (int i = 0; i < 16; ++i)
    inv[i] *= det;
}

// =======================================================================
// function : GenerateCornerRays
// purpose  : Generates primary rays for corners of screen quad
// =======================================================================
void GenerateCornerRays (const GLdouble theInvModelProj[16],
                         float          theOrigins[16],
                         float          theDirects[16])
{
  int aOriginIndex = 0;
  int aDirectIndex = 0;

  for (int y = -1; y <= 1; y += 2)
  {
    for (int x = -1; x <= 1; x += 2)
    {
      OpenGl_RTVec4f aOrigin (float(x),
                              float(y),
                              -1.f,
                              1.f);

      aOrigin = MatVecMult (theInvModelProj, aOrigin);

      OpenGl_RTVec4f aDirect (float(x),
                              float(y),
                              1.f,
                              1.f);

      aDirect = MatVecMult (theInvModelProj, aDirect) - aOrigin;

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
                                             const int              theSizeX,
                                             int                    theSizeY,
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

  myView->GetMatrices (theOrientation, theViewMapping, Standard_True);

  GLdouble aOrientationMatrix[16];
  GLdouble aViewMappingMatrix[16];
  GLdouble aOrientationInvers[16];

  for (int j = 0; j < 4; ++j)
    for (int i = 0; i < 4; ++i)
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
  cl_mem anImages[] = { myRaytraceOutputImage, myRaytraceOutputImageSmooth };
  cl_int anError = clEnqueueAcquireGLObjects (myRaytraceQueue,
                                              2, anImages,
                                              0, NULL, NULL);
  clFinish (myRaytraceQueue);

  if (myIsRaytraceDataValid)
  {
    RunRaytraceOpenCLKernels (theCView,
                              aOrigins,
                              aDirects,
                              theSizeX,
                              theSizeY);
  }

  anError |= clEnqueueReleaseGLObjects (myRaytraceQueue,
                                        2, anImages,
                                        0, NULL, NULL);
  clFinish (myRaytraceQueue);

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

  glBindTexture (GL_TEXTURE_RECTANGLE, myRaytraceOutputTexture[theCView.IsAntialiasingEnabled ? 1 : 0]);

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
