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

#include <StlAPI_Reader.hxx>

#include <BRepBuilderAPI_MakeShapeOnMesh.hxx>
#include <BRepBuilderAPI_Sewing.hxx>
#include <RWStl.hxx>

//=============================================================================
//function : Read
//purpose  :
//=============================================================================
Standard_Boolean StlAPI_Reader::Read (TopoDS_Shape&          theShape,
                                      const Standard_CString theFileName)
{
  Handle(Poly_Triangulation) aMesh = RWStl::ReadFile (theFileName);
  if (aMesh.IsNull())
    return Standard_False;

  BRepBuilderAPI_MakeShapeOnMesh aConverter(aMesh);
  aConverter.Build();
  if (!aConverter.IsDone())
    return Standard_False;

  TopoDS_Shape aResult = aConverter.Shape();
  if (aResult.IsNull())
    return Standard_False;

  theShape = aResult;
  return Standard_True;
}
