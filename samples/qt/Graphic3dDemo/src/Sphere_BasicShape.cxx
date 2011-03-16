#define xG005

#include <Sphere_BasicShape.ixx>
#include <TopLoc_Location.hxx>
#include <gp_Trsf.hxx>
#include <gp_XYZ.hxx>

//==================================================
// Function: 
// Purpose :
//==================================================

Sphere_BasicShape::Sphere_BasicShape(const TopoDS_Shape& aShape,
			 const Standard_Real aDeflection,
			 const Standard_Boolean hasVNormals,
			 const Standard_Boolean hasVColors,
			 const Standard_Boolean hasVTexels
) :
AIS_Shape(aShape),
myDeflection(aDeflection),
myVNormalsFlag(hasVNormals),
myVColorsFlag(hasVColors),
myVTexelsFlag(hasVTexels)
{
}

void Sphere_BasicShape::SetPosition(const gp_Pnt& aPosition) {
  gp_Trsf trsf;
  trsf.SetTranslationPart(gp_Vec(aPosition.XYZ()));
  TopLoc_Location location(trsf);
  myshape.Location(location);
}
 
gp_Pnt Sphere_BasicShape::Position() const {
  TopLoc_Location location = myshape.Location();
  gp_Trsf trsf = location.Transformation();
  gp_XYZ xyz = trsf.TranslationPart();
  return gp_Pnt(xyz.X(),xyz.Y(),xyz.Z());
}

void Sphere_BasicShape::SetVNormals(const Standard_Boolean aFlag) {
  myVNormalsFlag = aFlag;
}

Standard_Boolean Sphere_BasicShape::VNormalsFlag() const {
  return myVNormalsFlag;
}

void Sphere_BasicShape::SetVColors(const Standard_Boolean aFlag) {
  myVColorsFlag = aFlag;
}

Standard_Boolean Sphere_BasicShape::VColorsFlag() const {
  return myVColorsFlag;
}

void Sphere_BasicShape::SetVTexels(const Standard_Boolean aFlag) {
  myVTexelsFlag = aFlag;
}

Standard_Boolean Sphere_BasicShape::VTexelsFlag() const {
  return myVTexelsFlag;
}

void Sphere_BasicShape::SetDeflection(const Standard_Real aValue) {
  myDeflection = aValue;
}

Standard_Real Sphere_BasicShape::Deflection() const {
  return myDeflection;
}
