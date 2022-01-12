// Created on: 2016-07-07
// Copyright (c) 2016 OPEN CASCADE SAS
// Created by: Oleg AGASHIN
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

#ifndef _BRepMesh_MeshAlgoFactory_HeaderFile
#define _BRepMesh_MeshAlgoFactory_HeaderFile

#include <Standard_Transient.hxx>
#include <IMeshTools_MeshAlgoFactory.hxx>

//! Default implementation of IMeshTools_MeshAlgoFactory providing algorithms 
//! of different complexity depending on type of target surface.
class BRepMesh_MeshAlgoFactory : public IMeshTools_MeshAlgoFactory
{
public:

  //! Constructor.
  Standard_EXPORT BRepMesh_MeshAlgoFactory();

  //! Destructor.
  Standard_EXPORT virtual ~BRepMesh_MeshAlgoFactory();

  //! Creates instance of meshing algorithm for the given type of surface.
  Standard_EXPORT virtual Handle(IMeshTools_MeshAlgo) GetAlgo(
    const GeomAbs_SurfaceType    theSurfaceType,
    const IMeshTools_Parameters& theParameters) const Standard_OVERRIDE;

  DEFINE_STANDARD_RTTIEXT(BRepMesh_MeshAlgoFactory, IMeshTools_MeshAlgoFactory)
};

#endif