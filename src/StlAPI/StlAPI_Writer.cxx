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

void StlAPI_Writer::Write(const TopoDS_Shape& aShape,const Standard_CString aFileName) 
{
  OSD_Path aFile(aFileName);
  if (theRelativeMode) {
    Standard_Real aXmin, aYmin, aZmin, aXmax, aYmax, aZmax;
    Bnd_Box Total;
    BRepBndLib::Add(aShape, Total);
    Total.Get(aXmin, aYmin, aZmin, aXmax, aYmax, aZmax);
    theDeflection = MAX3(aXmax-aXmin , aYmax-aYmin , aZmax-aZmin)*theCoefficient;
  }
  StlTransfer::BuildIncrementalMesh(aShape, theDeflection, theStlMesh);
  // Write the built mesh
  if (theASCIIMode) {
    RWStl::WriteAscii(theStlMesh, aFile);
    }  
  else {
    RWStl::WriteBinary(theStlMesh, aFile);
    }
}

