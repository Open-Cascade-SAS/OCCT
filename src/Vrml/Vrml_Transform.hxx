// Created on: 1997-02-12
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

#ifndef _Vrml_Transform_HeaderFile
#define _Vrml_Transform_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <gp_Vec.hxx>
#include <Vrml_SFRotation.hxx>
#include <Standard_OStream.hxx>
class gp_Vec;
class Vrml_SFRotation;


//! defines a Transform of VRML specifying transform
//! properties.
//! This  node  defines  a  geometric  3D  transformation   consisting   of  (in  order)
//! a  (possibly)  non-uniform  scale  about  an  arbitrary  point,  a  rotation  about
//! an arbitrary point  and  axis  and  translation.
//! By  default  :
//! myTranslation (0,0,0)
//! myRotation  (0,0,1,0)
//! myScaleFactor (1,1,1)
//! myScaleOrientation (0,0,1,0)
//! myCenter (0,0,0)
class Vrml_Transform 
{
public:

  DEFINE_STANDARD_ALLOC

  
  Standard_EXPORT Vrml_Transform();
  
  Standard_EXPORT Vrml_Transform(const gp_Vec& aTranslation, const Vrml_SFRotation& aRotation, const gp_Vec& aScaleFactor, const Vrml_SFRotation& aScaleOrientation, const gp_Vec& aCenter);
  
  Standard_EXPORT void SetTranslation (const gp_Vec& aTranslation);
  
  Standard_EXPORT gp_Vec Translation() const;
  
  Standard_EXPORT void SetRotation (const Vrml_SFRotation& aRotation);
  
  Standard_EXPORT Vrml_SFRotation Rotation() const;
  
  Standard_EXPORT void SetScaleFactor (const gp_Vec& aScaleFactor);
  
  Standard_EXPORT gp_Vec ScaleFactor() const;
  
  Standard_EXPORT void SetScaleOrientation (const Vrml_SFRotation& aScaleOrientation);
  
  Standard_EXPORT Vrml_SFRotation ScaleOrientation() const;
  
  Standard_EXPORT void SetCenter (const gp_Vec& aCenter);
  
  Standard_EXPORT gp_Vec Center() const;
  
  Standard_EXPORT Standard_OStream& Print (Standard_OStream& anOStream) const;




protected:





private:



  gp_Vec myTranslation;
  Vrml_SFRotation myRotation;
  gp_Vec myScaleFactor;
  Vrml_SFRotation myScaleOrientation;
  gp_Vec myCenter;


};







#endif // _Vrml_Transform_HeaderFile
