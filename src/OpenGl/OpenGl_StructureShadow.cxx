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


IMPLEMENT_STANDARD_HANDLE (OpenGl_StructureShadow, OpenGl_Structure)
IMPLEMENT_STANDARD_RTTIEXT(OpenGl_StructureShadow, OpenGl_Structure)

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


  Composition   = myParent->Composition;
  ContainsFacet = myParent->ContainsFacet;
  IsInfinite    = myParent->IsInfinite;
  for (Standard_Integer i = 0; i <= 3; ++i)
  {
    for (Standard_Integer j = 0; j <= 3; ++j)
    {
      Graphic3d_CStructure::Transformation[i][j] = myParent->Graphic3d_CStructure::Transformation[i][j];
    }
  }

  TransformPersistence.IsSet = myParent->TransformPersistence.IsSet;
  TransformPersistence.Flag  = myParent->TransformPersistence.Flag;
  TransformPersistence.Point = myParent->TransformPersistence.Point;
}
