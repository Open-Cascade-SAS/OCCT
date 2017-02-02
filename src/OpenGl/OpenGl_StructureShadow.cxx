// Created on: 2014-09-01
// Created by: Ivan SAZONOV
// Copyright (c) 2014 OPEN CASCADE SAS
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

#include <OpenGl_StructureShadow.hxx>

#include <Graphic3d_GraphicDriver.hxx>
#include <Standard_ProgramError.hxx>

IMPLEMENT_STANDARD_RTTIEXT(OpenGl_StructureShadow,OpenGl_Structure)

//=======================================================================
//function : OpenGl_StructureShadow
//purpose  :
//=======================================================================
OpenGl_StructureShadow::OpenGl_StructureShadow (const Handle(Graphic3d_StructureManager)& theManager,
                                                const Handle(OpenGl_Structure)&           theStructure)
: OpenGl_Structure (theManager)
{
  Handle(OpenGl_StructureShadow) aShadow = Handle(OpenGl_StructureShadow)::DownCast (theStructure);
  myParent = aShadow.IsNull() ? theStructure : aShadow->myParent;

  ContainsFacet = myParent->ContainsFacet;
  IsInfinite    = myParent->IsInfinite;
  myBndBox      = myParent->BoundingBox();

  OpenGl_Structure::SetTransformation (myParent->Transformation());
  myInstancedStructure = const_cast<OpenGl_Structure*> (myParent->InstancedStructure());
  myTrsfPers = myParent->TransformPersistence();

  // reuse instanced structure API
  myInstancedStructure = myParent.operator->();
}

// =======================================================================
// function : Connect
// purpose  :
// =======================================================================
void OpenGl_StructureShadow::Connect (Graphic3d_CStructure& )
{
  throw Standard_ProgramError("Error! OpenGl_StructureShadow::Connect() should not be called!");
}

// =======================================================================
// function : Disconnect
// purpose  :
// =======================================================================
void OpenGl_StructureShadow::Disconnect (Graphic3d_CStructure& )
{
  throw Standard_ProgramError("Error! OpenGl_StructureShadow::Disconnect() should not be called!");
}
