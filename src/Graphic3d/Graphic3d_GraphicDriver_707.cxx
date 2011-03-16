// File   Graphic3d_GraphicDriver_707.cxx
// Created  Mardi 28 janvier 1997
// Author CAL
//              11/97 ; CAL : ajout polyline par 2 points
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

void Graphic3d_GraphicDriver::Polyline (const Graphic3d_CGroup& , const Graphic3d_Array1OfVertex& , const Standard_Boolean ) {
}

void Graphic3d_GraphicDriver::Polyline (const Graphic3d_CGroup& , const Graphic3d_Array1OfVertexC& , const Standard_Boolean ) {
}

void Graphic3d_GraphicDriver::Polyline (const Graphic3d_CGroup& , const Standard_Real , const Standard_Real , const Standard_Real , const Standard_Real , const Standard_Real , const Standard_Real , const Standard_Boolean ) {
}
