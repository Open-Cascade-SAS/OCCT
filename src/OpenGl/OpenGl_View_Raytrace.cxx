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
#include <OpenGl_BackgroundArray.hxx>
#include <OpenGl_FrameBuffer.hxx>
#include <OpenGl_PrimitiveArray.hxx>
#include <OpenGl_VertexBuffer.hxx>
#include <OpenGl_SceneGeometry.hxx>
#include <OpenGl_ShaderProgram.hxx>
#include <OpenGl_TextureBuffer.hxx>
#include <OpenGl_GlCore44.hxx>
#include <OSD_Protection.hxx>
#include <OSD_File.hxx>

#include "../Shaders/Shaders_RaytraceBase_vs.pxx"
#include "../Shaders/Shaders_RaytraceBase_fs.pxx"
#include "../Shaders/Shaders_PathtraceBase_fs.pxx"
#include "../Shaders/Shaders_RaytraceRender_fs.pxx"
#include "../Shaders/Shaders_RaytraceSmooth_fs.pxx"
#include "../Shaders/Shaders_Display_fs.pxx"
#include "../Shaders/Shaders_TangentSpaceNormal_glsl.pxx"

//! Use this macro to output ray-tracing debug info
// #define RAY_TRACE_PRINT_INFO

#ifdef RAY_TRACE_PRINT_INFO
  #include <OSD_Timer.hxx>
#endif

namespace
{
  static const OpenGl_Vec4 THE_WHITE_COLOR (1.0f, 1.0f, 1.0f, 1.0f);
  static const OpenGl_Vec4 THE_BLACK_COLOR (0.0f, 0.0f, 0.0f, 1.0f);
}

namespace
{
  //! Defines OpenGL texture samplers.
  static const Graphic3d_TextureUnit OpenGl_RT_EnvMapTexture = Graphic3d_TextureUnit_0;

  static const Graphic3d_TextureUnit OpenGl_RT_SceneNodeInfoTexture  = Graphic3d_TextureUnit_1;
  static const Graphic3d_TextureUnit OpenGl_RT_SceneMinPointTexture  = Graphic3d_TextureUnit_2;
  static const Graphic3d_TextureUnit OpenGl_RT_SceneMaxPointTexture  = Graphic3d_TextureUnit_3;
  static const Graphic3d_TextureUnit OpenGl_RT_SceneTransformTexture = Graphic3d_TextureUnit_4;

  static const Graphic3d_TextureUnit OpenGl_RT_GeometryVertexTexture = Graphic3d_TextureUnit_5;
  static const Graphic3d_TextureUnit OpenGl_RT_GeometryNormalTexture = Graphic3d_TextureUnit_6;
  static const Graphic3d_TextureUnit OpenGl_RT_GeometryTexCrdTexture = Graphic3d_TextureUnit_7;
  static const Graphic3d_TextureUnit OpenGl_RT_GeometryTriangTexture = Graphic3d_TextureUnit_8;

  static const Graphic3d_TextureUnit OpenGl_RT_RaytraceMaterialTexture = Graphic3d_TextureUnit_9;
  static const Graphic3d_TextureUnit OpenGl_RT_RaytraceLightSrcTexture = Graphic3d_TextureUnit_10;

  static const Graphic3d_TextureUnit OpenGl_RT_FsaaInputTexture = Graphic3d_TextureUnit_11;
  static const Graphic3d_TextureUnit OpenGl_RT_PrevAccumTexture = Graphic3d_TextureUnit_12;

  static const Graphic3d_TextureUnit OpenGl_RT_RaytraceDepthTexture = Graphic3d_TextureUnit_13;
}

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
    if (myRaytraceLayerListState != myZLayers.ModificationStateOfRaytracable())
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

  for (NCollection_List<Handle(Graphic3d_Layer)>::Iterator aLayerIter (myZLayers.Layers()); aLayerIter.More(); aLayerIter.Next())
  {
    const Handle(OpenGl_Layer)& aLayer = aLayerIter.Value();
    if (aLayer->NbStructures() == 0
    || !aLayer->LayerSettings().IsRaytracable()
    ||  aLayer->LayerSettings().IsImmediate())
    {
      continue;
    }

    for (Standard_Integer aPriorityIter = Graphic3d_DisplayPriority_Bottom; aPriorityIter <= Graphic3d_DisplayPriority_Topmost; ++aPriorityIter)
    {
      const Graphic3d_IndexedMapOfStructure& aStructures = aLayer->Structures ((Graphic3d_DisplayPriority )aPriorityIter);
      for (OpenGl_Structure::StructIterator aStructIt (aStructures); aStructIt.More(); aStructIt.Next())
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
            aNonRaytraceIDs.insert (aStructure->highlight ? aStructure->Identification() : -aStructure->Identification());
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

          for (OpenGl_Structure::GroupIterator aGroupIter (aStructure->Groups()); aGroupIter.More(); aGroupIter.Next())
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
    myRaytraceLayerListState = myZLayers.ModificationStateOfRaytracable();

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
    {
      myAccumFrames = 0;
    }

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
  if (theParams.IsNull())
  {
    return;
  }

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
OpenGl_RaytraceMaterial OpenGl_View::convertMaterial (const OpenGl_Aspects* theAspect,
                                                      const Handle(OpenGl_Context)& theGlContext)
{
  OpenGl_RaytraceMaterial aResMat;

  const Graphic3d_MaterialAspect& aSrcMat = theAspect->Aspect()->FrontMaterial();
  const OpenGl_Vec3& aMatCol  = theAspect->Aspect()->InteriorColor();
  const float        aShine   = 128.0f * float(aSrcMat.Shininess());

  const OpenGl_Vec3& aSrcAmb = aSrcMat.AmbientColor();
  const OpenGl_Vec3& aSrcDif = aSrcMat.DiffuseColor();
  const OpenGl_Vec3& aSrcSpe = aSrcMat.SpecularColor();
  const OpenGl_Vec3& aSrcEms = aSrcMat.EmissiveColor();
  switch (aSrcMat.MaterialType())
  {
    case Graphic3d_MATERIAL_ASPECT:
    {
      aResMat.Ambient .SetValues (aSrcAmb * aMatCol,  1.0f);
      aResMat.Diffuse .SetValues (aSrcDif * aMatCol, -1.0f); // -1 is no texture
      aResMat.Emission.SetValues (aSrcEms * aMatCol,  1.0f);
      break;
    }
    case Graphic3d_MATERIAL_PHYSIC:
    {
      aResMat.Ambient .SetValues (aSrcAmb,  1.0f);
      aResMat.Diffuse .SetValues (aSrcDif, -1.0f); // -1 is no texture
      aResMat.Emission.SetValues (aSrcEms,  1.0f);
      break;
    }
  }

  {
    // interior color is always ignored for Specular
    aResMat.Specular.SetValues (aSrcSpe, aShine);
    const Standard_ShortReal aMaxRefl = Max (aResMat.Diffuse.x() + aResMat.Specular.x(),
                                        Max (aResMat.Diffuse.y() + aResMat.Specular.y(),
                                             aResMat.Diffuse.z() + aResMat.Specular.z()));
    const Standard_ShortReal aReflectionScale = 0.75f / aMaxRefl;
    aResMat.Reflection.SetValues (aSrcSpe * aReflectionScale, 0.0f);
  }

  const float anIndex = (float )aSrcMat.RefractionIndex();
  aResMat.Transparency = BVH_Vec4f (aSrcMat.Alpha(), aSrcMat.Transparency(),
                                    anIndex == 0 ? 1.0f : anIndex,
                                    anIndex == 0 ? 1.0f : 1.0f / anIndex);

  aResMat.Ambient  = theGlContext->Vec4FromQuantityColor (aResMat.Ambient);
  aResMat.Diffuse  = theGlContext->Vec4FromQuantityColor (aResMat.Diffuse);
  aResMat.Specular = theGlContext->Vec4FromQuantityColor (aResMat.Specular);
  aResMat.Emission = theGlContext->Vec4FromQuantityColor (aResMat.Emission);

  // Serialize physically-based material properties
  const Graphic3d_BSDF& aBSDF = aSrcMat.BSDF();

  aResMat.BSDF.Kc = aBSDF.Kc;
  aResMat.BSDF.Ks = aBSDF.Ks;
  aResMat.BSDF.Kd = BVH_Vec4f (aBSDF.Kd, -1.0f); // no base color texture
  aResMat.BSDF.Kt = BVH_Vec4f (aBSDF.Kt, -1.0f); // no metallic-roughness texture
  aResMat.BSDF.Le = BVH_Vec4f (aBSDF.Le, -1.0f); // no emissive texture

  aResMat.BSDF.Absorption = aBSDF.Absorption;

  aResMat.BSDF.FresnelCoat = aBSDF.FresnelCoat.Serialize ();
  aResMat.BSDF.FresnelBase = aBSDF.FresnelBase.Serialize ();
  aResMat.BSDF.FresnelBase.w() = -1.0; // no normal map texture

  // Handle material textures
  if (!theAspect->Aspect()->ToMapTexture())
  {
    return aResMat;
  }

  const Handle(OpenGl_TextureSet)& aTextureSet = theAspect->TextureSet (theGlContext);
  if (aTextureSet.IsNull()
   || aTextureSet->IsEmpty()
   || aTextureSet->First().IsNull())
  {
    return aResMat;
  }

  if (theGlContext->HasRayTracingTextures())
  {
    // write texture ID to diffuse w-components
    for (OpenGl_TextureSet::Iterator aTexIter (aTextureSet); aTexIter.More(); aTexIter.Next())
    {
      const Handle(OpenGl_Texture)& aTexture = aTexIter.Value();
      if (aTexIter.Unit() == Graphic3d_TextureUnit_BaseColor)
      {
        buildTextureTransform (aTexture->Sampler()->Parameters(), aResMat.TextureTransform);
        aResMat.Diffuse.w() = aResMat.BSDF.Kd.w() = static_cast<Standard_ShortReal> (myRaytraceGeometry.AddTexture (aTexture));
      }
      else if (aTexIter.Unit() == Graphic3d_TextureUnit_MetallicRoughness)
      {
        buildTextureTransform (aTexture->Sampler()->Parameters(), aResMat.TextureTransform);
        aResMat.BSDF.Kt.w() = static_cast<Standard_ShortReal> (myRaytraceGeometry.AddTexture (aTexture));
      }
      else if (aTexIter.Unit() == Graphic3d_TextureUnit_Emissive)
      {
        buildTextureTransform (aTexture->Sampler()->Parameters(), aResMat.TextureTransform);
        aResMat.BSDF.Le.w() = static_cast<Standard_ShortReal> (myRaytraceGeometry.AddTexture (aTexture));
      }
      else if (aTexIter.Unit() == Graphic3d_TextureUnit_Normal)
      {
        buildTextureTransform (aTexture->Sampler()->Parameters(), aResMat.TextureTransform);
        aResMat.BSDF.FresnelBase.w() = static_cast<Standard_ShortReal> (myRaytraceGeometry.AddTexture (aTexture));
      }
    }
  }
  else if (!myIsRaytraceWarnTextures)
  {
    theGlContext->PushMessage (GL_DEBUG_SOURCE_APPLICATION, GL_DEBUG_TYPE_PORTABILITY, 0, GL_DEBUG_SEVERITY_HIGH,
                               "Warning: texturing in Ray-Trace requires GL_ARB_bindless_texture extension which is missing. "
                               "Please try to update graphics card driver. At the moment textures will be ignored.");
    myIsRaytraceWarnTextures = Standard_True;
  }

  return aResMat;
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
  OpenGl_RaytraceMaterial aDefaultMaterial;
  Standard_Boolean aResult = addRaytraceGroups (theStructure, aDefaultMaterial, theStructure->Transformation(), theGlContext);

  // Process all connected OpenGL structures
  const OpenGl_Structure* anInstanced = theStructure->InstancedStructure();

  if (anInstanced != NULL && anInstanced->IsRaytracable())
  {
    aResult &= addRaytraceGroups (anInstanced, aDefaultMaterial, theStructure->Transformation(), theGlContext);
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
                                                 const Handle(TopLoc_Datum3D)&  theTrsf,
                                                 const Handle(OpenGl_Context)&  theGlContext)
{
  OpenGl_Mat4 aMat4;
  for (OpenGl_Structure::GroupIterator aGroupIter (theStructure->Groups()); aGroupIter.More(); aGroupIter.Next())
  {
    // Get group material
    OpenGl_RaytraceMaterial aGroupMaterial;
    if (aGroupIter.Value()->GlAspects() != NULL)
    {
      aGroupMaterial = convertMaterial (aGroupIter.Value()->GlAspects(), theGlContext);
    }

    Standard_Integer aMatID = static_cast<Standard_Integer> (myRaytraceGeometry.Materials.size());

    // Use group material if available, otherwise use structure material
    myRaytraceGeometry.Materials.push_back (aGroupIter.Value()->GlAspects() != NULL ? aGroupMaterial : theStructMat);

    // Add OpenGL elements from group (extract primitives arrays and aspects)
    for (const OpenGl_ElementNode* aNode = aGroupIter.Value()->FirstNode(); aNode != NULL; aNode = aNode->next)
    {
      OpenGl_Aspects* anAspect = dynamic_cast<OpenGl_Aspects*> (aNode->elem);

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
            opencascade::handle<BVH_Transform<Standard_ShortReal, 4> > aTransform = new BVH_Transform<Standard_ShortReal, 4>();
            if (!theTrsf.IsNull())
            {
              theTrsf->Trsf().GetMat4 (aMat4);
              aTransform->SetTransform (aMat4);
            }

            aSet->SetProperties (aTransform);
            if (aSet->MaterialIndex() != OpenGl_TriangleSet::INVALID_MATERIAL && aSet->MaterialIndex() != aMatID)
            {
              aSet->SetMaterialIndex (aMatID);
            }
          }
          else
          {
            if (Handle(OpenGl_TriangleSet) aSet = addRaytracePrimitiveArray (aPrimArray, aMatID, 0))
            {
              opencascade::handle<BVH_Transform<Standard_ShortReal, 4> > aTransform = new BVH_Transform<Standard_ShortReal, 4>();
              if (!theTrsf.IsNull())
              {
                theTrsf->Trsf().GetMat4 (aMat4);
                aTransform->SetTransform (aMat4);
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
Handle(OpenGl_TriangleSet) OpenGl_View::addRaytracePrimitiveArray (const OpenGl_PrimitiveArray* theArray,
                                                                   const Standard_Integer       theMaterial,
                                                                   const OpenGl_Mat4*           theTransform)
{
  const Handle(Graphic3d_BoundBuffer)& aBounds   = theArray->Bounds();
  const Handle(Graphic3d_IndexBuffer)& anIndices = theArray->Indices();
  const Handle(Graphic3d_Buffer)&      anAttribs = theArray->Attributes();

  if (theArray->DrawMode() < GL_TRIANGLES
   || theArray->DrawMode() > GL_POLYGON
   || anAttribs.IsNull())
  {
    return Handle(OpenGl_TriangleSet)();
  }

  OpenGl_Mat4 aNormalMatrix;
  if (theTransform != NULL)
  {
    Standard_ASSERT_RETURN (theTransform->Inverted (aNormalMatrix),
      "Error: Failed to compute normal transformation matrix", NULL);

    aNormalMatrix.Transpose();
  }

  Handle(OpenGl_TriangleSet) aSet = new OpenGl_TriangleSet (theArray->GetUID(), myRaytraceBVHBuilder);
  {
    aSet->Vertices.reserve (anAttribs->NbElements);
    aSet->Normals.reserve  (anAttribs->NbElements);
    aSet->TexCrds.reserve  (anAttribs->NbElements);

    const size_t aVertFrom = aSet->Vertices.size();

    Standard_Integer anAttribIndex = 0;
    Standard_Size anAttribStride = 0;
    if (const Standard_Byte* aPosData = anAttribs->AttributeData (Graphic3d_TOA_POS, anAttribIndex, anAttribStride))
    {
      const Graphic3d_Attribute& anAttrib = anAttribs->Attribute (anAttribIndex);
      if (anAttrib.DataType == Graphic3d_TOD_VEC2
       || anAttrib.DataType == Graphic3d_TOD_VEC3
       || anAttrib.DataType == Graphic3d_TOD_VEC4)
      {
        for (Standard_Integer aVertIter = 0; aVertIter < anAttribs->NbElements; ++aVertIter)
        {
          const float* aCoords = reinterpret_cast<const float*> (aPosData + anAttribStride * aVertIter);
          aSet->Vertices.push_back (BVH_Vec3f (aCoords[0], aCoords[1], anAttrib.DataType != Graphic3d_TOD_VEC2 ? aCoords[2] : 0.0f));
        }
      }
    }
    if (const Standard_Byte* aNormData = anAttribs->AttributeData (Graphic3d_TOA_NORM, anAttribIndex, anAttribStride))
    {
      const Graphic3d_Attribute& anAttrib = anAttribs->Attribute (anAttribIndex);
      if (anAttrib.DataType == Graphic3d_TOD_VEC3
       || anAttrib.DataType == Graphic3d_TOD_VEC4)
      {
        for (Standard_Integer aVertIter = 0; aVertIter < anAttribs->NbElements; ++aVertIter)
        {
          aSet->Normals.push_back (*reinterpret_cast<const Graphic3d_Vec3*> (aNormData + anAttribStride * aVertIter));
        }
      }
    }
    if (const Standard_Byte* aTexData = anAttribs->AttributeData (Graphic3d_TOA_UV, anAttribIndex, anAttribStride))
    {
      const Graphic3d_Attribute& anAttrib = anAttribs->Attribute (anAttribIndex);
      if (anAttrib.DataType == Graphic3d_TOD_VEC2)
      {
        for (Standard_Integer aVertIter = 0; aVertIter < anAttribs->NbElements; ++aVertIter)
        {
          aSet->TexCrds.push_back (*reinterpret_cast<const Graphic3d_Vec2*> (aTexData + anAttribStride * aVertIter));
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
          aSet.Nullify();
          return Handle(OpenGl_TriangleSet)();
        }

        aBoundStart += aVertNum;
      }
    }
    else
    {
      const Standard_Integer aVertNum = !anIndices.IsNull() ? anIndices->NbElements : anAttribs->NbElements;

      if (!addRaytraceVertexIndices (*aSet, theMaterial, aVertNum, 0, *theArray))
      {
        aSet.Nullify();
        return Handle(OpenGl_TriangleSet)();
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
    case GL_QUAD_STRIP:     return addRaytraceQuadrangleStripArray (theSet, theMatID, theCount, theOffset, theArray.Indices());
    case GL_QUADS:          return addRaytraceQuadrangleArray      (theSet, theMatID, theCount, theOffset, theArray.Indices());
    case GL_POLYGON:        return addRaytracePolygonArray         (theSet, theMatID, theCount, theOffset, theArray.Indices());
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
      theSet.Elements.push_back (BVH_Vec4i (theIndices->Index (aVert + (aCW ? 1 : 0)),
                                            theIndices->Index (aVert + (aCW ? 0 : 1)),
                                            theIndices->Index (aVert + 2),
                                            theMatID));
    }
  }
  else
  {
    for (Standard_Integer aVert = theOffset, aCW = 0; aVert < theOffset + theCount - 2; ++aVert, aCW = (aCW + 1) % 2)
    {
      theSet.Elements.push_back (BVH_Vec4i (aVert + (aCW ? 1 : 0),
                                            aVert + (aCW ? 0 : 1),
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
TCollection_AsciiString OpenGl_View::ShaderSource::Source (const Handle(OpenGl_Context)& theCtx,
                                                           const GLenum theType) const
{
  TCollection_AsciiString aVersion = theCtx->GraphicsLibrary() == Aspect_GraphicsLibrary_OpenGLES
                                   ? "#version 320 es\n"
                                   : "#version 140\n";

  TCollection_AsciiString aPrecisionHeader;
  if (theType == GL_FRAGMENT_SHADER
   && theCtx->GraphicsLibrary() == Aspect_GraphicsLibrary_OpenGLES)
  {
    aPrecisionHeader = theCtx->hasHighp
                     ? "precision highp float;\n"
                       "precision highp int;\n"
                       "precision highp samplerBuffer;\n"
                       "precision highp isamplerBuffer;\n"
                     : "precision mediump float;\n"
                       "precision mediump int;\n"
                       "precision mediump samplerBuffer;\n"
                       "precision mediump isamplerBuffer;\n";
  }
  if (myPrefix.IsEmpty())
  {
    return aVersion + aPrecisionHeader + mySource;
  }
  return aVersion + aPrecisionHeader + myPrefix + "\n" + mySource;
}

// =======================================================================
// function : LoadFromFiles
// purpose  : Loads shader source from specified files
// =======================================================================
Standard_Boolean OpenGl_View::ShaderSource::LoadFromFiles (const TCollection_AsciiString* theFileNames,
                                                           const TCollection_AsciiString& thePrefix)
{
  myError.Clear();
  mySource.Clear();
  myPrefix = thePrefix;

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

  if (!aMissingFiles.IsEmpty())
  {
    myError = TCollection_AsciiString("Shader files ") + aMissingFiles + " are missing or inaccessible";
    return Standard_False;
  }
  return Standard_True;
}

// =======================================================================
// function : LoadFromStrings
// purpose  :
// =======================================================================
Standard_Boolean OpenGl_View::ShaderSource::LoadFromStrings (const TCollection_AsciiString* theStrings,
                                                             const TCollection_AsciiString& thePrefix)
{
  myError.Clear();
  mySource.Clear();
  myPrefix = thePrefix;

  for (Standard_Integer anIndex = 0; !theStrings[anIndex].IsEmpty(); ++anIndex)
  {
    TCollection_AsciiString aSource = theStrings[anIndex];
    if (!aSource.IsEmpty())
    {
      mySource += TCollection_AsciiString ("\n") + aSource;
    }
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

  if (myRaytraceParameters.IsZeroToOneDepth)
  {
    aPrefixString += TCollection_AsciiString ("\n#define THE_ZERO_TO_ONE_DEPTH");
  }

  if (myRaytraceParameters.TransparentShadows)
  {
    aPrefixString += TCollection_AsciiString ("\n#define TRANSPARENT_SHADOWS");
  }
  if (!theGlContext->ToRenderSRGB())
  {
    aPrefixString += TCollection_AsciiString ("\n#define THE_SHIFT_sRGB");
  }

  // If OpenGL driver supports bindless textures and texturing
  // is actually used, activate texturing in ray-tracing mode
  if (myRaytraceParameters.UseBindlessTextures && theGlContext->arbTexBindless != NULL)
  {
    aPrefixString += TCollection_AsciiString ("\n#define USE_TEXTURES") +
      TCollection_AsciiString ("\n#define MAX_TEX_NUMBER ") + TCollection_AsciiString (OpenGl_RaytraceGeometry::MAX_TEX_NUMBER);
  }

  if (myRaytraceParameters.GlobalIllumination) // path tracing activated
  {
    aPrefixString += TCollection_AsciiString ("\n#define PATH_TRACING");

    if (myRaytraceParameters.AdaptiveScreenSampling) // adaptive screen sampling requested
    {
      if (theGlContext->IsGlGreaterEqual (4, 4))
      {
        aPrefixString += TCollection_AsciiString ("\n#define ADAPTIVE_SAMPLING");
        if (myRaytraceParameters.AdaptiveScreenSamplingAtomic
         && theGlContext->CheckExtension ("GL_NV_shader_atomic_float"))
        {
          aPrefixString += TCollection_AsciiString ("\n#define ADAPTIVE_SAMPLING_ATOMIC");
        }
      }
    }

    if (myRaytraceParameters.TwoSidedBsdfModels) // two-sided BSDFs requested
    {
      aPrefixString += TCollection_AsciiString ("\n#define TWO_SIDED_BXDF");
    }

    switch (myRaytraceParameters.ToneMappingMethod)
    {
      case Graphic3d_ToneMappingMethod_Disabled:
        break;
      case Graphic3d_ToneMappingMethod_Filmic:
        aPrefixString += TCollection_AsciiString ("\n#define TONE_MAPPING_FILMIC");
        break;
    }
  }

  if (myRaytraceParameters.ToIgnoreNormalMap)
  {
    aPrefixString += TCollection_AsciiString("\n#define IGNORE_NORMAL_MAP");
  }

  if (myRaytraceParameters.CubemapForBack)
  {
    aPrefixString += TCollection_AsciiString("\n#define BACKGROUND_CUBEMAP");
  }

  if (myRaytraceParameters.DepthOfField)
  {
    aPrefixString += TCollection_AsciiString("\n#define DEPTH_OF_FIELD");
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
  theGlContext->PushMessage (GL_DEBUG_SOURCE_APPLICATION,
    GL_DEBUG_TYPE_ERROR, 0, GL_DEBUG_SEVERITY_HIGH, theMessage);

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
    theGlContext->PushMessage (GL_DEBUG_SOURCE_APPLICATION, GL_DEBUG_TYPE_ERROR, 0, GL_DEBUG_SEVERITY_HIGH,
                               TCollection_ExtendedString ("Error: Failed to create ") +
                               (theType == GL_VERTEX_SHADER ? "vertex" : "fragment") + " shader object");
    aShader->Release (theGlContext.get());
    return Handle(OpenGl_ShaderObject)();
  }

  if (!aShader->LoadAndCompile (theGlContext, "", theSource.Source (theGlContext, theType)))
  {
    aShader->Release (theGlContext.get());
    return Handle(OpenGl_ShaderObject)();
  }
  return aShader;
}

// =======================================================================
// function : initProgram
// purpose  : Creates GLSL program from the given shader objects
// =======================================================================
Handle(OpenGl_ShaderProgram) OpenGl_View::initProgram (const Handle(OpenGl_Context)&      theGlContext,
                                                       const Handle(OpenGl_ShaderObject)& theVertShader,
                                                       const Handle(OpenGl_ShaderObject)& theFragShader,
                                                       const TCollection_AsciiString& theName)
{
  const TCollection_AsciiString anId = TCollection_AsciiString("occt_rt_") + theName;
  Handle(OpenGl_ShaderProgram) aProgram = new OpenGl_ShaderProgram(Handle(Graphic3d_ShaderProgram)(), anId);

  if (!aProgram->Create (theGlContext))
  {
    theVertShader->Release (theGlContext.operator->());

    theGlContext->PushMessage (GL_DEBUG_SOURCE_APPLICATION,
      GL_DEBUG_TYPE_ERROR, 0, GL_DEBUG_SEVERITY_HIGH, "Failed to create shader program");

    return Handle(OpenGl_ShaderProgram)();
  }

  if (!aProgram->AttachShader (theGlContext, theVertShader)
   || !aProgram->AttachShader (theGlContext, theFragShader))
  {
    theVertShader->Release (theGlContext.operator->());

    theGlContext->PushMessage (GL_DEBUG_SOURCE_APPLICATION,
      GL_DEBUG_TYPE_ERROR, 0, GL_DEBUG_SEVERITY_HIGH, "Failed to attach shader objects");

    return Handle(OpenGl_ShaderProgram)();
  }

  aProgram->SetAttributeName (theGlContext, Graphic3d_TOA_POS, "occVertex");

  TCollection_AsciiString aLinkLog;

  if (!aProgram->Link (theGlContext))
  {
    aProgram->FetchInfoLog (theGlContext, aLinkLog);

    const TCollection_ExtendedString aMessage = TCollection_ExtendedString (
      "Failed to link shader program:\n") + aLinkLog;

    theGlContext->PushMessage (GL_DEBUG_SOURCE_APPLICATION,
      GL_DEBUG_TYPE_ERROR, 0, GL_DEBUG_SEVERITY_HIGH, aMessage);

    return Handle(OpenGl_ShaderProgram)();
  }
  else if (theGlContext->caps->glslWarnings)
  {
    aProgram->FetchInfoLog (theGlContext, aLinkLog);
    if (!aLinkLog.IsEmpty() && !aLinkLog.IsEqual ("No errors.\n"))
    {
      const TCollection_ExtendedString aMessage = TCollection_ExtendedString (
        "Shader program was linked with following warnings:\n") + aLinkLog;

      theGlContext->PushMessage (GL_DEBUG_SOURCE_APPLICATION,
        GL_DEBUG_TYPE_PORTABILITY, 0, GL_DEBUG_SEVERITY_LOW, aMessage);
    }
  }

  return aProgram;
}

// =======================================================================
// function : initRaytraceResources
// purpose  : Initializes OpenGL/GLSL shader programs
// =======================================================================
Standard_Boolean OpenGl_View::initRaytraceResources (const Standard_Integer theSizeX,
                                                     const Standard_Integer theSizeY,
                                                     const Handle(OpenGl_Context)& theGlContext)
{
  if (myRaytraceInitStatus == OpenGl_RT_FAIL)
  {
    return Standard_False;
  }

  Standard_Boolean aToRebuildShaders = Standard_False;

  if (myRenderParams.RebuildRayTracingShaders) // requires complete re-initialization
  {
    myRaytraceInitStatus = OpenGl_RT_NONE;
    releaseRaytraceResources (theGlContext, Standard_True);
    myRenderParams.RebuildRayTracingShaders = Standard_False; // clear rebuilding flag
  }

  if (myRaytraceInitStatus == OpenGl_RT_INIT)
  {
    if (!myIsRaytraceDataValid)
    {
      return Standard_True;
    }

    const Standard_Integer aRequiredStackSize =
      myRaytraceGeometry.TopLevelTreeDepth() + myRaytraceGeometry.BotLevelTreeDepth();

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

    const bool isZeroToOneDepth = myCaps->useZeroToOneDepth
                               && myWorkspace->GetGlContext()->arbClipControl;
    if (isZeroToOneDepth                           != myRaytraceParameters.IsZeroToOneDepth
     || myRenderParams.RaytracingDepth             != myRaytraceParameters.NbBounces
     || myRenderParams.IsTransparentShadowEnabled  != myRaytraceParameters.TransparentShadows
     || myRenderParams.IsGlobalIlluminationEnabled != myRaytraceParameters.GlobalIllumination
     || myRenderParams.TwoSidedBsdfModels          != myRaytraceParameters.TwoSidedBsdfModels
     || myRaytraceGeometry.HasTextures()           != myRaytraceParameters.UseBindlessTextures
     || myRenderParams.ToIgnoreNormalMapInRayTracing != myRaytraceParameters.ToIgnoreNormalMap)
    {
      myRaytraceParameters.IsZeroToOneDepth    = isZeroToOneDepth;
      myRaytraceParameters.NbBounces           = myRenderParams.RaytracingDepth;
      myRaytraceParameters.TransparentShadows  = myRenderParams.IsTransparentShadowEnabled;
      myRaytraceParameters.GlobalIllumination  = myRenderParams.IsGlobalIlluminationEnabled;
      myRaytraceParameters.TwoSidedBsdfModels  = myRenderParams.TwoSidedBsdfModels;
      myRaytraceParameters.UseBindlessTextures = myRaytraceGeometry.HasTextures();
      myRaytraceParameters.ToIgnoreNormalMap     = myRenderParams.ToIgnoreNormalMapInRayTracing;
      aToRebuildShaders = Standard_True;
    }

    if (myRenderParams.AdaptiveScreenSampling       != myRaytraceParameters.AdaptiveScreenSampling
     || myRenderParams.AdaptiveScreenSamplingAtomic != myRaytraceParameters.AdaptiveScreenSamplingAtomic)
    {
      myRaytraceParameters.AdaptiveScreenSampling       = myRenderParams.AdaptiveScreenSampling;
      myRaytraceParameters.AdaptiveScreenSamplingAtomic = myRenderParams.AdaptiveScreenSamplingAtomic;
      if (myRenderParams.AdaptiveScreenSampling) // adaptive sampling was requested
      {
        if (!theGlContext->HasRayTracingAdaptiveSampling())
        {
          // disable the feature if it is not supported
          myRaytraceParameters.AdaptiveScreenSampling = myRenderParams.AdaptiveScreenSampling = Standard_False;
          theGlContext->PushMessage (GL_DEBUG_SOURCE_APPLICATION, GL_DEBUG_TYPE_PORTABILITY, 0, GL_DEBUG_SEVERITY_LOW,
                                     "Adaptive sampling is not supported (OpenGL 4.4 is missing)");
        }
        else if (myRaytraceParameters.AdaptiveScreenSamplingAtomic
             && !theGlContext->HasRayTracingAdaptiveSamplingAtomic())
        {
          // disable the feature if it is not supported
          myRaytraceParameters.AdaptiveScreenSamplingAtomic = myRenderParams.AdaptiveScreenSamplingAtomic = Standard_False;
          theGlContext->PushMessage (GL_DEBUG_SOURCE_APPLICATION, GL_DEBUG_TYPE_PORTABILITY, 0, GL_DEBUG_SEVERITY_LOW,
                                     "Atomic adaptive sampling is not supported (GL_NV_shader_atomic_float is missing)");
        }
      }

      aToRebuildShaders = Standard_True;
    }
    myTileSampler.SetSize (myRenderParams, myRaytraceParameters.AdaptiveScreenSampling ? Graphic3d_Vec2i (theSizeX, theSizeY) : Graphic3d_Vec2i (0, 0));

    const bool isCubemapForBack = !myCubeMapBackground.IsNull();
    if (myRaytraceParameters.CubemapForBack != isCubemapForBack)
    {
      myRaytraceParameters.CubemapForBack = isCubemapForBack;
      aToRebuildShaders = Standard_True;
    }

    const bool toEnableDof = !myCamera->IsOrthographic() && myRaytraceParameters.GlobalIllumination;
    if (myRaytraceParameters.DepthOfField != toEnableDof)
    {
      myRaytraceParameters.DepthOfField = toEnableDof;
      aToRebuildShaders = Standard_True;
    }

    if (myRenderParams.ToneMappingMethod != myRaytraceParameters.ToneMappingMethod)
    {
      myRaytraceParameters.ToneMappingMethod = myRenderParams.ToneMappingMethod;
      aToRebuildShaders = true;
    }

    if (aToRebuildShaders)
    {
      // Reject accumulated frames
      myAccumFrames = 0;

      // Environment map should be updated
      myToUpdateEnvironmentMap = Standard_True;

      const TCollection_AsciiString aPrefixString = generateShaderPrefix (theGlContext);
#ifdef RAY_TRACE_PRINT_INFO
      Message::SendTrace() << "GLSL prefix string:" << std::endl << aPrefixString;
#endif
      myRaytraceShaderSource.SetPrefix (aPrefixString);
      myPostFSAAShaderSource.SetPrefix (aPrefixString);
      myOutImageShaderSource.SetPrefix (aPrefixString);
      if (!myRaytraceShader->LoadAndCompile (theGlContext, myRaytraceProgram->ResourceId(), myRaytraceShaderSource.Source (theGlContext, GL_FRAGMENT_SHADER))
       || !myPostFSAAShader->LoadAndCompile (theGlContext, myPostFSAAProgram->ResourceId(), myPostFSAAShaderSource.Source (theGlContext, GL_FRAGMENT_SHADER))
       || !myOutImageShader->LoadAndCompile (theGlContext, myOutImageProgram->ResourceId(), myOutImageShaderSource.Source (theGlContext, GL_FRAGMENT_SHADER)))
      {
        return safeFailBack ("Failed to compile ray-tracing fragment shaders", theGlContext);
      }

      myRaytraceProgram->SetAttributeName (theGlContext, Graphic3d_TOA_POS, "occVertex");
      myPostFSAAProgram->SetAttributeName (theGlContext, Graphic3d_TOA_POS, "occVertex");
      myOutImageProgram->SetAttributeName (theGlContext, Graphic3d_TOA_POS, "occVertex");
      if (!myRaytraceProgram->Link (theGlContext)
       || !myPostFSAAProgram->Link (theGlContext)
       || !myOutImageProgram->Link (theGlContext))
      {
        return safeFailBack ("Failed to initialize vertex attributes for ray-tracing program", theGlContext);
      }
    }
  }

  if (myRaytraceInitStatus == OpenGl_RT_NONE)
  {
    myAccumFrames = 0; // accumulation should be restarted

    if (theGlContext->GraphicsLibrary() == Aspect_GraphicsLibrary_OpenGLES)
    {
      if (!theGlContext->IsGlGreaterEqual (3, 2))
      {
        return safeFailBack ("Ray-tracing requires OpenGL ES 3.2 and higher", theGlContext);
      }
    }
    else
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
    }

    myRaytraceParameters.NbBounces = myRenderParams.RaytracingDepth;

    const TCollection_AsciiString aShaderFolder = Graphic3d_ShaderProgram::ShadersFolder();
    if (myIsRaytraceDataValid)
    {
      myRaytraceParameters.StackSize = Max (THE_DEFAULT_STACK_SIZE,
        myRaytraceGeometry.TopLevelTreeDepth() + myRaytraceGeometry.BotLevelTreeDepth());
    }

    const TCollection_AsciiString aPrefixString  = generateShaderPrefix (theGlContext);

#ifdef RAY_TRACE_PRINT_INFO
    Message::SendTrace() << "GLSL prefix string:" << std::endl << aPrefixString;
#endif

    ShaderSource aBasicVertShaderSrc;
    {
      if (!aShaderFolder.IsEmpty())
      {
        const TCollection_AsciiString aFiles[] = { aShaderFolder + "/RaytraceBase.vs", "" };
        if (!aBasicVertShaderSrc.LoadFromFiles (aFiles))
        {
          return safeFailBack (aBasicVertShaderSrc.ErrorDescription(), theGlContext);
        }
      }
      else
      {
        const TCollection_AsciiString aSrcShaders[] = { Shaders_RaytraceBase_vs, "" };
        aBasicVertShaderSrc.LoadFromStrings (aSrcShaders);
      }
    }

    {
      if (!aShaderFolder.IsEmpty())
      {
        const TCollection_AsciiString aFiles[] = { aShaderFolder + "/RaytraceBase.fs",
                                                   aShaderFolder + "/TangentSpaceNormal.glsl",
                                                   aShaderFolder + "/PathtraceBase.fs",
                                                   aShaderFolder + "/RaytraceRender.fs",
                                                   "" };
        if (!myRaytraceShaderSource.LoadFromFiles (aFiles, aPrefixString))
        {
          return safeFailBack (myRaytraceShaderSource.ErrorDescription(), theGlContext);
        }
      }
      else
      {
        const TCollection_AsciiString aSrcShaders[] = { Shaders_RaytraceBase_fs,
                                                        Shaders_TangentSpaceNormal_glsl,
                                                        Shaders_PathtraceBase_fs,
                                                        Shaders_RaytraceRender_fs,
                                                        "" };
        myRaytraceShaderSource.LoadFromStrings (aSrcShaders, aPrefixString);
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

      myRaytraceProgram = initProgram (theGlContext, aBasicVertShader, myRaytraceShader, "main");
      if (myRaytraceProgram.IsNull())
      {
        return safeFailBack ("Failed to initialize ray-trace shader program", theGlContext);
      }
    }

    {
      if (!aShaderFolder.IsEmpty())
      {
        const TCollection_AsciiString aFiles[] = { aShaderFolder + "/RaytraceBase.fs", aShaderFolder + "/RaytraceSmooth.fs", "" };
        if (!myPostFSAAShaderSource.LoadFromFiles (aFiles, aPrefixString))
        {
          return safeFailBack (myPostFSAAShaderSource.ErrorDescription(), theGlContext);
        }
      }
      else
      {
        const TCollection_AsciiString aSrcShaders[] = { Shaders_RaytraceBase_fs, Shaders_RaytraceSmooth_fs, "" };
        myPostFSAAShaderSource.LoadFromStrings (aSrcShaders, aPrefixString);
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

      myPostFSAAProgram = initProgram (theGlContext, aBasicVertShader, myPostFSAAShader, "fsaa");
      if (myPostFSAAProgram.IsNull())
      {
        return safeFailBack ("Failed to initialize FSAA shader program", theGlContext);
      }
    }

    {
      if (!aShaderFolder.IsEmpty())
      {
        const TCollection_AsciiString aFiles[] = { aShaderFolder + "/Display.fs", "" };
        if (!myOutImageShaderSource.LoadFromFiles (aFiles, aPrefixString))
        {
          return safeFailBack (myOutImageShaderSource.ErrorDescription(), theGlContext);
        }
      }
      else
      {
        const TCollection_AsciiString aSrcShaders[] = { Shaders_Display_fs, "" };
        myOutImageShaderSource.LoadFromStrings (aSrcShaders, aPrefixString);
      }

      Handle(OpenGl_ShaderObject) aBasicVertShader = initShader (GL_VERTEX_SHADER, aBasicVertShaderSrc, theGlContext);
      if (aBasicVertShader.IsNull())
      {
        return safeFailBack ("Failed to set vertex shader source", theGlContext);
      }

      myOutImageShader = initShader (GL_FRAGMENT_SHADER, myOutImageShaderSource, theGlContext);
      if (myOutImageShader.IsNull())
      {
        aBasicVertShader->Release (theGlContext.operator->());
        return safeFailBack ("Failed to set display fragment shader source", theGlContext);
      }

      myOutImageProgram = initProgram (theGlContext, aBasicVertShader, myOutImageShader, "out");
      if (myOutImageProgram.IsNull())
      {
        return safeFailBack ("Failed to initialize display shader program", theGlContext);
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
        "uEnvMapTexture", OpenGl_RT_EnvMapTexture);
      aShaderProgram->SetSampler (theGlContext,
        "uRaytraceMaterialTexture", OpenGl_RT_RaytraceMaterialTexture);
      aShaderProgram->SetSampler (theGlContext,
        "uRaytraceLightSrcTexture", OpenGl_RT_RaytraceLightSrcTexture);

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
      myUniformLocations[anIndex][OpenGl_RT_uViewPrMat] =
        aShaderProgram->GetUniformLocation (theGlContext, "uViewMat");
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

      myUniformLocations[anIndex][OpenGl_RT_uFsaaOffset] =
        aShaderProgram->GetUniformLocation (theGlContext, "uFsaaOffset");
      myUniformLocations[anIndex][OpenGl_RT_uSamples] =
        aShaderProgram->GetUniformLocation (theGlContext, "uSamples");

      myUniformLocations[anIndex][OpenGl_RT_uTexSamplersArray] =
        aShaderProgram->GetUniformLocation (theGlContext, "uTextureSamplers");

      myUniformLocations[anIndex][OpenGl_RT_uShadowsEnabled] =
        aShaderProgram->GetUniformLocation (theGlContext, "uShadowsEnabled");
      myUniformLocations[anIndex][OpenGl_RT_uReflectEnabled] =
        aShaderProgram->GetUniformLocation (theGlContext, "uReflectEnabled");
      myUniformLocations[anIndex][OpenGl_RT_uEnvMapEnabled] =
        aShaderProgram->GetUniformLocation (theGlContext, "uEnvMapEnabled");
      myUniformLocations[anIndex][OpenGl_RT_uEnvMapForBack] =
        aShaderProgram->GetUniformLocation (theGlContext, "uEnvMapForBack");
      myUniformLocations[anIndex][OpenGl_RT_uBlockedRngEnabled] =
        aShaderProgram->GetUniformLocation (theGlContext, "uBlockedRngEnabled");

      myUniformLocations[anIndex][OpenGl_RT_uWinSizeX] =
        aShaderProgram->GetUniformLocation (theGlContext, "uWinSizeX");
      myUniformLocations[anIndex][OpenGl_RT_uWinSizeY] =
        aShaderProgram->GetUniformLocation (theGlContext, "uWinSizeY");

      myUniformLocations[anIndex][OpenGl_RT_uAccumSamples] =
        aShaderProgram->GetUniformLocation (theGlContext, "uAccumSamples");
      myUniformLocations[anIndex][OpenGl_RT_uFrameRndSeed] =
        aShaderProgram->GetUniformLocation (theGlContext, "uFrameRndSeed");

      myUniformLocations[anIndex][OpenGl_RT_uRenderImage] =
        aShaderProgram->GetUniformLocation (theGlContext, "uRenderImage");
      myUniformLocations[anIndex][OpenGl_RT_uTilesImage] =
        aShaderProgram->GetUniformLocation (theGlContext, "uTilesImage");
      myUniformLocations[anIndex][OpenGl_RT_uOffsetImage] =
        aShaderProgram->GetUniformLocation (theGlContext, "uOffsetImage");
      myUniformLocations[anIndex][OpenGl_RT_uTileSize] =
        aShaderProgram->GetUniformLocation (theGlContext, "uTileSize");
      myUniformLocations[anIndex][OpenGl_RT_uVarianceScaleFactor] =
        aShaderProgram->GetUniformLocation (theGlContext, "uVarianceScaleFactor");

      myUniformLocations[anIndex][OpenGl_RT_uBackColorTop] =
        aShaderProgram->GetUniformLocation (theGlContext, "uBackColorTop");
      myUniformLocations[anIndex][OpenGl_RT_uBackColorBot] =
        aShaderProgram->GetUniformLocation (theGlContext, "uBackColorBot");

      myUniformLocations[anIndex][OpenGl_RT_uMaxRadiance] =
        aShaderProgram->GetUniformLocation (theGlContext, "uMaxRadiance");
    }

    theGlContext->BindProgram (myOutImageProgram);

    myOutImageProgram->SetSampler (theGlContext,
      "uInputTexture", OpenGl_RT_PrevAccumTexture);

    myOutImageProgram->SetSampler (theGlContext,
      "uDepthTexture", OpenGl_RT_RaytraceDepthTexture);

    theGlContext->BindProgram (NULL);
  }

  if (myRaytraceInitStatus != OpenGl_RT_NONE)
  {
    return myRaytraceInitStatus == OpenGl_RT_INIT;
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
// purpose  : Releases OpenGL resource
// =======================================================================
template <class T>
inline void nullifyResource (const Handle(OpenGl_Context)& theGlContext, Handle(T)& theResource)
{
  if (!theResource.IsNull())
  {
    theResource->Release (theGlContext.get());
    theResource.Nullify();
  }
}

// =======================================================================
// function : releaseRaytraceResources
// purpose  : Releases OpenGL/GLSL shader programs
// =======================================================================
void OpenGl_View::releaseRaytraceResources (const Handle(OpenGl_Context)& theGlContext, const Standard_Boolean theToRebuild)
{
  // release shader resources
  nullifyResource (theGlContext, myRaytraceShader);
  nullifyResource (theGlContext, myPostFSAAShader);

  nullifyResource (theGlContext, myRaytraceProgram);
  nullifyResource (theGlContext, myPostFSAAProgram);
  nullifyResource (theGlContext, myOutImageProgram);

  if (!theToRebuild) // complete release
  {
    myRaytraceFBO1[0]->Release (theGlContext.get());
    myRaytraceFBO1[1]->Release (theGlContext.get());
    myRaytraceFBO2[0]->Release (theGlContext.get());
    myRaytraceFBO2[1]->Release (theGlContext.get());

    nullifyResource (theGlContext, myRaytraceOutputTexture[0]);
    nullifyResource (theGlContext, myRaytraceOutputTexture[1]);

    nullifyResource (theGlContext, myRaytraceTileOffsetsTexture[0]);
    nullifyResource (theGlContext, myRaytraceTileOffsetsTexture[1]);
    nullifyResource (theGlContext, myRaytraceVisualErrorTexture[0]);
    nullifyResource (theGlContext, myRaytraceVisualErrorTexture[1]);
    nullifyResource (theGlContext, myRaytraceTileSamplesTexture[0]);
    nullifyResource (theGlContext, myRaytraceTileSamplesTexture[1]);

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

    if (myRaytraceScreenQuad.IsValid ())
    {
      myRaytraceScreenQuad.Release (theGlContext.get());
    }
  }
}

// =======================================================================
// function : updateRaytraceBuffers
// purpose  : Updates auxiliary OpenGL frame buffers.
// =======================================================================
Standard_Boolean OpenGl_View::updateRaytraceBuffers (const Standard_Integer        theSizeX,
                                                     const Standard_Integer        theSizeY,
                                                     const Handle(OpenGl_Context)& theGlContext)
{
  // Auxiliary buffers are not used
  if (!myRaytraceParameters.GlobalIllumination && !myRenderParams.IsAntialiasingEnabled)
  {
    myRaytraceFBO1[0]->Release (theGlContext.operator->());
    myRaytraceFBO2[0]->Release (theGlContext.operator->());
    myRaytraceFBO1[1]->Release (theGlContext.operator->());
    myRaytraceFBO2[1]->Release (theGlContext.operator->());

    return Standard_True;
  }

  if (myRaytraceParameters.AdaptiveScreenSampling)
  {
    Graphic3d_Vec2i aMaxViewport = myTileSampler.OffsetTilesViewportMax().cwiseMax (Graphic3d_Vec2i (theSizeX, theSizeY));
    myRaytraceFBO1[0]->InitLazy (theGlContext, aMaxViewport, GL_RGBA32F, myFboDepthFormat);
    myRaytraceFBO2[0]->InitLazy (theGlContext, aMaxViewport, GL_RGBA32F, myFboDepthFormat);
    if (myRaytraceFBO1[1]->IsValid()) // second FBO not needed
    {
      myRaytraceFBO1[1]->Release (theGlContext.operator->());
      myRaytraceFBO2[1]->Release (theGlContext.operator->());
    }
  }

  for (int aViewIter = 0; aViewIter < 2; ++aViewIter)
  {
    if (myRaytraceTileOffsetsTexture[aViewIter].IsNull())
    {
      myRaytraceOutputTexture[aViewIter] = new OpenGl_Texture();
      myRaytraceVisualErrorTexture[aViewIter] = new OpenGl_Texture();
      myRaytraceTileSamplesTexture[aViewIter] = new OpenGl_Texture();
      myRaytraceTileOffsetsTexture[aViewIter] = new OpenGl_Texture();
    }

    if (aViewIter == 1
     && myCamera->ProjectionType() != Graphic3d_Camera::Projection_Stereo)
    {
      myRaytraceFBO1[1]->Release (theGlContext.operator->());
      myRaytraceFBO2[1]->Release (theGlContext.operator->());
      myRaytraceOutputTexture[1]->Release (theGlContext.operator->());
      myRaytraceVisualErrorTexture[1]->Release (theGlContext.operator->());
      myRaytraceTileOffsetsTexture[1]->Release (theGlContext.operator->());
      continue;
    }

    if (myRaytraceParameters.AdaptiveScreenSampling)
    {
      if (myRaytraceOutputTexture[aViewIter]->SizeX() / 3 == theSizeX
       && myRaytraceOutputTexture[aViewIter]->SizeY() / 2 == theSizeY
       && myRaytraceVisualErrorTexture[aViewIter]->SizeX() == myTileSampler.NbTilesX()
       && myRaytraceVisualErrorTexture[aViewIter]->SizeY() == myTileSampler.NbTilesY())
      {
        if (myRaytraceParameters.AdaptiveScreenSamplingAtomic)
        {
          continue; // offsets texture is dynamically resized
        }
        else if (myRaytraceTileSamplesTexture[aViewIter]->SizeX() == myTileSampler.NbTilesX()
              && myRaytraceTileSamplesTexture[aViewIter]->SizeY() == myTileSampler.NbTilesY())
        {
          continue;
        }
      }

      myAccumFrames = 0;

      // Due to limitations of OpenGL image load-store extension
      // atomic operations are supported only for single-channel
      // images, so we define GL_R32F image. It is used as array
      // of 6D floating point vectors:
      // 0 - R color channel
      // 1 - G color channel
      // 2 - B color channel
      // 3 - hit time transformed into OpenGL NDC space
      // 4 - luminance accumulated for odd samples only
      myRaytraceOutputTexture[aViewIter]->InitRectangle (theGlContext, theSizeX * 3, theSizeY * 2, OpenGl_TextureFormat::Create<GLfloat, 1>());

      // workaround for some NVIDIA drivers
      myRaytraceVisualErrorTexture[aViewIter]->Release (theGlContext.operator->());
      myRaytraceTileSamplesTexture[aViewIter]->Release (theGlContext.operator->());
      myRaytraceVisualErrorTexture[aViewIter]->Init (theGlContext,
                                                     OpenGl_TextureFormat::FindSizedFormat (theGlContext, GL_R32I),
                                                     Graphic3d_Vec2i (myTileSampler.NbTilesX(), myTileSampler.NbTilesY()),
                                                     Graphic3d_TypeOfTexture_2D);
      if (!myRaytraceParameters.AdaptiveScreenSamplingAtomic)
      {
        myRaytraceTileSamplesTexture[aViewIter]->Init (theGlContext,
                                                       OpenGl_TextureFormat::FindSizedFormat (theGlContext, GL_R32I),
                                                       Graphic3d_Vec2i (myTileSampler.NbTilesX(), myTileSampler.NbTilesY()),
                                                       Graphic3d_TypeOfTexture_2D);
      }
    }
    else // non-adaptive mode
    {
      if (myRaytraceFBO1[aViewIter]->GetSizeX() != theSizeX
       || myRaytraceFBO1[aViewIter]->GetSizeY() != theSizeY)
      {
        myAccumFrames = 0; // accumulation should be restarted
      }

      myRaytraceFBO1[aViewIter]->InitLazy (theGlContext, Graphic3d_Vec2i (theSizeX, theSizeY), GL_RGBA32F, myFboDepthFormat);
      myRaytraceFBO2[aViewIter]->InitLazy (theGlContext, Graphic3d_Vec2i (theSizeX, theSizeY), GL_RGBA32F, myFboDepthFormat);
    }
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
                                OpenGl_Mat4&       theViewPr,
                                OpenGl_Mat4&       theUnview)
{
  // compute view-projection matrix
  theViewPr = theViewMapping * theOrientation;

  // compute inverse view-projection matrix
  theViewPr.Inverted (theUnview);

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
// function : updatePerspCameraPT
// purpose  : Generates viewing rays (path tracing, perspective camera)
// =======================================================================
void OpenGl_View::updatePerspCameraPT (const OpenGl_Mat4&           theOrientation,
                                       const OpenGl_Mat4&           theViewMapping,
                                       Graphic3d_Camera::Projection theProjection,
                                       OpenGl_Mat4&                 theViewPr,
                                       OpenGl_Mat4&                 theUnview,
                                       const int                    theWinSizeX,
                                       const int                    theWinSizeY)
{
  // compute view-projection matrix
  theViewPr = theViewMapping * theOrientation;

  // compute inverse view-projection matrix
  theViewPr.Inverted(theUnview);
  
  // get camera stereo params
  float anIOD = myCamera->GetIODType() == Graphic3d_Camera::IODType_Relative
    ? static_cast<float> (myCamera->IOD() * myCamera->Distance())
    : static_cast<float> (myCamera->IOD());

  float aZFocus = myCamera->ZFocusType() == Graphic3d_Camera::FocusType_Relative
    ? static_cast<float> (myCamera->ZFocus() * myCamera->Distance())
    : static_cast<float> (myCamera->ZFocus());

  // get camera view vectors
  const gp_Pnt anOrig = myCamera->Eye();

  myEyeOrig = OpenGl_Vec3 (static_cast<float> (anOrig.X()),
                           static_cast<float> (anOrig.Y()),
                           static_cast<float> (anOrig.Z()));

  const gp_Dir aView = myCamera->Direction();

  OpenGl_Vec3 anEyeViewMono = OpenGl_Vec3 (static_cast<float> (aView.X()),
                                           static_cast<float> (aView.Y()),
                                           static_cast<float> (aView.Z()));

  const gp_Dir anUp = myCamera->Up();

  myEyeVert = OpenGl_Vec3 (static_cast<float> (anUp.X()),
                           static_cast<float> (anUp.Y()),
                           static_cast<float> (anUp.Z()));

  myEyeSide = OpenGl_Vec3::Cross (anEyeViewMono, myEyeVert);

  const double aScaleY = tan (myCamera->FOVy() / 360 * M_PI);
  const double aScaleX = theWinSizeX * aScaleY / theWinSizeY;
 
  myEyeSize = OpenGl_Vec2 (static_cast<float> (aScaleX),
                           static_cast<float> (aScaleY));

  if (theProjection == Graphic3d_Camera::Projection_Perspective)
  {
    myEyeView = anEyeViewMono;
  }
  else // stereo camera
  {
    // compute z-focus point
    OpenGl_Vec3 aZFocusPoint = myEyeOrig + anEyeViewMono * aZFocus;

    // compute stereo camera shift
    float aDx = theProjection == Graphic3d_Camera::Projection_MonoRightEye ? 0.5f * anIOD : -0.5f * anIOD;
    myEyeOrig += myEyeSide.Normalized() * aDx;

    // estimate new camera direction vector and correct its length
    myEyeView = (aZFocusPoint - myEyeOrig).Normalized();
    myEyeView *= 1.f / anEyeViewMono.Dot (myEyeView);
  }
}

// =======================================================================
// function : uploadRaytraceData
// purpose  : Uploads ray-trace data to the GPU
// =======================================================================
Standard_Boolean OpenGl_View::uploadRaytraceData (const Handle(OpenGl_Context)& theGlContext)
{
  if (theGlContext->GraphicsLibrary() == Aspect_GraphicsLibrary_OpenGLES)
  {
    if (!theGlContext->IsGlGreaterEqual (3, 2))
    {
      Message::SendFail() << "Error: OpenGL ES version is less than 3.2";
      return Standard_False;
    }
  }
  else
  {
    if (!theGlContext->IsGlGreaterEqual (3, 1))
    {
      Message::SendFail() << "Error: OpenGL version is less than 3.1";
      return Standard_False;
    }
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
      Message::SendTrace() << "Error: Failed to get OpenGL texture handles";
      return Standard_False;
    }
  }

  /////////////////////////////////////////////////////////////////////////////
  // Create OpenGL BVH buffers

  if (mySceneNodeInfoTexture.IsNull()) // create scene BVH buffers
  {
    mySceneNodeInfoTexture  = new OpenGl_TextureBuffer();
    mySceneMinPointTexture  = new OpenGl_TextureBuffer();
    mySceneMaxPointTexture  = new OpenGl_TextureBuffer();
    mySceneTransformTexture = new OpenGl_TextureBuffer();

    if (!mySceneNodeInfoTexture->Create  (theGlContext)
     || !mySceneMinPointTexture->Create  (theGlContext)
     || !mySceneMaxPointTexture->Create  (theGlContext)
     || !mySceneTransformTexture->Create (theGlContext))
    {
      Message::SendTrace() << "Error: Failed to create scene BVH buffers";
      return Standard_False;
    }
  }

  if (myGeometryVertexTexture.IsNull()) // create geometry buffers
  {
    myGeometryVertexTexture = new OpenGl_TextureBuffer();
    myGeometryNormalTexture = new OpenGl_TextureBuffer();
    myGeometryTexCrdTexture = new OpenGl_TextureBuffer();
    myGeometryTriangTexture = new OpenGl_TextureBuffer();

    if (!myGeometryVertexTexture->Create (theGlContext)
     || !myGeometryNormalTexture->Create (theGlContext)
     || !myGeometryTexCrdTexture->Create (theGlContext)
     || !myGeometryTriangTexture->Create (theGlContext))
    {
      Message::SendTrace() << "\nError: Failed to create buffers for triangulation data";
      return Standard_False;
    }
  }

  if (myRaytraceMaterialTexture.IsNull()) // create material buffer
  {
    myRaytraceMaterialTexture = new OpenGl_TextureBuffer();
    if (!myRaytraceMaterialTexture->Create (theGlContext))
    {
      Message::SendTrace() << "Error: Failed to create buffers for material data";
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

    const BVH_Transform<Standard_ShortReal, 4>* aTransform = dynamic_cast<const BVH_Transform<Standard_ShortReal, 4>* > (aTriangleSet->Properties().get());
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

    Standard_ASSERT_RETURN (!aTriangleSet->QuadBVH().IsNull(),
      "Error: Failed to get bottom-level BVH of OpenGL element", Standard_False);

    aTotalBVHNodesNb += aTriangleSet->QuadBVH()->NodeInfoBuffer().size();
  }

  aTotalBVHNodesNb += myRaytraceGeometry.QuadBVH()->NodeInfoBuffer().size();

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
    Message::SendTrace() << "Error: Failed to upload buffers for bottom-level scene BVH";
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
    Message::SendTrace() << "Error: Failed to upload buffers for scene geometry";
    return Standard_False;
  }

  const QuadBvhHandle& aBVH = myRaytraceGeometry.QuadBVH();

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

    const Standard_Integer aBvhBuffersSize = aTriangleSet->QuadBVH()->Length();

    if (aBvhBuffersSize != 0)
    {
      aResult &= mySceneNodeInfoTexture->SubData (theGlContext, aBVHOffset, aBvhBuffersSize,
        reinterpret_cast<const GLuint*> (&aTriangleSet->QuadBVH()->NodeInfoBuffer().front()));
      aResult &= mySceneMinPointTexture->SubData (theGlContext, aBVHOffset, aBvhBuffersSize,
        reinterpret_cast<const GLfloat*> (&aTriangleSet->QuadBVH()->MinPointBuffer().front()));
      aResult &= mySceneMaxPointTexture->SubData (theGlContext, aBVHOffset, aBvhBuffersSize,
        reinterpret_cast<const GLfloat*> (&aTriangleSet->QuadBVH()->MaxPointBuffer().front()));

      if (!aResult)
      {
        Message::SendTrace() << "Error: Failed to upload buffers for bottom-level scene BVHs";
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
      Message::SendTrace() << "Error: Failed to upload triangulation buffers for OpenGL element";
      return Standard_False;
    }
  }

  /////////////////////////////////////////////////////////////////////////////
  // Write material buffer

  if (myRaytraceGeometry.Materials.size() != 0)
  {
    aResult &= myRaytraceMaterialTexture->Init (theGlContext, 4,
      GLsizei (myRaytraceGeometry.Materials.size() * 19), myRaytraceGeometry.Materials.front().Packed());

    if (!aResult)
    {
      Message::SendTrace() << "Error: Failed to upload material buffer";
      return Standard_False;
    }
  }

  myIsRaytraceDataValid = myRaytraceGeometry.Objects().Size() != 0;

#ifdef RAY_TRACE_PRINT_INFO

  Standard_ShortReal aMemTrgUsed = 0.f;
  Standard_ShortReal aMemBvhUsed = 0.f;

  for (Standard_Integer anElemIdx = 0; anElemIdx < myRaytraceGeometry.Size(); ++anElemIdx)
  {
    OpenGl_TriangleSet* aTriangleSet = dynamic_cast<OpenGl_TriangleSet*> (myRaytraceGeometry.Objects()(anElemIdx).get());

    aMemTrgUsed += static_cast<Standard_ShortReal> (
      aTriangleSet->Vertices.size() * sizeof (BVH_Vec3f));
    aMemTrgUsed += static_cast<Standard_ShortReal> (
      aTriangleSet->Normals.size() * sizeof (BVH_Vec3f));
    aMemTrgUsed += static_cast<Standard_ShortReal> (
      aTriangleSet->TexCrds.size() * sizeof (BVH_Vec2f));
    aMemTrgUsed += static_cast<Standard_ShortReal> (
      aTriangleSet->Elements.size() * sizeof (BVH_Vec4i));

    aMemBvhUsed += static_cast<Standard_ShortReal> (
      aTriangleSet->QuadBVH()->NodeInfoBuffer().size() * sizeof (BVH_Vec4i));
    aMemBvhUsed += static_cast<Standard_ShortReal> (
      aTriangleSet->QuadBVH()->MinPointBuffer().size() * sizeof (BVH_Vec3f));
    aMemBvhUsed += static_cast<Standard_ShortReal> (
      aTriangleSet->QuadBVH()->MaxPointBuffer().size() * sizeof (BVH_Vec3f));
  }

  aMemBvhUsed += static_cast<Standard_ShortReal> (
    myRaytraceGeometry.QuadBVH()->NodeInfoBuffer().size() * sizeof (BVH_Vec4i));
  aMemBvhUsed += static_cast<Standard_ShortReal> (
    myRaytraceGeometry.QuadBVH()->MinPointBuffer().size() * sizeof (BVH_Vec3f));
  aMemBvhUsed += static_cast<Standard_ShortReal> (
    myRaytraceGeometry.QuadBVH()->MaxPointBuffer().size() * sizeof (BVH_Vec3f));

  std::cout << "GPU Memory Used (Mb):\n"
    << "\tFor mesh: " << aMemTrgUsed / 1048576 << "\n"
    << "\tFor BVHs: " << aMemBvhUsed / 1048576 << "\n";

#endif

  return aResult;
}

// =======================================================================
// function : updateRaytraceLightSources
// purpose  : Updates 3D scene light sources for ray-tracing
// =======================================================================
Standard_Boolean OpenGl_View::updateRaytraceLightSources (const OpenGl_Mat4& theInvModelView, const Handle(OpenGl_Context)& theGlContext)
{
  std::vector<Handle(Graphic3d_CLight)> aLightSources;
  Graphic3d_Vec4 aNewAmbient (0.0f);
  if (myRenderParams.ShadingModel != Graphic3d_TypeOfShadingModel_Unlit
  && !myLights.IsNull())
  {
    aNewAmbient.SetValues (myLights->AmbientColor().rgb(), 0.0f);

    // move positional light sources at the front of the list
    aLightSources.reserve (myLights->Extent());
    for (Graphic3d_LightSet::Iterator aLightIter (myLights, Graphic3d_LightSet::IterationFilter_ExcludeDisabledAndAmbient);
         aLightIter.More(); aLightIter.Next())
    {
      const Graphic3d_CLight& aLight = *aLightIter.Value();
      if (aLight.Type() != Graphic3d_TypeOfLightSource_Directional)
      {
        aLightSources.push_back (aLightIter.Value());
      }
    }

    for (Graphic3d_LightSet::Iterator aLightIter (myLights, Graphic3d_LightSet::IterationFilter_ExcludeDisabledAndAmbient);
         aLightIter.More(); aLightIter.Next())
    {
      if (aLightIter.Value()->Type() == Graphic3d_TypeOfLightSource_Directional)
      {
        aLightSources.push_back (aLightIter.Value());
      }
    }
  }

  if (!myRaytraceGeometry.Ambient.IsEqual (aNewAmbient))
  {
    myAccumFrames = 0;
    myRaytraceGeometry.Ambient = aNewAmbient;
  }

  // get number of 'real' (not ambient) light sources
  const size_t aNbLights = aLightSources.size();
  Standard_Boolean wasUpdated = myRaytraceGeometry.Sources.size () != aNbLights;
  if (wasUpdated)
  {
    myRaytraceGeometry.Sources.resize (aNbLights);
  }

  for (size_t aLightIdx = 0, aRealIdx = 0; aLightIdx < aLightSources.size(); ++aLightIdx)
  {
    const Graphic3d_CLight& aLight = *aLightSources[aLightIdx];
    const Graphic3d_Vec4& aLightColor = aLight.PackedColor();
    BVH_Vec4f aEmission  (aLightColor.r() * aLight.Intensity(),
                          aLightColor.g() * aLight.Intensity(),
                          aLightColor.b() * aLight.Intensity(),
                          1.0f);

    BVH_Vec4f aPosition (-aLight.PackedDirectionRange().x(),
                         -aLight.PackedDirectionRange().y(),
                         -aLight.PackedDirectionRange().z(),
                         0.0f);

    if (aLight.Type() != Graphic3d_TypeOfLightSource_Directional)
    {
      aPosition = BVH_Vec4f (static_cast<float>(aLight.Position().X()),
                             static_cast<float>(aLight.Position().Y()),
                             static_cast<float>(aLight.Position().Z()),
                             1.0f);

      // store smoothing radius in W-component
      aEmission.w() = Max (aLight.Smoothness(), 0.f);
    }
    else
    {
      // store cosine of smoothing angle in W-component
      aEmission.w() = cosf (Min (Max (aLight.Smoothness(), 0.f), static_cast<Standard_ShortReal> (M_PI / 2.0)));
    }

    if (aLight.IsHeadlight())
    {
      aPosition = theInvModelView * aPosition;
    }

    for (int aK = 0; aK < 4; ++aK)
    {
      wasUpdated |= (aEmission[aK] != myRaytraceGeometry.Sources[aRealIdx].Emission[aK])
                 || (aPosition[aK] != myRaytraceGeometry.Sources[aRealIdx].Position[aK]);
    }

    if (wasUpdated)
    {
      myRaytraceGeometry.Sources[aRealIdx] = OpenGl_RaytraceLight (aEmission, aPosition);
    }

    ++aRealIdx;
  }

  if (myRaytraceLightSrcTexture.IsNull()) // create light source buffer
  {
    myRaytraceLightSrcTexture = new OpenGl_TextureBuffer();
  }

  if (myRaytraceGeometry.Sources.size() != 0 && wasUpdated)
  {
    const GLfloat* aDataPtr = myRaytraceGeometry.Sources.front().Packed();
    if (!myRaytraceLightSrcTexture->Init (theGlContext, 4, GLsizei (myRaytraceGeometry.Sources.size() * 2), aDataPtr))
    {
      Message::SendTrace() << "Error: Failed to upload light source buffer";
      return Standard_False;
    }

    myAccumFrames = 0; // accumulation should be restarted
  }

  return Standard_True;
}

// =======================================================================
// function : setUniformState
// purpose  : Sets uniform state for the given ray-tracing shader program
// =======================================================================
Standard_Boolean OpenGl_View::setUniformState (const Standard_Integer        theProgramId,
                                               const Standard_Integer        theWinSizeX,
                                               const Standard_Integer        theWinSizeY,
                                               Graphic3d_Camera::Projection  theProjection,
                                               const Handle(OpenGl_Context)& theGlContext)
{
  // Get projection state
  OpenGl_MatrixState<Standard_ShortReal>& aCntxProjectionState = theGlContext->ProjectionState;

  OpenGl_Mat4 aViewPrjMat;
  OpenGl_Mat4 anUnviewMat;
  OpenGl_Vec3 aOrigins[4];
  OpenGl_Vec3 aDirects[4];

  if (myCamera->IsOrthographic()
   || !myRenderParams.IsGlobalIlluminationEnabled)
  {
    updateCamera (myCamera->OrientationMatrixF(),
                  aCntxProjectionState.Current(),
                  aOrigins,
                  aDirects,
                  aViewPrjMat,
                  anUnviewMat);

    if (myRenderParams.UseEnvironmentMapBackground
     || myRaytraceParameters.CubemapForBack)
    {
      OpenGl_Mat4 aTempMat;
      OpenGl_Mat4 aTempInvMat;
      updatePerspCameraPT (myCamera->OrientationMatrixF(),
                           aCntxProjectionState.Current(),
                           theProjection,
                           aTempMat,
                           aTempInvMat,
                           theWinSizeX,
                           theWinSizeY);
    }
  }
  else
  {
    updatePerspCameraPT (myCamera->OrientationMatrixF(),
                         aCntxProjectionState.Current(),
                         theProjection,
                         aViewPrjMat,
                         anUnviewMat,
                         theWinSizeX,
                         theWinSizeY);
  }

  Handle(OpenGl_ShaderProgram)& theProgram = theProgramId == 0
                                           ? myRaytraceProgram
                                           : myPostFSAAProgram;

  if (theProgram.IsNull())
  {
    return Standard_False;
  }
  
  theProgram->SetUniform(theGlContext, "uEyeOrig", myEyeOrig);
  theProgram->SetUniform(theGlContext, "uEyeView", myEyeView);
  theProgram->SetUniform(theGlContext, "uEyeVert", myEyeVert);
  theProgram->SetUniform(theGlContext, "uEyeSide", myEyeSide);
  theProgram->SetUniform(theGlContext, "uEyeSize", myEyeSize);

  theProgram->SetUniform(theGlContext, "uApertureRadius", myRenderParams.CameraApertureRadius);
  theProgram->SetUniform(theGlContext, "uFocalPlaneDist", myRenderParams.CameraFocalPlaneDist);

  // Set camera state
  theProgram->SetUniform (theGlContext,
    myUniformLocations[theProgramId][OpenGl_RT_uOriginLB], aOrigins[0]);
  theProgram->SetUniform (theGlContext,
    myUniformLocations[theProgramId][OpenGl_RT_uOriginRB], aOrigins[1]);
  theProgram->SetUniform (theGlContext,
    myUniformLocations[theProgramId][OpenGl_RT_uOriginLT], aOrigins[2]);
  theProgram->SetUniform (theGlContext,
    myUniformLocations[theProgramId][OpenGl_RT_uOriginRT], aOrigins[3]);
  theProgram->SetUniform (theGlContext,
    myUniformLocations[theProgramId][OpenGl_RT_uDirectLB], aDirects[0]);
  theProgram->SetUniform (theGlContext,
    myUniformLocations[theProgramId][OpenGl_RT_uDirectRB], aDirects[1]);
  theProgram->SetUniform (theGlContext,
    myUniformLocations[theProgramId][OpenGl_RT_uDirectLT], aDirects[2]);
  theProgram->SetUniform (theGlContext,
    myUniformLocations[theProgramId][OpenGl_RT_uDirectRT], aDirects[3]);
  theProgram->SetUniform (theGlContext,
    myUniformLocations[theProgramId][OpenGl_RT_uViewPrMat], aViewPrjMat);
  theProgram->SetUniform (theGlContext,
    myUniformLocations[theProgramId][OpenGl_RT_uUnviewMat], anUnviewMat);

  // Set screen dimensions
  myRaytraceProgram->SetUniform (theGlContext,
    myUniformLocations[theProgramId][OpenGl_RT_uWinSizeX], theWinSizeX);
  myRaytraceProgram->SetUniform (theGlContext,
    myUniformLocations[theProgramId][OpenGl_RT_uWinSizeY], theWinSizeY);

  // Set 3D scene parameters
  theProgram->SetUniform (theGlContext,
    myUniformLocations[theProgramId][OpenGl_RT_uSceneRad], myRaytraceSceneRadius);
  theProgram->SetUniform (theGlContext,
    myUniformLocations[theProgramId][OpenGl_RT_uSceneEps], myRaytraceSceneEpsilon);

  // Set light source parameters
  const Standard_Integer aLightSourceBufferSize =
    static_cast<Standard_Integer> (myRaytraceGeometry.Sources.size());
  
  theProgram->SetUniform (theGlContext,
    myUniformLocations[theProgramId][OpenGl_RT_uLightCount], aLightSourceBufferSize);

  // Set array of 64-bit texture handles
  if (theGlContext->arbTexBindless != NULL && myRaytraceGeometry.HasTextures())
  {
    const std::vector<GLuint64>& aTextures = myRaytraceGeometry.TextureHandles();

    theProgram->SetUniform (theGlContext, myUniformLocations[theProgramId][OpenGl_RT_uTexSamplersArray],
      static_cast<GLsizei> (aTextures.size()), reinterpret_cast<const OpenGl_Vec2u*> (&aTextures.front()));
  }

  // Set background colors (only vertical gradient background supported)
  OpenGl_Vec4 aBackColorTop = myBgColor, aBackColorBot = myBgColor;
  if (myBackgrounds[Graphic3d_TOB_GRADIENT] != NULL
   && myBackgrounds[Graphic3d_TOB_GRADIENT]->IsDefined())
  {
    aBackColorTop = myBackgrounds[Graphic3d_TOB_GRADIENT]->GradientColor (0);
    aBackColorBot = myBackgrounds[Graphic3d_TOB_GRADIENT]->GradientColor (1);

    if (myCamera->Tile().IsValid())
    {
      Standard_Integer aTileOffset = myCamera->Tile().OffsetLowerLeft().y();
      Standard_Integer aTileSize = myCamera->Tile().TileSize.y();
      Standard_Integer aViewSize = myCamera->Tile().TotalSize.y();
      OpenGl_Vec4 aColorRange = aBackColorTop - aBackColorBot;
      aBackColorBot = aBackColorBot + aColorRange * ((float) aTileOffset / aViewSize);
      aBackColorTop = aBackColorBot + aColorRange * ((float) aTileSize / aViewSize);
    }
  }
  aBackColorTop = theGlContext->Vec4FromQuantityColor (aBackColorTop);
  aBackColorBot = theGlContext->Vec4FromQuantityColor (aBackColorBot);
  theProgram->SetUniform (theGlContext, myUniformLocations[theProgramId][OpenGl_RT_uBackColorTop], aBackColorTop);
  theProgram->SetUniform (theGlContext, myUniformLocations[theProgramId][OpenGl_RT_uBackColorBot], aBackColorBot);

  // Set environment map parameters
  const Handle(OpenGl_TextureSet)& anEnvTextureSet = myRaytraceParameters.CubemapForBack
                                                   ? myCubeMapParams->TextureSet (theGlContext)
                                                   : myTextureEnv;
  const bool toDisableEnvironmentMap = anEnvTextureSet.IsNull()
                                   ||  anEnvTextureSet->IsEmpty()
                                   || !anEnvTextureSet->First()->IsValid();
  theProgram->SetUniform (theGlContext, myUniformLocations[theProgramId][OpenGl_RT_uEnvMapEnabled],
                          toDisableEnvironmentMap ? 0 : 1);
  if (myRaytraceParameters.CubemapForBack)
  {
    theProgram->SetUniform (theGlContext, "uZCoeff", myCubeMapBackground->ZIsInverted() ? -1 :  1);
    theProgram->SetUniform (theGlContext, "uYCoeff", myCubeMapBackground->IsTopDown()   ?  1 : -1);
    theProgram->SetUniform (theGlContext, myUniformLocations[theProgramId][OpenGl_RT_uEnvMapForBack],
                            myBackgroundType == Graphic3d_TOB_CUBEMAP ? 1 : 0);
  }
  else
  {
    theProgram->SetUniform (theGlContext, myUniformLocations[theProgramId][OpenGl_RT_uEnvMapForBack],
                            myRenderParams.UseEnvironmentMapBackground ? 1 : 0);
  }

  // Set ambient light source
  theProgram->SetUniform (theGlContext,
                          myUniformLocations[theProgramId][OpenGl_RT_uLightAmbnt], myRaytraceGeometry.Ambient);
  if (myRenderParams.IsGlobalIlluminationEnabled) // GI parameters
  {
    theProgram->SetUniform (theGlContext,
      myUniformLocations[theProgramId][OpenGl_RT_uMaxRadiance], myRenderParams.RadianceClampingValue);

    theProgram->SetUniform (theGlContext,
      myUniformLocations[theProgramId][OpenGl_RT_uBlockedRngEnabled], myRenderParams.CoherentPathTracingMode ? 1 : 0);

    // Check whether we should restart accumulation for run-time parameters
    if (myRenderParams.RadianceClampingValue       != myRaytraceParameters.RadianceClampingValue
     || myRenderParams.UseEnvironmentMapBackground != myRaytraceParameters.UseEnvMapForBackground)
    {
      myAccumFrames = 0; // accumulation should be restarted

      myRaytraceParameters.RadianceClampingValue  = myRenderParams.RadianceClampingValue;
      myRaytraceParameters.UseEnvMapForBackground = myRenderParams.UseEnvironmentMapBackground;
    }
  }
  else // RT parameters
  {
    // Enable/disable run-time ray-tracing effects
    theProgram->SetUniform (theGlContext,
      myUniformLocations[theProgramId][OpenGl_RT_uShadowsEnabled], myRenderParams.IsShadowEnabled ?  1 : 0);
    theProgram->SetUniform (theGlContext,
      myUniformLocations[theProgramId][OpenGl_RT_uReflectEnabled], myRenderParams.IsReflectionEnabled ?  1 : 0);
  }

  return Standard_True;
}

// =======================================================================
// function : bindRaytraceTextures
// purpose  : Binds ray-trace textures to corresponding texture units
// =======================================================================
void OpenGl_View::bindRaytraceTextures (const Handle(OpenGl_Context)& theGlContext,
                                        int theStereoView)
{
  if (myRaytraceParameters.AdaptiveScreenSampling
   && myRaytraceParameters.GlobalIllumination)
  {
    theGlContext->core42->glBindImageTexture (OpenGl_RT_OutputImage,
                                              myRaytraceOutputTexture[theStereoView]->TextureId(), 0, GL_TRUE, 0, GL_READ_WRITE, GL_R32F);
    theGlContext->core42->glBindImageTexture (OpenGl_RT_VisualErrorImage,
                                              myRaytraceVisualErrorTexture[theStereoView]->TextureId(), 0, GL_TRUE, 0, GL_READ_WRITE, GL_R32I);
    if (myRaytraceParameters.AdaptiveScreenSamplingAtomic)
    {
      theGlContext->core42->glBindImageTexture (OpenGl_RT_TileOffsetsImage,
                                                myRaytraceTileOffsetsTexture[theStereoView]->TextureId(), 0, GL_TRUE, 0, GL_READ_ONLY, GL_RG32I);
    }
    else
    {
      theGlContext->core42->glBindImageTexture (OpenGl_RT_TileSamplesImage,
                                                myRaytraceTileSamplesTexture[theStereoView]->TextureId(), 0, GL_TRUE, 0, GL_READ_WRITE, GL_R32I);
    }
  }

  const Handle(OpenGl_TextureSet)& anEnvTextureSet = myRaytraceParameters.CubemapForBack
                                                   ? myCubeMapParams->TextureSet (theGlContext)
                                                   : myTextureEnv;
  if (!anEnvTextureSet.IsNull()
   && !anEnvTextureSet->IsEmpty()
   &&  anEnvTextureSet->First()->IsValid())
  {
    anEnvTextureSet->First()->Bind (theGlContext, OpenGl_RT_EnvMapTexture);
  }

  mySceneMinPointTexture   ->BindTexture (theGlContext, OpenGl_RT_SceneMinPointTexture);
  mySceneMaxPointTexture   ->BindTexture (theGlContext, OpenGl_RT_SceneMaxPointTexture);
  mySceneNodeInfoTexture   ->BindTexture (theGlContext, OpenGl_RT_SceneNodeInfoTexture);
  myGeometryVertexTexture  ->BindTexture (theGlContext, OpenGl_RT_GeometryVertexTexture);
  myGeometryNormalTexture  ->BindTexture (theGlContext, OpenGl_RT_GeometryNormalTexture);
  myGeometryTexCrdTexture  ->BindTexture (theGlContext, OpenGl_RT_GeometryTexCrdTexture);
  myGeometryTriangTexture  ->BindTexture (theGlContext, OpenGl_RT_GeometryTriangTexture);
  mySceneTransformTexture  ->BindTexture (theGlContext, OpenGl_RT_SceneTransformTexture);
  myRaytraceMaterialTexture->BindTexture (theGlContext, OpenGl_RT_RaytraceMaterialTexture);
  myRaytraceLightSrcTexture->BindTexture (theGlContext, OpenGl_RT_RaytraceLightSrcTexture);
}

// =======================================================================
// function : unbindRaytraceTextures
// purpose  : Unbinds ray-trace textures from corresponding texture units
// =======================================================================
void OpenGl_View::unbindRaytraceTextures (const Handle(OpenGl_Context)& theGlContext)
{
  mySceneMinPointTexture   ->UnbindTexture (theGlContext, OpenGl_RT_SceneMinPointTexture);
  mySceneMaxPointTexture   ->UnbindTexture (theGlContext, OpenGl_RT_SceneMaxPointTexture);
  mySceneNodeInfoTexture   ->UnbindTexture (theGlContext, OpenGl_RT_SceneNodeInfoTexture);
  myGeometryVertexTexture  ->UnbindTexture (theGlContext, OpenGl_RT_GeometryVertexTexture);
  myGeometryNormalTexture  ->UnbindTexture (theGlContext, OpenGl_RT_GeometryNormalTexture);
  myGeometryTexCrdTexture  ->UnbindTexture (theGlContext, OpenGl_RT_GeometryTexCrdTexture);
  myGeometryTriangTexture  ->UnbindTexture (theGlContext, OpenGl_RT_GeometryTriangTexture);
  mySceneTransformTexture  ->UnbindTexture (theGlContext, OpenGl_RT_SceneTransformTexture);
  myRaytraceMaterialTexture->UnbindTexture (theGlContext, OpenGl_RT_RaytraceMaterialTexture);
  myRaytraceLightSrcTexture->UnbindTexture (theGlContext, OpenGl_RT_RaytraceLightSrcTexture);

  theGlContext->core15fwd->glActiveTexture (GL_TEXTURE0);
}

// =======================================================================
// function : runRaytraceShaders
// purpose  : Runs ray-tracing shader programs
// =======================================================================
Standard_Boolean OpenGl_View::runRaytraceShaders (const Standard_Integer        theSizeX,
                                                  const Standard_Integer        theSizeY,
                                                  Graphic3d_Camera::Projection  theProjection,
                                                  OpenGl_FrameBuffer*           theReadDrawFbo,
                                                  const Handle(OpenGl_Context)& theGlContext)
{
  Standard_Boolean aResult = theGlContext->BindProgram (myRaytraceProgram);

  aResult &= setUniformState (0,
                              theSizeX,
                              theSizeY,
                              theProjection,
                              theGlContext);

  if (myRaytraceParameters.GlobalIllumination) // path tracing
  {
    aResult &= runPathtrace    (theSizeX, theSizeY, theProjection, theGlContext);
    aResult &= runPathtraceOut (theProjection, theReadDrawFbo, theGlContext);
  }
  else // Whitted-style ray-tracing
  {
    aResult &= runRaytrace (theSizeX, theSizeY, theProjection, theReadDrawFbo, theGlContext);
  }

  return aResult;
}

// =======================================================================
// function : runRaytrace
// purpose  : Runs Whitted-style ray-tracing
// =======================================================================
Standard_Boolean OpenGl_View::runRaytrace (const Standard_Integer        theSizeX,
                                           const Standard_Integer        theSizeY,
                                           Graphic3d_Camera::Projection  theProjection,
                                           OpenGl_FrameBuffer*           theReadDrawFbo,
                                           const Handle(OpenGl_Context)& theGlContext)
{
  Standard_Boolean aResult = Standard_True;

  // Choose proper set of frame buffers for stereo rendering
  const Standard_Integer aFBOIdx = (theProjection == Graphic3d_Camera::Projection_MonoRightEye) ? 1 : 0;
  bindRaytraceTextures (theGlContext, aFBOIdx);

  if (myRenderParams.IsAntialiasingEnabled) // if second FSAA pass is used
  {
    myRaytraceFBO1[aFBOIdx]->BindBuffer (theGlContext);

    theGlContext->core11fwd->glClear (GL_DEPTH_BUFFER_BIT); // render the image with depth
  }

  theGlContext->core20fwd->glDrawArrays (GL_TRIANGLES, 0, 6);

  if (myRenderParams.IsAntialiasingEnabled)
  {
    theGlContext->core11fwd->glDisable (GL_DEPTH_TEST); // improve jagged edges without depth buffer

    // bind ray-tracing output image as input
    myRaytraceFBO1[aFBOIdx]->ColorTexture()->Bind (theGlContext, OpenGl_RT_FsaaInputTexture);

    aResult &= theGlContext->BindProgram (myPostFSAAProgram);

    aResult &= setUniformState (1 /* FSAA ID */,
                                theSizeX,
                                theSizeY,
                                theProjection,
                                theGlContext);

    // Perform multi-pass adaptive FSAA using ping-pong technique.
    // We use 'FLIPTRI' sampling pattern changing for every pixel
    // (3 additional samples per pixel, the 1st sample is already
    // available from initial ray-traced image).
    for (Standard_Integer anIt = 1; anIt < 4; ++anIt)
    {
      OpenGl_Vec2 aFsaaOffset (1.f / theSizeX, 1.f / theSizeY);
      if (anIt == 1)
      {
        aFsaaOffset.x() *= -0.55f;
        aFsaaOffset.y() *=  0.55f;
      }
      else if (anIt == 2)
      {
        aFsaaOffset.x() *=  0.00f;
        aFsaaOffset.y() *= -0.55f;
      }
      else if (anIt == 3)
      {
        aFsaaOffset.x() *= 0.55f;
        aFsaaOffset.y() *= 0.00f;
      }

      aResult &= myPostFSAAProgram->SetUniform (theGlContext,
        myUniformLocations[1][OpenGl_RT_uSamples], anIt + 1);
      aResult &= myPostFSAAProgram->SetUniform (theGlContext,
        myUniformLocations[1][OpenGl_RT_uFsaaOffset], aFsaaOffset);

      Handle(OpenGl_FrameBuffer)& aFramebuffer = anIt % 2
                                               ? myRaytraceFBO2[aFBOIdx]
                                               : myRaytraceFBO1[aFBOIdx];

      aFramebuffer->BindBuffer (theGlContext);

      // perform adaptive FSAA pass
      theGlContext->core20fwd->glDrawArrays (GL_TRIANGLES, 0, 6);

      aFramebuffer->ColorTexture()->Bind (theGlContext, OpenGl_RT_FsaaInputTexture);
    }

    const Handle(OpenGl_FrameBuffer)& aRenderImageFramebuffer = myRaytraceFBO2[aFBOIdx];
    const Handle(OpenGl_FrameBuffer)& aDepthSourceFramebuffer = myRaytraceFBO1[aFBOIdx];

    theGlContext->core11fwd->glEnable (GL_DEPTH_TEST);

    // Display filtered image
    theGlContext->BindProgram (myOutImageProgram);

    if (theReadDrawFbo != NULL)
    {
      theReadDrawFbo->BindBuffer (theGlContext);
    }
    else
    {
      aRenderImageFramebuffer->UnbindBuffer (theGlContext);
    }

    aRenderImageFramebuffer->ColorTexture()       ->Bind (theGlContext, OpenGl_RT_PrevAccumTexture);
    aDepthSourceFramebuffer->DepthStencilTexture()->Bind (theGlContext, OpenGl_RT_RaytraceDepthTexture);

    // copy the output image with depth values
    theGlContext->core20fwd->glDrawArrays (GL_TRIANGLES, 0, 6);

    aDepthSourceFramebuffer->DepthStencilTexture()->Unbind (theGlContext, OpenGl_RT_RaytraceDepthTexture);
    aRenderImageFramebuffer->ColorTexture()       ->Unbind (theGlContext, OpenGl_RT_PrevAccumTexture);
  }

  unbindRaytraceTextures (theGlContext);

  theGlContext->BindProgram (NULL);

  return aResult;
}

// =======================================================================
// function : runPathtrace
// purpose  : Runs path tracing shader
// =======================================================================
Standard_Boolean OpenGl_View::runPathtrace (const Standard_Integer              theSizeX,
                                            const Standard_Integer              theSizeY,
                                            const Graphic3d_Camera::Projection  theProjection,
                                            const Handle(OpenGl_Context)&       theGlContext)
{
  if (myToUpdateEnvironmentMap) // check whether the map was changed
  {
    myAccumFrames = myToUpdateEnvironmentMap = 0;
  }
  
  if (myRenderParams.CameraApertureRadius != myPrevCameraApertureRadius
   || myRenderParams.CameraFocalPlaneDist != myPrevCameraFocalPlaneDist)
  {
    myPrevCameraApertureRadius = myRenderParams.CameraApertureRadius;
    myPrevCameraFocalPlaneDist = myRenderParams.CameraFocalPlaneDist;
    myAccumFrames = 0;
  }

  // Choose proper set of frame buffers for stereo rendering
  const Standard_Integer aFBOIdx = (theProjection == Graphic3d_Camera::Projection_MonoRightEye) ? 1 : 0;

  if (myRaytraceParameters.AdaptiveScreenSampling)
  {
    if (myAccumFrames == 0)
    {
      myTileSampler.Reset(); // reset tile sampler to its initial state

      // Adaptive sampling is starting at the second frame
      if (myRaytraceParameters.AdaptiveScreenSamplingAtomic)
      {
        myTileSampler.UploadOffsets (theGlContext, myRaytraceTileOffsetsTexture[aFBOIdx], false);
      }
      else
      {
        myTileSampler.UploadSamples (theGlContext, myRaytraceTileSamplesTexture[aFBOIdx], false);
      }

      theGlContext->core44->glClearTexImage (myRaytraceOutputTexture[aFBOIdx]->TextureId(), 0, GL_RED, GL_FLOAT, NULL);
    }

    // Clear adaptive screen sampling images
    theGlContext->core44->glClearTexImage (myRaytraceVisualErrorTexture[aFBOIdx]->TextureId(), 0, GL_RED_INTEGER, GL_INT, NULL);
  }

  bindRaytraceTextures (theGlContext, aFBOIdx);

  const Handle(OpenGl_FrameBuffer)& anAccumImageFramebuffer = myAccumFrames % 2 ? myRaytraceFBO2[aFBOIdx] : myRaytraceFBO1[aFBOIdx];
  anAccumImageFramebuffer->ColorTexture()->Bind (theGlContext, OpenGl_RT_PrevAccumTexture);

  // Set frame accumulation weight
  myRaytraceProgram->SetUniform (theGlContext, myUniformLocations[0][OpenGl_RT_uAccumSamples], myAccumFrames);

  // Set image uniforms for render program
  if (myRaytraceParameters.AdaptiveScreenSampling)
  {
    myRaytraceProgram->SetUniform (theGlContext, myUniformLocations[0][OpenGl_RT_uRenderImage], OpenGl_RT_OutputImage);
    myRaytraceProgram->SetUniform (theGlContext, myUniformLocations[0][OpenGl_RT_uTilesImage],  OpenGl_RT_TileSamplesImage);
    myRaytraceProgram->SetUniform (theGlContext, myUniformLocations[0][OpenGl_RT_uOffsetImage], OpenGl_RT_TileOffsetsImage);
    myRaytraceProgram->SetUniform (theGlContext, myUniformLocations[0][OpenGl_RT_uTileSize], myTileSampler.TileSize());
  }

  const Handle(OpenGl_FrameBuffer)& aRenderImageFramebuffer = myAccumFrames % 2 ? myRaytraceFBO1[aFBOIdx] : myRaytraceFBO2[aFBOIdx];
  aRenderImageFramebuffer->BindBuffer (theGlContext);
  if (myRaytraceParameters.AdaptiveScreenSampling
   && myRaytraceParameters.AdaptiveScreenSamplingAtomic)
  {
    // extend viewport here, so that tiles at boundaries (cut tile size by target rendering viewport)
    // redirected to inner tiles (full tile size) are drawn entirely
    const Graphic3d_Vec2i anOffsetViewport = myTileSampler.OffsetTilesViewport (myAccumFrames > 1); // shrunk offsets texture will be uploaded since 3rd frame
    theGlContext->core11fwd->glViewport (0, 0, anOffsetViewport.x(), anOffsetViewport.y());
  }
  const NCollection_Vec4<bool> aColorMask = theGlContext->ColorMaskRGBA();
  theGlContext->SetColorMaskRGBA (NCollection_Vec4<bool> (true)); // force writes into all components, including alpha

  // Generate for the given RNG seed
  theGlContext->core11fwd->glDisable (GL_DEPTH_TEST);

  // Adaptive Screen Sampling computes the same overall amount of samples per frame redraw as normal Path Tracing,
  // but distributes them unequally across pixels (grouped in tiles), so that some pixels do not receive new samples at all.
  //
  // Offsets map (redirecting currently rendered tile to another tile) allows performing Adaptive Screen Sampling in single pass,
  // but current implementation relies on atomic float operations (AdaptiveScreenSamplingAtomic) for this.
  // So that when atomic floats are not supported by GPU, multi-pass rendering is used instead.
  //
  // Single-pass rendering is more optimal due to smaller amount of draw calls,
  // memory synchronization barriers, discarding most of the fragments and bad parallelization in case of very small amount of tiles requiring more samples.
  // However, atomic operations on float values still produces different result (close, but not bit exact) making non-regression testing not robust.
  // It should be possible following single-pass rendering approach but using extra accumulation buffer and resolving pass as possible improvement.
  const int aNbPasses = myRaytraceParameters.AdaptiveScreenSampling
                    && !myRaytraceParameters.AdaptiveScreenSamplingAtomic
                      ? myTileSampler.MaxTileSamples()
                      : 1;
  if (myAccumFrames == 0)
  {
    myRNG.SetSeed(); // start RNG from beginning
  }
  for (int aPassIter = 0; aPassIter < aNbPasses; ++aPassIter)
  {
    myRaytraceProgram->SetUniform (theGlContext, myUniformLocations[0][OpenGl_RT_uFrameRndSeed], static_cast<Standard_Integer> (myRNG.NextInt() >> 2));
    theGlContext->core20fwd->glDrawArrays (GL_TRIANGLES, 0, 6);
    if (myRaytraceParameters.AdaptiveScreenSampling)
    {
      theGlContext->core44->glMemoryBarrier (GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
    }
  }
  aRenderImageFramebuffer->UnbindBuffer (theGlContext);

  theGlContext->SetColorMaskRGBA (aColorMask);
  if (myRaytraceParameters.AdaptiveScreenSampling
   && myRaytraceParameters.AdaptiveScreenSamplingAtomic)
  {
    theGlContext->core11fwd->glViewport (0, 0, theSizeX, theSizeY);
  }
  return true;
}

// =======================================================================
// function : runPathtraceOut
// purpose  :
// =======================================================================
Standard_Boolean OpenGl_View::runPathtraceOut (const Graphic3d_Camera::Projection  theProjection,
                                               OpenGl_FrameBuffer*                 theReadDrawFbo,
                                               const Handle(OpenGl_Context)&       theGlContext)
{
  // Output accumulated path traced image
  theGlContext->BindProgram (myOutImageProgram);

  // Choose proper set of frame buffers for stereo rendering
  const Standard_Integer aFBOIdx = (theProjection == Graphic3d_Camera::Projection_MonoRightEye) ? 1 : 0;

  if (myRaytraceParameters.AdaptiveScreenSampling)
  {
    // Set uniforms for display program
    myOutImageProgram->SetUniform (theGlContext, "uRenderImage",   OpenGl_RT_OutputImage);
    myOutImageProgram->SetUniform (theGlContext, "uAccumFrames",   myAccumFrames);
    myOutImageProgram->SetUniform (theGlContext, "uVarianceImage", OpenGl_RT_VisualErrorImage);
    myOutImageProgram->SetUniform (theGlContext, "uDebugAdaptive", myRenderParams.ShowSamplingTiles ?  1 : 0);
    myOutImageProgram->SetUniform (theGlContext, "uTileSize",      myTileSampler.TileSize());
    myOutImageProgram->SetUniform (theGlContext, "uVarianceScaleFactor", myTileSampler.VarianceScaleFactor());
  }

  if (myRaytraceParameters.GlobalIllumination)
  {
    myOutImageProgram->SetUniform(theGlContext, "uExposure", myRenderParams.Exposure);
    switch (myRaytraceParameters.ToneMappingMethod)
    {
      case Graphic3d_ToneMappingMethod_Disabled:
        break;
      case Graphic3d_ToneMappingMethod_Filmic:
        myOutImageProgram->SetUniform (theGlContext, "uWhitePoint", myRenderParams.WhitePoint);
        break;
    }
  }

  if (theReadDrawFbo != NULL)
  {
    theReadDrawFbo->BindBuffer (theGlContext);
  }

  const Handle(OpenGl_FrameBuffer)& aRenderImageFramebuffer = myAccumFrames % 2 ? myRaytraceFBO1[aFBOIdx] : myRaytraceFBO2[aFBOIdx];
  aRenderImageFramebuffer->ColorTexture()->Bind (theGlContext, OpenGl_RT_PrevAccumTexture);

  // Copy accumulated image with correct depth values
  theGlContext->core11fwd->glEnable (GL_DEPTH_TEST);
  theGlContext->core20fwd->glDrawArrays (GL_TRIANGLES, 0, 6);

  aRenderImageFramebuffer->ColorTexture()->Unbind (theGlContext, OpenGl_RT_PrevAccumTexture);

  if (myRaytraceParameters.AdaptiveScreenSampling)
  {
    // Download visual error map from the GPU and build adjusted tile offsets for optimal image sampling
    myTileSampler.GrabVarianceMap (theGlContext, myRaytraceVisualErrorTexture[aFBOIdx]);
    if (myRaytraceParameters.AdaptiveScreenSamplingAtomic)
    {
      myTileSampler.UploadOffsets (theGlContext, myRaytraceTileOffsetsTexture[aFBOIdx], myAccumFrames != 0);
    }
    else
    {
      myTileSampler.UploadSamples (theGlContext, myRaytraceTileSamplesTexture[aFBOIdx], myAccumFrames != 0);
    }
  }

  unbindRaytraceTextures (theGlContext);
  theGlContext->BindProgram (NULL);
  return true;
}

// =======================================================================
// function : raytrace
// purpose  : Redraws the window using OpenGL/GLSL ray-tracing
// =======================================================================
Standard_Boolean OpenGl_View::raytrace (const Standard_Integer        theSizeX,
                                        const Standard_Integer        theSizeY,
                                        Graphic3d_Camera::Projection  theProjection,
                                        OpenGl_FrameBuffer*           theReadDrawFbo,
                                        const Handle(OpenGl_Context)& theGlContext)
{
  if (!initRaytraceResources (theSizeX, theSizeY, theGlContext))
  {
    return Standard_False;
  }

  if (!updateRaytraceBuffers (theSizeX, theSizeY, theGlContext))
  {
    return Standard_False;
  }

  OpenGl_Mat4 aLightSourceMatrix;

  // Get inversed model-view matrix for transforming lights
  myCamera->OrientationMatrixF().Inverted (aLightSourceMatrix);

  if (!updateRaytraceLightSources (aLightSourceMatrix, theGlContext))
  {
    return Standard_False;
  }

  // Generate image using Whitted-style ray-tracing or path tracing
  if (myIsRaytraceDataValid)
  {
    myRaytraceScreenQuad.BindVertexAttrib (theGlContext, Graphic3d_TOA_POS);

    if (!myRaytraceGeometry.AcquireTextures (theGlContext))
    {
      theGlContext->PushMessage (GL_DEBUG_SOURCE_APPLICATION, GL_DEBUG_TYPE_ERROR,
        0, GL_DEBUG_SEVERITY_MEDIUM, "Error: Failed to acquire OpenGL image textures");
    }

    theGlContext->core11fwd->glDisable (GL_BLEND);

    const Standard_Boolean aResult = runRaytraceShaders (theSizeX,
                                                         theSizeY,
                                                         theProjection,
                                                         theReadDrawFbo,
                                                         theGlContext);

    if (!aResult)
    {
      theGlContext->PushMessage (GL_DEBUG_SOURCE_APPLICATION, GL_DEBUG_TYPE_ERROR,
        0, GL_DEBUG_SEVERITY_MEDIUM, "Error: Failed to execute ray-tracing shaders");
    }

    if (!myRaytraceGeometry.ReleaseTextures (theGlContext))
    {
      theGlContext->PushMessage (GL_DEBUG_SOURCE_APPLICATION, GL_DEBUG_TYPE_ERROR,
        0, GL_DEBUG_SEVERITY_MEDIUM, "Error: Failed to release OpenGL image textures");
    }

    myRaytraceScreenQuad.UnbindVertexAttrib (theGlContext, Graphic3d_TOA_POS);
  }

  return Standard_True;
}
