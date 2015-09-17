// Created on: 2015-02-20
// Created by: Denis BOGOLEPOV
// Copyright (c) 2015 OPEN CASCADE SAS
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

#include <OpenGl_View.hxx>

#include <Graphic3d_TextureParams.hxx>
#include <OpenGl_PrimitiveArray.hxx>
#include <OpenGl_VertexBuffer.hxx>
#include <OSD_Protection.hxx>
#include <OSD_File.hxx>

using namespace OpenGl_Raytrace;

//! Use this macro to output ray-tracing debug info
// #define RAY_TRACE_PRINT_INFO

#ifdef RAY_TRACE_PRINT_INFO
  #include <OSD_Timer.hxx>
#endif

// =======================================================================
// function : updateRaytraceGeometry
// purpose  : Updates 3D scene geometry for ray-tracing
// =======================================================================
Standard_Boolean OpenGl_View::updateRaytraceGeometry (const RaytraceUpdateMode      theMode,
                                                      const Standard_Integer        theViewId,
                                                      const Handle(OpenGl_Context)& theGlContext)
{
  // In 'check' mode (OpenGl_GUM_CHECK) the scene geometry is analyzed for
  // modifications. This is light-weight procedure performed on each frame
  if (theMode == OpenGl_GUM_CHECK)
  {
    if (myLayerListState != myZLayers.ModificationState())
    {
      return updateRaytraceGeometry (OpenGl_GUM_PREPARE, theViewId, theGlContext);
    }
  }
  else if (theMode == OpenGl_GUM_PREPARE)
  {
    myRaytraceGeometry.ClearMaterials();

    myArrayToTrianglesMap.clear();

    myIsRaytraceDataValid = Standard_False;
  }

  // The set of processed structures (reflected to ray-tracing)
  // This set is used to remove out-of-date records from the
  // hash map of structures
  std::set<const OpenGl_Structure*> anElements;

  // Set to store all currently visible OpenGL primitive arrays
  // applicable for ray-tracing
  std::set<Standard_Size> anArrayIDs;

  // Set to store all non-raytracable elements allowing tracking
  // of changes in OpenGL scene (only for path tracing)
  std::set<Standard_Integer> aNonRaytraceIDs;

  const OpenGl_Layer& aLayer = myZLayers.Layer (Graphic3d_ZLayerId_Default);

  if (aLayer.NbStructures() != 0)
  {
    const OpenGl_ArrayOfIndexedMapOfStructure& aStructArray = aLayer.ArrayOfStructures();

    for (Standard_Integer anIndex = 0; anIndex < aStructArray.Length(); ++anIndex)
    {
      for (OpenGl_IndexedMapOfStructure::Iterator aStructIt (aStructArray (anIndex)); aStructIt.More(); aStructIt.Next())
      {
        const OpenGl_Structure* aStructure = aStructIt.Value();

        if (theMode == OpenGl_GUM_CHECK)
        {
          if (toUpdateStructure (aStructure))
          {
            return updateRaytraceGeometry (OpenGl_GUM_PREPARE, theViewId, theGlContext);
          }
          else if (aStructure->IsVisible() && myRaytraceParameters.GlobalIllumination)
          {
            aNonRaytraceIDs.insert (aStructure->highlight ? aStructure->Id : -aStructure->Id);
          }
        }
        else if (theMode == OpenGl_GUM_PREPARE)
        {
          if (!aStructure->IsRaytracable() || !aStructure->IsVisible())
          {
            continue;
          }
          else if (!aStructure->ViewAffinity.IsNull() && !aStructure->ViewAffinity->IsVisible (theViewId))
          {
            continue;
          }

          for (OpenGl_Structure::GroupIterator aGroupIter (aStructure->DrawGroups()); aGroupIter.More(); aGroupIter.Next())
          {
            // Extract OpenGL elements from the group (primitives arrays)
            for (const OpenGl_ElementNode* aNode = aGroupIter.Value()->FirstNode(); aNode != NULL; aNode = aNode->next)
            {
              OpenGl_PrimitiveArray* aPrimArray = dynamic_cast<OpenGl_PrimitiveArray*> (aNode->elem);

              if (aPrimArray != NULL)
              {
                anArrayIDs.insert (aPrimArray->GetUID());
              }
            }
          }
        }
        else if (theMode == OpenGl_GUM_REBUILD)
        {
          if (!aStructure->IsRaytracable())
          {
            continue;
          }
          else if (addRaytraceStructure (aStructure, theGlContext))
          {
            anElements.insert (aStructure); // structure was processed
          }
        }
      }
    }
  }

  if (theMode == OpenGl_GUM_PREPARE)
  {
    BVH_ObjectSet<Standard_ShortReal, 3>::BVH_ObjectList anUnchangedObjects;

    // Filter out unchanged objects so only their transformations and materials
    // will be updated (and newly added objects will be processed from scratch)
    for (Standard_Integer anObjIdx = 0; anObjIdx < myRaytraceGeometry.Size(); ++anObjIdx)
    {
      OpenGl_TriangleSet* aTriangleSet = dynamic_cast<OpenGl_TriangleSet*> (
        myRaytraceGeometry.Objects().ChangeValue (anObjIdx).operator->());

      if (aTriangleSet == NULL)
      {
        continue;
      }

      if (anArrayIDs.find (aTriangleSet->AssociatedPArrayID()) != anArrayIDs.end())
      {
        anUnchangedObjects.Append (myRaytraceGeometry.Objects().Value (anObjIdx));

        myArrayToTrianglesMap[aTriangleSet->AssociatedPArrayID()] = aTriangleSet;
      }
    }

    myRaytraceGeometry.Objects() = anUnchangedObjects;

    return updateRaytraceGeometry (OpenGl_GUM_REBUILD, theViewId, theGlContext);
  }
  else if (theMode == OpenGl_GUM_REBUILD)
  {
    // Actualize the hash map of structures - remove out-of-date records
    std::map<const OpenGl_Structure*, StructState>::iterator anIter = myStructureStates.begin();

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
    myLayerListState = myZLayers.ModificationState();

    // Rebuild two-level acceleration structure
    myRaytraceGeometry.ProcessAcceleration();

    myRaytraceSceneRadius = 2.f /* scale factor */ * std::max (
      myRaytraceGeometry.Box().CornerMin().cwiseAbs().maxComp(),
      myRaytraceGeometry.Box().CornerMax().cwiseAbs().maxComp());

    const BVH_Vec3f aSize = myRaytraceGeometry.Box().Size();

    myRaytraceSceneEpsilon = Max (1.0e-6f, 1.0e-4f * aSize.Modulus());

    return uploadRaytraceData (theGlContext);
  }

  if (myRaytraceParameters.GlobalIllumination)
  {
    Standard_Boolean toRestart =
      aNonRaytraceIDs.size() != myNonRaytraceStructureIDs.size();

    for (std::set<Standard_Integer>::iterator anID = aNonRaytraceIDs.begin(); anID != aNonRaytraceIDs.end() && !toRestart; ++anID)
    {
      if (myNonRaytraceStructureIDs.find (*anID) == myNonRaytraceStructureIDs.end())
      {
        toRestart = Standard_True;
      }
    }

    if (toRestart)
      myAccumFrames = 0;

    myNonRaytraceStructureIDs = aNonRaytraceIDs;
  }

  return Standard_True;
}

// =======================================================================
// function : toUpdateStructure
// purpose  : Checks to see if the structure is modified
// =======================================================================
Standard_Boolean OpenGl_View::toUpdateStructure (const OpenGl_Structure* theStructure)
{
  if (!theStructure->IsRaytracable())
  {
    if (theStructure->ModificationState() > 0)
    {
      theStructure->ResetModificationState();

      return Standard_True; // ray-trace element was removed - need to rebuild
    }

    return Standard_False; // did not contain ray-trace elements
  }

  std::map<const OpenGl_Structure*, StructState>::iterator aStructState = myStructureStates.find (theStructure);

  if (aStructState == myStructureStates.end() || aStructState->second.StructureState != theStructure->ModificationState())
  {
    return Standard_True;
  }
  else if (theStructure->InstancedStructure() != NULL)
  {
    return aStructState->second.InstancedState != theStructure->InstancedStructure()->ModificationState();
  }

  return Standard_False;
}

// =======================================================================
// function : buildTextureTransform
// purpose  : Constructs texture transformation matrix
// =======================================================================
void buildTextureTransform (const Handle(Graphic3d_TextureParams)& theParams, BVH_Mat4f& theMatrix)
{
  theMatrix.InitIdentity();

  // Apply scaling
  const Graphic3d_Vec2& aScale = theParams->Scale();

  theMatrix.ChangeValue (0, 0) *= aScale.x();
  theMatrix.ChangeValue (1, 0) *= aScale.x();
  theMatrix.ChangeValue (2, 0) *= aScale.x();
  theMatrix.ChangeValue (3, 0) *= aScale.x();

  theMatrix.ChangeValue (0, 1) *= aScale.y();
  theMatrix.ChangeValue (1, 1) *= aScale.y();
  theMatrix.ChangeValue (2, 1) *= aScale.y();
  theMatrix.ChangeValue (3, 1) *= aScale.y();

  // Apply translation
  const Graphic3d_Vec2 aTrans = -theParams->Translation();

  theMatrix.ChangeValue (0, 3) = theMatrix.GetValue (0, 0) * aTrans.x() +
                                 theMatrix.GetValue (0, 1) * aTrans.y();

  theMatrix.ChangeValue (1, 3) = theMatrix.GetValue (1, 0) * aTrans.x() +
                                 theMatrix.GetValue (1, 1) * aTrans.y();

  theMatrix.ChangeValue (2, 3) = theMatrix.GetValue (2, 0) * aTrans.x() +
                                 theMatrix.GetValue (2, 1) * aTrans.y();

  // Apply rotation
  const Standard_ShortReal aSin = std::sin (
    -theParams->Rotation() * static_cast<Standard_ShortReal> (M_PI / 180.0));
  const Standard_ShortReal aCos = std::cos (
    -theParams->Rotation() * static_cast<Standard_ShortReal> (M_PI / 180.0));

  BVH_Mat4f aRotationMat;
  aRotationMat.SetValue (0, 0,  aCos);
  aRotationMat.SetValue (1, 1,  aCos);
  aRotationMat.SetValue (0, 1, -aSin);
  aRotationMat.SetValue (1, 0,  aSin);

  theMatrix = theMatrix * aRotationMat;
}

// =======================================================================
// function : convertMaterial
// purpose  : Creates ray-tracing material properties
// =======================================================================
OpenGl_RaytraceMaterial OpenGl_View::convertMaterial (const OpenGl_AspectFace*      theAspect,
                                                      const Handle(OpenGl_Context)& theGlContext)
{
  OpenGl_RaytraceMaterial theMaterial;

  const OPENGL_SURF_PROP& aProperties = theAspect->IntFront();

  theMaterial.Ambient = BVH_Vec4f (
    (aProperties.isphysic ? aProperties.ambcol.rgb[0] : aProperties.matcol.rgb[0]) * aProperties.amb,
    (aProperties.isphysic ? aProperties.ambcol.rgb[1] : aProperties.matcol.rgb[1]) * aProperties.amb,
    (aProperties.isphysic ? aProperties.ambcol.rgb[2] : aProperties.matcol.rgb[2]) * aProperties.amb,
    1.f);

  theMaterial.Diffuse = BVH_Vec4f (
    (aProperties.isphysic ? aProperties.difcol.rgb[0] : aProperties.matcol.rgb[0]) * aProperties.diff,
    (aProperties.isphysic ? aProperties.difcol.rgb[1] : aProperties.matcol.rgb[1]) * aProperties.diff,
    (aProperties.isphysic ? aProperties.difcol.rgb[2] : aProperties.matcol.rgb[2]) * aProperties.diff,
    -1.f /* no texture */);

  theMaterial.Specular = BVH_Vec4f (
    (aProperties.isphysic ? aProperties.speccol.rgb[0] : 1.f) * aProperties.spec,
    (aProperties.isphysic ? aProperties.speccol.rgb[1] : 1.f) * aProperties.spec,
    (aProperties.isphysic ? aProperties.speccol.rgb[2] : 1.f) * aProperties.spec,
    aProperties.shine);

  theMaterial.Emission = BVH_Vec4f (
    (aProperties.isphysic ? aProperties.emscol.rgb[0] : aProperties.matcol.rgb[0]) * aProperties.emsv,
    (aProperties.isphysic ? aProperties.emscol.rgb[1] : aProperties.matcol.rgb[1]) * aProperties.emsv,
    (aProperties.isphysic ? aProperties.emscol.rgb[2] : aProperties.matcol.rgb[2]) * aProperties.emsv,
    1.f);

  theMaterial.Transparency = BVH_Vec4f (aProperties.trans,
                                        1.f - aProperties.trans,
                                        aProperties.index == 0 ? 1.f : aProperties.index,
                                        aProperties.index == 0 ? 1.f : 1.f / aProperties.index);

  const Standard_ShortReal aMaxRefl = Max (theMaterial.Diffuse.x() + theMaterial.Specular.x(),
                                      Max (theMaterial.Diffuse.y() + theMaterial.Specular.y(),
                                           theMaterial.Diffuse.z() + theMaterial.Specular.z()));

  const Standard_ShortReal aReflectionScale = 0.75f / aMaxRefl;

  theMaterial.Reflection = BVH_Vec4f (
    aProperties.speccol.rgb[0] * aProperties.spec * aReflectionScale,
    aProperties.speccol.rgb[1] * aProperties.spec * aReflectionScale,
    aProperties.speccol.rgb[2] * aProperties.spec * aReflectionScale,
    0.f);

  // Serialize physically-based material properties
  const Graphic3d_BSDF& aBSDF = aProperties.BSDF;

  theMaterial.BSDF.Le = BVH_Vec4f (aBSDF.Le,               0.f);
  theMaterial.BSDF.Kd = BVH_Vec4f (aBSDF.Kd, -1.f /* no tex */);
  theMaterial.BSDF.Kr = BVH_Vec4f (aBSDF.Kr,               0.f);
  theMaterial.BSDF.Kt = BVH_Vec4f (aBSDF.Kt,               0.f);
  theMaterial.BSDF.Ks = BVH_Vec4f (aBSDF.Ks,   aBSDF.Roughness);

  theMaterial.BSDF.Fresnel = aBSDF.Fresnel.Serialize();

  theMaterial.BSDF.Absorption = BVH_Vec4f (aBSDF.AbsorptionColor,
                                           aBSDF.AbsorptionCoeff);

  // Handle material textures
  if (theAspect->DoTextureMap())
  {
    if (theGlContext->arbTexBindless != NULL)
    {
      buildTextureTransform (theAspect->TextureParams(), theMaterial.TextureTransform);

      // write texture ID to diffuse w-component
      theMaterial.Diffuse.w() = theMaterial.BSDF.Kd.w() =
        static_cast<Standard_ShortReal> (myRaytraceGeometry.AddTexture (theAspect->TextureRes (theGlContext)));
    }
    else if (!myIsRaytraceWarnTextures)
    {
      const TCollection_ExtendedString aWarnMessage =
        "Warning: texturing in Ray-Trace requires GL_ARB_bindless_texture extension which is missing. "
        "Please try to update graphics card driver. At the moment textures will be ignored.";

      theGlContext->PushMessage (GL_DEBUG_SOURCE_APPLICATION_ARB,
        GL_DEBUG_TYPE_PORTABILITY_ARB, 0, GL_DEBUG_SEVERITY_HIGH_ARB, aWarnMessage);

      myIsRaytraceWarnTextures = Standard_True;
    }
  }

  return theMaterial;
}

// =======================================================================
// function : addRaytraceStructure
// purpose  : Adds OpenGL structure to ray-traced scene geometry
// =======================================================================
Standard_Boolean OpenGl_View::addRaytraceStructure (const OpenGl_Structure*       theStructure,
                                                    const Handle(OpenGl_Context)& theGlContext)
{
  if (!theStructure->IsVisible())
  {
    myStructureStates[theStructure] = StructState (theStructure);

    return Standard_True;
  }

  // Get structure material
  OpenGl_RaytraceMaterial aStructMaterial;

  if (theStructure->AspectFace() != NULL)
  {
    aStructMaterial = convertMaterial (theStructure->AspectFace(), theGlContext);
  }

  Standard_ShortReal aStructTransform[16];

  if (theStructure->Transformation()->mat != NULL)
  {
    for (Standard_Integer i = 0; i < 4; ++i)
    {
      for (Standard_Integer j = 0; j < 4; ++j)
      {
        aStructTransform[j * 4 + i] = theStructure->Transformation()->mat[i][j];
      }
    }
  }

  Standard_Boolean aResult = addRaytraceGroups (theStructure, aStructMaterial,
    theStructure->Transformation()->mat ? aStructTransform : NULL, theGlContext);

  // Process all connected OpenGL structures
  const OpenGl_Structure* anInstanced = theStructure->InstancedStructure();

  if (anInstanced != NULL && anInstanced->IsRaytracable())
  {
    aResult &= addRaytraceGroups (anInstanced, aStructMaterial,
      theStructure->Transformation()->mat ? aStructTransform : NULL, theGlContext);
  }

  myStructureStates[theStructure] = StructState (theStructure);

  return aResult;
}

// =======================================================================
// function : addRaytraceGroups
// purpose  : Adds OpenGL groups to ray-traced scene geometry
// =======================================================================
Standard_Boolean OpenGl_View::addRaytraceGroups (const OpenGl_Structure*        theStructure,
                                                 const OpenGl_RaytraceMaterial& theStructMat,
                                                 const Standard_ShortReal*      theTransform,
                                                 const Handle(OpenGl_Context)&  theGlContext)
{
  for (OpenGl_Structure::GroupIterator aGroupIter (theStructure->DrawGroups()); aGroupIter.More(); aGroupIter.Next())
  {
    // Get group material
    OpenGl_RaytraceMaterial aGroupMaterial;
    if (aGroupIter.Value()->AspectFace() != NULL)
    {
      aGroupMaterial = convertMaterial (
        aGroupIter.Value()->AspectFace(), theGlContext);
    }

    Standard_Integer aMatID = static_cast<Standard_Integer> (myRaytraceGeometry.Materials.size());

    // Use group material if available, otherwise use structure material
    myRaytraceGeometry.Materials.push_back (
      aGroupIter.Value()->AspectFace() != NULL ? aGroupMaterial : theStructMat);

    // Add OpenGL elements from group (extract primitives arrays and aspects)
    for (const OpenGl_ElementNode* aNode = aGroupIter.Value()->FirstNode(); aNode != NULL; aNode = aNode->next)
    {
      OpenGl_AspectFace* anAspect = dynamic_cast<OpenGl_AspectFace*> (aNode->elem);

      if (anAspect != NULL)
      {
        aMatID = static_cast<Standard_Integer> (myRaytraceGeometry.Materials.size());

        OpenGl_RaytraceMaterial aMaterial = convertMaterial (anAspect, theGlContext);

        myRaytraceGeometry.Materials.push_back (aMaterial);
      }
      else
      {
        OpenGl_PrimitiveArray* aPrimArray = dynamic_cast<OpenGl_PrimitiveArray*> (aNode->elem);

        if (aPrimArray != NULL)
        {
          std::map<Standard_Size, OpenGl_TriangleSet*>::iterator aSetIter = myArrayToTrianglesMap.find (aPrimArray->GetUID());

          if (aSetIter != myArrayToTrianglesMap.end())
          {
            OpenGl_TriangleSet* aSet = aSetIter->second;

            BVH_Transform<Standard_ShortReal, 4>* aTransform = new BVH_Transform<Standard_ShortReal, 4>();

            if (theTransform != NULL)
            {
              aTransform->SetTransform (*(reinterpret_cast<const BVH_Mat4f*> (theTransform)));
            }

            aSet->SetProperties (aTransform);

            if (aSet->MaterialIndex() != OpenGl_TriangleSet::INVALID_MATERIAL && aSet->MaterialIndex() != aMatID)
            {
              aSet->SetMaterialIndex (aMatID);
            }
          }
          else
          {
            NCollection_Handle<BVH_Object<Standard_ShortReal, 3> > aSet =
              addRaytracePrimitiveArray (aPrimArray, aMatID, 0);

            if (!aSet.IsNull())
            {
              BVH_Transform<Standard_ShortReal, 4>* aTransform = new BVH_Transform<Standard_ShortReal, 4>;

              if (theTransform != NULL)
              {
                aTransform->SetTransform (*(reinterpret_cast<const BVH_Mat4f*> (theTransform)));
              }

              aSet->SetProperties (aTransform);

              myRaytraceGeometry.Objects().Append (aSet);
            }
          }
        }
      }
    }
  }

  return Standard_True;
}

// =======================================================================
// function : addRaytracePrimitiveArray
// purpose  : Adds OpenGL primitive array to ray-traced scene geometry
// =======================================================================
OpenGl_TriangleSet* OpenGl_View::addRaytracePrimitiveArray (const OpenGl_PrimitiveArray* theArray,
                                                            const Standard_Integer       theMaterial,
                                                            const OpenGl_Mat4*           theTransform)
{
  const Handle(Graphic3d_BoundBuffer)& aBounds   = theArray->Bounds();
  const Handle(Graphic3d_IndexBuffer)& anIndices = theArray->Indices();
  const Handle(Graphic3d_Buffer)&      anAttribs = theArray->Attributes();

  if (theArray->DrawMode() < GL_TRIANGLES
  #ifndef GL_ES_VERSION_2_0
   || theArray->DrawMode() > GL_POLYGON
  #else
   || theArray->DrawMode() > GL_TRIANGLE_FAN
  #endif
   || anAttribs.IsNull())
  {
    return NULL;
  }

  OpenGl_Mat4 aNormalMatrix;

  if (theTransform != NULL)
  {
    Standard_ASSERT_RETURN (theTransform->Inverted (aNormalMatrix),
      "Error: Failed to compute normal transformation matrix", NULL);

    aNormalMatrix.Transpose();
  }

  OpenGl_TriangleSet* aSet = new OpenGl_TriangleSet (theArray->GetUID());
  {
    aSet->Vertices.reserve (anAttribs->NbElements);
    aSet->Normals.reserve  (anAttribs->NbElements);
    aSet->TexCrds.reserve  (anAttribs->NbElements);

    const size_t aVertFrom = aSet->Vertices.size();

    for (Standard_Integer anAttribIter = 0; anAttribIter < anAttribs->NbAttributes; ++anAttribIter)
    {
      const Graphic3d_Attribute& anAttrib = anAttribs->Attribute       (anAttribIter);
      const size_t               anOffset = anAttribs->AttributeOffset (anAttribIter);
      if (anAttrib.Id == Graphic3d_TOA_POS)
      {
        if (anAttrib.DataType == Graphic3d_TOD_VEC3
         || anAttrib.DataType == Graphic3d_TOD_VEC4)
        {
          for (Standard_Integer aVertIter = 0; aVertIter < anAttribs->NbElements; ++aVertIter)
          {
            aSet->Vertices.push_back (
              *reinterpret_cast<const Graphic3d_Vec3*> (anAttribs->value (aVertIter) + anOffset));
          }
        }
        else if (anAttrib.DataType == Graphic3d_TOD_VEC2)
        {
          for (Standard_Integer aVertIter = 0; aVertIter < anAttribs->NbElements; ++aVertIter)
          {
            const Standard_ShortReal* aCoords =
              reinterpret_cast<const Standard_ShortReal*> (anAttribs->value (aVertIter) + anOffset);

            aSet->Vertices.push_back (BVH_Vec3f (aCoords[0], aCoords[1], 0.0f));
          }
        }
      }
      else if (anAttrib.Id == Graphic3d_TOA_NORM)
      {
        if (anAttrib.DataType == Graphic3d_TOD_VEC3
         || anAttrib.DataType == Graphic3d_TOD_VEC4)
        {
          for (Standard_Integer aVertIter = 0; aVertIter < anAttribs->NbElements; ++aVertIter)
          {
            aSet->Normals.push_back (
              *reinterpret_cast<const Graphic3d_Vec3*> (anAttribs->value (aVertIter) + anOffset));
          }
        }
      }
      else if (anAttrib.Id == Graphic3d_TOA_UV)
      {
        if (anAttrib.DataType == Graphic3d_TOD_VEC2)
        {
          for (Standard_Integer aVertIter = 0; aVertIter < anAttribs->NbElements; ++aVertIter)
          {
            aSet->TexCrds.push_back (
              *reinterpret_cast<const Graphic3d_Vec2*> (anAttribs->value (aVertIter) + anOffset));
          }
        }
      }
    }

    if (aSet->Normals.size() != aSet->Vertices.size())
    {
      for (Standard_Integer aVertIter = 0; aVertIter < anAttribs->NbElements; ++aVertIter)
      {
        aSet->Normals.push_back (BVH_Vec3f());
      }
    }

    if (aSet->TexCrds.size() != aSet->Vertices.size())
    {
      for (Standard_Integer aVertIter = 0; aVertIter < anAttribs->NbElements; ++aVertIter)
      {
        aSet->TexCrds.push_back (BVH_Vec2f());
      }
    }

    if (theTransform != NULL)
    {
      for (size_t aVertIter = aVertFrom; aVertIter < aSet->Vertices.size(); ++aVertIter)
      {
        BVH_Vec3f& aVertex = aSet->Vertices[aVertIter];

        BVH_Vec4f aTransVertex = *theTransform *
          BVH_Vec4f (aVertex.x(), aVertex.y(), aVertex.z(), 1.f);

        aVertex = BVH_Vec3f (aTransVertex.x(), aTransVertex.y(), aTransVertex.z());
      }
      for (size_t aVertIter = aVertFrom; aVertIter < aSet->Normals.size(); ++aVertIter)
      {
        BVH_Vec3f& aNormal = aSet->Normals[aVertIter];

        BVH_Vec4f aTransNormal = aNormalMatrix *
          BVH_Vec4f (aNormal.x(), aNormal.y(), aNormal.z(), 0.f);

        aNormal = BVH_Vec3f (aTransNormal.x(), aTransNormal.y(), aTransNormal.z());
      }
    }

    if (!aBounds.IsNull())
    {
      for (Standard_Integer aBound = 0, aBoundStart = 0; aBound < aBounds->NbBounds; ++aBound)
      {
        const Standard_Integer aVertNum = aBounds->Bounds[aBound];

        if (!addRaytraceVertexIndices (*aSet, theMaterial, aVertNum, aBoundStart, *theArray))
        {
          delete aSet;
          return NULL;
        }

        aBoundStart += aVertNum;
      }
    }
    else
    {
      const Standard_Integer aVertNum = !anIndices.IsNull() ? anIndices->NbElements : anAttribs->NbElements;

      if (!addRaytraceVertexIndices (*aSet, theMaterial, aVertNum, 0, *theArray))
      {
        delete aSet;
        return NULL;
      }
    }
  }

  if (aSet->Size() != 0)
  {
    aSet->MarkDirty();
  }

  return aSet;
}

// =======================================================================
// function : addRaytraceVertexIndices
// purpose  : Adds vertex indices to ray-traced scene geometry
// =======================================================================
Standard_Boolean OpenGl_View::addRaytraceVertexIndices (OpenGl_TriangleSet&                  theSet,
                                                        const Standard_Integer               theMatID,
                                                        const Standard_Integer               theCount,
                                                        const Standard_Integer               theOffset,
                                                        const OpenGl_PrimitiveArray&         theArray)
{
  switch (theArray.DrawMode())
  {
    case GL_TRIANGLES:      return addRaytraceTriangleArray        (theSet, theMatID, theCount, theOffset, theArray.Indices());
    case GL_TRIANGLE_FAN:   return addRaytraceTriangleFanArray     (theSet, theMatID, theCount, theOffset, theArray.Indices());
    case GL_TRIANGLE_STRIP: return addRaytraceTriangleStripArray   (theSet, theMatID, theCount, theOffset, theArray.Indices());
  #if !defined(GL_ES_VERSION_2_0)
    case GL_QUAD_STRIP:     return addRaytraceQuadrangleStripArray (theSet, theMatID, theCount, theOffset, theArray.Indices());
    case GL_QUADS:          return addRaytraceQuadrangleArray      (theSet, theMatID, theCount, theOffset, theArray.Indices());
    case GL_POLYGON:        return addRaytracePolygonArray         (theSet, theMatID, theCount, theOffset, theArray.Indices());
  #endif
  }

  return Standard_False;
}

// =======================================================================
// function : addRaytraceTriangleArray
// purpose  : Adds OpenGL triangle array to ray-traced scene geometry
// =======================================================================
Standard_Boolean OpenGl_View::addRaytraceTriangleArray (OpenGl_TriangleSet&                  theSet,
                                                        const Standard_Integer               theMatID,
                                                        const Standard_Integer               theCount,
                                                        const Standard_Integer               theOffset,
                                                        const Handle(Graphic3d_IndexBuffer)& theIndices)
{
  if (theCount < 3)
  {
    return Standard_True;
  }

  theSet.Elements.reserve (theSet.Elements.size() + theCount / 3);

  if (!theIndices.IsNull())
  {
    for (Standard_Integer aVert = theOffset; aVert < theOffset + theCount - 2; aVert += 3)
    {
      theSet.Elements.push_back (BVH_Vec4i (theIndices->Index (aVert + 0),
                                            theIndices->Index (aVert + 1),
                                            theIndices->Index (aVert + 2),
                                            theMatID));
    }
  }
  else
  {
    for (Standard_Integer aVert = theOffset; aVert < theOffset + theCount - 2; aVert += 3)
    {
      theSet.Elements.push_back (BVH_Vec4i (aVert + 0, aVert + 1, aVert + 2, theMatID));
    }
  }

  return Standard_True;
}

// =======================================================================
// function : addRaytraceTriangleFanArray
// purpose  : Adds OpenGL triangle fan array to ray-traced scene geometry
// =======================================================================
Standard_Boolean OpenGl_View::addRaytraceTriangleFanArray (OpenGl_TriangleSet&                  theSet,
                                                           const Standard_Integer               theMatID,
                                                           const Standard_Integer               theCount,
                                                           const Standard_Integer               theOffset,
                                                           const Handle(Graphic3d_IndexBuffer)& theIndices)
{
  if (theCount < 3)
  {
    return Standard_True;
  }

  theSet.Elements.reserve (theSet.Elements.size() + theCount - 2);

  if (!theIndices.IsNull())
  {
    for (Standard_Integer aVert = theOffset; aVert < theOffset + theCount - 2; ++aVert)
    {
      theSet.Elements.push_back (BVH_Vec4i (theIndices->Index (theOffset),
                                            theIndices->Index (aVert + 1),
                                            theIndices->Index (aVert + 2),
                                            theMatID));
    }
  }
  else
  {
    for (Standard_Integer aVert = theOffset; aVert < theOffset + theCount - 2; ++aVert)
    {
      theSet.Elements.push_back (BVH_Vec4i (theOffset,
                                            aVert + 1,
                                            aVert + 2,
                                            theMatID));
    }
  }

  return Standard_True;
}

// =======================================================================
// function : addRaytraceTriangleStripArray
// purpose  : Adds OpenGL triangle strip array to ray-traced scene geometry
// =======================================================================
Standard_Boolean OpenGl_View::addRaytraceTriangleStripArray (OpenGl_TriangleSet&                  theSet,
                                                             const Standard_Integer               theMatID,
                                                             const Standard_Integer               theCount,
                                                             const Standard_Integer               theOffset,
                                                             const Handle(Graphic3d_IndexBuffer)& theIndices)
{
  if (theCount < 3)
  {
    return Standard_True;
  }

  theSet.Elements.reserve (theSet.Elements.size() + theCount - 2);

  if (!theIndices.IsNull())
  {
    for (Standard_Integer aVert = theOffset, aCW = 0; aVert < theOffset + theCount - 2; ++aVert, aCW = (aCW + 1) % 2)
    {
      theSet.Elements.push_back (BVH_Vec4i (theIndices->Index (aVert + aCW ? 1 : 0),
                                            theIndices->Index (aVert + aCW ? 0 : 1),
                                            theIndices->Index (aVert + 2),
                                            theMatID));
    }
  }
  else
  {
    for (Standard_Integer aVert = theOffset, aCW = 0; aVert < theOffset + theCount - 2; ++aVert, aCW = (aCW + 1) % 2)
    {
      theSet.Elements.push_back (BVH_Vec4i (aVert + aCW ? 1 : 0,
                                            aVert + aCW ? 0 : 1,
                                            aVert + 2,
                                            theMatID));
    }
  }

  return Standard_True;
}

// =======================================================================
// function : addRaytraceQuadrangleArray
// purpose  : Adds OpenGL quad array to ray-traced scene geometry
// =======================================================================
Standard_Boolean OpenGl_View::addRaytraceQuadrangleArray (OpenGl_TriangleSet&                  theSet,
                                                          const Standard_Integer               theMatID,
                                                          const Standard_Integer               theCount,
                                                          const Standard_Integer               theOffset,
                                                          const Handle(Graphic3d_IndexBuffer)& theIndices)
{
  if (theCount < 4)
  {
    return Standard_True;
  }

  theSet.Elements.reserve (theSet.Elements.size() + theCount / 2);

  if (!theIndices.IsNull())
  {
    for (Standard_Integer aVert = theOffset; aVert < theOffset + theCount - 3; aVert += 4)
    {
      theSet.Elements.push_back (BVH_Vec4i (theIndices->Index (aVert + 0),
                                            theIndices->Index (aVert + 1),
                                            theIndices->Index (aVert + 2),
                                            theMatID));
      theSet.Elements.push_back (BVH_Vec4i (theIndices->Index (aVert + 0),
                                            theIndices->Index (aVert + 2),
                                            theIndices->Index (aVert + 3),
                                            theMatID));
    }
  }
  else
  {
    for (Standard_Integer aVert = theOffset; aVert < theOffset + theCount - 3; aVert += 4)
    {
      theSet.Elements.push_back (BVH_Vec4i (aVert + 0, aVert + 1, aVert + 2,
                                            theMatID));
      theSet.Elements.push_back (BVH_Vec4i (aVert + 0, aVert + 2, aVert + 3,
                                            theMatID));
    }
  }

  return Standard_True;
}

// =======================================================================
// function : addRaytraceQuadrangleStripArray
// purpose  : Adds OpenGL quad strip array to ray-traced scene geometry
// =======================================================================
Standard_Boolean OpenGl_View::addRaytraceQuadrangleStripArray (OpenGl_TriangleSet&                  theSet,
                                                               const Standard_Integer               theMatID,
                                                               const Standard_Integer               theCount,
                                                               const Standard_Integer               theOffset,
                                                               const Handle(Graphic3d_IndexBuffer)& theIndices)
{
  if (theCount < 4)
  {
    return Standard_True;
  }

  theSet.Elements.reserve (theSet.Elements.size() + 2 * theCount - 6);

  if (!theIndices.IsNull())
  {
    for (Standard_Integer aVert = theOffset; aVert < theOffset + theCount - 3; aVert += 2)
    {
      theSet.Elements.push_back (BVH_Vec4i (theIndices->Index (aVert + 0),
                                            theIndices->Index (aVert + 1),
                                            theIndices->Index (aVert + 2),
                                            theMatID));

      theSet.Elements.push_back (BVH_Vec4i (theIndices->Index (aVert + 1),
                                            theIndices->Index (aVert + 3),
                                            theIndices->Index (aVert + 2),
                                            theMatID));
    }
  }
  else
  {
    for (Standard_Integer aVert = theOffset; aVert < theOffset + theCount - 3; aVert += 2)
    {
      theSet.Elements.push_back (BVH_Vec4i (aVert + 0,
                                            aVert + 1,
                                            aVert + 2,
                                            theMatID));

      theSet.Elements.push_back (BVH_Vec4i (aVert + 1,
                                            aVert + 3,
                                            aVert + 2,
                                            theMatID));
    }
  }

  return Standard_True;
}

// =======================================================================
// function : addRaytracePolygonArray
// purpose  : Adds OpenGL polygon array to ray-traced scene geometry
// =======================================================================
Standard_Boolean OpenGl_View::addRaytracePolygonArray (OpenGl_TriangleSet&                  theSet,
                                                       const Standard_Integer               theMatID,
                                                       const Standard_Integer               theCount,
                                                       const Standard_Integer               theOffset,
                                                       const Handle(Graphic3d_IndexBuffer)& theIndices)
{
  if (theCount < 3)
  {
    return Standard_True;
  }

  theSet.Elements.reserve (theSet.Elements.size() + theCount - 2);

  if (!theIndices.IsNull())
  {
    for (Standard_Integer aVert = theOffset; aVert < theOffset + theCount - 2; ++aVert)
    {
      theSet.Elements.push_back (BVH_Vec4i (theIndices->Index (theOffset),
                                            theIndices->Index (aVert + 1),
                                            theIndices->Index (aVert + 2),
                                            theMatID));
    }
  }
  else
  {
    for (Standard_Integer aVert = theOffset; aVert < theOffset + theCount - 2; ++aVert)
    {
      theSet.Elements.push_back (BVH_Vec4i (theOffset,
                                            aVert + 1,
                                            aVert + 2,
                                            theMatID));
    }
  }

  return Standard_True;
}

const TCollection_AsciiString OpenGl_View::ShaderSource::EMPTY_PREFIX;

// =======================================================================
// function : Source
// purpose  : Returns shader source combined with prefix
// =======================================================================
TCollection_AsciiString OpenGl_View::ShaderSource::Source() const
{
  const TCollection_AsciiString aVersion = "#version 140";

  if (myPrefix.IsEmpty())
  {
    return aVersion + "\n" + mySource;
  }

  return aVersion + "\n" + myPrefix + "\n" + mySource;
}

// =======================================================================
// function : Load
// purpose  : Loads shader source from specified files
// =======================================================================
Standard_Boolean OpenGl_View::ShaderSource::Load (const TCollection_AsciiString* theFileNames,
                                                  const TCollection_AsciiString& thePrefix)
{
  myError.Clear();
  mySource.Clear();
  TCollection_AsciiString aMissingFiles;
  for (Standard_Integer anIndex = 0; !theFileNames[anIndex].IsEmpty(); ++anIndex)
  {
    OSD_File aFile (theFileNames[anIndex]);
    if (aFile.Exists())
    {
      aFile.Open (OSD_ReadOnly, OSD_Protection());
    }
    if (!aFile.IsOpen())
    {
      if (!aMissingFiles.IsEmpty())
      {
        aMissingFiles += ", ";
      }
      aMissingFiles += TCollection_AsciiString("'") + theFileNames[anIndex] + "'";
      continue;
    }
    else if (!aMissingFiles.IsEmpty())
    {
      aFile.Close();
      continue;
    }

    TCollection_AsciiString aSource;
    aFile.Read (aSource, (Standard_Integer) aFile.Size());
    if (!aSource.IsEmpty())
    {
      mySource += TCollection_AsciiString ("\n") + aSource;
    }
    aFile.Close();
  }

  myPrefix = thePrefix;
  if (!aMissingFiles.IsEmpty())
  {
    myError = TCollection_AsciiString("Shader files ") + aMissingFiles + " are missing or inaccessible";
    return Standard_False;
  }
  return Standard_True;
}

// =======================================================================
// function : generateShaderPrefix
// purpose  : Generates shader prefix based on current ray-tracing options
// =======================================================================
TCollection_AsciiString OpenGl_View::generateShaderPrefix (const Handle(OpenGl_Context)& theGlContext) const
{
  TCollection_AsciiString aPrefixString =
    TCollection_AsciiString ("#define STACK_SIZE ") + TCollection_AsciiString (myRaytraceParameters.StackSize) + "\n" +
    TCollection_AsciiString ("#define NB_BOUNCES ") + TCollection_AsciiString (myRaytraceParameters.NbBounces);

  if (myRaytraceParameters.TransparentShadows)
  {
    aPrefixString += TCollection_AsciiString ("\n#define TRANSPARENT_SHADOWS");
  }

  // If OpenGL driver supports bindless textures and texturing
  // is actually used, activate texturing in ray-tracing mode
  if (myRaytraceParameters.UseBindlessTextures && theGlContext->arbTexBindless != NULL)
  {
    aPrefixString += TCollection_AsciiString ("\n#define USE_TEXTURES") +
      TCollection_AsciiString ("\n#define MAX_TEX_NUMBER ") + TCollection_AsciiString (OpenGl_RaytraceGeometry::MAX_TEX_NUMBER);
  }

  if (myRaytraceParameters.GlobalIllumination)
  {
    aPrefixString += TCollection_AsciiString ("\n#define PATH_TRACING");
  }

  return aPrefixString;
}

// =======================================================================
// function : safeFailBack
// purpose  : Performs safe exit when shaders initialization fails
// =======================================================================
Standard_Boolean OpenGl_View::safeFailBack (const TCollection_ExtendedString& theMessage,
                                            const Handle(OpenGl_Context)&     theGlContext)
{
  theGlContext->PushMessage (GL_DEBUG_SOURCE_APPLICATION_ARB,
    GL_DEBUG_TYPE_ERROR_ARB, 0, GL_DEBUG_SEVERITY_HIGH_ARB, theMessage);

  myRaytraceInitStatus = OpenGl_RT_FAIL;

  releaseRaytraceResources (theGlContext);

  return Standard_False;
}

// =======================================================================
// function : initShader
// purpose  : Creates new shader object with specified source
// =======================================================================
Handle(OpenGl_ShaderObject) OpenGl_View::initShader (const GLenum                  theType,
                                                     const ShaderSource&           theSource,
                                                     const Handle(OpenGl_Context)& theGlContext)
{
  Handle(OpenGl_ShaderObject) aShader = new OpenGl_ShaderObject (theType);

  if (!aShader->Create (theGlContext))
  {
    const TCollection_ExtendedString aMessage = TCollection_ExtendedString ("Error: Failed to create ") +
      (theType == GL_VERTEX_SHADER ? "vertex" : "fragment") + " shader object";

    theGlContext->PushMessage (GL_DEBUG_SOURCE_APPLICATION_ARB,
      GL_DEBUG_TYPE_ERROR_ARB, 0, GL_DEBUG_SEVERITY_HIGH_ARB, aMessage);

    aShader->Release (theGlContext.operator->());

    return Handle(OpenGl_ShaderObject)();
  }

  if (!aShader->LoadSource (theGlContext, theSource.Source()))
  {
    const TCollection_ExtendedString aMessage = TCollection_ExtendedString ("Error: Failed to set ") +
      (theType == GL_VERTEX_SHADER ? "vertex" : "fragment") + " shader source";

    theGlContext->PushMessage (GL_DEBUG_SOURCE_APPLICATION_ARB,
      GL_DEBUG_TYPE_ERROR_ARB, 0, GL_DEBUG_SEVERITY_HIGH_ARB, aMessage);

    aShader->Release (theGlContext.operator->());

    return Handle(OpenGl_ShaderObject)();
  }

  TCollection_AsciiString aBuildLog;

  if (!aShader->Compile (theGlContext))
  {
    aShader->FetchInfoLog (theGlContext, aBuildLog);

    const TCollection_ExtendedString aMessage = TCollection_ExtendedString ("Error: Failed to compile ") +
      (theType == GL_VERTEX_SHADER ? "vertex" : "fragment") + " shader object:\n" + aBuildLog;

    theGlContext->PushMessage (GL_DEBUG_SOURCE_APPLICATION_ARB,
      GL_DEBUG_TYPE_ERROR_ARB, 0, GL_DEBUG_SEVERITY_HIGH_ARB, aMessage);

    aShader->Release (theGlContext.operator->());

    return Handle(OpenGl_ShaderObject)();
  }
  else if (theGlContext->caps->glslWarnings)
  {
    aShader->FetchInfoLog (theGlContext, aBuildLog);

    if (!aBuildLog.IsEmpty() && !aBuildLog.IsEqual ("No errors.\n"))
    {
      const TCollection_ExtendedString aMessage = TCollection_ExtendedString (theType == GL_VERTEX_SHADER ?
        "Vertex" : "Fragment") + " shader was compiled with following warnings:\n" + aBuildLog;

      theGlContext->PushMessage (GL_DEBUG_SOURCE_APPLICATION_ARB,
        GL_DEBUG_TYPE_PORTABILITY_ARB, 0, GL_DEBUG_SEVERITY_LOW_ARB, aMessage);
    }
  }

  return aShader;
}

// =======================================================================
// function : initProgram
// purpose  : Creates GLSL program from the given shader objects
// =======================================================================
Handle(OpenGl_ShaderProgram) OpenGl_View::initProgram (const Handle(OpenGl_Context)&      theGlContext,
                                                       const Handle(OpenGl_ShaderObject)& theVertShader,
                                                       const Handle(OpenGl_ShaderObject)& theFragShader)
{
  Handle(OpenGl_ShaderProgram) aProgram = new OpenGl_ShaderProgram;

  if (!aProgram->Create (theGlContext))
  {
    theVertShader->Release (theGlContext.operator->());

    theGlContext->PushMessage (GL_DEBUG_SOURCE_APPLICATION_ARB,
      GL_DEBUG_TYPE_ERROR_ARB, 0, GL_DEBUG_SEVERITY_HIGH_ARB, "Failed to create shader program");

    return Handle(OpenGl_ShaderProgram)();
  }

  if (!aProgram->AttachShader (theGlContext, theVertShader)
   || !aProgram->AttachShader (theGlContext, theFragShader))
  {
    theVertShader->Release (theGlContext.operator->());

    theGlContext->PushMessage (GL_DEBUG_SOURCE_APPLICATION_ARB,
      GL_DEBUG_TYPE_ERROR_ARB, 0, GL_DEBUG_SEVERITY_HIGH_ARB, "Failed to attach shader objects");

    return Handle(OpenGl_ShaderProgram)();
  }

  aProgram->SetAttributeName (theGlContext, Graphic3d_TOA_POS, "occVertex");

  TCollection_AsciiString aLinkLog;

  if (!aProgram->Link (theGlContext))
  {
    aProgram->FetchInfoLog (theGlContext, aLinkLog);

    const TCollection_ExtendedString aMessage = TCollection_ExtendedString (
      "Failed to link shader program:\n") + aLinkLog;

    theGlContext->PushMessage (GL_DEBUG_SOURCE_APPLICATION_ARB,
      GL_DEBUG_TYPE_ERROR_ARB, 0, GL_DEBUG_SEVERITY_HIGH_ARB, aMessage);

    return Handle(OpenGl_ShaderProgram)();
  }
  else if (theGlContext->caps->glslWarnings)
  {
    aProgram->FetchInfoLog (theGlContext, aLinkLog);
    if (!aLinkLog.IsEmpty() && !aLinkLog.IsEqual ("No errors.\n"))
    {
      const TCollection_ExtendedString aMessage = TCollection_ExtendedString (
        "Shader program was linked with following warnings:\n") + aLinkLog;

      theGlContext->PushMessage (GL_DEBUG_SOURCE_APPLICATION_ARB,
        GL_DEBUG_TYPE_PORTABILITY_ARB, 0, GL_DEBUG_SEVERITY_LOW_ARB, aMessage);
    }
  }

  return aProgram;
}

// =======================================================================
// function : initRaytraceResources
// purpose  : Initializes OpenGL/GLSL shader programs
// =======================================================================
Standard_Boolean OpenGl_View::initRaytraceResources (const Handle(OpenGl_Context)& theGlContext)
{
  if (myRaytraceInitStatus == OpenGl_RT_FAIL)
  {
    return Standard_False;
  }

  Standard_Boolean aToRebuildShaders = Standard_False;

  if (myRaytraceInitStatus == OpenGl_RT_INIT)
  {
    if (!myIsRaytraceDataValid)
      return Standard_True;

    const Standard_Integer aRequiredStackSize =
      myRaytraceGeometry.HighLevelTreeDepth() + myRaytraceGeometry.BottomLevelTreeDepth();

    if (myRaytraceParameters.StackSize < aRequiredStackSize)
    {
      myRaytraceParameters.StackSize = Max (aRequiredStackSize, THE_DEFAULT_STACK_SIZE);

      aToRebuildShaders = Standard_True;
    }
    else
    {
      if (aRequiredStackSize < myRaytraceParameters.StackSize)
      {
        if (myRaytraceParameters.StackSize > THE_DEFAULT_STACK_SIZE)
        {
          myRaytraceParameters.StackSize = Max (aRequiredStackSize, THE_DEFAULT_STACK_SIZE);
          aToRebuildShaders = Standard_True;
        }
      }
    }

    if (myRenderParams.RaytracingDepth != myRaytraceParameters.NbBounces)
    {
      myRaytraceParameters.NbBounces = myRenderParams.RaytracingDepth;
      aToRebuildShaders = Standard_True;
    }

    if (myRaytraceGeometry.HasTextures() != myRaytraceParameters.UseBindlessTextures)
    {
      myRaytraceParameters.UseBindlessTextures = myRaytraceGeometry.HasTextures();
      aToRebuildShaders = Standard_True;
    }

    if (myRenderParams.IsTransparentShadowEnabled != myRaytraceParameters.TransparentShadows)
    {
      myRaytraceParameters.TransparentShadows = myRenderParams.IsTransparentShadowEnabled;
      aToRebuildShaders = Standard_True;
    }

    if (myRenderParams.IsGlobalIlluminationEnabled != myRaytraceParameters.GlobalIllumination)
    {
      myRaytraceParameters.GlobalIllumination = myRenderParams.IsGlobalIlluminationEnabled;
      aToRebuildShaders = Standard_True;
    }

    if (aToRebuildShaders)
    {
      // Reject accumulated frames
      myAccumFrames = 0;

      // We need to update environment texture
      myToUpdateEnvironmentMap = Standard_True;

      TCollection_AsciiString aPrefixString = generateShaderPrefix (theGlContext);

#ifdef RAY_TRACE_PRINT_INFO
      std::cout << "GLSL prefix string:" << std::endl << aPrefixString << std::endl;
#endif

      myRaytraceShaderSource.SetPrefix (aPrefixString);
      myPostFSAAShaderSource.SetPrefix (aPrefixString);

      if (!myRaytraceShader->LoadSource (theGlContext, myRaytraceShaderSource.Source())
       || !myPostFSAAShader->LoadSource (theGlContext, myPostFSAAShaderSource.Source()))
      {
        return safeFailBack ("Failed to load source into ray-tracing fragment shaders", theGlContext);
      }

      if (!myRaytraceShader->Compile (theGlContext)
       || !myPostFSAAShader->Compile (theGlContext))
      {
        return safeFailBack ("Failed to compile ray-tracing fragment shaders", theGlContext);
      }

      myRaytraceProgram->SetAttributeName (theGlContext, Graphic3d_TOA_POS, "occVertex");
      myPostFSAAProgram->SetAttributeName (theGlContext, Graphic3d_TOA_POS, "occVertex");
      if (!myRaytraceProgram->Link (theGlContext)
       || !myPostFSAAProgram->Link (theGlContext))
      {
        return safeFailBack ("Failed to initialize vertex attributes for ray-tracing program", theGlContext);
      }
    }
  }

  if (myRaytraceInitStatus == OpenGl_RT_NONE)
  {
    if (!theGlContext->IsGlGreaterEqual (3, 1))
    {
      return safeFailBack ("Ray-tracing requires OpenGL 3.1 and higher", theGlContext);
    }
    else if (!theGlContext->arbTboRGB32)
    {
      return safeFailBack ("Ray-tracing requires OpenGL 4.0+ or GL_ARB_texture_buffer_object_rgb32 extension", theGlContext);
    }
    else if (!theGlContext->arbFBOBlit)
    {
      return safeFailBack ("Ray-tracing requires EXT_framebuffer_blit extension", theGlContext);
    }

    myRaytraceParameters.NbBounces = myRenderParams.RaytracingDepth;

    TCollection_AsciiString aFolder = Graphic3d_ShaderProgram::ShadersFolder();

    if (aFolder.IsEmpty())
    {
      return safeFailBack ("Failed to locate shaders directory", theGlContext);
    }

    if (myIsRaytraceDataValid)
    {
      myRaytraceParameters.StackSize = Max (THE_DEFAULT_STACK_SIZE,
        myRaytraceGeometry.HighLevelTreeDepth() + myRaytraceGeometry.BottomLevelTreeDepth());
    }

    TCollection_AsciiString aPrefixString  = generateShaderPrefix (theGlContext);

#ifdef RAY_TRACE_PRINT_INFO
    std::cout << "GLSL prefix string:" << std::endl << aPrefixString << std::endl;
#endif

    ShaderSource aBasicVertShaderSrc;
    {
      TCollection_AsciiString aFiles[] = { aFolder + "/RaytraceBase.vs", "" };
      if (!aBasicVertShaderSrc.Load (aFiles))
      {
        return safeFailBack (aBasicVertShaderSrc.ErrorDescription(), theGlContext);
      }
    }

    {
      TCollection_AsciiString aFiles[] = { aFolder + "/RaytraceBase.fs",
                                           aFolder + "/PathtraceBase.fs",
                                           aFolder + "/RaytraceRender.fs",
                                           "" };
      if (!myRaytraceShaderSource.Load (aFiles, aPrefixString))
      {
        return safeFailBack (myRaytraceShaderSource.ErrorDescription(), theGlContext);
      }

      Handle(OpenGl_ShaderObject) aBasicVertShader = initShader (GL_VERTEX_SHADER, aBasicVertShaderSrc, theGlContext);
      if (aBasicVertShader.IsNull())
      {
        return safeFailBack ("Failed to initialize ray-trace vertex shader", theGlContext);
      }

      myRaytraceShader = initShader (GL_FRAGMENT_SHADER, myRaytraceShaderSource, theGlContext);
      if (myRaytraceShader.IsNull())
      {
        aBasicVertShader->Release (theGlContext.operator->());
        return safeFailBack ("Failed to initialize ray-trace fragment shader", theGlContext);
      }

      myRaytraceProgram = initProgram (theGlContext, aBasicVertShader, myRaytraceShader);
      if (myRaytraceProgram.IsNull())
      {
        return safeFailBack ("Failed to initialize ray-trace shader program", theGlContext);
      }
    }

    {
      TCollection_AsciiString aFiles[] = { aFolder + "/RaytraceBase.fs",
                                           aFolder + "/RaytraceSmooth.fs",
                                           "" };
      if (!myPostFSAAShaderSource.Load (aFiles, aPrefixString))
      {
        return safeFailBack (myPostFSAAShaderSource.ErrorDescription(), theGlContext);
      }

      Handle(OpenGl_ShaderObject) aBasicVertShader = initShader (GL_VERTEX_SHADER, aBasicVertShaderSrc, theGlContext);
      if (aBasicVertShader.IsNull())
      {
        return safeFailBack ("Failed to initialize FSAA vertex shader", theGlContext);
      }

      myPostFSAAShader = initShader (GL_FRAGMENT_SHADER, myPostFSAAShaderSource, theGlContext);
      if (myPostFSAAShader.IsNull())
      {
        aBasicVertShader->Release (theGlContext.operator->());
        return safeFailBack ("Failed to initialize FSAA fragment shader", theGlContext);
      }

      myPostFSAAProgram = initProgram (theGlContext, aBasicVertShader, myPostFSAAShader);
      if (myPostFSAAProgram.IsNull())
      {
        return safeFailBack ("Failed to initialize FSAA shader program", theGlContext);
      }
    }

    {
      ShaderSource aDispShaderSrc;
      TCollection_AsciiString aFiles[] = { aFolder + "/Display.fs", "" };
      if (!aDispShaderSrc.Load (aFiles, aPrefixString))
      {
        return safeFailBack (aDispShaderSrc.ErrorDescription(), theGlContext);
      }

      Handle(OpenGl_ShaderObject) aBasicVertShader = initShader (GL_VERTEX_SHADER, aBasicVertShaderSrc, theGlContext);
      if (aBasicVertShader.IsNull())
      {
        return safeFailBack ("Failed to set vertex shader source", theGlContext);
      }

      Handle(OpenGl_ShaderObject) aDisplayShader = initShader (GL_FRAGMENT_SHADER, aDispShaderSrc, theGlContext);
      if (aDisplayShader.IsNull())
      {
        aBasicVertShader->Release (theGlContext.operator->());
        return safeFailBack ("Failed to set display fragment shader source", theGlContext);
      }

      myOutImageProgram = initProgram (theGlContext, aBasicVertShader, aDisplayShader);
      if (myOutImageProgram.IsNull())
      {
        return safeFailBack ("Failed to initialize output shader program", theGlContext);
      }
    }
  }

  if (myRaytraceInitStatus == OpenGl_RT_NONE || aToRebuildShaders)
  {
    for (Standard_Integer anIndex = 0; anIndex < 2; ++anIndex)
    {
      Handle(OpenGl_ShaderProgram)& aShaderProgram =
        (anIndex == 0) ? myRaytraceProgram : myPostFSAAProgram;

      theGlContext->BindProgram (aShaderProgram);

      aShaderProgram->SetSampler (theGlContext,
        "uSceneMinPointTexture", OpenGl_RT_SceneMinPointTexture);
      aShaderProgram->SetSampler (theGlContext,
        "uSceneMaxPointTexture", OpenGl_RT_SceneMaxPointTexture);
      aShaderProgram->SetSampler (theGlContext,
        "uSceneNodeInfoTexture", OpenGl_RT_SceneNodeInfoTexture);
      aShaderProgram->SetSampler (theGlContext,
        "uGeometryVertexTexture", OpenGl_RT_GeometryVertexTexture);
      aShaderProgram->SetSampler (theGlContext,
        "uGeometryNormalTexture", OpenGl_RT_GeometryNormalTexture);
      aShaderProgram->SetSampler (theGlContext,
        "uGeometryTexCrdTexture", OpenGl_RT_GeometryTexCrdTexture);
      aShaderProgram->SetSampler (theGlContext,
        "uGeometryTriangTexture", OpenGl_RT_GeometryTriangTexture);
      aShaderProgram->SetSampler (theGlContext, 
        "uSceneTransformTexture", OpenGl_RT_SceneTransformTexture);
      aShaderProgram->SetSampler (theGlContext,
        "uEnvironmentMapTexture", OpenGl_RT_EnvironmentMapTexture);
      aShaderProgram->SetSampler (theGlContext,
        "uRaytraceMaterialTexture", OpenGl_RT_RaytraceMaterialTexture);
      aShaderProgram->SetSampler (theGlContext,
        "uRaytraceLightSrcTexture", OpenGl_RT_RaytraceLightSrcTexture);

      aShaderProgram->SetSampler (theGlContext,
        "uOpenGlColorTexture", OpenGl_RT_OpenGlColorTexture);
      aShaderProgram->SetSampler (theGlContext,
        "uOpenGlDepthTexture", OpenGl_RT_OpenGlDepthTexture);

      if (anIndex == 1)
      {
        aShaderProgram->SetSampler (theGlContext,
          "uFSAAInputTexture", OpenGl_RT_FsaaInputTexture);
      }
      else
      {
        aShaderProgram->SetSampler (theGlContext,
          "uAccumTexture", OpenGl_RT_PrevAccumTexture);
      }

      myUniformLocations[anIndex][OpenGl_RT_aPosition] =
        aShaderProgram->GetAttributeLocation (theGlContext, "occVertex");

      myUniformLocations[anIndex][OpenGl_RT_uOriginLB] =
        aShaderProgram->GetUniformLocation (theGlContext, "uOriginLB");
      myUniformLocations[anIndex][OpenGl_RT_uOriginRB] =
        aShaderProgram->GetUniformLocation (theGlContext, "uOriginRB");
      myUniformLocations[anIndex][OpenGl_RT_uOriginLT] =
        aShaderProgram->GetUniformLocation (theGlContext, "uOriginLT");
      myUniformLocations[anIndex][OpenGl_RT_uOriginRT] =
        aShaderProgram->GetUniformLocation (theGlContext, "uOriginRT");
      myUniformLocations[anIndex][OpenGl_RT_uDirectLB] =
        aShaderProgram->GetUniformLocation (theGlContext, "uDirectLB");
      myUniformLocations[anIndex][OpenGl_RT_uDirectRB] =
        aShaderProgram->GetUniformLocation (theGlContext, "uDirectRB");
      myUniformLocations[anIndex][OpenGl_RT_uDirectLT] =
        aShaderProgram->GetUniformLocation (theGlContext, "uDirectLT");
      myUniformLocations[anIndex][OpenGl_RT_uDirectRT] =
        aShaderProgram->GetUniformLocation (theGlContext, "uDirectRT");
      myUniformLocations[anIndex][OpenGl_RT_uUnviewMat] =
        aShaderProgram->GetUniformLocation (theGlContext, "uUnviewMat");

      myUniformLocations[anIndex][OpenGl_RT_uSceneRad] =
        aShaderProgram->GetUniformLocation (theGlContext, "uSceneRadius");
      myUniformLocations[anIndex][OpenGl_RT_uSceneEps] =
        aShaderProgram->GetUniformLocation (theGlContext, "uSceneEpsilon");
      myUniformLocations[anIndex][OpenGl_RT_uLightCount] =
        aShaderProgram->GetUniformLocation (theGlContext, "uLightCount");
      myUniformLocations[anIndex][OpenGl_RT_uLightAmbnt] =
        aShaderProgram->GetUniformLocation (theGlContext, "uGlobalAmbient");

      myUniformLocations[anIndex][OpenGl_RT_uOffsetX] =
        aShaderProgram->GetUniformLocation (theGlContext, "uOffsetX");
      myUniformLocations[anIndex][OpenGl_RT_uOffsetY] =
        aShaderProgram->GetUniformLocation (theGlContext, "uOffsetY");
      myUniformLocations[anIndex][OpenGl_RT_uSamples] =
        aShaderProgram->GetUniformLocation (theGlContext, "uSamples");

      myUniformLocations[anIndex][OpenGl_RT_uTexSamplersArray] =
        aShaderProgram->GetUniformLocation (theGlContext, "uTextureSamplers");

      myUniformLocations[anIndex][OpenGl_RT_uShadowsEnabled] =
        aShaderProgram->GetUniformLocation (theGlContext, "uShadowsEnabled");
      myUniformLocations[anIndex][OpenGl_RT_uReflectEnabled] =
        aShaderProgram->GetUniformLocation (theGlContext, "uReflectEnabled");
      myUniformLocations[anIndex][OpenGl_RT_uSphereMapEnabled] =
        aShaderProgram->GetUniformLocation (theGlContext, "uSphereMapEnabled");
      myUniformLocations[anIndex][OpenGl_RT_uSphereMapForBack] =
        aShaderProgram->GetUniformLocation (theGlContext, "uSphereMapForBack");
      myUniformLocations[anIndex][OpenGl_RT_uBlockedRngEnabled] =
        aShaderProgram->GetUniformLocation (theGlContext, "uBlockedRngEnabled");

      myUniformLocations[anIndex][OpenGl_RT_uSampleWeight] =
        aShaderProgram->GetUniformLocation (theGlContext, "uSampleWeight");
      myUniformLocations[anIndex][OpenGl_RT_uFrameRndSeed] =
        aShaderProgram->GetUniformLocation (theGlContext, "uFrameRndSeed");

      myUniformLocations[anIndex][OpenGl_RT_uBackColorTop] =
        aShaderProgram->GetUniformLocation (theGlContext, "uBackColorTop");
      myUniformLocations[anIndex][OpenGl_RT_uBackColorBot] =
        aShaderProgram->GetUniformLocation (theGlContext, "uBackColorBot");
    }

    theGlContext->BindProgram (myOutImageProgram);

    myOutImageProgram->SetSampler (theGlContext,
      "uInputTexture", OpenGl_RT_PrevAccumTexture);

    theGlContext->BindProgram (NULL);
  }

  if (myRaytraceInitStatus != OpenGl_RT_NONE)
  {
    return myRaytraceInitStatus == OpenGl_RT_INIT;
  }

  if (myRaytraceFBO1.IsNull())
  {
    myRaytraceFBO1 = new OpenGl_FrameBuffer (GL_RGBA32F);
  }

  if (myRaytraceFBO2.IsNull())
  {
    myRaytraceFBO2 = new OpenGl_FrameBuffer (GL_RGBA32F);
  }

  const GLfloat aVertices[] = { -1.f, -1.f,  0.f,
                                -1.f,  1.f,  0.f,
                                 1.f,  1.f,  0.f,
                                 1.f,  1.f,  0.f,
                                 1.f, -1.f,  0.f,
                                -1.f, -1.f,  0.f };

  myRaytraceScreenQuad.Init (theGlContext, 3, 6, aVertices);

  myRaytraceInitStatus = OpenGl_RT_INIT; // initialized in normal way

  return Standard_True;
}

// =======================================================================
// function : nullifyResource
// purpose  :
// =======================================================================
inline void nullifyResource (const Handle(OpenGl_Context)& theGlContext,
                             Handle(OpenGl_Resource)&      theResource)
{
  if (!theResource.IsNull())
  {
    theResource->Release (theGlContext.operator->());
    theResource.Nullify();
  }
}

// =======================================================================
// function : releaseRaytraceResources
// purpose  : Releases OpenGL/GLSL shader programs
// =======================================================================
void OpenGl_View::releaseRaytraceResources (const Handle(OpenGl_Context)& theGlContext)
{
  nullifyResource (theGlContext, myOpenGlFBO);
  nullifyResource (theGlContext, myRaytraceFBO1);
  nullifyResource (theGlContext, myRaytraceFBO2);

  nullifyResource (theGlContext, myRaytraceShader);
  nullifyResource (theGlContext, myPostFSAAShader);

  nullifyResource (theGlContext, myRaytraceProgram);
  nullifyResource (theGlContext, myPostFSAAProgram);
  nullifyResource (theGlContext, myOutImageProgram);

  nullifyResource (theGlContext, mySceneNodeInfoTexture);
  nullifyResource (theGlContext, mySceneMinPointTexture);
  nullifyResource (theGlContext, mySceneMaxPointTexture);

  nullifyResource (theGlContext, myGeometryVertexTexture);
  nullifyResource (theGlContext, myGeometryNormalTexture);
  nullifyResource (theGlContext, myGeometryTexCrdTexture);
  nullifyResource (theGlContext, myGeometryTriangTexture);
  nullifyResource (theGlContext, mySceneTransformTexture);

  nullifyResource (theGlContext, myRaytraceLightSrcTexture);
  nullifyResource (theGlContext, myRaytraceMaterialTexture);

  myRaytraceGeometry.ReleaseResources (theGlContext);

  if (myRaytraceScreenQuad.IsValid())
    myRaytraceScreenQuad.Release (theGlContext.operator->());
}

// =======================================================================
// function : resizeRaytraceBuffers
// purpose  : Resizes OpenGL frame buffers
// =======================================================================
Standard_Boolean OpenGl_View::resizeRaytraceBuffers (const Standard_Integer        theSizeX,
                                                     const Standard_Integer        theSizeY,
                                                     const Handle(OpenGl_Context)& theGlContext)
{
  if (myRaytraceFBO1->GetVPSizeX() != theSizeX
   || myRaytraceFBO1->GetVPSizeY() != theSizeY)
  {
    myRaytraceFBO1->Init (theGlContext, theSizeX, theSizeY);
    myRaytraceFBO2->Init (theGlContext, theSizeX, theSizeY);
  }

  return Standard_True;
}

// =======================================================================
// function : updateCamera
// purpose  : Generates viewing rays for corners of screen quad
// =======================================================================
void OpenGl_View::updateCamera (const OpenGl_Mat4& theOrientation,
                                const OpenGl_Mat4& theViewMapping,
                                OpenGl_Vec3*       theOrigins,
                                OpenGl_Vec3*       theDirects,
                                OpenGl_Mat4&       theUnview)
{
  // compute inverse model-view-projection matrix
  (theViewMapping * theOrientation).Inverted (theUnview);

  Standard_Integer aOriginIndex = 0;
  Standard_Integer aDirectIndex = 0;

  for (Standard_Integer aY = -1; aY <= 1; aY += 2)
  {
    for (Standard_Integer aX = -1; aX <= 1; aX += 2)
    {
      OpenGl_Vec4 aOrigin (GLfloat(aX),
                           GLfloat(aY),
                          -1.0f,
                           1.0f);

      aOrigin = theUnview * aOrigin;

      aOrigin.x() = aOrigin.x() / aOrigin.w();
      aOrigin.y() = aOrigin.y() / aOrigin.w();
      aOrigin.z() = aOrigin.z() / aOrigin.w();

      OpenGl_Vec4 aDirect (GLfloat(aX),
                           GLfloat(aY),
                           1.0f,
                           1.0f);

      aDirect = theUnview * aDirect;

      aDirect.x() = aDirect.x() / aDirect.w();
      aDirect.y() = aDirect.y() / aDirect.w();
      aDirect.z() = aDirect.z() / aDirect.w();

      aDirect = aDirect - aOrigin;

      theOrigins[aOriginIndex++] = OpenGl_Vec3 (static_cast<GLfloat> (aOrigin.x()),
                                                static_cast<GLfloat> (aOrigin.y()),
                                                static_cast<GLfloat> (aOrigin.z()));

      theDirects[aDirectIndex++] = OpenGl_Vec3 (static_cast<GLfloat> (aDirect.x()),
                                                static_cast<GLfloat> (aDirect.y()),
                                                static_cast<GLfloat> (aDirect.z()));
    }
  }
}

// =======================================================================
// function : uploadRaytraceData
// purpose  : Uploads ray-trace data to the GPU
// =======================================================================
Standard_Boolean OpenGl_View::uploadRaytraceData (const Handle(OpenGl_Context)& theGlContext)
{
  if (!theGlContext->IsGlGreaterEqual (3, 1))
  {
#ifdef RAY_TRACE_PRINT_INFO
    std::cout << "Error: OpenGL version is less than 3.1" << std::endl;
#endif
    return Standard_False;
  }

  myAccumFrames = 0; // accumulation should be restarted

  /////////////////////////////////////////////////////////////////////////////
  // Prepare OpenGL textures

  if (theGlContext->arbTexBindless != NULL)
  {
    // If OpenGL driver supports bindless textures we need
    // to get unique 64- bit handles for using on the GPU
    if (!myRaytraceGeometry.UpdateTextureHandles (theGlContext))
    {
#ifdef RAY_TRACE_PRINT_INFO
      std::cout << "Error: Failed to get OpenGL texture handles" << std::endl;
#endif
      return Standard_False;
    }
  }

  /////////////////////////////////////////////////////////////////////////////
  // Create OpenGL BVH buffers

  if (mySceneNodeInfoTexture.IsNull())  // create scene BVH buffers
  {
    mySceneNodeInfoTexture  = new OpenGl_TextureBufferArb;
    mySceneMinPointTexture  = new OpenGl_TextureBufferArb;
    mySceneMaxPointTexture  = new OpenGl_TextureBufferArb;
    mySceneTransformTexture = new OpenGl_TextureBufferArb;

    if (!mySceneNodeInfoTexture->Create  (theGlContext)
     || !mySceneMinPointTexture->Create  (theGlContext)
     || !mySceneMaxPointTexture->Create  (theGlContext)
     || !mySceneTransformTexture->Create (theGlContext))
    {
#ifdef RAY_TRACE_PRINT_INFO
      std::cout << "Error: Failed to create scene BVH buffers" << std::endl;
#endif
      return Standard_False;
    }
  }

  if  (myGeometryVertexTexture.IsNull())  // create geometry buffers
  {
    myGeometryVertexTexture = new OpenGl_TextureBufferArb;
    myGeometryNormalTexture = new OpenGl_TextureBufferArb;
    myGeometryTexCrdTexture = new OpenGl_TextureBufferArb;
    myGeometryTriangTexture = new OpenGl_TextureBufferArb;

    if (!myGeometryVertexTexture->Create (theGlContext)
     || !myGeometryNormalTexture->Create (theGlContext)
     || !myGeometryTexCrdTexture->Create (theGlContext)
     || !myGeometryTriangTexture->Create (theGlContext))
    {
#ifdef RAY_TRACE_PRINT_INFO
      std::cout << "Error: Failed to create buffers for triangulation data" << std::endl;
#endif
      return Standard_False;
    }
  }

  if (myRaytraceMaterialTexture.IsNull()) // create material buffer
  {
    myRaytraceMaterialTexture = new OpenGl_TextureBufferArb;

    if (!myRaytraceMaterialTexture->Create (theGlContext))
    {
#ifdef RAY_TRACE_PRINT_INFO
      std::cout << "Error: Failed to create buffers for material data" << std::endl;
#endif
      return Standard_False;
    }
  }
  
  /////////////////////////////////////////////////////////////////////////////
  // Write transform buffer

  BVH_Mat4f* aNodeTransforms = new BVH_Mat4f[myRaytraceGeometry.Size()];

  bool aResult = true;

  for (Standard_Integer anElemIndex = 0; anElemIndex < myRaytraceGeometry.Size(); ++anElemIndex)
  {
    OpenGl_TriangleSet* aTriangleSet = dynamic_cast<OpenGl_TriangleSet*> (
      myRaytraceGeometry.Objects().ChangeValue (anElemIndex).operator->());

    const BVH_Transform<Standard_ShortReal, 4>* aTransform = 
      dynamic_cast<const BVH_Transform<Standard_ShortReal, 4>* > (aTriangleSet->Properties().operator->());

    Standard_ASSERT_RETURN (aTransform != NULL,
      "OpenGl_TriangleSet does not contain transform", Standard_False);

    aNodeTransforms[anElemIndex] = aTransform->Inversed();
  }

  aResult &= mySceneTransformTexture->Init (theGlContext, 4,
    myRaytraceGeometry.Size() * 4, reinterpret_cast<const GLfloat*> (aNodeTransforms));

  delete [] aNodeTransforms;

  /////////////////////////////////////////////////////////////////////////////
  // Write geometry and bottom-level BVH buffers

  Standard_Size aTotalVerticesNb = 0;
  Standard_Size aTotalElementsNb = 0;
  Standard_Size aTotalBVHNodesNb = 0;

  for (Standard_Integer anElemIndex = 0; anElemIndex < myRaytraceGeometry.Size(); ++anElemIndex)
  {
    OpenGl_TriangleSet* aTriangleSet = dynamic_cast<OpenGl_TriangleSet*> (
      myRaytraceGeometry.Objects().ChangeValue (anElemIndex).operator->());

    Standard_ASSERT_RETURN (aTriangleSet != NULL,
      "Error: Failed to get triangulation of OpenGL element", Standard_False);

    aTotalVerticesNb += aTriangleSet->Vertices.size();
    aTotalElementsNb += aTriangleSet->Elements.size();

    Standard_ASSERT_RETURN (!aTriangleSet->BVH().IsNull(),
      "Error: Failed to get bottom-level BVH of OpenGL element", Standard_False);

    aTotalBVHNodesNb += aTriangleSet->BVH()->NodeInfoBuffer().size();
  }

  aTotalBVHNodesNb += myRaytraceGeometry.BVH()->NodeInfoBuffer().size();

  if (aTotalBVHNodesNb != 0)
  {
    aResult &= mySceneNodeInfoTexture->Init (
      theGlContext, 4, GLsizei (aTotalBVHNodesNb), static_cast<const GLuint*>  (NULL));
    aResult &= mySceneMinPointTexture->Init (
      theGlContext, 3, GLsizei (aTotalBVHNodesNb), static_cast<const GLfloat*> (NULL));
    aResult &= mySceneMaxPointTexture->Init (
      theGlContext, 3, GLsizei (aTotalBVHNodesNb), static_cast<const GLfloat*> (NULL));
  }

  if (!aResult)
  {
#ifdef RAY_TRACE_PRINT_INFO
    std::cout << "Error: Failed to upload buffers for bottom-level scene BVH" << std::endl;
#endif
    return Standard_False;
  }

  if (aTotalElementsNb != 0)
  {
    aResult &= myGeometryTriangTexture->Init (
      theGlContext, 4, GLsizei (aTotalElementsNb), static_cast<const GLuint*> (NULL));
  }

  if (aTotalVerticesNb != 0)
  {
    aResult &= myGeometryVertexTexture->Init (
      theGlContext, 3, GLsizei (aTotalVerticesNb), static_cast<const GLfloat*> (NULL));
    aResult &= myGeometryNormalTexture->Init (
      theGlContext, 3, GLsizei (aTotalVerticesNb), static_cast<const GLfloat*> (NULL));
    aResult &= myGeometryTexCrdTexture->Init (
      theGlContext, 2, GLsizei (aTotalVerticesNb), static_cast<const GLfloat*> (NULL));
  }

  if (!aResult)
  {
#ifdef RAY_TRACE_PRINT_INFO
    std::cout << "Error: Failed to upload buffers for scene geometry" << std::endl;
#endif
    return Standard_False;
  }

  const NCollection_Handle<BVH_Tree<Standard_ShortReal, 3> >& aBVH = myRaytraceGeometry.BVH();

  if (aBVH->Length() > 0)
  {
    aResult &= mySceneNodeInfoTexture->SubData (theGlContext, 0, aBVH->Length(),
      reinterpret_cast<const GLuint*> (&aBVH->NodeInfoBuffer().front()));
    aResult &= mySceneMinPointTexture->SubData (theGlContext, 0, aBVH->Length(),
      reinterpret_cast<const GLfloat*> (&aBVH->MinPointBuffer().front()));
    aResult &= mySceneMaxPointTexture->SubData (theGlContext, 0, aBVH->Length(),
      reinterpret_cast<const GLfloat*> (&aBVH->MaxPointBuffer().front()));
  }

  for (Standard_Integer aNodeIdx = 0; aNodeIdx < aBVH->Length(); ++aNodeIdx)
  {
    if (!aBVH->IsOuter (aNodeIdx))
      continue;

    OpenGl_TriangleSet* aTriangleSet = myRaytraceGeometry.TriangleSet (aNodeIdx);

    Standard_ASSERT_RETURN (aTriangleSet != NULL,
      "Error: Failed to get triangulation of OpenGL element", Standard_False);

    Standard_Integer aBVHOffset = myRaytraceGeometry.AccelerationOffset (aNodeIdx);

    Standard_ASSERT_RETURN (aBVHOffset != OpenGl_RaytraceGeometry::INVALID_OFFSET,
      "Error: Failed to get offset for bottom-level BVH", Standard_False);

    const Standard_Integer aBvhBuffersSize = aTriangleSet->BVH()->Length();

    if (aBvhBuffersSize != 0)
    {
      aResult &= mySceneNodeInfoTexture->SubData (theGlContext, aBVHOffset, aBvhBuffersSize,
        reinterpret_cast<const GLuint*> (&aTriangleSet->BVH()->NodeInfoBuffer().front()));
      aResult &= mySceneMinPointTexture->SubData (theGlContext, aBVHOffset, aBvhBuffersSize,
        reinterpret_cast<const GLfloat*> (&aTriangleSet->BVH()->MinPointBuffer().front()));
      aResult &= mySceneMaxPointTexture->SubData (theGlContext, aBVHOffset, aBvhBuffersSize,
        reinterpret_cast<const GLfloat*> (&aTriangleSet->BVH()->MaxPointBuffer().front()));

      if (!aResult)
      {
#ifdef RAY_TRACE_PRINT_INFO
        std::cout << "Error: Failed to upload buffers for bottom-level scene BVHs" << std::endl;
#endif
        return Standard_False;
      }
    }

    const Standard_Integer aVerticesOffset = myRaytraceGeometry.VerticesOffset (aNodeIdx);

    Standard_ASSERT_RETURN (aVerticesOffset != OpenGl_RaytraceGeometry::INVALID_OFFSET,
      "Error: Failed to get offset for triangulation vertices of OpenGL element", Standard_False);

    if (!aTriangleSet->Vertices.empty())
    {
      aResult &= myGeometryNormalTexture->SubData (theGlContext, aVerticesOffset,
        GLsizei (aTriangleSet->Normals.size()), reinterpret_cast<const GLfloat*> (&aTriangleSet->Normals.front()));
      aResult &= myGeometryTexCrdTexture->SubData (theGlContext, aVerticesOffset,
        GLsizei (aTriangleSet->TexCrds.size()), reinterpret_cast<const GLfloat*> (&aTriangleSet->TexCrds.front()));
      aResult &= myGeometryVertexTexture->SubData (theGlContext, aVerticesOffset,
        GLsizei (aTriangleSet->Vertices.size()), reinterpret_cast<const GLfloat*> (&aTriangleSet->Vertices.front()));
    }

    const Standard_Integer anElementsOffset = myRaytraceGeometry.ElementsOffset (aNodeIdx);

    Standard_ASSERT_RETURN (anElementsOffset != OpenGl_RaytraceGeometry::INVALID_OFFSET,
      "Error: Failed to get offset for triangulation elements of OpenGL element", Standard_False);

    if (!aTriangleSet->Elements.empty())
    {
      aResult &= myGeometryTriangTexture->SubData (theGlContext, anElementsOffset, GLsizei (aTriangleSet->Elements.size()),
                                                   reinterpret_cast<const GLuint*> (&aTriangleSet->Elements.front()));
    }

    if (!aResult)
    {
#ifdef RAY_TRACE_PRINT_INFO
      std::cout << "Error: Failed to upload triangulation buffers for OpenGL element" << std::endl;
#endif
      return Standard_False;
    }
  }

  /////////////////////////////////////////////////////////////////////////////
  // Write material buffer

  if (myRaytraceGeometry.Materials.size() != 0)
  {
    aResult &= myRaytraceMaterialTexture->Init (theGlContext, 4,
      GLsizei (myRaytraceGeometry.Materials.size() * 18), myRaytraceGeometry.Materials.front().Packed());

    if (!aResult)
    {
#ifdef RAY_TRACE_PRINT_INFO
      std::cout << "Error: Failed to upload material buffer" << std::endl;
#endif
      return Standard_False;
    }
  }

  myIsRaytraceDataValid = myRaytraceGeometry.Objects().Size() != 0;

#ifdef RAY_TRACE_PRINT_INFO

  Standard_ShortReal aMemUsed = 0.f;

  for (Standard_Integer anElemIdx = 0; anElemIdx < myRaytraceGeometry.Size(); ++anElemIdx)
  {
    OpenGl_TriangleSet* aTriangleSet = dynamic_cast<OpenGl_TriangleSet*> (
      myRaytraceGeometry.Objects().ChangeValue (anElemIdx).operator->());

    aMemUsed += static_cast<Standard_ShortReal> (
      aTriangleSet->Vertices.size() * sizeof (BVH_Vec3f));
    aMemUsed += static_cast<Standard_ShortReal> (
      aTriangleSet->Normals.size() * sizeof (BVH_Vec3f));
    aMemUsed += static_cast<Standard_ShortReal> (
      aTriangleSet->TexCrds.size() * sizeof (BVH_Vec2f));
    aMemUsed += static_cast<Standard_ShortReal> (
      aTriangleSet->Elements.size() * sizeof (BVH_Vec4i));

    aMemUsed += static_cast<Standard_ShortReal> (
      aTriangleSet->BVH()->NodeInfoBuffer().size() * sizeof (BVH_Vec4i));
    aMemUsed += static_cast<Standard_ShortReal> (
      aTriangleSet->BVH()->MinPointBuffer().size() * sizeof (BVH_Vec3f));
    aMemUsed += static_cast<Standard_ShortReal> (
      aTriangleSet->BVH()->MaxPointBuffer().size() * sizeof (BVH_Vec3f));
  }

  aMemUsed += static_cast<Standard_ShortReal> (
    myRaytraceGeometry.BVH()->NodeInfoBuffer().size() * sizeof (BVH_Vec4i));
  aMemUsed += static_cast<Standard_ShortReal> (
    myRaytraceGeometry.BVH()->MinPointBuffer().size() * sizeof (BVH_Vec3f));
  aMemUsed += static_cast<Standard_ShortReal> (
    myRaytraceGeometry.BVH()->MaxPointBuffer().size() * sizeof (BVH_Vec3f));

  std::cout << "GPU Memory Used (MB): ~" << aMemUsed / 1048576 << std::endl;

#endif

  return aResult;
}

// =======================================================================
// function : updateRaytraceLightSources
// purpose  : Updates 3D scene light sources for ray-tracing
// =======================================================================
Standard_Boolean OpenGl_View::updateRaytraceLightSources (const OpenGl_Mat4& theInvModelView, const Handle(OpenGl_Context)& theGlContext)
{
  myRaytraceGeometry.Sources.clear();

  myRaytraceGeometry.Ambient = BVH_Vec4f (0.0f, 0.0f, 0.0f, 0.0f);

  OpenGl_ListOfLight::Iterator aLightIter (myShadingModel == Graphic3d_TOSM_NONE ? OpenGl_NoShadingLight() : myLights);
  for (; aLightIter.More(); aLightIter.Next())
  {
    const OpenGl_Light& aLight = aLightIter.Value();

    if (aLight.Type == Graphic3d_TOLS_AMBIENT)
    {
      myRaytraceGeometry.Ambient += BVH_Vec4f (aLight.Color.r() * aLight.Intensity,
                                               aLight.Color.g() * aLight.Intensity,
                                               aLight.Color.b() * aLight.Intensity,
                                               0.0f);
      continue;
    }

    BVH_Vec4f aDiffuse  (aLight.Color.r() * aLight.Intensity,
                         aLight.Color.g() * aLight.Intensity,
                         aLight.Color.b() * aLight.Intensity,
                         1.0f);

    BVH_Vec4f aPosition (-aLight.Direction.x(),
                         -aLight.Direction.y(),
                         -aLight.Direction.z(),
                         0.0f);

    if (aLight.Type != Graphic3d_TOLS_DIRECTIONAL)
    {
      aPosition = BVH_Vec4f (aLight.Position.x(),
                             aLight.Position.y(),
                             aLight.Position.z(),
                             1.0f);

      // store smoothing radius in w-component
      aDiffuse.w() = Max (aLight.Smoothness, 0.f);
    }
    else
    {
      // store cosine of smoothing angle in w-component
      aDiffuse.w() = cosf (Min (Max (aLight.Smoothness, 0.f), static_cast<Standard_ShortReal> (M_PI / 2.0)));
    }

    if (aLight.IsHeadlight)
    {
      aPosition = theInvModelView * aPosition;
    }

    myRaytraceGeometry.Sources.push_back (OpenGl_RaytraceLight (aDiffuse, aPosition));
  }

  if (myRaytraceLightSrcTexture.IsNull())  // create light source buffer
  {
    myRaytraceLightSrcTexture = new OpenGl_TextureBufferArb;

    if (!myRaytraceLightSrcTexture->Create (theGlContext))
    {
#ifdef RAY_TRACE_PRINT_INFO
      std::cout << "Error: Failed to create light source buffer" << std::endl;
#endif
      return Standard_False;
    }
  }

  if (myRaytraceGeometry.Sources.size() != 0)
  {
    const GLfloat* aDataPtr = myRaytraceGeometry.Sources.front().Packed();
    if (!myRaytraceLightSrcTexture->Init (theGlContext, 4, GLsizei (myRaytraceGeometry.Sources.size() * 2), aDataPtr))
    {
#ifdef RAY_TRACE_PRINT_INFO
      std::cout << "Error: Failed to upload light source buffer" << std::endl;
#endif
      return Standard_False;
    }
  }

  return Standard_True;
}

// =======================================================================
// function : updateRaytraceEnvironmentMap
// purpose  : Updates environment map for ray-tracing
// =======================================================================
Standard_Boolean OpenGl_View::updateRaytraceEnvironmentMap (const Handle(OpenGl_Context)& theGlContext)
{
  Standard_Boolean aResult = Standard_True;

  if (!myToUpdateEnvironmentMap)
  {
    return aResult;
  }

  for (Standard_Integer anIdx = 0; anIdx < 2; ++anIdx)
  {
    const Handle(OpenGl_ShaderProgram)& aProgram =
      anIdx == 0 ? myRaytraceProgram : myPostFSAAProgram;

    if (!aProgram.IsNull())
    {
      aResult &= theGlContext->BindProgram (aProgram);

      if (!myTextureEnv.IsNull() && mySurfaceDetail != Graphic3d_TOD_NONE)
      {
        myTextureEnv->Bind (theGlContext,
          GL_TEXTURE0 + OpenGl_RT_EnvironmentMapTexture);

        aResult &= aProgram->SetUniform (theGlContext,
          myUniformLocations[anIdx][OpenGl_RT_uSphereMapEnabled], 1);
      }
      else
      {
        aResult &= aProgram->SetUniform (theGlContext,
          myUniformLocations[anIdx][OpenGl_RT_uSphereMapEnabled], 0);
      }
    }
  }

  myToUpdateEnvironmentMap = Standard_False;

  theGlContext->BindProgram (NULL);

  return aResult;
}

// =======================================================================
// function : setUniformState
// purpose  : Sets uniform state for the given ray-tracing shader program
// =======================================================================
Standard_Boolean OpenGl_View::setUniformState (const OpenGl_Vec3*            theOrigins,
                                               const OpenGl_Vec3*            theDirects,
                                               const OpenGl_Mat4&            theUnviewMat,
                                               const Standard_Integer        theProgramId,
                                               const Handle(OpenGl_Context)& theGlContext)
{
  Handle(OpenGl_ShaderProgram)& theProgram =
    theProgramId == 0 ? myRaytraceProgram : myPostFSAAProgram;

  if (theProgram.IsNull())
  {
    return Standard_False;
  }

  const Standard_Integer aLightSourceBufferSize =
    static_cast<Standard_Integer> (myRaytraceGeometry.Sources.size());

  // Set camera state
  theProgram->SetUniform (theGlContext,
    myUniformLocations[theProgramId][OpenGl_RT_uOriginLB], theOrigins[0]);
  theProgram->SetUniform (theGlContext,
    myUniformLocations[theProgramId][OpenGl_RT_uOriginRB], theOrigins[1]);
  theProgram->SetUniform (theGlContext,
    myUniformLocations[theProgramId][OpenGl_RT_uOriginLT], theOrigins[2]);
  theProgram->SetUniform (theGlContext,
    myUniformLocations[theProgramId][OpenGl_RT_uOriginRT], theOrigins[3]);
  theProgram->SetUniform (theGlContext,
    myUniformLocations[theProgramId][OpenGl_RT_uDirectLB], theDirects[0]);
  theProgram->SetUniform (theGlContext,
    myUniformLocations[theProgramId][OpenGl_RT_uDirectRB], theDirects[1]);
  theProgram->SetUniform (theGlContext,
    myUniformLocations[theProgramId][OpenGl_RT_uDirectLT], theDirects[2]);
  theProgram->SetUniform (theGlContext,
    myUniformLocations[theProgramId][OpenGl_RT_uDirectRT], theDirects[3]);
  theProgram->SetUniform (theGlContext,
    myUniformLocations[theProgramId][OpenGl_RT_uUnviewMat], theUnviewMat);

  // Set scene parameters
  theProgram->SetUniform (theGlContext,
    myUniformLocations[theProgramId][OpenGl_RT_uSceneRad], myRaytraceSceneRadius);
  theProgram->SetUniform (theGlContext,
    myUniformLocations[theProgramId][OpenGl_RT_uSceneEps], myRaytraceSceneEpsilon);
  theProgram->SetUniform (theGlContext,
    myUniformLocations[theProgramId][OpenGl_RT_uLightCount], aLightSourceBufferSize);
  theProgram->SetUniform (theGlContext,
    myUniformLocations[theProgramId][OpenGl_RT_uLightAmbnt], myRaytraceGeometry.Ambient);

  // Set run-time rendering options
  theProgram->SetUniform (theGlContext,
    myUniformLocations[theProgramId][OpenGl_RT_uShadowsEnabled], myRenderParams.IsShadowEnabled ?  1 : 0);
  theProgram->SetUniform (theGlContext,
    myUniformLocations[theProgramId][OpenGl_RT_uReflectEnabled], myRenderParams.IsReflectionEnabled ?  1 : 0);

  if (myRenderParams.IsGlobalIlluminationEnabled)
  {
    theProgram->SetUniform (theGlContext,
      myUniformLocations[theProgramId][OpenGl_RT_uBlockedRngEnabled], myRenderParams.CoherentPathTracingMode ?  1 : 0);
  }

  // Set array of 64-bit texture handles
  if (theGlContext->arbTexBindless != NULL && myRaytraceGeometry.HasTextures())
  {
    const std::vector<GLuint64>& aTextures = myRaytraceGeometry.TextureHandles();

    theProgram->SetUniform (theGlContext, myUniformLocations[theProgramId][OpenGl_RT_uTexSamplersArray],
      static_cast<GLsizei> (aTextures.size()), (OpenGl_Vec2u* )&aTextures.front());
  }

  // Set background colors (only gradient background supported)
  if (myBgGradientArray != NULL
   && myBgGradientArray->IsDefined())
  {
    theProgram->SetUniform (theGlContext,
      myUniformLocations[theProgramId][OpenGl_RT_uBackColorTop], myBgGradientArray->GradientColor (0));
    theProgram->SetUniform (theGlContext,
      myUniformLocations[theProgramId][OpenGl_RT_uBackColorBot], myBgGradientArray->GradientColor (1));
  }
  else
  {
    const OpenGl_Vec4 aBackColor (myBgColor.rgb[0],
                                  myBgColor.rgb[1],
                                  myBgColor.rgb[2],
                                  1.0f);
    theProgram->SetUniform (theGlContext,
      myUniformLocations[theProgramId][OpenGl_RT_uBackColorTop], aBackColor);
    theProgram->SetUniform (theGlContext,
      myUniformLocations[theProgramId][OpenGl_RT_uBackColorBot], aBackColor);
  }

  theProgram->SetUniform (theGlContext,
    myUniformLocations[theProgramId][OpenGl_RT_uSphereMapForBack], myRenderParams.UseEnvironmentMapBackground ?  1 : 0);

  return Standard_True;
}

// =======================================================================
// function : bindRaytraceTextures
// purpose  : Binds ray-trace textures to corresponding texture units
// =======================================================================
void OpenGl_View::bindRaytraceTextures (const Handle(OpenGl_Context)& theGlContext)
{
  mySceneMinPointTexture->BindTexture    (theGlContext, GL_TEXTURE0 + OpenGl_RT_SceneMinPointTexture);
  mySceneMaxPointTexture->BindTexture    (theGlContext, GL_TEXTURE0 + OpenGl_RT_SceneMaxPointTexture);
  mySceneNodeInfoTexture->BindTexture    (theGlContext, GL_TEXTURE0 + OpenGl_RT_SceneNodeInfoTexture);
  myGeometryVertexTexture->BindTexture   (theGlContext, GL_TEXTURE0 + OpenGl_RT_GeometryVertexTexture);
  myGeometryNormalTexture->BindTexture   (theGlContext, GL_TEXTURE0 + OpenGl_RT_GeometryNormalTexture);
  myGeometryTexCrdTexture->BindTexture   (theGlContext, GL_TEXTURE0 + OpenGl_RT_GeometryTexCrdTexture);
  myGeometryTriangTexture->BindTexture   (theGlContext, GL_TEXTURE0 + OpenGl_RT_GeometryTriangTexture);
  mySceneTransformTexture->BindTexture   (theGlContext, GL_TEXTURE0 + OpenGl_RT_SceneTransformTexture);
  myRaytraceMaterialTexture->BindTexture (theGlContext, GL_TEXTURE0 + OpenGl_RT_RaytraceMaterialTexture);
  myRaytraceLightSrcTexture->BindTexture (theGlContext, GL_TEXTURE0 + OpenGl_RT_RaytraceLightSrcTexture);

  if (!myOpenGlFBO.IsNull())
  {
    myOpenGlFBO->ColorTexture()->Bind        (theGlContext, GL_TEXTURE0 + OpenGl_RT_OpenGlColorTexture);
    myOpenGlFBO->DepthStencilTexture()->Bind (theGlContext, GL_TEXTURE0 + OpenGl_RT_OpenGlDepthTexture);
  }
}

// =======================================================================
// function : unbindRaytraceTextures
// purpose  : Unbinds ray-trace textures from corresponding texture units
// =======================================================================
void OpenGl_View::unbindRaytraceTextures (const Handle(OpenGl_Context)& theGlContext)
{
  mySceneMinPointTexture->UnbindTexture    (theGlContext, GL_TEXTURE0 + OpenGl_RT_SceneMinPointTexture);
  mySceneMaxPointTexture->UnbindTexture    (theGlContext, GL_TEXTURE0 + OpenGl_RT_SceneMaxPointTexture);
  mySceneNodeInfoTexture->UnbindTexture    (theGlContext, GL_TEXTURE0 + OpenGl_RT_SceneNodeInfoTexture);
  myGeometryVertexTexture->UnbindTexture   (theGlContext, GL_TEXTURE0 + OpenGl_RT_GeometryVertexTexture);
  myGeometryNormalTexture->UnbindTexture   (theGlContext, GL_TEXTURE0 + OpenGl_RT_GeometryNormalTexture);
  myGeometryTexCrdTexture->UnbindTexture   (theGlContext, GL_TEXTURE0 + OpenGl_RT_GeometryTexCrdTexture);
  myGeometryTriangTexture->UnbindTexture   (theGlContext, GL_TEXTURE0 + OpenGl_RT_GeometryTriangTexture);
  mySceneTransformTexture->UnbindTexture   (theGlContext, GL_TEXTURE0 + OpenGl_RT_SceneTransformTexture);
  myRaytraceMaterialTexture->UnbindTexture (theGlContext, GL_TEXTURE0 + OpenGl_RT_RaytraceMaterialTexture);
  myRaytraceLightSrcTexture->UnbindTexture (theGlContext, GL_TEXTURE0 + OpenGl_RT_RaytraceLightSrcTexture);

  if (!myOpenGlFBO.IsNull())
  {
    myOpenGlFBO->ColorTexture()->Unbind        (theGlContext, GL_TEXTURE0 + OpenGl_RT_OpenGlColorTexture);
    myOpenGlFBO->DepthStencilTexture()->Unbind (theGlContext, GL_TEXTURE0 + OpenGl_RT_OpenGlDepthTexture);
  }

  theGlContext->core15fwd->glActiveTexture (GL_TEXTURE0);
}

// =======================================================================
// function : runRaytraceShaders
// purpose  : Runs ray-tracing shader programs
// =======================================================================
Standard_Boolean OpenGl_View::runRaytraceShaders (const Standard_Integer        theSizeX,
                                                  const Standard_Integer        theSizeY,
                                                  const OpenGl_Vec3*            theOrigins,
                                                  const OpenGl_Vec3*            theDirects,
                                                  const OpenGl_Mat4&            theUnviewMat,
                                                  OpenGl_FrameBuffer*           theReadDrawFbo,
                                                  const Handle(OpenGl_Context)& theGlContext)
{
  bindRaytraceTextures (theGlContext);

  Handle(OpenGl_FrameBuffer) aRenderFramebuffer;
  Handle(OpenGl_FrameBuffer) anAccumFramebuffer;

  if (myRaytraceParameters.GlobalIllumination) // if path-tracing is used
  {
    for (int anIdx = 0; anIdx < 3; ++anIdx)
    {
      if  (fabsf (theOrigins[anIdx].x() - myPreviousOrigins[anIdx].x()) > std::numeric_limits<Standard_ShortReal>::epsilon()
        || fabsf (theOrigins[anIdx].y() - myPreviousOrigins[anIdx].y()) > std::numeric_limits<Standard_ShortReal>::epsilon()
        || fabsf (theOrigins[anIdx].z() - myPreviousOrigins[anIdx].z()) > std::numeric_limits<Standard_ShortReal>::epsilon())
      {
        myAccumFrames = 0; // camera has been moved
      }

      myPreviousOrigins[anIdx] = theOrigins[anIdx];
    }

    aRenderFramebuffer = myAccumFrames % 2 ? myRaytraceFBO1 : myRaytraceFBO2;
    anAccumFramebuffer = myAccumFrames % 2 ? myRaytraceFBO2 : myRaytraceFBO1;

    anAccumFramebuffer->ColorTexture()->Bind (
      theGlContext, GL_TEXTURE0 + OpenGl_RT_PrevAccumTexture);

    aRenderFramebuffer->BindBuffer (theGlContext);
  }
  else if (myRenderParams.IsAntialiasingEnabled) // if 2-pass ray-tracing is used
  {
    myRaytraceFBO1->BindBuffer (theGlContext);

    glDisable (GL_BLEND);
  }

  Standard_Boolean aResult = theGlContext->BindProgram (myRaytraceProgram);

  aResult &= setUniformState (theOrigins,
                              theDirects,
                              theUnviewMat,
                              0, // ID of RT program
                              theGlContext);

  if (myRaytraceParameters.GlobalIllumination)
  {
    if (myAccumFrames == 0)
    {
      myRNG.SetSeed();
    }

    // Set frame accumulation weight
    myRaytraceProgram->SetUniform (theGlContext,
      myUniformLocations[0][OpenGl_RT_uSampleWeight], 1.f / (myAccumFrames + 1));

    // Set random number generator seed
    myRaytraceProgram->SetUniform (theGlContext,
      myUniformLocations[0][OpenGl_RT_uFrameRndSeed], static_cast<Standard_Integer> (myRNG.NextInt() >> 2));
  }

  theGlContext->core20fwd->glDrawArrays (GL_TRIANGLES, 0, 6);

  if (myRaytraceParameters.GlobalIllumination)
  {
    // Output accumulated image
    glDisable (GL_BLEND);

    theGlContext->BindProgram (myOutImageProgram);

    if (theReadDrawFbo != NULL)
    {
      theReadDrawFbo->BindBuffer (theGlContext);
    }
    else
    {
      aRenderFramebuffer->UnbindBuffer (theGlContext);
    }

    aRenderFramebuffer->ColorTexture()->Bind (
      theGlContext, GL_TEXTURE0 + OpenGl_RT_PrevAccumTexture);

    theGlContext->core20fwd->glDrawArrays (GL_TRIANGLES, 0, 6);

    ++myAccumFrames;
  }
  else if (myRenderParams.IsAntialiasingEnabled)
  {
    myRaytraceFBO1->ColorTexture()->Bind (theGlContext, GL_TEXTURE0 + OpenGl_RT_FsaaInputTexture);

    aResult &= theGlContext->BindProgram (myPostFSAAProgram);

    aResult &= setUniformState (theOrigins,
                                theDirects,
                                theUnviewMat,
                                1, // ID of FSAA program
                                theGlContext);

    // Perform multi-pass adaptive FSAA using ping-pong technique.
    // We use 'FLIPTRI' sampling pattern changing for every pixel
    // (3 additional samples per pixel, the 1st sample is already
    // available from initial ray-traced image).
    for (Standard_Integer anIt = 1; anIt < 4; ++anIt)
    {
      GLfloat aOffsetX = 1.f / theSizeX;
      GLfloat aOffsetY = 1.f / theSizeY;

      if (anIt == 1)
      {
        aOffsetX *= -0.55f;
        aOffsetY *=  0.55f;
      }
      else if (anIt == 2)
      {
        aOffsetX *=  0.00f;
        aOffsetY *= -0.55f;
      }
      else if (anIt == 3)
      {
        aOffsetX *= 0.55f;
        aOffsetY *= 0.00f;
      }

      aResult &= myPostFSAAProgram->SetUniform (theGlContext,
        myUniformLocations[1][OpenGl_RT_uSamples], anIt + 1);
      aResult &= myPostFSAAProgram->SetUniform (theGlContext,
        myUniformLocations[1][OpenGl_RT_uOffsetX], aOffsetX);
      aResult &= myPostFSAAProgram->SetUniform (theGlContext,
        myUniformLocations[1][OpenGl_RT_uOffsetY], aOffsetY);

      Handle(OpenGl_FrameBuffer)& aFramebuffer = anIt % 2 ? myRaytraceFBO2 : myRaytraceFBO1;

      if (anIt == 3) // disable FBO on last iteration
      {
        if (theReadDrawFbo != NULL)
        {
          theReadDrawFbo->BindBuffer (theGlContext);
        }
        else
        {
          aFramebuffer->UnbindBuffer (theGlContext);
        }
      }
      else
      {
        aFramebuffer->BindBuffer (theGlContext);
      }

      theGlContext->core20fwd->glDrawArrays (GL_TRIANGLES, 0, 6);

      if (anIt != 3) // set input for the next pass
      {
        aFramebuffer->ColorTexture()->Bind (theGlContext, GL_TEXTURE0 + OpenGl_RT_FsaaInputTexture);
      }
    }
  }

  unbindRaytraceTextures (theGlContext);

  theGlContext->BindProgram (NULL);

  return aResult;
}

// =======================================================================
// function : raytrace
// purpose  : Redraws the window using OpenGL/GLSL ray-tracing
// =======================================================================
Standard_Boolean OpenGl_View::raytrace (const Standard_Integer        theSizeX,
                                        const Standard_Integer        theSizeY,
                                        OpenGl_FrameBuffer*           theReadDrawFbo,
                                        const Handle(OpenGl_Context)& theGlContext)
{
  if (!initRaytraceResources (theGlContext))
  {
    return Standard_False;
  }

  if (!resizeRaytraceBuffers (theSizeX, theSizeY, theGlContext))
  {
    return Standard_False;
  }

  if (!updateRaytraceEnvironmentMap (theGlContext))
  {
    return Standard_False;
  }

  // Get model-view and projection matrices
  OpenGl_Mat4 aOrientationMatrix = myCamera->OrientationMatrixF();
  OpenGl_Mat4 aViewMappingMatrix = theGlContext->ProjectionState.Current();

  OpenGl_Mat4 aInverOrientMatrix;
  aOrientationMatrix.Inverted (aInverOrientMatrix);
  if (!updateRaytraceLightSources (aInverOrientMatrix, theGlContext))
  {
    return Standard_False;
  }

  OpenGl_Vec3 aOrigins[4];
  OpenGl_Vec3 aDirects[4];
  OpenGl_Mat4 anUnviewMat;

  updateCamera (aOrientationMatrix,
                aViewMappingMatrix,
                aOrigins,
                aDirects,
                anUnviewMat);

  glDisable (GL_BLEND);
  glDisable (GL_DEPTH_TEST);

  if (theReadDrawFbo != NULL)
  {
    theReadDrawFbo->BindBuffer (theGlContext);
  }

  // Generate ray-traced image
  if (myIsRaytraceDataValid)
  {
    myRaytraceScreenQuad.BindVertexAttrib (theGlContext, Graphic3d_TOA_POS);

    if (!myRaytraceGeometry.AcquireTextures (theGlContext))
    {
      theGlContext->PushMessage (GL_DEBUG_SOURCE_APPLICATION_ARB, GL_DEBUG_TYPE_ERROR_ARB,
        0, GL_DEBUG_SEVERITY_MEDIUM_ARB, "Error: Failed to acquire OpenGL image textures");
    }

    Standard_Boolean aResult = runRaytraceShaders (theSizeX,
                                                   theSizeY,
                                                   aOrigins,
                                                   aDirects,
                                                   anUnviewMat,
                                                   theReadDrawFbo,
                                                   theGlContext);

    if (!aResult)
    {
      theGlContext->PushMessage (GL_DEBUG_SOURCE_APPLICATION_ARB, GL_DEBUG_TYPE_ERROR_ARB,
        0, GL_DEBUG_SEVERITY_MEDIUM_ARB, "Error: Failed to execute ray-tracing shaders");
    }

    if (!myRaytraceGeometry.ReleaseTextures (theGlContext))
    {
      theGlContext->PushMessage (GL_DEBUG_SOURCE_APPLICATION_ARB, GL_DEBUG_TYPE_ERROR_ARB,
        0, GL_DEBUG_SEVERITY_MEDIUM_ARB, "Error: Failed to release OpenGL image textures");
    }

    myRaytraceScreenQuad.UnbindVertexAttrib (theGlContext, Graphic3d_TOA_POS);
  }

  glDisable (GL_BLEND);
  glEnable (GL_DEPTH_TEST);

  return Standard_True;
}
