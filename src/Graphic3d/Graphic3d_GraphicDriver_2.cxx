// File   Graphic3d_GraphicDriver_2.cxx
// Created  Mardi 28 janvier 1997
// Author CAL
//              15/09/97 ; PCT : ajout InquireTextureAvailable() pour les textures
//              11/97 ; CAL : retrait de la dependance avec math
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

#include <TColStd_Array2OfReal.hxx>

#include <Aspect_DriverDefinitionError.hxx>

//-Aliases

//-Global data definitions

//-Methods, in order

Standard_Integer Graphic3d_GraphicDriver::InquireLightLimit () {
  return Standard_False;
}

void Graphic3d_GraphicDriver::InquireMat (const Graphic3d_CView&
                                          , TColStd_Array2OfReal&
                                          , TColStd_Array2OfReal&
                                          )
{
}

Standard_Integer Graphic3d_GraphicDriver::InquirePlaneLimit ()
{
  return Standard_False;
}

Standard_Integer Graphic3d_GraphicDriver::InquireViewLimit () 
{
  return Standard_False;
}



Standard_Boolean Graphic3d_GraphicDriver::InquireTextureAvailable () 
{
  return Standard_False;
}

