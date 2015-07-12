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

#ifndef _Vrml_Translation_HeaderFile
#define _Vrml_Translation_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <gp_Vec.hxx>
#include <Standard_OStream.hxx>
class gp_Vec;


//! defines a Translation of VRML specifying transform
//! properties.
//! This  node  defines  a  translation  by  3D  vector.
//! By  default  :
//! myTranslation (0,0,0)
class Vrml_Translation 
{
public:

  DEFINE_STANDARD_ALLOC

  
  Standard_EXPORT Vrml_Translation();
  
  Standard_EXPORT Vrml_Translation(const gp_Vec& aTranslation);
  
  Standard_EXPORT void SetTranslation (const gp_Vec& aTranslation);
  
  Standard_EXPORT gp_Vec Translation() const;
  
  Standard_EXPORT Standard_OStream& Print (Standard_OStream& anOStream) const;




protected:





private:



  gp_Vec myTranslation;


};







#endif // _Vrml_Translation_HeaderFile
