// Created on: 2019-07-08
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

#ifndef _BRepMesh_ConstrainedBaseMeshAlgo_HeaderFile
#define _BRepMesh_ConstrainedBaseMeshAlgo_HeaderFile

#include <BRepMesh_BaseMeshAlgo.hxx>
#include <NCollection_Shared.hxx>
#include <IMeshTools_Parameters.hxx>

class BRepMesh_Delaun;

//! Class provides base functionality to build face triangulation using Dealunay approach.
//! Performs generation of mesh using raw data from model.
class BRepMesh_ConstrainedBaseMeshAlgo : public BRepMesh_BaseMeshAlgo
{
public:

  //! Constructor.
  BRepMesh_ConstrainedBaseMeshAlgo ()
  {
  }

  //! Destructor.
  virtual ~BRepMesh_ConstrainedBaseMeshAlgo ()
  {
  }

  DEFINE_STANDARD_RTTIEXT(BRepMesh_ConstrainedBaseMeshAlgo, BRepMesh_BaseMeshAlgo)

protected:

  //! Returns size of cell to be used by acceleration circles grid structure.
  virtual std::pair<Standard_Integer, Standard_Integer> getCellsCount (const Standard_Integer /*theVerticesNb*/)
  {
    return std::pair<Standard_Integer, Standard_Integer> (-1, -1);
  }

  //! Performs processing of generated mesh.
  //! By default does nothing.
  //! Expected to be called from method generateMesh() in successor classes.
  virtual void postProcessMesh (BRepMesh_Delaun&              /*theMesher*/,
                                const Message_ProgressRange&  /*theRange*/)
  {
  }
};

#endif
