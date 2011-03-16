// File   Graphic3d_GraphicDriver_4.cxx
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


void Graphic3d_GraphicDriver::ClearStructure (const Graphic3d_CStructure& ) {
}

void Graphic3d_GraphicDriver::Connect (const Graphic3d_CStructure& , const Graphic3d_CStructure& ) {
}

void Graphic3d_GraphicDriver::ContextStructure (const Graphic3d_CStructure& ) {
}

void Graphic3d_GraphicDriver::Disconnect (const Graphic3d_CStructure& , const Graphic3d_CStructure& ) {
}

void Graphic3d_GraphicDriver::DisplayStructure (const Graphic3d_CView& , const Graphic3d_CStructure& , const Standard_Integer ) {
}

void Graphic3d_GraphicDriver::EraseStructure (const Graphic3d_CView& , const Graphic3d_CStructure& ) {
}

void Graphic3d_GraphicDriver::RemoveStructure (const Graphic3d_CStructure& ) {
}

void Graphic3d_GraphicDriver::Structure (Graphic3d_CStructure& ) {
}
