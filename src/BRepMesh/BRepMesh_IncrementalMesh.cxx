// Created on: 1995-06-20
// Created by: Stagiaire Alain JOURDAIN
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

#include <BRepMesh_IncrementalMesh.hxx>
#include <BRepMesh_Context.hxx>
#include <BRepMesh_PluginMacro.hxx>
#include <IMeshData_Face.hxx>
#include <IMeshData_Wire.hxx>
#include <IMeshTools_MeshBuilder.hxx>

IMPLEMENT_STANDARD_RTTIEXT(BRepMesh_IncrementalMesh, BRepMesh_DiscretRoot)

namespace
{
  //! Default flag to control parallelization for BRepMesh_IncrementalMesh
  //! tool returned for Mesh Factory
  static Standard_Boolean IS_IN_PARALLEL = Standard_False;
}

//=======================================================================
//function : Default constructor
//purpose  : 
//=======================================================================
BRepMesh_IncrementalMesh::BRepMesh_IncrementalMesh()
: myModified(Standard_False),
  myStatus(IMeshData_NoError)
{
}

//=======================================================================
//function : Constructor
//purpose  : 
//=======================================================================
BRepMesh_IncrementalMesh::BRepMesh_IncrementalMesh( const TopoDS_Shape&    theShape,
                                                    const Standard_Real    theLinDeflection,
                                                    const Standard_Boolean isRelative,
                                                    const Standard_Real    theAngDeflection,
                                                    const Standard_Boolean isInParallel)
: myModified(Standard_False),
  myStatus(IMeshData_NoError)
{
  myParameters.Deflection = theLinDeflection;
  myParameters.Angle      = theAngDeflection;
  myParameters.Relative   = isRelative;
  myParameters.InParallel = isInParallel;

  myShape = theShape;
  Perform();
}

//=======================================================================
//function : Constructor
//purpose  : 
//=======================================================================
BRepMesh_IncrementalMesh::BRepMesh_IncrementalMesh(
  const TopoDS_Shape&          theShape,
  const IMeshTools_Parameters& theParameters,
  const Message_ProgressRange& theRange)
  : myParameters(theParameters)
{
  myShape = theShape;
  Perform(theRange);
}

//=======================================================================
//function : Destructor
//purpose  : 
//=======================================================================
BRepMesh_IncrementalMesh::~BRepMesh_IncrementalMesh()
{
}

//=======================================================================
//function : Perform
//purpose  : 
//=======================================================================
void BRepMesh_IncrementalMesh::Perform(const Message_ProgressRange& theRange)
{
  Handle(BRepMesh_Context) aContext = new BRepMesh_Context (myParameters.MeshAlgo);
  Perform (aContext, theRange);
}

//=======================================================================
//function : Perform
//purpose  : 
//=======================================================================
void BRepMesh_IncrementalMesh::Perform(const Handle(IMeshTools_Context)& theContext, const Message_ProgressRange& theRange)
{
  initParameters();

  theContext->SetShape(Shape());
  theContext->ChangeParameters()            = myParameters;
  theContext->ChangeParameters().CleanModel = Standard_False;

  Message_ProgressScope aPS(theRange, "Perform incmesh", 10);
  IMeshTools_MeshBuilder aIncMesh(theContext);
  aIncMesh.Perform(aPS.Next(9));
  if (!aPS.More())
  {
    myStatus = IMeshData_UserBreak;
    return;
  }
  myStatus = IMeshData_NoError;
  const Handle(IMeshData_Model)& aModel = theContext->GetModel();
  if (!aModel.IsNull())
  {
    for (Standard_Integer aFaceIt = 0; aFaceIt < aModel->FacesNb(); ++aFaceIt)
    {
      const IMeshData::IFaceHandle& aDFace = aModel->GetFace(aFaceIt);
      myStatus |= aDFace->GetStatusMask();

      for (Standard_Integer aWireIt = 0; aWireIt < aDFace->WiresNb(); ++aWireIt)
      {
        const IMeshData::IWireHandle& aDWire = aDFace->GetWire(aWireIt);
        myStatus |= aDWire->GetStatusMask();
      }
    }
  }
  aPS.Next(1);
  setDone();
}

//=======================================================================
//function : Discret
//purpose  :
//=======================================================================
Standard_Integer BRepMesh_IncrementalMesh::Discret(
  const TopoDS_Shape&    theShape,
  const Standard_Real    theDeflection,
  const Standard_Real    theAngle,
  BRepMesh_DiscretRoot* &theAlgo)
{
  BRepMesh_IncrementalMesh* anAlgo = new BRepMesh_IncrementalMesh();
  anAlgo->ChangeParameters().Deflection = theDeflection;
  anAlgo->ChangeParameters().Angle      = theAngle;
  anAlgo->ChangeParameters().InParallel = IS_IN_PARALLEL;
  anAlgo->SetShape (theShape);
  theAlgo = anAlgo;
  return 0; // no error
}

//=======================================================================
//function : IsParallelDefault
//purpose  :
//=======================================================================
Standard_Boolean BRepMesh_IncrementalMesh::IsParallelDefault()
{
  return IS_IN_PARALLEL;
}

//=======================================================================
//function : Discret
//purpose  :
//=======================================================================
void BRepMesh_IncrementalMesh::SetParallelDefault(
  const Standard_Boolean theInParallel)
{
  IS_IN_PARALLEL = theInParallel;
}

//! Export Mesh Plugin entry function
DISCRETPLUGIN(BRepMesh_IncrementalMesh)
