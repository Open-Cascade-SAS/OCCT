// Created on: 2016-04-07
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

#include <BRepMesh_ModelBuilder.hxx>
#include <BRepMeshData_Model.hxx>
#include <BRepMesh_ShapeVisitor.hxx>
#include <BRepMesh_ShapeTool.hxx>
#include <IMeshTools_ShapeExplorer.hxx>

#include <Bnd_Box.hxx>
#include <BRepBndLib.hxx>

IMPLEMENT_STANDARD_RTTIEXT(BRepMesh_ModelBuilder, IMeshTools_ModelBuilder)

//=======================================================================
// Function: Constructor
// Purpose : 
//=======================================================================
BRepMesh_ModelBuilder::BRepMesh_ModelBuilder ()
{
}

//=======================================================================
// Function: Destructor
// Purpose : 
//=======================================================================
BRepMesh_ModelBuilder::~BRepMesh_ModelBuilder ()
{
}

//=======================================================================
// Function: Perform
// Purpose : 
//=======================================================================
Handle (IMeshData_Model) BRepMesh_ModelBuilder::performInternal (
  const TopoDS_Shape&          theShape,
  const IMeshTools_Parameters& theParameters)
{
  Handle (BRepMeshData_Model) aModel;

  Bnd_Box aBox;
  BRepBndLib::Add (theShape, aBox, Standard_False);

  if (!aBox.IsVoid ())
  {
    // Build data model for further processing.
    aModel = new BRepMeshData_Model (theShape);

    if (theParameters.Relative)
    {
      Standard_Real aMaxSize;
      BRepMesh_ShapeTool::BoxMaxDimension (aBox, aMaxSize);
      aModel->SetMaxSize(aMaxSize);
    }
    else
    {
      aModel->SetMaxSize(Max(theParameters.Deflection,
                             theParameters.DeflectionInterior));
    }

    Handle (IMeshTools_ShapeVisitor) aVisitor =
      new BRepMesh_ShapeVisitor (aModel);

    IMeshTools_ShapeExplorer aExplorer (theShape);
    aExplorer.Accept (aVisitor);
    SetStatus (Message_Done1);
  }
  else
  {
    SetStatus (Message_Fail1);
  }

  return aModel;
}
