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

#include <Message.hxx>
#include <NCollection_Array1.hxx>
#include <OSD_OpenFile.hxx>
#include <RWGltf_GltfRootElement.hxx>

#ifdef HAVE_RAPIDJSON
  #include <RWGltf_GltfOStreamWriter.hxx>
#endif

IMPLEMENT_STANDARD_RTTIEXT(RWGltf_GltfMaterialMap, RWMesh_MaterialMap)

// =======================================================================
// function : baseColorTexture
// purpose  :
// =======================================================================
const Handle(Image_Texture)& RWGltf_GltfMaterialMap::baseColorTexture(
  const Handle(XCAFDoc_VisMaterial)& theMat)
{
  static const Handle(Image_Texture) THE_NULL_TEXTURE;
  if (theMat.IsNull())
  {
    return THE_NULL_TEXTURE;
  }
  else if (theMat->HasPbrMaterial() && !theMat->PbrMaterial().BaseColorTexture.IsNull())
  {
    return theMat->PbrMaterial().BaseColorTexture;
  }
  else if (theMat->HasCommonMaterial() && !theMat->CommonMaterial().DiffuseTexture.IsNull())
  {
    return theMat->CommonMaterial().DiffuseTexture;
  }
  return THE_NULL_TEXTURE;
}

// =======================================================================
// function : RWGltf_GltfMaterialMap
// purpose  :
// =======================================================================
RWGltf_GltfMaterialMap::RWGltf_GltfMaterialMap(const TCollection_AsciiString& theFile,
                                               const Standard_Integer         theDefSamplerId)
    : RWMesh_MaterialMap(theFile),
      myWriter(NULL),
      myDefSamplerId(theDefSamplerId)
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
void RWGltf_GltfMaterialMap::AddImages(RWGltf_GltfOStreamWriter* theWriter,
                                       const XCAFPrs_Style&      theStyle,
                                       Standard_Boolean&         theIsStarted)
{
  if (theWriter == NULL || theStyle.Material().IsNull() || theStyle.Material()->IsEmpty())
  {
    return;
  }

  addImage(theWriter, baseColorTexture(theStyle.Material()), theIsStarted);
  addImage(theWriter, theStyle.Material()->PbrMaterial().MetallicRoughnessTexture, theIsStarted);
  addImage(theWriter, theStyle.Material()->PbrMaterial().NormalTexture, theIsStarted);
  addImage(theWriter, theStyle.Material()->PbrMaterial().EmissiveTexture, theIsStarted);
  addImage(theWriter, theStyle.Material()->PbrMaterial().OcclusionTexture, theIsStarted);
}

// =======================================================================
// function : AddGlbImages
// purpose  :
// =======================================================================
void RWGltf_GltfMaterialMap::AddGlbImages(std::ostream& theBinFile, const XCAFPrs_Style& theStyle)
{
  if (theStyle.Material().IsNull() || theStyle.Material()->IsEmpty())
  {
    return;
  }

  addGlbImage(theBinFile, baseColorTexture(theStyle.Material()));
  addGlbImage(theBinFile, theStyle.Material()->PbrMaterial().MetallicRoughnessTexture);
  addGlbImage(theBinFile, theStyle.Material()->PbrMaterial().NormalTexture);
  addGlbImage(theBinFile, theStyle.Material()->PbrMaterial().EmissiveTexture);
  addGlbImage(theBinFile, theStyle.Material()->PbrMaterial().OcclusionTexture);
}

// =======================================================================
// function : addImage
// purpose  :
// =======================================================================
void RWGltf_GltfMaterialMap::addImage(RWGltf_GltfOStreamWriter*    theWriter,
                                      const Handle(Image_Texture)& theTexture,
                                      Standard_Boolean&            theIsStarted)
{
#ifdef HAVE_RAPIDJSON
  if (theTexture.IsNull() || myImageMap.Contains(theTexture) || myImageFailMap.Contains(theTexture))
  {
    return;
  }

  const TCollection_AsciiString aGltfImgKey = myImageMap.Extent();
  TCollection_AsciiString       aTextureUri;
  if (!CopyTexture(aTextureUri, theTexture, aGltfImgKey))
  {
    myImageFailMap.Add(theTexture);
    return;
  }
  myImageMap.Add(theTexture, RWGltf_GltfBufferView());

  if (!theIsStarted)
  {
    theWriter->Key(RWGltf_GltfRootElementName(RWGltf_GltfRootElement_Images));
    theWriter->StartArray();
    theIsStarted = true;
  }

  theWriter->StartObject();
  {
    theWriter->Key("uri");
    theWriter->String(aTextureUri.ToCString());
  }
  theWriter->EndObject();
#else
  (void)theWriter;
  (void)theTexture;
  (void)theIsStarted;
#endif
}

// =======================================================================
// function : addGlbImage
// purpose  :
// =======================================================================
void RWGltf_GltfMaterialMap::addGlbImage(std::ostream&                theBinFile,
                                         const Handle(Image_Texture)& theTexture)
{
  if (theTexture.IsNull() || myImageMap.Contains(theTexture) || myImageFailMap.Contains(theTexture))
  {
    return;
  }

  RWGltf_GltfBufferView aBuffImage;
  aBuffImage.ByteOffset = theBinFile.tellp();
  if (!theTexture->WriteImage(theBinFile, myFileName))
  {
    myImageFailMap.Add(theTexture);
    return;
  }

  // alignment by 4 bytes
  int64_t aContentLen64 = (int64_t)theBinFile.tellp();
  while (aContentLen64 % 4 != 0)
  {
    theBinFile.write(" ", 1);
    ++aContentLen64;
  }

  // aBuffImage.Id = myBuffViewImages.Size(); // id will be corrected later
  aBuffImage.ByteLength = (int64_t)theBinFile.tellp() - aBuffImage.ByteOffset;
  if (aBuffImage.ByteLength <= 0)
  {
    myImageFailMap.Add(theTexture);
    return;
  }

  myImageMap.Add(theTexture, aBuffImage);
}

// =======================================================================
// function : FlushBufferViews
// purpose  :
// =======================================================================
void RWGltf_GltfMaterialMap::FlushGlbBufferViews(RWGltf_GltfOStreamWriter* theWriter,
                                                 const Standard_Integer    theBinDataBufferId,
                                                 Standard_Integer&         theBuffViewId)
{
#ifdef HAVE_RAPIDJSON
  for (NCollection_IndexedDataMap<Handle(Image_Texture), RWGltf_GltfBufferView>::Iterator
         aBufViewIter(myImageMap);
       aBufViewIter.More();
       aBufViewIter.Next())
  {
    RWGltf_GltfBufferView& aBuffView = aBufViewIter.ChangeValue();
    if (aBuffView.ByteLength <= 0)
    {
      continue;
    }

    aBuffView.Id = theBuffViewId++;
    theWriter->StartObject();
    theWriter->Key("buffer");
    theWriter->Int(theBinDataBufferId);
    theWriter->Key("byteLength");
    theWriter->Int64(aBuffView.ByteLength);
    theWriter->Key("byteOffset");
    theWriter->Int64(aBuffView.ByteOffset);
    theWriter->EndObject();
  }
#else
  (void)theWriter;
  (void)theBinDataBufferId;
  (void)theBuffViewId;
#endif
}

// =======================================================================
// function : FlushGlbImages
// purpose  :
// =======================================================================
void RWGltf_GltfMaterialMap::FlushGlbImages(RWGltf_GltfOStreamWriter* theWriter)
{
#ifdef HAVE_RAPIDJSON
  bool isStarted = false;
  for (NCollection_IndexedDataMap<Handle(Image_Texture), RWGltf_GltfBufferView>::Iterator
         aBufViewIter(myImageMap);
       aBufViewIter.More();
       aBufViewIter.Next())
  {
    const Handle(Image_Texture)& aTexture  = aBufViewIter.Key();
    const RWGltf_GltfBufferView& aBuffView = aBufViewIter.Value();
    if (aBuffView.ByteLength <= 0)
    {
      continue;
    }

    if (!isStarted)
    {
      theWriter->Key(RWGltf_GltfRootElementName(RWGltf_GltfRootElement_Images));
      theWriter->StartArray();
      isStarted = true;
    }

    theWriter->StartObject();
    {
      const TCollection_AsciiString anImageFormat = aTexture->MimeType();
      if (anImageFormat != "image/png" && anImageFormat != "image/jpeg")
      {
        Message::SendWarning(TCollection_AsciiString("Warning! Non-standard mime-type ")
                             + anImageFormat + " (texture " + aTexture->TextureId()
                             + ") within glTF file");
      }
      theWriter->Key("mimeType");
      theWriter->String(anImageFormat.ToCString());
      theWriter->Key("bufferView");
      theWriter->Int(aBuffView.Id);
    }
    theWriter->EndObject();
  }
  if (isStarted)
  {
    theWriter->EndArray();
  }
#else
  (void)theWriter;
#endif
}

// =======================================================================
// function : AddMaterial
// purpose  :
// =======================================================================
void RWGltf_GltfMaterialMap::AddMaterial(RWGltf_GltfOStreamWriter* theWriter,
                                         const XCAFPrs_Style&      theStyle,
                                         Standard_Boolean&         theIsStarted)
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
    theWriter->Key(RWGltf_GltfRootElementName(RWGltf_GltfRootElement_Materials));
    theWriter->StartArray();
    theIsStarted = true;
  }
  myWriter = theWriter;
  AddMaterial(theStyle);
  myWriter = NULL;
#else
  (void)theWriter;
  (void)theStyle;
  (void)theIsStarted;
#endif
}

// =======================================================================
// function : AddTextures
// purpose  :
// =======================================================================
void RWGltf_GltfMaterialMap::AddTextures(RWGltf_GltfOStreamWriter* theWriter,
                                         const XCAFPrs_Style&      theStyle,
                                         Standard_Boolean&         theIsStarted)
{
  if (theWriter == NULL || theStyle.Material().IsNull() || theStyle.Material()->IsEmpty())
  {
    return;
  }

  addTexture(theWriter, baseColorTexture(theStyle.Material()), theIsStarted);
  addTexture(theWriter, theStyle.Material()->PbrMaterial().MetallicRoughnessTexture, theIsStarted);
  addTexture(theWriter, theStyle.Material()->PbrMaterial().NormalTexture, theIsStarted);
  addTexture(theWriter, theStyle.Material()->PbrMaterial().EmissiveTexture, theIsStarted);
  addTexture(theWriter, theStyle.Material()->PbrMaterial().OcclusionTexture, theIsStarted);
}

// =======================================================================
// function : addTexture
// purpose  :
// =======================================================================
void RWGltf_GltfMaterialMap::addTexture(RWGltf_GltfOStreamWriter*    theWriter,
                                        const Handle(Image_Texture)& theTexture,
                                        Standard_Boolean&            theIsStarted)
{
#ifdef HAVE_RAPIDJSON
  if (theTexture.IsNull() || myTextureMap.Contains(theTexture) || !myImageMap.Contains(theTexture))
  {
    return;
  }

  // clang-format off
  const Standard_Integer anImgKey = myImageMap.FindIndex (theTexture) - 1; // glTF indexation starts from 0
  // clang-format on
  myTextureMap.Add(theTexture);

  if (!theIsStarted)
  {
    theWriter->Key(RWGltf_GltfRootElementName(RWGltf_GltfRootElement_Textures));
    theWriter->StartArray();
    theIsStarted = true;
  }

  theWriter->StartObject();
  {
    theWriter->Key("sampler");
    theWriter->Int(myDefSamplerId); // mandatory field by specs
    theWriter->Key("source");
    theWriter->Int(anImgKey);
  }
  theWriter->EndObject();
#else
  (void)theWriter;
  (void)theTexture;
  (void)theIsStarted;
#endif
}

// =======================================================================
// function : AddMaterial
// purpose  :
// =======================================================================
TCollection_AsciiString RWGltf_GltfMaterialMap::AddMaterial(const XCAFPrs_Style& theStyle)
{
  return RWMesh_MaterialMap::AddMaterial(theStyle);
}

// =======================================================================
// function : DefineMaterial
// purpose  :
// =======================================================================
void RWGltf_GltfMaterialMap::DefineMaterial(const XCAFPrs_Style& theStyle,
                                            const TCollection_AsciiString& /*theKey*/,
                                            const TCollection_AsciiString& theName)
{
#ifdef HAVE_RAPIDJSON
  if (myWriter == NULL)
  {
    Standard_ProgramError::Raise(
      "RWGltf_GltfMaterialMap::DefineMaterial() should be called with JSON Writer");
    return;
  }

  XCAFDoc_VisMaterialPBR aPbrMat;
  const bool hasMaterial = !theStyle.Material().IsNull() && !theStyle.Material()->IsEmpty();
  if (hasMaterial)
  {
    aPbrMat = theStyle.Material()->ConvertToPbrMaterial();
  }
  else if (!myDefaultStyle.Material().IsNull() && myDefaultStyle.Material()->HasPbrMaterial())
  {
    aPbrMat = myDefaultStyle.Material()->PbrMaterial();
  }
  if (theStyle.IsSetColorSurf())
  {
    aPbrMat.BaseColor.SetRGB(theStyle.GetColorSurf());
    if (theStyle.GetColorSurfRGBA().Alpha() < 1.0f)
    {
      aPbrMat.Metallic = 0.0f;
      aPbrMat.BaseColor.SetAlpha(theStyle.GetColorSurfRGBA().Alpha());
    }
  }
  myWriter->StartObject();
  {
    myWriter->Key("name");
    myWriter->String(theName.ToCString());

    myWriter->Key("pbrMetallicRoughness");
    myWriter->StartObject();
    {
      myWriter->Key("baseColorFactor");
      myWriter->StartArray();
      {
        myWriter->Double(aPbrMat.BaseColor.GetRGB().Red());
        myWriter->Double(aPbrMat.BaseColor.GetRGB().Green());
        myWriter->Double(aPbrMat.BaseColor.GetRGB().Blue());
        myWriter->Double(aPbrMat.BaseColor.Alpha());
      }
      myWriter->EndArray();

      if (const Handle(Image_Texture)& aBaseTexture = baseColorTexture(theStyle.Material()))
      {
        const Standard_Integer aBaseImageIdx = myImageMap.FindIndex(aBaseTexture) - 1;
        if (aBaseImageIdx != -1)
        {
          myWriter->Key("baseColorTexture");
          myWriter->StartObject();
          {
            myWriter->Key("index");
            myWriter->Int(aBaseImageIdx);
          }
          myWriter->EndObject();
        }
      }

      if (hasMaterial || aPbrMat.Metallic != 1.0f)
      {
        myWriter->Key("metallicFactor");
        myWriter->Double(aPbrMat.Metallic);
      }

      const Standard_Integer aMetRoughImageIdx =
        !aPbrMat.MetallicRoughnessTexture.IsNull()
          ? myImageMap.FindIndex(aPbrMat.MetallicRoughnessTexture) - 1
          : -1;
      if (aMetRoughImageIdx != -1)
      {
        myWriter->Key("metallicRoughnessTexture");
        myWriter->StartObject();
        {
          myWriter->Key("index");
          myWriter->Int(aMetRoughImageIdx);
        }
        myWriter->EndObject();
      }

      if (hasMaterial || aPbrMat.Roughness != 1.0f)
      {
        myWriter->Key("roughnessFactor");
        myWriter->Double(aPbrMat.Roughness);
      }
    }
    myWriter->EndObject();

    // export automatic culling as doubleSided material;
    // extra preprocessing is necessary to automatically export
    // Solids with singleSided material and Shells with doubleSided material,
    // as both may share the same material having "auto" flag
    if (theStyle.Material().IsNull()
        || theStyle.Material()->FaceCulling() == Graphic3d_TypeOfBackfacingModel_Auto
        || theStyle.Material()->FaceCulling() == Graphic3d_TypeOfBackfacingModel_DoubleSided
        || theStyle.Material()->FaceCulling()
             == Graphic3d_TypeOfBackfacingModel_FrontCulled) // front culling flag cannot be
                                                             // exported to glTF
    {
      myWriter->Key("doubleSided");
      myWriter->Bool(true);
    }

    const Graphic3d_AlphaMode anAlphaMode = !theStyle.Material().IsNull()
                                              ? theStyle.Material()->AlphaMode()
                                              : Graphic3d_AlphaMode_BlendAuto;
    switch (anAlphaMode)
    {
      case Graphic3d_AlphaMode_BlendAuto: {
        if (aPbrMat.BaseColor.Alpha() < 1.0f)
        {
          myWriter->Key("alphaMode");
          myWriter->String("BLEND");
        }
        break;
      }
      case Graphic3d_AlphaMode_Opaque: {
        break;
      }
      case Graphic3d_AlphaMode_Mask: {
        myWriter->Key("alphaMode");
        myWriter->String("MASK");
        break;
      }
      case Graphic3d_AlphaMode_Blend:
      case Graphic3d_AlphaMode_MaskBlend: {
        myWriter->Key("alphaMode");
        myWriter->String("BLEND");
        break;
      }
    }
    if (!theStyle.Material().IsNull() && theStyle.Material()->AlphaCutOff() != 0.5f)
    {
      myWriter->Key("alphaCutoff");
      myWriter->Double(theStyle.Material()->AlphaCutOff());
    }

    if (aPbrMat.EmissiveFactor != Graphic3d_Vec3(0.0f, 0.0f, 0.0f))
    {
      myWriter->Key("emissiveFactor");
      myWriter->StartArray();
      {
        myWriter->Double(aPbrMat.EmissiveFactor.r());
        myWriter->Double(aPbrMat.EmissiveFactor.g());
        myWriter->Double(aPbrMat.EmissiveFactor.b());
      }
      myWriter->EndArray();
    }

    const Standard_Integer anEmissImageIdx =
      !aPbrMat.EmissiveTexture.IsNull() ? myImageMap.FindIndex(aPbrMat.EmissiveTexture) - 1 : -1;
    if (anEmissImageIdx != -1)
    {
      myWriter->Key("emissiveTexture");
      myWriter->StartObject();
      {
        myWriter->Key("index");
        myWriter->Int(anEmissImageIdx);
      }
      myWriter->EndObject();
    }

    const Standard_Integer aNormImageIdx =
      !aPbrMat.NormalTexture.IsNull() ? myImageMap.FindIndex(aPbrMat.NormalTexture) - 1 : -1;
    if (aNormImageIdx != -1)
    {
      myWriter->Key("normalTexture");
      myWriter->StartObject();
      {
        myWriter->Key("index");
        myWriter->Int(aNormImageIdx);
      }
      myWriter->EndObject();
    }

    const Standard_Integer anOcclusImageIdx =
      !aPbrMat.OcclusionTexture.IsNull() ? myImageMap.FindIndex(aPbrMat.OcclusionTexture) - 1 : -1;
    if (anOcclusImageIdx != -1)
    {
      myWriter->Key("occlusionTexture");
      myWriter->StartObject();
      {
        myWriter->Key("index");
        myWriter->Int(anOcclusImageIdx);
      }
      myWriter->EndObject();
    }
  }
  myWriter->EndObject();
#else
  (void)theStyle;
  (void)theName;
#endif
}
