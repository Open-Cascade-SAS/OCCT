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

#include <BRepMesh_FaceDiscret.hxx>
#include <IMeshData_Model.hxx>
#include <IMeshData_Face.hxx>
#include <IMeshData_Wire.hxx>
#include <IMeshData_Edge.hxx>
#include <IMeshTools_MeshAlgo.hxx>
#include <OSD_Parallel.hxx>

//=======================================================================
// Function: Constructor
// Purpose : 
//=======================================================================
BRepMesh_FaceDiscret::BRepMesh_FaceDiscret(
  const Handle(IMeshTools_MeshAlgoFactory)& theAlgoFactory)
  : myAlgoFactory(theAlgoFactory)
{
}

//=======================================================================
// Function: Destructor
// Purpose : 
//=======================================================================
BRepMesh_FaceDiscret::~BRepMesh_FaceDiscret()
{
}

//=======================================================================
// Function: Perform
// Purpose : 
//=======================================================================
Standard_Boolean BRepMesh_FaceDiscret::performInternal(
  const Handle(IMeshData_Model)& theModel,
  const IMeshTools_Parameters&   theParameters)
{
  myModel      = theModel;
  myParameters = theParameters;
  if (myModel.IsNull())
  {
    return Standard_False;
  }

  OSD_Parallel::For(0, myModel->FacesNb(), *this, !(myParameters.InParallel && myModel->FacesNb() > 1));

  myModel.Nullify(); // Do not hold link to model.
  return Standard_True;
}

//=======================================================================
// Function: process
// Purpose : 
//=======================================================================
void BRepMesh_FaceDiscret::process(const Standard_Integer theFaceIndex) const
{
  const IMeshData::IFaceHandle& aDFace = myModel->GetFace(theFaceIndex);
  if (aDFace->IsSet(IMeshData_Failure) ||
      aDFace->IsSet(IMeshData_Reused))
  {
    return;
  }

  try
  {
    OCC_CATCH_SIGNALS

    Handle(IMeshTools_MeshAlgo) aMeshingAlgo = 
      myAlgoFactory->GetAlgo(aDFace->GetSurface()->GetType(), myParameters);
  
    if (aMeshingAlgo.IsNull())
    {
      aDFace->SetStatus(IMeshData_Failure);
      return;
    }
  
    aMeshingAlgo->Perform(aDFace, myParameters);
  }
  catch (Standard_Failure const&)
  {
    aDFace->SetStatus (IMeshData_Failure);
  }
}
