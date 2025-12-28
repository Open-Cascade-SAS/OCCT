// Created on: 1997-07-28
// Created by: Pierre CHALAMET
// Copyright (c) 1997-1999 Matra Datavision
// Copyright (c) 1999-2014 OPEN CASCADE SAS
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

#ifndef _Graphic3d_TextureRoot_HeaderFile
#define _Graphic3d_TextureRoot_HeaderFile

#include <Image_PixMap.hxx>
#include <OSD_Path.hxx>
#include <Graphic3d_TypeOfTexture.hxx>
#include <Standard.hxx>
#include <Standard_Transient.hxx>
#include <Standard_Type.hxx>
#include <TCollection_AsciiString.hxx>

class Image_CompressedPixMap;
class Image_SupportedFormats;
class Graphic3d_TextureParams;

//! This is the texture root class enable the dialog with the GraphicDriver allows the loading of
//! texture.
class Graphic3d_TextureRoot : public Standard_Transient
{
  DEFINE_STANDARD_RTTIEXT(Graphic3d_TextureRoot, Standard_Transient)
public:
  //! The path to textures determined from CSF_MDTVTexturesDirectory or CASROOT environment
  //! variables.
  //! @return the root folder with default textures.
  Standard_EXPORT static TCollection_AsciiString TexturesFolder();

public:
  //! Destructor.
  Standard_EXPORT ~Graphic3d_TextureRoot();

  //! Checks if a texture class is valid or not.
  //! @return true if the construction of the class is correct
  Standard_EXPORT virtual bool IsDone() const;

  //! Returns the full path of the defined texture.
  //! It could be empty path if GetImage() is overridden to load image not from file.
  const OSD_Path& Path() const { return myPath; }

  //! @return the texture type.
  Graphic3d_TypeOfTexture Type() const { return myType; }

  //! This ID will be used to manage resource in graphic driver.
  //!
  //! Default implementation generates unique ID within constructor;
  //! inheritors may re-initialize it within their constructor,
  //! but should never modify it afterwards.
  //!
  //! Multiple Graphic3d_TextureRoot instances with same ID
  //! will be treated as single texture with different parameters
  //! to optimize memory usage though this will be more natural
  //! to use same instance of Graphic3d_TextureRoot when possible.
  //!
  //! If this ID is set to empty string by inheritor,
  //! then independent graphical resource will be created
  //! for each instance of Graphic3d_AspectFillArea3d where texture will be used.
  //!
  //! @return texture identifier.
  const TCollection_AsciiString& GetId() const { return myTexId; }

  //! Return image revision.
  size_t Revision() const { return myRevision; }

  //! Update image revision.
  //! Can be used for signaling changes in the texture source (e.g. file update, pixmap update)
  //! without re-creating texture source itself (since unique id should be never modified).
  void UpdateRevision() { ++myRevision; }

  //! This method will be called by graphic driver each time when texture resource should be
  //! created. It is called in front of GetImage() for uploading compressed image formats natively
  //! supported by GPU.
  //! @param[in] theSupported  the list of supported compressed texture formats;
  //!                          returning image in unsupported format will result in texture upload
  //!                          failure
  //! @return compressed pixmap or NULL if image is not in supported compressed format
  Standard_EXPORT virtual occ::handle<Image_CompressedPixMap> GetCompressedImage(
    const occ::handle<Image_SupportedFormats>& theSupported);

  //! This method will be called by graphic driver each time when texture resource should be
  //! created. Default constructors allow defining the texture source as path to texture image or
  //! directly as pixmap. If the source is defined as path, then the image will be dynamically
  //! loaded when this method is called (and no copy will be preserved in this class instance).
  //! Inheritors may dynamically generate the image.
  //! Notice, image data should be in Bottom-Up order (see Image_PixMap::IsTopDown())!
  //! @return the image for texture.
  Standard_EXPORT virtual occ::handle<Image_PixMap> GetImage(
    const occ::handle<Image_SupportedFormats>& theSupported);

  //! @return low-level texture parameters
  const occ::handle<Graphic3d_TextureParams>& GetParams() const { return myParams; }

  //! Return flag indicating color nature of values within the texture; TRUE by default.
  //!
  //! This flag will be used to interpret 8-bit per channel RGB(A) images as sRGB(A) textures
  //! with implicit linearizion of color components.
  //! Has no effect on images with floating point values (always considered linearized).
  //!
  //! When set to FALSE, such images will be interpreted as textures will be linear component
  //! values, which is useful for RGB(A) textures defining non-color properties (like
  //! Normalmap/Metalness/Roughness).
  bool IsColorMap() const { return myIsColorMap; }

  //! Set flag indicating color nature of values within the texture.
  void SetColorMap(bool theIsColor) { myIsColorMap = theIsColor; }

  //! Returns whether mipmaps should be generated or not.
  bool HasMipmaps() const { return myHasMipmaps; }

  //! Sets whether to generate mipmaps or not.
  void SetMipmapsGeneration(bool theToGenerateMipmaps) { myHasMipmaps = theToGenerateMipmaps; }

  //! Returns whether row's memory layout is top-down.
  bool IsTopDown() const { return myIsTopDown; }

protected:
  //! Creates a texture from a file
  //! Warning: Note that if <FileName> is NULL the texture must be realized
  //! using LoadTexture(image) method.
  Standard_EXPORT Graphic3d_TextureRoot(const TCollection_AsciiString& theFileName,
                                        const Graphic3d_TypeOfTexture  theType);

  //! Creates a texture from pixmap.
  //! Please note that the implementation expects the image data
  //! to be in Bottom-Up order (see Image_PixMap::IsTopDown()).
  Standard_EXPORT Graphic3d_TextureRoot(const occ::handle<Image_PixMap>& thePixmap,
                                        const Graphic3d_TypeOfTexture    theType);

  //! Unconditionally generate new texture id. Should be called only within constructor.
  Standard_EXPORT void generateId();

  //! Try converting image to compatible format.
  Standard_EXPORT static void convertToCompatible(
    const occ::handle<Image_SupportedFormats>& theSupported,
    const occ::handle<Image_PixMap>&           theImage);

  //! Method for supporting old API; another GetImage() method should be implemented instead.
  virtual occ::handle<Image_PixMap> GetImage() const { return occ::handle<Image_PixMap>(); }

protected:
  occ::handle<Graphic3d_TextureParams> myParams; //!< associated texture parameters
  // clang-format off
  TCollection_AsciiString         myTexId;      //!< unique identifier of this resource (for sharing graphic resource); should never be modified outside constructor
  occ::handle<Image_PixMap>            myPixMap;     //!< image pixmap - as one of the ways for defining the texture source
  OSD_Path                        myPath;       //!< image file path - as one of the ways for defining the texture source
  size_t                   myRevision;   //!< image revision - for signaling changes in the texture source (e.g. file update, pixmap update)
  Graphic3d_TypeOfTexture         myType;       //!< texture type
  bool                myIsColorMap; //!< flag indicating color nature of values within the texture
  bool                myIsTopDown;  //!< Stores rows's memory layout
  bool                myHasMipmaps; //!< Indicates whether mipmaps should be generated or not
  // clang-format on
};

#endif // _Graphic3d_TextureRoot_HeaderFile
