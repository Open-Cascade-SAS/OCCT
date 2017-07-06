// Created on: 1997-02-13
// Created by: Alexander BRIVIN
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

#ifndef _Vrml_TextureCoordinate2_HeaderFile
#define _Vrml_TextureCoordinate2_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <TColgp_HArray1OfVec2d.hxx>
#include <Standard_Transient.hxx>
#include <Standard_OStream.hxx>


class Vrml_TextureCoordinate2;
DEFINE_STANDARD_HANDLE(Vrml_TextureCoordinate2, Standard_Transient)

//! defines a TextureCoordinate2 node of VRML specifying properties of geometry
//! and its appearance.
//! This  node  defines  a  set  of  2D  coordinates  to  be  used  to  map  textures
//! to  the  vertices  of  subsequent  PointSet,  IndexedLineSet,  or  IndexedFaceSet
//! objects.  It  replaces  the  current  texture  coordinates  in  the  rendering
//! state  for  the  shapes  to  use.
//! Texture  coordinates  range  from  0  to  1  across  the  texture.
//! The  horizontal  coordinate,  called  S,  is  specified  first,  followed
//! by  vertical  coordinate,  T.
//! By  default  :
//! myPoint (0 0)
class Vrml_TextureCoordinate2 : public Standard_Transient
{

public:

  
  Standard_EXPORT Vrml_TextureCoordinate2();
  
  Standard_EXPORT Vrml_TextureCoordinate2(const Handle(TColgp_HArray1OfVec2d)& aPoint);
  
  Standard_EXPORT void SetPoint (const Handle(TColgp_HArray1OfVec2d)& aPoint);
  
  Standard_EXPORT Handle(TColgp_HArray1OfVec2d) Point() const;
  
  Standard_EXPORT Standard_OStream& Print (Standard_OStream& anOStream) const;




  DEFINE_STANDARD_RTTIEXT(Vrml_TextureCoordinate2,Standard_Transient)

protected:




private:


  Handle(TColgp_HArray1OfVec2d) myPoint;


};







#endif // _Vrml_TextureCoordinate2_HeaderFile
