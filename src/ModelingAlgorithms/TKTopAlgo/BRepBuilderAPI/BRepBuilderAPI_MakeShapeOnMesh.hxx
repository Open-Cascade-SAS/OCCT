// Created on: 2022-06-30
// Created by: Alexander MALYSHEV
// Copyright (c) 2022-2022 OPEN CASCADE SAS
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

#ifndef _BRepBuilderAPI_MakeShapeOnMesh_HeaderFile
#define _BRepBuilderAPI_MakeShapeOnMesh_HeaderFile

#include <BRepBuilderAPI_MakeShape.hxx>
#include <Poly_Triangulation.hxx>

//! Builds shape on per-facet basis on the input mesh. Resulting shape has shared
//! edges by construction, but no maximization (unify same domain) is applied.
//! No generation history is provided.
class BRepBuilderAPI_MakeShapeOnMesh : public BRepBuilderAPI_MakeShape
{
public:
  DEFINE_STANDARD_ALLOC

  //! Ctor. Sets mesh to process.
  //! @param[in] theMesh  - Mesh to construct shape for.
  BRepBuilderAPI_MakeShapeOnMesh(const Handle(Poly_Triangulation)& theMesh)
      : myMesh(theMesh)
  {
  }

  //! Builds shape on mesh.
  Standard_EXPORT virtual void Build(
    const Message_ProgressRange& theRange = Message_ProgressRange()) Standard_OVERRIDE;

private:
  Handle(Poly_Triangulation) myMesh;
};

#endif // _BRepBuilderAPI_MakeShapeOnMesh_HeaderFile
