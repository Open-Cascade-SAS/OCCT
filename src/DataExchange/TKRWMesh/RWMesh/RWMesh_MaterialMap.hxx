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

#ifndef _RWMesh_MaterialMap_HeaderFile
#define _RWMesh_MaterialMap_HeaderFile

#include <NCollection_DoubleMap.hxx>
#include <NCollection_Map.hxx>
#include <XCAFPrs_Style.hxx>

//! Material manager.
//! Provides an interface for collecting all materials within the document before writing it into
//! file, and for copying associated image files (textures) into sub-folder near by exported model.
class RWMesh_MaterialMap : public Standard_Transient
{
  DEFINE_STANDARD_RTTIEXT(RWMesh_MaterialMap, Standard_Transient)
public:
  //! Main constructor.
  Standard_EXPORT RWMesh_MaterialMap(const TCollection_AsciiString& theFile);

  //! Destructor.
  Standard_EXPORT virtual ~RWMesh_MaterialMap();

  //! Return default material definition to be used for nodes with only color defined.
  const XCAFPrs_Style& DefaultStyle() const { return myDefaultStyle; }

  //! Set default material definition to be used for nodes with only color defined.
  void SetDefaultStyle(const XCAFPrs_Style& theStyle) { myDefaultStyle = theStyle; }

  //! Find already registered material
  TCollection_AsciiString FindMaterial(const XCAFPrs_Style& theStyle) const
  {
    if (myStyles.IsBound1(theStyle))
    {
      return myStyles.Find1(theStyle);
    }
    return TCollection_AsciiString();
  }

  //! Register material and return its name identifier.
  Standard_EXPORT virtual TCollection_AsciiString AddMaterial(const XCAFPrs_Style& theStyle);

  //! Create texture folder "modelName/textures"; for example:
  //! MODEL:  Path/ModelName.gltf
  //! IMAGES: Path/ModelName/textures/
  //! Warning! Output folder is NOT cleared.
  Standard_EXPORT virtual bool CreateTextureFolder();

  //! Copy and rename texture file to the new location.
  //! @param[out] theResTexture  result texture file path (relative to the model)
  //! @param[in] theTexture  original texture
  //! @param[in] theKey  material key
  Standard_EXPORT virtual bool CopyTexture(TCollection_AsciiString&       theResTexture,
                                           const Handle(Image_Texture)&   theTexture,
                                           const TCollection_AsciiString& theKey);

  //! Virtual method actually defining the material (e.g. export to the file).
  virtual void DefineMaterial(const XCAFPrs_Style&           theStyle,
                              const TCollection_AsciiString& theKey,
                              const TCollection_AsciiString& theName) = 0;

  //! Return failed flag.
  bool IsFailed() const { return myIsFailed; }

protected:
  //! Copy file to another place.
  Standard_EXPORT static bool copyFileTo(const TCollection_AsciiString& theFileSrc,
                                         const TCollection_AsciiString& theFileDst);

protected:
  TCollection_AsciiString myFolder;            //!< output folder for glTF file
  TCollection_AsciiString myTexFolder;         //!< output folder for images (full  path)
  TCollection_AsciiString myTexFolderShort;    //!< output folder for images (short path)
  TCollection_AsciiString myFileName;          //!< output glTF file path
  TCollection_AsciiString myShortFileNameBase; //!< output glTF file name without extension
  TCollection_AsciiString myKeyPrefix;         //!< prefix for generated keys
  NCollection_DoubleMap<XCAFPrs_Style, TCollection_AsciiString>
                                         myStyles;       //!< map of processed styles
  NCollection_Map<Handle(Image_Texture)> myImageFailMap; //!< map of images failed to be copied
                                                         // clang-format off
  XCAFPrs_Style           myDefaultStyle;      //!< default material definition to be used for nodes with only color defined
                                                         // clang-format on
  Standard_Integer myNbMaterials;                        //!< number of registered materials
  Standard_Boolean myIsFailed;                           //!< flag indicating failure
  Standard_Boolean myMatNameAsKey; //!< flag indicating usage of material name as key
};

#endif // _RWMesh_MaterialMap_HeaderFile
