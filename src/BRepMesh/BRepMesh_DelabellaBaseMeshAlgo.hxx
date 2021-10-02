// Created on: 2019-07-05
// Copyright (c) 2019 OPEN CASCADE SAS
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

#ifndef _BRepMesh_DelabellaBaseMeshAlgo_HeaderFile
#define _BRepMesh_DelabellaBaseMeshAlgo_HeaderFile

#include <BRepMesh_CustomBaseMeshAlgo.hxx>
#include <NCollection_Shared.hxx>
#include <IMeshTools_Parameters.hxx>


//! Class provides base functionality to build face triangulation using Delabella project.
//! Performs generation of mesh using raw data from model.
class BRepMesh_DelabellaBaseMeshAlgo : public BRepMesh_CustomBaseMeshAlgo
{
public:

  //! Constructor.
  Standard_EXPORT BRepMesh_DelabellaBaseMeshAlgo ();

  //! Destructor.
  Standard_EXPORT virtual ~BRepMesh_DelabellaBaseMeshAlgo ();

  DEFINE_STANDARD_RTTIEXT(BRepMesh_DelabellaBaseMeshAlgo, BRepMesh_CustomBaseMeshAlgo)

protected:

  //! Builds base triangulation using Delabella project.
  Standard_EXPORT virtual void buildBaseTriangulation() Standard_OVERRIDE;
};

#endif
