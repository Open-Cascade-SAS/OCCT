// Copyright (c) 2017-2019 OPEN CASCADE SAS
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

#include <RWGltf_GltfMaterialMap.hxx>

#include <RWGltf_GltfRootElement.hxx>

#ifdef HAVE_RAPIDJSON
  #include <RWGltf_GltfOStreamWriter.hxx>
#endif

// =======================================================================
// function : baseColorTexture
// purpose  :
// =======================================================================
const Handle(Image_Texture)& RWGltf_GltfMaterialMap::baseColorTexture (const Handle(XCAFDoc_VisMaterial)& theMat)
{
  static const Handle(Image_Texture) THE_NULL_TEXTURE;
  if (theMat.IsNull())
  {
    return THE_NULL_TEXTURE;
  }
  else if (theMat->HasPbrMaterial()
       && !theMat->PbrMaterial().BaseColorTexture.IsNull())
  {
    return theMat->PbrMaterial().BaseColorTexture;
  }
  else if (theMat->HasCommonMaterial()
       && !theMat->CommonMaterial().DiffuseTexture.IsNull())
  {
    return theMat->CommonMaterial().DiffuseTexture;
  }
  return THE_NULL_TEXTURE;
}

// =======================================================================
// function : RWGltf_GltfMaterialMap
// purpose  :
// =======================================================================
RWGltf_GltfMaterialMap::RWGltf_GltfMaterialMap (const TCollection_AsciiString& theFile,
                                                const Standard_Integer theDefSamplerId)
: RWMesh_MaterialMap (theFile),
  myWriter (NULL),
  myDefSamplerId (theDefSamplerId),
  myNbImages (0)
{
  myMatNameAsKey = false;
}

// =======================================================================
// function : ~RWGltf_GltfMaterialMap
// purpose  :
// =======================================================================
RWGltf_GltfMaterialMap::~RWGltf_GltfMaterialMap()
{
  //
}

// =======================================================================
// function : AddImages
// purpose  :
// =======================================================================
void RWGltf_GltfMaterialMap::AddImages (RWGltf_GltfOStreamWriter* theWriter,
                                        const XCAFPrs_Style& theStyle,
                                        Standard_Boolean& theIsStarted)
{
  if (theWriter == NULL
   || theStyle.Material().IsNull()
   || theStyle.Material()->IsEmpty())
  {
    return;
  }

  addImage (theWriter, baseColorTexture (theStyle.Material()), theIsStarted);
  addImage (theWriter, theStyle.Material()->PbrMaterial().MetallicRoughnessTexture, theIsStarted);
  addImage (theWriter, theStyle.Material()->PbrMaterial().NormalTexture,    theIsStarted);
  addImage (theWriter, theStyle.Material()->PbrMaterial().EmissiveTexture,  theIsStarted);
  addImage (theWriter, theStyle.Material()->PbrMaterial().OcclusionTexture, theIsStarted);
}

// =======================================================================
// function : addImage
// purpose  :
// =======================================================================
void RWGltf_GltfMaterialMap::addImage (RWGltf_GltfOStreamWriter* theWriter,
                                       const Handle(Image_Texture)& theTexture,
                                       Standard_Boolean& theIsStarted)
{
#ifdef HAVE_RAPIDJSON
  if (theTexture.IsNull()
   || myImageMap.IsBound1 (theTexture)
   || myImageFailMap.Contains (theTexture))
  {
    return;
  }

  TCollection_AsciiString aGltfImgKey = myNbImages;
  ++myNbImages;
  for (; myImageMap.IsBound2 (aGltfImgKey); ++myNbImages)
  {
    aGltfImgKey = myNbImages;
  }

  TCollection_AsciiString aTextureUri;
  if (!CopyTexture (aTextureUri, theTexture, aGltfImgKey))
  {
    myImageFailMap.Add (theTexture);
    return;
  }

  myImageMap.Bind (theTexture, aGltfImgKey);

  if (!theIsStarted)
  {
    theWriter->Key (RWGltf_GltfRootElementName (RWGltf_GltfRootElement_Images));
    theWriter->StartArray();
    theIsStarted = true;
  }

  theWriter->StartObject();
  {
    theWriter->Key ("uri");
    theWriter->String (aTextureUri.ToCString());
  }
  theWriter->EndObject();
#else
  (void )theWriter;
  (void )theTexture;
  (void )theIsStarted;
#endif
}

// =======================================================================
// function : AddMaterial
// purpose  :
// =======================================================================
void RWGltf_GltfMaterialMap::AddMaterial (RWGltf_GltfOStreamWriter* theWriter,
                                          const XCAFPrs_Style& theStyle,
                                          Standard_Boolean& theIsStarted)
{
#ifdef HAVE_RAPIDJSON
  if (theWriter == NULL
   || ((theStyle.Material().IsNull() || theStyle.Material()->IsEmpty())
    && !theStyle.IsSetColorSurf()))
  {
    return;
  }

  if (!theIsStarted)
  {
    theWriter->Key (RWGltf_GltfRootElementName (RWGltf_GltfRootElement_Materials));
    theWriter->StartArray();
    theIsStarted = true;
  }
  myWriter = theWriter;
  AddMaterial (theStyle);
  myWriter = NULL;
#else
  (void )theWriter;
  (void )theStyle;
  (void )theIsStarted;
#endif
}

// =======================================================================
// function : AddTextures
// purpose  :
// =======================================================================
void RWGltf_GltfMaterialMap::AddTextures (RWGltf_GltfOStreamWriter* theWriter,
                                          const XCAFPrs_Style& theStyle,
                                          Standard_Boolean& theIsStarted)
{
  if (theWriter == NULL
   || theStyle.Material().IsNull()
   || theStyle.Material()->IsEmpty())
  {
    return;
  }

  addTexture (theWriter, baseColorTexture (theStyle.Material()), theIsStarted);
  addTexture (theWriter, theStyle.Material()->PbrMaterial().MetallicRoughnessTexture, theIsStarted);
  addTexture (theWriter, theStyle.Material()->PbrMaterial().NormalTexture,    theIsStarted);
  addTexture (theWriter, theStyle.Material()->PbrMaterial().EmissiveTexture,  theIsStarted);
  addTexture (theWriter, theStyle.Material()->PbrMaterial().OcclusionTexture, theIsStarted);
}

// =======================================================================
// function : addTexture
// purpose  :
// =======================================================================
void RWGltf_GltfMaterialMap::addTexture (RWGltf_GltfOStreamWriter* theWriter,
                                         const Handle(Image_Texture)& theTexture,
                                         Standard_Boolean& theIsStarted)
{
#ifdef HAVE_RAPIDJSON
  if (theTexture.IsNull()
  ||  myTextureMap.Contains (theTexture)
  || !myImageMap  .IsBound1 (theTexture))
  {
    return;
  }

  const TCollection_AsciiString anImgKey = myImageMap.Find1 (theTexture);
  myTextureMap.Add (theTexture);
  if (anImgKey.IsEmpty())
  {
    return;
  }

  if (!theIsStarted)
  {
    theWriter->Key (RWGltf_GltfRootElementName (RWGltf_GltfRootElement_Textures));
    theWriter->StartArray();
    theIsStarted = true;
  }

  theWriter->StartObject();
  {
    theWriter->Key ("sampler");
    theWriter->Int (myDefSamplerId); // mandatory field by specs
    theWriter->Key ("source");
    theWriter->Int (anImgKey.IntegerValue());
  }
  theWriter->EndObject();
#else
  (void )theWriter;
  (void )theTexture;
  (void )theIsStarted;
#endif
}

// =======================================================================
// function : AddMaterial
// purpose  :
// =======================================================================
TCollection_AsciiString RWGltf_GltfMaterialMap::AddMaterial (const XCAFPrs_Style& theStyle)
{
  return RWMesh_MaterialMap::AddMaterial (theStyle);
}

// =======================================================================
// function : DefineMaterial
// purpose  :
// =======================================================================
void RWGltf_GltfMaterialMap::DefineMaterial (const XCAFPrs_Style& theStyle,
                                             const TCollection_AsciiString& /*theKey*/,
                                             const TCollection_AsciiString& theName)
{
#ifdef HAVE_RAPIDJSON
  if (myWriter == NULL)
  {
    Standard_ProgramError::Raise ("RWGltf_GltfMaterialMap::DefineMaterial() should be called with JSON Writer");
    return;
  }

  XCAFDoc_VisMaterialPBR aPbrMat;
  const bool hasMaterial = !theStyle.Material().IsNull()
                        && !theStyle.Material()->IsEmpty();
  if (hasMaterial)
  {
    aPbrMat = theStyle.Material()->ConvertToPbrMaterial();
  }
  else if (!myDefaultStyle.Material().IsNull()
         && myDefaultStyle.Material()->HasPbrMaterial())
  {
    aPbrMat = myDefaultStyle.Material()->PbrMaterial();
  }
  if (theStyle.IsSetColorSurf())
  {
    aPbrMat.BaseColor.SetRGB (theStyle.GetColorSurf());
    if (theStyle.GetColorSurfRGBA().Alpha() < 1.0f)
    {
      aPbrMat.BaseColor.SetAlpha (theStyle.GetColorSurfRGBA().Alpha());
    }
  }
  myWriter->StartObject();
  {
    myWriter->Key ("name");
    myWriter->String (theName.ToCString());

    myWriter->Key ("pbrMetallicRoughness");
    myWriter->StartObject();
    {
      myWriter->Key ("baseColorFactor");
      myWriter->StartArray();
      {
        myWriter->Double (aPbrMat.BaseColor.GetRGB().Red());
        myWriter->Double (aPbrMat.BaseColor.GetRGB().Green());
        myWriter->Double (aPbrMat.BaseColor.GetRGB().Blue());
        myWriter->Double (aPbrMat.BaseColor.Alpha());
      }
      myWriter->EndArray();

      if (const Handle(Image_Texture)& aBaseTexture = baseColorTexture (theStyle.Material()))
      {
        if (myImageMap.IsBound1 (aBaseTexture))
        {
          myWriter->Key ("baseColorTexture");
          myWriter->StartObject();
          {
            myWriter->Key ("index");
            const TCollection_AsciiString& anImageIdx = myImageMap.Find1 (aBaseTexture);
            if (!anImageIdx.IsEmpty())
            {
              myWriter->Int (anImageIdx.IntegerValue());
            }
          }
          myWriter->EndObject();
        }
      }

      if (hasMaterial
       || aPbrMat.Metallic != 1.0f)
      {
        myWriter->Key ("metallicFactor");
        myWriter->Double (aPbrMat.Metallic);
      }

      if (!aPbrMat.MetallicRoughnessTexture.IsNull()
        && myImageMap.IsBound1 (aPbrMat.MetallicRoughnessTexture))
      {
        myWriter->Key ("metallicRoughnessTexture");
        myWriter->StartObject();
        {
          myWriter->Key ("index");
          const TCollection_AsciiString& anImageIdx = myImageMap.Find1 (aPbrMat.MetallicRoughnessTexture);
          if (!anImageIdx.IsEmpty())
          {
            myWriter->Int (anImageIdx.IntegerValue());
          }
        }
        myWriter->EndObject();
      }

      if (hasMaterial
       || aPbrMat.Roughness != 1.0f)
      {
        myWriter->Key ("roughnessFactor");
        myWriter->Double (aPbrMat.Roughness);
      }
    }
    myWriter->EndObject();

    if (theStyle.Material().IsNull()
     || theStyle.Material()->IsDoubleSided())
    {
      myWriter->Key ("doubleSided");
      myWriter->Bool (true);
    }

    const Graphic3d_AlphaMode anAlphaMode = !theStyle.Material().IsNull() ? theStyle.Material()->AlphaMode() : Graphic3d_AlphaMode_BlendAuto;
    switch (anAlphaMode)
    {
      case Graphic3d_AlphaMode_BlendAuto:
      {
        if (aPbrMat.BaseColor.Alpha() < 1.0f)
        {
          myWriter->Key ("alphaMode");
          myWriter->String ("BLEND");
        }
        break;
      }
      case Graphic3d_AlphaMode_Opaque:
      {
        break;
      }
      case Graphic3d_AlphaMode_Mask:
      {
        myWriter->Key ("alphaMode");
        myWriter->String ("MASK");
        break;
      }
      case Graphic3d_AlphaMode_Blend:
      {
        myWriter->Key ("alphaMode");
        myWriter->String ("BLEND");
        break;
      }
    }
    if (!theStyle.Material().IsNull()
      && theStyle.Material()->AlphaCutOff() != 0.5f)
    {
      myWriter->Key ("alphaCutoff");
      myWriter->Double (theStyle.Material()->AlphaCutOff());
    }

    if (aPbrMat.EmissiveFactor != Graphic3d_Vec3 (0.0f, 0.0f, 0.0f))
    {
      myWriter->Key ("emissiveFactor");
      myWriter->StartArray();
      {
        myWriter->Double (aPbrMat.EmissiveFactor.r());
        myWriter->Double (aPbrMat.EmissiveFactor.g());
        myWriter->Double (aPbrMat.EmissiveFactor.b());
      }
      myWriter->EndArray();
    }
    if (!aPbrMat.EmissiveTexture.IsNull()
      && myImageMap.IsBound1 (aPbrMat.EmissiveTexture))
    {
      myWriter->Key ("emissiveTexture");
      myWriter->StartObject();
      {
        myWriter->Key ("index");
        const TCollection_AsciiString& anImageIdx = myImageMap.Find1 (aPbrMat.EmissiveTexture);
        if (!anImageIdx.IsEmpty())
        {
          myWriter->Int (anImageIdx.IntegerValue());
        }
      }
      myWriter->EndObject();
    }

    if (!aPbrMat.NormalTexture.IsNull()
      && myImageMap.IsBound1 (aPbrMat.NormalTexture))
    {
      myWriter->Key ("normalTexture");
      myWriter->StartObject();
      {
        myWriter->Key ("index");
        const TCollection_AsciiString& anImageIdx = myImageMap.Find1 (aPbrMat.NormalTexture);
        if (!anImageIdx.IsEmpty())
        {
          myWriter->Int (anImageIdx.IntegerValue());
        }
      }
      myWriter->EndObject();
    }

    if (!aPbrMat.OcclusionTexture.IsNull()
      && myImageMap.IsBound1 (aPbrMat.OcclusionTexture))
    {
      myWriter->Key ("occlusionTexture");
      myWriter->StartObject();
      {
        myWriter->Key ("index");
        const TCollection_AsciiString& anImageIdx = myImageMap.Find1 (aPbrMat.OcclusionTexture);
        if (!anImageIdx.IsEmpty())
        {
          myWriter->Int (anImageIdx.IntegerValue());
        }
      }
      myWriter->EndObject();
    }
  }
  myWriter->EndObject();
#else
  (void )theStyle;
  (void )theName;
#endif
}
