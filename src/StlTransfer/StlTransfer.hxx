// Created on: 1994-11-14
// Created by: Jean Claude VAUTHIER 
// Copyright (c) 1994-1999 Matra Datavision
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

#ifndef _StlTransfer_HeaderFile
#define _StlTransfer_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

class TopoDS_Shape;
class StlMesh_Mesh;


//! The  package   Algorithm  for Meshing   implements
//! facilities to retrieve the Mesh data-structure from a shape of package
//! TopoDS.  The triangulation  should be computed before.
//! The  result   is  stored  in  the  mesh
//! data-structure Mesh from package StlMesh.
class StlTransfer 
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! Retrieve a Mesh data-structure from the Shape, convert and store it into the Mesh.
  Standard_EXPORT static void RetrieveMesh (const TopoDS_Shape& Shape, const Handle(StlMesh_Mesh)& Mesh);




protected:





private:





};







#endif // _StlTransfer_HeaderFile
