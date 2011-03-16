// File   Graphic3d_GraphicDriver_8.cxx
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


Standard_Boolean Graphic3d_GraphicDriver::BeginAddMode (const Graphic3d_CView& ) {
  return Standard_True;
}

void Graphic3d_GraphicDriver::EndAddMode (void) {
}

Standard_Boolean Graphic3d_GraphicDriver::BeginImmediatMode (const Graphic3d_CView& , const Aspect_CLayer2d& , const Aspect_CLayer2d& , const Standard_Boolean , const Standard_Boolean ) {
  return Standard_True;
}

void Graphic3d_GraphicDriver::BeginPolyline () {
}

void Graphic3d_GraphicDriver::ClearImmediatMode (const Graphic3d_CView&, const Standard_Boolean ) {
}

void Graphic3d_GraphicDriver::Draw (const Standard_ShortReal , const Standard_ShortReal , const Standard_ShortReal ) {
}

void Graphic3d_GraphicDriver::DrawStructure (const Graphic3d_CStructure& ) {
}

void Graphic3d_GraphicDriver::EndImmediatMode (const Standard_Integer ) {
}

void Graphic3d_GraphicDriver::EndPolyline () {
}

void Graphic3d_GraphicDriver::Move (const Standard_ShortReal , const Standard_ShortReal , const Standard_ShortReal ) {
}

void Graphic3d_GraphicDriver::SetLineColor (const Standard_ShortReal , const Standard_ShortReal , const Standard_ShortReal ) {
}

void Graphic3d_GraphicDriver::SetLineType (const Standard_Integer ) {
}

void Graphic3d_GraphicDriver::SetLineWidth (const Standard_ShortReal ) {
}


void Graphic3d_GraphicDriver::SetMinMax (const Standard_ShortReal , const Standard_ShortReal , const Standard_ShortReal , const Standard_ShortReal , const Standard_ShortReal , const Standard_ShortReal ) {
}

void Graphic3d_GraphicDriver::Transform (const TColStd_Array2OfReal& , const Graphic3d_TypeOfComposition ) {

}
