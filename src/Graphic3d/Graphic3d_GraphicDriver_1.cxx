// File   Graphic3d_GraphicDriver_1.cxx
// Created  Mardi 28 janvier 1997
// Author CAL
// Modified     02.15.100 : JR : Clutter
//    GG 27/12/02 IMP120302 Add new method Begin(Aspect_Display)

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


Standard_Boolean Graphic3d_GraphicDriver::Begin (const Standard_CString ) {
  return Standard_True;
}

//RIC120302
Standard_Boolean Graphic3d_GraphicDriver::Begin (const Aspect_Display)
{
  return Standard_True;
}
//RIC120302

void Graphic3d_GraphicDriver::End () {
}

void Graphic3d_GraphicDriver::BeginAnimation (const Graphic3d_CView&
                                              ) 
{
}

void Graphic3d_GraphicDriver::EndAnimation (const Graphic3d_CView&
                                            ) 
{
}
