// Created on: 1997-02-11
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

#ifndef _Vrml_Texture2Transform_HeaderFile
#define _Vrml_Texture2Transform_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <gp_Vec2d.hxx>
#include <Standard_OStream.hxx>


//! defines a Texture2Transform node of VRML specifying properties of geometry
//! and its appearance.
//! This  node  defines  a 2D  transformation  applied  to  texture  coordinates.
//! This  affect  the  way  textures  are  applied  to  the  surfaces  of  subsequent
//! shapes.
//! Transformation  consisits  of(in  order)  a  non-uniform  scale  about  an
//! arbitrary  center  point,  a  rotation  about  that  same  point,  and
//! a  translation.  This  allows  a  user  to  change  the  size  and  position  of
//! the  textures  on  the  shape.
//! By  default  :
//! myTranslation (0 0)
//! myRotation (0)
//! myScaleFactor (1 1)
//! myCenter (0 0)
class Vrml_Texture2Transform 
{
public:

  DEFINE_STANDARD_ALLOC

  
  Standard_EXPORT Vrml_Texture2Transform();
  
  Standard_EXPORT Vrml_Texture2Transform(const gp_Vec2d& aTranslation, const Standard_Real aRotation, const gp_Vec2d& aScaleFactor, const gp_Vec2d& aCenter);
  
  Standard_EXPORT void SetTranslation (const gp_Vec2d& aTranslation);
  
  Standard_EXPORT gp_Vec2d Translation() const;
  
  Standard_EXPORT void SetRotation (const Standard_Real aRotation);
  
  Standard_EXPORT Standard_Real Rotation() const;
  
  Standard_EXPORT void SetScaleFactor (const gp_Vec2d& aScaleFactor);
  
  Standard_EXPORT gp_Vec2d ScaleFactor() const;
  
  Standard_EXPORT void SetCenter (const gp_Vec2d& aCenter);
  
  Standard_EXPORT gp_Vec2d Center() const;
  
  Standard_EXPORT Standard_OStream& Print (Standard_OStream& anOStream) const;




protected:





private:



  gp_Vec2d myTranslation;
  Standard_Real myRotation;
  gp_Vec2d myScaleFactor;
  gp_Vec2d myCenter;


};







#endif // _Vrml_Texture2Transform_HeaderFile
