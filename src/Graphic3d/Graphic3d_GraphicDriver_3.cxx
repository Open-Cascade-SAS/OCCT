// File   Graphic3d_GraphicDriver_3.cxx
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

void Graphic3d_GraphicDriver::ClearGroup (const Graphic3d_CGroup& ) {
}

void Graphic3d_GraphicDriver::CloseGroup (const Graphic3d_CGroup& ) {
}


void Graphic3d_GraphicDriver::FaceContextGroup (const Graphic3d_CGroup& , const Standard_Integer NoInsert) {
}


void Graphic3d_GraphicDriver::Group (Graphic3d_CGroup& ) {
}


void Graphic3d_GraphicDriver::LineContextGroup (const Graphic3d_CGroup& , const Standard_Integer ) {
}

void Graphic3d_GraphicDriver::MarkerContextGroup (const Graphic3d_CGroup& , const Standard_Integer ) {
}

void Graphic3d_GraphicDriver::MarkerContextGroup (const Graphic3d_CGroup& , const Standard_Integer, const Standard_Integer, const Standard_Integer, const Handle(TColStd_HArray1OfByte)& ) {
}



void Graphic3d_GraphicDriver::OpenGroup (const Graphic3d_CGroup& ) {
}


void Graphic3d_GraphicDriver::RemoveGroup (const Graphic3d_CGroup& ) {
}

void Graphic3d_GraphicDriver::TextContextGroup (const Graphic3d_CGroup& , const Standard_Integer ) {

}
