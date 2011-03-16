// File   Graphic3d_GraphicDriver_Layer.cxx
// Created  Mardi 3 novembre 1998
// Author CAL
//      02.15.100 : JR : Clutter

//-Copyright  MatraDatavision 1998

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

void Graphic3d_GraphicDriver::Layer (Aspect_CLayer2d& ) {
}

void Graphic3d_GraphicDriver::RemoveLayer (const Aspect_CLayer2d& ) {
}

void Graphic3d_GraphicDriver::BeginLayer (const Aspect_CLayer2d& ) {
}

void Graphic3d_GraphicDriver::BeginPolygon2d () {
}

void Graphic3d_GraphicDriver::BeginPolyline2d () {
}

void Graphic3d_GraphicDriver::ClearLayer (const Aspect_CLayer2d& ) {
}

void Graphic3d_GraphicDriver::Draw (const Standard_ShortReal , const Standard_ShortReal ) {
}

void Graphic3d_GraphicDriver::Edge (const Standard_ShortReal , const Standard_ShortReal ) {
}

void Graphic3d_GraphicDriver::EndLayer () {
}

void Graphic3d_GraphicDriver::EndPolygon2d () {
}

void Graphic3d_GraphicDriver::EndPolyline2d () {
}

void Graphic3d_GraphicDriver::Move (const Standard_ShortReal , const Standard_ShortReal ) {
}

void Graphic3d_GraphicDriver::Rectangle (const Standard_ShortReal , const Standard_ShortReal , const Standard_ShortReal , const Standard_ShortReal ) {
}

void Graphic3d_GraphicDriver::SetColor (const Standard_ShortReal , const Standard_ShortReal , const Standard_ShortReal ) {
}

void Graphic3d_GraphicDriver::SetTransparency (const Standard_ShortReal ) {
}

void Graphic3d_GraphicDriver::UnsetTransparency () {
}

void Graphic3d_GraphicDriver::SetLineAttributes (const Standard_Integer , const Standard_ShortReal ) {
}

void Graphic3d_GraphicDriver::SetTextAttributes (const Standard_CString Font, const Standard_Integer AType, const Standard_ShortReal R, const Standard_ShortReal G, const Standard_ShortReal B) {
}

void Graphic3d_GraphicDriver::Text (const Standard_CString , const Standard_ShortReal , const Standard_ShortReal , const Standard_ShortReal ) {
}

void Graphic3d_GraphicDriver::TextSize (const Standard_CString , const Standard_ShortReal , Standard_ShortReal& , Standard_ShortReal& , Standard_ShortReal& ) const {
}
