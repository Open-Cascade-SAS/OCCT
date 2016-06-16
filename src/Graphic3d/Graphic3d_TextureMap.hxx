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

#ifndef _Graphic3d_TextureMap_HeaderFile
#define _Graphic3d_TextureMap_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Graphic3d_TextureRoot.hxx>
#include <Graphic3d_TypeOfTexture.hxx>
#include <Standard_Boolean.hxx>
#include <Graphic3d_LevelOfTextureAnisotropy.hxx>
class TCollection_AsciiString;


class Graphic3d_TextureMap;
DEFINE_STANDARD_HANDLE(Graphic3d_TextureMap, Graphic3d_TextureRoot)

//! This is an abstract class for managing texture applyable on polygons.
class Graphic3d_TextureMap : public Graphic3d_TextureRoot
{

public:

  

  //! enable texture smoothing
  Standard_EXPORT void EnableSmooth();
  

  //! Returns TRUE if the texture is smoothed.
  Standard_EXPORT Standard_Boolean IsSmoothed() const;
  

  //! disable texture smoothing
  Standard_EXPORT void DisableSmooth();
  

  //! enable texture modulate mode.
  //! the image is modulate with the shading of the surface.
  Standard_EXPORT void EnableModulate();
  

  //! disable texture modulate mode.
  //! the image is directly decal on the surface.
  Standard_EXPORT void DisableModulate();
  

  //! Returns TRUE if the texture is modulate.
  Standard_EXPORT Standard_Boolean IsModulate() const;
  

  //! use this methods if you want to enable
  //! texture repetition on your objects.
  Standard_EXPORT void EnableRepeat();
  

  //! use this methods if you want to disable
  //! texture repetition on your objects.
  Standard_EXPORT void DisableRepeat();
  

  //! Returns TRUE if the texture repeat is enable.
  Standard_EXPORT Standard_Boolean IsRepeat() const;
  
  //! @return level of anisontropy texture filter.
  //! Default value is Graphic3d_LOTA_OFF.
  Standard_EXPORT Graphic3d_LevelOfTextureAnisotropy AnisoFilter() const;
  
  //! @param theLevel level of anisontropy texture filter.
  Standard_EXPORT void SetAnisoFilter (const Graphic3d_LevelOfTextureAnisotropy theLevel);




  DEFINE_STANDARD_RTTIEXT(Graphic3d_TextureMap,Graphic3d_TextureRoot)

protected:

  
  Standard_EXPORT Graphic3d_TextureMap(const TCollection_AsciiString& theFileName, const Graphic3d_TypeOfTexture theType);
  
  Standard_EXPORT Graphic3d_TextureMap(const Handle(Image_PixMap)& thePixMap, const Graphic3d_TypeOfTexture theType);



private:




};







#endif // _Graphic3d_TextureMap_HeaderFile
