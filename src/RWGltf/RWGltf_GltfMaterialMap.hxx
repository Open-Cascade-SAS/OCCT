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

#ifndef _RWGltf_GltfMaterialMap_HeaderFile
#define _RWGltf_GltfMaterialMap_HeaderFile

#include <RWMesh_MaterialMap.hxx>
#include <RWGltf_GltfBufferView.hxx>

class RWGltf_GltfOStreamWriter;

//! Material manager for exporting into glTF format.
class RWGltf_GltfMaterialMap : public RWMesh_MaterialMap
{
  DEFINE_STANDARD_RTTIEXT(RWGltf_GltfMaterialMap, RWMesh_MaterialMap)
public:

  //! Main constructor.
  Standard_EXPORT RWGltf_GltfMaterialMap (const TCollection_AsciiString& theFile,
                                          const Standard_Integer theDefSamplerId);

  //! Destructor.
  Standard_EXPORT virtual ~RWGltf_GltfMaterialMap();

public:

  //! Add material images into GLB stream.
  //! @param theBinFile [in] [out] output file stream
  //! @param theStyle   [in] material images to add
  Standard_EXPORT void AddGlbImages (std::ostream& theBinFile,
                                     const XCAFPrs_Style& theStyle);

  //! Add bufferView's into RWGltf_GltfRootElement_BufferViews section with images collected by AddImagesToGlb().
  Standard_EXPORT void FlushGlbBufferViews (RWGltf_GltfOStreamWriter* theWriter,
                                            const Standard_Integer theBinDataBufferId,
                                            Standard_Integer& theBuffViewId);

  //! Write RWGltf_GltfRootElement_Images section with images collected by AddImagesToGlb().
  Standard_EXPORT void FlushGlbImages (RWGltf_GltfOStreamWriter* theWriter);

public:

  //! Add material images in case of non-GLB file
  //! (an alternative to AddImagesToGlb() + FlushBufferViews() + FlushImagesGlb()).
  Standard_EXPORT void AddImages (RWGltf_GltfOStreamWriter* theWriter,
                                  const XCAFPrs_Style& theStyle,
                                  Standard_Boolean& theIsStarted);

  //! Add material.
  Standard_EXPORT void AddMaterial (RWGltf_GltfOStreamWriter* theWriter,
                                    const XCAFPrs_Style& theStyle,
                                    Standard_Boolean& theIsStarted);
  //! Add material textures.
  Standard_EXPORT void AddTextures (RWGltf_GltfOStreamWriter* theWriter,
                                    const XCAFPrs_Style& theStyle,
                                    Standard_Boolean& theIsStarted);

  //! Return extent of images map.
  Standard_Integer NbImages() const { return myImageMap.Extent(); }

  //! Return extent of textures map.
  Standard_Integer NbTextures() const { return myTextureMap.Extent(); }

public:

  //! Return base color texture.
  Standard_EXPORT static const Handle(Image_Texture)& baseColorTexture (const Handle(XCAFDoc_VisMaterial)& theMat);

protected:

  //! Add texture image.
  Standard_EXPORT void addImage (RWGltf_GltfOStreamWriter* theWriter,
                                 const Handle(Image_Texture)& theTexture,
                                 Standard_Boolean& theIsStarted);

  //! Add texture image into GLB stream.
  //! @param theBinFile [in] [out] output file stream
  //! @param theTexture [in] texture image to add
  Standard_EXPORT void addGlbImage (std::ostream& theBinFile,
                                    const Handle(Image_Texture)& theTexture);

  //! Add texture.
  Standard_EXPORT void addTexture (RWGltf_GltfOStreamWriter* theWriter,
                                   const Handle(Image_Texture)& theTexture,
                                   Standard_Boolean& theIsStarted);

  //! Add material
  Standard_EXPORT virtual TCollection_AsciiString AddMaterial (const XCAFPrs_Style& theStyle) Standard_OVERRIDE;

  //! Virtual method actually defining the material (e.g. export to the file).
  Standard_EXPORT virtual void DefineMaterial (const XCAFPrs_Style& theStyle,
                                               const TCollection_AsciiString& theKey,
                                               const TCollection_AsciiString& theName) Standard_OVERRIDE;

protected:

  RWGltf_GltfOStreamWriter* myWriter;
  NCollection_IndexedDataMap<Handle(Image_Texture), RWGltf_GltfBufferView, Image_Texture> myImageMap;
  NCollection_Map<Handle(Image_Texture), Image_Texture> myTextureMap;

  Standard_Integer myDefSamplerId;

};

#endif // _RWGltf_GltfMaterialMap_HeaderFile
