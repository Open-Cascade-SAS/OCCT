// Author: Ilya Khramov
// Copyright (c) 2019 OPEN CASCADE SAS
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

#ifndef _Graphic3d_CubeMapSeparate_HeaderFile
#define _Graphic3d_CubeMapSeparate_HeaderFile

#include <Graphic3d_CubeMap.hxx>
#include <NCollection_Array1.hxx>
#include <OSD_Path.hxx>

//! Class to manage cubemap located in six different images.
class Graphic3d_CubeMapSeparate : public Graphic3d_CubeMap
{
  DEFINE_STANDARD_RTTIEXT(Graphic3d_CubeMapSeparate, Graphic3d_CubeMap)
public:
  //! Initializes cubemap to be loaded from file.
  //! @thePaths - array of paths to separate image files (has to have size equal 6).
  Standard_EXPORT Graphic3d_CubeMapSeparate(
    const NCollection_Array1<TCollection_AsciiString>& thePaths);

  //! Initializes cubemap to be set directly from PixMaps.
  //! @theImages - array if PixMaps (has to have size equal 6).
  Standard_EXPORT Graphic3d_CubeMapSeparate(
    const NCollection_Array1<Handle(Image_PixMap)>& theImages);

  //! Returns current cubemap side as compressed PixMap.
  Standard_EXPORT virtual Handle(Image_CompressedPixMap) CompressedValue(
    const Handle(Image_SupportedFormats)& theSupported) Standard_OVERRIDE;

  //! Returns current side of cubemap as PixMap.
  //! Returns null handle if current side or whole cubemap is invalid.
  //! All origin images have to have the same sizes, format and quad shapes to form valid cubemap.
  Standard_EXPORT virtual Handle(Image_PixMap) Value(
    const Handle(Image_SupportedFormats)& theSupported) Standard_OVERRIDE;

  //! Returns NULL.
  virtual Handle(Image_PixMap) GetImage(const Handle(Image_SupportedFormats)&) Standard_OVERRIDE
  {
    return Handle(Image_PixMap)();
  }

  //! Checks if a texture class is valid or not.
  //! Returns true if the construction of the class is correct.
  Standard_EXPORT Standard_Boolean IsDone() const Standard_OVERRIDE;

  //! Empty destructor.
  ~Graphic3d_CubeMapSeparate() {}

protected:
  OSD_Path             myPaths[6];  //!< array of paths to cubemap images
  Handle(Image_PixMap) myImages[6]; //!< array of cubemap images

  Standard_Size mySize;   //!< size of each side of cubemap
  Image_Format  myFormat; //!< format each side of cubemap

private:
  //! Nulifies whole images array.
  void resetImages();
};

DEFINE_STANDARD_HANDLE(Graphic3d_CubeMapSeparate, Graphic3d_CubeMap)

#endif // _Graphic3d_CubeMapSeparate_HeaderFile
