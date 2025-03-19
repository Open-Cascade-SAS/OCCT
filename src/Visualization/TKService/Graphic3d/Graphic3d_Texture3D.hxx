// Copyright (c) 2022 OPEN CASCADE SAS
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

#ifndef _Graphic3d_Texture3D_HeaderFile
#define _Graphic3d_Texture3D_HeaderFile

#include <Graphic3d_TextureMap.hxx>
#include <NCollection_Array1.hxx>

//! This abstract class for managing 3D textures.
class Graphic3d_Texture3D : public Graphic3d_TextureMap
{
  DEFINE_STANDARD_RTTIEXT(Graphic3d_Texture3D, Graphic3d_TextureMap)
public:
  //! Creates a texture from a file.
  Standard_EXPORT Graphic3d_Texture3D(const TCollection_AsciiString& theFileName);

  //! Creates a texture from the pixmap.
  Standard_EXPORT Graphic3d_Texture3D(const Handle(Image_PixMap)& thePixMap);

  //! Creates a texture from a file.
  Standard_EXPORT Graphic3d_Texture3D(const NCollection_Array1<TCollection_AsciiString>& theFiles);

  //! Destructor.
  Standard_EXPORT virtual ~Graphic3d_Texture3D();

  //! Assign new image to the texture.
  //! Note that this method does not invalidate already uploaded resources - consider calling
  //! ::UpdateRevision() if needed.
  Standard_EXPORT void SetImage(const Handle(Image_PixMap)& thePixMap);

  //! Load and return image.
  Standard_EXPORT virtual Handle(Image_PixMap) GetImage(
    const Handle(Image_SupportedFormats)& theSupported) Standard_OVERRIDE;

protected:
  NCollection_Array1<TCollection_AsciiString> myPaths;
};

#endif // _Graphic3d_Texture3D_HeaderFile
