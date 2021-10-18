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

#ifndef _Graphic3d_TextureEnv_HeaderFile
#define _Graphic3d_TextureEnv_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Graphic3d_NameOfTextureEnv.hxx>
#include <Graphic3d_TextureRoot.hxx>
#include <Standard_Integer.hxx>
class TCollection_AsciiString;


class Graphic3d_TextureEnv;
DEFINE_STANDARD_HANDLE(Graphic3d_TextureEnv, Graphic3d_TextureRoot)

//! This class provides environment texture.
class Graphic3d_TextureEnv : public Graphic3d_TextureRoot
{

public:

  
  //! Creates an environment texture from a file.
  Standard_EXPORT Graphic3d_TextureEnv(const TCollection_AsciiString& theFileName);
  
  //! Creates an environment texture from a predefined texture name set.
  Standard_EXPORT Graphic3d_TextureEnv(const Graphic3d_NameOfTextureEnv theName);
  
  //! Creates an environment texture from the pixmap.
  Standard_EXPORT Graphic3d_TextureEnv(const Handle(Image_PixMap)& thePixMap);
  

  //! Returns the name of the predefined textures or NOT_ENV_UNKNOWN
  //! when the name is given as a filename.
  Standard_EXPORT Graphic3d_NameOfTextureEnv Name() const;
  

  //! Returns the number of predefined textures.
  Standard_EXPORT static Standard_Integer NumberOfTextures();
  

  //! Returns the name of the predefined texture of rank <aRank>
  Standard_EXPORT static TCollection_AsciiString TextureName (const Standard_Integer theRank);




  DEFINE_STANDARD_RTTIEXT(Graphic3d_TextureEnv,Graphic3d_TextureRoot)

protected:




private:


  Graphic3d_NameOfTextureEnv myName;


};







#endif // _Graphic3d_TextureEnv_HeaderFile
