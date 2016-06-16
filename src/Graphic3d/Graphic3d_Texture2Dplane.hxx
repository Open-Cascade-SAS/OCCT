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

#ifndef _Graphic3d_Texture2Dplane_HeaderFile
#define _Graphic3d_Texture2Dplane_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Graphic3d_NameOfTexturePlane.hxx>
#include <Graphic3d_Texture2D.hxx>
#include <Graphic3d_NameOfTexture2D.hxx>
#include <Standard_ShortReal.hxx>
class TCollection_AsciiString;


class Graphic3d_Texture2Dplane;
DEFINE_STANDARD_HANDLE(Graphic3d_Texture2Dplane, Graphic3d_Texture2D)

//! This class allows the management of a 2D texture defined from a plane equation
//! Use the SetXXX() methods for positioning the texture as you want.
class Graphic3d_Texture2Dplane : public Graphic3d_Texture2D
{

public:

  
  //! Creates a texture from a file
  Standard_EXPORT Graphic3d_Texture2Dplane(const TCollection_AsciiString& theFileName);
  
  //! Creates a texture from a predefined texture name set.
  Standard_EXPORT Graphic3d_Texture2Dplane(const Graphic3d_NameOfTexture2D theNOT);
  
  //! Creates a texture from the pixmap.
  Standard_EXPORT Graphic3d_Texture2Dplane(const Handle(Image_PixMap)& thePixMap);
  
  //! Defines the texture projection plane for texture coordinate S
  //! default is <1.0, 0.0, 0.0, 0.0>
  Standard_EXPORT void SetPlaneS (const Standard_ShortReal A, const Standard_ShortReal B, const Standard_ShortReal C, const Standard_ShortReal D);
  
  //! Defines the texture projection plane for texture coordinate T
  //! default is <0.0, 1.0, 0.0, 0.0>
  Standard_EXPORT void SetPlaneT (const Standard_ShortReal A, const Standard_ShortReal B, const Standard_ShortReal C, const Standard_ShortReal D);
  
  //! Defines the texture projection plane for both S and T texture coordinate
  //! default is NOTP_XY meaning:
  //! <1.0, 0.0, 0.0, 0.0> for S
  //! and  <0.0, 1.0, 0.0, 0.0> for T
  Standard_EXPORT void SetPlane (const Graphic3d_NameOfTexturePlane thePlane);
  
  //! Defines the texture scale for the S texture coordinate
  //! much easier than recomputing the S plane equation
  //! but the result is the same
  //! default to 1.0
  Standard_EXPORT void SetScaleS (const Standard_ShortReal theVal);
  
  //! Defines the texture scale for the T texture coordinate
  //! much easier than recompution the T plane equation
  //! but the result is the same
  //! default to 1.0
  Standard_EXPORT void SetScaleT (const Standard_ShortReal theVal);
  
  //! Defines the texture translation for the S texture coordinate
  //! you can obtain the same effect by modifying the S plane
  //! equation but its not easier.
  //! default to 0.0
  Standard_EXPORT void SetTranslateS (const Standard_ShortReal theVal);
  
  //! Defines the texture translation for the T texture coordinate
  //! you can obtain the same effect by modifying the T plane
  //! equation but its not easier.
  //! default to 0.0
  Standard_EXPORT void SetTranslateT (const Standard_ShortReal theVal);
  
  //! Sets the rotation angle of the whole texture.
  //! the same result might be achieved by recomputing the
  //! S and T plane equation but it's not the easiest way...
  //! the angle is expressed in degrees
  //! default is 0.0
  Standard_EXPORT void SetRotation (const Standard_ShortReal theVal);
  
  //! Returns the current texture plane name or NOTP_UNKNOWN
  //! when the plane is user defined.
  Standard_EXPORT Graphic3d_NameOfTexturePlane Plane() const;
  
  //! Returns the current texture plane S equation
  Standard_EXPORT void PlaneS (Standard_ShortReal& A, Standard_ShortReal& B, Standard_ShortReal& C, Standard_ShortReal& D) const;
  
  //! Returns   the current texture plane T equation
  Standard_EXPORT void PlaneT (Standard_ShortReal& A, Standard_ShortReal& B, Standard_ShortReal& C, Standard_ShortReal& D) const;
  
  //! Returns  the current texture S translation value
  Standard_EXPORT void TranslateS (Standard_ShortReal& theVal) const;
  
  //! Returns the current texture T translation value
  Standard_EXPORT void TranslateT (Standard_ShortReal& theVal) const;
  
  //! Returns the current texture S scale value
  Standard_EXPORT void ScaleS (Standard_ShortReal& theVal) const;
  
  //! Returns the current texture T scale value
  Standard_EXPORT void ScaleT (Standard_ShortReal& theVal) const;
  
  //! Returns the current texture rotation angle
  Standard_EXPORT void Rotation (Standard_ShortReal& theVal) const;




  DEFINE_STANDARD_RTTIEXT(Graphic3d_Texture2Dplane,Graphic3d_Texture2D)

protected:




private:


  Graphic3d_NameOfTexturePlane myPlaneName;


};







#endif // _Graphic3d_Texture2Dplane_HeaderFile
