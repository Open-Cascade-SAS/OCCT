// File   Graphic3d_GraphicDriver_711.cxx
// Created  Mardi 28 janvier 1997
// Author CAL
//
// Modified
//              27/08/97 ; PCT : ajout coordonne texture
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

void Graphic3d_GraphicDriver::TriangleMesh (const Graphic3d_CGroup& , const Graphic3d_Array1OfVertex& , const Standard_Boolean ) {
}

void Graphic3d_GraphicDriver::TriangleMesh (const Graphic3d_CGroup& , const Graphic3d_Array1OfVertexN& , const Standard_Boolean ) {
}


void Graphic3d_GraphicDriver::TriangleMesh(const Graphic3d_CGroup& ,const Graphic3d_Array1OfVertexNT& ,const Standard_Boolean ) 
{
}


void Graphic3d_GraphicDriver::TriangleSet (const Graphic3d_CGroup& , const Graphic3d_Array1OfVertex& , const Aspect_Array1OfEdge& , const Standard_Boolean ) {
}

void Graphic3d_GraphicDriver::TriangleSet (const Graphic3d_CGroup& , const Graphic3d_Array1OfVertexN& , const Aspect_Array1OfEdge& , const Standard_Boolean ) {
}

void Graphic3d_GraphicDriver::TriangleSet (const Graphic3d_CGroup& , const Graphic3d_Array1OfVertexC& , const Aspect_Array1OfEdge& , const Standard_Boolean ) {
}

void Graphic3d_GraphicDriver::TriangleSet (const Graphic3d_CGroup& , const Graphic3d_Array1OfVertexNC& , const Aspect_Array1OfEdge& , const Standard_Boolean ) {
}



void Graphic3d_GraphicDriver::TriangleSet(const Graphic3d_CGroup& ,const Graphic3d_Array1OfVertexNT& ,const Aspect_Array1OfEdge& ,const Standard_Boolean ) 
{
}
