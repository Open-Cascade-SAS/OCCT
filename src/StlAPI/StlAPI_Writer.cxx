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


#include <Bnd_Box.hxx>
#include <BRepBndLib.hxx>
#include <OSD_Path.hxx>
#include <RWStl.hxx>
#include <StlAPI_Writer.hxx>
#include <StlMesh_Mesh.hxx>
#include <StlTransfer.hxx>
#include <TopoDS_Shape.hxx>

StlAPI_Writer::StlAPI_Writer()
{
  theStlMesh = new StlMesh_Mesh;
  theASCIIMode = Standard_True;
}

Standard_Boolean& StlAPI_Writer::ASCIIMode() 
{
  return theASCIIMode;
}

StlAPI_ErrorStatus StlAPI_Writer::Write(const TopoDS_Shape& theShape, const Standard_CString theFileName)
{
  OSD_Path aFile(theFileName);
  StlTransfer::RetrieveMesh(theShape, theStlMesh);

  if (theStlMesh.IsNull() || theStlMesh->IsEmpty())
    return StlAPI_MeshIsEmpty;

  // Write the built mesh
  Standard_Boolean wasFileOpened = Standard_False;
  if (theASCIIMode) {
    wasFileOpened = RWStl::WriteAscii(theStlMesh, aFile);
    }  
  else {
    wasFileOpened = RWStl::WriteBinary(theStlMesh, aFile);
    }

  if (!wasFileOpened)
    return StlAPI_CannotOpenFile;

  return StlAPI_StatusOK;
}

