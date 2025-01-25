// Created on: 2016-04-19
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

#ifndef _BRepMesh_FaceDiscret_HeaderFile
#define _BRepMesh_FaceDiscret_HeaderFile

#include <IMeshTools_ModelAlgo.hxx>
#include <IMeshTools_Parameters.hxx>
#include <IMeshTools_MeshAlgoFactory.hxx>

//! Class implements functionality starting triangulation of model's faces.
//! Each face is processed separately and can be executed in parallel mode.
//! Uses mesh algo factory passed as initializer to create instance of triangulation
//! algorithm according to type of surface of target face.
class BRepMesh_FaceDiscret : public IMeshTools_ModelAlgo
{
public:
  //! Constructor.
  Standard_EXPORT BRepMesh_FaceDiscret(const Handle(IMeshTools_MeshAlgoFactory)& theAlgoFactory);

  //! Destructor.
  Standard_EXPORT virtual ~BRepMesh_FaceDiscret();

  DEFINE_STANDARD_RTTIEXT(BRepMesh_FaceDiscret, IMeshTools_ModelAlgo)

protected:
  //! Performs processing of faces of the given model.
  Standard_EXPORT virtual Standard_Boolean performInternal(
    const Handle(IMeshData_Model)& theModel,
    const IMeshTools_Parameters&   theParameters,
    const Message_ProgressRange&   theRange) Standard_OVERRIDE;

private:
  //! Checks existing discretization of the face and updates data model.
  void process(const Standard_Integer theFaceIndex, const Message_ProgressRange& theRange) const;

private:
  class FaceListFunctor;

private:
  Handle(IMeshTools_MeshAlgoFactory) myAlgoFactory;
  Handle(IMeshData_Model)            myModel;
  IMeshTools_Parameters              myParameters;
};

#endif
