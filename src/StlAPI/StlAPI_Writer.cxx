// Copyright (c) 1999-2012 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.

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

