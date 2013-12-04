// Copyright (c) 1999-2014 OPEN CASCADE SAS
//
// This file is part of Open CASCADE Technology software library.
//
// This library is free software; you can redistribute it and / or modify it
// under the terms of the GNU Lesser General Public version 2.1 as published
// by the Free Software Foundation, with special exception defined in the file
// OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
// distribution for complete text of the license and disclaimer of any warranty.
//
// Alternatively, this file may be used under the terms of Open CASCADE
// commercial license or contractual agreement.

#include <StlAPI_Writer.ixx>
#include <StlTransfer.hxx>
#include <TopoDS_Shape.hxx>
#include <Bnd_Box.hxx>
#include <RWStl.hxx>
#include <BRepBndLib.hxx>
#include <OSD_Path.hxx>

#define MAX2(X, Y)      (  Abs(X) > Abs(Y)? Abs(X) : Abs(Y) )
#define MAX3(X, Y, Z)   ( MAX2 ( MAX2(X,Y) , Z) )

StlAPI_Writer::StlAPI_Writer()
{
  theStlMesh = new StlMesh_Mesh;
  theASCIIMode = Standard_True;
  theDeflection = 0.01;
  theRelativeMode = Standard_True;
  theCoefficient = 0.001;
}

void StlAPI_Writer::SetDeflection(const Standard_Real aDeflection) 
{
  theDeflection = aDeflection;
}
void StlAPI_Writer::SetCoefficient(const Standard_Real aCoefficient) 
{
  theCoefficient = aCoefficient;
}

Standard_Boolean& StlAPI_Writer::RelativeMode() 
{
  return theRelativeMode;
}

Standard_Boolean& StlAPI_Writer::ASCIIMode() 
{
  return theASCIIMode;
}

void StlAPI_Writer::Write(const TopoDS_Shape& theShape, const Standard_CString theFileName, const Standard_Boolean theInParallel) 
{
  OSD_Path aFile(theFileName);
  if (theRelativeMode) {
    Standard_Real aXmin, aYmin, aZmin, aXmax, aYmax, aZmax;
    Bnd_Box Total;
    BRepBndLib::Add(theShape, Total);
    Total.Get(aXmin, aYmin, aZmin, aXmax, aYmax, aZmax);
    theDeflection = MAX3(aXmax-aXmin , aYmax-aYmin , aZmax-aZmin)*theCoefficient;
  }
  StlTransfer::BuildIncrementalMesh(theShape, theDeflection, theInParallel, theStlMesh);
  // Write the built mesh
  if (theASCIIMode) {
    RWStl::WriteAscii(theStlMesh, aFile);
    }  
  else {
    RWStl::WriteBinary(theStlMesh, aFile);
    }
}

