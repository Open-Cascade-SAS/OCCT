// Created on: 1995-02-16
// Created by: Dieter THIEMANN
// Copyright (c) 1995-1999 Matra Datavision
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

#ifndef _TopoDSToStep_FacetedTool_HeaderFile
#define _TopoDSToStep_FacetedTool_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <TopoDSToStep_FacetedError.hxx>
class TopoDS_Shape;


//! This Tool Class provides Information about Faceted Shapes
//! to be mapped to STEP.
class TopoDSToStep_FacetedTool 
{
public:

  DEFINE_STANDARD_ALLOC

  
  Standard_EXPORT static TopoDSToStep_FacetedError CheckTopoDSShape (const TopoDS_Shape& SH);




protected:





private:





};







#endif // _TopoDSToStep_FacetedTool_HeaderFile
