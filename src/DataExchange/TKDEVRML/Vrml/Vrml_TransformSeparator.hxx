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

#ifndef _Vrml_TransformSeparator_HeaderFile
#define _Vrml_TransformSeparator_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Boolean.hxx>
#include <Standard_OStream.hxx>

//! defines a TransformSeparator node of VRML specifying group properties.
//! This  group  node  is  similar  to  separator  node  in  that it  saves state
//! before  traversing  its  children  and  restores  it  afterwards.
//! This  node  can  be  used  to  isolate  transformations  to  light  sources
//! or  objects.
class Vrml_TransformSeparator
{
public:
  DEFINE_STANDARD_ALLOC

  Standard_EXPORT Vrml_TransformSeparator();

  Standard_EXPORT Standard_OStream& Print(Standard_OStream& anOStream);

protected:
private:
  Standard_Boolean myFlagPrint;
};

#endif // _Vrml_TransformSeparator_HeaderFile
