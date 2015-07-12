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

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <TCollection_AsciiString.hxx>
#include <Image_PixMap_Handle.hxx>
#include <OSD_Path.hxx>
#include <Graphic3d_TypeOfTexture.hxx>
#include <MMgt_TShared.hxx>
#include <Standard_Boolean.hxx>
class Graphic3d_TextureParams;
class TCollection_AsciiString;
class OSD_Path;


class Graphic3d_TextureRoot;
DEFINE_STANDARD_HANDLE(Graphic3d_TextureRoot, MMgt_TShared)

//! This is the texture root class enable the dialog with the GraphicDriver allows the loading of texture.
class Graphic3d_TextureRoot : public MMgt_TShared
{

public:

  
  Standard_EXPORT void Destroy() const;
~Graphic3d_TextureRoot()
{
  Destroy();
}
  
  //! Checks if a texture class is valid or not.
  //! @return true if the construction of the class is correct
  Standard_EXPORT virtual Standard_Boolean IsDone() const;
  

  //! Returns the full path of the defined texture.
  //! It could be empty path if GetImage() is overridden to load image not from file.
  Standard_EXPORT const OSD_Path& Path() const;
  
  //! @return the texture type.
  Standard_EXPORT Graphic3d_TypeOfTexture Type() const;
  

  //! This ID will be used to manage resource in graphic driver.
  //!
  //! Default implementation generates unique ID although inheritors may re-initialize it.
  //!
  //! Multiple Graphic3d_TextureRoot instancies with same ID
  //! will be treated as single texture with different parameters
  //! to optimize memory usage though this will be more natural
  //! to use same instance of Graphic3d_TextureRoot when possible.
  //!
  //! Notice that inheritor may set this ID to empty string.
  //! In this case independent graphical resource will be created
  //! for each instance of Graphic3d_AspectFillArea3d where texture will be used.
  //!
  //! @return texture identifier.
  Standard_EXPORT const TCollection_AsciiString& GetId() const;
  

  //! This method will be called by graphic driver each time when texture resource should be created.
  //! Default constructors allow defining the texture source as path to texture image or directly as pixmap.
  //! If the source is defined as path, then the image will be dynamically loaded when this method is called
  //! (and no copy will be preserved in this class instance).
  //! Inheritors may dynamically generate the image.
  //! Notice, image data should be in Bottom-Up order (see Image_PixMap::IsTopDown())!
  //! @return the image for texture.
  Standard_EXPORT virtual Image_PixMap_Handle GetImage() const;
  
  //! @return low-level texture parameters
  Standard_EXPORT const Handle(Graphic3d_TextureParams)& GetParams() const;
  

  //! The path to textures determined from CSF_MDTVTexturesDirectory or CASROOT environment variables.
  //! @return the root folder with default textures.
  Standard_EXPORT static TCollection_AsciiString TexturesFolder();



  DEFINE_STANDARD_RTTI(Graphic3d_TextureRoot,MMgt_TShared)

protected:

  
  //! Creates a texture from a file
  //! Warning: Note that if <FileName> is NULL the texture must be realized
  //! using LoadTexture(image) method.
  Standard_EXPORT Graphic3d_TextureRoot(const TCollection_AsciiString& theFileName, const Graphic3d_TypeOfTexture theType);
  
  //! Creates a texture from pixmap.
  //! Please note that the implementation expects the image data
  //! to be in Bottom-Up order (see Image_PixMap::IsTopDown()).
  Standard_EXPORT Graphic3d_TextureRoot(const Image_PixMap_Handle& thePixmap, const Graphic3d_TypeOfTexture theType);

  Handle(Graphic3d_TextureParams) myParams;
  TCollection_AsciiString myTexId;
  Image_PixMap_Handle myPixMap;
  OSD_Path myPath;


private:


  Graphic3d_TypeOfTexture myType;


};







#endif // _Graphic3d_TextureRoot_HeaderFile
