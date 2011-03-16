// File   Graphic3d_GraphicDriver_703.cxx
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
#include <Aspect_TypeOfMarker.hxx>

#include <Aspect_DriverDefinitionError.hxx>

//-Aliases

//-Global data definitions

//-Methods, in order

void Graphic3d_GraphicDriver::Marker (const Graphic3d_CGroup& , const Graphic3d_Vertex& , const Standard_Boolean ) {
}

void Graphic3d_GraphicDriver::MarkerSet (const Graphic3d_CGroup& , const Graphic3d_Array1OfVertex& , const Standard_Boolean ) {
}
