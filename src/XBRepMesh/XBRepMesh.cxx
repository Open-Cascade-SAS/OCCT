// Created on: 2008-04-11
// Created by: Peter KURNEV
// Copyright (c) 2008-2014 OPEN CASCADE SAS
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

#include <XBRepMesh.hxx>
#include <BRepMesh_PluginMacro.hxx>
#include <BRepMesh_IncrementalMesh.hxx>

//=======================================================================
//function : Discret
//purpose  : 
//=======================================================================
Standard_Integer XBRepMesh::Discret(
  const TopoDS_Shape&   theShape,
  const Standard_Real   theDeflection,
  const Standard_Real   theAngle,
  BRepMesh_DiscretRoot* &theAlgo)
{
  Standard_Integer iErr;
  //
  iErr=0;
  BRepMesh_IncrementalMesh* anAlgo = new BRepMesh_IncrementalMesh;
  anAlgo->ChangeParameters().Deflection = theDeflection;
  anAlgo->ChangeParameters().Angle = theAngle;
  anAlgo->SetShape(theShape);
  theAlgo = anAlgo;

  return iErr;
}
DISCRETPLUGIN(XBRepMesh)
