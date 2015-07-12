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

#ifndef _Graphic3d_Texture2D_HeaderFile
#define _Graphic3d_Texture2D_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Graphic3d_NameOfTexture2D.hxx>
#include <Graphic3d_TextureMap.hxx>
#include <Graphic3d_TypeOfTexture.hxx>
#include <Image_PixMap_Handle.hxx>
#include <Standard_Integer.hxx>
class Standard_OutOfRange;
class TCollection_AsciiString;


class Graphic3d_Texture2D;
DEFINE_STANDARD_HANDLE(Graphic3d_Texture2D, Graphic3d_TextureMap)

//! This abstract class for managing 2D textures
class Graphic3d_Texture2D : public Graphic3d_TextureMap
{

public:

  

  //! Returns the name of the predefined textures or NOT_2D_UNKNOWN
  //! when the name is given as a filename.
  Standard_EXPORT Graphic3d_NameOfTexture2D Name() const;
  

  //! Returns the number of predefined textures.
  Standard_EXPORT static Standard_Integer NumberOfTextures();
  

  //! Returns the name of the predefined texture of rank <aRank>
  Standard_EXPORT static TCollection_AsciiString TextureName (const Standard_Integer theRank);




  DEFINE_STANDARD_RTTI(Graphic3d_Texture2D,Graphic3d_TextureMap)

protected:

  
  Standard_EXPORT Graphic3d_Texture2D(const TCollection_AsciiString& theFileName, const Graphic3d_TypeOfTexture theType);
  
  Standard_EXPORT Graphic3d_Texture2D(const Graphic3d_NameOfTexture2D theName, const Graphic3d_TypeOfTexture theType);
  
  Standard_EXPORT Graphic3d_Texture2D(const Image_PixMap_Handle& thePixMap, const Graphic3d_TypeOfTexture theType);



private:


  Graphic3d_NameOfTexture2D myName;


};







#endif // _Graphic3d_Texture2D_HeaderFile
