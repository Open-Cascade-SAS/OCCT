// Created on: 1997-02-11
// Created by: Alexander BRIVIN and Dmitry Tarasov
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

#ifndef _Vrml_NormalBinding_HeaderFile
#define _Vrml_NormalBinding_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Vrml_MaterialBindingAndNormalBinding.hxx>
#include <Standard_OStream.hxx>


//! defines a NormalBinding node of VRML specifying properties of geometry
//! and its appearance.
//! This node specifies how the current normals are bound to shapes that follow in the scene
//! graph. Each shape node may interpret bindings differently.
//! The bindings for faces and vertices are meaningful only for shapes that are made from
//! faces and vertices. Similarly, the indexed bindings are only used by the shapes that allow
//! indexing. For bindings that require multiple normals, be sure to have at least as many
//! normals defined as are necessary; otherwise, errors will occur.
class Vrml_NormalBinding 
{
public:

  DEFINE_STANDARD_ALLOC

  
  Standard_EXPORT Vrml_NormalBinding(const Vrml_MaterialBindingAndNormalBinding aValue);
  
  Standard_EXPORT Vrml_NormalBinding();
  
  Standard_EXPORT void SetValue (const Vrml_MaterialBindingAndNormalBinding aValue);
  
  Standard_EXPORT Vrml_MaterialBindingAndNormalBinding Value() const;
  
  Standard_EXPORT Standard_OStream& Print (Standard_OStream& anOStream) const;




protected:





private:



  Vrml_MaterialBindingAndNormalBinding myValue;


};







#endif // _Vrml_NormalBinding_HeaderFile
