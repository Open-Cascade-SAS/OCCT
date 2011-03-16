// File   Graphic3d_GraphicDriver_5.cxx
// Created  Mardi 28 janvier 1997
// Author CAL
//      02.15.100 : JR : Clutter

//-Copyright  MatraDatavision 1997

//-Version  

//-Design Declaration des variables specifiques aux Drivers

//-Warning  Un driver encapsule les Pex et OpenGl drivers

//-References 

//-Language C++ 2.0

//-Declarations

// for the class
#include <Graphic3d_GraphicDriver.jxx>

#include <Aspect_DriverDefinitionError.hxx>

//-Aliases

//-Global data definitions

//-Methods, in order


void Graphic3d_GraphicDriver::DumpGroup (const Graphic3d_CGroup& ) {
}


void Graphic3d_GraphicDriver::DumpStructure (const Graphic3d_CStructure& ) {
}


void Graphic3d_GraphicDriver::DumpView (const Graphic3d_CView& ) {
}


Standard_Boolean Graphic3d_GraphicDriver::ElementExploration (const Graphic3d_CStructure & , const Standard_Integer , Graphic3d_VertexNC& , Graphic3d_Vector& ) {
  return Standard_False;
}


Graphic3d_TypeOfPrimitive Graphic3d_GraphicDriver::ElementType (const Graphic3d_CStructure & , const Standard_Integer ) {
  return Graphic3d_TOP_UNDEFINED;
}
