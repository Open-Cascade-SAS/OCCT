// File   Graphic3d_GraphicDriver_710.cxx
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
#include <TCollection_AsciiString.hxx>

#include <Aspect_DriverDefinitionError.hxx>

//-Aliases

//-Global data definitions

//-Methods, in order

void Graphic3d_GraphicDriver::Text (
                                    const Graphic3d_CGroup& ,
                                    const Standard_CString ,
                                    const Graphic3d_Vertex& ,
                                    const Standard_Real ,
                                    const Quantity_PlaneAngle ,
                                    const Graphic3d_TextPath ,
                                    const Graphic3d_HorizontalTextAlignment ,
                                    const Graphic3d_VerticalTextAlignment ,
                                    const Standard_Boolean 
                                    ) 
{
}

void Graphic3d_GraphicDriver::Text (
                                    const Graphic3d_CGroup& ,
                                    const Standard_CString , 
                                    const Graphic3d_Vertex& , 
                                    const Standard_Real , 
                                    const Standard_Boolean 
                                    ) 
{
}

void Graphic3d_GraphicDriver::Text (
                                    const Graphic3d_CGroup& ,
                                    const TCollection_ExtendedString& , 
                                    const Graphic3d_Vertex& , 
                                    const Standard_Real , 
                                    const Quantity_PlaneAngle , 
                                    const Graphic3d_TextPath , 
                                    const Graphic3d_HorizontalTextAlignment , 
                                    const Graphic3d_VerticalTextAlignment , 
                                    const Standard_Boolean 
                                    ) 
{
}

void Graphic3d_GraphicDriver::Text (
                                    const Graphic3d_CGroup& ,
                                    const TCollection_ExtendedString& , 
                                    const Graphic3d_Vertex& , 
                                    const Standard_Real , 
                                    const Standard_Boolean 
                                    )
{
}
