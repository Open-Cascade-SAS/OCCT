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

#ifndef _StlMesh_MeshExplorer_HeaderFile
#define _StlMesh_MeshExplorer_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Real.hxx>
#include <Standard_Integer.hxx>
#include <TColgp_SequenceOfXYZ.hxx>
#include <StlMesh_SequenceOfMeshTriangle.hxx>
#include <Standard_Boolean.hxx>
class StlMesh_Mesh;
class Standard_OutOfRange;
class Standard_NoMoreObject;
class Standard_NoSuchObject;


//! Provides  facilities to explore  the triangles  of
//! each mesh domain.
class StlMesh_MeshExplorer 
{
public:

  DEFINE_STANDARD_ALLOC

  
  Standard_EXPORT StlMesh_MeshExplorer(const Handle(StlMesh_Mesh)& M);
  
  //! Returns the mesh deflection of the current domain.
  Standard_EXPORT Standard_Real Deflection() const;
  
  //! Initializes the exploration  of the  triangles  of
  //! the mesh domain of range <DomainIndex>.
  //! Raised if <DomainIndex> is lower than 1 or greater
  //! than the number of domains.
  Standard_EXPORT void InitTriangle (const Standard_Integer DomainIndex = 1);
  
    Standard_Boolean MoreTriangle() const;
  
  //! Raised if there is no more triangle in the current
  //! domain.
  Standard_EXPORT void NextTriangle();
  
  //! Raised if there is no more triangle in the current
  //! domain.
  Standard_EXPORT void TriangleVertices (Standard_Real& X1, Standard_Real& Y1, Standard_Real& Z1, Standard_Real& X2, Standard_Real& Y2, Standard_Real& Z2, Standard_Real& X3, Standard_Real& Y3, Standard_Real& Z3) const;
  
  //! Raised if there is no more triangle in the current
  //! domain.
  Standard_EXPORT void TriangleOrientation (Standard_Real& Xn, Standard_Real& Yn, Standard_Real& Zn) const;




protected:





private:



  Handle(StlMesh_Mesh) mesh;
  Standard_Real xn;
  Standard_Real yn;
  Standard_Real zn;
  Standard_Integer v1;
  Standard_Integer v2;
  Standard_Integer v3;
  Standard_Integer domainIndex;
  Standard_Integer nbTriangles;
  Standard_Integer triangleIndex;
  TColgp_SequenceOfXYZ trianglesVertex;
  StlMesh_SequenceOfMeshTriangle trianglesdef;


};


#include <StlMesh_MeshExplorer.lxx>





#endif // _StlMesh_MeshExplorer_HeaderFile
