// Created on: 1995-09-21
// Created by: Philippe GIRODENGO
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

#ifndef _StlMesh_HeaderFile
#define _StlMesh_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

class StlMesh_Mesh;
class StlMesh_Mesh;
class StlMesh_MeshExplorer;
class StlMesh_MeshDomain;
class StlMesh_MeshTriangle;


//! Implements a  basic  mesh data-structure  for  the
//! needs of the application fast prototyping.
class StlMesh 
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! Sequence of meshes
  //! Make a merge of two Mesh and returns a new Mesh.
  //! Very useful if you want to merge partMesh and CheckSurfaceMesh
  //! for example
  Standard_EXPORT static Handle(StlMesh_Mesh) Merge (const Handle(StlMesh_Mesh)& mesh1, const Handle(StlMesh_Mesh)& mesh2);




protected:





private:




friend class StlMesh_Mesh;
friend class StlMesh_MeshExplorer;
friend class StlMesh_MeshDomain;
friend class StlMesh_MeshTriangle;

};







#endif // _StlMesh_HeaderFile
