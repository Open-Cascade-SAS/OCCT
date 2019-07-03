// Created on: 2019-06-07
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

#ifndef _BRepMesh_CustomBaseMeshAlgo_HeaderFile
#define _BRepMesh_CustomBaseMeshAlgo_HeaderFile

#include <BRepMesh_ConstrainedBaseMeshAlgo.hxx>
#include <NCollection_Shared.hxx>
#include <IMeshTools_Parameters.hxx>

#include <BRepMesh_Delaun.hxx>
#include <BRepMesh_MeshTool.hxx>

class BRepMesh_DataStructureOfDelaun;

//! Class provides base fuctionality to build face triangulation using custom triangulation algorithm.
//! Performs generation of mesh using raw data from model.
class BRepMesh_CustomBaseMeshAlgo : public BRepMesh_ConstrainedBaseMeshAlgo
{
public:

  //! Constructor.
  Standard_EXPORT BRepMesh_CustomBaseMeshAlgo ()
  {
  }

  //! Destructor.
  Standard_EXPORT virtual ~BRepMesh_CustomBaseMeshAlgo ()
  {
  }

  DEFINE_STANDARD_RTTI_INLINE(BRepMesh_CustomBaseMeshAlgo, BRepMesh_ConstrainedBaseMeshAlgo)

protected:

  //! Generates mesh for the contour stored in data structure.
  Standard_EXPORT virtual void generateMesh () Standard_OVERRIDE
  {
    const Handle (BRepMesh_DataStructureOfDelaun)& aStructure = this->getStructure ();
    buildBaseTriangulation ();

    std::pair<Standard_Integer, Standard_Integer> aCellsCount = this->getCellsCount (aStructure->NbNodes ());
    BRepMesh_Delaun aMesher (aStructure, aCellsCount.first, aCellsCount.second, Standard_False);
    aMesher.ProcessConstraints ();

    BRepMesh_MeshTool aCleaner (aStructure);
    aCleaner.EraseFreeLinks ();

    postProcessMesh (aMesher);
  }

protected:

  //! Builds base triangulation using custom triangulation algorithm.
  Standard_EXPORT virtual void buildBaseTriangulation() = 0;
};

#endif
