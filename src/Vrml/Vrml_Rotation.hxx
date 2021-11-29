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

#ifndef _Vrml_Rotation_HeaderFile
#define _Vrml_Rotation_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Vrml_SFRotation.hxx>
#include <Standard_OStream.hxx>


//! defines a Rotation node of VRML specifying matrix and transform properties.
//! This  node  defines  a  3D  rotation  about  an  arbitrary  axis  through  the  origin.
//! By  default  :  myRotation  =  (0 0 1 0)
class Vrml_Rotation 
{
public:

  DEFINE_STANDARD_ALLOC

  
  Standard_EXPORT Vrml_Rotation();
  
  Standard_EXPORT Vrml_Rotation(const Vrml_SFRotation& aRotation);
  
  Standard_EXPORT void SetRotation (const Vrml_SFRotation& aRotation);
  
  Standard_EXPORT Vrml_SFRotation Rotation() const;
  
  Standard_EXPORT Standard_OStream& Print (Standard_OStream& anOStream) const;




protected:





private:



  Vrml_SFRotation myRotation;


};







#endif // _Vrml_Rotation_HeaderFile
