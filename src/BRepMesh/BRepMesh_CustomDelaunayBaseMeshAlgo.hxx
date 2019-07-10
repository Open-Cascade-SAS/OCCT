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

#ifndef _BRepMesh_CustomDelaunayBaseMeshAlgo_HeaderFile
#define _BRepMesh_CustomDelaunayBaseMeshAlgo_HeaderFile

class BRepMesh_DataStructureOfDelaun;
class BRepMesh_Delaun;

//! Class provides base functionality to build face triangulation using custom
//! triangulation algorithm with possibility to modify final mesh.
//! Performs generation of mesh using raw data from model.
template<class BaseAlgo>
class BRepMesh_CustomDelaunayBaseMeshAlgo : public BaseAlgo
{
public:

  //! Constructor.
  BRepMesh_CustomDelaunayBaseMeshAlgo ()
  {
  } 

  //! Destructor.
  virtual ~BRepMesh_CustomDelaunayBaseMeshAlgo ()
  {
  }

protected:

  //! Performs processing of generated mesh.
  virtual void postProcessMesh (BRepMesh_Delaun& theMesher,
                                const Message_ProgressRange& theRange)
  {
    const Handle(BRepMesh_DataStructureOfDelaun)& aStructure  = this->getStructure();
    std::pair<Standard_Integer, Standard_Integer> aCellsCount = this->getCellsCount (aStructure->NbNodes());
    theMesher.InitCirclesTool (aCellsCount.first, aCellsCount.second);

    BaseAlgo::postProcessMesh (theMesher, theRange);
  }
};

#endif
